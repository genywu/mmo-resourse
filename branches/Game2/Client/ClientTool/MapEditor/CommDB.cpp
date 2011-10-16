#include "StdAfx.h"
#include ".\commdb.h"

IMPLEMENT_SINGLETON(CommDB);

CommDB::CommDB(void)
{
	m_pMap = NULL;
	m_pTerrainMesh = NULL;
}

CommDB::~CommDB(void)
{
}

