//=============================================================================
/**
 *  file: Expressions.h
 *
 *  Brief:实现了状态、技能中需要使用到的一些脚本表达式
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-8-31
 */
//=============================================================================

#ifndef __EXPRESSIONS_H__
#define __EXPRESSIONS_H__

#pragma once
class CScript;
class Script;

class CExpressions
{
public:
	CExpressions(void);
	virtual ~CExpressions(void);
private:
	//表达式
	string m_strFileName;
	string m_strContent;
public:
	void InitBaseData(string& strFileName,string& strContend)
	{
		m_strFileName = strFileName;
		m_strContent = strContend;
	}
	//执行结果
	int Execute(Script* pScript,CMoveShape* pDestShape);
};

#endif
