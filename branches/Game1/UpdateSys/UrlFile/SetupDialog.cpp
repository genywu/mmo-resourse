// SetupDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "urlfile.h"
#include "SetupDialog.h"
#include ".\setupdialog.h"
#include "hintinfo.h"
#include "SetupPage.h"
#include "CheckFilesDialog.h"
#include "Ping.h"
#include "../../PUBLIC/Crc32Static.h"
#include "strsafe.h"
#include "../../PUBLIC/ziplib/zlib.h"
#include "../../PUBLIC/ziplib/unzip.h"
#include "../../public/tools.h"

#include "changebmp.h"
//#define WINAPI _stdcall


#define DISPLAY_UPDATE	WM_USER+6000
#define IN_GAME	WM_USER+6001
#define UPDATE_OK WM_USER+6002
#define UPDATE_INGAME WM_USER+6003
#define UPDATE_ERROR WM_USER+6004
#define UPDATE_PATCH WM_USER+6005
#define CHECK_VERSION WM_USER+6006
#define DOWNFILE_UPDATE WM_USER+6007
#define DEDOWNFILE_UPDATE WM_USER+6008

#define GET_BUFFER_MAX 1024 * 16
tagDisplayInfo	g_DisplayInfo;
HANDLE h_VersonThread=0;
HANDLE h_InfoThread=0;
HANDLE hThread=0;
bool b_Exit = false;
int CSetupDialog::m_Sever = 0;
CString CSetupDialog::m_strDisplayInfoEx="";
string  CSetupDialog::m_strPathName="";
static DWORD g_dwCurSize;
static DWORD g_dwFileSize;

HINSTANCE g_hInstSDLoadDLL = 0;
typedef int(_stdcall * LoadSDPlugin)(LPCSTR lpBuf,int nBufLen);
typedef int(_stdcall * UnLoadSDPlugin)(void);

LoadSDPlugin loadSDPlugin;
UnLoadSDPlugin unloadSDPlugin;

struct tagServer
{
	string	strIP;
	string	strPath;
};

struct stServer
{
	char strServerIP[32];
	long wServerPort;
	char strServerName[64];
};

list<stServer> g_ServerList;

bool ReadTo(istream& stream, const char* name)
{
	char str[256];

	stream >> str;
	while (strcmp(str,name)!=0)
	{
		if (stream.eof())
		{
			return false;
		}		
		stream >> str;
	}
	return true;
}
// CSetupDialog 对话框

IMPLEMENT_DYNAMIC(CSetupDialog, CDialog)
CSetupDialog::CSetupDialog(string& strWorkPath,string& strPackageName,string& strFileInfoName,CWnd* pParent /*=NULL*/)
	: CDialog(CSetupDialog::IDD, pParent)
	,m_strWorkPath(strWorkPath)
	,m_strPackageName(strPackageName)
	,m_strFilesInfoName(strFileInfoName)
	,m_pPatchFilesInfo(NULL)
	,m_pPatchPackage(NULL)
	,m_pClientResource(NULL)
{
	IniHintWeb();
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIconBig = AfxGetApp()->LoadIcon(IDI_ICON);
	m_hIconMin = AfxGetApp()->LoadIcon(IDI_ICON);

	m_pBitmapbk = NULL;
	m_mapButtonRect.clear();
}

CSetupDialog::~CSetupDialog()
{
	if(m_pClientResource )
	{
		delete m_pClientResource;
		m_pClientResource=NULL;
	}
	if(m_pPatchFilesInfo)	delete m_pPatchFilesInfo;
	if(m_pPatchPackage) delete m_pPatchPackage;

	CClientResource* pClientResource = CClientResource::GetInstance();
	pClientResource->Release();
	pClientResource->ReleaseInstance();

#ifndef	_DEBUG
	DeleteFile(m_strTemptPackageName.c_str());
	DeleteFile(m_strTemptFilesInfoName.c_str());
	::RemoveDirectory(m_strExePath.c_str());
#endif
}

void CSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_COMBO_SERVERLIST, m_ServerList);
	DDX_Control(pDX, IDC_HINTINFO, m_webHintInfo);
	//DDX_Control(pDX, IDC_HINTINFO2, m_webHintInfo2);
	DDX_Control(pDX, IDOK, m_bnPlayGame);
	DDX_Control(pDX, IDC_BUTTON_SYSSET, m_bnSysSet);
	DDX_Control(pDX, IDC_BUTTON_CHECKALLFILE, m_bnCheckAllFile);
	DDX_Control(pDX, IDCANCEL, m_bnQuit);
	DDX_Control(pDX, IDC_BUTTON_LOGIN, m_bnSelectUpdate);

	//DDX_Control(pDX, IDC_BUTTON_CHECKSYSCON, m_bnCheckSysCon);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_BUTTON_LONGING, m_bnLoading);
//	DDX_Control(pDX, IDC_BUTTON_CACEL, m_bnExit);
	DDX_Control(pDX, IDC_BUTTON_WIRE, m_bnServerWire);
	DDX_Control(pDX, IDC_BUTTON_NET, m_bnServerNet);
	DDX_Control(pDX,IDC_BUTTON5_CHOOSER,m_bnServerChoose);
    DDX_Control(pDX,IDC_BUTTON_CHOOSER2,m_bnServerChoose2);

	DDX_Control(pDX,IDC_BUTTON_FIRSTPAGE,m_bnFirstPage);
	DDX_Control(pDX,IDC_BUTTON_NEW,m_bnNEW);
	DDX_Control(pDX,IDC_BUTTON_ZHANGHAO,m_bnZhangHao);
	DDX_Control(pDX,IDC_BUTTON_DOWNLOAD,m_bnDownLoad);
	DDX_Control(pDX,IDC_BUTTON_KEFU,m_bnKeFu);
	DDX_Control(pDX,IDC_BUTTON_LUNTAN,m_bnLunTan);
	
}


BEGIN_MESSAGE_MAP(CSetupDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SYSSET, OnBnClickedButtonSysset)
	ON_BN_CLICKED(IDC_BUTTON_CHECKALLFILE, OnBnClickedButtonCheckallfile)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVERLIST, OnCbnSelchangeComboServerlist)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
//	ON_WM_ERASEBKGND()
ON_BN_CLICKED(IDC_BUTTON_LOGIN, OnBnClickedButtonLogin)
//ON_BN_CLICKED(IDC_BUTTON_CHECKSYSCON, OnBnClickedButtonChecksyscon)
//ON_WM_MOUSEMOVE()
ON_BN_CLICKED(IDOK, &CSetupDialog::OnBnClickedOk)
//ON_BN_CLICKED(IDC_BUTTON_NEWPLAYER, &CSetupDialog::OnBnClickedButtonNewplayer)
ON_BN_CLICKED(IDCANCEL, &CSetupDialog::OnBnClickedCancel)
ON_BN_CLICKED(IDC_BUTTON_CACEL, &CSetupDialog::OnBnClickedButtonCacel)
ON_BN_CLICKED(IDC_BUTTON_WIRE, &CSetupDialog::OnBnClickedButtonWire)
ON_BN_CLICKED(IDC_BUTTON_NET, &CSetupDialog::OnBnClickedButtonNet)
ON_BN_CLICKED(IDC_BUTTON_NEW, &CSetupDialog::OnBnClickedButtonNew)
ON_BN_CLICKED(IDC_BUTTON_FIRSTPAGE, &CSetupDialog::OnBnClickedButtonFirstpage)
ON_BN_CLICKED(IDC_BUTTON_ZHANGHAO, &CSetupDialog::OnBnClickedButtonZhanghao)
ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CSetupDialog::OnBnClickedButtonDownload)
ON_BN_CLICKED(IDC_BUTTON_KEFU, &CSetupDialog::OnBnClickedButtonKefu)
ON_BN_CLICKED(IDC_BUTTON_LUNTAN, &CSetupDialog::OnBnClickedButtonLuntan)
END_MESSAGE_MAP()


