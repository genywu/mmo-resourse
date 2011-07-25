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

// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "CalendarBar.h"
#include "Resource.h"

class COutlookBar : public CMFCOutlookBar
{
    virtual BOOL AllowShowOnPaneMenu() CONST { return TRUE; }
    virtual void GetPaneName(CString& strName) CONST
    {
        BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR);
        AssertEx(AL_NORMAL, bNameValid, _T("名字无效"));
    }
};

class CMainFrame : public CMDIFrameWndEx
{
    DECLARE_DYNAMIC(CMainFrame)
public:
    CMainFrame();

// 属性
public:

// 操作
public:

// 重写
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() CONST;
    virtual void Dump(CDumpContext& dc) CONST;
#endif

protected:  // 控件条嵌入成员
    CMFCRibbonBar     m_wndRibbonBar;
    CMFCRibbonApplicationButton m_MainButton;
    CMFCToolBarImages m_PanelImages;
    CMFCRibbonStatusBar  m_wndStatusBar;
    CFileView         m_wndFileView;
    CClassView        m_wndClassView;
    COutputWnd        m_wndOutput;
    CPropertiesWnd    m_wndProperties;
    COutlookBar       m_wndNavigationBar;
    CMFCShellTreeCtrl m_wndTree;
    CCalendarBar      m_wndCalendar;
    CMFCCaptionBar    m_wndCaptionBar;

// 生成的消息映射函数
protected:
    afx_msg INT OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnWindowManager();
    afx_msg void OnApplicationLook(UINT id);
    afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
    afx_msg void OnViewCaptionBar();
    afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
    DECLARE_MESSAGE_MAP()

    void InitializeRibbon();
    BOOL CreateDockingWindows();
    void SetDockingWindowIcons(BOOL bHiColorIcons);
    BOOL CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CMFCShellTreeCtrl& tree, CCalendarBar& calendar, INT nInitialWidth);
    BOOL CreateCaptionBar();

    INT FindFocusedOutlookWnd(CMFCOutlookBarTabCtrl** ppOutlookWnd);

    CMFCOutlookBarTabCtrl* FindOutlookParent(CWnd* pWnd);
    CMFCOutlookBarTabCtrl* m_pCurrOutlookWnd;
    CMFCOutlookBarPane*    m_pCurrOutlookPage;
};


