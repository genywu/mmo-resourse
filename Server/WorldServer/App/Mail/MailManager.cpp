#include "StdAfx.h"
#include "MailManager.h"
#include "../DBEntity/EntityManager.h"
#include "../Goods/GoodsFactory.h"
#include "../Linkman/LinkmanSystem.h"

#include "../../../../Public/Setup/GlobalSetup.h"
#include "../../../../Public/Setup/MailList.h"

CMailSystem *CMailSystem::instance=NULL;
CTimerQueue<CGameEvent*> * CMailSystem::m_pMailTimer = NULL;
long g_lMailCount=0;


CMailSystem::CMailSystem()
{
	m_mapMail.clear();
}

CMailSystem *CMailSystem::GetInstance()
{
	if(instance==NULL)
	{
		instance = new CMailSystem();
		m_pMailTimer = new MailTimer(1024,1000);
	}
	return instance;
}



CMailSystem::~CMailSystem()
{
	map<CGUID,CMail*>::iterator it=m_mapMail.begin();
	for (; it!=m_mapMail.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}
	m_mapMail.clear();

	
	for (map<DWORD,CMail*>::iterator it=m_mapSysMail.begin(); it!=m_mapSysMail.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}
	m_mapSysMail.clear();


	for (map<CGUID,CMail*>::iterator it=m_mapTempMail.begin(); it!=m_mapTempMail.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}
	m_mapTempMail.clear();

	for (map<CGUID,list<CMail*>>::iterator it=m_mapPlayerMail.begin(); it!=m_mapPlayerMail.end();++it)
	{
		list<CMail*> lMail = (*it).second;
		list<CMail*>::iterator mit = lMail.begin();
		for (;mit!=lMail.end();++mit)
		{
			SAFE_DELETE(*mit);
		}		
	}
	m_mapPlayerMail.clear();

	for (map<long,tagTimerVar*>::iterator it=m_mapTimer.begin(); it!=m_mapTimer.end(); ++it)
	{
		SAFE_DELETE((*it).second->pvar);
		SAFE_DELETE((*it).second);
	}
	m_mapTimer.clear();

	
	for (map<DWORD,stTimerParam*>::iterator it =  m_mapTimerParam.begin();it!=m_mapTimerParam.end(); ++it)
	{		
		SAFE_DELETE((*it).second);
	}
	m_mapTimerParam.clear();

	
	m_mapSysMail.clear();



	SAFE_DELETE(m_pMailTimer);
}

//转发邮件
void CMailSystem::SendMail(CMail *pMail,long lSysID)
{

	CPlayer *pWriter = GetGame().GetMapPlayer(pMail->GetWriterExID());
	if (!GetNetHandler().IsServerReady(NF_DB_SERVER))
	{
		//返回发信失败消息
		//////////////////////////////////////////////////////////////////////////
		if (pWriter)
		{
			FailedSendMail(pWriter,MAIL_SENDING_FAILED_INVALID_STATE);	//向GameServer发送 MSG_W2S_MAIL_SEND_FAILED 消息		
		}
		//////////////////////////////////////////////////////////////////////////

		return;
	}
	//设定邮件保存时间
	if (pMail)
	{
		time_t t_CurrentTime;
		long lType = pMail->GetType();		
		string strText="";
		t_CurrentTime = time(NULL);
		if (lType>=MAX_MAIL)
		{
			lType = SYSTEM_MAIL;
			pMail->SetText(strText.c_str());			
		}
		else if (lType==SENDGOLD_MAIL)
		{
			lType = AFFIX_MAIL;
		}
		time_t tRemainTime =  t_CurrentTime + (long)GlobalSetup::GetSetup()->fMaxRemainTime[lType]*86400 ;
		pMail->SetRemainTime(tRemainTime);
	}
	for (list<string>::iterator it=pMail->GetReceiverList().begin(); it!=pMail->GetReceiverList().end(); ++it)
	{
		if ((*it)=="Everybody")
		{
			SendSysMail(lSysID);
		}
		else
		{
			pMail->SetReceiver((*it));
			CPlayer *pPlayer = GetGame().GetMapPlayer((*it).c_str());

			tagMailParam* pNewMailParam = new tagMailParam;
			InitMailParam(pNewMailParam,pMail->GetBaseParm());
			CMail *pNewMail = new CMail(pNewMailParam);

			CGUID MailGuid;
			CGUID::CreateGUID(MailGuid);
			pNewMail->SetExID(MailGuid);		

			if(pPlayer)
			{	
				//在线玩家
				pNewMail->SetReceiverExID(pPlayer->GetExID());
				if(!(LinkmanSystem::GetInstance().CanMail(pPlayer,pMail->GetWriter().c_str())) )					
				{
					//返回发信失败消息
					//////////////////////////////////////////////////////////////////////////
					if (pWriter)
					{
						FailedSendMail(pWriter,MAIL_SENDING_FAILED_INVALID_RECEIVER);
					}
					//////////////////////////////////////////////////////////////////////////
					SAFE_DELETE(pNewMail);					
					return;
				}
			}
			SaveMailToTempMail(pNewMail);//--邮件保存在临时邮件列表中(定期删除)
			SendMailMsgToGS(pNewMail->GetExID(),1);	
		}
	}
}
//邮件发送失败
void	CMailSystem::FailedSendMail(CPlayer *pPlayer,long lValue)
{
	if (pPlayer)
	{
		CMessage msg(MSG_W2S_MAIL_SEND_FAILED);
		msg.Add(lValue);
		msg.Add(pPlayer->GetExID());
		msg.SendToSocket(pPlayer->GetGsSocket());
	}
}

//删除临时邮件
void	CMailSystem::DeleteTempMail(CGUID &guid)
{
	map<CGUID,CMail*>::iterator it = m_mapTempMail.find(guid);
	if (it!=m_mapTempMail.end())
	{
		CMail *pMail = (*it).second;
		m_mapTempMail.erase(it);
		SAFE_DELETE(pMail);
	}
}
//保存临时邮件
bool CMailSystem::SaveMailToTempMail(CMail *pMail)
{
	map<CGUID,CMail*>::iterator it = m_mapTempMail.find(pMail->GetExID());
	if (it!=m_mapTempMail.end())
	{
		SAFE_DELETE((*it).second);
		m_mapTempMail.erase(it);		
	}


	CPlayer *pPlayer=GetGame().GetMapPlayer(pMail->GetReceiverExID());
	if (pPlayer)//--玩家在线
	{
		CGUID MailGuid = pMail->GetExID();
		pPlayer->AddMailList(MailGuid);
	}
	m_mapTempMail[pMail->GetExID()]  = pMail;

	return true;
}
//邮件数据库保存
bool CMailSystem::SaveMailToDB(CPlayer *pPlayer,CMail *pMail)
{	
	if(pMail)
	{		
		if((LinkmanSystem::GetInstance().CanMail(pPlayer,pMail->GetWriter().c_str())) )
		{
			//发送者不在收信的黑名单中					
			return true;
		}				
	}
	return false;
}
void	CMailSystem::SaveAndSendMailToReceiver(CMail *pMail)
{
	CPlayer *pPlayer=GetGame().GetMapPlayer(pMail->GetWriterExID());

	
	if (pMail)
	{
#ifdef 	_OPERATE_MAIL	
			char szOutPutStr[1024];
			char szMailGuid[256];
			pMail->GetExID().tostring(szMailGuid); 			
			sprintf(szOutPutStr, "发送保存邮件到DB:玩家:%s发信给玩家:%s,主题:%s,邮件GUID:%s到DB,金钱:%d,物品数量:%d",pMail->GetWriter().c_str(),pMail->GetReceiver().c_str(),pMail->GetSubject().c_str(),szMailGuid,pMail->GetGold(),pMail->GetMGoodsContainer().size());
			CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);

			for (vector<CGoods*>::iterator it=pMail->GetMGoodsContainer().begin(); it!=pMail->GetMGoodsContainer().end();++it)
			{
				char szGoodsGuid[256];
				char szOutStr[256];
				if (*it)
				{
					(*it)->GetExID().tostring(szGoodsGuid);
					sprintf(szOutStr,"邮件%s包含的物品:%s,物品名:%s,数量:%d",szMailGuid,szGoodsGuid,(*it)->GetName(),(*it)->GetAmount());
					CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutStr);
				}
			}	
