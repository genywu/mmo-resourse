#pragma once
#ifndef _INC_GOODSID_H
#define _INC_GOODSID_H

class CGoodsID
{
private:
	CGoodsID(void);
	virtual ~CGoodsID(void);

public:
	static CGoodsID* GetInstance();
	static void Release();

private:
	static CGoodsID *instance;

public:
	// 物品起始ID信息
	struct tagGoodsID
	{
		bool	bUsed;				// 是否已经分配出去了
		DWORD	dwGoodsID;			// 起始ID
		DWORD	dwGameServerIndex;	// 分配给的GameServer
	};

public:
	static const DWORD GOODSID_ZONE_SIZE = 0x10000000;	// 物品区间大小(一共16个区间)

	/*static*/ DWORD GetGoodsID(int nIndex);											// 获取一个物品起始ID
	/*static*/ void SetGoodsID(int nIndex, DWORD dwVal, bool bUsed=false);				// 设置物品ID的起始值
	/*static*/ void SetGoodsIDGameServerIndex(int nIndex, DWORD dwGameServerIndex);	// 设置物品ID所分配给的GameServer
	/*static*/ void AddGoodsID(int nIndex);											// 添加物品的起始ID
	/*static*/ int AssignFirstFreeGoodsID(DWORD dwGameServerIndex);					// 获取第一个可以分配的物品ID位置(除了0号区间),并作上标记
	/*static*/ void FreeGoodsIDByGameServerIndex(DWORD dwGameServerIndex);				// 释放一个指定GameServerIndex的物品ID

public:
	/*static*/ tagGoodsID m_stGoodsID[16];							// 物品起始ID
};


#endif // _INC_GOODSID_H
