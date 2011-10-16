// urlfile.h : main header file for the URLFILE application
//

#if !defined(AFX_URLFILE_H__BB9CC6D1_27C9_11D3_ACCC_00104B247756__INCLUDED_)
#define AFX_URLFILE_H__BB9CC6D1_27C9_11D3_ACCC_00104B247756__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "../../PUBLIC/ClientResource.h"

#include <vector>
#include <set>
#include <d3d9.h>
#include "Winreg.h"
using namespace std;

struct tagDisplayInfo
{
	CString	strText;		//文本显示值
	int		nMax;			//进度条最大值
	int		nCur;			//进度条当前值
};

/////////////////////////////////////////////////////////////////////////////
// CUrlFileApp:
// See urlfile.cpp for the implementation of this class
//
struct tagSetup
{
	bool	bWindow;
	long	lWidth;
	long	lHeight;
	long	lBitsPerPel;
	long	lFrequency;
	long	lTexturemode;
	long	lLight;
	long	lModel;
	long	lShadow;
	long	lTexture;
	long	lWeather;
	long	lControl;
	long	lViewRange;
	bool	bSound;
	bool	bMusic;
	float	fSoundVolume;
	float	fMusicVolume;
};
struct tagSetupEx
{

	BOOL	bFullscreen;
	long	lWidth;
	long	lHeight;
	long	lBitsPerPel;
	long	lFrequency;
	long	lTexturemode;
	bool	b2DLinear;		//是否开启2D线性纹理过滤模式
	bool	b3DLinear;		//是否开启3D线性纹理过滤模式
	long	lLight;
	long	lModel;
	long	lShadow;
	long	lTexture;
	bool	bRoleFlash;			//是否开启角色受伤闪光效果
	bool	bRoleShake;			//是否开启角色受伤震动效果
	bool	bShock;				//是否开启某些技能释放时,屏幕震动效果
	long	lWeather;
	long	lControl;
	long	lViewRange;
	bool	bSound;
	bool	bMusic;
	bool    bSpeech;
	float	fSoundVolume;
	float	fMusicVolume;
	float   fSpeechVolume;
	bool    bOthersFootstep;
	long    lAntiAliasing;
	bool    bUseVSync;
	long    lFrameControl;
	long    lsimplemodelnum;
	bool    bupdateonlostfoucs;
};

struct DisplayMode
{
	int		nWidth;
	int		nHeight;

	set<int>	vColors;
	set<int>	vRefresh;
};

typedef	vector<DisplayMode>	VECTORDISPLAYMODE;

class CUrlFileApp : public CWinApp
{
public:
	CUrlFileApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUrlFileApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CUrlFileApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	string					m_strWorkPath;									//工作路径
	string					m_strPackageName;								//包名
	string					m_strFilesInfoName;								//文件信息名字
	//CClientResource*		m_pClientResource;								//客户端资源信息
	//HINTERNET				m_hUpdateServerConnect;							//和更新服务器的连接

private:
	VECTORDISPLAYMODE	g_mDisplayMode;

	void LoadSetup();
	void SaveSetup();

	bool	IsNeeded( D3DDISPLAYMODE & mode );
	void	InsertMode( D3DDISPLAYMODE & mode, int n );
	void	CheckDisplayMode();
	void    SetDisplayMode();

	tagSetupEx m_tagSetup;

public:
	bool	InitResource();													//初始化客户端资源信息
	bool    WriteReg(string strPath);                                                      //每次运行时向注册表写入路径

	VECTORDISPLAYMODE& GetDisplayMode()	{return g_mDisplayMode;	}
};


void PutDebugString(const char*);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_URLFILE_H__BB9CC6D1_27C9_11D3_ACCC_00104B247756__INCLUDED_)
