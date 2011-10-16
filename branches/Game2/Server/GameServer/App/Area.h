#pragma once
#include "baseobject.h"

extern long AREA_WIDTH;		// 区域的宽度
extern long AREA_HEIGHT;	// 区域的高度
const int AREA_HIBERNATE_DELAYTIME= 15000; //延迟休眠时间

//! 物品保护的拾取判断返回值
enum eGoodsPickValue
{
	eGPV_Cant			= 0,		//! 不能拾取
	eGPV_Allow_NoOwner	= 0xFF00,	//! 允许拾取，无主物品
	eGPV_Allow_IsYours,				//! 允许拾取，物品只属于被测试玩家
	eGPV_Allow_IsYourTeams,			//! 允许拾取，物品属于被测试玩家所在队伍
};


class CGoods;
class CShape;
class CArea :
	public CBaseObject
{
private:
	//物品保护和定时删除
	enum PROTECTION_LEVEL
	{
		PL_PLAYER,			// 玩家
		PL_TEAM,			// 组队
	};

	struct tagGoodsProtection
	{
		tagGoodsProtection(){lTimerID=-1;dwTimeStamp=0;}
		//定时器id
		long				lTimerID;
		//时间戳
		DWORD				dwTimeStamp;
		//物品保护类型（玩家， 队伍）
		PROTECTION_LEVEL	plLevel;
		//
		CGUID				guid;
	};

	long m_lX;		// 地图中的x坐标
	long m_lY;		// 地图中的y坐标

	hashsetGUID	m_setPlayers;
	hashsetGUID	m_setActivedMonsters;
	hashsetGUID	m_setSleepedMonsters;
	hashsetGUID	m_setGoods;	
	hashsetGUID	m_setPets;
	hashsetGUID	m_setBuild;
	hashsetGUID	m_setCollection;


	map<CGUID, tagGoodsProtection,guid_compare>	m_mGoodsProtection;
	typedef map<CGUID, tagGoodsProtection,guid_compare>::iterator itGoodsPro;
	//挂起
	int m_iPlayerAreaAround;
	BOOL m_bIsHangUp;
	long m_lTimerID;

public:
	long GetX()			{return m_lX;}
	long GetY()			{return m_lY;}
	void SetX(long l)	{m_lX=l;}
	void SetY(long l)	{m_lY=l;}

	CArea(void);
	virtual ~CArea(void);

	void				PlayerEnter		();
	void				PlayerLeave		();
	hashsetGUID&		GetAllPlayers	();

	void				WakeUpMonsters	();

	//##关于刷新怪物的时候
	void				OnRefreshMonster(long lRefeashIndex);

	virtual void RemoveObject(CBaseObject* pObject);			// 移出OBJECT
	virtual void AddObject(CBaseObject* pObject);				// 添加OBJECT

	//##关于物品保护的函数
	void SetGoodsProtection( CPlayer* pPlayer, CGoods* pGoods,BOOL bTeamProtected=TRUE);

	eGoodsPickValue CanPickUpGoods( CPlayer* pPlayer, CGoods* pGoods );

	//关于物品在区域定时删除
	//添加物品删除时间
	void	AddGoodsDelTime(CGoods *pGoods,tagGoodsProtection& pGoodsProtect);
	void	AddGoodsDelTime(CGoods *pGoods);
	//取消物品删除时间
	void	LogoutGoodsDelTime(CGoods* pGoods);
	//保护超时，执行删除
	void	OnGoodsDelTimeOut(stDestObj* pDestObj);

	//##查找对象的函数
	BOOL FindShapes(long lType,hashsetGUID& setOut);
	BOOL FindShapes			( LONG lType, vector<CShape*>& vOut );
	BOOL GetAllShapes		( vector<CShape*>& vOut );

	//##
	DWORD GetNumShapes		();

	//超时调用
	virtual long OnTimeOut(DWORD timerid,DWORD curtime,const void* var);
	//注册,删除，注销 事件
	BOOL AddEvent(DWORD timerid,DWORD timer);
	BOOL DelEvent();
	BOOL LogoutEvent(long& timerid);
	//休眠通知
	void HibernateCall				();
	//唤醒通知
	void WakeUpCall					();
	//休眠本区域
	void Hibernate					();
	//唤醒本区域
	void WakeUp						();
	//是否休眠
	BOOL IsHibernate				(){return m_bIsHangUp;}
};
