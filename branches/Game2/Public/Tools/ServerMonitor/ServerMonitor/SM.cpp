#include "StdAfx.h"
#include "SM.h"
#include "../net/Message.h"
#include "../../../nets/Servers.h"
#include "../../../nets/MapFile.h"
CSM* g_pSM = NULL;	
HANDLE g_hSMThreadExitEvent=NULL;// 主线程退出事件
bool	g_bSMThreadExit=false;// 是否退出主线程
bool	g_bRunCmdLine=true;//是否处理命令行		
#pragma comment(lib,"pdh.lib")
void Command();
CSM::CSM(void)
: s_pNetServer(NULL)
, m_pDBAllocator(NULL)
, b_IsFirstConnectionSCC(false)
, m_lSocketID(0)
, m_CurrScc(NULL)
, m_GappTimer(1024, 1024)
{
}

CSM::~CSM(void)
{
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP5"));
}

BOOL CSM::InitMemUsageName()
{
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	_snprintf(m_pszMemUsageFile,MAX_PATH,"HardWareUsage[%02d_%02d_%02d_%02d_%02d].txt",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	_snprintf(m_pszLogFile,MAX_PATH,"SM_RunStackInfo[%02d_%02d_%02d_%02d_%02d].txt",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	return TRUE;
}

BOOL CSM::LoadSetup()
{
	//ifstream stream;
	//stream.open("setup.ini");
	//
	//if( stream.is_open() )
	//{
	//	stream  >> strTemp >> GetSetup()->strLoginIP
	//		>> strTemp >> GetSetup()->dwLoginPort
	//		>> strTemp >>GetSetup()->dwLoginPassword
	//		>> strTemp >> GetSetup()->lSwitch;
	//	//end ;

	//	stream.close();
	//	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP6"));
	//}
	string strTemp;
	ifstream streamSetupEx;
	streamSetupEx.open("setup/SetupEx.ini");
	if( streamSetupEx.is_open() )
	{
		streamSetupEx  >> strTemp >> GetSetup()->lMaxBlockConNum
			>> strTemp >> GetSetup()->lValidDelayRecDataTime
			>> strTemp >> GetSetup()->dwDataBlockNum
			>> strTemp >> GetSetup()->dwDataBlockSize
			>> strTemp >> GetSetup()->dwFreeMsgNum

			>> strTemp >> GetSetup()->dwFreeSockOperNum
			>> strTemp >> GetSetup()->dwFreeIOOperNum
			>> strTemp >> GetSetup()->lIOOperDataBufNum

			>> strTemp >> GetSetup()->bCheckNet
			>> strTemp >> GetSetup()->dwBanIPTime
			>> strTemp >> GetSetup()->dwMaxMsgLen			
			>> strTemp >> GetSetup()->lMaxConnectNum
			>> strTemp >> GetSetup()->lMaxClientsNum
			>> strTemp >> GetSetup()->lPendingWrBufNum
			>> strTemp >> GetSetup()->lPendingRdBufNum
			>> strTemp >> GetSetup()->lMaxSendSizePerSecond
			>> strTemp >> GetSetup()->lMaxRecvSizePerSecond
			>> strTemp >> GetSetup()->lMaxBlockedSendMsgNum
			>> strTemp >> GetSetup()->lConPendingWrBufNum
			>> strTemp >> GetSetup()->lConPendingRdBufNum
			>> strTemp >> GetSetup()->lConMaxSendSizePerSecond
			>> strTemp >> GetSetup()->lConMaxRecvSizePerSecond
			>> strTemp >> GetSetup()->lConMaxBlockedSendMsgNum			
			>> strTemp >> GetSetup()->dwRefeashInfoTime
			>> strTemp >> GetSetup()->dwSaveInfoTime
			>> strTemp >> GetSetup()->dwGoodsLogTime
			>> strTemp >> GetSetup()->bWatchRunTimeInfo
			>> strTemp >> GetSetup()->dwWatchRunTime
			>> strTemp >> GetSetup()->dwMsgValidateTime;
		streamSetupEx.close();
	}
	else
	{
		return false;
	}
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP7"));
	return TRUE;
}

BOOL CSM::Init()
{
	//! 创建日志目录
	char szCurrDir[256] = {0};
	char szTempDir[256] = {0};
	::GetModuleFileName(NULL, szCurrDir, MAX_PATH);
	GetFilePath(szCurrDir,szTempDir);
	::SetCurrentDirectory(szTempDir);
	::CreateDirectory( "log",NULL);
	//字符串资源
	CClientResource::InitialInstance();
	CClientResource* pClientResource = CClientResource::GetInstance();
	pClientResource->Initial(GAME_RES,string(szCurrDir),string(""));
//	CStringReading::InitUniString("setup/globaltextres.xml","GlobalFile");
    AppFrame::LoadTextRes();
	Command();
	LoadSetup();
	
	//分配数据块
	m_pDBAllocator = new CDataBlockAllocator(m_Setup.dwDataBlockNum,m_Setup.dwDataBlockSize,true);

	InitMemUsageName();
	//初始化消息
	if(CMessage::Initial(m_pDBAllocator,m_Setup.dwFreeMsgNum))
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP8"));
	}
	// 初始化服务器网络
	if(CMySocket::MySocketInit())
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP9"));
	}
	//资源
	//LoadServerResource();
	//读配置文件
	
	LoadSccSetup("setup/scclist.xml");
	
	//初始化内存映射文件
	if (LoadGappInfo("setup/GappInfo.xml"))
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP10"));
		InitMemServer();
	}
	if(InitNetServer())
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP11"));
	}
	/*if(StartAccept())
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP12"));
	}*/
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP13"));
	bool bConnScc=false;
	if (GetSetup()->lSwitch > 0)
	{
		
		if (ConnScc())
		{
			bConnScc=true;
		}
	}
	else
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP14"));
	if (!bConnScc)
	{
		InitSccPrama();
	}
	return TRUE;
}

