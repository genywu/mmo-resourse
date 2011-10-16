#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\nets\Servers.h"
#include "..\player.h"
#include "..\goods\CGoods.h"
#include "..\goods\CGoodsFactory.h"


extern bool IncShopProcessing( CPlayer* pPlayer, DWORD dwGoodsIndex, DWORD dwNum, DWORD dwTradeMode, long lTotalAmount);
extern bool IncPrsentProcessing(CPlayer* pPlayer, vector<tagSGoods>, DWORD dwTradeMode, long &lTotalAmount , CGUID &MailGuid);
extern void SDOSendResponse(int nMsgType,int nMsgForm,int nMsgSeq, list<tagValue>);

enum eMax
{
	MAXSTRING = 512,
};

// 响应LOG消息
void OnBillingMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_B2S_GET_BALANCE:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			DWORD dwBalance=pMsg->GetDWord();
			CPlayer* pPlayer=GetGame()->FindPlayer(guid);
			if(pPlayer)
			{
				pPlayer->SetYuanbao(dwBalance);				
			}
		}
		break;
	case MSG_B2S_ACCOUNT_LOCK:
		{
			long lRet=pMsg->GetLong();
			BYTE btConsumeType=pMsg->GetByte();
			CGUID operGuid;
			pMsg->GetGUID(operGuid);
			CGUID guid;
			pMsg->GetGUID(guid);
			CGUID attachGuid;
			pMsg->GetGUID(attachGuid);
			
			char strCdkey[256];			
			pMsg->GetStr(strCdkey,256);
			DWORD dwNum=pMsg->GetDWord();
			if(dwNum<1)
				return;
			vector<tagSGoods> vecGoodsData;
			for(int i=0;i<dwNum;i++)
			{
				tagSGoods tgSGoods;
				tgSGoods.lIndex = pMsg->GetDWord();
				tgSGoods.lNum = pMsg->GetDWord();
				vecGoodsData.push_back(tgSGoods);
			}
			
			long lLockNum=pMsg->GetLong();
			DWORD dwTradeMode = pMsg->GetDWord();
			switch(lRet)
			{
			case ACCOUNT_LOCK_OK:
				{
					//预冻成功 发放道具					
					CPlayer* pPlayer=GetGame()->FindPlayer(guid);
					if(pPlayer)
					{
						if(btConsumeType==eICT_NORMAL_BUY_ITEM)
						{
							if(dwNum>1)
								return;
							vector<tagSGoods>::iterator it=vecGoodsData.begin();

							CIncrementShopList::MAP_INCGOODS* pIncGoodsList = CIncrementShopList::GetIncGoodsList();
							if ( !pIncGoodsList ) return ;
							CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties((*it).lIndex);
							if ( !pProperties ) return ;
							CIncrementShopList::ITR_INCGOODS itIncGoodsList = pIncGoodsList->find( dwTradeMode );
							CIncrementShopList::MAP_INCSHOP& mapIncGoodsList = itIncGoodsList->second;
							CIncrementShopList::ITR_INCSHOP itInc = mapIncGoodsList.find((*it).lIndex);

							DWORD dwBuyNum = (*it).lNum*itInc->second.dwGoodsNum;
							bool bSucceed=IncShopProcessing(pPlayer,(*it).lIndex,dwBuyNum,dwTradeMode,lLockNum);
							if(bSucceed)
							{
								

								pPlayer->SetYuanbao(pPlayer->GetYuanbao()-lLockNum);

								CMessage msg(MSG_S2B_ACCOUNT_UNLOCK);
								msg.Add(strCdkey);//cd key
								msg.Add(operGuid);//oper guid
								msg.Add(pPlayer->GetName());//player name
								msg.Add(pPlayer->GetExID());//player guid			
								
								msg.Add((DWORD)1);
								msg.Add((*it).lIndex);//goods id
								msg.Add((*it).lNum);
								msg.Add(lLockNum);//lock num
								msg.Add(GetGame()->GetWSNumber());
								string strGoodsName="";
								if(CGoodsFactory::QueryGoodsBaseProperties((*it).lIndex))
								{
									strGoodsName=CGoodsFactory::QueryGoodsName((*it).lIndex);
								}								
								msg.Add(strGoodsName.c_str());
								msg.SendToBS();
							}	
							else
							{
								//空间不够
								char* strRes=CStringReading::LoadString(22,3);
								if(strRes)
									pPlayer->SendNotifyMessage(strRes);
								//预冻回滚
								CMessage msg(MSG_S2B_ACCOUNT_LOCK_ROLLBACK);
								msg.Add(strCdkey);
								msg.Add(lLockNum);
								msg.Add(operGuid);
								msg.SendToBS();

							}
						}
						else if(btConsumeType==eICT_MAIL_BUY_ITEM)
						{
						}
						else if(btConsumeType==eICT_USE_FUNCTION)
						{
							//使用增值功能
							if(vecGoodsData.size()==1)
							{
								switch(vecGoodsData[0].lIndex)
								{
								case eIFUNC_RELIVE:
									{
										//原地复活
										pPlayer->SetYuanbao(pPlayer->GetYuanbao()-lLockNum);
										pPlayer->OnRelive(1);
										string str=CGlobeSetup::GetSetup()->szReliveGoodsName;
										DWORD dwIndex = CGoodsFactory::QueryGoodsIDByOriginalName(str.c_str());
										CMessage msg(MSG_S2C_PLAYER_RELIVE_RESULT);
										msg.Add(BYTE(1));//成功
										msg.Add(dwIndex);
										msg.Add(pPlayer->GetExID());

										CMessage msg2BS(MSG_S2B_ACCOUNT_UNLOCK);
										msg2BS.Add(strCdkey);//cd key
										msg2BS.Add(operGuid);//oper guid
										msg2BS.Add(pPlayer->GetName());//player name
										msg2BS.Add(pPlayer->GetExID());//player guid			

										msg2BS.Add((DWORD)1);
										msg2BS.Add(vecGoodsData[0].lIndex);//goods id
										msg2BS.Add(vecGoodsData[0].lNum);
										msg2BS.Add(lLockNum);//lock num
										msg2BS.Add(GetGame()->GetWSNumber());
										string strGoodsName="FUNC_RELIVE";																	
										msg2BS.Add(strGoodsName.c_str());
										msg2BS.SendToBS();
									}
									break;
								default:
									break;
								}
							}
						}
					}
				}
				break;
			case ACCOUNT_LOCK_ERROR_CDKEY:
				{
				}
				break;
			case ACCOUNT_LOCK_NOT_ENOUGH:
				{
					//账户余额不足
					CPlayer* pPlayer=GetGame()->FindPlayer(guid);
					if(pPlayer)
					{
						char* strRes=CStringReading::LoadString(22,22);
						if(strRes)
							pPlayer->SendNotifyMessage(strRes);
					}					
				}
				break;
			case ACCOUNT_LOCK_UNKNOWN_ERROR:
				{
				}
				break;
			}
		}
		break;
	case MSG_B2S_ACCOUNT_UPDATED:
		{
		}
		break;
	case MSG_B2S_SDO_ITEM_SEND_REQUEST:
		{
			static int nCallTimes=0;

			char strTemp[MAXSTRING];
			char strTemp1[MAXSTRING];
			char strTemp2[MAXSTRING];
			char strTemp3[MAXSTRING];
			char strTemp4[MAXSTRING];
			char strTemp5[MAXSTRING];
			char strTemp6[MAXSTRING];
			char strTemp7[MAXSTRING];
			char strTemp8[MAXSTRING];
			char strTemp9[MAXSTRING];
			pMsg->GetStr(strTemp,MAXSTRING);
			const char *payErrorMsg = strTemp;

			pMsg->GetStr(strTemp1,MAXSTRING);
			const char *szCallTime = strTemp1;

			pMsg->GetStr(strTemp2,MAXSTRING);
			const char *szGoodsInfoEx = strTemp2;

			pMsg->GetStr(strTemp3,MAXSTRING);
			const char *szGoodsNum = strTemp3;

			pMsg->GetStr(strTemp4,MAXSTRING);
			const char *szIndication = strTemp4;

			pMsg->GetStr(strTemp5,MAXSTRING);
			const char *szOrderId = strTemp5;

			pMsg->GetStr(strTemp6,MAXSTRING);
			const char *szPayStatus = strTemp6;

			pMsg->GetStr(strTemp7,MAXSTRING);
			const char *szRoleName = strTemp7;

			pMsg->GetStr(strTemp8,MAXSTRING);
			const char *szSendTimes = strTemp8;

			pMsg->GetStr(strTemp9,MAXSTRING);
			const char *szUserId = strTemp9;
			int nMsgType = pMsg->GetWord();
			int nMsgForm = pMsg->GetWord();
			int nMsgSeq	 = pMsg->GetWord();

			if (szOrderId)
			{
				if(CGoodsFactory::IsOrderExist(szOrderId))
					return;
			}
			

#ifdef _ASCYN_DEBUG
			char szOutPut[512];
			_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg) msgtype:%d;msgform:%d,msgseq:%d,szOrderId:%s,szPayStatus:%s",nMsgType,nMsgForm,nMsgSeq,szOrderId,szPayStatus);
			PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif

			int nErrorCode=atoi(szPayStatus);

