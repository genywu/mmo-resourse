// SetupPage.cpp : 实现文件
//

#include "stdafx.h"
#include "urlfile.h"
#include "SetupPage.h"
#include ".\setuppage.h"
#include "CheckFilesDialog.h"

#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <d3d9.h>
using namespace std;

extern CUrlFileApp theApp;

VECTORDISPLAYMODE	g_vDisplayMode;

void CSetupPage::LoadOrginSetup()
{
	string strTemp;
	ifstream stream;
	stream.open("setup/params-orgin.ini");
	if( stream.is_open() )
	{
		stream
			>> strTemp >> m_OrginSetup[0].bWindow >> m_OrginSetup[1].bWindow >> m_OrginSetup[2].bWindow >> m_OrginSetup[3].bWindow >> m_OrginSetup[4].bWindow
			>> strTemp >> m_OrginSetup[0].lWidth >> m_OrginSetup[1].lWidth >> m_OrginSetup[2].lWidth >> m_OrginSetup[3].lWidth >> m_OrginSetup[4].lWidth
			>> strTemp >> m_OrginSetup[0].lHeight >> m_OrginSetup[1].lHeight >> m_OrginSetup[2].lHeight >> m_OrginSetup[3].lHeight >> m_OrginSetup[4].lHeight
			>> strTemp >> m_OrginSetup[0].lBitsPerPel >> m_OrginSetup[1].lBitsPerPel >> m_OrginSetup[2].lBitsPerPel >> m_OrginSetup[3].lBitsPerPel >> m_OrginSetup[4].lBitsPerPel
			>> strTemp >> m_OrginSetup[0].lFrequency >> m_OrginSetup[1].lFrequency >> m_OrginSetup[2].lFrequency >> m_OrginSetup[3].lFrequency >> m_OrginSetup[4].lFrequency
			>> strTemp >> m_OrginSetup[0].lTexturemode >> m_OrginSetup[1].lTexturemode >> m_OrginSetup[2].lTexturemode >> m_OrginSetup[3].lTexturemode >> m_OrginSetup[4].lTexturemode
			>> strTemp >> m_OrginSetup[0].lLight >> m_OrginSetup[1].lLight >> m_OrginSetup[2].lLight >> m_OrginSetup[3].lLight >> m_OrginSetup[4].lLight
			>> strTemp >> m_OrginSetup[0].lModel >> m_OrginSetup[1].lModel >> m_OrginSetup[2].lModel >> m_OrginSetup[3].lModel >> m_OrginSetup[4].lModel
			>> strTemp >> m_OrginSetup[0].lShadow >> m_OrginSetup[1].lShadow >> m_OrginSetup[2].lShadow >> m_OrginSetup[3].lShadow >> m_OrginSetup[4].lShadow
			>> strTemp >> m_OrginSetup[0].lTexture >> m_OrginSetup[1].lTexture >> m_OrginSetup[2].lTexture >> m_OrginSetup[3].lTexture >> m_OrginSetup[4].lTexture
			>> strTemp >> m_OrginSetup[0].lWeather >> m_OrginSetup[1].lWeather >> m_OrginSetup[2].lWeather >> m_OrginSetup[3].lWeather >> m_OrginSetup[4].lWeather
			>> strTemp >> m_OrginSetup[0].lControl >> m_OrginSetup[1].lControl >> m_OrginSetup[2].lControl >> m_OrginSetup[3].lControl >> m_OrginSetup[4].lControl
			>> strTemp >> m_OrginSetup[0].lViewRange >> m_OrginSetup[1].lViewRange >> m_OrginSetup[2].lViewRange >> m_OrginSetup[3].lViewRange >> m_OrginSetup[4].lViewRange
			>> strTemp >> m_OrginSetup[0].bSound >> m_OrginSetup[1].bSound >> m_OrginSetup[2].bSound >> m_OrginSetup[3].bSound >> m_OrginSetup[4].bSound
			>> strTemp >> m_OrginSetup[0].bMusic >> m_OrginSetup[1].bMusic >> m_OrginSetup[2].bMusic >> m_OrginSetup[3].bMusic >> m_OrginSetup[4].bMusic
			>> strTemp >> m_OrginSetup[0].fSoundVolume  >> m_OrginSetup[1].fSoundVolume  >> m_OrginSetup[2].fSoundVolume  >> m_OrginSetup[3].fSoundVolume  >> m_OrginSetup[4].fSoundVolume 
			>> strTemp >> m_OrginSetup[0].fMusicVolume >> m_OrginSetup[1].fMusicVolume >> m_OrginSetup[2].fMusicVolume >> m_OrginSetup[3].fMusicVolume >> m_OrginSetup[4].fMusicVolume;
		stream.close();
	}
}

