#include "StdAfx.h"
#include "..\BaseObject.h"
#include "..\Player.h"
#include "..\ServerRegion.h"
#include "..\Goods\CGoodsFactory.h"
#include "..\Session\CSession.h"
#include "..\Session\CSessionFactory.h"
#include "..\Session\CPlug.h"
#include "../session/ctrader.h"
#include "../session/CEquipmentEnchase.h"
#include "../session/CPersonalShopSeller.h"
#include "..\Container\CGoodsShadowContainer.h"
#include "../container/CEquipmentContainer.h"
#include "..\Container\CSubpackContainer.h"
#include "../container/CTestContainer.h"
#include "CC2SContainerObjectMove.h"
#include "CS2CContainerObjectMove.h"
#include "..\..\..\setup\globesetup.h"
#include "../../../setup/LogSystem.h"
#include "../../../../public/StrUnifyInput.h"


#include "..\OrganizingSystem/GSTeam.h"
#include "..\OrganizingSystem/GameOrganizingCtrl.h"

#include "../../AppServer/GoodsExManage.h"
#include "../PlayerDepot.h"
#include "CS2CContainerObjectMove.h"
#include "..\Goods\CGoods.h"


CC2SContainerObjectMove::CC2SContainerObjectMove()
{
	m_lSourceContainerType				= 0;
	m_SourceContainerID				= NULL_GUID;
	m_lSourceContainerExtendID			= 0;
	m_dwSourceContainerPosition			= 0;

	m_lDestinationContainerType			= 0;
	m_DestinationContainerID			= NULL_GUID;
	m_lDestinationContainerExtendID		= 0;
	m_dwDestinationContainerPosition	= 0;

	m_lObjectType						= 0;
	m_dwAmount							= 0;
}

CC2SContainerObjectMove::~CC2SContainerObjectMove()
{
}

BOOL CC2SContainerObjectMove::Receive( CMessage* pMessage )
{
	BOOL bResult = FALSE;
	if(pMessage->GetPlayer() == NULL || pMessage->GetRegion() == NULL)
		return bResult;

	pClientPlayer = pMessage->GetPlayer();
	if( pMessage )
	{
		m_lSourceContainerType				= pMessage -> GetLong();
		pMessage -> GetGUID(m_SourceContainerID);
		m_lSourceContainerExtendID			= pMessage -> GetLong();
		m_dwSourceContainerPosition			= pMessage -> GetLong();

		m_lDestinationContainerType			= pMessage -> GetLong();
		pMessage -> GetGUID(m_DestinationContainerID);		

		m_lDestinationContainerExtendID		= pMessage -> GetLong();
		m_dwDestinationContainerPosition	= pMessage -> GetLong();

		m_lObjectType						= pMessage -> GetLong();
		//m_lObjectID							= 0;
		pMessage ->GetGUID(m_guObjectID);
		m_dwAmount							= pMessage -> GetLong();

		if((m_lSourceContainerExtendID==m_lDestinationContainerExtendID) && (m_dwSourceContainerPosition==m_dwDestinationContainerPosition))
			return FALSE;
		if(m_lSourceContainerExtendID==CS2CContainerObjectMove::PEI_EQUIPMENT && m_lDestinationContainerType==TYPE_SESSION)
			return FALSE;

#ifdef _DEBUG

		//CHAR szMsg[512];
		//_snprintf( szMsg, "客户端容器消息: Src: %d, %d, eid: %d, pos %d, Dest: %d, %d, eid %d, pos %d, Obj: %d, %d, mnt %d\n", 
		//	m_lSourceContainerType, m_SourceContainerID, m_lSourceContainerExtendID, m_dwSourceContainerPosition, 
		//	m_lDestinationContainerType, m_DestinationContainerID, m_lDestinationContainerExtendID, m_dwDestinationContainerPosition, 
		//	m_lObjectType, m_lObjectID, m_dwAmount );
		//OutputDebugString( szMsg );

#endif

#ifdef _GOODSLOG1_
		char pszStatckInfo[1024]="";
		char ScGuid[50]="";m_SourceContainerID.tostring(ScGuid);
		char DCGuid[50]="";m_DestinationContainerID.tostring(DCGuid);
		char ObjID[50]="";m_guObjectID.tostring(ObjID);
		_snprintf(pszStatckInfo,1024,"CC2SContainerObjectMove::Receive(SCT:%d,SCID:%s,SCExID:%d,SCP:%d;DCT:%d,DCID:%s,DCExID:%d,DCP:%d;ObjT:%d,ObjID:%s,Amount:%d)",
			m_lSourceContainerType,ScGuid,m_lSourceContainerExtendID,m_dwSourceContainerPosition,
			m_lDestinationContainerType,DCGuid,m_lDestinationContainerExtendID,m_dwDestinationContainerPosition,
			m_lObjectType,ObjID,m_dwAmount);
		CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszStatckInfo);
#endif

		//检测、修正客户端发来的值
		if(m_lObjectType != TYPE_GOODS)
		{
			//客户端发来了错误的消息
			return FALSE;
		}
		CPlayer* pPlayer = pMessage->GetPlayer();
		CServerRegion* pServerRegion = pMessage->GetRegion();	

		bResult		= TRUE;

		if(0 >= m_dwAmount)
		{
			return FALSE;
		}

		//! 如果原是仓库（除开金币和宝石的仓库）,则只能与自己和背包（主背包、背包容器、子背包）发生关系
		if(m_lSourceContainerExtendID >= eDCT_Primary)
		{
			if(m_lSourceContainerExtendID == eDCT_Primary && m_dwSourceContainerPosition >= PRIMARY_SIZE)
				return FALSE;

			if(m_lDestinationContainerExtendID < eDCT_Primary
				&& (
				m_lDestinationContainerExtendID != CS2CContainerObjectMove::PEI_PACKET &&
				m_lDestinationContainerExtendID != CS2CContainerObjectMove::PEI_PACK &&
				m_lDestinationContainerExtendID != CS2CContainerObjectMove::PEI_PACK1 &&
				m_lDestinationContainerExtendID != CS2CContainerObjectMove::PEI_PACK2 &&
				m_lDestinationContainerExtendID != CS2CContainerObjectMove::PEI_PACK3 &&
				m_lDestinationContainerExtendID != CS2CContainerObjectMove::PEI_PACK4 &&
				m_lDestinationContainerExtendID != CS2CContainerObjectMove::PEI_PACK5
				)
				)
			{
				bResult	= FALSE;
			}
		}

		if(m_lSourceContainerExtendID==CS2CContainerObjectMove::PEI_EQUIPMENT || m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_EQUIPMENT)
		{
			CServerRegion* pPlayerRegioin=dynamic_cast<CServerRegion*>(pPlayer->GetFather());
			if(pPlayerRegioin)
			{
				if(!pPlayerRegioin->IsChangeEquip())
				{
					//不允许穿上，取下，交换装备
					char* pStrMsg=CStringReading::LoadString(20,51);
					if(pStrMsg)
						pPlayer->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
					return FALSE;
				}
			}
		}

		//!如果目标是仓库（除开金币和宝石的仓库）,同样只能与自己和背包（主背包、背包容器、子背包）发生关系
		if(m_lDestinationContainerExtendID >= eDCT_Primary && m_lDestinationContainerType!=TYPE_SESSION)
		{
			if(m_lDestinationContainerExtendID == eDCT_Primary && m_dwDestinationContainerPosition >= PRIMARY_SIZE)
				return FALSE;
			if(m_lSourceContainerExtendID < eDCT_Primary
				&& (
				m_lSourceContainerExtendID != CS2CContainerObjectMove::PEI_PACKET &&
				m_lSourceContainerExtendID != CS2CContainerObjectMove::PEI_PACK &&
				m_lSourceContainerExtendID != CS2CContainerObjectMove::PEI_PACK1 &&
				m_lSourceContainerExtendID != CS2CContainerObjectMove::PEI_PACK2 &&
				m_lSourceContainerExtendID != CS2CContainerObjectMove::PEI_PACK3 &&
				m_lSourceContainerExtendID != CS2CContainerObjectMove::PEI_PACK4 &&
				m_lSourceContainerExtendID != CS2CContainerObjectMove::PEI_PACK5
				)
				)
			{
				bResult	= FALSE;

			}
			//物品是否是不可存仓库物品
			if(pPlayer)
			{
				CGoods *pGoods = pPlayer->FindGoods(m_lSourceContainerExtendID,m_dwSourceContainerPosition);
				if(pGoods)
				{
					if(pGoods->GetAddonPropertyValues( GAP_CANNOT_SAVE_DEPOT, 1 ))
						return FALSE;
				}
			}
			
		}

		//! 装备到勋章容器
		if(m_lDestinationContainerExtendID == CS2CContainerObjectMove::PEI_MEDAL_CONTAINER)
		{
			if(m_lSourceContainerExtendID != CS2CContainerObjectMove::PEI_EQUIPMENT || 
				!(CEquipmentContainer::EC_MEDAL1 == m_dwSourceContainerPosition || CEquipmentContainer::EC_MEDAL2 == m_dwSourceContainerPosition || CEquipmentContainer::EC_MEDAL3 == m_dwSourceContainerPosition)
				)
			{
				return FALSE;
			}
		}

		if(m_lSourceContainerExtendID == CS2CContainerObjectMove::PEI_MEDAL_CONTAINER)
		{
			if(m_lDestinationContainerExtendID != CS2CContainerObjectMove::PEI_EQUIPMENT || 
				!(CEquipmentContainer::EC_MEDAL1 == m_dwDestinationContainerPosition || CEquipmentContainer::EC_MEDAL2 == m_dwDestinationContainerPosition || CEquipmentContainer::EC_MEDAL3 == m_dwDestinationContainerPosition)
				)
			{
				return FALSE;
			}
		}

		if(m_lSourceContainerExtendID == CS2CContainerObjectMove::PEI_EQUIPMENT && 
			(CEquipmentContainer::EC_MEDAL1 == m_dwSourceContainerPosition || CEquipmentContainer::EC_MEDAL2 == m_dwSourceContainerPosition || CEquipmentContainer::EC_MEDAL3 == m_dwSourceContainerPosition)
			)
		{
			if (m_lDestinationContainerExtendID != CS2CContainerObjectMove::PEI_MEDAL_CONTAINER)
			{
				return FALSE;
			}
		}

		// 商品移动规则，目前只允许在商业背包内移动，只有商品可以移动到商业背包里
		if( ( m_lDestinationContainerExtendID == CS2CContainerObjectMove::PEI_BUSSINESPACK &&
			m_lSourceContainerExtendID != CS2CContainerObjectMove::PEI_BUSSINESPACK ) ||
			( m_lDestinationContainerExtendID != CS2CContainerObjectMove::PEI_BUSSINESPACK &&
			m_lSourceContainerExtendID == CS2CContainerObjectMove::PEI_BUSSINESPACK ) )
		{
			return FALSE;
		}


		//从玩家身上到玩家身上
		if(m_lSourceContainerType == TYPE_PLAYER && m_lDestinationContainerType == TYPE_PLAYER)
		{
			//既然是玩家到玩家  那就必须是同一个玩家
			
			if(m_SourceContainerID!=pPlayer->GetExID() || m_DestinationContainerID!=pPlayer->GetExID())
				return FALSE;
				

			if(m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_EQUIPMENT)
			{
				if(!pPlayer->GetUseItem())
				{
					return FALSE;
				}
			}

			//范围限制			
			if(m_lSourceContainerExtendID < 0 || 
				(m_lSourceContainerExtendID > CS2CContainerObjectMove::PEI_MEDAL_CONTAINER && m_lSourceContainerExtendID<CS2CContainerObjectMove::PEI_PACK) ||
				m_lDestinationContainerExtendID < 0 || 
				(m_lDestinationContainerExtendID > CS2CContainerObjectMove::PEI_MEDAL_CONTAINER&& m_lDestinationContainerExtendID<CS2CContainerObjectMove::PEI_PACK )
				)
			{
				return FALSE;
			}			
			
			if(m_lSourceContainerExtendID == eDCT_Gold)
			{
				//如果SRC是仓库金币 则DST必须是背包金币
				if(m_lDestinationContainerExtendID != CS2CContainerObjectMove::PEI_WALLET)
				{
					return FALSE;
				}
			}
			else if(m_lSourceContainerExtendID == eDCT_Silver)
			{
				//如果SRC是仓库银币 则DST必须是背包银币
				if(m_lDestinationContainerExtendID != CS2CContainerObjectMove::PEI_SILVERWALLET)
				{
					return FALSE;
				}
			}
			else if(m_lSourceContainerExtendID == CS2CContainerObjectMove::PEI_WALLET)
			{
				//如果SRC是背包金币 则DST必须是仓库金币
				if(m_lDestinationContainerExtendID != eDCT_Gold)
				{
					return FALSE;
				}
			}
			else if(m_lSourceContainerExtendID == CS2CContainerObjectMove::PEI_SILVERWALLET)
			{
				//如果SRC是背包银币 则DST必须是仓库银币
				if(m_lDestinationContainerExtendID != eDCT_Silver)
				{
					return FALSE;
				}
			}

			//! 如果原是宝石,流向做判断和限制
			if(m_lSourceContainerExtendID == CS2CContainerObjectMove::PEI_GEM || 
				m_lDestinationContainerExtendID == CS2CContainerObjectMove::PEI_GEM)
			{				
				return FALSE;				
			}
			
			//如果目标是元宝
			if(m_lSourceContainerExtendID ==CS2CContainerObjectMove::PEI_YUANBAO || 
				m_lDestinationContainerExtendID == CS2CContainerObjectMove::PEI_YUANBAO)
			{
				return FALSE;
			}
		}
		//从场景到玩家身上
		else if(m_lSourceContainerType == TYPE_REGION && m_lDestinationContainerType == TYPE_PLAYER)
		{
			if(timeGetTime() - pPlayer->GetLastPickGoodsTime() <= CGlobeSetup::GetSetup()->dwPickGoodsInterval)
			{
				return FALSE;
			}
			else
			{
				pPlayer->SetLastPickGoodsTime(timeGetTime());
			}
			
			if(m_SourceContainerID!=pServerRegion->GetExID() ||	m_DestinationContainerID!=pPlayer->GetExID())
				return FALSE;


			if(m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_PACKET || 
				m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_WALLET || 
				m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_SILVERWALLET || 
				(m_lDestinationContainerExtendID>=CS2CContainerObjectMove::PEI_PACK1 && m_lDestinationContainerExtendID<=CS2CContainerObjectMove::PEI_PACK5))
			{
			}
			else
			{
				return FALSE;
			}

			m_lSourceContainerExtendID			= 0;
			m_dwSourceContainerPosition			= pPlayer->GetTileY()*pServerRegion->GetWidth()+pPlayer->GetTileX();

			//如果玩家在摆摊和交易时则不能捡物品
			if( pPlayer->GetCurrentProgress() == CPlayer::PROGRESS_OPEN_STALL )
			{
				pPlayer -> SendNotifyMessage( CStringReading::LoadString(21,1) );
				return FALSE;
			}
			if(	pPlayer->GetCurrentProgress() == CPlayer::PROGRESS_TRADING)
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(23,1) );
				return FALSE;
			}
			string str = "bRide";
			if (pPlayer->GetAttribute(str) == FALSE)
			{				
				pPlayer->SendPlayerInfo(CStringReading::LoadString(20,32));// SendSystemInfo();
				return FALSE;
			}
			
			if(m_lDestinationContainerExtendID == CS2CContainerObjectMove::PEI_WALLET)
				m_dwDestinationContainerPosition	= 0;						
		}
		//从玩家身上到场景
		else if(m_lSourceContainerType == TYPE_PLAYER && m_lDestinationContainerType == TYPE_REGION)
		{			
			//发过来的场景GUID不是玩家所在场景			
			if(m_SourceContainerID!=pPlayer->GetExID())
				return FALSE;
			if(m_DestinationContainerID!=pServerRegion->GetExID())
				return FALSE;

			//范围限制
			if(m_lSourceContainerExtendID==CS2CContainerObjectMove::PEI_PACKET || 
				m_lSourceContainerExtendID==CS2CContainerObjectMove::PEI_EQUIPMENT || 
				(m_lSourceContainerExtendID>=CS2CContainerObjectMove::PEI_PACK1 && m_lSourceContainerExtendID<=CS2CContainerObjectMove::PEI_PACK5))
			{				
			}
			else
			{
				return FALSE;
			}			
			m_DestinationContainerID			= pServerRegion->GetExID();
			m_lDestinationContainerExtendID		= 0;
			m_dwDestinationContainerPosition	= pPlayer->GetTileY()*pServerRegion->GetWidth()+pPlayer->GetTileX();
		}
		//从Session到玩家身上
		else if(m_lSourceContainerType == TYPE_SESSION && m_lDestinationContainerType == TYPE_PLAYER)
		{
			return FALSE;
		}
		//从玩家身上到Session
		else if(m_lSourceContainerType == TYPE_PLAYER && m_lDestinationContainerType == TYPE_SESSION)
		{
			if(m_SourceContainerID!=pPlayer->GetExID())
				return FALSE;
			//m_SourceContainerID				= pPlayer->GetExID();	

			//范围限制
			if(m_lSourceContainerExtendID==CS2CContainerObjectMove::PEI_PACKET || 
				m_lSourceContainerExtendID==CS2CContainerObjectMove::PEI_WALLET || 
				(m_lSourceContainerExtendID>=CS2CContainerObjectMove::PEI_PACK1 && m_lSourceContainerExtendID<=CS2CContainerObjectMove::PEI_PACK5))
			{				
			}
			else
			{
				return FALSE;
			}			
			
			//如果是从钱包到Session
			if(m_lSourceContainerExtendID == CS2CContainerObjectMove::PEI_WALLET)
				m_dwSourceContainerPosition			= 0;
		}
		else
		{
			bResult = FALSE;
		}
		if(bResult == FALSE)
		{
		}
	}
	return bResult;
}

