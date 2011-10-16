#pragma once

class LogClient;

//! 逻辑日志接口封装类
//! 将日志参数封装为有逻辑意义的对象指针
class LogicLogInterface
{
public:
	LogicLogInterface(LogClient *pLogClient);


private:

	//!							得到当前时间字符串（00-00-00 00:00:00格式）
	void						GetCurrTimeString(string &strTime);

private:

	//!							日志客户端对象
	LogClient					*m_pLogClient;

};