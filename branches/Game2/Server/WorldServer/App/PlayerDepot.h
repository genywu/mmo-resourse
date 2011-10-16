
/*
*	file:		PlayerDepot.h
*	Brief:		仓库类
*	detail:		保存仓库数据		
*				
*	Authtor:	张科智
*	Datae:		2008-01-22
*/
#pragma once

#include "Container/AmountLimitGoodsContainer.h"
#include "Container/SubpackContainer.h"
#include "Container/VolumeLimitGoodsContainer.h"
#include "Container/Wallet.h"
#include "Goods/Goods.h"
#include "../../../Public/Common/DepotDef.h"

//!				容器列表pair<容器指针, 容器类型>
typedef			list<pair<CGoodsContainer*, long>>	GoodsContainerList;

//! ------------------------------------类申明------------------------------------
class PlayerDepot
{
public:
	PlayerDepot(void);
	~PlayerDepot(void);

private:
	PlayerDepot(const PlayerDepot&){};
	PlayerDepot&operator=(const PlayerDepot& r){};


public:

	//!									编码
	void								AddToByteArray_ForServer			(vector<BYTE> *pByteArray);
	//!									解码
	void								GetFormByteArray_FromServer			(BYTE *pByteArray, long &lPos);
	//!									设置数据解码
	void								GetSetupFormByteArray				(BYTE *pByteArray, long &lPos);


	//!									编码
	void								CodeToDataBlock_ForServer			(DBWriteSet& setWriteDB);
	//!									解码
	void								DecodeFormDataBlock_FromServer		(DBReadSet& setReadDB);
	//!									设置数据解码
	void								DecodeSetupFromDataBlock			(DBReadSet& setReadDB);

	//!									重置对象
	void								Clear								(void);
	//!									释放对象
	void								Release								(void);
	//!									绑定角色
	void								BindPlayer							(CPlayer *pPlayer);

private:
	//!									初始化对象
	void								Init								(void);


public:
	//!									得到容器列表
	void								GetGoodsContainerList				(GoodsContainerList &ContainerList);
	//!									找到容器对象
	CGoodsContainer*					FindContainer						(long eContainerId);

	//!									得到密码
	const char*							GetPassword							(void);
	//!									设置密码
	void								SetPassword							(const char* pPassword);

	//!									得到子背包购买标识
	long								GetSubpackFlag						(void);
	//!									设置子背包购买与否
	void								SetSubpackFlag						(long lFlag);

	//!									得到解冻时间
	long								GetThawdate						(void);
	//!									得到解冻时间
	void								SetThawdate						(long lThawdate);


public:

	//!									读取配置
	static	bool						LoadSetup							(string filename);
	//!									得到配置
	static	const	tagDepotSetup*		GetDepotSetup						(void);
private:

	static	tagDepotSetup	c_DepotSetup;


	//! 
	//!---------------------------------------------------------------------------------------------------------
private:
	//!											主仓库
	CVolumeLimitGoodsContainer					m_Primary;
	//!											主仓库2
	//CVolumeLimitGoodsContainer					m_Primary2;
	//!											附加仓库栏位（5个子背包栏位）
	CSubpackContainer							m_Secondary;
	//!											可用的的子背包栏位
	bool										m_arrHlodSubpack[DEPOT_SUBPACK_NUM];

	//!											金币仓库
	CWallet										m_GoldDepot;
	//!											银币仓库（由不用的宝石仓库对象改，对象名称不变）
	CWallet                 			        m_GemDepot;


private:
	//!											绑定的玩家
	CPlayer										*m_pPlayer;

	//!											仓库密码
	char										m_szPassword[DEPOT_PASSWORD_ARRAY_LEN + 1];

	//!											存取标记
	bool										m_bIsFree;


	//!											密码输入错误次数
	long										m_lInputPwdErrCount;
	//!											第一次错误的时刻（一段时间内）
	long										m_lFirstErrTime;
	//!											解冻时刻保存
	long										m_lThawDate;

	//!											合法对话半径
	ULONG										m_uSessionRadius;
	//!											对话中心X
	long										m_lSessionPosX;
	//!											对话中心Y
	long										m_lSessionPosY;

};