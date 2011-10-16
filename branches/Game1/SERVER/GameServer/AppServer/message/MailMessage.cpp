#include "stdafx.h"
#include "..\Player.h"
#include "..\Monster.h"
#include "..\Mail\Mail.h"
#include "..\nets\netserver\message.h"
#include "../ServerRegion.h"

enum eMAXGOODSNUM
{
	MAXGOODNUM = 5,
};

void OnMailMessageForClient(CMessage *pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_C2S_MAIL_SEND:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if (NULL == pPlayer)
			{
				return;
			}
			//玩家在交易或摆摊中不能交易
			if (pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL 
				|| pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_TRADING )
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

			if (pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)
			{


				tagMailParam *pMailParam = OBJ_CREATE(tagMailParam);

				long lReceiver	= pMsg->GetLong();						//收信人个数

				if (lReceiver>100)
				{
					char str[1024]="";
					char pszGuid[50]="";
					pPlayer->GetExID().tostring(pszGuid);
					_snprintf(str,1024,"%s(ID:%s) 玩家群发邮件的收信人的数量%d超过限制",pPlayer->GetName(),pszGuid,lReceiver);
					PutDebugString(str);				

					OBJ_RELEASE(tagMailParam, pMailParam);
					return;
				}

				for (int i=0; i<lReceiver; ++i)
				{
					char strReceiver[128];
					pMsg->GetStr(strReceiver,CGlobeSetup::GetSetup()->lMaxName+1);

					if (lstrlen(strReceiver)>=CGlobeSetup::GetSetup()->lMaxName)
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
				pMsg->GetStr(strTemp,CGlobeSetup::GetSetup()->lMaxSubject+1);								//主题
				if (lstrlen(strTemp)>=CGlobeSetup::GetSetup()->lMaxSubject)
				{
					GameManager::GetInstance()->GetMailManager()->SendMailFailedToClient(pPlayer->GetExID(),MAIL_SENDING_FAILED_INVALID_MAXSUBJECT);
					return;
				}
				pMailParam->strSubject	= strTemp;


				pMsg->GetStr(strTemp,CGlobeSetup::GetSetup()->lMaxText+1);							//内容
				if (lstrlen(strTemp)>=CGlobeSetup::GetSetup()->lMaxText)
				{
					GameManager::GetInstance()->GetMailManager()->SendMailFailedToClient(pPlayer->GetExID(),MAIL_SENDING_FAILED_INVALID_MAXTEXT);
					return;
				}

				pMailParam->strText			= strTemp;

				pMailParam->lGold			= pMsg->GetLong();			//金币
				pMailParam->lType			= pMsg->GetByte();			//类型


				//普通物品
				long lCGoodNum	= pMsg->GetLong();	

				if (lCGoodNum>MAXGOODNUM)
				{
					char str[1024]="";
					char pszGuid[50]="";
					pPlayer->GetExID().tostring(pszGuid);
					_snprintf(str,1024,"%s(ID:%s) 玩家发送物品的数量:%d超过限制(此玩家利用非常规方式发送消息)",pPlayer->GetName(),pszGuid,lCGoodNum);
					PutDebugString(str);

					OBJ_RELEASE(tagMailParam, pMailParam);
					return;
				}


		
				for (int i=0; i<lCGoodNum; ++i)
				{		
					tagCGoods *ptgGoods = OBJ_CREATE(tagCGoods);
					pMsg->GetGUID(ptgGoods->goodsguid);
					ptgGoods->lNum = pMsg->GetLong();
					ptgGoods->lContainerID = pMsg->GetDWord();
					ptgGoods->lPos	= pMsg->GetDWord();
					pMailParam->lCGoods.push_back(ptgGoods);
				}
				//}
				pMailParam->strWriter = pPlayer->GetName();
				pMailParam->WriterGuid = pPlayer->GetExID();
				pMailParam->bReject = 1;




				//! 发信			
				CMail *pMail = MP_NEW CMail(pMailParam);

				if (pMail->GetType()==SYSTEM_MAIL || pMail->GetGold()<0 || pMail->GetType()>=SENDGOLD_MAIL)
				{
					pMail->DeleteMailGoods();
					MP_DELETE(pMail);
					return;
				}			



				GameManager::GetInstance()->GetMailManager()->AddMail(pMail,pPlayer);
			}

		}	
		break;
	case MSG_C2S_MAIL_RECEIVER:
		{
			
		}
		break;
	case MSG_C2S_MAIL_GET_GOODS:
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

			if (pPlayer &&pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)
			{
				CGUID MailGuid;
				pMsg->GetGUID(MailGuid);
				long lIndex = pMsg->GetLong();			
				
				GameManager::GetInstance()->GetMailManager()->GetGoodsFromMail(MailGuid,pPlayer,lIndex);		
				
			}

		}
		break;
	case MSG_C2S_MAIL_READ:
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

			if (pPlayer &&  pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)
			{
				CGUID guid;
				pMsg->GetGUID(guid);
			
				GameManager::GetInstance()->GetMailManager()->ReadMail(guid,pPlayer);
				
			}
		}
		break;
	case MSG_C2S_MAIL_DELETE:
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

			if (pPlayer && pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)
			{
				long lNum = pMsg->GetWord();

				if (lNum<0 || NULL==pPlayer)
				{
					return;
				}	

				for (int i=0; i<lNum; ++i)
				{
					CGUID guid;
					pMsg->GetGUID(guid);
					lGuid.push_back(guid);
				}

				if (GameManager::GetInstance()->GetMailManager()->CheckDeleteMail(lGuid,pPlayer))
				{
					GameManager::GetInstance()->GetMailManager()->DeleteMailToWS(lGuid,pPlayer);			
				}
			}

		}
		break;
	case MSG_C2S_MAIL_REJECT:
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

			if (pPlayer && pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)
			{

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
			}

		}
		break;
	case MSG_C2S_MAIL_AGREED_PAY:
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

			if (pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)
			{			
				CGUID MailGuid;
				pMsg->GetGUID(MailGuid);
				CMail *pMail = GameManager::GetInstance()->GetMailManager()->GetMail(MailGuid);
				if (pMail && pMail->GetReceiverGuid()==pPlayer->GetExID() && pMail->GetType()==PAYGOLD_MAIL)
				{
					GameManager::GetInstance()->GetMailManager()->PayMoneyMail(pMail,pPlayer);
				}
			}
		}
		break;
	case MSG_C2S_MAIL_AGREED_PRESENT:
		{
			
		}
		break;
	case MSG_C2S_MAIL_GET_GOLDS:
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

			if (pPlayer && pMailNpc && strcmp(pMailNpc->GetOriginName(),"10000Email")==0  && pMailNpc->Distance( pPlayer ) < 8)
			{
				CGUID MailGuid;
				pMsg->GetGUID(MailGuid);
				CPlayer *pPlayer = pMsg->GetPlayer();
				CMail *pMail = GameManager::GetInstance()->GetMailManager()->GetMail(MailGuid);
				if (NULL!=pPlayer && pMail && pMail->GetReceiverGuid()==pPlayer->GetExID())
				{
					GameManager::GetInstance()->GetMailManager()->SendGetGoldFromMailToWS(MailGuid,pPlayer);
				}
			}

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
			//! 发信失败
			CGUID PlayerGuid = NULL_GUID;
			long lRet = pMsg->GetLong();
			pMsg->GetGUID(PlayerGuid);
			GameManager::GetInstance()->GetMailManager()->SendMailFailedToClient(PlayerGuid,lRet);
		}
		break;
	case MSG_W2S_MAIL_OPERA_GOLD:
		{
			//金钱操作
			CGUID MailGuid;
			CGUID PlayerGuid;
			pMsg->GetGUID(MailGuid);
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);
			GameManager::GetInstance()->GetMailManager()->GetGoldFromMail(MailGuid,pPlayer);					
		}
		break;
	case MSG_W2S_MAIL_OPERA_GOODS:
		{
			CGUID MailGuid;
			CGUID PlayerGuid;
			long lIndex = pMsg->GetLong();
			pMsg->GetGUID(MailGuid);
			pMsg->GetGUID(PlayerGuid);
			GameManager::GetInstance()->GetMailManager()->SendGetGoodsGSToClient(MailGuid,PlayerGuid,lIndex);
		}
		break;
	case MSG_W2S_MAIL_SEND:
		{
			//! 发信
			BYTE *ByteData = NULL;	
			CGUID MailGuid;

			pMsg->GetGUID(MailGuid);

			long lByteDataLen = pMsg->GetLong();
			ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);	
			pMsg->GetEx(ByteData,lByteDataLen);
			long Pos = 0;
			tagMailParam *pMailParam = OBJ_CREATE(tagMailParam);

			GameManager::GetInstance()->GetMailManager()->DecordFromByteArray(ByteData,Pos,pMailParam);
			CMail *pMail = MP_NEW CMail(pMailParam);
			GameManager::GetInstance()->GetMailManager()->SendMailToClient(MailGuid,pMail);		
			M_FREE(ByteData, sizeof(BYTE)*lByteDataLen);
		}
		break;
	case MSG_W2S_MAIL_SEND_RESULT:
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
				CMail *pMail = MP_NEW CMail(pMailParam);
				pPlayer = GetGame()->FindPlayer(pMail->GetWriter().c_str());
			
				if (pPlayer)
				{
					int nRet = GameManager::GetInstance()->GetMailManager()->ReMoveGoldAndGoods(pMail,pPlayer);
					GameManager::GetInstance()->GetMailManager()->SendMailToReceiver(pMail->GetExID(),nRet);
				}

				MP_DELETE(pMail);			
				M_FREE(ByteData, sizeof(BYTE)*lByteDataLen);				
			}
			else
			{
				GameManager::GetInstance()->GetMailManager()->SendMailFailedToClient(WriterGuid,MAIL_SENDING_FAILED_INVALID_RECEIVER);
			}
		}
		break;

	case MSG_W2S_MAIL_DELETE:
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
	case MSG_W2S_MAIL_SEND_SYS:
		{
			//系统群发邮件
			long lSysMailID = pMsg->GetLong();
			GameManager::GetInstance()->GetMailManager()->SendSysMailMsgToReceiver(lSysMailID);
		}
		break;


	case MSG_W2S_MAIL_RECEIVE:
		{
			//! 收信
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

					CMail *pMail = MP_NEW CMail(pMailParam);

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
				for (list<CMail*>::iterator it=lMailList.begin();it!=lMailList.end();++it)
				{
					pPlayer->SetReceiveMail(true);	
					pPlayer->AddReceiveMailList((*it)->GetExID());
					(*it)->AddToByteArrayForClient(vectorByte,pPlayer);

					long lGoodsNum = 0;
					for (vector<CGoods*>::iterator mit=(*it)->GetMGoodsContainer().begin(); mit!=(*it)->GetMGoodsContainer().end(); ++mit)
					{
						lGoodsNum += (*mit)->GetAmount();
					}

					GetGameLogInterface()->logT600_Receive_mail_log(pPlayer,(*it)->GetExID(),(*it)->GetWriter().c_str(),(*it)->GetType(),(*it)->GetGold(),lGoodsNum,(*it)->GetSGood().size(),1);

				}

				GameManager::GetInstance()->GetMailManager()->SendAllMailToClient(vectorByte, pPlayer);
			}
			M_FREE(ByteData, lByteDataLen);
		}
		break;
	}
}


