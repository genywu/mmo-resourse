//=============================================================================
/**
 *  file: EffLine.h
 *
 *  Brief:作用对象类，根据某个条件选出一个直线上的作用对象集合
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-8-31
 */
//=============================================================================

#ifndef __EFFECT_LINE_H__
#define __EFFECT_LINE_H__


#pragma once
#include "effectobjects.h"

class CEffLine :
	public CEffectObjects
{
public:
	CEffLine(stModuParam *pParam,CCondiGroup* pCondiGroup);
	virtual ~CEffLine(void);
private:
	//作用点类型
	ePosition m_ePos;
	//适用距离
	int m_nDistance;
public:
	void InitBaseData(string strPosition,int distance)
	{
		m_ePos = GetPosition(strPosition);
		m_nDistance = distance;
	}
	//得到所用的作用对象
	unsigned int GetEffectObjs(list<CMoveShape*>& listObjs);
};

#endif
