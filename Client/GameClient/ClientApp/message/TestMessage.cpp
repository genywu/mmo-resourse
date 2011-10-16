#include "stdafx.h"
#include "Message.h"
#include "Other\TestMsgType.h"
#include "Player.h"
#include "Monster.h"
#include "Clientregion.h"
#include "Other\Audiolist.h"
#include "Other\RegionList.h"
#include "../GameClient/GameControl.h"
#include "Depot.h"
#include "../../Game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 模拟鼠标消息
void OnTestMouseMessage( CMessage *pMsg )
{
#ifdef _CLENTTOTEST_
	if(!pMsg)
		return;
	UINT message = 0;
	WPARAM wParam = 0;
	LPARAM lParam = 0;
	long type = pMsg->GetType();
	// 设置鼠标坐标
	int x = pMsg->GetLong();
	int y = pMsg->GetLong();
	/*char str1[256]="";
	pMsg->GetStr(str1,256);
	PutDebugString(str1);
	char str2[256]="";
	pMsg->GetStr(str2,256);
	PutDebugString(str2);*/
	switch( type )
	{
	case MSG_T2C_MOUSE_MOVE:						// 鼠标移动
		message = WM_MOUSEMOVE;
		break;
	case MSG_T2C_MOUSE_LBUTTONDOWN:					// 左键按下
		message = WM_LBUTTONDOWN;
		break;
	case MSG_T2C_MOUSE_LBUTTONUP:					// 左键释放
		message = WM_LBUTTONUP;
		break;
	case MSG_T2C_MOUSE_LBUTTONDBLCLK:				// 左键双击
		message = WM_LBUTTONDBLCLK;
		break;
	case MSG_T2C_MOUSE_RBUTTONDOWN:					// 右键按下
		message = WM_RBUTTONDOWN;
		break;
	case MSG_T2C_MOUSE_RBUTTONUP:					// 右键释放
		message = WM_RBUTTONUP;
		break;
	case MSG_T2C_MOUSE_RBUTTONDBLCLK:				// 右键双击
		message = WM_RBUTTONDBLCLK;
		break;
	case MSG_T2C_MOUSE_MOUSEWHEEL:					// 鼠标中间滚动
		message = WM_MOUSEWHEEL;
		break;
	default:
		return;
	}
	if(message != 0)
	{
		if(x > 0 && x < SCREEN_WIDTH && y > 0 && y < SCREEN_HEIGHT)
		{
			POINT point;
			point.x = x;
			point.y = y;
			ClientToScreen(g_hWnd,&point);
			SetCursorPos(point.x,point.y);
		}
		PostMessage(g_hWnd,message,0,0);
	}
#endif
}

// 模拟键盘消息
void OnTestKeyMessage( CMessage *pMsg )
{
#ifdef _CLENTTOTEST_
	if(!pMsg)
		return;
	UINT message = 0;
	WPARAM wParam = 0;
	LPARAM lParam = 0;
	switch( pMsg -> GetType() )
	{
	case MSG_T2C_KEY_DOWN:							// 键盘按下
		message = WM_KEYDOWN;
		break;
	case MSG_T2C_KEY_UP:							// 键盘释放
		message = WM_KEYUP;
		break;
	case MSG_T2C_KEY_CHAR:							// 键盘操作
		message = WM_CHAR;
		break;
	default:
		return;
	}
	// 操作的键值
	wParam = (WPARAM)pMsg->GetByte();
	if(message != 0)
	{
		PostMessage(g_hWnd,message,wParam,0);
	}
#endif
}

