//////////////////////////////////////////////////////////////////////////
//worldwarregion.h/
//战斗争夺场景的基类
//Author：Joe
//Create Date:2005.7.5
//////////////////////////////////////////////////////////////////////////


#pragma once
#include "worldregion.h"


class CWorldWarRegion :
	public CWorldRegion
{
public:
	CWorldWarRegion(void);
	~CWorldWarRegion(void);
protected:

	long	m_lSymbolTotalNum;					//标志物总数量
	long	m_lWinVicSymbolNum;					//获胜夺取标志物的数量
	long	m_lVicSymbolNum;					//坚持到最后获胜的标志物数量
	
	//////////////////////////////////////////////////////////////////////////
	//	接口
	//////////////////////////////////////////////////////////////////////////
public:
	BOOL Load();	// 读取	
	BOOL Save();	// 读取

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
};
