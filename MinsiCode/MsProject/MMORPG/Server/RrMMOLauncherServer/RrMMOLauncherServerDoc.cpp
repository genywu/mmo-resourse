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

// RrMMOLauncherServerDoc.cpp : CRrMMOLauncherServerDoc 类的实现
//

#include "Precompiled.h"
#include "RrMMOLauncherServer.h"

#include "RrMMOLauncherServerDoc.h"

// CRrMMOLauncherServerDoc

IMPLEMENT_DYNCREATE(CRrMMOLauncherServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CRrMMOLauncherServerDoc, CDocument)
    ON_COMMAND(ID_FILE_SEND_MAIL, &CRrMMOLauncherServerDoc::OnFileSendMail)
    ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, &CRrMMOLauncherServerDoc::OnUpdateFileSendMail)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CRrMMOLauncherServerDoc, CDocument)
END_DISPATCH_MAP()

// 注意: 我们添加了对 IID_IRrMMOLauncherServer 的支持
//  以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {16F8DBF5-6632-43DD-9834-F96AD0BDC911}
static CONST IID IID_IRrMMOLauncherServer =
{ 0x16F8DBF5, 0x6632, 0x43DD, { 0x98, 0x34, 0xF9, 0x6A, 0xD0, 0xBD, 0xC9, 0x11 } };

BEGIN_INTERFACE_MAP(CRrMMOLauncherServerDoc, CDocument)
    INTERFACE_PART(CRrMMOLauncherServerDoc, IID_IRrMMOLauncherServer, Dispatch)
END_INTERFACE_MAP()


// CRrMMOLauncherServerDoc 构造/析构

CRrMMOLauncherServerDoc::CRrMMOLauncherServerDoc()
{
    // TODO: 在此添加一次性构造代码

    EnableAutomation();

    AfxOleLockApp();
}

CRrMMOLauncherServerDoc::~CRrMMOLauncherServerDoc()
{
    AfxOleUnlockApp();
}

BOOL CRrMMOLauncherServerDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO: 在此添加重新初始化代码
    // (SDI 文档将重用该文档)

    return TRUE;
}




// CRrMMOLauncherServerDoc 序列化

VOID CRrMMOLauncherServerDoc::Serialize(CArchive& ar)
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


// CRrMMOLauncherServerDoc 诊断

#ifdef _DEBUG
VOID CRrMMOLauncherServerDoc::AssertValid() CONST
{
    CDocument::AssertValid();
}

VOID CRrMMOLauncherServerDoc::Dump(CDumpContext& dc) CONST
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


// CRrMMOLauncherServerDoc 命令
