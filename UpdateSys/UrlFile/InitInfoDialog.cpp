// InitInfoDialog.cpp : 实现文件
//

#include "stdafx.h"
#include <Mmsystem.h>
#include "urlfile.h"
#include "InitInfoDialog.h"
#include ".\initinfodialog.h"
#include "Ping.h"

#include "../../PUBLIC/Package.h"
#include "../../PUBLIC/Crc32Static.h"
#include <vector>
#include <map>
#include <time.h>
#include <process.h>
using namespace std;

#define GET_BUFFER_MAX 1024 * 16

#define DISPLAY_UPDATE	WM_USER+6000
const char* FAcceptTypes = "*/*";

tagDisplayInfo	gDisplayInfo;
HANDLE hInfoThread=0;
bool bExit = false;
// CInitInfoDialog 对话框

struct tagServer
{
	string	strIP;
	string	strPath;
};

IMPLEMENT_DYNAMIC(CInitInfoDialog, CDialog)
CInitInfoDialog::CInitInfoDialog(string& strWorkPath,string& strPackageName,string& strFileInfoName,CWnd* pParent /*=NULL*/)
	: CDialog(CInitInfoDialog::IDD, pParent)
	,m_strWorkPath(strWorkPath)
	,m_strPackageName(strPackageName)
	,m_strFilesInfoName(strFileInfoName)
	,m_strDisplayInfo(_T(""))
	,m_strServerCurDir("")
	,m_strnewVersion("")
	,m_pClientResource(NULL)
	,m_hUpdateSession(NULL)
	,m_hUpdateServerConnect(NULL)
	,m_bReadError(false)
{
	//初始化资源类的静态变量
}

CInitInfoDialog::~CInitInfoDialog()
{
	Release();
}

void	  CInitInfoDialog::Release()
{
	if(m_pClientResource )
	{
		delete m_pClientResource;
		m_pClientResource=NULL;
	}

	if(m_hUpdateServerConnect)
		::InternetCloseHandle(m_hUpdateServerConnect);
	if(m_hUpdateSession)
		::InternetCloseHandle(m_hUpdateSession);
}

void CInitInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDCANCEL, m_bnCancel);
	DDX_Control(pDX, IDC_DOWNLOADHINTINFO, m_DownloadHintInfo);
}


BEGIN_MESSAGE_MAP(CInitInfoDialog, CDialog)
//	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CInitInfoDialog 消息处理程序

