// PatchDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Patch.h"
#include "PatchDlg.h"

#include "../../PUBLIC/ClientResource.h"
#include "../../PUBLIC/Package.h"
#include "../../PUBLIC/FilesInfo.h"
#include "../../PUBLIC/ziplib/zlib.h"
#include "../../PUBLIC/ziplib/unzip.h"
#include ".\patchdlg.h"
#include "../../public/tools.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DISPLAY_UPDATE	WM_USER+6000

void PutDebugStringMsg(const char *msg)
{
	AfxMessageBox(msg);
}

struct tagDisplayInfo
{
	CString	strText;		//文本显示值
	int		nMax;			//进度条最大值
	int		nCur;			//进度条当前值
};
tagDisplayInfo	gDisplayInfo;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
HANDLE hThread=0;
bool bExit = false;

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPatchDlg 对话框



CPatchDlg::CPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchDlg::IDD, pParent)
	,m_pPatchFilesInfo(NULL)
	,m_pPatchPackage(NULL)
	,m_pClientResource(NULL)
	, m_strWorkPath("")
	, m_strGameRunFolder(_T(""))
	, m_strDisplayInfo(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CPatchDlg::~CPatchDlg()
{
	if(m_pPatchFilesInfo)	delete m_pPatchFilesInfo;
	if(m_pPatchPackage) delete m_pPatchPackage;
	if(m_pClientResource) delete m_pClientResource;

#ifndef	_DEBUG
	DeleteFile(m_strTemptPackageName.c_str());
	DeleteFile(m_strTemptFilesInfoName.c_str());
	::RemoveDirectory(m_strExePath.c_str());
#endif
}

void CPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//	DDX_Text(pDX, IDC_EDIT_GAMERUNFOLDER, m_strGameRunFolder);
	DDX_Text(pDX, IDC_STATIC_TEXTINFO, m_strDisplayInfo);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_OUTPUTINFO, m_OutPutInfoEdit);
}

BEGIN_MESSAGE_MAP(CPatchDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDC_BUTTON_SELECTFOLDER, OnBnClickedButtonSelectfolder)
//	ON_EN_CHANGE(IDC_EDIT_GAMERUNFOLDER, OnEnChangeEditGamerunfolder)
	ON_BN_CLICKED(ID_UPDATE, OnBnClickedUpdate)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_STN_CLICKED(IDC_STATIC_TEXTINFO, &CPatchDlg::OnStnClickedStaticTextinfo)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CPatchDlg 消息处理程序