#endif
		if(pMail->GetType()==SENDGOLD_MAIL) //"付费回复类邮件"
		{
			//设置付款回复类邮件为普通附件邮件
			pMail->SetType(AFFIX_MAIL);
		}
		time_t tCurrentTime = time(NULL);
		pMail->SetWriteTime(tCurrentTime);
		pMail->SetSave(MAIL_SAVE_SAVEING);
		GetGame().GetEntityManager()->CreateInsertMailSession(pMail);
	}
}
//从信中取物品
void CMailSystem::GetGoodsFromMail(CGUID &guid,unsigned long lIndex)
{
	map<CGUID,CMail*>::iterator it=m_mapMail.find(guid);
	if (it!=m_mapMail.end())
	{
		CMail *pMail = (*it).second;
		CPlayer *pPlayer = GetGame().GetMapPlayer(pMail->GetReceiverExID());
		if (pPlayer)
		{
#ifdef 	_OPERATE_MAIL
			char szOutPutStr[1024];
			char szMailGuid[256];
			char szGoodsGuid[256];
			pMail->GetExID().tostring(szMailGuid); 
			if (lIndex<pMail->GetMGoodsContainer().size())
			{
				CGoods *pGoods = (pMail->GetMGoodsContainer()).at(lIndex);
				if (pGoods)
				{
					pGoods->GetExID().tostring(szGoodsGuid);
					sprintf(szOutPutStr, "玩家:%s从邮件:%s中取下物品:%s.物品名:%s,数量:%d",pPlayer->GetName(),szMailGuid,szGoodsGuid,pGoods->GetName(),pGoods->GetAmount());
					CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);
				}
			}	
			else
			{
					char szOutPutStr[1024];
					sprintf(szOutPutStr, "玩家:%s从邮件:%s中取下物品位置%d超出栏位%d.",pPlayer->GetName(),szMailGuid,lIndex,pMail->GetMGoodsContainer().size());
					CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);
			}
#endif
			if (lIndex>=pMail->GetMGoodsContainer().size())
			{
				return;
			}
			pMail->ClearMailGoodsByIndex(lIndex);
			vector<CMail*> vMail;
			vMail.push_back(pMail);
			GetGame().GetEntityManager()->CreateSaveOnlineMailSession(pPlayer->GetExID(),vMail);// --在线玩家保存信件

			CMessage msg(MSG_W2S_MAIL_OPERA_GOODS);
			msg.Add(lIndex);
			msg.Add(guid);
			msg.Add(pPlayer->GetExID());
			msg.SendToSocket(pPlayer->GetGsSocket());
		}			
	}
}
//付款收信
void CMailSystem::PayMail(CGUID &guid)													
{
	map<CGUID,CMail*>::iterator it=m_mapMail.find(guid);
	if (it!=m_mapMail.end())
	{
		CMail *pMail = (*it).second;

		CPlayer *pPlayer = GetGame().GetMapPlayer(pMail->GetReceiverExID());
		if (pPlayer)
		{	
			if (pMail->GetMGoodsContainer().size()>0)
			{
				pMail->SetType(AFFIX_MAIL);
			}
			else
			{
				pMail->SetType(COMMON_MAIL);
			}
			pMail->SetGold(0);

			//  [7/30/2009 chenxianj]
			//付款后重新设定邮件的保存时间
			time_t t_CurrentTime;		
			t_CurrentTime = time(NULL);
			
			time_t tRemainTime =  t_CurrentTime + (long)GlobalSetup::GetSetup()->fMaxRemainTime[pMail->GetType()]*86400 ;
			pMail->SetRemainTime(tRemainTime);	


#ifdef 	_OPERATE_MAIL
			char szOutPutStr[1024];
			char szMailGuid[256];			
			pMail->GetExID().tostring(szMailGuid); 			
			sprintf(szOutPutStr, "玩家:%s为邮件:%s成功付款收信当前邮件类型:%d.",pPlayer->GetName(),szMailGuid,pMail->GetType());
			CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);
#endif

			vector<CMail*> vMail;
			vMail.push_back(pMail);
			GetGame().GetEntityManager()->CreateSaveOnlineMailSession(pPlayer->GetExID(),vMail);

			//  [7/30/2009 chenxianj]
			//更新gs上的邮件(改变该邮件的参数)，注销和重新注册定时器（邮件类型不一样在系统里保存的时间不一样）
			SendMailToGS(pMail,pPlayer);			
			UnReisterTimerOnReject(pMail->GetExID());
			StartRegisterTimer(pMail->GetReceiverExID(),pMail);

		}		
	}
}
//从邮件中取金币
void	CMailSystem::GetGoldFromMail(CGUID &guid)												
{
	map<CGUID,CMail*>::iterator it=m_mapMail.find(guid);
	if (it!=m_mapMail.end())
	{
		CMail *pMail = (*it).second;
		CPlayer *pPlayer = GetGame().GetMapPlayer(pMail->GetReceiverExID());
		if (pPlayer)
		{
#ifdef 	_OPERATE_MAIL
			char szOutPutStr[1024];
			char szMailGuid[256];
			pMail->GetExID().tostring(szMailGuid);

			sprintf(szOutPutStr, "玩家:%s从邮件:%s中取金钱:%d.",pPlayer->GetName(),szMailGuid, pMail->GetGold());
			CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);
#endif

			pMail->SetGold(0);
			pMail->SetReject(0);
			if (pMail->GetMGoodsContainer().size()==0&& pMail->GetType()!=SYSTEM_MAIL&&pMail->GetType()<MAX_MAIL)
			{
				pMail->SetType(COMMON_MAIL);
			}
			vector<CMail*> vMail;
			vMail.push_back(pMail);
			GetGame().GetEntityManager()->CreateSaveOnlineMailSession(pPlayer->GetExID(),vMail);

			CMessage msg(MSG_W2S_MAIL_OPERA_GOLD);
			msg.Add(guid);
			msg.Add(pPlayer->GetExID());
			msg.SendToSocket(pPlayer->GetGsSocket());
		}	
	}
}
//阅读邮件
void CMailSystem::ReadMail(CGUID &guid)
{
	map<CGUID,CMail*>::iterator it=m_mapMail.find(guid);
	if (it!=m_mapMail.end())
	{
		CMail *pMail = (*it).second;
		CPlayer *pPlayer = GetGame().GetMapPlayer(pMail->GetReceiverExID());
		if (pPlayer)
		{
			pMail->SetRead(1);//--设为已读

#ifdef 	_OPERATE_MAIL1
			char szOutPutStr[1024];
			char szMailGuid[256];			
			pMail->GetExID().tostring(szMailGuid); 			
			sprintf(szOutPutStr, "玩家:%s读取邮件:%s,金钱为:%d,物品数量:%d",pPlayer->GetName(),szMailGuid,pMail->GetGold(),pMail->GetMGoodsContainer().size());
			CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);

			for (vector<CGoods*>::iterator it=pMail->GetMGoodsContainer().begin(); it!=pMail->GetMGoodsContainer().end();++it)
			{
				char szGoodsGuid[256];
				char szOutStr[256];
				if (*it)
				{
					(*it)->GetExID().tostring(szGoodsGuid);
					sprintf(szOutStr,"邮件:%s包含的物品有:%s,物品名:%s,数量:%d",szMailGuid,szGoodsGuid,(*it)->GetName(),(*it)->GetAmount());
					CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutStr);

				}
			}
#endif
			vector<CMail*> vMail;
			vMail.push_back(pMail);
			GetGame().GetEntityManager()->CreateSaveOnlineMailSession(pPlayer->GetExID(),vMail);// --在线玩家保存信件到数据库
		}		
	}	
}
//释放玩家未取邮件
void	CMailSystem::ReleasePlayerMail(CPlayer* pPlayer)
{
	if (pPlayer)
	{
		map<CGUID,list<CMail*>>::iterator it = m_mapPlayerMail.find(pPlayer->GetExID());
		if (it!=m_mapPlayerMail.end())
		{
			list<CMail*> lMail = (*it).second;
			list<CMail*>::iterator mit = lMail.begin();
			for (;mit!=lMail.end();++mit)
			{
				SAFE_DELETE(*mit);
			}
			m_mapPlayerMail.erase(it);
		}
	}	
}


