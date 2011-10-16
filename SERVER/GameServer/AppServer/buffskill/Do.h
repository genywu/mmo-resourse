//=============================================================================
/**
 *  file: Buff.h
 *
 *  Brief:具体的实施类，封装了一个条件集合和一些基本操作的
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-8-31
 */
//=============================================================================

#ifndef __DO_H__
#define __DO_H__
#pragma once

#include "BuffSkillDef.h"

class CCondiGroup;
class CEffectObjects;
class COperator;
class CMoveShape;

class CDo : public BaseMemObj
{
public:
	CDo(stModuParam *pParam);
	virtual ~CDo(void);

	struct stRet
	{
		eModuleRet m_etrueRet;
		eModuleRet m_efalseRet;
	};

	//操作集合
	typedef vector<COperator*>::iterator itOper;
	struct tagGroup : public BaseMemObj
	{
		//条件组
		CCondiGroup *pCondiGroup;
		//条件正确时执行的操作
		vector<COperator*> pTrueOperators;
		//条件错误时候的执行操作
		vector<COperator*> pFalseOperators;
		//执行真条件组
		tagGroup *pTrueGroup;
		//执行假条件组
		tagGroup *pFalseGroup;

		//返回结果
		stRet Ret;
		void Init(eModuleRet tret,eModuleRet fret)
		{
			Ret.m_etrueRet = tret;
			Ret.m_efalseRet = fret;
		}
	};
private:
	//作用对象
	CEffectObjects *m_pEffObjects;
	//所有组集合
	typedef vector<tagGroup*>::iterator itGroup;
	vector<tagGroup*> m_Groups;
	eModuleRet m_Ret;
	///////////////////////////////////////////////////////////////////////////////
	//私有接口
private:
	void DoOperator(list<CMoveShape*>& listObjs, vector<COperator*>& Operators, stModuParam *pParam);
	void DoOperator(CMoveShape* pObj, vector<COperator*>& Operators, stModuParam *pParam);
	void SendSKillFailMsg(int nValue, stModuParam *pParam);
	///////////////////////////////////////////////////////////////////////////////
	//公共操作接口
public:
	//初始化
	void InitBaseData(CEffectObjects *pEffObjects,vector<tagGroup*> &vtgGroup,eModuleRet eRet)
	{
		m_pEffObjects = pEffObjects;
		m_Groups = vtgGroup;
		m_Ret = eRet;
	}
	//执行
	eModuleRet Execute(stModuParam *pParam);
	//操作
	void  DoTrueOrFalseOperator(eModuleRet &eRet,tagGroup *ptgGroup,CMoveShape *pMoveShape,stModuParam *pParam);
	//释放
	void  ClearGroup(tagGroup *group);
};

#endif