BOOL CPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//初始相关信息
	if(FALSE == InitialInfo())
		return FALSE;

	if(CheckGameRunFolder())
	{
		GetDlgItem(ID_UPDATE)->EnableWindow(TRUE);
		m_strGameRunFolder= CString(m_strWorkPath.c_str());
	}
	else
	{
		GetDlgItem(ID_UPDATE)->EnableWindow(FALSE);
		m_strWorkPath="";
	}

	char strTitle[200];
	sprintf(strTitle,"升级游戏");

	SetWindowText(strTitle);

	UpdateData(false);


	HGLOBAL  hResLoad;     // handle to loaded resource 
	HRSRC hRes;         // handle/ptr. to res. info. in hExe 
	VOID *lpResLock;    // pointer to resource data 

	//#ifndef _DEBUG
	//得到包文件数据
	hRes = ::FindResource(NULL,MAKEINTRESOURCE(IDR_PATCH1),"PATCH");
	if( hRes == NULL ) 
	{ 
		DWORD dwError = GetLastError();
		return FALSE; 
	} 

	hResLoad = ::LoadResource(NULL, hRes); 
	if (hResLoad == NULL) 
	{ 
		return FALSE;
	} 

	// Lock the dialog box into global memory. 
	lpResLock = ::LockResource(hResLoad); 
	if (lpResLock == NULL) 
	{ 
		return FALSE;
	} 

	string tmpZipFileNmae = m_strWorkPath + "\\updatefiles\\tmp.zip";
	FILE* pf = fopen(tmpZipFileNmae.c_str(),"wb");
	if(!pf)
	{	
		char str[100]="";
		sprintf(str,"初始化对话框创建临时ZIP文件 %s 出错。",tmpZipFileNmae.c_str());
		AfxMessageBox(str);
		return FALSE;
	}	

	DWORD dwSize = SizeofResource(NULL,hRes);
	DWORD dwCount = fwrite(lpResLock,dwSize,1, pf);
	if(dwCount != 1)
	{
		char str[100]="";
		sprintf(str,"初始化对话框写临时ZIP文件 %s 数据出错，请重试。",tmpZipFileNmae.c_str());
		AfxMessageBox(str);
		return FALSE;
	}

	int nRet = fflush(pf);
	if(nRet != 0)
	{
		char str[100]="";
		sprintf(str,"初始化对话框创建临时ZIP文件 %s 出错，请重试。",tmpZipFileNmae.c_str());
		AfxMessageBox(str);
		return FALSE;
	}
	fclose(pf);

	//解压缩数据
	unzFile dataZipFile = unzOpen(tmpZipFileNmae.c_str());
	if(dataZipFile)
	{
		unz_file_info zipFileInfo;
		// 找到Update.html
		if( UNZ_OK == unzLocateFile(dataZipFile, "Update.txt", 1) )
		{
			unzGetCurrentFileInfo(dataZipFile, &zipFileInfo, NULL, 0,
				NULL, 0, NULL, 0);

			string tmpUpdateTxtPath = m_strWorkPath + "\\updatefiles\\Update.txt";

			char* buf = new char[zipFileInfo.uncompressed_size+1];
			unzOpenCurrentFile(dataZipFile);
			unzReadCurrentFile(dataZipFile, buf, zipFileInfo.uncompressed_size);

			FILE* file = fopen(tmpUpdateTxtPath.c_str(), "wb");
			if(file)
			{
				fwrite(buf, zipFileInfo.uncompressed_size, 1, file);
				buf[zipFileInfo.uncompressed_size] = '\0';
				fclose(file);
			}
			unzCloseCurrentFile(dataZipFile);
			m_OutPutInfoEdit.SetWindowText(buf);
			SAFE_DELETE_ARRAY(buf);
		}
	}
	unzClose(dataZipFile);

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CPatchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam); 
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPatchDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
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
		CPaintDC dc(this); // device context for painting
		// TODO: 在此处添加消息处理程序代码
		// 不为绘图消息调用 CDialog::OnPaint()

		CRect rect;
		GetClientRect(&rect);

		//CDC memDC;
		//memDC.CreateCompatibleDC(&dc);
		//CBitmap bpt;
		//bpt.LoadBitmap(IDB_BITMAP1);
		//BITMAP bm;
		//bpt.GetBitmap(&bm);
		//memDC.SelectObject(&bpt);
		//dc.BitBlt(10,10, bm.bmWidth , bm.bmHeight , &memDC, 0, 0, SRCCOPY);
		//创建新字体

		//CFont myFont;
		//myFont.CreatePointFont(350,"宋体");

		//CFont *OldFont = dc.SelectObject(&myFont);
		//dc.SetTextColor(RGB(255,0,0));

		////创建CHECT说明的矩形窗口
		//CRect myRect;
		//myRect.left=rect.left+80;
		//myRect.top=rect.top+10;
		//myRect.right=rect.right-10;
		//myRect.bottom=rect.bottom/3;

		////采用透明的方式在屏幕上写字
		//dc.SetBkMode(TRANSPARENT);
		//dc.DrawText("锦  天  科  技",&myRect,DT_LEFT);

		//dc.SelectObject(OldFont);
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CPatchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//初始化客户端资源信息
BOOL	CPatchDlg::InitialInfo()
{
	// 查找客户端路径,注册表
	//注册表信息
	string strReg("HKEY_LOCAL_MACHINE/SOFTWARE/xingman/Luvinia");

	HKEY hKey;
	DWORD dwBufLen =2*MAX_PATH+10;
	char szKeyValue[2*MAX_PATH+10];
	memset(szKeyValue, 0, 2*MAX_PATH+10);

	LONG lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		"SOFTWARE\\XINGMAN\\LUVINIA",
		0, KEY_QUERY_VALUE, &hKey );


	lRet = RegQueryValueEx( hKey, "Client_Path", NULL, NULL,
		(LPBYTE)szKeyValue, &dwBufLen);

	RegCloseKey( hKey );

	if(szKeyValue[0] != '\0')
	{
		m_strExePath = string(szKeyValue);
		m_strWorkPath = m_strExePath;
	}

	bExit = false;

	string strGameExeName = m_strWorkPath + "\\PackRes.cfg";
	
	// 查找GameClient.exe
	WIN32_FIND_DATA stFindData;
	HANDLE hFind = FindFirstFile(strGameExeName.c_str(), &stFindData);
	if (hFind==INVALID_HANDLE_VALUE)
	{
		CHAR* strPath = new CHAR[MAX_PATH];
		::GetSystemDirectory(strPath,MAX_PATH);
		string strTemptr(strPath);
		SAFE_DELETE_ARRAY(strPath);

		string strTemptFoler;strTemptFoler.clear();
		strTemptFoler.insert(0,strTemptr,0,2);

		////#ifdef	_DEBUG
		CHAR* strWorkPath = new CHAR[MAX_PATH];
		if(::GetCurrentDirectory(MAX_PATH,strWorkPath))
		{
			m_strExePath = string(strWorkPath);
			m_strWorkPath = m_strExePath;
		}
		strGameExeName = m_strWorkPath + "\\PackRes.cfg";
		SAFE_DELETE_ARRAY(strWorkPath);

		HANDLE hFind = FindFirstFile(strGameExeName.c_str(), &stFindData);
		if (hFind==INVALID_HANDLE_VALUE)
		{
			MessageBox("请将更新包放置于游戏根目录下!", "错误!", MB_OK);
			return false;
		}
	}

	string tmpUpdatePath = m_strWorkPath + "\\updatefiles";
	//"gameclient"
	::CreateDirectory(tmpUpdatePath.c_str(), NULL);
	
	m_strFilesInfoName="PackRes.cfg";

	m_strTemptFilesInfoName=m_strExePath+"\\Patch.cfg";	//临时包
	m_strTemptPackageName=m_strExePath+"\\Patch.dp";//临时文件姓名

	return true;
}

