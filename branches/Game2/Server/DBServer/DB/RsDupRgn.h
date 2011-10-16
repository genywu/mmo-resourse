#pragma once
#include "..\myadobase.h"
#include "../public/GUID.h"
#include <map>
//#include "..\..\server\dbserver\appworld\DBCityWar.h"
class CBaseDBEntity;
class CDBDupRgn;
class CDBRgnGoods;
class CDBRgnGoodsGroup;

class CRsDupRgn :
	public CMyAdoBase
{
public:
	CRsDupRgn(void);
	~CRsDupRgn(void);

public:
	bool	LoadAll(_ConnectionPtr& cn);							//装载所有的敌对帮会关系
	bool	SaveAll(void);//保存所有的敌对帮会关系

	// GS查询指定玩家的个人房屋副本GUID
	void	QuestPHGuidByPlayerID(const char* playerName, CGUID& retPHGUID);

	//! 读取个人房屋信息
	void    LoadDupRgn(CDBDupRgn* cw, _ConnectionPtr& cn);
	//! 保存个人房屋信息
	void    SaveDupRgn(CDBDupRgn* cw, _ConnectionPtr& cn);
	//! 删除个人房屋信息
	void    DeleteDupRgn(CDBDupRgn* cw);

	//! 读取个人房屋内的物品信息
	bool    LoadDupRgnObj(const CGUID& rgnGUID, CDBRgnGoodsGroup* goodsGroup, _ConnectionPtr& cn);
	//! 保存个人房屋内的物品信息
	static bool    SaveDupRgnObj(const CGUID& rgnGUID, map<CGUID,CDBRgnGoods*>& goodsGroup, _ConnectionPtr& cn);

	//! 删除个人房屋内的物品信息
	void    DeleteDupRgnObj(const CGUID& rgnGUID);

	//存Buffer
	static bool SaveBufferField(CBaseDBEntity *pGoods, const char* szFiledName, _RecordsetPtr &pRs);
	//读Buffer
	bool LoadBufferField(CBaseDBEntity *pGoods, const char* szFiledName, _RecordsetPtr &pRs);
};
