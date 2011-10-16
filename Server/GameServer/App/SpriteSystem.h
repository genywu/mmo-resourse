
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

class CPlayer;

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

	//!							响应自动补给设置修改		
	void						OnChangeAutoSupply			(CMessage *pMsg);							


private:
	//!							能够使用的物品索引集
	static	map<LONG, DWORD>	c_mapCanUseGoodsIndex;
	
	//!							自动补给数据对象
	GSAutoSupply				m_AutoSupply;



};