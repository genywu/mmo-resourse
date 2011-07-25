================================================================================
    MICROSOFT 基础类库: RrMMOSimpleClient 项目概述
===============================================================================

应用程序向导已为您创建此 RrMMOSimpleClient 应用程序。此应用程序不仅演示使用 Microsoft 基础类的基本知识，而且可作为编写应用程序的起点。

此文件包含组成 RrMMOSimpleClient 应用程序的各个文件的内容摘要。

RrMMOSimpleClient.vcproj
    这是使用应用程序向导生成的 VC++ 项目的主项目文件。
    它包含有关生成文件的 Visual C++ 版本的信息，以及有关使用应用程序向导选择的平台、配置和项目功能的信息。

RrMMOSimpleClient.h
    这是应用程序的主要头文件。它包括其他项目特定的头文件(包括 Resource.h)，并声明 CRrMMOSimpleClientApp 应用程序类。

RrMMOSimpleClient.cpp
    这是包含应用程序类 CRrMMOSimpleClientApp 的主要应用程序源文件。

RrMMOSimpleClient.rc
    这是程序使用的所有 Microsoft Windows 资源的列表。它包括 RES 子目录中存储的图标、位图和光标。此文件可以直接在 Microsoft Visual C++ 中进行编辑。项目资源位于 2052 中。

res\RrMMOSimpleClient.ico
    这是用作应用程序图标的图标文件。此图标包括在主要资源文件 RrMMOSimpleClient.rc 中。

res\RrMMOSimpleClient.rc2
    此文件包含不是由 Microsoft Visual C++ 编辑的资源。您应该将不可由资源编辑器编辑的所有资源放在此文件中。

RrMMOSimpleClient.reg
    这是一个示例 .reg 文件，它显示了框架将为您设置的注册设置的种类。可以将它用作
    将与您的应用程序一起使用的 .reg 文件，或者索性将它删除，而采用默认的 RegisterShellFileTypes 注册。

RrMMOSimpleClient.idl
    此文件包含应用程序类型库的接口描述语言源代码。

/////////////////////////////////////////////////////////////////////////////

对于主框架窗口:
    项目包含标准 MFC 界面。

MainFrm.h, MainFrm.cpp
    这些文件包含框架类 CMainFrame，该类派生自
    CMDIFrameWnd 并控制所有 MDI 框架功能。

res\Toolbar.bmp
    此位图文件用于为工具栏创建平铺图像。
    初始工具栏和状态栏是在 CMainFrame 类中构造的。使用资源编辑器编辑此工具栏位图，并更新 RrMMOSimpleClient.rc 中的 IDR_MAINFRAME TOOLBAR 数组以添加工具栏按钮。
/////////////////////////////////////////////////////////////////////////////

对于子框架窗口:

ChildFrm.h, ChildFrm.cpp
    这些文件定义并实现 CChildFrame 类，该类支持 MDI 应用程序中的子窗口。

/////////////////////////////////////////////////////////////////////////////

应用程序向导创建一个文档类型和一个视图:

RrMMOSimpleClientDoc.h，RrMMOSimpleClientDoc.cpp - 文档
    这些文件包含 CRrMMOSimpleClientDoc 类。编辑这些文件可以添加特殊文档数据并可实现文件保存和加载(通过 CRrMMOSimpleClientDoc::Serialize)。

RrMMOSimpleClientView.h，RrMMOSimpleClientView.cpp - 文档的视图
    这些文件包含 CRrMMOSimpleClientView 类。
    CRrMMOSimpleClientView 对象用于查看 CRrMMOSimpleClientDoc 对象。

res\RrMMOSimpleClientDoc.ico
    这是图标文件，它用作 CRrMMOSimpleClientDoc 类的 MDI 子窗口的图标。此图标包括在主要资源文件 RrMMOSimpleClient.rc 中。




/////////////////////////////////////////////////////////////////////////////

其他功能:

ActiveX 控件
    应用程序包括对使用 ActiveX 控件的支持。

打印及打印预览支持
    应用程序向导已通过从 MFC 库调用 CView 类中的成员函数，生成了用于处理打印、打印设置和打印预览命令的代码。

MAPI 支持
    生成的项目包含创建、操作、传输和存储邮件所需的代码。

Windows Sockets
    应用程序支持通过 TCP/IP 网络建立通信。

/////////////////////////////////////////////////////////////////////////////

其他标准文件:

StdAfx.h, StdAfx.cpp
    这些文件用于生成名为 RrMMOSimpleClient.pch 的预编译头(PCH)文件和名为 StdAfx.obj 的预编译类型文件。

Resource.h
    这是标准头文件，它定义新资源 ID。
    Microsoft Visual C++ 将读取并更新此文件。

/////////////////////////////////////////////////////////////////////////////

其他注释:

应用程序向导使用“TODO:”指示应添加或自定义的源代码部分。

如果应用程序在共享 DLL 中使用 MFC,则将需要重新发布 MFC DLL。如果应用程序所用与操作系统的区域设置不同，则也将必须重新发布对应的本地化资源 MFC90XXX.DLL。
有关这两个主题的详细信息，请参阅 MSDN 文档中有关重新发布 Visual C++ 应用程序的部分。

/////////////////////////////////////////////////////////////////////////////