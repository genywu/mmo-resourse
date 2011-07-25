
// MsScriptEditorDoc.h : CMsScriptEditorDoc 类的接口
//


#pragma once


class CMsScriptEditorDoc : public CDocument
{
protected: // 仅从序列化创建
	CMsScriptEditorDoc();
	DECLARE_DYNCREATE(CMsScriptEditorDoc)

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

    static void LUACALL LuaHookCallBackFun(lua_State *L, lua_Debug *ar);

// 实现
public:
	virtual ~CMsScriptEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    TCHAR*  GetTextBuff(VOID);

    INT     MessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType);

protected:

    CMsLuaManager*  m_pCMsLuaManager;
    TCHAR           m_szText[MAX_PATH];

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

	// 生成的 OLE 调度映射函数

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};


