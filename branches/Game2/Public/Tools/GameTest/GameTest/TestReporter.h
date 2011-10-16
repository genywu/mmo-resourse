/**
============================================================
*  Copyright (c) 2010
*  All rights reserved
*  文件名称：TestReporter.h
*  功能：测试报告类,主要提供输出测试状态字符串到UI
*  当前版本：1.0.0
*  作者：    刘贵荣
*  完成日期：2010.11.8
*  取代版本：---
*  原作者：	 刘贵荣
*  完成日期：2010.11.8
*
============================================================
*/

#pragma once

class CTestReporter
{
	/*主要记录并计算:  (鉴于现阶段机械人主要为用于测试服务器非客户端,性能非功能的黑盒自动化测试程序,测试结果记录略有侧重)
	1.基础测试环境:
	A.此次测试链接服务器IP
	B.此次测试总连接数
	C.此次测试类型节点个数
	D:此次测试类型节点名称
	E:此次测试项目名称
	F:测试开始时间
	G:测试结束时间
	H:测试开始账号
	I:测试结束账号


	2.本地与服务器之间网络通信状态:
	A.此次测试同时登录峰值
	B.此次测试同时登录谷值
	C.此次测试服务器踢出机械人总数量
	D:此次测试每小时在平均在线人数


	3.服务器测试(系统测试):
	A.性能测试
	a.冒烟测试
	α.此次测试第一次脚本是否能执行完正常流程

	b.压力测试
	α.此次测试机械人连续测试时间
	β.此次测试机械人连续与服务器数据交换量

	c.负载测试
	α.此次测试机械人最多连接数量

	B:安全测试
	a.数据安全(服务器非法消息验证)
	α.此次测试机械人是否使用随机MsgHead,随机MsgData
	β.此次测试机械人是否使用固定MsgHead,固定MsgData
	ε.此次测试机械人是否使用固定MsgHead,随机MsgData
	γ.此次测试机械人是否使用随机MsgHead,固定MsgData
	b.代码安全
	α.此次测试是否有跳逻辑判断成功
	β.此次测试跳逻辑处理位置(以消息头开头)

	4.客户端测试(系统测试):...

	*/

	//1.基础测试环境:
	static string m_strConctIp;					//此次测试链接服务器IP
	static long   m_lConctTotlNum;				//此次测试总连接数
	static string m_strTestTypeNodeNum;			//此次测试类型节点个数
	static string m_strTestTypeName;			//此次测试类型节点名称
	static string m_strProjName;				//此次测试项目名称
	static string m_strStartTestTime;			//测试开始时间
	static string m_strLaseTestTime;			//测试持续时间
	static string m_strStartCDKEY;				//测试开始账号
	static string m_strEndCDKEY;				//测试结束账号


	//2.本地与服务器之间网络通信状态:
	static long m_lMaxConctNum;				    //此次测试同时登录峰值
	static long m_lMinConctNum;				    //此次测试同时登录谷值
	static long m_lKickedByServerNum;	        //此次测试服务器踢出机械人总数量
	static long m_lPerHourConctNum;		        //此次测试每小时在平均在线人数


	//3.服务器测试(系统测试):
	static bool m_bIsCanRun;					//此次测试第一次脚本是否能执行完正常流程
	static long m_lTotalRuntime;			    //此次测试机械人连续测试时间
	static long m_lMsgInterchangeNum;		    //此次测试机械人连续与服务器数据交换量
	static long m_lMaxConctNumSysTest;          //此次测试机械人最多连接数量
	static bool m_bIsRadHeadRadData;		    //此次测试机械人是否使用随机MsgHead,随机MsgData
	static bool m_bIsFixedHeadFixedData;	    //此次测试机械人是否使用固定MsgHead,固定MsgData
	static bool m_bIsFixedHeadRadData;		    //此次测试机械人是否使用固定MsgHead,随机MsgData
	static bool m_bIsRadHeadFixeData;		    //此次测试机械人是否使用随机MsgHead,固定MsgData
	static bool m_bIsJumpLogicDrive;		    //此次测试是否有跳逻辑判断成功

public:
	CTestReporter();
	virtual ~CTestReporter();

	static void		SetConctIp(string _arg)				{ m_strConctIp = _arg; }
	static string	GetConctIp()						{ return m_strConctIp; }

