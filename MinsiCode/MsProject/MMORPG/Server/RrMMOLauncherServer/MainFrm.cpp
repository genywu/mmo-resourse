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

// MainFrm.cpp : CMainFrame 类的实现
//

#include "Precompiled.h"
#include "RrMMOLauncherServer.h"

#include "MainFrm.h"

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
    ON_WM_CREATE()
    ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
    ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
    ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
    // TODO: 在此添加成员初始化代码
    theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
}

CMainFrame::~CMainFrame()
{
}

INT CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    BOOL bNameValid;
    // 基于持久值设置视觉管理器和样式
    OnApplicationLook(theApp.m_nAppLook);

    CMDITabInfo mdiTabParams;
    mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 其他可用样式...
    mdiTabParams.m_bActiveTabCloseButton = TRUE;      // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
    mdiTabParams.m_bTabIcons = FALSE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
    mdiTabParams.m_bAutoColor = TRUE;    // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
    mdiTabParams.m_bDocumentMenu = TRUE; // 在选项卡区域的右边缘启用文档菜单
    EnableMDITabbedGroups(TRUE, mdiTabParams);

    m_wndRibbonBar.Create(this);
    InitializeRibbon();

    if (!m_wndStatusBar.Create(this))
    {
        TRACE0("未能创建状态栏\n");
        return -1;      // 未能创建
    }

    CString strTitlePane1;
    CString strTitlePane2;
    bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
    ASSERT(bNameValid);
    bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
    ASSERT(bNameValid);
    m_wndStatusBar.AddElement(NEW CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
    m_wndStatusBar.AddExtendedElement(NEW CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

    // 启用 Visual Studio 2005 样式停靠窗口行为
    CDockingManager::SetDockingMode(DT_SMART);
    // 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
    EnableAutoHidePanes(CBRS_ALIGN_ANY);

    // 导航窗格将创建在左侧，因此将暂时禁用左侧的停靠:
    EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

    // 创建并设置“Outlook”导航栏:
    if (!CreateOutlookBar(m_wndNavigationBar, ID_VIEW_NAVIGATION, m_wndTree, m_wndCalendar, 250))
    {
        TRACE0("未能创建导航窗格\n");
        return -1;      // 未能创建
    }

    // 创建标题栏:
    if (!CreateCaptionBar())
    {
        TRACE0("未能创建标题栏\n");
        return -1;      // 未能创建
    }

    // 已创建 Outlook 栏，应允许在左侧停靠。
    EnableDocking(CBRS_ALIGN_LEFT);
    EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

    // 加载菜单项图像(不在任何标准工具栏上):
    CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

    // 创建停靠窗口
    if (!CreateDockingWindows())
    {
        TRACE0("未能创建停靠窗口\n");
        return -1;
    }

    m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
    m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndFileView);
    CDockablePane* pTabbedBar = NULL;
    m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
    m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndOutput);
    m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndProperties);


    // 启用增强的窗口管理对话框
    EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CMDIFrameWndEx::PreCreateWindow(cs) )
        return FALSE;
    // TODO: 在此处通过修改
    //  CREATESTRUCT cs 来修改窗口类或样式

    return TRUE;
}