// CSetupDialog 消息处理程序

BOOL CSetupDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//GetDlgItem(IDOK)->EnableWindow(!m_bReadError);

	//m_bnPlayGame.SetFocus();

	SetIcon(m_hIconBig, TRUE);			// 设置大图标
	SetIcon(m_hIconMin, FALSE);		// 设置小图标
	//设置对话框及其控件相关位置
	int cy = ::GetSystemMetrics(SM_CYSCREEN) / 2; 
	int cx = ::GetSystemMetrics(SM_CXSCREEN) / 2; 
	cx = cx - 769/2;
	cy = cy - 688/2;	

	m_pClientResource = new CClientResource(GAME_RES,m_strWorkPath,m_strFilesInfoName);

	CClientResource::InitialInstance();
	CClientResource* pClientResource = CClientResource::GetInstance();
	pClientResource->Initial(GAME_RES,m_strWorkPath,m_strFilesInfoName);
	if(!pClientResource->LoadEx())
	{
		//装载客户端资源信息出错
		//ASSERT(false);
		//return false;
	}

	if(!m_Interface.LoadInterfaceXml())
	{
		return false;
	}
	if(!CStringRead::LoadTextresource())
	{
		return false;
	}
	CWnd* pMainWnd = FindWindow("OnlineCLIENT",NULL);
	if (pMainWnd != NULL)
	{
		AfxMessageBox(_T(CStringRead::GetString(1).c_str()));
		//::PostMessage(this->m_hWnd,WM_CLOSE,NULL,NULL);
		return false;
	}
	MoveWindow(cx,cy,m_Interface.GetLauncherBack().width,m_Interface.GetLauncherBack().height);
	HBITMAP hbk = (HBITMAP) ::LoadImageA(AfxGetInstanceHandle(),m_Interface.GetLauncherBack().ImagePath.c_str(),IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(hbk!=NULL)
		m_pBitmapbk = CBitmap::FromHandle(hbk);
	m_webHintInfo.MoveWindow(m_Interface.GetLauncherBack().webX,m_Interface.GetLauncherBack().webY,m_Interface.GetLauncherBack().webW,m_Interface.GetLauncherBack().webH);

	SetWindowText(CStringRead::GetString(2).c_str());//设置对话框的标题

	CPaintDC dc(this); // device context for painting
	SetupRgn(&dc);
	LPUNKNOWN   pUnk   =   m_webHintInfo.GetControlUnknown();   
    if(pUnk   !=   NULL)   
    {   
          IWebBrowser2*   pWB2   =   NULL;   
          HRESULT   hr   =   pUnk->QueryInterface(IID_IWebBrowser2,   (void**)&pWB2);   
          if(SUCCEEDED(hr)   &&   pWB2   !=   NULL)   
          {   
                m_webUICtrl.Enable3DBorder(FALSE);           //   此代码禁止三维边框   
                m_webUICtrl.EnableScrollBar(FALSE);   //   此代码禁止滚动条   
                m_webUICtrl.SetWebBrowser(pWB2);   
                pWB2->Release();   
          }   
     }   
	m_webHintInfo.Navigate("about:blank",NULL,NULL,NULL,NULL);

	m_webHintInfo.Navigate(m_strwebLink[0].c_str(),FALSE,FALSE,FALSE,FALSE);
	m_webHintInfo.SetForegroundWindow();;
	m_bnQuit.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnPlayGame.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnSelectUpdate.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnSysSet.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE); 
	m_bnLoading.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnServerWire.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnServerNet.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnServerChoose.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnServerChoose2.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnFirstPage.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnNEW.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnZhangHao.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnDownLoad.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnKeFu.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);
	m_bnLunTan.LoadBitmaps(IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE,IDB_BITMAP_BACKPAGE);

	//LoadBitmapFromFile();
	InitInterface();
	m_bnPlayGame.EnableWindow(0);

	c_rectLoading = m_mapButtonRect["Loading"];


	m_Progress.SetBkColor(0x000000);

	m_Progress.ShowWindow(0);
	m_bnPlayGame.ShowWindow(0);
	m_bnServerChoose.EnableWindow(0);
	m_bnServerWire.EnableWindow(0);
	m_bnServerChoose2.EnableWindow(0);
	m_bnServerChoose2.ShowWindow(0);
	m_bnSelectUpdate.EnableWindow(0);
	unsigned threadID;
	h_VersonThread = (HANDLE)_beginthreadex(NULL,0,LoadVersion,this,0,&threadID);
	UpdateData(false);
	LoadSdoLoad();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSetupDialog::OnDestroy()
{
	CDialog::OnDestroy();
	m_webUICtrl.SetWebBrowser(NULL);
}

void CSetupDialog::OnBnClickedButtonSysset()
{
	// TODO: 在此添加控件通知处理程序代码
	CSetupPage dlg;
	dlg.DoModal();
}

void CSetupDialog::OnBnClickedButtonCheckallfile()
{
	return;
}

//初始化信息提示web,如果internet webpage不可用，用本地webpage;
void CSetupDialog::IniHintWeb(void)
{
	for(int i=0;i<7;i++)
	{
		m_strwebLink[i]="";
	}
	FILE* pf = fopen("setup/hintserver.ini","rb");
	if(pf==NULL)
	{
		return;
	}
	char strbuf[200]= "";
	string strch(""),strTemp("");
	int i=0;
	while(!feof(pf))
	{
	   fscanf(pf,"%s\n",strbuf);
	   strTemp = strbuf;
	   if(i<7)
	   {
		   m_strwebLink[i] = strTemp;
	   }
	   i++;
	}
	fclose(pf);
}

void CSetupDialog::OnCbnSelchangeComboServerlist()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	ofstream stream;
	stream.open("setup/default.dat");
	if( stream.is_open() )
	{
		//stream << "ServerNum" << '\t' << m_ServerList.GetCurSel() << '\n';
	}
	stream.close();

	m_webHintInfo.SetFocus();
}

void CSetupDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	CRect rect;
	GetClientRect(&rect);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap *bpt=m_pBitmapbk;
	if(bpt!=NULL)
	{	
		//bpt.LoadBitmap(IDB_BITMAP_BACKPAGE);
	    BITMAP bm;
	    bpt->GetBitmap(&bm);
	    memDC.SelectObject(bpt);
	    dc.BitBlt(0,0, bm.bmWidth , bm.bmHeight , &memDC, 0, 0, SRCCOPY);

	}



	//创建新字体
	CFont myFont;
	myFont.CreatePointFont(90,"宋体"); 

	dc.SelectObject(&myFont);
	//dc.SetTextColor(RGB(30,121,70));
	dc.SetTextColor(RGB(0,181,70));

	//创建CHECT说明的矩形窗口
	CRect myRect;
	myRect.left=245;
	myRect.top=540;
	myRect.right=452;
	myRect.bottom=575; 

	//采用透明的方式在屏幕上写字
	dc.SetBkMode(TRANSPARENT);
	dc.DrawText(m_strDisplayInfo,&myRect,DT_LEFT);
}

// //按图构造对话框形状
BOOL CSetupDialog::SetupRgn(CDC* pDC)
{
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap *bpt = m_pBitmapbk;
	if(bpt==NULL)
		return false;
	//bpt.LoadBitmap(IDB_BITMAP_BACKPAGE);
	BITMAP bm;
	bpt->GetBitmap(&bm);
	memDC.SelectObject(bpt);
	CRgn TempRgn, WndRgn;
	WndRgn.CreateRectRgn(0,0,0, 0);
	COLORREF MaskColor = memDC.GetPixel( 0, 0);

	bool bCombine = false;
	long lStartX = -1, lEndX = 0;
	for(int i=0; i<bm.bmHeight; i++)
	{
		for(int j=1; j<bm.bmWidth; j++)
		{
			if (MaskColor != memDC.GetPixel(j,i))
			{
				if (lStartX==-1)
					lStartX = j;
			}
			else
			{
				if (lStartX!=-1)
				{
					bCombine = true;
					lEndX = j;
				}				
			}

			if (j == bm.bmWidth-1 && lStartX != -1)
			{
				bCombine = true;
				lEndX = j;
			}

			if (bCombine)
			{
				TempRgn.CreateRectRgn(lStartX+1,i+1,lEndX+1,i+2);
				WndRgn.CombineRgn(&WndRgn, &TempRgn, RGN_OR);
				TempRgn.DeleteObject();

				bCombine = false;
				lStartX = -1;
			}
		}
	}

	SetWindowRgn(WndRgn, TRUE);
	//MoveWindow(100, 100, bm.bmWidth, bm.bmHeight);

	return TRUE;
}

void CSetupDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x, point.y));

	CDialog::OnLButtonDown(nFlags, point);
}


void CSetupDialog::OnBnClickedButtonLogin()
{
	//m_SetupServerPage.DoModal();
	unsigned threadID;
	h_InfoThread = (HANDLE)_beginthreadex(NULL,0,LoadInfoProc,this,0,&threadID);
	m_bnSelectUpdate.EnableWindow(0);
}


BOOL CSetupDialog::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_MOUSEMOVE:
		{
			BOOL bShow = FALSE;

			if (bShow)//GetFocus() == &m_ServerList)
				break;

			RECT rect;
			GetWindowRect(&rect);

			POINT pt;
			GetCursorPos(&pt);

			pt.x -= rect.left;
			pt.y -= rect.top;

			// 自动更新
			if (pt.x > m_mapButtonRect["SelectUpdate"].left && pt.x < m_mapButtonRect["SelectUpdate"].right &&
				pt.y > m_mapButtonRect["SelectUpdate"].top && pt.y < m_mapButtonRect["SelectUpdate"].bottom)
			{
				m_bnSelectUpdate.SetFocus();
				m_bnPlayGame.SetFocus();
			}
			// 系统设置
			else if (pt.x > m_mapButtonRect["SysSet"].left && pt.x < m_mapButtonRect["SysSet"].right &&
				pt.y > m_mapButtonRect["SysSet"].top && pt.y < m_mapButtonRect["SysSet"].bottom)
			{
				m_bnSysSet.SetFocus();
			}
			// 退出
			else if (pt.x > m_mapButtonRect["Quit"].left && pt.x < m_mapButtonRect["Quit"].right &&
				pt.y > m_mapButtonRect["Quit"].top && pt.y < m_mapButtonRect["Quit"].bottom)
			{
				m_bnQuit.SetFocus();
			}
			// 进入
			else if (pt.x > m_mapButtonRect["PlayGame"].left && pt.x < m_mapButtonRect["PlayGame"].right &&
				pt.y > m_mapButtonRect["PlayGame"].top && pt.y < m_mapButtonRect["PlayGame"].bottom)
			{
				m_bnPlayGame.SetFocus();
			}
			//首页
			else if(pt.x > m_mapButtonRect["FirstPage"].left && pt.x < m_mapButtonRect["FirstPage"].right &&
				pt.y > m_mapButtonRect["FirstPage"].top && pt.y < m_mapButtonRect["FirstPage"].bottom)
			{
				m_bnFirstPage.SetFocus();
			}
			//新闻中心
			else if(pt.x > m_mapButtonRect["NEW"].left && pt.x < m_mapButtonRect["NEW"].right &&
				pt.y > m_mapButtonRect["NEW"].top && pt.y < m_mapButtonRect["NEW"].bottom)
			{
				m_bnNEW.SetFocus();
			}
			//张号管理
			else if(pt.x > m_mapButtonRect["ZhangHao"].left && pt.x < m_mapButtonRect["ZhangHao"].right &&
				pt.y > m_mapButtonRect["ZhangHao"].top && pt.y < m_mapButtonRect["ZhangHao"].bottom)
			{
				m_bnZhangHao.SetFocus();
			}
			//下载管理
			else if(pt.x > m_mapButtonRect["DownLoad"].left && pt.x < m_mapButtonRect["DownLoad"].right &&
				pt.y > m_mapButtonRect["DownLoad"].top && pt.y < m_mapButtonRect["DownLoad"].bottom)
			{
				m_bnDownLoad.SetFocus();
			}
			//客服中心
			else if(pt.x > m_mapButtonRect["KeFu"].left && pt.x < m_mapButtonRect["KeFu"].right &&
				pt.y > m_mapButtonRect["KeFu"].top && pt.y < m_mapButtonRect["KeFu"].bottom)
			{
				m_bnKeFu.SetFocus();
			}
			//游戏论坛
			else if(pt.x > m_mapButtonRect["LunTan"].left && pt.x < m_mapButtonRect["LunTan"].right &&
				pt.y > m_mapButtonRect["LunTan"].top && pt.y < m_mapButtonRect["LunTan"].bottom)
			{
				m_bnLunTan.SetFocus();
			}
			else
			{
				m_webHintInfo.SetFocus();
			}

		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSetupDialog::SetDisplayInfo(HWND hwnd,CString strText,int nPos,int nMax)
{
	g_DisplayInfo.strText=strText;
	g_DisplayInfo.nCur=nPos;
	if(nMax!=-1)
		g_DisplayInfo.nMax=nMax;
	::PostMessage(hwnd,DISPLAY_UPDATE,NULL,NULL);
	Sleep(2); 
}
void CSetupDialog::SetDisplayInfo(HWND hwnd,int nPos,int nMax)
{
	g_DisplayInfo.nCur=nPos;
	if(nMax!=-1)
		g_DisplayInfo.nMax=nMax;
	::PostMessage(hwnd,DISPLAY_UPDATE,NULL,NULL);
	Sleep(2);
}

// CInitInfoDialog 消息处理程序

BOOL CSetupDialog::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
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

		}
		break;
	case IN_GAME:
		{
			m_bnSelectUpdate.ShowWindow(0);
			m_bnSelectUpdate.EnableWindow(0);
			m_bnPlayGame.ShowWindow(1);
			m_bnPlayGame.EnableWindow(1);
		}
		break;
	case UPDATE_OK:
		{
			//m_bnPlayGame.ShowWindow(0);
			//m_bnSelectUpdate.ShowWindow(1);

			m_bnSelectUpdate.ShowWindow(1);
			m_bnSelectUpdate.EnableWindow(1);
#ifdef _Version_1_0
			m_bnSelectUpdate.EnableWindow(0);
#endif

			m_bnPlayGame.ShowWindow(0);
			m_bnPlayGame.EnableWindow(0);
		}
		break;
	case UPDATE_INGAME:
		{
			OnOK();
		}
		break;
	case UPDATE_ERROR:
		{
            m_bnSelectUpdate.EnableWindow(1);
		}
		break;
	case UPDATE_PATCH:
		{
			if(ChackPackage())
			{
				m_bnPlayGame.ShowWindow(0);
			    m_bnPlayGame.EnableWindow(0);
				SetDisplayInfo(m_hWnd,CStringRead::GetString(25).c_str(),2,2);
				//InitialInfo();
				unsigned threadID;
				hThread = (HANDLE)_beginthreadex(NULL,0,UpdateProc,this,0,&threadID);
			}
			else{
				string delPath = m_strWorkPath + "\\updatefiles";
	            DeleteDirectory(delPath.c_str());
				WinExec(m_strPathName.c_str(),SW_SHOWNORMAL);
				b_Exit = true;
				::PostMessage(m_hWnd,WM_CLOSE,NULL,NULL);  //下载升级包成功关闭窗口
			}
		}
		break;
	case CHECK_VERSION:
		{
			if(m_pClientResource )
			{
				delete m_pClientResource;
				m_pClientResource=NULL;
			}
			if(m_pPatchFilesInfo)	
			{
				delete m_pPatchFilesInfo;
				m_pPatchFilesInfo = NULL;
			}
			if(m_pPatchPackage) 
			{
				delete m_pPatchPackage;
				m_pPatchPackage = NULL;
			}
			SetDisplayInfo(m_hWnd,CStringRead::GetString(26).c_str(),2,2);
			m_pClientResource = new CClientResource(GAME_RES,m_strWorkPath,m_strFilesInfoName);
	        unsigned threadID;
	        h_VersonThread = (HANDLE)_beginthreadex(NULL,0,LoadVersion,this,0,&threadID);
		}
		break;
	case DOWNFILE_UPDATE:
		{
	         FILE *fp_d = fopen("updateflie.fc","wb");
	         if(fp_d==NULL) return false;
	         fwrite(&g_dwCurSize,1,sizeof(DWORD),fp_d);
	         fclose(fp_d);
		}
		break;
	case DEDOWNFILE_UPDATE:
		{
			string strFullFileName = "updateflie.fc";
            ::DeleteFileA(strFullFileName.c_str());
		}
		break;
	}
	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void	CSetupDialog::OnUpdateDisplay()
{
	if(m_strDisplayInfo != g_DisplayInfo.strText)
	{
		m_strDisplayInfo=g_DisplayInfo.strText;
		CRect myRect;
		myRect.left=245;
		myRect.top=540;
		myRect.right=452;
		myRect.bottom=575;
		//重画文本显示区
		InvalidateRect(&myRect);
	}
	
	m_Progress.SetRange(0,10000);
	if(g_DisplayInfo.nMax==0)
	{
		m_Progress.SetPos(0);
	}
	else
	{
		float fNum = (double)g_DisplayInfo.nCur/(double)g_DisplayInfo.nMax;
		short nCurpos = (float)10000*fNum;
		m_Progress.SetPos(nCurpos);
		long Max = c_rectLoading.right-c_rectLoading.left;
		RECT c_rectLoadingTemp = c_rectLoading;
		long Cur = Max * fNum;
		c_rectLoadingTemp.right = c_rectLoadingTemp.left + Cur;
        m_bnLoading.MoveWindow(&c_rectLoadingTemp);
	}

	UpdateData(false);
}
//自动更新线程
unsigned __stdcall CSetupDialog::LoadInfoProc(void* pParam)
{
	
	CSetupDialog* pSetupDialog = (CSetupDialog*)pParam;

	if(b_Exit==true)
		return false;

	if(LoadUpdateServerResource(pSetupDialog)==false)
	{
		::PostMessage(pSetupDialog->m_hWnd,UPDATE_ERROR,IDOK,NULL);
		_endthreadex( 0 );
		return 0;
	}

	pSetupDialog->m_bIDOK = true;
	::PostMessage(pSetupDialog->m_hWnd,WM_COMMAND,IDOK,NULL);
	_endthreadex( 0 );
	return 0;
}