void CSetupPage::LoadSetup()
{
	string strTemp;
	ifstream stream;
	stream.open("setup/params.ini");
	if( stream.is_open() )
	{
		tagSetupEx stSetup;

		//stream	>> strTemp >> stSetup.bWindow
		//	>> strTemp >> stSetup.lWidth
		//	>> strTemp >> stSetup.lHeight
		//	>> strTemp >> stSetup.lBitsPerPel
		//	>> strTemp >> stSetup.lFrequency
		//	>> strTemp >> stSetup.lTexturemode
		//	>> strTemp >> stSetup.lLight
		//	>> strTemp >> stSetup.lModel
		//	>> strTemp >> stSetup.lShadow
		//	>> strTemp >> stSetup.lTexture
		//	>> strTemp >> stSetup.lWeather
		//	>> strTemp >> stSetup.lControl
		//	>> strTemp >> stSetup.lViewRange
		//	>> strTemp >> stSetup.bSound
		//	>> strTemp >> stSetup.bMusic
		//	>> strTemp >> stSetup.fSoundVolume 
		//	>> strTemp >> stSetup.fMusicVolume;
		stream	>> strTemp >> stSetup.bFullscreen
		>> strTemp >> stSetup.lWidth
		>> strTemp >> stSetup.lHeight
		>> strTemp >> stSetup.lBitsPerPel
		>> strTemp >> stSetup.lFrequency
		>> strTemp >> stSetup.lTexturemode
		>> strTemp >> stSetup.b2DLinear
		>> strTemp >> stSetup.b3DLinear
		>> strTemp >> stSetup.lLight
		>> strTemp >> stSetup.lModel
		>> strTemp >> stSetup.lShadow
		>> strTemp >> stSetup.lTexture
		>> strTemp >> stSetup.bRoleFlash
		>> strTemp >> stSetup.bRoleShake
		>> strTemp >> stSetup.bShock
		>> strTemp >> stSetup.lWeather
		>> strTemp >> stSetup.lControl
		>> strTemp >> stSetup.lViewRange
		>> strTemp >> stSetup.bSound
		>> strTemp >> stSetup.bMusic
		>> strTemp >> stSetup.bSpeech
		>> strTemp >> stSetup.fSoundVolume 
		>> strTemp >> stSetup.fMusicVolume
		>> strTemp >> stSetup.fSpeechVolume
		>> strTemp >> stSetup.bOthersFootstep
		>> strTemp >> stSetup.lAntiAliasing
		>> strTemp >> stSetup.bUseVSync
		>> strTemp >> stSetup.lFrameControl
		>> strTemp >> stSetup.lsimplemodelnum
		>> strTemp >> stSetup.bupdateonlostfoucs;
		stream.close();

		//stSetup.bWindow = !stSetup.bWindow;

		if(stSetup.fSoundVolume>255)
			stSetup.fSoundVolume= 255;
		if(stSetup.fMusicVolume>255)
			stSetup.fMusicVolume=255;
		if(stSetup.fSpeechVolume>255)
			stSetup.fSpeechVolume=255;

		if(stSetup.fSoundVolume<0)
			stSetup.fSoundVolume= 0;
		if(stSetup.fMusicVolume<0)
			stSetup.fMusicVolume=0;
		if(stSetup.fSpeechVolume<0)
			stSetup.fSpeechVolume=0;
		if(stSetup.lShadow==2)
		{
			stSetup.lShadow=1;
		}
		if(stSetup.lViewRange>88)
			stSetup.lViewRange = 88;
		if(stSetup.lViewRange<40)
			stSetup.lViewRange = 40;
		Refeash(&stSetup);

		m_tagSetup = stSetup;
	}
	else
	{
		MessageBox("Can't find file setup/params.ini", "message", MB_OK);
	}
}

