// ServerUpdateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerUpdate.h"
#include "ServerUpdateDlg.h"
#include ".\serverupdatedlg.h"

#include "../../PUBLIC/Package.h"
#include "../../PUBLIC/Crc32Static.h"

#include <AFXINET.H>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerUpdateDlg 对话框

#define GET_BUFFER_MAX 1024 * 16
#define UPDATE_DISPLAY_INFO	WM_USER+6000
#define UPDATE_PROGRESS_MAX_LEN	WM_USER+6001
#define UPDATE_PROGRESS_CUR_LEN	WM_USER+6002

#define START_BUTTON_ENABLE	WM_USER+6003
#define EXIT_BUTTON_ENABLE	WM_USER+6004

#define UPDATE_TEXT	WM_USER+6000
HANDLE hUpdateThread=0;
static string strExceFilePath = "";
string g_strExceFileName="";

HINTERNET hUpdateServerConnect = NULL;							//和更新服务器的连接
HINTERNET hUpdateSession = NULL;

static string g_strOldVersion="";
static string g_strCurVersion="";
static string g_strServerTile="";

struct tagThreadParam
{
	HWND hWnd;
	CClientResource *pClientResouce;
	string strCurDirecotry;
	string strServerAddress;
	string strServerFolder;
};

CServerUpdateDlg::CServerUpdateDlg(string& strCurDirectory,string& strServerAdress,
								  string& strServerFolder,string &strFileName,
								  string& strServerTitle,CWnd* pParent /*=NULL*/)
	: CDialog(CServerUpdateDlg::IDD, pParent)
	,m_strWorkPath(strCurDirectory)
	,m_strServerAddress(strServerAdress)
	,m_strServerFolder(strServerFolder)
	,m_strFilesInfoName("FilesInfo.ril")
	,m_pClientResouce(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	g_strExceFileName = strFileName;
	strExceFilePath = "\\"+strFileName;
	g_strServerTile=strServerTitle;
}

void CServerUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_UPDATE, m_UpdateProgress);
	DDX_Control(pDX, IDC_STATIC_UPDATE_INFO, m_UpdateInfo);
	DDX_Control(pDX, IDC_BUTTON_STARTSERVER, m_StartServer);
	DDX_Control(pDX, IDC_BUTTON_CANCEL, m_Exit);
}

BEGIN_MESSAGE_MAP(CServerUpdateDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
//	ON_WM_CHAR()
ON_BN_CLICKED(IDC_BUTTON_STARTSERVER, OnBnClickedButtonStartserver)
ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CServerUpdateDlg 消息处理程序

BOOL CServerUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_StartServer.EnableWindow(false);
	m_Exit.EnableWindow(false);
	//初始化资源实例
	m_pClientResouce = new CClientResource(GAME_RES,m_strWorkPath,m_strFilesInfoName);
	if(m_pClientResouce->LoadExForAutoUpdate()==false)
	{
		delete m_pClientResouce;m_pClientResouce= NULL;
		m_Exit.EnableWindow(true);
		return TRUE;
	}
	g_strCurVersion = m_pClientResouce->GetFilesInfo()->GetVersion();
	g_strOldVersion = g_strCurVersion;

	string strTitle = g_strServerTile;
	strTitle += "(";
	strTitle += g_strCurVersion;
	strTitle += ")";
	SetWindowText(strTitle.c_str());
	//开始更新线程
	unsigned threadID;
	tagThreadParam* pParam = new tagThreadParam;
	pParam->hWnd = m_hWnd;
	pParam->pClientResouce = m_pClientResouce;
	pParam->strCurDirecotry= m_strWorkPath;
	pParam->strServerAddress= m_strServerAddress;
	pParam->strServerFolder = "/"+m_strServerFolder;
	hUpdateThread = (HANDLE)_beginthreadex(NULL,0,UpdateProc,(void*)pParam,0,&threadID);	
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CServerUpdateDlg::Release()
{
	::WaitForSingleObject(hUpdateThread,INFINITE);

	if(m_pClientResouce )
	{
		delete m_pClientResouce;
		m_pClientResouce = NULL;
	}

	if(hUpdateServerConnect)
		::InternetCloseHandle(hUpdateServerConnect);
	if(hUpdateSession)
		::InternetCloseHandle(hUpdateSession);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServerUpdateDlg::OnPaint() 
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CServerUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerUpdateDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
}

void CServerUpdateDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
}