BOOL CInitInfoDialog::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)
	{
	case DISPLAY_UPDATE:
		{
			OnUpdateDisplay();
		}
		break;
	case TREELIST_ADD:
		{
			//if(gDisplayInfo.nCur<gDisplayInfo.nMax)
			//{
			//	gDisplayInfo.nCur++;
			//	OnUpdateDisplay();
			//}
		}
		break;
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

BOOL CInitInfoDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	int cy = ::GetSystemMetrics(SM_CYSCREEN) / 2; 
	int cx = ::GetSystemMetrics(SM_CXSCREEN) / 2; 
	cx = cx - 560/2;
	cy = cy - 430/2;
	MoveWindow(cx,cy,560,430);
	m_DownloadHintInfo.MoveWindow(5,4,546,296);
	m_Progress.MoveWindow(5,330,546,20);
	m_bnCancel.MoveWindow(225,360,112,30);

	SetWindowText("检查更新");//设置对话框的标题

	UpdateData(false);

	// TODO:  在此添加额外的初始化

	CWnd* pMainWnd = FindWindow("OnlineCLIENT",NULL);
	if (pMainWnd != NULL)
	{
		AfxMessageBox(_T("你的主程序已经打开，请先关闭主程序，再运行更新程序！"));
		return TRUE;
	}

	m_bReadError=false;
	m_bIDOK		=false;
	gDisplayInfo.nCur=0;
	gDisplayInfo.nMax=0;
	gDisplayInfo.strText="";
	m_Progress.SetRange(0,1);
	m_pClientResource = new CClientResource(GAME_RES,m_strWorkPath,m_strFilesInfoName);

	unsigned threadID;
	hInfoThread = (HANDLE)_beginthreadex(NULL,0,LoadInfoProc,this,0,&threadID);	

	IniDownloadHintWeb();
	m_DownloadHintInfo.Navigate(m_strDownloadHitAddress.c_str(),FALSE,FALSE,FALSE,FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//初始化信息提示web,如果internet webpage不可用，用本地webpage;
void CInitInfoDialog::IniDownloadHintWeb(void)
{
	m_strDownloadHitAddress = "";
	vector<string>		HintList;
	CHAR* chBuffer = new CHAR[250];
	ifstream stream;
	stream.open("setup/downloadhintserver.ini");

	if(!stream.is_open())
	{
		delete []chBuffer;
		return;
	}
	while( !stream.eof() )
	{
		stream >> chBuffer;
		if (lstrlen(chBuffer) > 0)
			HintList.push_back(chBuffer);
	}
	if(HintList.size() ==0)
	{
		delete []chBuffer;
		stream.close();
		return;
	}

	srand((unsigned)time(NULL));
	int nRandom  = rand();
	nRandom = nRandom%(HintList.size());
	m_strDownloadHitAddress = HintList[nRandom];
	delete []chBuffer;
	stream.close();
}

void CInitInfoDialog::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	bExit = true;

	DWORD dwResult = ::WaitForSingleObject(hInfoThread,6000);
	if(dwResult == WAIT_TIMEOUT)
	{
		TerminateThread(hInfoThread,0);
	}
	//m_pClientResource->UpdateSave();

	if(m_bReadError==true)
		Release();
	CDialog::OnCancel();
}

void	CInitInfoDialog::OnUpdateDisplay()
{
	if(m_strDisplayInfo != gDisplayInfo.strText)
	{
		m_strDisplayInfo=gDisplayInfo.strText;
		//bFlag = true;
		CRect myRect;
		myRect.left=5;
		myRect.top=310;
		myRect.right=500;
		myRect.bottom=325;
		//重画文本显示区
		InvalidateRect(&myRect);
	}
	

	m_Progress.SetRange(0,10000);
	if(gDisplayInfo.nMax==0)
	{
		m_Progress.SetPos(0);
	}
	else
	{
		float fNum = (double)gDisplayInfo.nCur/(double)gDisplayInfo.nMax;
		short nCurpos = (float)10000*fNum;
		m_Progress.SetPos(nCurpos);
	}

	UpdateData(false);
}

void CInitInfoDialog::SetDisplayInfo(HWND hwnd,CString strText,int nPos,int nMax)
{
	gDisplayInfo.strText=strText;
	gDisplayInfo.nCur=nPos;
	if(nMax!=-1)
		gDisplayInfo.nMax=nMax;
	::PostMessage(hwnd,DISPLAY_UPDATE,NULL,NULL);
	Sleep(2);
}
void CInitInfoDialog::SetDisplayInfo(HWND hwnd,int nPos,int nMax)
{
	gDisplayInfo.nCur=nPos;
	if(nMax!=-1)
		gDisplayInfo.nMax=nMax;
	::PostMessage(hwnd,DISPLAY_UPDATE,NULL,NULL);
	Sleep(2);
}

unsigned __stdcall CInitInfoDialog::LoadInfoProc(void* pParam)
{
	
	CInitInfoDialog* pInfoDialog = (CInitInfoDialog*)pParam;

	//装载客户端资源
	if(LoadClientResource(pInfoDialog)==false)
	{
		//if(pInfoDialog->m_bReadError==true)
		//{
		//	//要校验文件
		//	::PostMessage(pInfoDialog->m_hWnd,WM_COMMAND,IDOK,NULL);
		//}
		_endthreadex( 0 );
		return 0;
	}

	if(bExit==true)
		return false;

	if(LoadUpdateServerResource(pInfoDialog)==false)
	{
		_endthreadex( 0 );
		return 0;
	}

	pInfoDialog->m_bIDOK = true;
	::PostMessage(pInfoDialog->m_hWnd,WM_COMMAND,IDOK,NULL);
	_endthreadex( 0 );
	return 0;
}

//装载客户端资源
bool	CInitInfoDialog::LoadClientResource(CInitInfoDialog* pInfoDialog)
{
	if(pInfoDialog==NULL || pInfoDialog->GetClientResource() == NULL)	return false;

	CClientResource* pClientResour = pInfoDialog->GetClientResource();

	int nMax = 1;
	int nPos = 0;
	SetDisplayInfo(pInfoDialog->m_hWnd,"读取客户端文件信息...",nPos,nMax);

	if(pClientResour->LoadExForAutoUpdate()==false)
	{
		SetDisplayInfo(pInfoDialog->m_hWnd,"读取客户端重要文件信息出错，请重新尝试。[官方主页 www.saga3d.com]",nMax,nMax);

		AfxMessageBox("自动更新失败，您可以到 www.saga3d.com 去下载最新的补丁包文件。");
		ShellExecute( 0, 0, "http://www.saga3d.com", 0, 0, SW_SHOWNORMAL );

		pInfoDialog->m_bReadError=true;
		return false;
	}

	SetDisplayInfo(pInfoDialog->m_hWnd,"读取客户端文件信息...",nMax,nMax);

	char strVersion[250];
	sprintf(strVersion,"当前游戏版本号为%s",pClientResour->GetFilesInfo()->GetVersion().c_str());
	pInfoDialog->SetWindowText(strVersion);
	return true;;
}

bool CInitInfoDialog::LoadUpdateServerResource(CInitInfoDialog* pInfoDialog)
{
	if(pInfoDialog==NULL || pInfoDialog->GetClientResource() == NULL)	return false;

	CClientResource* pClientResour = pInfoDialog->GetClientResource();

	if(bExit==true)	return false;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SetDisplayInfo(pInfoDialog->m_hWnd,"获取服务器列表文件...",0,2);
	
	if(::InternetAttemptConnect(0) != ERROR_SUCCESS)
	{
		SetDisplayInfo(pInfoDialog->m_hWnd,"连接更新服务器失败，请检查您的网络连接是否正常。[官方主页 www.saga3d.com]",1,1);
		return false;
	}

	string strTempt = pClientResour->GetCurFolder();
	strTempt += "\\setup\\Updatehostlist.ini"; 

	FILE* pf = fopen(strTempt.c_str(),"rb");
	if(!pf)
	{
		CString str("获取更新服务器列表文件出错。请到 www.saga3d.com 下载升级包，升级后进入游戏。");
		SetDisplayInfo(pInfoDialog->m_hWnd,str,1,1);
		return false;
	}

	CPing ping;
	string strUpdateServer("");
	long lTimeOut = 2000;
	multimap<long,tagServer> UpdateServerList;

	string str; //存储文件中读取的字符串
	//从文件获取更新服务器列表
	char strbuf[100];
	string strch,strtmp;
	while(!feof(pf))
	{
	   fscanf(pf,"%s\n",strbuf);
	   strtmp = strbuf;
	   strch+=strtmp;
	}
	fclose(pf);
	str = strch;
	int Sever = 0;   //0表示选择电信服务器，1表示网通服务器
	int Pos = str.find_first_of("@");    //@为电信和网通服务器的分界符
	if(Sever == 0)
	{
		//cout<<"电信服务器："<<endl;
		str.erase(Pos,str.length()-Pos);  //读出电信服务器的段
	}
	else if(Sever == 1)
	{
		//cout<<"网通服务器："<<endl;
		str.erase(0,Pos+1);   //读出网通服务器的段

	}
	int nPos=-1;
    string strTemp(""); 
	string	strServer("");
	string	strPath("");
	string strVersion("");
	bool isSelect = false;
	DWORD dwFileInfoCRC;
	//循环读出选择段中每一个服务器的IP和Path
	SetDisplayInfo(pInfoDialog->m_hWnd,"连接更新服务器...",1,2);
	while(str.find_first_of(",")!=-1)
	{
		nPos=str.find_first_of(",");

		strTemp = str;
		strTemp.erase(0,nPos+1);
		str.erase(nPos,str.length()-nPos);
		string strPathTemp,strIpTemp;

		int npos=str.find_first_of("#");
		strPathTemp = str;
		strPathTemp.erase(npos,str.length()-npos);
		strIpTemp = str;
		strIpTemp.erase(0,npos+1);

		tagServer Server = {strIpTemp,strPathTemp};
		//得到ping值 ,寻找最快更新服务器
		long lRet = ping.Ping((char*)strIpTemp.c_str(),1,2000);
		if(lRet != -1 && lRet < 100)
		{
			strServer = strIpTemp;
			strPath = strPathTemp;
			isSelect = true;
			break;
		}else if(lRet!=-1&&lRet<lTimeOut)
		{
			UpdateServerList.insert(make_pair(lRet,Server));
		}else{
			srand((unsigned)time(NULL));
			int nRandom  = rand()+2000;
			UpdateServerList.insert(make_pair(nRandom,Server));
		}
		str = strTemp;
	}

	if(UpdateServerList.size()<=0&&isSelect==false)
	{
		CString str("获取更新服务器列表文件出错。");
		SetDisplayInfo(pInfoDialog->m_hWnd,str,1,1);
		return false;
	}
	//找出ping值最小的更新服务器
	if(!isSelect)
	{
			multimap<long,tagServer>::iterator it = UpdateServerList.begin();
			long minRel = (*it).first;
			strServer = (*it).second.strIP;
			strPath = (*it).second.strPath;
			for(;it != UpdateServerList.end();it++)
			{
				if((*it).first<minRel)
				{
					minRel = (*it).first;
					strServer = (*it).second.strIP;
					strPath = (*it).second.strPath;
				}
			}
	}

	HINTERNET hSession = ::InternetOpen(_T("Tnt"),INTERNET_OPEN_TYPE_PRECONFIG,_T(""),
		INTERNET_INVALID_PORT_NUMBER,0);
	if(hSession == NULL)
	{		
		SetDisplayInfo(pInfoDialog->m_hWnd,"连接更新服务器失败，请检查您的网络连接是否正常。[官方主页 www.saga3d.com]",1,1);
		return false;
	}
	SetDisplayInfo(pInfoDialog->m_hWnd,"检查更新...",2,2);
	if(bExit==true)	return false;
	HINTERNET hUpdateServerConnect=NULL;

	int nAllPro = UpdateServerList.size()+2;
	SetDisplayInfo(pInfoDialog->m_hWnd,"检查更新...",1,nAllPro);
	if(GetServerVersionFromOneGameServer(pInfoDialog,hSession,strServer,strPath,
			hUpdateServerConnect,dwFileInfoCRC,strVersion) == false)
	{
		char strTempt[256] = "连接更新服务器失败，请检查您的网络连接是否正常。[官方主页 www.saga3d.com]";
		SetDisplayInfo(pInfoDialog->m_hWnd,strTempt,1,1);
		return false;
	}

	SetDisplayInfo(pInfoDialog->m_hWnd,"检查更新...",nAllPro-1,nAllPro);
	string strOldVersion = pClientResour->GetFilesInfo()->GetVersion();
	//版本一样退出,进入设置界面
	if(strVersion==strOldVersion)
	{
		SetDisplayInfo(pInfoDialog->m_hWnd,"当前是最新版本。",nAllPro,nAllPro);
		return	true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(bExit==true)	return false;
	//为当前客户端版本设置服务器的更新目录,包含客户端版本号信息
	strPath = strPath + "/Update-" + strOldVersion;

    SetDisplayInfo(pInfoDialog->m_hWnd,"从服务器得更新列表...",1,2);

	//CFilesInfo ServerFilesInfo,TemptFilesInfo;;
	string ServerFilesName("");
	string ServerFilesCRC("");

	string strFileName1 = strPath;
	strFileName1 +="/FilesInfo.txt";
	string strLocalFileName = pInfoDialog->GetWorkPath();
	strLocalFileName += "\\UpdateFilesInfo.ril";
	if(!GetUpdateFileInfosEx(hUpdateServerConnect,strFileName1,strLocalFileName,ServerFilesCRC,ServerFilesName))
	{
		SetDisplayInfo(pInfoDialog->m_hWnd,"下载更新列表出错，请再次尝试连接，或者到 www.saga3d.com 下载升级包。",2,2);
	 	return false;
	}

	int nTotalSize = 0;

	//建立一个零时文件，用来暂存下载的文件，也同时完成端点续传
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(bExit==true)	return false;

	BYTE *pBuf = new BYTE[GET_BUFFER_MAX];
	DWORD dwBufLen = GET_BUFFER_MAX;
	int nCount=2;
	//先尝试续传文件
	SetDisplayInfo(pInfoDialog->m_hWnd,"下载文件...",0,nCount);
	string ServerFile = strPath+"/"+ServerFilesName;  //计算更新文件在服务器上的完整路径
	if(ContinueDownLoadEx(hUpdateServerConnect,strOldVersion,pBuf,dwBufLen,ServerFile,nTotalSize,pInfoDialog->m_hWnd))
	{
		string ServerTemp = "update\\Game"+strOldVersion+"Patch.exe";
		SetDisplayInfo(pInfoDialog->m_hWnd,"下载完毕.",2,2);
		WinExec(ServerTemp.c_str(),SW_SHOWNORMAL);
		bExit = true;
		::PostMessage(pInfoDialog->m_hWnd,WM_CLOSE,NULL,NULL);  //下载升级包成功关闭窗口
		_endthreadex( 0 );
		return true;
	}
	else{
		if(DownLoadEXEFile(hUpdateServerConnect,strOldVersion,pBuf,dwBufLen,ServerFile,nTotalSize,pInfoDialog->m_hWnd))
		{
		   string ServerTemp = "update\\Game"+strOldVersion+"Patch.exe";
		   SetDisplayInfo(pInfoDialog->m_hWnd,"下载完毕.",2,2);
		   WinExec(ServerTemp.c_str(),SW_SHOWNORMAL);
		   bExit = true;
		   ::PostMessage(pInfoDialog->m_hWnd,WM_CLOSE,NULL,NULL);  //下载升级包成功关闭窗口
		   _endthreadex( 0 );
		   return true;
		}
		else{
			SetDisplayInfo(pInfoDialog->m_hWnd,"文件更新发生错误，请再次尝试连接，或者到 www.saga3d.com 下载升级包。",nCount,nCount);
			return false;
		}
	}
	//return true;
}

bool CInitInfoDialog::GetServerVersionFromOneGameServer(CInitInfoDialog* pInfoDialog,HINTERNET hSession,
														string& strUrl1,string& strServerName,
														HINTERNET& hUpdateServerConnect,DWORD& dwFileInfoCRC,string& strVersion)
{
	//尝试是否能打开服务器
	string strUrl = "http://"+strUrl1;
	strUrl +=strServerName;
	HINTERNET hOpen = ::InternetOpenUrl(hSession,strUrl.c_str(),NULL,0,INTERNET_FLAG_EXISTING_CONNECT,NULL);
	if(hOpen)
	{
		InternetCloseHandle(hOpen);
		HINTERNET hConnect = ::InternetConnect(hSession,strUrl1.c_str(),
			INTERNET_INVALID_PORT_NUMBER,_T(""),_T(""),INTERNET_SERVICE_HTTP,0,0);
		if(hConnect)
		{
			hUpdateServerConnect = hConnect;
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

	string strFileName = strServerName;
	//strFileName +="/AutoUpdateGame.vs";
	strFileName +="/AutoUpdateGame.txt";
	string strLocalVersionFileName = pInfoDialog->GetWorkPath();
	strLocalVersionFileName += "\\AutoUpdateGame.vs";
	if(GetServerVersion(hUpdateServerConnect,strFileName,strLocalVersionFileName,dwFileInfoCRC,strVersion)==false)
	{
		InternetCloseHandle(hUpdateServerConnect);
		return false;
	}
	return true;
}
//得到服务器游戏版本
bool CInitInfoDialog::GetServerVersion(HINTERNET hConnect,string& strPathName,string& strLocalVersionName,
									   DWORD& dwFileInfoCRC,string& strVersion)
{

	HINTERNET hHttpFile = ::HttpOpenRequest(hConnect,_T("GET"),
		strPathName.c_str(),HTTP_VERSION,NULL,0,
		INTERNET_FLAG_RELOAD,0);
	if(hHttpFile == NULL)
	{
		return false;
	}
	bool bSendRequest = ::HttpSendRequest(hHttpFile,NULL,0,0,0);
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
		return	true;
	}
	else
	{
		DWORD dwError = GetLastError();
		return false;
	}
	//return	false;
}

//得到服务器更新文件信息
bool	CInitInfoDialog::GetUpdateFileInfosEx(HINTERNET hConnect,string& strPathName,string& strLocalFileName,
											string &dwFileInfoCRC,string &ServerFilesName)
{
	if(hConnect==NULL)	return false;

	

	//if(!::FtpGetFile(hConnect,strPathName.c_str(),strLocalFileName.c_str(),FALSE,FILE_ATTRIBUTE_NORMAL,FTP_TRANSFER_TYPE_BINARY |
	//	INTERNET_FLAG_NO_CACHE_WRITE,0))
	//{
	//	//InternetCloseHandle(hFtpFile);
	//	return false;
	//}
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
		DWORD dwContentLen;
        DWORD dwBufLen = sizeof(dwContentLen);

		BOOL	bQuery = ::HttpQueryInfo(hHttpFile,HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER,(LPVOID)&dwContentLen,&dwBufLen,NULL);

		if(!bQuery)
		{
			::InternetCloseHandle(hHttpFile);
			return false;
		}
		
		DWORD	dwFileSize = dwContentLen;
		FILE* pf = fopen(strLocalFileName.c_str(),"wb");
		if(!pf)
		{
			::InternetCloseHandle(hHttpFile);
			return	false;
		}
		//dwFileSize = (DWORD) atol(achQueryBuf);				
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
		//从临时下载文件中读取更新资源包名字和CRC码
		FILE* pf_read = fopen(strLocalFileName.c_str(),"rb");
		char strbuf[100];
		fscanf(pf_read,"%s\n",strbuf);
		ServerFilesName = strbuf;
		fscanf(pf_read,"%s\n",strbuf);
		dwFileInfoCRC = strbuf;
		fclose(pf_read);
		string strfile = "UpdateFilesInfo.ril"; 
		::DeleteFile(strfile.c_str());	
		return	true;
	}
	return false;
}

bool    CInitInfoDialog::ContinueDownLoadEx(HINTERNET hConnect,string strOldVersion,BYTE* pBuf,DWORD& dwBufLen,
		                        string& strFtpPath,int& nTotalSize,HWND hwnd)
{
	if(hConnect==NULL)
		return false;
	//打开update目录下的临时文件，如果失败，退出断点续传
	FILE *fp_date = fopen("update//updateflie.fc","rb");
	if(fp_date==NULL)
	{
		return false;
	}
	//如果打开成功，说明有上次未下载完成的文件
	DWORD dwCount=0;
	fread(&dwCount,1,sizeof(DWORD),fp_date);
	fclose(fp_date);
	if(dwCount==0)
	{
		//读取错误删除该文件
		//string strFullFileName = pInfoDialog->GetWorkPath();
	    string strFullFileName = "update\\updateflie.fc";
	    ::DeleteFile(strFullFileName.c_str());
	}
	//string strOldVersion = pClientResour->GetFilesInfo()->GetVersion();
	//检查文件夹中是否存在未下载完的.exe的升级文件，如果不存在，跳出断点续传
	string UpdateFile = "update\\Game"+strOldVersion+"Patch.exe";
	FILE *fp_Temp = fopen(UpdateFile.c_str(),"rb");
	if(fp_Temp==NULL)
	{
		return false;
	}
	fclose(fp_Temp);
	FILE *fp_f = fopen(UpdateFile.c_str(),"wb");
	if(fp_f==NULL)
	{
		return false;
	}
	//将文件指针指向已下载文件的末尾
	fseek(fp_f,dwCount,0);
	nTotalSize = dwCount;
	HINTERNET hHttpFile = ::HttpOpenRequest(hConnect,_T("GET"),
			strFtpPath.c_str(),HTTP_VERSION,NULL,0,
			INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_DONT_CACHE |
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_KEEP_CONNECTION,0);
	if(hHttpFile == NULL)	return false;

	BOOL bSendRequest = ::HttpSendRequest(hHttpFile,NULL,0,0,0);
	if(!bSendRequest)	return false;

	DWORD dwContentLen;
	DWORD dwBufLenth = sizeof(dwContentLen);
	bool bQuery =::HttpQueryInfo(hHttpFile, 
			  HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
			  (LPVOID)&dwContentLen,
			  &dwBufLenth,
			  0);

	if(!bQuery)
	{
		::InternetCloseHandle(hHttpFile);
		return false;
	}
	DWORD	dwServerFileSize = dwContentLen;//(DWORD) atol(achQueryBuf);
	//设置传输的起始点
	DWORD nt = InternetSetFilePointer(hHttpFile,dwCount,NULL,FILE_BEGIN,0);
	if(nt==-1||nt!=dwCount)
	{
		//不支持断点续传
		::InternetCloseHandle(hHttpFile);
		string strFullFileName = "update\\updateflie.fc";
	    ::DeleteFile(strFullFileName.c_str());
		//::DeleteFile(UpdateFile.c_str());
		fclose(fp_f);
		//pPckage->ClearData();
		return false;
	}
	nTotalSize = dwCount;
	DWORD	dwBytesRead = 0;
	BOOL	bRead;
	DWORD   dwFileSize = dwServerFileSize;
	DWORD   dwRequest =
		( GET_BUFFER_MAX > dwFileSize - dwCount ) ? dwFileSize - dwCount : GET_BUFFER_MAX;
	while(dwRequest > 0)
	{
		bRead = ::InternetReadFile(hHttpFile,pBuf,dwRequest,&dwBytesRead);

		if(!bRead || bExit == true)
		{
			return false;
		}
		if(dwBytesRead == 0)
			break;
		fwrite(pBuf,dwBytesRead,1,fp_f);   //将下载的数据流存入到文件中
		dwCount += dwBytesRead;
		FILE *fp_d = fopen("update//updateflie.fc","wb");
		if(fp_d==NULL) return false;
		fwrite(&dwCount,1,sizeof(DWORD),fp_d);
		fclose(fp_d);
		if ( dwFileSize - dwCount < GET_BUFFER_MAX )
			dwRequest = dwFileSize - dwCount;

		//显示进度
		nTotalSize += dwBytesRead;
		CString strFileSize("");
		strFileSize.Format("下载文件...                                                        %d/%d",nTotalSize,dwFileSize);
		SetDisplayInfo(hwnd,strFileSize,nTotalSize,dwFileSize);
		//SetDisplayInfo(hwnd,nTotalSize,dwFileSize);
	}
	fclose(fp_f);
	return true;

}

bool    CInitInfoDialog::DownLoadEXEFile(HINTERNET hConnect,string strOldVersion,BYTE* pBuf,DWORD& dwBufLen,
		                        string& strFtpPath,int& nTotalSize,HWND hwnd)
{
	if(hConnect==NULL)
       return false;
	//string strOldVersion = pClientResour->GetFilesInfo()->GetVersion();
	string UpdateFile = "update\\Game"+strOldVersion+"Patch.exe";
	FILE *fp_f = fopen(UpdateFile.c_str(),"wb");
	if(fp_f==NULL)
	{
		return false;
	}
	nTotalSize = 0;
	DWORD dwCount=0;
	HINTERNET hHttpFile = ::HttpOpenRequest(hConnect,_T("GET"),
			strFtpPath.c_str(),HTTP_VERSION,NULL,0,
			INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_DONT_CACHE |
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_KEEP_CONNECTION,0);
	if(hHttpFile == NULL)	return false;

	BOOL bSendRequest = ::HttpSendRequest(hHttpFile,NULL,0,0,0);
	if(!bSendRequest)	return false;

	DWORD dwContentLen;
    DWORD dwBufLenth = sizeof(dwContentLen);
	bool bQuery =::HttpQueryInfo(hHttpFile, 
              HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
              (LPVOID)&dwContentLen,
              &dwBufLenth,
              0);

	if(!bQuery)
	{
		::InternetCloseHandle(hHttpFile);
		return false;
	}
	DWORD	dwServerFileSize = dwContentLen;//(DWORD) atol(achQueryBuf);
	DWORD   dwFileSize = dwServerFileSize;
	DWORD	dwBytesRead = 0;
	BOOL	bRead;
	DWORD   dwRequest =
		( GET_BUFFER_MAX > dwFileSize - dwCount ) ? dwFileSize - dwCount : GET_BUFFER_MAX;
	while(dwRequest > 0)
	{
		bRead = ::InternetReadFile(hHttpFile,pBuf,dwRequest,&dwBytesRead);

		if(!bRead || bExit == true)
		{
			return false;
		}
		if(dwBytesRead == 0)
			break;
		fwrite(pBuf,dwBytesRead,1,fp_f);   //将下载的数据流存入到文件中
		dwCount += dwBytesRead;
		//将已下载的大小存入.fc文件中以便断点续传
		FILE *fp_d = fopen("update//updateflie.fc","wb");
		if(fp_d==NULL) return false;
		fwrite(&dwCount,1,sizeof(DWORD),fp_d);
		fclose(fp_d);
		if ( dwFileSize - dwCount < GET_BUFFER_MAX )
			dwRequest = dwFileSize - dwCount;

		//显示进度
		nTotalSize += dwBytesRead;
		CString strFileSize("");
		strFileSize.Format("下载文件...                                                       %d/%d",nTotalSize,dwFileSize);
		SetDisplayInfo(hwnd,strFileSize,nTotalSize,dwFileSize);
		//SetDisplayInfo(hwnd,nTotalSize,dwFileSize);
	}
	string strFullFileName = "update\\updateflie.fc";
	::DeleteFile(strFullFileName.c_str());
	fclose(fp_f);
	return true;
}

void CInitInfoDialog::OnOK()
{
	if(m_bIDOK == false)
		return;
	// TODO: 在此添加::GetMouse
	if(m_bReadError==true)
		Release();
	CDialog::OnOK();
}

//BOOL CInitInfoDialog::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	
//
//	return CDialog::OnEraseBkgnd(pDC);
//}

void CInitInfoDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

	CRect rect;
	GetClientRect(&rect);

	////创建新字体
	CFont myFont;
	myFont.CreatePointFont(90,"宋体");

	dc.SelectObject(&myFont);
	dc.SetTextColor(RGB(0,0,0));

	//创建CHECT说明的矩形窗口
	CRect myRect;
	myRect.left=5;
	myRect.top=310;
	myRect.right=500;
	myRect.bottom=325;

	//采用透明的方式在屏幕上写字
	dc.SetBkMode(TRANSPARENT);
	dc.DrawText(m_strDisplayInfo,&myRect,DT_LEFT);
}

void CInitInfoDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x, point.y));

	CDialog::OnLButtonDown(nFlags, point);
}
