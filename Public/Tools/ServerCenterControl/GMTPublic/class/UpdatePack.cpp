


#include "stdafx.h"
#include "UpdatePack.h"

UpdatePack::UpdatePack(VOID)
:m_dwGappType(eGT_INVALID)
{
	memset(m_szGappVersion, 0, MAX_GAPP_NAME_SZIE);
	memset(m_szReadMe, 0, PACK_README_TEXT_SIZE);
}
UpdatePack::~UpdatePack(VOID)
{

}

//!	编码到数据集
BOOL UpdatePack::Code(DBWriteSet &setWriteDB)
{
	setWriteDB.AddToByteArray(m_dwGappType);
	setWriteDB.AddToByteArray(m_szGappVersion);
	setWriteDB.AddToByteArray(m_szReadMe);

	Code(setWriteDB, m_Root);
	return TRUE;
}
//! 从数据集解码
BOOL UpdatePack::DeCode(DBReadSet &setReadDB)
{
	m_dwGappType = setReadDB.GetDwordFromByteArray();
	setReadDB.GetStringFromByteArray(m_szGappVersion, MAX_GAPP_NAME_SZIE);
	setReadDB.GetStringFromByteArray(m_szReadMe, PACK_README_TEXT_SIZE);

	DeCode(setReadDB, m_Root);
	return TRUE;
}

//! 设置更新包的服务器类型和版本号
VOID UpdatePack::SetBaseInfo(DWORD dwGappType, LPCSTR pVersion, LPCSTR pReadMe)
{
	m_dwGappType = dwGappType;

	LONG lVersionSize = strlen(pVersion);
	lVersionSize = min(lVersionSize, MAX_GAPP_NAME_SZIE - 1);
	memcpy(m_szGappVersion, pVersion, lVersionSize);
	
	LONG lReadMeSize = strlen(pReadMe);
	lReadMeSize = min(lReadMeSize, PACK_README_TEXT_SIZE - 1);
	memcpy(m_szReadMe, pReadMe, lReadMeSize);
}
//! 获得服务器类型
DWORD UpdatePack::GetGappType(VOID)
{
	return m_dwGappType;
}
//! 获得版本号
LPCSTR UpdatePack::GetVersion(VOID)
{
	return m_szGappVersion;
}
//! 得到说明文本
LPCSTR UpdatePack::GetReadMe(VOID)
{
	return m_szReadMe;
}



//! 递归的编码接口
VOID UpdatePack::Code(DBWriteSet &setWriteDB, tagDirectory &Directory)
{
	setWriteDB.AddToByteArray(Directory.szName);

	DWORD dwFileNum = Directory.listFile.size();
	setWriteDB.AddToByteArray((DWORD)dwFileNum);
	list<CUpdateFile>::iterator iteFile = Directory.listFile.begin();
	for (; Directory.listFile.end() != iteFile; ++iteFile)
	{
		iteFile->Code(setWriteDB);
	}

	DWORD dwDirNum = Directory.listDir.size();
	setWriteDB.AddToByteArray((DWORD)dwDirNum);
	list<tagDirectory>::iterator iteDir = Directory.listDir.begin();
	for (; Directory.listDir.end() != iteDir; ++iteDir)
	{
		Code(setWriteDB, *iteDir);
	}
}
//! 递归的解码接口
VOID UpdatePack::DeCode(DBReadSet &setReadDB, tagDirectory &Directory)
{
	setReadDB.GetStringFromByteArray(Directory.szName, MAX_PATH);

	DWORD dwFileNum = setReadDB.GetDwordFromByteArray();
	for (DWORD i = 0; i < dwFileNum; ++i)
	{
		Directory.listFile.push_back(CUpdateFile(""));
		(-- Directory.listFile.end())->DeCode(setReadDB);
	}

	DWORD dwDirNum = setReadDB.GetDwordFromByteArray();
	for (DWORD i = 0; i < dwDirNum; ++i)
	{
		Directory.listDir.push_back(tagDirectory());
		DeCode(setReadDB, *(-- Directory.listDir.end()));
	}
}