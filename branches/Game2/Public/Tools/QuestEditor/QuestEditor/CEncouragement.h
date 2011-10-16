#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// Encouragement 对话框
class CEncouragement : public CDialog
{
	DECLARE_DYNAMIC(CEncouragement)

public:
	CEncouragement(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEncouragement();

// 对话框数据
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	  //重载回车键的响应函数
	  afx_msg void OnOK();
	  //读取任务里面的任务奖励信息
	  afx_msg void ReadRoleEncouragement(CString *pItemText);
	  //获取更新后的任务奖励信息
	  afx_msg bool GetUpdatedEn(vector<tagRoleEncouragement*>& vcUpdatedeEn,int& Cell);
	  //界面控件的响应
	  afx_msg void OnBnClickedButton3();
	  afx_msg void OnBnClickedButton1();
	  afx_msg void OnBnClickedButton2();
	  //显示界面中,List控件的响应
	  afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	  //保存当前所选择的任务的任务奖励
	  vector<tagRoleEncouragement*>m_pSelectedRoleEn;
	  //tagRoleEncouragement类型的迭代器
	  vector<tagRoleEncouragement*>::iterator iter;
	  //保存更新后了的任务的奖励
	  vector<tagRoleEncouragement*>vcUpdatedeEn;
	  //任务ID
	  int RoleId;
	  //任务文件名称
	  string strFileName;
	  //任务奖励物品所需要的包裹的数量
	  int m_EncouragementCell;
	  // //奖励物品的名称
	  CString m_EncouragementName;
	  // 奖励物品的数量
	  int m_EncouragementNum;
	  //显示任务奖励的类型
	  int m_EncouragementTypeIndex;
	  //定义一个CComBox控件
	  CComboBox m_EncouragementType;
	  // //显示任务奖励的列表
	  CListCtrl m_EncouragementList;
      CString m_textDes;

      afx_msg void OnEnChangeEdit3();
      afx_msg void OnEnChangeEdit1();


private:
    bool  isLoading ;
public:
    CString  m_Textfile;
    CEdit m_TextIndex;
    CEdit m_TextContext;

public:
    CString  filename;
};
