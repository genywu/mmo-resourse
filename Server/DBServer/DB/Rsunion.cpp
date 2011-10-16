#include "StdAfx.h"
#include ".\Rsunion.h"
#include "..\..\server\dbserver\AppWorld\DBUnion.h"
#include "..\..\server\dbserver\AppWorld\DBentityManager.h"
#include "..\..\server\DBServer/dbserver/game.h"
#include "../../server/DBServer/AppWorld/DBAccessThread.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CRsUnion::CRsUnion(void)
{
}

CRsUnion::~CRsUnion(void)
{
}
//bool CRsUnion::SaveAllConfederation()
//{
//	return true;
//}
////添加一条盟的记录
////返回值0:失败，大于0成功
//long CRsUnion::AddConfederation(const CGUID& MasterID,const char* strName)
//{
//	//	char sql[512];
//	//	_ConnectionPtr& cn;
//	//	_RecordsetPtr rs;
//	//	int lID = 0;
//	//	try
//	//	{
//	//		TESTHR(CreateCn(cn));
//	//		TESTHR(CreateRs(rs));
//	//
//	//		TESTHR(OpenCn(cn));
//	//		sprintf(sql,"SELECT MAX(ID) AS maxid FROM CSL_CONFEDERATION");
//	//		TESTHR(OpenRs(sql, rs,cn));
//	//		lID = rs->GetCollect("maxid");
//	//		lID++;	
//	//	}
//	//	catch (_com_error &e) 
//	//	{
//	//		ReleaseRs(rs);
//	//		//ReleaseCn(cn);
//	//		PrintErr("load Faction error", e);
//	//		return 0;
//	//	}
//	//	ReleaseRs(rs);
//	//	
//	//	SYSTEMTIME Time;
//	//	GetLocalTime(&Time);
//	//	char strDate[20];
//	//	sprintf(strDate,"%d-%d-%d %d:%d:%d",Time.wYear,Time.wMonth,Time.wDay,Time.wHour,Time.wMinute,Time.wSecond);
//	//
//	//	sprintf(sql,"INSERT INTO CSL_CONFEDERATION(ID,Name,MasterID,EstaTime) VALUES (%d,'%s',%d,'%s')",
//	//		lID,strName,lMasterID,strDate);
//	//	if(!ExecuteCn(sql, cn))
//	//	{
//	//		//ReleaseCn(cn);
//	//		PrintErr("AddCondederation error");
//	//		return 0;
//	//	}
//	//	//ReleaseCn(cn);
//	//	return lID;
//	return 0;
//}
//
////删除一个盟得记录
//bool CRsUnion::DelConfederation(const CGUID& ConfederID,_ConnectionPtr& cn)
//{
//	char sql[256];
//	bool IsLocalConn = false;
//	// 存盘线程传过来的连接不存在
///*	if(cn==NULL) 
//	{
//		TESTHR(CreateCn(cn));
//		TESTHR(OpenCn(cn));
//	//	PrintErr("存盘线程传过来的连接不存在");
//	//	return false;
//		IsLocalConn = true;
//	}*/
//
//	char szConfederID[40] = {0};
//	ConfederID.tostring(szConfederID);
//	sprintf(sql,"DELETE CSL_UNION_BaseProperty WHERE guid = '%s'", szConfederID);
//	if(!ExecuteCn(sql, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()))
//	{
////		if(IsLocalConn)
//			//ReleaseCn(cn);
//		PrintErr("Delete Union error");
//		return false;
//	}
////	if(IsLocalConn)
//		//ReleaseCn(cn);
//	return true;
//}
//
////装载盟
//bool CRsUnion::LoadConfederation(CDBUnion* pConf)
//{
//	return true;
//}
//
////保存盟
//bool CRsUnion::SaveConfederation(CDBUnion* pConf,_ConnectionPtr& cn)
//{
//	char sql[256];
//	// 存盘线程传过来的连接不存在
//
//	bool IsLocalConn = false;
//
///*	if(cn==NULL) 
//	{
//		TESTHR(CreateCn(cn));
//		TESTHR(OpenCn(cn));
//	//	PrintErr("存盘线程传过来的连接不存在");
//	//	return false;
//		IsLocalConn = true;
//	}*/
//
//	_RecordsetPtr rs;
//
//	char szConfID[128];
//	pConf->GetExID().tostring(szConfID);
//	char szMasterID[128];
//	try
//	{
//		TESTHR(CreateRs(rs));
//		sprintf(sql, "Select * from CSL_UNION_BaseProperty where ID = '%s'", szConfID);
//		TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr(), adOpenForwardOnly, adLockOptimistic, adCmdText));
//		if(rs->GetadoEOF())
//		{
//			//如果没有帮会记录就插入
//			ReleaseRs(rs);
//			pConf->GetMasterID().tostring(szMasterID);
//			// 插入CSL_FACTION_BaseProperty表
//			sprintf(sql, "INSERT INTO CSL_UNION_BaseProperty (guid,Name,MasterID) VALUES ('%s','%s','%s')",
//				szConfID,pConf->GetName().c_str(),szMasterID);
//
//			TESTHR(ExecuteCn(sql, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()));
//		}
//		else
//		{
//			rs->PutCollect("MasterID", szMasterID);
//			rs->Update();
//			ReleaseRs(rs);
//		}
//
//
//		if(	!SaveConfeMembers(pConf, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()) )
//		{
//	//		if(IsLocalConn)
//				//ReleaseCn(cn);
//			return false;
//		}
//
//	//	if(IsLocalConn)
//	//		//ReleaseCn(cn);
//		return true;
//	}
//	catch(_com_error &e)
//	{
//		ReleaseRs(rs);
//	//	if(IsLocalConn)
//			//ReleaseCn(cn);
//		PrintErr("Save Union error", e);
//		return false;
//	}
//}
//
////装载所有盟
//int CRsUnion::LoadAllConfederation()
//{
//	int nRet = 0;
//	char sql[256];
//	CGUID lConfeID;
//	string strName;
//	CGUID lMasterID;
//	tagTime Time;
////	_ConnectionPtr& cn;
//	_RecordsetPtr rs;
//
//	try
//	{
//	//	TESTHR(CreateCn(cn));
//		TESTHR(CreateRs(rs));
//
//	//	TESTHR(OpenCn(cn));
//		sprintf(sql,"SELECT * FROM CSL_UNION_BaseProperty");
//		TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()/*m_Connection*/));
//		while(!rs->GetadoEOF())
//		{
//			nRet++;
//
//			lConfeID = CGUID(((char*)(_bstr_t)rs->GetCollect("ID")));
//			strName = (_bstr_t)rs->GetCollect("Name");
//			lMasterID = CGUID(((char*)(_bstr_t)rs->GetCollect("MasterID")));
//
//			//VariantTimeToSystemTime( rs->GetCollect("EstaTime"), &sysTime );
//
//			//memcpy(&Time, &sysTime, sizeof(tagTime));
//
//			//建立帮会
//			CDBUnion* pConf = new CDBUnion(lConfeID);
//			pConf->SetMasterID(lMasterID);
//			pConf->SetName(strName);
//			if(!LoadConfeMembers(pConf, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()/*m_Connection*/))
//			{
//				SAFE_DELETE(pConf);
//
//				ReleaseRs(rs);
//				//ReleaseCn(cn);
//				return false;
//			}
//
//			//pConf->SetChangeData(Org_NoDataChange);
//			//GetOrganiCtrl()->AddConfederationOrganizing(lConfeID,pConf);
//			CDBEntityManager::DBUnionMap& u = GetGame()->GetDBEntityManager()->GetDBUnionMap();
//			u[lConfeID] = pConf;
//
//			rs->MoveNext();
//		}
//		ReleaseRs(rs);
//		//ReleaseCn(cn);
//		return nRet;
//
//	}
//	catch(_com_error &e)
//	{
//		ReleaseRs(rs);
//		//ReleaseCn(cn);
//		PrintErr("Save Confederation error", e);
//		return false;
//	}
//}
//
////装载盟成员
//bool CRsUnion::LoadConfeMembers(CDBUnion *pConf,_ConnectionPtr& cn)
//{
//	assert(pConf);
//	if(!pConf) return false;
//
//	_RecordsetPtr rs;
//	char sql[512];
//
//	char szConfID[128];
//	pConf->GetExID().tostring(szConfID);
//
//	try
//	{
//		TESTHR(CreateRs(rs));
//		sprintf(sql, "SELECT * FROM CSL_UNION_Members WHERE UnionID= '%s'", szConfID);
//		TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()));
//		tagMemInfo MemInfo;
//		while(!rs->GetadoEOF())
//		{
//			MemInfo.MemberGuid = CGUID(((char*)(_bstr_t)rs->GetCollect("FactionID")));
//			MemInfo.lJobLvl = rs->GetCollect("MemberLvl");
//			string strTitle = (_bstr_t)rs->GetCollect("Title");
//			//判断字符串长度是否大于20
//			if(strlen(strTitle.c_str()) <= 20 )
//			{
//				strcpy(MemInfo.strTitle,strTitle.c_str());
//			}
//			else
//			{
//				strcpy(MemInfo.strTitle,"");
//			}
//			//VariantTimeToSystemTime( rs->GetCollect("LastOnlineTime"), &sysTime );
//			//memcpy(&MemInfo.LastOnlineTime,&sysTime,sizeof(tagTime));
//			MemInfo.bControbute = rs->GetCollect("bControbute");
//
//			MemInfo.listPV[PV_Disband]		 =
//				static_cast<ePurviewOwnState>((int)rs->GetCollect("PV_Disband"));
//			MemInfo.listPV[PV_Exit]		 =
//				static_cast<ePurviewOwnState>((int)rs->GetCollect("PV_Exit"));
//			MemInfo.listPV[PV_DubJobLvl]	 =
//				static_cast<ePurviewOwnState>((int)rs->GetCollect("PV_DubJobLvl"));
//			MemInfo.listPV[PV_ConMem]		 =
//				static_cast<ePurviewOwnState>((int)rs->GetCollect("PV_ConMem"));
//			MemInfo.listPV[PV_FireOut]		 =
//				static_cast<ePurviewOwnState>((int)rs->GetCollect("PV_FireOut"));
//			MemInfo.listPV[PV_Pronounce]	 =
//				static_cast<ePurviewOwnState>((int)rs->GetCollect("PV_Pronounce"));
//			MemInfo.listPV[PV_LeaveWord]	 =
//				static_cast<ePurviewOwnState>((int)rs->GetCollect("PV_LeaveWord"));
//			MemInfo.listPV[PV_EditLeaveWord]= 
//				static_cast<ePurviewOwnState>((int)rs->GetCollect("PV_EditLeaveWord"));
//			MemInfo.listPV[PV_ObtainTax]	 =
//				static_cast<ePurviewOwnState>((int)rs->GetCollect("PV_ObtainTax"));
//			MemInfo.listPV[PV_OperCityGate] =
//				static_cast<ePurviewOwnState>((int)rs->GetCollect("PV_OperCityGate"));
//			MemInfo.listPV[PV_EndueROR]	 =
//				static_cast<ePurviewOwnState>((int)rs->GetCollect("PV_EndueROR"));
//
//			if( GetUnionMemInfo(MemInfo,cn) == true )
//			{
//				map<CGUID,tagMemInfo>& mapMem = pConf->GetMembers();
//				mapMem[MemInfo.MemberGuid] = MemInfo;
//			}
//
//			rs->MoveNext();
//		}
//
//		//当前成员的个数
//		ReleaseRs(rs);		
//		return true;
//	}
//	catch(_com_error &e)
//	{
//		ReleaseRs(rs);
//		PrintErr("load Union Memebers", e);
//		return false;
//	}
//
//}
//
//
////得到同盟成员信息
//bool CRsUnion::GetUnionMemInfo(tagMemInfo& MemInfo,_ConnectionPtr& cn)
//{
//	//_RecordsetPtr rs;
//	//char sql[512];
//	//try
//	//{
//	//	TESTHR(CreateRs(rs));
//	//	// 去数据库中取
//	//	sprintf(sql,"SELECT * FROM CSL_FACTION_BaseProperty WHERE ID=%d",MemInfo.lID);
//	//	TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()));
//	//	if (rs->GetadoEOF())
//	//	{
//	//		char strError[256];
//	//		sprintf(strError,"在数据库中没有查到到同盟成员%ID", MemInfo.lID);
//	//		PrintErr(strError);
//	//		ReleaseRs(rs);
//	//		return false;
//	//	}
//
//	//	string strFactionName= (_bstr_t)(rs->GetCollect("Name"));
//	//	if(strlen(strFactionName.c_str()) <= 20 )
//	//	{
//	//		strcpy(MemInfo.strName,strFactionName.c_str());
//	//	}
//	//	else
//	//	{
//	//		strcpy(MemInfo.strName,"");
//	//	}
//	//	MemInfo.lLvl=0;
//	//	MemInfo.lOccu=0;
//	//	ReleaseRs(rs);
//	//	return true;
//	//}
//	//catch (_com_error &e)
//	//{
//	//	ReleaseRs(rs);
//	//	PrintErr("load Union Memebers", e);
//	//	return false;
//	//}
//	return true;
//}
////保存盟成员
//bool CRsUnion::SaveConfeMembers(CDBUnion *pConf,_ConnectionPtr& cn)
//{
//	//if(pConf==NULL)
//	//	return false;
//	//char sql[500];
//	//char szConfID[128];
//	//pConf->GetExID().tostring(szConfID);
//	//// 先删除该帮会的成员
//	//sprintf(sql,"DELETE FROM CSL_UNION_Members WHERE UnionID='%s'", szConfID);
//	//if(!ExecuteCn(sql, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()))
//	//{
//	//	PrintErr("delete CSL_UNION_Members error");
//	//	return false;
//	//}
//
//	//try
//	//{
//	//	map<CGUID,tagMemInfo>::iterator it = pConf->GetMembers().begin();
//	//	for(; it != pConf->GetMembers().end();it++)
//	//	{
//	//		tagMemInfo& MemInfo = (*it).second;
//
//	//		//sprintf(strDate,"%d-%d-%d %d:%d:%d" ,MemInfo.LastOnlineTime.wYear,MemInfo.LastOnlineTime.wMonth,
//	//		//	MemInfo.LastOnlineTime.wDay,MemInfo.LastOnlineTime.wHour,MemInfo.LastOnlineTime.wMinute,
//	//		//	MemInfo.LastOnlineTime.wSecond);
//
//	//		sprintf(sql, "INSERT INTO CSL_UNION_Members (UnionID,FactionID,MemberLvl,Title,bControbute,\
//	//					 PV_Disband,PV_Exit,PV_DubJobLvl,PV_ConMem,PV_FireOut,PV_Pronounce,PV_LeaveWord,\
//	//					 PV_EditLeaveWord,PV_ObtainTax,PV_OperCityGate,PV_EndueROR)\
//	//					 VALUES ('%s',%d,%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
//
//	//					 szConfID,MemInfo.lID,MemInfo.lJobLvl,MemInfo.strTitle,MemInfo.bControbute,
//	//					 MemInfo.listPV[PV_Disband],MemInfo.listPV[CDBUnion::PV_Exit],
//	//					 MemInfo.listPV[CDBUnion::PV_DubJobLvl],MemInfo.listPV[CDBUnion::PV_ConMem],
//	//					 MemInfo.listPV[CDBUnion::PV_FireOut],MemInfo.listPV[CDBUnion::PV_Pronounce],
//	//					 MemInfo.listPV[CDBUnion::PV_LeaveWord],MemInfo.listPV[CDBUnion::PV_EditLeaveWord],
//	//					 MemInfo.listPV[CDBUnion::PV_ObtainTax],MemInfo.listPV[CDBUnion::PV_OperCityGate],
//	//					 MemInfo.listPV[CDBUnion::PV_EndueROR]);
//
//	//		TESTHR(ExecuteCn(sql, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()));
//	//	}
//	//	return true;
//	//}
//	//catch (_com_error &e)
//	//{
//	//	PrintErr("Save Faction MemberData", e);
//	//	return false;
//	//}
//	return true;
//}