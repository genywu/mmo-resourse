#pragma once
#include "afxcmn.h"



// CAllGoodsPage 窗体视图

class CAllGoodsPage : public CFormView
{
	DECLARE_DYNCREATE(CAllGoodsPage)

public:
	CAllGoodsPage();           // 动态创建所使用的受保护的构造函数
	virtual ~CAllGoodsPage();
	void InitMyCtrls();
	void SetInitFlag(bool bVal){m_bInit=bVal;}
	bool GetInitFlag(){return m_bInit;}
	int GetSortedCol(){return m_nSortedCol;}
	bool GetSortFlag(){return m_bSortFlag;}
	static int CALLBACK MyCompareProc(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort);

public:
	enum { IDD = IDD_ALLGOODSDIALOG };

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool m_bInit;
	bool m_bSortFlag;
	int m_nSortedCol;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	friend class CContainerPage;
public:
	afx_msg void OnLvnItemchangedAllgoodslist(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_cAllGoodsList;
	afx_msg void OnHdnItemdblclickAllgoodslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkAllgoodslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownAllgoodslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReturnAllgoodslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnColumnclickAllgoodslist(NMHDR *pNMHDR, LRESULT *pResult);

	void ChangeItemText(DWORD dwIndex,int subitem,char* text);
	void UpDataAllGoodsList();
};


