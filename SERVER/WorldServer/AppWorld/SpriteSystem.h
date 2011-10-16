
/*
*	file:		SpriteSystem.h
*	Brief:		小精灵辅助系统
*	detail:		在WS上，本类只具有数据保持、交换功能，
*				对小精灵系统的所有数据进行统一管理。
*					
*	Authtor:	张科智
*	Datae:		2008-05-21
*/
#include "../public/AutoSupplyData.h"
#include "../public/AutoFightData.h"

#pragma once

class CPlayer;

//! 小精灵辅助系统类
////////////////////////////////////////////////////////////////////////////////
class SpriteSystem
{
public:
	SpriteSystem(void);
	~SpriteSystem(void);
	

public:
	//!								读取配置
	static		BOOL				LoadSetup(void);
	//!								配置编码
	static		void				SetupAddToByteArray(vector<BYTE> &ByteArray);
	

public:
	//!								编码
	void							AddToByteArray		(vector<BYTE> &ByteArray);
	//!								解码
	void							GetFormByteArray	(BYTE *pByteArray, long &lPos);

	//!	编码
	void CodeToDataBlock(DBWriteSet& setWriteDB);
	//!	解码
	void DecodeFromDataBlock(DBReadSet& setReadDB);


    //! 设置稳定挂机时间
    void SetStableTime(DWORD dwTime);
    //得到稳定挂机时间
    DWORD GetStableTime(void);
private:
    //!                             查找容器相同函数
    static      bool        FindExist(vector<LONG> &refVector, LONG lValue);
private:
	//!								能够使用的物品（索引 + 可用范围枚举）
	static		map<LONG, DWORD>	c_mapCanUseGoodsIndex;
    //!                             能够使用的战斗技能
    static      vector<LONG>        c_vectorCanUseFightSkill;
    //!                             能够使用的辅助技能
    static      vector<LONG>        c_vectorCanUseAssistSkill;
    //!                             能够使用的辅助道具物品
    static      vector<LONG>        c_vectorCanUseAssistGoods;

	//!								自动补给数据对象
	AutoSupplyData					m_AutoSupplyData;
    //!                             自动战斗数据对象
    CAutoFightData                  m_AutoFightData;
	


};