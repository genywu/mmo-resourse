
// MsScriptEditorDoc.cpp : CMsScriptEditorDoc 类的实现
//

#include "stdafx.h"
#include "MsScriptEditor.h"

#include "MsScriptEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMsLuaManager* g_pCMsLuaManager2 = NULL;

void LUACALL CMsScriptEditorDoc::LuaHookCallBackFun(lua_State* L, lua_Debug* ar)
{
    lua_getinfo(L, "Slnu", ar);
    g_pCMsLuaManager2->LoadLuaFile(_T("Script//LuaTest.lua"));
    INT nTop = lua_gettop(L);

    struct
    {
        INT x;
        INT y;
        INT z;
    }a;

    lua_getfield(L, LUA_GLOBALSINDEX, "a");

    lua_getfield(L, -1, "x");
    a.x = lua_tointeger(L, nTop+2);

    lua_getfield(L, -2, "y");
    a.y = lua_tointeger(L, nTop+3);

    lua_getfield(L, -3, "z");
    a.z = lua_tointeger(L, nTop+4);

    a.x++;
    a.y++;
    a.z++;

    lua_getfield(L, LUA_GLOBALSINDEX, "a");
    
    lua_pushinteger(L, a.x);
    lua_setfield(L, -2, "x");

    lua_pushinteger(L, a.y);
    lua_setfield(L, -2, "y");

    lua_pushinteger(L, a.z);
    lua_setfield(L, -2, "z");

    lua_settop(L, nTop);

    //lua_getstack(L, 0, ar);
    lua_getinfo(L, "Slnu", ar);

    //lua_getfield(L, LUA_REGISTRYINDEX, "Lua_Test");
    //lua_getstack(L, 0, ar);
    //lua_getinfo(L, "n", ar);

    //lua_getfield(L, lua_upvalueindex(0), "Lua_Test");
    //lua_getstack(L, 0, ar);
    //lua_getinfo(L, "n", ar);

    //lua_getfield(L, lua_upvalueindex(0), "");
    //lua_getstack(L, 0, ar);

    //lua_getfield(L, LUA_ENVIRONINDEX, "3");
    //lua_getinfo(L, ">Snluf", ar);

    //lua_getfield(L, LUA_GLOBALSINDEX, "0");
    //lua_getinfo(L, ">Snluf", ar);
}


INT LUACALL CMsScriptEditorDoc_MessageBox(lua_State *L)
{
    INT nNumber = lua_tointeger(L, 1);
    CONST CHAR* lpString = lua_tostring(L, 2);
    CMsScriptEditorDoc* lpCMsScriptEditorDoc = (CMsScriptEditorDoc*)nNumber;
    TCHAR* lpTextBuff = lpCMsScriptEditorDoc->GetTextBuff();
    ACharToTChar(lpString, lpTextBuff, MAX_PATH);
    lpCMsScriptEditorDoc->MessageBox(lpTextBuff, _T("文档"), MB_OK);
    return 0;
}

// CMsScriptEditorDoc

IMPLEMENT_DYNCREATE(CMsScriptEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMsScriptEditorDoc, CDocument)
	ON_COMMAND(ID_FILE_SEND_MAIL, &CMsScriptEditorDoc::OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, &CMsScriptEditorDoc::OnUpdateFileSendMail)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMsScriptEditorDoc, CDocument)
END_DISPATCH_MAP()

// 注意: 我们添加了对 IID_IMsScriptEditor 的支持
//  以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {BB07F553-F611-44E1-ACEA-17EB2A472FC9}
static const IID IID_IMsScriptEditor =
{ 0xBB07F553, 0xF611, 0x44E1, { 0xAC, 0xEA, 0x17, 0xEB, 0x2A, 0x47, 0x2F, 0xC9 } };

BEGIN_INTERFACE_MAP(CMsScriptEditorDoc, CDocument)
	INTERFACE_PART(CMsScriptEditorDoc, IID_IMsScriptEditor, Dispatch)
END_INTERFACE_MAP()


// CMsScriptEditorDoc 构造/析构

CMsScriptEditorDoc::CMsScriptEditorDoc()
{
	// TODO: 在此添加一次性构造代码
    ZeroMemoryArray(m_szText);

    m_pCMsLuaManager = NEW CMsLuaManager;
    AssertEx(AL_NORMAL, m_pCMsLuaManager, _T("内存不足"));

    g_pCMsLuaManager2 = m_pCMsLuaManager;

    m_pCMsLuaManager->Init();
    m_pCMsLuaManager->SetLuaFileRootPath();

    luaL_Reg lr = {"Doc_MessageBox", CMsScriptEditorDoc_MessageBox};
    m_pCMsLuaManager->RegisterCFunToLua(lr);

    m_pCMsLuaManager->LoadLuaFile(_T("Script//LuaTest.lua"));


    m_pCMsLuaManager->EnableDebug(TRUE, LuaHookCallBackFun, LUA_MASKLINE);
    FOR_x_TO(i, 1000)
    {
        LUA_FUN_RET_VALUE lv(LUA_RET_VOID);
        m_pCMsLuaManager->RunLuaFun(&lv, 1, _T("Lua_Test"), _T("%d"), this);
        m_pCMsLuaManager->EnableDebug(TRUE, LuaHookCallBackFun, LUA_MASKLINE);
    }

	EnableAutomation();

	AfxOleLockApp();
}

CMsScriptEditorDoc::~CMsScriptEditorDoc()
{
    SAFE_DELETE(m_pCMsLuaManager);
	AfxOleUnlockApp();
}

BOOL CMsScriptEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CMsScriptEditorDoc 序列化

void CMsScriptEditorDoc::Serialize(CArchive& ar)
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


// CMsScriptEditorDoc 诊断

#ifdef _DEBUG
void CMsScriptEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMsScriptEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMsScriptEditorDoc 命令

TCHAR* CMsScriptEditorDoc::GetTextBuff(VOID)
{
    return m_szText;
}

INT CMsScriptEditorDoc::MessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType)
{
    return this->GetRoutingFrame()->MessageBox(lpszText, lpszCaption, nType);
}
