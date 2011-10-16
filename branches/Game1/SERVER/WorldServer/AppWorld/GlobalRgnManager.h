//GlobalRgnManager.h/////////////////////////////////////////////////////////////////////
//对象类:该对象管理所有的场景对象实例
//Author:安海川
//Create Time:2008.11.03
#ifndef GLOBAL_RGN_MANAGER
#define GLOBAL_RGN_MANAGER
#include <map>
#include "..\public\BaseDef.h"
#include "../public/GUID.h"
#include "../public/msgtype.h"
#include "../public/DBDefine.h"

class CWorldRegion;

class CGlobalRgnManager
{
public:

	//使用Region GUID查找
	typedef std::map<CGUID/*RgnID*/, CWorldRegion*> MapRgn;
	typedef MapRgn::iterator MapRgnItr;

	CGlobalRgnManager();
	~CGlobalRgnManager();

	//! 运行接口
	void Run(void);
	//! 读取Ini配置文件数据,将场景放入模板管理map
	bool LoadRgnSetupIni(const char* strFilePath);
	// 读取副本创建优先服务器配置文件
	void LoadDupRgnServerIni(const char* filePath);
	// 读取RegionPlugSetup数据
	BOOL LoadRegionPlugSetupIni(const char* szPath);
	//! 创建指定GS的Rgn
	bool CreateGSRgn(long templateRgnID, eRgnType rgnType, const CGUID& ownerID, const CGUID& scriptID/*挂起的脚本ID*/, long pMsgGSID, const char* szVarName, const char* szRgnIDName, bool isDupRgnDataFlag);
	//! 删除指定GS的Rgn
	bool DeleteGSRgn(eRgnType rgnType, const CGUID& ownerID, const CGUID& rgnID, const CGUID& scriptID/*脚本ID*/, long pMsgGSID, const char* szVarName, const char* szRgnIDName);
	//! 查找Region
	CWorldRegion* FindRgnByGUID(const CGUID& guid);
	//! 组装分线场景数据
	bool AddLinedRgnInfoToByteArray(vector<BYTE>& pBA);
	//! 计算当前GS该类型副本是否达到上限,返回有效GS的gsid
	long GetCurGsDupRgnNumIsMax(eRgnType rgnType);

	// 玩家副本数据操作接口
	//!					添加一个玩家队伍副本GUID
	void				AddTeamRgnGUID(const CGUID& ownerGuid, LONG lTemplateRgnID, const CGUID& rgnGuid, BOOL bCover);
	//!					删除一个玩家队伍副本GUID
	void				DelTeamRgnGUID(const CGUID& ownerGuid, LONG lTemplateRgnID);
	void				DelTeamRgnGUID(const CGUID& ownerGuid, const CGUID& rgnGuid);
	//!					清除一个玩家的队伍副本
	void				ClearTeamRgnGUID(const CGUID& ownerGuid);
	//!					得到一个队伍副本的GUID
	const	CGUID&		GetTeamRgnGUID(const CGUID& ownerGuid, LONG lTemplateRgnID);
	//!					通过顺序得到一个玩家的队伍的副本GUID
	const	CGUID&		GetTeamRgnGuidByIndex(const CGUID& ownerGuid, LONG lIndex);
	//!					通过顺序得到一个玩家的队伍的副本模板ID
	LONG				GetTeamRgnTemplateIdByIndex(const CGUID& ownerGuid, LONG lIndex);
	//!					得到玩家的队伍副本GUID列表
	map<LONG, CGUID>*	GetTeamRgnGuidMap(const CGUID& ownerGuid);//{return m_mapTeamRgnGUID;}
	//!					玩家的队伍副本GUID编码
	VOID				RgnGuidCode(const CGUID& ownerGuid, vector<BYTE> &vData);
	//!					玩家的队伍副本GUID解码
	VOID				RgnGuidDecode(const CGUID& ownerGuid, BYTE *pData, LONG &lPos);
	//!					解码交换玩家的队伍副本（不覆盖相同模板的副本）
	VOID				RgnGuidSwapFromArray(const CGUID& ownerGuid, BYTE *pData, LONG &lPos);

	//!					玩家的队伍副本GUID编码
	VOID				RgnGuidCode(const CGUID& ownerGuid, DBWriteSet& setWriteDB);
	//!					玩家的队伍副本GUID解码
	VOID				RgnGuidDecode(const CGUID& ownerGuid, DBReadSet& setReadDB);
	//!					解码交换玩家的队伍副本（不覆盖相同模板的副本）
	VOID				RgnGuidSwapFromArray(const CGUID& ownerGuid, DBReadSet& setReadDB);