//发送所有邮件到GS
void CMailSystem::SendAllMailToGs(CPlayer *pPlayer)								
{
	map<CGUID,CMail*> MailMap;
	if (NULL == pPlayer)
		return;


	map<CGUID,list<CMail*>>::iterator it = m_mapPlayerMail.find(pPlayer->GetExID());
	if (it!=m_mapPlayerMail.end())
	{
		list<CMail*> lMail = (*it).second;
		list<CMail*>::iterator mit = lMail.begin();
		for (;mit!=lMail.end();)
		{
			MailMap[(*mit)->GetExID()] = (*mit);
			mit = lMail.erase(mit);
		}
		m_mapPlayerMail.erase(it);
	}

	if (MailMap.size() > 0)
	{
		for (map<CGUID,CMail*>::iterator it=MailMap.begin(); it!=MailMap.end();)
		{
			pPlayer->AddReceiveMailList((*it).first);
			if(!SaveMailToMailSys(pPlayer, (*it).second))
			{			
				it = MailMap.erase(it);		
			}
			else
			{			
				++it;
			}
		}
	}
	else
	{
		list<CGUID> lReceiveMail = pPlayer->GetReceiveMailList();
		list<CGUID>::iterator it = lReceiveMail.begin();
		for (;it!=lReceiveMail.end();++it)
		{
			map<CGUID,CMail*>::iterator mit = m_mapMail.find((*it));
			if (mit!=m_mapMail.end())
			{
				MailMap[(*mit).first] = (*mit).second;
			}
		}

	}
	vector<BYTE> vectorByte;	
	vectorByte.clear();
	_AddToByteArray(&vectorByte,(long)MailMap.size());
	for (map<CGUID,CMail*>::iterator it=MailMap.begin(); it!=MailMap.end(); ++it)
	{
		(*it).second->AddToByteArrayToGS(vectorByte);

#ifdef 	_OPERATE_MAIL1
		char szOutPutStr[1024];
		char szMailGuid[256];			
		(*it).first.tostring(szMailGuid); 			
		sprintf(szOutPutStr, "玩家:%s接收信邮件:%s,金钱:%d,物品数量:%d",pPlayer->GetName(),szMailGuid,(*it).second->GetGold(),(*it).second->GetMGoodsContainer().size());
		CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);

		for (vector<CGoods*>::iterator git=(*it).second->GetMGoodsContainer().begin(); git!=(*it).second->GetMGoodsContainer().end();++git)
		{
			char szGoodsGuid[256];
			char szOutStr[256];
			if (*git)
			{
				(*git)->GetExID().tostring(szGoodsGuid);
				sprintf(szOutStr,"邮件:%s包含的物品有:%s,物品名:%s,数量:%d",szMailGuid,szGoodsGuid,(*git)->GetName(),(*git)->GetAmount());
				CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutStr);

			}
		}
#endif

	}

	CMessage msg(MSG_W2S_MAIL_RECEIVE);
	msg.Add(pPlayer->GetExID());
	msg.Add((long)vectorByte.size());
	if(vectorByte.size())
	{
		msg.AddEx(&vectorByte[0], (long)vectorByte.size());		
	}
	msg.SendToSocket(pPlayer->GetGsSocket());
}

//向GS返回寄信结果
void CMailSystem::SendMailMsgToGS(const CGUID &MailGuid,int nRet)		
{
	CGUID WriterGuid = NULL_GUID;	
	CGUID ReceiverGuid = NULL_GUID;

	vector<BYTE> vectorByte;	
	vectorByte.clear();
	map<CGUID,CMail*>::iterator it=m_mapTempMail.find(MailGuid);

	if (it!=m_mapTempMail.end())
	{
		CMail *pMail=(*it).second;
		if (pMail)
		{	
			ReceiverGuid = pMail->GetReceiverExID();
			WriterGuid = pMail->GetWriterExID();
			if (nRet==1 && WriterGuid!=NULL_GUID)
			{					
				pMail->AddMailMsgByteArrayToGS(vectorByte);					
			}

			CPlayer *pPlayer = GetGame().GetMapPlayer(WriterGuid);


			if (pPlayer && (pMail->GetType()!=SYSTEM_MAIL && pMail->GetType()<=SENDGOLD_MAIL))
			{
				CMessage msg(MSG_W2S_MAIL_SEND_RESULT);
				msg.Add(WriterGuid);
				msg.Add((long)nRet);
				msg.Add((long)vectorByte.size());
				if (vectorByte.size())
					msg.AddEx(&vectorByte[0],(long)vectorByte.size());
				msg.SendToSocket(pPlayer->GetGsSocket());
			}
			else if(pMail->GetType()==SYSTEM_MAIL)
			{
				//屏蔽脱机离线玩家发送邮件
				//系统类邮件
				time_t tCurrentTime = time(NULL);
				pMail->SetWriteTime(tCurrentTime);
				pMail->SetSave(MAIL_SAVE_SAVEING);//--保存中
				//pMail->ClearSGoods();//?
#ifdef 	_OPERATE_MAIL	
				char szOutPutStr[1024];
				char szMailGuid[256];
				pMail->GetExID().tostring(szMailGuid); 			
				sprintf(szOutPutStr, "发送保存邮件到DB:玩家:%s发信给玩家:%s,主题:%s,邮件GUID:%s到DB,金钱:%d,物品数量:%d",pMail->GetWriter().c_str(),pMail->GetReceiver().c_str(),pMail->GetSubject().c_str(),szMailGuid,pMail->GetGold(),pMail->GetMGoodsContainer().size());
				CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);

				for (vector<CGoods*>::iterator it=pMail->GetMGoodsContainer().begin(); it!=pMail->GetMGoodsContainer().end();++it)
				{
					char szGoodsGuid[256];
					char szOutStr[256];
					if (*it)
					{
						(*it)->GetExID().tostring(szGoodsGuid);
						sprintf(szOutStr,"邮件%s包含的物品:%s,物品名:%s,数量:%d",szMailGuid,szGoodsGuid,(*it)->GetName(),(*it)->GetAmount());
						CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutStr);
					}
				}	
#endif
				GetGame().GetEntityManager()->CreateInsertMailSession(pMail);
			}
			if(nRet==0)
			{	
				it = m_mapMail.erase(it);
				SAFE_DELETE(pMail);
			}
		}		
	}
}
//发信到GS
void CMailSystem::SendMailToGS(CMail *pMail, CPlayer *pPlayer)
{
	if (pPlayer && pMail)
	{
#ifdef 	_OPERATE_MAIL1
		char szOutPutStr[1024];
		char szMailGuid[256];
		pMail->GetExID().tostring(szMailGuid); 			
		sprintf(szOutPutStr, "在线发信: %s发信给玩家:%s,主题:%s, 邮件GUID:%s到DB,金钱:%d,物品数量:%d",pMail->GetWriter().c_str(), pMail->GetReceiver().c_str(),pMail->GetSubject().c_str(), szMailGuid,pMail->GetGold(),pMail->GetMGoodsContainer().size());
		CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);

		for (vector<CGoods*>::iterator it=pMail->GetMGoodsContainer().begin(); it!=pMail->GetMGoodsContainer().end();++it)
		{
			char szGoodsGuid[256];
			char szOutStr[256];
			if (*it)
			{
				(*it)->GetExID().tostring(szGoodsGuid);
				sprintf(szOutStr,"邮件%s包含的物品:%s,物品名:%s,数量:%d",szMailGuid,szGoodsGuid,(*it)->GetName(),(*it)->GetAmount());
				CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutStr);

			}
		}
#endif
		vector<BYTE> vectorByte;	
		vectorByte.clear();
		pMail->AddToByteArrayToGS(vectorByte);

		CMessage msg(MSG_W2S_MAIL_SEND);
		msg.Add(pMail->GetExID());
		msg.Add((long)vectorByte.size());
		if(vectorByte.size())
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());		
		msg.SendToSocket(pPlayer->GetGsSocket());
	}
}


