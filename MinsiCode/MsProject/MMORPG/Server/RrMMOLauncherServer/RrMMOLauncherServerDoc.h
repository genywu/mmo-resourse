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

// RrMMOLauncherServerDoc.h : CRrMMOLauncherServerDoc 类的接口
//


#pragma once


class CRrMMOLauncherServerDoc : public CDocument
{
protected: // 仅从序列化创建
    CRrMMOLauncherServerDoc();
    DECLARE_DYNCREATE(CRrMMOLauncherServerDoc)

// 属性
public:

// 操作
public:

// 重写
public:
    virtual BOOL OnNewDocument();
    virtual VOID Serialize(CArchive& ar);

// 实现
public:
    virtual ~CRrMMOLauncherServerDoc();
#ifdef _DEBUG
    virtual VOID AssertValid() CONST;
    virtual VOID Dump(CDumpContext& dc) CONST;
#endif

protected:

// 生成的消息映射函数
protected:
    DECLARE_MESSAGE_MAP()

    // 生成的 OLE 调度映射函数

    DECLARE_DISPATCH_MAP()
    DECLARE_INTERFACE_MAP()
};


