/*
=========================================================================
备注：该功能已取消，界面取消。2010.10.28  BY:LGR
=========================================================================
*/

/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：ACAnswerDlg.h
*  功能：活力点测试对话框界面
*  当前版本：1.1.0
*  作者：
*  完成日期：
*  取代版本：1.0.0
*  原作者：
*  完成日期：
*
============================================================
*/

#include "stdafx.h"
#include "GameTest.h"
#include "ACAnswerDlg.h"
#include "../Script/ScriptSys.h"
#include "GameTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CACAnswer, CDialog)

CACAnswer::CACAnswer(CWnd* pParent)
	: CDialog(CACAnswer::IDD, pParent)
	, m_lRegionID(20100)
	, m_lPosX(0)
	, m_lPosY(0)
	, m_lStartCdKey(1900)
	, m_lEndCdKey(1999)
	, m_lSendMsgTimer(800)
	, m_lSeverSub(5008)
	, m_bIsCanAnswer(TRUE)
	, m_bIsCanSafeTest(FALSE)
	, m_bIsCopyMsg(FALSE)
	, m_bIsCopySnedMsg(FALSE)
	, m_bIsExitClient(FALSE)
	, m_bIsCanPtLogin(FALSE)
{

}

CACAnswer::~CACAnswer()
{
}

void CACAnswer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ACC_REGION, m_lRegionID);
	DDX_Text(pDX, IDC_EDIT_ACC_POSX, m_lPosX);
	DDX_Text(pDX, IDC_EDIT_ACC_POSY, m_lPosY);
	DDX_Text(pDX, IDC_EDIT_ACC_StartCdKey, m_lStartCdKey);
	DDX_Text(pDX, IDC_EDIT_ACC_EndCdkey, m_lEndCdKey);
	DDX_Text(pDX, IDC_EDIT_ACC_MsgTimer, m_lSendMsgTimer);
	DDX_Text(pDX, IDC_EDIT_ACC_SeverSub, m_lSeverSub);
	DDX_Check(pDX, IDC_ACC_CHECK_Answer, m_bIsCanAnswer);
	DDX_Check(pDX, IDC_ACC_CHECK_SAFE, m_bIsCanSafeTest);
	DDX_Check(pDX, IDC_ACC_CHECK_CopyMsg, m_bIsCopyMsg);
	DDX_Check(pDX, IDC_ACC_CHECK_CopySend, m_bIsCopySnedMsg);
	DDX_Check(pDX, IDC_ACC_CHECK_ExitClient, m_bIsExitClient);
}


BEGIN_MESSAGE_MAP(CACAnswer, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ACC_START, &CACAnswer::OnBnClickedButtonAccStart)
	ON_BN_CLICKED(IDC_ACC_CHECK_Answer, &CACAnswer::OnBnClickedAccCheckAnswer)
	ON_BN_CLICKED(IDC_ACC_CHECK_SAFE, &CACAnswer::OnBnClickedAccCheckSafe)
	ON_BN_CLICKED(IDC_ACC_CHECK_CopyMsg, &CACAnswer::OnBnClickedAccCheckCopymsg)
	ON_BN_CLICKED(IDC_ACC_CHECK_CopySend, &CACAnswer::OnBnClickedAccCheckCopysend)
	ON_BN_CLICKED(IDC_ACC_CHECK_ExitClient, &CACAnswer::OnBnClickedAccCheckExitclient)
	ON_BN_CLICKED(IDC_CHECK_ACA_PT_LOGIN, &CACAnswer::OnBnClickedCheckAcaPtLogin)
END_MESSAGE_MAP()


