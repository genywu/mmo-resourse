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

// ChildFrm.cpp : CChildFrame 类的实现
//

#include "Precompiled.h"
#include "RrMMOLauncherServer.h"

#include "ChildFrm.h"

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
    ON_COMMAND(ID_FILE_PRINT, &CChildFrame::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CChildFrame::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CChildFrame::OnFilePrintPreview)
    ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CChildFrame::OnUpdateFilePrintPreview)
END_MESSAGE_MAP()

// CChildFrame 构造/析构

CChildFrame::CChildFrame()
{
    // TODO: 在此添加成员初始化代码
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
    if( !CMDIChildWndEx::PreCreateWindow(cs) )
        return FALSE;

    return TRUE;
}

// CChildFrame 诊断

#ifdef _DEBUG
VOID CChildFrame::AssertValid() CONST
{
    CMDIChildWndEx::AssertValid();
}

VOID CChildFrame::Dump(CDumpContext& dc) CONST
{
    CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 消息处理程序

VOID CChildFrame::OnFilePrint()
{
    if (m_dockManager.IsPrintPreviewValid())
    {
        PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
    }
}

VOID CChildFrame::OnFilePrintPreview()
{
    if (m_dockManager.IsPrintPreviewValid())
    {
        PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // 强制关闭“打印预览”模式
    }
}

VOID CChildFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_dockManager.IsPrintPreviewValid());
}
