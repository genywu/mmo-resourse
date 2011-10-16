///////////////////////////////////////////////////////////////////////////////
///  MailManager.cpp
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\GameServer\AppServer\Mail
///
///  CREATED: 09/12/2008  by 陈先进
///
///  PURPOSE:   邮件管理类
///
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "MailManager.h"
#include "../goods/CGoods.h"
#include "../Player.h"
#include "../public/WordsFilter.h"
#include "../setup/MailList.h"
#include "../setup\IncrementShopList.h"
#include "..\Goods\CGoodsFactory.h"

extern VOID CancelIncrementTrading( CPlayer* pPlayer );
CMailManager::CMailManager()
{	
	m_mapMail.clear();	
}

CMailManager::~CMailManager()
{
	for (map<CGUID,CMail*>::iterator it=m_mapMail.begin(); it!=m_mapMail.end(); ++it)
	{
		it->second->DeleteMailGoods();
		MP_DELETE(it->second);
	}
	m_mapMail.clear();

	for (map<long,CMail*>::iterator it=m_mapSysMail.begin(); it!=m_mapSysMail.end(); ++it)
	{
		MP_DELETE(it->second);
	}
	m_mapSysMail.clear();
}


//获取元宝数量
long CMailManager::GetYuanBaoNum(long lIndex,long lNum,CPlayer *pPlayer)									
{
	CIncrementShopList::MAP_INCGOODS* pIncGoodsList = CIncrementShopList::GetIncGoodsList();
	CIncrementShopList::ITR_INCGOODS itIncGoodsList = pIncGoodsList->find( CIncrementShopList::TM_YUANBAO );
	CIncrementShopList::MAP_INCSHOP mapIncGoodsList = itIncGoodsList->second;
	CIncrementShopList::ITR_INCSHOP it;
	if( !pIncGoodsList )
	{
		char *strRes=CStringReading::LoadString(22,17);
		if(strRes && pPlayer)
			pPlayer->SendNotifyMessage(strRes);
		CancelIncrementTrading( pPlayer );
		return 0;
	}
	else
	{
		it = mapIncGoodsList.find(lIndex);
		if( it == mapIncGoodsList.end() || !(&it->second) )
		{
			char *strRes=CStringReading::LoadString(22,18);
			if(strRes && pPlayer)
				pPlayer->SendNotifyMessage(strRes);
			return 0;
		}
	}

	CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( it->first );
	if( !pProperties )
	{
		char *strRes=CStringReading::LoadString(22,19);
		if(strRes && pPlayer)
			pPlayer->SendNotifyMessage(strRes);
		return 0;
	}	
	lNum = lNum/(it->second.dwGoodsNum);
	return it->second.dwPrice*lNum;
}

//赠送类邮件
void CMailManager::SendPMail(CGUID &MailGuid,CPlayer *pPlayer,long lRet)
{
	
	map<CGUID,CMail*>::iterator it=m_mapMail.find(MailGuid);
	if (it!=m_mapMail.end())
	{
		CMail *pMail = (*it).second;	
		if (pMail && pPlayer)
		{
			BYTE bType = pMail->GetType();
			long lSilver = pPlayer->GetSilver() - CGlobeSetup::GetSetup()->lPostage[bType];
			if (lSilver>=0)
			{
				pPlayer->SetSilver(lSilver);
			}
		}		

		SendPresentMailToWS(pMail,lRet);
		pMail->DeleteMailGoods();
		MP_DELETE(pMail);
		m_mapMail.erase(it);
	}
}
//发送赠送邮件到WS
void	CMailManager::SendPresentMailToWS(CMail *pMail,long lRet)						
{
	if (pMail)
	{
		CMessage msg(MSG_S2W_MAIL_PRESENT_RECEIVER);
		msg.Add(pMail->GetExID());
		msg.Add(lRet);

		long lCGoodNum = pMail->GetMGoodsContainer().size();
		msg.Add(lCGoodNum);
		int nPos = 0;
		vector<BYTE> pBA;
		for (vector<CGoods*>::iterator it=pMail->GetMGoodsContainer().begin(); it!=pMail->GetMGoodsContainer().end(); ++it)
		{				
			(*it)->Serialize(&pBA);
			msg.Add((LONG)pBA.size());
			msg.AddEx(&pBA[0], pBA.size());			
		}
		msg.Send();

	}
}
//通过邮件购买增值道具
void	CMailManager::BuyItemByMail(CMail *pMail,CPlayer *pPlayer)
{
	long lTotalAmount = 0;
	long lYuanBao = 0;
	list<CGUID> lMailGuid;
	lMailGuid.push_back(pMail->GetExID());
	list<tagSGoods*> lSGoods = pMail->GetSGood();

	for (list<tagSGoods*>::iterator it = lSGoods.begin(); it!=lSGoods.end(); ++it)
	{			
		lYuanBao = GetYuanBaoNum((*it)->lIndex,(*it)->lNum,pPlayer);
		if (lYuanBao!=0)
		{	
			lTotalAmount += lYuanBao;
		}
		else
		{
			SendMailFailedToClient(pMail->GetExID(),MAIL_SENDING_FAILED_INVALID_GOODS);
			DeleteMailToWS(lMailGuid,pPlayer);
			pMail->DeleteMailGoods();
			MP_DELETE(pMail);
			return;
		}
	}

	m_mapMail[pMail->GetExID()] = pMail;


	CMessage msg(MSG_S2B_ACCOUNT_LOCK);
	msg.Add((BYTE)eICT_MAIL_BUY_ITEM); //消费类型
	msg.Add(pPlayer->GetExID());	//player guid
	msg.Add(pPlayer->GetAccount());	//player cdkey
	msg.Add(pMail->GetExID());				//attach guid

	lSGoods = pMail->GetSGood();
	msg.Add((DWORD)lSGoods.size());
	;
	for (list<tagSGoods*>::iterator it = lSGoods.begin(); it!=lSGoods.end(); ++it)
	{				
		msg.Add((*it)->lIndex);			//goods id
		msg.Add((*it)->lNum);			//goods num					
	}
	msg.Add(lTotalAmount);			//money sum
	msg.SendToBS();		
}