BOOL CServerUpdateDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)
	{
	case UPDATE_DISPLAY_INFO:
		{
			char *strInfo = (char*)wParam;
			m_UpdateInfo.SetWindowText(strInfo);
			delete []strInfo;
		}
		break;
	case UPDATE_PROGRESS_MAX_LEN:
		{
			int nNum = (int)wParam;
			m_UpdateProgress.SetRange(0,nNum);
		}
		break;
	case UPDATE_PROGRESS_CUR_LEN:
		{
			int nPos = (int)wParam;
			m_UpdateProgress.SetPos(nPos);
		}
		break;
	case START_BUTTON_ENABLE:
		{
			m_StartServer.EnableWindow(true);
			m_Exit.EnableWindow(true);

			string strTitle = g_strServerTile;
			strTitle += "(";
			strTitle += g_strCurVersion;
			strTitle += ")";
			SetWindowText(strTitle.c_str());
			Release();
		}
		break;
	case EXIT_BUTTON_ENABLE:
		{
			m_Exit.EnableWindow(true);
			Release();
		}
		break;
	}
	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void CServerUpdateDlg::SetProgressMaxLen(HWND hwnd,int nNum)
{
	::PostMessage(hwnd,UPDATE_PROGRESS_MAX_LEN,(WPARAM)nNum,NULL);
}
void CServerUpdateDlg::SetProgressCunPos(HWND hwnd,int nPos)
{
	::PostMessage(hwnd,UPDATE_PROGRESS_CUR_LEN,(WPARAM)nPos,NULL);
}
void CServerUpdateDlg::SetDisplayText(HWND hwnd,CString strInfo)
{
	int len = strInfo.GetLength();
	char *szInfo = new char[len+1];
	strcpy(szInfo,strInfo.GetBuffer());
	szInfo[len] = '\0';
	::PostMessage(hwnd,UPDATE_DISPLAY_INFO,(WPARAM)szInfo,NULL);
}

