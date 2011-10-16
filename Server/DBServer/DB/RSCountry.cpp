#include "stdafx.h"
#include "RsCountry.h"
#include "..\..\server\dbserver\AppWorld\DBCountry.h"
#include "..\..\server\dbserver\DBServer\game.h"
#include "..\..\server\dbserver\appworld\dbentitymanager.h"
#include "../../server/DBServer/AppWorld/DBAccessThread.h"
#include "../../server/DBServer/AppWorld/DBLoginPlayer.h"
#include "../../server/DBServer/AppWorld/DBLoadThread.h"
#include "../../server/DBServer/AppWorld/DBSaveThread.h"


CRSCountry::CRSCountry()
{
}

CRSCountry::~CRSCountry()
{
}

bool CRSCountry::Load(std::list<LoadThreadData*>& loadDataList, _ConnectionPtr& cn)
{
	char sql[128];
	_RecordsetPtr rs;

	try
	{
		TESTHR(CreateRs(rs));
		sprintf(sql,"SELECT * FROM CSL_Countrys");
		TESTHR(	OpenRs(sql, rs, cn, adOpenForwardOnly) );

		int pos = 0;
		CDBCountry* pCountry = NULL;
		
		long nCountryNum = rs->GetRecordCount();

		while (!rs->GetadoEOF())
		{
			long id = rs->GetCollect("id");
			long power = rs->GetCollect("power");
			long tl = rs->GetCollect("tech_level");
			CGUID king = CGUID(((char*)(_bstr_t)rs->GetCollect("king")));
			
			CGUID guid;
			CGUID::CreateGUID(guid);
			
			LoadThreadData* loadData = new LoadThreadData(guid, DB_COUNTRY);
			if((CDBCountry*)loadData->dbObj)
			{
				((CDBCountry*)loadData->dbObj)->SetID(id);
				((CDBCountry*)loadData->dbObj)->SetPower(power);
				((CDBCountry*)loadData->dbObj)->SetTechLevel(tl);
				((CDBCountry*)loadData->dbObj)->SetKing(king);
			}
			else
			{
				TESTHR(false);
			}
			loadData->loadObj->objNum = nCountryNum;
			loadData->loadObj->msgID = -1;
			loadData->loadObj->objType = DB_COUNTRY;
			loadData->loadObj->objGUID = guid;
			loadDataList.insert(loadDataList.end(), loadData);

			rs->MoveNext();
			pos++;
		}
		ReleaseRs(rs);
		return true;
	}
	catch(_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr("load Country", e);		
		return false;
	}
}
bool CRSCountry::Save(CDBCountry* pCountry ,_ConnectionPtr& cn)
{
	if(!pCountry)
		return false;

	bool IsLocalConn = false;

	if(cn==NULL) 
	{
		// 存盘线程传过来的连接不存在
		TESTHR(CreateCn(cn)); // 创建连接对象失败
	//	PrintErr("存盘线程传过来的连接不存在,创建连接失败");
	//	return false;
		IsLocalConn = true;
	}

	char sql[128];

	_RecordsetPtr rs;

	try
	{
		TESTHR(CreateRs(rs));

		TESTHR(OpenCn(cn));
		sprintf(sql, "SELECT * FROM CSL_Countrys WHERE id='%d'", pCountry->GetID());
		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockOptimistic, adCmdText));


		// 如果此记录不存在（不可能发生）
		if(rs->GetadoEOF())
		{
			sprintf( sql, "INSERT INTO CSL_Countrys(id,power,tech_level,king) \
						VALUES (%d,%d,%d,%d)",
						pCountry->GetID(),pCountry->GetPower(),pCountry->GetTechLevel(),pCountry->GetKing() );
			TESTHR(ExecuteCn(sql, cn));
		}
		else
		{
			char szKing[128];
			pCountry->GetKing().tostring(szKing);
			rs->PutCollect("power",pCountry->GetPower());
			rs->PutCollect("tech_level",pCountry->GetTechLevel());
			rs->PutCollect("king", szKing);
			rs->Update();
		}

		if(IsLocalConn)
			//ReleaseCn(cn);
		ReleaseRs(rs);
		return true;
	}
	catch(_com_error &e)
	{
		if(IsLocalConn)
			//ReleaseCn(cn);
		ReleaseRs(rs);
		PrintErr("load Country", e);		
		return false;
	}
}
