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

#pragma once

#include "ViewTree.h"

class CFileViewToolBar : public CMFCToolBar
{
    virtual VOID OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
    {
        CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
    }

    virtual BOOL AllowShowOnList() CONST { return FALSE; }
};

class CFileView : public CDockablePane
{
// 构造
public:
    CFileView();

    VOID AdjustLayout();
    VOID OnChangeVisualStyle();

// 属性
protected:

    CViewTree m_wndFileView;
    CImageList m_FileViewImages;
    CFileViewToolBar m_wndToolBar;

protected:
    VOID FillFileView();

// 实现
public:
    virtual ~CFileView();

protected:
    afx_msg INT OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg VOID OnSize(UINT nType, INT cx, INT cy);
    afx_msg VOID OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg VOID OnProperties();
    afx_msg VOID OnFileOpen();
    afx_msg VOID OnFileOpenWith();
    afx_msg VOID OnDummyCompile();
    afx_msg VOID OnEditCut();
    afx_msg VOID OnEditCopy();
    afx_msg VOID OnEditClear();
    afx_msg VOID OnPaint();
    afx_msg VOID OnSetFocus(CWnd* pOldWnd);

    DECLARE_MESSAGE_MAP()
};

