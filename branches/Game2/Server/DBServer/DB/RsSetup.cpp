#include "StdAfx.h"
#include ".\rssetup.h"
#include "..\..\server\dbserver\dbserver\game.h"
#include "..\..\server\dbserver\AppWorld\DBSetup.h"
#include "..\..\server\dbserver\AppWorld\DBentityManager.h"
#include "../../server/DBServer/AppWorld/DBAccessThread.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRsSetup::CRsSetup(void)
{
	//	LoadPlayerID();
	//LoadOrganizingID();
	//	LoadLeaveWorldID();
}

CRsSetup::~CRsSetup(void)
{
}

bool CRsSetup::SavePlayerID(_ConnectionPtr& cn)
{
	// 存盘线程传过来的连接不存在

	bool IsLocalConn = false;
	/*if(cn == NULL) 
	{
		TESTHR(CreateCn(cn));
		TESTHR(OpenCn(cn));
		//	PrintErr("存盘线程传过来的连接不存在");
		//	return false;
		IsLocalConn = true;
	}*/

	if(GetGame()->GetDBEntityManager()->GetDBSetupMap().size())
	{
		CGUID id = (GetGame()->GetDBEntityManager()->GetDBSetupMap().begin())->second->GetPlayerID();
		char sql[512];
		char szid[128];
		id.tostring(szid);
		sprintf(sql, "UPDATE csl_setup SET playerID='%s'", szid);
		if( !ExecuteCn(sql, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()) )
		{
//			if(IsLocalConn)
				//ReleaseCn(cn);
			PrintErr("save playerid error");
			return false;
		}
	}
	else
	{
		CDBSetup* s = new CDBSetup;
		GetGame()->GetDBEntityManager()->AddOneDBSetup(s);
		char sql[512];
		sprintf(sql, "UPDATE csl_setup SET playerID=%d", s->GetPlayerID());
		if( !ExecuteCn(sql, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()) )
		{
//			if(IsLocalConn)
				//ReleaseCn(cn);
			PrintErr("save playerid error");
			return false;
		}
	}

//	if(IsLocalConn)
		//ReleaseCn(cn);
	return true;
}

void CRsSetup::LoadPlayerID()
{
	char sql[512];
	_ConnectionPtr cn;
	_RecordsetPtr rs;

	CDBEntityManager::DBSetupMapItr itr = GetGame()->GetDBEntityManager()->GetDBSetupMap().begin();
	if(itr != GetGame()->GetDBEntityManager()->GetDBSetupMap().end())
	{
		try
		{
		//	TESTHR(CreateCn(cn));
			TESTHR(CreateRs(rs));

	//		TESTHR(OpenCn(cn));
			sprintf(sql, "SELECT TOP 1 playerID FROM csl_setup");
			TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()/*m_Connection*/));


			if(rs->GetadoEOF())
			{
				itr->second->SetPlayerID(NULL_GUID);
			}
			else
			{
				itr->second->SetPlayerID( CGUID(((char*)(_bstr_t)rs->GetCollect("playerID"))) );
			}
			ReleaseRs(rs);
			//ReleaseCn(cn);
		}
		catch(_com_error &e)
		{
			ReleaseRs(rs);
			//ReleaseCn(cn);
			PrintErr("load playerid error", e);
			itr->second->SetPlayerID(NULL_GUID);
		}
	}
	else
	{
		CGUID guid;
		CGUID::CreateGUID(guid);
		CDBSetup* s = new CDBSetup(guid);
		GetGame()->GetDBEntityManager()->AddOneDBSetup(s);

		try
		{
		//	TESTHR(CreateCn(cn));
			TESTHR(CreateRs(rs));

		//	TESTHR(OpenCn(cn));
			sprintf(sql, "SELECT TOP 1 playerID FROM csl_setup");
			TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()/*m_Connection*/));


			if(rs->GetadoEOF())
			{
				s->SetPlayerID(NULL_GUID);
			}
			else
			{
				s->SetPlayerID( CGUID(((char*)(_bstr_t)rs->GetCollect("playerID"))) );
			}
			ReleaseRs(rs);
			//ReleaseCn(cn);
		}
		catch(_com_error &e)
		{
			ReleaseRs(rs);
			//ReleaseCn(cn);
			PrintErr("load playerid error", e);
			s->SetPlayerID(NULL_GUID);
		}
	}
}