//启动定时器
bool	CMailSystem::StartRegisterTimer(const CGUID &PlayerGuid,CMail *pMail)
{
	time_t t_CurrentTime = time(NULL);
	time_t lRemainTime = 0;	
	lRemainTime = pMail->GetRemainTime() - t_CurrentTime;

	if (lRemainTime>0)
	{
		RegisterTimer(PlayerGuid, pMail->GetExID(),lRemainTime*1000);
		return true;
	}
	else
	{
		list<CGUID> lMailGuid;
		lMailGuid.push_back(pMail->GetExID());
		OnChangeMailDest(pMail);					
		SendDeleteMailToDB(PlayerGuid,lMailGuid);
		return false;
	}
}

//向GS返回付款收信
void CMailSystem::SendPayMailMsgToGs(CPlayer *pPlayer, CGUID &MailGuid)
{
	CMessage msg(MSG_W2S_MAIL_PAY);
	msg.Add(MailGuid);
	msg.Add(pPlayer->GetExID());
	msg.SendToSocket(pPlayer->GetGsSocket());
}

//保存邮件到邮件系统
bool	CMailSystem::SaveMailToMailSys(CPlayer *pPlayer,CMail *pMail)
{
	CGUID MailGuid = pMail->GetExID();
	CGUID PlayerGuid;
	if (pPlayer)
	{
		PlayerGuid = pPlayer->GetExID();
	}
	else
	{
		PlayerGuid = NULL_GUID;
	}

	map<CGUID,CMail*>::iterator itmail = m_mapMail.find(MailGuid);
	if (itmail!=m_mapMail.end())
	{
		SAFE_DELETE((*itmail).second);		
	}
	m_mapMail[MailGuid] = pMail;
		

	if(StartRegisterTimer(PlayerGuid,pMail))
		return true;
	else 
		return false;
}
//获取邮件对象	
CMail   *CMailSystem::GetMail(const CGUID &guid)
{
	map<CGUID,CMail*>::iterator it=m_mapMail.find(guid);
	if (it!=m_mapMail.end())
	{
		return (*it).second;
	}
	return NULL;
}
//获取临时邮件

CMail	*CMailSystem::GetTempMail(const CGUID &guid)
{
	map<CGUID,CMail*>::iterator it=m_mapTempMail.find(guid);
	if (it!=m_mapTempMail.end())
	{
		return (*it).second;
	}
	return NULL;
}

//发送删除邮件to GS
void    CMailSystem::SendDeleteMailToGS(const CGUID &PlayerGuid, list<CGUID> &lMailGuid)
{	
	CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
	for (list<CGUID>::iterator it=lMailGuid.begin(); it!=lMailGuid.end();++it)
	{
		map<CGUID,CMail*>::iterator mit=m_mapMail.find((*it));
		if (mit!=m_mapMail.end())
		{
#ifdef 	_OPERATE_MAIL1
			if (pPlayer)
			{
				char szOutPutStr[1024];
				char szMailGuid[256];			
				(*it).tostring(szMailGuid); 			
				sprintf(szOutPutStr, "收信人:%s删除邮件:%s,并向gs发送删除消息,金钱:%d,物品数量%d,发信人:%s",pPlayer->GetName(),szMailGuid,(*mit).second->GetGold(),(*mit).second->GetMGoodsContainer().size(),((*mit).second->GetWriter()).c_str());
				CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);
				for (vector<CGoods*>::iterator git=(*mit).second->GetMGoodsContainer().begin(); git!=(*mit).second->GetMGoodsContainer().end();++git)
				{
					char szGoodsGuid[256];
					char szOutStr[256];
					if (*git)
					{
						(*git)->GetExID().tostring(szGoodsGuid);
						sprintf(szOutStr,"邮件%s包含的物品有%s,物品名:%s,数量:%d",szMailGuid,szGoodsGuid,(*git)->GetName(),(*git)->GetAmount());
						CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutStr);

					}
				}
			}			
#endif	
			if (pPlayer)
			{
				pPlayer->EraseMail((*it));
			}
			CMail *pMail = (*mit).second;
			mit =  m_mapMail.erase(mit);
			SAFE_DELETE(pMail);

		}		
	}

	//给gs返回删除消息
	size_t lNum = lMailGuid.size();
	if (pPlayer)
	{
		CMessage msg(MSG_W2S_MAIL_DELETE); 	
		msg.Add(PlayerGuid);
		msg.Add((long)lNum);
		for (list<CGUID>::iterator it=lMailGuid.begin(); it!=lMailGuid.end(); ++it)
		{	
			msg.Add((*it));
		}
		msg.SendToSocket(pPlayer->GetGsSocket());
	}
}

//删除邮件
void	CMailSystem::SendDeleteMailToDB(const CGUID &PlayerGuid, list<CGUID> &lMailGuid)
{
	if (NULL_GUID!=PlayerGuid)
	{
		//删除邮件系统的邮件
		vector<CMail*> vMails;
		vMails.clear();
		for (list<CGUID>::iterator it=lMailGuid.begin(); it!=lMailGuid.end();++it)
		{
			map<CGUID,CMail*>::iterator mit=m_mapMail.find((*it));
			if (mit!=m_mapMail.end())
			{
				vMails.push_back((*mit).second);	
#ifdef 	_OPERATE_MAIL1
				CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
				if (pPlayer)
				{
					char szOutPutStr[1024];
					char szMailGuid[256];			
					(*mit).first.tostring(szMailGuid); 			
					sprintf(szOutPutStr, "收信人:%s删除邮件:%s,并向DB发送删除消息,金钱%d,物品数量%d,发信人:%s",pPlayer->GetName(),szMailGuid,(*mit).second->GetGold(),(*mit).second->GetMGoodsContainer().size(),((*mit).second->GetWriter()).c_str());
					CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);

					for (vector<CGoods*>::iterator git=(*mit).second->GetMGoodsContainer().begin(); git!=(*mit).second->GetMGoodsContainer().end();++git)
					{
						char szGoodsGuid[256];
						char szOutStr[256];
						if (*git)
						{
							(*git)->GetExID().tostring(szGoodsGuid);
							sprintf(szOutStr,"邮件%s包含的物品有%s,物品名:%s,数量:%d",szMailGuid,szGoodsGuid,(*git)->GetName(),(*git)->GetAmount());
							CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutStr);

						}
					}
				}				
#endif	
			}
		}
		GetGame().GetEntityManager()->CreateDeleteOnlineMailSession(PlayerGuid,vMails);

		SendDeleteMailToGS(PlayerGuid,lMailGuid);
	}		
}

