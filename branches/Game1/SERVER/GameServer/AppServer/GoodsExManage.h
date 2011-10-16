/*
*	file:		GoodsExManage.h
*	Brief:		物品扩展功能（分解/合成、武器充能、锁定/冻结）管理类
*	detail:		本类基本数据不多，主要是对其他模块的使用进行的封装；
*				对象按功能分为以下几个部分：
*				——基本成员（配置信息）
*				——物品分解
*				——物品合成
*				——武器充能
*	Authtor:	张科智
*	Datae:		2007-12-11
*/

#include "..\..\..\public\GoodsExSetup.h"

class CGoodsContainer;
class CPlayer;
class CGoods;


class GoodsExManage
{
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 基本成员（定义文件：GoodsExManage_base.cpp）
	///////////////////////////////////////////////////////////////////////////////////////////////////
public:
	GoodsExManage(void);
	~GoodsExManage(void);

public:
	//!								得到静态实例
	static		GoodsExManage&		GetInstance(void);

private:
	//!								静态实例
	static		GoodsExManage		c_GoodsExManage;


public:
	//!								通过消息获取配置信息，消息类型：MSG_W2S_SERVER_SENDSETUP、数据编号：SI_GOODSEX
	bool							InitSetup(BYTE *pData, long &lPos);

	//!
	GoodsExSetup&					GetGoodsExSetup(void){return m_GoodsExSetup;}

	//!	玩家物品扩展功能会话
	struct tagGoodsExWndSession
	{
		ULONG	uWndType;	//! 会话窗口类型
		ULONG	uRadius;	//! 会话时允许的玩家活动半径
		long	lPosX;		//! 活动范围中心X坐标
		long	lPosY;		//! 活动范围中心Y坐标
		CGUID	NpcGuid;	//! 与之对话的NPC的guid
	};

	//！							打开客户端物品扩展操作窗口（同时只能打开一种类型窗口）
	bool							OpenGoodsExWindow(CPlayer *pPlayer, tagGoodsExWndSession& GoodsExWndSession);

	//!								关闭客户端物品扩展操作窗口（清除相关会话）
	void							CloseGoodsExWindow(CPlayer *pPlayer);

	//！							响应客户端关闭物品扩展窗口
	void							OnClientCloseGoodsExWindow(CMessage *pMsg);

	//!								为玩家加锁或者解锁物品
	BOOL							LockGoods(CPlayer *pPlayer, const CGUID& GoodsGuid, BOOL bLock);

private:
	//!								验证消息会话存在
	bool							TestSession(const CGUID& guid, eGoodsExWindowType eGoodsWinType);
	//!								在玩家身上找到一个物品容器（限制为主背包和5个子背包）
	CGoodsContainer*				GetGoodsContainer(CPlayer *pPlayer, long lContianerId);


private:
	
	//!										玩家打开的窗口map<玩家ID，pair<窗口类型，会话距离>>
	map<CGUID, tagGoodsExWndSession>		m_mapOpendGEWnd;

	

	
	//!-----------------------------------------------------------------------------------------
	//! 物品操作方法描述
	struct tagGoodsOperate
	{
		long lContianerPos;	//! 目的容器枚举
		long lGoodsPos;		//! 具体位置
		long lOperateWay;	//!	操作方法（CS2CContainerObjectMove::OPERATION_TYPE）
		long lOperateNum;	//! 操作数量
		CGoods *pGoods;
		long lReserved;		//! 保留字段，内部使用
		tagGoodsOperate(void){ memset(this, 0, sizeof(tagGoodsOperate)); }
	};

	//!		在玩家身上指定的位置找到一个物品（只能指定主背包和5个子背包）
	CGoods* FindGoodsFromPlayer(CPlayer *pPlayer, long lContianerID, long lPos);

	//!		测试一个物品是否在指定的位置上
	CGoods*	TestGoodsHere(CPlayer *pPlayer, const CGUID &GoodsGuid, long lContianerID, long lPos, BOOL bTestFrost = FALSE);

	//!		测试物品放入空间是否足够,返回剩余空间格数，如果为负，则说明空间不够
	long	TestPutWay(CPlayer *pPlayer, vector<tagGoodsOperate> &vecDelGoods, vector<CGoods*> &vPutGoods); 

	//!				给玩家添加物品
	bool	AddGoods(CPlayer *pPlayer, vector<CGoods*>& vAddGoods, vector<long>& vOptType, vector<long>& vOptNum);

	//!				删除玩家物品
	bool	DelGoods(CPlayer *pPlayer, vector<tagGoodsOperate> &vDelGoods, DWORD dwLogType, const CGUID &optGuid);
	//!-----------------------------------------------------------------------------------------


private:
	//!								配置管理对象
	GoodsExSetup					m_GoodsExSetup;

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 物品分解（定义文件：GoodsExManage_Disassemble.cpp）
	///////////////////////////////////////////////////////////////////////////////////////////////////
public:
	//!				响应物品分解请求
	void			OnGoodsDisassemble(CMessage *pMsg);

private:

