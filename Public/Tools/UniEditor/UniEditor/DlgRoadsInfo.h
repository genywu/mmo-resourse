#pragma once
#include "DlgEdit.h"

// CDlgRoadsInfo 对话框

class CDlgRoadsInfo : public CDlgEdit
{
	DECLARE_DYNAMIC(CDlgRoadsInfo)

public:
	CDlgRoadsInfo(CWnd* pParent = NULL);   // 标准构造函数
	void SetDlg(CRegion *pRegion, int n,CRoads::tagRoadsPoint *roadspoint=NULL, BOOL bALL = FALSE);		// 显示路径点
	void SetDlgNo(MAPROADS& mapRoads,int n);		// 显示路径点
	virtual BOOL OnInitDialog();
	virtual ~CDlgRoadsInfo();

// 对话框数据
	enum { IDD = IDD_ROADPOINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	CRoads::tagRoadsPoint *m_roadspoint;
	BOOL	m_bDlgEdit;
	BOOL m_Auto;
	int  m_RoadsID;
	MAPROADS m_MapRoads;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeRoadspointx();
	afx_msg void OnEnChangeRoadspointy();
	afx_msg void OnEnChangeRoadsno();
};
