


#ifndef _PlayerCollect_H_
#define _PlayerCollect_H_
#pragma once


class CPlayerCollect
{
public:
	CPlayerCollect(void);

	~CPlayerCollect(void);

	void Release();

	void DecodeFromDataBlock(DBReadSet& dbRead);


	DWORD   GetCollectSkillLevel()			  {return m_dwCollectLevel;}			//采集等级
	void	SetCollectSkillLevel(DWORD dwLevel) {m_dwCollectLevel = dwLevel;}

	DWORD	GetCollectSkillExp()			{return m_dwCollectExp;}			//采集经验
	void	SetCollectSkillExp(DWORD dwExp)	{m_dwCollectExp = dwExp;}

	DWORD	GetFactureSkillExp()			{return m_dwFactureExp;}			//制作经验
	void	SetFactureSkillExp(DWORD dwExp) {m_dwFactureExp = dwExp;}			

	DWORD	GetFactureSkillLevel()			 {return m_dwFactureLevel;}			//制作等级
	void	SetFactureSkillLevel(DWORD dwLevel){m_dwFactureLevel = dwLevel;}


private:
	DWORD						m_dwCollectExp;					//采集经验
	DWORD						m_dwCollectLevel;					//采集等级
	DWORD						m_dwFactureExp;					//制作经验
	DWORD						m_dwFactureLevel;					//制作等级
	DWORD						m_dwToolIndex;						//当前使用的工具物编ID
	vector<DWORD>				m_vecFactureEntrys;				//学会的制作条目列表
};
#endif//_PlayerCollect_H_