#ifdef _ASCYN_DEBUG
			_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg)szUserId:%s szGoodsNum:%s szGoodsInfoEx:%s",szUserId,szGoodsNum,szGoodsInfoEx);
			PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
			if(szUserId && szGoodsNum && szGoodsInfoEx)
			{
#ifdef _ASCYN_DEBUG
				char szOutPut[512];
				_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg)if(szUserId && szGoodsNum && szGoodsInfoEx)");
				PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
				nCallTimes++;
				char descMsg[64],appendix[256];
				_snprintf(descMsg, 64, "descMsg-%d",nCallTimes);
				_snprintf(appendix, 256, "%s",szIndication);

				if(nErrorCode!=0)
				{
#ifdef _ASCYN_DEBUG
					char szOutPut[512];
					_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg)第1返回,%d",nErrorCode);
					PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
					CGoodsFactory::ErasePendingOrder(szOrderId);
					list<tagValue> lstValue;
					tagValue stValue;
					stValue.strName = "result";
					stValue.strValue = "2";
					lstValue.push_back(stValue);

					stValue.strName = "orderId";
					stValue.strValue = szOrderId;
					lstValue.push_back(stValue);

					
				
					stValue.strName = "descMsg";
					stValue.strValue = descMsg;
					lstValue.push_back(stValue);				

					stValue.strName = "appendix";
					stValue.strValue = appendix;
					lstValue.push_back(stValue);
					SDOSendResponse(nMsgType,nMsgForm,nMsgSeq, lstValue);
					
					return;
				}

				CPlayer* pPlayer=NULL;
				CGUID& playerGuid=CGoodsFactory::GetGUIDByOrderId(szOrderId);
