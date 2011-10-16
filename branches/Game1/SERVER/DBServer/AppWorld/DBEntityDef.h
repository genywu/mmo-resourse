#ifndef DB_ENTITY_DEF_H
#define DB_ENTITY_DEF_H
#include "../public/BaseDef.h"
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


// 多线程读取处理所需信息
struct LoadObj
{
	CGUID				objGUID;
	long				objID;
	long				msgID;
	long				objNum;
	DB_OBJ_TYPE			objType;
	char				szCDKEY[64];
	DB_OPERATION_TYPE	opType;
	DWORD				dwIP;
	long				lQueryTime;

	LoadObj();
	virtual ~LoadObj();
	LoadObj& operator=(const LoadObj& right);
};

// 多线程写入处理所需信息
struct SaveObj
{
	CGUID		objGUID;
	long		objID;
	long		msgID;
	long		objNum;// 如果个数为负数则负了多少就表示删除多少个对象
	DB_OBJ_TYPE objType;
	std::vector<long> objAttrEnum; // 要保存的属性枚举值
	DB_OPERATION_TYPE	opType;
	DB_OPERATION_RESULT_TYPE	opResultType;
	DWORD		dwIP; // 计费需
	SaveObj();
	virtual ~SaveObj();
	SaveObj& operator=(const SaveObj& right);

};

#endif//DB_ENTITY_DEF_H