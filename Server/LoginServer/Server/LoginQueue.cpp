#include "StdAfx.h"
#include "Game.h"
#include ".\loginqueue.h"
#include "..\nets\servers.h"
#include "..\nets\netlogin\message.h"
//#include "..\..\dbaccess\logindb\rslogInfo.h"
#include <WinSock2.h>

//////////////////////////////////////////////////////////////////////////
//LoginQueue.cpp
//Fun:	登陆缓冲队列
//Author:Joe
//Create Time:2005.1.31
//Modify Time:
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLoginQueue::CLoginQueue(void)
{
	m_QuestCdkey.clear();
	m_QuestPlayerList.clear();
	m_QuestPlayerData.clear();
	m_nWordNum = 0;

	LoadNoQueueCdkeyList();
}

//初始化
void CLoginQueue::OnInitial()
{
	m_dwInter = GetGame()->GetSetup()->dwDoQueueInter;
	m_dwSendMessageInter=GetGame()->GetSetup()->dwSendMsgToQueInter;
	m_nWorldMaxPlayerNum = GetGame()->GetSetup()->m_dwMaxWsMum;

	if(m_nWordNum==0)
		m_nWordNum=1;
	DWORD dwCurTime = timeGetTime();
	m_dwDoLogQueueTime=dwCurTime+m_dwInter/m_nWordNum;
	m_dwWorldQueueTime=dwCurTime+m_dwInter;
}

CLoginQueue::~CLoginQueue(void)
{
	NoQueueCDkeyList.clear();
	m_mLoginList.clear();
}


void CLoginQueue::AddQuestCdkey(long lSocketID,DWORD dwIP,long lVersion,const char* szCdkey,
								vector<BYTE>& bytePassWord,char* szWSName, long lChallengeCode, long lEkey)
{
	//! 登陆等待人数大于最大在线人数两倍时，不接受登陆
	if(m_setQuestCdkey.size() > (GetGame()->GetSetup()->m_dwMaxWsMum << 1) && !IsInNoQueueList(szCdkey))
	{
		CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
		msg.Add((LONG)LOGIN_LOGINSERVERPLAYERFULL);
		msg.SendToClient(lSocketID);
		return;
	}

	DWORD dwTime = timeGetTime()+m_dwSendMessageInter;
	tagQuestCdkey iQuestCdkey(lSocketID, dwIP, lVersion, szCdkey, bytePassWord, szWSName, lChallengeCode, lEkey, dwTime, false, 0);

	if(IsInNoQueueList(szCdkey))
	{
		m_NoQueueQuestCdkey.push_back(iQuestCdkey);
	}
	else
	{
		if(m_setQuestCdkey.end() == m_setQuestCdkey.find(szCdkey))
		{
			m_setQuestCdkey.insert(szCdkey);
			m_QuestCdkey.push_back(iQuestCdkey);
		}
	}
}
void	CLoginQueue::AddQuestPlayerList(long lSocketID,const char* szCdkey,const char* szWSName)
{
	if(szWSName == NULL)
		return;
	if(GetGame()->IsExitWorld(szWSName) == false)
		return;

	//判断是否登陆过
	if(GetGame()->GetLoginCdkeyWorldServer(szCdkey)==NULL)
		return;
	//最大人数
	long lCurWorldPlayerNum = GetGame()->GetLoginWorldPlayerNumByWorldName(szWSName);
	//错误
	if(lCurWorldPlayerNum == -1)
		return;

	DWORD dwTime = timeGetTime()+m_dwSendMessageInter;
	tagQuestPlayerList QuestPlayerList = {lSocketID,szCdkey,szWSName,dwTime};

	if(IsInNoQueueList(szCdkey))
	{
		m_NoQueueQuestPlayerList[szWSName].push_back(QuestPlayerList);
	}
	else
	{
		list<tagQuestPlayerList> & PlayerList = m_QuestPlayerList[szWSName];
		set<string>&	setWaitInfo = m_mapQuestBaseInfo[szWSName];
		if(setWaitInfo.end() != setWaitInfo.find(szCdkey) )
		{
			return;
		}
		if(setWaitInfo.size() > (GetGame()->GetSetup()->m_dwMaxWsMum << 1))
		{
			CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
			msg.Add((LONG)LOGIN_LOGINSERVERPLAYERFULL);
			msg.SendToClient(lSocketID);
			return;
		}

		PlayerList.push_back(QuestPlayerList);
		setWaitInfo.insert(szCdkey);
	}
}
void	CLoginQueue::AddQuestPlayerData(long lSocketID,const char* szCdkey,CGUID& playerid,DWORD dwIP, DWORD dwSignCode)
{
	if(szCdkey == NULL)
		return;

	//得到登陆的WorldServer名字
	const char* szWSName = GetGame()->GetLoginCdkeyWorldServer(szCdkey);

	if(szWSName == NULL)
		return;

	DWORD dwTime = timeGetTime()+m_dwSendMessageInter;
	tagQuestPlayerData QuestPlayerData = {lSocketID,szCdkey,playerid,dwIP,dwTime, dwSignCode};

	if(IsInNoQueueList(szCdkey))
	{
		m_NoQueueQuestPlayerData[szWSName].push_back(QuestPlayerData);
	}	
	else
	{
		m_QuestPlayerData[szWSName].push_back(QuestPlayerData);
	}
}

