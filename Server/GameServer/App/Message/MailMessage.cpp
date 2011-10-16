/**************************************************
*		文件名  ：MailMessage.h
*		简要描述：处理Client以及WS发来的消息

*		当前版本：1.0
*		作者    ：石喜
*		完成日期：10/10/18
*		修订说明：
****************************************************/

#include "StdAfx.h"
#include "../Player.h"
#include "../Monster.h"
#include "../Mail/Mail.h"
#include "../../Net/Message.h"
#include "../ServerRegion.h"

#include "../../../../Public/Setup/GlobalSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum eMAXGOODSNUM
{
	MAXGOODNUM = 5,
};
//客户端消息
void DoC2SMailSend( CMessage *pMsg );
void DoC2SMailGetGoods( CMessage *pMsg );
void DoC2SMailRead( CMessage *pMsg );
void DoC2SMailDelete( CMessage *pMsg );
void DoC2SMailReject( CMessage *pMsg );
void DoC2SMailAgreedPay( CMessage *pMsg );
void DoC2SMailGetGolds( CMessage *pMsg );
void DoC2SMailReceive( CMessage *pMsg );
//WS发过来的消息
void DoW2SMailSendFailed( CMessage *pMsg );
void DoW2SMailOperaGold( CMessage *pMsg );
void DoW2SMailOperaGoods( CMessage *pMsg );
void DoW2SMailSend( CMessage *pMsg );
void DoW2SMailSendResult( CMessage *pMsg );
void DoW2SMailDelete( CMessage *pMsg );
void DoW2SMailSendSys( CMessage *pMsg );
void DoW2SMailReceive( CMessage *pMsg );

void OnMailMessageForClient( CMessage *pMsg )
{
	switch(pMsg->GetType())
	{
	case MSG_C2S_MAIL_SEND:
		{
			DoC2SMailSend( pMsg );
		}	
		break;
	case MSG_C2S_MAIL_RECEIVER:
		{
			DoC2SMailReceive( pMsg );
		}
		break;
	case MSG_C2S_MAIL_GET_GOODS:
		{
			DoC2SMailGetGoods( pMsg );
		}
		break;
	case MSG_C2S_MAIL_READ:
		{
			DoC2SMailRead( pMsg );
		}
		break;
	case MSG_C2S_MAIL_DELETE:
		{
			DoC2SMailDelete( pMsg );
		}
		break;
	case MSG_C2S_MAIL_REJECT:
		{
			DoC2SMailReject( pMsg );
		}
		break;
	case MSG_C2S_MAIL_AGREED_PAY:
		{
			DoC2SMailAgreedPay( pMsg );
		}
		break;
	case MSG_C2S_MAIL_GET_GOLDS:
		{
			DoC2SMailGetGolds( pMsg );
		}
		break;
	}
}

void OnMailMessageForWs(CMessage *pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_W2S_MAIL_SEND_FAILED:
		{
			DoW2SMailSendFailed( pMsg );
		}
		break;
	case MSG_W2S_MAIL_OPERA_GOLD:
		{
			DoW2SMailOperaGold( pMsg );					
		}
		break;
	case MSG_W2S_MAIL_OPERA_GOODS:
		{
			DoW2SMailOperaGoods( pMsg );	
		}
		break;
	case MSG_W2S_MAIL_SEND:
		{
			DoW2SMailSend( pMsg );		
		}
		break;
	case MSG_W2S_MAIL_SEND_RESULT:
		{
			DoW2SMailSendResult( pMsg );			
		}
		break;
	case MSG_W2S_MAIL_DELETE:
		{
			DoW2SMailDelete( pMsg );			
		}
	case MSG_W2S_MAIL_SEND_SYS:
		{
			DoW2SMailSendSys( pMsg );		
		}
		break;
	case MSG_W2S_MAIL_RECEIVE:
		{
			DoW2SMailReceive( pMsg );
		}
		break;
	}
}