//转发邮件
bool	CMailSystem::OnChangeMailDest(CMail *pMail)
{
	if (pMail->GetBaseParm())
	{
		if ((pMail->GetMGoodsContainer().size()>0 || pMail->GetGold()>0) && pMail->GetType()!=SYSTEM_MAIL && pMail->GetType()<SENDGOLD_MAIL)
		{

			//转发			
			if (pMail->GetType() == PAYGOLD_MAIL)
			{
				pMail->SetGold(0);
			}
			time_t t_CurrentTime;
			char   strSysMsg[128];
			char   strSuject[128];
			char   strText[1280];
			t_CurrentTime = time(NULL);

			wsprintf(strSysMsg,AppFrame::GetText("GS_SYSMAIL_THIRD"));
			wsprintf(strSuject,AppFrame::GetText("GS_SYSMAIL_EIGHTH"),pMail->GetSubject().c_str());
			wsprintf(strText,AppFrame::GetText("GS_SYSMAIL_TWELVE"),pMail->GetText().c_str());



			time_t tRemainTime =  t_CurrentTime + (long)GlobalSetup::GetSetup()->fMaxRemainTime[SYSTEM_MAIL]*86400 ;
			pMail->SetType(SYSTEM_MAIL);	
			pMail->SetReject(0);
			pMail->GetReceiverList().clear();
			pMail->SetReceiverList(pMail->GetWriter().c_str());
			pMail->SetReceiver(pMail->GetWriter());
			pMail->SetReceiverExID(pMail->GetWriterExID());
			pMail->SetSubject(strSuject);
			pMail->SetRemainTime(tRemainTime);
			pMail->SetWriter(strSysMsg);
			pMail->SetWriterID(NULL_GUID);
			pMail->SetWriteTime(t_CurrentTime);
			pMail->SetText(strText);
#ifdef 	_OPERATE_MAIL1	
			char szOutPutStr[1024];
			char szMailGuid[256];
			pMail->GetExID().tostring(szMailGuid); 			
			sprintf(szOutPutStr, "退信发送保存邮件到DB:玩家:%s发信给玩家:%s,主题:%s,邮件GUID:%s到DB,金钱:%d,物品数量:%d",pMail->GetWriter().c_str(),pMail->GetReceiver().c_str(),pMail->GetSubject().c_str(),szMailGuid,pMail->GetGold(),pMail->GetMGoodsContainer().size());
			CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);

			for (vector<CGoods*>::iterator it=pMail->GetMGoodsContainer().begin(); it!=pMail->GetMGoodsContainer().end();++it)
			{
				char szGoodsGuid[256];
				char szOutStr[256];
				if (*it)
				{
					(*it)->GetExID().tostring(szGoodsGuid);
					sprintf(szOutStr,"邮件%s包含的物品:%s,物品名:%s,数量:%d",szMailGuid,szGoodsGuid,(*it)->GetName(),(*it)->GetAmount());
					CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutStr);
				}
			}	
#endif

			SendMail(pMail);				
		}
		return true;
	}
	return false;
}
//退信
void	CMailSystem::RejectMail(CPlayer *pPlayer, CGUID &guid)
{

	CMail *pMail = GetMail(guid);
	list<CGUID> lMailGuid;
	CGUID playerguid = NULL_GUID;
	lMailGuid.push_back(guid);
	if (pPlayer)
	{
		playerguid = pPlayer->GetExID();
	}

	if (pMail && pMail->GetReject()  && pMail->GetType()!=SYSTEM_MAIL && pMail->GetType()<SENDGOLD_MAIL)
	{
		char   strSysMsg[128];
		char   szSysMsg[128];
		char   strText[1280];
		sprintf(strSysMsg,AppFrame::GetText("GS_SYSMAIL_THIRD"));
		sprintf(szSysMsg,AppFrame::GetText("GS_SYSMAIL_FOURTH"),pMail->GetReceiver().c_str());
		sprintf(strText,AppFrame::GetText("GS_SYSMAIL_ELEVEN"),pMail->GetText().c_str());

		time_t t_CurrentTime = time(NULL);
		time_t tRemainTime =  t_CurrentTime + (long)GlobalSetup::GetSetup()->fMaxRemainTime[SYSTEM_MAIL]*86400 ;
		UnReisterTimerOnReject(pMail->GetExID());
		CPlayer *pReceiver = GetGame().GetMapPlayer(pMail->GetWriterExID());
		pMail->GetReceiverList().clear();
		pMail->SetReceiverList(pMail->GetWriter().c_str());	
		pMail->SetRemainTime(tRemainTime);
		pMail->SetReceiver(pMail->GetWriter());
		pMail->SetReceiverExID(pMail->GetWriterExID());
		pMail->SetText(strText);

		pMail->SetSave(MAIL_SAVE_SUCCESS_SAVE);
		if(pMail->GetType()==PAYGOLD_MAIL)
		{
			pMail->SetGold(0);
		}

		pMail->SetWriter(strSysMsg);
		pMail->SetWriterID(NULL_GUID);
		pMail->SetType(SYSTEM_MAIL);
		pMail->SetRead(0);
		pMail->SetReject(0);
		StartRegisterTimer(pMail->GetReceiverExID(),pMail);

		string strSubject = szSysMsg;
		strSubject +=pMail->GetSubject();
		pMail->SetSubject(strSubject.c_str());

		
		if (pReceiver)
		{
			//收信人在线
			SaveMailToTempMail(pMail);		
		}
		
		m_mapMail.erase(guid);

		vector<CMail*> vMail;
		vMail.push_back(pMail);
		GetGame().GetEntityManager()->CreateSaveOnlineMailSession(pMail->GetWriterExID(),vMail);

		if(!pReceiver)
		{
			//释放邮件
			SAFE_DELETE(pMail);
		}
		if (pPlayer)
		{
			pPlayer->EraseMail(guid);

			list<CGUID> lMailGuid;
			lMailGuid.push_back(guid);
			size_t lNum = lMailGuid.size();
			CMessage msg(MSG_W2S_MAIL_DELETE); 	
			msg.Add(pPlayer->GetExID());
			msg.Add((long)lNum);
			for (list<CGUID>::iterator it=lMailGuid.begin(); it!=lMailGuid.end(); ++it)
			{	
				msg.Add((*it));
			}
			msg.SendToSocket(pPlayer->GetGsSocket());
		}
	}	

}

void CMailSystem::Run()
{
	DWORD dwCurTime = timeGetTime();
	m_pMailTimer->Expire(dwCurTime);
}

long CMailSystem::Schedule(	CGameEvent* const pEvent,const void *arg,unsigned long futuretime,unsigned long interval)
{
	return m_pMailTimer->Schedule(pEvent,arg,futuretime,interval);
}

int CMailSystem::Cancel(CGameEvent* const pEvent)
{
	return m_pMailTimer->Cancel(pEvent);
}

//取消guid的定时器
int CMailSystem::Cancel(long timerid, const void** arg)
{
	return m_pMailTimer->Cancel(timerid,arg);
}


//网络消息响应
void CMailSystem::OnMessage(CMessage*)
{

}
//退信后注销定时器
void	CMailSystem::UnReisterTimerOnReject(const CGUID &MailGuid)		
{
	map<long,tagTimerVar*>::iterator it=m_mapTimer.begin();
	for (;it!=m_mapTimer.end();)
	{
		CMail::tagMail *ptgmail = reinterpret_cast<CMail::tagMail*>((it->second)->pvar);
		if (MailGuid==ptgmail->MailGuid)
		{
			UnRegisterTimer(it->first);
			SAFE_DELETE(ptgmail);
			SAFE_DELETE(it->second);
			it=m_mapTimer.erase(it);
			break;
		}
		++it;
	}
}