void CSetupDialog::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	b_Exit = true;

	DWORD dwResult = ::WaitForSingleObject(h_InfoThread,6000);
	if(dwResult == WAIT_TIMEOUT)
	{
		TerminateThread(h_InfoThread,0);
	}
	CDialog::OnCancel();
}

//装载客户端资源
bool	CSetupDialog::LoadClientResource(CSetupDialog* pSetupDialog)
{
	if(pSetupDialog==NULL || pSetupDialog->GetClientResource() == NULL)	return false;

	CClientResource* pClientResour = pSetupDialog->GetClientResource();

	int nMax = 2;
	int nPos = 0;
	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(3).c_str(),nPos,nMax);
	if(pClientResour->LoadExForAutoUpdate()==false)
	{
		SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(4).c_str(),nMax,nMax);

		AfxMessageBox(CStringRead::GetString(5).c_str());
		//ShellExecute( 0, 0, "http://www.saga3d.com", 0, 0, SW_SHOWNORMAL );

		pSetupDialog->m_bReadError=true;
		return false;
	}

	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(3).c_str(),nMax-1,nMax);

	char strVersion[250];
	sprintf(strVersion,CStringRead::GetString(6).c_str(),pClientResour->GetFilesInfo()->GetVersion().c_str());
	pSetupDialog->SetWindowText(strVersion);
	//SetDisplayInfo(pSetupDialog->m_hWnd,strVersion,1,1);
	m_strDisplayInfoEx = strVersion;
	//SetDisplayInfo(pSetupDialog->m_hWnd,"读取客户端文件信息完毕...",nMax,nMax); 
	SetDisplayInfo(pSetupDialog->m_hWnd,strVersion,nMax,nMax); 
	::PostMessage(pSetupDialog->m_hWnd,IN_GAME,NULL,NULL);
	//_endthreadex( 0 );
	return true;;
}

bool CSetupDialog::LoadUpdateServerResource(CSetupDialog* pSetupDialog)
{
	if(pSetupDialog==NULL || pSetupDialog->GetClientResource() == NULL)	return false;

	CClientResource* pClientResour = pSetupDialog->GetClientResource();

	if(b_Exit==true)	return false;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(7).c_str(),0,2);
	
	if(::InternetAttemptConnect(0) != ERROR_SUCCESS)
	{
		SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(8).c_str(),1,1);
		return false;
	}

		char strbuf[100] = ""; 
	string strch(""),strtmp("");
	string str("");
	int ServerTemp = 0;
	bool isSelect = false;
	string	strServer("");
	string	strPath("");
	string strVersion("");
	DWORD dwFileInfoCRC;


    string strTempt = pClientResour->GetCurFolder();
	FindServerIP(pSetupDialog,strTempt,strServer,strPath);

	HINTERNET hSession = ::InternetOpen(_T("Tnt"),INTERNET_OPEN_TYPE_PRECONFIG,_T(""),
		INTERNET_INVALID_PORT_NUMBER,0);
	if(hSession == NULL)
	{		
		SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(8).c_str(),1,1);
		return false;
	}
	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(9).c_str(),2,2);
	if(b_Exit==true)	return false;
	HINTERNET hUpdateServerConnect=NULL;

	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(9).c_str(),1,1);
	if(GetServerVersionFromOneGameServer(pSetupDialog,hSession,strServer,strPath,
			hUpdateServerConnect,dwFileInfoCRC,strVersion) == false)
	{
		string strTempt =CStringRead::GetString(8).c_str();
		SetDisplayInfo(pSetupDialog->m_hWnd,strTempt.c_str(),1,1);
		return false;
	}

	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(9).c_str(),1,1);
	string strOldVersion = pClientResour->GetFilesInfo()->GetVersion();
	//版本一样退出,进入设置界面
	if(strVersion==strOldVersion)
	{
		SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(10).c_str(),1,1);
		::PostMessage(pSetupDialog->m_hWnd,IN_GAME,NULL,NULL);
		::PostMessage(pSetupDialog->m_hWnd,UPDATE_INGAME,NULL,NULL);
		return	true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(b_Exit==true)	return false;
	//为当前客户端版本设置服务器的更新目录,包含客户端版本号信息
	//strPath = strPath + "/Update-" + strOldVersion; 

    SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(11).c_str(),1,2);

	//CFilesInfo ServerFilesInfo,TemptFilesInfo;;
	string ServerFilesName("");
	DWORD  ServerFilesCRC = 0;

	string strFileName = strPath;
	//strFileName1 +="/FilesInfo.vs";
	strFileName +="/Update-" + strOldVersion+".vs"; 
	//strFileName1 +="/Game.vs";
	string strLocalFileName = pSetupDialog->GetWorkPath();
	strLocalFileName += "\\UpdateFilesInfo.ril";
	if(!GetUpdateFileInfosEx(hUpdateServerConnect,strFileName,strLocalFileName,ServerFilesCRC,ServerFilesName))
	{
		SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(12).c_str(),2,2);
	 	return false;
	}
	m_strPathName = ServerFilesName;
	//检查本地是否有该版本的更新包
	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(13).c_str(),2,2);
	FILE *fp_f = fopen(ServerFilesName.c_str(),"rb");
	if(fp_f)
	{
        DWORD dwLoadFlieCRC=0;
		CCrc32Static::FileCrc32Filemap(ServerFilesName.c_str(),dwLoadFlieCRC);
		if(dwLoadFlieCRC==ServerFilesCRC)
		{
			fclose(fp_f);
			SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(14).c_str(),2,2);
			string ServerTemp = ServerFilesName;
			SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(15).c_str(),2,2);
