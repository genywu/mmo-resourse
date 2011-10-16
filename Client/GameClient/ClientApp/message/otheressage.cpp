#include "stdafx.h"
#include "appclient\clientregion.h"
#include "appclient\player.h"
#include "appclient\npc.h"
#include "appclient\goods.h"
#include "appclient\monster.h"
#include "..\..\..\..\nets\netclient\message.h"
#include "gameclient\game.h"
//#include "..\interface\interface.h"
//#include "..\interface\mainbar.h"
//#include "..\interface\goodslinkpage.h"
#include "..\other\chat.h"
#include "..\other\FaceActionList.h"
//#include "../../GameUI/MList.h"
//#include "../../GameUI/mcheck.h"
//#include "..\interface\npclistpage.h"
#include "..\guiextend\CGuiEx.h"
#include "AppClient\other\QuestSystem.h"
//#include "StrUnifyInput.h"
#include "..\\other\\VideoManager.h"

#include "..\guiextend\MainBarPageEx.h"
#include "..\guiextend\FriendsPageEx.h"
#include "..\guiextend\TalkBoxPageEx.h"
#include "..\guiextend\PixyTalkBoxPageEx.h"
#include "..\guiextend\NpcListPageEx.h"
#include "..\guiextend\ClewInfo.h"
#include "..\guiextend\TopPage.h"
#include "..\guiextend\TopMsgPageEx.h"
#include "..\guiextend\InputBoxPageEx.h"
#include "..\guiextend\MessageBoxPageEx.h"
#include "..\guiextend\LoginPageEx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/*
 * 功能: 响应其他消息
 * 摘要: -
 * 参数: pMsg - 服务器发送的消息
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.04.21 - lpf
 *		增加了对MSG_S2C_OTHER_EXPATC消息的响应,用来支持仪态动作的播放;
 *	2008.06.12 - lpf
 *		在MSG_S2C_OTHER_EXPATC消息的响应中,增加了"没有说话消息的判断";
 *	2008.06.17 - lpf
 *		根据仪态动作新的结构修改了MSG_S2C_OTHER_EXPATC消息的响应,对得到仪态动作的处理;
 *	2008.08.14 - lpf
 *		在MSG_S2C_OTHER_EXPATC消息响应处增加了对得到场景指针是否为空的判断;
 *	2008.08.21 - lpf
 *		增加了对视频播放消息的处理;
 */
