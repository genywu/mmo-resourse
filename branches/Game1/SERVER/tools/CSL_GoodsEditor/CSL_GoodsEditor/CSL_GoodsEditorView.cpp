// CSL_GoodsEditorView.cpp : CCSL_GoodsEditorView 类的实现
//

#include "stdafx.h"
#include "CSL_GoodsEditor.h"

#include "CSL_GoodsEditorDoc.h"
#include "CSL_GoodsEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCSL_GoodsEditorView

IMPLEMENT_DYNCREATE(CCSL_GoodsEditorView, CView)

BEGIN_MESSAGE_MAP(CCSL_GoodsEditorView, CView)
END_MESSAGE_MAP()

// CCSL_GoodsEditorView 构造/析构

CCSL_GoodsEditorView::CCSL_GoodsEditorView()
{
	// TODO: 在此处添加构造代码

}

CCSL_GoodsEditorView::~CCSL_GoodsEditorView()
{
}

BOOL CCSL_GoodsEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return CView::PreCreateWindow(cs);
}

// CCSL_GoodsEditorView 绘制

void CCSL_GoodsEditorView::OnDraw(CDC* /*pDC*/)
{
	CCSL_GoodsEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CCSL_GoodsEditorView 诊断

#ifdef _DEBUG
void CCSL_GoodsEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CCSL_GoodsEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCSL_GoodsEditorDoc* CCSL_GoodsEditorView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCSL_GoodsEditorDoc)));
	return (CCSL_GoodsEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CCSL_GoodsEditorView 消息处理程序