//加信
bool CMailManager::SetMapMail(CMail *pMail)
{
   	time_t t_CurrentTime = time(NULL);
	map<CGUID,CMail*>::iterator it=m_mapMail.find(pMail->GetExID());
	if (it!=m_mapMail.end())
	{
		CMail *pMail = (*it).second;
		it=m_mapMail.erase(it);	
		pMail->DeleteMailGoods();
		MP_DELETE(pMail);
	}

	if ((pMail->GetType()==SYSTEM_MAIL || pMail->GetType()>=MAX_MAIL) && !stricmp(pMail->GetWriter().c_str(),CStringReading::LoadString(IDS_WGS_SYSMAIL,STR_WS_SYSMAIL_THIRD)))
	{
		char strSubject[256];
		_snprintf(strSubject, 256, CStringReading::LoadString(IDS_WGS_SYSMAIL,STR_WS_SYSMAIL_FIFTH),pMail->GetSubject().c_str());
		pMail->SetSubject(strSubject);
	}
	if (pMail->GetType()>=MAX_MAIL)
	{
		long lSysMailID = pMail->GetType();
		string strText;
		map<long,CMail*>::iterator mit = m_mapSysMail.find(lSysMailID);
		if (mit!=m_mapSysMail.end())
		{
			strText = (*mit).second->GetText();
			pMail->SetText(strText.c_str());
			pMail->SetType(SYSTEM_MAIL);
		}
		else
		{			
			m_mapMail[pMail->GetExID()] = pMail;
			return false;
		}
	}

	m_mapMail[pMail->GetExID()] = pMail;
	return true;	
}
bool CMailManager::AddMail(CMail *pMail,CPlayer *pPlayer)
{
	CGUID guid;
	CGUID::CreateGUID(guid);
	pMail->SetExID(guid);

	if (SendMailToWS(pMail))
	{
		MP_DELETE(pMail);
		return true;
	}
	MP_DELETE(pMail);
	return false;

}
//检查收信人是否是自己
bool CMailManager::CheckReceiver(CMail *pMail,CPlayer *pPlayer)
{
	if (pPlayer && pMail->GetType()!=SYSTEM_MAIL && pMail->GetType()<MAX_MAIL)
	{
		list<string>::iterator it = pMail->GetReceiverList().begin();
		for (;it!=pMail->GetReceiverList().end();++it)
		{
			if (!stricmp((*it).c_str(),pPlayer->GetName()) || !CheckInvalidCharacter((*it)))
			{
				return false;
			}
		}
	}
	return true;
}

//检查多收信中是否包含物品金钱
bool CMailManager::CheckManyReceiver(CMail *pMail)
{

	if ((pMail->GetType()!=SYSTEM_MAIL || pMail->GetType()<MAX_MAIL) && pMail->GetReceiverList().size()>1)
	{
		if (pMail->GetGold()>0 || pMail->GetSGood().size()>0 || pMail->GetCGood().size()>0)
		{
			return false;
		}
		return true;
	}
	return true;
}
//检查和扣除邮资
bool CMailManager::CheckPostage(CMail *pMail, CPlayer *pPlayer)
{
	if (pMail && pPlayer)
	{
		BYTE bType = pMail->GetType();
		long lPostage =  CGlobeSetup::GetSetup()->lPostage[bType]*pMail->GetReceiverList().size();
		long lCurrentMoney = pPlayer->GetSilver();

		if (lCurrentMoney<lPostage)
		{
			//邮资不足			
			return false;
		}		
	}
	return true;
}
//检查金钱
bool CMailManager::CheckMoney(CMail *pMail, CPlayer *pPlayer)
{
	if (pMail && pPlayer)
	{
		BYTE bType = pMail->GetType();		
		long lCurrentMoney = pPlayer->GetMoney();

		//非付费邮件
		if (pMail->GetGold()>CGlobeSetup::GetSetup()->lMaxMoney || pMail->GetGold()>lCurrentMoney)
		{
			//附件金钱超出限制
			return false;
		}			
	}
	return true;	
}


//检查邮件
eMAILRESULT CMailManager::CheckMailCondition(CMail *pMail, CPlayer *pPlayer)
{

	if (pMail->GetSubject().size()==0 || pMail->GetReceiverList().size()==0)
	{
		//无主题或收信人
		return MAIL_SENDING_FALLED_INVALID_SUBJECTORRECEIVER;
	}
	//
	if (!CheckInvalidCharacter(pMail->GetSubject()))
	{
		//邮件主题文字有非法字符		
		return MAIL_SENDING_FAILED_INVALID_TEXT;
	}

	if (!CheckInvalidCharacter(pMail->GetExText()))
	{
		//邮件附加内容文字有非法字符	
		return MAIL_SENDING_FAILED_INVALID_EXTEXT;
	}
	if (!CheckManyReceiver(pMail))
	{
		//检查群发邮件中是否有物或金钱
		return MAIL_SENDING_FAILED_INVALID_MANYRECEIVER;
	}
	//检查邮资(付款回复类邮件不检查邮资)
	if(!CheckPostage(pMail,pPlayer) && pMail->GetType()!=SENDGOLD_MAIL)
	{
		return MAIL_SENDING_FAILED_INVALID_POSTAGE;

	}
	//收信人不能是发信人
	if (!CheckReceiver(pMail,pPlayer))
	{
		return MAIL_SENDING_FAILED_INVALID_SELF;
	}
	
	//检查金钱
	if (pMail->GetType()!=PAYGOLD_MAIL && !CheckMoney(pMail,pPlayer) )
	{
		return MAIL_SENDING_FAILED_INVALID_GOLD;
	}
	//检查物品
	if(!pMail->CheckGoodsContainer(pPlayer) || !pMail->AddGoodsToContainer(pPlayer))
	{
		return MAIL_SENDING_FAILED_INVALID_GOODS;
	}
	//检查邮件类型是否一致
	if ((pMail->GetType()==COMMON_MAIL && (pMail->GetGold()>0 || pMail->GetCGood().size()>0 || pMail->GetSGood().size()>0)) 	
															  || pMail->GetType()<SYSTEM_MAIL
															  || (pMail->GetType()==REQUEST_MAIL || pMail->GetType()==PRESENT_MAIL)
										&& (pMail->GetCGood().size()>0 || pMail->GetSGood().size()==0 || pMail->GetGold()>0))
	{
		return MAIL_SENDING_FAILED_INVALID_TYPE;
	}

	return MAIL_SENDING_SUCCESS;
}
//邮件发送失败
void CMailManager::SendMailFailedToClient(CGUID guid,long lValue)
{
	CMessage msg(MSG_S2C_MAIL_SEND_FAILED);
	msg.Add(lValue);
	msg.SendToPlayer(guid);
}



