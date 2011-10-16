// CSL_GoodsEditorDoc.cpp :  CCSL_GoodsEditorDoc 类的实现
//

#include "stdafx.h"
#include "CSL_GoodsEditor.h"

#include "CSL_GoodsEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCSL_GoodsEditorDoc

IMPLEMENT_DYNCREATE(CCSL_GoodsEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CCSL_GoodsEditorDoc, CDocument)
END_MESSAGE_MAP()


// CCSL_GoodsEditorDoc 构造/析构

CCSL_GoodsEditorDoc::CCSL_GoodsEditorDoc()
{
	// TODO: 在此添加一次性构造代码

}

CCSL_GoodsEditorDoc::~CCSL_GoodsEditorDoc()
{
}

BOOL CCSL_GoodsEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CCSL_GoodsEditorDoc 序列化

void CCSL_GoodsEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CCSL_GoodsEditorDoc 诊断

#ifdef _DEBUG
void CCSL_GoodsEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCSL_GoodsEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCSL_GoodsEditorDoc 命令
