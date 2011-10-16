#include "StdAfx.h"
#include ".\rscitywar.h"
#include "..\..\server\dbserver\AppWorld\DBCityWar.h"
#include "..\..\server\dbserver\AppWorld\DBentityManager.h"
#include "..\..\server\DBServer/dbserver/game.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//
//CRsCityWar::CRsCityWar(void)
//{
//}
//
//CRsCityWar::~CRsCityWar(void)
//{
//}
//bool CRsCityWar::Load(CDBCityWar* cw)
//{
//	return true;
//}
//bool CRsCityWar::Save(CDBCityWar* cw)
//{
//	return true;
//}
////装载所有的城战参数
//bool	CRsCityWar::LoadAllCityWarParam()
//{
//	_ConnectionPtr cn;
//	_RecordsetPtr rs;
//	char sql[500] = "";
//	try
//	{
//		TESTHR(CreateCn(cn));
//		TESTHR(CreateRs(rs));
//
//		TESTHR(OpenCn(cn));
//		sprintf(sql,"SELECT * FROM CSL_CityWar");
//		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockOptimistic, adCmdTable));
//
//		SYSTEMTIME	sysTime;
//		tagTime Time;
//		long pos = 0;
//
//		CDBEntityManager::DBCityWarMapItr itr = GetGame()->GetDBEntityManager()->DBCityWarMapBegin();
//		for(; itr != GetGame()->GetDBEntityManager()->DBCityWarMapEnd(); itr++)
//		{
//			if(itr->second)
//				SAFE_DELETE(itr->second);
//		}
//		GetGame()->GetDBEntityManager()->GetDBCityWarMap().clear();
//
//		while(!rs->GetadoEOF())
//		{
//			// [未完成]
//			pos = (long)rs->GetCollect("ID");
//			long lRegionID = rs->GetCollect("RegionID");
//			VariantTimeToSystemTime( rs->GetCollect("WarStartTime"), &sysTime );
//			memcpy(&Time,&sysTime,sizeof(tagTime));
//
//			long lSize;
//			list<CGUID>		DecWarFactions;
//			DecWarFactions.clear();					
//			lSize = rs->GetFields()->GetItem("DecWarFactions")->ActualSize;
//			if(lSize>0)
//			{
//				_variant_t varBLOB;
//				varBLOB = rs->GetFields()->GetItem("DecWarFactions")->GetChunk(lSize);
//				BYTE *pBuf=NULL;
//				SafeArrayAccessData(varBLOB.parray,(void**)&pBuf);
//				SafeArrayUnaccessData(varBLOB.parray);
//
//				CGUID* pGuid = NULL;
//				long lCount = lSize/sizeof(CGUID);
//				for(long i=0; i < lCount;i++)
//				{
//					pGuid = (CGUID*)pBuf;
//					DecWarFactions.push_back(*pGuid);
//					pBuf+=sizeof(CGUID);
//				}
//			}
//
//			CGUID guid;
//			CGUID::CreateGUID(guid);
//			CDBCityWar* cw = new CDBCityWar(guid);
//			CDBEntityManager::DBCityWarMap&  wm = GetGame()->GetDBEntityManager()->GetDBCityWarMap();
//			cw->SetRegionID(lRegionID);
//			cw->SetWarStartTime(Time);
//			cw->SetDecWarFactions(DecWarFactions);
//			wm[guid] = cw;
//			//GetAttackCitySys()->SetDecWarFactionsFromDB(lRegionID,Time,DecWarFactions);
//
//			rs->MoveNext();
//			pos++;
//		}
//	}
//	catch (_com_error &e)
//	{
//		ReleaseRs(rs);
//		//ReleaseCn(cn);
//		PrintErr("Load CSL_CityWar error", e);
//		return false;
//	}
//
//	ReleaseRs(rs);
//	//ReleaseCn(cn);
//	return true;
//}
////保存所有的城战参
//bool CRsCityWar::SaveAllCityWarParam(std::map<CGUID, CDBCityWar*>& CityWarParam)
//{
//	_ConnectionPtr cn;
//	_RecordsetPtr rs;
//	char sql[500] = "";
//	try
//	{
//		TESTHR(CreateCn(cn));
//		TESTHR(CreateRs(rs));
//
//		TESTHR(OpenCn(cn));
//		sprintf(sql,"SELECT * FROM CSL_CityWar");
//		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockOptimistic, adCmdTable));
//
//	char sql[500] = "DELETE FROM CSL_CityWar";
//	// 先删除该帮会的成员
//	if(!ExecuteCn(sql, cn))
//	{
//		PrintErr("delete CSL_CityWar error");
//			//ReleaseCn(cn);
//		return false;
//	}
//	_RecordsetPtr rs;
//	try
//	{
//		char strDate[200];
//		TESTHR(CreateRs(rs));
//		TESTHR(OpenRs("CSL_CityWar", rs, cn, adOpenForwardOnly, adLockOptimistic, adCmdTable));
//		CDBEntityManager::DBCityWarMapItr it = CityWarParam.begin();
//		for(; it != CityWarParam.end();it++)
//		{
//				rs->AddNew();
//				rs->PutCollect("RegionID",(*it).second->GetRegionID());
//					
//				sprintf(strDate,"%d-%d-%d %d:%d:%d" ,it->second->GetWarStartTime().Year(),it->second->GetWarStartTime().Month(),
//					it->second->GetWarStartTime().Day(),it->second->GetWarStartTime().Hour(),it->second->GetWarStartTime().Minute(),
//					it->second->GetWarStartTime().Second());
//				rs->PutCollect("WarStartTime",strDate);
//
//				//保存报名玩家列表
//				VARIANT varBLOB;
//				SAFEARRAY* psa;
//				SAFEARRAYBOUND rgsabound[1];
//
//				vector<BYTE> ByteArray;
//				//得到成员信息的数据
//					list<CGUID>::iterator ita = it->second->GetDecWarFactions().begin();
//					for(;ita != it->second->GetDecWarFactions().end();ita++)
//				{
//						_AddToByteArray(&ByteArray,&(*ita), 128);
//				}
//
//				rgsabound[0].lLbound = 0;
//				rgsabound[0].cElements = ByteArray.size();
//				psa = SafeArrayCreate(VT_UI1,1,rgsabound);
//
//				for(long i=0;i < ByteArray.size();i++)
//				{
//					SafeArrayPutElement(psa,&i,&ByteArray[i]);
//				}
//				varBLOB.vt = VT_UI1|VT_ARRAY;
//				varBLOB.parray = psa;
//
//				rs->GetFields()->GetItem("DecWarFactions")->AppendChunk(varBLOB);
//
//				rs->Update();
//
//				SafeArrayDestroy(psa);
//			}
//		ReleaseRs(rs);
//		return true;
//	}
//	catch (_com_error &e)
//	{
//		ReleaseRs(rs);
//		PrintErr("Save CSL_CityWar error", e);
//		return false;
//	}
//	}
//	catch (_com_error &e)
//	{
//		ReleaseRs(rs);
//		//ReleaseCn(cn);
//		PrintErr("Load CSL_CityWar error", e);
//		return false;
//	}
//
//	ReleaseRs(rs);
//	//ReleaseCn(cn);
//
//	return true;
//}
