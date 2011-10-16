///////////////////////////////////////////////////////////////
//worldregion.h///////////////////////////////////////////////
//攻城战地图类，负责攻城战的处理
//Author：Joe
//Create Date:2004.7.19
#pragma once
#include "worldwarregion.h"

//当前城市的状态

class CWorldCityRegion :
	public CWorldWarRegion
{
public:
	CWorldCityRegion(void);
	~CWorldCityRegion(void);

	//Build
	struct tagBuild
	{
		long lID;				// 城门的编号
		long lPicID;			// 图片ID
		long lDir;				// 方向
		long lAction;			// 初始Action
		long lMaxHP;			// 最大生命上限
		long lDef;				// 防御力
		long lElementResistant;	//元素抗性
		long lTitleX;			// 坐标
		long lTitleY;
		long lWidthInc;			// 体型
		long lHeightInc;
		
		string strName;			// 名字
		string strScript;		// 击毁执行脚本
	};


private:
	list<tagBuild>		m_CityGates;				//城门
	tagRegionSetup		m_DefenceSideRS;			//防守方复活区域
//////////////////////////////////////////////////////////////////////////
//	接口
//////////////////////////////////////////////////////////////////////////
public:
	BOOL Load();	// 读取
	BOOL Save();	// 读取

	BOOL LoadCitySetup();//读取城市配置
	virtual void SetEnterPosXY(CPlayer* pPlayer);

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
};