//检查群发类邮件国家条件
bool	CMailManager::CheckMailCountryToEverybody(CMail *pMail,CPlayer *pPlayer)
{
	long lCountry = 0;
	list<tagMailCondition*> lMailCondition = pMail->GetMailCondition();
	list<tagMailCondition*>::iterator it=lMailCondition.begin();
	for (;it!=lMailCondition.end();++it)
	{
		if ((*it)->strName=="Country")
		{
			++lCountry;
			if ((*it)->lValue==pPlayer->GetCountry())	
			{
				return true;
			}
		}
	}

	if (lCountry>0)
	{
		return false;
	}
	return true;
}


//检查群发类邮件条件
bool	CMailManager::CheckMailToEverybody(CMail *pMail,CPlayer *pPlayer)
{
	list<tagMailCondition*> lMailCondition = pMail->GetMailCondition();
	list<tagMailCondition*>::iterator it=lMailCondition.begin();
	for (;it!=lMailCondition.end();++it)
	{
		if ((*it)->strName=="Level")
		{
			if ((*it)->lValue>pPlayer->GetLevel())			
			{
				return false;
			}
		}

		else if ((*it)->strName=="Sex")
		{
			if ((*it)->lValue!=pPlayer->GetSex())		
			{
				return false;
			}
		}
	}	
	return true;
}

//发送系统邮件到收信人
void	CMailManager::SendSysMailMsgToReceiver(long lSysID)
{
	CMail *pMail = GetSysMail(lSysID);
	long lBeginTime = 0;
	long lEndTime = 0;

	if (pMail)
	{
		pMail->GetSysSendTime(lBeginTime,lEndTime);
		GetGame()->SendSysMailMsgToOnlinePlayer(pMail);	
	}
}
//发送系统邮件消息
void	CMailManager::SendSysMailMsg(CMail *pMail,CPlayer *pPlayer)
{
	if (pPlayer && CheckMailCountryToEverybody(pMail,pPlayer) && CheckMailToEverybody(pMail,pPlayer))
	{
		CMessage msg(MSG_S2C_MAIL_SYSMAILMSG);
		msg.Add((char*)pMail->GetSubject().c_str());
		msg.SendToPlayer(pPlayer->GetExID());
	}

}
//发送系统邮件
void	CMailManager::SendSysMail(CMail *pMail,CPlayer *pPlayer,long lSysID,long lEndTime)	
{
	if (pPlayer && CheckMailCountryToEverybody(pMail,pPlayer) && CheckMailToEverybody(pMail,pPlayer))
	{
		pMail->GetReceiverList().clear();
		pMail->SetReceiverList(pPlayer->GetName());
		pMail->AddGoodsToContainerBySys();
		pMail->SetReject(0);
		SendMailToWS(pMail);

		pPlayer->InsertSysMail(lSysID,lEndTime);
	}		
}
//通过脚本发送系统邮件
void	CMailManager::SendSysMailByScript(CMail *pMail)
{
	if (pMail)
	{
		list<string>::iterator it = pMail->GetReceiverList().begin();
		CPlayer *pPlayer=GetGame()->FindPlayer((*it).c_str());
		pMail->AddGoodsToContainerBySys();
		pMail->SetReject(0);
		SendMailToWS(pMail);
		pMail->DeleteMailGoods();
		MP_DELETE(pMail);

	}	
}
void   CMailManager::SendMailToReceiver(CGUID &guid,long lRet)
{
	CMessage msg(MSG_S2W_MAIL_SEND_RECEIVER);
	msg.Add(guid);
	msg.Add(lRet);
	msg.Send();
}

