//=============================================================================
/**
 *  file: EffTeam.h
 *
 *  Brief:作用对象类，根据某个条件选出一个队伍里的作用对象集合
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-8-31
 */
//=============================================================================

#ifndef __EFFECT_TEAM_H__
#define __EFFECT_TEAM_H__


#pragma once
#include "effectobjects.h"

class CEffTeam :
	public CEffectObjects
{
public:
	CEffTeam(stModuParam *pParam,CCondiGroup* pCondiGroup);
	virtual ~CEffTeam(void);
private:
	//作用点类型
	ePosition m_eCenter;
	//适用距离
	int m_nDistance;
public:
	void InitBaseData(string strPosition,int distance)
	{
		m_eCenter = GetPosition(strPosition);
		m_nDistance = distance;
	}
	//得到所用的作用对象
	unsigned int GetEffectObjs(list<CMoveShape*>& listObjs);
};

#endif