#ifdef _Version_1_0
			::PostMessage(pSetupDialog->m_hWnd,UPDATE_PATCH,NULL,NULL);  //下载升级包成功关闭窗口
#else
			WinExec(ServerTemp.c_str(),SW_SHOWNORMAL);
			b_Exit = true;
			::PostMessage(pSetupDialog->m_hWnd,WM_CLOSE,NULL,NULL);  //下载升级包成功关闭窗口
#endif
			_endthreadex( 0 );
		}
		fclose(fp_f);
	}
	int nTotalSize = 0;

	//建立一个零时文件，用来暂存下载的文件，也同时完成端点续传
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(b_Exit==true)	return false;

	BYTE *pBuf = new BYTE[GET_BUFFER_MAX];
	DWORD dwBufLen = GET_BUFFER_MAX;
	int nCount=2;
	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(16).c_str(),0,nCount);
	string ServerFile = "http://"+strServer;
	ServerFile += strPath+"/"+ServerFilesName;  //计算更新文件在服务器上的完整路径

	if(DownloadFile(ServerFile.c_str(),ServerFilesName.c_str(),pSetupDialog->m_hWnd))
	{
		SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(17).c_str(),0,nCount);
        DWORD dwLoadFlieCRC=0;
		CCrc32Static::FileCrc32Filemap(ServerFilesName.c_str(),dwLoadFlieCRC);
		if(dwLoadFlieCRC==ServerFilesCRC)
		{
			string ServerTemp = ServerFilesName;
			SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(15).c_str(),2,2);
#ifdef _Version_1_0
			::PostMessage(pSetupDialog->m_hWnd,UPDATE_PATCH,NULL,NULL);  //下载升级包成功关闭窗口
#else
			WinExec(ServerTemp.c_str(),SW_SHOWNORMAL);
			b_Exit = true;
			::PostMessage(pSetupDialog->m_hWnd,WM_CLOSE,NULL,NULL);  //下载升级包成功关闭窗口
#endif
			_endthreadex( 0 );
		}else{
		    SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(18).c_str(),1,1);
		}

		return false;
	}else{
		SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(5).c_str(),1,1);
		_endthreadex( 0 );
	}
	return true;
}

bool CSetupDialog::GetServerVersionFromOneGameServer(CSetupDialog* pSetupDialog,HINTERNET hSession,
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
	strFileName +="/AutoUpdateGame.vs"; 
	//strFileName +="/Game.vs";
	string strLocalVersionFileName = pSetupDialog->GetWorkPath();
	strLocalVersionFileName += "\\AutoUpdateGame.vs"; 
	if(GetServerVersion(hUpdateServerConnect,strFileName,strLocalVersionFileName,dwFileInfoCRC,strVersion)==false)
	{
		InternetCloseHandle(hUpdateServerConnect);
		return false;
	}
	return true;
}
//得到服务器游戏版本
bool CSetupDialog::GetServerVersion(HINTERNET hConnect,string& strPathName,string& strLocalVersionName,
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
		return	true;
	}
	else
	{
		//DWORD dwError = GetLastError();
		return false;
	}
	//return	false;
}

//得到服务器更新文件信息
bool	CSetupDialog::GetUpdateFileInfosEx(HINTERNET hConnect,string& strPathName,string& strLocalFileName,
											DWORD &dwFileInfoCRC,string &ServerFilesName)
{
	if(hConnect==NULL)	return false;
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
		//***************检测所下文件是否存在********************//
		char achQueryBuf[16];
//		DWORD FileSize;
		DWORD QueryBufLen=sizeof(achQueryBuf);
		BOOL bQueryTemp;
		bQueryTemp =::HttpQueryInfo(
			hHttpFile,
			HTTP_QUERY_STATUS_CODE,
			achQueryBuf,&QueryBufLen,NULL);
		if(atoi(achQueryBuf)==404)
		{
			//请示失败，没有找到
			return false;
		}
        //******************************************************//
		DWORD dwContentLen;
        DWORD dwBufLen = sizeof(dwContentLen);
		DWORD dwStates = 0;
		BOOL	bQuery = ::HttpQueryInfo(hHttpFile,HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER,(LPVOID)&dwContentLen,&dwBufLen,&dwStates);
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
		fscanf(pf_read,"%u",&dwFileInfoCRC);
		fscanf(pf_read,"%s\n",strbuf);
		ServerFilesName = strbuf;
		ServerFilesName = "Game"+ServerFilesName+"Patch.exe"; 
		fclose(pf_read);
		string strfile = "UpdateFilesInfo.ril"; 
		::DeleteFile(strfile.c_str());	
		return	true;
	}
	return false;
}

bool CSetupDialog::CheckVersion(CSetupDialog* pSetupDialog)
{
	if(pSetupDialog==NULL || pSetupDialog->GetClientResource() == NULL)	return false;

	::PostMessage(pSetupDialog->m_hWnd,UPDATE_OK,NULL,NULL);

	CClientResource* pClientResour = pSetupDialog->GetClientResource();

	if(b_Exit==true)	return false;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(7).c_str(),0,2);
	
	if(::InternetAttemptConnect(0) != ERROR_SUCCESS)
	{
		SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(8).c_str(),1,1);
		::PostMessage(pSetupDialog->m_hWnd,IN_GAME,NULL,NULL);
		return false;
	}

	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(19).c_str(),0,2);

	char strbuf[100] = ""; 
	string strch(""),strtmp("");
	string str("");
	int ServerTemp = 0;
	bool isSelect = false;
	string	strServer("");
	string	strPath("");
	string strVersion("");
	DWORD dwFileInfoCRC;


    string strTempt = pClientResour->GetCurFolder();
	FindServerIP(pSetupDialog,strTempt,strServer,strPath);

	HINTERNET hSession = ::InternetOpen(_T("Tnt"),INTERNET_OPEN_TYPE_PRECONFIG,_T(""),
		INTERNET_INVALID_PORT_NUMBER,0);
	if(hSession == NULL)
	{		
		SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(8).c_str(),1,1);
		::PostMessage(pSetupDialog->m_hWnd,IN_GAME,NULL,NULL);
		return false;
	}
	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(9).c_str(),2,2);
	if(b_Exit==true)	return false;
	HINTERNET hUpdateServerConnect=NULL;

	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(9).c_str(),1,1);
	if(GetServerVersionFromOneGameServer(pSetupDialog,hSession,strServer,strPath,
			hUpdateServerConnect,dwFileInfoCRC,strVersion) == false)
	{
		string strTempt = CStringRead::GetString(8).c_str();
		::PostMessage(pSetupDialog->m_hWnd,IN_GAME,NULL,NULL);
		SetDisplayInfo(pSetupDialog->m_hWnd,strTempt.c_str(),1,1);
		return false;
	}
	///////////////////////////////////下载serverlist和gamenote///////////////////////////////////
	string strUrllist = "http://"+strServer;
	strUrllist += strPath;
	string strGameNote = strUrllist + "/GameNote.ini";
    strUrllist += "/serverlist.ini" ;
    string strList = "setup/serverlist.ini";
	string strNote = "setup/GameNote.ini";
	if(DownloadFile(strUrllist.c_str(),strList.c_str(),pSetupDialog->m_hWnd,false))
	{}
	if(DownloadFile(strGameNote.c_str(),strNote.c_str(),pSetupDialog->m_hWnd,false))
	{}
	/////////////////////////////////////////////////////////////////////

	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(9).c_str(),1,1);
	string strOldVersion = pClientResour->GetFilesInfo()->GetVersion();
	//版本一样退出,进入设置界面
	if(strVersion==strOldVersion)
	{
		SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(10).c_str(),1,1);
		::PostMessage(pSetupDialog->m_hWnd,IN_GAME,NULL,NULL);
		//::PostMessage(pSetupDialog->m_hWnd,UPDATE_OK,NULL,NULL);   //发送消息将自动更新按钮置为不可用
		_endthreadex( 0 );
		return	false;
	}else{
	    SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(20).c_str(),1,1);
		//	unsigned threadID;
	   // h_InfoThread = (HANDLE)_beginthreadex(NULL,0,LoadInfoProc,pSetupDialog,0,&threadID);
	   // ::PostMessage(pSetupDialog->m_hWnd,UPDATE_OK,NULL,NULL);   //发送消息将自动更新按钮置为不可用		
