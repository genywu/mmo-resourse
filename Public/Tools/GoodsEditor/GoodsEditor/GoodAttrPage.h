#pragma once
#include "afxcmn.h"
#include "afxwin.h"



// CGoodAttrPage 窗体视图

class CGoodAttrPage : public CFormView
{
	DECLARE_DYNCREATE(CGoodAttrPage)

public:
	CGoodAttrPage();           // 动态创建所使用的受保护的构造函数
	virtual ~CGoodAttrPage();
	void InitMyCtrls();
	void ClearCtrls();
	void FillCtrls(int nIndex);
	int GetVecIndex(){return m_nVecIndex;}
	void SetVecIndex(int nIndex){m_nVecIndex=nIndex;}
	bool GetEditFlag(){return m_bEdit;}
	void SetEditFlag(bool bVal){m_bEdit=bVal;}
	bool GetAddFlag(){return m_bAdd;}
	void SetAddFlag(bool bVal){m_bAdd=bVal;}
	void ClearAttr();
	void SetGoodAttrTag(int nIndex);
	void EnableCtrlGroup();
	void DisableCtrlGroup();
public:
	enum { IDD = IDD_GOODSDETAIL };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool bInit;
	bool m_bEdit;
	bool m_bAdd;
	int m_nVecIndex;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	tagGoodAttr m_tagGoodAttr;
	vector<string> AttrNamesVec;

	afx_msg void OnCbnSelchangeTypecombo();
	int m_nSaveDB;
	CString m_strName;
	CString m_strUIID;
	CString m_strGroundID;
	CString m_strCosplayID;
	int m_nPrice;
	int m_nSilverPrice;
	int m_nGoodType;
	CString m_strSound;
	int m_nWeight;
	CString m_strDesc;
	int m_nAttrSel;
	int m_nVail;
	int m_nHide;
	int m_nVal1;
	int m_nVal2;

	int m_nOdds;
	int m_nMin;
	int m_nMax;
	int m_nOdds2;
	CListCtrl m_cAttrList;
	//CListCtrl m_cValList;
	CComboBox m_cAttrCombo;
	CComboBox m_cGoodType;
	afx_msg void OnCbnSelchangeAttrcombo();
	CComboBox m_cSaveDBCombo;
	CComboBox m_cHideCombo;
	CComboBox m_cVailCombo;	
	afx_msg void OnCbnSelchangeVailcombo();
	afx_msg void OnCbnSelchangeHidecombo();
	CEdit m_cValEdit1;
	CEdit m_cValEdit2;
	afx_msg void OnEnChangeValedit1();
	afx_msg void OnEnChangeValedit2();
	afx_msg void OnLvnItemchangedAttrlist(NMHDR *pNMHDR, LRESULT *pResult);
	//CComboBox m_cValSelCombo;
	int m_nValSel;

	friend class CMainFrame;
	friend class CContainerPage;
	CEdit m_cIDLab;
	afx_msg void OnCbnSelchangeSavedbcombo();
	afx_msg void OnEnChangeNameedit();
	CEdit m_cNameEdit;
	afx_msg void OnEnChangeUiedit();
	CEdit m_cUIEdit;
	CEdit m_cGroundEdit;
	CEdit m_cCosplayEdit;
	CEdit m_cPriceEdit;
	//CEdit m_cXSizeEdit;
	//CEdit m_cYSizeEdit;
	CEdit m_cSoundEdit;
	//CEdit m_cWeightEdit;
	CEdit m_cDescEdit;
	afx_msg void OnEnChangeGroundedit();
	afx_msg void OnEnChangeCosplayedit();
	afx_msg void OnEnChangePriceedit();
	//afx_msg void OnEnChangeXsizeedit();
	//afx_msg void OnEnChangeYsizeedit();
	afx_msg void OnEnChangeSoundedit();
	//afx_msg void OnEnChangeWeightedit();
	afx_msg void OnEnChangeDescedit();
	afx_msg void OnCbnSelchangeValselcombo();
	//CEdit m_cMinEdit;
	//CEdit m_cMaxEdit;
	//CEdit m_cOddsEdit;
	//CEdit m_cOddsEdit2;
	//CButton m_cAddBtn;
	//afx_msg void OnBnClickedAddbutton();	

	//afx_msg void OnEnChangeMinedit();
	//afx_msg void OnEnChangeMaxedit();
	//afx_msg void OnEnChangeOddsedit();
	//afx_msg void OnEnChangeOddsedit2();
	//CButton m_cDelBtn;
	CButton m_cDelAttrBtn;
	afx_msg void OnBnClickedDelattrbtn();
	//afx_msg void OnBnClickedDelbutton();
	CEdit m_cOrigName;
	afx_msg void OnEnChangeNameedit2();
	afx_msg void OnEnChangeGoodidlab();
	afx_msg void OnBnClickedButton2();
	CEdit m_cSound1;
	CEdit m_cSound2;
	CComboBox m_cSoundSwitch;
	//CEdit m_cWeaponActType;
	long m_lSoundID1;
	long m_lSoundID2;
	int m_bSoundSwitch;
	//long m_lWeaponActType;
	afx_msg void OnEnChangeSound1();
	afx_msg void OnEnChangeSound2();
	afx_msg void OnCbnSelchangeSoundswitch();
	//afx_msg void OnEnChangeWeaponActtype();


	afx_msg void OnEnKillfocusNameedit2();
	CButton m_cSaveAsButton;
	afx_msg void OnBnClickedSaveasbutton();
	//afx_msg void OnLvnItemchangedVallist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCopyattrbtn();
	afx_msg void OnBnClickedPasteattrbtn();
	afx_msg void OnBnClickedDelattrbtn2();
	afx_msg void OnBnClickedDelattrbtn3();
	afx_msg void OnCountEquip();
	afx_msg void OnBnClickedCountlevellimet();
public:
	CEdit m_cWeaponActType;	
public:
	long m_lWeaponActType;
public:
	afx_msg void OnEnChangeWeaponactedit();
public:
	CComboBox m_cAttrCombo2;
	//add by wang-xx 2009-6-30 begin for 激发属性
	CComboBox m_cArouseAttrCombo;
	CListCtrl m_cArouseAttrList;
	CEdit m_cArouseVal1;
	CEdit m_cArouseVal2;
	CEdit m_cArouseNum;
	//add by wang-xx 2009-6-30 end for 激发属性
public:
	CListCtrl m_cAttrList2;
public:
	CEdit m_cSuitValEdit1;
public:
	CEdit m_cSuitValEdit2;
public:
	CEdit m_cSuitNumEdit;
public:
	afx_msg void OnBnClickedButton1();
public:
	afx_msg void OnBnClickedButton4();
public:
	afx_msg void OnEnChangeEdit1();
public:
	afx_msg void OnEnChangeEdit2();
public:
	afx_msg void OnEnChangeEdit3();
public:
	void UpDataOwnAttrList(int mIndex);
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_cSilverPrice;
	afx_msg void OnEnChangeSilverPrice();
	afx_msg void OnBnClickedArouseAdd();
	afx_msg void OnBnClickedArouseDel();
	afx_msg void OnEnChangeArouseVal1();
	afx_msg void OnEnChangeArouseVal2();
	afx_msg void OnEnChangeArouseNum();
	afx_msg void OnNMClickArouseList(NMHDR *pNMHDR, LRESULT *pResult);
};


