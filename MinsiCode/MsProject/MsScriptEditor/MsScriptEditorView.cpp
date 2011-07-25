
// MsScriptEditorView.cpp : CMsScriptEditorView 类的实现
//

#include "stdafx.h"
#include "MsScriptEditor.h"

#include "MsScriptEditorDoc.h"
#include "MsScriptEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMsScriptEditorView

IMPLEMENT_DYNCREATE(CMsScriptEditorView, CView)

BEGIN_MESSAGE_MAP(CMsScriptEditorView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMsScriptEditorView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMsScriptEditorView 构造/析构

CMsScriptEditorView::CMsScriptEditorView()
{
	EnableActiveAccessibility();
	// TODO: 在此处添加构造代码

}

CMsScriptEditorView::~CMsScriptEditorView()
{
}

BOOL CMsScriptEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMsScriptEditorView 绘制

void CMsScriptEditorView::OnDraw(CDC* /*pDC*/)
{
	CMsScriptEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CMsScriptEditorView 打印


void CMsScriptEditorView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CMsScriptEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMsScriptEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMsScriptEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CMsScriptEditorView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMsScriptEditorView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CMsScriptEditorView 诊断

#ifdef _DEBUG
void CMsScriptEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CMsScriptEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMsScriptEditorDoc* CMsScriptEditorView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMsScriptEditorDoc)));
	return (CMsScriptEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CMsScriptEditorView 消息处理程序
