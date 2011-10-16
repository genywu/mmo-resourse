



#ifndef _MedalMgr_H_
#define _MedalMgr_H_
#pragma once

class CMedalMgr
{
public:
	CMedalMgr(void);
	~CMedalMgr(void);

	void Release();

	void DecodeFromDataBlock(DBReadSet& dbRead);

	void AddMedal(DWORD dwIndex)		{ m_vecMedal.push_back(dwIndex); }

private:
	vector<DWORD>	m_vecMedal;
};
#endif//_MedalMgr_H_