VOID CMainFrame::InitializeRibbon()
{
    CMFCRibbonButton* pBtnPrint             = NULL;
    CMFCRibbonButton* pBtnPaste             = NULL;
    CMFCRibbonButton* pBtnStatusBar         = NULL;
    CMFCRibbonButton* pBtnCaptionBar        = NULL;
    CMFCRibbonButton* pBtnWindows           = NULL;
    CMFCRibbonButton* pVisualStyleButton    = NULL;
    _ENTER_FUN_C;

    BOOL bNameValid;

    CString strTemp;
    bNameValid = strTemp.LoadString(IDS_RIBBON_FILE);
    ASSERT(bNameValid);

    // 加载面板图像:
    m_PanelImages.SetImageSize(CSize(16, 16));
    m_PanelImages.Load(IDB_BUTTONS);

    // 初始主按钮:
    m_MainButton.SetImage(IDB_MAIN);
    m_MainButton.SetText(_T("\nf"));
    m_MainButton.SetToolTipText(strTemp);

    m_wndRibbonBar.SetApplicationButton(&m_MainButton, CSize (45, 45));
    CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory(strTemp, IDB_FILESMALL, IDB_FILELARGE);

    bNameValid = strTemp.LoadString(IDS_RIBBON_NEW);
    ASSERT(bNameValid);
    pMainPanel->Add(new CMFCRibbonButton(ID_FILE_NEW, strTemp, 0, 0));
    bNameValid = strTemp.LoadString(IDS_RIBBON_OPEN);
    ASSERT(bNameValid);
    pMainPanel->Add(new CMFCRibbonButton(ID_FILE_OPEN, strTemp, 1, 1));
    bNameValid = strTemp.LoadString(IDS_RIBBON_SAVE);
    ASSERT(bNameValid);
    pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE, strTemp, 2, 2));
    bNameValid = strTemp.LoadString(IDS_RIBBON_SAVEAS);
    ASSERT(bNameValid);
    pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE_AS, strTemp, 3, 3));

    bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT);
    ASSERT(bNameValid);
    pBtnPrint = new CMFCRibbonButton(ID_FILE_PRINT, strTemp, 6, 6);
    pBtnPrint->SetKeys(_T("p"), _T("w"));
    bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_LABEL);
    ASSERT(bNameValid);
    pBtnPrint->AddSubItem(new CMFCRibbonLabel(strTemp));
    bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_QUICK);
    ASSERT(bNameValid);
    pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_DIRECT, strTemp, 7, 7, TRUE));
    bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_PREVIEW);
    ASSERT(bNameValid);
    pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_PREVIEW, strTemp, 8, 8, TRUE));
    bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_SETUP);
    ASSERT(bNameValid);
    pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_SETUP, strTemp, 11, 11, TRUE));
    pMainPanel->Add(pBtnPrint);
    pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

    bNameValid = strTemp.LoadString(IDS_RIBBON_CLOSE);
    ASSERT(bNameValid);
    pMainPanel->Add(new CMFCRibbonButton(ID_FILE_CLOSE, strTemp, 9, 9));

    bNameValid = strTemp.LoadString(IDS_RIBBON_RECENT_DOCS);
    ASSERT(bNameValid);
    pMainPanel->AddRecentFilesList(strTemp);

    bNameValid = strTemp.LoadString(IDS_RIBBON_EXIT);
    ASSERT(bNameValid);
    pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_APP_EXIT, strTemp, 15));

    // 为“剪贴板”面板添加“主”类别:
    bNameValid = strTemp.LoadString(IDS_RIBBON_HOME);
    ASSERT(bNameValid);
    CMFCRibbonCategory* pCategoryHome = m_wndRibbonBar.AddCategory(strTemp, IDB_WRITESMALL, IDB_WRITELARGE);

    // 创建“剪贴板”面板:
    bNameValid = strTemp.LoadString(IDS_RIBBON_CLIPBOARD);
    ASSERT(bNameValid);
    CMFCRibbonPanel* pPanelClipboard = pCategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));

    bNameValid = strTemp.LoadString(IDS_RIBBON_PASTE);
    ASSERT(bNameValid);
    pBtnPaste = new CMFCRibbonButton(ID_EDIT_PASTE, strTemp, 0, 0);
    pPanelClipboard->Add(pBtnPaste);

    bNameValid = strTemp.LoadString(IDS_RIBBON_CUT);
    ASSERT(bNameValid);
    pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_CUT, strTemp, 1));
    bNameValid = strTemp.LoadString(IDS_RIBBON_COPY);
    ASSERT(bNameValid);
    pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_COPY, strTemp, 2));
    bNameValid = strTemp.LoadString(IDS_RIBBON_SELECTALL);
    ASSERT(bNameValid);
    pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_SELECT_ALL, strTemp, -1));

    // 创建和添加“视图”面板:
    bNameValid = strTemp.LoadString(IDS_RIBBON_VIEW);
    ASSERT(bNameValid);
    CMFCRibbonPanel* pPanelView = pCategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon (7));

    bNameValid = strTemp.LoadString(IDS_RIBBON_STATUSBAR);
    ASSERT(bNameValid);
    pBtnStatusBar = new CMFCRibbonCheckBox(ID_VIEW_STATUS_BAR, strTemp);
    pPanelView->Add(pBtnStatusBar);
    bNameValid = strTemp.LoadString(IDS_RIBBON_CAPTIONBAR);
    ASSERT(bNameValid);
    pBtnCaptionBar = new CMFCRibbonCheckBox(ID_VIEW_CAPTION_BAR, strTemp);
    pPanelView->Add(pBtnCaptionBar);

    // 创建和添加“窗口”面板:
    bNameValid = strTemp.LoadString(IDS_RIBBON_WINDOW);
    ASSERT(bNameValid);
    CMFCRibbonPanel* pPanelWindow = pCategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon (7));

    bNameValid = strTemp.LoadString(IDS_RIBBON_WINDOWS);
    ASSERT(bNameValid);
    pBtnWindows = new CMFCRibbonButton(ID_WINDOW_MANAGER, strTemp, -1, 1);
    pBtnWindows->SetMenu(IDR_WINDOWS_MENU, TRUE);
    pPanelWindow->Add(pBtnWindows);

    // 将元素添加到选项卡右侧:
    bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE);
    ASSERT(bNameValid);
    pVisualStyleButton = new CMFCRibbonButton(INVALID_UID, strTemp, -1, -1);

    pVisualStyleButton->SetMenu(IDR_THEME_MENU, FALSE /* 无默认命令*/, TRUE /* 右对齐*/);

    bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_TIP);
    ASSERT(bNameValid);
    pVisualStyleButton->SetToolTipText(strTemp);
    bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_DESC);
    ASSERT(bNameValid);
    pVisualStyleButton->SetDescription(strTemp);
    m_wndRibbonBar.AddToTabs(pVisualStyleButton);

    // 添加快速访问工具栏命令:
    CList<UINT, UINT> lstQATCmds;

    lstQATCmds.AddTail(ID_FILE_NEW);
    lstQATCmds.AddTail(ID_FILE_OPEN);
    lstQATCmds.AddTail(ID_FILE_SAVE);
    lstQATCmds.AddTail(ID_FILE_PRINT_DIRECT);

    m_wndRibbonBar.SetQuickAccessCommands(lstQATCmds);

    m_wndRibbonBar.AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, _T("\na"), m_PanelImages.ExtractIcon (0)));

    _DISPOSE_END_C
    {
        SAFE_DELETE(pBtnPrint);
        SAFE_DELETE(pBtnPaste);
        SAFE_DELETE(pBtnStatusBar);
        SAFE_DELETE(pBtnCaptionBar);
        SAFE_DELETE(pBtnWindows);
        SAFE_DELETE(pVisualStyleButton);
    }_LEAVE_FUN;
}