CBaseObject* CC2SContainerObjectMove::GetSource( LONG lType, const CGUID& guid )
{
	LONG lSourceType	= 0;
	const CGUID* pSourceID;
	if( lType == 0 && guid == NULL_GUID )
	{
		lSourceType		= m_lSourceContainerType;
		pSourceID		= &m_SourceContainerID;
	}
	else 
	{
		lSourceType		= lType;
		pSourceID		= &guid;
	}

	CBaseObject* pResult = NULL;
	if( lSourceType == TYPE_REGION )
	{
		pResult = GetGame() -> FindRegion (*pSourceID );
	}
	else if( lSourceType == TYPE_PLAYER )
	{
		pResult = GetGame() -> FindPlayer( *pSourceID );
	}
	else if( lSourceType == TYPE_SESSION )
	{
		pResult = const_cast<CSession*>( CSessionFactory::QuerySession(*pSourceID) );
	}
	return pResult;
}

CBaseObject* CC2SContainerObjectMove::GetDestination()
{
	CBaseObject* pResult = NULL;
	if( m_lDestinationContainerType == TYPE_REGION )
	{
		pResult = GetGame() -> FindRegion( m_DestinationContainerID );
	}
	else if( m_lDestinationContainerType == TYPE_PLAYER )
	{
		pResult = GetGame() -> FindPlayer( m_DestinationContainerID );
	}
	else if( m_lDestinationContainerType == TYPE_SESSION )
	{
		pResult = const_cast<CSession*>( CSessionFactory::QuerySession(m_DestinationContainerID) );
	}
	return pResult;
}

CGoods* CC2SContainerObjectMove::GetGoods( CS2CContainerObjectMove* pMessage )
{
	CGoods* pResult			= NULL;
	//获得物品的来源
	CBaseObject* pSource	= GetSource();
	if( pSource )
	{
		if( pSource -> GetType() == TYPE_REGION ) //来源为场景
		{
			//##FindChildObject效率很低。到时候应该替换掉。现在暂时处理成这个样子。
			CServerRegion* pRegion	= dynamic_cast<CServerRegion*>( pSource );
			CPlayer* pPlayer		= dynamic_cast<CPlayer*>( GetDestination() );
			CGoods* pGoods			= dynamic_cast<CGoods*>( pSource -> FindChildObject(m_lObjectType,m_guObjectID) );

			if(pGoods)
				m_dwAmount = pGoods->GetAmount();

			if( pRegion && pPlayer && pGoods )
			{
				if( abs(pGoods -> GetTileX() - pPlayer -> GetTileX()) <= 2 &&
					abs(pGoods -> GetTileY() - pPlayer -> GetTileY()) <= 2 )
				{
					//! 物品的归属判断
					eGoodsPickValue ePickValue = pRegion -> CanPickUpGoods(pPlayer, pGoods);
					switch(ePickValue)
					{
					case eGPV_Cant:
						pPlayer -> SendNotifyMessage(CStringReading::LoadString(20,3));
						break;
					case eGPV_Allow_NoOwner:
					case eGPV_Allow_IsYours:
						pResult = pGoods;
					    break;
					case eGPV_Allow_IsYourTeams:
						{
							GSTeam* pTeam = (GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(pPlayer->GetTeamID()));

							assert(NULL != pTeam);
							if(NULL == pTeam)
							{
								pResult = pGoods;
								break;
							}

							//! 如果是钱，则平分
							if(CGoodsFactory::GetGoldCoinIndex() == pGoods->GetBasePropertiesIndex()
								|| CGoodsFactory::GetSilverIndex() == pGoods->GetBasePropertiesIndex()
								)
							{
								long lGross = pGoods->GetAmount();
								long lMemberNum = pTeam->GetLucrativeAreaMemberAmount_Alive(pPlayer->GetRegionGUID(), pPlayer->GetPosX(), pPlayer->GetPosY(),TRUE);
								assert(0 != lMemberNum);
								if(0 == lMemberNum)
								{
									pResult = pGoods;
									break;
								}
								long lPortion = lGross / lMemberNum;

								//! 为了防止除不尽，多出来的钱归获得物品的玩家
								m_dwAmount = lGross - lPortion * (lMemberNum - 1);
								pGoods->SetAmount(m_dwAmount);
								pResult = pGoods;

								map<CGUID,CPlayer*> RegionTeamates;
								pTeam->GetLucrativeArea_Alive(RegionTeamates, pPlayer->GetRegionGUID(), pPlayer->GetPosX(), pPlayer->GetPosY(), TRUE);
								for (map<CGUID,CPlayer*>::iterator ite = RegionTeamates.begin(); ite != RegionTeamates.end(); ++ite)
								{
									if (pPlayer->GetExID() != ite->first)
									{
										if(CGoodsFactory::GetSilverIndex() == pGoods->GetBasePropertiesIndex())
											(ite->second)->SetSilver((ite->second)->GetSilver() + lPortion);
										else
											(ite->second)->SetMoney((ite->second)->GetMoney() + lPortion,29);
									}
								}
							}
							else
							{
 								long lLoopNum = pTeam->GetLucrativeAreaMemberAmount_Alive(pPlayer->GetRegionGUID(), pPlayer->GetPosX(), pPlayer->GetPosY(), TRUE);
								list<CGUID> listMember;
								pTeam->GetLucrativeArea_Alive(listMember, pPlayer->GetRegionGUID(), pPlayer->GetPosX(), pPlayer->GetPosY(), TRUE);
								for (long i = 0; i < lLoopNum; ++i)
								{
									CPlayer *pOwner = pTeam->GetOneOwner(m_DestinationContainerID);
									if (NULL != pOwner)
									{
										CTestContainer testContainer;
										testContainer.Init(pOwner);
										vector<tagTestResult> vecResult;
										vector<CGoods*> vGoods;
										vGoods.push_back(pGoods);
										BOOL bSucced=testContainer.AddTest(vGoods,&vecResult);	
										if(bSucced && vecResult.size() == vGoods.size())
										{
											m_lDestinationContainerExtendID=CS2CContainerObjectMove::PEI_PACKET;
											pResult = pGoods;
											m_DestinationContainerID = pOwner->GetExID();

											//! 生成消息
											////////////////////////////////////////////////////////////
											tagChatsGoodsInfo ChatGoodsInfo;
											CGUID::CreateGUID(ChatGoodsInfo.KeyGuid);

											vector<BYTE> vGoodsInfo;
											pGoods->SerializeForOldClient(&vGoodsInfo);

											//! 超连接数据消息
											CMessage msgLink(MSG_S2C_OTHER_GoodsInfo_Re);
											msgLink.Add(NULL_GUID);
											msgLink.Add(ChatGoodsInfo.KeyGuid);
											msgLink.Add((LONG)vGoodsInfo.size());
											msgLink.AddEx(&vGoodsInfo[0], vGoodsInfo.size());

											//! 聊天内容消息
											char szGetInfo[256] = {0};
											_snprintf(szGetInfo, 256, "%s%s", pOwner->GetName(), CStringReading::LoadString(20,34));
											ChatGoodsInfo.posBegin = strlen(szGetInfo);
											_snprintf(&(szGetInfo[strlen(szGetInfo)]), 256 - strlen(szGetInfo), "%s", pGoods->GetName());
											ChatGoodsInfo.posEnd = strlen(szGetInfo);
											ChatGoodsInfo.color = pGoods->GetAddonPropertyValues(GAP_COLOR, 1);
											if(0 == ChatGoodsInfo.color)
												ChatGoodsInfo.color = 0xFFFFFFFF;
											ChatGoodsInfo.lInfoType = eCGIT_Copy;
											
											CMessage msg( MSG_S2C_OTHER_ADDINFO_AND_GOODSINFO );
											msg.Add(szGetInfo);
											msg.Add(pGoods->GetName());
											msg.AddEx(&ChatGoodsInfo, sizeof(tagChatsGoodsInfo));
											////////////////////////////////////////////////////////////			

											for (list<CGUID>::iterator ite = listMember.begin(); ite != listMember.end(); ++ite)
											{
												CPlayer *pTempPlayer = GetGame()->FindPlayer(*ite);
												if(NULL != pTempPlayer)
												{
													msgLink.SendToPlayer(pTempPlayer->GetExID());
													msg.SendToPlayer(pTempPlayer->GetExID());
												}
											}
											break;
										}
										else
										{
											pOwner -> SendNotifyMessage(CStringReading::LoadString(20,4));
										}
									}
								}

								if(0 == lLoopNum)
									pResult = pGoods;

								if (NULL == pResult)
								{
									pPlayer->SendNotifyMessage( CStringReading::LoadString(20,5));
								}
							}
						}
					    break;
					}
				}
			}

			if( pResult && pResult -> GetAmount() == m_dwAmount )
			{				
				//##如果有消息，则填充消息。
				if( pMessage )
				{
					pMessage -> SetSourceContainer( m_lSourceContainerType, m_SourceContainerID, m_dwSourceContainerPosition );
					pMessage -> SetSourceContainerExtendID( m_lSourceContainerExtendID );
					pMessage -> SetSourceObject( m_lObjectType, m_guObjectID);
					pMessage -> SetSourceObjectAmount( m_dwAmount );
				}				
			}
		}
		else if( pSource -> GetType() == TYPE_PLAYER )
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>( pSource );
			if( pPlayer )
			{
				//! 勋章特例
				if(m_lSourceContainerExtendID==CS2CContainerObjectMove::PEI_MEDAL_CONTAINER)
				{
					CGoods *pRe = pPlayer->m_MedalContainer.UseMedal(m_dwSourceContainerPosition);
					if(NULL == pRe)
					{
						CGoodsContainer* pContainer = GetCGoodsContainer(pPlayer, m_lDestinationContainerExtendID);
						CEquipmentContainer* pEquipmentContainer = dynamic_cast<CEquipmentContainer*>( pContainer );
						if(NULL == pEquipmentContainer) return NULL;
						CGoods *pAimGoods = pEquipmentContainer->GetGoods(m_dwDestinationContainerPosition);
						if(NULL != pAimGoods)
							pRe = pPlayer->m_MedalContainer.SwapMedal(pAimGoods->GetBasePropertiesIndex(), m_dwSourceContainerPosition);
					}
					return pRe;
				}

				CGoodsContainer* pContainer = GetCGoodsContainer(pPlayer, m_lSourceContainerExtendID);

				if(m_lSourceContainerExtendID==CS2CContainerObjectMove::PEI_EQUIPMENT && 
					(CEquipmentContainer::EC_MEDAL1 == m_dwSourceContainerPosition || CEquipmentContainer::EC_MEDAL2 == m_dwSourceContainerPosition || CEquipmentContainer::EC_MEDAL3 == m_dwSourceContainerPosition)
					)
				{
					CEquipmentContainer* pEquipmentContainer = dynamic_cast<CEquipmentContainer*>( pContainer );
					return pEquipmentContainer->GetGoods(m_dwSourceContainerPosition);
				}				
				
				if( pContainer )
				{
					DWORD dwPosition	= 0;
					CGoods* pGoods		= dynamic_cast<CGoods*>( pContainer -> Find(m_lObjectType, m_guObjectID) );
					if( pGoods )
					{
						//如果该玩家处于交易或个人商店期间，无法对物品进行拆分操作
						if( pGoods->GetAmount() != m_dwAmount && 
							pGoods -> GetBasePropertiesIndex() != CGoodsFactory::GetGoldCoinIndex() &&
							pGoods -> GetBasePropertiesIndex() != CGoodsFactory::GetYuanBaoIndex())
						{
							if( pPlayer -> GetCurrentProgress() == CPlayer::PROGRESS_OPEN_STALL )
							{
								//发送消息给当前玩家.
								pPlayer -> SendNotifyMessage( CStringReading::LoadString(21,2) );
								return pResult;
							}
							else if(pPlayer -> GetCurrentProgress() == CPlayer::PROGRESS_TRADING )
							{
								//发送消息给当前玩家.
								pPlayer -> SendNotifyMessage( CStringReading::LoadString(23,2) );
								return pResult;
							}
						}

						if( pContainer -> QueryGoodsPosition(pGoods, dwPosition) )
						{
							if( dwPosition == m_dwSourceContainerPosition )
							{	
								/*
								if(pGoods->GetAmount()>m_dwAmount)
								{
									//这里主要针对可叠加物品 如果移动的数量小于物品当前数量
									//需要创建出一个新的GOODS 以移动物品的一部分
									pResult = CGoodsFactory::CreateGoods( pGoods -> GetBasePropertiesIndex() );
									pResult -> SetAmount(m_dwAmount);
								}
								else if(pGoods->GetAmount()==m_dwAmount)
								{
									pResult=pGoods;
								}
								*/
								pResult=pGoods;
								
								if( pResult && pMessage)
								{
									//##填充消息中必要的部分。																
									pMessage -> SetSourceContainerExtendID( m_lSourceContainerExtendID );																			
								}
							}
						}
					}
				}
			}
		}
	}
	return pResult;
}