//初始化游戏网络服务器端，建立网络服务器端，装载数据
BOOL CSM::InitNetServer(void)
{
	s_pNetServer = new CServer;
	//做为服务器
	long lRet = s_pNetServer->Start(1|2,m_pDBAllocator,
		m_Setup.dwFreeSockOperNum,m_Setup.dwFreeIOOperNum,m_Setup.lIOOperDataBufNum,
		m_Setup.bCheckNet,m_Setup.dwBanIPTime,
		m_Setup.dwMaxMsgLen,m_Setup.lMaxConnectNum,m_Setup.lMaxClientsNum,
		m_Setup.lPendingWrBufNum,m_Setup.lPendingRdBufNum,
		m_Setup.lMaxSendSizePerSecond,m_Setup.lMaxRecvSizePerSecond,
		m_Setup.lMaxBlockedSendMsgNum,
		m_Setup.lConPendingWrBufNum,m_Setup.lConPendingRdBufNum,
		m_Setup.lConMaxSendSizePerSecond,m_Setup.lConMaxRecvSizePerSecond,
		m_Setup.lConMaxBlockedSendMsgNum);
	CMessage::SetServer(s_pNetServer);
	return TRUE; 
}

BOOL CSM::StartAccept()
{
	/*if(s_pNetServer == NULL)	return FALSE;

	BOOL bRet = s_pNetServer->Host(m_Setup.dwListenPort,NULL,NF_SCC_Client,SOCK_STREAM);
	if(!bRet)
	{
		return FALSE;
	}*/
	return TRUE;
}

BOOL CSM::InitMemServer(void)
{
	if( !GetInst(CMemServer).Initial(m_pDBAllocator) )
			return FALSE;
	GAPPITER it=m_mapGappInfo.begin();
	for (;it!=m_mapGappInfo.end();it++)
	{
		CGapp* pGapp = it->second;
		if (pGapp)
		{
			pGapp->InitMemServer();
		}
	}
	return TRUE;
}

