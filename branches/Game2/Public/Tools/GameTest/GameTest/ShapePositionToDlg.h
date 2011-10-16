#pragma once
#include "afxwin.h"


// CShapePositionToDlg 对话框

class CShapePositionToDlg : public CDialog
{
	DECLARE_DYNAMIC(CShapePositionToDlg)

public:
	CShapePositionToDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShapePositionToDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ShapePosition };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	long lPosX;
public:
	long lPosY;
public:
	afx_msg void OnSetpositionto();
public:
	CEdit m_PosX;
public:
	CEdit m_PosY;
};