void CSetupPage::Refeash(tagSetupEx* pSetup, bool bAuto)
{
	if (bAuto)
	{
		m_bWindow = !pSetup->bFullscreen;
		m_lControl = pSetup->lControl;
		m_bSound = pSetup->bSound;
		m_bSpeech = pSetup->bSpeech;
		m_bMusic = pSetup->bMusic;
		m_bUpdateOnFouce = pSetup->bupdateonlostfoucs;
		m_bFootStep = pSetup->bOthersFootstep;
		m_cSound.SetPos(pSetup->fSoundVolume);
		m_cMusic.SetPos(pSetup->fMusicVolume);
		m_lSpeech.SetPos(pSetup->fSpeechVolume);
		m_cGamma.SetPos(50);

		if (pSetup->lFrequency==75)
			m_lRefeash=1;
		else if (pSetup->lFrequency==85)
			m_lRefeash=2;
		else
			m_lRefeash=0;
	}

	m_lLight = pSetup->lLight;
	m_lModel = pSetup->lModel;
	m_lShadow = pSetup->lShadow;
	m_lTexture = pSetup->lTexture;
	m_lWeather = pSetup->lWeather;
	m_lAntiAliasing = pSetup->lAntiAliasing;


	if (pSetup->lWidth==800)
		m_lRes = 0;
	else if (pSetup->lWidth==1024)
		m_lRes = 1;
	else if (pSetup->lWidth==1280)
		m_lRes = 2;
	else if (pSetup->lWidth==1600)
		m_lRes = 3;

	if (pSetup->lBitsPerPel==32)
		m_lColor = 1;
	else
		m_lColor = 0;

	m_cViewRange.SetPos(pSetup->lViewRange);
	//m_comResolution.SetCurSel(3);
	m_cSliderEffect.SetPos(m_lRes);

	UpdateData(false);
}

void CSetupPage::Refeash(tagSetup* pSetup, bool bAuto)
{
	if (bAuto)
	{
		m_bWindow = !pSetup->bWindow;
		m_lControl = pSetup->lControl;
		m_bSound = pSetup->bSound;
		m_bMusic = pSetup->bMusic;
		m_cSound.SetPos(pSetup->fSoundVolume);
		m_cMusic.SetPos(pSetup->fMusicVolume);
		m_cGamma.SetPos(50);

		if (pSetup->lFrequency==75)
			m_lRefeash=1;
		else if (pSetup->lFrequency==85)
			m_lRefeash=2;
		else
			m_lRefeash=0;
	}

	m_lLight = pSetup->lLight;
	m_lModel = pSetup->lModel;
	m_lShadow = pSetup->lShadow;
	m_lTexture = pSetup->lTexture;
	m_lWeather = pSetup->lWeather;

	if (pSetup->lWidth==800)
		m_lRes = 0;
	else if (pSetup->lWidth==1024)
		m_lRes = 1;
	else if (pSetup->lWidth==1280)
		m_lRes = 2;
	else if (pSetup->lWidth==1600)
		m_lRes = 3;

	if (pSetup->lBitsPerPel==32)
		m_lColor = 1;
	else
		m_lColor = 0;

	m_cViewRange.SetPos(pSetup->lViewRange);
	//m_comResolution.SetCurSel(m_lRes);
	//m_cSliderEffect.SetPos(m_lRes);
	

	UpdateData(false);
}

