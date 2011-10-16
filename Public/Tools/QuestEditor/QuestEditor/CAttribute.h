#pragma once
#include "afxwin.h"
#include <string.h>
#include "resource.h"
#include "struct.h"
#include "afxcmn.h"

// Attribute 对话框

class CAttribute : public CDialog
{
	DECLARE_DYNAMIC(CAttribute)

public:
	// 任务ID
	int m_RoleID;
	//文件名称
	string	strFileName;
	// 任务类型索引
	int m_RoleType;
	// ……………………
	int m_RoleEmbracer;
	//
	int m_ShowAble;
	//
	int m_BeRecord;
	//
	int m_Repeated;
	//
	int m_Deadlost;
	//
	int	m_LostRepeated;
	// 任务难度
	int m_HardFactor;
	// 删除任务脚本
	CString m_DeleteScript;
	// 任务名称
	CString m_RoleName;
	//
	int m_EmbracerEffrct;
	// 完成任务时的特效
	int m_FinishEmbracerEffect;
	// 接受任务时的地图特效
	int m_EmbracerMapEffect;
	// 任务的稀有度
	int m_Odds;
	// 集结点
	int m_MassPoint;
	// 任务描述文本的ID
	CString m_DiscribTextID;
	// 最小等级限制
	int m_MiniGrade;
	// 最大等级限制
	int m_MaxGrade;
	// 佣兵声望需求
	int m_EngageArmsRepute;
	// 任务提示类型
	int m_NPCShowType;
	// NPC原始名
	CString m_NPCID;
	// 完成任务特效
	int m_FinishEffect;
	// 任务线索类型
	int m_NPCClewType;
	// 查询任务描述索引
	CString m_BeSearchDesc;
	// 查询任务简述索引
	CString m_BeSearchSpDesc;
	// 章数________主线特有
	int m_ChapterNum;
	// 图片________主线任务特有
	int m_picture;
	// 图标_______主线任务特有
	int m_Icon;
	// 主线任务描述文本
	CString m_StoryDesc;
	// //前序任务索引
	CString m_RoleIndex;
	// 是否被记录
	CComboBox m_BoxBeRecord;
	// 是否显示
	CComboBox m_BoxShowAble;
	// 是否重复
	CComboBox m_BoxRepeated;
	// 死亡是否失败
	CComboBox m_BoxDeadlost;
	// 死亡后是否可以重复
	CComboBox m_BoxLostRepeated;
	CAttribute(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAttribute();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
		//保存当前选中的任务的任务属性
		tagRoleAttribute*           m_pSelectedRoleAttribute;
        tagRole         *           m_pRole;

	    //重载回车键的相应函数
	    afx_msg void OnOK();
		//从文件中读取文件信息的函数
		afx_msg void ReadRoleAttribute(CString *pItemText);
		//从界面获取更改有的任务属性信息
		afx_msg bool GetUpdatedAttribute(vector<tagRoleAttribute*>& vcUpdatedeAttribute);
		//确定控件的响应函数
		afx_msg void OnBnClickedButton1();

        //
        afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnCbnSelchangeCombo1();

        CString m_Sex;
        long m_ReqSkill;
        CString m_DDoc;
        CString m_Occupation;
        CListCtrl m_EncouragementList;
        CString m_QuestType;
        CString m_QuestShare;
        CString m_DelScript;
        long m_TotePet;
//         long m_EncouragementCell;
//         CString m_AwardName;
//         long m_AwardNum;
//         double m_AwardOdds;


private:
       bool   isLoading ;
public:
    CString m_AwardTypeDes;
    //CString m_AwardType;
    afx_msg void OnEnChangeEdit37();
    afx_msg void OnEnChangeEdit3();
    afx_msg void OnEnChangeEdit8();
    afx_msg void OnEnChangeEdit36();
    CEdit m_EncouragementCell;
    CComboBox m_AwardType;
    CEdit m_AwardName;
    CEdit m_AwardOdds;

    long  m_QuestId;
    CEdit m_AwardNum;
    afx_msg void OnBnClickedButton7();
    afx_msg void OnBnClickedButton8();
};
