#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "CBillOper.h"

#include "../../../public/GUID.h"



class CAccountLockOper:public CBillOper
{
private:

	string m_strCDKey;
	CGUID m_UserGuid;

	CGUID m_AttachGuid;

	BYTE m_btConsumeType;

	LONG m_lLockNum;
	//DWORD m_dwGoodsId;
	//DWORD m_dwGoodsNum;

	LONG m_lGsId;
	DWORD m_dwTradeMode;

	vector<tagItemData> m_vecItemData;

public:			
	CAccountLockOper(eBillOperType eType);

	virtual ~CAccountLockOper();	

	const char* GetCDKey(){return m_strCDKey.c_str();}
	VOID SetCDKey(char* cdkey){m_strCDKey=cdkey;};
	VOID SetTradeMode(DWORD dwTradeMode){m_dwTradeMode = dwTradeMode;};

	VOID SetConsumeType(BYTE btType){m_btConsumeType=btType;}

	VOID SetAttachGuid(CGUID& attachGuid){m_AttachGuid=attachGuid;}

	VOID SetUserGuid(CGUID& userid){m_UserGuid=userid;}
	CGUID& GetUserGuid(){return m_UserGuid;}

	VOID SetGSID(LONG lId){m_lGsId=lId;}

	//VOID SetGoodsId(DWORD dwId){m_dwGoodsId=dwId;}
	//DWORD GetGoodsId(){return m_dwGoodsId;}

	VOID SetLockNum(LONG lNum);
	LONG GetLockNum(){return m_lLockNum;}

	VOID AddItemData(tagItemData* pData){m_vecItemData.push_back(*pData);}

	//VOID SetGoodsNum(DWORD dwNum){m_dwGoodsNum=dwNum;}
	//DWORD GetGoodsNum(){return m_dwGoodsNum;}



public:	
	virtual BOOL DoOpertaion();

	VOID WriteLog(const CGUID& operGuid);
};