#ifdef _ASCYN_DEBUG
				char szOutPut1[256];
				playerGuid.tostring(szOutPut1);
				_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg),playerGuid:%s",szOutPut1);
				PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
				if(playerGuid==CGUID::GUID_INVALID)
				{
#ifdef _ASCYN_DEBUG
					char szOutPut[512];
					_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg),if(playerGuid==CGUID::GUID_INVALID)");
					PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif

					list<tagValue> lstValue;
					tagValue stValue;
					stValue.strName = "result";
					stValue.strValue = "-10227000";
					lstValue.push_back(stValue);

					stValue.strName = "orderId";
					stValue.strValue = szOrderId;
					lstValue.push_back(stValue);



					stValue.strName = "descMsg";
					stValue.strValue = descMsg;
					lstValue.push_back(stValue);				

					stValue.strName = "appendix";
					stValue.strValue = appendix;
					lstValue.push_back(stValue);
					SDOSendResponse(nMsgType,nMsgForm,nMsgSeq, lstValue);
#ifdef _ASCYN_DEBUG
					_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg)第2返回");
					PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
					return;
				}
				else
				{
#ifdef _ASCYN_DEBUG
					char szOutPut[512];
					_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg),pPlayer=GetGame()->FindPlayer(playerGuid);");
					PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
					pPlayer=GetGame()->FindPlayer(playerGuid);

					if(!pPlayer) //找不到该玩家
					{	
#ifdef _ASCYN_DEBUG
						char szOutPut[512];
						_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg),if(!pPlayer) //找不到该玩家");
						PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
						list<tagValue> lstValue;
						tagValue stValue;
						stValue.strName = "result";
						stValue.strValue = "-10227000";
							lstValue.push_back(stValue);

						stValue.strName = "orderId";
						stValue.strValue = szOrderId;
						lstValue.push_back(stValue);

						stValue.strName = "descMsg";
						stValue.strValue = descMsg;
						lstValue.push_back(stValue);				

						stValue.strName = "appendix";
						stValue.strValue = appendix;
						lstValue.push_back(stValue);
						SDOSendResponse(nMsgType,nMsgForm,nMsgSeq, lstValue);
#ifdef _ASCYN_DEBUG
						_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg)第3返回");
						PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
						return;
					}
				}				

