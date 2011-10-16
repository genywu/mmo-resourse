//=============================================================================
/**
 *  file: OpSendMsg.h
 *
 *  Brief:包装了注册定时器和事件操作
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-9-5
 */
//=============================================================================

#ifndef __OPERATOR_REGISTER_H__
#define __OPERATOR_REGISTER_H__
#pragma once

#include "operator.h"
#include "BuffSkillDef.h"

class CMoveShape;
class CScript;

class COpRegisterTimer :
	public COperator
{
public:
	COpRegisterTimer(stModuParam *pParam);
	virtual ~COpRegisterTimer(void);
private:
	//子对象类型
	long m_eChildObjType;
	//定时器标记
	long m_lFlag;
	//预定触发时间
	long m_lFuture;
	string m_strFutVarname;
	//间隔时间
	long m_lInter;
	string m_strInterVarName;
	//调用次数
	DWORD m_nCount;
	string m_strCountVarName;
public:
	//初始化
	void InitBaseData(eObj Obj,long oType,long flag,long lFuture,string strFname,long lIntel,string strIntelName,DWORD count,string strCountName)
	{
		m_Obj = Obj;
		m_eChildObjType = oType;
		m_lFlag = flag;
		m_lFuture = lFuture;
		m_strFutVarname = strFname;
		m_lInter = lIntel;
		m_strInterVarName = strIntelName;
		m_nCount = count;
		m_strCountVarName = strCountName;
	}
	//执行
    void Execute(CMoveShape* pShape);

};

class COpRegisterEvent :
	public COperator
{
public:
	COpRegisterEvent(stModuParam *pParam);
	virtual ~COpRegisterEvent(void);
private:
	//子对象类型
	long m_eChildObjType;
	//事件标记
	long m_lFlag;
	//事件id
	long m_lEventType;
public:
	//初始化
	void InitDataBase(eObj Obj,long type, long flag,long eventtype)
	{
		m_Obj = Obj;
		m_eChildObjType = type;
		m_lFlag = flag;
		m_lEventType = eventtype;
	}
	//执行
    void Execute(CMoveShape* pShape);

};

class COpUnRegisterTimer :
	public COperator
{
public:
	COpUnRegisterTimer(stModuParam *pParam,long lFlag);
	virtual ~COpUnRegisterTimer(void);
private:
	//定时器标记
	//flag ==0,表示注销所有定时器
	long m_lFlag;
public:
	//初始化
	void InitDataBase(eObj Obj)
	{
		m_Obj = Obj;
	}
	//执行
    void Execute(CMoveShape* pShape);

};

class COpUnRegisterEvent :
	public COperator
{
public:
	COpUnRegisterEvent(stModuParam *pParam,long lFlag);
	virtual ~COpUnRegisterEvent(void);
private:
	//事件标记
	//flag ==0,表示注销所有事件
	long m_lFlag;
public:
	//初始化
	void InitDataBase(eObj Obj)
	{
		m_Obj = Obj;
	}
	//执行
    void Execute(CMoveShape* pShape);

};

#endif