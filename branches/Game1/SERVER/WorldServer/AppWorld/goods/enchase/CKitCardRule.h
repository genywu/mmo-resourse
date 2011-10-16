#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

/*
*套卡规则
*/
class CKitCardRule:public BaseMemObj
{
public:
	CKitCardRule();
	CKitCardRule(DWORD dwId,BYTE btHoleNum,BYTE btCardNum);
	~CKitCardRule();
public:
	struct tagRuleValue
	{
		string strType;
		long  lVal;
	};
private:
	//套卡规则ID
	DWORD m_dwRuleId;
	//要求开孔的数量
	BYTE m_btRequireHoleNum;
	//套卡决定数量
	BYTE m_btRequireCardNum;
	//套卡各ID
	LONG* m_pCardIdArray;
	//属性修正
public:
	vector<CKitCardRule::tagRuleValue> m_vAttr;
	
public:	
	BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);
	BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);	

	DWORD GetRuleId(){return m_dwRuleId;}
	BYTE GetRequireHoleNum(){return m_btRequireHoleNum;}
	BYTE GetRequireCardNum(){return m_btRequireCardNum;}
	VOID SetCardIds(int* pIds);
};