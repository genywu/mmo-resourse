// YaYaDiskDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "YaYaDisk.h"
#include "YaYaDiskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CYaYaDiskDlg 对话框




CYaYaDiskDlg::CYaYaDiskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CYaYaDiskDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CYaYaDiskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CYaYaDiskDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    //ON_BN_CLICKED(IDOK, &CYaYaDiskDlg::OnBnClickedOk)
    ON_STN_CLICKED(IDC_QQ, &CYaYaDiskDlg::OnStnClickedQq)
    ON_STN_CLICKED(IDC_BEACH, &CYaYaDiskDlg::OnStnClickedBeach)
    ON_STN_CLICKED(IDC_WIND, &CYaYaDiskDlg::OnStnClickedWind)
    ON_STN_CLICKED(IDC_QQGAME, &CYaYaDiskDlg::OnStnClickedQqgame)
    ON_STN_CLICKED(IDC_QQ_DANCE, &CYaYaDiskDlg::OnStnClickedQqDance)
    ON_STN_CLICKED(IDC_EXIT, &CYaYaDiskDlg::OnStnClickedExit)
    ON_STN_CLICKED(IDC_SMALL, &CYaYaDiskDlg::OnStnClickedSmall)
END_MESSAGE_MAP()


// CYaYaDiskDlg 消息处理程序

BOOL CYaYaDiskDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CYaYaDiskDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CYaYaDiskDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//void CYaYaDiskDlg::OnBnClickedOk()
//{
//    //SECURITY_DESCRIPTOR sd;
//    //PACL                pAcl;
//    //DWORD               cbSid = 96;
//    //SID                 Sid;
//    //TCHAR               RefDomain[DNLEN + 1];
//    //DWORD               cchDomain = DNLEN + 1;
//    //DWORD               dwAclSize = sizeof(ACL) + 1 * (sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD)) + sizeof(Sid);
//    //SID_NAME_USE        Use;
//    //::LookupAccountName(
//    //    NULL,           //   [in]       这个参数指明查找的用户或组在哪个系统上,为NULL表示本地系统
//    //    _T("Everyone"), //   [in]       欲授予访问权限的用户或组
//    //    &Sid,           //   [out]      存放返回的SID值
//    //    &cbSid,         //   [in,out]   进去的是你设定的缓冲区长度,出来的是实际SID的长度
//    //    RefDomain,      //   [out]      域名
//    //    &cchDomain,     //   [in,out]   长度
//    //    &Use);          //   [out]      结构,用来指示用户的类型
//
//    //pAcl = (PACL)::HeapAlloc(::GetProcessHeap(), 0, dwAclSize);
//
//    //::InitializeAcl(pAcl, dwAclSize, ACL_REVISION);
//    //::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
//    //::AddAccessAllowedAce(pAcl, ACL_REVISION, GENERIC_ALL, &Sid);
//    ////::AddAccessDeniedAce(pAcl, ACL_REVISION, GENERIC_READ, &Sid);
//    //::SetSecurityDescriptorDacl(&sd, TRUE, pAcl, FALSE);
//    //::SetFileSecurity(_T("K:\\*"), DACL_SECURITY_INFORMATION, &sd);
//
//    HANDLE hPtoken;
//    if (::LogonUser(_T("Minsi"), _T("RrServer\\"), _T("XJBCEMLSPD"), LOGON32_LOGON_NETWORK_CLEARTEXT, LOGON32_PROVIDER_DEFAULT, &hPtoken))
//    {
//        AfxMessageBox(_T("登陆成功"));
//    }
//    else
//    {
//        INT a = ::GetLastError();
//        AfxMessageBox(_T("登陆失败"));
//    }
//    //::CreateProcessAsUser(hPtoken, _T("H:\\暴风影音\\Storm.exe"), _T(""), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
//
//    //PSID    pSid = NULL;                // 定义一个指向SID结构的指针
//    //DWORD   cbSid;                      // 存放SID的长度
//    //TCHAR   RefDomain[DNLEN + 1];       // 一个缓冲区用来存放所在域的名称
//    //DWORD   cchDomain = DNLEN + 1;      // 可以存入缓冲区的字符的个数
//    //SID_NAME_USE    peUse;              // 一个枚举类型,用来指出SID的类型
//    //SECURITY_DESCRIPTOR sd;             // 用来描述安全属性的结构
//    //PACL    pDacl = NULL;               // 指向权限控制列表的指针
//    //DWORD   dwAclSize;                  // 列表的长度
//
//    //cbSid   = 96;                       // 设定SID的长度为96
//    //pSid    = (PSID)::HeapAlloc(::GetProcessHeap(), 0, cbSid);    // 为SID分配空间
//    //if(pSid == NULL)                    // 错误处理
//    //{     
//    //    AfxMessageBox(_T("HeapAlloc Error!\n"));
//    //}                 
//
//    //// 得到可以访问此资源的用户或组的SID
//    //if(!::LookupAccountName(
//    //    NULL,           //   [in]       这个参数指明查找的用户或组在哪个系统上,为NULL表示本地系统
//    //    _T("Everyone"), //   [in]       欲授予访问权限的用户或组
//    //    pSid,           //   [out]      存放返回的SID值
//    //    &cbSid,         //   [in,out]   进去的是你设定的缓冲区长度,出来的是实际SID的长度
//    //    RefDomain,      //   [out]      域名
//    //    &cchDomain,     //   [in,out]   长度
//    //    &peUse))        //   [out]      结构,用来指示用户的类型
//    //{
//    //    if(::GetLastError() == ERROR_INSUFFICIENT_BUFFER)   // 如果缓冲区不足
//    //    {                                 
//    //        pSid = (PSID)::HeapReAlloc(::GetProcessHeap(), 0, pSid, cbSid);
//    //        if(pSid == NULL)
//    //        {
//    //            AfxMessageBox(_T("HeapAlloc Error!\n"));
//    //        }
//    //        cchDomain = DNLEN + 1;
//    //        if(!::LookupAccountName(
//    //            NULL,
//    //            _T("Everyone"),
//    //            pSid,
//    //            &cbSid,
//    //            RefDomain,
//    //            &cchDomain,
//    //            &peUse))
//    //        {
//    //            AfxMessageBox(_T("LookupAccountName Error! (rc=%lu)\n"), ::GetLastError());
//    //        }
//    //    }
//    //    else
//    //    {
//    //        AfxMessageBox(_T("LookupAccountName Error! (rc=%lu)\n"), ::GetLastError());
//    //    }
//    //}
//
//    //// 计算权限控制列表所需长度
//    //dwAclSize = sizeof(ACL) + 1 * (sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD)) + ::GetLengthSid(pSid);
//
//    //// 为pDacl分配所需的空间
//    //pDacl = (PACL)::HeapAlloc(::GetProcessHeap(), 0, dwAclSize);
//    //if(pDacl == NULL) return;
//
//    //// 将pDacl初始化
//    //::InitializeAcl(pDacl, dwAclSize, ACL_REVISION);
//
//    //// 授予GENERIC_ALL权限授于得到的SID,并加入到列表中
//    //::AddAccessAllowedAce(pDacl, ACL_REVISION, GENERIC_ALL, pSid);
//
//    //// 初始化 SECURITY_DESCRIPTOR 结构的实例sd
//    //::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
//
//    //// 将pDacl加入到sd中     
//    //::SetSecurityDescriptorDacl(&sd, TRUE, pDacl, FALSE);       
//}

