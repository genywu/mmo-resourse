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
	struct tagTmpParam
	{
		stModuParam *pParam;
		CCondiGroup* pCondiGroup;
	};
	CEffLine(stModuParam *pParam,CCondiGroup* pCondiGroup);
	CEffLine(void *pParam);
	virtual ~CEffLine(void);

private:

    //是否是范围内对象
    bool      m_IsWithinObj;
	//作用点类型
	ePosition m_ePos;
	//适用距离
	int m_nDistance;
    //数量
    int m_nNum;
    //作用对象变量
    string    m_strNum;
    
public:
    void InitBaseData(string strPosition,int distance,int num,bool Within,string strNum)
    {
        m_IsWithinObj = Within;
        m_strNum = strNum;
		m_ePos = GetPosition(strPosition);
		m_nDistance = distance;
        m_nNum  = num;
	}
	//得到所用的作用对象
	unsigned int GetEffectObjs(list<CMoveShape*>& listObjs);
};

#endif
