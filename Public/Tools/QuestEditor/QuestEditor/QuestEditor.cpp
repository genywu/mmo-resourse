// QuestEditor.cpp : 定义应用程序的类行为。
//
#include "stdafx.h"
#include "QuestEditor.h"
#include "QuestEditorDlg.h"
#include "CXmlEdit.h"
//#include "vld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CQuestEditorApp
BEGIN_MESSAGE_MAP(CQuestEditorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()
// CQuestEditorApp 构造
CQuestEditorApp::CQuestEditorApp()
{
}
CQuestEditorApp::~CQuestEditorApp()
{
/*
	vector<tagRole*>::iterator iter;
	iter = m_AllRole.begin();
	for (;iter!=m_AllRole.end();iter++)
	{
		if (*iter==0)
		{
			break;
		}
		else
		{
			delete (*iter);
			*iter=0;
		}
	}
	m_AllRole.clear();*/

}
// 唯一的一个 CQuestEditorApp 对象
 CQuestEditorApp theApp;
// CQuestEditorApp 初始化
BOOL CQuestEditorApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinApp::InitInstance();
	AfxEnableControlContainer();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CQuestEditorDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}
	return FALSE;
}