void	CLoginQueue::Run()
{
	if(m_NoQueueQuestCdkey.size() > 0)
	{
		for(itCdkey it = m_NoQueueQuestCdkey.begin();it != m_NoQueueQuestCdkey.end();it++)
		{
			tagQuestCdkey& QuestCdkey = (*it);
			OnQuestCdkey(QuestCdkey);
		}
		m_NoQueueQuestCdkey.clear();
	}

	if(m_NoQueueQuestPlayerList.size() > 0)
	{
		itPlayerListMap itPlayer = m_NoQueueQuestPlayerList.begin();
		for(;itPlayer != m_NoQueueQuestPlayerList.end();itPlayer++)
		{
			QuestPlayerList& TemptQuestPlayerList = (*itPlayer).second;
			for(itPlayerList it = TemptQuestPlayerList.begin();it != TemptQuestPlayerList.end();it++)
			{
				tagQuestPlayerList& QuestPlayerList = (*it);
				OnQuestPlayerList(QuestPlayerList);
			}
			TemptQuestPlayerList.clear();
		}
		m_NoQueueQuestPlayerList.clear();
	}

	if(m_NoQueueQuestPlayerData.size() > 0)
	{
		itPlayerDataMap itData = m_NoQueueQuestPlayerData.begin();
		for(; itData != m_NoQueueQuestPlayerData.end();itData++)
		{
			QuestPlayerData& TemptQuestPlayerData = (*itData).second;
			for(itPlayerData it = TemptQuestPlayerData.begin();it != TemptQuestPlayerData.end();it++)
			{
				tagQuestPlayerData& QuestPlayerData = (*it);
				OnQuestPlayerData(QuestPlayerData);
			}
			TemptQuestPlayerData.clear();
		}
		m_NoQueueQuestPlayerData.clear();
	}

	DWORD dwCurTime = timeGetTime();
	//队列中分别处理一个
	//if(dwCurTime >= m_dwDoLogQueueTime)
	{
		if(m_nWordNum==0)
		{
			m_dwDoLogQueueTime = dwCurTime+1000;
		}
		else
		{
			m_dwDoLogQueueTime = dwCurTime+m_dwInter/m_nWordNum;
		}
		if(m_QuestCdkey.size() > 0)
		{
			LONG lLoopCount = (LONG)m_QuestCdkey.size() / 10;
			lLoopCount = (lLoopCount > 0) ? lLoopCount : 1;
			for (LONG i = 0; i < lLoopCount; ++i)
			{
				tagQuestCdkey& QuestCdkey = (*m_QuestCdkey.begin());
				OnQuestCdkey(QuestCdkey);
				m_setQuestCdkey.erase(QuestCdkey.strCdkey);
				m_QuestCdkey.pop_front();
			}
		}
	}

//	HandlePtChecked();

	//if(dwCurTime >= m_dwWorldQueueTime)
	{
		m_dwWorldQueueTime = dwCurTime+m_dwInter;


		itPlayerListMap itPlayer = m_QuestPlayerList.begin();
		for(;itPlayer != m_QuestPlayerList.end();itPlayer++)
		{
			QuestPlayerList& TemptQuestPlayerList = (*itPlayer).second;
			set<string>&	setWaitInfo = m_mapQuestBaseInfo[itPlayer->first];
			if(TemptQuestPlayerList.size() > 0)
			{
				LONG lLoopCount = (LONG)TemptQuestPlayerList.size() / 10;
				lLoopCount = (lLoopCount > 0) ? lLoopCount : 1;
				for (LONG i = 0; i < lLoopCount; ++i)
				{
					tagQuestPlayerList& QuestPlayerList = (*TemptQuestPlayerList.begin());
					OnQuestPlayerList(QuestPlayerList);
					setWaitInfo.erase(QuestPlayerList.strCdkey);
					TemptQuestPlayerList.pop_front();
				}
			}
		}

		itPlayerDataMap itData = m_QuestPlayerData.begin();
		for(; itData != m_QuestPlayerData.end();itData++)
		{
			QuestPlayerData& TemptQuestPlayerData = (*itData).second;
			if(TemptQuestPlayerData.size() > 0)
			{
				tagQuestPlayerData& QuestPlayerData = (*TemptQuestPlayerData.begin());
				OnQuestPlayerData(QuestPlayerData);
				TemptQuestPlayerData.pop_front();
			}
		}
	}


	//发送消息给客户端
	long i=1;
	/*for(itCdkey it = m_QuestCdkey.begin();it != m_QuestCdkey.end();i++,it++)
	{
		if( dwCurTime >= (*it).dwSendMessageTime )
		{
			(*it).dwSendMessageTime = dwCurTime+m_dwSendMessageInter;
			CMessage msg(MSG_L2C_LOG_UPDATEQUEUENUM);
			msg.Add(i);
			msg.SendToClient((*it).lSocketID);
		}
	}*/

	itPlayerListMap itPlayer = m_QuestPlayerList.begin();
	/*for(;itPlayer != m_QuestPlayerList.end();itPlayer++)
	{
		i=1;
		QuestPlayerList& TemptQuestPlayerList = (*itPlayer).second;
		for(itPlayerList it = TemptQuestPlayerList.begin();it != TemptQuestPlayerList.end(); i++,it++)
		{
			if( dwCurTime >= (*it).dwSendMessageTime)
			{
				(*it).dwSendMessageTime = dwCurTime+m_dwSendMessageInter;
				CMessage msg(MSG_L2C_LOG_UPDATEQUEUENUM);
				msg.Add(i);
				msg.SendToClient((*it).lSocketID);
			}
		}
	}*/


	itPlayerDataMap itData = m_QuestPlayerData.begin();
	/*for(; itData != m_QuestPlayerData.end();itData++)
	{
		i=1;
		QuestPlayerData& TemptQuestPlayerData = (*itData).second;
		for(itPlayerData it = TemptQuestPlayerData.begin();it != TemptQuestPlayerData.end(); i++,it++)
		{
			if( dwCurTime >= (*it).dwSendMessageTime )
			{
				(*it).dwSendMessageTime = dwCurTime+m_dwSendMessageInter;
				CMessage msg(MSG_L2C_LOG_UPDATEQUEUENUM);
				msg.Add(i);
				msg.SendToClient((*it).lSocketID);
			}
		}
	}*/


	ClearTimeoutList();
}

