
/*
*	file:		SpriteSystem.h
*	Brief:		小精灵辅助系统
*	detail:		每个类针对一个单独的玩家，功能包括：
*					自动补给
*					
*	Authtor:	张科智
*	Datae:		2008-05-21
*/


#pragma once

#include "GSAutoSupply.h"
#include "GSAutoFight.h"

class CPlayer;
class CServerRegion;

//! 小精灵辅助系统类
////////////////////////////////////////////////////////////////////////////////
class SpriteSystem
{
public:
	SpriteSystem(void);
	~SpriteSystem(void);

public:
	//!							配置编码
	static		void			SetupAddToByteArray			(vector<BYTE> &ByteArray);
	//!							配置解码
	static		void			SetupGetFormByteArray		(BYTE *pByteArray, long &lPos);

	//!							配置编码
	static		void			CodeSetupToDataBlock		(DBWriteSet& setWriteDB);
	//!							配置解码
	static		void			DecodeSetupFromDataBlock	(DBReadSet& setReadDB);

	//!							测试一个物品能否用于自动补给
	static		BOOL			TestAutoSupplyGoods			(LONG lGoodsIdx);

    //!                         测试一个战斗技能能否使用
    static      BOOL            TestAutoFightSkill       (LONG lSkillID);

    //!                         测试一个战斗辅助技能能否使用
    static      BOOL            TestAutoAssistSkill       (LONG lSkillID);

    //!                         测试一个辅助道具能否使用
    static      BOOL            TestAutoAssistGoods       (LONG lGoodsID);

public:
	//!							设置所有者
	void						SetOwner					(CPlayer *pMainPlayer);
	//!							编码
	void						AddToByteArray				(vector<BYTE> &ByteArray);
	//!							解码
	void						GetFormByteArray			(BYTE *pByteArray, long &lPos);

	//!							编码
	void						CodeToDataBlock				(DBWriteSet& setWriteDB);
	//!							解码
	void						DecodeFromDataBlock			(DBReadSet& setReadDB);

    //!                         设置稳定挂机时间
    void                        SetStableTime(DWORD dwTime)
    {
        if (dwTime <= MAX_STABLETIME)
        {
            m_AutoFight.SetStableTime(dwTime);
        }
        else
        {
            m_AutoFight.SetStableTime(MAX_STABLETIME);
        }
    }
    //!                         判断自动修理的条件
    bool                        CanAutoRepair(void);
    //!                         得到稳定挂机时间
    long                        GetStableTime(void)         {return m_AutoFight.GetStableTime();}

	//!							响应自动补给设置修改		
	void						OnChangeAutoSupply			(CMessage *pMsg);							
    //!                         响应自动打怪设置
    void                        OnChangeAutoFight           (CMessage *pMsg);
    //!                         响应技能和辅助技能道具的检查可用性
    //void                        OnCheckCanUse(CMessage *pMsg);


private:
	//!							能够使用的物品索引集
	static	map<LONG, DWORD>	c_mapCanUseGoodsIndex;
    //!                             能够使用的战斗技能
    static  vector<LONG>        c_vectorCanUseFightSkill;
    //!                             能够使用的辅助技能
    static  vector<LONG>        c_vectorCanUseAssistSkill;
    //!                             能够使用的辅助道具物品
    static  vector<LONG>        c_vectorCanUseAssistGoods;
	
	//!							自动补给数据对象
	GSAutoSupply				m_AutoSupply;
    //!                         自动打怪数据对象
    GSAutoFight                 m_AutoFight;



};