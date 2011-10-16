#pragma once


#include "LogDef.h"

class CBaseMessage;

/////////////////////////////////////////////////////////////////////////////////
//!本地初始化信息
struct  LogClientInfo
{
	long		lClientNo;
	
	string		LogFileName;		//! 本地日志记录文件（服务器格式）
	string		TxtLogFileName;		//!	本地日志记录文件（文本格式）
	string		ErrLogFileName;		//! 错误记录文件
};



/////////////////////////////////////////////////////////////////////////////////
class LogClient
{
public:
	LogClient(void);
	~LogClient(void);

public:
	//!						
	bool						Init(CBaseMessage *pMsg, LogClientInfo &logClientInfo);

	//!							更新控制条件
	bool						UpdateCondition(CBaseMessage *pMsg);

	//!							设置是否获得LOGSERVER确认
	void						PassAffirm(bool bAffirm);

	void						SetSoket(long lLogSocket);
	long						GetSoket(void);
	
	void						Release(void);

public:
	//!							发送日志，按配置的类型依次输入参数
	//!							参数只允许Long、float*、char*、CGUID*四种类型
	//!							时间和服务器编号是默认的输入的
	bool						SendLogToServer(long LogType, ...);
	//!							发送SQL语句日志
	bool						SendStrSqlLogToServer(const char *pSql);

	//!							设置日志的保存模式
	bool						SetSaveMode(long lLogType, eSendControl eSC);

	//!							在本地记录文本格式日志
	bool						LogTxtToHere(const char *pTxt);

private:
	//!							在本地记录服务器格式日志
	bool						LogToHere(long LogType, ...);

	//!							从数组中获得表结构
	void						ByteArrayToTable(uchar *pData, long pos);

	//!							把日志参数压入uchar数组
	bool						GetLogByteArray(va_list& args, LogTable &tableInfo, vector<uchar> &vData);
	//!							把输入的参数值按类型压入uchar数组
	//!							参数args必须是va_start之后的
	//!							参数lType类型为eDataTypeCode
	bool						ParamToByteArray(va_list& args, const LogItem& logItem, vector<uchar> &vData);
	//!							同上，但要进行附加条件检测
	//!							RE: -1，执行失败； 0，无须检测或检测失败；1，检测通过
	long						ParamToByteArray_AndTestCondition(va_list& args, const LogItem& logItem, vector<uchar> &vData);

	/*
	//!							生成SQL语句
	//!							参数args必须是va_start之后的
	bool						GetSQL(va_list& args, LogTable &tableInfo, string &strValue);
	//!							根据输入的类型从参数中获得一个DB格式的值的字符串（自动添加单引号）
	//!							参数args必须是va_start之后的
	//!							参数lType类型为eDataTypeCode
	bool						GetValueByType_WithDB(va_list& args, const LogItem& logItem, string &strValue);
	//!							根据输入的类型从参数中获得一个DB格式的值的字符串，在执行过程中进行条件检测（自动添加单引号）
	bool						GetValueByType_AndTestCondition_WithDB(va_list& args, const LogItem& logItem, string &strValue);
	*/

	//!							测试筛选条件
	long						TestCondition(const ItemCondition &condition, void *pCurrValue);

	//!							文件写入线程
	static	unsigned __stdcall	FileWirteThread(void* pThis);


public:
	//!								连接线程
	HANDLE							hConnectThread;
	bool							bConnectThreadExit;

private:
	
	//------------------------------------------------------------------------
	//!	支持多线程添加文件内容的封装结构
	struct LogFile_ForThread
	{
		CRITICAL_SECTION	_critical_section;	//! 文件缓冲写入同步对象

		string				_str_buf;			//! 写入缓冲
		bool				_buf_is_null;		//! 缓冲为空标记

		string				_file_name;			//! 文件名称
		ofstream			_ofstream;			//! 文件写入流

		//!					同步的将文本放入写入缓冲
		void				WirteToBuf(string &strTxt);

		//!					将缓冲内容写入文件（用于单线程）
		long				BufToFile(void);

	private:
		//!					打开文件流
		bool				OpenStream(void);
	};
	
	//!									写入文件事件
	HANDLE								m_hEvent;
	//!									写入线程
	HANDLE								m_hWirteThread;
	//!
	long								m_lWirteThreadExit;

	//!									连接Logserver的客户端
	long								m_LogSocket;

	//!									本客户端编号
	long								m_lClientNo;

	//!									错误记录文件
	LogFile_ForThread					m_ErrFile;
	//!									本地日志记录（DB格式）
	LogFile_ForThread					m_LogFile;
	//!									本地日志记录（明文格式）
	LogFile_ForThread					m_TxtLogFile;	

	//!									对象可用标记
	bool								m_thisUsable;
	//!									对象被LogServer确认
	bool								m_BeAffirmed;

private:

	//!									控制标记对应数据表结构
	TableMap_byType						m_TableInfo;

	vector<ItemCondition>				m_vItemCondition;

};