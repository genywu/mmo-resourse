/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：CSkillInfo.h
*  功能：单个玩家技能显示列表
*  当前版本：1.1.0
*  作者：
*  完成日期：
*  取代版本：1.0.0
*  原作者：
*  完成日期： 
*
============================================================
*/


#pragma once
#include "afxcmn.h"
#include "..\App\Player.h"
#include "afxwin.h"


#include <list>

using namespace std;


class CSkillInfo : public CDialog
{
	DECLARE_DYNAMIC(CSkillInfo)

public:
	CSkillInfo(CWnd* pParent = NULL);
	virtual ~CSkillInfo();

	enum { IDD = IDD_PROPPAGE_SKILL_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
private:

	//职业结构
	struct tagOccupation{
		string strOccupName;		//职业名称			
		long lOccupID;				//职业ID
		string strOccupDesc;		//职业描述
	};

	//职业技能结构
	struct tagOccupSkill{
		string strSkillName;		// 技能的名字
		long lSkillID;				// 技能ID
		long lSkillMaxLeve;			// 最多能够升的级数
		tagOccupSkill()
		{
			strSkillName = "";
			lSkillID = 0;
			lSkillMaxLeve = 0;
		}
	};
	list<tagOccupation*> m_OccupList;		// 所有的职业技能列表;
	vector<tagOccupSkill*> m_vecOccSkill;				
public:

	//load 玩家的技能
	BOOL LoadOccSkillListXml();
	BOOL Init(TiXmlElement* pElem);

	BOOL QuerySkill(tagPlayerSkill* pSkill);
	//bool IsBeing(CPlayer::tagSkill* pSkill);
	//load所有技能 包括怪物和npc

	BOOL LoadOneSkillXml();
	BOOL LoadSkillConfigXml(const char* str);


public:

	// 职业数据描述
	struct tagSkillOcc
	{
		string strOccName;						// 职业名称
		DWORD	dwOccId;						// 职业编号
	};
	// 技能基本数据描述
	struct tagLevelValue 
	{
		string	strKeyWord;						// 关键字
		DWORD	dwID;							// 数据ID
		DWORD	dwValue;						// 值
		tagLevelValue()
		{
			strKeyWord = "";
			dwID = 0;
			dwValue = 0;
		}
	};

	// 技能等级信息
	struct tagLevel
	{
		DWORD	dwSkill_Level;					// 技能等级
		string	strSkillLevelDesc;				// 技能等级描述
		string	strSkillLevelDescEx;			// 技能等级附加描述（描述下一级的基本信息等）
		DWORD	dwSkillLevelAI;					// 各等级技能AI
		vector<tagLevelValue> vecStepSend;		// 发送技能验证阶段
		vector<tagLevelValue> vecStepBegin;		// 技能开始阶段
		vector<tagLevelValue> vecStepRun;		// 技能使用成功阶段
		vector<tagLevelValue> vecStepEnd;		// 技能结束阶段
		vector<tagLevelValue> vecStepSummon;	// 召唤技能阶段
		//vector<tagLevelValue> vecStepState;	// 技能持有状态特效
		tagLevel()
		{
			dwSkill_Level = 0;
			strSkillLevelDesc = "";
			dwSkillLevelAI = 0;
		}
	};

	// 单个技能信息
	struct tagOneSkill
	{
		DWORD	dwSkillID;						// 技能编号
		string	strSkillName;					// 技能名称
		string	strSkillDesc;					// 技能描述
		DWORD	dwSkillState;					// 技能状态（一般技能、光环类技能）
		DWORD	dwSkillElement;					// 技能属性
		DWORD	dwSkillType;					// 技能类型(吟唱、释放)
		DWORD	dwSkillUseType;					// 技能使用类型（物理、魔法）
		bool	bLockTarget;					// 是否锁定目标
		bool	bLockSkill;						// 是否是锁定技能
		bool	bHasAct;						// 是否拥有动作
		vector<tagSkillOcc>		vecUseByOcc;		// 可用职业列表
		vector<tagLevel>	vectorLevels;		// 技能等级队列
		tagOneSkill()
		{
			dwSkillID = 0;
			strSkillName = "";
			strSkillDesc = "";
			dwSkillState = 0;
			dwSkillElement = 0;
			dwSkillType = 0;
			dwSkillUseType = 0;
			bLockSkill = false;
			bLockTarget = false;
			bHasAct = false;
		}
	};

private:

	//单个职业的技能结构体
	map<DWORD, tagOneSkill> m_mapSkillListXml;				// 所有技能数据队列
//	string m_strOneOccSkillName;
//	string m_strOneOccSkillDesc;
//	long m_lOneOccSkillID;
//	vector<tagOneOccupSkill*> m_vecOneOccSkillList;
//	void ReleaseOneOccSkill();

public:
	struct tagOneOccupSkill
	{
		string strOccSkillName;
		long lOccSkillID;
		long lOccMaxLevel;
		tagOneOccupSkill()
		{
			strOccSkillName = "";
			lOccSkillID = 0;
			lOccMaxLevel = 0;
		}
	};

public:

	static bool Init();

	CPlayer* m_pDisplaySkill; 
	CListCtrl m_CtrSkillList;
	virtual BOOL OnInitDialog();
	void SetDisplaySkill(CPlayer* pPlayer);
	CEdit m_edSkillInfo;
	CString m_strSkillInfo;
	//afx_msg void OnNMClickSkillList(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_OccSkillList;

public:

	//技能
	CStatic m_ctrlOccup;
	//afx_msg void OnNMRclickSkillList(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnSkillinfoUseskill();
	//long GetAllSkillNum()	{ return (long)m_vecOccSkill.size(); }
};