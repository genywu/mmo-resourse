 
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
/*
 tanhaowen 2009.1.23 修改添加 For AutoDriveClient
*/
#ifndef __MAP_SCRIPT_SYSTEM_H__
#define __MAP_SCRIPT_SYSTEM_H__

#pragma once
// #include "..\Utility\TimerDef.h"
// #include "..\Utility\GameEvent.h"
// #include "..\Utility\TimerDef.h"
// #include "../Utility//GUID.h"
// 
// class CMessage;
// class CGameTestDlg;
// 
// class CMapScriptSystem : public CGameEvent
// {
// public:
// 	CMapScriptSystem(void);
// 	virtual ~CMapScriptSystem(void);
// 
// 	struct tagMapScriptTimer
// 	{
// 		//参数1
// 		unsigned long arg;
// 	};
// private:
// 	//为脚本提供的缓冲数据块
// 	BYTE* m_pDB;
// 	//缓冲块大小
// 	int	m_nDBSize;
// 	//MapFile脚本状态机实列
// 	static CMapScriptSystem *m_MapSYSInstance;
// 	//脚本状态机
// 	lua_State			    *m_lMapLuaState;
// 	//TestDialog
// 	CGameTestDlg		    *m_pTestDialog;
// 	//统一管理脚本里的定时器
// 	typedef map<long, tagMapScriptTimer*>	MapScriptTimer;
// 	map<long, tagMapScriptTimer*>::iterator itScriptTimer;
// 	MapScriptTimer	m_mapScriptTimers;							
// 
// public:
// 	//创建并获取MapFile脚本状态机实列
// 	static CMapScriptSystem		*getInstance();
// 	//释放资源
// 	static bool					 Release();
// 	//初始化
// 	bool						 Init(CGameTestDlg *pTestDialog, const char *strScriptFile=NULL);
// 	//销毁Lua_state
// 	bool						 UnInitial(); 
// 	//收集内存垃圾
// 	int							 LuaGC(int nGcType);
// 	//创建并获取脚本缓冲数据
// 	BYTE						*GetDB(int minsize);
// 	//获取Lua状态机
// 	lua_State					*GetLuaState()			{ return m_lMapLuaState;}
// 	//获取当前mfc面板类
// 	CGameTestDlg				*GetTestDialog()		{ return m_pTestDialog;}
// public:
// 	////顶事器调用 读数据
// 	//virtual long				 OnTimerRead(/*CMapFile * pMap*/);
// 	////顶事器调用 写数据
// 	//virtual void				 OnTimerWrite();
// 
// 	int							 RunMessage(CMessage *pMsg);
// 	//加载Lua需要的脚本，避免重复加载
// 	bool				         LoadLuaRequireFile(const char *fileName);
// 	void						 StartTest(const char *strFielName);		
// 	void						 PerformString(const char *strContent);			
// 	void						 EndTest(const char *strFielName);
// 	//超时响应
// 	virtual long				 OnTimeOut(DWORD timerid, DWORD curtime, const void *var);
// 	//当定时器被取消的调用
// 	virtual void				 OnTimerCancel(DWORD timerid, const void *var);
// 	//网络消息响应
// 	virtual void				 OnMessage(CMessage*) {}
// 	//处理实体间消息函数
// 	virtual long				 Proc(DWORD type, const void *var1, const void *var2)		{ return 0; }
// 	//定制一个定时器
// 	long						 Schedule(unsigned long arg,
// 								          unsigned long futuretime,
// 										  unsigned long interval = 0);
// 	//取消一个定时器
// 	int							 Cancel(long timerid);
// 	//执行脚本
// 	void						 RunScript(string ScriptName);
// 	//
// 	long						 SendInfo(string strCDkey, CMessage *msg);
// 	long						 AcceptInfo(string strCDkey, CMessage *msg);
// 	//动态新加测试脚本
// 	bool						 AddScriptFile(const char *strScriptFileName);
// 	bool						 AddScriptString(const char *strString);
// 	//加载指定的lua文件
// 	bool						 LoadOneScriptFile(const char *strFileName);
// };

#endif //__MAP_SCRIPT_SYSTEM_H__

