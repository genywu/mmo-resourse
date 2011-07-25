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

// RrMMOSimpleClientView.cpp : CRrMMOSimpleClientView 类的实现
//

#include "Precompiled.h"
#include "RrMMOSimpleClient.h"

#include "RrMMOSimpleClientDoc.h"
#include "RrMMOSimpleClientView.h"

// CRrMMOSimpleClientView

IMPLEMENT_DYNCREATE(CRrMMOSimpleClientView, CView)

BEGIN_MESSAGE_MAP(CRrMMOSimpleClientView, CView)
    // 标准打印命令
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CRrMMOSimpleClientView::OnFilePrintPreview)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CRrMMOSimpleClientView 构造/析构

CRrMMOSimpleClientView::CRrMMOSimpleClientView()
{
    // TODO: 在此处添加构造代码

}

CRrMMOSimpleClientView::~CRrMMOSimpleClientView()
{
}

BOOL CRrMMOSimpleClientView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 在此处通过修改
    //  CREATESTRUCT cs 来修改窗口类或样式

    return CView::PreCreateWindow(cs);
}

// CRrMMOSimpleClientView 绘制

void CRrMMOSimpleClientView::OnDraw(CDC* /*pDC*/)
{
    CRrMMOSimpleClientDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // TODO: 在此处为本机数据添加绘制代码
}


// CRrMMOSimpleClientView 打印


void CRrMMOSimpleClientView::OnFilePrintPreview()
{
    AFXPrintPreview(this);
}

BOOL CRrMMOSimpleClientView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // 默认准备
    return DoPreparePrinting(pInfo);
}

void CRrMMOSimpleClientView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: 添加额外的打印前进行的初始化过程
}

void CRrMMOSimpleClientView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: 添加打印后进行的清理过程
}

void CRrMMOSimpleClientView::OnRButtonUp(UINT nFlags, CPoint point)
{
    USE_PARAM(nFlags);
    ClientToScreen(&point);
    OnContextMenu(this, point);
}

void CRrMMOSimpleClientView::OnContextMenu(CWnd* pWnd, CPoint point)
{
    USE_PARAM(pWnd);
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CRrMMOSimpleClientView 诊断

#ifdef _DEBUG
void CRrMMOSimpleClientView::AssertValid() CONST
{
    CView::AssertValid();
}

void CRrMMOSimpleClientView::Dump(CDumpContext& dc) CONST
{
    CView::Dump(dc);
}

CRrMMOSimpleClientDoc* CRrMMOSimpleClientView::GetDocument() CONST // 非调试版本是内联的
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRrMMOSimpleClientDoc)));
    return (CRrMMOSimpleClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CRrMMOSimpleClientView 消息处理程序


void CRrMMOSimpleClientView::OnLButtonDown(UINT nFlags, CPoint point)
{
    USE_PARAM(nFlags, point);
    this->MessageBox(_T("CRrMMOSimpleClientDoc::OnLButtonDown"));
}

void CRrMMOSimpleClientView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    USE_PARAM(nFlags, point);
    this->MessageBox(_T("CRrMMOSimpleClientDoc::OnLButtonDblClk"));
}