BOOL	CPatchDlg::GetPatchData()
{	
	HGLOBAL  hResLoad;     // handle to loaded resource 
	HRSRC hRes;         // handle/ptr. to res. info. in hExe 
	VOID *lpResLock;    // pointer to resource data 

	//#ifndef _DEBUG
	//得到包文件数据
	hRes = ::FindResource(NULL,MAKEINTRESOURCE(IDR_PATCH1),"PATCH");
	if( hRes == NULL ) 
	{ 
		DWORD dwError = GetLastError();
		return FALSE; 
	} 

	hResLoad = ::LoadResource(NULL, hRes); 
	if (hResLoad == NULL) 
	{ 
		return FALSE;
	} 

	// Lock the dialog box into global memory. 
	lpResLock = ::LockResource(hResLoad); 
	if (lpResLock == NULL) 
	{ 
		return FALSE;
	} 

	string tmpZipFileNmae = m_strWorkPath + "\\updatefiles\\tmp.zip";
	FILE* pf = fopen(tmpZipFileNmae.c_str(),"wb");
	if(!pf)
	{	
		char str[100]="";
		sprintf(str,"创建临时ZIP文件 %s 出错。",tmpZipFileNmae.c_str());
		AfxMessageBox(str);
		return FALSE;
	}	

	DWORD dwSize = SizeofResource(NULL,hRes);
	DWORD dwCount = fwrite(lpResLock,dwSize,1, pf);
	if(dwCount != 1)
	{
		char str[100]="";
		sprintf(str,"写临时ZIP文件 %s 数据出错，请重试。",tmpZipFileNmae.c_str());
		AfxMessageBox(str);
		return FALSE;
	}

	int nRet = fflush(pf);
	if(nRet != 0)
	{
		char str[100]="";
		sprintf(str,"创建临时ZIP文件 %s 出错，请重试。",tmpZipFileNmae.c_str());
		AfxMessageBox(str);
		return FALSE;
	}
	fclose(pf);
	SetDisplayInfo(this->m_hWnd,"解压得到数据...",16,20);
	PutDebugString("解压得到数据...");

	//解压缩数据
	unzFile dataZipFile = unzOpen(tmpZipFileNmae.c_str());
	if(dataZipFile)
	{
		unz_file_info zipFileInfo;
		//// 找到Update.html
		//if( UNZ_OK == unzLocateFile(dataZipFile, "Update.txt", 1) )
		//{
		//	unzGetCurrentFileInfo(dataZipFile, &zipFileInfo, NULL, 0,
		//		NULL, 0, NULL, 0);

		//	string tmpUpdateTxtPath = m_strWorkPath + "\\updatefiles\\Update.txt";
		//	
		//	char* buf = new char[zipFileInfo.uncompressed_size+1];
		//	unzOpenCurrentFile(dataZipFile);
		//	unzReadCurrentFile(dataZipFile, buf, zipFileInfo.uncompressed_size);
		//	
		//	FILE* file = fopen(tmpUpdateTxtPath.c_str(), "wb");
		//	if(file)
		//	{
		//		fwrite(buf, zipFileInfo.uncompressed_size, 1, file);
		//		buf[zipFileInfo.uncompressed_size] = '\0';
		//		fclose(file);
		//	}
		//	unzCloseCurrentFile(dataZipFile);

		//	FILE* outInfoFile = fopen(tmpUpdateTxtPath.c_str(), "rb");
		//	if(outInfoFile)
		//	{
		//		char fileBuf[5120];
		//		memset(fileBuf, 0, 5120);
		//		fread(fileBuf, 5120, 1, outInfoFile);
		//		SetDlgItemText(IDC_OUTPUTINFO, &fileBuf[0]);
		//	}

		//	SAFE_DELETE_ARRAY(buf);
		//}
		// 找到Patch.cfg
		if( UNZ_OK == unzLocateFile(dataZipFile, "Patch.cfg", 1) )
		{
			unzGetCurrentFileInfo(dataZipFile, &zipFileInfo, NULL, 0,
				NULL, 0, NULL, 0);

			char* buf = new char[zipFileInfo.uncompressed_size];
			unzOpenCurrentFile(dataZipFile);
			unzReadCurrentFile(dataZipFile, buf, zipFileInfo.uncompressed_size);
			string tmpUpdatePath = m_strWorkPath + "\\updatefiles\\Patch.cfg";
			FILE* file = fopen(tmpUpdatePath.c_str(), "wb");
			if(file)
			{
				fwrite(buf, zipFileInfo.uncompressed_size, 1, file);
				fclose(file);
			}
			unzCloseCurrentFile(dataZipFile);
			SAFE_DELETE_ARRAY(buf);
		}

		// 找到Patch.pak
		if( UNZ_OK == unzLocateFile(dataZipFile, "Patch.dp", 1) )
		{
			unzGetCurrentFileInfo(dataZipFile, &zipFileInfo, NULL, 0,
				NULL, 0, NULL, 0);

			char* buf = new char[zipFileInfo.uncompressed_size];
			unzOpenCurrentFile(dataZipFile);
			unzReadCurrentFile(dataZipFile, buf, zipFileInfo.uncompressed_size);
			string tmpUpdatePath = m_strWorkPath + "\\updatefiles\\Patch.dp";
			FILE* file = fopen(tmpUpdatePath.c_str(), "wb");
			if(file)
			{
				fwrite(buf, zipFileInfo.uncompressed_size, 1, file);
				fclose(file);
			}
			unzCloseCurrentFile(dataZipFile);
			SAFE_DELETE_ARRAY(buf);
		}

		if(!m_pPatchFilesInfo)
			m_pPatchFilesInfo = new CFilesInfo;
		m_pPatchFilesInfo->Clear();
		string tmpPatchRilPath = m_strWorkPath + "\\updatefiles\\Patch.cfg";
		m_pPatchFilesInfo->Load(tmpPatchRilPath);

		SetDisplayInfo(this->m_hWnd,"解压得到数据...",19,20);
		PutDebugString("解压得到数据...");

		if(!m_pPatchPackage)
			m_pPatchPackage = new CPackage(NULL,m_strExePath);
		m_pPatchPackage->Open("\\updatefiles\\Patch");
	}

	unzClose(dataZipFile);

	//得到被升级的版本数据
	hRes = ::FindResource(NULL,MAKEINTRESOURCE(IDR_PATCH2),"PATCH");
	if( hRes == NULL ) 
	{ 
		DWORD dwError = GetLastError();
		return FALSE; 
	} 

	hResLoad = ::LoadResource(NULL, hRes); 
	if (hResLoad == NULL) 
	{ 
		return FALSE;
	} 

	lpResLock = ::LockResource(hResLoad); 
	if (lpResLock == NULL) 
	{ 
		return FALSE;
	}
	m_listStrUpdatedVersion = (char*)lpResLock;	
	return true;
}

