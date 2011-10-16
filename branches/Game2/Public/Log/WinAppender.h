/**************************************************
*		文件名  ：WinAppender.h
*		简要描述：重写log4cplus中Appender类的append接口，使日志能输出到界面上

*		当前版本：1.0
*		作者    ：石喜
*		完成日期：2010-11-10
*		修订说明：
****************************************************/

#pragma once


#include <log4cplus/spi/factory.h>
#include <log4cplus/appender.h>
using namespace log4cplus::spi;
using namespace log4cplus;

namespace log4cplus 
{

    class WinAppender:public Appender
    {
    public:
        WinAppender(void);
        WinAppender(const log4cplus::helpers::Properties& properties);

        // Dtor
        virtual ~WinAppender();

        // Methods
        virtual void close();

    protected:
        virtual void append(const log4cplus::spi::InternalLoggingEvent& event);

    private:
        // Disallow copying of instances of this class
        WinAppender(const WinAppender&);
        WinAppender& operator=(const WinAppender&);
    };

}
class Properties;

class WinAppenderFactory : public AppenderFactory {
public:
    SharedAppenderPtr createObject(const log4cplus::helpers::Properties& props)
    {
        return SharedAppenderPtr(new log4cplus::WinAppender(props));
    }

    tstring getTypeName() { 
        return LOG4CPLUS_TEXT("log4cplus::WinAppender");     
    }
};