void CSetupPage::SaveSetup()
{
	fstream stream;
	stream.open("setup/params.ini");

	int iResolution = m_comResolution.GetCurSel();
	int iColors = m_comColors.GetCurSel();
	int iRefresh = m_comRefresh.GetCurSel();

	if (iResolution == -1 || iColors == -1 || iRefresh == -1)
		stream.close();

	if( stream.is_open() )
	{
		long res[4][2] = {800,600,  1024,768,  1280,960, 1600,1200};
		long color[2] = {16, 32};
		long refeash[3] = {60, 75, 85};


		CString strColor, strRef;

		m_comColors.GetLBText(iColors, strColor);
		m_comRefresh.GetLBText(iRefresh, strRef);


		long lWidth =  g_vDisplayMode[iResolution].nWidth;//res[m_lRes][0]; 
		long lHeight = g_vDisplayMode[iResolution].nHeight;
		//m_bWindow = 1;
		long lBitsPerPel = atoi(strColor);
		long lFrequency = atoi(strRef);
		long lTexturemode = 32;
		long lViewRange = m_cViewRange.GetPos();
		float fSoundVolume = (float)m_cSound.GetPos();
		float fMusicVolume = (float)m_cMusic.GetPos() ;
		float fSpeechVolume = (float)m_lSpeech.GetPos();
		m_tagSetup.bupdateonlostfoucs = m_bUpdateOnFouce;
		if(m_lShadow==1)
		{
			m_lShadow=2;
		}

		stream 
			<< "fullscreen      "	<< !m_bWindow << endl
			<< "width           "	<< lWidth << endl
			<< "height          "	<< lHeight << endl
			<< "BitsPerPel      "	<< lBitsPerPel << endl
			<< "frequency       "	<< lFrequency << endl
			<< "texturemode     "	<< lTexturemode << endl
			<< "2DLinear        "	<< m_tagSetup.b2DLinear << endl
			<< "3DLinear        "	<< m_tagSetup.b3DLinear << endl
			<< "LIGHT           "	<< m_lLight << endl
			<< "MODEL           "	<< m_lModel << endl
			<< "SHADOW          "	<< m_lShadow << endl
			<< "TEXTURE         "	<< m_lTexture << endl
			<< "RoleFlash       "	<< m_tagSetup.bRoleFlash << endl
			<< "RoleShake       "	<< m_tagSetup.bRoleShake << endl
			<< "ScreenShock     "	<< m_tagSetup.bShock << endl
			<< "WEATHER         "	<< m_lWeather << endl
			<< "CONTROL         "	<< m_lControl << endl
			<< "VIEWRANGE       "	<< lViewRange << endl
			<< "SOUND           "	<< m_bSound << endl 
			<< "MUSIC           "	<< m_bMusic << endl
			<< "SPEECH          "	<< m_bSpeech << endl
			<< "SOUNDVOLUME     "	<< fSoundVolume   << endl 
			<< "MUSICVOLUME     "	<< fMusicVolume   << endl
		    << "SPEECHVOLUME    "	<< fSpeechVolume  << endl
			<< "FOOTSTEP        "	<< m_bFootStep << endl
			<< "AntiAliasing    "   << m_lAntiAliasing << endl
			<< "bUseVSync       "   << m_tagSetup.bUseVSync <<endl
			<< "FrameControl    "   << m_tagSetup.lFrameControl << endl
			<< "lsimplemodelnum "   << m_tagSetup.lsimplemodelnum << endl
			<< "bupdatefoucs    "   << m_tagSetup.bupdateonlostfoucs << endl;
		stream.close();
	}
	else
	{
		MessageBox("Can't find file setup/params.ini", "message", MB_OK);
	}
}

// CSetupPage 对话框

IMPLEMENT_DYNAMIC(CSetupPage, CDialog)
CSetupPage::CSetupPage(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupPage::IDD, pParent)
	, m_lRes(0)
	, m_lColor(0)
	, m_lRefeash(0)
	, m_lModel(0)
	, m_lLight(0)
	, m_lShadow(0)
	, m_lTexture(0)
	, m_lWeather(0)
	, m_lControl(0)
	, m_bFootStep(0)
	, m_bMusic(FALSE)
	, m_bSound(FALSE)
	, m_bSpeech(FALSE)
	, m_bWindow(FALSE)
	, m_bUpdateOnFouce(FALSE)
{
}

