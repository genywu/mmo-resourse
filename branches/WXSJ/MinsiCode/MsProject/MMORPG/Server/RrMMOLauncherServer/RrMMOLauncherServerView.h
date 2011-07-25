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

// RrMMOLauncherServerView.h : CRrMMOLauncherServerView 类的接口
//


#pragma once


class CRrMMOLauncherServerView : public CView
{
protected: // 仅从序列化创建
    CRrMMOLauncherServerView();
    DECLARE_DYNCREATE(CRrMMOLauncherServerView)

// 属性
public:
    CRrMMOLauncherServerDoc* GetDocument() CONST;

// 操作
public:

// 重写
public:
    virtual VOID OnDraw(CDC* pDC);  // 重写以绘制该视图
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    afx_msg VOID OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg VOID OnLButtonDblClk(UINT nFlags, CPoint point);

protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual VOID OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual VOID OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
    virtual ~CRrMMOLauncherServerView();
#ifdef _DEBUG
    virtual VOID AssertValid() CONST;
    virtual VOID Dump(CDumpContext& dc) CONST;
#endif

protected:

// 生成的消息映射函数
protected:
    afx_msg VOID OnFilePrintPreview();
    afx_msg VOID OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg VOID OnContextMenu(CWnd* pWnd, CPoint point);
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // RrMMOLauncherServerView.cpp 中的调试版本
inline CRrMMOLauncherServerDoc* CRrMMOLauncherServerView::GetDocument() CONST
   { return reinterpret_cast<CRrMMOLauncherServerDoc*>(m_pDocument); }
#endif