long	CMailManager::ReMoveGoldAndGoods(CMail *pMail,CPlayer *pPlayer)
{
	if (pPlayer)
	{
		BYTE bType = pMail->GetType();
		long lGold = 0;
		long lSilver = 0;
		char strGoodsInfo[512]="";

		if(!pMail->CheckGoodsContainer(pPlayer))
		{
			SendMailFailedToClient(pMail->GetExID(),MAIL_SENDING_FAILED_INVALID_GOODS);
			return 0;
		}	

		if (pMail->GetType()!=SENDGOLD_MAIL)
		{
			lSilver = pPlayer->GetSilver() - CGlobeSetup::GetSetup()->lPostage[bType];
			if (lSilver<0)
				return 0;
		}


		if (pMail->GetType()!=PAYGOLD_MAIL)
		{
			lGold = pPlayer->GetMoney() - pMail->GetGold();
			if (lGold<0)
				return 0;			
		}	

		for (list<tagCGoods*>::iterator it=pMail->GetCGood().begin(); it!=pMail->GetCGood().end(); ++it)
		{
			CGoods *pGoods =pPlayer->FindGoodByGuid((*it)->goodsguid);
			if (pGoods)
			{
				char strNameNum[64];
				sprintf_s(strNameNum, 64, "-%s-%d----",pGoods->GetName(),(*it)->lNum);
				strcat(strGoodsInfo,strNameNum);				
			}			
		}

		if(!pMail->DeleteGoodsInContainer(pPlayer))
		{
			SendMailFailedToClient(pMail->GetExID(),MAIL_SENDING_FAILED_INVALID_GOODS);
			return 0;
		}		

		if (pMail->GetType()!=PAYGOLD_MAIL)
		{
			pPlayer->SetMoney(lGold,1);
		}	

		//付款回复类邮件不扣除邮資
		if (pMail->GetType()!=SENDGOLD_MAIL)
		{
			pPlayer->SetSilver(lSilver);
		} 		

	
		long lGoodsNum = 0;
		for (list<tagCGoods*>::iterator it=pMail->GetCGood().begin(); it!=pMail->GetCGood().end(); ++it)
		{
			lGoodsNum += (*it)->lNum;
		}



		//发信的日志记录
		GetGameLogInterface()->logT601_Send_mail_log(pMail->GetExID(),pMail->GetReceiver().c_str(),pMail->GetWriter().c_str(),pMail->GetType(),pMail->GetGold(),lGoodsNum,pMail->GetSGood().size(),strGoodsInfo);
		return 1;		
	}
	return 0;
	
}
//发信到WSconst CGUID &guid,
bool CMailManager::SendMailToWS(CMail *pMail)
{
	
	CGUID playerguid = NULL_GUID;
	CPlayer *pPlayer = GetGame()->FindPlayer(pMail->GetWriterGuid());


	//  [6/6/2009 chenxianj]
	//////////////////////////////////////////////////////////////////////////
	
	if (NULL!=pMail && NULL==pPlayer && pMail->GetType()!=SYSTEM_MAIL && pMail->GetType()<=SENDGOLD_MAIL)
	{
		//脱机玩家使用外挂发送邮件
		char szOutPutStr[1024];
		char szWriterGuid[256];
		pMail->GetWriterGuid().tostring(szWriterGuid); 			
		_snprintf(szOutPutStr, 1024, "脱机玩家名称:%s,GUID:%s使用外挂发送邮件,类型:%d",pMail->GetWriter().c_str(),szWriterGuid,pMail->GetType());					
		PutStringToFile("Mail",szOutPutStr);
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	if (pPlayer)
	{
		//检查邮件条件	
		long lRet = CheckMailCondition(pMail,pPlayer);
		if (lRet!=0)
		{
			if (pMail->GetType()!=SYSTEM_MAIL && pMail->GetType()<MAX_MAIL)
			{		
				SendMailFailedToClient(pPlayer->GetExID(),lRet);
				return false;
			}			
		}
	}


	if (pMail)
	{	
		CMessage msg(MSG_S2W_MAIL_SEND);		
		msg.Add(pMail->GetWriter().c_str());		//发信人	
		msg.Add(pMail->GetWriterGuid());
		msg.Add(pMail->GetSubject().c_str());		//主题
		msg.Add(pMail->GetText().c_str());			//邮件内容
		msg.Add(pMail->GetRead());					//读取标志符
		msg.Add(pMail->GetType());			//邮件类型
		msg.Add(pMail->GetGold());					//金钱		
		msg.Add(pMail->GetReject());				//退信标志符

		if (pPlayer)
		{
			msg.Add(pPlayer->GetExID());
			playerguid = NULL_GUID;
		} 
		else
		{
			msg.Add(NULL_GUID);
		}		

		//收信人
		long lReceiverNum = pMail->GetReceiverList().size();
		msg.Add(lReceiverNum);
		for (list<string>::iterator it=pMail->GetReceiverList().begin(); it!=pMail->GetReceiverList().end(); ++it)
		{
			msg.Add((*it).c_str());
		}
		//增值物品列表
		long lSGoodNum =  pMail->GetSGood().size();
		msg.Add(lSGoodNum);							
		for (list<tagSGoods*>::iterator it=pMail->GetSGood().begin(); it!=pMail->GetSGood().end(); ++it)
		{
			msg.Add((*it)->lIndex);
			msg.Add((*it)->lNum);
		
		}

		//普通物品列表
		long lCGoodsNum = pMail->GetCGood().size();
		msg.Add(lCGoodsNum);
		list<tagCGoods*>::iterator it=pMail->GetCGood().begin();
		for (;it!=pMail->GetCGood().end();++it)
		{
			msg.Add((*it)->goodsguid);
			msg.Add((*it)->lNum);
			msg.Add((*it)->lContainerID);
			msg.Add((*it)->lPos);
		}
		

		long lCGoodNum = pMail->GetMGoodsContainer().size();
		msg.Add(lCGoodNum);
		int nPos = 0;
		vector<BYTE> pBA;
		for (vector<CGoods*>::iterator it=pMail->GetMGoodsContainer().begin(); it!=pMail->GetMGoodsContainer().end(); ++it)
		{				
			(*it)->Serialize(&pBA);
			msg.Add((LONG)pBA.size());
			msg.AddEx(&pBA[0], pBA.size());			
		}
		msg.Send();
		return true;
	}
	return false;

}


//发送付款收信消息到客户端
void	CMailManager::SendPayMailMsg(const CGUID &guid,BYTE bValue,CPlayer *pPlayer)
{	
	if (pPlayer==NULL)
		return;
	CMessage msg(MSG_S2C_MAIL_AGREED_PAY);
	msg.Add(guid);
	msg.Add(bValue);	
	msg.SendToPlayer(pPlayer->GetExID());
}

//发信
void   CMailManager::SendMailToClient(CGUID guid,CMail *pMail)
{
	CPlayer *pPlayer = GetGame()->FindPlayer(pMail->GetReceiver().c_str());

	if (pPlayer && pMail)
	{
		pPlayer->AddReceiveMailList(pMail->GetExID());
		
		if(SetMapMail(pMail))
		{
			CMessage msg(MSG_S2C_MAIL_SEND);			
			msg.SendToPlayer(pPlayer->GetExID());
			long lGoodsNum = 0;
			for (vector<CGoods*>::iterator it=pMail->GetMGoodsContainer().begin(); it!=pMail->GetMGoodsContainer().end(); ++it)
			{
				lGoodsNum += (*it)->GetAmount();
			}
			GetGameLogInterface()->logT600_Receive_mail_log(pPlayer,pMail->GetExID(),pMail->GetWriter().c_str(),pMail->GetType(),pMail->GetGold(),lGoodsNum,pMail->GetSGood().size(),2);
		}		
	}
	else
	{
		pMail->DeleteMailGoods();
		MP_DELETE(pMail);
	}		
}
//解码邮件信息
void	CMailManager::DecordMailMsgFromByteArray(BYTE* pByte, long& pos,tagMailParam *pMailParam)
{
	char strTemp[2048];
	if (pMailParam==NULL)
		return;
	_GetBufferFromByteArray(pByte,pos,pMailParam->guid);	
	_GetStringFromByteArray(pByte,pos,strTemp);
	pMailParam->strSubject = strTemp;
	_GetStringFromByteArray(pByte,pos,strTemp);
	pMailParam->strWriter = strTemp;
	_GetStringFromByteArray(pByte,pos,strTemp);
	pMailParam->strText = strTemp;
	_GetStringFromByteArray(pByte,pos,strTemp);
	pMailParam->strExText = strTemp;

	pMailParam->lGold = _GetLongFromByteArray(pByte,pos);
	pMailParam->tRemainTime = time_t(_GetLongFromByteArray(pByte,pos));		
	pMailParam->bRead = _GetByteFromByteArray(pByte,pos);
	pMailParam->lType = _GetLongFromByteArray(pByte,pos);
	pMailParam->bReject = _GetByteFromByteArray(pByte,pos);
	_GetStringFromByteArray(pByte,pos,strTemp);
	pMailParam->strReceiver = strTemp;
	_GetBufferFromByteArray(pByte,pos,pMailParam->ReceiverGuid);


	long  lSNum = _GetLongFromByteArray(pByte,pos);
	for (int i=0; i<lSNum; ++i)
	{
		tagSGoods *ptgSGoods = OBJ_CREATE(tagSGoods);
		ptgSGoods->lIndex = _GetLongFromByteArray(pByte,pos);
		ptgSGoods->lNum = _GetLongFromByteArray(pByte,pos);
		pMailParam->lSGoods.push_back(ptgSGoods);
	}
	long lCNum = _GetLongFromByteArray(pByte,pos);

	for (int i=0; i<lCNum; ++i)
	{
		tagCGoods *ptgCGoods = OBJ_CREATE(tagCGoods);
		_GetBufferFromByteArray(pByte,pos,ptgCGoods->goodsguid);
		ptgCGoods->lNum = _GetLongFromByteArray(pByte,pos);
		ptgCGoods->lContainerID = _GetLongFromByteArray(pByte,pos);
		ptgCGoods->lPos = _GetLongFromByteArray(pByte,pos);
		pMailParam->lCGoods.push_back(ptgCGoods);
	}
}
//解码
void	CMailManager::DecordFromByteArray(BYTE* pByte, long& pos,tagMailParam *pMailParam)
{
	char strTemp[2048];
	if (pMailParam==NULL)
		return;
	_GetBufferFromByteArray(pByte,pos,pMailParam->guid);	
	_GetStringFromByteArray(pByte,pos,strTemp);
	pMailParam->strSubject = strTemp;
	_GetStringFromByteArray(pByte,pos,strTemp);
	pMailParam->strWriter = strTemp;
	_GetStringFromByteArray(pByte,pos,strTemp);
	pMailParam->strText = strTemp;
	_GetStringFromByteArray(pByte,pos,strTemp);
	pMailParam->strExText = strTemp;

	pMailParam->lGold = _GetLongFromByteArray(pByte,pos);
	pMailParam->tRemainTime = time_t(_GetDwordFromByteArray(pByte,pos));		
	pMailParam->bRead = _GetByteFromByteArray(pByte,pos);
	pMailParam->lType = _GetLongFromByteArray(pByte,pos);
	pMailParam->bReject = _GetByteFromByteArray(pByte,pos);
	_GetStringFromByteArray(pByte,pos,strTemp);
	pMailParam->strReceiver = strTemp;
	_GetBufferFromByteArray(pByte,pos,pMailParam->ReceiverGuid);


	long  lSNum = _GetLongFromByteArray(pByte,pos);
	for (int i=0; i<lSNum; ++i)
	{
		tagSGoods *ptgSGoods = OBJ_CREATE(tagSGoods);
		ptgSGoods->lIndex = _GetLongFromByteArray(pByte,pos);
		ptgSGoods->lNum = _GetLongFromByteArray(pByte,pos);
		pMailParam->lSGoods.push_back(ptgSGoods);
	}
	long lCNum = _GetLongFromByteArray(pByte,pos);

	for (int i=0; i<lCNum; ++i)
	{
		tagCGoods *ptgCGoods = OBJ_CREATE(tagCGoods);
		_GetBufferFromByteArray(pByte,pos,ptgCGoods->goodsguid);
		ptgCGoods->lNum = _GetLongFromByteArray(pByte,pos);
		ptgCGoods->lContainerID = _GetLongFromByteArray(pByte,pos);
		ptgCGoods->lPos = _GetLongFromByteArray(pByte,pos);
		pMailParam->lCGoods.push_back(ptgCGoods);
	}

	long lGNum = _GetLongFromByteArray(pByte,pos);
	for (int i=0; i<lGNum; ++i)
	{
		CGoods *pGoods = CGoodsFactory::UnserializeGoods(pByte,pos,25);
		if( pGoods )
		{
			CGoodsFactory::UpdateEquipmentAttr(pGoods);
			pMailParam->vMailGoods.push_back(pGoods);
		}
	}

}
//检查增值物品
bool    CMailManager::CheckPresentGoods(CMail *pMail,CPlayer *pPlayer,long &lNum)
{
	list<tagSGoods*> lSGoods = pMail->GetSGood();
	if (pPlayer==NULL)
		return false;

	long lIncGoodsNum = lSGoods.size();
	list<tagSGoods*>::iterator it = lSGoods.begin();

	for (; it!=lSGoods.end();)
	{
		if(GetYuanBaoNum((*it)->lIndex,(*it)->lNum,pPlayer) > 0)
		{
			lNum += GetYuanBaoNum((*it)->lIndex,(*it)->lNum,pPlayer);
			++it;
		}
		else
		{
			it = lSGoods.erase(it);
		}
	}
	if(lIncGoodsNum>lSGoods.size())
	{
		//存在过期道具
		return false;
	}
	else
	{
		//不存在
		return true;
	}

}
//同意赠送
void	CMailManager::AgreeToPresent(CMail *pMail,CPlayer *pPlayer,BYTE bType)
{
	long lNum = 0;//计算元宝数量
	if (pPlayer==NULL)
		return;

	if (bType==0)
	{
		if (CheckPresentGoods(pMail,pPlayer,lNum))
		{
			//成功
			CMessage msg(MSG_S2C_MAIL_PRESENT_OK);
			msg.Add(pMail->GetExID());
			msg.Add(lNum);
			msg.SendToPlayer(pPlayer->GetExID());
		} 
		else
		{
			//存在过期道具
			CMessage msg(MSG_S2C_MAIL_PRESENT_FAILED);
			msg.Add(pMail->GetExID());
			msg.Add((long)pMail->GetSGood().size());
			list<tagSGoods*>::iterator it=pMail->GetSGood().begin();
			for (; it!=pMail->GetSGood().end();++it)
			{
				msg.Add((*it)->lIndex);
				msg.Add((*it)->lNum);
			}
			msg.SendToPlayer(pPlayer->GetExID());
		}
	}
	else
	{	
		CheckPresentGoods(pMail,pPlayer,lNum);
		if (lNum>pPlayer->GetYuanbao())
		{
			char* strRes=CStringReading::LoadString(22,22);
			if(strRes)
				pPlayer->SendNotifyMessage(strRes);
			return;
		}
		char   strSysMsg[128];	
		
		_snprintf(strSysMsg, 128, CStringReading::LoadString(IDS_WGS_SYSMAIL,STR_WS_SYSMAIL_SEVENTH),pMail->GetSubject().c_str());

		tagMailParam *ptgMailParam = OBJ_CREATE(tagMailParam);
		CMail *pSendMail = MP_NEW CMail(ptgMailParam);
		string strReceiver = pMail->GetWriter();
		pSendMail->SetExID(pMail->GetExID());
		pSendMail->SetWriter(pPlayer->GetName());
		pSendMail->SetReceiverList(strReceiver.c_str());
		pSendMail->SetType(PRESENT_MAIL);
		pSendMail->SetSubject(strSysMsg);

		pSendMail->SetSGoods(pMail->GetSGood());

		if(AddMail(pSendMail))
		{
			list<CGUID> lGuid;
			lGuid.push_back(pMail->GetExID());
			DeleteMailToWS(lGuid,pPlayer);
		}		
	}		
}

//付款收信
void	CMailManager::PayMoneyMail(CMail *pMail,CPlayer *pPlayer)							
{
	char   strSysMsg[128];	
	if (pPlayer==NULL || pMail->GetGold()==0)
		return;
	_snprintf(strSysMsg, 128, CStringReading::LoadString(IDS_WGS_SYSMAIL,STR_WS_SYSMAIL_SIXTH));

	long lPayGold = pMail->GetGold();
	string strReceiver;
	string strSubject = strSysMsg;
	strReceiver = pMail->GetWriter();
	tagMailParam *ptgMailParam = OBJ_CREATE(tagMailParam);
	ptgMailParam->lType = SENDGOLD_MAIL;
	ptgMailParam->lGold = lPayGold;
	ptgMailParam->strWriter = pPlayer->GetName();
	ptgMailParam->strReceiverList.push_back(strReceiver);
	ptgMailParam->WriterGuid = pPlayer->GetExID();
	ptgMailParam->strSubject=strSubject;

	CMail *pPayMail = MP_NEW CMail(ptgMailParam);
	if(AddMail(pPayMail,pPlayer))
	{
		pMail->SetGold(0);
		if (pMail->GetMGoodsContainer().size()>0)
		{
			pMail->SetType(AFFIX_MAIL);
		}
		else
		{
			pMail->SetType(COMMON_MAIL);
		}


		SendPayMailMsg(pMail->GetExID(),BYTE(AFFIX_MAIL),pPlayer);

		CMessage msg(MSG_S2W_MAIL_PAY);
		msg.Add(pMail->GetExID());
		msg.Send();

	}	
}

//从邮箱取物品放到背包
void	CMailManager::GetGoodsFromMail(CGUID &guid,CPlayer *pPlayer,long lIndex)	
{
	CMail *pMail = GetMail(guid);
	if (pMail && pPlayer && pMail->GetType()!=PAYGOLD_MAIL &&  pMail->GetReceiverGuid()==pPlayer->GetExID())//strcmp(pMail->GetReceiver().c_str(),pPlayer->GetName())==0
	{
		pMail->SendGetGoodsMsg(pPlayer,lIndex);
	}
}
//发送取物品给客户端
void	CMailManager::SendGetGoodsGSToClient(CGUID &MailGuid,CGUID &PlayerGuid,long lIndex)
{
	CMail *pMail = GetMail(MailGuid);
	CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);
	if (pMail && pPlayer)
	{
		pMail->GetGoodsFromContainer(pPlayer,lIndex);
	}
}
//向WS发送取金钱消息
void	CMailManager::SendGetGoldFromMailToWS(CGUID &guid,CPlayer *pPlayer)
{
	if (pPlayer)
	{
		CMail* pMail = GetMail(guid);
		if (pMail && (pMail->GetType()==AFFIX_MAIL || pMail->GetType()==SYSTEM_MAIL))
		{
			long lGold = pMail->GetGold();


			if(pPlayer->GetMoney()+lGold>CGlobeSetup::GetSetup()->dwGoldCoinLimit || lGold<=0)
			{
				char* strRes=CStringReading::LoadString(21,38);
				if(strRes)
				{
					pPlayer->SendNotifyMessage(strRes);
				}
				return ;
			}	

			//向WS发送取金钱消息
			CMessage OperaGoldmsg(MSG_S2W_MAIL_OPERA_GOLD);
			OperaGoldmsg.Add(guid);
			OperaGoldmsg.Send();

		}
	}
}
//从邮箱中取金钱
void CMailManager::GetGoldFromMail(CGUID &guid,CPlayer *pPlayer)
{
	if (pPlayer)
	{
		CMail* pMail = GetMail(guid);
		if (pMail && (pMail->GetType()==AFFIX_MAIL || pMail->GetType()==SYSTEM_MAIL))
		{
			long lGold = pMail->GetGold();


			if(pPlayer->GetMoney()+lGold>CGlobeSetup::GetSetup()->dwGoldCoinLimit || lGold<=0)
			{
				char* strRes=CStringReading::LoadString(21,38);
				if(strRes)
				{
					pPlayer->SendNotifyMessage(strRes);
				}
				return ;
			}			
			
			if (pPlayer->SetMoney(lGold+pPlayer->GetMoney(),2))
			{
				pMail->SetReject(0);
				pMail->SetGold(0);
				if (pMail->GetMGoodsContainer().size()==0 && pMail->GetSGood().size()==0 && pMail->GetType()!=SYSTEM_MAIL && pMail->GetType()<MAX_MAIL)
				{
					pMail->SetType(COMMON_MAIL);
				}				
			}
			else
			{	
				CMessage msg(MSG_S2C_MAIL_GET_GOLD_RESULT);
				msg.Add(guid);
				msg.Add((BYTE)0);
				msg.SendToPlayer(pPlayer->GetExID());
				return;
			}
			
			//取金钱日志
			long lGoodsNum = 0;
			for (vector<CGoods*>::iterator it=pMail->GetMGoodsContainer().begin(); it!=pMail->GetMGoodsContainer().end(); ++it)
			{
				lGoodsNum += (*it)->GetAmount();
			}
			GetGameLogInterface()->logT603_GetGold_mail_log(pMail->GetExID(),pMail->GetReceiver().c_str(),pMail->GetWriter().c_str(),pMail->GetType(),lGold, lGoodsNum,pMail->GetSGood().size());	

			CMessage msg(MSG_S2C_MAIL_GET_GOLD_RESULT);
			msg.Add(guid);
			msg.Add((BYTE)1);
			msg.SendToPlayer(pPlayer->GetExID());	

		}
	}

}