BOOL CMainFrame::CreateDockingWindows()
{
    BOOL bNameValid;

    // 创建类视图
    CString strClassView;
    bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
    ASSERT(bNameValid);
    if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
    {
        TRACE0("未能创建“类视图”窗口\n");
        return FALSE; // 未能创建
    }

    // 创建文件视图
    CString strFileView;
    bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
    ASSERT(bNameValid);
    if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
    {
        TRACE0("未能创建“文件视图”窗口\n");
        return FALSE; // 未能创建
    }

    // 创建输出窗口
    CString strOutputWnd;
    bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
    ASSERT(bNameValid);
    if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
    {
        TRACE0("未能创建输出窗口\n");
        return FALSE; // 未能创建
    }

    // 创建属性窗口
    CString strPropertiesWnd;
    bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
    ASSERT(bNameValid);
    if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
    {
        TRACE0("未能创建“属性”窗口\n");
        return FALSE; // 未能创建
    }

    SetDockingWindowIcons(theApp.m_bHiColorIcons);
    return TRUE;
}

VOID CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
    HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    m_wndFileView.SetIcon(hFileViewIcon, FALSE);

    HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    m_wndClassView.SetIcon(hClassViewIcon, FALSE);

    HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

    HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

    UpdateMDITabbedBarsIcons();
}