CSetupPage::~CSetupPage()
{
	g_vDisplayMode.clear();
}

void CSetupPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_EFFECT, m_cSliderEffect);
	DDX_CBIndex(pDX, IDC_COMBO_RES, m_lRes);
	DDX_CBIndex(pDX, IDC_COMBO_COLOR, m_lColor);
	DDX_CBIndex(pDX, IDC_COMBO_REFEASH, m_lRefeash);
	DDX_CBIndex(pDX, IDC_COMBO_MODEL, m_lModel);
	DDX_CBIndex(pDX, IDC_COMBO_LIGHT, m_lLight);
	DDX_CBIndex(pDX, IDC_COMBO_SHADOW, m_lShadow);
	DDX_CBIndex(pDX, IDC_COMBO_TEXTURE, m_lTexture);
	DDX_CBIndex(pDX, IDC_COMBO_WEAHTER, m_lWeather);
	DDX_CBIndex(pDX, IDC_COMBO_CONTROL, m_lControl);
	DDX_Control(pDX, IDC_SLIDER_GAMMA, m_cGamma);
	DDX_Control(pDX, IDC_SLIDER_VIEWRANGE, m_cViewRange);
	DDX_Control(pDX, IDC_SLIDER_MUSIC, m_cMusic);
	DDX_Control(pDX, IDC_SLIDER_SOUND, m_cSound);
	DDX_Check(pDX, IDC_CHECK1_MUSIC, m_bMusic);
	DDX_Check(pDX, IDC_CHECK_SOUND, m_bSound);
	DDX_Check(pDX, IDC_CHECK_SPEECH, m_bSpeech);
	DDX_Check(pDX, IDC_CHECK_WINDOW, m_bWindow);
	DDX_Control(pDX, IDC_COMBO_RES, m_comResolution);
	DDX_Control(pDX, IDC_COMBO_COLOR, m_comColors);
	DDX_Control(pDX, IDC_COMBO_REFEASH, m_comRefresh);
	DDX_Control(pDX, IDC_SLIDER_Speech, m_lSpeech);
	DDX_Check(pDX, IDC_CHECK_FOOTSTEP, m_bFootStep);
	DDX_Check(pDX,IDC_CHECK_UPDATEONFOUCE,m_bUpdateOnFouce);
}


BEGIN_MESSAGE_MAP(CSetupPage, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
//	ON_WM_CREATE()
//	ON_WM_ACTIVATE()
//	ON_WM_ACTIVATEAPP()
//ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_EFFECT, OnNMCustomdrawSliderEffect)
ON_NOTIFY(NM_THEMECHANGED, IDC_SLIDER_EFFECT, OnNMThemeChangedSliderEffect)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_EFFECT, OnNMCustomdrawSliderEffect)
ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_EFFECT, OnNMReleasedcaptureSliderEffect)
ON_CBN_SELCHANGE(IDC_COMBO_RES, OnCbnSelchangeComboRes)
END_MESSAGE_MAP()


// CSetupPage 消息处理程序

void CSetupPage::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	SaveSetup();
	OnOK();
}


//int CSetupPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CDialog::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	// TODO:  在此添加您专用的创建代码
//
//	return 0;
//}

//void CSetupPage::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
//{
//	CDialog::OnActivate(nState, pWndOther, bMinimized);
//
//	// TODO: 在此处添加消息处理程序代码
//}

//void CSetupPage::OnActivateApp(BOOL bActive, DWORD dwThreadID)
//{
//	CDialog::OnActivateApp(bActive, dwThreadID);
//
//	// TODO: 在此处添加消息处理程序代码
//}

