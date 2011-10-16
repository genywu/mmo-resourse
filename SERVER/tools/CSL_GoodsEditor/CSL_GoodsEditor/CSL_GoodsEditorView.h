// CSL_GoodsEditorView.h : CCSL_GoodsEditorView 类的接口
//


#pragma once


class CCSL_GoodsEditorView : public CView
{
protected: // 仅从序列化创建
	CCSL_GoodsEditorView();
	DECLARE_DYNCREATE(CCSL_GoodsEditorView)

// 属性
public:
	CCSL_GoodsEditorDoc* GetDocument() const;

// 操作
public:

// 重写
	public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CCSL_GoodsEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // CSL_GoodsEditorView.cpp 的调试版本
inline CCSL_GoodsEditorDoc* CCSL_GoodsEditorView::GetDocument() const
   { return reinterpret_cast<CCSL_GoodsEditorDoc*>(m_pDocument); }
#endif

