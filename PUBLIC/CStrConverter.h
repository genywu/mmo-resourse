///////////////////////////////////////////////////////////
//  CStrConverter.h
//  实现对象：	Class CStrConverter
//  创建日期：	06-9月-2006 10:37:37
//  原始作者：	陈先进
///////////////////////////////////////////////////////////
#ifndef _CSTRING_CONVERTER
#define _CSTRING_CONVERTER
#include <String>
//#include "../server/GameServer/AppServer/states/AttackPower.h"



/// <summary>
/// 字符串转换类，将以字符串转化为特定的类型
/// </summary>
class CStrConverter
{		
public:
	//将以十六进制字符串转化为long
	static long parseStringToDec(const char *val);
	//将以字符串转化为int
	static int  parseInt(const char *val);
	//将以字符串转化为bool
	static bool parseBool(const char *val);
	//将字符串转化为long
	static long parseLong(const char *val);
	//将字符串转化为float
	static float parseFloat(const char *val);
	//将排列方式字符串转化为long
	static long parseRankStyle(const char *val);
	//将格式转化为long
	static long parseStyle(const char *val);
	/*  
	 *	作者:zhaohang(kaba)
	 *  创建日期: 2009/7/24
	 */ 
	//将Bool转化为字符串
	static const char* ParseBoolToString(const bool b);
	//zhahang 2009/11/18
	//处理有关中文的字符串截断,iSplitPos指定位置[1,val.length()-1],超出区间，左边直接返回原字符串，右边返回"",SplitStrRight返回指定位置右边的字符串,函数返回指定位置左边的字符串
	static string GetLeftSplitStr(const std::string &val,size_t SplitPos,string &SplitStrRight);
	static void parseList(char *val,list<long> &lTemp);
	//将格式转化为AttackType
	//static AttackType parseAttackType(const char *val);
	//
private:
	static bool CanSplit(const std::string &str,size_t pos);//判断是否可以在指定的位置截断字符串
};
#endif//_STRING_CONVERTER