void	CLoginQueue::OnQuestCdkey(tagQuestCdkey& QuestCdkey)
{
	// 首次登陆
	bool bFirstLogin = true;

	//验证版本是否正确
	long lSocketID				= QuestCdkey.lSocketID;
	DWORD dwIP					= QuestCdkey.dwIP;
	const char* szCdkey			= QuestCdkey.strCdkey.c_str();
	vector<BYTE>& szPassWord	= QuestCdkey.strPassWord;
	const char* szWSName		= QuestCdkey.strWSName.c_str();
	long lChallengeCode			= QuestCdkey.lChallengeCode;
	long lEkey					= QuestCdkey.lEkey;

	extern long g_lVersion;
	if(QuestCdkey.lVersion!=g_lVersion)
	{
		CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
		msg.Add((LONG)LOGIN_VERSIONERROR);
		msg.SendToClient(lSocketID);
		return;
	}

	// 有服务器名，在重新回到选人界面了。
	if( strlen(szWSName) )
	{
		bFirstLogin = false;
	}

	if( bFirstLogin )
	{
		//把二进制密码转换成字符串
		char szPwd[33];
		static char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
		BYTE* pByte = &szPassWord[0];
		for(int i=0; i<16; i++)
		{
			int bValue = szPassWord[i];
			int bValue1 = bValue>>4;
			int bValue2 = bValue&0x0F;
			if(bValue1 >= 0 && bValue1 <= 15 && bValue2 >= 0 && bValue2 <= 15)
			{
				szPwd[i*2] = hexDigits[bValue1];
				szPwd[i*2+1] = hexDigits[bValue2];
			}
		}
		szPwd[32]=0;
		strupr(szPwd);

		//! 最大在线人数判断
		long lCurWorldPlayerNum = GetGame()->GetLoginWorldCdkeyNumbers();
		if(-1 < lCurWorldPlayerNum)
		{
			if(GetGame()->GetSetup()->m_dwMaxWsMum <= (DWORD)lCurWorldPlayerNum)
			{
				if(!IsInNoQueueList(szCdkey))
				{
					CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
					msg.Add((LONG)LOGIN_LOGINSERVERPLAYERFULL);
					msg.SendToClient(lSocketID);
					return;
				}
			}
		}

		

		//// 加入到已处理队列
		//tagQuestCdkey* qc = new CLoginQueue::tagQuestCdkey(QuestCdkey.lSocketID, QuestCdkey.dwIP, 0, string(QuestCdkey.strCdkey), 
		//	QuestCdkey.strPassWord, QuestCdkey.strWSName, QuestCdkey.lChallengeCode, QuestCdkey.lEkey, 0, false, 0);
		//qc->bISCPTAuthen = false;
		//PushBackPtChecked(qc);

		//! 开始验证账号
		tagAccountInfo_ToAS AccountInfo_ToAS(szCdkey, szPwd, szWSName, dwIP, lSocketID);
		GetGame()->GetAccountClient().ValidateAccount(AccountInfo_ToAS);

	}
	else
		GetGame()->EnterGame(szCdkey, dwIP, lSocketID, szWSName);
}

