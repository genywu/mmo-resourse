#pragma once

#include "ServiceSCC.h"


class ServiceControl
{
public:
	//!									安装服务
	static		BOOL					InstallService(VOID);
	//!									启动服务
	static		BOOL					StartService(VOID);

private:
	//!									服务程序的运行入口
	static		VOID	WINAPI			ServiceMain(int argc, char** argv);
	//!									服务程序的事件响应入口
	static		VOID	WINAPI			ControlHandler(DWORD request);
	

private:
	//!									服务名称
	static		char					*c_pServiceName;
	//!									运行错误值保存
	static		LONG					c_lServiceRunError;

	static		SERVICE_STATUS			c_ServiceStatus;
	static		SERVICE_STATUS_HANDLE	c_hStatus;
	static		DWORD					c_dwThreadID;
};