void CACAnswer::OnBnClickedButtonAccStart()
{
	UpdateData(TRUE);
	char strScriptName[256] = " ";
	
	if ( !m_bIsCanPtLogin )			// SiMo方式登陆
	{
		if ( m_lStartCdKey > m_lEndCdKey )
		{	
			::MessageBox(NULL, "输入的起始帐号<<小于>>结束帐号,请重新输入帐号","ERRO!", MB_OK);
			return ;
		}
		if ( m_lStartCdKey < 1000 )
		{
			::MessageBox(NULL, "输入的起始帐号<<大于>> 1000 ","ERRO!", MB_OK);
			return ; 
		}
		if ( m_lStartCdKey == m_lEndCdKey )
		{
			::MessageBox(NULL, "输入的起始帐号等于结束帐号,为一个帐号","OK!", MB_OK);
		}
		

		sprintf(strScriptName,"ACCAnswerLoginCDKey(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
			m_lStartCdKey,				//起始帐号
			m_lEndCdKey,				//结束帐号
			12,							//测试类型
			m_lSendMsgTimer,			//消息间隔
			m_lRegionID,				//起始场景Id									
			m_lPosX,					//坐标X									
			m_lPosY,					//坐标Y									
			m_lSeverSub,				//服务器版本	
			m_bIsCanAnswer,
										// 是否激活安全测试
			m_bIsCanSafeTest,
										// 是否复制整块消息
			m_bIsCopyMsg,
										// 是否在消息Send那复制
			m_bIsCopySnedMsg,
										// 是否上下线
			m_bIsExitClient	
			);
	}
	else
	{
		if ( m_lStartCdKey < 0 )
		{	
			::MessageBox(NULL, "输入的起始帐号数量小于0,请重新输入", "ERRO!", MB_OK);
			return ;
		}

		sprintf(strScriptName,"ACCAnswerPtLoginCDKey(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
			m_lStartCdKey,				//起始帐号
			//m_lEndCdKey,				//结束帐号
			12,							//测试类型
			m_lSendMsgTimer,			//消息间隔
			m_lRegionID,				//起始场景Id									
			m_lPosX,					//坐标X									
			m_lPosY,					//坐标Y									
			m_lSeverSub,				//服务器版本	
			m_bIsCanAnswer,
			// 是否激活安全测试
			m_bIsCanSafeTest,
			// 是否复制整块消息
			m_bIsCopyMsg,
			// 是否在消息Send那复制
			m_bIsCopySnedMsg,
			// 是否上下线
			m_bIsExitClient	
			);
	}

	CScriptSys::getInstance()->PerformString(strScriptName);

}

void CACAnswer::OnBnClickedAccCheckAnswer()
{
	m_bIsCanAnswer = ((CButton*)GetDlgItem(IDC_ACC_CHECK_Answer))->GetCheck() ? true : false ;
}

void CACAnswer::OnBnClickedAccCheckSafe()
{
	m_bIsCanSafeTest = ((CButton*)GetDlgItem(IDC_ACC_CHECK_SAFE))->GetCheck() ? true : false ;
}

void CACAnswer::OnBnClickedAccCheckCopymsg()
{
	m_bIsCopyMsg = ((CButton*)GetDlgItem(IDC_ACC_CHECK_CopyMsg))->GetCheck() ? true : false ;
}

void CACAnswer::OnBnClickedAccCheckCopysend()
{
	m_bIsCopySnedMsg = ((CButton*)GetDlgItem(IDC_ACC_CHECK_CopySend))->GetCheck() ? true : false ;
}

void CACAnswer::OnBnClickedAccCheckExitclient()
{
	m_bIsExitClient = ((CButton*)GetDlgItem(IDC_ACC_CHECK_ExitClient))->GetCheck() ? true : false ;
}

void CACAnswer::OnBnClickedCheckAcaPtLogin()
{
	m_bIsCanPtLogin = ((CButton*)GetDlgItem(IDC_CHECK_ACA_PT_LOGIN))->GetCheck() ? true : false ;
	if ( m_bIsCanPtLogin )
	{
		((CButton*)GetDlgItem(IDC_STATIC_ACC_strEndCDkey))->ShowWindow(false);	
		((CButton*)GetDlgItem(IDC_EDIT_ACC_EndCdkey))->ShowWindow(false);
		((CButton*)GetDlgItem(IDC_STATIC_ACC_strStartCDkey))->SetWindowText("机器人数量:");
	}
	else
	{
		((CButton*)GetDlgItem(IDC_STATIC_ACC_strEndCDkey))->ShowWindow(true);	
		((CButton*)GetDlgItem(IDC_EDIT_ACC_EndCdkey))->ShowWindow(true);
		((CButton*)GetDlgItem(IDC_STATIC_ACC_strStartCDkey))->SetWindowText("起始账号: GM");
	}
}
