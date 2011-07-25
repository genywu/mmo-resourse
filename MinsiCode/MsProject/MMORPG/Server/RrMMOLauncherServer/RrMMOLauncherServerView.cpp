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

// RrMMOLauncherServerView.cpp : CRrMMOLauncherServerView 类的实现
//

#include "Precompiled.h"
#include "RrMMOLauncherServer.h"

#include "RrMMOLauncherServerDoc.h"
#include "RrMMOLauncherServerView.h"

// CRrMMOLauncherServerView

IMPLEMENT_DYNCREATE(CRrMMOLauncherServerView, CView)

BEGIN_MESSAGE_MAP(CRrMMOLauncherServerView, CView)
    // 标准打印命令
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CRrMMOLauncherServerView::OnFilePrintPreview)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CRrMMOLauncherServerView 构造/析构

CRrMMOLauncherServerView::CRrMMOLauncherServerView()
{
    // TODO: 在此处添加构造代码

}

CRrMMOLauncherServerView::~CRrMMOLauncherServerView()
{
}

BOOL CRrMMOLauncherServerView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 在此处通过修改
    //  CREATESTRUCT cs 来修改窗口类或样式

    return CView::PreCreateWindow(cs);
}

// CRrMMOLauncherServerView 绘制

VOID CRrMMOLauncherServerView::OnDraw(CDC* /*pDC*/)
{
    CRrMMOLauncherServerDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // TODO: 在此处为本机数据添加绘制代码
}


// CRrMMOLauncherServerView 打印


VOID CRrMMOLauncherServerView::OnFilePrintPreview()
{
    AFXPrintPreview(this);
}

BOOL CRrMMOLauncherServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // 默认准备
    return DoPreparePrinting(pInfo);
}

VOID CRrMMOLauncherServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: 添加额外的打印前进行的初始化过程
}

VOID CRrMMOLauncherServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: 添加打印后进行的清理过程
}

VOID CRrMMOLauncherServerView::OnRButtonUp(UINT nFlags, CPoint point)
{
    USE_PARAM(nFlags);
    ClientToScreen(&point);
    OnContextMenu(this, point);
}

VOID CRrMMOLauncherServerView::OnContextMenu(CWnd* pWnd, CPoint point)
{
    USE_PARAM(pWnd);
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CRrMMOLauncherServerView 诊断

#ifdef _DEBUG
VOID CRrMMOLauncherServerView::AssertValid() CONST
{
    CView::AssertValid();
}

VOID CRrMMOLauncherServerView::Dump(CDumpContext& dc) CONST
{
    CView::Dump(dc);
}

CRrMMOLauncherServerDoc* CRrMMOLauncherServerView::GetDocument() CONST // 非调试版本是内联的
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRrMMOLauncherServerDoc)));
    return (CRrMMOLauncherServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CRrMMOLauncherServerView 消息处理程序


VOID CRrMMOLauncherServerView::OnLButtonDown(UINT nFlags, CPoint point)
{
    USE_PARAM(nFlags, point);
    //this->MessageBox(_T("CRrMMOLauncherServerDoc::OnLButtonDown"));
}

VOID CRrMMOLauncherServerView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    USE_PARAM(nFlags, point);
    this->MessageBox(_T("CRrMMOLauncherServerDoc::OnLButtonDblClk"));
}