void CYaYaDiskDlg::OnStnClickedQq()
{
    //TCHAR szDirBuff[MAX_PATH] = {0};
    //::GetCurrentDirectory(MAX_PATH, szDirBuff);
    STARTUPINFOW StartInfo;
    ZeroMemory(&StartInfo, sizeof(StartInfo));
    StartInfo.cb = sizeof(StartInfo);
    PROCESS_INFORMATION ProcessInfo;  // 进程信息
    //_tcscat_s(szDirBuff, _T("腾讯\\QQ\\Bin\\QQ.exe"));
    ::CreateProcess(_T(".\\腾讯\\QQ\\Bin\\QQ.exe"), _T(""), NULL, NULL, FALSE, 0, NULL, NULL, &StartInfo, &ProcessInfo);
    this->ShowWindow(SW_MINIMIZE);
}

void CYaYaDiskDlg::OnStnClickedBeach()
{
    STARTUPINFOW StartInfo;
    ZeroMemory(&StartInfo, sizeof(StartInfo));
    StartInfo.cb = sizeof(StartInfo);
    PROCESS_INFORMATION ProcessInfo;
    ::CreateProcess(_T(".\\性感海滩3\\SexyBeach3PlusCN.exe"), _T(""), NULL, NULL, FALSE, 0, NULL, NULL, &StartInfo, &ProcessInfo);
    this->ShowWindow(SW_MINIMIZE);
}

void CYaYaDiskDlg::OnStnClickedWind()
{
    STARTUPINFOW StartInfo;
    ZeroMemory(&StartInfo, sizeof(StartInfo));
    StartInfo.cb = sizeof(StartInfo);
    PROCESS_INFORMATION ProcessInfo;
    ::CreateProcess(_T(".\\暴风影音\\Storm.exe"), _T(""), NULL, NULL, FALSE, 0, NULL, NULL, &StartInfo, &ProcessInfo);
    this->ShowWindow(SW_MINIMIZE);
}

void CYaYaDiskDlg::OnStnClickedQqgame()
{
    STARTUPINFOW StartInfo;
    ZeroMemory(&StartInfo, sizeof(StartInfo));
    StartInfo.cb = sizeof(StartInfo);
    PROCESS_INFORMATION ProcessInfo;
    ::CreateProcess(_T(".\\腾讯\\QQGAME\\QQGame.exe"), _T(""), NULL, NULL, FALSE, 0, NULL, NULL, &StartInfo, &ProcessInfo);
    this->ShowWindow(SW_MINIMIZE);
}

void CYaYaDiskDlg::OnStnClickedQqDance()
{
    STARTUPINFOW StartInfo;
    ZeroMemory(&StartInfo, sizeof(StartInfo));
    StartInfo.cb = sizeof(StartInfo);
    PROCESS_INFORMATION ProcessInfo;
    ::CreateProcess(_T(".\\腾讯\\QQ炫舞\\bin\\QQ炫舞.exe"), _T(""), NULL, NULL, FALSE, 0, NULL, NULL, &StartInfo, &ProcessInfo);
    this->ShowWindow(SW_MINIMIZE);
}

void CYaYaDiskDlg::OnStnClickedExit()
{
    this->EndDialog(0);
}

void CYaYaDiskDlg::OnStnClickedSmall()
{
    this->ShowWindow(SW_MINIMIZE);
}
