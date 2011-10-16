#pragma once


// CMerger 对话框

class CMerger : public CDialog
{
	DECLARE_DYNAMIC(CMerger)

public:
	bool MergerFile(CString strfilename);//合并属性文件
	bool MergerFileSkill(CString strfilename);//合并技能文件
	bool IsSame(CString strfilename,tagMonster *monster);//判断是否有重复的属性记录
	bool IsSameSkill(CString strfilename,tagMonster *monster);
	CMerger(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMerger();

// 对话框数据
	enum { IDD = IDD_MERGER };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	//打开文件控件
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnMerger();
};
