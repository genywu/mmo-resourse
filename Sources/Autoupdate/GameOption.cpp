// GameOption.cpp : implementation file
//

#include "stdafx.h"
#include "autoupdate.h"
#include "GameOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GameOption dialog


GameOption::GameOption(CWnd* pParent /*=NULL*/)
	: CDialog(GameOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(GameOption)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void GameOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GameOption)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GameOption, CDialog)
	//{{AFX_MSG_MAP(GameOption)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GameOption message handlers

void GameOption::OnCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
	
}
