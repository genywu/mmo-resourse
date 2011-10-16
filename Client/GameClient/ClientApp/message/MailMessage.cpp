/**************************************************
*		文件名  ：Mail.h
*		简要描述：处理GS的邮件消息

*		当前版本：1.0
*		作者    ：石喜
*		完成日期：10/10/18
*		修订说明：
****************************************************/

#include "stdafx.h"

#include "ClientRegion.h"
#include "Message.h"
#include "Player.h"
#include "Other/Audiolist.h"
#include "../GameClient/Game.h"
#include "../Mail/Mail.h"
using namespace mail;
#include "../Mail/MailDef.h"
#include "../../../Public/Common/MailDef.h"
#include "../../../Public/Common/Public.h"
#include "../../../UI/UIDef.h"
#include "../MsgBoxSys/MsgEventManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void DoS2CMailReceive( CMessage *pMsg );
void DoS2CMailSendFailed( CMessage *pMsg );
void DoS2CMailSend( CMessage *pMsg );
void DoS2CMailRead( CMessage *pMsg );
void DoS2CMailGetGoldReslt( CMessage *pMsg );
void DoS2CMailMoveGoodsResult( CMessage *pMsg );// 取附件金钱
void DoS2CMailDeleteOk( CMessage *pMsg );// 删除邮件
void DoS2CMailAgreedPay( CMessage *pMsg );// 付款收信返回
void DoS2CMailSysMailMsg( CMessage *pMsg );//系统邮件发送消息

void OnMailMessage( CMessage *pMsg )
{	
	CPlayer* pPlayer = pMsg -> GetPlayer();
	if (!pPlayer)
	{
		return;
	}
	switch( pMsg -> GetType() )
	{
	case MSG_S2C_MAIL_RECEIVER:					// 打开收件箱(返回的所有邮件信息)
		{
			DoS2CMailReceive( pMsg );
		}
		break;
	case MSG_S2C_MAIL_SEND_FAILED:				// 邮件发送失败
		{
			DoS2CMailSendFailed( pMsg );
		}
		break;
	case MSG_S2C_MAIL_SEND:						// 发送邮件返回
		{
			DoS2CMailSend( pMsg );			
		}
		break;
	case MSG_S2C_MAIL_READ:						// 阅读邮件
		{
			DoS2CMailRead( pMsg );
		}
		break;
	case MSG_S2C_MAIL_GET_GOLD_RESULT:			// 取附件金钱
		{

			DoS2CMailGetGoldReslt( pMsg );
		}
		break;
	case MSG_S2C_MAIL_MOVE_GOODS_RESULT:		// 取附件物品
		{
			DoS2CMailMoveGoodsResult( pMsg );
		}
		break;
	case MSG_S2C_MAIL_DELETE_OK:				// 删除邮件
		{
			DoS2CMailDeleteOk( pMsg );
		}
		break;
	case MSG_S2C_MAIL_AGREED_PAY:				// 付款收信返回
		{
			DoS2CMailAgreedPay( pMsg );
		}
		break;
	case MSG_S2C_MAIL_SYSMAILMSG:				//系统邮件发送消息
		{
			DoS2CMailSysMailMsg( pMsg );
		}
		break;
	}
}