CGoods*	CC2SContainerObjectMove::RemoveSrcGoods(CS2CContainerObjectMove* pMessage)
{
	CGoods* pResult			= NULL;
	//获得物品的来源
	CBaseObject* pSource	= GetSource();
	if( pSource )
	{
		if( pSource -> GetType() == TYPE_REGION ) //来源为场景
		{
			//##FindChildObject效率很低。到时候应该替换掉。现在暂时处理成这个样子。
			CServerRegion* pRegion	= dynamic_cast<CServerRegion*>( pSource );
			CPlayer* pPlayer		= dynamic_cast<CPlayer*>( GetDestination() );
			CGoods* pGoods			= dynamic_cast<CGoods*>( pSource -> FindChildObject(m_lObjectType,m_guObjectID) );	
			if(pGoods)
			{
				pResult=pGoods;
				pSource -> RemoveObject( pResult );
				CArea* pArea = pRegion->GetArea(pResult->GetTileX()/AREA_WIDTH, pResult->GetTileY()/AREA_HEIGHT);
				if (pArea)
				{
					pArea->LogoutGoodsDelTime(pResult);
				}	
			}				
		}
		else if( pSource -> GetType() == TYPE_SESSION )
		{
			CSession* pSession = dynamic_cast<CSession*>( pSource );
			if( pSession )
			{
				CPlug* pPlug = pSession -> QueryPlugByID( m_lSourceContainerExtendID >> 8 );
				if( pPlug )
				{
					CGoodsContainer* pContainer = dynamic_cast<CGoodsContainer*>(
						pPlug -> GetContainer(m_lSourceContainerExtendID & 0x000000ff) );
					CPlayer* pPlayer = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
					if( pContainer && pPlayer && pPlayer -> GetType() == m_lDestinationContainerType &&
						pPlayer -> GetExID() == m_DestinationContainerID )
					{
						DWORD dwPosition	= 0;
						CGoods* pGoods		= dynamic_cast<CGoods*>( pContainer -> Find(m_lObjectType, m_guObjectID) );
						if( pGoods )
						{
							//无法对关于Session里的物品进行拆分操作
							if( pGoods->GetAmount() != m_dwAmount)
							{
								return pResult;
							}

							if( pContainer -> QueryGoodsPosition(pGoods, dwPosition) )
							{
								if( dwPosition == m_dwSourceContainerPosition )
								{									
									pResult	= dynamic_cast<CGoods*>( pContainer -> Remove(m_dwSourceContainerPosition, 
										m_dwAmount, pMessage) );									
								}
							}
						}
					}
				}
			}
		}
		else if( pSource -> GetType() == TYPE_PLAYER )
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>( pSource );
			if( pPlayer )
			{
				//! 勋章特例
				if(m_lSourceContainerExtendID==CS2CContainerObjectMove::PEI_MEDAL_CONTAINER)
				{
					CGoods *pRe = pPlayer->m_MedalContainer.UseMedal(m_dwSourceContainerPosition);
					if(NULL == pRe)
					{
						CGoodsContainer* pContainer = GetCGoodsContainer(pPlayer, m_lDestinationContainerExtendID);
						CEquipmentContainer* pEquipmentContainer = dynamic_cast<CEquipmentContainer*>( pContainer );
						if(NULL == pEquipmentContainer) return NULL;
						CGoods *pAimGoods = pEquipmentContainer->GetGoods(m_dwDestinationContainerPosition);
						if(NULL != pAimGoods)
							pRe = pPlayer->m_MedalContainer.SwapMedal(pAimGoods->GetBasePropertiesIndex(), m_dwSourceContainerPosition);
					}
					return pRe;
				}

				CGoodsContainer* pContainer = GetCGoodsContainer(pPlayer, m_lSourceContainerExtendID);

				if(m_lSourceContainerExtendID==CS2CContainerObjectMove::PEI_EQUIPMENT && 
					(CEquipmentContainer::EC_MEDAL1 == m_dwSourceContainerPosition || CEquipmentContainer::EC_MEDAL2 == m_dwSourceContainerPosition || CEquipmentContainer::EC_MEDAL3 == m_dwSourceContainerPosition)
					)
				{
					CEquipmentContainer* pEquipmentContainer = dynamic_cast<CEquipmentContainer*>( pContainer );
					CGoods *pRe = pEquipmentContainer->GetGoods(m_dwSourceContainerPosition);
					if(NULL != pRe)
					{
						pEquipmentContainer->Remove(m_dwSourceContainerPosition, 1);
					}
					return pRe;
				}

				if( pContainer )
				{
					DWORD dwPosition	= 0;
					CGoods* pGoods		= dynamic_cast<CGoods*>( pContainer -> Find(m_lObjectType, m_guObjectID) );								

					pResult	= dynamic_cast<CGoods*>( pContainer -> Remove(m_dwSourceContainerPosition, 
						m_dwAmount, pMessage) );								
				}
			}
		}
	}
	return pResult;
}

