// QuestEditorDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "CAttribute.h"
#include "CStep.h"
#include "CEncouragement.h"
#include "afxwin.h"

#include <map>


// CQuestEditorDlg 对话框
class CQuestEditorDlg : public CDialog
{
// 构造
public:
	CQuestEditorDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CQuestEditorDlg();

// 对话框数据
	enum { IDD = IDD_QUESTEDITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	HACCEL Menu;;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnOK();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//Tab控件变量
	CTabCtrl Tab;		
	//3个页面的的变量
	CAttribute Attribute;		
	CStep Step;		
	CEncouragement Encouragement;	
   // 定义一个树型结构列表图标列表
	CTreeCtrl RoleList;
	//
	CImageList imagelist;
	//标签索引
	int m_CurSelTab;
	//
	CListBox m_Loglist;
	
	//初始化TAB界面的函数
	afx_msg void ShowTab();
	//显示文件列表函数
	afx_msg void ShowRoleList();
	//切换标签的函数
	afx_msg void OnNMClickTab(int TabNum);
	//树的右键相应函数
	afx_msg void OnNMRclickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	//在树中被选中的节点发生变化时的响应
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
    //Tab控件事件改变
    afx_msg void OnTvnSelchangedTab(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void ShowLog(vector<string>strlog);
    afx_msg void OnAddNewQuest();
	afx_msg void On32792();
	afx_msg void OnExit();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnDelQuest();
    afx_msg void OnCopyQuest();
    afx_msg void OnStickQuest();
    afx_msg void OnCreateTextRes();
    afx_msg void OnQuestQuery();

    afx_msg void OnOnlySaveQuest();
    afx_msg void OnSaveAllRes();
    afx_msg void OnReLoad();

    LRESULT OnHotKey(WPARAM wPararm,LPARAM lPararm);
    
    void    GoOnFindTreeItem();

private:
    long        CopyId;
    CString     m_Findtext;
    std::map<HTREEITEM,long>   m_VcCurrItem;

};
