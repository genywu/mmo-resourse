#ifndef THREAD_DATA_DEF_H
#define THREAD_DATA_DEF_H
#include "../public/GUID.h"
#include "../public/BaseDef.h"
#include "../public/DBDefine.h"

class CBaseDBEntity;
struct LoadObj;
struct SaveObj;

struct LoadThreadData 
{
	LoadObj*		loadObj;
	CBaseDBEntity*	dbObj;

	LoadThreadData();
	LoadThreadData(const CGUID& guid, DB_OBJ_TYPE type);
	~LoadThreadData();
	LoadThreadData& operator=(const LoadThreadData& right);
};
struct SaveThreadData 
{
	SaveObj*		saveObj;
	CBaseDBEntity*  dbObj;

	SaveThreadData();
	SaveThreadData(const CGUID& guid, DB_OBJ_TYPE type);
	~SaveThreadData();
	SaveThreadData& operator=(const SaveThreadData& right);
};
#endif//THREAD_DATA_DEF_H