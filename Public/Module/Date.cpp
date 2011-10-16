////////////////////////////////////////////////////////////////////////////////
//Date.cpp/////////////////////////////////////////////////////////////////////
//日期类:
//Author:Joe
//Create Time:2004.7.28
#include "StdAfx.h"
#include "date.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


tagTime::tagTime(void)
{
	time_t t;
	time(&t);
	tm* pTime=localtime(&t);
	memcpy(&tmTime,pTime,sizeof(tm));
}

tagTime::tagTime(SYSTEMTIME sysTime)
{
	memset(&tmTime,0,sizeof(tm));
	tmTime.tm_sec = sysTime.wSecond;
	tmTime.tm_min = sysTime.wMinute;
	tmTime.tm_hour = sysTime.wHour;
	tmTime.tm_mday = sysTime.wDay;
	tmTime.tm_mon = sysTime.wMonth-1;
	tmTime.tm_year = sysTime.wYear-1900;
	tmTime.tm_wday = sysTime.wDayOfWeek;
}
tagTime::tagTime(const char* pszDate)
{
	memset(&tmTime,0,sizeof(tm));
	if(NULL == pszDate)	return;

	string strDate(pszDate);
	string str="0";
	long lPos=(long)strDate.find(":");
	str = strDate.substr(0,lPos);
	tmTime.tm_year=atoi((char*)str.c_str());

	strDate = strDate.substr(lPos+1,strDate.length()-lPos-1);
	str="0";
	lPos=(long)strDate.find(":");
	str = strDate.substr(0,lPos);
	tmTime.tm_mon=atoi((char*)str.c_str());

	strDate = strDate.substr(lPos+1,strDate.length()-lPos-1);
	str="0";
	lPos=(long)strDate.find(":");
	str = strDate.substr(0,lPos);
	tmTime.tm_mday=atoi((char*)str.c_str());

	strDate = strDate.substr(lPos+1,strDate.length()-lPos-1);
	str="0";
	lPos=(long)strDate.find(":");
	str = strDate.substr(0,lPos);
	tmTime.tm_hour=atoi((char*)str.c_str());

	strDate = strDate.substr(lPos+1,strDate.length()-lPos-1);
	str="0";
	lPos=(long)strDate.find(":");
	str = strDate.substr(0,lPos);
	tmTime.tm_min=atoi((char*)str.c_str());

	strDate = strDate.substr(lPos+1,strDate.length()-lPos-1);
	str="0";
	lPos=(long)strDate.find(":");
	str = strDate.substr(0,lPos);
	tmTime.tm_sec=atoi((char*)str.c_str());
}


//转换成SYSTEMTIME格式
void tagTime::ToSystemTime(SYSTEMTIME& sysTime)
{
	memset(&sysTime,0,sizeof(SYSTEMTIME));
	sysTime.wSecond = tmTime.tm_sec;
	sysTime.wMinute = tmTime.tm_min;
	sysTime.wHour = tmTime.tm_hour;
	sysTime.wDay = tmTime.tm_mday;
	sysTime.wMonth = tmTime.tm_mon+1;
	sysTime.wYear = tmTime.tm_year+1900;
	sysTime.wDayOfWeek = tmTime.tm_wday;
}

//得到日期格式化的字符串
const char* tagTime::GetDateFormatStr()
{
	static char str[256]="";
	_snprintf(str,256,"%d年%d月%d日%d时%d分%d秒",
		tmTime.tm_year+1900,tmTime.tm_mon+1,tmTime.tm_mday,tmTime.tm_hour,tmTime.tm_min,tmTime.tm_sec);
	return str;
}
//得到时间格式化的字符串
const char* tagTime::GetTimeFormatStr()
{
	static char str[256]="";
	_snprintf(str,256,"%d时%d分%d秒",tmTime.tm_hour,tmTime.tm_min,tmTime.tm_sec);
	return str;
}
//得到值字符串
const char* tagTime::GetValueFormatStr()
{
	static char str[256]="";
	_snprintf(str,256,"%d年%d月%d日%d时%d分%d秒",
		tmTime.tm_year,tmTime.tm_mon,tmTime.tm_mday,tmTime.tm_hour,tmTime.tm_min,tmTime.tm_sec);
	return str;
}

