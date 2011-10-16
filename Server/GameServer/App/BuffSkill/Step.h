//=============================================================================
/**
 *  file: Step.h
 *
 *  Brief:基本步骤类，一个抽象基本类，实现一个单独的条件判断和操作
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-8-31
 */
//=============================================================================

#ifndef __BASE__STEP_H__
#define __BASE__STEP_H__
#pragma once

#include "BuffSkillDef.h"

class CDo;
class CMoveShape;

class CStep
{
public:
	CStep(void);
	virtual ~CStep(void);
private:
	typedef vector<CDo*>::iterator itDo;
	vector<CDo*>	m_DoSome;

public:
	//初始化基本数据
	void InitBaseData(CDo *pDo);
	//重新设置数据
	void Reset(CMoveShape* pUser);
	//执行所有Do
	eModuleRet DoSome(stModuParam *pParam);
};

#endif
