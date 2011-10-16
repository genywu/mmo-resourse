
//=============================================================================
/**
 *  file: ScriptSys.h
 *
 *  Brief:实现对<lua_State* >状态的管理
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-7-25
 */
//=============================================================================
#ifndef __SCRIPT_SYSTEM_H__
#define __SCRIPT_SYSTEM_H__

#pragma once
#include "..\Utility\TimerDef.h"

class CMessage;
class CGameTestDlg;
class CTestScript;

class CScriptSys:
		public CGameEvent
{
public:
	virtual ~CScriptSys(void);

	struct tagScriptTimer
	{
		//参数1
		unsigned long arg;
	};
private:
	CScriptSys(void);
	//	线程互斥
	HANDLE m_hStateMutex;
//	static CScriptSys* instance;
	//脚本状态机
	lua_State* m_luaState;
	//TestDialog
	CGameTestDlg* m_pTestDialog;

	//统一管理脚本里的定时器
	typedef map<long,tagScriptTimer*>	ScriptTimers;
	typedef ScriptTimers::iterator		itScriTimer;

	ScriptTimers	m_ScriptTimers;

	//为脚本提供的缓冲数据块
	BYTE* m_pDB;
	//缓冲块大小
	int	m_nDBSize;
public:
	static CScriptSys* getInstance();

	static bool Release();

	bool Init(CGameTestDlg* pTestDialog);
	
	bool UnInitial(); 

	lua_State* GetLuaState()	{return m_luaState;}
	BYTE* GetDB(int minsize);
	
	CGameTestDlg* GetTestDialog()	{return m_pTestDialog;}
public:
	//
	void RunMessage(CMessage *pMsg);
	//加载指定的lua文件
	BOOL LoadOneFileOrString(const char* strFileName);
	//加载Lua需要的脚本，避免重复加载
	//bool LoadLuaRequireFile(const char* fileName);
	//void StartTest(const char* strFielName);		
	void PerformString(const char* strContent);			
	//void EndTest(const char* strFielName);
	
	//超时响应
	virtual long OnTimeOut(DWORD timerid,DWORD curtime,const void* var);
	//当定时器被取消的调用
	virtual void OnTimerCancel(DWORD timerid,const void* var);
	
	//网络消息响应
	virtual void OnMessage(CMessage*){}
	//处理实体间消息函数
	//virtual long Proc(DWORD type,const void* var1,const void* var2){return 0;}
    virtual long Proc(eEventType type,const tagProcVar *pVar) { return 0; }
	//定制一个定时器
	long Schedule(	unsigned long arg,
					 unsigned long futuretime,
					 unsigned long interval = 0);
	//取消一个定时器
	int Cancel(long timerid);
	//连接到服务器
	int ConnectLoginServer();
	int ConnectServer(const char* strIP,int nPort,long lFlag);
	int ShutDownSocket(long lSocket);
private:
	//通过TESTDIALOG读到的配置来设置需要连接的IP地址
	string m_strIp;
	//通过TESTDIALOG读到的配置来设置需要的测试类型
	int m_TestType;
	//通过TESTDIALOG的编辑框得到的发送消息的时间间隔
	long m_lTestTimer;
};

#endif

