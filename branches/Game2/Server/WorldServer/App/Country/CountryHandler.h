// 国家管理类
// 主要是管理国家对象
// 是Singleton类
// bugs
// 06/6/27

#pragma once
#ifndef _CountryHandler_h_
#define _CountryHandler_h_

class CCountry;

class CCountryHandler
{
public:
	CCountryHandler(void);
	virtual ~CCountryHandler(void);

	map<BYTE,CCountry*>					m_pCountrys;

private:
	static CCountryHandler				*instance;

public:
	struct tagCountryList 
	{
		long lID;
		char strName[MAX_PATH];
		char strDesc[MAX_PATH];
	};
private:
//	map<long,tagCountryList*> m_mpContryList;
	

public:
	static CCountryHandler				*GetInstance(void)
	{
		if(instance == NULL)
		{
			instance = new CCountryHandler;
		}
		return instance;
	}

	CCountry* GetCountry(BYTE id)
	{
		if(id <= 0) return NULL;
		if(m_pCountrys.find(id) != m_pCountrys.end())
		{
			return m_pCountrys[id];
		}
		return NULL;
	}

	bool			Initialize(void);
	bool			Load();
	void			Release(void);
	void			GenerateSaveData(void);
	bool			Append( CCountry* c );
	
	bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码
};


inline CCountryHandler* GetCountryHandler(void)
{
	return CCountryHandler::GetInstance();
}


#endif