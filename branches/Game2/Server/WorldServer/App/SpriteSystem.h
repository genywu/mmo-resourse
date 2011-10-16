
/*
*	file:		SpriteSystem.h
*	Brief:		小精灵辅助系统
*	detail:		在WS上，本类只具有数据保持、交换功能，
*				对小精灵系统的所有数据进行统一管理。
*					
*	Authtor:	张科智
*	Datae:		2008-05-21
*/
#pragma once

#include "../../../Public/ServerPublic/Server/Common/AutoSupplyData.h"

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


private:
	//!								能够使用的物品（索引 + 可用范围枚举）
	static		map<LONG, DWORD>	c_mapCanUseGoodsIndex;

	//!								自动补给数据对象
	AutoSupplyData					m_AutoSupplyData;
	


};