void	CLoginQueue::OnQuestPlayerList(tagQuestPlayerList& QuestPlayerList)
{
	const char* szCdkey = QuestPlayerList.strCdkey.c_str();
	const char* szWSName= QuestPlayerList.strWSName.c_str();

	if( GetGame()->L2W_PlayerBase_Send(szWSName,szCdkey) )//向worldserver请求该客户的基本信息
	{
		GetGame()->SetLoginCdkeyWorldServer(szCdkey,szWSName);//设置该连接所选择的worldserver名
	}
}

void	CLoginQueue::OnQuestPlayerData(tagQuestPlayerData& QuestPlayerData)
{
	const char* szCdkey = QuestPlayerData.strCdkey.c_str();
	const char *szWSName= GetGame()->GetLoginCdkeyWorldServer(szCdkey);//获得worldserver的名字

	//发送获取数据消息前，做一个时间效验。 
	if( IsValidQuest(QuestPlayerData.PlayerID) )
	{
		GetGame()->L2W_QuestDetail_Send(szWSName,szCdkey,QuestPlayerData.PlayerID,QuestPlayerData.dwIP, QuestPlayerData.dwSignCode);

		PushLoginList(QuestPlayerData.PlayerID);
	}
	else
	{
		CMessage msg(MSG_L2C_LOG_ANSWER_PLAYERDATA);
		msg.Add(QuestPlayerData.dwSignCode);
		msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
		msg.Add((char*)szCdkey);
		msg.SendToClient(szCdkey);
	}
}

void	CLoginQueue::OnClientLost(const char* szCdkey)
{
	if(szCdkey == NULL)
		return;
	for(itCdkey it = m_QuestCdkey.begin();it != m_QuestCdkey.end();it++)
	{
		if( (*it).strCdkey == szCdkey )
		{
			m_setQuestCdkey.erase(szCdkey);
			m_QuestCdkey.erase(it);
			break;
		}
	}

	itPlayerListMap itPlayer = m_QuestPlayerList.begin();
	for(;itPlayer != m_QuestPlayerList.end();itPlayer++)
	{
		QuestPlayerList& TemptQuestPlayerList = (*itPlayer).second;
		set<string> &setWaitInfo = m_mapQuestBaseInfo[itPlayer->first];
		for(itPlayerList it = TemptQuestPlayerList.begin();it != TemptQuestPlayerList.end();it++)
		{
			if((*it).strCdkey == szCdkey)
			{
				TemptQuestPlayerList.erase(it);
				setWaitInfo.erase(szCdkey);
				break;
			}
		}
	}


	itPlayerDataMap itData = m_QuestPlayerData.begin();
	for(; itData != m_QuestPlayerData.end();itData++)
	{
		QuestPlayerData& TemptQuestPlayerData = (*itData).second;
		for(itPlayerData it = TemptQuestPlayerData.begin();it != TemptQuestPlayerData.end();it++)
		{
			if((*it).strCdkey == szCdkey)
			{
				TemptQuestPlayerData.erase(it);
				break;
			}
		}
	}
}