//bool CRsSetup::SaveGoodsID(_ConnectionPtr& cn)
//{
//	// 存盘线程传过来的连接不存在
//	if(cn == NULL) 
//	{
//		PrintErr("存盘线程传过来的连接不存在");
//		return false;
//	}
//
//	char sql[1024];
//
//	sprintf(sql, "UPDATE csl_setup SET goodsID00=%d,goodsID01=%d,goodsID02=%d,goodsID03=%d,goodsID04=%d, \
//				 goodsID05=%d,goodsID06=%d,goodsID07=%d,goodsID08=%d,goodsID09=%d,goodsID10=%d, \
//				 goodsID11=%d,goodsID12=%d,goodsID13=%d,goodsID14=%d,goodsID15=%d", 
//				 GetGame()->GetGoodsID(0) - CGame::GOODSID_ZONE_SIZE*0,
//				 GetGame()->GetGoodsID(1) - CGame::GOODSID_ZONE_SIZE*1,
//				 GetGame()->GetGoodsID(2) - CGame::GOODSID_ZONE_SIZE*2,
//				 GetGame()->GetGoodsID(3) - CGame::GOODSID_ZONE_SIZE*3,
//				 GetGame()->GetGoodsID(4) - CGame::GOODSID_ZONE_SIZE*4,
//				 GetGame()->GetGoodsID(5) - CGame::GOODSID_ZONE_SIZE*5,
//				 GetGame()->GetGoodsID(6) - CGame::GOODSID_ZONE_SIZE*6,
//				 GetGame()->GetGoodsID(7) - CGame::GOODSID_ZONE_SIZE*7,
//				 GetGame()->GetGoodsID(8) - CGame::GOODSID_ZONE_SIZE*8,
//				 GetGame()->GetGoodsID(9) - CGame::GOODSID_ZONE_SIZE*9,
//				 GetGame()->GetGoodsID(10) - CGame::GOODSID_ZONE_SIZE*10,
//				 GetGame()->GetGoodsID(11) - CGame::GOODSID_ZONE_SIZE*11,
//				 GetGame()->GetGoodsID(12) - CGame::GOODSID_ZONE_SIZE*12,
//				 GetGame()->GetGoodsID(13) - CGame::GOODSID_ZONE_SIZE*13,
//				 GetGame()->GetGoodsID(14) - CGame::GOODSID_ZONE_SIZE*14,
//				 GetGame()->GetGoodsID(15) - CGame::GOODSID_ZONE_SIZE*15);
//
//	if(!ExecuteCn(sql, cn))
//	{
//		PrintErr("save goodsid error");	
//		return false;
//	}
//	return true;
//}