void DoC2SMailSend( CMessage *pMsg )
{
	CPlayer *pPlayer = pMsg->GetPlayer();
	if (NULL == pPlayer)
	{
		return;
	}
	//玩家在交易或摆摊中不能交易
	if (pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL || pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_TRADING )
	{
		return;
	}

	CGUID NpcGuid = NULL_GUID;
	pMsg->GetGUID(NpcGuid);

	CServerRegion *pRegion = pMsg->GetRegion();
	if (NULL==pRegion)
	{
		return;
	}
	CBaseObject *pShape = pRegion->FindChildObject(TYPE_MONSTER,NpcGuid);

	CMonster *pMailNpc = dynamic_cast<CMonster*>(pShape);

	//if (pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)//  --attention
	//{


	tagMailParam *pMailParam = OBJ_CREATE(tagMailParam);

	long lReceiver	= pMsg->GetLong();						//收信人个数
	if (lReceiver > 100)
	{
		return;
	}

	for (int i=0; i<lReceiver; ++i)
	{
		char strReceiver[128];
		pMsg->GetStr(strReceiver,GlobalSetup::GetSetup()->lMaxName+1);

		if (lstrlen(strReceiver)>=GlobalSetup::GetSetup()->lMaxName)
		{
			GameManager::GetInstance()->GetMailManager()->SendMailFailedToClient(pPlayer->GetExID(),MAIL_SENDING_FAILED_INVALID_MAXNAME);
			continue;
		}
		list<string>::iterator it= find((pMailParam->strReceiverList).begin(),(pMailParam->strReceiverList).end(),strReceiver);//           (pMailParam->strReceiverList).f
		if (it==(pMailParam->strReceiverList).end())
		{
			(pMailParam->strReceiverList).push_back(strReceiver);
		}				
	}
	char strTemp[2048];
	pMsg->GetStr(strTemp,GlobalSetup::GetSetup()->lMaxSubject+1);								//主题
	if (lstrlen(strTemp)>=GlobalSetup::GetSetup()->lMaxSubject)
	{
		GameManager::GetInstance()->GetMailManager()->SendMailFailedToClient(pPlayer->GetExID(),MAIL_SENDING_FAILED_INVALID_MAXSUBJECT);
		return;
	}
	pMailParam->strSubject	= strTemp;


	pMsg->GetStr(strTemp,GlobalSetup::GetSetup()->lMaxText+1);							//内容
	if (lstrlen(strTemp)>=GlobalSetup::GetSetup()->lMaxText)
	{
		GameManager::GetInstance()->GetMailManager()->SendMailFailedToClient(pPlayer->GetExID(),MAIL_SENDING_FAILED_INVALID_MAXTEXT);
		return;
	}

	pMailParam->strText			= strTemp;

	pMailParam->lGold			= pMsg->GetLong();			//金币
	pMailParam->lType			= pMsg->GetByte();			//类型


	////普通物品
	long lCGoodNum	= pMsg->GetLong();	

	if (lCGoodNum>MAXGOODNUM)
	{
		char pszGuid[50]="";
		pPlayer->GetExID().tostring(pszGuid);
		Log4c::Warn(ROOT_MODULE,FormatText("GS_GOODS_91",pPlayer->GetName(),pszGuid,lCGoodNum));//%s(ID:%s) 玩家发送物品的数量:%d超过限制(此玩家利用非常规方式发送消息)

		OBJ_RELEASE(tagMailParam, pMailParam);
		return;
	}
	for (int i=0; i<lCGoodNum; ++i)
	{		
		listGoods *ptgGoods = OBJ_CREATE(listGoods);
		pMsg->GetGUID(ptgGoods->goodsguid);
		ptgGoods->lNum = pMsg->GetLong();
		ptgGoods->lContainerID = pMsg->GetDWord();
		ptgGoods->lPos	= pMsg->GetDWord();
		pMailParam->Goodslist.push_back(ptgGoods);
	}
	//}
	pMailParam->strWriter = pPlayer->GetName();
	pMailParam->WriterGuid = pPlayer->GetExID();
	pMailParam->bReject = 1;


	//! 发信			
	CMail *pMail = OBJ_CREATE_PVOID(CMail, (void*)pMailParam);

	if (pMail->GetType()==SYSTEM_MAIL || pMail->GetGold()<0 || pMail->GetType()>=SENDGOLD_MAIL)
	{
		pMail->DeleteMailGoods();
		OBJ_RELEASE(CMail, pMail);
		return;
	}			

	GameManager::GetInstance()->GetMailManager()->AddMail(pMail,pPlayer);//“在这里发送 MSG_S2W_MAIL_SEND 消息给 worldserver
	//	}
}

