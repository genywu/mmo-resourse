#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// Step 对话框

class CStep : public CDialog
{
	DECLARE_DYNAMIC(CStep)

public:
	CStep(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStep();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
		afx_msg void OnOK();
		afx_msg void ReadRoleStep(CString *pItemText);
		afx_msg void Showdetail(int StepID);
		afx_msg bool GetUpdatedStep(tagRoleStep* pUpdatedeStep);
		// 任务步骤列表
		CListCtrl m_StepList;
		// 任务文本列表
		CListCtrl m_StepTextList;
		// 就步获取的物品列表
		CListCtrl m_StepEncourageList;
		// 任务目标列表
		CListCtrl m_StepAimList;
		// 任务步骤ID
		int m_StepId;
		// 就歨的NPC原始名
		CString m_StepNpcId;
		// //.............
		int m_StepEffect_1;
		// .......................
		int m_StepEffect_2;
		// 显示模式
		int m_ShowNum;
		// 追踪物品的数量
		int m_Sum;
		// 追踪物品ID
		CString m_TraceID;
		// 确定计数的索引
		int m_Param;
		// 表示文本类型的ID
		int m_TextID;
		// 任务文本的索引
		int m_TextName;
		// 就歨奖励的物品类型
		CString m_StepEncouragementName;
		// 就歨奖励物品的数量
		int m_StepEncouragementNum;
		// 就歨奖励物品的类型
		int m_StepEncouragementType;
		//任务ID的标记变量,记录任务ID
		int m_nRoleID;
		//任务所在的文件名
		string	strFileName;
		// 任务追踪列表
		CListCtrl m_StepTraceList;
		//保存更新后的任务步骤的指针
		tagRoleStep* pUpdatedStep;
		//保存选中的任务的任务步骤
		tagRoleStep*  m_pSlectRoleStep;
		//tagRoleStep类型的迭代器
		vector<tagRoleStep*>::iterator StepIter;
		//界面各个控件的响应函数
		afx_msg void OnBnClickedButton8();
		afx_msg void OnBnClickedButton14();
		afx_msg void OnBnClickedButton10();
		afx_msg void OnBnClickedButton11();
		afx_msg void OnBnClickedButton5();
		afx_msg void OnBnClickedButton1();
		afx_msg void OnBnClickedButton9();
		afx_msg void OnBnClickedButton4();
		afx_msg void OnBnClickedButton6();
		afx_msg void OnBnClickedButton12();
		afx_msg void OnBnClickedButton13();
		afx_msg void OnBnClickedButton2();
		afx_msg void OnBnClickedButton3();
		afx_msg void OnBnClickedButton7();
		//界面中各个List控件的鼠标点击响应
		afx_msg void OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnNMClickList3(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnNMClickList5(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnNMClickList6(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnNMClickList4(NMHDR *pNMHDR, LRESULT *pResult);

        afx_msg void OnEnChangeEdit3();
        afx_msg void OnEnChangeEdit1();

        long m_QuestCell;
        CEdit m_TextIndex;
        CEdit m_TextContext;
        CString m_textDes;


private:
    bool  isLoading;
public:
    long m_QuestId;
    CEdit m_TraceDes;
    CEdit m_TraceShowNum;
    CEdit m_TraceSum;
    CEdit m_TraceParam;
    CEdit m_TraceId;
    CEdit m_AimId;
    CEdit m_AimModel;
    CEdit m_AimType;
    CEdit m_AimName;
    CEdit m_AimNum;
    CEdit m_DropFrom;
    CEdit m_DropOdds;
    CEdit m_AimParam;
    afx_msg void OnEnChangeEdit7();
    afx_msg void OnEnChangeEdit8();
    afx_msg void OnEnChangeEdit9();
    afx_msg void OnEnChangeEdit10();
    afx_msg void OnEnChangeEdit11();
    afx_msg void OnEnChangeEdit31();
    afx_msg void OnEnChangeEdit27();
    afx_msg void OnEnChangeEdit23();
    afx_msg void OnEnChangeEdit22();
    afx_msg void OnEnChangeEdit21();
    afx_msg void OnEnChangeEdit19();
    afx_msg void OnEnChangeEdit18();
    afx_msg void OnEnChangeEdit20();
};