//超时响应
long CMailSystem::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
{

	const tagTimerVar* pTimerVar = (const tagTimerVar*)var;

	CMail::tagMail *ptgmail = reinterpret_cast<CMail::tagMail*>(pTimerVar->pvar);

	if (pTimerVar->TimerType == ChildObjType_Mail)
	{
		list<CGUID> lMailGuid;
		lMailGuid.push_back(ptgmail->MailGuid);
		map<CGUID,CMail*>::iterator it=m_mapMail.find(ptgmail->MailGuid);
		if (it!=m_mapMail.end())
		{
			UnRegisterTimer(timerid);
			OnChangeMailDest((*it).second);
			SendDeleteMailToDB(ptgmail->PlayerGuid,lMailGuid);		
			map<long,tagTimerVar*>::iterator it=m_mapTimer.find(timerid);
			if (it!=m_mapTimer.end())
			{
				SAFE_DELETE(ptgmail);
				SAFE_DELETE(pTimerVar);
				it = m_mapTimer.erase(it);
			}		
			
		}
	}	 
	else if (pTimerVar->TimerType == ChildObjType_SysMail)//"系统邮件"
	{
		long lSysMailID = ptgmail->lMailID;
		CMail *pSysMail = GetSysMail(lSysMailID);
		map<DWORD,stTimerParam*>::iterator it=m_mapTimerParam.find(lSysMailID);
		if (it!=m_mapTimerParam.end())
		{
			if ((*it).second->lCalledCount<=(*it).second->lCount)
			{
				++(*it).second->lCalledCount;
				UnRegisterSysMailTimer(timerid);		
				SendMail(pSysMail,lSysMailID);//“向GS发邮件”

			}
		}
	}
	else if (pTimerVar->TimerType == ChildObjType_Mail_CheckTime)// --邮件定时检查
	{
		list<CGUID> lMailGuid;
		lMailGuid.push_back(ptgmail->MailGuid);
		CPlayer *pPlayer = GetGame().GetMapPlayer(ptgmail->PlayerGuid);

		if (pPlayer)
		{
			list<CGUID> &lNewMailGuid = pPlayer->GetNewMailList();
			list<CGUID>::iterator it = lNewMailGuid.begin();
			for (;it!=lNewMailGuid.end();)
			{
				map<CGUID,CMail*>::iterator mit=m_mapTempMail.find(*it);

				if (mit!=m_mapTempMail.end() )
				{
					if ((*mit).second->GetSave()==MAIL_SAVE_SUCCESS_SAVE)
					{
						CGUID MailGuid = (*mit).first;
						SendMailToGS((*mit).second,pPlayer);
						pPlayer->AddReceiveMailList(MailGuid);
					
						SaveMailToMailSys(pPlayer,(*mit).second);
						m_mapTempMail.erase(mit);
						 it = lNewMailGuid.erase(it);
						 continue;
					}				
				}
				++it;
			}			
		}		
	}
	map<long,tagTimerVar*>::iterator it=m_mapTimer.find(timerid);
	if (it!=m_mapTimer.end() && pTimerVar->TimerType!=ChildObjType_Mail_CheckTime)
	{
		SAFE_DELETE(ptgmail);
		SAFE_DELETE(pTimerVar);
		it = m_mapTimer.erase(it);
	}
	return 1;
}
//当定时器被取消的调用
void CMailSystem::OnTimerCancel(DWORD timerid,const void* var)
{

}
//处理实体间消息函数
long CMailSystem::Proc(eEventType type,const tagProcVar *pVar)
{
	return 1;
}
//注册邮件检查定时器
void	CMailSystem::RegisterCheckTime(const CGUID &PlayerGuid,long lFuture)
{
	void* pVar = new tagTimerVar;
	tagTimerVar *ptgTimerVar = (tagTimerVar*)pVar;

	((tagTimerVar*)pVar)->TimerType = ChildObjType_Mail_CheckTime;
	CMail::tagMail *ptgMail = new CMail::tagMail;
	ptgMail->PlayerGuid = PlayerGuid;
	ptgMail->MailGuid = NULL_GUID;
	((tagTimerVar*)pVar)->pvar = ptgMail;

	long lTimerID = Schedule(this, pVar, lFuture + timeGetTime(),lFuture);	
	m_mapCheckMailTimer[PlayerGuid] = lTimerID;
	m_mapTimer[lTimerID] = ptgTimerVar;
}
//注消邮件延时定时器
void	CMailSystem::UnRegisterCheckTime(const CGUID &PlayerGuid)		
{
	void* pVar = NULL;	
	map<CGUID,long>::iterator tit=m_mapCheckMailTimer.find(PlayerGuid);
	if (tit!=m_mapCheckMailTimer.end())
	{
		map<long,tagTimerVar*>::iterator it=m_mapTimer.find((*tit).second);
		if (it!=m_mapTimer.end())
		{
			tagTimerVar *ptgTimerVar = (*it).second;
			CMail::tagMail *ptgMail  = (CMail::tagMail*)(ptgTimerVar->pvar);
			SAFE_DELETE(ptgMail);
			SAFE_DELETE(ptgTimerVar);
			it = m_mapTimer.erase(it);
		}
		Cancel((*tit).second,(const void**)&pVar);

	}	
}

//注册定时器
void	CMailSystem::RegisterTimer(const CGUID &PlayerGuid,const CGUID &MailGuid,long lFuture)		
{
	void* pVar = new tagTimerVar;
	tagTimerVar *ptgTimerVar = (tagTimerVar*)pVar;

	((tagTimerVar*)pVar)->TimerType = ChildObjType_Mail;
	CMail::tagMail *ptgMail = new CMail::tagMail;
	ptgMail->PlayerGuid = PlayerGuid;
	ptgMail->MailGuid = MailGuid;
	((tagTimerVar*)pVar)->pvar = ptgMail;

	long lTimerID = Schedule(this, pVar, lFuture + timeGetTime());	
	m_mapTimer[lTimerID] = ptgTimerVar;
}


//注册系统邮件定时器
bool	CMailSystem::RegisterSysMailTimer(DWORD dwID,CMail *pMail)													
{
	if (pMail->GetBaseParm()->ltgTimer.size()>0)//--ltgTimer 定时器
	{
		list<tagTimer*>::iterator it=pMail->GetBaseParm()->ltgTimer.begin();

		long lIntev = (*it)->lIntev;
		long lCount = (*it)->lCount;
		time_t tCurrentTime = time(NULL);
		time_t tSendTime = (time_t)((*it)->lTimer);
		time_t lFuture = (tSendTime - tCurrentTime)*1000;

		if (lFuture>0)
		{	
			stTimerParam *pSysTimer = new stTimerParam;
			if (pSysTimer)
			{
				pSysTimer->lCalledCount = 0;
				pSysTimer->nStartTime =  timeGetTime();
				pSysTimer->lFuture = lFuture;
				pSysTimer->lCount	= lCount;

				void *pVar = new tagTimerVar;
				tagTimerVar *pTimvar = (tagTimerVar*)pVar;

				((tagTimerVar*)pTimvar)->TimerType = ChildObjType_SysMail;//--系统邮件;
				CMail::tagMail *ptgMail = new  CMail::tagMail;
				ptgMail->lMailID = dwID;
				((tagTimerVar*)pTimvar)->pvar = ptgMail;					
				long lTimerID =	Schedule(this,pTimvar,(ulong)(pSysTimer->nStartTime+pSysTimer->lFuture),lIntev);
				m_mapTimerParam[dwID] = pSysTimer;	
				m_mapTimer[lTimerID] = pTimvar;
				return true;
			}			
		}
	}
	return false;
}

//注消系统邮件定时器
void	CMailSystem::UnRegisterSysMailTimer(long lTimerID)															
{	
	void* pVar = NULL;	
	Cancel(lTimerID,(const void**)&pVar);
}

//注消定时器
void	CMailSystem::UnRegisterTimer(long lTimerID)															
{	
	void* pVar = NULL;	
	Cancel(lTimerID,(const void**)&pVar);
}

//初始化系统邮件参数
void	CMailSystem::InitMailParam(tagMailParam *pParam,tagMailParam *pPamramList)
{
	pParam->bRead = pPamramList->bRead;
	pParam->lType = pPamramList->lType;

	pParam->strExText = pPamramList->strExText;
	pParam->strText  = pPamramList->strText;
	pParam->tRemainTime = pPamramList->tRemainTime;

	pParam->strWriter = pPamramList->strWriter;
	pParam->strSubject = pPamramList->strSubject;
	pParam->strReceiverList = pPamramList->strReceiverList;
	pParam->lGold = pPamramList->lGold;
	pParam->strReceiver = pPamramList->strReceiver;
	pParam->WriterGuid = pPamramList->WriterGuid;
	pParam->bReject = pPamramList->bReject;


	for (vector<CGoods*>::iterator it=pPamramList->vMailGoods.begin(); it!=pPamramList->vMailGoods.end(); ++it)
	{
		pParam->vMailGoods.push_back((*it));
	}
	pPamramList->vMailGoods.clear();

	for (list<listGoods*>::iterator it=pPamramList->Goodslist.begin();it!=pPamramList->Goodslist.end();++it)
	{
		listGoods *ptgCGoods = new listGoods;
		ptgCGoods->goodsguid = (*it)->goodsguid;
		ptgCGoods->lNum	  = (*it)->lNum;
		ptgCGoods->lContainerID = (*it)->lContainerID;
		ptgCGoods->lPos		 = (*it)->lPos;
		pParam->Goodslist.push_back(ptgCGoods);
	}

}
//初始化系统邮件
bool	CMailSystem::InitSysMail()
{
	ClearSysMail();
	map<DWORD,tagMailParam*> mapMail = MailList::GetSysMail();
	map<DWORD,tagMailParam*>::iterator it=mapMail.begin();
	for (;it!=mapMail.end();++it)
	{			
		tagMailParam *ptgMailParam = new tagMailParam;
		InitSysMailParam(ptgMailParam,(*it).second);
		ptgMailParam->lType = it->first;
		CMail *pMail = new CMail(ptgMailParam);
		AddSysMail(pMail,it->first);	
	}
    return true;
}
//添加系统邮件
void	CMailSystem::AddSysMail(CMail *pMail,DWORD dwID)
{
	if (pMail)
	{
		map<DWORD,CMail*>::iterator it=m_mapSysMail.find(dwID);
		if (it==m_mapSysMail.end())
		{
			if((RegisterSysMailTimer(dwID,pMail)))
			{
				m_mapSysMail[dwID]=pMail;	
				return;
			}			
		}		
		SAFE_DELETE(pMail);
	}		
}

