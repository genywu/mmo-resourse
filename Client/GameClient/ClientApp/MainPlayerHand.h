#pragma once
#include "../ClientApp/player.h"
#include "../ClientApp/Goods/goods.h"
#include "../ClientApp/other/piclist.h"
class CMainPlayerHand
{
public:

	// 事件发生地种类
	enum PlaceType
	{
		PlaceType_BackpacksPage = 0,		//背包（包括子背包）
		PlaceType_EquipmentPage,			//装备栏
		PlaceType_Fastfield,				//MainBar快捷栏
		PlaceType_NpcShopPage,				//NPC商店
		PlaceType_ExchangePage,				//交易界面
		PlaceType_PlayerShopPage,			//个人商店
		PlaceType_CardMutifunctionalPage,	//卡片镶嵌界面
		PlaceType_PetSkillPage,				//宠物界面中技能界面
		PlaceType_PetStrenthenPage,			//宠物强化界面
		PlaceType_Num
	};

	// （刘科08.11.19）
	struct tagSkillEffect
	{
		CGUID gEffectID;				// 手上特效的GUID
		DWORD dwSkillID;				// 对应的技能ID
		tagSkillEffect()
		{
			gEffectID = CGUID::GUID_INVALID;
			dwSkillID = 0;
		}
	};

	struct tagHand 
	{
		DWORD dwSourcePlace;			//手被设置时的触发地点
		DWORD dwObjectTypeOnHand;		//手上所显示的目标的类型
		DWORD dwGoodsIndex;				//要显示的物品图标ID（此时手上只是一个图标，并没有存储实际的物品数据）
		DWORD dwSkillIndex;				//手上所拿技能的编号
		WORD  wIconType;				//图标的类型
		DWORD dwPetPicID;				// 宠物图档ID
		CPet* handPet;					// 宠物对象指针
		CPlayer::tagGoods HandGoods;	//手上所拿物品的指针
		tagSkillEffect	stSkillEffect;	// 手上范围特效信息
		tagHand()
		{
			dwSourcePlace = PlaceType_Num;
			wIconType = -1;
			dwGoodsIndex = 0;
			dwObjectTypeOnHand = 0;
			dwSkillIndex = 0;
		}
	};
	CMainPlayerHand();
	
	// 获得手
	tagHand* GetMainPlayerHand() {return &m_Hand;}
	
	// 获得手上所拿物体的类型
	DWORD	 GetObjectTypeOfMainPlayerHand()	{return m_Hand.dwObjectTypeOnHand;}
	
	// 获得手上所拿物品
	CPlayer::tagGoods* GetTGoodsOfMainPlayerHand()	{return &m_Hand.HandGoods;}
	
	// 获得手上所拿物品的pGoods
	CGoods*	 GetPGoodsOfMainPlayerHand()	{return m_Hand.HandGoods.pGoods;}
	
	// 获得手上所拿技能的Index
	DWORD	 GetSkillOfMainPlayerHand()		{return m_Hand.dwSkillIndex;}

	// 获取手上所拿宠物图档ID
	DWORD GetPetPicIDOfMainPlayerHand()		{ return m_Hand.dwPetPicID; }

	// 获取手上所拿宠物对象指针
	CPet* GetPetOfMainPlayerHand()			{ return m_Hand.handPet; }

	// 获得手上显示的物品的Index
	DWORD	 GetGoodsIndexOfMainPlayerHand()	{return m_Hand.dwGoodsIndex;}

	// 获取当前手被设置时的触发地点类型
	DWORD	 GetSourcePlaceTypeOfMainPlayerHand() {return m_Hand.dwSourcePlace;}
	
	// 设置手
	void SetMainPlayerHand(tagHand* pTagGoods) {m_Hand = *pTagGoods;}
	
	// 设置手上显示的物品的Index(没有实际的物品信息),第二个参数记录设置手时的触发地点
	void SetMainPlayerHandByGoodsIndex(DWORD dwGoodsIndex,DWORD dwSourcePlace = PlaceType_Num);
	
	// 设置手上拿的物品
	void SetMainPlayerHandByGoods(bool bChanged,long lNum,DWORD lPos,CGoods *pGood,DWORD lType = 0);
	
	// 设置手上拿的技能
	void SetMainPlayerHandBySkill(DWORD dwSkillIndex,WORD wIconType=CPicList::PT_SKILL_ICON,DWORD dwSourcePlace = PlaceType_Num);

	// 设置手上拿的宠物
	void SetMainPlayerHandByPet(DWORD dwPetPicID, CPet* pet,WORD wIconType=CPicList::PT_PET_ICON, DWORD dwSourcePlace = PlaceType_Num);
	
	// 清空手上所拿物体或者图标
	void ClearMainPlayerHand();

	// 设置手所操作的物品将去的目标地的lExtendID
	void SetDesExtendIDofGoods(long lExtendID) {m_lDesExtenID = lExtendID;}
	
	// 获得手所操作的物品将去的目标地的lExtendID
	long GetDesExtendIDofGoods()	{return m_lDesExtenID;}
	
	// 清除手所操作的物品的阴影状态
	void DelShadowStateOfGoods();
	
	// 手的显示
	bool Display(long lGraphicsID,long x, long y, DWORD color = 0xffffffff, RECT *rectClip=NULL);
	
	// 手放置物品时发送的消息（目前只对手上的物品操作有效）
	void SendPutDownMes(long SorceType/*起始点类型*/,const CGUID& SorceGUID/*起始点的GUID*/,long lSorceObjectExtendID/*起始点容器ID*/,long lSorceObjectPos/*起始点位置*/,
						long DesType/*目的点类型*/,const CGUID& DesGUID/*目的点GUID*/,long lDesObjectExtendID/*目的点容器ID*/,long lDesObjectPos/*目的点位置*/,
						long lObjectType/*目标类型*/,const CGUID& ObjectGUID/*目标GUID*/,long lObjectNum/*目标数量*/);

	/*
	 * 鼠标位置的特效显示相关方法
	 * 08.11.19刘科添加
	 */
	// 设置特效
	void SetMainPlayerHandByEffect(DWORD dwSkillID,DWORD dwEffectID);
	// 修正手上特效的绘制位置
	bool DisplayEffectInHand();
	// 获得手上范围技能
	DWORD GetAreaSkillID()	{return m_Hand.stSkillEffect.dwSkillID;}
private:
	tagHand m_Hand;
	long m_lDesExtenID;
	//long m_lDesPos;
};