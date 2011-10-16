#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

class CKitCardRule;

class CKitCardRuleFactory 
{
public:
	CKitCardRuleFactory();
	~CKitCardRuleFactory();	
public:
	struct tagCardPrice
	{
		DWORD dwEquipLev;
		DWORD dwOpenHolePrice;
		DWORD dwEnchasePrice;
	};
private:
	static map<int,CKitCardRule*> m_mRules;
	static vector<CKitCardRuleFactory::tagCardPrice> m_vecCardPrice;
public:	
	static BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);
	static BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);
	static BOOL Load(const CHAR* StrPath);
	static VOID Release();
	static DWORD GetOpenHolePrice(DWORD dwLev);
	static DWORD GetEnchasePrice(DWORD dwLev);
};