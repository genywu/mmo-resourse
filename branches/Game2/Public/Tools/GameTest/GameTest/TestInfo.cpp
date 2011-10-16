//=============================================================================
/**
 *  file: TestInfo.h
 *
 *  Brief:管理游戏中的数据显示信息
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================


#include "stdafx.h"
#include "GameTest.h"
#include "TestInfo.h"
#include "WndMsgDef.h"
#include "..\App\GameClient.h"
#include "..\App\ClientRegion.h"
#include ".\testinfo.h"
#include "GameTestDlg.h"

#include "RegionInfo.h"
// #include "../../nets/Clients.h"
// #include "../../nets/Servers.h"
// #include "../../nets/netGT/Message.h"

#include "../Net/Message.h"
#include "../Script/ScriptSys.h"
//#include "../GameTest/LuaFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTestInfo, CDialog)
CTestInfo::CTestInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CTestInfo::IDD, pParent)
	,m_pSelGameClient(NULL)
	, m_StrInputBox(_T(""))
	, m_strScriptName(_T(""))
	, m_lMaxContNum(0)
	, m_lMinContNum(1000)
{
}

CTestInfo::~CTestInfo()
{
}

void CTestInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INPUT_INFO, m_edInputBox);
	DDX_Control(pDX, IDC_EDIT_OUTPUT_INFO, m_edOutputBox);
	DDX_Text(pDX, IDC_EDIT_INPUT_INFO, m_StrInputBox);
	DDX_Text(pDX, IDC_ScriptName,	m_strScriptName);
	DDX_Control(pDX,IDC_CheckOne,	m_CheckOne);
	DDX_Control(pDX,IDC_STATIC_ShowAmount,m_ShowAmountCtrl);
}


BEGIN_MESSAGE_MAP(CTestInfo, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_OUTPUT_INFO, OnEnChangeEditOutputInfo)
	ON_BN_CLICKED(IDC_RunScript,	OnRunScript )
//	ON_WM_RBUTTONDOWN()
//ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CTestInfo::OnInitDialog()
 {
	CDialog::OnInitDialog();

	m_RegionInfo.Create(IDD_PROPPAGE_REGION_INFO,this);
	m_PlayerInfo.Create(IDD_PROPPAGE_PLAYER_INFO,this);
	m_SkillInfo.Create(IDD_PROPPAGE_SKILL_INFO,this);
	m_MonsterInfo.Create(IDD_PROPPAGE_MONSTER_INFO,this);
	m_RegionGoods.Create(IDD_PROPPAGE_Region_Goods_Info,this);
	

	CRect rs;
	GetClientRect(rs); 
	
	CRect treerect = rs;
	treerect.left += 1;
	treerect.right = 150;
	treerect.top += 1;
	treerect.bottom -= 50;
	//创建树控件
	m_ctrGameClients.Create(WS_BORDER|WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_HASBUTTONS,treerect,this,IDT_GAME_CLIENT);
	m_ctrGameClients.InsertItem("测试客户端(0)个");
	//对话框
	rs.left = treerect.right + 1;
	rs.top = treerect.top;
	rs.bottom = 380;
	//
	m_RegionInfo.MoveWindow(rs);
	m_PlayerInfo.MoveWindow(rs);
	m_SkillInfo.MoveWindow(rs);
	m_MonsterInfo.MoveWindow(rs);
	m_RegionGoods.MoveWindow(rs);

	m_PlayerInfo.ShowWindow(FALSE);
	m_RegionInfo.ShowWindow(FALSE);
	m_SkillInfo.ShowWindow(FALSE);
	m_MonsterInfo.ShowWindow(FALSE);
	m_RegionGoods.ShowWindow(FALSE);

	m_edOutputBox.SetLimitText(100);

	//m_lTimerID = SetTimer(1,1000,NULL);

	return TRUE; 
}

void CTestInfo::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == 1)
	{
 		CServer* pServer = CScriptSys::getInstance()->GetTestDialog()->GetNetServer();
 		if( pServer )
 		{
 			CString strAmount("");
 			strAmount.Format("%d",pServer->GetClientCount());
 			m_ShowAmountCtrl.SetWindowText(strAmount);

			long cnum = CScriptSys::getInstance()->GetTestDialog()->GetClient().size();
			strAmount.Format("%d",cnum);
			SetDlgItemText(IDC_STATIC_ShowClientNum,strAmount);

			if (cnum > m_lMaxContNum)
				m_lMaxContNum = cnum;
			else 
				if (cnum < m_lMinContNum)
					m_lMinContNum = cnum;
 		}
	}
}
void CTestInfo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}


/*
=========================================================================
函数功能:控件颜色重载  By: LGR
=========================================================================
*/
HBRUSH CTestInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case
IDC_STATIC_ShowAmount:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(LIGHTRED);
			COLORREF color = GetDC()->GetBkColor();
			HBRUSH b = CreateSolidBrush(color);
			return (HBRUSH)b;
		}
		case