//设置升级包数据
BOOL	CPatchDlg::SetPatchData()
{

	BOOL result;
	HANDLE hUpdateRes;  // update resource handle 

	hUpdateRes = BeginUpdateResource(NULL, TRUE); 
	if (hUpdateRes == NULL) 
	{ 
		return FALSE; 
	} 

	//把包数据写临时文件
	HANDLE hFile = CreateFile("test.txt",
		GENERIC_READ,0,(LPSECURITY_ATTRIBUTES) NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,(HANDLE) NULL);

	if (hFile == INVALID_HANDLE_VALUE) { 
		return FALSE; 
	} 

	DWORD dwSize = ::GetFileSize(hFile,NULL);
	BYTE* pBuf = new BYTE[dwSize+1];
	DWORD dwReadSize;
	result = ReadFile(hFile,pBuf,dwSize,&dwReadSize,NULL);
	if( !result )
	{
		return FALSE;
		delete []pBuf;
	}
	pBuf[dwSize]='\0';

	result = UpdateResource(hUpdateRes,"PATCH",
		MAKEINTRESOURCE(IDR_PATCH1), 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		pBuf,
		dwSize);

	delete []pBuf;
	if (result == FALSE) 
	{ 
		return FALSE;
	} 

	// Write changes to FOOT.EXE and then close it. 
	if (!EndUpdateResource(hUpdateRes, FALSE)) 
	{ 
		return FALSE; 
	} 
	return TRUE;
}
void CPatchDlg::OnBnClickedButtonSelectfolder()
{
	//// TODO: 在此添加控件通知处理程序代码
	//BROWSEINFO bi;
	//ZeroMemory(&bi,sizeof(BROWSEINFO));
	//bi.hwndOwner=GetSafeHwnd();
	//bi.lpszTitle="选择游戏运行目录:";
	//LPITEMIDLIST idl=SHBrowseForFolder(&bi);
	//if(idl==NULL)
	//	return;
	//SHGetPathFromIDList(idl,m_strGameRunFolder.GetBuffer(MAX_PATH));

	//UpdateData(false);
	//m_strWorkPath = m_strGameRunFolder.GetBuffer(MAX_PATH);

	//if(CheckGameRunFolder())
	//{
	//	GetDlgItem(ID_UPDATE)->EnableWindow(TRUE);
	//}
	//else
	//{
	//	GetDlgItem(ID_UPDATE)->EnableWindow(FALSE);
	//	AfxMessageBox("该目录不是游戏运行目录。");
	//}
}


