// YaYaDiskDlg.h : 头文件
//

#pragma once


// CYaYaDiskDlg 对话框
class CYaYaDiskDlg : public CDialog
{
// 构造
public:
	CYaYaDiskDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_YAYADISK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

    CFile m_File;

	// 生成的消息映射函数
    virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    //afx_msg void OnBnClickedOk();
    afx_msg void OnStnClickedQq();
    afx_msg void OnStnClickedBeach();
    afx_msg void OnStnClickedWind();
    afx_msg void OnStnClickedQqgame();
    afx_msg void OnStnClickedQqDance();
    afx_msg void OnStnClickedExit();
    afx_msg void OnStnClickedSmall();
};
