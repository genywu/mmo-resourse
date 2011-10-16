// updaterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "updater.h"
#include "updaterDlg.h"
#include ".\updaterdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CupdaterDlg 对话框

//判断文件是否存在
bool CheckFile(const char *name)
{
	WIN32_FIND_DATA fdt;
	if( FindFirstFile(name, &fdt)==INVALID_HANDLE_VALUE )
		return false;
	else 
		return true;
}

CupdaterDlg::CupdaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CupdaterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CupdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TEXT, m_cText);
}

BEGIN_MESSAGE_MAP(CupdaterDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CupdaterDlg 消息处理程序

BOOL CupdaterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetTimer(1, 1000, NULL);
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CupdaterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CupdaterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CupdaterDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	static bool bRemoveOld = false, bRenameExe2Old = false;

	// 10秒钟不成功则打开关闭按钮
	static int num = 0;
	num ++;
	if (num>10)
	{
		KillTimer(1);
		MessageBox("更新失败！请手动把游戏目录中的update.new，改名为update.exe，然后重试。\n如果还不能正常进入游戏，请到 www.saga3d.com 下载补丁包。");
		CDialog::OnOK();
		ShellExecute( 0, 0, "http://www.saga3d.com", 0, 0, SW_SHOWNORMAL );
		return;
	}

	// 如果update.new文件不存在，则不执行
	if (CheckFile("update.new")==false)
	{
		CDialog::OnTimer(nIDEvent);
		return;
	}

	if (bRemoveOld == false)
	{
		remove("update.old");
		bRemoveOld = true;
	}

	if (bRenameExe2Old == false)
	{
		if (rename("update.exe","update.old") == 0)
		{
			bRenameExe2Old = true;
		}
		else
		{
			if (CheckFile("update.exe") == false)
			{
				bRenameExe2Old = true;
			}
			else
			{
				CDialog::OnTimer(nIDEvent);
				return;
			}
		}
	}

	if (rename("update.new","update.exe") == 0)
	{
		KillTimer(1);

		m_cText.SetWindowText("更新成功，正在重新启动...");

		Sleep(1000);
		CDialog::OnOK();
		WinExec("update.exe",SW_SHOWNORMAL);
		return;
	}

	CDialog::OnTimer(nIDEvent);
}

void CupdaterDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
}

void CupdaterDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

//	CDialog::OnCancel();
}
