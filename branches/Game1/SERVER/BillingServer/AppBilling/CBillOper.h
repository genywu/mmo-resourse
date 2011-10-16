#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "../../../dbaccess/MyAdoBase.h"

using namespace std;

enum eBillOperType
{
	eBILL_OT_GET_BALANCE=1,//获取余额
	eBILL_OT_ACCOUNT_LOCK,//帐户预冻
	eBILL_OT_ACCOUNT_UNLOCK,//帐户解冻
	eBILL_OT_ACCOUNT_UPDATE,//帐户更新
	eBILL_OT_ACCOUNT_LOCK_ROLLBACK,//帐户预冻回滚
	eBILL_OT_EXIT_THREAD
};

struct tagItemData 
{
	DWORD dwItemId;
	DWORD dwItemNum;
};

class CBillOper
{
protected:
	eBillOperType m_eOperType;

	_ConnectionPtr m_SqlConnection;
public:		
	CBillOper();
	CBillOper(eBillOperType eType);
	virtual ~CBillOper();	
public:
	eBillOperType GetOperType(){return m_eOperType;}
	VOID SetOperType(eBillOperType eType){m_eOperType=eType;}	

	VOID GetCurrTimeString(string &strTime);

	VOID SetSqlConnection(_ConnectionPtr& cn){m_SqlConnection=cn;}

	virtual BOOL DoOpertaion();
};