void DoC2SMailGetGoods( CMessage *pMsg )
{
	//! 从邮件中取物品到背包
	CPlayer *pPlayer = pMsg->GetPlayer();
	CGUID NpcGuid = NULL_GUID;
	pMsg->GetGUID(NpcGuid);

	CServerRegion *pRegion = pMsg->GetRegion();
	if (NULL==pRegion)
	{
		return;
	}
	CBaseObject *pShape = pRegion->FindChildObject(TYPE_MONSTER,NpcGuid);

	CMonster *pMailNpc = dynamic_cast<CMonster*>(pShape);

	//if (pPlayer &&pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)//attention
	//{
		CGUID MailGuid;
		pMsg->GetGUID(MailGuid);
		long lIndex = pMsg->GetLong();			

		GameManager::GetInstance()->GetMailManager()->GetGoodsFromMail(MailGuid,pPlayer,lIndex);		

	//}

}

void DoC2SMailRead( CMessage *pMsg )
{
	CPlayer *pPlayer = pMsg->GetPlayer();
	CGUID NpcGuid = NULL_GUID;
	pMsg->GetGUID(NpcGuid);

	//////////////////////////////////////////////////////////////////////////		attention		add
	//CServerRegion *pRegion = pMsg->GetRegion();
	//if (NULL==pRegion)
	//{
	//	return;
	//}
	//CBaseObject *pShape = pRegion->FindChildObject(TYPE_MONSTER,NpcGuid);

	//CMonster *pMailNpc = dynamic_cast<CMonster*>(pShape);

	//if (pPlayer &&  pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)
	//{
	//	CGUID guid;
	//	pMsg->GetGUID(guid);
	//
	//	GameManager::GetInstance()->GetMailManager()->ReadMail(guid,pPlayer);
	//	
	//}
	//////////////////////////////////////////////////////////////////////////

	CGUID guid;
	pMsg->GetGUID(guid);

	GameManager::GetInstance()->GetMailManager()->ReadMail(guid,pPlayer);
}
void DoC2SMailDelete( CMessage *pMsg )
{
	//! 删除邮件
	list<CGUID> lGuid;
	CPlayer *pPlayer = pMsg->GetPlayer();	

	CGUID NpcGuid = NULL_GUID;
	pMsg->GetGUID(NpcGuid);

	CServerRegion *pRegion = pMsg->GetRegion();
	if (NULL==pRegion)
	{
		return;
	}
	CBaseObject *pShape = pRegion->FindChildObject(TYPE_MONSTER,NpcGuid);
	CMonster *pMailNpc = dynamic_cast<CMonster*>(pShape);

	//if (pPlayer && pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)//tag
	//{
		long lNum = pMsg->GetWord();

		if (lNum<0 || NULL==pPlayer)
		{
			return;
		}	

		for (int i = 0; i < lNum; ++i)
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			lGuid.push_back(guid);
		}

		//if (GameManager::GetInstance()->GetMailManager()->CheckDeleteMail(lGuid,pPlayer))
		//{
		GameManager::GetInstance()->GetMailManager()->DeleteMailToWS(lGuid,pPlayer);			
