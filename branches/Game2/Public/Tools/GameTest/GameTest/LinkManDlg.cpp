/*
=========================================================================
备注：暂未使用，界面取消。2010.10.28  BY:LGR
=========================================================================
*/

/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：LinkManDlg.cpp
*  功能：好友测试对话框
*  当前版本：1.1.0
*  作者：
*  完成日期：
*  取代版本：1.0.0
*  原作者：MartySa
*  完成日期： 2009.4.24
*
============================================================
*/

#include "stdafx.h"
#include "GameTest.h"
#include "LinkManDlg.h"
#include "../Script/ScriptSys.h"
#include "GameTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CLinkManDlg, CDialog)

CLinkManDlg::CLinkManDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLinkManDlg::IDD, pParent)
	, m_lRegionID(60101)
	, m_lPosX(301)
	, m_lPosY(254)
	, m_lStartCdKey(1900)
	, m_lEndCdKey(1999)
	, m_lSendMsgTimer(800)
	, m_lSeverSub(5008)
	, m_lMoveRrob(1)
	, m_lDelProb(1)
	, m_lExitProb(25)
	, m_isCanSendAdd(false)				//是否发添加请求
	, m_isCanResponseAdd(false)			//是否回复请求
	, m_isCanMoveLink(false)			//是否移动好友
	, m_isCanDelLink(false)				//是否删除好友
	, m_isCanExit(false)				//是否上下线
	, m_isCanPtLogin(false)				//是否盛大登陆方式
{

}

CLinkManDlg::~CLinkManDlg()
{
}

void CLinkManDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LINKMAN_REGION, m_lRegionID);
	DDX_Text(pDX, IDC_EDIT_LINKMAN_POSX, m_lPosX);
	DDX_Text(pDX, IDC_EDIT_LINKMAN_POSY, m_lPosY);
	DDX_Text(pDX, IDC_EDIT_LINKMAN_StartCdKey, m_lStartCdKey);
	DDX_Text(pDX, IDC_EDIT_LINKMAN_EndCdkey, m_lEndCdKey);
	DDX_Text(pDX, IDC_EDIT_LINKMAN_MsgTimer, m_lSendMsgTimer);
	DDX_Text(pDX, IDC_EDIT_LINKMAN_SeverSub, m_lSeverSub);
	DDX_Text(pDX, IDC_EDIT_LINKMAN_MoveProbability, m_lMoveRrob);
	DDX_Text(pDX, IDC_EDIT_LINKMAN_DelProbability, m_lDelProb);
	DDX_Text(pDX, IDC_EDIT_LINKMAN_ExitProbability, m_lExitProb);
	DDX_Control(pDX, IDC_COMBO_LINKMAN_SeverIP, m_comboServerIP);
}


BEGIN_MESSAGE_MAP(CLinkManDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_LINKMAN_SendAdd, &CLinkManDlg::OnBnClickedCheckLinkmanSendadd)
	ON_BN_CLICKED(IDC_CHECK_LINKMAN_ResponseAdd, &CLinkManDlg::OnBnClickedCheckLinkmanResponseadd)
	ON_BN_CLICKED(IDC_CHECK_LINKMAN_SendMove, &CLinkManDlg::OnBnClickedCheckLinkmanSendmove)
	ON_BN_CLICKED(IDC_CHECK_LINKMAN_Del, &CLinkManDlg::OnBnClickedCheckLinkmanDel)
	ON_BN_CLICKED(IDC_CHECK_LINKMAN_ExitClient, &CLinkManDlg::OnBnClickedCheckLinkmanExitclient)
	ON_BN_CLICKED(IDC_BUTTON_LINKMAN_START, &CLinkManDlg::OnBnClickedButtonLinkmanStart)
	ON_BN_CLICKED(IDC_CHECK_LINKMAN_PT_LOGIN, &CLinkManDlg::OnBnClickedCheckLinkmanPtLogin)
END_MESSAGE_MAP()

BOOL CLinkManDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	return TRUE;
}

// CLinkManDlg 消息处理程序
void CLinkManDlg::OnBnClickedCheckLinkmanSendadd()
{
	m_isCanSendAdd = ((CButton*)GetDlgItem(IDC_CHECK_LINKMAN_SendAdd))->GetCheck() ? true : false ;
}

