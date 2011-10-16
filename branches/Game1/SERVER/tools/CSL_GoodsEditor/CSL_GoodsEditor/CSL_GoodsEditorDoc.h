// CSL_GoodsEditorDoc.h :  CCSL_GoodsEditorDoc 类的接口
//


#pragma once

class CCSL_GoodsEditorDoc : public CDocument
{
protected: // 仅从序列化创建
	CCSL_GoodsEditorDoc();
	DECLARE_DYNCREATE(CCSL_GoodsEditorDoc)

// 属性
public:

// 操作
public:

// 重写
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CCSL_GoodsEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};


