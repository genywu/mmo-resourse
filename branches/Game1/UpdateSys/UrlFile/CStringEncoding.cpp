#include "stdafx.h"
#include "CStringEncoding.h"

/*
 * 功能: 得到代码页编号
 * 摘要: -
 * 参数: uiCharSet - 使用字符集
 * 返回值: 返回转换相对应的代码页
 * 作者: lpf
 * 创建日期: 2010.06.04
 * 修改日志:
 */
UINT CStringEncoding::GetCodePageFromCharset(UINT uiCharSet)
{
	switch (uiCharSet)
	{
	case SHIFTJIS_CHARSET:
		return 932;
	case HANGUL_CHARSET:
		return 949;
	case GB2312_CHARSET:
		return 936;
	case CHINESEBIG5_CHARSET:
		return 950;
	case GREEK_CHARSET:
		return 1253;
	case TURKISH_CHARSET:
		return 1254;
	case HEBREW_CHARSET:
		return 1255;
	case ARABIC_CHARSET:
		return 1256;
	case BALTIC_CHARSET:
		return 1257;
	case THAI_CHARSET:
		return 874;
	case EASTEUROPE_CHARSET:
		return 1250;
	default:
		return 1252;
	}
}



/*
 * 功能: ANSI编码到Unicode编码的转换
 * 摘要: -
 * 参数: s - 待转换的ANSI字符串
 * 返回值: 返回转换完成的宽字节字符串
 * 作者: lpf
 * 创建日期: 2010.06.04
 * 修改日志:
 */
wstring CStringEncoding::ANSIToUnicode(const string & s, UINT uiCharSet)
{
	int iLen = (int)s.length();
	if (iLen <= 0)
		return L"";

	int		  iUnicodeLen = MultiByteToWideChar(GetCodePageFromCharset(uiCharSet), 0, s.c_str(), -1, NULL, 0);
	wchar_t * pUnicode	  = new wchar_t[iUnicodeLen + 1];
	ZeroMemory(pUnicode, (iUnicodeLen + 1) * sizeof(wchar_t));

	MultiByteToWideChar(GetCodePageFromCharset(uiCharSet), 0, s.c_str(), -1, (LPWSTR)pUnicode, iUnicodeLen);
	wstring ws(pUnicode);
	delete [] pUnicode;

	return ws;
}



/*
 * 功能: Unicode编码到ANSI编码的转换
 * 摘要: -
 * 参数: ws - 待转换的Unicode字符串
 * 返回值: 返回转换完成的多字节字符串
 * 作者: lpf
 * 创建日期: 2010.06.04
 * 修改日志:
 */
string CStringEncoding::UnicodeToANSI(const wstring & ws, UINT uiCharSet)
{
	int iANSILen = WideCharToMultiByte(GetCodePageFromCharset(uiCharSet), 0, ws.c_str(), -1, NULL, 0, NULL, FALSE);
	if (iANSILen <= 0)
		return "";

	char * psz = new char[iANSILen + 1];
	ZeroMemory(psz, (iANSILen + 1) * sizeof(char));

	WideCharToMultiByte(GetCodePageFromCharset(uiCharSet), 0, ws.c_str(), -1, psz, iANSILen, NULL, FALSE);
	string s(psz);
	delete [] psz;

	return s;
}



/*
 * 功能: UTF8到Unicode编码的转换
 * 摘要: -
 * 参数: s - 待转换的utf8字符串
 * 返回值: 返回转换完成的宽字节字符串
 * 作者: lpf
 * 创建日期: 2010.06.04
 * 修改日志:
 */
wstring CStringEncoding::UTF8ToUnicode(const string & s, UINT uiCharSet)
{
	int iLen = (int)s.length();
	if (iLen <= 0)
		return L"";

	int iUnicodeLen	   = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
	wchar_t * pUnicode = new wchar_t[iUnicodeLen + 1];
	ZeroMemory(pUnicode, (iUnicodeLen + 1) * sizeof(wchar_t));

	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, (LPWSTR)pUnicode, iUnicodeLen);
	wstring ws(pUnicode);
	delete [] pUnicode;

	return ws;
}



/*
 * 功能: Unicode到UTF8编码的转换
 * 摘要: -
 * 参数: ws - 待转换的Unicode字符串
 * 返回值: 返回转换完成的多字节字符串
 * 作者: lpf
 * 创建日期: 2010.06.04
 * 修改日志:
 */
string CStringEncoding::UnicodeToUTF8(const wstring & ws, UINT uiCharSet)
{
	int iANSILen = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, NULL, 0, NULL, FALSE);
	if (iANSILen <= 0)
		return "";

	char * psz = new char[iANSILen + 1];
	ZeroMemory(psz, (iANSILen + 1) * sizeof(char));

	WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, psz, iANSILen, NULL, FALSE);
	string s(psz);
	delete [] psz;

	return s;
}



