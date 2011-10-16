#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\player.h"
#include "..\goods\cgoods.h"
#include "..\..\..\Setup\GlobeSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//void OnDepotMessage(CMessage* pMsg)
//{
//	CPlayer* pPlayer=pMsg->GetPlayer();
//	if (pPlayer==NULL)
//		return;
//
//	if( pPlayer -> IsInChangingServer() ||
//		pPlayer -> IsInChangingRegion() )
//	{
//		return;
//	}
//
//	if( pPlayer -> GetCurrentProgress() != CPlayer::PROGRESS_BANKING )
//	{
//		return;
//	}
//
//	switch(pMsg->GetType())
//	{		
//	case MSG_C2S_DEPOT_CHECKPASSWORD://玩家进行仓库密码的验证
//		{
//			char strPassWord[7];
//			pMsg->GetStr(strPassWord,7);
//			if(strcmp(pPlayer->GetDepotPassword(),strPassWord)==0)
//			{
//				CMessage msg(MSG_S2C_DEPOT_CHECKPASSWORD);
//				msg.Add((BYTE)1);
//				msg.SendToPlayer(pPlayer->GetExID());
//				pPlayer -> m_cBank.Unlock( strPassWord );
//				pPlayer -> m_cDepot.Unlock( strPassWord );
//				//pMsg->GetPlayer()->SetDepotPassWordCheckFlag(true);
//				//pMsg->GetPlayer()->SetSavedDepotPassWord(strPassWord);
//			}
//			else
//			{
//				CMessage msg(MSG_S2C_DEPOT_CHECKPASSWORD);
//				msg.Add((BYTE)0);
//				msg.SendToPlayer(pPlayer -> GetExID());
//			}
//		}
//		break;
//	case MSG_C2S_DEPOT_CHANGEPASSWORD://玩家改变仓库密码
//		{
//			char strOldPassWord[7];
//			char strNewPassWord[7];
//			pMsg->GetStr(strOldPassWord,7);
//			pMsg->GetStr(strNewPassWord,7);
//			//旧密码是否正确
//			if(strcmp(pPlayer->GetDepotPassword(),strOldPassWord)==0)
//			{
//				//##如果密码长度大于6位，则返回。
//				if( strlen( strNewPassWord ) > 6 )
//				{
//					CMessage msg(MSG_S2C_DEPOT_CHANGEPASSWORD);
//					msg.Add( static_cast<BYTE>(3) /*##3表示字符串密码大于6位*/ );
//					msg.SendToPlayer(pPlayer -> GetExID());
//				}
//				else 
//				{
//					//##做是否有非法字符的判断.
//					BOOL bHasInvalidWord = FALSE;
//					for( size_t i = 0; i < strlen( strNewPassWord ); i ++ )
//					{
//						if( !( (strNewPassWord[i] >= 30 && strNewPassWord[i] <= 39) ||
//							(strNewPassWord[i] >= 65 && strNewPassWord[i] <= 90) ||
//							(strNewPassWord[i] >= 48 && strNewPassWord[i] <= 57) ||
//							(strNewPassWord[i] >= 97 && strNewPassWord[i] <= 122) ) )
//						{
//							bHasInvalidWord = TRUE;
//						}
//					}
//
//					if( bHasInvalidWord )
//					{
//						//##有非法字符。
//						CMessage msg(MSG_S2C_DEPOT_CHANGEPASSWORD);
//						msg.Add( static_cast<BYTE>(2) /*##2表示字符串有非法字符*/ );
//						msg.SendToPlayer(pPlayer -> GetExID());
//					}
//					else 
//					{
//						//##没有非法字符。
//						CMessage msg(MSG_S2C_DEPOT_CHANGEPASSWORD);
//						msg.Add( static_cast<BYTE>(0) /*##0表示修改成功*/ );
//						msg.SendToPlayer(pPlayer -> GetExID());
//						//设置新密码
//						pPlayer->SetDepotPassword(strNewPassWord);
//					}
//				}
//			}
//			else
//			{
//				CMessage msg(MSG_S2C_DEPOT_CHANGEPASSWORD);
//				msg.Add( static_cast<BYTE>(1) /*##1表示原密码错误*/ );
//				msg.SendToPlayer(pPlayer -> GetExID());
//			}
//		}
//		break;
//	case MSG_C2S_DEPOT_CLOSE://玩家关闭了仓库界面
//		{
//			pPlayer -> m_cBank.Lock();
//			pPlayer -> m_cDepot.Lock();
//
//			//##设置当前的操作为空
//			pPlayer -> SetCurrentProgress( CPlayer::PROGRESS_NONE );
//		}
//		break;
//	}
//}