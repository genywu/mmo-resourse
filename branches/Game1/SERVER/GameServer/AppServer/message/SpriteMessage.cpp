#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\nets\Servers.h"
#include "..\player.h"
#include "../SpriteSystem.h"
#include "../Region.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"
#include "..\serverregion.h"
#include "../../public/PlayerSetupOnServer.h"


//! 响应客户端的小精灵消息
void OnSpriteMessage_ForClient(CMessage *pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_C2S_SPRITE_AutoSupply_OnOff_Request://! （暂不使用）自动补给系统：功能开关请求
		{

		}
		break;
	case MSG_C2S_SPRITE_AutoSupply_ChangeSetup_Request://! 自动补给系统：修改一个补给设置请求
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				pPlayer->m_SpriteSystem.OnChangeAutoSupply(pMsg);
				
			}	
		}
		break;
    case MSG_C2S_SPRITE_AutoFight_ChangeSetup_Request://! 自动补给系统：修改一个辅助挂机设置请求
        {
            CPlayer *pPlayer = pMsg->GetPlayer();
            if (NULL != pPlayer)
            {
                pPlayer->m_SpriteSystem.OnChangeAutoFight(pMsg);
            }
        }
        break;
    case MSG_C2S_SPRITE_AutoRepair://! 自动补给系统：自动修理请求
        {
            CPlayer *pPlayer = pMsg->GetPlayer();
            if (NULL != pPlayer)
            {
                //pPlayer->m_SpriteSystem.OnAutoRepair(pMsg);

                if( pPlayer -> IsDied() )
                {
                    //CancelTrading( pPlayer );
                    pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,1));
                    return;
                }

                bool bCan = pPlayer->m_SpriteSystem.CanAutoRepair();
                if (!bCan)
                {
                    //银币不足
                    pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,35));
                    return;
                }

                CServerRegion* pRegion = pMsg -> GetRegion();
                //##其次，开始修理物品
                LONG lContainerId=pMsg->GetLong();
                LONG lPos=pMsg->GetLong();			


                //根据ID取容器对象
                CGoodsContainer* pContainer = NULL;
                CGoods* pGoods=NULL;
                switch(lContainerId)
                {
                case CS2CContainerObjectMove::PEI_PACKET:
                    pContainer = pPlayer -> getOriginPack();
                    break;
                case CS2CContainerObjectMove::PEI_EQUIPMENT:
                    pContainer = &( pPlayer -> m_cEquipment );
                    break;						
                    /*			case CS2CContainerObjectMove::PEI_DEPOT:										
                    pContainer = &( pPlayer -> m_cDepot);					
                    break;	*/			
                case CS2CContainerObjectMove::PEI_PACK:
                    pContainer =  pPlayer ->getSubpackContainer();
                    break;
                case CS2CContainerObjectMove::PEI_PACK1:
                    pContainer=pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer;
                    break;
                case CS2CContainerObjectMove::PEI_PACK2:
                    pContainer=pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer;
                    break;
                case CS2CContainerObjectMove::PEI_PACK3:
                    pContainer=pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer;
                    break;
                case CS2CContainerObjectMove::PEI_PACK4:
                    pContainer=pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer;
                    break;
                case CS2CContainerObjectMove::PEI_PACK5:
                    pContainer=pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer;
                    break;
                }
                if( pContainer )
                {
                    pGoods=pContainer->GetGoods(lPos);
                }
                //物品Guid
               /* CGUID goodsID;
                pMsg->GetGUID(goodsID);                
                CServerRegion* pRegion = pMsg -> GetRegion();
                CGoods *pGoods = NULL;
                pGoods = pPlayer->GetGoodsById(goodsID);*/

                if(pGoods && pGoods -> CanReparied())
                {
                    LONG lCurrentDurability	= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 2 );
                    DWORD dwPrice = CGoodsFactory::CalculateRepairPrice( pGoods );
                    dwPrice = static_cast<DWORD>( dwPrice * ( 1 + pRegion -> GetTaxRate() / 100 ) );
                    if( pPlayer ->GetSilver() >= dwPrice )
                    {
                        if( CGoodsFactory::RepairEquipment(pGoods) )
                        {
                            GetGameLogInterface()->logT023_goods_log_repair(pPlayer,pGoods->GetBasePropertiesIndex(),pGoods->GetExID(),1);
                            pPlayer ->SetSilver( pPlayer ->GetSilver() - dwPrice );
                            if (pContainer==&( pPlayer -> m_cEquipment )&&lCurrentDurability==0)
                            {
                                DWORD  dwPosition = 0;
                                pPlayer->GetEquipmentContainer()->QueryGoodsPosition(pGoods,dwPosition);
                                pPlayer->MountEquipEx(pGoods,true);
                                pPlayer->UpdateAttribute(TRUE);
                                pPlayer->ProduceEvent(ET_EquipMount,
                                    CS2CContainerObjectMove::PEI_EQUIPMENT,
                                    dwPosition,
                                    pGoods->GetGoodsBaseType(), 
                                    pGoods-> GetAddonPropertyValues( GAP_WEAPON_CATEGORY, 1 ),
                                    (void*)CGoodsFactory::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex()));
                            }
                            CMessage msg(MSG_S2C_SHOP_REPAIR);
                            msg.Add((char)SHOP_REPAIR_SUCCEED);
                            msg.Add(lContainerId);
                            msg.Add(lPos);
                            msg.Add(dwPrice);
                            msg.SendToPlayer( pPlayer -> GetExID() );
                            return;
                        }
                        else
                        {
                            pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,8));
                        }
                    }
                    else
                    {
                        pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,9));
                    }
                }
            }
        }
        break;
	//!-------------------------------------------------------------------------------------------
	//! 非小精灵系统消息
	case MSG_C2S_SaveNewSetupData_Request:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				tagSetupOnServer SetupOnServer;
				pMsg->GetEx(&SetupOnServer, sizeof(tagSetupOnServer));
				BOOL bRevert = pPlayer->m_SetupOnServer.SetSetup(SetupOnServer);
				CMessage msg(MSG_S2C_SaveNewSetupData_Re);
				msg.Add((LONG)bRevert);
				msg.SendToPlayer(pPlayer->GetExID());
			}
		}
		break;
		  //! 请求打开日程表
	case	MSG_C2S_Calendar_Request:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				const char* strScript = CGlobeSetup::GetSetup()->pszCalendar;
				if(strScript)
				{
					stRunScript sScript;
					sScript.pszFileName = (char*)strScript;
					sScript.desShape = pPlayer;
					sScript.pRegion =  dynamic_cast<CRegion*>(pPlayer->GetFather());
					sScript.srcShape	= pPlayer;
					RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
				}
			}
		}
		break;
			//! 请求打开迦纳维魔盒
	case	MSG_C2S_JanaweBox_Request:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				const char* strScript = CGlobeSetup::GetSetup()->pszMagicBox;
				if(strScript)
				{
					stRunScript sScript;
					sScript.pszFileName = (char*)strScript;
					sScript.desShape = pPlayer;
					sScript.pRegion =  dynamic_cast<CRegion*>(pPlayer->GetFather());
					sScript.srcShape	= pPlayer;
					RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
				}
			}
		}
		break;
			//! 问答与奖励
	case	MSG_C2S_Answer_Request:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				const char* strScript = CGlobeSetup::GetSetup()->pszAnswerRequest;
				if(strScript)
				{
					stRunScript sScript;
					sScript.pszFileName = (char*)strScript;
					sScript.desShape = pPlayer;
					sScript.pRegion =  dynamic_cast<CRegion*>(pPlayer->GetFather());
					sScript.srcShape	= pPlayer;
					RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
				}
			}
		}
	break;
		//帮助
	case	MSG_C2S_Help_Request:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				const char* strScript = CGlobeSetup::GetSetup()->pszHelpRequest;
				if(strScript)
				{
					stRunScript sScript;
					sScript.pszFileName = (char*)strScript;
					sScript.desShape = pPlayer;
					sScript.pRegion =  dynamic_cast<CRegion*>(pPlayer->GetFather());
					sScript.srcShape	= pPlayer;
					RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
				}
			}
		}
		break;
	}
}