BOOL CC2SContainerObjectMove::Move( CS2CContainerObjectMove* pResult )
{
	BOOL bResult = FALSE;
	
	CBaseObject* pSource		= GetSource();
   	CGoods*	pGoods				= GetGoods( pResult );	
	CBaseObject* pDestination	= GetDestination();

	if(pSource && pDestination && pGoods)
	{
		DWORD dwSrcGoodsNum=pGoods->GetAmount();
		BOOL bCreateNewFlag=FALSE;
		eGoodsBaseType eGoodsType=pGoods->GetGoodsBaseType();
		CGoodsBaseProperties* pBaseProperty=CGoodsFactory::QueryGoodsBaseProperties(pGoods->GetBasePropertiesIndex());
		if(!pBaseProperty)
			return FALSE;

		if(pSource->GetType()==TYPE_PLAYER)
		{
			CPlayer* pPlayer=dynamic_cast<CPlayer*>(pSource);
			if(!pPlayer)
				return FALSE;
			if(pPlayer->IsInPersonalShop(pGoods) && pGoods!=pPlayer->m_cWallet.GetGoldGoods())
				return FALSE;
			if(m_lSourceContainerExtendID==CS2CContainerObjectMove::PEI_PACK)
			{				
				if(m_dwSourceContainerPosition>=CS2CContainerObjectMove::PEI_PACK1 && 
					m_dwSourceContainerPosition<=CS2CContainerObjectMove::PEI_PACK5)
				{

					CVolumeLimitGoodsContainer* pVolContainer=pPlayer->getSubpackContainer()->GetSubpack(m_dwSourceContainerPosition-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
					if(pVolContainer)				
					{
						if(pVolContainer->GetGoodsMap()->size()>0)
							return FALSE;
					}
					else
						return FALSE;
				}
				else
				{
					return FALSE;
				}			
			}
			else if (eDCT_Secondary == m_lSourceContainerExtendID)
			{				
				if(m_dwSourceContainerPosition >=CS2CContainerObjectMove::PEI_PACK1 && 
					m_dwSourceContainerPosition<=CS2CContainerObjectMove::PEI_PACK4)
				{
					CVolumeLimitGoodsContainer* pVolContainer=(CVolumeLimitGoodsContainer*)GetCGoodsContainer(pPlayer, m_dwSourceContainerPosition - CS2CContainerObjectMove::PEI_PACK + eDCT_Secondary);
					if(pVolContainer)				
					{
						if(pVolContainer->GetGoodsMap()->size()>0)
							return FALSE;
					}
					else
						return FALSE;
				}
				else
				{
					return FALSE;
				}
			}
		}
		


		//! 如果是操作勋章，不作这个判断
		if(!(m_lSourceContainerExtendID == CS2CContainerObjectMove::PEI_MEDAL_CONTAINER || m_lDestinationContainerExtendID == CS2CContainerObjectMove::PEI_MEDAL_CONTAINER))
			if(m_guObjectID!=pGoods->GetExID() || m_dwAmount>pGoods->GetAmount())
				return FALSE;
		//源是场景 目标是玩家
		if(m_lSourceContainerType == TYPE_REGION && m_lDestinationContainerType == TYPE_PLAYER)
		{	
			CPlayer* pPlayer = dynamic_cast<CPlayer*>( pDestination );
			//是否是拾取品
			
			if(m_lDestinationContainerExtendID!=CS2CContainerObjectMove::PEI_WALLET && m_lDestinationContainerExtendID!=CS2CContainerObjectMove::PEI_SILVERWALLET)
			{			
				//如果原始背包已经满，改destination	
				if(pPlayer)
				{
					//是否是拾取品
					if(pGoods->GetGoodsBaseType()==GT_PICKGOODS)
					{
						//直接从场景删除
						DWORD dwGoodsId=pGoods->GetBasePropertiesIndex();
						pSource->RemoveObject(pGoods);
						pResult->SetOperation(CS2CContainerObjectMove::OT_DELETE_OBJECT);	
						CGoodsFactory::RunScriptOnGet(dwGoodsId,pPlayer);

#ifdef _GOODSLOG1_
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						pGoods->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:拾取品][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
						CGoodsFactory::GarbageCollect(&pGoods,150);
						return TRUE;
					}

					//唯一检测	
					if(pGoods->HasAddonProperty(GAP_UNIQUE))
					{
						DWORD dwGoodsId=pGoods->GetBasePropertiesIndex();
						if(!pPlayer->IsGoodsUnique(CGoodsFactory::QueryGoodsOriginalName(dwGoodsId)))
						{
							char* strMsg=CStringReading::LoadString(20,49);
							if(strMsg)
							{
								CMessage msg(MSG_S2C_OTHER_GAMEMESSAGE);
								msg.Add(strMsg);
								msg.Add((long)0);
								msg.Add(CGUID::GUID_INVALID);
								msg.SendToPlayer(pPlayer->GetExID());
							}
							return FALSE;
						}
					}					

					CTestContainer testContainer;
					testContainer.Init(pPlayer);
					vector<tagTestResult> vecResult;
					vector<CGoods*> vGoods;
					vGoods.push_back(pGoods);
					BOOL bSucced=testContainer.AddTest(vGoods,&vecResult);	
					if(bSucced && vecResult.size()==vGoods.size())
					{
						m_lDestinationContainerExtendID=vecResult[0].dwContainerId;
						m_dwDestinationContainerPosition=vecResult[0].dwPos;
					}
					else
					{
						pPlayer -> SendNotifyMessage(CStringReading::LoadString(20,2));
						bResult	= FALSE;
					}					
				}
			}
		}		
		
		CGUID guid;
		DWORD dwPrice;
		char szGoodsName[32] = "";
		
		guid = pGoods->GetExID();		
		memset(szGoodsName, 0, sizeof(szGoodsName));
		memcpy(szGoodsName, pGoods->GetGoodsName(), sizeof(szGoodsName));
		szGoodsName[31] = '\0';

		if(!lstrcmp(szGoodsName, CGoodsFactory::GetGoldName()))
		{
			dwPrice = pGoods->GetAmount();

			if(pSource->GetType() == TYPE_REGION)
			{
				m_lDestinationContainerExtendID		= CS2CContainerObjectMove::PEI_WALLET;
				m_dwDestinationContainerPosition	= 0;
				pResult->SetDestinationObjectAmount(dwPrice);
			}
			else if(pDestination->GetType() == TYPE_REGION)
			{
				m_lSourceContainerExtendID = CS2CContainerObjectMove::PEI_WALLET;
				m_dwSourceContainerPosition = 0;
			}
		}
		else if(!lstrcmp(szGoodsName, CGoodsFactory::GetSilverName()))
		{
			dwPrice = pGoods->GetAmount();

			if(pSource->GetType() == TYPE_REGION)
			{
				m_lDestinationContainerExtendID		= CS2CContainerObjectMove::PEI_SILVERWALLET;
				m_dwDestinationContainerPosition	= 0;
				pResult->SetDestinationObjectAmount(dwPrice);
			}
			else if(pDestination->GetType() == TYPE_REGION)
			{
				m_lSourceContainerExtendID = CS2CContainerObjectMove::PEI_SILVERWALLET;
				m_dwSourceContainerPosition = 0;
			}
		}		
		else
		{
			dwPrice = pGoods->GetPrice();					
		}

		//判断物品是放置 OR 交换	

		if(pGoods->GetAmount()>0 && m_dwAmount>0)
		{			
			//这里主要针对可叠加物品 如果移动的数量小于物品当前数量
			//需要创建出一个新的GOODS 以移动物品的一部分
			if(pGoods->GetMaxStackNumber()>1 && pGoods->GetAmount()>m_dwAmount)
			{
				DWORD dwGoodsId=pGoods->GetBasePropertiesIndex();
				CGoods* pOldGoods=pGoods;
				const CGUID& oldGuid=pGoods->GetExID();
				pGoods=CGoodsFactory::CreateGoods(dwGoodsId,50);
				bCreateNewFlag=TRUE;
				pGoods->SetAmount(m_dwAmount);
#ifdef _GOODSLOG1_
				if(pGoods->GetExID()==oldGuid)
				{
					char pszGoodsLog[1024]="";			
					char srcGoodsGUID[50]="";
					char oldGoodsGUID[50]="";
					pGoods->GetExID().tostring(srcGoodsGUID);
					oldGuid.tostring(oldGoodsGUID);
					_snprintf(pszGoodsLog,1024,"[ERROR:原物品和新创建物品GUID相同][GUID:%s][OldGUID:%s]",srcGoodsGUID,oldGoodsGUID);
					CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
				}										
#endif
				
			}			
		}
		else
		{
			return FALSE;
		}

		//目标物品名
		char strDesGoodsName[50]="";
		DWORD dwDesGoodsAmount=0;
		
		CS2CContainerObjectMove::OPERATION_TYPE eOperType=CS2CContainerObjectMove::OT_UNKNOWN;
		if( pDestination -> GetType() == TYPE_REGION && pSource->GetType()==TYPE_PLAYER)
		{
			if (GoodsExManage::GetInstance().IsFrost(pGoods)) //! 如果冻结
			{
				return FALSE;
			}
			//取得场景对象
			CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pDestination );
			if( pRegion )
			{
				LONG lX = m_dwDestinationContainerPosition % pRegion -> GetWidth();
				LONG lY = m_dwDestinationContainerPosition / pRegion -> GetWidth();

				LONG lDropGoodsStartPos = 0;
				lDropGoodsStartPos = pRegion->GetDropGoodsPos(lX, lY, lX, lY, lDropGoodsStartPos);
				if( lDropGoodsStartPos != -1 )
				{
					eOperType=CS2CContainerObjectMove::OT_MOVE_OBJECT;
					m_dwDestinationContainerPosition = lX + pRegion->GetWidth() * lY;
					pGoods -> SetPosXY( lX+0.5f, lY+0.5f );
					CGoods* pRemovedGoods=RemoveSrcGoods(pResult);
					if(!pRemovedGoods)
						return FALSE;
					pDestination -> AddObject( pGoods );
					
					if(pRemovedGoods)
					{
						if(pRemovedGoods!=pGoods && bCreateNewFlag)
						{
							CGoodsFactory::GarbageCollect(&pRemovedGoods,14);
						}
					}
					
					CArea* pArea = pRegion->GetArea(lX/AREA_WIDTH, lY/AREA_HEIGHT);
					if(pArea)
					{
						pArea->AddGoodsDelTime(pGoods);
					}

					pResult -> SetDestinationContainer( m_lDestinationContainerType, 
						m_DestinationContainerID, m_dwDestinationContainerPosition );
					pResult -> SetDestinationContainerExtendID( m_lDestinationContainerExtendID );
					pResult -> SetDestinationObject( pGoods -> GetType(),pGoods->GetExID() );
					pResult -> SetDestinationObjectAmount( pGoods -> GetAmount() );

					pResult->SetSourceContainer(m_lSourceContainerType,m_SourceContainerID,m_dwSourceContainerPosition);
					pResult->SetSourceContainerExtendID(m_lSourceContainerExtendID);
					pResult->SetSourceObject(pGoods->GetType(),m_guObjectID);
					pResult->SetSourceObjectAmount(pGoods->GetAmount());
					pResult->SetOperation(CS2CContainerObjectMove::OT_MOVE_OBJECT);
					
					if(m_lSourceContainerType==TYPE_PLAYER)
					{						
						CPlayer* pPlayer=dynamic_cast<CPlayer*>(GetSource());
						if(pPlayer)
						{
							//丢弃物品日志
							DWORD dwRemoveGoodsId=pGoods->GetBasePropertiesIndex();
							const char* strRemovedGoodsName=CGoodsFactory::QueryGoodsName(dwRemoveGoodsId);
							CGUID removeGoodsGuid=pGoods->GetExID();
							DWORD dwRemovedGoodsNum=pGoods->GetAmount();
							pPlayer->OnLoseGoods(pGoods);
							GetGameLogInterface()->logT020_goods_log_pickup(pPlayer,dwRemoveGoodsId,removeGoodsGuid,strRemovedGoodsName,1,dwRemovedGoodsNum);
#ifdef _GOODSLOG1_
							char pszGoodsLog[1024]="";
							char playerGUID[50]="";
							pPlayer->GetExID().tostring(playerGUID);
							char goodsGUID[50]="";
							pGoods->GetExID().tostring(goodsGUID);
							_snprintf(pszGoodsLog,1024,"[操作:玩家丢弃物品][玩家名:%s][玩家GUID:%s][场景ID:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d][X坐标:%d][Y坐标:%d]",
								pPlayer->GetName(),playerGUID,pRegion->GetID(),pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),
								goodsGUID,pGoods->GetAmount(),lX,lY);
							CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif														
						}
					}
				}				
			}
		}
		else if(pDestination->GetType()==TYPE_PLAYER)
		{
			//目标位置是否有物品
			CContainer::tagPreviousContainer tPreviousContainer;
			CPlayer* pPlayer = dynamic_cast<CPlayer*>( pDestination );
			if(!pPlayer)
				return FALSE;
			
			if(m_lDestinationContainerExtendID == CS2CContainerObjectMove::PEI_MEDAL_CONTAINER)
			{
				if(eMCRV_Succeed == pPlayer->m_MedalContainer.ReturnMedal(pGoods->GetBasePropertiesIndex()))
				{
					RemoveSrcGoods(pResult);
					pResult->SetOperation(CS2CContainerObjectMove::OT_MOVE_OBJECT);
					pResult->SetSourceContainer(pPlayer->GetType(), pPlayer->GetExID(), m_dwSourceContainerPosition);
					pResult -> SetSourceContainerExtendID( m_lSourceContainerExtendID );
					pResult->SetDestinationContainer(pPlayer->GetType(), pPlayer->GetExID(), m_dwDestinationContainerPosition);
					pResult -> SetDestinationContainerExtendID( m_lDestinationContainerExtendID );
					return TRUE;
				}
				return FALSE;
			}
			

			if(m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_PACKET || 
				(m_lDestinationContainerExtendID>=CS2CContainerObjectMove::PEI_PACK1 && 
				m_lDestinationContainerExtendID<=CS2CContainerObjectMove::PEI_PACK5) || (IS_DEPOT_VOLCONTAINER(m_lDestinationContainerExtendID) && eDCT_Secondary != m_lDestinationContainerExtendID) )
			{

				//目标位置是否有物品
				CVolumeLimitGoodsContainer* pVolContainer=(CVolumeLimitGoodsContainer*)GetCGoodsContainer(pPlayer, m_lDestinationContainerExtendID);
				
				if(!pVolContainer)
					return FALSE;	
				if(pVolContainer->GetVolume() <= m_dwDestinationContainerPosition)
					return FALSE;
				CGoods* pDesGoods=pVolContainer->GetGoods(m_dwDestinationContainerPosition);
				if(pDesGoods)
				{
					if(pPlayer->IsInPersonalShop(pDesGoods))
						return FALSE;
					if(dwSrcGoodsNum<m_dwAmount)
						return FALSE;
					//目标位置有物品					
					if((pGoods->GetBasePropertiesIndex() == pDesGoods->GetBasePropertiesIndex()) && pGoods->GetMaxStackNumber()>1 && 
						pGoods->GetAmount()+pDesGoods->GetAmount()<=pGoods->GetMaxStackNumber())
					{
						//满足条件 物品ID相同 且 物品可叠加 且 物品叠加后不超过最大叠加数
						eOperType=CS2CContainerObjectMove::OT_MOVE_OBJECT;
#ifdef _GOODSLOG1_
						char pszGoodsLog[1024]="";
						char playerGUID[50]="";
						pPlayer->GetExID().tostring(playerGUID);
						char srcGoodsGUID[50]="";
						pGoods->GetExID().tostring(srcGoodsGUID);
						char desGoodsGUID[50]="";
						pDesGoods->GetExID().tostring(desGoodsGUID);
						_snprintf(pszGoodsLog,1024,"[操作:玩家叠加物品][玩家名:%s][玩家GUID:%s][源容器类型:%d][源容器ID:%d][源容器位置:%d][源物品ID:%d][源物品名:%s][源物品GUID:%s][源物品数量:%d]--[目标容器类型:%d][目标容器ID:%d][目标容器位置:%d][目标物品ID:%d][目标物品名:%s][目标物品GUID:%s][目标物品数量:%d]",
							pPlayer->GetName(),playerGUID,m_lSourceContainerType,m_lSourceContainerExtendID,m_dwSourceContainerPosition,pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount(),
							m_lDestinationContainerType,m_lDestinationContainerExtendID,m_dwDestinationContainerPosition,pDesGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pDesGoods->GetBasePropertiesIndex()),desGoodsGUID,pDesGoods->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif		

						//移除源物品
						CGoods* pRemovedGoods=RemoveSrcGoods(pResult);	
						if(pRemovedGoods)
						{
							DWORD dwRemoveGoodsId=pRemovedGoods->GetBasePropertiesIndex();
							const char* strRemovedGoodsName=CGoodsFactory::QueryGoodsName(dwRemoveGoodsId);
							CGUID removeGoodsGuid=pGoods->GetExID();
							DWORD dwRemovedGoodsNum=pGoods->GetAmount();
							if(pRemovedGoods!=pGoods && bCreateNewFlag && m_lSourceContainerType==TYPE_PLAYER)
							{
								CGoodsFactory::GarbageCollect(&pRemovedGoods,14);
							}

							if(pVolContainer->Add(m_dwDestinationContainerPosition,pGoods,&tPreviousContainer,pResult))
							{
								if(m_lSourceContainerType==TYPE_REGION)
								{
									pPlayer->OnGetGoods(dwRemoveGoodsId);
									GetGameLogInterface()->logT020_goods_log_pickup(pPlayer,dwRemoveGoodsId,removeGoodsGuid,strRemovedGoodsName,0,dwRemovedGoodsNum);
								}								
								pGoods=NULL;

								if(m_lSourceContainerExtendID >= eDCT_Primary && m_lDestinationContainerExtendID < eDCT_Primary)
								{
									//仓库取出								
									GetGameLogInterface()->logT060_goods_log_depository(pPlayer,dwRemoveGoodsId,removeGoodsGuid,strRemovedGoodsName,dwRemovedGoodsNum,1);
									pPlayer->OnGetGoods(dwRemoveGoodsId);								
								}
								else if(m_lSourceContainerExtendID<eDCT_Primary && m_lDestinationContainerExtendID >= eDCT_Primary)
								{
									//仓库放入
									GetGameLogInterface()->logT060_goods_log_depository(pPlayer,dwRemoveGoodsId,removeGoodsGuid,strRemovedGoodsName,dwRemovedGoodsNum,0);
									

								}
							}							
						}
						else
						{
							if(bCreateNewFlag)
							{
								CGoodsFactory::GarbageCollect(&pGoods,701);
							}
							return FALSE;
						}
					}
					else
					{
						//如果是叠加物品且操作数量不是全部数量，禁止操作
						if(pGoods->GetMaxStackNumber()>1 && dwSrcGoodsNum!=m_dwAmount)
						{
							if(bCreateNewFlag)
							{
								CGoodsFactory::GarbageCollect(&pGoods,702);
							}
							return FALSE;
						}
						//交换操作
						if(pSource->GetType()==TYPE_PLAYER)
						{
							//源容器获取
							CGoodsContainer* pSrcContainer = GetCGoodsContainer(pPlayer, m_lSourceContainerExtendID);
							if(!pSrcContainer)
							{
								if(bCreateNewFlag)
								{
									CGoodsFactory::GarbageCollect(&pGoods,701);
								}
								return FALSE;
							}
							if(m_lSourceContainerExtendID!=CS2CContainerObjectMove::PEI_EQUIPMENT)
							{
								if(pSrcContainer->CanPutIn(pDesGoods,pPlayer))
								{
									eOperType=CS2CContainerObjectMove::OT_SWITCH_OBJECT;
									CBaseObject* pSrcRemoved=pSrcContainer->Remove(pGoods,pResult);
									CBaseObject* pDesRemoved=pVolContainer->Remove(pDesGoods);
									if(pSrcRemoved && pDesRemoved)
									{
										pSrcContainer->Add(m_dwSourceContainerPosition,pDesGoods,NULL,NULL);								
										pVolContainer->Add(m_dwDestinationContainerPosition,pGoods,NULL,NULL);
									}
									else
									{
										return FALSE;
									}
									
									pResult->SetSourceContainer(TYPE_PLAYER,pPlayer->GetExID(),m_dwSourceContainerPosition);
									pResult->SetDestinationContainer(TYPE_PLAYER,pPlayer->GetExID(),m_dwDestinationContainerPosition);
									pResult->SetSourceContainerExtendID(m_lSourceContainerExtendID);
									pResult->SetSourceObjectAmount(pGoods->GetAmount());
									pResult->SetSourceObject(TYPE_GOODS,pGoods->GetExID());
									pResult->SetDestinationContainerExtendID(m_lDestinationContainerExtendID);
									pResult->SetDestinationObjectAmount(pDesGoods->GetAmount());
									pResult->SetDestinationObject(TYPE_GOODS,pDesGoods->GetExID());
									pResult->SetOperation(CS2CContainerObjectMove::OT_SWITCH_OBJECT);
#ifdef _GOODSLOG1_
									char pszGoodsLog[1024]="";
									char playerGUID[50]="";
									pPlayer->GetExID().tostring(playerGUID);
									char srcGoodsGUID[50]="";
									pGoods->GetExID().tostring(srcGoodsGUID);
									char desGoodsGUID[50]="";
									pDesGoods->GetExID().tostring(desGoodsGUID);
									_snprintf(pszGoodsLog,1024,"[操作:玩家交换物品][玩家名:%s][玩家GUID:%s][源容器类型:%d][源容器ID:%d][源容器位置:%d][源物品ID:%d][源物品名:%s][源物品GUID:%s][源物品数量:%d]--[目标容器类型:%d][目标容器ID:%d][目标容器位置:%d][目标物品ID:%d][目标物品名:%s][目标物品GUID:%s][目标物品数量:%d]",
										pPlayer->GetName(),playerGUID,m_lSourceContainerType,m_lSourceContainerExtendID,m_dwSourceContainerPosition,pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount(),
										m_lDestinationContainerType,m_lDestinationContainerExtendID,m_dwDestinationContainerPosition,pDesGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pDesGoods->GetBasePropertiesIndex()),desGoodsGUID,pDesGoods->GetAmount());
									CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif	


									if(m_lSourceContainerExtendID >= eDCT_Primary && m_lDestinationContainerExtendID < eDCT_Primary && pGoods)
									{
										// 仓库到玩家的交换	
										const char* strGoodsName=CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex());
										const char* strDesGoodsName=CGoodsFactory::QueryGoodsName(pDesGoods->GetBasePropertiesIndex());
										GetGameLogInterface()->logT060_goods_log_depository(pPlayer,pGoods->GetBasePropertiesIndex(),pGoods->GetExID(),strGoodsName,pGoods->GetAmount(),1);
										GetGameLogInterface()->logT060_goods_log_depository(pPlayer,pDesGoods->GetBasePropertiesIndex(),pDesGoods->GetExID(),strDesGoodsName,pDesGoods->GetAmount(),0);
										pPlayer->OnGetGoods(pGoods->GetBasePropertiesIndex(),pGoods);
										pPlayer->OnLoseGoods(pDesGoods);										
									}
									else if(m_lSourceContainerExtendID<eDCT_Primary && m_lDestinationContainerExtendID >= eDCT_Primary && pGoods)
									{
										//玩家到仓库的交换
										const char* strGoodsName=CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex());
										const char* strDesGoodsName=CGoodsFactory::QueryGoodsName(pDesGoods->GetBasePropertiesIndex());
										GetGameLogInterface()->logT060_goods_log_depository(pPlayer,pDesGoods->GetBasePropertiesIndex(),pDesGoods->GetExID(),strDesGoodsName,pDesGoods->GetAmount(),1);										
										GetGameLogInterface()->logT060_goods_log_depository(pPlayer,pGoods->GetBasePropertiesIndex(),pGoods->GetExID(),strDesGoodsName,pGoods->GetAmount(),0);	
										pPlayer->OnGetGoods(pDesGoods->GetBasePropertiesIndex(),pDesGoods);
										pPlayer->OnLoseGoods(pGoods);
									}
								}
							}
							else
							{
								//源是装备栏
								//判断目标物品是否能装备到装备栏位
								if(pPlayer->CanMountEquip(pDesGoods,m_dwSourceContainerPosition)==CPlayer::EQUIPFIT)
								{
									eOperType=CS2CContainerObjectMove::OT_SWITCH_OBJECT;
									pResult->SetOperation(CS2CContainerObjectMove::OT_SWITCH_OBJECT);
									if(RemoveSrcGoods(pResult) && pVolContainer->Remove(pDesGoods))
									{
										pPlayer->m_cEquipment.Add(m_dwSourceContainerPosition,pDesGoods,NULL,pResult);
										pVolContainer->Add(m_dwDestinationContainerPosition,pGoods,NULL,pResult);
									}
									else
									{
										return FALSE;
									}
									
									pDesGoods->MountBind(pPlayer->GetExID());
									pResult->SetDestinationContainerExtendID(m_lDestinationContainerExtendID);
									pResult->SetSourceObject(pGoods->GetType(),pGoods->GetExID());
									pResult->SetDestinationObject(pDesGoods->GetType(),pDesGoods->GetExID());

#ifdef _GOODSLOG1_
									char pszGoodsLog[1024]="";
									char playerGUID[50]="";
									pPlayer->GetExID().tostring(playerGUID);
									char srcGoodsGUID[50]="";
									pGoods->GetExID().tostring(srcGoodsGUID);
									char desGoodsGUID[50]="";
									pDesGoods->GetExID().tostring(desGoodsGUID);
									_snprintf(pszGoodsLog,1024,"[操作:玩家交换物品][玩家名:%s][玩家GUID:%s][源容器类型:%d][源容器ID:%d][源容器位置:%d][源物品ID:%d][源物品名:%s][源物品GUID:%s][源物品数量:%d]--[目标容器类型:%d][目标容器ID:%d][目标容器位置:%d][目标物品ID:%d][目标物品名:%s][目标物品GUID:%s][目标物品数量:%d]",
										pPlayer->GetName(),playerGUID,m_lSourceContainerType,m_lSourceContainerExtendID,m_dwSourceContainerPosition,pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount(),
										m_lDestinationContainerType,m_lDestinationContainerExtendID,m_dwDestinationContainerPosition,pDesGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pDesGoods->GetBasePropertiesIndex()),desGoodsGUID,pDesGoods->GetAmount());
									CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif	
								}
								else
								{
									//目标物品装备到装备栏上不符合条件
									return FALSE;
								}
							}
						}						
					}
				}
				else
				{
					//目标位置没有物品					
					eOperType=CS2CContainerObjectMove::OT_MOVE_OBJECT;
					CGoods* pRemovedGoods=RemoveSrcGoods(pResult);
					if(pRemovedGoods)
					{						
						if(pVolContainer->Add(m_dwDestinationContainerPosition,pGoods,&tPreviousContainer,pResult))
						{
							DWORD dwRemoveGoodsId=pRemovedGoods->GetBasePropertiesIndex();
							const char* strRemovedGoodsName=CGoodsFactory::QueryGoodsName(dwRemoveGoodsId);
							CGUID removeGoodsGuid=pGoods->GetExID();
							DWORD dwRemovedGoodsNum=pGoods->GetAmount();
							if(pRemovedGoods!=pGoods && bCreateNewFlag && m_lSourceContainerType==TYPE_PLAYER)
								CGoodsFactory::GarbageCollect(&pRemovedGoods,14);
							if(m_lSourceContainerType==TYPE_REGION && pGoods)
							{																
								pPlayer->OnGetGoods(dwRemoveGoodsId,pGoods);
								pGoods->SerializeForOldClient(&(pResult->GetObjectStream()));
								GetGameLogInterface()->logT020_goods_log_pickup(pPlayer,dwRemoveGoodsId,removeGoodsGuid,strRemovedGoodsName,0,dwRemovedGoodsNum);
							}
							else if(m_lSourceContainerExtendID >= eDCT_Primary && m_lDestinationContainerExtendID < eDCT_Primary && pGoods)
							{
								//仓库取出								
								GetGameLogInterface()->logT060_goods_log_depository(pPlayer,dwRemoveGoodsId,removeGoodsGuid,strRemovedGoodsName,dwRemovedGoodsNum,1);
								pPlayer->OnGetGoods(dwRemoveGoodsId,pGoods);								
							}
							else if(m_lSourceContainerExtendID<eDCT_Primary && m_lDestinationContainerExtendID >= eDCT_Primary && pGoods)
							{
								//仓库放入
								GetGameLogInterface()->logT060_goods_log_depository(pPlayer,dwRemoveGoodsId,removeGoodsGuid,strRemovedGoodsName,dwRemovedGoodsNum,0);
								pPlayer->OnLoseGoods(pGoods);

							}
							
						}
						
					}
					else
						return FALSE;
					

#ifdef _GOODSLOG1_
					char pszGoodsLog[1024]="";
					char playerGUID[50]="";
					pPlayer->GetExID().tostring(playerGUID);
					char srcGoodsGUID[50]="";
					pGoods->GetExID().tostring(srcGoodsGUID);					
					_snprintf(pszGoodsLog,1024,"[操作:玩家移动物品][玩家名:%s][玩家GUID:%s][源容器类型:%d][源容器ID:%d][源容器位置:%d][源物品ID:%d][源物品名:%s][源物品GUID:%s][源物品数量:%d]--[目标容器类型:%d][目标容器ID:%d][目标容器位置:%d]",
						pPlayer->GetName(),playerGUID,m_lSourceContainerType,m_lSourceContainerExtendID,m_dwSourceContainerPosition,pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount(),
						m_lDestinationContainerType,m_lDestinationContainerExtendID,m_dwDestinationContainerPosition);
					CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif	
				}
			}
			else if(m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_PACK || eDCT_Secondary == m_lDestinationContainerExtendID)
			{
				//目标位置只能是[pack1,pack5]
				if((m_dwDestinationContainerPosition<CS2CContainerObjectMove::PEI_PACK1 || m_dwDestinationContainerPosition>CS2CContainerObjectMove::PEI_PACK5) &&
					(m_dwDestinationContainerPosition<eDCT_Secondary1 || m_dwDestinationContainerPosition>eDCT_Secondary4)
					)
				{
					return FALSE;
				}

				// 限制跑商背包不能被装备于子背包栏位
				DWORD dwPackType = pGoods->GetAddonPropertyValues( GAP_PACK_TYPE, 1 );									
				if( dwPackType == SPT_BUSINESS )
				{
					pPlayer->SendNotifyMessage( "跑商背包不能被放置于子背包栏位！" );
					return FALSE;
				}

				if(m_lSourceContainerExtendID == m_dwDestinationContainerPosition && eDCT_Secondary != m_lDestinationContainerExtendID)
					return FALSE;
				if(m_lSourceContainerExtendID - eDCT_Secondary == m_dwDestinationContainerPosition - 10000 && CS2CContainerObjectMove::PEI_PACK != m_lDestinationContainerExtendID)
					return FALSE;
				if(!pPlayer->CanMountSubpack(pGoods))
					return FALSE;
				//目标位置上是否已经存在子背包
				CSubpackContainer* pSubpackContainer = NULL;
				if(m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_PACK)
					pSubpackContainer = pPlayer->getSubpackContainer();
				else
					pSubpackContainer = (CSubpackContainer*)GetCGoodsContainer(pPlayer, m_lDestinationContainerExtendID);

				if(NULL == pSubpackContainer)
				{
					return FALSE;
				}
				CGoods* pDesGoods=pSubpackContainer->GetGoods(m_dwDestinationContainerPosition);
				if(pDesGoods)
				{
					if (pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL 
						|| pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_TRADING )
					{
						char* strRes=CStringReading::LoadString(20,37);
						if(strRes)
						{
							pPlayer->SendNotifyMessage(strRes,NOTIFY_COLOR_RED);
						}	
						return FALSE;
					}
					//检查是否可交换
					if(pSubpackContainer->CanSwap(static_cast<CSubpackContainer::SUBPACK_COLUMN>(
						m_dwDestinationContainerPosition),pGoods))
					{
						
						//源容器获取
						CGoodsContainer* pSrcContainer = GetCGoodsContainer(pPlayer, m_lSourceContainerExtendID);
						if(pSrcContainer)
						{
							eOperType=CS2CContainerObjectMove::OT_SWITCH_OBJECT;
							CGoods* pSrcGoods=NULL;
							if(RemoveSrcGoods(pResult))
							{
								pSubpackContainer->Swap(static_cast<CSubpackContainer::SUBPACK_COLUMN>(
									m_dwDestinationContainerPosition), pGoods, &pSrcGoods, pResult);						
								pSrcContainer->Add(m_dwSourceContainerPosition,pSrcGoods,NULL,NULL);
							}
							else
							{
								return FALSE;
							}
							

							if (eDCT_Secondary == m_lDestinationContainerExtendID && eDCT_Primary > m_lSourceContainerExtendID)
							{
								GetGameLogInterface()->logT060_goods_log_depository(pPlayer,pGoods->GetBasePropertiesIndex(),pGoods->GetExID(),pGoods->GetName(),pGoods->GetAmount(),0);
								GetGameLogInterface()->logT060_goods_log_depository(pPlayer,pDesGoods->GetBasePropertiesIndex(),pDesGoods->GetExID(),pDesGoods->GetName(),pDesGoods->GetAmount(),1);
							}
							else if (eDCT_Secondary == m_lSourceContainerExtendID && eDCT_Primary > m_lDestinationContainerExtendID)
							{
								GetGameLogInterface()->logT060_goods_log_depository(pPlayer,pGoods->GetBasePropertiesIndex(),pGoods->GetExID(),pGoods->GetName(),pGoods->GetAmount(),1);
								GetGameLogInterface()->logT060_goods_log_depository(pPlayer,pDesGoods->GetBasePropertiesIndex(),pDesGoods->GetExID(),pDesGoods->GetName(),pDesGoods->GetAmount(),0);
							}

							pResult->SetSourceContainer(TYPE_PLAYER,pPlayer->GetExID(),m_dwSourceContainerPosition);
							pResult->SetDestinationContainer(TYPE_PLAYER,pPlayer->GetExID(),m_dwDestinationContainerPosition);
							pResult->SetSourceContainerExtendID(m_lSourceContainerExtendID);
							pResult->SetSourceObjectAmount(pGoods->GetAmount());
							pResult->SetSourceObject(TYPE_GOODS,pGoods->GetExID());
							pResult->SetDestinationContainerExtendID(m_lDestinationContainerExtendID);
							pResult->SetDestinationObjectAmount(pDesGoods->GetAmount());
							pResult->SetDestinationObject(TYPE_GOODS,pDesGoods->GetExID());
						}
					}
				}
				else
				{
					//直接添加
					eOperType=CS2CContainerObjectMove::OT_MOVE_OBJECT;
					if(RemoveSrcGoods(pResult))
					{
						pSubpackContainer->Add(m_dwDestinationContainerPosition,pGoods,&tPreviousContainer,pResult);
						if (eDCT_Secondary == m_lDestinationContainerExtendID && eDCT_Primary > m_lSourceContainerExtendID)
							GetGameLogInterface()->logT060_goods_log_depository(pPlayer,pGoods->GetBasePropertiesIndex(),pGoods->GetExID(),pGoods->GetName(),pGoods->GetAmount(),0);
					}
					else
						return FALSE;					
				}
			}	
			// 商业背包安装/交换
			else if(m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_BUSSINESPACKFIELD)
			{
				CGoods* pOldBusinessPackGoods=pPlayer->GetBusinessContainer().m_pGoods;
				if(pOldBusinessPackGoods)
				{
					//交换
					if( pPlayer->GetBusinessContainer().CanSwap( pGoods ) )
					{
						CGoodsContainer *pSrcContainer = GetCGoodsContainer( pPlayer, m_lSourceContainerExtendID );
						if( pSrcContainer != NULL )
						{
							// 将原物品从原容器移除
							pSrcContainer->Remove( pGoods->GetExID() );

							// 交换
							int ret = pPlayer->GetBusinessContainer().Swap( pGoods );
							if( ret == 1 )
							{
								eOperType = CS2CContainerObjectMove::OT_SWITCH_OBJECT;
							}
							else if( ret == 2 )
							{
								eOperType = CS2CContainerObjectMove::OT_SWITCH_FILL;
							}

							// 将原物品交换
							pSrcContainer->Add( m_dwSourceContainerPosition, pOldBusinessPackGoods, NULL, NULL );

							pResult->SetSourceContainer( TYPE_PLAYER, pPlayer->GetExID(), m_dwSourceContainerPosition );
							pResult->SetDestinationContainer( TYPE_PLAYER, pPlayer->GetExID(), m_dwDestinationContainerPosition );
							pResult->SetSourceContainerExtendID( m_lSourceContainerExtendID );
							pResult->SetSourceObjectAmount( pGoods->GetAmount() );
							pResult->SetSourceObject( TYPE_GOODS, pGoods->GetExID() );
							pResult->SetDestinationContainerExtendID( m_lDestinationContainerExtendID );
							pResult->SetDestinationObjectAmount( pOldBusinessPackGoods->GetAmount() );
							pResult->SetDestinationObject( TYPE_GOODS, pOldBusinessPackGoods->GetExID() );
							pResult->SetOperation( eOperType );
						}
					}
					else
					{
						// error
					}
				}
				else
				{
					//添加
					if(pPlayer->GetBusinessContainer().MountBusinessPackGoods(pGoods))
					{
						eOperType=CS2CContainerObjectMove::OT_MOVE_OBJECT;
						RemoveSrcGoods(pResult);
						pResult -> SetDestinationContainer( m_lDestinationContainerType, 
							m_DestinationContainerID, m_dwDestinationContainerPosition );
						pResult -> SetDestinationContainerExtendID( m_lDestinationContainerExtendID );
						pResult -> SetDestinationObject( pGoods -> GetType(),pGoods->GetExID() );
						pResult -> SetDestinationObjectAmount( pGoods -> GetAmount() );

						pResult->SetSourceContainer(m_lSourceContainerType,m_SourceContainerID,m_dwSourceContainerPosition);
						pResult->SetSourceContainerExtendID(m_lSourceContainerExtendID);
						pResult->SetSourceObject(pGoods->GetType(),m_guObjectID);
						pResult->SetSourceObjectAmount(pGoods->GetAmount());
						pResult->SetOperation(CS2CContainerObjectMove::OT_MOVE_OBJECT);
					}
				}
			}
			else if(m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_EQUIPMENT)
			{
				CGoods* pDesGoods=pPlayer->m_cEquipment.GetGoods(m_dwDestinationContainerPosition);
				if(pDesGoods)
				{
					//交换装备
					if(CS2CContainerObjectMove::PEI_MEDAL_CONTAINER == m_lSourceContainerExtendID)
					{
						CGoods *pAimMedal = pPlayer->m_cEquipment.GetGoods(m_dwDestinationContainerPosition);
						if(pPlayer->m_cEquipment.Swap((CEquipmentContainer::EQUIPMENT_COLUMN)m_dwDestinationContainerPosition, pGoods, &pAimMedal))
						{
							RemoveSrcGoods(pResult);
							pResult->SetOperation(CS2CContainerObjectMove::OT_SWITCH_OBJECT);
							pResult->SetSourceContainer(pPlayer->GetType(), pPlayer->GetExID(), m_dwSourceContainerPosition);
							pResult -> SetSourceContainerExtendID( m_lSourceContainerExtendID );
							pResult->SetDestinationContainer(pPlayer->GetType(), pPlayer->GetExID(), m_dwDestinationContainerPosition);
							pResult -> SetDestinationContainerExtendID( m_lDestinationContainerExtendID );
							pPlayer->m_MedalContainer.ReturnMedal(pAimMedal->GetBasePropertiesIndex());
						}
						else
						{
							if(pAimMedal)
								pPlayer->m_MedalContainer.SwapMedal(m_dwSourceContainerPosition, pAimMedal->GetBasePropertiesIndex());
							else
								pPlayer->m_MedalContainer.ReturnMedal(m_dwSourceContainerPosition);
							return FALSE;
						}
					}
					else if(pPlayer->CanMountEquip(pGoods,m_dwDestinationContainerPosition,TRUE)==CPlayer::EQUIPFIT)
					{
						//满足交换条件
						CGoods* pSrcGoods=NULL;
						//源容器获取
						CGoodsContainer* pSrcContainer = GetCGoodsContainer(pPlayer, m_lSourceContainerExtendID);
						if(!pSrcContainer)
							return FALSE;
						if( pPlayer->m_cEquipment.Swap( static_cast<CEquipmentContainer::EQUIPMENT_COLUMN>(
							m_dwDestinationContainerPosition), pGoods, &pSrcGoods, pResult ) )
						{
							if(RemoveSrcGoods(pResult))
							{
								pSrcContainer->Add(m_dwSourceContainerPosition,pSrcGoods,NULL,NULL);
								pGoods->MountBind(pPlayer->GetExID());
								eOperType=CS2CContainerObjectMove::OT_SWITCH_OBJECT;
								pResult->SetOperation(CS2CContainerObjectMove::OT_SWITCH_OBJECT);	
								pResult->SetDestinationContainer(pPlayer->GetType(),pPlayer->GetExID(),m_dwDestinationContainerPosition);
								pResult->SetDestinationContainerExtendID(m_lDestinationContainerExtendID);
								pResult->SetDestinationObjectAmount(pDesGoods->GetAmount());
								pResult->SetSourceContainer(pPlayer->GetType(),pPlayer->GetExID(),m_dwSourceContainerPosition);
							}
							else
							{
								return FALSE;
							}
						}				
					}
				}
				else
				{
					//直接装备
					if(pPlayer->CanMountEquip(pGoods,m_dwDestinationContainerPosition,TRUE)==CPlayer::EQUIPFIT)
					{
						if(CS2CContainerObjectMove::PEI_MEDAL_CONTAINER != m_lSourceContainerExtendID)
							RemoveSrcGoods(pResult);
						if(pPlayer->GetEquipmentContainer()->Add(m_dwDestinationContainerPosition,pGoods,&tPreviousContainer,pResult))
						{
							pGoods->MountBind(pPlayer->GetExID());
							pResult->SetDestinationContainer(pPlayer->GetType(),pPlayer->GetExID(),m_dwDestinationContainerPosition);
							pResult->SetSourceContainer(m_lSourceContainerType,m_SourceContainerID,m_dwSourceContainerPosition);						
							eOperType=CS2CContainerObjectMove::OT_MOVE_OBJECT;
						}
					}
					else
					{
						//! 勋章
						if(m_lSourceContainerExtendID==CS2CContainerObjectMove::PEI_MEDAL_CONTAINER)
							pPlayer->m_MedalContainer.ReturnMedal(pGoods->GetBasePropertiesIndex());
					}
				}				
			}
			else if(m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_WALLET || 
				m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_SILVERWALLET || 
				eDCT_Gold == m_lDestinationContainerExtendID || 
				eDCT_Silver == m_lDestinationContainerExtendID)
			{
				//金币是没有交换操作的，直接检测是否超过上限，直接添加
				CWallet *pWallet = (CWallet*)GetCGoodsContainer(pPlayer, m_lDestinationContainerExtendID);
				if(NULL == pWallet)
					return FALSE;

				DWORD dwMaxMoney=CGoodsFactory::QueryGoodsMaxStackNumber( CGoodsFactory::GetGoldCoinIndex());
				DWORD dwMaxSilver=CGoodsFactory::QueryGoodsMaxStackNumber(CGoodsFactory::GetSilverIndex());	
				if(m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_WALLET)
				{
					if(pPlayer->GetMoney()+pGoods->GetAmount()>dwMaxMoney)
					{
						//金币超过上限
						return FALSE;
					}

				}

				if(m_lDestinationContainerExtendID==CS2CContainerObjectMove::PEI_SILVERWALLET)
				{
					if(pPlayer->GetSilver()+pGoods->GetAmount()>dwMaxSilver)
					{
						//银币超过上限
						return FALSE;
					}
				}

#ifdef _GOODSLOG1_
				char pszGoodsLog[1024]="";
				char playerGUID[50]="";
				pPlayer->GetExID().tostring(playerGUID);
				char srcGoodsGUID[50]="";
				pGoods->GetExID().tostring(srcGoodsGUID);					
				_snprintf(pszGoodsLog,1024,"[操作:玩家移动物品][玩家名:%s][玩家GUID:%s][源容器类型:%d][源容器ID:%d][源容器位置:%d][源物品ID:%d][源物品名:%s][源物品GUID:%s][源物品数量:%d]--[目标容器类型:%d][目标容器ID:%d][目标容器位置:%d]",
					pPlayer->GetName(),playerGUID,m_lSourceContainerType,m_lSourceContainerExtendID,m_dwSourceContainerPosition,pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount(),
					m_lDestinationContainerType,m_lDestinationContainerExtendID,m_dwDestinationContainerPosition);
				CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif	

				DWORD dwRemoveGoodsId=pGoods->GetBasePropertiesIndex();
				const char* strRemovedGoodsName=CGoodsFactory::QueryGoodsName(dwRemoveGoodsId);
				CGUID removeGoodsGuid=pGoods->GetExID();
				DWORD dwRemovedGoodsNum=pGoods->GetAmount();

				if(m_lSourceContainerType==TYPE_REGION)
				{
					GetGameLogInterface()->logT020_goods_log_pickup(pPlayer,dwRemoveGoodsId,removeGoodsGuid,strRemovedGoodsName,0,dwRemovedGoodsNum);
				}															

				if(RemoveSrcGoods(pResult))
				{
					BOOL bSuccess=pWallet->Add(m_dwDestinationContainerPosition,pGoods,&tPreviousContainer,pResult);
					eOperType=CS2CContainerObjectMove::OT_MOVE_OBJECT;
				}
				else
				{
					return FALSE;
				}
			}
			else if(CS2CContainerObjectMove::PEI_MEDAL_CONTAINER == m_lDestinationContainerExtendID)
			{
				if(eMCRV_Succeed == pPlayer->m_MedalContainer.ReturnMedal(pGoods->GetBasePropertiesIndex()))
				{
					RemoveSrcGoods(pResult);
					pResult->SetOperation(CS2CContainerObjectMove::OT_MOVE_OBJECT);
					pResult->SetSourceContainer(pPlayer->GetType(), pPlayer->GetExID(), m_dwSourceContainerPosition);
					pResult -> SetSourceContainerExtendID( m_lSourceContainerExtendID );
					pResult->SetDestinationContainer(pPlayer->GetType(), pPlayer->GetExID(), m_dwDestinationContainerPosition);
					pResult -> SetDestinationContainerExtendID( m_lDestinationContainerExtendID );
				}
				else
					return FALSE;
			}


			//加注释那段			
		}
		else if(pDestination->GetType()==TYPE_SESSION)
		{
			if(bCreateNewFlag && pGoods->GetBasePropertiesIndex()!=CGlobeSetup::GetSetup()->dwGoldGoodsId)
				return FALSE;
			CSession* pSession = dynamic_cast<CSession*>( pDestination );
			if( pSession )
			{
				CPlug* pPlug = pSession -> QueryPlugByID( m_lDestinationContainerExtendID >> 8 );
				if(!pPlug)
					return FALSE;
				CEquipmentEnchase* pEnchasePlug=dynamic_cast<CEquipmentEnchase*>(pPlug);
				CTrader* pTrader=dynamic_cast<CTrader*>(pPlug);
				if(pTrader)
				{
					if(pTrader->IsLocked())
						return FALSE;
				}
				CPlayer* pPlayer = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
				BOOL bForst=GoodsExManage::GetInstance().IsFrost(pGoods);
				if(bForst && !pEnchasePlug)
				{
					char* strRes=CStringReading::LoadString(20,25);
					if(strRes)
					{
						pPlayer->SendNotifyMessage(strRes);
					}									
				}
				BOOL bBind=GoodsExManage::GetInstance().IsBind(pGoods);
				if(bBind && !pEnchasePlug)
				{
					char* strRes=CStringReading::LoadString(20,26);
					if(strRes)
					{
						pPlayer->SendNotifyMessage(strRes);
					}						
				}
				if( (pPlug && !bForst && !bBind) || pEnchasePlug)
				{
					CGoodsContainer* pContainer = dynamic_cast<CGoodsContainer*>(
						pPlug -> GetContainer(m_lDestinationContainerExtendID & 0x000000ff) );
					if(!pContainer)
						return FALSE;
					CPersonalShopSeller* pSeller=dynamic_cast<CPersonalShopSeller*>(pPlug);


					if(pSeller==NULL && pContainer && pPlayer && pPlayer -> GetType() == m_lSourceContainerType &&
						pPlayer -> GetExID() == m_SourceContainerID )
					{
						CContainer::tagPreviousContainer tPreviousContainer;
						//##如果是从手上拿起出的,那么就记录先前容器的信息.
						if( m_lSourceContainerType == TYPE_PLAYER )
						{
							tPreviousContainer.lContainerType		= m_lSourceContainerType;
							tPreviousContainer.ContainerID			= m_SourceContainerID;								
							tPreviousContainer.lContainerExtendID	= m_lSourceContainerExtendID;
							tPreviousContainer.dwGoodsPosition		= m_dwSourceContainerPosition;								
						}

						if( tPreviousContainer.lContainerExtendID == 0 )
						{
							//##可能是开始的时候就把物品放在手上，将这个容器设置为背包
							tPreviousContainer.lContainerExtendID	= CS2CContainerObjectMove::PEI_PACKET;
							tPreviousContainer.dwGoodsPosition		= 0xFFFFFFFF;
						}						
						bResult = pContainer -> Add( m_dwDestinationContainerPosition, 
							pGoods, &tPreviousContainer, pResult );
						
						if(pTrader)
						{
							/*
							if(bCreateNewFlag)
							{
								CGoodsFactory::GarbageCollect(&pGoods,50);
							}*/							
							pPlayer->AddPersonalShopGoodsData(pGoods);
						}										
					}					
				}				
			}
		}

		if(eOperType==CS2CContainerObjectMove::OT_MOVE_OBJECT)
		{
			if( pResult )
			{
				pResult -> SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );											
				pResult -> SetDestinationContainerExtendID( m_lDestinationContainerExtendID );								

				if(GT_MEDAL == eGoodsType)
				{
					pResult -> SetSourceContainer(m_lSourceContainerType, m_SourceContainerID, m_dwSourceContainerPosition);
					pResult -> SetSourceContainerExtendID( m_lSourceContainerExtendID );
					pResult -> SetSourceObject( m_lObjectType, m_guObjectID);

					CPlayer* pPlayer = dynamic_cast<CPlayer*>( pSource );
					if(NULL != pPlayer)
						pPlayer->UpdateTitle(TRUE);
				}

			}

			//##保留最后一次操作的源容器.
			CPlayer* pPlayer = dynamic_cast<CPlayer*>( pSource );
			if( pPlayer )
			{
				
			}
			else
			{
				CSession* pSession = dynamic_cast<CSession*>( pSource );
				if( pSession )
				{
					CPlug* pPlug = pSession -> QueryPlugByID( m_lSourceContainerExtendID >> 8 );
					if( pPlug )
					{
						pPlayer = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
						if( pPlayer )
						{
							
						}
					}
				}
			}

			bResult = TRUE;			

			if( m_lDestinationContainerType == TYPE_PLAYER )
			{
				CPlayer* pPlayer=dynamic_cast<CPlayer*>(GetDestination());
				if(pPlayer)
				{					
					GetGame()->UpdatePlayerPack(pPlayer,m_lDestinationContainerExtendID,m_dwDestinationContainerPosition);				
				}
			}
			if( m_lSourceContainerType == TYPE_PLAYER )
			{
				CPlayer* pPlayer=dynamic_cast<CPlayer*>(GetSource());
				if(pPlayer)
				{					
					GetGame()->UpdatePlayerPack(pPlayer,m_lSourceContainerExtendID,m_dwSourceContainerPosition);
				}
			}

			//! 仓库相关日志
			if (eDCT_Gold == m_lSourceContainerExtendID)
			{
				GetGameLogInterface()->logT070_goods_log_bank(pPlayer, 1, dwPrice);
			}					
			else if (eDCT_Gold == m_lDestinationContainerExtendID)
			{
				GetGameLogInterface()->logT070_goods_log_bank(pPlayer, 0, dwPrice);
			}
		}
		else if(eOperType==CS2CContainerObjectMove::OT_SWITCH_OBJECT)
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>( pSource );
			if(pPlayer)
			{
				pPlayer->UpdateTitle(TRUE);				
				GetGame()->UpdatePlayerPack(pPlayer,m_lDestinationContainerExtendID,m_dwDestinationContainerPosition);				
				GetGame()->UpdatePlayerPack(pPlayer,m_lSourceContainerExtendID,m_dwSourceContainerPosition);				
			}			
		}
	}
	return bResult;
}


