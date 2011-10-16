/// 用于搜索物品
/// Add by 肖杨 2010.5.21
#pragma once
#include "afxwin.h"


// Search 对话框

class Search : public CDialog
{
	DECLARE_DYNAMIC(Search)

public:
	Search(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Search();

// 对话框数据
	enum { IDD = IDD_SEARCH };

// 搜索方式
	//enum{
	//	ST_ID=0,			// 按物品ＩＤ值进行搜索
	//	ST_ORINAME=1,		// 按物品原始名进行搜索
	//	ST_SHOWNAME=2,　	// 按物品显示名称进行搜索
	//};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeSearchoption();
	afx_msg void OnEnKillfocusSearchcontent();
	afx_msg void OnBnClickedCancel();
	// 搜索方式选择框变量
	CComboBox m_choseType;
	afx_msg void OnBnClickedNext();
	// 要搜索的内容
	CEdit m_SerContent;

	// 保存上一次搜索到的物品索引值
	int m_preSerResult;

	// 检测字符窜是否是数字
	bool CheckStrIsNum(const char* str,DWORD len);
};
