#pragma once

#include <string>
#include <map>
using namespace std;

enum eVersion
{
	V_SCHINA,
	V_JAPAN,
	V_KOREA,
	V_TCHINA,
	V_RUSSIAN,
	V_ENGLISH,
};
enum eFileType
{
	TYPE_TEXT,
	TYPE_IMAGE,
	TYPE_OTHER,
};

// 全局参数
class CLrmSetup
{
public:
	CLrmSetup();
	~CLrmSetup();

public:
	static bool LoadDBTableConfig();
	static map<long,string> &GetDBTableConfig(){return m_mpTable;}
	static string GetTableName(long lID);
	static list<string> &GetRsType(){return m_lRSType;}
	static void  GetCountyEnum(long &lVersion);

private:
	static map<long,string> m_mpTable;
	static list<string>	m_lRSType;

};