//向客户端返回所有邮件信息
void	CMailManager::SendAllMailToClient(vector<BYTE> &vectorByte, CPlayer *pPlayer)
{	
	if (pPlayer==NULL)
		return;
	CMessage msg(MSG_S2C_MAIL_RECEIVER);
	msg.Add((long)vectorByte.size());
	if (vectorByte.size())	
		msg.AddEx(&vectorByte[0], (long)vectorByte.size());			
	msg.SendToPlayer(pPlayer->GetExID());
}

//检查是否有非法字符
bool	CMailManager::CheckInvalidCharacter(string strText)							
{
	if(!CWordsFilter::GetInstance()->Check(strText,false,true))//
	{
		return false;
	}
	return true;
}

//系统删除邮件
void 	CMailManager::DeleteMail(list<CGUID> &guid,CPlayer* pPlayer)
{
	if (pPlayer==NULL)
		return;
	long lNum = guid.size();
	if (lNum>0 && pPlayer)
	{
		for (list<CGUID>::iterator it=guid.begin();it!=guid.end();++it)
		{
			map<CGUID,CMail*>::iterator mit=m_mapMail.find((*it));
			if (mit!=m_mapMail.end())
			{
				pPlayer->DeleteReceiveMailList(*it);

				long lGoodsNum = 0;
				for (vector<CGoods*>::iterator tit=((*mit).second)->GetMGoodsContainer().begin(); tit!=((*mit).second)->GetMGoodsContainer().end(); ++tit)
				{
					lGoodsNum += (*tit)->GetAmount();
				}
				//删除邮件日志
				GetGameLogInterface()->logT605_delete_mail_log((*mit).first, ((*mit).second)->GetReceiver().c_str(),
																			  ((*mit).second)->GetWriter().c_str(),
																			  ((*mit).second)->GetType(),
																			  ((*mit).second)->GetGold(),
																			  lGoodsNum,
																			  ((*mit).second)->GetSGood().size());
				(*mit).second->DeleteMailGoods();
				MP_DELETE((*mit).second);
				m_mapMail.erase(mit);
			}
		}
	}
}
//检查删信
bool	CMailManager::CheckDeleteMail(list<CGUID> &lguid,CPlayer *pPlayer)
{
	for (list<CGUID>::iterator dit=lguid.begin(); dit!=lguid.end(); ++dit)
	{
		map<CGUID,CMail*>::iterator it=m_mapMail.find((*dit));
		if (it!=m_mapMail.end())
		{	
			if((*it).second->GetType()==PAYGOLD_MAIL || (*it).second->GetReceiverGuid()!=pPlayer->GetExID())
				return false;	
		}
	}
	return true;
}
//发送删除邮件到WS
void	CMailManager::DeleteMailToWS(list<CGUID> &guid,CPlayer *pPlayer)
{
	if (pPlayer==NULL)
		return;
	long lNum = guid.size();
	CMessage msg(MSG_S2W_MAIL_DELETE);	
	msg.Add(pPlayer->GetExID());
	msg.Add(lNum);
	for (list<CGUID>::iterator it=guid.begin(); it!=guid.end(); ++it)
	{
		msg.Add((*it));
	}
	msg.Send();
}
//发送删除邮件到Client
void	CMailManager::DeleteMailToClient(list<CGUID> &guid,CPlayer *pPlayer)
{
	if (pPlayer==NULL)
		return;
	long lNum = guid.size();
	CMessage msg(MSG_S2C_MAIL_DELETE_OK);	
	msg.Add(lNum);
	for (list<CGUID>::iterator it=guid.begin(); it!=guid.end(); ++it)
	{
		msg.Add((*it));
	}
	msg.SendToPlayer(pPlayer->GetExID());
	DeleteMail(guid,pPlayer);
}