//发送系统邮件
void	CMailSystem::SendSysMail(long lSysID)													
{	
	CMessage msg(MSG_W2S_MAIL_SEND_SYS);
	msg.Add(lSysID);
	msg.SendToAllGS();
}

//初始化系统邮件参数
void	CMailSystem::InitSysMailParam(tagMailParam *pParam,tagMailParam *pPamramList)
{
	pParam->bRead = pPamramList->bRead;
	pParam->lType = pPamramList->lType;

	pParam->strExText = pPamramList->strExText;
	pParam->strText  = pPamramList->strText;
	pParam->tRemainTime = pPamramList->tRemainTime;

	pParam->strWriter = pPamramList->strWriter;
	pParam->strSubject = pPamramList->strSubject;
	pParam->strReceiverList = pPamramList->strReceiverList;

	for (list<tagMailEvent*>::iterator it=pPamramList->ltgMailEvent.begin(); it!=pPamramList->ltgMailEvent.end(); ++it)
	{
		tagMailEvent *ptgMailEvent = new tagMailEvent;

		ptgMailEvent->lBeginTime = (*it)->lBeginTime;
		ptgMailEvent->lEndTime = (*it)->lEndTime;
		pParam->ltgMailEvent.push_back(ptgMailEvent);
	}

	for (list<tagTimer*>::iterator it=pPamramList->ltgTimer.begin(); it!=pPamramList->ltgTimer.end(); ++it)
	{
		tagTimer *ptgTimer = new tagTimer;
		ptgTimer->lTimer = (*it)->lTimer;
		ptgTimer->lIntev = (*it)->lIntev;
		ptgTimer->lCount = (*it)->lCount;
		pParam->ltgTimer.push_back(ptgTimer);
	}

	for (list<tagMailCondition*>::iterator it=pPamramList->lMailCondition.begin(); it!=pPamramList->lMailCondition.end(); ++it)
	{
		tagMailCondition *ptgMailCondition = new tagMailCondition;
		ptgMailCondition->strName = (*it)->strName;
		ptgMailCondition->lValue = (*it)->lValue;
		pParam->lMailCondition.push_back(ptgMailCondition);
	}

}

//通过id得到系统邮件
CMail*  CMailSystem::GetSysMail(long lID)
{
	map<DWORD,CMail*>::iterator it=m_mapSysMail.find(lID);
	if (it!=m_mapSysMail.end())
	{
		return (*it).second;
	}
	return NULL;
}
//清除系统邮件
void	CMailSystem::ClearSysMail()														
{
	map<DWORD,CMail*>::iterator it=m_mapSysMail.begin();
	for (;it!=m_mapSysMail.end();++it)
	{
		SAFE_DELETE((*it).second);
	}
	m_mapSysMail.clear();
}


//玩家下线后删除ws上的邮件
void	CMailSystem::DeleteMailOnPlayerExit(CPlayer *pPlayer)
{
	if (pPlayer)
	{
		UnRegisterCheckTime(pPlayer->GetExID());

		//释放玩家身上的未被取出的邮件		
		ReleasePlayerMail(pPlayer);

		list<CGUID> &lNewMailList = pPlayer->GetNewMailList();
		if (lNewMailList.size()>0)
		{
			list<CGUID>::iterator it = lNewMailList.begin();

			for (;it!=lNewMailList.end();)
			{
				map<CGUID,CMail*>::iterator mit =m_mapTempMail.find((*it));
					if (mit!=m_mapTempMail.end())
					{
						SAFE_DELETE((*mit).second);
						m_mapTempMail.erase(mit);
					}
					it = lNewMailList.erase(it);
			}
		}	
		lNewMailList.clear();

		map<CGUID,CMail*>::iterator mit =m_mapTempMail.begin();
		for (;mit!=m_mapTempMail.end();)
		{
			if (pPlayer->GetExID()==(*mit).second->GetReceiverExID())
			{
				SAFE_DELETE((*mit).second);
				mit = m_mapTempMail.erase(mit);
			}
			else
			{
				++mit;
			}

		}

		list<CGUID> &lRMailList = pPlayer->GetReceiveMailList();
		if (lRMailList.size()>0)
		{
			list<CGUID>::iterator it = lRMailList.begin();

			for (;it!=lRMailList.end();)
			{
				map<CGUID,CMail*>::iterator mit =m_mapMail.find((*it));
					if (mit!=m_mapMail.end())
					{
						SAFE_DELETE((*mit).second);
						m_mapMail.erase(mit);
					}
				 it = lRMailList.erase(it);
			}
		}
		lRMailList.clear();
	
	}
	
}

//邮件保存成功
void	CMailSystem::SuccessSaveMail(CGUID &MailGuid)										
{
	map<CGUID,CMail*>::iterator mit = m_mapTempMail.find(MailGuid);
	if (mit!=m_mapTempMail.end())
	{
		CMail *pMail = (*mit).second; 
		pMail->SetSave(MAIL_SAVE_SUCCESS_SAVE);

#ifdef 	_OPERATE_MAIL	
		char szOutPutStr[1024];
		char szMailGuid[256];
		pMail->GetExID().tostring(szMailGuid); 			
		sprintf(szOutPutStr, "成功保存邮件到DB:玩家:%s发信给玩家:%s,主题:%s,邮件GUID:%s到DB,金钱:%d,物品数量:%d",pMail->GetWriter().c_str(),pMail->GetReceiver().c_str(),pMail->GetSubject().c_str(),szMailGuid,pMail->GetGold(),pMail->GetMGoodsContainer().size());
		CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);

		for (vector<CGoods*>::iterator it=pMail->GetMGoodsContainer().begin(); it!=pMail->GetMGoodsContainer().end();++it)
		{
			char szGoodsGuid[256];
			char szOutStr[256];
			if (*it)
			{
				(*it)->GetExID().tostring(szGoodsGuid);
				sprintf(szOutStr,"邮件%s包含的物品:%s,物品名:%s,数量:%d",szMailGuid,szGoodsGuid,(*it)->GetName(),(*it)->GetAmount());
				CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutStr);
			}
		}	
#endif

		CPlayer *pPlayer = GetGame().GetMapPlayer(pMail->GetReceiverExID());
		
		
		if (pPlayer)
		{
			list<CGUID> lNewMailList = pPlayer->GetNewMailList();
			list<CGUID>::iterator it = find(lNewMailList.begin(),lNewMailList.end(),pMail->GetExID());
			if (it==lNewMailList.end())
			{
				//发信时离线，返回结果时在线的处理，删除临时邮件列表
				SAFE_DELETE(pMail);
				m_mapTempMail.erase(mit);
			}			
		}		
		else
		{
			//离线
			SAFE_DELETE(pMail);
			m_mapTempMail.erase(mit);
		}
	}
}

