// -*- C++ -*-

/**
 * @file   Def.h
 * @date   Wed Nov 18 14:01:21 CST 2009
 * @author Fox(yulefox.at.gmail.com)
 */

#pragma once

#include "Def.h"

namespace AppFrame {

const size_t TimeStrLength = 24;

/**
 * 时间精确到毫秒甚至微�? */
struct TimeValue
{
    int year;
    int mon;
    int mday;
    int wday;
    int hour;
    int min;
    int sec;
    int msec;
};

int GetCurTime(TimeValue &tv);
void GetCurTimeString(OStringStream &s);
void Time2String(OStringStream& s,
                          const TimeValue &t);
}; // namespace AppFrame
