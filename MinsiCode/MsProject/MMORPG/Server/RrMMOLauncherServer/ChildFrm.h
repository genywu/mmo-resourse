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

// ChildFrm.h : CChildFrame 类的接口
//

#pragma once

class CChildFrame : public CMDIChildWndEx
{
    DECLARE_DYNCREATE(CChildFrame)
public:
    CChildFrame();

// 属性
public:

// 操作
public:

// 重写
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
    virtual ~CChildFrame();
#ifdef _DEBUG
    virtual VOID AssertValid() CONST;
    virtual VOID Dump(CDumpContext& dc) CONST;
#endif

// 生成的消息映射函数
protected:
    afx_msg VOID OnFilePrint();
    afx_msg VOID OnFilePrintPreview();
    afx_msg VOID OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
    DECLARE_MESSAGE_MAP()
};