	static void		SetConctTotlNum(long _arg)			{ m_lConctTotlNum = _arg; }
	static long		GetConctTotlNum()					{ return m_lConctTotlNum; }

	static void		SetTestTypeNode(string _arg)		{ m_strTestTypeNodeNum = _arg; }
	static string	GetTestTypeNode()					{ return m_strTestTypeNodeNum; }

	static void		SetTestTypeName(string _arg)		{ m_strTestTypeName = _arg; }
	static string	GetTestTypeName()					{ return m_strTestTypeName; }

	static void		SetProjName(string _arg)			{ m_strProjName = _arg; }
	static string	GetProjName()						{ return m_strProjName; }

	static void		SetStartTestTime(string _arg)		{ m_strStartTestTime = _arg; }
	static string	GetStartTestTime()					{ return m_strStartTestTime; }

	static void		SetEndTestTime(string _arg)			{ m_strLaseTestTime = _arg; }
	static string	GetEndTestTime()					{ return m_strLaseTestTime; }

	static void		SetStartCDKEY(string _arg)			{ m_strStartCDKEY = _arg; }
	static string	GetStartCDKEY()						{ return m_strStartCDKEY; }

	static void		SetEndCDKEY(string _arg)			{ m_strEndCDKEY = _arg; }
	static string	GetEndCDKEY()						{ return m_strEndCDKEY; }

	static void		SetMaxConctNum(long _arg)			{ m_lMaxConctNum = _arg; }
	static long		GetMaxConctNum()					{ return m_lMaxConctNum; }

	static void		SetMinConctNum(long _arg)			{ m_lMinConctNum = _arg; }
	static long		GetMinConctNum()					{ return m_lMinConctNum; }

	static void		SetKickedByServerNum(long _arg)		{ m_lKickedByServerNum = _arg; }
	static long		GetKickedByServerNum()				{ return m_lKickedByServerNum; }

	static void		SetPerHourConctNum(long _arg)		{ m_lPerHourConctNum = _arg; }
	static long		GetPerHourConctNum()				{ return m_lPerHourConctNum; }

	static void		SetIsCanRun(bool _arg)				{ m_bIsCanRun = _arg; }
	static bool		GetIsCanRun()						{ return m_bIsCanRun; }

	static void		SetTotalRuntimem(long _arg)			{ m_lTotalRuntime = _arg; }
	static long		GetTotalRuntime()					{ return m_lTotalRuntime; }

	static void		SetMsgInterchangeNum(long _arg)		{ m_lMsgInterchangeNum = _arg; }
	static long		GetMsgInterchangeNum()				{ return m_lMsgInterchangeNum; }

	static void		SetMaxConctNumSysTest(long _arg)	{ m_lMaxConctNumSysTest = _arg; }
	static long		GetMaxConctNumSysTest()				{ return m_lMaxConctNumSysTest; }

	static void		SetIsRadHeadRadDatat(bool _arg)		{ m_bIsRadHeadRadData = _arg; }
	static bool		GetIsRadHeadRadData()				{ return m_bIsRadHeadRadData; }

	static void		SetIsFixedHeadFixedData(bool _arg)	{ m_bIsFixedHeadFixedData = _arg; }
	static bool		GetIsFixedHeadFixedData()			{ return m_bIsFixedHeadFixedData; }

	static void		SetIsFixedHeadRadData(bool _arg)	{ m_bIsFixedHeadRadData = _arg; }
	static bool		GetIsFixedHeadRadData()				{ return m_bIsFixedHeadRadData; }

	static void		SetIsRadHeadFixeData(bool _arg)		{ m_bIsRadHeadFixeData = _arg; }
	static bool		GetIsRadHeadFixeData()				{ return m_bIsRadHeadFixeData; }

	static void		SetIsJumpLogicDrive(bool _arg)		{ m_bIsJumpLogicDrive = _arg; }
	static bool		GetIsJumpLogicDrive()				{ return m_bIsJumpLogicDrive; }

	static CTestReporter*	GetInstance();
	static string			GetTestResultInfo();
	static bool				Release();

	bool					Init(void*);
	void*					GetUIInstance() { return m_pDlg; }
private:
	void* m_pDlg;
};
