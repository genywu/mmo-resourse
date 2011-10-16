////////////////////////////////////////////////////////////////////////////
//CollectionGoods.h///////////////////////////////////////////////////////////////////
//采集类，此物体无生命和防御
//Author:肖杨
//Create Time:2008.2.3
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "MoveShape.h"
#include "../../../Public/Common/DeputyOccu.h"
class CCollectionGoods :
	public CMoveShape
{
public:
	CCollectionGoods();
	~CCollectionGoods(){}

private:
	long	m_lCreatorType;		// 创建关联者的类型
	CGUID	m_CreatorID;		// 创建关联者的GUID
	//DWORD m_dwCollectionGoodsID;		//采集物的ID
	//bool  m_bCanBeCollect;			//采集物是否可被采集的标志
	//string m_strCollectionGoodsShowName;//采集物的显示名字
	//string m_strCollectionGoodsOriName;	//采集物的原始名字
	//eDeputyOccu		m_eOccuID;			//副职业id
	//DWORD			m_dwPicID;			//图形id
	//DWORD			m_dwRadarID;		//小地图标志(雷达id)
	//DWORD			m_dwMixTimes;		//最小采集次数
	//DWORD			m_dwMaxTimes;		//最大采集次数
	//DWORD			m_dwCollectLevel;	//采集等级
	//DWORD			m_dwRoleID;			//需要任务等级
	//DWORD			m_dwRoleStep;		//需要任务步骤

public:
	// 设置采集物的显示模型的种类
	//void SetModelLevel();

	//初始化显示模型
	void SetDisplayModelGroup(bool bAsyn = true);

	//设置采集物的状态
	void SetAction(WORD l);

	//采集物的显示
	bool Display(void);

	//采集物的AI
	void AI();
	//long AI_Stand();			// 站AI
	//long AI_BeAttack();			// 被攻擊AI
	//long AI_Died();				// 死亡AI

	//切换采集物的状态
	void OnActionChange();

	//采集物解码
	bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);

	//获取物品的显示名字
	string GetColletctionGoodsShowName();

	//获取物品的原始名
	string GetCollectionGoodsOriName();

	//获取副职业的ID
	DWORD  GetOccuID();

	//获取采集物显示模型的ID
	DWORD  GetPicID();

	//获取雷达地图的ID
	DWORD  GetRadarID();

	//获取最小可被采集的次数
	DWORD  GetMixTimes();

	//获取最大可被采集的次数
	DWORD  GetMaxTimes();

	//获取采集物要求的采集等级
	DWORD  GetCollectLevel();

	//获取任务ID
	int  GetRoleID();

	//获取任务阶段
	DWORD  GetRoleStep();

	///获取采集此采集物时使用的动作ID
	DWORD  GetActionID();

	//获取任务ID列表
	const vector<tagQuestGoods>* GetRoleInfo();

	//typedef vector<tagQuestGoods>::iterator QUESTGOODS_ITER;

	//获取采集物的被采集的条件列表
	const list<tgConditionGoods>* GetConditionList();

	// 获得Monster创建者的类型和ID
	long GetCreatorType()		{return m_lCreatorType;}
	CGUID GetCreatorID()		{return m_CreatorID;}
	// 设置Monster创建者的类型和ID
	void SetCreatorType(long type)		{m_lCreatorType = type;}
	void SetCreatorID(CGUID guid)		{m_CreatorID = guid;}
};