///////////////////////////////////////////////////////////////////////////////
///  StrUnifyInput.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\PUBLIC
///
///  CREATED: 02/28/2008 13:15:01 PM by 陈先进
///
///  PURPOSE:   字符串统一读取
//////////////////////////////////////////////////////////////////////
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_STRINGREAD_INCLUDED
#define _INC_STRINGREAD_INCLUDED
#include "tinyxml.h"
#include "StrType.h"

struct tagString 
{
	//string strTypeName;		
	map<DWORD,string>	m_mapUniString;
};


class CStringReading
{

public:
	CStringReading();
	~CStringReading();


public:
	static bool InitUniString(string strFileName,string strNode,string strType="",DWORD dwID=0);
	static char* LoadString(DWORD dwTypeID,DWORD dwID);	
	static void LoadStringConfigration(TiXmlElement *pElem,string strType="",DWORD dwID=0);
	static void LoadStringConfigration(TiXmlElement *pElem,DWORD dwID=0);
	static void AddToByteArray(vector<BYTE>* pByteArray);
	static void DecordFromByteArray(BYTE* pByte, long& pos);
	static long GetStringCount(string strType);
	static DWORD GetDwordAttribute(TiXmlElement *pElem,string strName);
	static void	Release();
	
private:	
	static map<DWORD,tagString*> m_mapUniFile;
	typedef map<DWORD,tagString*>::iterator	itUniFile;	
};
#endif//_INC_STRINGREAD_INCLUDED