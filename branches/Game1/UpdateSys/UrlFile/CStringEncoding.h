/*
 * CStringEncoding.h
 * 功能: 字符串编码转换
 * 摘要: 使用windows API根据代码页对多字节和宽字节字符进行相互转换,以辅助多国家地区字符串编码操作
 *		 目前支持编码格式 : unicode,utf8,ansi
 * 作者: lpf
 * 创建日期: 2010-06-04
 */

#pragma once
using namespace std;

class CStringEncoding
{
public:
	static UINT	GetCodePageFromCharset(UINT uiCharSet);										//得到代码页编号

	static wstring ANSIToUnicode(const string & s, UINT uiCharSet = GB2312_CHARSET);		//ANSI编码到Unicode编码的转换
	static string  UnicodeToANSI(const wstring & ws, UINT uiCharSet = GB2312_CHARSET);		//Unicode编码到ANSI编码的转换
	static wstring UTF8ToUnicode(const string & s, UINT uiCharSet = GB2312_CHARSET);		//UTF8到Unicode编码的转换
	static string  UnicodeToUTF8(const wstring & ws, UINT uiCharSet = GB2312_CHARSET);		//Unicode到UTF8编码的转换
	static string  UTF8ToANSI(const string & s, UINT uiCharSet = GB2312_CHARSET);			//UTF8到ANSI编码的转换

	static wchar_t * ANSIToUnicode(const char * psz, UINT uiCharSet = GB2312_CHARSET);		//ANSI编码到Unicode编码的转换
	static char	   * UnicodeToANSI(const wchar_t * pwsz, UINT uiCharSet = GB2312_CHARSET);	//Unicode编码到ANSI编码的转换
	static wchar_t * UTF8ToUnicode(const char * psz, UINT uiCharSet = GB2312_CHARSET);		//UTF8到Unicode编码的转换
	static char	   * UnicodeToUTF8(const wchar_t * pwsz, UINT uiCharSet = GB2312_CHARSET);	//Unicode到UTF8编码的转换
	static char	   * UTF8ToANSI(const char * psz, UINT uiCharSet = GB2312_CHARSET);			//UTF8到ANSI编码的转换
};