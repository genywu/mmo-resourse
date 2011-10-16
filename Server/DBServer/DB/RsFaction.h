#pragma once
#include "..\myadobase.h"
#include "..\..\server\dbserver\appworld\DBFaction.h"

class CRsFaction :
	public CMyAdoBase
{
public:
	CRsFaction(void);
	~CRsFaction(void);

public:
	//!					读取所有帮会
	BOOL				LoadAllFaction					(map<CGUID, CDBFaction*> &mapFaction, _ConnectionPtr& cn);
	//!					保存一个帮会
	BOOL				SaveOneFaction					(CDBFaction* pDBFaction, _ConnectionPtr& cn);
	//!					删除一个帮会
	BOOL				DelOneFaction					(const CGUID &FactionGuid, _ConnectionPtr& cn);
	//!					更新（包括新建）一个帮会的成员
	BOOL				SaveOneFaction_Member			(CDBFactionMemberGroup* memberGroup, _ConnectionPtr& cn);
	//!					更新（包括新建）一个帮会的申请者
	BOOL				SaveOneFaction_Apply			(CDBFactionApplyGroup* applyGroup, _ConnectionPtr& cn);
private:
	//!					取出所有帮会基本信息
	BOOL				LoadAllFaction_BaseInfo			(map<CGUID, CDBFaction*> &mapFaction, _ConnectionPtr& cn);
	//! 				取出所有帮会成员
	BOOL				LoadAllFaction_Member			(map<CGUID, CDBFaction*> &mapFaction, _ConnectionPtr& cn);
	//! 				取出所有帮会成员申请者
	BOOL				LoadAllFaction_Apply			(map<CGUID, CDBFaction*> &mapFaction, _ConnectionPtr& cn);
	//! 				取出所有帮会留言
	BOOL				LoadAllFaction_LeaveWord		(map<CGUID, CDBFaction*> &mapFaction, _ConnectionPtr& cn);


	//!					更新（包括新建）一个帮会基本信息
	BOOL				SaveOneFaction_BaseInfo			(CDBFaction* pDBFaction, _ConnectionPtr& cn);
	//!					更新（包括新建）一个帮会的留言
	BOOL				SaveOneFaction_LeaveWord		(CDBFaction* pDBFaction, _ConnectionPtr& cn);


	//!					删除一个帮会基本信息
	BOOL				DelOneFaction_BaseInfo			(const CGUID &FactionGuid, _ConnectionPtr& cn);
	//!					删除一个帮会所有成员
	BOOL				DelOneFaction_Member			(const CGUID &FactionGuid, _ConnectionPtr& cn);
	//!					删除一个帮会所有申请者
	BOOL				DelOneFaction_Apply				(const CGUID &FactionGuid, _ConnectionPtr& cn);
	//!					删除一个帮会留言
	BOOL				DelOneFaction_LeaveWord			(const CGUID &FactionGuid, _ConnectionPtr& cn);

	//存Buffer
	bool SaveBufferField(CBaseDBEntity *pBaseEntity, const char* szFiledName, _RecordsetPtr &pRs);
	//读Buffer
	bool LoadBufferField(CBaseDBEntity *pBaseEntity, const char* szFiledName, _RecordsetPtr &pRs);
};