CGoodsContainer* CC2SContainerObjectMove::GetCGoodsContainer(CPlayer *pPlayer, long lContainerExtendID)
{
	switch( lContainerExtendID )
	{
	case CS2CContainerObjectMove::PEI_PACKET:
		return pPlayer ->getOriginPack();
	case CS2CContainerObjectMove::PEI_EQUIPMENT:
		return &( pPlayer -> m_cEquipment );			
	case CS2CContainerObjectMove::PEI_WALLET:
		return &( pPlayer -> m_cWallet );
	case CS2CContainerObjectMove::PEI_SILVERWALLET:
		return &(pPlayer->m_cSilverWallet);
	case CS2CContainerObjectMove::PEI_GEM:
		return &( pPlayer -> m_cGem );
	case CS2CContainerObjectMove::PEI_PACK:
		return pPlayer->getSubpackContainer();
	case CS2CContainerObjectMove::PEI_PACK1:
		return pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer;
	case CS2CContainerObjectMove::PEI_PACK2:
		return pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer;
	case CS2CContainerObjectMove::PEI_PACK3:
		return pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer;
	case CS2CContainerObjectMove::PEI_PACK4:
		return pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer;
	case CS2CContainerObjectMove::PEI_PACK5:
		return pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer;
	}

	//! 得到仓库容器
	return pPlayer->GetPlayerDepot()->FindContainer(lContainerExtendID);
}