//申请共享数据块
BYTE* CSM::AllockShareDB(long lSize)
{
	return m_pDBAllocator->AllockShareDB(lSize);
}
//释放共享数据块
void CSM::FreeShareDB(BYTE* pData)
{
	m_pDBAllocator->FreeShareDB(pData);
}

BOOL CreateSM()
{
	g_pSM = new CSM;
	return TRUE;
}

CSM* GetSM()
{
	return g_pSM;
}

BOOL CSM::Release()
{
	GAPPITER it=m_mapGappInfo.begin();
	for (;it!=m_mapGappInfo.end();it++)
	{
		delete	it->second;
	}
	GetInst(CMemServer).Release();
	if(NULL != s_pNetServer)
		s_pNetServer->Exit();
	SAFE_DELETE(s_pNetServer);	
	CMySocket::MySocketClearUp();
	CMessage::Release();
	SAFE_DELETE(m_pDBAllocator);
#ifdef _DEBUG
	CCmdLine::GetInstance()->Release();
#endif
	ReleaseTimerVarManager();
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP15"));
	return TRUE;
}

BOOL DeleteSM()
{
	if (g_pSM)
	{
		delete g_pSM;
		g_pSM = NULL;
	}
	return true;
}


BOOL CSM::LoadSccSetup(const char *filename)
{
	TiXmlNode *pNode = NULL; 		

	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox;
	if(!m_Tdoc->LoadFile())
		return FALSE;

	pNode=m_Tdoc->FirstChild("SCCList");

	if (pNode==NULL)
		return FALSE;

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
		return FALSE;
	int lswitch = 0;
	te->Attribute("Switch",&lswitch);
	GetSetup()->lSwitch = lswitch;
	TiXmlElement* p=te->FirstChildElement();
	if (p == NULL)
		return FALSE;
	
	tagSCC tgscc;
	for (; p!=NULL; p=p->NextSiblingElement())
	{
		int nid = 0,port = 0,Auto= 0;
		p->Attribute("Auto",&Auto);
		p->Attribute("ID",&nid);
		p->Attribute("Port",&port);
		tgscc.strIP = p->Attribute("Ip");
		string strpass;
		strpass = p->Attribute("Password");
		strcpy(tgscc.cPassward,strpass.c_str());
		tgscc.lAuto = Auto;
		tgscc.dwPort = port;
		tgscc.dwIndex = nid;
		s_mapSCC[nid] = tgscc;
	}

	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP16"));
	return TRUE;
}

