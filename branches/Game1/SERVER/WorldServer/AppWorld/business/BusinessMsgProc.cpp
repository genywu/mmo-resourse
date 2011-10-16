///
/// @file BusinessMsgProc.cpp
/// @brief 处理商业系统相关消息
///
#include "StdAfx.h"
#include "../nets/networld/message.h"
#include "CBusinessManager.h"
#include "../DBEntity/EntityManager.h"

using namespace Business;

/// 处理存档消息
static void OnBusinessSaveMsg( CMessage *pMsg )
{
	DBReadSet db;
	pMsg->GetDBReadSet( db );
	// 缓存存档数据在WS
	GetInst( CBusinessManager ).DecodeFromGS( db );
	// 发起存档请求，此时某些数据可能并没更新
	GetGame()->GetEntityManager()->CreateSaveBusinessSession();

	BUSINESS_LOG_INFO( "Start save some business data" );
}

void OnBusinessMsg( CMessage *pMsg )
{
	switch( pMsg->GetType() )
	{
	case MSG_S2W_BUSINESS_SAVE:
		OnBusinessSaveMsg( pMsg );
		break;

	default:
		BUSINESS_LOG_WARNING( FMT_STR( "Unknonw message [%d].", pMsg->GetType() ) );
	}
}