void OnOtherMessage(CMessage* pMsg)
{
	CGlobeSetup::tagSetup *pSetup = CGlobeSetup::GetSetup();

	switch(pMsg->GetType())
	{
	///////////////////////////////////////////////////////////////////////////////////////
	// 对话消息
	///////////////////////////////////////////////////////////////////////////////////////
	case MSG_S2C_OTHER_TALK:
		{
			if (GetGame()->GetMainPlayer() == NULL)
				break;
			CMainBarPageEx * pMainPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
			if( ! pMainPage )
				return;

			// 字符or索引
			BYTE bFlag = pMsg->GetByte();
			long lType = pMsg->GetLong();

			// 聊天信息中，物品信息其实位置的调节长度
			long lAdjustLen = 0;
			char str[1024] = "";

			// 说话人type 和 GUID
			long lSenderType = pMsg->GetLong();	
			CGUID SenderID;
			pMsg->GetGUID(SenderID);

			string strName;
			string strmem;

			//=========================对话信息中的聊天信息=======================
			list<CEditField::stSpecialTextInfo> * pTextInfoList = pMainPage->GetSpecialText();
			pTextInfoList->clear();

			char strGoodsName[1024] = "";
			long lNum = pMsg->GetLong();
			for(long i=0; i<lNum; ++i)
			{
				CEditField::stSpecialTextInfo stInfo;
				pMsg->GetEx(&stInfo, sizeof(CEditField::stSpecialTextInfo));
				stInfo.str = pMsg->GetStr(strGoodsName,1024);
				pTextInfoList->push_back(stInfo);
			}

			//====================================================================
			TCHAR	szChatWords[ 2056 ];

			switch( lType )
			{
				case RUN_HELP:
				case CHAT_NPC:
				case CHAT_NORMAL:
					{
						strName = pMsg->GetStr(str,1024);

						// 检查黑名单
						if( GetGame()->GetCGuiEx()->GetFriendsPageEx()->IsInGroup( strName.c_str(), CFriendsPageEx::PANEL_BLACK ) )
							break;

						if(bFlag==0)
							strmem = pMsg->GetStr(str,1024);
						else
						{
							DWORD dwType = pMsg->GetDWord();
							DWORD dwIndex = pMsg->GetDWord();
							strmem = CStringReading::LoadString(dwType,dwIndex);
						}

						// 脏话过滤
						CClientWordsFilter::GetInstance()->Check(strmem, true, true);

						// 加入说话者的头上
						CMoveShape* pSender = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(lSenderType, SenderID));
						if(pSender)
						{
							pSender->SetTalkText(strmem.c_str());
							pSender->SetTalkLiveTimer(500);
							if(lSenderType == TYPE_MONSTER)
								pSender->SetTalkLiveTimer(1000);
						}

						// 格式化聊天字段，NPC说话和普通一样 所以 lType 固定为 Normal
						wsprintf( szChatWords, CStringReading::LoadString( 14, CHAT_NORMAL+101 ), strName.c_str(), strmem.c_str() );

						// 前缀长度
						lAdjustLen = lstrlen( szChatWords ) - strmem.size();
						pMainPage->m_lAdjPos = lAdjustLen;
						pMainPage->AdjGoodsInfoPos(lAdjustLen);

						//
						if(lSenderType != TYPE_PLAYER)
							strName = "";
						// 怪物说话不显示在聊天框上
						if(lSenderType != TYPE_MONSTER)
							pMainPage->AddChatWords(szChatWords,
												pSetup->colorChatText[CHAT_NORMAL],
												pSetup->colorChatTextBack[CHAT_NORMAL], 
												pSetup->colorChatTextShadow[CHAT_NORMAL],
												strName.c_str(),
												lType);
						
					}
					break;

				case CHAT_PRIVATE:
					{
						char strTargetName[256] = "";
						char strSourceName[256] = "";
						pMsg->GetStr(strTargetName,256);
						pMsg->GetStr(strSourceName,256);
						strName = strSourceName;

						// 检查是否是自己
						if( !lstrcmp( strTargetName, strSourceName ) )
							break;

						// 好友系统UI
						CFriendsPageEx* pFriendsPageEx	= GetGame()->GetCGuiEx()->GetFriendsPageEx();

						// 检查黑名单
						if( pFriendsPageEx->IsInGroup( strName.c_str(), CFriendsPageEx::PANEL_BLACK ) )
							break;

						// 屏蔽说话的的玩家
						if( GetGame()->CheckDisableChatter(strSourceName) )
							break;

						strmem = pMsg->GetStr(str,256);

						// 脏话过滤
						CClientWordsFilter::GetInstance()->Check(strmem, true, true);

						// 王峰：陌生人的名字
						std::string nameStanger;

						//发给别人的话在名字前加上"=>"
						if(strcmp(strSourceName, GetGame()->GetMainPlayer()->GetName())==0)
						{
							// 格式化聊天字段
							wsprintf( szChatWords, CStringReading::LoadString( 14, lType+121 ), strTargetName, strmem.c_str() );

							// 王峰：记录陌生人的名字
							nameStanger = strTargetName;
						}
						else
						{
							// 格式化聊天字段
							wsprintf( szChatWords, CStringReading::LoadString( 14, lType+101 ), strName.c_str(), strmem.c_str() );

							// 王峰：记录陌生人的名字
							nameStanger = strName;

							// 播放密聊声音
							if(!GetGame()->GetAudioList()->IsPlaying(
								pFriendsPageEx->GetConfig( CFriendsPageEx::PANEL_STRANGER )->m_NotifySetup.szMusic_SecretTalk ))
							{
								GetGame()->GetAudioList()->Play2DSound(
									pFriendsPageEx->GetConfig( CFriendsPageEx::PANEL_STRANGER )->m_NotifySetup.szMusic_SecretTalk );
							}
						}

						// 前缀长度
						lAdjustLen = lstrlen( szChatWords ) - strmem.size();
						pMainPage->m_lAdjPos = lAdjustLen;

						// 王峰：显示密聊文字
						pMainPage->AdjGoodsInfoPos(lAdjustLen);
						GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords( szChatWords, 
																				pSetup->colorChatText[lType],
																				pSetup->colorChatTextBack[lType], 
																				pSetup->colorChatTextShadow[lType],
																				nameStanger.c_str(),
																				lType);

						//记录最后一个发密语给自己的人
						if(!strcmp(GetGame()->GetMainPlayer()->GetName(),strTargetName))
						{
							extern string g_strLastWisper;
							g_strLastWisper = strSourceName;
						}
						//记录最后一个主动密聊的对象
						else if(!strcmp(GetGame()->GetMainPlayer()->GetName(),strSourceName))
						{
							extern string g_strLastWisper;
							g_strLastWisper = strTargetName;
						}

						// 王峰：自动添加陌生人
						CFriendsPageEx::ENUM_PANEL ePanelFind;
						tagLinkman* pstLinkmanFind;

						if( ! pFriendsPageEx->FindLinkman( nameStanger.c_str(), ePanelFind, pstLinkmanFind ) )
						{
							tagLinkman* pstLinkman = new tagLinkman;
							ZeroMemory( pstLinkman, sizeof(tagLinkman) );
							lstrcpy( pstLinkman->szName, nameStanger.c_str() );

							pFriendsPageEx->AddLinkman( CFriendsPageEx::PANEL_STRANGER, pstLinkman );
						}
					}
					break;

				case CHAT_TEAM:
					{
						strName = pMsg->GetStr(str,1024);

						strmem = pMsg->GetStr(str,1024);

						// 脏话过滤
						CClientWordsFilter::GetInstance()->Check(strmem, true, true);

						// 格式化聊天字段
						wsprintf( szChatWords, CStringReading::LoadString( 14, lType+101 ), strName.c_str(), strmem.c_str() );

						// 前缀长度
						lAdjustLen = lstrlen( szChatWords ) - strmem.size();
						pMainPage->m_lAdjPos = lAdjustLen;

						pMainPage->AdjGoodsInfoPos(lAdjustLen);
						GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(szChatWords,
																				pSetup->colorChatText[CHAT_TEAM],
																				pSetup->colorChatTextBack[CHAT_TEAM],
																				pSetup->colorChatTextShadow[CHAT_TEAM],
																				strName.c_str(),
																				CHAT_TEAM);
					}
					break;

				case CHAT_REGION:
				case CHAT_FACTION:
				case CHAT_UNION:
					{
						strName = pMsg->GetStr(str,1024);

						// 检查黑名单
						if( GetGame()->GetCGuiEx()->GetFriendsPageEx()->IsInGroup( strName.c_str(), CFriendsPageEx::PANEL_BLACK ) )
							break;

						strmem = pMsg->GetStr(str,1024);

						// 脏话过滤
						CClientWordsFilter::GetInstance()->Check(strmem, true, true);

						// 格式化聊天字段
						wsprintf( szChatWords, CStringReading::LoadString( 14, lType+101 ), strName.c_str(), strmem.c_str() );

						// 前缀长度
						lAdjustLen = lstrlen( szChatWords ) - strmem.size();
						pMainPage->m_lAdjPos = lAdjustLen;

						pMainPage->AdjGoodsInfoPos(lAdjustLen);
						GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords( szChatWords, 
																				pSetup->colorChatText[lType],
																				pSetup->colorChatTextBack[lType], 
																				pSetup->colorChatTextShadow[lType],
																				strName.c_str(),
																				lType);
					}
					break;

				case CHAT_BOARDCAST:
				case GM_CMD:
					{
						strName = pMsg->GetStr(str,1024);
						strmem = pMsg->GetStr(str,1024);

						// 脏话过滤
						CClientWordsFilter::GetInstance()->Check(strmem, true, true);

						GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords( strmem.c_str(),
																				pSetup->colorChatText[lType],
																				pSetup->colorChatTextBack[lType],
																				pSetup->colorChatTextShadow[lType],
																				strName.c_str(),
																				lType);
					}
					break;
			}
		}
		break;

	///////////////////////////////////////////////////////////////////////////////////////
	// 打开TalkBox
	///////////////////////////////////////////////////////////////////////////////////////
	case MSG_S2C_OTHER_OPENTALKBOX:
		{
			long lFlag = pMsg->GetLong();
			if( pMsg->GetPlayer() )
			{
				char str[2048];
				string strText = "";

				CGUID guScriptID;
				pMsg->GetGUID(guScriptID);
				// 传入的是字符串
				if(lFlag == 0)
				{
					strText = pMsg->GetStr(str,2048);				// 标题文字
				}
				// 传入索引
				else
				{
					DWORD dwType = pMsg->GetDWord();
					DWORD dwIndex = pMsg->GetDWord();
					strText = CStringReading::LoadString(dwType,dwIndex);
				}
				// 按钮相关信息
				string strBtn1 = "";
				string strBtn2 = "";
				strBtn1 = pMsg->GetStr(str,2048);
				bool bEnable1 = pMsg->GetByte()>0?true:false;
				strBtn2 = pMsg->GetStr(str,2048);
				bool bEnable2 = pMsg->GetByte()>0?true:false;

				long lStyle = pMsg->GetLong();
				// 新增TalkBox的数据
				BYTE bIsCloseWindow = 0;
				long lPicID = 0;
				long lPicX = 0;
				long lPicY = 0;
				long lSoundID = 0;
				long lBkMusicID = 0;
				// 不是关闭TalkBox的时候需要解新增数据
				if(lStyle)
				{
					bIsCloseWindow = pMsg->GetByte();
					lPicID = pMsg->GetLong();
					lPicX = pMsg->GetLong();
					lPicY = pMsg->GetLong();
					lBkMusicID = pMsg->GetLong();
					lSoundID = pMsg->GetLong();
				}
				CTalkBoxPageEx * pPage = GetGame()->GetCGuiEx()->GetTalkBoxPageEx();
				// 先执行一次关闭操作
				pPage->TidyPageOnClose();
				pPage->Release();
				if (lStyle == 0)	// 关闭对话框
				{
					return;
				}
				if (lStyle == 1)	// 背景1
				{
					pPage->OpenBox(CTalkBoxPageEx::NORMAL_TALKBOX);
				}
				else if (lStyle == 2)	// 背景2
				{
					pPage->OpenBox(CTalkBoxPageEx::NORMAL_TALKBOX);
				}
				if(bIsCloseWindow)
				{
					GetGame()->GetCGuiEx()->ClosePageExcept(pPage);
				}
				pPage->SetMessageText(strText.c_str());
				pPage->SetScriptID(guScriptID);
				// 播放音乐和声音
				pPage->PlaySounds(lSoundID);
				pPage->PlayBkMusic(lBkMusicID);
				pPage->SetBkImage(lPicID,lPicX,lPicY);
				// 设置链接按钮
				pPage->SetAnchorBtn(strBtn1,bEnable1);
				pPage->SetAnchorBtn(strBtn2,bEnable2);

				// 记录打开对话框时候的坐标
				extern CGUID g_OpenNpcTalkBoxRegionID;
				extern long g_lOpenNpcTalkBoxPosX;
				extern long g_lOpenNpcTalkBoxPosY;
				g_OpenNpcTalkBoxRegionID = GetGame()->GetRegion()->GetExID();
				g_lOpenNpcTalkBoxPosX = pMsg->GetPlayer()->GetTileX();
				g_lOpenNpcTalkBoxPosY = pMsg->GetPlayer()->GetTileY();
			}
			
		}
		break;
	case MSG_S2C_OTHER_OPENSTORYTALKBOX:
		{
			long lFlag = pMsg->GetLong();
			if( pMsg->GetPlayer() )
			{
				char str[2048];
				string strText = "";
				CGUID guScriptID;
				pMsg->GetGUID(guScriptID);
				// 传入的是字符串
				if(lFlag == 0)
				{
					strText = pMsg->GetStr(str,2048);				// 标题文字
				}
				// 传入索引
				else
				{
					DWORD dwType = pMsg->GetDWord();
					DWORD dwIndex = pMsg->GetDWord();
					strText = CStringReading::LoadString(dwType,dwIndex);
				}
				// 按任意键继续标志
				BYTE bIsContinue = pMsg->GetByte();
				// 新增TalkBox的数据
				BYTE bIsCloseWindow = pMsg->GetByte();
				long lSoundID = pMsg->GetLong();
				long lBkMusicID = pMsg->GetLong();
				long lHeadID = pMsg->GetLong();
				string strName = pMsg->GetStr(str,2048);
				// 表情动画队列
				long lFaceID1 = pMsg->GetLong();
				long lFaceTime1 = pMsg->GetLong();

				long lFaceID2 = pMsg->GetLong();
				long lFaceTime2 = pMsg->GetLong();

				long lFaceID3 = pMsg->GetLong();
				long lFaceTime3 = pMsg->GetLong();

				long lFaceID4 = pMsg->GetLong();
				long lFaceTime4 = pMsg->GetLong();

				long lFaceID5 = pMsg->GetLong();
				long lFaceTime5 = pMsg->GetLong();
				CTalkBoxPageEx * pPage = GetGame()->GetCGuiEx()->GetTalkBoxPageEx();
				// 打开
				pPage->Release();
				// 设置当前NPC的名字
				pPage->SetCurNpcName(strName);
				pPage->OpenBox(CTalkBoxPageEx::STORY_TALKBOX);
				pPage->SetIsContinueByAnyKey(bIsContinue);
				if(bIsCloseWindow)
				{
					GetGame()->GetCGuiEx()->ClosePageExcept(pPage);
				}
				char facenum[5];
				sprintf(facenum,"%04d",lFaceID1);
				lFaceID1 = *(DWORD*)facenum;
				sprintf(facenum,"%04d",lFaceID2);
				lFaceID2 = *(DWORD*)facenum;
				sprintf(facenum,"%04d",lFaceID3);
				lFaceID3 = *(DWORD*)facenum;
				sprintf(facenum,"%04d",lFaceID4);
				lFaceID4 = *(DWORD*)facenum;
				sprintf(facenum,"%04d",lFaceID5);
				lFaceID5 = *(DWORD*)facenum;
				pPage->SetModelIcon(TYPE_TALKBOXMODEL,lHeadID,
					lFaceID1,lFaceTime1,
					lFaceID2,lFaceTime2,
					lFaceID3,lFaceTime3,
					lFaceID4,lFaceTime4,
					lFaceID5,lFaceTime5,
					40,320,D3DXToRadian(0.0f));
				pPage->SetMessageText(strText.c_str());
				pPage->SetScriptID(guScriptID);
				// 播放音乐和声音
				pPage->PlaySoundsEx(lSoundID);
				pPage->PlayBkMusic(lBkMusicID);
				// 记录打开对话框时候的坐标
				extern CGUID g_OpenNpcTalkBoxRegionID;
				extern long g_lOpenNpcTalkBoxPosX;
				extern long g_lOpenNpcTalkBoxPosY;
				g_OpenNpcTalkBoxRegionID = GetGame()->GetRegion()->GetExID();
				g_lOpenNpcTalkBoxPosX = pMsg->GetPlayer()->GetTileX();
				g_lOpenNpcTalkBoxPosY = pMsg->GetPlayer()->GetTileY();
			}
		}
		break;
	case MSG_S2C_OTHER_OPENPIXYTALKBOX:
		{
			long lFlag = pMsg->GetLong();

				char str[2048];
				string strText = "";
				CGUID guScriptID;
				pMsg->GetGUID(guScriptID);
				// 传入的是字符串
				if(lFlag == 0)
				{
					strText = pMsg->GetStr(str,2048);				// 标题文字
				}
				// 传入索引
				else
				{
					DWORD dwType = pMsg->GetDWord();
					DWORD dwIndex = pMsg->GetDWord();
					strText = CStringReading::LoadString(dwType,dwIndex);
				}
				//背景语音
				long lSoundID = pMsg->GetLong();
				long lHeadID = pMsg->GetLong();
				// 表情动画队列
				long lFaceID1 = pMsg->GetLong();
				long lFaceTime1 = pMsg->GetLong();

				long lFaceID2 = pMsg->GetLong();
				long lFaceTime2 = pMsg->GetLong(); 
				lFaceTime2 = 1;

				long lFaceID3 = pMsg->GetLong();
				long lFaceTime3 = pMsg->GetLong();

				long lFaceID4 = pMsg->GetLong();
				long lFaceTime4 = pMsg->GetLong();

				long lFaceID5 = pMsg->GetLong();
				long lFaceTime5 = pMsg->GetLong();
				CPixyTalkBoxPageEx * pPage = GetGame()->GetCGuiEx()->GetPixyTalkBoxPageEx();
				// 打开
				if(strcmp(strText.c_str(),"") != 0)
				{
					if(pPage->IsOpen())
					{
                       pPage->Release();
					}
					pPage->Open();
					pPage->SetMessageText(strText.c_str());
					pPage->SetIsContinueByAnyKey(true); 
				}else{
					pPage->Release();
					return;
				}

				char facenum[5];
				sprintf(facenum,"%04d",lFaceID1);
				lFaceID1 = *(DWORD*)facenum;
				sprintf(facenum,"%04d",lFaceID2);
				lFaceID2 = *(DWORD*)facenum;
				sprintf(facenum,"%04d",lFaceID3);
				lFaceID3 = *(DWORD*)facenum;
				sprintf(facenum,"%04d",lFaceID4);
				lFaceID4 = *(DWORD*)facenum;
				sprintf(facenum,"%04d",lFaceID5);
				lFaceID5 = *(DWORD*)facenum;
				pPage->SetModelIcon(TYPE_TALKBOXMODEL,lHeadID,
					lFaceID1,lFaceTime1,
					lFaceID2,lFaceTime2,
					lFaceID3,lFaceTime3,
					lFaceID4,lFaceTime4,
					lFaceID5,lFaceTime5,
					40,160,D3DXToRadian(0.0f));
				pPage->SetScriptID(guScriptID);
				// 播放音乐和声音
				pPage->PlaySoundsEx(lSoundID);
		}
		break;

	///////////////////////////////////////////////////////////////////////////////////////
	// 接受对象属性
	///////////////////////////////////////////////////////////////////////////////////////
	case MSG_S2C_OTHER_SENDOBJECTPROPERTIY:
		{
			long lType = pMsg->GetLong();
			CGUID id;
			pMsg->GetGUID(id);
					
			CBaseObject* pObj = GetGame()->GetRegion()->RecursiveFindObject( lType, id );
			if( pObj )
			{
				switch(lType)
				{
				case TYPE_PLAYER:
					{
						CPlayer* pPlayer = (CPlayer*)pObj;
						BYTE ByteData[102400];
						pMsg->GetEx(ByteData,102400);
						long lPos = 0;
						pPlayer->DecordFromByteArray( ByteData, lPos );
						pPlayer->SetPosX( pPlayer->GetPosX() );
						pPlayer->SetPosY( pPlayer->GetPosY() );

					}
					break;

				/*case TYPE_NPC:
					{
						CNpc* pNpc = (CNpc*)pObj;
						BYTE ByteData[102400];
						pMsg->GetEx(ByteData,102400);
						long lPos = 0;
						pNpc->DecordFromByteArray( ByteData, lPos, false );
						pNpc->SetPosX( pNpc->GetPosX() );
						pNpc->SetPosY( pNpc->GetPosY() );
					}
					break;*/

				case TYPE_GOODS:
					{
						CGoods* pGoods = (CGoods*)pObj;
						BYTE ByteData[102400];
						pMsg->GetEx(ByteData,102400);
						long lPos = 0;
						pGoods->DecordFromByteArray( ByteData, lPos );
					}
					break;

				case TYPE_MONSTER:
					{
						CMonster* pMonster = (CMonster*)pObj;
						BYTE ByteData[102400];
						pMsg->GetEx(ByteData,102400);
						long lPos = 0;
						pMonster->DecordFromByteArray(ByteData, lPos );
					}
					break;
				}
			}
		}
		break;

	// 改变对象属性
	case MSG_S2C_OTHER_CHANGESHAPEPROPERTIY:
		{
			long lType = pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);
			char str[256] = "";
			pMsg->GetStr(str,256);
			long lChangeNum = pMsg->GetLong();

			CShape* p = (CShape*)GetGame()->GetRegion()->FindChildObject(lType, ID);
			if( p )
			{
				p->ChangeValue(str, lChangeNum);
				if( p->GetType() == TYPE_PLAYER )
				{
					//((CPlayer*)p)->UpdateProperty();
				}
			}
		}
		break;
	
	// 设置对象属性
	case MSG_S2C_OTHER_SETSHAPEPROPERTIY:
		{
			long lType = pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);
			char str[256] = "";
			pMsg->GetStr(str,256);
			long lSetNum = pMsg->GetLong();

			CShape* p = (CShape*)GetGame()->GetRegion()->FindChildObject(lType, ID);
			if( p )
			{
				p->SetValue(str, lSetNum);
				if( p->GetType() == TYPE_PLAYER )
				{
					//((CPlayer*)p)->UpdateProperty();
				}
			}
		}
		break;

	// 广播
	case MSG_S2C_OTHER_BROADCAST:
		{
			char strVal[1024];

			long lRegionID = pMsg->GetLong();
			long lImportLevel = pMsg->GetLong();
			D3DCOLOR colorText = pMsg->GetDWord();
			D3DCOLOR colorBack = pMsg->GetDWord();
			pMsg->GetStr(strVal,1024);

			GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(strVal, colorText, colorBack,0xff000000,"",CHAT_BOARDCAST);
			GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords( strVal, colorText, colorBack, 0, "", CHAT_SYSTEM );

			// 王峰：置顶广播
			CTopMsg::GetInstance()->AddBroadcastMsg( strVal );
		}
		break;

	// 王峰：紧急脚本公告（高优先级）
	case MSG_S2C_OTHER_NEWS_TOP_WORLD:
		{
			char szNews[1024];

			long lLoop = pMsg->GetLong();
			pMsg->GetStr( szNews, 1024 );

			long lBKIndex = pMsg->GetLong();

			GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords( szNews,
				pSetup->colorScriptBroadcast, pSetup->colorScriptBroadcastBack, pSetup->colorScriptBroadcastShadow, "", CHAT_BOARDCAST );
			GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords( szNews, pSetup->colorScriptBroadcast, pSetup->colorScriptBroadcastBack, 0, "", CHAT_SYSTEM );

			// 
			CTopMsg::GetInstance()->AddBroadcastMsg( szNews, true, lLoop, lBKIndex );
		}
		break;

	// 消息
	case MSG_S2C_OTHER_ADDINFO:
		{
			char *strText = NULL;
			D3DCOLOR color = pMsg->GetDWord();
			D3DCOLOR bkColor = pMsg->GetDWord();
			BYTE bPos = pMsg->GetByte();
			char str[1024];
			strText = pMsg->GetStr(str,1024);
			GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(bPos,strText,color,bkColor);
		}
		break;
		// 添加提示消息（索引）
	case MSG_S2C_OTHER_ADDINFO_BYINDEX:
		{
			D3DCOLOR color = pMsg->GetDWord();			// 文本颜色
			D3DCOLOR bkColor = pMsg->GetDWord();		// 文本背景色
			BYTE bPos = pMsg->GetByte();				// 文本的显示位置
			DWORD dwType = pMsg->GetDWord();			// 字符串类型
			DWORD dwIndex = pMsg->GetDWord();			// 字符串的索引
			string strText = CStringReading::LoadString(dwType,dwIndex);
			GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(bPos,strText.c_str(),color,bkColor);
		}
		break;
	case MSG_S2C_OTHER_ADDSYSTEMINFO:
		{
			D3DCOLOR color = pMsg->GetDWord();
			char str[1024];
			pMsg->GetStr(str,1024);
			GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_SYSTEM,str,color);
		}
		break;
	// 消息
	case MSG_S2C_OTHER_GAMEMESSAGE:
		{
			char str[1024];
			pMsg->GetStr(str,1024);
			long type = pMsg->GetLong();
			CGUID guScriptID;
			pMsg->GetGUID(guScriptID);
			CMessageBoxPageEx *pMsgPage = GetGame()->GetCGuiEx()->GetMessageBoxPageEx();
			if (type == 1)
			{
				type = CMessageBoxPageEx::MSG_OK_NO;
			}
			else
			{
				type = CMessageBoxPageEx::MSG_OK;
			}
			pMsgPage->SetScriptID(guScriptID);
			pMsgPage->Open(pMsgPage,str,type,true);
			//GetGame()->GetInterface()->GetMsgBox()->Run(str, type, true);
			//GetGame()->GetInterface()->GetMsgBox()->SetScriptID(lScriptID);
		}
		break;

	// 定时PING
	case MSG_S2C_OTHER_PING:
		{
			//WORD wState = pMsg->GetWord();
			//GetGame()->GetMainPlayer()->SetState(wState);

			GetGame()->SetLastPingTime(GetCurTickCount());
			//pMsg->Send();
		}
		break;
	case MSG_S2C_OTHER_VIEWGOODSLINK:
		{
			long lPlayerID=pMsg->GetLong();
			long bSuccessful=pMsg->GetLong();
			if(bSuccessful==1)
			{
				CGoods* pGoods=new CGoods;
				BYTE ByteData[102400];
				pMsg->GetEx(ByteData,102400);
				long lPos = 0;
				pGoods->DecordFromByteArray(ByteData,lPos);
				/*CGoodsLinkPage* pGoodsLinkPage=new CGoodsLinkPage;
				pGoodsLinkPage->Init(pGoods);
				pGoodsLinkPage->Open();	
				CGoodsLinkPage::SetLinkRequestSendedFlag(false);*/
			}
		}
		break;
	case MSG_S2C_OTHER_SCRIPTTIMERRUN:
		{
			long lTime = pMsg->GetLong();
			GetGame()->SetScriptTimer(lTime);
		}
		break;
	case MSG_S2C_OTHER_CHANGENAME_OPENUI:
		{
			if (GetGame()->GetMainPlayer() == NULL)
				break;

			char ret=pMsg->GetChar();
			if(ret==0)
			{
				//GetGame()->GetInterface()->GetMsgBox()->Run("你不能更改名字！");
				break;
			}
			//GetGame()->GetInterface()->GetInputPasswordPage()->SetShowText("请输入您的新名字");
			/*if(GetGame()->GetInterface()->GetInputPasswordPage()->Run()==1)
			{
				if(strlen(GetGame()->GetInterface()->GetInputPasswordPage()->GetPassword())>3)
				{
					CMessage msg(MSG_C2S_OTHER_CHANGENAME);
					msg.Add(GetGame()->GetMainPlayer()->GetID());
					msg.Add(GetGame()->GetInterface()->GetInputPasswordPage()->GetPassword());
					msg.Send();
				}
			}*/
		}
		break;
	case MSG_S2C_OTHER_CHANGENAME:
		{
			if (GetGame()->GetMainPlayer() == NULL)
				break;

			CGUID ID;
			pMsg->GetGUID(ID);
			char strName[32];
			pMsg->GetStr(strName,sizeof(strName));
			char ret=(char)pMsg->GetByte();
			CPlayer* pPlayer=(CPlayer *)GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,ID);
			if(pPlayer==NULL)
				break;
			if(ret==1)//成功
			{
				pPlayer->SetName(strName);
				if(ID==GetGame()->GetMainPlayer()->GetExID())
				{
					//GetGame()->GetInterface()->GetMsgBox()->Run("你更改名字成功！");
					GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,CStringReading::LoadString(206,216));
                    GetGame()->GetCGuiEx()->GetMainBarPageEx()->UpdateName();
				}
				else
				{
					string str=pPlayer->GetName();
					//str+="更改名字为：";
					//str+=strName;
					//GetGame()->GetInterface()->AddText((char *)str.c_str(),0xffff0000);
				}

			}
			else if(ID==GetGame()->GetMainPlayer()->GetExID())
			{
				if(ret==-1)
				{
						// 名字已存在
						GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,CStringReading::LoadString(206,217));
				}else if(ret==-2)
				{
						// 名字不合法
						GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,CStringReading::LoadString(206,218));
				}else if(ret==0)
				{
						GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,CStringReading::LoadString(206,219));
				}
				//GetGame()->GetInterface()->GetMsgBox()->Run("你更改名字失败！");
			}
		}
		break;
	case MSG_S2C_OTHER_AddTopInfo:
		{
			CGUID lPlayerID;
			pMsg->GetGUID(lPlayerID);
			long lTopInfoID = pMsg->GetLong();
			long lFlag = pMsg->GetLong();
			long lParam = pMsg->GetLong();
			char strInfo[256]="";
			pMsg->GetStr(strInfo,256);
			GetGame()->AddTopInfo(lTopInfoID,lFlag,lParam,string(strInfo));
		}
		break;
	case MSG_S2C_OTHER_ADDPLAYERINFO:
		{
			if (GetGame()->GetMainPlayer() == NULL)
				break;
			// 字符or索引
			long lFlag = pMsg->GetLong();
			char *strText = NULL;

			DWORD dwForeColor=pMsg->GetDWord();
			DWORD dwBackColor=pMsg->GetDWord();
			char str[256];
			if(lFlag==0)
				strText = pMsg->GetStr(str,256);
			else
			{
				DWORD dwType = pMsg->GetDWord();
				DWORD dwIndex = pMsg->GetDWord();
				strText = CStringReading::LoadString(dwType,dwIndex);
			}
			if(GetGame()->GetCGuiEx()->GetMainBarPageEx())
				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,strText,dwForeColor);
			//信息提示音效
			//if(lFlag==0)
			   //GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-39.wav"); 
		}
		break;
	case MSG_S2C_OTHER_SHOWREADMEWINDOW:
		{
			//GetGame()->GetInterface()->GetReadmePage()->Open();
		}
		break;

		// NPC名字列表
	case MSG_S2C_OTHER_ANSWERNPCLIST:
		{
			vector<CNpcListPageEx::tagNpcList>* pList = GetGame()->GetCGuiEx()->GetNpcListPageEx()->GetNpcList();
			pList->clear();
			long lNum = pMsg->GetLong();
			long lLen = pMsg->GetLong();
			BYTE* str = new BYTE[lLen+1];
			pMsg->GetEx(str,lLen);
			long pos = 0;
			char strName[256];
			int lX, lY;
			for (int i=0; i<lNum; i++)
			{
				_GetStringFromByteArray(str,pos,strName);
				lX = _GetDwordFromByteArray(str,pos);
				lY = _GetDwordFromByteArray(str,pos);
				if(strcmp(strName,"$space"))
				{
					CNpcListPageEx::tagNpcList st(strName, lX, lY);
					pList->push_back(st);
				}
			}

			SAFE_DELETE_ARRAY(str);
			GetGame()->GetCGuiEx()->GetNpcListPageEx()->Close();
			GetGame()->GetCGuiEx()->GetNpcListPageEx()->UpdateList();
		}
		break;

	case MSG_S2C_OTHER_TALK_WORLD:
		{
			if (GetGame()->GetMainPlayer() == NULL)
				break;

			BYTE bIsOk;
			char szName[256];
			char szContent[1024];
			char szOutText[2056];
			string strContent;
			memset(szName,'\0',256);
			//=========================对话信息中的聊天信息=======================
			long lAdjustLen = 0;
			CMainBarPageEx * pMainPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
			list<CEditField::stSpecialTextInfo> * pTextInfoList = pMainPage->GetSpecialText();
			pTextInfoList->clear();
			char strGoodsName[1024] = "";
			long lNum = pMsg->GetLong();
			for(long i=0; i<lNum; ++i)
			{
				CEditField::stSpecialTextInfo stInfo;
				pMsg->GetEx(&stInfo, sizeof(CEditField::stSpecialTextInfo));
				stInfo.str = pMsg->GetStr(strGoodsName,1024);
				pTextInfoList->push_back(stInfo);
			}
			//====================================================================

			bIsOk = pMsg->GetByte();
			if(bIsOk)
			{
				pMsg->GetStr(szName, 256);
				lAdjustLen = lstrlen(szName)+11;

				pMsg->GetStr(szContent,1024);
				// 脏话过滤
				strContent = szContent;
				CClientWordsFilter::GetInstance()->Check(strContent, true, true);
				// 格式化聊天字段
				wsprintf( szOutText, CStringReading::LoadString( 14, CHAT_WOLRD+101 ), szName, strContent.c_str() );
			}
			else
			{
				sprintf(szOutText,CStringReading::LoadString(eIDS_BASEUI,eIDS_STR_WORLDCHAT_UNSUCCESS));
			}

			// 前缀长度
			lAdjustLen = lstrlen( szOutText ) - lstrlen( szContent );
			pMainPage->m_lAdjPos = lAdjustLen;

			pMainPage->AdjGoodsInfoPos(lAdjustLen);
			GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(szOutText,
				CGlobeSetup::GetSetup()->colorChatText[CHAT_WOLRD],
				CGlobeSetup::GetSetup()->colorChatTextBack[CHAT_WOLRD],
				CGlobeSetup::GetSetup()->colorChatTextShadow[CHAT_WOLRD],
				szName,CHAT_WOLRD);
		}
		break;
		// 盟国
	case MSG_S2C_OTHER_TALK_COUNTRY_UNION:
		{
			if (GetGame()->GetMainPlayer() == NULL)
				break;

			BYTE bIsOk;
			char szName[256];
			char szContent[1024];
			char szOutText[2056];
			string strContent;
			memset(szName,'\0',256);
			//=========================对话信息中的聊天信息=======================
			long lAdjustLen = 0;
			CMainBarPageEx * pMainPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
			list<CEditField::stSpecialTextInfo> * pTextInfoList = pMainPage->GetSpecialText();
			pTextInfoList->clear();
			char strGoodsName[1024] = "";
			long lNum = pMsg->GetLong();
			for(long i=0; i<lNum; ++i)
			{
				CEditField::stSpecialTextInfo stInfo;
				pMsg->GetEx(&stInfo, sizeof(CEditField::stSpecialTextInfo));
				stInfo.str = pMsg->GetStr(strGoodsName,1024);
				pTextInfoList->push_back(stInfo);
			}
			//====================================================================

			bIsOk = pMsg->GetByte();
			if(bIsOk)
			{
				pMsg->GetStr(szName, 256);
				lAdjustLen = lstrlen(szName)+11;
				// 检查黑名单
				if( GetGame()->GetCGuiEx()->GetFriendsPageEx()->IsInGroup( szName, CFriendsPageEx::PANEL_BLACK ) )
					break;

				pMsg->GetStr(szContent,1024);
				// 脏话过滤
				strContent = szContent;
				CClientWordsFilter::GetInstance()->Check(strContent, true, true);
				// 格式化聊天字段
				wsprintf( szOutText, CStringReading::LoadString( 14, CHAT_COUNTRY_UNION+101 ), szName, strContent.c_str() );
			}
			else
			{
				sprintf(szOutText,CStringReading::LoadString(eIDS_BASEUI,eIDS_STR_UNIONCOUNCHAT_UNSUCCESS));
			}

			// 前缀长度
			lAdjustLen = lstrlen( szOutText ) - lstrlen( szContent );
			pMainPage->m_lAdjPos = lAdjustLen;

			pMainPage->AdjGoodsInfoPos(lAdjustLen);
			GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(szOutText,
				CGlobeSetup::GetSetup()->colorChatText[CHAT_COUNTRY_UNION],
				CGlobeSetup::GetSetup()->colorChatTextBack[CHAT_COUNTRY_UNION],
				CGlobeSetup::GetSetup()->colorChatTextShadow[CHAT_COUNTRY_UNION],
				szName,CHAT_COUNTRY_UNION);
		}
		break;

	case MSG_S2C_OTHER_TALK_COUNTRY:
		{
			if (GetGame()->GetMainPlayer() == NULL)
				break;

			BYTE bIsOk;
			char szName[256];
			char szContent[1024];
			char szOutText[2056];
			string strContent;
			memset(szName,'\0',256);
			//=========================对话信息中的聊天信息=======================
			long lAdjustLen = 0;
			CMainBarPageEx * pMainPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
			list<CEditField::stSpecialTextInfo> * pTextInfoList = pMainPage->GetSpecialText();
			pTextInfoList->clear();
			char strGoodsName[1024] = "";
			long lNum = pMsg->GetLong();
			for(long i=0; i<lNum; ++i)
			{
				CEditField::stSpecialTextInfo stInfo;
				pMsg->GetEx(&stInfo, sizeof(CEditField::stSpecialTextInfo));
				stInfo.str = pMsg->GetStr(strGoodsName,1024);
				pTextInfoList->push_back(stInfo);
			}
			//====================================================================

			bIsOk = pMsg->GetByte();
			if(bIsOk)
			{
				pMsg->GetStr(szName, 256);
				lAdjustLen = lstrlen(szName)+11;
				// 检查黑名单
				if( GetGame()->GetCGuiEx()->GetFriendsPageEx()->IsInGroup( szName, CFriendsPageEx::PANEL_BLACK ) )
					break;

				pMsg->GetStr(szContent,1024);
				// 脏话过滤
				strContent = szContent;
				CClientWordsFilter::GetInstance()->Check(strContent, true, true);
				// 格式化聊天字段
				wsprintf( szOutText, CStringReading::LoadString( 14, CHAT_COUNTRY+101 ), szName, strContent.c_str() );
			}
			else
			{
				sprintf(szOutText,CStringReading::LoadString(eIDS_BASEUI,eIDS_STR_COUNCHAT_UNSUCCESS));
			}

			// 前缀长度
			lAdjustLen = lstrlen( szOutText ) - lstrlen( szContent );
			pMainPage->m_lAdjPos = lAdjustLen;

			pMainPage->AdjGoodsInfoPos(lAdjustLen);
			GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(szOutText,
				CGlobeSetup::GetSetup()->colorChatText[CHAT_COUNTRY],
				CGlobeSetup::GetSetup()->colorChatTextBack[CHAT_COUNTRY],
				CGlobeSetup::GetSetup()->colorChatTextShadow[CHAT_COUNTRY],
				szName,CHAT_COUNTRY);
		}
		break;

	// 王峰：置顶聊天
	case MSG_S2C_OTHER_TALK_TOP_WORLD:
		{
			CTopMsg::GetInstance()->RecvTopChatMsg( pMsg );
		}

	// 国家通知
	case MSG_S2C_OTHER_TALK_NOTIFY:
		{
			BYTE btType;
			char szContent[1024];

			btType = pMsg->GetByte();
			pMsg->GetStr(szContent,1024);

			switch( btType)
			{
			case 1:
				// 本国人被杀
				//GetGame()->GetInterface()->AddOtherInfo(szContent);
				break;

			case 2:
				// 杀死敌人
				//GetGame()->GetInterface()->AddSystemInfo(szContent,0XFF00FF00,10000);
				break;
			case 0:
			default:
				// 被袭击
				//GetGame()->GetInterface()->AddOtherInfo(szContent);
				break;
			}
		}
		break;
		//转换摄像机
	case MSG_S2C_OTHER_TRANSFORCAMERA:
		{
			//转换方式 
			long lTransformMode = pMsg->GetLong();
			long lParam1 =  pMsg->GetLong();
			long lParam2 =  pMsg->GetLong();
			long lParam3 =  pMsg->GetLong();

			GetGame()->GetRegion()->TransformCamer(lTransformMode,lParam1,lParam2,lParam3);
		}
		break;
		//操作界面
	case MSG_S2C_OTHER_OPERAINTERFACE:
		{
			//转换方式 
			long lOperaMode = pMsg->GetLong();
			//界面名字
			char pszInterfaceName[256];
			pMsg->GetStr(pszInterfaceName,256);
			/*CComponent * pParentComp = NULL;
			CComponent * pComp = NULL;
			if(IGUIRoot::GetGUIInstance()->GetGUIComp())
			{
				pParentComp = IGUIRoot::GetGUIInstance()->GetGUIComp()->GetContentContainer();
				if(pParentComp)
					pComp = pParentComp->GetComponent(pszInterfaceName);
			}*/
			switch(lOperaMode)
			{
				//打开指定界面
			case 1:
				{
					GetGame()->GetCGuiEx()->OpenPageByName(pszInterfaceName);
				}
				break;
				//关闭指定界面
			case 2:
				{
					GetGame()->GetCGuiEx()->ClosePageByName(pszInterfaceName);
				}
				break;
				//关闭所有界面
			case 3:
				{
					GetGame()->GetCGuiEx()->CloseAllPage();
				}
				break;
			}
		}
		break;
		//设置帮助提示
	case MSG_S2C_OTHER_SETHELPHINT:
		{
			BYTE bDel = pMsg->GetByte();
			long lHintID = pMsg->GetLong();
			//删除提示
			if(bDel)
			{
				GetGame()->GetCGuiEx()->GetClewInfo()->DeleteClewInfo(lHintID);
			}
			//添加帮助提示
			else
			{
				//标志位，向服务器发消息的时候，需要返回
				long lFlag = pMsg->GetLong();
				//删除的超时时间
				long lFutureDelTime = pMsg->GetLong();

				GetGame()->GetCGuiEx()->GetClewInfo()->AddClewInfo(lHintID,lFlag,lFutureDelTime);
			}
		}
		break;
		//设置界面提示
	case MSG_S2C_OTHER_SETINTERFACEHINT:
		{
			BYTE bDel = pMsg->GetByte();
			long lHintID = pMsg->GetLong();
			//删除界面提示
			if(bDel)
			{
				GetGame()->GetCGuiEx()->GetTopPage()->DeleteRectFocus(lHintID);
			}
			//添加界面提示
			else
			{
				//标志位，向服务器发消息的时候，需要返回
				long lFlag = pMsg->GetLong();
				//删除的超时时间
				long lFutureDelTime = pMsg->GetLong();
				//主窗口的名字
				char pszInterfaceName[256]="";
				pMsg->GetStr(pszInterfaceName,256);
				//聚焦框的相对于主窗口的相对位置和大小
				long lX = pMsg->GetLong();
				long lY = pMsg->GetLong();
				long lWidth = pMsg->GetLong();
				long lHeight = pMsg->GetLong();
				// 根据名字打开对应的页面
				CGuiExPage *pPage = GetGame()->GetCGuiEx()->OpenPageByName(pszInterfaceName);
				// 查找对应的主界面
				CGUIComp * pParentComp = IGUIRoot::GetGUIInstance()->GetGUIComp();
				if(pParentComp)
				{
					CComponent * pComp = pParentComp->GetComponentByName(pszInterfaceName);
					if(pComp)
					{
						RECT rt;
						rt.left = pComp->GetXOnWindow() + lX;
						rt.right = rt.left + lWidth;
						rt.top = pComp->GetYOnWindow() + lY;
						rt.bottom = rt.top + lHeight;
						GetGame()->GetCGuiEx()->GetTopPage()->AddRectFocus(pPage,lHintID,lFlag,lFutureDelTime,rt);
					}
				}
			}
		}
		break;

		// 播放动画特效
	case MSG_S2C_OTHER_EFFECT:
		{
			GetGame()->GetCGuiEx()->GetEffectUIManager()->AddGroup( pMsg->GetDWord() );
		}
		break;

		// 播放仪态动作 - add by lpf(2008.04.11)
	case MSG_S2C_OTHER_EXPATC:
		{
			//string		 sChatMsg;
			CMoveShape * pSourceMoveShape = NULL;
			CMoveShape * pTargetMoveShape = NULL;
			tagFaceAction * stpFaceAction = NULL;

			// 得到服务器数据
			DWORD dwFaceActionID = pMsg->GetDWord();
			long  lSourceObjType = pMsg->GetLong();
			CGUID SourceObjGUID;
			pMsg->GetGUID(SourceObjGUID);

			long  lTargetObjType = pMsg->GetLong();
			CGUID TargetObjGUID;
			pMsg->GetGUID(TargetObjGUID);

			// 得到需要处理的对象
			CClientRegion * pRegion = GetGame()->GetRegion();
			if (pRegion == NULL)
				break;

			pSourceMoveShape = dynamic_cast<CMoveShape *>(pRegion->FindChildObject(lSourceObjType, SourceObjGUID));
			pTargetMoveShape = dynamic_cast<CMoveShape *>(pRegion->FindChildObject(lTargetObjType, TargetObjGUID));

			// 设定表情动画
			GetGame()->GetFaceActionList()->SetupFaceAction(dwFaceActionID, pSourceMoveShape, lSourceObjType, pTargetMoveShape, lTargetObjType);
			if(pSourceMoveShape)
			{
				pSourceMoveShape->InitActionAudio(dwFaceActionID);
			}
		}
		break;
		// 物品信息回复
	case MSG_S2C_OTHER_GoodsInfo_Re:
		{
			CGUID PlayerGUID;
			pMsg->GetGUID(PlayerGUID);
			CGUID GUID;
			pMsg->GetGUID(GUID);
			DWORD dwSize=pMsg->GetDWord();
			if( dwSize==0 )
				break;
			BYTE *pBuff=new BYTE[dwSize];
			pMsg->GetEx(pBuff,dwSize);
			CGoods *pGoods=new CGoods();
			LONG lPos=0;
			pGoods->DecordFromByteArray(pBuff,lPos);
			SAFE_DELETE_ARRAY(pBuff);
			list<CGame::stChatGoodsInfo> * pChatGoodsInfo = GetGame()->GetChatGoodsInfoList();
			if(pChatGoodsInfo)
			{
				if(pChatGoodsInfo->size() >= 120)
				{
					SAFE_DELETE(pChatGoodsInfo->front().pGoods);
					pChatGoodsInfo->erase(pChatGoodsInfo->begin());
				}
				CGame::stChatGoodsInfo info;
				info.idEx = GUID;
				info.pGoods = pGoods;
				pChatGoodsInfo->push_back(info);
			}
		}
		break;
		//! 一条带物品信息的消息
	case MSG_S2C_OTHER_ADDINFO_AND_GOODSINFO:
		{
			char str[256]="";
			pMsg->GetStr(str,256);
			char strName[256]="";
			pMsg->GetStr(strName,256);
			tagChatsGoodsInfo tgGoodsInfo;
			pMsg->GetEx(&tgGoodsInfo,sizeof(tagChatsGoodsInfo));
			CMainBarPageEx * pMainPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
			if(pMainPage)
			{
				// 本次信息中的特殊字符
				list<CEditField::stSpecialTextInfo> * pTextInfoList = pMainPage->GetSpecialText();
				pTextInfoList->clear();
				CEditField::stSpecialTextInfo stInfo;
				stInfo.str = strName;
				stInfo.stInfo = tgGoodsInfo;
				pTextInfoList->push_back(stInfo);

				pMainPage->m_lAdjPos = 0;
				pMainPage->AdjGoodsInfoPos(0);
				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(str,
					CGlobeSetup::GetSetup()->colorChatText[CHAT_SYSTEM],
					CGlobeSetup::GetSetup()->colorChatTextBack[CHAT_SYSTEM],
					CGlobeSetup::GetSetup()->colorChatTextShadow[CHAT_SYSTEM],
					"",CHAT_SYSTEM);
			}
		}
		break;
		// 视频播放
	case MSG_S2C_OTHER_PLAY_VIDEO:
		{
			RECT rc;
			char szFileName[MAX_PATH];
			int  iLoop;
			pMsg->GetStr(szFileName, MAX_PATH);
			iLoop = pMsg->GetLong();
			rc.left = pMsg->GetLong();
			rc.top = pMsg->GetLong();
			rc.right = pMsg->GetLong();
			rc.bottom = pMsg->GetLong();
			CVideoManager::GetInstance()->PlayVideo(szFileName, iLoop, &rc);
		}
		break;
		//停止视频播放
	case MSG_S2C_OTHER_STOP_VIDEO:
		{
			int iIndex = pMsg->GetLong();
			CVideoManager::GetInstance()->StopVideo(iIndex);
		}
		break;
		//背包闪烁
	case MSG_S2C_OTHER_PackGlint:
		{
			CItemPageEx * pItems = GetGame()->GetCGuiEx()->GetItemPageEx();
			CMainBarPageEx * pMainbar = GetGame()->GetCGuiEx()->GetMainBarPageEx();
			if(pMainbar && pItems && !pItems->IsOpen() && !pItems->HasSubPackOpen())
			{
				// 在Mainbar上的任务按钮添加闪动效果
				if(pMainbar->GetInterfaceComp())
				{
					CButton *pQuestBtn = (CButton *)pMainbar->GetInterfaceComp()->GetComponent("Item");
					if(pQuestBtn && pQuestBtn->GetSpecialIcon())
						((CImageIcon *)pQuestBtn->GetSpecialIcon())->SetIsChangeAlpha(true);
				}
			}
		}
		break;
		// 帐号重复登陆
	case MSG_S2C_OTHER_REPEATLOGIN:
		{
			char str[256]="";
			pMsg->GetStr(str, 256);
			CLoginPageEx * pPage = GetGame()->GetCGuiEx()->GetLoginPageEx();
			CMessageBoxPageEx * pMesBox = GetGame()->GetCGuiEx()->GetMessageBoxPageEx();
			if(pPage && pMesBox)
			{
				pMesBox->Open(pPage,str,CMessageBoxPageEx::MSG_OK,true);
			}
		}
		break;
		//打开脚本输入框
	case MSG_S2C_OTHER_UPDATEVARVALUEOPEN:
		{
			//脚本ID
			CGUID ScriptsID;
			pMsg->GetGUID(ScriptsID);
			GetGame()->GetCGuiEx()->GetInputBoxPageEx()->SetScriptsGuid(ScriptsID);

			//显示文本
			char strTextOut[MAX_PATH];
			pMsg->GetStr(strTextOut, MAX_PATH);
			GetGame()->GetCGuiEx()->GetInputBoxPageEx()->SetTextOut(strTextOut);

			//存储变量
		    char strValue[MAX_PATH];
			pMsg->GetStr(strValue, MAX_PATH);
			GetGame()->GetCGuiEx()->GetInputBoxPageEx()->CheckBoxType(strValue);

			//打开输入框
			CPlayer *pPlayer = GetGame()->GetMainPlayer();
			if(pPlayer)
				GetGame()->GetCGuiEx()->GetInputBoxPageEx()->SetPlayerMoveRange( pPlayer->GetTileX(), pPlayer->GetTileY(), 4 );
			GetGame()->GetCGuiEx()->GetInputBoxPageEx()->Open();
			GetGame()->GetCGuiEx()->GetInputBoxPageEx()->SetChangeName(false);
		}
		break;

	case MSG_S2C_OTHER_UPDATENAMEOPEN:
		{
			//脚本ID
			CGUID ScriptsID;
			pMsg->GetGUID(ScriptsID);
			GetGame()->GetCGuiEx()->GetInputBoxPageEx()->SetScriptsGuid(ScriptsID);

			//显示文本
			char str[256]="";
			sprintf(str,"%s",CStringReading::LoadString(206,220));
			GetGame()->GetCGuiEx()->GetInputBoxPageEx()->SetTextOut(str);

			//存储变量
			GetGame()->GetCGuiEx()->GetInputBoxPageEx()->CheckBoxType("#");

			//打开输入框
			CPlayer *pPlayer = GetGame()->GetMainPlayer();
			if(pPlayer)
				GetGame()->GetCGuiEx()->GetInputBoxPageEx()->SetPlayerMoveRange( pPlayer->GetTileX(), pPlayer->GetTileY(), 4 );
			GetGame()->GetCGuiEx()->GetInputBoxPageEx()->Open();
			GetGame()->GetCGuiEx()->GetInputBoxPageEx()->SetChangeName(true);
		}
		break;

	// 王峰：活动公告
	case MSG_S2C_OTHER_SHOW_STRING:
		{
			// 是否显示开关
			if( pMsg->GetLong() )
			{
				// xy坐标
				long x = pMsg->GetLong();
				long y = pMsg->GetLong();

				// 显示的字符串
				TCHAR	szPlacard[2056];
				pMsg->GetStr( szPlacard, 2056 );

				// 设置
				if( CMainBarPageEx* pMainBarPage = GetGame()->GetCGuiEx()->GetMainBarPageEx() )
				{
					if( lstrlen( szPlacard ) )
					{
						pMainBarPage->SetPlacardVisibled( false );
						pMainBarPage->SetPlacard( szPlacard, x, y );
						pMainBarPage->SetPlacardVisibled( true );
					}
				}
			}
			else
			{
				// 关闭公告显示
				if( CMainBarPageEx* pMainBarPage = GetGame()->GetCGuiEx()->GetMainBarPageEx() )
				{
					pMainBarPage->SetPlacardVisibled( false );
				}
			}
		}
		break;
	}
}