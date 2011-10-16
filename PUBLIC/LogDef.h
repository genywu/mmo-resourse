



#pragma once



//! --------------------------------------------------------------------------------------
//! 数据类型代号
enum eDataTypeCode
{
	eDTC_ErrorType	= -1,		//! 错误
	eDTC_INT32		= 0,		//! 32位整数
	eDTC_FLOAT,					//! 浮点
	eDTC_DATE,					//! 时间
	eDTC_GUID,					//! 128位GUID
	eDTC_STR128,				//!	128位字符串
	eDTC_STR512,				//!	512位字符串

	eDTC_END,
};


//! 日志的保存模式
enum eSendControl
{
	eSC_LOGSERVER		= 1,			//! 发送到LogServer
	eSC_HERE			= 2,			//! 记录在本地
	sSC_SERVER_AND_HERE = 3,			//! 本地和server同时记录

	eSC_INVALID,
};

enum eCondition
{
	eC_Even,					//! 等于
	eC_More_Than,				//! 大于
	eC_Less_Than,				//! 小于
};

enum eConditionUse
{
	eCU_INTERSECTION,			//! 交集
	eCU_UNION,					//! 并集
};

//! --------------------------------------------------------------------------------------
//! 条件
struct ConditionTile
{
	long			m_eCondition;		//!	eCondition
	long			m_eConditionUse;	//! eConditionUse
	vector<BYTE>	m_vData;			//!	值保存
};

//! 字段条件
struct  ItemCondition
{
	long						m_valueType;	//!	eSendControl
	vector<pair<long, long>>	m_vPosList;	//! first:table_type, second:item_index
	vector<ConditionTile>		m_vConditionTile;
};


//! --------------------------------------------------------------------------------------
//! 日志字段
struct LogItem
{
	char			m_szItemName[128];
	long			m_eType;
	long			m_conditionId;

	LogItem(void)		{ memset(this, 0, sizeof(LogItem)); }
};

//! 字段数组
typedef	vector<LogItem>				ItemVector;
typedef	vector<LogItem>::iterator	ItemIte;



//! --------------------------------------------------------------------------------------
//! 日志表
struct LogTable
{
	char			m_szTableName[128];
	long			m_tableType;
	long			m_SendControl;
	long			m_isLog;
	ItemVector		m_vecItem;

	LogTable(void)		{ Clear(); }
	void Clear(void)	{ memset(this, 0, sizeof(LogTable) - sizeof(ItemVector)); m_vecItem.clear();}
};

//! 日志表基础类型数据长度
const long LogTableBaseDataSize = sizeof(LogTable) - sizeof(ItemVector);

//! 表数组
typedef	map<long, LogTable>					TableMap_byType;
typedef	map<long, LogTable>::iterator		TableMap_ite;



//! --------------------------------------------------------------------------------------
//! 日志数据库
struct LogDbInfo
{
	long			m_worldNo;			//! 对应世界编号
	long			m_lClientIp;
	char			m_szProvider[128];	//! 提供者
	char			m_szDbName[256];	//! 数据库名称
	char			m_szDbIP[256];		//! 数据库服务器IP
	char			m_szUsername[128];	//! 用户名
	char			m_szPassword[128];	//! 密码

	LogDbInfo(void)		{ memset(this, 0, sizeof(LogDbInfo)); }
};

typedef	vector<LogDbInfo>				DbInfoVector;
typedef	vector<LogDbInfo>::iterator		DbInfoIte;