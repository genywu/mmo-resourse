#pragma once
#include <fstream>
#include <string>

using namespace std;

class CRegionList
{
public:
	CRegionList(void);
public:
	~CRegionList(void);

// 数据读写
public:
	const inline ULONG GetID()const							{return m_uResourceID;}
	const inline string& GetName()const						{return m_strName;}

public:
	// 读场景列表文件 "regionlist.ini"
	friend ifstream &operator>>(ifstream &rstream, CRegionList &regionlist);

protected:
	ULONG		m_uID;
	unsigned	m_uResourceID;
	unsigned	m_uExpScale;
	unsigned	m_uType;
	unsigned	m_uNoPK;
	unsigned	m_uContribution;
	string		m_strName;
	unsigned	m_uGS_ID;
	unsigned	m_uCountry;
	unsigned	m_uNotify;
};

// 读场景列表文件 "regionlist.ini"
ifstream &operator>>(ifstream &rstream, CRegionList &regionlist);
