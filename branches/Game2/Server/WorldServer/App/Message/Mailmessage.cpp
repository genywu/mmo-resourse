/**************************************************
*		文件名  ：MailMessage.h
*		简要描述：处理Client以及WS发来的消息

*		当前版本：1.0
*		作者    ：石喜
*		完成日期：10/10/18
*		修订说明：
****************************************************/
#include "StdAfx.h"

#include "MsgOutTimeDef.h"
#include "../DBEntity/EntityGroup.h"
#include "../DBEntity/EntityManager.h"
#include "../GlobalRgnManager.h"
#include "../Goods/Goods.h"
#include "../Goods/GoodsFactory.h"
#include "../OrganizingSystem/OrganizingCtrl.h"
#include "../Player.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void DoS2WMailDelete( CMessage *pMsg );
void DoS2WMailSend( CMessage *pMsg );
void DoS2WMailSendReceiver( CMessage *pMsg );// --发信给某个玩家
void DoS2WMailReceive( CMessage *pMsg );
void DoS2WMailOperaGoods( CMessage *pMsg );
void DoS2WMailOperaGold( CMessage *pMsg );
void DoS2WMailPay( CMessage *pMsg );
void DoS2WMailRead( CMessage *pMsg );
void DoS2WMailReject( CMessage *pMsg );
//void DoS2WMailPresentReceiver( CMessage *pMsg );
//void DoDB2WMailOperationResult( CMessage *pMsg );

void OnMailMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	
	case MSG_S2W_MAIL_DELETE:
		{
			DoS2WMailDelete( pMsg );
		}
		break;
	case MSG_S2W_MAIL_SEND:
		{
			DoS2WMailSend( pMsg );	
		}
		break;

	case MSG_S2W_MAIL_SEND_RECEIVER:// --发信给某个玩家
		{
			DoS2WMailSendReceiver( pMsg );
		}
		break;
	case MSG_S2W_MAIL_RECEIVE:
		{
			DoS2WMailReceive( pMsg );
		}
		break;
	case MSG_S2W_MAIL_OPERA_GOODS:
		{
			DoS2WMailOperaGoods( pMsg );
		}
		break;
	case MSG_S2W_MAIL_OPERA_GOLD:
		{
			DoS2WMailOperaGold( pMsg );
		}
		break;
	case MSG_S2W_MAIL_PAY:
		{
			DoS2WMailPay( pMsg );
		}
		break;
	case MSG_S2W_MAIL_READ:
		{
			DoS2WMailRead( pMsg );		
		}
		break;
	case MSG_S2W_MAIL_REJECT:
		{
			DoS2WMailReject( pMsg );
		}
		break;
	//case	MSG_S2W_MAIL_PRESENT_RECEIVER:
	//	{
	//		DoS2WMailPresentReceiver( pMsg );
	//	}
	//	break;
	//case MSG_DB2W_MAIL_OPERATION_RESLUT://del
	//	{
	//		DoDB2WMailOperationResult( pMsg );
	//	}
	//	break;
	}
}
void DoS2WMailDelete( CMessage *pMsg )
{
	list<CGUID> lMailGuid;
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);
	long lMailNum = pMsg->GetLong();
	for (int i=0; i<lMailNum; ++i)
	{
		CGUID TempGuid;
		pMsg->GetGUID(TempGuid);
		lMailGuid.push_back(TempGuid);
	}
	GetGame().GetMailManager()->SendDeleteMailToDB(PlayerGuid, lMailGuid);
}
void DoS2WMailSend( CMessage *pMsg )
{
	char  strTemp[102400];
	tagMailParam *ptgMailParam = new tagMailParam;			
	pMsg->GetStr(strTemp,256);
	ptgMailParam->strWriter = strTemp;
	pMsg->GetGUID(ptgMailParam->WriterGuid);
	pMsg->GetStr(strTemp,512);
	ptgMailParam->strSubject = strTemp;
	pMsg->GetStr(strTemp,102400);
	ptgMailParam->strText = strTemp;
	ptgMailParam->bRead = pMsg->GetByte();
	ptgMailParam->lType = pMsg->GetLong();
	ptgMailParam->lGold = pMsg->GetLong();
	ptgMailParam->bReject = pMsg->GetByte();
	pMsg->GetGUID(ptgMailParam->WriterGuid);

	long lReceiverNum = pMsg->GetLong();
	for (int i=0; i<lReceiverNum; ++i)
	{
		pMsg->GetStr(strTemp,256);
		ptgMailParam->strReceiverList.push_back(strTemp);
	}

	long lCGoodsNum = pMsg->GetLong();
	for (int i=0; i<lCGoodsNum; ++i)
	{
		listGoods *ptgCGoods = new listGoods;
		pMsg->GetGUID(ptgCGoods->goodsguid);
		ptgCGoods->lNum = pMsg->GetLong();
		ptgCGoods->lContainerID = pMsg->GetLong();
		ptgCGoods->lPos = pMsg->GetLong();
		ptgMailParam->Goodslist.push_back(ptgCGoods);
	}
	//实体物品
	long lGoodNum = pMsg->GetLong();	
	long lPos = 0;
	for (int i=0; i<lGoodNum; ++i)
	{				
		long lSize = pMsg->GetLong();
		BYTE *pGoodData = new BYTE[lSize];					
		pMsg->GetEx(pGoodData,lSize);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(71);
#endif
		CGoods* pGoods = CGoodsFactory::UnserializeGoods(pGoodData,lPos,28);
		if (pGoods)
		{
			ptgMailParam->vMailGoods.push_back(pGoods);
		}

		SAFE_DELETE_ARRAY(pGoodData);
	}
	CMail *pMail = new CMail(ptgMailParam);
	GetGame().GetMailManager()->SendMail(pMail,pMail->GetType());

	SAFE_DELETE(pMail);
}
void DoS2WMailSendReceiver( CMessage *pMsg )// --发信给某个玩家
{
	CGUID MailGuid;
	pMsg->GetGUID(MailGuid);
	long lRet = pMsg ->GetLong();

	CMail *pMail=GetGame().GetMailManager()->GetTempMail(MailGuid);//--获取临时邮件
	if (pMail && lRet==1)
	{
		GetGame().GetMailManager()->SaveAndSendMailToReceiver(pMail);
	}
	else
	{
		//失败
		GetGame().GetMailManager()->DeleteTempMail(MailGuid);				
	}
}
void DoS2WMailReceive( CMessage *pMsg )
{
	CGUID PlayerGuid = NULL_GUID;
	pMsg->GetGUID(PlayerGuid);
	CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
	if (pPlayer)
	{				
		GetGame().GetMailManager()->SendAllMailToGs(pPlayer);
	}
}
void DoS2WMailOperaGoods( CMessage *pMsg )
{
	CGUID MailGuid;
	long lIndex;
	pMsg->GetGUID(MailGuid);
	lIndex = pMsg->GetLong();
	GetGame().GetMailManager()->GetGoodsFromMail(MailGuid,lIndex);
}
void DoS2WMailOperaGold( CMessage *pMsg )
{
	CGUID MailGuid;
	pMsg->GetGUID(MailGuid);
	GetGame().GetMailManager()->GetGoldFromMail(MailGuid);
}
void DoS2WMailPay( CMessage *pMsg )
{
	CGUID MailGuid;
	pMsg->GetGUID(MailGuid);
	GetGame().GetMailManager()->PayMail(MailGuid);
}
void DoS2WMailRead( CMessage *pMsg )
{
	CGUID MailGuid;
	pMsg->GetGUID(MailGuid);
	GetGame().GetMailManager()->ReadMail(MailGuid);
}
void DoS2WMailReject( CMessage *pMsg )
{
	CGUID MailGuid;
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);
	pMsg->GetGUID(MailGuid);
	CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
	GetGame().GetMailManager()->RejectMail(pPlayer,MailGuid);
}
//void DoS2WMailPresentReceiver( CMessage *pMsg )
//{
//	CGUID MailGuid;
//	pMsg->GetGUID(MailGuid);
//	long lRet = pMsg ->GetLong();
//
//	CMail *pMail=GetGame().GetMailManager()->GetTempMail(MailGuid);
//
//	if (pMail && lRet==1)
//	{	
//		//实体物品
//		long lGoodNum = pMsg->GetLong();	
//		long lPos = 0;
//		for (int i=0; i<lGoodNum; ++i)
//		{				
//			long lSize = pMsg->GetLong();
//			BYTE *pGoodData = new BYTE[lSize];					
//			pMsg->GetEx(pGoodData,lSize);
//#ifdef __MEMORY_LEAK_CHECK__
//			CBaseObject::SetConstructFlag(71);
//#endif
//			CGoods* pGoods = CGoodsFactory::UnserializeGoods(pGoodData,lPos,29);
//			pMail->GetMGoodsContainer().push_back(pGoods);
//			SAFE_DELETE_ARRAY(pGoodData);
//		}
//		GetGame().GetMailManager()->SaveAndSendMailToReceiver(pMail);//--发给DB
//	}
//	else
//	{
//		//失败
//		GetGame().GetMailManager()->DeleteTempMail(MailGuid);				
//	}
//}
//void DoDB2WMailOperationResult( CMessage *pMsg )
//{
//	// 操作类型
//	DB_OPERATION_TYPE opType = (DB_OPERATION_TYPE)pMsg->GetLong();
//	// 操作结果
//	HRESULT opResultType = (HRESULT)pMsg->GetLong();
//	// 数据块长度
//	long lBufSize = pMsg->GetLong();
//
//	if(lBufSize)
//	{
//		BYTE* buf = new BYTE[lBufSize];
//		pMsg->GetEx(buf, lBufSize);
//		long pos = 0;
//		CGUID mailGroupGuid;
//		_GetBufferFromByteArray(buf, pos, mailGroupGuid, lBufSize);
//		CGUID OwnerID;
//		_GetBufferFromByteArray(buf, pos, OwnerID, lBufSize);
//		list<CGUID> lMailGuid;
//		long lMailNum = _GetLongFromByteArray(buf, pos, lBufSize);
//		for (long i=0; i<lMailNum; ++i)
//		{
//			CGUID MailGuid;
//			_GetBufferFromByteArray(buf,pos,MailGuid,lBufSize);
//			lMailGuid.push_back(MailGuid);
//		}
//
//		// 相关处理 
//		switch(opType)
//		{
//		case DB_OPER_DELETE:
//			{
//				if (opResultType==S_OK)
//				{							
//				}
//			}
//			break;
//		}
//		SAFE_DELETE_ARRAY(buf);
//	}
//}