//=============================================================================
/**
 *  file: EffectCirc.h
 *
 *  Brief:作用对象类，根据某个条件选出一个圆形的作用对象集合
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-8-31
 */
//=============================================================================

#ifndef __EFFECT_CIRC_H__
#define __EFFECT_CIRC_H__


#pragma once
#include "effectobjects.h"

class CEffCirc :
	public CEffectObjects
{
public:
	CEffCirc(stModuParam *pParam,CCondiGroup* pCondiGroup);
	struct tagTmpParam
	{
		stModuParam *pParam;
		CCondiGroup* pCondiGroup;
	};
	CEffCirc(void *pParam);
	virtual ~CEffCirc(void);

private: 
    //是否是范围内对象
    bool      m_IsWithinObj;
	//作用点类型
	ePosition m_eCenter;
	//适用距离
	float     m_nRadius;
	//作用对象个数
	int       m_nNum;
    //作用对象变量
    string    m_strNum;
   
public:
	void InitBaseData(string strPosition,float distance,int num,bool Within,string strNum)
	{
        m_IsWithinObj = Within;
        m_strNum = strNum;
		m_eCenter = GetPosition(strPosition);	
		m_nRadius = distance;
		m_nNum = num;
		
	}
	//得到所用的作用对象
	unsigned int GetEffectObjs(list<CMoveShape*>& listObjs);
};

#endif
