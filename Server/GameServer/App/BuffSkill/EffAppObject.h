//=============================================================================
/**
*  file: EffAppObject.h
*
*  Brief:作用对象类，根据某个条件选出一个队伍里的作用对象集合
*
*  Authtor:Andy_Chan
*	
*	Datae:2008-1-7
*/
//=============================================================================

#ifndef __EFFECT_APPOBJECT_H__
#define __EFFECT_APPOBJECT_H__


#pragma once
#include "effectobjects.h"

class CEffAppObject :
	public CEffectObjects
{
public:
	CEffAppObject(stModuParam *pParam,CCondiGroup* pCondiGroup);
	struct tagTmpParam
	{
		stModuParam *pParam;
		CCondiGroup* pCondiGroup;
	};
	CEffAppObject(void *pParam);
	virtual ~CEffAppObject(void);
	unsigned int GetEffectObjs(list<CMoveShape*>& listObjs);
};

#endif