/*
<!--收取邮件日志-->
<Table name="t600_receive_mail_log" log_no="600" log_pos="1">
<item type="client_head"     head_name=""></item>    
<item type="player"   head_name=""></item>
<item type="alone"    Item_name="mail_guid"            data_type="guid"></item><!--邮件guid-->
<item type="alone"	  Item_name="writer"			   data_type="str128"></item><!--发信人-->
<item type="alone"	  Item_name="type"			   	   data_type="int32"></item><!--邮件类型-->
<item type="alone"	  Item_name="gold"			   	   data_type="int32"></item><!--金钱--> 
<item type="alone"	  Item_name="goodsnum"			   data_type="int32"></item><!--物品数量-->
<item type="alone"	  Item_name="incgoodsnum"		   data_type="int32"></item><!--增值物品数量-->    
<item type="alone"    Item_name="flag"          	   data_type="int32"></item><!--操作类型：1=上线收取、2=在线接收-->
</Table>
*/
bool LogicLogInterface::logT600_Receive_mail_log(CPlayer *pPlayer,const CGUID &MailGuid,const char *strWriter,int nType,long lGold,long lGoodsNum,long lIncGoodsNum,int nFlag)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;
	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(600,strTime.c_str(),&(pPlayer->GetExID()),pPlayer->GetName(),&MailGuid,strWriter,nType,lGold,lGoodsNum,lIncGoodsNum,nFlag);
}
/*
<Table name="t601_send_mail_log" log_no="601" log_pos="1">
<item type="client_head"     head_name=""></item>    
<item type="alone"    Item_name="mail_guid"            data_type="guid"></item><!--邮件guid-->
<item type="alone"	  Item_name="receiver"			   data_type="str128"></item><!--收信人-->
<item type="alone"	  Item_name="writer"			   data_type="str128"></item><!--发信人-->
<item type="alone"	  Item_name="type"			   	   data_type="int32"></item><!--邮件类型-->
<item type="alone"	  Item_name="gold"			   	   data_type="int32"></item><!--金钱--> 
<item type="alone"	  Item_name="goodsnum"			   data_type="int32"></item><!--物品数量-->
<item type="alone"	  Item_name="incgoodsnum"		   data_type="int32"></item><!--增值物品数量--> 
<item type="alone"	  Item_name="goodsinfo"				   data_type="str512"></item><!--物品-->    
</Table>
*/
bool LogicLogInterface::logT601_Send_mail_log(const CGUID &MailGuid,const char *strReceiver,const char *strWriter,int nType,long lGold,long lGoodsNum,long lIncGoodsNum,const char *strGoodsInfo)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;

	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(601,strTime.c_str(), &MailGuid,strReceiver, strWriter,nType,lGold,lGoodsNum,lIncGoodsNum,strGoodsInfo);
}

