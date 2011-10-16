#pragma once
#include "Collection.h"
#include "DlgEdit.h"
// CDlgCollection 对话框

class CDlgCollection : public  CDlgEdit
{
	DECLARE_DYNAMIC(CDlgCollection)

public:
	CDlgCollection(CWnd* pParent = NULL);   // 标准构造函数
	void SetDlg(CRegion *pRegion, CCollection *pCollection, BOOL bALL = FALSE);
	virtual ~CDlgCollection();
	virtual BOOL OnInitDialog();
	
// 对话框数据
	enum { IDD = IDD_COLLECTION };

protected:
	CCollection		*m_pCollection;						// 当前对应Collection指针
	CRegion			*m_pRegion;							// 当前对应Region指针
	BOOL			m_bDlgEdit;
	BOOL			m_bError;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	DECLARE_MESSAGE_MAP()
public:
	int m_ncollleft;
	afx_msg void OnEnChangeCollNo();
	afx_msg void OnEnChangeCollCoorLeft();
	afx_msg void OnEnChangeCollCoorTop();
	afx_msg void OnEnChangeCollCoorRight();
	afx_msg void OnEnChangeCollCoorBottom();
	afx_msg void OnEnChangeCollQuantity();
	afx_msg void OnEnChangeCollTimeBorn();
	afx_msg void OnEnChangeCollTimeStart();
	afx_msg void OnEnChangeCollDirection();
	afx_msg void OnBnClickedCheckBorn();
	afx_msg void OnBnClickedCheckCollBorn();
};