//读取邮件
void	CMailManager::ReadMail(CGUID &guid,CPlayer *pPlayer)
{	
	if (pPlayer==NULL)
		return;
	map<CGUID,CMail*>::iterator it = m_mapMail.find(guid);

	if (it!=m_mapMail.end())
	{		
		if((*it).second->GetReceiverGuid()!=pPlayer->GetExID())
		{
			GetGameLogInterface()->logT604_read_mail_log(pPlayer,guid,0,0,0,0,0);
			return;
		}

		if ((*it).second->GetRead()==0)
		{
			CMessage msg(MSG_S2W_MAIL_READ);
			msg.Add(guid);
			msg.Send();
		}
		//读取邮件成功日志
		long lGoodsNum = 0;
		for (vector<CGoods*>::iterator mit=(*it).second->GetMGoodsContainer().begin(); mit!=(*it).second->GetMGoodsContainer().end(); ++mit)
		{
			lGoodsNum += (*mit)->GetAmount();
		}

		GetGameLogInterface()->logT604_read_mail_log(pPlayer,guid,(*it).second->GetType(),(*it).second->GetGold(),lGoodsNum,(*it).second->GetSGood().size(),1);
		(*it).second->SetRead(1);				
	}
	else
	{
		//读取邮件失败日志
		GetGameLogInterface()->logT604_read_mail_log(pPlayer,guid,0,0,0,0,0);
		SendReadMailRet(guid,pPlayer->GetExID(),0);
		return;
	}
	SendReadMailRet(guid,pPlayer->GetExID(),1);
}
//发送读取邮件结果	
void	CMailManager::SendReadMailRet(CGUID &MailGuid,const CGUID &PlayerGuid, BYTE bRet)
{
	CMessage msg(MSG_S2C_MAIL_READ);
	msg.Add(bRet);
	msg.Add(MailGuid);

	msg.SendToPlayer(PlayerGuid);
}
//初始化系统邮件参数
void	CMailManager::InitSysMailParam(tagMailParam *pParam,tagMailParam *pPamramList)
{
	pParam->bRead = pPamramList->bRead;
	pParam->lType = pPamramList->lType;

	pParam->strExText = pPamramList->strExText;
	pParam->strText  = pPamramList->strText;
	pParam->bReject = 0;
	pParam->tRemainTime = pPamramList->tRemainTime;

	pParam->strWriter = pPamramList->strWriter;
	pParam->WriterGuid = NULL_GUID;
	pParam->strSubject = pPamramList->strSubject;
	pParam->strReceiverList = pPamramList->strReceiverList;
	pParam->lGold = pPamramList->lGold;

	for (list<tagMailEvent*>::iterator it=pPamramList->ltgMailEvent.begin(); it!=pPamramList->ltgMailEvent.end(); ++it)
	{
		tagMailEvent *ptgMailEvent = OBJ_CREATE(tagMailEvent);
		ptgMailEvent->strEventName = (*it)->strEventName;
		ptgMailEvent->lBeginTime = (*it)->lBeginTime;
		ptgMailEvent->lEndTime = (*it)->lEndTime;
		pParam->ltgMailEvent.push_back(ptgMailEvent);
	}

	for (list<tagSGoods*>::iterator it=pPamramList->lSGoods.begin(); it!=pPamramList->lSGoods.end(); ++it)
	{
		tagSGoods *ptgSGoods = OBJ_CREATE(tagSGoods);
		ptgSGoods->lIndex = (*it)->lIndex;
		ptgSGoods->lNum = (*it)->lNum;
		pParam->lSGoods.push_back(ptgSGoods);
	}
	for (list<tagMailCondition*>::iterator it=pPamramList->lMailCondition.begin(); it!=pPamramList->lMailCondition.end(); ++it)
	{
		tagMailCondition *ptgMailCondition = OBJ_CREATE(tagMailCondition);
		ptgMailCondition->strName = (*it)->strName;
		ptgMailCondition->lValue = (*it)->lValue;
		pParam->lMailCondition.push_back(ptgMailCondition);
	}

}
//初始化系统邮件
void	CMailManager::InitSysMail()
{
	ClearSysMail();
	map<DWORD,tagMailParam*> mapMail = CMailList::GetSysMail();
	map<DWORD,tagMailParam*>::iterator it=mapMail.begin();
	for (;it!=mapMail.end();++it)
	{		
		CGUID guid;
		CGUID::CreateGUID(guid);
		tagMailParam *ptgMailParam = OBJ_CREATE(tagMailParam);
		InitSysMailParam(ptgMailParam,(*it).second);
		ptgMailParam->guid =guid;
		ptgMailParam->lType = (*it).first;

		CMail *pMail = MP_NEW CMail(ptgMailParam);
		AddSysMail((*it).first,pMail);	
	}
}
//在玩家进入游戏时如果有系统邮件发送的消息提示
void CMailManager::SendSysMailMsgToReciver(CPlayer *pPlayer)
{
	if (pPlayer==NULL)
		return;
	time_t t_CurrentTime;
	t_CurrentTime = time(NULL);
	long lCurrentTime = (long)t_CurrentTime;
	map<long,CMail*>::iterator it=m_mapSysMail.begin();

	for (;it!=m_mapSysMail.end();++it)
	{
		CMail *pSysMail = (*it).second;
		long lBeingTime = 0;
		long lEndTime = 0;
		pSysMail->GetSysSendTime(lBeingTime,lEndTime);
		if (!pPlayer->CheckSysMail((*it).first) && lCurrentTime>lBeingTime && lCurrentTime<=lEndTime)
		{			
			SendSysMailMsg(pSysMail,pPlayer);
		}
	}

}
//收信
void CMailManager::SendReceiveMail(CPlayer *pPlayer)
{
	list<CGUID> lReceiveMail = pPlayer->GetReceiveMailList();
	if (lReceiveMail.size()>0 && pPlayer->GetReceiveMail())
	{		
		vector<BYTE> vectorByte;
		vectorByte.clear();


		_AddToByteArray(&vectorByte,(long)lReceiveMail.size());
		for (list<CGUID>::iterator it=lReceiveMail.begin();it!=lReceiveMail.end();++it)
		{
			map<CGUID,CMail*>::iterator mit = m_mapMail.find((*it));
			if (mit!=m_mapMail.end())
			{
				(*mit).second->AddToByteArrayForClient(vectorByte,pPlayer);

				long lGoodsNum = 0;
				for (vector<CGoods*>::iterator tit=(*mit).second->GetMGoodsContainer().begin(); tit!=(*mit).second->GetMGoodsContainer().end(); ++tit)
				{
					lGoodsNum += (*tit)->GetAmount();
				}

				GetGameLogInterface()->logT600_Receive_mail_log(pPlayer,(*mit).second->GetExID(),(*mit).second->GetWriter().c_str(),(*mit).second->GetType(),(*mit).second->GetGold(),lGoodsNum,(*mit).second->GetSGood().size(),1);
			}
		}
		SendAllMailToClient(vectorByte,pPlayer);
	} 
	else
	{
		CMessage msg(MSG_S2W_MAIL_RECEIVE);
		msg.Add(pPlayer->GetExID());
		msg.Send();
	}	
}



