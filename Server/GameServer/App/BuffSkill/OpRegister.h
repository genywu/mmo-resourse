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

#include "Operator.h"
#include "BuffSkillDef.h"

class CMoveShape;
class CScript;

class COpRegisterTimer :
	public COperator
{
public:
	COpRegisterTimer(stModuParam *pParam);
	COpRegisterTimer(void* pParam);
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
    //@by:expter 直接读取配置变量
    string m_strFutureParamname;
    string m_strInterParamname;
    string m_strCountParamname;    
public:
	//初始化
	void InitBaseData(eObj Obj,long oType,long flag,long lFuture,string strFname,
                      long lIntel,string strIntelName,DWORD count,
                      string strCountName,string futname,string intname,
                      string countname)
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
        m_strFutureParamname=futname;
        m_strInterParamname=intname;
        m_strCountParamname=countname; 
	}
	//执行
    void Execute(CMoveShape* pShape);

};

class COpRegisterEvent :
	public COperator
{
public:
	COpRegisterEvent(stModuParam *pParam);
	COpRegisterEvent(void* pParam);
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
	struct tagParam
	{
		stModuParam *pParam;
		long lFlag;
	};
	COpUnRegisterTimer(stModuParam *pParam,long lFlag);
	COpUnRegisterTimer(void *pParam);
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
	struct tagParam
	{
		stModuParam *pParam;
		long lFlag;
	};
	COpUnRegisterEvent(stModuParam *pParam,long lFlag);
	COpUnRegisterEvent(void* pParam);
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