#ifdef _Version_1_0
		unsigned threadID;
	    h_InfoThread = (HANDLE)_beginthreadex(NULL,0,LoadInfoProc,pSetupDialog,0,&threadID);
#else
		::PostMessage(pSetupDialog->m_hWnd,UPDATE_OK,NULL,NULL);   //发送消息将自动更新按钮置为不可用	 	
#endif
        _endthreadex( 0 );
	   // m_bnSelectUpdate.EnableWindow(0);
		return true;
	}
}

//程序运行时自动检查版本
unsigned __stdcall CSetupDialog::LoadVersion(void* pParam)
{
	CSetupDialog* pSetupDialog = (CSetupDialog*)pParam;
	if(LoadClientResource(pSetupDialog)==false)
	{
		_endthreadex( 0 );
		return 0;
	}
	if(CheckVersion(pSetupDialog)==false)
	{
		_endthreadex( 0 );
		return 0;
	}
	return 1;
}
void CSetupDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CSetupDialog::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	UnLoadSdoLoad();
	OnCancel();
} 

void CSetupDialog::OnBnClickedButtonCacel()
{
	// TODO: 在此添加控件通知处理程序代码
	UnLoadSdoLoad();
	OnCancel();
}

void CSetupDialog::OnBnClickedButtonWire()
{
	// TODO: 在此添加控件通知处理程序代码
	
	//m_bnServerChoose.MoveWindow(&c_rectChooseWire);
	if(m_Sever==1)
	{
		m_bnServerChoose.ShowWindow(1);
		m_bnServerChoose2.ShowWindow(0);
		m_bnServerWire.EnableWindow(0);
		m_bnServerNet.EnableWindow(1);
	}
	//OnUpdateDisplay();

	m_Sever = 0;
}


void CSetupDialog::OnBnClickedButtonNet()
{
	// TODO: 在此添加控件通知处理程序代码
	
	if(m_Sever==0)
	{
		m_bnServerChoose.ShowWindow(0);
		m_bnServerChoose2.ShowWindow(1);
		m_bnServerWire.EnableWindow(1);
		m_bnServerNet.EnableWindow(0);
	}

    m_Sever = 1;
	//OnUpdateDisplay();
}

int CSetupDialog::LoadSdoLoad()
{
		char strSDLoadDLL[MAX_PATH]= {0};
		sprintf(strSDLoadDLL,"%s\\SDLoad.dll",m_strWorkPath.c_str());
		int i=0;
		if((GetFileAttributes(strSDLoadDLL) != 0xFFFFFFFF))
		{
			 g_hInstSDLoadDLL=LoadLibrary(strSDLoadDLL);
			 if(g_hInstSDLoadDLL)
			 {
				 loadSDPlugin = (LoadSDPlugin)GetProcAddress(g_hInstSDLoadDLL,"LoadSDPlugin");
				 unloadSDPlugin = (UnLoadSDPlugin)GetProcAddress(g_hInstSDLoadDLL,"UnLoadSDPlugin");
			 }		
			char str[500]="";
			HWND hMainWin=0;
			hMainWin = this->m_hWnd;
			sprintf(str,"<MainDlgHandle=\"%d\" Game=\"101\" BG=\"0\" Local=\"1\"/>",(int)hMainWin);
			i=loadSDPlugin(str,sizeof(str));
		}
        return i;
}

void CSetupDialog::UnLoadSdoLoad()
{
	unloadSDPlugin();
}
void CSetupDialog::OnBnClickedButtonNew()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(0, NULL, m_strwebLink[2].c_str(), NULL, NULL, 1);
}

void CSetupDialog::OnBnClickedButtonFirstpage()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(0, NULL, m_strwebLink[1].c_str(), NULL, NULL, 1); 
}

void CSetupDialog::OnBnClickedButtonZhanghao()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(0, NULL, m_strwebLink[3].c_str(), NULL, NULL, 1); 
}

void CSetupDialog::OnBnClickedButtonDownload()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(0, NULL, m_strwebLink[4].c_str(), NULL, NULL, 1); 
}

void CSetupDialog::OnBnClickedButtonKefu()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(0, NULL, m_strwebLink[5].c_str(), NULL, NULL, 1); 
}

void CSetupDialog::OnBnClickedButtonLuntan()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(0, NULL, m_strwebLink[6].c_str(), NULL, NULL, 1); 
}

BOOL CSetupDialog::InitialInfo()
{
	CHAR* strPath = new CHAR[MAX_PATH];
	::GetSystemDirectory(strPath,MAX_PATH);
	string strTemptr(strPath);
	delete []strPath;

	string strTemptFoler;strTemptFoler.clear();
	strTemptFoler.insert(0,strTemptr,0,2);
	
//#ifdef	_DEBUG
	CHAR* strWorkPath = new CHAR[MAX_PATH];
	if(::GetCurrentDirectory(MAX_PATH,strWorkPath))
	{
		m_strExePath = string(strWorkPath);
	}
	delete []strWorkPath;
	m_strPathName = "\\"+m_strPathName;
	m_strPathName = m_strExePath+m_strPathName;
	//m_strFilesInfoNameEx="FilesInfo.ril";

	m_strTemptFilesInfoName=m_strExePath+"\\Patch.ril";	//临时包
	m_strTemptPackageName=m_strExePath+"\\Patch.pak";//临时文件姓名

	string tmpUpdatePath = m_strWorkPath + "\\updatefiles";
	//"gameclient"
	::CreateDirectory(tmpUpdatePath.c_str(), NULL);
	return true;
}

