/**
=========================================================================
备注：暂未使用，界面取消。2010.10.28  BY:LGR
=========================================================================
*/

/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：MailDlg.cpp
*  功能：邮件测试对话框
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
#include "MailDlg.h"
#include "../Script/ScriptSys.h"
#include "GameTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMailDlg, CDialog)

CMailDlg::CMailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMailDlg::IDD, pParent)
	, m_lStartRegionID(60101)
	, m_lPosX(301)
	, m_lPosY(254)
	, m_lStartCdKey(1900)
	, m_lEndCdKey(1999)
	, m_lMsgSendTimer(800)
	, m_cstrGoodsName(_T("随机设定"))
	, m_lSendMoney(0)
	, m_lMailType(-1)
	, m_lGoodsNum(-1)
	, m_cstrSendName(_T("随机设定"))
	, m_lSendRepeatNum(0)
	, m_lRejectMailProb(0)
	, m_lExitClientPro(25)
	, m_lDeleteMailProb(0)
	, m_lSeverSub(5008)
	, m_lSendMsgRepeatNum(0)
	, m_isCanSendMail(false)			//是否能发信
	, m_isCanRepeatSendName(false)		//是否重复发送给一个玩家
	, m_isCanRecMail(false)				//是否能收信
	, m_isCanRejectMail(false)			//是否退信
	, m_isCanDeleteMail(false)			//是否删信
	, m_isCanGetGoods(false)			//是否取物品
	, m_isCanAgreePay(false)			//是否同意付款
	, m_isCanGetMoney(false)			//是否取金钱
	, m_isCanExitClient(false)			//是否上下线
	, m_isCanSendMsgRepeat(false)		//是否重复发送一次消息
	, m_isCanPtLogin(false)				//是否盛大登陆方式
{

}

CMailDlg::~CMailDlg()
{
}

void CMailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MAIL_REGION, m_lStartRegionID);
	DDX_Text(pDX, IDC_EDIT_MAIL_POSX, m_lPosX);
	DDX_Text(pDX, IDC_EDIT_MAIL_POSY, m_lPosY);
	DDX_Text(pDX, IDC_EDIT_MAIL_StartCdKey, m_lStartCdKey);
	DDX_Text(pDX, IDC_EDIT_MAIL_EndCdkey, m_lEndCdKey);
	DDX_Text(pDX, IDC_EDIT_MAIL_MsgTimer, m_lMsgSendTimer);	
	DDX_Text(pDX, IDC_EDIT_MAIL_SendDoodsName, m_cstrGoodsName);
	DDX_Text(pDX, IDC_EDIT_MAIL_SendMoney, m_lSendMoney);
	DDX_Text(pDX, IDC_EDIT_MAIL_SendType, m_lMailType);
	DDX_Text(pDX, IDC_EDIT_MAIL_SendGoodsNum, m_lGoodsNum);
	DDX_Text(pDX, IDC_EDIT_MAIL_SendName, m_cstrSendName);
	DDX_Text(pDX, IDC_EDIT_MAIL_SendRepeatNum, m_lSendRepeatNum);
	DDX_Text(pDX, IDC_EDIT_MAIL_RejectProbability, m_lRejectMailProb);
	DDX_Text(pDX, IDC_EDIT_MAIL_ExitProbability, m_lExitClientPro);
	DDX_Text(pDX, IDC_EDIT_MAIL_DeleteProbability, m_lDeleteMailProb);
	DDX_Text(pDX, IDC_EDIT_MAIL_SeverSub, m_lSeverSub);
	DDX_Text(pDX, IDC_EDIT_MAIL_SendMsgRepeatNum, m_lSendMsgRepeatNum);

	DDX_Control(pDX, IDC_COMBO_MAILSever, m_comboxServer);
	DDX_Control(pDX, IDC_CHECK_MAIL_SEND, m_CheckMailSend);		
	DDX_Control(pDX, IDC_CHECK_MAIL_RepeatSendName, m_CheckMailRepratName);
	DDX_Control(pDX, IDC_CHECK_MAILREC, m_CheckMailRec);
	DDX_Control(pDX, IDC_CHECK_MAIL_Reject, m_CheckMailReject);
	DDX_Control(pDX, IDC_CHECK_MAIL_Delete, m_CheckMailDelete);
	DDX_Control(pDX, IDC_CHECK_MAIL_GetGoods, m_CheckMailGetDoods);
	DDX_Control(pDX, IDC_CHECK_MAIL_AgreedPay, m_CheckMailAgreePay);
	DDX_Control(pDX, IDC_CHECK_MAIL_GetMoney, m_CheckMailGetMoney);
	DDX_Control(pDX, IDC_CHECK_MAIL_ExitClient, m_CheckMailExitClient);
	DDX_Control(pDX, IDC_CHECK_MAIL_RepeatSendMsg, m_CheckMailSendMsgRepeat);
	DDX_Control(pDX, IDC_CHECK_MAIL_PT_LOGIN, m_CheckMailPtLogin);

}


