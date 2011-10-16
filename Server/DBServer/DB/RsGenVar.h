#pragma once
#include "..\myadobase.h"
#include <map>
#include "../public/DBDefine.h"

class CDBGenVarGroup;

class CRsGenVar :
	public CMyAdoBase
{
public:
	CRsGenVar(void);
	~CRsGenVar(void);

public:
	DB_OPERATION_RESULT_TYPE Add(CDBGenVarGroup* pGenVarGroup, _ConnectionPtr& cn);
	DB_OPERATION_RESULT_TYPE Load(CDBGenVarGroup* pGenVarGroup, _ConnectionPtr& cn);
	DB_OPERATION_RESULT_TYPE Save(CDBGenVarGroup* pGenVarGroup, _ConnectionPtr& cn);
	DB_OPERATION_RESULT_TYPE Del(CDBGenVarGroup* pGenVarGroup, _ConnectionPtr& cn);
};
