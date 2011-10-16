/**************************************************
*		文件名  ：WinAppender.cpp
*		简要描述：重写log4cplus中Appender类的append接口，使日志能输出到界面上

*		当前版本：1.0
*		作者    ：石喜
*		完成日期：2010-11-10
*		修订说明：
****************************************************/
#include "StdAfx.h"
#include "WinAppender.h"

#include <iostream>
#include <log4cplus/helpers/property.h>
#include "../AppFrame/AppFrameDef.h"

using namespace std;
using namespace log4cplus::helpers;
using namespace AppFrame;
namespace log4cplus
{

    const int MaxLogContentLen = 65536;

    WinAppender::WinAppender(void)
    {
    }

    WinAppender::~WinAppender(void)
    {
        destructorImpl();
    }


    WinAppender::WinAppender(
        const log4cplus::helpers::Properties& properties)
        : Appender(properties)
    {
        
    }

    void WinAppender::close()
    {
        closed = true; 
    }

    void WinAppender::append(const log4cplus::spi::InternalLoggingEvent& event)
    {
        tostringstream buf;
        layout->formatAndAppend(buf, event);
        tstring sz = buf.str();

        HWND wnd = GetWnd(LOG);
        static DWORD last = timeGetTime();
        if (timeGetTime() - last > 300000 ||         // 大于时间间隔
            (::GetWindowTextLength(wnd)) >= MaxLogContentLen)     // 内容超过64K
        {
            last = timeGetTime();
            ::SetWindowText(wnd, "");
        }
        char strText[MaxLogContentLen];
        ::GetWindowText(wnd,strText,MaxLogContentLen);
        strcat(strText,"\r\n");
        strcat(strText,sz.c_str());
        ::SetWindowText(wnd,strText);

        int nLine = ::SendMessage(wnd, EM_GETLINECOUNT, 0, 0);
        ::SendMessage(wnd, EM_LINESCROLL, 0, nLine);
    }
}



namespace
{
    template <typename Fac, typename Reg>
    static void reg_factory (Reg & reg)
    {
        reg.put (std::auto_ptr<typename Reg::product_type> (new Fac));
    }

} // namespace


void initializeWin32LogFactory()
{
    AppenderFactoryRegistry& reg = getAppenderFactoryRegistry();
    reg_factory<WinAppenderFactory> (reg);

}