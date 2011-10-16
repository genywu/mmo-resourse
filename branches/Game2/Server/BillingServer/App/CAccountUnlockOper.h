#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "CBillOper.h"

class CAccountUnlockOper:public CBillOper
{
private:

	string m_strCDKey;
	string m_strUserName;
	CGUID m_UserGuid;

	CGUID m_OperGuid;

	LONG m_lUnlockNum;

	DWORD m_dwWorldId;
	DWORD m_dwGameId;
	string m_strGoodsName;

	
	//DWORD m_dwGoodsId;
	//DWORD m_dwGoodsNum;

	LONG m_lGsId;

	vector<tagItemData> m_vecItemData;

public:			
	CAccountUnlockOper(eBillOperType eType);

	virtual ~CAccountUnlockOper();

	VOID SetWorldId(DWORD dwId){m_dwWorldId=dwId;}
	VOID SetGameId(DWORD dwId){m_dwGameId=dwId;}
	VOID SetGoodsName(const char* name){m_strGoodsName=name;}

	const char* GetCDKey(){return m_strCDKey.c_str();}
	VOID SetCDKey(char* cdkey){m_strCDKey=cdkey;}

	const char* GetUserName(){return m_strUserName.c_str();}
	VOID SetUserName(char* name){m_strUserName=name;}

	VOID SetUserGuid(const CGUID& userid){m_UserGuid=userid;}
	CGUID& GetUserGuid(){return m_UserGuid;}

	VOID SetOperGuid(const CGUID& operGuid){m_OperGuid=operGuid;}
	CGUID& GetOperGuid(){return m_OperGuid;}

	VOID SetGSID(LONG lId){m_lGsId=lId;}

	//VOID SetGoodsId(DWORD dwId){m_dwGoodsId=dwId;}
	//DWORD GetGoodsId(){return m_dwGoodsId;}

	VOID SetUnlockNum(LONG lNum);
	LONG GetUnlockNum(){return m_lUnlockNum;}

	VOID AddItemData(tagItemData* pData){m_vecItemData.push_back(*pData);}

	//VOID SetGoodsNum(DWORD dwNum){m_dwGoodsNum=dwNum;}
	//DWORD GetGoodsNum(){return m_dwGoodsNum;}



public:	
	virtual BOOL DoOpertaion();

	VOID WriteLog();
};