// 测试程序的逻辑事件消息处理
void OnTestLogicMessage( CMessage *pMsg )
{
#ifdef _CLENTTOTEST_
	if(!pMsg)
		return;
	switch( pMsg -> GetType() )
	{
	case MSG_T2C_LOGIC_MOVE:							// 移动到某个坐标点
		{
			long lRegionID = pMsg->GetLong();
			long x = pMsg->GetLong();
			long y = pMsg->GetLong();
			CPlayer * pPlayer = GetGame()->GetMainPlayer();
			if(pPlayer && GetGame()->GetGameControl())
			{
				long lCurRegionID = pPlayer->GetRegionID();
				// 本场景移动
				if(lRegionID == lCurRegionID)
					GetGame()->GetGameControl()->AutoMoveTo((float)x,(float)y);
				// 跨场景移动
				else
				{
					// 计算寻路的场景节点
					CRegionList::FindRegionPath(lCurRegionID,lRegionID);
					// 具体寻路
					if(!CRegionList::GetRegionPathList().empty())
					{
						// 获取第一个场景节点信息
						CRegionList::tagRegionPathInfo stPathInfo = CRegionList::GetRegionPathList().front();
						GetGame()->GetGameControl()->FindPathToOtherRegion(lCurRegionID,pPlayer->GetTileX(),pPlayer->GetTileY(),
							lRegionID,x,y);
					}
					//else
					//	GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(CStringReading::LoadText(eIDS_QUEST,eIDS_QUEST_NOTMOVETO),0xffff0000);
				}
			}
		}
		break;
	case MSG_T2C_LOGIC_SETPOS:							// 瞬移到某个坐标点
		{
			CPlayer * pMainPlayer = GetGame()->GetMainPlayer();
			long x = pMsg->GetLong();
			long y = pMsg->GetLong();
			if(pMainPlayer)
			{
				CMessage msg(MSG_C2S_SHAPE_SETPOS);
				msg.Add(pMainPlayer->GetType());
				msg.Add(pMainPlayer->GetExID());
				msg.Add(x);
				msg.Add(y);
				msg.Send();
			}
			if(GetGame()->GetGameControl())
			{
				GetGame()->GetGameControl()->AutoAttactStop();
				GetGame()->GetGameControl()->AutoMoveStop();
			}
		}
		break;
	case MSG_T2C_LOGIC_SELECTEDSHAPE:					// 选择某个对象
		{
			long type = pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			CMoveShape* pMoveShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(type, guid);
			if(pMoveShape && GetGame()->GetGameControl())
			{
				GetGame()->GetGameControl()->SetSelectedShape(pMoveShape);
			}
		}
		break;
	case MSG_T2C_LOGIC_UIISOPENED:						// 请求客户端返回一个指定界面是否开启的消息
		{
			BYTE bIsOpen = 0;
			//界面名字
			char pszInterfaceName[256];
			pMsg->GetStr(pszInterfaceName,256);
			// 背包界面
			if(strcmp(pszInterfaceName,"ItemPage") == 0)
			{
				//if(GetGame()->GetCGuiEx()->GetItemPageEx() && 
				//	GetGame()->GetCGuiEx()->GetItemPageEx()->IsOpen())
					bIsOpen = 1;
			}
			else
			{
				// 查找对应的主界面
				//CGUIComp * pParentComp = IGUIRoot::GetGUIInstance()->GetGUIComp();
				/*if(pParentComp)
				{
					CComponent * pComp = pParentComp->GetComponentByName(pszInterfaceName);
					if(pComp && pComp->IsVisible() && pComp->IsEnabled())
					{
						bIsOpen = 1;
					}
				}*/
			}
			// 向测试程序发送结果
			CMessage msgtot(MSG_C2T_LOGIC_UIISOPENED);
			msgtot.Add(pszInterfaceName);
			msgtot.Add(bIsOpen);
			msgtot.SendMemToTest();
		}
		break;
		// 请求客户端拾取指定物品
	case MSG_T2C_LOGIC_PICKGOODS:
		{
			CClientRegion *pRegion = GetGame()->GetRegion();
			CGameControl * pCtl = GetGame()->GetGameControl();
			if(!pRegion || !pCtl)
				break;
			long lNum = pMsg->GetLong();
			for (long i=0; i<lNum; ++i)
			{
				CGUID id;
				pMsg->GetGUID(id);
				CGoods *pGoods = dynamic_cast<CGoods *>(pRegion->FindChildGoods(id));
				if(pGoods)
					pCtl->PickupGoods(pGoods);
			}
		}
		break;
		// 请求客户端向服务器发送脚本调用功能
	case MSG_T2C_LOGIC_CALLSCREAPT:
		{
			// 要运行的脚本命令
			char pszstr[256];
			pMsg->GetStr(pszstr,256);
			CPlayer * pPlayer = GetGame()->GetMainPlayer();
			if(g_bDebug && pPlayer)
			{
				// 发送消息
				CMessage msg(MSG_C2S_OTHER_TALK);
				msg.Add((BYTE)0);
				msg.Add((long)RUN_SCRIPT);
				msg.Add(pPlayer->GetType());		// 发送者type
				msg.Add(pPlayer->GetExID());		// 发送者ID
				//==========================物品信息========================================
				msg.Add((long)0);
				msg.Add((char *)pszstr);
				msg.Send();
			}
		}
		break;
		// 请求客户端向服务器发送拆分物品消息
	case MSG_T2C_LOGIC_SPLITGOODS:
		{
			CPlayer * pPlayer = GetGame()->GetMainPlayer();
			if(pPlayer == NULL)
				return;
			// 物品源容器和索引
			long lSorceObjectExtendID = pMsg->GetLong();
			long lSorceObjectPos = pMsg->GetLong();
			// 物品目标容器和索引
			long lDesObjectExtendID = pMsg->GetLong();
			long lDesObjectPos = pMsg->GetLong();
			// 数量
			long lObjectNum = pMsg->GetLong();

			CGoods *pGoods = NULL;
			// 物品类型和ID
			long lObjectType = 0;
			CGUID ObjectGUID = CGUID::GUID_INVALID;
			// 背包中的物品
			if(lSorceObjectExtendID == PEI_PACKET || 
				(lSorceObjectExtendID > PEI_SUBPACK &&
				lSorceObjectExtendID >= PEI_PACK5))
			{
				CPlayer::tagGoods *pstGoods = pPlayer->GetGoodsOnPlayer((WORD)lSorceObjectExtendID,(WORD)lSorceObjectPos);
				if(pstGoods)
					pGoods = pstGoods->pGoods;
			}
			// 仓库
			else if(IS_DEPOT_CONTAINER(lSorceObjectExtendID))
			{
				pGoods = CGCMana::Inst()->GetGoods((DWORD)lSorceObjectExtendID, (DWORD)lSorceObjectPos);
			}
			// 有物品
			if(pGoods)
			{
				lObjectType = pGoods->GetType();
				ObjectGUID = pGoods->GetExID();
			}
			CMessage msg(MSG_C2S_CONTAINER_OBJECT_MOVE);
			msg.Add(pPlayer->GetType());
			msg.Add(pPlayer->GetExID());
			msg.Add((LONG)lSorceObjectExtendID);
			msg.Add((LONG)lSorceObjectPos);
			msg.Add(pPlayer->GetType());
			msg.Add(pPlayer->GetExID());
			msg.Add( lDesObjectExtendID);
			msg.Add( lDesObjectPos );
			msg.Add((LONG)lObjectType);
			msg.Add(ObjectGUID);
			msg.Add((LONG)lObjectNum);
			msg.Send();
		}
		break;
	default:
		return;
	}
#endif
}