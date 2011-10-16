/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：CDriveClientDlg.cpp
*  功能：主要用于管理操作DriveClient Dlg
*  当前版本：1.1.0
*  作者： MartySa
*  完成日期：
*  取代版本：1.0.0
*  原作者：
*  完成日期：  2009.2.2
*
============================================================
*/

#include "stdafx.h"
#include "GameTest.h"
#include "DriveClientDlg.h"
#include <process.h>
#include "GameTestDlg.h"
//#include "..\App\AutoDriveClient\AutoDriveClient.h"
//#include "..\Script\MapScriptSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CDriveClientDlg, CDialog)

CDriveClientDlg::CDriveClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDriveClientDlg::IDD, pParent),
	 m_DriveClientManager(NULL)
	 , m_lStartCDkey(0)
	 , m_lEndCDkey(0)
{
	m_DriveClientManager = NULL;
	m_pTimer = NULL;
}

CDriveClientDlg::~CDriveClientDlg()
{
	//CloseHandle(m_stOutputInfo.hEvent);
	//CloseHandle(m_stOutputInfo.hThrd);
	//m_stOutputInfo.bExit = TRUE;
}

void CDriveClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	m_lStartCDkey = 1111;
	m_lEndCDkey   = 1111;
	DDX_Text(pDX, IDC_EDIT1, m_lStartCDkey);
	DDX_Text(pDX, IDC_EDIT2, m_lEndCDkey);
}


BEGIN_MESSAGE_MAP(CDriveClientDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDriveClientDlg::OnBnClickedButtonStart)
	ON_EN_CHANGE(IDC_EDIT1, &CDriveClientDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()

//
////子线程调用
//UINT WINAPI OutputThread(void* lParam)
//{
//	CGameTestDlg* pTestDlg = (CGameTestDlg*)lParam;
//	if( pTestDlg != NULL )
//	{	
//		CDriveClientDlg *pDriveDlg = pTestDlg->GetDriveDlg();
//		//SetEvent(pDriveDlg->GetstOutPutInfo()->hEvent);
//		for(;;)
//		{
//			//extern m_cs;
//			//WaitForSingleObject(pDriveDlg->GetstOutPutInfo()->hEvent, INFINITE);
//			// MSG_FILE Loop 
//			if(pDriveDlg->GetThreadExit())
//				break;
//			EnterCriticalSection( pTestDlg->GetTestCS());
//			// do some thing
//			//获取消息队列
//			
//
//			//设置消息队列
//			
//			CDriveClient *DriveClientManager = pTestDlg->GetDriveClientManager();
//			CDriveGameClient *gameclient = (CDriveGameClient*)DriveClientManager->_FindGameClient("GM1111");
//			msgQueue *MsgQueue = gameclient->GetMsgQueueMSG_FILE();
//			if ( DriveClientManager && gameclient && MsgQueue)
//			{
//				if ( MsgQueue->size()>0 )
//				{	
//					msgQueue::iterator it = MsgQueue->begin();
//					for ( ; it!=MsgQueue->end(); ++it)
//					{
//						CMessage *pMsg = (CMessage*)(*it);		
//						if ( pMsg )
//						{	
//							long lType = pMsg->GetType();
//							lType = lType - (lType&0x00000ff);	
//
//							switch( lType )
//							{	
//								//for内存映射文件 文件操作
//								case MSG_FILE:
//									{	
//										long lType = pMsg->GetType();
//									
//										switch(lType)
//										{				
//											case MSG_FILE_WRITE:
//											{
//												char strFileName[256] = " ";
//												pMsg->GetStr(strFileName, 256);
//
//												char strFiletxt[256] = " ";
//												pMsg->GetStr(strFiletxt, 256);
//												
//												if ( ::strlen(strFileName)>0 && ::strlen(strFiletxt)>0 )
//												{
//													pTestDlg->GetMapFileManager()->WriteFile(strFileName, strFiletxt);
//												}
//												
//											}
//											break;
//
//											case MSG_FILE_CLEAR:
//											{
//												char strFileName[256] = " ";
//												pMsg->GetStr(strFileName, 256);
//												
//												if ( ::strlen(strFileName)>0 )
//												{											
//													pTestDlg->GetMapFileManager()->ClearFile(strFileName);
//												}
//											}
//											break;
//
//											case MSG_FILE_OPEN:
//											{
//												char strFileName[256] = " ";
//												pMsg->GetStr(strFileName, 256);
//												
//												if ( ::strlen(strFileName)>0 )
//												{
//													pTestDlg->GetMapFileManager()->OpenFile(strFileName);
//												}
//											}
//											break;
//
//											case MSG_FILE_CLOSE:
//											{
//												char strFileName[256] = " ";
//												pMsg->GetStr(strFileName, 256);
//
//												if (::strlen(strFileName)>0)
//												{
//													pTestDlg->GetMapFileManager()->CloseFile(strFileName);
//												}
//												
//											}
//											break;
//											
//											default:
//												break;															
//										}
////										CBaseMessage::FreeMsg(pMsg);
//									}
//									break;
//								default:
//									break;
//							}
//						}
//						pTestDlg->FreeMsg(pMsg);
//					}
//					MsgQueue->clear();
//				}
//			}
//
//			LeaveCriticalSection(pTestDlg->GetTestCS());
//			Sleep(100);
//		}
//	}
//	return 0;
//}
// CDriveClientDlg 消息处理程序

void CDriveClientDlg::OnBnClickedButtonStart()
{
	if ( m_DriveClientManager != NULL )
	{

		//创建内存映射文件
		unsigned uId = m_DriveClientManager->_CreateFileMapping("GameClient", "MemServer", "Run//");		
		UpdateData(true);
		//创建DriveGameClient,根据输入的帐号
		_CreatDriveGameClient(m_lStartCDkey, m_lEndCDkey, uId);		
		
		//创建子线程，处理MapFile 文件操作
		//m_stOutputInfo.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		//CGameTestDlg* pDlg = CMapScriptSystem::getInstance()->GetTestDialog();
		//m_stOutputInfo.hThrd = (HANDLE)_beginthreadex(NULL,0,OutputThread,(void*)pDlg,0,NULL);

		//::ShellExecute(NULL, "open", "Luvinia.exe", NULL, NULL, SW_SHOW);
	}
	else
	{
		::MessageBox(NULL, "《〈创建内存映射文件失败,DriveClientManager = NULL〉》", "ERRO!",MB_OK);
	}
}

void CDriveClientDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	
}

//创建DriveGameClient
void  CDriveClientDlg::_CreatDriveGameClient(long startcdkey, long endcdkey, unsigned uID)
{
	if ( startcdkey > endcdkey )
	{	
		::MessageBox(NULL, "输入的起始帐号<<小于>>结束帐号,请重新输入帐号","ERRO!", MB_YESNO);
		return ;
	}
	if ( startcdkey < 1000 )
	{
		::MessageBox(NULL, "输入的起始帐号<<大于>> 1000 ","ERRO!", MB_YESNO);
		return ; 
	}
	if ( startcdkey == endcdkey )
	{
		::MessageBox(NULL, "输入的起始帐号等于结束帐号,为一个帐号","OK!", MB_YESNO);
	}
	CString strPat = "GM";
	CString strCDkey;
	for ( int i = startcdkey; i <= endcdkey; ++i)
	{
		strCDkey.Format("%d", i);
		strCDkey = strPat+strCDkey;

		//开始创建DriveClient
		m_DriveClientManager->_CreateGameClient(strCDkey.GetBuffer(), uID, m_pTimer);
		strCDkey.ReleaseBuffer();
	}
	
}