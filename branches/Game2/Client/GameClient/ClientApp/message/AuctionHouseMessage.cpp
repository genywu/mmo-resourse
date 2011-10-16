/**
* @filename:AuctionHouseMessage.cpp
* @date: 2010/5/6
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 仙贝寄售消息处理
*/

#include "stdafx.h"
#include "../Auction/AuctionSys.h"
#include "../../../Net/Message.h"
#include "../MsgBoxSys/MsgEventManager.h"
/////////////////////////////////////////////////
// zhaohang  2010/5/12 
// 消息处理暂时以消息框形式反映，待以后添加对应处理
/***********************************************************************/
/* zhaohang fix 2010-9-3
/* 修改消息框形式，采用封装的MsgEventManager,以及新的字符资源加载方式Appframe
/***********************************************************************/
/////////////////////////////////////////////////

///处理服务器返回的仙贝寄售消息
void OnAuctionHouseMessage(CMessage* pMsg)
{
	//获取类型
	int optype = (int)pMsg->GetByte();
	AHdata& ahd = GetInst(AHdata);
	//解码填充数据,这里按optype类型处理了AHBaseDef.h定义的 message operation type 枚举值里的前三个
	if(true == ahd.DecodeFromDataBlock(pMsg,optype))
	{
		return ; 
	}
	else //处理其他optype类型
	{
		uint result = AH::RES_END;
		uint orderID = 0;
		switch(optype)
		{
		case AH::AH_ADD_AGENT:
			result = (uint)pMsg->GetByte();
			orderID = (uint)pMsg->GetLong();
			if(result== AH::RES_OK) //添加订单成功，发送一次更新个人订单列表请求
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_1"),NULL,NULL,true);
				ahd.Send_AH_REQUEST_ALL();
			}
			else if(result == AH::RES_NOT_ENOUGH_MONEY)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_2"),NULL,NULL,true);
			}
			else if(result == AH::RES_NOT_ENOUGH_GOODS)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_3"),NULL,NULL,true);
			}
			else if(result == AH::RES_INVALID_ORDER)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_4"),NULL,NULL,true);
			}
			else if (result == AH::RES_INVALID_ORDER_CNT)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_5"),NULL,NULL,true);
			}
			else if ( result == AH::RES_MAX_MONEY)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_6"),NULL,NULL,true);
			}
			else if ( result == AH::RES_MAX_GOODS)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_7"),NULL,NULL,true);
			}
			else if(result == AH::RES_INVALID_ARG)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_54"),NULL,NULL,true);
			}
			else if(result == AH::RES_AGENT_MAX)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_8"),NULL,NULL,true);
			}
			else if(result == AH::RES_UNKNWON_ERROR)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_9"),NULL,NULL,true);
			}
			else if(result == AH::RES_END)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_10"),NULL,NULL,true);
			}
			break;
		case AH::AH_REMOVE_AGENT:
			result = (uint)pMsg->GetByte();
			if(result== AH::RES_OK) //移除订单成功，
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_11"),NULL,NULL,true);
				ahd.Send_AH_REQUEST_ALL();//更新三个表
			}
			else if(result == AH::RES_NOT_ENOUGH_MONEY)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_12"),NULL,NULL,true);
			}
			else if(result == AH::RES_NOT_ENOUGH_GOODS)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_55"),NULL,NULL,true);
			}
			else if(result == AH::RES_INVALID_ORDER)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_13"),NULL,NULL,true);
				ahd.Send_AH_REQUEST_AGENT();//撤销订单失败，发送个人订单列表更新请求
			}
			else if (result == AH::RES_INVALID_ORDER_CNT)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_14"),NULL,NULL,true);
				ahd.Send_AH_REQUEST_AGENT();//撤销订单失败，发送个人订单列表更新请求
			}
			else if ( result == AH::RES_MAX_MONEY)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_15"),NULL,NULL,true);
			}
			else if ( result == AH::RES_MAX_GOODS)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_16"),NULL,NULL,true);
			}
			else if(result == AH::RES_INVALID_ARG)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_17"),NULL,NULL,true);
			}
			else if(result == AH::RES_AGENT_MAX)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_18"),NULL,NULL,true);
			}
			else if(result == AH::RES_UNKNWON_ERROR)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_19"),NULL,NULL,true);
			}
			else if(result == AH::RES_END)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_20"),NULL,NULL,true);
			}
			break;
		case AH::AH_BUY:
			result = (uint)pMsg->GetByte();

			if(result == AH::RES_OK)//购买成功
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_21"),NULL,NULL,true);
				GetInst(AHdata).Send_AH_REQUEST_BUY();//更新可购买列表
				GetInst(AHdata).Send_AH_REQUEST_AGENT();//更新个人列表和提取信息
			}
			else if(result == AH::RES_NOT_ENOUGH_MONEY)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_22"),NULL,NULL,true);
			}
			else if(result == AH::RES_NOT_ENOUGH_GOODS)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_23"),NULL,NULL,true);
			}
			else if(result == AH::RES_INVALID_ORDER)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_24"),NULL,NULL,true);
				GetInst(AHdata).Send_AH_REQUEST_BUY();//购买失败更新可购买列表
			}
			else if (result == AH::RES_INVALID_ORDER_CNT)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_25"),NULL,NULL,true);
				GetInst(AHdata).Send_AH_REQUEST_BUY();//购买失败更新可购买列表
			}
			else if ( result == AH::RES_MAX_MONEY)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_26"),NULL,NULL,true);
			}
			else if ( result == AH::RES_MAX_GOODS)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_27"),NULL,NULL,true);
			}
			else if(result == AH::RES_INVALID_ARG)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_28"),NULL,NULL,true);
			}
			else if(result == AH::RES_AGENT_MAX)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_29"),NULL,NULL,true);
			}
			else if(result == AH::RES_UNKNWON_ERROR)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_30"),NULL,NULL,true);
			}
			else if(result == AH::RES_END)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_31"),NULL,NULL,true);
			}
			break;
		case AH::AH_SELL:
			result = (uint)pMsg->GetByte();
			if(result == AH::RES_OK)//出售成功，
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_32"),NULL,NULL,true);
				GetInst(AHdata).Send_AH_REQUEST_SELL();//更新一次可出售信息列表
				GetInst(AHdata).Send_AH_REQUEST_AGENT();//跟新一次个人列表和可提取信息
			}
			else if(result == AH::RES_NOT_ENOUGH_MONEY)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_33"),NULL,NULL,true);
			}
			else if(result == AH::RES_NOT_ENOUGH_GOODS)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_34"),NULL,NULL,true);
			}
			else if(result == AH::RES_INVALID_ORDER)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_35"),NULL,NULL,true);
				GetInst(AHdata).Send_AH_REQUEST_SELL();//出售失败，更新一次可出售列表
			}
			else if (result == AH::RES_INVALID_ORDER_CNT)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_36"),NULL,NULL,true);
				GetInst(AHdata).Send_AH_REQUEST_SELL();//出售失败，更新一次可出售列表
			}
			else if ( result == AH::RES_MAX_MONEY)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_37"),NULL,NULL,true);
			}
			else if ( result == AH::RES_MAX_GOODS)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_38"),NULL,NULL,true);
			}
			else if(result == AH::RES_INVALID_ARG)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_39"),NULL,NULL,true);
			}
			else if(result == AH::RES_AGENT_MAX)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_40"),NULL,NULL,true);
			}
			else if(result == AH::RES_UNKNWON_ERROR)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_41"),NULL,NULL,true);
			}
			else if(result == AH::RES_END)
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_42"),NULL,NULL,true);
			}
			break;
		case AH::AH_PICKUP:
			result = (uint)pMsg->GetByte();
			if(result == AH::RES_OK)//提取成功，发送一次更新提取列表请求
			{

				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_43"),NULL,NULL,true);
				GetInst(AHdata).Send_AH_REQUEST_AGENT();
			}
			else if(result == AH::RES_NOT_ENOUGH_MONEY)
			{
				
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_44"),NULL,NULL,true);
			}
			else if(result == AH::RES_NOT_ENOUGH_GOODS)
			{
				
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_45"),NULL,NULL,true);
			}
			else if(result == AH::RES_INVALID_ORDER)
			{
				
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_46"),NULL,NULL,true);
				GetInst(AHdata).Send_AH_REQUEST_AGENT();//提取失败，更新一次可提取列表
			}
			else if (result == AH::RES_INVALID_ORDER_CNT)
			{
				
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_47"),NULL,NULL,true);
				GetInst(AHdata).Send_AH_REQUEST_AGENT();//提取失败，更新一次可提取列表
			}
			else if ( result == AH::RES_MAX_MONEY)
			{
				
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_48"),NULL,NULL,true);
			}
			else if ( result == AH::RES_MAX_GOODS)
			{
				
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_49"),NULL,NULL,true);
			}
			else if(result == AH::RES_INVALID_ARG)
			{
				
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_50"),NULL,NULL,true);
			}
			else if(result == AH::RES_AGENT_MAX)
			{
				
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_51"),NULL,NULL,true);
			}
			else if(result == AH::RES_UNKNWON_ERROR)
			{
				
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_52"),NULL,NULL,true);
			}
			else if(result == AH::RES_END)
			{
				
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_53"),NULL,NULL,true);
			}
			break;
		default:
			break;
		}
	}
}