#include "stdafx.h"
#include "DateTime.h"
#include <stdio.h>

#ifdef WIN32
#else
#include <time.h>
#include <sys/time.h>
#endif

namespace AppFrame {

int GetCurTime(TimeValue &tv)
{
#ifdef WIN32
    SYSTEMTIME t;
    GetLocalTime(&t);
    tv.year = t.wYear;
    tv.mon = t.wMonth;
    tv.mday = t.wDay;
    tv.wday = t.wDayOfWeek;
    tv.hour = t.wHour;
    tv.min = t.wMinute;
    tv.sec = t.wSecond;
    tv.msec = t.wMilliseconds;
#else
    timeval time;
    gettimeofday(&time, NULL);
    tm *t = localtime(&time.tv_sec);
    tv.year = t->tm_year + 1900;
    tv.mon = t->tm_mon + 1;
    tv.mday = t->tm_mday;
    tv.wday = t->tm_wday;
    tv.hour = t->tm_hour;
    tv.min = t->tm_min;
    tv.sec = t->tm_sec;
    tv.msec = time.tv_usec / 1000;
#endif
    return 0;
}
void GetCurTimeString(OStringStream& s)
{
    TimeValue tv;
    GetCurTime(tv);
    return Time2String(s, tv);
}

void Time2String(OStringStream& s,
                          const TimeValue &t)
{
    using namespace std;
    s << setw(2) << setfill('0') << t.mon << '/'
      << setw(2) << setfill('0') << t.mday << '/'
      << setw(4) << t.year << ' '
      << setw(2) << setfill('0') << t.hour << ':'
      << setw(2) << setfill('0') << t.min << ':'
      << setw(2) << setfill('0') << t.sec << '.'
      << setw(3) << setfill('0') << t.msec << ' ';
}

}; // namespace AppFrame