//		}
	//}

}
void DoC2SMailReject( CMessage *pMsg )
{
	CPlayer *pPlayer = pMsg->GetPlayer();
	CGUID NpcGuid = NULL_GUID;
	pMsg->GetGUID(NpcGuid);

	CServerRegion *pRegion = pMsg->GetRegion();
	if (NULL==pRegion)
	{
		return;
	}
	CBaseObject *pShape = pRegion->FindChildObject(TYPE_MONSTER,NpcGuid);
	CMonster *pMailNpc = dynamic_cast<CMonster*>(pShape);

	//if (pPlayer && pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)
	//{

		//! 拒收邮件
		CGUID guid;
		pMsg->GetGUID(guid);				
		CMail *pMail = GameManager::GetInstance()->GetMailManager()->GetMail(guid);
		if (pMail && pMail->GetReceiverGuid()==pPlayer->GetExID())
		{
			pPlayer->DeleteReceiveMailList(guid);
			CMessage msg(MSG_S2W_MAIL_REJECT);
			msg.Add(pPlayer->GetExID());
			msg.Add(guid);
			msg.Send();					
		}
	//}
}
void DoC2SMailAgreedPay( CMessage *pMsg )
{
	//! 同意付费
	CPlayer *pPlayer = pMsg->GetPlayer();
	CGUID NpcGuid = NULL_GUID;
	pMsg->GetGUID(NpcGuid);

	CServerRegion *pRegion = pMsg->GetRegion();
	if (NULL==pRegion)
	{
		return;
	}
	if (NULL==pPlayer)
	{
		return;
	}

	//玩家在交易或摆摊中不能付款
	if (pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL 
		|| pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_TRADING )
	{
		return;
	}

	CBaseObject *pShape = pRegion->FindChildObject(TYPE_MONSTER,NpcGuid);
	CMonster *pMailNpc = dynamic_cast<CMonster*>(pShape);

	//if (pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)//--attention
	//{			
		CGUID MailGuid;
		pMsg->GetGUID(MailGuid);
		CMail *pMail = GameManager::GetInstance()->GetMailManager()->GetMail(MailGuid);
		if (pMail && pMail->GetReceiverGuid()==pPlayer->GetExID() && pMail->GetType()==PAYGOLD_MAIL)
		{
			GameManager::GetInstance()->GetMailManager()->PayMoneyMail(pMail,pPlayer);
		}
	//}
}
void DoC2SMailGetGolds( CMessage *pMsg )
{
	//取金钱
	CPlayer *pPlayer = pMsg->GetPlayer();
	CGUID NpcGuid = NULL_GUID;
	pMsg->GetGUID(NpcGuid);
	CServerRegion *pRegion = pMsg->GetRegion();

	if (NULL==pRegion)
	{
		return;
	}
	CBaseObject *pShape = pRegion->FindChildObject(TYPE_MONSTER,NpcGuid);
	CMonster *pMailNpc = dynamic_cast<CMonster*>(pShape);

	//if (pPlayer && pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)// attention
	//{
		CGUID MailGuid;
		pMsg->GetGUID(MailGuid);
		CMail *pMail = GameManager::GetInstance()->GetMailManager()->GetMail(MailGuid);
		if (NULL!=pPlayer && pMail && pMail->GetReceiverGuid()==pPlayer->GetExID())
		{
			GameManager::GetInstance()->GetMailManager()->SendGetGoldFromMailToWS(MailGuid,pPlayer);
		}
	//}

}

void DoC2SMailReceive( CMessage *pMsg )
{
	CPlayer *pPlayer = pMsg->GetPlayer();
	GameManager::GetInstance()->GetMailManager()->SendReceiveMail( pPlayer );
}




