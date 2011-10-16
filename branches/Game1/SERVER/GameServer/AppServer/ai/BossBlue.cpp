#include "StdAfx.h"
#include ".\BossBlue.h"
#include "..\Player.h"
#include "..\Monster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 基本功能开发完成
// BOSS在IDLE时，需要记录时间，在WAKEUP时通过该时间进行回血功能，尚未完成
// 2006年4月26日 Bugs 版本签入注释 


CBossBlue::CBossBlue(void)
{
	// 初始成false
	memset(_isUsed,0,sizeof(bool)*8);
}

CBossBlue::~CBossBlue(void)
{
	memset(_isUsed,0,sizeof(bool)*8);
}
