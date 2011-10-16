/*
=========================================================================
备注：暂未使用。2010.10.28  BY:LGR
=========================================================================
*/

/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：CSkillTest.h
*  功能：技能测试对话框界面
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
#include "GameTest.h"
#include "afxwin.h"
#include "..\Script\ScriptSys.h"

class CSkillTest : public CDialog
{
	DECLARE_DYNAMIC(CSkillTest)

public:

	CSkillTest(CWnd* pParent = NULL);
	virtual ~CSkillTest();

	enum { IDD = IDD_PROPPAGE_SKILL };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

public:

	//需要压入栈中的元素
	long m_RegionID;									//场景ID
	long m_lPosX;										//玩家的坐标X
	long m_lPosY;										//玩家的坐标Y
	long m_lHighAttack;									//玩家的最大攻击力	
	long m_lLowAttack;									//玩家的最小攻击力
	string m_lWeapon;										//压入的武器的种类
	long m_lOccu;										//机器人的职业
	long m_lSkillRange;									//使用的技能的范围

	long m_lCustomSkillID1;								//自定义的技能1
	long m_lCustomSkillID2;								//自定义的技能2
	long m_lCustomSkillID3;								//自定义的技能3
	long m_lCustomSkillID4;								//自定义的技能4

	long m_lSkillAccountBegin;							//起始账号
	long m_lSkillAccountEnd;							//结束账号
	long m_lTimer;										//定时器的时间间隔
	long m_lVersion;									//服务器的版本号
	string m_strIP;										//IP地址
	
	//初始化对话框
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedRadioAttackHigh();
	afx_msg void OnBnClickedRadioAttackLow();
	afx_msg void OnBnClickedRadioWeapon1();
	afx_msg void OnBnClickedRadioWeapon2();
	afx_msg void OnBnClickedRadioWeapon3();
	afx_msg void OnBnClickedRadioWeapon4();
	afx_msg void OnBnClickedRadioWeapon5();
	afx_msg void OnBnClickedRadioWeapon6();
	afx_msg void OnBnClickedRadioWeapon7();
	//
	afx_msg void OnBnClickedACCAnswer();   // By MartySa
	//
	afx_msg void OnBnClickedButtonSkillstart();

	CComboBox m_CombOccu;
	CComboBox m_CombIP;			

	afx_msg void OnCbnSelchangeComboOccu();
	afx_msg void OnCbnSelchangeComboSkillip();

	BOOL m_bUseSkill1;									//是否使用一转技能
	BOOL m_bUseSkill2;									//是否使用二转技能
	BOOL m_bUseSkill3;									//是否使用三转技能0

	BOOL m_bRandomPos;

	BOOL m_bIsCanACCAnswer;		//是否开启反外挂答题 add By MartySa 2009.7.10
	afx_msg void OnBnClickedCheck2();
	BOOL m_bUseSDO;
};
