#include "StdAfx.h"
#include ".\rsregion.h"

#include "..\..\server\dbserver\appworld\DBRegion.h"
#include "..\..\server\dbserver\appworld\DBEntityManager.h"
#include "..\..\server\DBServer/dbserver/game.h"
#include "../../server/DBServer/AppWorld/DBAccessThread.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRsRegion::CRsRegion(void)
{
}

CRsRegion::~CRsRegion(void)
{
}
bool CRsRegion::LoadAll()
{
	//_ConnectionPtr& cn;
	_RecordsetPtr rs;
	char sql[500] = "";
	try
	{
	//	TESTHR(CreateCn(cn));
		TESTHR(CreateRs(rs));

	//	TESTHR(OpenCn(cn));
		sprintf(sql,"SELECT * FROM CSL_Region");
		TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()/*m_Connection*/));

		while(!rs->GetadoEOF())
		{
			
			long lRgionID = rs->GetCollect("RegionID");
			CGUID lOwnedFactionID = CGUID(((char*)(_bstr_t)rs->GetCollect("OwnedFactionID")));
			CGUID lOwnedUnionID = CGUID(((char*)(_bstr_t)rs->GetCollect("OwnedUnionID")));
			long lCurTaxRate = rs->GetCollect("CurTaxRate");
			long lTodayTotalTax = rs->GetCollect("TodayTotalTax");
			long lTotalTax = rs->GetCollect("TotalTax");
			long			lBusinessTaxRate=rs->GetCollect("BusinessTaxRate");
			long			lBusinessTotalTax=rs->GetCollect("BusinessTotalTax");
			long			lYuanbaoTaxRate=rs->GetCollect("YuanbaoTaxRate");	

			CDBRegion* pRegion = new CDBRegion;
			CDBRegion::tagRegionParam& rp = pRegion->GetRegionParam();

			rp.OwnedFactionID= lOwnedFactionID;			
			rp.OwnedUnionID = lOwnedUnionID;
			rp.lCurrentTaxRate = lCurTaxRate;
			rp.dwTodayTotalTax = lTodayTotalTax;
			rp.dwTotalTax = lTotalTax;
			rp.lBusinessTaxRate=lBusinessTaxRate;
			rp.dwBusinessTotalTax=lBusinessTotalTax;
			rp.lYuanbaoTaxRate=lYuanbaoTaxRate;

			GetGame()->GetDBEntityManager()->AddOneDBRegion(pRegion);
		}

		ReleaseRs(rs);
		//ReleaseCn(cn);
		return true;
	}
	catch (_com_error &e)
	{
		ReleaseRs(rs);
		//ReleaseCn(cn);
		PrintErr("Load RegionTax error", e);
		return false;
	}

	
	return true;
}
bool CRsRegion::SaveAll()
{
	return true;
}
bool CRsRegion::LoadRegionParam(CDBRegion* pRegion)
{
	if(pRegion == NULL)
		return false;
	CDBRegion::tagRegionParam& rp = pRegion->GetRegionParam();
	rp.lID=pRegion->GetID();
//	_ConnectionPtr& cn;
	_RecordsetPtr rs;
	char sql[500] = "";
	try
	{
//		TESTHR(CreateCn(cn));
		TESTHR(CreateRs(rs));

	//	TESTHR(OpenCn(cn));
		sprintf(sql,"SELECT * FROM CSL_Region WHERE RegionID = %d",pRegion->GetID());
		TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()/*m_Connection*/));
		if( !rs->GetadoEOF() )
		{
			CGUID lOwnedFactionID = CGUID(((char*)(_bstr_t)rs->GetCollect("OwnedFactionID")));
			CGUID lOwnedUnionID = CGUID(((char*)(_bstr_t)rs->GetCollect("OwnedUnionID")));
			long			lCurTaxRate = rs->GetCollect("CurTaxRate");
			long			lTodayTotalTax = rs->GetCollect("TodayTotalTax");
			long			lTotalTax = rs->GetCollect("TotalTax");
			long			lBusinessTaxRate=rs->GetCollect("BusinessTaxRate");
			long			lBusinessTotalTax=rs->GetCollect("BusinessTotalTax");
			long			lYuanbaoTaxRate=rs->GetCollect("YuanbaoTaxRate");			
			
			rp.OwnedFactionID = lOwnedFactionID;
			rp.OwnedUnionID = lOwnedUnionID;
			rp.lCurrentTaxRate = lCurTaxRate;
			rp.dwTodayTotalTax = lTodayTotalTax;
			rp.dwTotalTax = lTotalTax;
			rp.lBusinessTaxRate=lBusinessTaxRate;
			rp.dwBusinessTotalTax=lBusinessTotalTax;
			rp.lYuanbaoTaxRate=lYuanbaoTaxRate;
			pRegion->SetLoadFromDBFlag(true);
		}

		ReleaseRs(rs);
		//ReleaseCn(cn);
		return true;
	}
	catch (_com_error &e)
	{
		ReleaseRs(rs);
		//ReleaseCn(cn);
		PrintErr("Load RegionTax error", e);
		return false;
	}
}
bool CRsRegion::Save(CDBRegion* pRegion,_ConnectionPtr& cn)
{
	if(pRegion == NULL)	return false;

	CDBRegion::tagRegionParam& RegionParam = pRegion->GetRegionParam();
			
	char sql[256];
	// 存盘线程传过来的连接不存在

/*	if(cn==NULL) 
	{
		TESTHR(CreateCn(cn));
		TESTHR(OpenCn(cn));
		//PrintErr("存盘线程传过来的连接不存在");
		//return false;
	}*/

	_RecordsetPtr rs;

	try
	{
		TESTHR(CreateRs(rs));
		sprintf(sql, "Select * from CSL_Region where RegionID = %d",RegionParam.lID);
		TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr(), adOpenForwardOnly, adLockOptimistic, adCmdText));
		if(rs->GetadoEOF())
		{
			//如果没有这个场景记录就插入
			CloseRs(rs);
			TESTHR(OpenRs("CSL_Region", rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr(), adOpenForwardOnly, adLockOptimistic, adCmdTable));
			rs->AddNew();
			rs->PutCollect("RegionID",RegionParam.lID);
		}
		char szOwnedFactionGuid[128];
		RegionParam.OwnedFactionID.tostring(szOwnedFactionGuid);
		char szOwnedUnionGuid[128];
		RegionParam.OwnedUnionID.tostring(szOwnedUnionGuid);
		rs->PutCollect("OwnedFactionID", szOwnedFactionGuid);
		rs->PutCollect("OwnedUnionID", szOwnedUnionGuid);
		rs->PutCollect("CurTaxRate", RegionParam.lCurrentTaxRate);
		rs->PutCollect("TodayTotalTax", RegionParam.dwTodayTotalTax);
		rs->PutCollect("TotalTax", RegionParam.dwTotalTax);
		rs->PutCollect("BusinessTaxRate",RegionParam.lBusinessTaxRate);
		rs->PutCollect("BusinessTotalTax",RegionParam.dwBusinessTotalTax);
		rs->PutCollect("YuanbaoTaxRate",RegionParam.lYuanbaoTaxRate);
		rs->Update();
		ReleaseRs(rs);
		//ReleaseCn(cn);
		return true;
	}
	catch(_com_error &e)
	{
		ReleaseRs(rs);
		//ReleaseCn(cn);
		PrintErr("Save CSL_Region error", e);
		return false;
	}
}