BOOL CSetupPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cGamma.EnableWindow(false);

	// TODO:  在此添加额外的初始化

	m_cSliderEffect.SetRange(0, 4);
	m_cGamma.SetRange(0, 100);
	m_cViewRange.SetRange(40, 88);  
	m_cSound.SetRange(0, 255);
	m_cMusic.SetRange(0, 255);
	m_lSpeech.SetRange(0,255);


	LoadOrginSetup();
	LoadSetup();

	//CheckDisplayMode();
	g_vDisplayMode = theApp.GetDisplayMode();

	m_comResolution.ResetContent();
	m_comColors.ResetContent();
	m_comRefresh.ResetContent();


	char	szTemp[1024];
	int i = 0;
	for (; i < g_vDisplayMode.size(); ++i)
	{
		sprintf(szTemp, "%d ╳ %d", g_vDisplayMode[i].nWidth, g_vDisplayMode[i].nHeight);
		m_comResolution.InsertString(-1, szTemp);

		if(m_tagSetup.lWidth==g_vDisplayMode[i].nWidth)
		{
			m_lRes= i;
		}
	}

	if(m_lRes >= g_vDisplayMode.size())
		i--;

	if (g_vDisplayMode.size() > 0)
	{
		int iCount = 0;
		for (set<int>::iterator iterBeg  = g_vDisplayMode[m_lRes].vColors.begin(); iterBeg != g_vDisplayMode[m_lRes].vColors.end(); ++iterBeg)
		{
			sprintf(szTemp, "%d", *iterBeg);
			m_comColors.InsertString(-1, szTemp);

			if(m_tagSetup.lBitsPerPel == *iterBeg)
			{
				m_lColor = iCount;
			}
			iCount++;
		}

		iCount=0;
		for (set<int>::iterator iterBeg = g_vDisplayMode[m_lRes].vRefresh.begin(); iterBeg != g_vDisplayMode[m_lRes].vRefresh.end(); ++iterBeg)
		{
			sprintf(szTemp, "%d", *iterBeg);
			m_comRefresh.InsertString( -1 , szTemp);
			if(m_tagSetup.lFrequency==*iterBeg)
			{
				m_lRefeash = iCount;
			}
			iCount++;
		}
	}

	m_comResolution.SetCurSel(m_lRes);
	m_comColors.SetCurSel(m_lColor);
	m_comRefresh.SetCurSel(m_lRefeash);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//void CSetupPage::OnNMCustomdrawSliderEffect(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//	// TODO: 在此添加控件通知处理程序代码
//	*pResult = 0;
//}

void CSetupPage::OnNMThemeChangedSliderEffect(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 该功能要求使用 Windows XP 或更高版本。
	// 符号 _WIN32_WINNT 必须 >= 0x0501。
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CSetupPage::OnNMCustomdrawSliderEffect(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码


	*pResult = 0;
}

void CSetupPage::OnNMReleasedcaptureSliderEffect(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(true);
	Refeash(&m_OrginSetup[m_cSliderEffect.GetPos()], false);

	*pResult = 0;
}


void CSetupPage::OnCbnSelchangeComboRes()
{
	// TODO: 在此添加控件通知处理程序代码
	int n = m_comResolution.GetCurSel();
	if (-1 == n)
		return;

	m_comColors.ResetContent();
	m_comRefresh.ResetContent();

	char szTemp[1024];
	for (set<int>::iterator iterBeg  = g_vDisplayMode[n].vColors.begin(); iterBeg != g_vDisplayMode[n].vColors.end(); ++iterBeg)
	{
		sprintf(szTemp, "%d", *iterBeg);
		m_comColors.InsertString(-1, szTemp);
	}

	for (set<int>::iterator iterBeg = g_vDisplayMode[n].vRefresh.begin(); iterBeg != g_vDisplayMode[n].vRefresh.end(); ++iterBeg)
	{
		sprintf(szTemp, "%d", *iterBeg);
		m_comRefresh.InsertString( -1 , szTemp);
	}

	m_comColors.SetCurSel(m_comColors.GetCount()-1);
	m_comRefresh.SetCurSel(m_comRefresh.GetCount()-1);
	m_comColors.Invalidate();
	m_comRefresh.Invalidate();
}