//主线程
unsigned __stdcall SMThreadFunc(void* pArguments)
{
	::CoInitialize(NULL);
#ifndef _DEBUG
	__try
	{
		
#endif
	CreateSM();
	

	//初始化
	BOOL bRet = GetSM()->Init();
	if (bRet)
	{
		while(1)
		{
			if (!GetSM()->MainLoop())
			{
				break;
			}
		}	
	}
	if(GetSM())
		GetSM()->Release();
	DeleteSM();
#ifndef _DEBUG
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
	{
		
	}
#endif
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP17"));
	SetEvent(g_hSMThreadExitEvent);
	_endthreadex(0);
	return 0;
}

BOOL CSM::MainLoop()
{
	LogTools::OutToFile(true,m_pszLogFile,AppFrame::GetText("IDS_SM_TIP18"));
	// 结束线程
	if (g_bSMThreadExit)
	{
		return false;
	}
	LogTools::OutToFile(false,m_pszLogFile,AppFrame::GetText("IDS_SM_TIP19"));
	//消息处理
	ProcessMessage();
	LogTools::OutToFile(false,m_pszLogFile,AppFrame::GetText("IDS_SM_TIP20"));
	static DWORD now = timeGetTime(),old = now,dwsendhardinfo = now,reconntime = now,pingtime=now;
	now = timeGetTime(); 
	/*if (now - old >GetSetup()->dwGetHardwareTime)
	{
		CHardWareInfo::GetInstance().GetHardwareInfo();
		old = now;
	}*/
	
#ifdef SEND_HARDWARE_INFO
	if (now - dwsendhardinfo > GetSccPrama()->dwGetHardWareInfoTime)
	{
		CMessage msg(MSG_SM2SCC_INFO_Notify_HardInfo);
		DBWriteSet dbWriteSet;
		msg.GetDBWriteSet(dbWriteSet);
		CHardWareInfo::GetInstance().AddOneInfo(dbWriteSet);
		msg.SendToAllSCC();
		
		dwsendhardinfo = now;
	}
#endif

	//测试延迟
	if (now - pingtime > GetSccPrama()->dwDelayTestTime)
	{
		//DelayTest();
		pingtime=now;
	}

	//重连SCC
	if (now - reconntime > 10000)
	{
		if (m_CurrScc==NULL && GetSetup()->lSwitch > 0)
		{
			LogTools::OutToFile(false,m_pszLogFile,AppFrame::GetText("IDS_SM_TIP21"));
			if (ConnScc())
			{	
			}		
			LogTools::OutToFile(false,m_pszLogFile,AppFrame::GetText("IDS_SM_TIP22"));
		}
		reconntime = now;
	}
	GetGappTimer()->Expire(now);

	if(g_bRunCmdLine)
	{
		//执行外部命令
		LogTools::OutToFile(false,m_pszLogFile,AppFrame::GetText("IDS_SM_TIP23"));
		RunCmdLine();
		LogTools::OutToFile(false,m_pszLogFile,AppFrame::GetText("IDS_SM_TIP24"));
	}
	LogTools::OutToFile(false,m_pszLogFile,AppFrame::GetText("IDS_SM_TIP25"));
	Sleep(1);
	return true;
}

void Command()
{
	LogTools::OutTextToInterface(AppFrame::GetText("IDS_SM_TIP26"));
}

void CSM::RunCmdLine()
{
	deque<string>dcmdline;
	CCmdLine::GetInstance()->GetAllCmd(dcmdline);
	CMDITER it = dcmdline.begin();
	for (;it!=dcmdline.end();it++)
	{
		char szCmd[BASE_TEXT_SIZE]={0};
		strcpy(szCmd,(*it).c_str());
		if (strlen(szCmd)>BASE_NAME_SIZE)
			continue;
		if (0 == strcmp(szCmd,"1"))
		{
			Command();
			continue;
		}
		else if(0 == strcmp(szCmd,"update"))
		{
			GetSM()->CheckGappVersion(-1);
			continue;
		}
		else if(0 == strcmp(szCmd,"startall"))
		{
			GetSM()->Start(-1);
			continue;
		}
		else if(0 == strcmp(szCmd,"show"))
		{
			GetSM()->ShowGapp();
		}
		else if(0 == strcmp(szCmd,"packres"))
		{
			GetSM()->GetPackRes(-1);
			continue;
		}
		else if (0 == strcmp(szCmd,"clear"))
		{
			system("cls");
			continue;
		}
		else if (0 == strcmp(szCmd,"news"))
		{
		/*	CMessage msg(MSG_SM2WS_SEND_TOPNEWS);
			msg.Add((BYTE)2);
			char szText[32]={0};
			strcpy(szText,"系统公告：SM发送");
			msg.Add((long)strlen(szText));
			msg.AddEx(szText,strlen(szText));
			msg.SendToGapp(6);*/
			continue;
		}
		string str(szCmd);
		long pos = (long)str.find("shutdown");
		if(pos >=0 && pos <0xffff&& str.length()>strlen("shutdown"))
		{
			char szID[BASE_NAME_SIZE]="";
			strcpy(szID,(char*)&str[strlen("shutdown")+1]);
			long lGappID = atoi(szID);
			GetSM()->ShutDown(lGappID);
			continue;
		}
		pos = (long)str.find("start");
		if(pos >=0 && pos <0xffff&& str.length()>strlen("start"))
		{
			char szID[BASE_NAME_SIZE]={0};
			strcpy(szID,(char*)&str[strlen("start")+1]);
			long lGappID = atoi(szID);
			GetSM()->Start(lGappID);
			continue;
		}
		pos = (long)str.find("update");
		if(pos >=0 && pos <0xffff&& str.length()>strlen("update"))
		{
			char szID[BASE_NAME_SIZE]="";
			strcpy(szID,(char*)&str[strlen("update")+1]);
			long lGappID = atoi(szID);
			GetSM()->CheckGappVersion(lGappID);
			continue;
		}
		pos = (long)str.find("switch");
		if(pos >=0 && pos <0xffff && str.length()>strlen("switch"))
		{
			char szID[BASE_NAME_SIZE]="";
			strcpy(szID,(char*)&str[strlen("switch")+1]);
			long bSwitch = atoi(szID);
			if (bSwitch>0)
			{
				GetSM()->GetSetup()->lSwitch = 1;
				LogTools::OutTextToInterface(AppFrame::GetText("IDS_SM_TIP27"));
			}
			else
			{
				GetSM()->GetSetup()->lSwitch = 0;
				LogTools::OutTextToInterface(AppFrame::GetText("IDS_SM_TIP28"));
			}

			continue;
		}

		pos = (long)str.find("packres");
		if(pos >=0 && pos <0xffff &&str.length()>strlen("packres"))
		{
			char szID[BASE_NAME_SIZE]="";
			strcpy(szID,(char*)&str[strlen("packres")+1]);
			long lGappID = atoi(szID);
			GetSM()->GetPackRes(lGappID);
			continue;
		}
			
		pos = (long)str.find("set");
		
		if(pos >=0 && pos <0xffff&&str.length()>strlen("set"))
		{
			char szID[BASE_NAME_SIZE]="";
			strcpy(szID,(char*)&str[strlen("set")+1]);
			long lGappID = atoi(szID);
			CGapp* pGapp=FindGapp(lGappID);
			if (pGapp!=NULL)
			{
				pGapp->SetLSPort(2346);
			}
			continue;
		}
			
		
		//Sleep(10);
	}
}
BOOL CSM::ProcessMessage()
{
	static msgQueue TemptMsgQueue;
	//网络消息
	if(s_pNetServer)
	{
		s_pNetServer->GetRecvMessages()->GetAllMessage(TemptMsgQueue);
		msgQueue::iterator it = TemptMsgQueue.begin();
		
		for(;it != TemptMsgQueue.end();it++)
		{
			CMessage* pMsg = (CMessage*)(*it);
			if( pMsg )
			{
				pMsg->Run();
				CBaseMessage::FreeMsg(pMsg);
			}
		}
	}
	TemptMsgQueue.clear();
	//通过内存映射的消息
	CMsgQueue* pMsgs = GetInst(CMemServer).GetRecvMsgs();
	if (pMsgs != NULL)
	{
		pMsgs->GetAllMessage(TemptMsgQueue);
		msgQueue::iterator it = TemptMsgQueue.begin();
		for(;it != TemptMsgQueue.end();it++)
		{
			CMessage* pMsg = (CMessage*)(*it);
			if( pMsg )
			{
				pMsg->Run();
				CBaseMessage::FreeMsg(pMsg);
			}
		}

		TemptMsgQueue.clear();
	}
	return true;
}

CSM::tagSCC* CSM::GetSCC(const char* strIP,const char* strPass)
{
	for(map<long, tagSCC>::iterator it=s_mapSCC.begin(); it!=s_mapSCC.end(); it++)
	{
		tagSCC *p = &it->second;
		TMD5 tmd5;
		LONG lCopyLen = (LONG)strlen(p->cPassward);
		char OutPut[MD5_16_PASSWORD_SIZE]={0};
		tmd5.MessageDigest((const unsigned char *)p->cPassward, lCopyLen, (unsigned char*)OutPut);
		if (0 == strcmp(p->strIP.c_str(),strIP) &&
			0 == memcmp(strPass,OutPut,MD5_16_PASSWORD_SIZE))
		{
			//IP与密码相符,是否重复连接
			/*if (!p->bConnected)
			{
				return p;
			}
			else
			{
				s_pNetServer->DisConn(CMessage::GetSocketBySCCID(p->dwIndex));
				return NULL;
			}*/
		}
	}
	return NULL;
}

bool CSM::ConnScc()
{
	SCCITER it = s_mapSCC.begin();
	for (;it!= s_mapSCC.end();it++)
	{
		tagSCC tgscc = it->second;
		if (0 == tgscc.lAuto)
			continue;
		m_lSocketID = s_pNetServer->Connect(tgscc.strIP.c_str(),tgscc.dwPort,NF_SCC_Client,1);
		
		if (m_lSocketID>0)
		{
			//当前连接的SCC
			m_CurrScc = &(it->second);
			LogTools::OutLog(AppFrame::GetText("IDS_SM_TIP29"),tgscc.dwIndex,tgscc.strIP.c_str(),m_lSocketID);
			CMessage::SetSccSocket(m_lSocketID);
			//MD5
			TMD5 tmd5;
			char szpass[MD5_16_PASSWORD_SIZE]={0};
			strcpy(szpass,tgscc.cPassward);
			LONG lCopyLen = (LONG)strlen(szpass);
			char OutPut[MD5_16_PASSWORD_SIZE]={0};
			tmd5.MessageDigest((const unsigned char *)szpass, lCopyLen, (unsigned char*)OutPut);
			//发送密码登录验证
			CMessage msg(MSG_SM2SCC_LOGIN_Request_Login);
			msg.AddEx(OutPut,MD5_16_PASSWORD_SIZE);
			msg.Add(CURR_VERSION);
			msg.SendToAllSCC();
			return true;
		}
		else
		{
			LogTools::OutLog(AppFrame::GetText("IDS_SM_TIP30"));
		}
	}
	return false;
}
BOOL CSM::OnSCCClose()
{
	/*tagSCC* pSCC = GetSCC(dwIndex);
	if (pSCC)
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP31"),dwIndex);
		pSCC->bConnected = false;
	}*/
	LogTools::OutLog(AppFrame::GetText("IDS_SM_TIP31"));
	CMessage::SetSccSocket(0);
	m_lSocketID = 0;
	m_CurrScc = NULL;
	return TRUE;
}

BOOL CSM::LoadGappInfo(const char* filename)
{
	TiXmlNode *pNode = NULL; 		

	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox;
	if(!m_Tdoc->LoadFile())
		return FALSE;

	pNode=m_Tdoc->FirstChild("Gappinfo");

	if (pNode==NULL)
		return FALSE;

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
		return FALSE;

	TiXmlElement* p=te->FirstChildElement();
	for (; p!=NULL; p=p->NextSiblingElement())
	{
		CGapp* pGapp = new CGapp;
		BOOL bRet = pGapp->Init(p);
		//pGapp->Update();
		if (bRet)
		{
			if (FindGapp(pGapp->GetID()))
			{
				LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP32"),pGapp->GetID());
				continue;
			}
			m_mapGappInfo[pGapp->GetID()] = pGapp;
			LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP33"),pGapp->GetID(),pGapp->GetDisplayName(),pGapp->GetVersion());
		}
	}
	return TRUE;
}

BOOL CSM::AddGapp(CGapp* pGapp)
{
	if (!pGapp)
	{
		return FALSE;
	}
	m_mapGappInfo[pGapp->GetID()] = pGapp;
	return TRUE;
}


//发送所有GAPP信息
void CSM::SendGappInfo()
{
	CMessage msg(MSG_SM2SCC_INFO_Notify_Gapp_BaseInfo);
	GAPPITER it=m_mapGappInfo.begin();
	DBWriteSet dbWriteSet;
	msg.GetDBWriteSet(dbWriteSet);
	dbWriteSet.AddToByteArray((long)m_mapGappInfo.size());
	for (;it!=m_mapGappInfo.end();it++)
	{
		CGapp* pGapp=it->second;
		if (pGapp)
		{
			pGapp->AddToByteArray(dbWriteSet);
		}
	}
	msg.SendToAllSCC();
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP34"));
}
//发送所有硬件信息
void CSM::SendHardWareInfo()
{
	CMessage msg(MSG_SM2SCC_INFO_Notify_HardInfoList);
	DBWriteSet dbWriteSet;
	msg.GetDBWriteSet(dbWriteSet);
	CHardWareInfo::GetInstance().AddToByteArray(dbWriteSet);
	msg.SendToAllSCC();
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP35"));
	CHardWareInfo::GetInstance().ClearInfoList();
}

void CSM::DelayTest()
{
	GAPPITER it=m_mapGappInfo.begin();
	for (;it!=m_mapGappInfo.end();it++)
	{
		CGapp* pGapp = it->second;
		if (pGapp)
			pGapp->DelayTest();	
	}
}

bool CSM::UpdateGappByType(DWORD dwType,DBReadSet& dbreadset)
{
	GAPPITER it = m_mapGappInfo.begin();
	for (;it!=m_mapGappInfo.end();it++)
	{
		CGapp* pGapp = it->second;
		if (pGapp)
		{
			if (dwType ==pGapp->GetType())
			{
				//更新到最新版本
				pGapp->SetIsLatest(1);
				pGapp->CheckVersion();
			}
		}
	}
	return true;
}

BOOL CSM::CheckGappVersion(long lGappID)
{
	if (m_CurrScc == NULL)
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP36"));
		return FALSE;
	}

	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP37"),m_CurrScc->dwIndex,m_CurrScc->strIP.c_str());
	if (-1 == lGappID)
	{
		GAPPITER it = m_mapGappInfo.begin();
		for (;it!=m_mapGappInfo.end();it++)
		{
			CGapp* pGapp = it->second;
			if (pGapp)
			{
				pGapp->CheckVersion();
			}
		}
	}
	else
	{
		CGapp* pGapp = FindGapp(lGappID);
		if (pGapp)
		{
			pGapp->CheckVersion();
		}
		else
		{
			LogTools::OutTextToInterface(AppFrame::GetText("IDS_SM_TIP38"));
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CSM::ShutDown(long id)
{
	CGapp* pGapp = FindGapp(id);
	if (pGapp == NULL)
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP39"),id);
		return FALSE;
	}
	pGapp->ShutDown();
	return TRUE;
}

BOOL CSM::Start(long id)
{
	if (-1 == id)
	{
		GAPPITER it = m_mapGappInfo.begin();
		for (;it!= m_mapGappInfo.end();it++)
		{
			it->second->Start();
		}
		return TRUE;
	}
	CGapp* pGapp = FindGapp(id);
	if (pGapp == NULL)
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP39"),id);
		return FALSE;
	}
	pGapp->Start();
	return TRUE;
}

BOOL CSM::ShowGapp()
{
	GAPPITER it = m_mapGappInfo.begin();
	for (;it!=m_mapGappInfo.end();it++)
	{
		char msg[BASE_TEXT_SIZE]={0};
		sprintf(msg,">> ID:%d	名称：%s	",it->second->GetID(),it->second->GetDisplayName());	
		cout<<msg<<endl;	
	}
	return TRUE;
}

BOOL CSM::GetPackRes(long id)
{
	if (m_CurrScc == NULL)
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP36"));
		return FALSE;
	}
	//获取所有GAPP原始包
	if (-1 == id)
	{
		GAPPITER it = m_mapGappInfo.begin();
		for (;it!=m_mapGappInfo.end();it++)
		{
			LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP40"),m_CurrScc->dwIndex,
				m_CurrScc->strIP.c_str(),it->second->GetDisplayName());
			it->second->GetPackRes();
		}
		return true;
	}
	

	
	GAPPITER it = m_mapGappInfo.find(id);
	if (it!=m_mapGappInfo.end())
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP40"),m_CurrScc->dwIndex,
			m_CurrScc->strIP.c_str(),it->second->GetDisplayName());
		it->second->GetPackRes();
	}
	else
	{
		char msg[BASE_TEXT_SIZE+1];
		sprintf(msg,AppFrame::GetText("IDS_SM_TIP91"),id);
		msg[BASE_TEXT_SIZE]='\0';
		LogTools::OutTextToInterface(msg);
	}
	return TRUE;
}