//////////////////////////////////////////////////////////////////////////
void DoW2SMailSendFailed( CMessage *pMsg )
{
	//! 发信失败
	CGUID PlayerGuid = NULL_GUID;
	long lRet = pMsg->GetLong();
	pMsg->GetGUID(PlayerGuid);
	GameManager::GetInstance()->GetMailManager()->SendMailFailedToClient(PlayerGuid,lRet);
}
void DoW2SMailOperaGold( CMessage *pMsg )
{
	//金钱操作
	CGUID MailGuid;
	CGUID PlayerGuid;
	pMsg->GetGUID(MailGuid);
	pMsg->GetGUID(PlayerGuid);

	CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);
	GameManager::GetInstance()->GetMailManager()->GetGoldFromMail(MailGuid,pPlayer);			
}
void DoW2SMailOperaGoods( CMessage *pMsg )
{
	CGUID MailGuid;
	CGUID PlayerGuid;
	long lIndex = pMsg->GetLong();
	pMsg->GetGUID(MailGuid);
	pMsg->GetGUID(PlayerGuid);
	GameManager::GetInstance()->GetMailManager()->SendGetGoodsGSToClient(MailGuid,PlayerGuid,lIndex);
}
void DoW2SMailSend( CMessage *pMsg )
{
	BYTE *ByteData = NULL;	
	CGUID MailGuid;

	pMsg->GetGUID(MailGuid);

	long lByteDataLen = pMsg->GetLong();
	ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);	
	pMsg->GetEx(ByteData,lByteDataLen);
	long Pos = 0;
	tagMailParam *pMailParam = OBJ_CREATE(tagMailParam);

	GameManager::GetInstance()->GetMailManager()->DecordFromByteArray(ByteData,Pos,pMailParam);
	CMail *pMail = OBJ_CREATE_PVOID(CMail, (void*)pMailParam);//用pMailParam作为构造函数创建CMail对象
	GameManager::GetInstance()->GetMailManager()->SendMailToClient(pMail);
	M_FREE(ByteData, sizeof(BYTE)*lByteDataLen);
}
void DoW2SMailSendResult( CMessage *pMsg )
{
	long lRet=0;
	BYTE *ByteData = NULL;
	CPlayer *pPlayer=NULL;
	CGUID WriterGuid;
	pMsg->GetGUID(WriterGuid);
	lRet = pMsg->GetLong();
	long lByteDataLen = pMsg->GetLong();					

	if (lRet==1)
	{
		ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);	
		pMsg->GetEx(ByteData,lByteDataLen);
		long Pos = 0;
		tagMailParam *pMailParam = OBJ_CREATE(tagMailParam);
		GameManager::GetInstance()->GetMailManager()->DecordMailMsgFromByteArray(ByteData,Pos,pMailParam);
		CMail *pMail = OBJ_CREATE_PVOID(CMail, (void*)pMailParam);
		pPlayer = GetGame()->FindPlayer(pMail->GetWriter().c_str());

		if (pPlayer)
		{
			int nRet = GameManager::GetInstance()->GetMailManager()->ReMoveGoldAndGoods(pMail,pPlayer);//--扣除物品和金钱
			GameManager::GetInstance()->GetMailManager()->SendMailToReceiver(pMail->GetExID(),nRet);
		}

		OBJ_RELEASE(CMail, pMail);			
		M_FREE(ByteData, sizeof(BYTE)*lByteDataLen);				
	}
	else
	{
		GameManager::GetInstance()->GetMailManager()->SendMailFailedToClient(WriterGuid,MAIL_SENDING_FAILED_INVALID_RECEIVER);//--非法收信人
	}
}
void DoW2SMailDelete( CMessage *pMsg )
{
	//! 删除信件
	CGUID guid;
	pMsg->GetGUID(guid);
	long lNum = pMsg->GetLong();
	list<CGUID> lGuid;
	for (int i=0; i<lNum; ++i)
	{
		CGUID TempGuid;
		pMsg->GetGUID(TempGuid);
		lGuid.push_back(TempGuid);
	}
	CPlayer *pPlayer = GetGame()->FindPlayer(guid);
	if (pPlayer)
	{
		GameManager::GetInstance()->GetMailManager()->DeleteMailToClient(lGuid,pPlayer);
	}
}
void DoW2SMailSendSys( CMessage *pMsg )
{
	//系统群发邮件
	long lSysMailID = pMsg->GetLong();
	GameManager::GetInstance()->GetMailManager()->SendSysMailMsgToReceiver(lSysMailID);
}
void DoW2SMailReceive( CMessage *pMsg )
{
	BYTE *ByteData = NULL;
	list<CGUID> lMailGuid;
	list<CMail*> lMailList;
	CGUID playerGuid;
	pMsg->GetGUID(playerGuid);

	long lByteDataLen = pMsg->GetLong();
	ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);	
	pMsg->GetEx(ByteData,lByteDataLen);
	time_t t_CurrentTime = time(NULL);

	CPlayer *pPlayer =  GetGame()->FindPlayer(playerGuid);
	if (pPlayer)
	{				
		long Pos = 0;
		long lNum = _GetLongFromByteArray(ByteData,Pos);
		for (int i=0; i<lNum; ++i)
		{
			tagMailParam *pMailParam = OBJ_CREATE(tagMailParam);
			GameManager::GetInstance()->GetMailManager()->DecordFromByteArray(ByteData,Pos,pMailParam);

			CMail *pMail = OBJ_CREATE_PVOID(CMail, (void*)pMailParam);

			if(!GameManager::GetInstance()->GetMailManager()->SetMapMail(pMail))
			{
				lMailGuid.push_back(pMailParam->guid);					
			}
			else
			{
				lMailList.push_back(pMail);
			}
		}
		if(lMailGuid.size()>0)
		{
			//删除邮件		
			GameManager::GetInstance()->GetMailManager()->DeleteMailToWS(lMailGuid,pPlayer);				
		}

		vector<BYTE> vectorByte;
		vectorByte.clear();

		_AddToByteArray(&vectorByte,(long)lMailList.size());
			
		pPlayer->SetReceiveMail(true);	
		for (list<CMail*>::iterator it=lMailList.begin();it!=lMailList.end();++it)
		{
			pPlayer->AddReceiveMailList((*it)->GetExID());
		}

		GameManager::GetInstance()->GetMailManager()->SendReceiveMail( pPlayer );
	}
	M_FREE(ByteData, lByteDataLen);
}