	struct tagReValue
	{
		long lGoodsIndex;
		long lGoodsNum;
	};
	//!				得到一个物品的分解物类型ID和数量，返回产品种类的数量
	long			GetGoodsDisassembleResult(long lGoodsID,  tagReValue arrReValue[MAX_DISASSEMBLE_NUM]);

	





	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 物品合成（定义文件：GoodsExManage_Synthesize.cpp）
	///////////////////////////////////////////////////////////////////////////////////////////////////
public:
	//!				响应物品合成请求
	void			OnGoodsSynthesize(CMessage *pMsg);


private:





	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 武器充能
	///////////////////////////////////////////////////////////////////////////////////////////////////
public:
	//!				补充能量物品作用到玩家
	//!				物品合法性、使用后的物品删除，均在外部执行
	bool			ReinforcePower(CGoods *pArm, CGoods *pGoods, CPlayer *pPlayer);

	//!				开始玩家的武器加成效果（一个技能动作只能调用一次接口，效果在返回值中）
	bool			ArmReinforceBegin(CPlayer *pPlayer, CGoods *pArm, bool isMagic, bool isAttack, pair<float, float>&);

	//!				判断是否还可以减能量
	long			CanExpendPower(CGoods *pArm, BOOL isMagic);

	//!				根据输入的条件得到一个配置的加成值
	float			GetReinforceByEnum(long lAimLevel, long lPowerIntensity, long eValuePos);

	//!				得到一次消耗的武器的能量
	long			GetExpendPower(CGoods *pArm, BOOL isMagic);

private:
	//!
	GoodsExSetup::tagAmendCoefficient	m_Immobile;



	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 绑定、冻结
	///////////////////////////////////////////////////////////////////////////////////////////////////
public:

	//! 绑定
	//!------------------------------------------------------------------

	//!				玩家设置物品的绑定属性，玩家只能在eBT_Untie和eBT_Bind之间转换
	//!				关键返回值：
	//!					0：成功
	//!					30：操作值于当前值相同
	long			SetGoodsBindProperty_Player(CGoods *pGoods, long lBindType, const CGUID& playerGuid);

	//!				为脚本提供的绑定函数
	//！			参数1：物品拥有者
	//!				参数2、3：物品所在包裹位置（只能是主背包和5个子背包）
	//!				参数4：0=解绑定、1=绑定。其他值非法
	//!				返回值：0=失败；1=成功；
	long			SetGoodsBindProperty_Script(CPlayer *pPlayer, long lContianerID, long lPos, long lBindType);

	//!				系统设置物品的绑定属性
	long			SetGoodsBindProperty_System(CGoods *pGoods, long lBindType, const CGUID& playerGuid);

	//!				检测物品是否绑定
	bool			IsBind(CGoods *pGoods);
	

	//! 绑定
	//!------------------------------------------------------------------
	//!				玩家设置物品的冻结属性
	//!				关键返回值：
	//!					0：成功
	//!					30：操作值于当前值相同
	//!					40：解冻中，7天时间未到
	long			SetGoodsFrostProperty_Player(CGoods *pGoods, long lFrostType, const CGUID& playerGuid);

	//!				为脚本提供的冻结函数
	//！			参数1：物品拥有者
	//!				参数2、3：物品所在包裹位置（只能是主背包和5个子背包）
	//!				参数4：0=解冻、1=冻结。其他值非法
	//!				返回值：0=失败；1=成功；
	long			SetGoodsFrostProperty_Script(CPlayer *pPlayer, long lContianerID, long lPos, long lBindType);

	//!				检测物品是否冻结
	//!				如果物品解冻时间已到，触发解冻属性的设置
	bool			IsFrost(CGoods *pGoods);


	//! 响应消息
	//!------------------------------------------------------------------
	//!				响应物品绑定
	void			OnGoodsBind(CMessage *pMsg);
	//!				响应物品冻结
	void			OnGoodsFrost(CMessage *pMsg);

private:

	//!				真正的消息响应在此执行函数
	void			BindAndFrost(bool isBind, CMessage *pMsg);		


	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 物品升级（冲等）
	///////////////////////////////////////////////////////////////////////////////////////////////////
public:
	//!				响应装备升级预览
	void			OnEquipmentUpgradeView(CMessage *pMsg);
	//!				响应装备升级
	void			OnEquipmentUpgrade(CMessage *pMsg);
	//!				播放一个玩家的升级物品特效
	void			PlayUpgradeEffect(CPlayer *pPlayer, LONG lEffectID);


	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 物品魔化
	///////////////////////////////////////////////////////////////////////////////////////////////////
public:
	//!									响应装备魔化
	void								OnEquipmentPlugEvil(CMessage *pMsg);
	//!									响应重置魔化类型
	void								OnEquipmentResetElementType(CMessage *pMsg);
	//!									响应重置魔化相性
	void								OnEquipmentResetXiangXing(CMessage *pMsg);


	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 限时物品
	///////////////////////////////////////////////////////////////////////////////////////////////////
public:


};
 