//更新相应的文件
BOOL  CSetupDialog::UpdateClientResource(tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,BYTE*& pBuf,DWORD& dwBuflen,
					 CPackage* pPackage, CClientResource* pClientResource,HWND hwnd,int& nTotalSize)
{
	if(ptagFileInfo==NULL || pFilesInfo==NULL || pPackage==NULL 
		|| pClientResource==NULL || b_Exit==true )
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
							sprintf(outPtr, CStringRead::GetString(40).c_str(), strFileName.c_str());
							PutDebugString(outPtr);
							return false;
						}
					}
				}
				catch (...)
				{
					char strTempt[256]="";
					sprintf(strTempt,CStringRead::GetString(41).c_str(),
						strFileName.c_str(),dwSize);
					AfxMessageBox(strTempt);
					return false;
				}
			}
			else
			{
				char outPtr[1024];
				sprintf(outPtr, CStringRead::GetString(42).c_str(), strFileName.c_str());
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
					sprintf(outPtr, CStringRead::GetString(42).c_str(), strFileName.c_str());
					PutDebugString(outPtr);
					return false;
				}
			}
			catch (...)
			{
				AfxMessageBox(CStringRead::GetString(43).c_str());
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

//升级线程
unsigned __stdcall CSetupDialog::UpdateProc(void* pParam)
{
	CSetupDialog* pPatchDlg = (CSetupDialog*)pParam;
	if(pPatchDlg == NULL || pPatchDlg->m_pClientResource == NULL)	return 0;

	if(pPatchDlg->GetPatchData()==FALSE)
	{
		AfxMessageBox(CStringRead::GetString(29).c_str());
		_endthreadex( 0 );
		return 0;
	}

	//设置包信息
	CClientResource* pClientResour = pPatchDlg->m_pClientResource;
	if(!LoadClientResource(pPatchDlg))
	{
		AfxMessageBox(CStringRead::GetString(30).c_str());
		_endthreadex( 0 );
		return FALSE;
	}

	if(b_Exit == true)
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
		sprintf(szOutPut, CStringRead::GetString(31).c_str(), pPatchDlg->m_listStrUpdatedVersion.c_str(), strVersion);
		AfxMessageBox(szOutPut);
		return FALSE;
	}

	sprintf(strTitle, CStringRead::GetString(32).c_str(), strVersion,
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
		SetDisplayInfo(pPatchDlg->m_hWnd,CStringRead::GetString(33).c_str(),1,1);
		PutDebugString(CStringRead::GetString(33).c_str());
		return FALSE;
	}
#endif

	//得到不同部分
	CFilesInfo DirfFilesInfo;
	pPatchDlg->m_pPatchFilesInfo->GetDifferenceByCrc32( *(pClientResour->GetFilesInfo()),DirfFilesInfo);


	SetDisplayInfo(pPatchDlg->m_hWnd,CStringRead::GetString(34).c_str(),0,
				DirfFilesInfo.GetRootFile()->dwSize);
	PutDebugString(CStringRead::GetString(34).c_str());

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
		SetDisplayInfo(pPatchDlg->m_hWnd,CStringRead::GetString(35).c_str(),1,1);
		::PostMessage(pPatchDlg->m_hWnd,CHECK_VERSION,NULL,NULL);
		//AfxMessageBox("升级成功");
	}

	else
	{
		pClientResour->UpdateSave();
		SetDisplayInfo(pPatchDlg->m_hWnd,CStringRead::GetString(36).c_str(),1,1);
		//AfxMessageBox("升级失败");
	}

	// 关闭包资源
	pPatchDlg->m_pPatchPackage->CloseFileHandle();

	string delPath = pPatchDlg->m_strWorkPath + "\\updatefiles";
	DeleteDirectory(delPath.c_str());

	//::SendMessage(pPatchDlg->m_hWnd, WM_CLOSE, 0, 0);
	// 打开自动更新程序
	string updateExe = pPatchDlg->m_strWorkPath + "\\update.exe";
	WinExec(updateExe.c_str(), SW_SHOWNORMAL);
	_endthreadex( 0 );
	return 0;
}

BOOL	CSetupDialog::GetPatchData()
{	
	HGLOBAL  hResLoad;     // handle to loaded resource 
	HRSRC hRes;         // handle/ptr. to res. info. in hExe 
	VOID *lpResLock;    // pointer to resource data 
	HMODULE ip;

	//#ifndef _DEBUG
	//得到包文件数据
    ip = LoadLibrary(m_strPathName.c_str());
	hRes = ::FindResource((HMODULE)ip,MAKEINTRESOURCE(IDR_PATCH1),"PATCH");
	if( hRes == NULL ) 
	{ 
		DWORD dwError = GetLastError();
		return FALSE; 
	} 

	hResLoad = ::LoadResource((HMODULE)ip, hRes); 
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
	FILE* pf = fopen(tmpZipFileNmae.c_str(),"wb+");
	if(!pf)
	{	
		char str[100]="";
		sprintf(str,CStringRead::GetString(37).c_str(),tmpZipFileNmae.c_str());
		AfxMessageBox(str);
		return FALSE;
	}	

	DWORD dwSize = SizeofResource((HMODULE)ip,hRes);
	DWORD dwCount = fwrite(lpResLock,dwSize,1, pf);
	if(dwCount != 1)
	{
		char str[100]="";
		sprintf(str,CStringRead::GetString(38).c_str(),tmpZipFileNmae.c_str());
		AfxMessageBox(str);
		return FALSE;
	}

	int nRet = fflush(pf);
	if(nRet != 0)
	{
		char str[100]="";
		sprintf(str,CStringRead::GetString(37).c_str(),tmpZipFileNmae.c_str());
		AfxMessageBox(str);
		return FALSE;
	}
	fclose(pf);
	SetDisplayInfo(this->m_hWnd,CStringRead::GetString(39).c_str(),16,20);
	PutDebugString(CStringRead::GetString(39).c_str());

	//解压缩数据
	unzFile dataZipFile = unzOpen(tmpZipFileNmae.c_str());
	if(dataZipFile)
	{
		unz_file_info zipFileInfo;
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

		SetDisplayInfo(this->m_hWnd,CStringRead::GetString(39).c_str(),19,20);
		PutDebugString(CStringRead::GetString(39).c_str());

		if(!m_pPatchPackage)
			m_pPatchPackage = new CPackage(NULL,m_strExePath);
		m_pPatchPackage->Open("\\updatefiles\\Patch");
	}

	unzClose(dataZipFile);

	//得到被升级的版本数据
	hRes = ::FindResource((HMODULE)ip,MAKEINTRESOURCE(IDR_PATCH2),"PATCH");
	if( hRes == NULL ) 
	{ 
		DWORD dwError = GetLastError();
		return FALSE; 
	} 

	hResLoad = ::LoadResource((HMODULE)ip, hRes); 
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

bool  CSetupDialog::ChackPackage()
{
	bool bReg = false;
	InitialInfo();
	if(GetPatchData())
	{
		if(m_pPatchFilesInfo)
		{
			string str = "\\launcher.exe";
			if(m_pPatchFilesInfo->FindFileInfo(str))
			{
				bReg = true;
			}else{
				bReg = false;
			}
		}
	}
	return bReg;
}

BOOL CSetupDialog::DownloadFile(const CString& strFileURLInServer, //待下载文件的URL
const CString & strFileLocalFullPath,HWND hwnd,bool bContinue)//存放到本地的路径
{
 ASSERT(strFileURLInServer != "");
 ASSERT(strFileLocalFullPath != "");
 CInternetSession session;
 CHttpConnection* pHttpConnection = NULL;
 CHttpFile* pHttpFile = NULL;
 CString strServer, strObject;
 INTERNET_PORT wPort;

 DWORD dwType;
 const int nTimeOut = 2000;
 session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeOut); //重试之间的等待延时
 session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);   //重试次数
 char* pszBuffer = NULL; 

	DWORD dwCount=0;//偏移量
	if(bContinue)   //当指定断点续传时,在读写该文件
	{
		FILE *fp_date = fopen("updateflie.fc","rb");
		if(fp_date!=NULL)
		{	
		
			fread(&dwCount,1,sizeof(DWORD),fp_date);
			fclose(fp_date);
			if(dwCount==0)
			{
				//读取错误删除该文件
				//string strFullFileName = pInfoDialog->GetWorkPath();
				/*string strFullFileName = "update\\updateflie.fc";*/
				string strFullFileName = "updateflie.fc";
				::DeleteFileA(strFullFileName.c_str());
			}
		}
	}



 try
 {
  AfxParseURL(strFileURLInServer, dwType, strServer, strObject, wPort);
  pHttpConnection = session.GetHttpConnection(strServer, wPort);
  pHttpFile = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject);
  if(pHttpFile->SendRequest() == FALSE)
   return false;
  DWORD dwStateCode;

  pHttpFile->QueryInfoStatusCode(dwStateCode);
  if(dwStateCode == HTTP_STATUS_OK)
  {
	  HANDLE hFile = NULL;

	  if(dwCount>0)
	  {
         hFile = CreateFile(strFileLocalFullPath, GENERIC_WRITE,
         FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
         NULL);  //打开本地文件
	  }else{
         hFile = CreateFile(strFileLocalFullPath, GENERIC_WRITE,
         FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
         NULL);  //创建本地文件
	  }

   if(hFile == INVALID_HANDLE_VALUE)
   {
    pHttpFile->Close();
    pHttpConnection->Close();
    session.Close();
    return false;
   }
 
   char szInfoBuffer[10000];  //返回消息
   DWORD dwFileSize = 0;   //文件长度
   DWORD dwInfoBufferSize = sizeof(szInfoBuffer);
   BOOL bResult = FALSE;
   CString strLength;
   pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH,dwFileSize);
   g_dwFileSize = dwFileSize;

   if(dwCount!=0)
   {
	   SetDisplayInfo(hwnd,CStringRead::GetString(27).c_str(),2,2);
	   long l = pHttpFile->Seek(dwCount,0);
	   DWORD dwSeek = SetFilePointer(hFile,dwCount,0,FILE_BEGIN);
	   if(l==0||dwSeek==0)
	   {
	   	  SetDisplayInfo(hwnd,CStringRead::GetString(28).c_str(),2,2);
		  dwCount=0;
	   }
	 //	   return false;
   }

   const int BUFFER_LENGTH = 1024 * 10;
   pszBuffer = new char[BUFFER_LENGTH];  //读取文件的缓冲
   DWORD dwWrite, dwTotalWrite;
   dwWrite = 0;
   dwTotalWrite = 0;
   g_dwCurSize = dwCount;
   UINT nRead = pHttpFile->Read(pszBuffer, BUFFER_LENGTH); //读取服务器上数据
   CString strFileSize;
   DWORD dwStartTime = timeGetTime();
   DWORD dwStartTimeEx = timeGetTime();

   while(nRead > 0)
   {

      WriteFile(hFile, pszBuffer, nRead, &dwWrite, NULL);  //写到本地文件
      dwTotalWrite += dwWrite;

	  g_dwCurSize += dwWrite;

		//计算下载速度
	    DWORD CurTime = timeGetTime();
		if((CurTime-dwStartTime)>1000)
		{				
			float fLastTime = (float)(CurTime-dwStartTimeEx);             //计算这段间隔的时间

			float fSpeed = ((float)dwTotalWrite)/fLastTime;

			dwStartTime=CurTime;

			strFileSize.Format(CStringRead::GetString(21).c_str(),((float)g_dwCurSize)/(1000.0f*1000.0f),((float)g_dwFileSize)/(1000.0f*1000.0f),fSpeed);
			SetDisplayInfo(hwnd,strFileSize,g_dwCurSize,g_dwFileSize);
		}
	  Sleep(2); 
	  if(bContinue)
	     ::PostMessage(hwnd,DOWNFILE_UPDATE,NULL,NULL);
      nRead = pHttpFile->Read(pszBuffer, BUFFER_LENGTH);
   }
   if(bContinue)
     ::PostMessage(hwnd,DEDOWNFILE_UPDATE,NULL,NULL);

   delete[]pszBuffer;
   pszBuffer = NULL;
   CloseHandle(hFile);
  }
  else
  {
   delete[]pszBuffer;
   pszBuffer = NULL;
   if(pHttpFile != NULL)
   {
    pHttpFile->Close();
    delete pHttpFile;
    pHttpFile = NULL;
   }
   if(pHttpConnection != NULL)
   {
    pHttpConnection->Close();
    delete pHttpConnection;
    pHttpConnection = NULL;
   }
   session.Close();
    return false;
  }
 }
 catch(...)
 {
  delete[]pszBuffer;
  pszBuffer = NULL;
  if(pHttpFile != NULL)
  {
   pHttpFile->Close();
   delete pHttpFile;
   pHttpFile = NULL;
  }
  if(pHttpConnection != NULL)
  {
   pHttpConnection->Close();
   delete pHttpConnection;
   pHttpConnection = NULL;
  }
  session.Close();
  return false;
 }

 if(pHttpFile != NULL)
  pHttpFile->Close();
 if(pHttpConnection != NULL)
  pHttpConnection->Close();
 session.Close();
 return true;
}