//更新线程
unsigned __stdcall CServerUpdateDlg::UpdateProc(void* pParam)
{
	tagThreadParam* pThreadParam = (tagThreadParam*)pParam;
	SetProgressMaxLen(pThreadParam->hWnd,2);
	SetProgressCunPos(pThreadParam->hWnd,0);
	SetDisplayText(pThreadParam->hWnd,"得到服务器最新版本信息...");

	if(::InternetAttemptConnect(0) != ERROR_SUCCESS)
	{
		SetDisplayText(pThreadParam->hWnd,"网络初始化失败(InternetAttemptConnect)!");
		::PostMessage(pThreadParam->hWnd,EXIT_BUTTON_ENABLE,NULL,NULL);
		delete pThreadParam;
		_endthreadex( 0 );
		return 0;
	}

	HINTERNET hSession = ::InternetOpen(_T("Tnt"),INTERNET_OPEN_TYPE_PRECONFIG,_T(""),
		INTERNET_INVALID_PORT_NUMBER,0);
	if(hSession == NULL)
	{		
		SetDisplayText(pThreadParam->hWnd,"网络初始化失败(InternetOpen)!");
		::PostMessage(pThreadParam->hWnd,EXIT_BUTTON_ENABLE,NULL,NULL);
		delete pThreadParam;
		_endthreadex( 0 );
		return 0;
	}
	hUpdateSession = hSession;

	HINTERNET hUpdateConnect=NULL;
	string strServerVersion("");
	DWORD dwServerFileInfoCRC;
	if(GetLastFileInfo(hSession,pThreadParam->strCurDirecotry,
					pThreadParam->strServerAddress,pThreadParam->strServerFolder,
					hUpdateConnect,dwServerFileInfoCRC,strServerVersion) == false)
	{
		SetDisplayText(pThreadParam->hWnd,"得最新版本信息失败!");
		::PostMessage(pThreadParam->hWnd,EXIT_BUTTON_ENABLE,NULL,NULL);
		delete pThreadParam;
		_endthreadex( 0 );
		return 0;
	}
	hUpdateServerConnect = hUpdateConnect;

	string strCurVersion = pThreadParam->pClientResouce->GetFilesInfo()->GetVersion();
	//版本一样退出,进入设置界面
	if(strServerVersion==strCurVersion)
	{
		SetProgressCunPos(pThreadParam->hWnd,2);
		SetDisplayText(pThreadParam->hWnd,"当前是最新版本。");

		::PostMessage(pThreadParam->hWnd,START_BUTTON_ENABLE,NULL,NULL);
		delete pThreadParam;
		_endthreadex( 0 );
		return	0;
	}
	SetProgressCunPos(pThreadParam->hWnd,1);
	SetDisplayText(pThreadParam->hWnd,"版本已过期,得最新文件信息...");

	//加一个变量作用范围,以使线程退出前，释放所有资源
	{
		CFilesInfo ServerFilesInfo,TemptFilesInfo;
		string strServerFileInfoName = pThreadParam->strServerFolder;
		strServerFileInfoName +="\\FilesInfo.ril";
		string strLocalFileName = pThreadParam->strCurDirecotry;
		strLocalFileName += "\\UpdateFilesInfo.ril";
		if( GetLastFileInfos(hUpdateConnect,strServerFileInfoName,
							strLocalFileName,dwServerFileInfoCRC,&ServerFilesInfo) == false)
		{
			SetDisplayText(pThreadParam->hWnd,"得最新文件信息失败。");
			::PostMessage(pThreadParam->hWnd,EXIT_BUTTON_ENABLE,NULL,NULL);
			delete pThreadParam;
			_endthreadex( 0 );
			return	0;
		}

		ServerFilesInfo.GetDifferenceByCrc32(*(pThreadParam->pClientResouce->GetFilesInfo()),TemptFilesInfo);
		pThreadParam->pClientResouce->ResetPackInfosForAutoUpdate(ServerFilesInfo.GetPackageTypeInfo());
		SetProgressCunPos(pThreadParam->hWnd,2);
		SetDisplayText(pThreadParam->hWnd,"得最新文件列表完毕。");

		//下载文件
		int nFileTotalSize = TemptFilesInfo.GetRootFile()->dwSize;
		int nUpdateSize = 0;
		SetProgressCunPos(pThreadParam->hWnd,nFileTotalSize);
		SetDisplayText(pThreadParam->hWnd,"更新文件...");

		//打开临时下载包
		CPackage DownloadPack(NULL, pThreadParam->strCurDirecotry,1);
		DownloadPack.OpenForUpdate("TemptDownloadPack");
		string strDownloadPackFileName = pThreadParam->strCurDirecotry;
		strDownloadPackFileName = strDownloadPackFileName+"\\"+"TemptDownloadPack"+".pak";

		BYTE *pBuf = new BYTE[GET_BUFFER_MAX];
		DWORD dwBufLen = GET_BUFFER_MAX;

		if(DownLoadFilesEx(hUpdateConnect,&DownloadPack,pBuf,dwBufLen,
			TemptFilesInfo.GetRootFile(),&TemptFilesInfo,
			pThreadParam->strServerFolder,pThreadParam->pClientResouce,
			nUpdateSize,pThreadParam->hWnd)==false)
		{
			SetDisplayText(pThreadParam->hWnd,"文件更新发生错误。");
			pThreadParam->pClientResouce->UpdateSave();
			delete[] pBuf;
			DownloadPack.CloseFileHandle();
			::DeleteFile(strDownloadPackFileName.c_str());
			::PostMessage(pThreadParam->hWnd,EXIT_BUTTON_ENABLE,NULL,NULL);
			delete pThreadParam;
			_endthreadex( 0 );
			return 0;
		}
		delete[] pBuf;
		DownloadPack.CloseFileHandle();
		::DeleteFile(strDownloadPackFileName.c_str());

		string strNewExceFileName = strExceFilePath+".new";
		tagFileInfo* pFileInfo =
			pThreadParam->pClientResouce->GetFilesInfo()->GetFileInfoByText(strNewExceFileName);

		//察看服务器执行文件是否更新
		if(pFileInfo && pFileInfo->dwValidSize == pFileInfo->dwSize)
		{
			pThreadParam->pClientResouce->GetFilesInfo()->UpdateFileInfo(strExceFilePath,pFileInfo->dwCrc32,
						pFileInfo->dwSize,pFileInfo->dwOrginSize,pFileInfo->dwValidSize,false,
						pFileInfo->dwPackageType,pFileInfo->dwCompressType);

			pThreadParam->pClientResouce->GetFilesInfo()->DelFileInfo(strNewExceFileName);

			
			WIN32_FIND_DATA myWFD;
			CString strNewExceName(g_strExceFileName.c_str());
			strNewExceName = strNewExceName+".new";
			HANDLE hl = FindFirstFile(strNewExceName.GetBuffer(),&myWFD);
			if ( hl != INVALID_HANDLE_VALUE)
			{
				CString strOldExceName(g_strExceFileName.c_str());
				
				SYSTEMTIME sysCurrTime;
				::GetLocalTime(&sysCurrTime);
				char strCurTime[100];
				sprintf(strCurTime,"(%d_%d_%d[%d_%d_%d])(%s)",
									sysCurrTime.wYear,sysCurrTime.wMonth,sysCurrTime.wDay,
									sysCurrTime.wHour,sysCurrTime.wMinute,sysCurrTime.wSecond,
									g_strOldVersion.c_str());
				strOldExceName = strOldExceName+strCurTime;

				rename(g_strExceFileName.c_str(),strOldExceName.GetBuffer());
				rename(strNewExceName.GetBuffer(),g_strExceFileName.c_str());
			}
		}

		SetDisplayText(pThreadParam->hWnd,"保存更新信息...");
		SetProgressCunPos(pThreadParam->hWnd,nFileTotalSize);
		pThreadParam->pClientResouce->GetFilesInfo()->SetVersion(strServerVersion);
		pThreadParam->pClientResouce->UpdateSave();

		//记录版本号
		g_strCurVersion = strServerVersion;
	}

	//建立版本号文件
	if( _access(g_strOldVersion.c_str(),0) != -1 )
	{
		//文件存在
		rename(g_strOldVersion.c_str(),g_strCurVersion.c_str());
	}
	else
	{
		//不存在
		FILE *pf = fopen( g_strCurVersion.c_str(),"w" );
		if(pf)
			fclose(pf);
	}


	SetDisplayText(pThreadParam->hWnd,"更新完毕。");
	::PostMessage(pThreadParam->hWnd,START_BUTTON_ENABLE,NULL,NULL);
	delete pThreadParam;
	_endthreadex( 0 );
	return 0;
}

