//=============================================================================
/**
 *  file: EffectObjects.h
 *
 *  Brief:作用对象类，根据某个条件选出某个范围的作用对象集合
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-8-31
 */
//=============================================================================

#ifndef __EFFECT_OBJECTS_H__
#define __EFFECT_OBJECTS_H__

#pragma once

class CCondiGroup;
class CMoveShape;
class CScript;
struct stModuParam;

class CEffectObjects : public BaseMemObj
{
public:
	CEffectObjects(stModuParam *pParam,CCondiGroup* pCondiGroup);
	virtual ~CEffectObjects(void);

	enum ePosition
	{
		ePos_Mouse=0,
		ePos_Self,
		ePos_Dest,
		ePos_DestCoord,//目标坐标
		ePos_Coord,    //坐标
		ePos_Firer,	   //施放者
		ePos_Attacker, //攻击者
		ePos_Host,	   //宠物主人
		ePos_Target,   //选择目标
		ePos_AttackTarget,//攻击目标
	};
protected:
	//条件组,判断作用对象的条件
	CCondiGroup* m_pCondiGroup;
	//技能参数
	stModuParam *m_pParam;


public:
	//
	ePosition GetPosition(string strname)
	{
		if (strname=="ePos_Mouse")
		{
			return ePos_Mouse;
		} 
		else if(strname=="ePos_Self")
		{
			return ePos_Self;
		}
		else if (strname=="ePos_Dest")
		{
			return ePos_Dest;
		}
		else if (strname=="ePos_DestCoord")
		{
			return ePos_DestCoord;
		}
		else if (strname=="ePos_Coord")
		{
			return ePos_Coord;
		}
		else if (strname=="ePos_Firer")
		{
			return ePos_Firer;
		}
		else if (strname=="ePos_Attacker")
		{
			return ePos_Attacker;
		}		
		else if (strname=="ePos_Host")
		{
			return ePos_Host;
		}	
		else if (strname=="ePos_Target")
		{
			return ePos_Target;
		}
		else if (strname=="ePos_AttackTarget")
		{
			return ePos_AttackTarget;
		}
		
		return ePos_Mouse;
	}
	//得到所用的作用对象
	virtual unsigned int GetEffectObjs(list<CMoveShape*>& listObjs) = 0;
	void SetParam(stModuParam *pParam)	{m_pParam = pParam;}
};

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

class CEffDestObjs :
	public CEffectObjects
{
public:
	CEffDestObjs(stModuParam *pParam,CCondiGroup* pCondiGroup);
	virtual ~CEffDestObjs(void);

public:
	//得到所用的作用对象
	unsigned int GetEffectObjs(list<CMoveShape*>& listObjs);
};

class CEffTimerObjs :
	public CEffectObjects
{
public:
	CEffTimerObjs(stModuParam *pParam,CCondiGroup* pCondiGroup);
	virtual ~CEffTimerObjs(void);

public:
	//得到所用的作用对象
	unsigned int GetEffectObjs(list<CMoveShape*>& listObjs);
};

#endif