	//!					得到玩家的个人副本GUID列表
	map<CGUID, CGUID>&	GetPersonalRgnGuidMap(const CGUID& ownerGuid);
	void				AddPersonalRgnGUID(const CGUID& ownerGuid, const CGUID& rgnGuid);
	//!					删除一个玩家个人副本GUID
	void				DelPersonalRgnGUID(const CGUID& ownerGuid);
	//!					得到一个个人副本的GUID
	const	CGUID&		GetPersonalRgnGUID(const CGUID& ownerGuid);
public:
	// 场景分配信息
	struct tagRegion
	{
		CWorldRegion	*pRegion;			// 场景指针
		DWORD			dwGameServerIndex;	// 所在Gameserver编号
		eRgnType		RegionType;			// 场景类型
	};

public:
	// 根据地图ID得到tagRegion指针
	tagRegion* GetTemplateNormalRegion(long lID/*模板ID*/)				{return (s_mapRegionList.find(lID)!=s_mapRegionList.end())?&s_mapRegionList[lID]:NULL;}
	//根据地图名字得到tagRegion指针
	tagRegion* GetTemplateNormalRegion(const char* strName);
	// 根据地图ID得到tagRegion指针
	tagRegion* GetTemplatePersonalRegion(long lID/*模板ID*/)		{return (s_mapPersonalRgnList.find(lID)!=s_mapPersonalRgnList.end())?&s_mapPersonalRgnList[lID]:NULL;}
	// 根据地图ID得到tagRegion指针
	tagRegion* GetTemplatePersonalHouseRegion(long lID/*模板ID*/)	{return (s_mapPersonalHouseRgnList.find(lID)!=s_mapPersonalHouseRgnList.end())?&s_mapPersonalHouseRgnList[lID]:NULL;}
	// 根据地图ID得到tagRegion指针
	tagRegion* GetTemplateTeamRegion(long lID/*模板ID*/)		{return (s_mapTeamRgnList.find(lID)!=s_mapTeamRgnList.end())?&s_mapTeamRgnList[lID]:NULL;}

	// 取得Normal Rgn模板场景列表
	map<long, tagRegion>* GetTemplateRegionListMap()	{return &s_mapRegionList;}
	// 取得Personal Rgn场景列表
	map<long, tagRegion>* GetTemplatePersonalRgnListMap()	{return &s_mapPersonalRgnList;}
	// 取得Personal House Rgn场景列表
	map<long, tagRegion>* GetTemplatePersonalHouseRgnListMap()	{return &s_mapPersonalHouseRgnList;}
	// 取得Team Rgn场景列表
	map<long, tagRegion>* GetTemplateTeamRgnListMap()	{return &s_mapTeamRgnList;}

	MapRgn& GetRgnMap() { return m_mapRgn; };
	
	//WorldServer使用,从原始场景和副本场景中随机得到一场景
	long GetRandomRegion(long RegionID);

	CWorldRegion* GetWorldRegionByRgnId(long lId);

	//! 查找GSID by RgnGUID
	long FindGSIDByRgnID(const CGUID& ownerID);

	// 个人房屋副本创建优先服务器
	vector<long>&	GetFirstPersonalHouseServer(void) { return m_vecFirstPersonalHouseServer; }
	// 个人副本创建优先服务器
	vector<long>&	GetFirstPersonalServer(void) { return m_vecFirstPersonalServer; }
	// 组队副本创建优先服务器
	vector<long>&	GetFirstTeamServer(void) { return m_vecFirstTeamServer; }

	//! 创建Rgn,向GS发送RGN_CREATE消息
	CWorldRegion* CreateRgn(bool isDupRgnDataFlag, long scrGSID/*发送创建请求的GSID*/, long templateRgnID/*模板Region的ID*/, eRgnType rgnType, long GSID, 
		const CGUID& ownerID=NULL_GUID/*NORMAL类型则ID为空*/, const CGUID& scriptID=NULL_GUID, const CGUID& dbRgnGUID=NULL_GUID, 
		const char* szRetVarName=NULL, const char* szRetRgnIDName=NULL/*异步返回值变量名*/);
	//! 删除Rgn,向GS发送RGN_DELETE消息
	void DeleteRgn(eRgnType rgnType, const CGUID& ownerID, const CGUID& rgnID, const CGUID& scriptID=NULL_GUID, const char* szRetVarName=NULL, const char* szRetRgnIDName=NULL/*异步返回值变量名*/);


	// 发送数据库删除DBDupRgn消息
	void SendDelPHDupRgnMsgToDB(const CGUID& rgnGUID);

	// 分线场景数据
	map<long/*分线ID*/, vector<long/*templateID*/>>& GetLinedRgnIdByLinedIdMap(void) { return m_mapLinedRgnIdByLinedId; }
private:
	// 场景列表<id, tagRegion>, 保存从配置文件读取的场景数据对象
	map<long, tagRegion>			s_mapRegionList;
	map<long, tagRegion>			s_mapPersonalRgnList;
	map<long, tagRegion>			s_mapTeamRgnList;
	map<long, tagRegion>			s_mapPersonalHouseRgnList;

	// 分线场景数据
	map<long/*分线ID*/, vector<long/*templateID*/>> m_mapLinedRgnIdByLinedId;
private:

	map<CGUID/*ownerID*/, map<LONG/*templateID*/, CGUID/*rgnGuid*/>> m_mapTeamRgnGUID;
	map<CGUID/*ownerID*/, CGUID/*rgnGuid*/> m_mapPersonalRgnGUID;

	MapRgn m_mapRgn;

	// 个人房屋副本创建优先服务器
	vector<long>	m_vecFirstPersonalHouseServer;
	// 个人副本创建优先服务器
	vector<long>	m_vecFirstPersonalServer;
	// 组队副本创建优先服务器
	vector<long>	m_vecFirstTeamServer;
};

#endif//GLOBAL_RGN_MANAGER