////没有找到收信人
void	CMailSystem::FindNoReceiver(CGUID &MailGuid)										
{
	map<CGUID,CMail*>::iterator mit = m_mapTempMail.find(MailGuid);
	if (mit!=m_mapTempMail.end())
	{
		CMail *pMail = (*mit).second;		

		if (pMail && pMail->GetBaseParm())
		{	
			
			pMail->SetSave(MAIL_SAVE_SUCCESS_SAVE);
			
			//转发			
			if (pMail->GetType() == PAYGOLD_MAIL)
			{
				pMail->SetGold(0);
			}
			time_t t_CurrentTime;
			char   strSysMsg[128];
			char   strSuject[128];
			char   strText[1280];
			t_CurrentTime = time(NULL);

			wsprintf(strSysMsg,AppFrame::GetText("GS_SYSMAIL_THIRD"));
			wsprintf(strSuject,AppFrame::GetText("GS_SYSMAIL_EIGHTH"),pMail->GetSubject().c_str());
			wsprintf(strText,AppFrame::GetText("GS_SYSMAIL_NINE"),pMail->GetText().c_str());

			time_t tRemainTime =  t_CurrentTime + (long)GlobalSetup::GetSetup()->fMaxRemainTime[SYSTEM_MAIL]*86400 ;
			pMail->SetType(SYSTEM_MAIL);	
			pMail->SetReject(0);
			pMail->GetReceiverList().clear();
			pMail->SetReceiverList(pMail->GetWriter().c_str());
			pMail->SetReceiver(pMail->GetWriter());
			pMail->SetReceiverExID(pMail->GetWriterExID());
			pMail->SetRemainTime(tRemainTime);
			pMail->SetWriter(strSysMsg);	
			pMail->SetWriterID(NULL_GUID);
			pMail->SetWriteTime(t_CurrentTime);
			pMail->SetSubject(strSuject);
			pMail->SetText(strText);

#ifdef 	_OPERATE_MAIL	
			char szOutPutStr[1024];
			char szMailGuid[256];
			pMail->GetExID().tostring(szMailGuid); 			
			sprintf(szOutPutStr, "没有找到收信人退信:玩家:%s发信给玩家:%s,主题:%s,邮件GUID:%s到DB,金钱:%d,物品数量:%d",pMail->GetWriter().c_str(),pMail->GetReceiver().c_str(),pMail->GetSubject().c_str(),szMailGuid,pMail->GetGold(),pMail->GetMGoodsContainer().size());
			CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);

			for (vector<CGoods*>::iterator it=pMail->GetMGoodsContainer().begin(); it!=pMail->GetMGoodsContainer().end();++it)
			{
				char szGoodsGuid[256];
				char szOutStr[256];
				if (*it)
				{
					(*it)->GetExID().tostring(szGoodsGuid);
					sprintf(szOutStr,"邮件%s包含的物品:%s,物品名:%s,数量:%d",szMailGuid,szGoodsGuid,(*it)->GetName(),(*it)->GetAmount());
					CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutStr);
				}
			}	
#endif

			CPlayer *pPlayer = GetGame().GetMapPlayer(pMail->GetReceiverExID());
			
			if(pPlayer)
			{
				//在线
				pPlayer->AddMailList(MailGuid);
			}
			else
			{
				//离线
				SAFE_DELETE(pMail);
				m_mapTempMail.erase(mit);
			}		
		}
	}
}
//邮件发送失败
void	CMailSystem::FailedSaveMail(CGUID &MailGuid)										
{
	map<CGUID,CMail*>::iterator it = m_mapTempMail.find(MailGuid);
	if (it!=m_mapTempMail.end())
	{
		CMail *pMail = (*it).second;		

		if (pMail && pMail->GetBaseParm())
		{			
			if (( pMail->GetMGoodsContainer().size()>0 || pMail->GetGold()>0) && pMail->GetType()!=SYSTEM_MAIL && pMail->GetType()<SENDGOLD_MAIL)
			{
				//转发			
				if (pMail->GetType() == PAYGOLD_MAIL)
				{
					pMail->SetGold(0);
				}

				time_t t_CurrentTime;
				char   strSysMsg[128];
				char   strSuject[128];
				char   strText[1280];
				t_CurrentTime = time(NULL);

				wsprintf(strSysMsg,AppFrame::GetText("GS_SYSMAIL_THIRD"));
				wsprintf(strSuject,AppFrame::GetText("GS_SYSMAIL_EIGHTH"),pMail->GetSubject().c_str());
				wsprintf(strText,AppFrame::GetText("GS_SYSMAIL_TEN"),pMail->GetText().c_str());

				

				time_t tRemainTime =  t_CurrentTime + (long)GlobalSetup::GetSetup()->fMaxRemainTime[SYSTEM_MAIL]*86400 ;
				pMail->SetType(SYSTEM_MAIL);	
				pMail->SetReject(0);
				pMail->GetReceiverList().clear();
				pMail->SetReceiverList(pMail->GetWriter().c_str());
				pMail->SetReceiver(pMail->GetWriter());
				pMail->SetReceiverExID(pMail->GetWriterExID());
				pMail->SetSubject(strSuject);
				pMail->SetRemainTime(tRemainTime);
				pMail->SetWriter(strSysMsg);									
				pMail->SetWriteTime(t_CurrentTime);
				pMail->SetText(strText);
#ifdef 	_OPERATE_MAIL	
				char szOutPutStr[1024];
				char szMailGuid[256];
				pMail->GetExID().tostring(szMailGuid); 			
				sprintf(szOutPutStr, "退信发送保存邮件到DB:玩家:%s发信给玩家:%s,主题:%s,邮件GUID:%s到DB,金钱:%d,物品数量:%d",pMail->GetWriter().c_str(),pMail->GetReceiver().c_str(),pMail->GetSubject().c_str(),szMailGuid,pMail->GetGold(),pMail->GetMGoodsContainer().size());
				CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutPutStr);

				for (vector<CGoods*>::iterator it=pMail->GetMGoodsContainer().begin(); it!=pMail->GetMGoodsContainer().end();++it)
				{
					char szGoodsGuid[256];
					char szOutStr[256];
					if (*it)
					{
						(*it)->GetExID().tostring(szGoodsGuid);
						sprintf(szOutStr,"邮件%s包含的物品:%s,物品名:%s,数量:%d",szMailGuid,szGoodsGuid,(*it)->GetName(),(*it)->GetAmount());
						CMessage::AsyWriteFile(GetGame().GetOpeateMailFileName(),szOutStr);
					}
				}	
#endif
				GetGame().GetEntityManager()->CreateInsertMailSession(pMail);
			}			
		}
	}
}
//保存从DB初始化获取的邮件
void	CMailSystem::AddPlayerMail(CGUID &PlayetGuid,list<CMail*> &lMail)						
{
	map<CGUID,list<CMail*>>::iterator it = m_mapPlayerMail.find(PlayetGuid);
	if (it==m_mapPlayerMail.end())
	{
		m_mapPlayerMail[PlayetGuid] = lMail;
	}	
	else
	{
		list<CMail*>::iterator it = lMail.begin();
		for (;it!=lMail.end();++it)
		{
			SAFE_DELETE(*it);
		}
		lMail.clear();
	}
}
//返回保存邮件结果
void	CMailSystem::SendSaveMailRet(CGUID &MailGuid,long lRet)								
{
	switch(lRet)
	{
	case 1:
		{
			//未找到收件人
			FindNoReceiver(MailGuid);
		}
		break;
	case 2:
		{
			//添加邮件成功
			SuccessSaveMail(MailGuid);
		}
		break;
	}
}


void CMailSystem::OutMailInfo(VOID)
{
//	char szTmp[1024] = {0};

	LONG lMailNum = (LONG)m_mapMail.size();
	LONG lTmpMailNum = (LONG)m_mapTempMail.size();
	LONG lSysMailNum = (LONG)m_mapSysMail.size();

	LONG lPlayerMailNum = 0;
	map<CGUID,list<CMail*>>::iterator ite = m_mapPlayerMail.begin();
	for (; m_mapPlayerMail.end() != ite; ++ite)
		lPlayerMailNum += (LONG)ite->second.size();

// 	sprintf(szTmp, "NewMailCount = %d\r\nMailNum = %d\r\nTmpMailNum = %d\r\nSysMailNum = %d\r\nPlayerMailNum = %d\r\n\r\n",g_lMailCount,  lMailNum, lTmpMailNum, lSysMailNum, lPlayerMailNum);
// 	PutStringToFile("MemAllocInfo", szTmp);

    Log4c::Trace(ROOT_MODULE,"NewMailCount = %d\r\nMailNum = %d\r\nTmpMailNum = %d\r\nSysMailNum = %d\r\nPlayerMailNum = %d\r\n\r\n",g_lMailCount,  lMailNum, lTmpMailNum, lSysMailNum, lPlayerMailNum);
}




