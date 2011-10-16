#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

using namespace std;

class CSuitRule:public BaseMemObj
{
public:
	struct tagRuleValue
	{
		string strType;
		long  lVal;
	};
public:		
	CSuitRule();
	~CSuitRule();

	BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);
	BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);
	
public:
	map<int,string> m_mEquip;
	map<int ,vector<tagRuleValue*>*> m_mAttr;
	
};