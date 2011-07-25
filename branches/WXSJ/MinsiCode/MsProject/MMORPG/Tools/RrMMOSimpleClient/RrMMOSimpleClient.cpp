// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面
// ("Fluent UI")，该示例仅作为参考资料提供，
// 用以补充《Microsoft 基础类参考》和
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问
// http://msdn.microsoft.com/officeui。
//
// 版权所有 (C) Microsoft Corporation
// 保留所有权利。

// RrMMOSimpleClient.cpp : 定义应用程序的类行为。
//

#include "Precompiled.h"
#include "afxwinappex.h"
#include "RrMMOSimpleClient.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "RrMMOSimpleClientDoc.h"
#include "RrMMOSimpleClientView.h"

// CRrMMOSimpleClientApp

BEGIN_MESSAGE_MAP(CRrMMOSimpleClientApp, CWinAppEx)
    ON_COMMAND(ID_APP_ABOUT, &CRrMMOSimpleClientApp::OnAppAbout)
    // 基于文件的标准文档命令
    ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
    // 标准打印设置命令
    ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CRrMMOSimpleClientApp 构造

CRrMMOSimpleClientApp::CRrMMOSimpleClientApp()
{

    m_bHiColorIcons = TRUE;

    // TODO: 在此处添加构造代码，
    // 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CRrMMOSimpleClientApp 对象

CRrMMOSimpleClientApp theApp;
// 生成的此标识符在统计上对于您的应用程序是唯一的
// 如果您更愿选择一个特定的标识符，则可以更改它

// {54913426-1B7F-4EFB-A4C9-0A00628AD316}
static CONST CLSID clsid =
{ 0x54913426, 0x1B7F, 0x4EFB, { 0xA4, 0xC9, 0xA, 0x0, 0x62, 0x8A, 0xD3, 0x16 } };

CONST GUID CDECL BASED_CODE _tlid =
        { 0x5B178151, 0xD3A0, 0x4F01, { 0xBB, 0x26, 0x12, 0x27, 0xAB, 0xE1, 0xA9, 0x79 } };
CONST WORD _wVerMajor = 1;
CONST WORD _wVerMinor = 0;


// CRrMMOSimpleClientApp 初始化

BOOL CRrMMOSimpleClientApp::InitInstance()
{
    CWinAppEx::InitInstance();

    if (!AfxSocketInit())
    {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
    }

    // 初始化 OLE 库
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }
    AfxEnableControlContainer();
    // 标准初始化
    // 如果未使用这些功能并希望减小
    // 最终可执行文件的大小，则应移除下列
    // 不需要的特定初始化例程
    // 更改用于存储设置的注册表项
    // TODO: 应适当修改该字符串，
    // 例如修改为公司或组织名
    SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
    LoadStdProfileSettings(0);  // 加载标准 INI 文件选项

    InitContextMenuManager();
    InitShellManager();

    InitKeyboardManager();

    InitTooltipManager();
    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
        RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    // 注册应用程序的文档模板。文档模板
    // 将用作文档、框架窗口和视图之间的连接
    CMultiDocTemplate* pDocTemplate;
    pDocTemplate = new CMultiDocTemplate(IDR_RRMMOSIMPLECLIETYPE,
        RUNTIME_CLASS(CRrMMOSimpleClientDoc),
        RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
        RUNTIME_CLASS(CRrMMOSimpleClientView));
    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);
    // 将 COleTemplateServer 连接到文档模板
    // COleTemplateServer 通过使用
    // 文档模板中指定的信息来为请求 OLE 容器
    // 创建新文档
    m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);
    // 将所有的 OLE 服务器工厂注册为正在运行。这将启用
    //  OLE 库以从其他应用程序中创建对象
    COleTemplateServer::RegisterAll();
        // 注意: MDI 应用程序将注册所有的服务器对象，而不管
        // 命令行上的 /Embedding 或 /Automation

    // 创建主 MDI 框架窗口
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
    {
        delete pMainFrame;
        return FALSE;
    }
    m_pMainWnd = pMainFrame;
    // 仅当具有后缀时才调用 DragAcceptFiles
    //  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生


    // 分析标准外壳命令、DDE、打开文件操作的命令行
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    // 应用程序是用 /Embedding 或 /Automation 开关启动的。
    //使应用程序作为自动化服务器运行。
    if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
    {
        // 不显示主窗口
        return TRUE;
    }
    // 使用 /Unregserver 或 /Unregister 开关启动应用程序。注销
    // 类型库。其他注销操作在 ProcessShellCommand() 中发生。
    else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
    {
        m_server.UpdateRegistry(OAT_DISPATCH_OBJECT, NULL, NULL, FALSE);
        AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
    }
    // 应用程序是以独立方式或用其他开关(如 /Register
    // 或 /Regserver)启动的。更新注册表项，包括类型库。
    else
    {
        m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
        COleObjectFactory::UpdateRegistryAll();
        AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
    }

    // 调度在命令行中指定的命令。如果
    // 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;
    // 主窗口已初始化，因此显示它并对其进行更新
    pMainFrame->ShowWindow(m_nCmdShow);
    pMainFrame->UpdateWindow();

    return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// 对话框数据
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CRrMMOSimpleClientApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

// CRrMMOSimpleClientApp 自定义加载/保存方法

void CRrMMOSimpleClientApp::PreLoadState()
{
    BOOL bNameValid;
    CString strName;
    bNameValid = strName.LoadString(IDS_EDIT_MENU);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
    bNameValid = strName.LoadString(IDS_EXPLORER);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CRrMMOSimpleClientApp::LoadCustomState()
{
}

void CRrMMOSimpleClientApp::SaveCustomState()
{
}

// CRrMMOSimpleClientApp 消息处理程序