/*
 * 功能: UTF8到ANSI编码的转换
 * 摘要: -
 * 参数: s - 待转换的多字节字符串
 * 返回值: 返回转换完成的多字节字符串
 * 作者: lpf
 * 创建日期: 2010.06.07
 * 修改日志:
 */
string CStringEncoding::UTF8ToANSI(const string & s, UINT uiCharSet)
{
	wstring ws = UTF8ToUnicode(s, uiCharSet);
	return UnicodeToANSI(ws, uiCharSet);
}



/*
 * 功能: ANSI编码到Unicode编码的转换
 * 摘要: -
 * 参数: psz - 待转换的ANSI字符串
 * 返回值: 返回转换完成的宽字节字符串
 * 作者: lpf
 * 创建日期: 2010.06.04
 * 修改日志:
 */
wchar_t * CStringEncoding::ANSIToUnicode(const char * psz, UINT uiCharSet)
{
	int iLen = (int)strlen(psz);
	if (iLen <= 0)
		return NULL;

	int		  iUnicodeLen = MultiByteToWideChar(GetCodePageFromCharset(uiCharSet), 0, psz, -1, NULL, 0);
	wchar_t * pUnicode	  = new wchar_t[iUnicodeLen + 1];
	ZeroMemory(pUnicode, (iUnicodeLen + 1) * sizeof(wchar_t));

	MultiByteToWideChar(GetCodePageFromCharset(uiCharSet), 0, psz, -1, (LPWSTR)pUnicode, iUnicodeLen);
	return pUnicode;
}



/*
 * 功能: Unicode编码到ANSI编码的转换
 * 摘要: -
 * 参数: pwsz - 待转换的Unicode字符串
 * 返回值: 返回转换完成的多字节字符串
 * 作者: lpf
 * 创建日期: 2010.06.04
 * 修改日志:
 */
char * CStringEncoding::UnicodeToANSI(const wchar_t * pwsz, UINT uiCharSet)
{
	int iANSILen = WideCharToMultiByte(GetCodePageFromCharset(uiCharSet), 0, pwsz, -1, NULL, 0, NULL, FALSE);
	if (iANSILen <= 0)
		return NULL;

	char * psz = new char[iANSILen + 1];
	ZeroMemory(psz, (iANSILen + 1) * sizeof(char));

	WideCharToMultiByte(GetCodePageFromCharset(uiCharSet), 0, pwsz, -1, psz, iANSILen, NULL, FALSE);
	return psz;
}



/*
 * 功能: UTF8到Unicode编码的转换
 * 摘要: -
 * 参数: psz - 待转换的utf8字符串
 * 返回值: 返回转换完成的宽字节字符串
 * 作者: lpf
 * 创建日期: 2010.06.07
 * 修改日志:
 */
wchar_t * CStringEncoding::UTF8ToUnicode(const char * psz, UINT uiCharSet)
{
	int iLen = (int)strlen(psz);
	if (iLen <= 0)
		return NULL;

	int iUnicodeLen	   = MultiByteToWideChar(CP_UTF8, 0, psz, -1, NULL, 0);
	wchar_t * pUnicode = new wchar_t[iUnicodeLen + 1];
	ZeroMemory(pUnicode, (iUnicodeLen + 1) * sizeof(wchar_t));

	MultiByteToWideChar(CP_UTF8, 0, psz, -1, (LPWSTR)pUnicode, iUnicodeLen);

	return pUnicode;
}



/*
 * 功能: Unicode到UTF8编码的转换
 * 摘要: -
 * 参数: ws - 待转换的Unicode字符串
 * 返回值: 返回转换完成的多字节字符串
 * 作者: lpf
 * 创建日期: 2010.06.07
 * 修改日志:
 */
char * CStringEncoding::UnicodeToUTF8(const wchar_t * pwsz, UINT uiCharSet)
{
	int iANSILen = WideCharToMultiByte(CP_UTF8, 0, pwsz, -1, NULL, 0, NULL, FALSE);
	if (iANSILen <= 0)
		return NULL;

	char * psz = new char[iANSILen + 1];
	ZeroMemory(psz, (iANSILen + 1) * sizeof(char));

	WideCharToMultiByte(CP_UTF8, 0, pwsz, -1, psz, iANSILen, NULL, FALSE);

	return psz;
}



/*
 * 功能: UTF8到ANSI编码的转换
 * 摘要: -
 * 参数: psz - 待转换的多字节字符串
 * 返回值: 返回转换完成的多字节字符串
 * 作者: lpf
 * 创建日期: 2010.06.07
 * 修改日志:
 */
char * CStringEncoding::UTF8ToANSI(const char * psz, UINT uiCharSet)
{
	if ((int)strlen(psz) <= 0)
		return NULL;

	wchar_t * pwsz	 = UTF8ToUnicode(psz, uiCharSet);
	char	* pszOut = UnicodeToANSI(pwsz, uiCharSet);
	delete [] pwsz;
	return pszOut;
}