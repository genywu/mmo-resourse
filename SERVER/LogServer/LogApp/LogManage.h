

#pragma once
#include "DbOperator.h"


//! --------------------------------------------------------------------------------------
//! 日志管理类
//!
class LogManage
{
public:
	LogManage(HWND hwnd = NULL);
	~LogManage(VOID);


public:
	//!					初始化
	BOOL				Init(VOID);
	//!					关闭
	VOID				Exit(VOID);
	//!					释放
	VOID				Release(VOID);

	//!					验证服务器编号
	BOOL				AffirmClientNo(LONG dClientNo, LONG lClientIp, LONG lSocket);

	//!					根据Socket获得客户端编号和IP
	//!					返回值：pair<客户端编号, IP>
	pair<LONG, LONG>	GetNoAndIpBySockte(LONG lSocket);

	//!					客户端掉线
	VOID				ClientLost(LONG lSocket);
	
	//!					添加一条日志写入语句
	BOOL				AddLog(LONG lDbID, LPCSTR pSql);

	//!					将数据转换为sql语句，在添加到写入缓冲
	BOOL				AddData(LONG lDbID, LONG lTableType, BYTE* pData);

	//!					响应超时（写入日志条数小于缓存数时，由此触发写入）
	VOID				OnOutTime(VOID);

	//!					序列化所有表到vector
	VOID				TableAddToByteArray(vector<byte>& outVector);

	//!					序列化控制条件到vector
	VOID				ConditionAddToByteArray(vector<byte>& outVector);

	//!					重读日志控制信息
	BOOL				ReLoadControlConfig(VOID);

	//!					在DB中创建表格（按照配置中读取的表格信息），并为每个表格创建一个时间聚集索引
	//!					为防止日志记录被误删除，不生成DROP语句
	//!					开启参数bOutToFile，则只输出一个后缀名为sql的文件，不在DB中执行创建语句
	VOID				CreateTableInDb(BOOL bOutToFile = TRUE, string file_path = "setup/");

	//!					输出数据表信息到文件
	VOID				OutTableInfoToFile(VOID);

private:
	//!					读取数据库配置
	BOOL				LoadServerConfig(VOID);
	//!					读取数据表结构集合
	BOOL				LoadTableConfig(VOID);
	//!					读取日志控制信息
	BOOL				LoadControlConfig(VOID);

	//!					创建缓存
	BOOL				CreateCache(VOID);
	
	//!					添加DB信息到执行对象
	LONG				AddDbInfoToOperator(VOID);
	//!					弹出错误消息
	BOOL				OutErrMsgBox(char*	pMsg);

	//!					将值类型转换为字符串表示
	BOOL				GetValueByType_WithDB(BYTE *pData, LONG &lPos, const LogItem& logItem, string &strValue);

	//!					添加一条日志写入语句
	BOOL				AddLog(LONG lDbID, LONG lTableID, LPCSTR pSql);

	

private:
	//!					得到一个库信息
	const	LogDbInfo*	GetLogDbInfo(LONG lDbNo);


private:
	//!									所有库信息
	DbInfoVector						m_vecDbInfo;
	//!									所有表类型
	TableMap_byType						m_TableMap;
	//!									每个表都有的附加表头（字段集）
	ItemVector							m_TableHead;
	//!									筛选条件集合
	vector<ItemCondition>				m_vItemCondition;

	//!									客户端信息（key=客户端socket, value=<客户端编号, 客户端IP>）
	map<LONG, pair<LONG, LONG>>			m_mapClientInfo;

	//!									消息发送控制
	vector<pair<LONG, LONG>>			m_vSendControl;
	//!									上级模块句柄（弹出错误消息用，可为NULL）
	HWND								m_hWnd;

	map<LONG, map<LONG, DbOperator*>>	m_mapmapSqlStr;



public:
	//!									日志写入计数map<数据库编号, map<表名, 写入数量>>
	map<LONG, map<string, LONG>>		m_WriteCount;
};