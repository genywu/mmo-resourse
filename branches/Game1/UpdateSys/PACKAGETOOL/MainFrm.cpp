// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "PackageTool.h"
#include "PackageToolDoc.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPENFOLDER, OnFileOpenfolder)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OPERATOR,OnUpdateStatuOperator)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_FILENAME,OnUpdateStatuFileName)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TOTALNUM,OnUpdateStatuTotaNumb)
	ON_COMMAND(ID_FILE_OPENCLIENTINFO, OnFileOpenclientinfo)
	ON_COMMAND(ID_FILE_PATCH_INFO, OnFileOpenpatchinfo)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	m_strLastOpenFolder = "F:\\saga3d2\\RUN\\UpdateSys\\Client-Resouce";
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}


	static UINT indicatorss[]=
	{
		ID_INDICATOR_OPERATOR,
		ID_INDICATOR_FILENAME,
		//ID_INDICATOR_TOTALNUM,
	};

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicatorss,
		  sizeof(indicatorss)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	int index = m_wndStatusBar.CommandToIndex(ID_INDICATOR_OPERATOR);
	m_wndStatusBar.SetPaneInfo(index,ID_INDICATOR_OPERATOR,SBPS_NORMAL,150);

	index = m_wndStatusBar.CommandToIndex(ID_INDICATOR_FILENAME);
	//m_wndStatusBar.SetPaneInfo(index,ID_INDICATOR_FILENAME,SBPS_NORMAL,60);
	m_wndStatusBar.SetPaneStyle(index,SBPS_STRETCH);

	//index = m_wndStatusBar.CommandToIndex(ID_INDICATOR_TOTALNUM);
	//m_wndStatusBar.SetPaneInfo(index,ID_INDICATOR_TOTALNUM,SBPS_NORMAL,100);

	// TODO: 如果不需要工具栏可停靠，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	CMenu* pMenu = GetMenu();
	if(pMenu)
	{
		pMenu->CheckMenuItem(ID_SETUPSERVERMODE, MF_UNCHECKED);
		pMenu->CheckMenuItem(ID_SETUPCLIENTMODE, MF_CHECKED);
	}
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序


void CMainFrame::OnFileOpenfolder()
{
	// TODO: 在此添加命令处理程序代码
	//char strPath[MAX_PATH] = "F:\saga3d2\RUN\UpdateSys\Client-Resouce";
	CString strFolder;
	BROWSEINFO bi;
	ZeroMemory(&bi,sizeof(BROWSEINFO));
	bi.hwndOwner=GetSafeHwnd();
	//bi.pidlRoot =SHSimpleIDListFromPath( (LPCWSTR)strPath );
	bi.lpszTitle="选择要操作的资源文件夹:";
	LPITEMIDLIST idl=SHBrowseForFolder(&bi);
	if(idl==NULL)
		return;
	SHGetPathFromIDList(idl,strFolder.GetBuffer(MAX_PATH));
	//m_strLastOpenFolder = strFolder;

	CPackageToolDoc* pDoc = static_cast<CPackageToolDoc*>(GetActiveDocument());
	if(pDoc)	pDoc->OnOpenfolder(strFolder);
}


void CMainFrame::OnUpdateStatuOperator(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CMainFrame::OnUpdateStatuFileName(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}
void CMainFrame::OnUpdateStatuTotaNumb(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void	CMainFrame::OnDisplayStatus(tagStaDisplay* pStaDisplay)
{
	if(pStaDisplay==NULL)	return;

	int index = m_wndStatusBar.CommandToIndex(ID_INDICATOR_OPERATOR);
	m_wndStatusBar.SetPaneText(index,pStaDisplay->strOp.c_str());

	index = m_wndStatusBar.CommandToIndex(ID_INDICATOR_FILENAME);
	m_wndStatusBar.SetPaneText(index,pStaDisplay->strFileName.c_str());

	//CString str;
	//str.Format("%d",pStaDisplay->nNumber);

	//index = m_wndStatusBar.CommandToIndex(ID_INDICATOR_TOTALNUM);
	//m_wndStatusBar.SetPaneText(index,(LPCTSTR)str);
}
BOOL CMainFrame::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	
	switch(message)
	{
	case STATUS_UPDATE:
		{
			tagStaDisplay* pSta = (tagStaDisplay*)wParam;
			OnDisplayStatus(pSta);
		}
		break;
	}

	return CFrameWnd::OnWndMsg(message, wParam, lParam, pResult);
}

void CMainFrame::OnFileOpenclientinfo()
{
	// TODO: 在此添加命令处理程序代码
	CString strFolder;
	BROWSEINFO bi;
	ZeroMemory(&bi,sizeof(BROWSEINFO));
	bi.hwndOwner=GetSafeHwnd();
	//bi.pidlRoot =SHSimpleIDListFromPath( (LPCWSTR)strPath );
	bi.lpszTitle="选择游戏资源文件夹:";
	LPITEMIDLIST idl=SHBrowseForFolder(&bi);
	if(idl==NULL)
		return;
	SHGetPathFromIDList(idl,strFolder.GetBuffer(MAX_PATH));
	//m_strLastOpenFolder = strFolder;

	CPackageToolDoc* pDoc = static_cast<CPackageToolDoc*>(GetActiveDocument());
	if(pDoc)	pDoc->OnOpenGameResouce(strFolder);
}

void CMainFrame::OnFileOpenpatchinfo()
{
	// TODO: 在此添加命令处理程序代码
	CString strFolder;
	BROWSEINFO bi;
	ZeroMemory(&bi,sizeof(BROWSEINFO));
	bi.hwndOwner=GetSafeHwnd();
	//bi.pidlRoot =SHSimpleIDListFromPath( (LPCWSTR)strPath );
	bi.lpszTitle="选择patch资源文件夹:";
	LPITEMIDLIST idl=SHBrowseForFolder(&bi);
	if(idl==NULL)
		return;
	SHGetPathFromIDList(idl,strFolder.GetBuffer(MAX_PATH));
	//m_strLastOpenFolder = strFolder;

	CPackageToolDoc* pDoc = static_cast<CPackageToolDoc*>(GetActiveDocument());
	if(pDoc)	pDoc->OnOpenPatchResouce(strFolder);
}