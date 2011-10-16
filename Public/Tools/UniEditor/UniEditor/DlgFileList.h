#pragma once
#include "afxwin.h"


// CDlgFileList 对话框

class CDlgFileList : public CDialog
{
	DECLARE_DYNAMIC(CDlgFileList)

public:
	CDlgFileList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgFileList();

// 对话框数据
	enum { IDD = IDD_FILELIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	// 未保存文件的列表
	CListBox	m_listFile;
	int			m_iCurSel;

public:
	afx_msg void OnLbnSetfocusFilelist();
	afx_msg void OnLbnKillfocusFilelist();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedSaveAll();
	afx_msg void OnLbnSelcancelFilelist();
	afx_msg void OnLbnSelchangeFilelist();
};