//void CRsSetup::LoadGoodsID()
//{	
//	char sql[512];
//	_ConnectionPtr& cn;
//	_RecordsetPtr rs;
//	try
//	{	
//		TESTHR(CreateCn(cn));
//		TESTHR(CreateRs(rs));
//
//		TESTHR(OpenCn(cn));
//		sprintf(sql, "SELECT TOP 1 * FROM csl_setup");
//		TESTHR(OpenRs(sql, rs, cn/*m_Connection*/));
//
//		GetGame()->SetGoodsID(0,(DWORD)rs->GetCollect("GoodsID00") + CGame::GOODSID_ZONE_SIZE*0);
//		GetGame()->SetGoodsID(1,(DWORD)rs->GetCollect("GoodsID01") + CGame::GOODSID_ZONE_SIZE*1);
//		GetGame()->SetGoodsID(2,(DWORD)rs->GetCollect("GoodsID02") + CGame::GOODSID_ZONE_SIZE*2);
//		GetGame()->SetGoodsID(3,(DWORD)rs->GetCollect("GoodsID03") + CGame::GOODSID_ZONE_SIZE*3);
//		GetGame()->SetGoodsID(4,(DWORD)rs->GetCollect("GoodsID04") + CGame::GOODSID_ZONE_SIZE*4);
//		GetGame()->SetGoodsID(5,(DWORD)rs->GetCollect("GoodsID05") + CGame::GOODSID_ZONE_SIZE*5);
//		GetGame()->SetGoodsID(6,(DWORD)rs->GetCollect("GoodsID06") + CGame::GOODSID_ZONE_SIZE*6);
//		GetGame()->SetGoodsID(7,(DWORD)rs->GetCollect("GoodsID07") + CGame::GOODSID_ZONE_SIZE*7);
//		GetGame()->SetGoodsID(8,(DWORD)rs->GetCollect("GoodsID08") + CGame::GOODSID_ZONE_SIZE*8);
//		GetGame()->SetGoodsID(9,(DWORD)rs->GetCollect("GoodsID09") + CGame::GOODSID_ZONE_SIZE*9);
//		GetGame()->SetGoodsID(10,(DWORD)rs->GetCollect("GoodsID10") + CGame::GOODSID_ZONE_SIZE*10);
//		GetGame()->SetGoodsID(11,(DWORD)rs->GetCollect("GoodsID11") + CGame::GOODSID_ZONE_SIZE*11);
//		GetGame()->SetGoodsID(12,(DWORD)rs->GetCollect("GoodsID12") + CGame::GOODSID_ZONE_SIZE*12);
//		GetGame()->SetGoodsID(13,(DWORD)rs->GetCollect("GoodsID13") + CGame::GOODSID_ZONE_SIZE*13);
//		GetGame()->SetGoodsID(14,(DWORD)rs->GetCollect("GoodsID14") + CGame::GOODSID_ZONE_SIZE*14);
//		GetGame()->SetGoodsID(15,(DWORD)rs->GetCollect("GoodsID15") + CGame::GOODSID_ZONE_SIZE*15);	
//		ReleaseRs(rs);
//		//ReleaseCn(cn);
//	}
//	catch (_com_error &e) 
//	{	
//		ReleaseRs(rs);
//		//ReleaseCn(cn);
//		PrintErr("load goodsid error", e);		
//	}
//}

//void CRsSetup::LoadOrganizingID()
//{
//	char sql[100];
//	_ConnectionPtr& cn;
//	_RecordsetPtr rs;
//	try
//	{
//		TESTHR(CreateCn(cn));
//		TESTHR(CreateRs(rs));
//
//		TESTHR(OpenCn(cn));
//		sprintf(sql, "SELECT TOP 1 OrganizingID FROM csl_setup");
//		TESTHR(OpenRs(sql, rs, cn/*m_Connection*/));
//
//		if(rs->GetadoEOF())
//		{
//			GetGame()->SetDBOrganizingID(0);
//		}
//		else
//		{
//			GetGame()->SetDBOrganizingID( (long)(rs->GetCollect("OrganizingID")) );
//		}
//		ReleaseRs(rs);
//		//ReleaseCn(cn);
//	}
//	catch(_com_error &e)
//	{
//		ReleaseRs(rs);
//		//ReleaseCn(cn);
//		PrintErr("load OrganizingID error", e);
//		GetGame()->SetDBOrganizingID(0);
//	}
//}
//bool CRsSetup::SaveOrganizingID(_ConnectionPtr& cn)
//{
//	// 存盘线程传过来的连接不存在
//	if(cn == NULL) 
//	{
//		PrintErr("存盘线程传过来的连接不存在");
//		return false;
//	}
//
//	char sql[512];
//	sprintf(sql, "UPDATE csl_setup SET OrganizingID=%d", GetGame()->GetDBData()->nOrganizingID);
//	if( !ExecuteCn(sql, cn) )
//	{
//		PrintErr("save OrganizingID error");
//		return false;
//	}
//	return true;
//}