void CLinkManDlg::OnBnClickedCheckLinkmanResponseadd()
{
	m_isCanResponseAdd = ((CButton*)GetDlgItem(IDC_CHECK_LINKMAN_ResponseAdd))->GetCheck() ? true : false ;
}

void CLinkManDlg::OnBnClickedCheckLinkmanSendmove()
{
	m_isCanMoveLink = ((CButton*)GetDlgItem(IDC_CHECK_LINKMAN_SendMove))->GetCheck() ? true : false ;
}

void CLinkManDlg::OnBnClickedCheckLinkmanDel()
{
	m_isCanDelLink = ((CButton*)GetDlgItem(IDC_CHECK_LINKMAN_Del))->GetCheck() ? true : false ;
}

void CLinkManDlg::OnBnClickedCheckLinkmanExitclient()
{
	m_isCanExit = ((CButton*)GetDlgItem(IDC_CHECK_LINKMAN_ExitClient))->GetCheck() ? true : false ;
}


//盛大登陆方式Check
void CLinkManDlg::OnBnClickedCheckLinkmanPtLogin()
{
	m_isCanPtLogin = ((CButton*)GetDlgItem(IDC_CHECK_LINKMAN_PT_LOGIN))->GetCheck() ? true : false ;
	if ( m_isCanPtLogin )
	{
		((CButton*)GetDlgItem(IDC_STATIC_LINKMAN_strEndCDkey))->ShowWindow(false);	
		((CButton*)GetDlgItem(IDC_EDIT_LINKMAN_EndCdkey))->ShowWindow(false);
		((CButton*)GetDlgItem(IDC_STATIC_LINKMAN_strStartCDkey))->SetWindowText("机器人数量:");
	}
	else
	{
		((CButton*)GetDlgItem(IDC_STATIC_LINKMAN_strEndCDkey))->ShowWindow(true);	
		((CButton*)GetDlgItem(IDC_EDIT_LINKMAN_EndCdkey))->ShowWindow(true);
		((CButton*)GetDlgItem(IDC_STATIC_LINKMAN_strStartCDkey))->SetWindowText("起始账号: GM");
	}
}

void CLinkManDlg::OnBnClickedButtonLinkmanStart()
{
	UpdateData(TRUE);
	char strScriptName[256] = " ";

	if ( !m_isCanPtLogin )		//SiMo登陆方式
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
		

		sprintf(strScriptName,"LinkManLoginCDKey(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
													m_lStartCdKey,				//起始帐号
													m_lEndCdKey,				//结束帐号
													10,							//测试类型
													m_lSendMsgTimer,			//消息间隔
													m_lRegionID,				//起始场景Id									
													m_lPosX,					//坐标X									
													m_lPosY,					//坐标Y									
													m_lSeverSub,				//服务器版本	
													m_lMoveRrob,				//移动几率
													m_lDelProb,					//删除几率
													m_lExitProb,				//退出几率
													m_isCanSendAdd,				//是否发添加请求
													m_isCanResponseAdd,			//是否回复请求
													m_isCanMoveLink,			//是否移动好友
													m_isCanDelLink,				//是否删除好友
													m_isCanExit				//是否上下线
													);
	}
	else
	{
		if ( m_lStartCdKey < 0 )
		{	
			::MessageBox(NULL, "输入的起始帐号数量小于0,请重新输入", "ERRO!", MB_OK);
			return ;
		}
		//和上面少EndCdkey
		sprintf(strScriptName,"LinkManPtLoginCDKey(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
			m_lStartCdKey,				//起始帐号
			//m_lEndCdKey,				//结束帐号
			10,							//测试类型
			m_lSendMsgTimer,			//消息间隔
			m_lRegionID,				//起始场景Id									
			m_lPosX,					//坐标X									
			m_lPosY,					//坐标Y									
			m_lSeverSub,				//服务器版本	
			m_lMoveRrob,				//移动几率
			m_lDelProb,					//删除几率
			m_lExitProb,				//退出几率
			m_isCanSendAdd,				//是否发添加请求
			m_isCanResponseAdd,			//是否回复请求
			m_isCanMoveLink,			//是否移动好友
			m_isCanDelLink,				//是否删除好友
			m_isCanExit				//是否上下线
			);
	}
	CScriptSys::getInstance()->PerformString(strScriptName);
}