void CPatchDlg::OnEnChangeEditGamerunfolder()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	UpdateData(true);
	m_strWorkPath = m_strGameRunFolder.GetBuffer(MAX_PATH);

	if(CheckGameRunFolder())
	{
		GetDlgItem(ID_UPDATE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(ID_UPDATE)->EnableWindow(FALSE);
		AfxMessageBox("该目录不是游戏运行目录。");
	}
}

void CPatchDlg::OnBnClickedUpdate()
{
	// TODO: 在此添加控件通知处理程序代码
	//禁用这个按钮
	GetDlgItem(ID_UPDATE)->EnableWindow(FALSE);

	if(m_pClientResource)
	{
		delete m_pClientResource;
		m_pClientResource=NULL;
	}

	m_pClientResource = new CClientResource(GAME_RES,m_strWorkPath,m_strFilesInfoName);

	unsigned threadID;
	hThread = (HANDLE)_beginthreadex(NULL,0,UpdateProc,this,0,&threadID);
}


BOOL	CPatchDlg::CheckGameRunFolder()
{
	string strFilesInfoName = m_strWorkPath+"\\PackRes.cfg";
	WIN32_FIND_DATA fdt;

	HANDLE hFind = FindFirstFile(strFilesInfoName.c_str(), &fdt);
	if( hFind == INVALID_HANDLE_VALUE )
		return FALSE;
	FindClose(hFind);

	/*strFilesInfoName = m_strWorkPath+"\\Game.dp";
	hFind = FindFirstFile(strFilesInfoName.c_str(), &fdt);
	if( hFind == INVALID_HANDLE_VALUE )
		return FALSE;
	FindClose(hFind);*/

	return TRUE;
}