#ifdef _ASCYN_DEBUG
				_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg),//检查nGoodsNum是否合法");
				PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
				//检查nGoodsNum是否合法
				int nGoodsNum=atoi(szGoodsNum);
				if(nGoodsNum!=1)
					return;

#ifdef _ASCYN_DEBUG
				_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg),//检查goodsInfoEx的格式");
				PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
				//检查goodsInfoEx的格式
				string strGoodsInfoEx=szGoodsInfoEx;
				vector<string> goodsInfoArgs;
				size_t start=0,pos=0;

				do 
				{
					pos=strGoodsInfoEx.find_first_of(',',start);
					goodsInfoArgs.push_back(strGoodsInfoEx.substr(start,pos-start));
					start=pos+1;
				} 
				while(pos!=string::npos);
				if(goodsInfoArgs.size()!=4)
				{
					//AddLogText("goodsInfoEx格式错误");
					return;
				}

				int nGoodsId=atoi(goodsInfoArgs[0].c_str());
				int nNum=atoi(goodsInfoArgs[2].c_str());
				int nLockNum=atoi(goodsInfoArgs[3].c_str());

				CIncrementShopList::MAP_INCGOODS* pIncGoodsList = CIncrementShopList::GetIncGoodsList();
				//CIncrementShopList::MAP_INCSHOP* pIncMapGoodsList = CIncrementShopList::GetMapIncShopList();
				CIncrementShopList::ITR_INCGOODS itIncGoodsList = pIncGoodsList->find( CIncrementShopList::TM_YUANBAO );
				CIncrementShopList::MAP_INCSHOP& mapIncGoodsList = itIncGoodsList->second;
				CIncrementShopList::ITR_INCSHOP it;
				if( !pIncGoodsList )//|| !pIncMapGoodsList 
				{
					return;
				}

				//if (nLockNum==1) // 绑定，即分红
				//{
				//	//分红配置里查找
				//	it =  pIncMapGoodsList->find(nGoodsId);
				//	if (it == pIncMapGoodsList->end())
				//	{
				//		//正常配置里查找
				//		it = mapIncGoodsList.find( nGoodsId );
				//	}
				//}
				//else
				{
					//正常配置里查找
					it = mapIncGoodsList.find(nGoodsId);
					if( it == mapIncGoodsList.end() || !(&it->second) )
					{
						return;
					}
				}
	
#ifdef _ASCYN_DEBUG
				_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg),正常配置里查找");
				PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
				long lTotalAmount = it->second.dwPrice * nNum * it->second.dwDiscount / 10;
				bool bSucceed=IncShopProcessing(pPlayer,nGoodsId,nNum*it->second.dwGoodsNum,
					CIncrementShopList::TM_YUANBAO,lTotalAmount);				

#ifdef _ASCYN_DEBUG
				_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg),IncShopProcessing(pPlayer,nGoodsId,nNum*it->second.dwGoodsNum,CIncrementShopList::TM_YUANBAO,lTotalAmount)");
				PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
				int retcode=0;

				list<tagValue> lstValue;
				tagValue stValue;
				

				if(bSucceed)
				{					
					stValue.strName = "result";
					stValue.strValue = "0";
					lstValue.push_back(stValue);
					CGoodsFactory::PushOrder(szOrderId,szGoodsInfoEx);
					CGoodsFactory::ErasePendingOrder(szOrderId);
				}
				else
				{
					retcode=-10227000;
					stValue.strName = "result";
					stValue.strValue = "-10227000";
					lstValue.push_back(stValue);
					
					char* strRes=CStringReading::LoadString(22,3);
					if(strRes)
						pPlayer->SendNotifyMessage(strRes);
				}
				//AddLogText("logT522_snda_ticket_consume_log");
				GetGameLogInterface()->logT522_snda_ticket_consume_log(pPlayer,szOrderId,szSendTimes,szGoodsInfoEx,
					payErrorMsg,retcode);

			


				stValue.strName = "orderId";
				stValue.strValue = szOrderId;
				lstValue.push_back(stValue);

				stValue.strName = "descMsg";
				stValue.strValue = descMsg;
				lstValue.push_back(stValue);				

				stValue.strName = "appendix";
				stValue.strValue = appendix;
				lstValue.push_back(stValue);
				SDOSendResponse(nMsgType,nMsgForm,nMsgSeq, lstValue);
#ifdef _ASCYN_DEBUG
				_snprintf(szOutPut,512,"OnBillingMessage(CMessage* pMsg)第4返回");
				PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
			}		
		}
		break;
	}
}