bool CServerUpdateDlg::GetLastFileInfo(HINTERNET hSession,string& strCurDirectory,
									string& strUrl,string& strServerFolder,
									HINTERNET& hUpdateConnect,DWORD& dwFileInfoCRC,string& strVersion)
{
	//尝试是否能打开服务器
	string strUpdateUrl = "http://"+strUrl;
	strUpdateUrl += strServerFolder;
	HINTERNET hOpen = ::InternetOpenUrl(hSession,strUpdateUrl.c_str(),NULL,0,INTERNET_FLAG_EXISTING_CONNECT,NULL);
	if(hOpen)
	{
		InternetCloseHandle(hOpen);
		HINTERNET hConnect = ::InternetConnect(hSession,strUrl.c_str(),
			INTERNET_INVALID_PORT_NUMBER,_T(""),_T(""),INTERNET_SERVICE_HTTP,0,0);
		if(hConnect)
		{
			hUpdateConnect = hConnect;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	string strFileName = strServerFolder;
	strFileName +="/AutoUpdateGame.vs";
	string strLocalVersionFileName = strCurDirectory;
	strLocalVersionFileName += "\\AutoUpdateGame.vs";
	if(GetLastVersion(hUpdateConnect,strFileName,strLocalVersionFileName,dwFileInfoCRC,strVersion)==false)
	{
		InternetCloseHandle(hUpdateConnect);
		hUpdateConnect = NULL;
		return false;
	}

	return true;
}
bool CServerUpdateDlg::GetLastVersion(HINTERNET hConnect,string& strPathName,string& strLocalVersionName,
								DWORD& dwFileInfoCRC,string& strVersion)
{
	HINTERNET hHttpFile = ::HttpOpenRequest(hConnect,_T("GET"),
		strPathName.c_str(),HTTP_VERSION,NULL,0,
		INTERNET_FLAG_RELOAD,0);
	if(hHttpFile == NULL)
	{
		return false;
	}
	BOOL bSendRequest = ::HttpSendRequest(hHttpFile,NULL,0,0,0);
	if(bSendRequest)
	{
		char	achQueryBuf[16];
		DWORD	dwFileSize;
		DWORD	dwQueryBufLen = sizeof(achQueryBuf);

		BOOL	bQuery = ::HttpQueryInfo(hHttpFile,
			HTTP_QUERY_CONTENT_LENGTH,achQueryBuf,&dwQueryBufLen,NULL);

		if(!bQuery)
		{
			::InternetCloseHandle(hHttpFile);
			return false;
		}


		FILE* pf = fopen(strLocalVersionName.c_str(),"w+b");
		if(!pf)
		{
			::InternetCloseHandle(hHttpFile);
			return	false;
		}

		dwFileSize = (DWORD) atol(achQueryBuf);	
		char * lpszBuf = new char[dwFileSize + 1 ];
		DWORD	dwBytesRead;
		BOOL	bRead;
		
		bRead = ::InternetReadFile(hHttpFile,lpszBuf,dwFileSize+1,&dwBytesRead);
		::InternetCloseHandle(hHttpFile);

		fwrite(lpszBuf,1,dwBytesRead,pf);
		fflush(pf);
		fseek(pf,0,SEEK_SET);
		char chVersion[50]="";
		fscanf(pf,"%s\n",chVersion);
		fscanf(pf,"%u ",&dwFileInfoCRC);
		fclose(pf);
		chVersion[10]='\0';
		strVersion = chVersion;

		::DeleteFile(strLocalVersionName.c_str());	

		if( lpszBuf[0] < '0' || lpszBuf[0] > '9' || lpszBuf[1] < '0' || lpszBuf[1] > '9' ||
			 lpszBuf[3] < '0' || lpszBuf[3] > '9' || lpszBuf[4] < '0' || lpszBuf[4] > '9' ||
			  lpszBuf[6] < '0' || lpszBuf[6] > '9' || lpszBuf[7] < '0' || lpszBuf[7] > '9' ||
			   lpszBuf[8] < '0' || lpszBuf[8] > '9' || lpszBuf[9] < '0' || lpszBuf[9] > '9')
		{
			return false;
		}
		delete []lpszBuf;
		return	true;
	}
	else
	{
		DWORD dwError = GetLastError();
		return false;
	}
	return	false;
}

bool CServerUpdateDlg::GetLastFileInfos(HINTERNET hConnect,string& strPathName,string& strLocalFileName,
									DWORD dwFileInfoCRC,CFilesInfo* pFilesInfo)
{
	if(pFilesInfo==NULL || hConnect==NULL)	return false;

	HINTERNET hHttpFile = ::HttpOpenRequest(hConnect,_T("GET"),
		strPathName.c_str(),HTTP_VERSION,NULL,0,
		INTERNET_FLAG_RELOAD,0);
	if(hHttpFile == NULL)
	{
		return false;
	}
	BOOL bSendRequest = ::HttpSendRequest(hHttpFile,NULL,0,0,0);
	if(bSendRequest)
	{
		char	achQueryBuf[16];
		DWORD	dwFileSize;
		DWORD	dwQueryBufLen = sizeof(achQueryBuf);

		BOOL	bQuery = ::HttpQueryInfo(hHttpFile,HTTP_QUERY_CONTENT_LENGTH,achQueryBuf,&dwQueryBufLen,NULL);

		if(!bQuery)
		{
			::InternetCloseHandle(hHttpFile);
			return false;
		}
		
		FILE* pf = fopen(strLocalFileName.c_str(),"wb");
		if(!pf)
		{
			::InternetCloseHandle(hHttpFile);
			return	false;
		}
		dwFileSize = (DWORD) atol(achQueryBuf);				
		BYTE	strBuffer[GET_BUFFER_MAX];
		DWORD	dwBytesRead;
		BOOL	bRead;

		DWORD dwCount = 0;
		DWORD dwRequest = ( GET_BUFFER_MAX > dwFileSize - dwCount ) ? dwFileSize - dwCount : GET_BUFFER_MAX;
		while (true)
		{
			bRead = ::InternetReadFile(hHttpFile,strBuffer,dwRequest,&dwBytesRead);
			if(!bRead || dwBytesRead <= 0)
			{
				fclose(pf);
				::DeleteFile(strLocalFileName.c_str());	
				::InternetCloseHandle(hHttpFile);
				return	false;
			}

			fwrite(strBuffer,1,dwBytesRead,pf);
			dwCount += dwBytesRead;
			if ( dwFileSize - dwCount < GET_BUFFER_MAX )
				dwRequest = dwFileSize - dwCount;

			if ( dwRequest <= 0 )
			{
				break;
			}
		}
		::InternetCloseHandle(hHttpFile);
		fclose(pf);

		DWORD dwCRC;
		CCrc32Static::FileCrc32Filemap(strLocalFileName.c_str(),dwCRC);
		if(dwCRC != dwFileInfoCRC)
		{
			::DeleteFile(strLocalFileName.c_str());	
			return	false;
		}
		pFilesInfo->Load(strLocalFileName);
		::DeleteFile(strLocalFileName.c_str());	
		return	true;
	}
	return false;
}

bool	 CServerUpdateDlg::DownLoadFilesEx(HINTERNET hConnect,CPackage* pPckage,BYTE*& pBuf,DWORD& dwBufLen,
								tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,
								string& strFtpPath,CClientResource* pResource,
								int& nTotalSize,HWND hwnd)
{
	if(ptagFileInfo==NULL)	return false;

	string strTemptFileName = pFilesInfo->GetFileTextWithParent(ptagFileInfo);

	if(pFilesInfo->HaveChild(ptagFileInfo))
	{
		//如果没有此文件夹，则添加
		tagFileInfo* pCurFolderInfo = pResource->GetFilesInfo()->GetFileInfoByText(strTemptFileName);
		if(pCurFolderInfo==NULL)
		{
			string strParentName = pFilesInfo->GetFileTextWithParent(ptagFileInfo->ParentFileInfo);
			pResource->GetFilesInfo()->AddFolderInfo(strParentName,ptagFileInfo->strFileName,
										ptagFileInfo->dwSize,ptagFileInfo->dwOrginSize,ptagFileInfo->dwValidSize, 
										ptagFileInfo->dwPackageType,ptagFileInfo->dwCompressType);
		}

		//如果未打包的，则要创建文件夹
		if( ENABLEBIT(ptagFileInfo->dwPackageType,NOPACK) )
		{
			//尝试创建该文件夹
			string strTemptFileName1 = pResource->GetCurFolder()+strTemptFileName;
			::CreateDirectory(strTemptFileName1.c_str(),NULL);
		}

		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			if( !DownLoadFilesEx(hConnect,pPckage,pBuf,dwBufLen,
								(*it).second,pFilesInfo,
								strFtpPath,pResource,
								nTotalSize,hwnd) )
				return false;
		}
	}
	else if(ptagFileInfo->bFolder == false)
	{
		string strServerFilename = strFtpPath+strTemptFileName;
		//如果是更新是游戏执行文件，则改变名字
		if( strTemptFileName == strExceFilePath )
		{
			strTemptFileName=strExceFilePath+".new";
		}
		else if(strTemptFileName == "ServerUpdate.exe")
		{
			//不更新自己
			return true;
		}

		SetDisplayText(hwnd,strTemptFileName.c_str());
			
		if( DownLoadOneFile(hConnect,pPckage,pBuf,
						strTemptFileName,strServerFilename,
						ptagFileInfo,nTotalSize,hwnd) == true)
		{
			bool bRet = false;
			//如果在包中
			if( !ENABLEBIT(ptagFileInfo->dwPackageType,NOPACK) )
			{
				//下载成功则向包中更新数据
				tagFileIndex FileIndex;
				DWORD dwSize = ptagFileInfo->dwSize;
				if(dwSize > dwBufLen)
				{
					delete[] pBuf;
					pBuf = new BYTE[dwSize];
					dwBufLen = dwSize;
				}
				if(pPckage->ExtractToBuf(strTemptFileName.c_str(),pBuf,dwSize,FileIndex))
				{
					FileIndex.dwValidSize = FileIndex.dwSize;
					CPackage *pUpdatePack = pResource->GetPackageForUpdate(ptagFileInfo->dwPackageType);
					if(pUpdatePack)
					{
						pUpdatePack->WriteData(FileIndex,pBuf);

						pResource->GetFilesInfo()->UpdateFileInfo(strTemptFileName,ptagFileInfo->dwCrc32,
							ptagFileInfo->dwSize,ptagFileInfo->dwOrginSize,ptagFileInfo->dwValidSize,false,
							ptagFileInfo->dwPackageType,ptagFileInfo->dwCompressType);
						
						bRet = true;
					}
					
				}
			}
			else
			{
				string strFullFileName = pResource->GetCurFolder()+strTemptFileName;
				//提取到文件
				if( pPckage->ExtractToFile(strTemptFileName.c_str(),strFullFileName.c_str()) )
				{
					pResource->GetFilesInfo()->UpdateFileInfo(strTemptFileName,ptagFileInfo->dwCrc32,
							ptagFileInfo->dwSize,ptagFileInfo->dwOrginSize,ptagFileInfo->dwValidSize,false,
							ptagFileInfo->dwPackageType,ptagFileInfo->dwCompressType);
					bRet = true;
				}
			}
			//清除数据，下载下一个文件
			pPckage->ClearData();
			return bRet;
			
		}
		else
		{
			//错误则保存下载的数据，退出。
			pPckage->WriteIndexEx();
			pPckage->CloseFileHandle();
			return false;
		}
	}
	return true;
}

bool	CServerUpdateDlg::DownLoadOneFile(HINTERNET hConnect,CPackage* pPckage,BYTE* pBuf,
							string& strTemptFileName,string& strServerFilename,
							tagFileInfo* ptagFileInfo,
							int& nTotalSize,HWND hwnd)
{
	HINTERNET hHttpFile = ::HttpOpenRequest(hConnect,_T("GET"),
			strServerFilename.c_str(),HTTP_VERSION,NULL,0,
			INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_DONT_CACHE |
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_KEEP_CONNECTION,0);
	if(hHttpFile == NULL)	return false;

	BOOL bSendRequest = ::HttpSendRequest(hHttpFile,NULL,0,0,0);
	if(!bSendRequest)	return false;

	char	achQueryBuf[16];
	DWORD	dwQueryBufLen = sizeof(achQueryBuf);
	BOOL bQuery = ::HttpQueryInfo(hHttpFile,
		HTTP_QUERY_CONTENT_LENGTH,achQueryBuf,&dwQueryBufLen,NULL);

	if(!bQuery)
	{
		::InternetCloseHandle(hHttpFile);
		return false;
	}
	DWORD	dwServerFileSize = (DWORD) atol(achQueryBuf);
	if(dwServerFileSize != ptagFileInfo->dwSize)	return false;

	pPckage->AddFile(strTemptFileName.c_str(),dwServerFileSize,ptagFileInfo->dwCrc32,
		ptagFileInfo->dwCompressType,ptagFileInfo->dwOrginSize);

	bool bRet = DownloadFile(hHttpFile,pPckage,pBuf,
							strTemptFileName,dwServerFileSize,0,
							nTotalSize,hwnd);
	
	::InternetCloseHandle(hHttpFile);
	return bRet;
}

bool CServerUpdateDlg::DownloadFile(HINTERNET hHttpFile,CPackage* pPckage,BYTE* pBuf,
							string& strTemptFileName,
							DWORD dwFileSize,DWORD dwCount,
							int& nTotalSize,HWND hwnd)
{
	DWORD   dwInvaSize = 0;
	int		nRet = true;
	DWORD	dwBytesRead = 0;
	BOOL	bRead;
	DWORD   dwRequest =
		( GET_BUFFER_MAX > dwFileSize - dwCount ) ? dwFileSize - dwCount : GET_BUFFER_MAX;
	while(dwRequest > 0)
	{
		bRead = ::InternetReadFile(hHttpFile,pBuf,dwRequest,&dwBytesRead);

		if(!bRead )
		{
			return false;
		}
		if(dwBytesRead == 0)
			break;
		nRet = pPckage->AddFileData(strTemptFileName.c_str(),pBuf,dwInvaSize,dwBytesRead);
		if(nRet == false)
		{
			return false;
		}
		dwCount += dwBytesRead;

		if ( dwFileSize - dwCount < GET_BUFFER_MAX )
			dwRequest = dwFileSize - dwCount;

		//显示进度
		nTotalSize += dwBytesRead;
		SetProgressCunPos(hwnd,nTotalSize);
	}
	return true;
}
void CServerUpdateDlg::OnBnClickedButtonStartserver()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnOK();
}

void CServerUpdateDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}