BEGIN_MESSAGE_MAP(CMailDlg, CDialog)

ON_BN_CLICKED(IDC_CHECK_MAIL_SEND, &CMailDlg::OnBnClickedCheckMailSend)
ON_BN_CLICKED(IDC_CHECK_MAIL_RepeatSendName, &CMailDlg::OnBnClickedCheckMailRepeatsendname)
ON_BN_CLICKED(IDC_CHECK_MAILREC, &CMailDlg::OnBnClickedCheckMailrec)
ON_BN_CLICKED(IDC_CHECK_MAIL_Reject, &CMailDlg::OnBnClickedCheckMailReject)
ON_BN_CLICKED(IDC_CHECK_MAIL_Delete, &CMailDlg::OnBnClickedCheckMailDelete)
ON_BN_CLICKED(IDC_CHECK_MAIL_GetGoods, &CMailDlg::OnBnClickedCheckMailGetgoods)
ON_BN_CLICKED(IDC_CHECK_MAIL_AgreedPay, &CMailDlg::OnBnClickedCheckMailAgreedpay)
ON_BN_CLICKED(IDC_CHECK_MAIL_GetMoney, &CMailDlg::OnBnClickedCheckMailGetmoney)
ON_BN_CLICKED(IDC_CHECK_MAIL_ExitClient, &CMailDlg::OnBnClickedCheckMailExitclient)
ON_BN_CLICKED(IDC_BUTTON_MailStart, &CMailDlg::OnBnClickedButtonMailstart)
ON_BN_CLICKED(IDC_CHECK_MAIL_RepeatSendMsg, &CMailDlg::OnBnClickedCheckMailRepeatsendmsg)
ON_BN_CLICKED(IDC_CHECK_MAIL_PT_LOGIN, &CMailDlg::OnBnClickedCheckMailLogin)
END_MESSAGE_MAP()


BOOL CMailDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//m_CheckMailSend.SetCheck(1);				//默认能发信
	//m_CheckMailRec.SetCheck(1);				//默认能收信
	//m_CheckMailRepratName.SetCheck(0);		//默认不能重复发送一个人名
	//m_CheckMailReject.SetCheck(0);			//默认不能退信
	//m_CheckMailDelete.SetCheck(1);
	//m_CheckMailGetDoods.SetCheck(1);
	//m_CheckMailAgreePay.SetCheck(1);
	//m_CheckMailGetMoney.SetCheck(1);
	//m_CheckMailExitClient.SetCheck(0);
	m_comboxServer.InsertString(-1,"21服务器");
	m_comboxServer.InsertString(-1,"桥哥40");
	m_comboxServer.InsertString(-1,"桥哥50");
	m_comboxServer.InsertString(-1,"先进");
	m_comboxServer.InsertString(-1,"张敏");
	m_comboxServer.InsertString(-1,"科智");
	m_comboxServer.InsertString(-1,"桥哥2");
	m_comboxServer.InsertString(-1,"26服务器");
	m_comboxServer.InsertString(-1,"相运峰");
	m_comboxServer.InsertString(-1,"肖尧");
	m_comboxServer.InsertString(-1,"海川");
	m_comboxServer.SetCurSel(0);
	return true;
}


