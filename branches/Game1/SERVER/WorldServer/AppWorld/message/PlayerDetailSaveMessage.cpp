#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\player.h"
#include "..\..\WorldServer\Game.h"
#include "../dbentity/entityManager.h"
#include "../dbentity/entitygroup.h"
#include "../PetCtrl.h"
#include "../Linkman/LinkmanSystem.h"



void OnPlayerDetailSaveMessage(CMessage *pMsg)
{
	// player GUID
	CGUID playerGUID;
	char pszSaveInfo[512]="";
	pMsg->GetGUID(playerGUID);

	switch(pMsg->GetType())
	{
	case MSG_S2W_PLAYER_DETAIL_SAVE_PROPERTY:
		{
			// decode MSG_S2W_PLAYER_DETAIL_SAVE_PROPERTY 消息
			DWORD bitValue = pMsg->GetDWord();

			char pszStackInfo[1024]="";
			char playguid[128]="";
			playerGUID.tostring(playguid);

			DBReadSet setReadDB;
			pMsg->GetDBReadSet(setReadDB);
#ifdef _RUNSTACKINFO1_
			//输出运行栈标记	
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"OnPlayerDetailSaveMessage() Start");
#endif
			CPlayer* pPlayer = GetGame()->GetMapPlayer(playerGUID);
			if(!pPlayer)
			{
#ifdef _RUNSTACKINFO1_
				//输出运行栈标记
				char szOutStr[1024];
				sprintf(szOutStr, "OnPlayerDetailSaveMessage() Find Player[%s] is NULL!", playguid);
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), szOutStr);
#endif
				return;
			}
			// 按bit解析消息
			if( bitValue & SAVE_DETAIL_ATTRPROPERTY)// 基本属性:金钱,等级...等
			{
				if(!GetGame()->DecodeSavePropertyMsg(pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTREQUIPMENTGOODS)// 装备物品
			{
				if(!GetGame()->DecodeSaveEquipmentGoodsMsg(pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRPACKGOODS)// 主背包物品
			{
				if(!GetGame()->DecodeSavePackGoodsMsg(pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRSUBPACKGOODS1)// 子背包1物品
			{
				if(!GetGame()->DecodeSaveSubPackGoodsMsg(0, pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRSUBPACKGOODS2)// 子背包2物品
			{
				if(!GetGame()->DecodeSaveSubPackGoodsMsg(1, pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRSUBPACKGOODS3)// 子背包3物品
			{
				if(!GetGame()->DecodeSaveSubPackGoodsMsg(2, pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRSUBPACKGOODS4)// 子背包4物品
			{
				if(!GetGame()->DecodeSaveSubPackGoodsMsg(3, pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRSUBPACKGOODS5)// 子背包5物品
			{
				if(!GetGame()->DecodeSaveSubPackGoodsMsg(4, pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRWALLETGOODS)// 钱包物品
			{
				if(!GetGame()->DecodeSaveWalletGoodsMsg(pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRSILVERGOODS)// 银币物品
			{
				if(!GetGame()->DecodeSaveSivlerGoodsMsg(pPlayer, setReadDB))
					return;
			}
			//! 仓库
			/////////////////////////////

			if( bitValue & SAVE_DETAIL_ATTRDEPOTGOLD)// 黄金仓库
			{
				if(!GetGame()->DecodeDepotGoldMsg(pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRDEPOTSILVER)// 银币仓库
			{
				if(!GetGame()->DecodeDepotSilverMsg(pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRPRIMARY)// 主仓库
			{
				if(!GetGame()->DecodeDepotGoodsMsg(pPlayer, setReadDB))
					return;
			}		
			if( bitValue & SAVE_DETAIL_ATTRSUBDEPOT1)// 子仓库1
			{
				if(!GetGame()->DecodeSaveSubDepotGoodsMsg(0, pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRSUBDEPOT2)// 子仓库2
			{
				if(!GetGame()->DecodeSaveSubDepotGoodsMsg(1, pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRSUBDEPOT3)// 子仓库3
			{
				if(!GetGame()->DecodeSaveSubDepotGoodsMsg(2, pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRSUBDEPOT4)// 子仓库4
			{
				if(!GetGame()->DecodeSaveSubDepotGoodsMsg(3, pPlayer, setReadDB))
					return;
			}

			if( bitValue & SAVE_DETAIL_ATTRBUSINESSPACK ) // 商业背包
			{
				pPlayer->GetBusinessPackage().DecodeFromGS( pPlayer, setReadDB );
			}

			if( bitValue & SAVE_DETAIL_ATTRVARLIST)// 变量列表
			{
				if(!GetGame()->DecodeSaveVarListMsg(pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRQUESTLIST)// 任务列表
			{
				if(!GetGame()->DecodeSaveQuestListMsg(pPlayer, setReadDB))
					return;
			}
			
			if( bitValue & SAVE_DETAIL_ATTRSKILLLIST)// 技能列表
			{ 
				if(!GetGame()->DecodeSaveSkillListMsg(pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRSTATELIST)// 状态列表
			{	
				if(!GetGame()->DecodeSaveStateListMsg(pPlayer, setReadDB))
					return;
			}
			if( bitValue & SAVE_DETAIL_ATTRDEPUTYOCCU)// 副职业
			{
				if(!GetGame()->DecodeSaveDeputyOccuListMsg(pPlayer, setReadDB))
					return;
			}
	
			if(bitValue & SAVE_DETAIL_ATTRMERQUESTLIST)// 俑兵任务列表
			{	
				if(!GetGame()->DecodeSaveMerQuestListMsg(pPlayer, setReadDB))
					return;
			}

			if(bitValue & SAVE_DETAIL_PETS )			// 宠物列表
			{	
				GetInst(CPetCtrl).DecodeFromDataBlock(setReadDB, pPlayer);
			}

            if (bitValue & SAVE_DETAIL_INNERHOOK)      //小精灵系统
            {
                if (!GetGame()->DecodeSaveSpriteSystemMsg(pPlayer,setReadDB))
                    return;
            }

#ifdef _RUNSTACKINFO1_
			//输出运行栈标记	
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SendSavePlayerMsg() Start.");
#endif
			// 创建SaveSession
			GetGame()->GetEntityManager()->CreateSavePlayerDataSession(playerGUID, bitValue);

#ifdef _RUNSTACKINFO1_
			//输出运行栈标记	
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SendSavePlayerMsg() End.");
#endif

#ifdef _RUNSTACKINFO1_
	//输出运行栈标记	
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"OnPlayerDetailSaveMessage() End");
#endif
		}
		break;
	default:
		break;
	}
}