bool CSetupDialog:: FindServerIP(CSetupDialog* pSetupDialog,string strFolder,string &strServer,string &strPath)
{

	strFolder += "\\setup\\Updatehostlist.ini";
	FILE* pf = fopen(strFolder.c_str(),"rb");
	if(!pf)
	{
		CString str(CStringRead::GetString(22).c_str());
		SetDisplayInfo(pSetupDialog->m_hWnd,str,1,1);
		return false;
	}
	SetDisplayInfo(pSetupDialog->m_hWnd,CStringRead::GetString(23).c_str(),0,2);

	CPing ping;
	string strUpdateServer("");
	long lTimeOut = 2000;
	multimap<long,tagServer> UpdateServerList; 

	char strbuf[100]="";
	string strch(""),strtmp("");
	string str("");
	int ServerTemp = 0;
	bool isSelect = false;
	string strVersion("");
	DWORD dwFileInfoCRC;
	while(!feof(pf))
	{
	   fscanf(pf,"%s\n",strbuf);
	   strtmp = strbuf;
	   //strch+=strtmp;
	   int nPos=-1;
	   nPos=strtmp.find_first_of("/");
	   if(nPos==-1&&strcmp(strtmp.c_str(),"@")!=0)
		   return 0;
	   if(strcmp(strtmp.c_str(),"@")==0)
	   {
		   ServerTemp = 1;
	   }
	   else{
		   if(ServerTemp == m_Sever)
		   {
			   string strIP("");
			   string strPathTemp("");
			   strIP=strtmp;
			   strIP.erase(nPos,str.length()-nPos);
			   strPathTemp=strtmp;
			   strPathTemp.erase(0,nPos);

			   tagServer Server = {strIP,strPathTemp};

				//得到ping值 ,寻找最快更新服务器
				long lRet = ping.Ping((char*)strIP.c_str(),1,2000);
				if(lRet != -1 && lRet < 20)
				{
					strServer = strIP;
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
		   }
	   }

	}
	fclose(pf);

	if(UpdateServerList.size()<=0&&isSelect==false)
	{
		CString str(CStringRead::GetString(24).c_str());
		SetDisplayInfo(pSetupDialog->m_hWnd,str,1,1);
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
	return true;
}

void CSetupDialog::LoadBitmapFromFile(CBitmapButton &button,LPCTSTR lpszFilenameNor,LPCTSTR lpszFilenamesel,LPCTSTR lpszFilenamefoc)
{
	if(SetBitmapBtnImage(button,lpszFilenameNor,BTN_NOR)&&SetBitmapBtnImage(button,lpszFilenamesel,BTN_SEL)
		&&SetBitmapBtnImage(button,lpszFilenamefoc,BTN_FOC))
	{
	    button.SizeToContent();
	    button.Invalidate();
	}
}

void CSetupDialog::InitButtonPos(const char* Name,CBitmapButton &button)
{
	tagInterface Interface;
	m_Interface.GetInterfaceTag(Name,Interface);
    const RECT c_rect = {Interface.PosX,Interface.PosY,Interface.PosX+Interface.Width,Interface.PosY+Interface.Height};
	button.MoveWindow(&c_rect);
	m_mapButtonRect[Name] = c_rect;
	LoadBitmapFromFile(button,Interface.Path1.c_str(),Interface.Path3.c_str(),Interface.Path2.c_str());
}

void CSetupDialog::InitInterface()
{
    InitButtonPos("Quit",m_bnQuit);
    InitButtonPos("PlayGame",m_bnPlayGame);
	InitButtonPos("SelectUpdate",m_bnSelectUpdate);
	InitButtonPos("SysSet",m_bnSysSet);
    InitButtonPos("Loading",m_bnLoading);
	InitButtonPos("ServerWire",m_bnServerWire);
	InitButtonPos("ServerNet",m_bnServerNet);
	InitButtonPos("ServerChoose",m_bnServerChoose);
	InitButtonPos("ServerChoose2",m_bnServerChoose2);
	InitButtonPos("FirstPage",m_bnFirstPage);
	InitButtonPos("NEW",m_bnNEW);
	InitButtonPos("ZhangHao",m_bnZhangHao);
	InitButtonPos("DownLoad",m_bnDownLoad);
	InitButtonPos("KeFu",m_bnKeFu);
	InitButtonPos("LunTan",m_bnLunTan);
}