void CMailDlg::OnBnClickedButtonMailstart()
{	
	UpdateData(TRUE);
	
	char strScriptName[256] = " " ;

	//SiMo登陆方式
	if ( !m_isCanPtLogin )	
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
		
		sprintf(strScriptName,"MailLoginCDKey(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s')",
												m_lStartCdKey,				//起始帐号
												m_lEndCdKey,				//结束帐号
												3,							//测试类型
												m_lMsgSendTimer,			//消息间隔
												m_lStartRegionID,			// 起始场景Id									
												m_lPosX,					// 坐标X									
												m_lPosY,					// 坐标Y									
												m_lSendMoney,				// 金钱											
												m_lMailType,				// 邮件类型， -1为随机０－５发送											
												m_lGoodsNum,				// 物品数量																															
												m_lSendRepeatNum,			// 重复发送次数											
												m_lRejectMailProb,			// 退信几率											
												m_lExitClientPro,			// 上线线几率										
												m_lDeleteMailProb,			// 删信几率					
												m_lSeverSub,			    // 服务器版本
												m_lSendMsgRepeatNum,		//　重复发一次信次数
												m_isCanSendMail,			//是否能发信
												m_isCanRepeatSendName,		//是否重复发送给一个玩家
												m_isCanRecMail,				//是否能收信
												m_isCanRejectMail,			//是否退信
												m_isCanDeleteMail,			//是否删信
												m_isCanGetGoods,			//是否取物品
												m_isCanAgreePay,			//是否同意付款
												m_isCanGetMoney,			//是否取金钱
												m_isCanExitClient,			//是否上下线
												m_isCanSendMsgRepeat,       //是否重复发信
												m_cstrSendName,				// 发送的接收者人名
												m_cstrGoodsName		// 物品名
												);
	}
	else
	{
		if ( m_lStartCdKey < 0 )
		{	
			::MessageBox(NULL, "输入的起始帐号数量小于0,请重新输入", "ERRO!", MB_OK);
			return ;
		}

		//比SiMo方式少一个EndCDkey
		sprintf(strScriptName,"MailPtLoginCDKey(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s')",
			m_lStartCdKey,				//起始帐号
			//m_lEndCdKey,				//结束帐号
			3,							//测试类型
			m_lMsgSendTimer,			//消息间隔
			m_lStartRegionID,			// 起始场景Id									
			m_lPosX,					// 坐标X									
			m_lPosY,					// 坐标Y									
			m_lSendMoney,				// 金钱											
			m_lMailType,				// 邮件类型， -1为随机０－５发送											
			m_lGoodsNum,				// 物品数量																															
			m_lSendRepeatNum,			// 重复发送次数											
			m_lRejectMailProb,			// 退信几率											
			m_lExitClientPro,			// 上线线几率										
			m_lDeleteMailProb,			// 删信几率					
			m_lSeverSub,			    // 服务器版本
			m_lSendMsgRepeatNum,		//　重复发一次信次数
			m_isCanSendMail,			//是否能发信
			m_isCanRepeatSendName,		//是否重复发送给一个玩家
			m_isCanRecMail,				//是否能收信
			m_isCanRejectMail,			//是否退信
			m_isCanDeleteMail,			//是否删信
			m_isCanGetGoods,			//是否取物品
			m_isCanAgreePay,			//是否同意付款
			m_isCanGetMoney,			//是否取金钱
			m_isCanExitClient,			//是否上下线
			m_isCanSendMsgRepeat,       //是否重复发信
			m_cstrSendName,				// 发送的接收者人名
			m_cstrGoodsName				// 物品名
			);
	}
	
	CScriptSys::getInstance()->PerformString(strScriptName);
}

//发信
void CMailDlg::OnBnClickedCheckMailSend()
{
	m_isCanSendMail = m_CheckMailSend.GetCheck() ? true : false ;
}

//收信	
void CMailDlg::OnBnClickedCheckMailrec()
{
	m_isCanRecMail = m_CheckMailRec.GetCheck() ? true : false ;
}

//重复发送名字
void CMailDlg::OnBnClickedCheckMailRepeatsendname()
{
	m_isCanRepeatSendName = m_CheckMailRepratName.GetCheck() ? true : false ;
}

//退信
void CMailDlg::OnBnClickedCheckMailReject()
{
	m_isCanRejectMail = m_CheckMailReject.GetCheck() ? true : false ;
}

//删信
void CMailDlg::OnBnClickedCheckMailDelete()
{
	m_isCanDeleteMail = m_CheckMailDelete.GetCheck() ? true : false ;
}

//取物品
void CMailDlg::OnBnClickedCheckMailGetgoods()
{
	m_isCanGetGoods = m_CheckMailGetDoods.GetCheck() ? true : false ;
}

//同意付款
void CMailDlg::OnBnClickedCheckMailAgreedpay()
{
	m_isCanAgreePay = m_CheckMailAgreePay.GetCheck() ? true : false ;
}

//取钱
void CMailDlg::OnBnClickedCheckMailGetmoney()
{
	m_isCanGetMoney = m_CheckMailGetMoney.GetCheck() ? true : false ;
}

//上下线
void CMailDlg::OnBnClickedCheckMailExitclient()
{
	m_isCanExitClient = m_CheckMailExitClient.GetCheck() ? true : false ;
}

//重复发送一次消息
void CMailDlg::OnBnClickedCheckMailRepeatsendmsg()
{
	m_isCanSendMsgRepeat = m_CheckMailSendMsgRepeat.GetCheck() ? true : false ;
}

//登陆方式
void CMailDlg::OnBnClickedCheckMailLogin()
{
	m_isCanPtLogin =  m_CheckMailPtLogin.GetCheck() ? true : false ;
	if ( m_isCanPtLogin )
	{
		((CButton*)GetDlgItem(IDC_STATIC_strEndCDkey))->ShowWindow(false);	
		((CButton*)GetDlgItem(IDC_EDIT_MAIL_EndCdkey))->ShowWindow(false);
		((CButton*)GetDlgItem(IDC_STATIC_strStartCDkey))->SetWindowText("机器人数量:");
	}
	else
	{
		((CButton*)GetDlgItem(IDC_STATIC_strEndCDkey))->ShowWindow(true);	
		((CButton*)GetDlgItem(IDC_EDIT_MAIL_EndCdkey))->ShowWindow(true);
		((CButton*)GetDlgItem(IDC_STATIC_strStartCDkey))->SetWindowText("起始账号: GM");
	}
}