IDC_EDIT_OUTPUT_INFO:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(LIGHTBLUE);
			HBRUSH b = CreateSolidBrush(WHITE);
			return (HBRUSH)b;
		}
	default:
		return CWnd::OnCtlColor(pDC,pWnd,nCtlColor);	
	}		
}


void CTestInfo::ResetSelGameClient(CGameClient* pGameClient)
{
	if(pGameClient == NULL)
	{
		m_RegionInfo.SetDisplayRegion(NULL);
		m_PlayerInfo.SetDisplayPlayer(NULL);
		m_MonsterInfo.SetDisplayMonster(NULL);
		m_SkillInfo.SetDisplaySkill(NULL);
		m_RegionGoods.SetDisplayGoods(NULL);
		return;
	}

	m_pSelGameClient = pGameClient;
	switch(m_ctrGameClients.GetSelectNode()->lType)
	{
	case TYPE_PLAYER:
		{
			m_PlayerInfo.SetDisplayPlayer(m_pSelGameClient);
			m_PlayerInfo.ShowWindow(TRUE);
			m_RegionInfo.ShowWindow(FALSE);
			m_SkillInfo.ShowWindow(FALSE);
			m_MonsterInfo.ShowWindow(FALSE);
			m_RegionGoods.ShowWindow(FALSE);
		}
		break;
	case TYPE_REGION:
		{
			CPlayer* pPlayer = (CPlayer*)m_pSelGameClient->GetMainPlayer();
			m_RegionInfo.SetDisplayRegion(m_pSelGameClient);		
			m_RegionInfo.ShowWindow(TRUE);
			m_PlayerInfo.ShowWindow(FALSE);
			m_SkillInfo.ShowWindow(FALSE);
			m_MonsterInfo.ShowWindow(FALSE);
			m_RegionGoods.ShowWindow(FALSE);
		}
		break;
	case TYPE_SKILL:
		{
			CPlayer* pPlayer = (CPlayer*)m_pSelGameClient->GetMainPlayer();
			m_SkillInfo.SetDisplaySkill(pPlayer);
			m_SkillInfo.ShowWindow(TRUE);
			m_PlayerInfo.ShowWindow(FALSE);
			m_RegionInfo.ShowWindow(FALSE);
			m_MonsterInfo.ShowWindow(FALSE);
			m_RegionGoods.ShowWindow(FALSE);
		}
		break;
	case TYPE_MONSTER:
		{
			CClientRegion* pRegion = (CClientRegion*)m_pSelGameClient->GetClientRegion();
			m_MonsterInfo.SetDisplayMonster(pRegion);
			m_MonsterInfo.ShowWindow(TRUE);
			m_PlayerInfo.ShowWindow(FALSE);
			m_RegionInfo.ShowWindow(FALSE);
			m_SkillInfo.ShowWindow(FALSE);
			m_RegionGoods.ShowWindow(FALSE);
		}
		break;
	case TYPE_GOODS:
		{
			CClientRegion* pRegion = (CClientRegion*)m_pSelGameClient->GetClientRegion();
			m_RegionGoods.SetDisplayGoods(pRegion);
			m_RegionGoods.ShowWindow(TRUE);
			m_PlayerInfo.ShowWindow(FALSE);
			m_RegionInfo.ShowWindow(FALSE);
			m_SkillInfo.ShowWindow(FALSE);
			m_MonsterInfo.ShowWindow(FALSE);
		}
		break;
	default:
		break;
	}
}


void CTestInfo::OutputGameInfo(const char* strInfo)
{
	CString strText("");
	m_edOutputBox.GetWindowText(strText);
	if (strText.GetLength() >= 10240)
	{
		strText = "";
	}
	strText += "\n\0";
	strText += strInfo;
	strText += "\r\n";
	m_edOutputBox.SetWindowText(strText);
	m_edOutputBox.LineScroll(m_edOutputBox.GetLineCount(),0); 
}
void CTestInfo::OutputThreadInfo(const char* info)
{
	CString strText("");
	m_edInputBox.GetWindowText(strText);
	if (strText.GetLength() >= 10240000)
	{
		strText = "";
	}
	strText += "\n\0";
	strText += info;
	strText += "\r\n";
	m_edInputBox.SetWindowText(strText);
	m_edInputBox.LineScroll(m_edInputBox.GetLineCount(),0); 
}



