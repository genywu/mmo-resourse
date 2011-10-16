///////////////////////////////////////////////////////////////////////////////
///  State.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\WorldServer\AppWorld\State
///
///  CREATED: 06/06/2007 13:49:01 PM by 陈先进
///
///  PURPOSE:   状态类
///
///  COPYRIGHT NOTICE:   Copyright (C) Aurora Game
///
///  LAST CHANGED: $Date$
///
///  REVISION HISTORY:
///  $Log$
/// 

// State.h: interface for the CState class.
//
//////////////////////////////////////////////////////////////////////

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_STATE_INCLUDED
#define _INC_STATE_INCLUDED
#include <vector>
#include <string>
#include <map>
using namespace std;


class CState
{
public:
	struct tagFettle
	{	
		string strFettleName;				//基本状态名称		
		DWORD dwCondition;					//条件ID		
		string strOpera;					//操作符
		DWORD dwValue;  					//基本状态的修正值
		string strFormat;					//计算公式
	}; 
	struct tagEvent
	{
		DWORD dwCondition;
		string strScripte;
	};

	struct tagState
	{
		DWORD dwLevel;						//等级
		DWORD dwTime;						//间隔时间
		DWORD dwCount;						//次数

		vector<tagFettle*> vtgBegingFettle;	//开始基本状态		
		string strBeginScript;				//脚本

		vector<tagFettle*> vtgUpdateFettle;	//更新基本状态		
		string strUpdateScript;				//更新脚本


	
		vector<tagFettle*> vtgEndFettle;	//结束的基本状态
		string strEndScript;				//结束脚本

		map<string,tagEvent*> mapEvent;		//事件脚本

	};

	struct tagGoodCondition
	{
		DWORD dwPlace;						//地点
		DWORD dwPos;						//位置
		DWORD dwType;						//类型
	};
	struct tagCondition
	{
		vector<tagGoodCondition> vtgGoodCondition;			//物品条件
		vector<DWORD>			  vtgSkillID;		    	//技能条件
	};
	
private:
	
	DWORD						m_dwID;						//状态ID	
	string						m_strName;					//状态名称
	DWORD						m_dwOnly;					//施放后技能状态是否唯一性	
	DWORD						m_dwDeathIsClear;			//可见状态是否在死亡时被清除
	map<DWORD,tagCondition>		m_mapCondition;				//条件
	vector<tagState*>			m_vtgState;					//状态
	
	



public:
	CState(void);
	CState(DWORD dwStateID);
	virtual ~CState(void);

	void 			AddState(tagState  *tgState);						//添加状态	
	void			SetCondition(DWORD dwConID,tagCondition tgCondition);//设置条件
	void			SetStateName(string strName);						//设置状态名称
	void			SetStateID(long lStateID);							//设置状态ID
	void			SetStateOnly(DWORD dwOnly);							//设置施放后技能状态是否唯一性
	void			SetStateDeathIsClear(DWORD dwDeathIsClear);			//设置可见状态是否在死亡时被清除
	void			SetStateLevel(long lLevel);							//状态等级
	bool			AddStateToByteArray(vector<BYTE>& vRet);			//压入状态
	tagState*		GetState(DWORD dwLevel);							//获取状态

	void			Clear();											//清空


};

#endif//_INC_STATE_INCLUDED