////////////////////////////////////////////////////////////////////////////////
//Date.h/////////////////////////////////////////////////////////////////////
//日期类:
//Author:Joe
//Create Time:2004.7.28
#pragma once

#include <time.h>

//时间结构
struct tagTime
{
	//系统时间
	tm tmTime;

	tagTime(void);
	tagTime(SYSTEMTIME sysTime);
	tagTime(const char* pszDate);

	//转换成SYSTEMTIME格式
	void ToSystemTime(SYSTEMTIME& sysTime);
	//得到日期格式化的字符串
	const char* GetDateFormatStr();
	//得到时间格式化的字符串
	const char* GetTimeFormatStr();

	//得到值字符串
	const char* GetValueFormatStr();
	//返回时间差(秒值)
	long	GetSecondDiff(tagTime FutureTime);
	//得到距离当前时间的差(返回格式为n天n小时n分钟n秒钟)
	tagTime GetTimeDiff(tagTime FutureTime);
	//判断两个时间是否相等，精确到秒
	bool operator==(const tagTime& Time) const;
	bool operator> (const tagTime& Time) const;
	bool operator>=(const tagTime& Time) const;
	bool operator<(const tagTime& Time) const;
	bool operator<=(const tagTime& Time) const;
	tagTime& AddSecond(long lSecond);
	tagTime& AddMinute(long lMinute);
	tagTime& AddHour(long lHour);
	tagTime& AddDay(long lDay);

	//时间的值
	long Year();
	long Month();
	long Day();
	long Hour();
	long Minute();
	long Second();
	long Week();

	static bool IsLeap(long lYear)
	{return lYear%4 == 0 && lYear%100 != 0 || lYear%400 == 0;}

};

//距离当前的时间差
long DiffSecond(tagTime& Time1,const tagTime& Time2);
tagTime DiffTime(tagTime& Time1,const tagTime& Time2);