//获取一个定时器参数
tagSMTimerVar* CSM::GetTimerVar()
{
	tagSMTimerVar* result=0;

	if (m_listTimerManager.size()!=0)
	{
		result= m_listTimerManager.front();
		m_listTimerManager.pop_front();
	}
	else
	{
		result= new tagSMTimerVar;
	}
	return  result;
}

//释放一个定时器参数
void CSM::FreeTimerVar(tagSMTimerVar* p)
{
	if (m_listTimerManager.size()> 100)
	{
		SAFE_DELETE(p);
	}
	else
	{
		if (p== NULL)	return;
		m_listTimerManager.push_back(p);
	}
}

//释放定时器参数管理器
void CSM::ReleaseTimerVarManager()
{
	list<tagSMTimerVar*>::iterator it= m_listTimerManager.begin();
	for (; it!= m_listTimerManager.end(); it++)
	{
		SAFE_DELETE(*it);
	}
	m_listTimerManager.clear();
}


void CSM::SendError(const char* Info,...)
{
	char szText[512]={0};
	va_list va;
	va_start(va,Info);
	vsprintf(szText,Info,va);
	va_end(va);
	CMessage msg(MSG_SM2SCC_INFO_Notify_Error);
	msg.Add((long)strlen(szText));
	msg.AddEx(szText,(long)strlen(szText));
	msg.SendToAllSCC();

	LogTools::OutLogAndTxt(szText);
}