BOOL CMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CMFCShellTreeCtrl& tree, CCalendarBar& calendar, INT nInitialWidth)
{
    CWindowDC dc(NULL);

    bar.SetMode2003();

    BOOL bNameValid;
    CString strTemp;
    bNameValid = strTemp.LoadString(IDS_SHORTCUTS);
    ASSERT(bNameValid);
    if (!bar.Create(strTemp, this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE | CBRS_LEFT))
    {
        return FALSE; // 未能创建
    }

    CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();

    if (pOutlookBar == NULL)
    {
        ASSERT(FALSE);
        return FALSE;
    }

    pOutlookBar->EnableInPlaceEdit(TRUE);

    static UINT uiPageID = 1;

    DWORD dwPaneStyle = AFX_DEFAULT_TOOLBAR_STYLE | CBRS_FLOAT_MULTI;
    USE_PARAM(dwPaneStyle);

    // 可浮动，可自动隐藏，可调整大小，但不能关闭
    DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE;

    CRect rectDummy(0, 0, 0, 0);
    CONST DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

    tree.Create(dwTreeStyle, rectDummy, &bar, 1200);
    bNameValid = strTemp.LoadString(IDS_FOLDERS);
    ASSERT(bNameValid);
    pOutlookBar->AddControl(&tree, strTemp, 2, TRUE, dwStyle);

    calendar.Create(rectDummy, &bar, 1201);
    bNameValid = strTemp.LoadString(IDS_CALENDAR);
    ASSERT(bNameValid);
    pOutlookBar->AddControl(&calendar, strTemp, 3, TRUE, dwStyle);

    bar.SetPaneStyle(bar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

    pOutlookBar->SetImageList(theApp.m_bHiColorIcons ? IDB_PAGES_HC : IDB_PAGES, 24);
    pOutlookBar->SetToolbarImageList(theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL, 16);
    pOutlookBar->RecalcLayout();

    BOOL bAnimation = theApp.GetInt(_T("OutlookAnimation"), TRUE);
    CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);

    bar.SetButtonsFont(&afxGlobalData.fontBold);

    return TRUE;
}

BOOL CMainFrame::CreateCaptionBar()
{
    if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
    {
        TRACE0("未能创建标题栏\n");
        return FALSE;
    }

    BOOL bNameValid;

    CString strTemp, strTemp2;
    bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
    ASSERT(bNameValid);
    m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
    bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
    ASSERT(bNameValid);
    m_wndCaptionBar.SetButtonToolTip(strTemp);

    bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
    ASSERT(bNameValid);
    m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

    m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
    bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
    ASSERT(bNameValid);
    bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
    ASSERT(bNameValid);
    m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

    return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
VOID CMainFrame::AssertValid() CONST
{
    CMDIFrameWndEx::AssertValid();
}

VOID CMainFrame::Dump(CDumpContext& dc) CONST
{
    CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

VOID CMainFrame::OnWindowManager()
{
    ShowWindowsDialog();
}

VOID CMainFrame::OnApplicationLook(UINT id)
{
    CWaitCursor wait;

    theApp.m_nAppLook = id;

    switch (theApp.m_nAppLook)
    {
    case ID_VIEW_APPLOOK_WIN_2000:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
        break;

    case ID_VIEW_APPLOOK_OFF_XP:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
        break;

    case ID_VIEW_APPLOOK_WIN_XP:
        CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
        break;

    case ID_VIEW_APPLOOK_OFF_2003:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
        CDockingManager::SetDockingMode(DT_SMART);
        break;

    case ID_VIEW_APPLOOK_VS_2005:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
        CDockingManager::SetDockingMode(DT_SMART);
        break;

    default:
        switch (theApp.m_nAppLook)
        {
        case ID_VIEW_APPLOOK_OFF_2007_BLUE:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_BLACK:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_SILVER:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_AQUA:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
            break;
        }

        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
        CDockingManager::SetDockingMode(DT_SMART);
    }

    RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

    theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

VOID CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

VOID CMainFrame::OnViewCaptionBar()
{
    m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
    RecalcLayout(FALSE);
}

VOID CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}