/*<!--从邮件中取物品日志-->
<Table name="t602_getgoods_mail_log" log_no="602" log_pos="1">
<item type="client_head"     head_name=""></item>    
<item type="alone"    Item_name="mail_guid"            data_type="guid"></item><!--邮件guid-->
<item type="alone"	  Item_name="receiver"			   data_type="str128"></item><!--收信人-->
<item type="alone"	  Item_name="writer"			   data_type="str128"></item><!--发信人-->
<item type="goods"    head_name=""></item>
<item type="alone"	  Item_name="type"			   	   data_type="int32"></item><!--邮件类型-->
<item type="alone"	  Item_name="gold"			   	   data_type="int32"></item><!--金钱--> 
<item type="alone"	  Item_name="goodsnum"			   data_type="int32"></item><!--物品数量-->
<item type="alone"	  Item_name="incgoodsnum"		   data_type="int32"></item><!--增值物品数量-->    
</Table>
*/
bool LogicLogInterface::logT602_GetGoods_mail_log(const CGUID &MailGuid,const char *strReceiver,const char *strWriter,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,int nType,long lGold,long lGoodsNum,long lIncGoodsNum)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;

	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(602,strTime.c_str(), &MailGuid,strReceiver, strWriter,dwGoodsId,&goodsGuid,strGoodsName,nType,lGold,lGoodsNum,lIncGoodsNum);
}
/*
<!--从邮件中取金币日志-->
<Table name="t603_getgold_mail_log" log_no="603" log_pos="1">
<item type="client_head"     head_name=""></item>    
<item type="alone"    Item_name="mail_guid"            data_type="guid"></item><!--邮件guid-->
<item type="alone"	  Item_name="receiver"			   data_type="str128"></item><!--收信人-->
<item type="alone"	  Item_name="writer"			   data_type="str128"></item><!--发信人-->    
<item type="alone"	  Item_name="type"			   	   data_type="int32"></item><!--邮件类型-->
<item type="alone"	  Item_name="gold"			   	   data_type="int32"></item><!--金钱--> 
<item type="alone"	  Item_name="goodsnum"			   data_type="int32"></item><!--物品数量-->
<item type="alone"	  Item_name="incgoodsnum"		   data_type="int32"></item><!--增值物品数量-->    
</Table>
*/
bool LogicLogInterface::logT603_GetGold_mail_log(const CGUID &MailGuid,const char *strReceiver,const char *strWriter,int nType,long lGold,long lGoodsNum,long lIncGoodsNum)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;

	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(603,strTime.c_str(), &MailGuid,strReceiver, strWriter,nType,lGold,lGoodsNum,lIncGoodsNum);
}
/*
<!--读取邮件日志-->
<Table name="t604_read_mail_log" log_no="604" log_pos="1">
<item type="client_head"     head_name=""></item>    
<item type="player"   head_name=""></item>
<item type="alone"    Item_name="mail_guid"            data_type="guid"></item><!--邮件guid-->		
<item type="alone"	  Item_name="type"			   	   data_type="int32"></item><!--邮件类型-->
<item type="alone"	  Item_name="gold"			   	   data_type="int32"></item><!--金钱--> 
<item type="alone"	  Item_name="goodsnum"			   data_type="int32"></item><!--物品数量-->
<item type="alone"	  Item_name="incgoodsnum"		   data_type="int32"></item><!--增值物品数量-->    
<item type="alone"    Item_name="flag"          	   data_type="int32"></item><!--操作类型：1=成功、0=失败-->
</Table>
*/
bool LogicLogInterface::logT604_read_mail_log(CPlayer *pPlayer,const CGUID &MailGuid,int nType,long lGold,long lGoodsNum,long lIncGoodsNum,int nFlag)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;

	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(604,strTime.c_str(),&(pPlayer->GetExID()),pPlayer->GetName(), &MailGuid,nType,lGold,lGoodsNum,lIncGoodsNum,nFlag);
}

/*
<!--删除邮件日志-->
<Table name="t605_delete_mail_log" log_no="605" log_pos="1">
<item type="client_head"     head_name=""></item> 
<item type="alone"    Item_name="mail_guid"            data_type="guid"></item><!--邮件guid-->   
<item type="alone"	  Item_name="receiver"			   data_type="str128"></item><!--收信人-->
<item type="alone"	  Item_name="writer"			   data_type="str128"></item><!--发信人-->    
<item type="alone"	  Item_name="type"			   	   data_type="int32"></item><!--邮件类型-->
<item type="alone"	  Item_name="gold"			   	   data_type="int32"></item><!--金钱--> 
<item type="alone"	  Item_name="goodsnum"			   data_type="int32"></item><!--物品数量-->
<item type="alone"	  Item_name="incgoodsnum"		   data_type="int32"></item><!--增值物品数量-->    
</Table>*/
bool LogicLogInterface::logT605_delete_mail_log(const CGUID &MailGuid,const char *strReceiver,const char *strWriter,int nType,long lGold,long lGoodsNum,long lIncGoodsNum)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;

	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(605,strTime.c_str(), &MailGuid,strReceiver, strWriter,nType,lGold,lGoodsNum,lIncGoodsNum);
}
