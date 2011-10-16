//=============================================================================
/**
 *  file: EffPoint.h
 *
 *  Brief:作用对象类，根据某个条件选出一个上的作用对象集合
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-8-31
 */
//=============================================================================

#ifndef __EFFECT_POINT_H__
#define __EFFECT_POINT_H__


#pragma once
#include "effectobjects.h"

class CEffPoint :
	public CEffectObjects
{
public:
	CEffPoint(stModuParam *pParam,CCondiGroup* pCondiGroup);

	struct tagTmpParam
	{
		stModuParam *pParam;
		CCondiGroup* pCondiGroup;
	};
	CEffPoint(void *pParam);
	virtual ~CEffPoint(void);

private:
	//作用点类型
	ePosition m_ePos;
    
public:
	//初始化
	void InitBaseData(string strName)
	{
		m_ePos = GetPosition(strName);
	}
	//得到所用的作用对象
	unsigned int GetEffectObjs(list<CMoveShape*>& listObjs);
};

#endif
