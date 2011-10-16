#ifndef DB_DEFINE_H
#define DB_DEFINE_H

// 对象类型
enum COMPOSITE_TYPE
{
	COM_COMPONENT = 0,
	COM_LEAF,
	COM_COMPOSITE,
	//COM_DETAILCOMPOSITE, // 是其父节点的子类型,在配置文件中设置
};

// 预分配Rgn内存块
#define MAX_FREE_RGN_NUM 1024
// 预分配Proxy Rgn内存块
#define MAX_FREE_PROXY_RGN_NUM 256
// 定义一个数表示读取全部该类型Entity
#define MAX_ENTITY_ID_NUM 99999999

// 定义操作全部该类型实体标志
const long ALL_ENTITIES_FLAG = -99999999;
const long ERROR_FLAG		= -99999999;
const long NULL_FLAG		= -99999999;



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



//数据库关系类型
enum DB_RELATE_TYPE
{
	DB_RELATE_NULL = 0,
	DB_RELATE_AND,
	DB_RELATE_OR,
	DB_RELATE_GREAT,
	DB_RELATE_EQUAL,
	DB_RELATE_LESS,
	DB_RELATE_GREATANDEQUAL,
	DB_RELATE_LESSANDEQUAL,
	DB_RELATE_LBLANKAND, // 左括号+AND : ) AND
	DB_RELATE_RBLANKAND, // 右括号+AND : AND (
	DB_RELATE_LBLANK, // 左括号 : (
	DB_RELATE_RBLANK, // 右括号 : )
	DB_RELATE_RBLANKOR, // 右括号 : ) OR
	DB_RELATE_LBLANKOR, // 左括号+OR : OR (
};

enum DATA_OBJECT_TYPE
{
	DATA_OBJECT_UNKNOWN = 0,
	DATA_OBJECT_FLOAT,
	DATA_OBJECT_LONG,
	DATA_OBJECT_SHORT,
	DATA_OBJECT_ULONG,
	DATA_OBJECT_USHORT,
	DATA_OBJECT_BOOL,
	DATA_OBJECT_CHAR,
	DATA_OBJECT_BYTE,
	DATA_OBJECT_WORD,
	DATA_OBJECT_DWORD,
	DATA_OBJECT_STRING,
	DATA_OBJECT_BUFFER,
	DATA_OBJECT_GUID,
	DATA_OBJECT_STRUCT,
	DATA_OBJECT_TIME,
	DATA_OBJECT_BIGINT,
};

struct tagQueryInfo 
{
	tagQueryInfo() : relateType(DB_RELATE_NULL),objType(DATA_OBJECT_UNKNOWN)
	{
		memset(szAttrName, 0, sizeof(szAttrName));
	}
	~tagQueryInfo()
	{
	}

	tagQueryInfo& operator=(const tagQueryInfo& right)
	{
		if(this != &right)
		{
			memcpy(szAttrName, right.szAttrName, sizeof(szAttrName));
			relateType = right.relateType;
			objType = right.objType;
		}
		return *this;
	}
	char szAttrName[32];
	DB_RELATE_TYPE relateType;
	DATA_OBJECT_TYPE objType;
};

//数据库操作类型
enum DB_OPERATION_TYPE
{
	DB_OPER_INIT = 0,
	DB_OPER_INSERT,
	DB_OPER_DELETE,
	DB_OPER_SAVE,
	DB_OPER_LOAD,
	DB_OPER_REGISTER,	// 从DBS上注册该实体的缓冲对象
	DB_OPER_UNREGISTER, // 从DBS上注销该实体的缓冲对象
	DB_OPER_DELETE_INSERT,// DBS上先删除再插入数据
	DB_OPER_PROCEDURE,		// 标准存储过程
	DB_OPER_CREATEROLE_PROCEDURE, // 创建人物时的存储过程
	DB_OPER_INSERTMAIL_PROCEDURE, // 添加邮件时的存储过程
	DB_OPER_CHANGENAME_PROCEDURE, // 玩家改名的存储过程
	DB_OPER_ONLY_MEMSAVE,		// 只存到对象缓存中，不存入DB
	DB_OPER_DEL_LEAVES,		// 删除其叶子节点
	DB_OPER_SAVE_LEAVES,		// 保存其叶子节点
	DB_OPER_LOAD_LEAVES,		// 读取其叶子节点
	DB_OPER_SAVE_RETURN,       // 保存并返回
	DB_OPER_LOAD_NOCACHE,      // 读取不缓存
};

enum PLAYER_EXTEND_ID
{
	PEI_PACKET					= 1,	//普通背包
	PEI_EQUIPMENT,						//装备栏	
	PEI_WALLET,							//金钱
	PEI_SILVERWALLET,					//银币钱包
	PEI_GEM,							//! 宝石
	PEI_YUANBAO,						//元宝
	PEI_JIFEN,							//积分
	PEI_MEDAL_CONTAINER,
	PEI_PACK =10000,					//子背包容器
	PEI_PACK1=10001,					//子背包栏位1
	PEI_PACK2=10002,					//子背包栏位2
	PEI_PACK3=10003,					//子背包栏位3
	PEI_PACK4=10004,					//子背包栏位4
	PEI_PACK5=10005,					//子背包栏位5

	PEI_DCT_Gold = 0xFFFF01,		//! 金币仓库
	PEI_DCT_Silver,					//! 银币仓库（以前是宝石仓库，枚举值不变）
	PEI_DCT_Primary,				//! 主仓库
	//PEI_DCT_Primary2,				//! 主仓库2
	PEI_DCT_Secondary,				//! 扩展包裹容器
	PEI_DCT_Secondary1,				//! 扩展仓库1
	PEI_DCT_Secondary2,				//! 扩展仓库2
	PEI_DCT_Secondary3,				//! 扩展仓库3
	PEI_DCT_Secondary4,				//! 扩展仓库4

	PEI_BUSINESSPACKFIELD = 20000,   //放置跑商背包的栏位ID
	PEI_BUSINESSPACK = 20001,		//跑商背包ID
};

enum SESSION_TYPE
{
	ST_NORMAL_SESSION,
	ST_WS_SESSION,
};

enum DbOperThreadType
{
	eDbOperPlayerType = 0,
	eDbOperOtherType  = 1,
	eDbOperNormallyType = 2,
	eDbOperMailThread = 3,
};

enum eThreadType
{
	eThreadInit = -1,
	eThreadLoadType = 0,
	eThreadSaveType = 1,
};

#endif//DB_DEFINE_H