//返回时间差(秒值)
long	tagTime::GetSecondDiff(tagTime FutureTime)
{
	time_t	tTime1 = mktime(&tmTime);
	time_t	tTime2 = mktime(&FutureTime.tmTime);
	return (long)difftime(tTime2,tTime1);
}
//得到距离当前时间的差
tagTime tagTime::GetTimeDiff(tagTime FutureTime)
{
	tagTime TemptTime;
	memset(&TemptTime,0,sizeof(tagTime));
	long nDiffTime = GetSecondDiff(FutureTime);
	TemptTime.tmTime.tm_mday = nDiffTime/86400;
	nDiffTime = nDiffTime-TemptTime.tmTime.tm_mday*86400;
	TemptTime.tmTime.tm_hour = nDiffTime/3600;
	nDiffTime = nDiffTime-TemptTime.tmTime.tm_hour*3600;
	TemptTime.tmTime.tm_min = nDiffTime/60;
	TemptTime.tmTime.tm_sec = nDiffTime-TemptTime.tmTime.tm_min*60;
	return TemptTime;
}
//判断两个时间是否相等，精确到秒
bool tagTime::operator==(const tagTime& Time) const
{
	time_t	tTime1 = mktime((tm*)&tmTime);
	time_t	tTime2 = mktime((tm*)&Time.tmTime);
	return tTime1==tTime2;
}
bool tagTime::operator> (const tagTime& Time) const
{
	time_t	tTime1 = mktime((tm*)&tmTime);
	time_t	tTime2 = mktime((tm*)&Time.tmTime);
	return tTime1>tTime2;
}

bool tagTime::operator>= (const tagTime& Time) const
{
	time_t	tTime1 = mktime((tm*)&tmTime);
	time_t	tTime2 = mktime((tm*)&Time.tmTime);
	return tTime1>=tTime2;
}

bool tagTime::operator<(const tagTime& Time) const
{
	if(!operator>=(Time))
		return true;
	return false;
}

bool tagTime::operator<= (const tagTime& Time) const
{
	if( ! operator>(Time) )
		return true;
	return false;
}

tagTime& tagTime::AddSecond(long lSecond)
{
	if(lSecond==0) return *this;
	time_t	tTime = mktime((tm*)&tmTime);
	tTime += lSecond;
	tm* pTime=localtime(&tTime);
	memcpy(&tmTime,pTime,sizeof(tm));
	return *this;
}
tagTime& tagTime::AddMinute(long lMinute)
{
	if(lMinute==0) return *this;

	time_t	tTime = mktime(&tmTime);
	tTime += lMinute*60;
	tm* pTime=localtime(&tTime);
	memcpy(&tmTime,pTime,sizeof(tm));

	return *this;
}
tagTime& tagTime::AddHour(long lHour)
{
	if(lHour==0) return *this;

	time_t	tTime = mktime(&tmTime);
	tTime += lHour*3600;
	tm* pTime=localtime(&tTime);
	memcpy(&tmTime,pTime,sizeof(tm));
	return *this;
}
tagTime& tagTime::AddDay(long lDay)
{
	if(lDay==0) return *this;
	time_t	tTime = mktime(&tmTime);
	tTime += lDay*86400;
	tm* pTime=localtime(&tTime);
	memcpy(&tmTime,pTime,sizeof(tm));
	return *this;
}


//时间的值
long tagTime::Year()
{
	return tmTime.tm_year;
}
long tagTime::Month()
{
	return tmTime.tm_mon+1;
}
long tagTime::Day()
{
	return tmTime.tm_mday;
}
long tagTime::Hour()
{
	return tmTime.tm_hour;
}
long tagTime::Minute()
{
	return tmTime.tm_min;
}
long tagTime::Second()
{
	return tmTime.tm_sec;
}
long tagTime::Week()
{
	return tmTime.tm_wday;
}


long DiffSecond(tagTime& Time1,const tagTime& Time2)
{
	return Time1.GetSecondDiff(Time2);
}
//距离当前的时间差
tagTime DiffTime(tagTime& Time1,const tagTime& Time2)
{
	return Time1.GetTimeDiff(Time2);
}