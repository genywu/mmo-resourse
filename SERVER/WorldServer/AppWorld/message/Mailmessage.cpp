
#include "stdafx.h"
#include "..\..\..\..\nets\networld\message.h"
#include "..\nets\Servers.h"
#include "..\goods\cgoods.h"
#include "..\goods\CGoodsFactory.h"
#include "..\..\worldserver\game.h"
#include "..\organizingsystem\OrganizingCtrl.h"
#include "..\player.h"
#include "MsgOutTimeDef.h"
#include "..\dbentity\entityManager.h"
#include "..\dbentity\entitygroup.h"
#include "../GlobalRgnManager.h"
#include "../Player.h"
#include "../public/AutoSupplyData.h"
#include "../public/PlayerSetupOnServer.h"
#include "../public/MailDef.h"




void OnMailMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_DB2W_MAIL_OPERATION_RESLUT:
		{
			// 操作类型
			DB_OPERATION_TYPE opType = (DB_OPERATION_TYPE)pMsg->GetLong();
			// 操作结果
			HRESULT opResultType = (HRESULT)pMsg->GetLong();
			// 数据块长度
			long lBufSize = pMsg->GetLong();

			if(lBufSize)
			{
				BYTE* buf = (BYTE*)M_ALLOC(lBufSize);
				pMsg->GetEx(buf, lBufSize);
				long pos = 0;
				CGUID mailGroupGuid;
				_GetBufferFromByteArray(buf, pos, mailGroupGuid, lBufSize);
				CGUID OwnerID;
				_GetBufferFromByteArray(buf, pos, OwnerID, lBufSize);
				list<CGUID> lMailGuid;
				long lMailNum = _GetLongFromByteArray(buf, pos, lBufSize);
				for (long i=0; i<lMailNum; ++i)
				{
					CGUID MailGuid;
					_GetBufferFromByteArray(buf,pos,MailGuid,lBufSize);
					lMailGuid.push_back(MailGuid);
				}

				// 相关处理 
				switch(opType)
				{
				case DB_OPER_DELETE:
					{
						if (opResultType==S_OK)
						{							
						}
					}
					break;
				}
				M_FREE(buf,lBufSize);
			}

		}
		break;
	case MSG_S2W_MAIL_DELETE:
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
			GetGame()->GetMailManager()->SendDeleteMailToDB(PlayerGuid, lMailGuid);
		}
		break;
	case MSG_S2W_MAIL_SEND:
		{
			
			char  strTemp[102400];
			tagMailParam *ptgMailParam = OBJ_CREATE(tagMailParam);			
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

			long lRGoodNum = pMsg->GetLong();
			for (int i=0; i<lRGoodNum; ++i)
			{
				tagSGoods *ptgSGoods = OBJ_CREATE(tagSGoods);
				ptgSGoods->lIndex = pMsg->GetLong();
				ptgSGoods->lNum  = pMsg->GetLong();
				ptgMailParam->lSGoods.push_back(ptgSGoods);
			}


			long lCGoodsNum = pMsg->GetLong();
			for (int i=0; i<lCGoodsNum; ++i)
			{
				tagCGoods *ptgCGoods = OBJ_CREATE(tagCGoods);
				pMsg->GetGUID(ptgCGoods->goodsguid);
				ptgCGoods->lNum = pMsg->GetLong();
				ptgCGoods->lContainerID = pMsg->GetLong();
				ptgCGoods->lPos = pMsg->GetLong();
				ptgMailParam->lCGoods.push_back(ptgCGoods);
			}

			//实体物品
			long lGoodNum = pMsg->GetLong();	
			long lPos = 0;
			for (int i=0; i<lGoodNum; ++i)
			{				
				long lSize = pMsg->GetLong();
				BYTE *pGoodData = (BYTE*)M_ALLOC(lSize);				
				pMsg->GetEx(pGoodData,lSize);
#ifdef __MEMORY_LEAK_CHECK__
				CBaseObject::SetConstructFlag(71);
#endif
				CGoods* pGoods = CGoodsFactory::UnserializeGoods(pGoodData,lPos,28);
				if (pGoods)
				{
					ptgMailParam->vMailGoods.push_back(pGoods);
				}
				
				M_FREE(pGoodData,lSize);
			}

			CMail *pMail = MP_NEW CMail(ptgMailParam);
			GetGame()->GetMailManager()->SendMail(pMail);
			MP_DELETE(pMail);
		}
		break;

	case MSG_S2W_MAIL_SEND_RECEIVER:
		{
			CGUID MailGuid;
			pMsg->GetGUID(MailGuid);
			long lRet = pMsg ->GetLong();

			CMail *pMail=GetGame()->GetMailManager()->GetTempMail(MailGuid);
			if (pMail && lRet==1)
			{
				GetGame()->GetMailManager()->SaveAndSendMailToReceiver(pMail);
			}
			else
			{
				//失败
				GetGame()->GetMailManager()->DeleteTempMail(MailGuid);				
			}
		}
		break;
	case	MSG_S2W_MAIL_PRESENT_RECEIVER:
		{
			CGUID MailGuid;
			pMsg->GetGUID(MailGuid);
			long lRet = pMsg ->GetLong();

			CMail *pMail=GetGame()->GetMailManager()->GetTempMail(MailGuid);

			if (pMail && lRet==1)
			{	
				//实体物品
			long lGoodNum = pMsg->GetLong();	
			long lPos = 0;
			for (int i=0; i<lGoodNum; ++i)
			{				
				long lSize = pMsg->GetLong();
				BYTE *pGoodData = (BYTE*)M_ALLOC(lSize);				
				pMsg->GetEx(pGoodData,lSize);
#ifdef __MEMORY_LEAK_CHECK__
				CBaseObject::SetConstructFlag(71);
#endif
				CGoods* pGoods = CGoodsFactory::UnserializeGoods(pGoodData,lPos,29);
				pMail->GetMGoodsContainer().push_back(pGoods);
				M_FREE(pGoodData,lSize);
			}
				GetGame()->GetMailManager()->SaveAndSendMailToReceiver(pMail);
			}
			else
			{
				//失败
				GetGame()->GetMailManager()->DeleteTempMail(MailGuid);				
			}
		}
		break;

	case MSG_S2W_MAIL_RECEIVE:
		{
			CGUID PlayerGuid = NULL_GUID;
			pMsg->GetGUID(PlayerGuid);
			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);

			if (pPlayer)
			{				
				//
				GetGame()->GetMailManager()->SendAllMailToGs(pPlayer);
			}
		}
		break;
	case MSG_S2W_MAIL_OPERA_GOODS:
		{
			CGUID MailGuid;
			long lIndex;
			pMsg->GetGUID(MailGuid);
			lIndex = pMsg->GetLong();
			GetGame()->GetMailManager()->GetGoodsFromMail(MailGuid,lIndex);
		}
		break;
	case MSG_S2W_MAIL_OPERA_GOLD:
		{
			CGUID MailGuid;
			pMsg->GetGUID(MailGuid);
			GetGame()->GetMailManager()->GetGoldFromMail(MailGuid);

		}
		break;
	case MSG_S2W_MAIL_PAY:
		{
			CGUID MailGuid;
			pMsg->GetGUID(MailGuid);
			GetGame()->GetMailManager()->PayMail(MailGuid);

		}
		break;
	case MSG_S2W_MAIL_READ:
		{
			CGUID MailGuid;
			pMsg->GetGUID(MailGuid);
			GetGame()->GetMailManager()->ReadMail(MailGuid);
		}
		break;
	case MSG_S2W_MAIL_REJECT:
		{
			CGUID MailGuid;
			CGUID PlayerGuid;

			pMsg->GetGUID(PlayerGuid);
			pMsg->GetGUID(MailGuid);
			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			GetGame()->GetMailManager()->RejectMail(pPlayer,MailGuid);
		}
		break;	
	}
}