void CTestInfo::OnTreeSelect()
{
	tagNodeInfo* pSelNode = m_ctrGameClients.GetSelectNode();
	if(pSelNode == NULL )
		return;

	if( (m_pSelGameClient == NULL && pSelNode != NULL && pSelNode->lClientID > 0) ||
		(m_pSelGameClient != NULL && pSelNode != NULL && pSelNode->lClientID != m_pSelGameClient->GetSocketID()) )
 	{
		::PostMessage(	this->GetParent()->GetParent()->m_hWnd,
						DISPLAY_INFO_RESET_GAME_CLIENT,
						m_ctrGameClients.GetSelectNode()->lClientID,NULL);
		return;
	}
	ResetSelGameClient(m_pSelGameClient);
}


BOOL CTestInfo::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{

	switch(message)
	{
	case GAME_CLIENT_TREE_SELCHANGED:
		{
			OnTreeSelect();
		}
		break;
	}
	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}


void CTestInfo::OnEnChangeEditOutputInfo()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	//CWnd *pWnd = GetParent()->GetDlgItem(IDC_EDIT_OUTPUT_INFO);
	//p.LineScroll(m_edOutputBox.GetLineCount(),0);
	int num = m_edOutputBox.GetLineCount();
}

void CTestInfo::UpdateDisplayInfo(CGameClient* pGameClient)
{
	if(pGameClient != NULL && pGameClient == m_pSelGameClient)
	{
		m_RegionInfo.ValidateRect(NULL);
		m_RegionInfo.RedrawWindow();
		m_MonsterInfo.SetDisplayMonster(pGameClient->GetClientRegion());
		m_RegionGoods.SetDisplayGoods(pGameClient->GetClientRegion());
		m_PlayerInfo.SetDisplayPlayer(pGameClient);
	}
}

BOOL CTestInfo::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam ==13)
	{
		if(m_pSelGameClient == NULL)
		{
			AfxMessageBox("请选择玩家");
			m_edInputBox.SetWindowText("");
			return FALSE;
		}
		UpdateData(TRUE);
		m_edInputBox.SetWindowText(m_StrInputBox);
		CString strScriptName = "C2SOtherTalk()";
		CScriptSys::getInstance()->PerformString(strScriptName);
	}
	return CDialog::PreTranslateMessage(pMsg);
}
const CGUID* CTestInfo::GetPlayerAttributeToGUID()		{ return &(m_pSelGameClient->GetMainPlayer()->GetExID()); }
long CTestInfo::GetPlayerAttributeToSocketID()	{ return m_pSelGameClient->GetSocketID(); }
const char*	CTestInfo::GetPlayerAttributeToName()		{ return m_pSelGameClient->GetMainPlayer()->GetName(); }
const CString CTestInfo::GetInputBoxInfo()	{
	CString str("");
	m_edInputBox.GetWindowText(str);

	CString strText("");
	m_edOutputBox.GetWindowText(strText);
	if(!str.IsEmpty())
	{
		strText += "\n\0";
		strText += str;
		strText += "\r\n";
		m_edOutputBox.SetWindowText(strText);
		m_edOutputBox.LineScroll(m_edOutputBox.GetLineCount(),0); 
		m_edInputBox.SetWindowText("");
		return str;
	}
	return NULL;
}

void CTestInfo::OnRunScript()
{
	UpdateData(TRUE);
	if ( m_ctrGameClients.GetGameClientNode().empty() )
	{
		MessageBox("没有登录的客户端!","提示",MB_OK);
	//	return;
	}
	// TRUE 代表执行单个客户端 --------- 树状控件选择
	m_bCheckOne = m_CheckOne.GetCheck() == 1 ? TRUE:FALSE;
	if ( m_bCheckOne)
	{
		if (m_pSelGameClient)
		{
			long socketid = m_pSelGameClient->GetSocketID();
			//CMessage msg(MSG_C2S_OTHER_TALK);
			//msg.Add((BYTE)0);
			//msg.Add((long)11);
			//msg.Add((long)400);
			//msg.Add(m_pSelGameClient->GetMainPlayer()->GetExID());
			//msg.Add((long)0);
			//msg.Add(m_strScriptName);
			//msg.SendBySocket(socketid);
			return;
		}
	}
	else{
		int size = m_strScriptName.GetLength();
		char *strScript = new char[size + 1];
		sprintf(strScript,"%s",m_strScriptName);
		CScriptSys::getInstance()->PerformString(strScript);
		delete [] strScript;
	}
}