//! 日志
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
<!--================================================================-->
<!--仓库操作日志-->
<Table name="t060_goods_log_depository" log_no="60" log_pos="1">
	<item type="client_head"     head_name=""></item>
	<item type="player"   head_name=""></item>
	<item type="goods"    head_name=""></item>
	<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--类型（0：存；1：取）-->
	<item type="alone"    Item_name="goods_num"       data_type="int32"></item><!--物品数量-->
</Table>
*/
bool LogicLogInterface::logT060_goods_log_depository(CPlayer *pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,DWORD dwGoodsNum, long lActType)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) return true;

	if(NULL == m_pLogClient)
		return false;
	if (NULL == pPlayer) return false;

	string strTime;
	GetCurrTimeString(strTime);

	return m_pLogClient->SendLogToServer( 
		60, strTime.c_str(),
		&(pPlayer->GetExID()),	pPlayer->GetName(),
		dwGoodsId, &goodsGuid,strGoodsName, 
		lActType, dwGoodsNum
		);
}

/*
<!--================================================================-->
<!--银行操作日志-->
<Table name="t070_goods_log_bank" log_no="70" log_pos="1">
	<item type="client_head"     head_name=""></item>
	<item type="player"   head_name=""></item>
	<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--类型（0：存；1：取）-->
	<item type="alone"    Item_name="number"          data_type="int32"></item><!--数量-->
</Table>
*/
bool LogicLogInterface::logT070_goods_log_bank(CPlayer *pPlayer, long lActType, long lNum)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) return true;

	if(NULL == m_pLogClient)
		return false;
	if (NULL == pPlayer) return false;

	string strTime;
	GetCurrTimeString(strTime);

	return m_pLogClient->SendLogToServer( 
		70, strTime.c_str(),
		&(pPlayer->GetExID()),	pPlayer->GetName(),
		lActType, lNum
		);
}

