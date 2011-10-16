#pragma once


// CCreateNewRes 对话框

class CCreateNewRes : public CDialog
{
	DECLARE_DYNAMIC(CCreateNewRes)

public:
	CCreateNewRes(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCreateNewRes();

// 对话框数据
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    CString m_fileresname;
};
