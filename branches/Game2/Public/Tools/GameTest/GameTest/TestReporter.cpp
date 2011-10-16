/**
============================================================
*  Copyright (c) 2010
*  All rights reserved
*  文件名称：TestReporter.cpp
*  功能：测试报告类
*  当前版本：1.0.0
*  作者：    刘贵荣
*  完成日期：2010.11.8
*  取代版本：---
*  原作者：	 刘贵荣
*  完成日期：2010.11.8
*
============================================================
*/
#include "stdafx.h"
#include "TestReporter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

string CTestReporter::m_strConctIp("");					//此次测试链接服务器IP
long   CTestReporter::m_lConctTotlNum = 0;				//此次测试总连接数
string CTestReporter::m_strTestTypeNodeNum("");			//此次测试类型节点个数
string CTestReporter::m_strTestTypeName("");			//此次测试类型节点名称
string CTestReporter::m_strProjName("");				//此次测试项目名称
string CTestReporter::m_strStartTestTime("");			//测试开始时间
string CTestReporter::m_strLaseTestTime("");				//测试结束时间
string CTestReporter::m_strStartCDKEY("");				//测试开始账号
string CTestReporter::m_strEndCDKEY("");				//测试结束账号
long CTestReporter::m_lMaxConctNum = 0;					//此次测试同时登录峰值
long CTestReporter::m_lMinConctNum = 0;					//此次测试同时登录谷值
long CTestReporter::m_lKickedByServerNum = 0;	        //此次测试服务器踢出机械人总数量
long CTestReporter::m_lPerHourConctNum = 0;				//此次测试每小时在平均在线人数
bool CTestReporter::m_bIsCanRun = false;				//此次测试第一次脚本是否能执行完正常流程
long CTestReporter::m_lTotalRuntime = 0;			    //此次测试机械人连续测试时间
long CTestReporter::m_lMsgInterchangeNum = 0;		    //此次测试机械人连续与服务器数据交换量
long CTestReporter::m_lMaxConctNumSysTest = 0;			//此次测试机械人最多连接数量
bool CTestReporter::m_bIsRadHeadRadData = false;		//此次测试机械人是否使用随机MsgHead,随机MsgData
bool CTestReporter::m_bIsFixedHeadFixedData = false;	//此次测试机械人是否使用固定MsgHead,固定MsgData
bool CTestReporter::m_bIsFixedHeadRadData = false;		//此次测试机械人是否使用固定MsgHead,随机MsgData
bool CTestReporter::m_bIsRadHeadFixeData = false;		//此次测试机械人是否使用随机MsgHead,固定MsgData
bool CTestReporter::m_bIsJumpLogicDrive = false;		//此次测试是否有跳逻辑判断成功


CTestReporter::CTestReporter()
{
	m_pDlg = NULL;
}
CTestReporter::~CTestReporter()
{

}


/*
=========================================================================
函数功能:TestReporterLogic初始化函数  By: LGR
=========================================================================
*/
bool CTestReporter::Init(void* p)
{
	m_pDlg = p;
	return true;
}


/*
=========================================================================
函数功能:TestReporterLogic 静态全局对象创建函数  By: LGR
=========================================================================
*/
CTestReporter*	CTestReporter::GetInstance()
{
	static CTestReporter*	pInstance = NULL;
	if(NULL == pInstance)
	{
		pInstance= new CTestReporter();
	}
	return pInstance;
}


/*
=========================================================================
函数功能:TestReporter 引用对象释放  By: LGR
=========================================================================
*/
bool CTestReporter::Release()
{
	CTestReporter* pInstance = GetInstance();
	SAFE_DELETE(pInstance);
	return true;
}

/*
=========================================================================
函数功能:TestReporter 得到测试结果输出字符串  By: LGR
=========================================================================
*/
string	CTestReporter::GetTestResultInfo()
{
	char _out[1024] = "";
	sprintf(_out,"此次测试链接服务器IP:%s\r\n此次测试总连接数: %d\r\n此次测试类型节点: %s\r\n此次测试类型节点名称: %s\r\n此次测试项目名称: %s\r\n测试开始时间: %s\r\n测试持续时间: %s\r\n测试开始账号: %s\r\n测试结束账号: %s\r\n此次测试同时登录峰值: %d\r\n此次测试同时登录谷值: %d\r\n此次测试服务器踢出机械人总数量: %d\r\n此次测试每小时在平均在线人数: %d\r\n此次测试第一次脚本是否能执行完正常流程: %d\r\n此次测试机械人连续测试时间: %d\r\n此次测试机械人连续与服务器数据交换量: %d\r\n此次测试机械人最多连接数量:  %d\r\n此次测试机械人是否使用随机MsgHead,随机MsgData:  %d\r\n此次测试机械人是否使用固定MsgHead,固定MsgData: %d\r\n此次测试机械人是否使用固定MsgHead,随机MsgData: %d\r\n此次测试机械人是否使用随机MsgHead,固定MsgData: %d\r\n此次测试是否有跳逻辑判断成功: %d\r\n",m_strConctIp.c_str(),m_lConctTotlNum,m_strTestTypeNodeNum.c_str(),m_strTestTypeName.c_str(),m_strProjName.c_str(),m_strStartTestTime.c_str(),m_strLaseTestTime.c_str(),m_strStartCDKEY.c_str(),m_strEndCDKEY.c_str(),m_lMaxConctNum,m_lMinConctNum,m_lKickedByServerNum,m_lPerHourConctNum,m_bIsCanRun,m_lTotalRuntime,m_lMsgInterchangeNum,m_lMaxConctNumSysTest,m_bIsRadHeadRadData,m_bIsFixedHeadFixedData,m_bIsFixedHeadRadData,m_bIsRadHeadFixeData,m_bIsJumpLogicDrive);
	return _out;
}