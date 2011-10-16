// urlfile.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "urlfile.h"
#include "Splash.h"
#include "InitInfoDialog.h"
#include "SetupDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#pragma comment(lib, "d3d9")

//输出调试信息
//void PutDebugString(const char *msg)
//{
//	AfxMessageBox(msg);
//}

/////////////////////////////////////////////////////////////////////////////
// CUrlFileApp

BEGIN_MESSAGE_MAP(CUrlFileApp, CWinApp)
	//{{AFX_MSG_MAP(CUrlFileApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUrlFileApp construction

CUrlFileApp::CUrlFileApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUrlFileApp object

CUrlFileApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUrlFileApp initialization

BOOL CUrlFileApp::InitInstance()
{
	// 如果更新程序已经打开，则关闭自己
	HANDLE hM = CreateMutex(NULL,TRUE,"CS_Update");
	if( GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}
	HWND hWnd = FindWindow("《星空战记》",NULL);  
	if (hWnd != NULL)
	{
		AfxMessageBox(_T("你正在运行游戏程序，请先关闭游戏程序，再运行更新程序！"));
		return FALSE;
	}
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
   	
	AfxEnableControlContainer();  //enable support for containment of OLE controls.

		CCommandLineInfo cmdInfo;

		ParseCommandLine(cmdInfo);

		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);

//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif
	
	srand((unsigned)time(NULL));
	//初始化相关参数
	InitResource();
	//装载信息
	//CInitInfoDialog InfoDialog(m_strWorkPath,m_strPackageName,m_strFilesInfoName);
	//if( InfoDialog.DoModal() ==IDOK)
	//{
			CSetupDialog SetupDialog(m_strWorkPath,m_strPackageName,m_strFilesInfoName);
			//SetupDialog.m_bReadError=InfoDialog.m_bReadError;
			if(SetupDialog.DoModal()==IDOK)
			{
				SetupDialog.UnLoadSdoLoad();
				WinExec("Luvinia.exe",SW_SHOWNORMAL);
			}

	//}
    //WinExec("gameclient.dat",SW_SHOWNORMAL);
	if (hM) 
	{
		ReleaseMutex(hM);
		CloseHandle(hM);
	}
	return FALSE;
}

//初始化客户端资源信息
bool	CUrlFileApp::InitResource()
{
	CHAR* strPath = new CHAR[MAX_PATH];
	if(::GetCurrentDirectory(MAX_PATH,strPath)==false)
		return false;
	m_strWorkPath=string(strPath);
	delete []strPath;
	
	m_strPackageName="Game.pak";
	//m_strFilesInfoName="FilesInfo.ril";
	m_strFilesInfoName="PackRes.cfg";
	//m_pClientResource = NULL;
	//m_hUpdateServerConnect=NULL;
	m_tagSetup.bUseVSync = 0;
	m_tagSetup.lFrameControl = 10;
	m_tagSetup.lsimplemodelnum = 10;
	m_tagSetup.bupdateonlostfoucs = false;

	if(WriteReg(m_strWorkPath)==false)
	{
		return false;
	}

	LoadSetup();
	CheckDisplayMode();

	SetDisplayMode();

	SaveSetup();

	return true;
}


void CUrlFileApp::LoadSetup()
{
	string strTemp;
	ifstream stream;
	stream.open("setup/params.ini");
	if( stream.is_open() )
	{
		//stream	>> strTemp >> m_tagSetup.bFullscreen
		//	>> strTemp >> m_tagSetup.lWidth
		//	>> strTemp >> m_tagSetup.lHeight
		//	>> strTemp >> m_tagSetup.lBitsPerPel
		//	>> strTemp >> m_tagSetup.lFrequency
		//	>> strTemp >> m_tagSetup.lTexturemode
		//	>> strTemp >> m_tagSetup.lLight
		//	>> strTemp >> m_tagSetup.lModel
		//	>> strTemp >> m_tagSetup.lShadow
		//	>> strTemp >> m_tagSetup.lTexture
		//	>> strTemp >> m_tagSetup.lWeather
		//	>> strTemp >> m_tagSetup.lControl
		//	>> strTemp >> m_tagSetup.lViewRange
		//	>> strTemp >> m_tagSetup.bSound
		//	>> strTemp >> m_tagSetup.bMusic
		//	>> strTemp >> m_tagSetup.fSoundVolume 
		//	>> strTemp >> m_tagSetup.fMusicVolume;
		stream	>> strTemp >> m_tagSetup.bFullscreen
		>> strTemp >> m_tagSetup.lWidth
		>> strTemp >> m_tagSetup.lHeight
		>> strTemp >> m_tagSetup.lBitsPerPel
		>> strTemp >> m_tagSetup.lFrequency
		>> strTemp >> m_tagSetup.lTexturemode
		>> strTemp >> m_tagSetup.b2DLinear
		>> strTemp >> m_tagSetup.b3DLinear
		>> strTemp >> m_tagSetup.lLight
		>> strTemp >> m_tagSetup.lModel
		>> strTemp >> m_tagSetup.lShadow
		>> strTemp >> m_tagSetup.lTexture
		>> strTemp >> m_tagSetup.bRoleFlash
		>> strTemp >> m_tagSetup.bRoleShake
		>> strTemp >> m_tagSetup.bShock
		>> strTemp >> m_tagSetup.lWeather
		>> strTemp >> m_tagSetup.lControl
		>> strTemp >> m_tagSetup.lViewRange
		>> strTemp >> m_tagSetup.bSound
		>> strTemp >> m_tagSetup.bMusic
		>> strTemp >> m_tagSetup.bSpeech
		>> strTemp >> m_tagSetup.fSoundVolume 
		>> strTemp >> m_tagSetup.fMusicVolume
		>> strTemp >> m_tagSetup.fSpeechVolume
		>> strTemp >> m_tagSetup.bOthersFootstep
		>> strTemp >> m_tagSetup.lAntiAliasing
		>> strTemp >> m_tagSetup.bUseVSync
		>> strTemp >> m_tagSetup.lFrameControl
		>> strTemp >> m_tagSetup.lsimplemodelnum
		>> strTemp >> m_tagSetup.bupdateonlostfoucs;
		stream.close();

		//m_tagSetup.bWindow = !m_tagSetup.bWindow;
	}
	else
	{
		//AfxMessageBox("Can't find file setup/params.ini");
	}
}