void CRsSetup::LoadLeaveWorldID()
{
	char sql[100];
	_ConnectionPtr cn;
	_RecordsetPtr rs;
	CDBEntityManager::DBSetupMapItr itr = GetGame()->GetDBEntityManager()->GetDBSetupMap().begin();
	if(itr != GetGame()->GetDBEntityManager()->GetDBSetupMap().end())
	{
		try
		{
	//		TESTHR(CreateCn(cn));
			TESTHR(CreateRs(rs));

	//		TESTHR(OpenCn(cn));
			sprintf(sql, "SELECT TOP 1 LeaveWordID FROM csl_setup");
			TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()/*m_Connection*/));

			if(rs->GetadoEOF())
			{
				itr->second->SetLeaveWordID(0);
			}
			else
			{
				itr->second->SetLeaveWordID( (long)(rs->GetCollect("LeaveWordID")) );
			}
			ReleaseRs(rs);
			//ReleaseCn(cn);
		}
		catch(_com_error &e)
		{
			ReleaseRs(rs);
			//ReleaseCn(cn);
			PrintErr("load playerid error", e);
			itr->second->SetLeaveWordID(0);
		}
	}
	else
	{
		CGUID guid;
		CGUID::CreateGUID(guid);
		CDBSetup* s = new CDBSetup(guid);
		GetGame()->GetDBEntityManager()->AddOneDBSetup(s);
		try
		{
	//		TESTHR(CreateCn(cn));
			TESTHR(CreateRs(rs));

	//		TESTHR(OpenCn(cn));
			sprintf(sql, "SELECT TOP 1 LeaveWordID FROM csl_setup");
			TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()/*m_Connection*/));

			if(rs->GetadoEOF())
			{
				s->SetLeaveWordID(0);
			}
			else
			{
				s->SetLeaveWordID( (long)(rs->GetCollect("LeaveWordID")) );
			}
			ReleaseRs(rs);
			//ReleaseCn(cn);
		}
		catch(_com_error &e)
		{
			ReleaseRs(rs);
			//ReleaseCn(cn);
			PrintErr("load playerid error", e);
			s->SetLeaveWordID(0);
		}
	}
}


bool CRsSetup::SaveLeaveWorldID(_ConnectionPtr& cn)
{
	bool IsLocalConn = false;
	// 存盘线程传过来的连接不存在
/*	if(cn == NULL) 
	{
		TESTHR(CreateCn(cn));
		TESTHR(OpenCn(cn));
		//	PrintErr("存盘线程传过来的连接不存在");
		//	return false;
		IsLocalConn = true;
	}*/

	CDBEntityManager::DBSetupMapItr itr = GetGame()->GetDBEntityManager()->GetDBSetupMap().begin();
	if(itr != GetGame()->GetDBEntityManager()->GetDBSetupMap().end())
	{
		char sql[512];
		sprintf(sql, "UPDATE csl_setup SET LeaveWordID=%d", itr->second->GetLeaveWordID());
		if( !ExecuteCn(sql, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()) )
		{
	//		if(IsLocalConn)
				//ReleaseCn(cn);
			PrintErr("save LeaveWordID error");
			return false;
		}
	}
	else
	{
		CGUID guid;
		CGUID::CreateGUID(guid);
		CDBSetup* s = new CDBSetup(guid);
		GetGame()->GetDBEntityManager()->AddOneDBSetup(s);
		char sql[512];
		sprintf(sql, "UPDATE csl_setup SET LeaveWordID=%d", s->GetLeaveWordID());
		if( !ExecuteCn(sql, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()) )
		{
	//		if(IsLocalConn)
				//ReleaseCn(cn);
			PrintErr("save LeaveWordID error");
			return false;
		}
	}
	//if(IsLocalConn)
		//ReleaseCn(cn);
	return true;
}