/*
<!--物品交易日志-->
<Table name="t010_goods_log_trade" log_no="10" log_pos="1">
<item type="client_head"     head_name=""></item>
<item type="pos"      head_name=""></item>
<item type="player"   head_name="sponsor"></item>
<item type="player"   head_name="participator"></item>
<item type="goods"    head_name=""></item>
<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--交易类型（0：普通；1个人商店）-->
<item type="alone"    Item_name="goods_price"     data_type="int32"></item><!--物品价格-->
<item type="alone"    Item_name="goods_num"       data_type="int32"></item><!--物品数量-->
</Table>
*/
bool LogicLogInterface::logT010_goods_log_trade(CPlayer* pPlayer1,CPlayer* pPlayer2,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,long lActType,long lPrice,long lNum,DWORD dwMoneyType)
{
	const char* szOriginName=CGoodsFactory::QueryGoodsOriginalName(dwGoodsId);
	if(!szOriginName)
		return false;
	string strOriginName(szOriginName);
	if(strOriginName!="GDMONEY" && strOriginName.substr(0,2)=="GD")
		return false;
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(pPlayer1==NULL || pPlayer2==NULL)
		return false;
	CRegion *pRegion = dynamic_cast<CRegion*>(GetGame()->FindRegion(pPlayer1->GetRegionGUID()));
	if(!pRegion)
		return false;

	string strTime;
	GetCurrTimeString(strTime);

	float posX = pPlayer1->GetTileX();
	float posY = pPlayer1->GetTileY();

	return m_pLogClient->SendLogToServer(
		10, strTime.c_str(), 
		pRegion->GetID(),pRegion->GetName(),&posX,&posY,
		&(pPlayer1->GetExID()),pPlayer1->GetName(),
		&(pPlayer2->GetExID()),pPlayer2->GetName(),
		dwGoodsId, &goodsGuid,strGoodsName,
		lActType,lPrice,lNum,dwMoneyType);
}