void DoS2CMailReceive( CMessage *pMsg )
{
	long dwSize=pMsg->GetLong();
	if( dwSize==0 )
		return;
	BYTE *pBuff=new BYTE[dwSize];
	pMsg->GetEx(pBuff,dwSize);
	long lPos = 0;
	
	GetInst(CMail).ClearAllMailData();//删除所有邮件信息
	
	// 邮件个数
	long lNum = _GetLongFromByteArray(pBuff, lPos);
	for (int i=0; i<lNum; ++i)
	{
		stMailData stData;
		GetInst(CMail).DecordDataFromByteArray(stData,pBuff,lPos);
		GetInst(CMail).AddMail(stData);
	}
	FireUIEvent(MAILLIST,EVENT_UPDATA);
	
	SAFEDELETEARRAY(pBuff);

	// 打开的情况下，直接刷新收件箱列表页面
	//if(pPage->IsOpen())
	//	pPage->UpdataInBoxMail(pPage->GetMailListCurPage());
	//else
	//	pPage->Open();
	// 图标停止闪动
	//CComponent *pComp = GetGame()->GetCGuiEx()->GetMainBarPageEx()->GetMiniMapComp();
	//if(pComp)
	//{
	//	pComp = pComp->GetComponent("Mail");
	//	if(pComp)
	//	{
	//		CImageIcon *pIcon = (CImageIcon *)pComp->GetSpecialIcon();
	//		if(pIcon)
	//			pIcon->SetIsChangeAlpha(false);
	//	}
	//}
}
void DoS2CMailSendFailed( CMessage *pMsg )
{
	long index = pMsg->GetLong();
	string str = "";
	switch(index)
	{
		//邮资不够
	case MAIL_SENDING_FAILED_INVALID_POSTAGE:
		str = AppFrame::GetText("Mail_51");
		break;	
		//金币超出限制
	case MAIL_SENDING_FAILED_INVALID_GOLD:
		str = AppFrame::GetText("Mail_55");
		break;
		//物品超出限制
	case MAIL_SENDING_FAILED_INVALID_GOODS:
		str = AppFrame::GetText("Mail_56");
		break;
		//名字过长，发送失败！
	case MAIL_SENDING_FAILED_INVALID_MAXNAME:
		str = AppFrame::GetText("Mail_57");
		break;
		//主题过长
	case MAIL_SENDING_FAILED_INVALID_MAXSUBJECT:
		str = AppFrame::GetText("Mail_58");
		break;
		//文字内容过长
	case MAIL_SENDING_FAILED_INVALID_MAXTEXT:
		str = AppFrame::GetText("Mail_59");
		break;
	//	//非法收信人
	case MAIL_SENDING_FAILED_INVALID_RECEIVER:
		str = AppFrame::GetText("Mail_60");
		break;
		//无收信人或标题
	//case MAIL_SENDING_FALLED_INVALID_SUBJECTORRECEIVER:
	//	str = AppFrame::GetText("Mail_61");
	//	break;
		//给自己发邮件
	/*case MAIL_SENDING_FAILED_INVALID_SELF:
		str = AppFrame::GetText("Mail_27");
		break;*/
	//case MAIL_SENDING_FAILED_INVALID_MANYRECEIVER:
	//	str = AppFrame::GetText("Mail_24");
	//	break;
	//主题包含非法字符
	//case MAIL_SENDING_FAILED_INVALID_SUBJECT:
	//	str = AppFrame::GetText("Mail_52");
	//	break;
	//	//内容包含非法字符
	//case MAIL_SENDING_FAILED_INVALID_TEXT:
	//	str = AppFrame::GetText("Mail_53");
	//	break;
	//附加内容包含非法字符
	//case MAIL_SENDING_FAILED_INVALID_EXTEXT:
	//	str = AppFrame::GetText("Mail_54");
	//	break;
	}
	if(str!="")
	{
		 GetInst(MsgEventManager).PushEvent(Msg_Ok,str.c_str());
	}
}
void DoS2CMailSend( CMessage *pMsg )
{
	//[COLOR 4D1F00][B]您有新的邮件，请注意查收！
	GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_30"));
}
void DoS2CMailRead( CMessage *pMsg )
{
	BYTE bFlag = pMsg->GetByte();
	if(bFlag > 0)
	{
		CGUID guid;
		pMsg->GetGUID(guid);
		stMailData *stData = GetInst(CMail).GetMailByGUID(guid);
		if(stData)
		{
			stData->bIsReaded = true;
			GetInst(CMail).SetCurReadMailGUID(guid);
			FireUIEvent(READMAIL,EVENT_UPDATA);
		}
	}
	else
	{
		//此物品与之前的附件物品类型不符，物品添加失败！
		//sGetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_3"));//del
	}
}