void	CLoginQueue::LoadNoQueueCdkeyList()
{
	NoQueueCDkeyList.clear();
	ifstream ListFile;
	ListFile.open( "NoQueueCdkeyList.txt" ,ios_base::in);
	if( ListFile.fail() )
	{
		ListFile.close();
		MessageBox(g_hWnd, "Can't open NoQueueCdkeyList.txt", "error", MB_OK);
		return;
	}
	char strTempt[256];
	while (!ListFile.eof())
	{
		ListFile >> strTempt;
		strlwr(strTempt);
		NoQueueCDkeyList.insert(strTempt);
	}

	AddLogText("Load the No Queue Cdkey OK.");
}

//该CDKey是否在不用排队的列表中
bool	CLoginQueue::IsInNoQueueList(const char* szCdkey)
{
	if(szCdkey == NULL)	return false;

	if( NoQueueCDkeyList.find(szCdkey) != NoQueueCDkeyList.end() )
	{
		return true;
	}
	return false;
}


// 把角色加入登陆表
bool CLoginQueue::PushLoginList(CGUID playerid)
{
	itLogList i = m_mLoginList.find(playerid);
	if( i == m_mLoginList.end() )
	{
		m_mLoginList[playerid] = timeGetTime();
		return true;
	}
	return false;
}
// 玩家是否合法请求
bool CLoginQueue::IsValidQuest(CGUID playerid)
{
	itLogList i = m_mLoginList.find(playerid);
	if( i == m_mLoginList.end() )
	{
		return true;
	}

	if( timeGetTime() > i->second + GetGame()->GetSetupEx()->lQuestPlayerDataInterval )
	{
		m_mLoginList.erase(i);
		return true;
	}

	return false;
}

void CLoginQueue::ClearTimeoutList()
{
	for( itLogList i = m_mLoginList.begin();
		i != m_mLoginList.end(); )
	{
		if( timeGetTime() > i->second + GetGame()->GetSetupEx()->lQuestPlayerDataInterval )
		{
			i = m_mLoginList.erase(i);
		}
		else
		{
			++i;
		}
	}
}

//-----------------------------------------------------------------------------
// 把一个账号压入队尾
//-----------------------------------------------------------------------------
CLoginQueue::tagQuestCdkey* CLoginQueue::PushBackPtChecked( tagQuestCdkey* pAcc )
{
	if( !pAcc ) return NULL;

	vector<BYTE> vecPWD;

	lockPtChecked.Acquire();

	// 检查账号是否已经在队列中，在的话，踢他。
	for( QuestPtCheck::iterator it = m_lstPtChecked.begin(); 
		it != m_lstPtChecked.end(); ++it )
	{
		// 已通过PT验证
		if( *it && (*it)->strCdkey == pAcc->strCdkey && (*it)->bISCPTAuthen == true)
		{
			// 1、踢人
			GetGame()->KickOut( (*it)->strCdkey.c_str() );

			// 2、从队列删除
			vecPWD = (*it)->strPassWord;
			delete (*it);
			m_lstPtChecked.erase( it );
			break;
		}
		else if( *it && (*it)->strCdkey == pAcc->strCdkey && (*it)->bISCPTAuthen == false)
		{
			// 2、从队列删除
			vecPWD = (*it)->strPassWord;
			delete (*it);
			m_lstPtChecked.erase( it );
			break;
		}
	}

	// 压入队尾
	m_lstPtChecked.push_back( pAcc );
	// 拷贝Cdkey
	string cdkey = pAcc->strCdkey;

	lockPtChecked.Release();

	return pAcc;
}
void CLoginQueue::ClearPtChecked(QuestPtCheck &lstChecked)
{
	lstChecked.clear();
	lockPtChecked.Acquire();
	QuestPtCheck::iterator itr = m_lstPtChecked.begin();
	while (itr != m_lstPtChecked.end())
	{
		// 已通过PT验证
		if((*itr)->bISCPTAuthen == true)
		{
			lstChecked.push_back(*itr);
			itr = m_lstPtChecked.erase(itr);
		}
		else
			itr++;
	}
	lockPtChecked.Release();
}

