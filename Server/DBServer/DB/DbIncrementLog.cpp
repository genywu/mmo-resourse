#include "StdAfx.h"
#include ".\DbIncrementLog.h"
#include "..\..\server\dbserver\dbserver\game.h"
#include "..\..\server\dbserver\appworld\dbentitymanager.h"
#include "../../server/DBServer/AppWorld/DBAccessThread.h"
#include "../public/StrType.h"
#include "../../public/StrUnifyInput.h"

CDbIncrementLog::CDbIncrementLog(void)
{
}

CDbIncrementLog::~CDbIncrementLog(void)
{
}
bool CDbIncrementLog::LoadAll()
{
	_RecordsetPtr rs;
	if( !CreateRs(rs) ) return false;
	try
	{
		char sql[1024];
		
		// 检查此卡是否存在数据库中 [TEST]
		sprintf( sql,"SELECT * FROM increment_log WHERE DATEDIFF(day,log_time,GETDATE())<=%d ORDER BY log_time",
			10 );

		TESTHR( OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()) );

		SYSTEMTIME time;

		CMessage msg(MSG_DB2W_PSASS_OTHER_GETLOG_REQUEST);
		msg.Add((long)rs->GetRecordCount()); // 个数

		//[070716 AHC]
		BYTE byType = 0;
		DWORD dwMoney = 0;
		char szDesc[256];
		CGUID PlayerID;
		char szAcc[64];
		while( !rs->GetadoEOF() )
		{
			memset( &time, 0, sizeof(SYSTEMTIME) );
			VariantTimeToSystemTime( (double)rs->GetCollect("log_time"), &time );
			
			byType = rs->GetCollect("type");
			dwMoney = rs->GetCollect("money");
			strcpy(szDesc, (_bstr_t)rs->GetCollect("Name"));
			PlayerID = CGUID((_bstr_t)rs->GetCollect("player_id"));
			strcpy(szAcc, (_bstr_t)rs->GetCollect("player_acc"));

			msg.AddEx(&time, sizeof(SYSTEMTIME));
			msg.Add(byType);
			msg.Add(dwMoney);
			msg.Add(szDesc);
			msg.Add(PlayerID);
			msg.Add(szAcc);

			rs->MoveNext();
		}

		msg.SendToSocket(GetGame()->GetWSSocket());
		return true;
	}
	catch(_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr(CStringReading::LoadString(IDS_DBS_DBINCREMENTLOG,STR_DBS_GOODSINCLOG_LOADALLFAIL), e);
	}
	return false;
}
bool CDbIncrementLog::Load(const CGUID& guid, CMessage* msg, _ConnectionPtr& cn)
{
	char szGUID[128];
	guid.tostring(szGUID);

	_RecordsetPtr rs;
	if( !CreateRs(rs) ) return false;
	try
	{
		char sql[1024];

		TESTHR(CreateRs(rs));
		// 检查此卡是否存在数据库中 [TEST]
		sprintf( sql,"SELECT * FROM increment_log WHERE player_id='%s'", szGUID );

		TESTHR( OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr(), adOpenStatic) );
		SYSTEMTIME time;
		long logNum = 0;
		if(!rs->GetadoEOF())
		{
			logNum = rs->GetRecordCount();
		}
		msg->Add(guid); // 请求的玩家GUID
		msg->Add(logNum); // 个数

		//[070716 AHC]
		BYTE byType = 0;
		DWORD dwMoney = 0;
		char szDesc[256];
		CGUID PlayerID;
		char szAcc[64];
		while( !rs->GetadoEOF() )
		{
			memset( &time, 0, sizeof(SYSTEMTIME) );
			VariantTimeToSystemTime( (double)rs->GetCollect("log_time"), &time );

			byType = rs->GetCollect("type");
			dwMoney = rs->GetCollect("money");
			strcpy(szDesc, (_bstr_t)rs->GetCollect("description"));
			PlayerID = (CGUID)((_bstr_t)rs->GetCollect("player_id"));
			strcpy(szAcc, (_bstr_t)rs->GetCollect("player_acc"));

			msg->Add(byType);
			msg->Add(dwMoney);
			msg->Add(szDesc);
			msg->Add(szAcc);
			msg->AddEx(&time, sizeof(SYSTEMTIME));

			rs->MoveNext();
		}

		ReleaseRs(rs);
		return true;
	}
	catch(_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr(CStringReading::LoadString(IDS_DBS_DBINCREMENTLOG,STR_DBS_GOODSINCLOG_LOADFAIL), e);
	}
	return false;
}
bool CDbIncrementLog::Save(BYTE byType, DWORD dwMoney, 
		  const char* szDesc, const CGUID& PlayerID, 
		  const char* szAcc, SYSTEMTIME* logTime, _ConnectionPtr& cn)
{
	char szPlayerID[128];
	PlayerID.tostring(szPlayerID);
	
	_RecordsetPtr rs;
	long nTransactionCounter = 0;
	try
	{
		TESTHR(CreateRs(rs));
		// 启动事务
		//nTransactionCounter = cn->BeginTrans();

		// 插入csl_player_ability表
		TESTHR(OpenRs("increment_log", rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr(), adOpenForwardOnly, adLockOptimistic, adCmdTable));

		double pvtime;
		SystemTimeToVariantTime(logTime, &pvtime);
		TESTHR(rs->AddNew());
		rs->PutCollect("type", byType);
		rs->PutCollect("money", dwMoney);
		rs->PutCollect("description", szDesc);
		rs->PutCollect("player_id", szPlayerID);
		rs->PutCollect("player_acc", szAcc);
		rs->PutCollect("log_time", pvtime);
		TESTHR(rs->Update());
		// end add new
		ReleaseRs(rs);
	//	cn->CommitTrans();
	}
	catch(_com_error &e)
	{
		//ReleaseRs(rs);
		//PrintErr("", e);

		//if(nTransactionCounter)
		//{
		//	ReleaseRs(rs);
		//	// 异常发生，本次操作取消（回滚）
		//	PrintErr(CStringReading::LoadString(IDS_DBS_DBINCREMENTLOG,STR_DBS_GOODSINCLOG_SAVEROLLBACK), e);
		//	cn->RollbackTrans();
		//}
		//else
		//{
		//	PrintErr(CStringReading::LoadString(IDS_DBS_DBINCREMENTLOG, STR_DBS_GOODSINCLOG_SAVEFAIL));	
		//}
		PrintErr(CStringReading::LoadString(IDS_DBS_DBINCREMENTLOG,STR_DBS_GOODSINCLOG_SAVEERROR), e);	
		ReleaseRs(rs);

	}
	return false;
}