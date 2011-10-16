#include "StdAfx.h"
#include ".\rsvillagewar.h"
#include "..\..\server\dbserver\AppWorld\DBVillageWar.h"
#include "..\..\server\DBServer/dbserver/game.h"
#include "..\..\server\dbserver\AppWorld\DBentityManager.h"
#include "../../server/DBServer/AppWorld/DBAccessThread.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//
//CRsVillageWar::CRsVillageWar(void)
//{
//}
//
//CRsVillageWar::~CRsVillageWar(void)
//{
//}
//bool CRsVillageWar::Load(CDBVillageWar* vw)
//{
//	return true;
//}
//bool CRsVillageWar::Save(CDBVillageWar* vw)
//{
//	return true;
//}
////装载所有的敌对帮会关系
//bool	CRsVillageWar::LoadAllVillageWarParam()
//{
//	_ConnectionPtr cn;
//	_RecordsetPtr rs;
//	char sql[500] = "";
//	try
//	{
//	//	TESTHR(CreateCn(cn));
//		TESTHR(CreateRs(rs));
//
//	//	TESTHR(OpenCn(cn));
//		sprintf(sql,"SELECT * FROM CSL_VillageWar");
//		TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()/*m_Connection*/));
//
//		SYSTEMTIME	sysTime;
//		tagTime Time;
//		long pos = 0;
//
//		CDBEntityManager::DBVillageWarMapItr itr = GetGame()->GetDBEntityManager()->DBVillageWarMapBegin();
//		for(; itr != GetGame()->GetDBEntityManager()->DBVillageWarMapEnd(); itr++)
//		{
//			if(itr->second)
//				SAFE_DELETE(itr->second);
//		}
//		GetGame()->GetDBEntityManager()->GetDBVillageWarMap().clear();
//
//		while(!rs->GetadoEOF())
//		{
//			// [未完成]
//			pos = (long)rs->GetCollect("ID");
//			CDBVillageWar* vw = new CDBVillageWar(NULL_GUID);
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
//				CGUID lID;
//				long lCount = lSize/sizeof(long);
//				for(long i=0; i < lCount;i++)
//				{
//					lID = *(CGUID*)pBuf;
//					DecWarFactions.push_back(lID);
//					pBuf+=128;
//				}
//			}
//
//			vw->SetRegionID(lRegionID);
//			vw->SetWarStartTime(Time);
//			vw->SetDecWarFactions(DecWarFactions);
//			GetGame()->GetDBEntityManager()->AddOneDBVillageWar(vw);
//
//			rs->MoveNext();
//			pos++;
//		}
//
//		ReleaseRs(rs);
//		//ReleaseCn(cn);
//		return true;
//	}
//	catch (_com_error &e)
//	{
//		ReleaseRs(rs);
//		//ReleaseCn(cn);
//		PrintErr("Load CSL_VillageWar error", e);
//		return false;
//	}
//}
//
////保存所有的敌对帮会关系
//bool	CRsVillageWar::SaveAllVillageWarParam(std::map<CGUID, CDBVillageWar*>& VillageWarSetup,_ConnectionPtr& cn)
//{
////	TESTHR(CreateCn(cn));
////	TESTHR(OpenCn(cn));
//
//	char sql[500] = "DELETE FROM CSL_VillageWar";
//	// 先删除该帮会的成员
//	if(!ExecuteCn(sql, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()))
//	{
//		PrintErr("delete CSL_CityWar error");
//		return false;
//	}
//	_RecordsetPtr rs;
//	try
//	{
//		char strDate[200];
//		TESTHR(CreateRs(rs));
//		TESTHR(OpenRs("CSL_VillageWar", rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr(), adOpenForwardOnly, adLockOptimistic, adCmdTable));
//		CDBEntityManager::DBVillageWarMapItr it = VillageWarSetup.begin();
//		for(; it != VillageWarSetup.end();it++)
//		{
//			//tagVilWarSetup* pParam = (*it);
//			//if(pParam)
//			{
//				rs->AddNew();
//				rs->PutCollect("RegionID",it->second->GetRegionID());
//				sprintf(strDate,"%d-%d-%d %d:%d:%d" ,it->second->GetWarStartTime().Year(),it->second->GetWarStartTime().Month(),
//				it->second->GetWarStartTime().tmTime.tm_wday,it->second->GetWarStartTime().Hour(),it->second->GetWarStartTime().Minute(),
//				it->second->GetWarStartTime().Second());
//				rs->PutCollect("WarStartTime",strDate);
//
//				//保存报名玩家列表
//				VARIANT varBLOB;
//				SAFEARRAY* psa;
//				SAFEARRAYBOUND rgsabound[1];
//
//				vector<BYTE> ByteArray;
//				//得到成员信息的数据
//				list<CGUID>::iterator ita = it->second->GetDecWarFactions().begin();
//				for(;ita != it->second->GetDecWarFactions().end();it++)
//				{
//					_AddToByteArray(&ByteArray,&(*ita), 128);
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
//				SafeArrayDestroy(psa);
//			}
//			rs->Update();
//		}
//		ReleaseRs(rs);
//		//ReleaseCn(cn);
//		return true;
//	}
//	catch (_com_error &e)
//	{
//		ReleaseRs(rs);
//		//ReleaseCn(cn);
//		PrintErr("Save CSL_VillageWar error", e);
//		return false;
//	}
//	return true;
//}