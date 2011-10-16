#include "StdAfx.h"
#include ".\goodsid.h"

CGoodsID::CGoodsID(void)
{
	for(int i=0;i<16;i++)
	{
		m_stGoodsID[i].bUsed=false;
		m_stGoodsID[i].dwGoodsID=GOODSID_ZONE_SIZE*i;
		m_stGoodsID[i].dwGameServerIndex=0;
	}
}
//
//CGoodsID::~CGoodsID(void)
//{
//}

CGoodsID *CGoodsID::instance = NULL;

CGoodsID* CGoodsID::GetInstance()
{
	if(!instance)
	{
		instance = new CGoodsID();
	}
	return instance;
}

void CGoodsID::Release()
{
	if(instance)
	{
		delete instance;
		instance = NULL;
	}
}

DWORD CGoodsID::GetGoodsID(int nIndex)
{
	if(nIndex>=0 && nIndex<16)
	{
		return m_stGoodsID[nIndex].dwGoodsID;
	}
	return 0;
}

void CGoodsID::SetGoodsID(int nIndex,DWORD dwVal, bool bUsed)
{
	if(nIndex>=0 && nIndex<16)
	{
		m_stGoodsID[nIndex].dwGoodsID=dwVal;
		m_stGoodsID[nIndex].bUsed = bUsed;
	}
}

// 设置物品ID所分配给的GameServer
void CGoodsID::SetGoodsIDGameServerIndex(int nIndex, DWORD dwGameServerIndex)
{
	if(nIndex>=0 && nIndex<16)
	{
		m_stGoodsID[nIndex].dwGameServerIndex = dwGameServerIndex;
	}
}

void CGoodsID::AddGoodsID(int nIndex)
{
	if(nIndex>=0 && nIndex<16)
	{
		m_stGoodsID[nIndex].dwGoodsID++;
	}
}

// 获取第一个可以分配的物品ID位置(除了0号区间),并作上标记
int CGoodsID::AssignFirstFreeGoodsID(DWORD dwGameServerIndex)
{
	for(int i=1;i<16;i++)
	{
		if(!m_stGoodsID[i].bUsed)
		{
			m_stGoodsID[i].bUsed = true;
			m_stGoodsID[i].dwGameServerIndex = dwGameServerIndex;
			return i;
		}
	}
	return 0;
}

// 释放一个指定的物品ID
void CGoodsID::FreeGoodsIDByGameServerIndex(DWORD dwGameServerIndex)
{
	for(int i=1;i<16;i++)
	{
		if (m_stGoodsID[i].dwGameServerIndex==dwGameServerIndex)
		{
			m_stGoodsID[i].bUsed=false;
			m_stGoodsID[i].dwGameServerIndex=0;
		}
	}
}
