#include "stdafx.h"
#include "Player.h"
#include "Message.h"
#include "Effect.h"
#include "ClientRegion.h"
#include "Skills/SkillXml.h"
#include "../../../Public/Common/SpriteDef.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应小精灵相关消息
void OnSpriteMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	  case MSG_S2C_SPRITE_AutoSupply_ChangeSetup_Re:
			if(pMsg->GetLong())    //设置成功
			{
				//GetGame()->GetCGuiEx()->GetAutoPageEx()->SetNoErr(pMsg->GetLong());
			}
			else{                  //设置失败
				//GetGame()->GetCGuiEx()->GetAutoPageEx()->ReSetDate(pMsg->GetLong());
				//GetGame()->GetCGuiEx()->GetAutoPageEx()->SetGoodsChange();
				long Index=pMsg->GetLong();
				long ErrSc = pMsg->GetLong();   //失败原因
                //分析失败原因
				if(ErrSc==eASET_NotErr)
				{
					//设置成功
				}
				else if(ErrSc==eASET_TypeWithout)
				{
					//补给类型超出范围
					//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords("补给类型超出范围"); 
				}
				else if(ErrSc==eASET_ValueErr)
				{
					//设置的值不合法
					//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords("设置的值不合法");
					
				}
				else if(ErrSc==eASET_NotChange)
				{
					//没有作出改变
					//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords("没有作出改变");
				}
				else if(ErrSc==eASET_UnknowErr)
				{
					//未知错误
					//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords("未知错误");
		
				}
			}
			break;
	  case MSG_S2C_SPRITE_AutoSupply_UseItem_Notify:

		   long indexNum =pMsg->GetLong(); //栏位编号
		   //long ItemIndex = GetGame()->GetCGuiEx()->GetAutoPageEx()->GetItemIndex(indexNum);
		   //long ItemNum =  GetGame()->GetCGuiEx()->GetAutoPageEx()->GetItemAounmt(indexNum); //获得该栏位物编

		   //if(ItemNum !=0)
		   //{
			  // CGoods *pGoods=new CGoods();
			  // CGoodsList::FillGoodsInfo(ItemIndex,pGoods);
			  // if(pGoods==NULL)
				 //  return;
			  // CPlayer * pPlayer = GetGame()->GetMainPlayer();
			  // if(pPlayer==NULL)
				 //  return;
			  // pPlayer->SetGoodsCoolDownStartTime(ItemIndex,timeGetTime());
			  // if(ItemNum<5)
			  // {
				 //  char Text[256]; 
				 //  wsprintf(Text,"%s%s: %d",CStringReading::LoadText(eIDS_SPRITE,eIDS_SPRITE_WEARING),pGoods->GetProperty()->strName.c_str(),ItemNum); 
				 //  //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(CStringReading::LoadText(9,5));
				 //  GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_LEFT_BOTTOM,Text); 

			  // }
			  // SAFE__DELETE(pGoods);
		   //}else{
			  //     GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetOccuButtonSpecialState(true,4);   //设置按钮的闪动
		   //}


		    break;
	}
	
}
