// AnnuncePanel.cpp : implementation file
//

#include "stdafx.h"
#include "autoupdate.h"
#include "AnnuncePanel.h"
#include "AutoUpdateRes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BITMAP bmp_info;

/////////////////////////////////////////////////////////////////////////////
// AnnuncePanel dialog

const CString Downloadurl = "http://www.jxonline.net/index/dl_up.shtml";
AnnuncePanel::AnnuncePanel(CWnd* pParent /*=NULL*/)
	: CDialog(AnnuncePanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(AnnuncePanel)
	m_NoticeInfo = _T("系统检测到您的游戏版本太旧，以至无法完成自动升级。");
    m_NoticeInfo = m_NoticeInfo + '\n' + _T("您需要下载最新的升级包进行升级。");

	//}}AFX_DATA_INIT
}


void AnnuncePanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AnnuncePanel)
	DDX_Text(pDX, IDC_NoticeInfo, m_NoticeInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AnnuncePanel, CDialog)
	//{{AFX_MSG_MAP(AnnuncePanel)
	ON_BN_CLICKED(IDC_BUTTON1, OnCancel)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AnnuncePanel message handlers

void AnnuncePanel::OnOK() 
{
	// TODO: Add extra validation here
	ShellExecute(NULL,"open",Downloadurl,NULL,NULL,SW_SHOW);
	CDialog::OnOK();
}

void AnnuncePanel::OnCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

BOOL AnnuncePanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	HBITMAP TempBitMap = LoadBitmap(m_ResModule, MAKEINTRESOURCE(IDB_TITLE));
	if(!m_bmTitle.Attach(TempBitMap)) {
	}

//	LoadBitMapFromInstance(m_bmTitle, m_ResModule, IDB_TITLE);
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if (!((HBITMAP)m_bmTitle))
	{
	    return 0;
	}
	m_bmTitle.GetBitmap(&bmp_info);	
	
    CRect WindowRange(0,0,	bmp_info.bmWidth - 60, 400);
	MoveWindow(WindowRange);
	CenterWindow(NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AnnuncePanel::OnPaint() 
{
//	CPaintDC dc(this); // device context for painting
	

	CPaintDC PaintDC(this); 
	
	if (IsIconic())
	{
	}
	else
	{
		//CDialog::OnPaint();
		//画标题
        CDC dc;
        CBitmap *pOldBitmap = NULL;
		dc.CreateCompatibleDC(&PaintDC);
        pOldBitmap = dc.SelectObject(&m_bmTitle);
		PaintDC.BitBlt(0, 0, bmp_info.bmWidth, bmp_info.bmHeight, &dc, 30, 0, SRCCOPY);
        dc.SelectObject(pOldBitmap);
	}
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
}