//-----------------------------------------------------------------------------
// 帐号队列是否为空
//-----------------------------------------------------------------------------
bool CLoginQueue::IsPtCheckedEmpty()
{
	lockPtChecked.Acquire();

	bool bEmpty = m_lstPtChecked.empty();

	lockPtChecked.Release();
	return bEmpty;
}

//-----------------------------------------------------------------------------
// 处理一个已通过密码验证的帐号，给客户端发送验证码并加入验证码map
//-----------------------------------------------------------------------------
void CLoginQueue::HandlePtChecked()
{
	QuestPtCheck ListChecked;
	ClearPtChecked(ListChecked);

	while( !ListChecked.empty() )
	{
		tagQuestCdkey* pAcc = ListChecked.front();
		ListChecked.pop_front();

		if( !pAcc )
		{
			continue;
		}

		int         sockid      = pAcc->lSocketID;
		DWORD       ip          = pAcc->dwIP;
		const char* account     = pAcc->strCdkey.c_str();
		string      wsname      = pAcc->strWSName;
		//bool        matrix      = pAcc->has_matrix;
		bool        bLimit		= pAcc->bLimit;
		//bool        is_mobile_user  = pAcc->is_mobile_user;

		if( !account || !sockid || !ip )
		{
			if( pAcc ) delete pAcc;
			continue;
		}

		GetGame()->EnterGame( account, ip, sockid, wsname.c_str() );


		//// 如果此帐号在验证错误队列中
		//if( IsValidErrManyTimes( pAcc->szPtAccountount.c_str() ) )
		//{
		//    // 将玩家弹回登录界面（输入帐号密码）
		//    CMessage msg( CMessage::MSG_L2C_LOG_ANSWERCDKEY );
		//    msg.Add( (LONG)LOGIN_FAILED_MANY_TIMES );
		//    msg.SendToClient( sockid );
		//}
		//else
		//{
		//    if( GetGame()->GetSetupEx()->bValidCode )
		//    {
		//        // 如果帐号account已经在验证码map中，把他踢掉先。
		//        ValidCodeIter it = m_mapValidCode.find( string(account) );
		//        if( it != m_mapValidCode.end() )
		//        {
		//            CMessage msg( CMessage::MSG_L2C_LOG_ANSWERCDKEY );
		//            msg.Add( (LONG)(LOGIN_VALID_CODE_KICKOUT) );
		//            msg.SendToClient( it->second.socket_id );
		//        }

		//        // 生成新的验证码
		//        CValidCode vcode;

		//        // 加入验证码map。在一定时间内不输入，会超时处理。
		//        m_mapValidCode[string(account)] = tagValidCode( sockid, ip, timeGetTime(), vcode.GetString(), wsname, matrix, bLimit, is_mobile_user, 0 );

		//        // 发送消息（包含一个验证码图片）到client，通知输入验证码。
		//        CMessage msg( CMessage::MSG_L2C_LOG_ANSWERCDKEY );
		//        msg.Add( (LONG)(LOGIN_VALID_CODE) );
		//        msg.Add( (char*)account );
		//        CValidCode::BitmapFile& bmpFile = vcode.m_bmpValidCode;
		//        msg.Add( static_cast<long>( sizeof(bmpFile) ) );
		//        msg.Add( static_cast<void*>(&bmpFile), sizeof(bmpFile) );
		//        msg.SendToClient( sockid );
		//    }
		//    else // 验证码关闭
		//    {
		//        if( GetGame()->PrepareEnter( account, ip, sockid, wsname.c_str(), matrix, bLimit, is_mobile_user ) )
		//        {
		//            if( pAcc ) delete pAcc;
		//            continue;
		//        }
		//        
		//		GetGame()->EnterGame( account, ip, sockid, wsname.c_str() );
		//    }
		//}

		if( pAcc ) delete pAcc;
	}


}