void CUrlFileApp::SaveSetup()
{
	fstream stream;
	stream.open("setup/params.ini");

	if( stream.is_open() )
	{
		stream 
			//<< "fullscreen	"	<< !m_tagSetup.bWindow << endl
			//<< "width		"	<< m_tagSetup.lWidth << endl
			//<< "height		"	<< m_tagSetup.lHeight << endl
			//<< "BitsPerPel	"	<< m_tagSetup.lBitsPerPel << endl
			//<< "frequency	"	<< m_tagSetup.lFrequency << endl
			//<< "texturemode	"	<< m_tagSetup.lTexturemode << endl
			//<< "LIGHT		"	<< m_tagSetup.lLight << endl
			//<< "MODEL		"	<< m_tagSetup.lModel << endl
			//<< "SHADOW		"	<< m_tagSetup.lShadow << endl
			//<< "TEXTURE		"	<< m_tagSetup.lTexture << endl
			//<< "WEATHER		"	<< m_tagSetup.lWeather << endl
			//<< "CONTROL		"	<< m_tagSetup.lControl << endl
			//<< "VIEWRANGE	"	<< m_tagSetup.lViewRange << endl
			//<< "SOUND		"	<< m_tagSetup.bSound << endl 
			//<< "MUSIC		"	<< m_tagSetup.bMusic << endl
			//<< "SOUNDVOLUME	"	<< m_tagSetup.fSoundVolume << endl 
			//<< "MUSICVOLUME	"	<< m_tagSetup.fMusicVolume << endl;
			<< "fullscreen      "	<< m_tagSetup.bFullscreen << endl
			<< "width           "	<< m_tagSetup.lWidth << endl
			<< "height          "	<< m_tagSetup.lHeight << endl
			<< "BitsPerPel      "	<< m_tagSetup.lBitsPerPel << endl
			<< "frequency       "	<< m_tagSetup.lFrequency << endl
			<< "texturemode     "	<< m_tagSetup.lTexturemode << endl
			<< "2DLinear        "	<< m_tagSetup.b2DLinear << endl
			<< "3DLinear        "	<< m_tagSetup.b3DLinear << endl
			<< "LIGHT           "	<< m_tagSetup.lLight << endl
			<< "MODEL           "	<< m_tagSetup.lModel << endl
			<< "SHADOW          "	<< m_tagSetup.lShadow << endl
			<< "TEXTURE         "	<< m_tagSetup.lTexture << endl
			<< "RoleFlash       "	<< m_tagSetup.bRoleFlash << endl
			<< "RoleShake       "	<< m_tagSetup.bRoleShake << endl
			<< "ScreenShock     "	<< m_tagSetup.bShock << endl
			<< "WEATHER         "	<< m_tagSetup.lWeather << endl
			<< "CONTROL         "	<< m_tagSetup.lControl << endl
			<< "VIEWRANGE       "	<< m_tagSetup.lViewRange << endl
			<< "SOUND           "	<< m_tagSetup.bSound << endl 
			<< "MUSIC           "	<< m_tagSetup.bMusic << endl
			<< "SPEECH          "	<< m_tagSetup.bSpeech << endl
			<< "SOUNDVOLUME     "	<< m_tagSetup.fSoundVolume  << endl 
			<< "MUSICVOLUME     "	<< m_tagSetup.fMusicVolume  << endl
			<< "SPEECHVOLUME    "	<< m_tagSetup.fSpeechVolume  << endl
			<< "FOOTSTEP        "	<< m_tagSetup.bOthersFootstep  << endl
			<< "AntiAliasing    "   << m_tagSetup.lAntiAliasing  << endl
			<< "bUseVSync       "   << m_tagSetup.bUseVSync << endl
			<< "FrameControl    "   << m_tagSetup.lFrameControl << endl
		    << "lsimplemodelnum "   << m_tagSetup.lsimplemodelnum << endl
			<< "bupdatefoucs    "   << m_tagSetup.bupdateonlostfoucs << endl;
		stream.close();
	}
	else
	{
		//AfxMessageBox("Can't find file setup/params.ini");
	}
}