/*
<Table name="t020_goods_log_pickup" log_no="20" log_pos="1">
<item type="client_head"     head_name=""></item>
<item type="pos"      head_name=""></item>
<item type="player"   head_name=""></item>
<item type="goods"    head_name=""></item>
<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：拾取；1：丢弃）-->
<item type="alone"    Item_name="goods_num"       data_type="int32"></item><!--物品数量-->
</Table>
*/
bool LogicLogInterface::logT020_goods_log_pickup(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,long lActType,long lNum)
{
	const char* szOriginName=CGoodsFactory::QueryGoodsOriginalName(dwGoodsId);
	if(!szOriginName)
		return false;
	string strOriginName(szOriginName);
	if(strOriginName!="GDMONEY" && strOriginName.substr(0,2)=="GD")
		return false;

	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;
	CRegion *pRegion = dynamic_cast<CRegion*>(GetGame()->FindRegion(pPlayer->GetRegionGUID()));
	if(NULL == pRegion)
		return false;


	string strTime;
	GetCurrTimeString(strTime);

	float posX = pPlayer->GetTileX();
	float posY = pPlayer->GetTileY();
	return m_pLogClient->SendLogToServer(
		20, strTime.c_str(),
		pRegion->GetID(),pRegion->GetName(),&posX,&posY,
		&(pPlayer->GetExID()),pPlayer->GetName(),dwGoodsId,&goodsGuid,strGoodsName,lActType,lNum,pPlayer->GetMoney(),pPlayer->GetSilver());
}

/*
<Table name="t027_goods_log_pickup" log_no="20" log_pos="1">
<item type="client_head"     head_name=""></item>
<item type="pos"      head_name=""></item>
<item type="player"   head_name=""></item>
<item type="goods"    head_name=""></item>
<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：拾取；1：丢弃）-->
<item type="alone"    Item_name="goods_num"       data_type="int32"></item><!--物品数量-->
</Table>
*/
bool LogicLogInterface::logT027_goods_log_pickup(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,long lActType,long lNum)
{
	const char* szOriginName=CGoodsFactory::QueryGoodsOriginalName(dwGoodsId);
	if(!szOriginName)
		return false;
	string strOriginName(szOriginName);
	if(strOriginName!="GDMONEY" && strOriginName.substr(0,2)=="GD")
		return false;

	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;
	CRegion *pRegion = dynamic_cast<CRegion*>(GetGame()->FindRegion(pPlayer->GetRegionGUID()));
	if(NULL == pRegion)
		return false;


	string strTime;
	GetCurrTimeString(strTime);

	float posX = pPlayer->GetTileX();
	float posY = pPlayer->GetTileY();
	return m_pLogClient->SendLogToServer(
		27, strTime.c_str(),
		pRegion->GetID(),pRegion->GetName(),&posX,&posY,
		&(pPlayer->GetExID()),pPlayer->GetName(),dwGoodsId,&goodsGuid,strGoodsName,lActType,lNum,pPlayer->GetMoney(),pPlayer->GetSilver());
}


bool LogicLogInterface::logT022_goods_log_consume(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,DWORD dwNum,DWORD dwUsedNum,DWORD dwPlace,DWORD dwPos)
{
	const char* szOriginName=CGoodsFactory::QueryGoodsOriginalName(dwGoodsId);
	if(!szOriginName)
		return false;
	string strOriginName(szOriginName);
	if(strOriginName.substr(0,3)=="PPH" || strOriginName.substr(0,3)=="PPM" 
										|| strOriginName=="QPG399" 
										|| strOriginName=="GZ0010")
	{
		return false;
	}

	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;
	CRegion *pRegion = dynamic_cast<CRegion*>(GetGame()->FindRegion(pPlayer->GetRegionGUID()));
	if(NULL == pRegion)
		return false;


	string strTime;
	GetCurrTimeString(strTime);

	float posX = pPlayer->GetTileX();
	float posY = pPlayer->GetTileY();
	return m_pLogClient->SendLogToServer(
		22, strTime.c_str(),
		pRegion->GetID(),pRegion->GetName(),&posX,&posY,
		&(pPlayer->GetExID()),pPlayer->GetName(),dwGoodsId,&goodsGuid,strGoodsName,dwNum,dwUsedNum,dwPlace,dwPos);
}


bool LogicLogInterface::logT023_goods_log_repair(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,int nActType)
{	
	string strGoodsName="-";
	const char* szGoodsName=CGoodsFactory::QueryGoodsName(dwGoodsId);
	if(szGoodsName)
		strGoodsName=szGoodsName;
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;

	string strTime;
	GetCurrTimeString(strTime);

	return m_pLogClient->SendLogToServer(23,strTime.c_str(),&(pPlayer->GetExID()),pPlayer->GetName(),dwGoodsId,&goodsGuid,strGoodsName.c_str(),nActType);
}

bool LogicLogInterface::logT021_goods_log_enchase(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,long lActType,DWORD dwHolePos,
							   DWORD dwCardId,const char* strCardName,DWORD dwReserveCard)
{
	const char* szOriginName=CGoodsFactory::QueryGoodsOriginalName(dwGoodsId);
	if(!szOriginName)
		return false;
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;
	CRegion *pRegion = dynamic_cast<CRegion*>(GetGame()->FindRegion(pPlayer->GetRegionGUID()));
	if(NULL == pRegion)
		return false;
	string strTime;
	GetCurrTimeString(strTime);

	float posX = pPlayer->GetTileX();
	float posY = pPlayer->GetTileY();

	return m_pLogClient->SendLogToServer(
		21, strTime.c_str(),
		pRegion->GetID(),pRegion->GetName(),&posX,&posY,
		&(pPlayer->GetExID()),pPlayer->GetName(),dwGoodsId,&goodsGuid,strGoodsName,lActType,dwHolePos,dwCardId,strCardName,dwReserveCard);
}

bool LogicLogInterface::logT501_Ticket_consume_log(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,DWORD dwPrice,DWORD dwGoodsNum,DWORD dwMoney,DWORD dwTicket,DWORD dwActType)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;
	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(501,strTime.c_str(),&(pPlayer->GetExID()),pPlayer->GetName(),dwGoodsId,&goodsGuid,strGoodsName,dwPrice,dwGoodsNum,dwMoney,dwTicket,dwActType);
}

bool LogicLogInterface::logT504_weimian_log(CPlayer* pPlayer,int nNum,int nActType,int nFlag)
{
	if(GetGame()->GetSetup()->lUseLogServer==0)
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;
	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(504,strTime.c_str(),&(pPlayer->GetExID()),pPlayer->GetName(),nNum,nActType,nFlag);
}

bool LogicLogInterface::logT502_Ticket_gain_log(CPlayer* pPlayer,int nGainNum)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;
	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(502,strTime.c_str(),&(pPlayer->GetExID()),pPlayer->GetName(),nGainNum,pPlayer->GetTicket());
}

bool LogicLogInterface::logT513_Ticket_gain_log(CPlayer* pPlayer,int nGainNum,float fRatio)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;
	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(513,strTime.c_str(),&(pPlayer->GetExID()),pPlayer->GetName(),nGainNum,&fRatio, pPlayer->GetTicket());
}

bool LogicLogInterface::logT902_player_progress_chg_log(CPlayer* pPlayer,int nOldProgress,int nNewProgress)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;
	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(902,strTime.c_str(),&(pPlayer->GetExID()),pPlayer->GetName(),nOldProgress,nNewProgress);
}

/*
<!--与NPC的物品交易日志-->
<Table name="t030_goods_log_npc" log_no="30" log_pos="1">
<item type="client_head"     head_name=""></item>
<item type="pos"      head_name=""></item>
<item type="player"   head_name=""></item>
<item type="goods"    head_name=""></item>
<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：玩家买；1：玩家卖）-->
<item type="alone"    Item_name="goods_price"     data_type="int32"></item><!--物品价格-->
<item type="alone"    Item_name="goods_num"       data_type="int32"></item><!--物品数量-->
</Table>
*/
bool LogicLogInterface::logT030_goods_log_npc(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,long lActType,long lPrice,long lNum)
{
	const char* szOriginName=CGoodsFactory::QueryGoodsOriginalName(dwGoodsId);
	if(!szOriginName)
		return false;
	string strOriginName(szOriginName);
	if(strOriginName!="GDMONEY" && strOriginName!="GDMONEYS" && strOriginName.substr(0,2)=="GD")
		return false;
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	CRegion *pRegion = dynamic_cast<CRegion*>(GetGame()->FindRegion(pPlayer->GetRegionGUID()));
	if(!pRegion)
		return false;

	string strTime;
	GetCurrTimeString(strTime);

	float posX = pPlayer->GetTileX();
	float posY = pPlayer->GetTileY();
	
	
	return m_pLogClient->SendLogToServer(
		30, strTime.c_str(),
		pRegion->GetID(),pRegion->GetName(),&posX,&posY,
		&(pPlayer->GetExID()),pPlayer->GetName(),
		dwGoodsId, &goodsGuid,strGoodsName,
		lActType,lPrice,lNum,pPlayer->GetMoney(),pPlayer->GetSilver());
		
}
/*<!--与NPC通过位面精华交易日志-->
<Table name="t031_goods_wm_npc" log_no="31" log_pos="1">	
<item type="client_head"     head_name=""></item>
<item type="player"   head_name=""></item>
<item type="goods"    head_name=""></item>  
<item type="alone"	  Item_name="Goods_Num"		  data_type="int32"></item><!--获取物品的数量-->
<item type="alone"	  Item_name="wm_Num"		  data_type="int32"></item><!--位面数量-->
</Table>*/
bool LogicLogInterface::logT031_goods_wm_npc(CPlayer* pPlayer,DWORD dwGoodsID,const CGUID& guid,LONG lGoodsNum,LONG lWmNum)
{
	const char* szOriginName=CGoodsFactory::QueryGoodsName(dwGoodsID);
	if(!szOriginName)
		return false;
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(31,strTime.c_str(),
		&(pPlayer->GetExID()),pPlayer->GetName(),
		dwGoodsID,&guid,szOriginName,lGoodsNum,lWmNum);
}

/*
<!--物品消失日志-->
<Table name="t040_goods_log_disappear" log_no="40" log_pos="1">
<item type="client_head"     head_name=""></item>
<item type="player"   head_name=""></item>
<item type="goods"    head_name=""></item>
<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--类型（0：损坏；1：升级消失）-->
</Table>
*/
bool LogicLogInterface::logT040_goods_log_disappear(CPlayer* pPlayer,CGoods* pGoods,long lActType)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer || NULL == pGoods)
		return false;
	
	string strTime;
	GetCurrTimeString(strTime);

	float posX = pPlayer->GetTileX();
	float posY = pPlayer->GetTileY();
	return m_pLogClient->SendLogToServer(
		40, strTime.c_str(),
		&(pPlayer->GetExID()),pPlayer->GetName(),
		pGoods->GetBasePropertiesIndex(), &(pGoods->GetExID()), pGoods->GetName(),
		lActType);
}

/*
<!--物品掉落日志-->
<Table name="t050_goods_log_fall" log_no="50" log_pos="1">
<item type="client_head"     head_name=""></item>
<item type="pos"      head_name=""></item>
<item type="player"   head_name=""></item>
<item type="goods"    head_name=""></item>
</Table>
*/
bool LogicLogInterface::logT050_goods_log_fall(CPlayer* pPlayer,CGoods* pGoods)
{
	if(!pGoods)
		return false;
	const char* szOriginName=CGoodsFactory::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex());
	if(!szOriginName)
		return false;
	string strOriginName(szOriginName);
	if(strOriginName!="GDMONEY" && strOriginName!="GDMONEYS" && strOriginName.substr(0,2)=="GD")
		return false;
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(!pPlayer)
		return false;
	
	CRegion *pRegion = dynamic_cast<CRegion*>(GetGame()->FindRegion(pPlayer->GetRegionGUID()));
	if(!pRegion)
		return false;

	string strTime;
	GetCurrTimeString(strTime);

	float posX = pPlayer->GetTileX();
	float posY = pPlayer->GetTileY();
	return m_pLogClient->SendLogToServer(
		50, strTime.c_str(),
		pRegion->GetID(),pRegion->GetName(),&posX,&posY,
		&(pPlayer->GetExID()),pPlayer->GetName(),
		pGoods->GetBasePropertiesIndex(), &(pGoods->GetExID()), pGoods->GetName());
}

