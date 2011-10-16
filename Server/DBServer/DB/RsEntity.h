//RsEntity.h/////////////////////////////////////////////////////////////////////
//对象类:数据库操作接口类
//Author:安海川
//Create Time:2008.11.03
#pragma once


#include "../../../Public/ServerPublic/db/CnPool.h"

class CBaseEntity;
class CEntityGroup;

class CRsEntity :
	public CMyAdoBase
{
public:

	CRsEntity(void);
	virtual ~CRsEntity(void);

public:
	// 存储过程
	static HRESULT ProcedureEntity(CEntityGroup *pEntityGroup, _ConnectionPtr& cn);

    /// 执行存储过程
    static HRESULT ExecuteProcedure(string strProcName,
        vector<CTempEntityProperty*>& vInputParam,
        vector<CTempEntityProperty*>& vOutputParam, _ConnectionPtr& cn);

	// 创建角色特定存储过程
	static HRESULT CreateRoleProcedureEntity(CEntityGroup *pEntityGroup, _ConnectionPtr& cn);

	// 添加邮件特定存储过程
	static HRESULT InserMailProcedureEntity(CEntityGroup *pBaseEntity, _ConnectionPtr& cn);    

    // DB_OPERATION_TYPE 类型目前只适用于INSERT and SAVE 两种
    static HRESULT OperEntityGroups(CEntityGroup *pBaseEntity, DB_OPERATION_TYPE opType, _ConnectionPtr& cn);
    static HRESULT OperLeaves(const char* szTblName, const char* szComFlag, map<CGUID,CBaseEntity*>& entityMap, DB_OPERATION_TYPE opType, _ConnectionPtr& cn);
    static HRESULT OperAllEntityGroups(CEntityGroup *pBaseEntity, DB_OPERATION_TYPE opType, _ConnectionPtr& cn);

	static HRESULT LoadEntityAttrs(CBaseEntity *pEntity, _ConnectionPtr& cn);
	static HRESULT LoadEntitys(const char* szSql, const string& strComFlag, map<CGUID, CBaseEntity*>& entityMap, _ConnectionPtr& cn);
	static HRESULT LoadAllEntityGroup(CEntityGroup *pEntityGroup, _ConnectionPtr& cn);

	static HRESULT DeleteInsertEntityGroupAttrs(CEntityGroup *pEntity, _ConnectionPtr& cn);
	static HRESULT DeleteInsertLeaves(const char* szSql, const string& strComFlag, map<CGUID, CBaseEntity*>& entityMap, _ConnectionPtr& cn);
	static HRESULT DeleteInsertAllEntityGroup(CEntityGroup *pEntityGroup, _ConnectionPtr& cn);

	static HRESULT DeleteEntityGroupAttrs(CEntityGroup *pEntity, _ConnectionPtr& cn);
	static HRESULT DeleteEntitys(const char* szSql, const char* szTblName, const string& strComFlag, map<CGUID, CBaseEntity*>& entityMap, _ConnectionPtr& cn);

	//读Buffer
	static HRESULT LoadBufferField(CBaseEntity *pEntityGroup, const char* szFiledName, _RecordsetPtr &pRs);

	//产生查询字符串
	static HRESULT GenerateSqlString(char* szRetSqlStr, long lStrSize, DB_OPERATION_TYPE opType, CEntityGroup* pBaseEntity);
};


