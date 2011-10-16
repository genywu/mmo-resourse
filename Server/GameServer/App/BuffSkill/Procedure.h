//=============================================================================
/**
 *  file: Procedure.h
 *
 *  Brief:包装了一个具有开始、事件、超时、结束步骤的处理过程
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-8-31
 */
//=============================================================================

#ifndef __PROCEDURE_H__
#define __PROCEDURE_H__
#pragma once

#include "BuffSkillDef.h"

class CStep;
class CEffectObjects;
class CMoveShape;
struct tagEvenRegVar;

class CProcedure
{
public:
	CProcedure(stModuParam *pParam);
	CProcedure(void* pParam);
	~CProcedure(void);

private:
	//作用目标
	stModuParam	*m_pParam;

	//开始执行步骤
	CStep	*m_pBegin;
	//事件执行步骤
	CStep *m_pEvent;
	//超时执行步骤
	CStep *m_pTimeOut;
	//结束执行步骤
	CStep *m_pEnd;
	///////////////////////////////////////////////////////////////////////////////////
	//私有接口
private:
	///////////////////////////////////////////////////////////////////////////////////
public:

	void InitBaseData(string strStepName,CStep *pStep);
	void SetParam(stModuParam *pParam)	{ m_pParam = pParam;}

	//开始
	eModuleRet Begin(stModuParam *pParam);
	//事件处理接口
	eModuleRet Proc(DWORD type,const tagProcVar *pVar,stModuParam *pParam);
	//超时处理接口
	eModuleRet OnTimeOut(DWORD timerid,DWORD curtime,const void* var,stModuParam *pParam);
	//结束调用接口
	eModuleRet End(stModuParam *pParam);	
};


#endif