// MonsterEditorDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CMonsterEditorDlg 对话框
class CMonsterEditorDlg : public CDialog
{
// 构造
public:
	void ReadFile();//读取文件
	void MonList();//显示到listcontrol
	void MonTree();//显示到Tree
	void ListSetData(vector<tagMonster>::iterator iter,int i);//设置listcontrol的值
	void LogList(vector<tagMonster>::iterator iter,int nflag);//制作日志文件
	void SaveFile(const char *filename,const char *filenameskill);//保存文件
	void RecoverFile(string strfilename,string strfilenameskill);//恢复文件
	CMonsterEditorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MONSTEREDITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	HACCEL m_hAccelTable;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_MonTree;
	CListCtrl m_MonList;
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNew();
	afx_msg void OnBnClickedButton1();
	
	CListBox m_LogListBox;
	afx_msg void OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDelete();//弹出式菜单，删除记录
	afx_msg void OnSaveToFile();//菜单，保存文件
	afx_msg void OnRecover();//菜单，恢复文件
	afx_msg void OnPartFile();

	afx_msg void OnMergerFile();
	afx_msg void OnClose();


};