void	CPatchDlg::OnUpdateDisplay()
{
	m_strDisplayInfo=gDisplayInfo.strText;
	

	m_Progress.SetRange(0,200);
	if(gDisplayInfo.nMax==0)
	{
		m_Progress.SetPos(0);
	}
	else
	{
		float fNum = (double)gDisplayInfo.nCur/(double)gDisplayInfo.nMax;
		short nCurpos = (float)200*fNum;
		m_Progress.SetPos(nCurpos);
	}

	UpdateData(false);
}

void CPatchDlg::SetDisplayInfo(HWND hwnd,CString strText,int nPos,int nMax)
{
	gDisplayInfo.strText=strText;
	gDisplayInfo.nCur=nPos;
	if(nMax!=-1)
		gDisplayInfo.nMax=nMax;
	::PostMessage(hwnd,DISPLAY_UPDATE,NULL,NULL);
	Sleep(2);
}
void CPatchDlg::SetDisplayInfo(HWND hwnd,int nPos,int nMax)
{
	gDisplayInfo.nCur=nPos;
	if(nMax!=-1)
		gDisplayInfo.nMax=nMax;
	::PostMessage(hwnd,DISPLAY_UPDATE,NULL,NULL);
	Sleep(2);
}


BOOL CPatchDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)
	{
	case DISPLAY_UPDATE:
		{
			OnUpdateDisplay();
		}
		break;
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

//升级线程
unsigned __stdcall CPatchDlg::UpdateProc(void* pParam)
{
	CPatchDlg* pPatchDlg = (CPatchDlg*)pParam;
	if(pPatchDlg == NULL || pPatchDlg->m_pClientResource == NULL)	return 0;

	/*if(pPatchDlg->GetPatchInfo()==FALSE)
	{
		AfxMessageBox("UpdateProc::得到包信息数据错误。");
		_endthreadex( 0 );
		return 0;
	}*/

	if(pPatchDlg->GetPatchData()==FALSE)
	{
		AfxMessageBox("UpdateProc::得到包数据错误。");
		_endthreadex( 0 );
		return 0;
	}

	//设置包信息
	CClientResource* pClientResour = pPatchDlg->m_pClientResource;
	if(!LoadClientResource(pPatchDlg))
	{
		AfxMessageBox("UpdateProc::读取资源数据错误。");
		_endthreadex( 0 );
		return FALSE;
	}

	if(bExit == true)
	{
		_endthreadex( 0 );
		return FALSE;
	}

	char strTitle[200];
	string tmpFolder = pPatchDlg->m_strExePath + "\\PackRes.cfg";
	FILE* file = fopen(tmpFolder.c_str(), "rb");
	char strVersion[16] = {0};
	if(file)
	{
		fread(strVersion, 10, 1, file);
		fclose(file);
	}
	
	
	if(strcmp(strVersion, pPatchDlg->m_listStrUpdatedVersion.c_str()) != 0) // 补丁包内的被升级版本号跟客户端FileInfo记录的不符.
	{
		char szOutPut[1024];
		sprintf(szOutPut, "补丁包内的被升级客户端版本号[%s]与客户端的版本号[%s]不符,\r\n请检查客户端的当前版本号。", pPatchDlg->m_listStrUpdatedVersion.c_str(), strVersion);
		AfxMessageBox(szOutPut);
		return FALSE;
	}

	sprintf(strTitle,"升级游戏从版本%s 到版本%s。 ", strVersion,
		pPatchDlg->m_pPatchFilesInfo->GetVersion().c_str());
	pPatchDlg->SetWindowText(strTitle);

	

	string strTemptOld;		
	string strTemptNew;
	strTemptOld.insert(0,pClientResour->GetVersion(),0,5);
	strTemptNew.insert(0,strVersion,0,5);

#ifndef	_DEBUG
	if(strTemptOld != strTemptNew)
	{
		//被升级的游戏版本不符
		SetDisplayInfo(pPatchDlg->m_hWnd,"被升级的游戏版本不符。",1,1);
		PutDebugString("被升级的游戏版本不符。");
		return FALSE;
	}
#endif

	//得到不同部分
	CFilesInfo DirfFilesInfo;
	pPatchDlg->m_pPatchFilesInfo->GetDifferenceByCrc32( *(pClientResour->GetFilesInfo()),DirfFilesInfo);


	SetDisplayInfo(pPatchDlg->m_hWnd,"更新文件...",0,
				DirfFilesInfo.GetRootFile()->dwSize);
	PutDebugString("更新文件...");

	int nTotalSize = 0;

	DWORD	dwBufSize = 1024*1024*2;
	BYTE *pBuf = new BYTE[dwBufSize];
	BOOL bFlag = UpdateClientResource(DirfFilesInfo.GetRootFile(),&DirfFilesInfo,pBuf,dwBufSize,
		pPatchDlg->m_pPatchPackage,pClientResour,pPatchDlg->m_hWnd,nTotalSize);
	delete[] pBuf;

	if( bFlag )
	{
		pClientResour->GetFilesInfo()->SetVersion(pPatchDlg->m_pPatchFilesInfo->GetVersion());
		pClientResour->UpdateSave();
		SetDisplayInfo(pPatchDlg->m_hWnd,"升级成功。",1,1);
		AfxMessageBox("升级成功");
	}

	else
	{
		pClientResour->UpdateSave();
		SetDisplayInfo(pPatchDlg->m_hWnd,"升级失败。",1,1);
		AfxMessageBox("升级失败");
	}

	// 关闭包资源
	pPatchDlg->m_pPatchPackage->CloseFileHandle();

	string delPath = pPatchDlg->m_strWorkPath + "\\updatefiles";
	DeleteDirectory(delPath.c_str());

	::SendMessage(pPatchDlg->m_hWnd, WM_CLOSE, 0, 0);
	// 打开自动更新程序
	string updateExe = pPatchDlg->m_strWorkPath + "\\update.exe";
	WinExec(updateExe.c_str(), SW_SHOWNORMAL);
	_endthreadex( 0 );
	return 0;
}

//装载客户端资源信息
BOOL	CPatchDlg::LoadClientResource(CPatchDlg* pPatchDlg)
{
	
	CClientResource* pClientResour = pPatchDlg->m_pClientResource;

	int nMax = 1;
	int nPos = 0;
	SetDisplayInfo(pPatchDlg->m_hWnd,"读取客户端文件信息...",nPos,nMax);
	PutDebugString("读取客户端文件信息...");
	if(pClientResour->LoadEx( 
					pPatchDlg->m_pPatchFilesInfo->GetPackageTypeInfo() )==false)
	{
		SetDisplayInfo(pPatchDlg->m_hWnd,"读取客户端重要文件信息出错。",nMax,nMax);
		PutDebugString("读取客户端重要文件信息出错.");
		return FALSE;
	}

	SetDisplayInfo(pPatchDlg->m_hWnd,"读取客户端文件信息完毕。",nMax,nMax);
	PutDebugString("读取客户端文件信息完毕.");
	return TRUE;;
}

//更新相应的文件
BOOL  CPatchDlg::UpdateClientResource(tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,BYTE*& pBuf,DWORD& dwBuflen,
					 CPackage* pPackage, CClientResource* pClientResource,HWND hwnd,int& nTotalSize)
{
	if(ptagFileInfo==NULL || pFilesInfo==NULL || pPackage==NULL 
		|| pClientResource==NULL || bExit==true )
		return false;

	string strFileName = pFilesInfo->GetFileTextWithParent(ptagFileInfo);
	if( pFilesInfo->HaveChild(ptagFileInfo) )
	{
		//如果没有此文件夹，则添加
		tagFileInfo* pCurFolderInfo = pClientResource->GetFilesInfo()->GetFileInfoByText(strFileName);
		if(pCurFolderInfo==NULL)
		{
			string strParentName = pFilesInfo->GetFileTextWithParent(ptagFileInfo->ParentFileInfo);
			pClientResource->GetFilesInfo()->AddFolderInfo(strParentName,ptagFileInfo->strFileName,
				ptagFileInfo->dwSize,ptagFileInfo->dwOrginSize,ptagFileInfo->dwValidSize,ptagFileInfo->dwPackageType,
				ptagFileInfo->dwCompressType);
		}

		if( ENABLEBIT(ptagFileInfo->dwPackageType,NOPACK) )
		{
			//尝试创建该文件夹
			string strTemptFileName = pClientResource->GetCurFolder()+strFileName;
			::CreateDirectory(strTemptFileName.c_str(),NULL);
		}

		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			if(!UpdateClientResource((*it).second,pFilesInfo,pBuf,dwBuflen,
										pPackage,pClientResource,hwnd,nTotalSize))
			{
				return FALSE;
			}
		}
	}
	else if(ptagFileInfo->bFolder == false)
	{
		//nTotalSize += ptagFileInfo->dwSize;
		//SetDisplayInfo(hwnd,nTotalSize);

		if( !ENABLEBIT(ptagFileInfo->dwPackageType,NOPACK) )
		{
			DWORD dwSize = pPackage->GetFileSize(strFileName.c_str());
			// 0字节文件不写
			if(dwSize == 0) return true;

			tagFileIndex FileIndex;
			if(dwSize > dwBuflen)
			{
				delete[] pBuf;
				dwBuflen = dwSize;
				pBuf = new BYTE[dwSize];
			}

			//从包中提取数据
			if(pPackage->ExtractToBuf(strFileName.c_str(),pBuf,dwSize,FileIndex))
			{
				try
				{
					CPackage *pGamePackage = pClientResource->GetPackageForUpdate(ptagFileInfo->dwPackageType);
					if(pGamePackage)
					{
						//	FileIndex.dwRealSize=FileIndex.dwSize;
						int nRet = pGamePackage->WriteData(FileIndex,pBuf);
						if(nRet == false)
						{
							char outPtr[1024];
							sprintf(outPtr, "写文件 %s 出错!", strFileName.c_str());
							PutDebugString(outPtr);
							return false;
						}
					}
				}
				catch (...)
				{
					char strTempt[256]="";
					sprintf(strTempt,"写数据出现严重错误:FileName:%s,dwSize:%d",
						strFileName.c_str(),dwSize);
					AfxMessageBox(strTempt);
					return false;
				}
			}
			else
			{
				char outPtr[1024];
				sprintf(outPtr, "从包中提取数据文件 %s 出错!", strFileName.c_str());
				PutDebugString(outPtr);
				return false;
			}
		}
		else
		{
			try
			{
				string strTemptFileName = pClientResource->GetCurFolder()+strFileName;
				//提取到文件
				if( pPackage->ExtractToFile(strFileName.c_str(),strTemptFileName.c_str()) == false)
				{
					char outPtr[1024];
					sprintf(outPtr, "从包中提取到文件 %s 出错!", strFileName.c_str());
					PutDebugString(outPtr);
					return false;
				}
			}
			catch (...)
			{
				AfxMessageBox("提取文件出现严重错误。");
				return false;
			}
		}

		pClientResource->GetFilesInfo()->UpdateFileInfo(strFileName,ptagFileInfo->dwCrc32,
						ptagFileInfo->dwSize,ptagFileInfo->dwOrginSize,ptagFileInfo->dwValidSize,false,
						ptagFileInfo->dwPackageType,ptagFileInfo->dwCompressType);

		nTotalSize += ptagFileInfo->dwSize;
		SetDisplayInfo(hwnd,strFileName.c_str(),nTotalSize);
	}
	//Sleep(2);
	// 升级完成清除临时文件Update.txt,tmp.zip,

	return TRUE;
}
void CPatchDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	::WaitForSingleObject(hThread,INFINITE);
	CDialog::OnCancel();
}

void CPatchDlg::OnBnClickedCancel()
{
	bExit = true;
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CPatchDlg::OnStnClickedStaticTextinfo()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CPatchDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	// 查找客户端路径,注册表
	//注册表信息
	string strReg("HKEY_LOCAL_MACHINE/SOFTWARE/xingman/Luvinia");

	HKEY hKey;
	DWORD dwBufLen =2*MAX_PATH+10;
	char szKeyValue[2*MAX_PATH+10];
	memset(szKeyValue, 0, 2*MAX_PATH+10);

	LONG lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		"SOFTWARE\\XINGMAN\\LUVINIA",
		0, KEY_QUERY_VALUE, &hKey );


	lRet = RegQueryValueEx( hKey, "Client_Path", NULL, NULL,
		(LPBYTE)szKeyValue, &dwBufLen);

	RegCloseKey( hKey );

	if(szKeyValue[0] != '\0')
	{
		m_strExePath = string(szKeyValue);
		m_strWorkPath = m_strExePath;
	}

	bExit = false;

	string strFileName = m_strWorkPath + "\\launcher.exe";
	WinExec(strFileName.c_str(), SW_SHOW);
}