bool	CUrlFileApp::IsNeeded( D3DDISPLAYMODE & mode )
{
	if (mode.Width == 1024 && mode.Height == 768)
		return true;
	if (mode.Width == 1152 && mode.Height == 864)
		return true;
	if (mode.Width == 1280 && mode.Height == 960) 
		return true;
	if (mode.Width == 1280 && mode.Height == 1024)
		return true;
	if (mode.Width == 1680 && mode.Height == 1050)
		return true;

	return false;
}


void	CUrlFileApp::InsertMode( D3DDISPLAYMODE & mode, int n )
{
	if (n != 16 && n != 32)
		return;

	if (!IsNeeded( mode ))
		return;

	bool	bAdded = false;

	for (int i = 0; i < g_mDisplayMode.size(); ++i)
	{
		if (g_mDisplayMode[i].nWidth == mode.Width && g_mDisplayMode[i].nHeight == mode.Height )
		{
			g_mDisplayMode[i].vRefresh.insert( mode.RefreshRate );
			g_mDisplayMode[i].vColors.insert( n );

			bAdded = true;
		}
	}

	if (!bAdded)
	{
		DisplayMode	newmode;
		newmode.nHeight = mode.Height;
		newmode.nWidth = mode.Width;
		newmode.vColors.insert( n );
		newmode.vRefresh.insert( mode.RefreshRate );

		g_mDisplayMode.push_back(newmode);
	}
}

void	CUrlFileApp::CheckDisplayMode()
{

	DEVMODE DisplayMode;
	D3DDISPLAYMODE	mode;

	BOOL bReturn = true;

	int i = 0;
	while(bReturn)
	{
		bReturn = EnumDisplaySettings(NULL, i, &DisplayMode);
		if (bReturn)
		{
			mode.Height = DisplayMode.dmPelsHeight;
			mode.Width = DisplayMode.dmPelsWidth;
			mode.RefreshRate = DisplayMode.dmDisplayFrequency;
			InsertMode(mode, DisplayMode.dmBitsPerPel);
		}
		i++;
	}

}

void    CUrlFileApp::SetDisplayMode()
{

	bool bFlag = false;
	int i = 0;
	for (; i < g_mDisplayMode.size(); ++i)
	{
		if(m_tagSetup.lWidth==g_mDisplayMode[i].nWidth)
		{
			bFlag=true;
			break;
		}
	}

	if(i >= g_mDisplayMode.size() && i >= 0 )
	{
		i--;
		m_tagSetup.lWidth=g_mDisplayMode[i].nWidth;
		m_tagSetup.lHeight=g_mDisplayMode[i].nHeight;
	}

	if (g_mDisplayMode.size() > 0)
	{
		set<int>::iterator iterBeg1;
		for (iterBeg1  = g_mDisplayMode[i].vColors.begin(); iterBeg1 != g_mDisplayMode[i].vColors.end(); ++iterBeg1)
		{
			if(m_tagSetup.lBitsPerPel == *iterBeg1)
			{
				break;
			}
		}

		if(iterBeg1 == g_mDisplayMode[i].vColors.end())
		{
			iterBeg1--;
			m_tagSetup.lBitsPerPel = *iterBeg1;
		}

		set<int>::iterator iterBeg2;
		for (iterBeg2 = g_mDisplayMode[i].vRefresh.begin(); iterBeg2 != g_mDisplayMode[i].vRefresh.end(); ++iterBeg2)
		{
			if(m_tagSetup.lFrequency==*iterBeg2)
			{
				break;
			}
		}
		if(iterBeg2==g_mDisplayMode[i].vRefresh.end())
		{
			iterBeg2--;
			m_tagSetup.lFrequency=*iterBeg2;
		}
	}
}

bool CUrlFileApp::WriteReg(string strPath)
{
	BYTE cValues[] = { 0x11,0x12,0x55 };

	BYTE Vals[100];
	DWORD lenIt = 100;
	HKEY hk;

	//创建注册表项
	if(!::RegCreateKeyA(HKEY_LOCAL_MACHINE,"SOFTWARE\\xingman\\Luvinia\\",&hk)==ERROR_SUCCESS)
	{
		return false;
	}

	//设置注册表键值
	if(!::RegSetValueExA(hk,"Client_Path",0,REG_SZ,(LPBYTE)(LPCSTR)strPath.c_str(),strPath.size()+1)==ERROR_SUCCESS)
	{
		return false; 
	}

	//获得注册表键值
	//if(::RegQueryValueExA(hk,"Client_Path",0,NULL,(BYTE*)Vals,&lenIt)==ERROR_SUCCESS)
	//{}
	::RegCloseKey(hk);
	return true;
}