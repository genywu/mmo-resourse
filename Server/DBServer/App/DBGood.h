#ifndef DB_GOOD_H
#define DB_GOOD_H
#include "BaseDBEntity.h"
#include <list>
#include "..\..\..\public\GUID.h"
#include "../public/DBDefine.h"

using namespace std;


class CDBGoods : public CBaseDBEntity
{
public:
	//物品类别
	enum DB_GOODS_TYPE
	{
		//不能用
		GT_USELESS,
		//消耗品
		GT_CONSUMABLE,
		//装备
		GT_EQUIPMENT,
		//背包
		GT_PACK
	};

	enum GOODS_PLACE
	{		
		PLACE_PACKET					= 1,	//普通背包
		PLACE_EQUIP,//装备
		//PLACE_HAND,//手上
		PLACE_WALLET,//钱包
		PLACE_SILVERWALLET,//银币
		PLACE_GEM, //! 宝石
		PLACE_YUANBAO,//元宝
		PLACE_JIFEN,//积分
		PLACE_EQUIPMENT_UPGRADING,			//装备升级
		//PLACE_BANK,//银行
		PLACE_DEPOT, //仓库
		PEI_ENHANCEMENT,//

		PLACE_PACK =10000,
		PLACE_PACK1=10001,
		PLACE_PACK2=10002,
		PLACE_PACK3=10003,
		PLACE_PACK4=10004,
		PLACE_PACK5=10005
	};

private:
	CDBGoods();
public:
	CDBGoods(const CGUID& guid);
	virtual ~CDBGoods();

	void SetID(long id) { m_ID = id; }
	long GetID(void)    { return m_ID; }

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	// Get Interfaces
	const char*	GetSZID(void)				{ return &m_szID[0];}
	const char*	GetGoodsID(void)		{ return &m_szGoodsID[0];}
	
	//const CGUID& GetExID(void)			{ return m_guExID;}
	long GetGoodsType(void)	{ return m_GoodsType;}
	// Set Interfaces
	void SetSZID					(const char* buf, long size)		{ memcpy(m_szID, buf, strlen(buf));}
	void SetGoodsID				(const char* buf, long size)		{ memcpy(m_szGoodsID, buf, strlen(buf));}
	
	//void SetExID				(CGUID& guid)			{ m_guExID = guid;}
	void SetGoodsType			(long type)	{ m_GoodsType = type;}

	
	// 从Player消息中读取数据
	void ReadDataFromMsgForPlayer(BYTE* msgBuf, long& msgBufPos);

	void AddEntityDataToMsg(DB_OPERATION_TYPE opType,vector<BYTE>& pBA);

	CDBGoods& operator=(CDBGoods& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{
			memcpy(m_szID, right.m_szID, 64);
			memcpy(m_szGoodsID, right.m_szGoodsID, 64);
			m_lBasePropertiesIndex = right.m_lBasePropertiesIndex;
			m_GoodsType = right.m_GoodsType;
			m_ID = right.m_ID;
			b_IsFirstCreated = right.b_IsFirstCreated;
		}

		return *this;
	}

	// 是否是刚创建还未被Load/Save线程处理过的对象
	bool	GetIsFirstCreated(void) { return b_IsFirstCreated; }
	void	SetIsFirstCreated(bool flag) { b_IsFirstCreated = flag; }

	// 初始化属性配置
	void				InitProperty();

	// 检查更新属性里是否有指定表名的属性名
	bool				FindTableUpdateFlag(const string& tableName);

private:
	// 物品数据库编号
	char	m_szID[64];
	char	m_szGoodsID[64];
	long    m_lBasePropertiesIndex;
	long	m_GoodsType;

	long m_ID;

	bool b_IsFirstCreated;
};

#endif//DB_GOOD_H