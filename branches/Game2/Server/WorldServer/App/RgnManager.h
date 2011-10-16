#ifndef RGN_MANAGER_H
#define RGN_MANAGER_H

#include "..\public\BaseDef.h"
#include <map>
#include "../public/GUID.h"
#include "WorldRegion.h"
#include "WorldVillageRegion.h"
#include "WorldWarRegion.h"
//#include "ProxyServerRegion.h"
#include "../public/msgtype.h"
#include "DBEntityDef.h"

// 预分配Rgn内存块
#define MAX_FREE_RGN_NUM 16

class CMessage;

class CRgnManager
{
public:

	CRgnManager();
	~CRgnManager();

	//使用Region GUID查找
	typedef std::map<CGUID/*RgnID*/, CWorldRegion*> RgnByGUIDMap;
	typedef RgnByGUIDMap::iterator RgnByIDMapItr;

	typedef std::map<CGUID, CGUID> OwnerID2RgnGUIDMap;
	typedef OwnerID2RgnGUIDMap::iterator OwnerID2RgnIDMapItr;

	//! 创建Rgn,向GS发送RGN_CREATE消息
	CWorldRegion* CreateRgn(long scrGSID/*发送创建请求的GSID*/, long templateRgnID/*模板Region的ID*/, eRgnType rgnType, long GSID, const CGUID& ownerID=NULL_GUID/*NORMAL类型则ID为空*/, long scriptID=0, const CGUID& dbRgnGUID=NULL_GUID, const char* szRetVarName=NULL, const char* szRetRgnIDName=NULL/*异步返回值变量名*/);
	//! 删除Rgn,向GS发送RGN_DELETE消息
	void DeleteRgn(eRgnType rgnType, const CGUID& ownerID, const CGUID& rgnID, long GSID, long scriptID=0, const char* szRetVarName=NULL, const char* szRetRgnIDName=NULL/*异步返回值变量名*/);

	//! 查找Region
	CWorldRegion* FindRgnByGUID(eRgnType rgnType, const CGUID& rgnID=NULL_GUID);
	
	//! 添加变量, 向WS发送VAR_ADD消息
	bool AddVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, const char* strValue);
	bool AddVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, long value);
	bool AddVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, long value, long arraySize/*数组大小*/);
	bool AddVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, const CGUID& value);
	//! 删除变量, 向WS发送VAR_DEL消息
	bool DelVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName);
	//! 设置变量, 向WS发送VAR_SET消息
	bool SetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, const char* strValue);
	bool SetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, long value);
	bool SetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, long value, long arrayPos/*数组位置*/);
	bool SetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, const CGUID& value);
	//! 取得变量, 向WS发送VAR_GET消息
	bool GetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, const char* strValue);
	bool GetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, long& value);
	bool GetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, long& value, long arrayPos/*数组位置*/);
	bool GetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, CGUID& value);

	RgnByGUIDMap& GetPersonalDupRgn(void) { return m_mapPersonalDupRgn; }
	RgnByGUIDMap& GetTeamDupRgn(void) { return m_mapTeamDupRgn; }
	RgnByGUIDMap& GetPersonalHouseDupRgn(void) { return m_mapPersonalHouseDupRgn; }
	RgnByGUIDMap& GetNormalRgn(void) { return m_mapNormalRgn; }

	// 发送数据库删除DBDupRgn消息
	void SendDelPHDupRgnMsgToDB(const CGUID& rgnGUID);

private:
	RgnByGUIDMap m_mapPersonalDupRgn;
	RgnByGUIDMap m_mapTeamDupRgn;
	RgnByGUIDMap m_mapPersonalHouseDupRgn;
	RgnByGUIDMap m_mapNormalRgn;

	OwnerID2RgnGUIDMap m_mapPersonalID2RgnID;
	OwnerID2RgnGUIDMap m_mapPersonalHouseID2RgnID;
	OwnerID2RgnGUIDMap m_mapTeamID2RgnID;
};



#endif//RGN_MANAGER_H