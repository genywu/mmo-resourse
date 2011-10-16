#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

using namespace std;

class CSuitRule : public BaseMemObj
{
public:
	struct tagRuleValue : public BaseMemObj
	{
		string strType;
		long  lVal;
	};
public:		
	CSuitRule();
	~CSuitRule();

	BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);
	BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);

	BOOL RuleActive(CPlayer* pPlayer,int suit_num);
	BOOL RuleInvalidate(CPlayer* pPlayer,int suit_num); 

	
public:
	map<int,string> m_mEquip;
	map<int ,vector<tagRuleValue*>*> m_mAttr;
	
};