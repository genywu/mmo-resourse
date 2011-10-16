#ifndef DB_ENTITY_DEF_H
#define DB_ENTITY_DEF_H
#include "../public/dbdefine.h"

// 定义一个数表示读取全部该类型Entity
#define MAX_ENTITY_ID_NUM 99999999

// 定义操作全部该类型实体标志
const long ALL_ENTITIES_FLAG = -99999999;

// [070622 AHC]
//属性数据类型:	该属性的C++数据类型
enum DB_ATTR_DATA_TYPE
{
	DB_ATTR_DATA_BYTE = 0,	// 1 BYTE
	DB_ATTR_DATA_WORD,		// 2 BYTE
	DB_ATTR_DATA_DWORD,		// 4 BYTE
	DB_ATTR_DATA_FLOAT,		// 4 BYTE
	DB_ATTR_DATA_LONG,		// 4 BYTE
	DB_ATTR_DATA_SHORT,		// 2 BYTE
	DB_ATTR_DATA_CHAR,		// 1 BYTE
	DB_ATTR_DATA_BOOL,		// 1 BYTE
	DB_ATTR_DATA_BUF,		// N BYTE(或特定值BYTE)

	DB_ATTR_DATA_BYTEARRAY, // 1 BYTE * M个数
	DB_ATTR_DATA_WORDARRAY,	// 2 BYTE * M个数
	DB_ATTR_DATA_DWORDARRAY,// 4 BYTE * M个数
	DB_ATTR_DATA_FLOATARRAY,// 4 BYTE * M个数
	DB_ATTR_DATA_LONGARRAY,	// 4 BYTE * M个数
	DB_ATTR_DATA_SHORTARRAY,// 2 BYTE * M个数
	DB_ATTR_DATA_CHARARRAY,	// 1 BYTE * M个数
	DB_ATTR_DATA_BOOLARRAY,	// 1 BYTE * M个数
	DB_ATTR_DATA_BUFARRAY	// N BYTE(或特定值BYTE) * M个数
};


enum PLAYER_EXTEND_ID
{
	PEI_PACKET					= 1,	//普通背包
	PEI_EQUIPMENT,						//装备栏
	//PEI_HAND,							//玩家手上
	PEI_WALLET,							//金钱
	PEI_SILVERWALLET,					//银币钱包
	PEI_GEM,							//! 宝石
	PEI_YUANBAO,						//元宝
	PEI_JIFEN,							//积分
	PEI_EQUIPMENT_UPGRADING,			//装备升级
	//PEI_BANK,							//银行
	//PEI_DEPOT,							//仓库
	PEI_ENHANCEMENT,					//
	PEI_PACK =10000,						//子背包容器
	PEI_PACK1=10001,					//子背包栏位1
	PEI_PACK2=10002,					//子背包栏位2
	PEI_PACK3=10003,					//子背包栏位3
	PEI_PACK4=10004,					//子背包栏位4
	PEI_PACK5=10005						//子背包栏位5
};

#endif//DB_ENTITY_DEF_H