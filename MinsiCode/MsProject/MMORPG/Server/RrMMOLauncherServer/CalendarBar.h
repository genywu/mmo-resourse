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

/////////////////////////////////////////////////////////////////////////////
// CCalendarBar 窗口

class CCalendarBar : public CWnd
{
// 构造
public:
    CCalendarBar();

// 属性
protected:
    CMonthCalCtrl m_wndCalendar;
    INT m_nMyCalendarsY;
    CImageList m_Images;

// 重写
public:
    virtual BOOL Create(CONST RECT& rect, CWnd* pParentWnd, UINT nID = (UINT)-1);

// 实现
public:
    virtual ~CCalendarBar();

protected:
    afx_msg INT OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg VOID OnSize(UINT nType, INT cx, INT cy);
    afx_msg VOID OnPaint();

    DECLARE_MESSAGE_MAP()
};
