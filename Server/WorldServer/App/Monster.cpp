#include "StdAfx.h"
#include "monster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMonster::CMonster(void)
{
	SetType(TYPE_MONSTER);
}

CMonster::~CMonster(void)
{
}
