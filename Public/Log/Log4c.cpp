/**************************************************
*		文件名  ：Log4c.cpp
*		简要描述：用log4cplus包装的日志文件

*		当前版本：1.0
*		作者    ：石喜
*		完成日期：2010-11-10
*		修订说明：
****************************************************/
#include "StdAfx.h"
#include "Log4c.h"
#include <stdarg.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>


using namespace log4cplus;
using namespace log4cplus::helpers;


//---------------------------The variable definition--------------------------------------
#define LOGINIT Logger::getInstance("")
static Logger  logger[32] = {LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,
								LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,
								LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT,LOGINIT};

//---------------------------The function definition--------------------------------------

#define DO_LOGGER(module,logLevel, pFormat, bufSize)     \
	if(logger[module].isEnabledFor(logLevel))       \
{                \
	va_list args;            \
	va_start(args, pFormat);        \
	char buf[bufSize] = {0};        \
	vsnprintf_s(buf, bufSize, bufSize, pFormat, args);    \
	va_end(args);           \
	logger[module].forcedLog(logLevel, buf, __FILE__, __LINE__); \
}
//////////////////////////////////////////////////////////////////////////////////////////
//
//    Log4c
// 
//////////////////////////////////////////////////////////////////////////////////////////
void Log4c::Init(const char *filename)
{
	PropertyConfigurator::doConfigure(filename);
}

void Log4c::Register(int module,const char *filename)
{
	logger[module] = Logger::getInstance(filename);
}

void Log4c::Fatal(int module,const char * pFormat, ...)
{
	DO_LOGGER(module,log4cplus::FATAL_LOG_LEVEL, pFormat, 256);
}


void Log4c::Error(int module,const char * pFormat, ...)
{
	DO_LOGGER(module,log4cplus::ERROR_LOG_LEVEL, pFormat, 256);
}


void Log4c::Warn(int module,const char * pFormat, ...)
{
	DO_LOGGER(module,log4cplus::WARN_LOG_LEVEL, pFormat, 256);
}


void Log4c::Info(int module,const char * pFormat, ...)
{
	DO_LOGGER(module,log4cplus::INFO_LOG_LEVEL, pFormat, 512);
}


void Log4c::Debug(int module,const char * pFormat, ...)
{
	DO_LOGGER(module,log4cplus::DEBUG_LOG_LEVEL, pFormat, 1024);
}


void Log4c::Trace(int module,const char * pFormat, ...)
{
	DO_LOGGER(module,log4cplus::TRACE_LOG_LEVEL, pFormat, 1024);
}