//在玩家第一次收信时将没有收到的系统邮件接收
void CMailManager::SendSysMailToReciver(CPlayer *pPlayer)
{
	if (pPlayer==NULL)
		return;
	time_t t_CurrentTime;
	t_CurrentTime = time(NULL);
	long lCurrentTime = (long)t_CurrentTime;
	map<long,CMail*>::iterator it=m_mapSysMail.begin();

	for (;it!=m_mapSysMail.end();++it)
	{
		CMail *pSysMail = (*it).second;
		long lBeingTime = 0;
		long lEndTime = 0;
		pSysMail->GetSysSendTime(lBeingTime,lEndTime);
		if (!pPlayer->CheckSysMail((*it).first) && lCurrentTime>lBeingTime && lCurrentTime<=lEndTime)
		{
			SendSysMail(pSysMail,pPlayer,(*it).first,lEndTime);	
			pSysMail->DeleteMailGoods();
		}
	}
	pPlayer->CheckSysMailTime();
}

//添加系统邮件
bool	CMailManager::AddSysMail(long lMailid, CMail *pMail)
{
	if (pMail)
	{
		map<long,CMail*>::iterator it=m_mapSysMail.find(lMailid);
		if (it==m_mapSysMail.end())
		{			
			m_mapSysMail[lMailid]=pMail;	
			return true;
		}
		else
		{
			pMail->DeleteMailGoods();
			MP_DELETE(pMail);
			return false;
		}
	}	
	return false;
}
//获取邮件
CMail	*CMailManager::GetMail(CGUID &guid)	
{
	map<CGUID,CMail*>::iterator it=m_mapMail.find(guid);
	if (it!=m_mapMail.end())
	{
		return (*it).second;
	}
	return NULL;
}

//通过guid得到系统邮件
CMail*  CMailManager::GetSysMail(long  lMailID)												
{
	map<long,CMail*>::iterator it=m_mapSysMail.find(lMailID);
	if (it!=m_mapSysMail.end())
	{
		return (*it).second;
	}
	return NULL;
}
//清除系统邮件
void	CMailManager::ClearSysMail()															
{
	map<long,CMail*>::iterator it=m_mapSysMail.begin();
	for (;it!=m_mapSysMail.end();)
	{
		MP_DELETE((*it).second);
		it = m_mapSysMail.erase(it);
	}
}
//释放邮件
void	CMailManager::ReleaseMail(list<CGUID> &lMailGuid)
{
	list<CGUID>::iterator it = lMailGuid.begin();
	for (; it!=lMailGuid.end(); ++it)
	{
		map<CGUID,CMail*>::iterator mit=m_mapMail.find(*it);
		if (mit!=m_mapMail.end())
		{
			(*mit).second->DeleteMailGoods();
			MP_DELETE((*mit).second);
			mit = m_mapMail.erase(mit);
		}
	}
}