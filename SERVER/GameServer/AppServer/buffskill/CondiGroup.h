//=============================================================================
/**
 *  file: CondiGroup.h
 *
 *  Brief:包装了多个基本的条件
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-8-31
 */
//=============================================================================

#ifndef __CONDITION_GROUP_H__
#define __CONDITION_GROUP_H__

#pragma once

class CCondition;
class CMoveShape;
struct stModuParam;

class CCondiGroup : public BaseMemObj
{
public:
	CCondiGroup(void);
	virtual ~CCondiGroup(void);

	//组合模式
	enum eGrouMode
	{
		Mode_And=1,//"&&"模式
		Mode_Or,//"||"模式
	};
private:
	typedef vector<CCondition*>::iterator itCond;
	vector<CCondition*>	m_Conditions;

	eGrouMode	m_eMode;

private:
	//用&&组合所有条件判断结果
	bool ResultByAnd(CMoveShape* pDest,stModuParam *pParam);
	//用||组合所有条件判断结果
	bool ResultByOr(CMoveShape* pDest,stModuParam *pParam);
public:
	//
	void InitBaseData(vector<CCondition*> &vConditon,eGrouMode mode)
	{
		m_Conditions = vConditon;	
		m_eMode = mode;
	}
	//重新设置数据
	void Reset(CMoveShape* pUser);
	//得到结果
	bool Result(CMoveShape* pDest,stModuParam *pParam);
};

#endif
