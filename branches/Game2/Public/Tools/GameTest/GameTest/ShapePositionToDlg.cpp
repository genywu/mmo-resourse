// ShapePositionToDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GameTest.h"
#include "ShapePositionToDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CShapePositionToDlg 对话框

IMPLEMENT_DYNAMIC(CShapePositionToDlg, CDialog)

CShapePositionToDlg::CShapePositionToDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShapePositionToDlg::IDD, pParent)
	, lPosX(0)
	, lPosY(0)
{

}

CShapePositionToDlg::~CShapePositionToDlg()
{
}

void CShapePositionToDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PosX, lPosX);
	DDX_Text(pDX, IDC_PosY, lPosY);
	DDX_Control(pDX, IDC_PosX, m_PosX);
	DDX_Control(pDX, IDC_PosY, m_PosY);
}


BEGIN_MESSAGE_MAP(CShapePositionToDlg, CDialog)
//	ON_COMMAND(ID_SetPositionTo, &CShapePositionToDlg::OnSetpositionto)
END_MESSAGE_MAP()


// CShapePositionToDlg 消息处理程序

void CShapePositionToDlg::OnSetpositionto()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);
	char str[5];
	lPosX = m_PosX.GetWindowTextA(str,4);
	lPosY = m_PosY.GetWindowTextA(str,4);
}