BOOL CSM::InitSccPrama(CMessage* pMsg)
{
	if (pMsg==NULL)
	{
		m_SCCPrama.dwDelayTestTime=1000;
		m_SCCPrama.dwGetHardWareInfoTime=1000;
		m_SCCPrama.dwPingTimeOut=2000;
		m_SCCPrama.dwSaveHardWareInfoSize=3;
		m_SCCPrama.dwShutdownTimeOut=3000;
		m_SCCPrama.dwStartTimeOut=120000;
		return TRUE;
	}
	pMsg->GetEx(&m_SCCPrama,sizeof(m_SCCPrama));
	return TRUE;
}

DWORD CSM::GetGappNumByType(eGappType eType)
{
	DWORD lResult=0;
	map<long,CGapp*>::iterator it=m_mapGappInfo.begin();
	for (;it!=m_mapGappInfo.end();it++)
	{
		if (it->second->GetType()==eType)
		{
			lResult+=1;
		}
	}
	return lResult;
}

bool CSM::IsSameProcID(DWORD dwProcID)
{
	map<long,CGapp*>::iterator it=m_mapGappInfo.begin();
	for (;it!=m_mapGappInfo.end();it++)
	{
		if (it->second->GetProcessID()==dwProcID)
		{
			return true;
		}
	}
	return false;
}