#pragma once
#include "..\myadobase.h"
class CWorldRegion;

class CDBRegion;

class CRsRegion :
	public CMyAdoBase
{
public:
	CRsRegion(void);
	~CRsRegion(void);
public:
	bool LoadRegionParam(CDBRegion* pRegion);
	bool Save(CDBRegion* pRegion,_ConnectionPtr& cn);
	bool LoadAll();
	bool SaveAll();
};