void DoS2CMailGetGoldReslt( CMessage *pMsg )
{
	//CPlayer *pPlayer = GetGame()->GetMainPlayer();//del
	//char str2[256]="";//del
	//sprintf_s(str2,"领取金币后的金钱为%d",pPlayer->GetMoney());//del
	//GetInst(MsgEventManager).PushEvent(Msg_Ok,str2);//del

	CGUID guid;
	pMsg->GetGUID(guid);
	BYTE bFlag = pMsg->GetByte();
	if(bFlag > 0)
	{
		stMailData *stData = GetInst(CMail).GetMailByGUID(guid);
		if(stData)
		{
			stData->dwGoldNum = 0;
			stData->bIsCanReBack = false;
			if(stData->GoodsList.empty() && stData->dwGoldNum == 0 && stData->eMailType != SYSTEM_MAIL)
			{
				stData->eMailType = COMMON_MAIL;
			}
			GetInst(CMail).UpdataMail(*stData);
			if (guid == GetInst(CMail).GetCurReadMailGUID())
			{
				FireUIEvent(GOLD,EVENT_UPDATA);//通知界面更新金钱
			}		
		}
		//领取金币成功！
		 GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_39"));
	}
}
void DoS2CMailMoveGoodsResult( CMessage *pMsg )
{
	CGUID guid;
	pMsg->GetGUID(guid);
	long index = pMsg->GetLong();
	//GetInst(CMail).SetGoodsIndex( index );
	BYTE bFlag = pMsg->GetByte();
	if(bFlag > 0)// 成功
	{
		stMailData *stData = GetInst(CMail).GetMailByGUID(guid);
		if(stData)
		{
			list<CGoods *>::iterator it = stData->GoodsList.begin();
			for (int i=0; it != stData->GoodsList.end(); ++it, ++i)
			{
				if( i == index )
				{
					SAFE_DELETE((*it));
					stData->GoodsList.erase(it);
					break;
				}
			}
			stData->bIsCanReBack = false;
			if (guid == GetInst(CMail).GetCurReadMailGUID())
			{
				FireUIEvent( GETGOODSCONTAINER,EVENT_DELGOODS );
			}
			// 物品和金币都没有，且不是系统邮件，转化成普通类型的邮件
			if(stData->GoodsList.empty() && stData->dwGoldNum == 0 &&stData->eMailType != SYSTEM_MAIL)
			{
				stData->eMailType = COMMON_MAIL;
			}
			GetInst(CMail).UpdataMail(*stData);
		}
	}
	else
	{	
		//"取附件物品失败，请检查背包是否有空位！"
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_28"));
	}
}
void DoS2CMailDeleteOk( CMessage *pMsg )// 删除邮件
{
	long size = pMsg->GetLong();
	if(size > 0)
	{
		for (int i=0; i<size; ++i)
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			GetInst(CMail).DelMailByGuid(guid);
		}
		//邮件删除成功！
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_35")); 
		FireUIEvent(MAILLIST,EVENT_UPDATA);
	}
}
void DoS2CMailAgreedPay( CMessage *pMsg )// 付款收信返回（普通）
{
	CGUID guid;
	pMsg->GetGUID(guid);
	eMAILTYPE eType = (eMAILTYPE)pMsg->GetByte();
	stMailData *stData = GetInst(CMail).GetMailByGUID(guid);
	if(stData)
	{
		stData->eMailType = eType;//更新邮件类型
		stData->dwGoldNum = 0;
		GetInst(CMail).UpdataMail(*stData);
	}

	FireUIEvent(READ,EVENT_UPDATA);
	//付费成功
	GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_40"));//del
	
	//CPlayer *pPlayer = GetGame()->GetMainPlayer();//del
	//char str2[256]="";//del
	//sprintf_s(str2,"付费后的金钱为%d",pPlayer->GetMoney());//del
	//GetInst(MsgEventManager).PushEvent(Msg_Ok,str2);//del

}

void DoS2CMailSysMailMsg( CMessage *pMsg )//系统邮件发送消息
{
	GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_30"));
}