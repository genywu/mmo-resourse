#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "urlfile.h"


// CSetupPage 对话框

class CSetupPage : public CDialog
{
	DECLARE_DYNAMIC(CSetupPage)

public:
	CSetupPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetupPage();


	void LoadSetup();
	void SaveSetup();
	void LoadOrginSetup();
	void Refeash(tagSetupEx* pSetup, bool bAuto=true); 
	void Refeash(tagSetup* pSetup, bool bAuto=true); 


	tagSetup m_OrginSetup[5];
	tagSetupEx m_tagSetup;

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_cSliderEffect;
	int m_lRes;
	int m_lColor;
	int m_lRefeash;
	int m_lModel;
	int m_lLight;
	int m_lShadow;
	int m_lTexture;
	int m_lWeather;
	int m_lAntiAliasing;
	int m_lUseVSync;
	int m_lFrameControl;
	int m_lControl;
	BOOL m_bFootStep;
	BOOL m_bUpdateOnFouce;
	CSliderCtrl m_cGamma;
	CSliderCtrl m_cViewRange;
	CSliderCtrl m_cMusic;
	CSliderCtrl m_cSound;
	BOOL m_bMusic;
	BOOL m_bSound;
	BOOL m_bSpeech;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	BOOL m_bWindow;
//	afx_msg void OnNMCustomdrawSliderEffect(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMThemeChangedSliderEffect(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderEffect(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderEffect(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_comResolution;
	CComboBox m_comColors;
	CComboBox m_comRefresh;
	afx_msg void OnCbnSelchangeComboRes();
	CSliderCtrl m_lSpeech;
};
