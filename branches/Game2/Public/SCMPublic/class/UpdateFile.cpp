

#include "stdafx.h"
#include "UpdateFile.h"

CUpdateFile::CUpdateFile(const char *pFileName)
:m_pContent(NULL)
,m_dwSize(0)
{
	memset(m_szName, 0, MAX_PATH);
	SetName(pFileName);
}
CUpdateFile::~CUpdateFile()
{
	if( m_pContent!=NULL)
		SAFE_DELETE_ARRAY(m_pContent);
}

CUpdateFile::CUpdateFile(const CUpdateFile &SrcObj)
:m_pContent(NULL)
,m_dwSize(0)
{
	strcpy(this->m_szName, SrcObj.m_szName);

	if(NULL != SrcObj.m_pContent)
	{
#ifndef IN_DEPTH_COPY
		throw;
#else
		if(NULL != this->m_pContent)
		{
			SAFE_DELETE_ARRAY(this->m_pContent);
		}

		this->m_dwSize = SrcObj.m_dwSize;
		this->m_pContent = new char[this->m_dwSize];
		memcpy(this->m_pContent, SrcObj.m_pContent, this->m_dwSize);
#endif
	}
}

CUpdateFile& CUpdateFile::operator=(const CUpdateFile &RightObj)
{
	new(this)CUpdateFile(RightObj);
	return (*this);
}

VOID CUpdateFile::SetName(LPCSTR pName)
{
	if(NULL != pName)
	{
		LONG lSize = strlen(pName);
		lSize = min((MAX_PATH - 1), lSize);
		memcpy(m_szName, pName, lSize);
		m_szName[lSize] = 0;
	}
}
BOOL CUpdateFile::LoadDataFromDir(LPCSTR pFileName)
{
	if( pFileName == NULL )
		return FALSE;
	//	文件句柄
	HANDLE hFile = NULL;
	hFile = ::CreateFile(pFileName,GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile==INVALID_HANDLE_VALUE)
	{
#ifdef USE_IN_CLIENT
		char strOut[128];
		sprintf(strOut,"Read File<%s>Failed!",pFileName);
		::MessageBox(NULL,strOut,"Tips",MB_OK);
#endif		
		CloseHandle(hFile);
		return FALSE;
	}
	else
	{
		//	读取的大小
		DWORD dwReadedSize;
		//	文件大小
		m_dwSize = GetFileSize(hFile,NULL);
		//	读文件内容
		if(NULL != m_pContent)
			SAFE_DELETE_ARRAY(m_pContent);
		m_pContent = new char[m_dwSize];
		ReadFile(hFile,m_pContent,m_dwSize,&dwReadedSize,NULL);

		CloseHandle(hFile);
		return TRUE;
	}
}

BOOL CUpdateFile::WriteDataToPath(LPCSTR pDirPath) const
{
	if( pDirPath == NULL )
		return FALSE;
	//	无内容
	if( m_pContent == NULL )
		return FALSE;

	char szDirName[256];
	sprintf(szDirName,"%s\\%s",pDirPath,m_szName);
	//	文件句柄
	HANDLE hFile;
//	CreateDirectory(pDirPath,NULL);
	hFile = ::CreateFile(szDirName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
#ifdef USE_IN_CLIENT
		char strOut[128];
		sprintf(strOut,"Write File <%s> Failed!",szDirName);
		::MessageBox(NULL,strOut,"Tips",MB_OK);
#endif
		return FALSE;
	}

	//	这里开始写文件
	DWORD dwWritenSize = 0;
	WriteFile(hFile, m_pContent,m_dwSize,&dwWritenSize, NULL);		
	FlushFileBuffers(hFile);

	//	关闭句柄
	CloseHandle(hFile);

	return TRUE;
}

//	解码
BOOL CUpdateFile::DeCode(DBReadSet &setReadDB)
{
	setReadDB.GetStringFromByteArray(m_szName, MAX_PATH);
	m_dwSize = setReadDB.GetDwordFromByteArray();
	m_pContent = new char[m_dwSize+1];
	setReadDB.GetBufferFromByteArray((void*)m_pContent,m_dwSize);
	m_pContent[m_dwSize]='\0';

	return TRUE;
}

//	编码
BOOL CUpdateFile::Code(DBWriteSet &setWriteDB) const
{
	//	文件名
	setWriteDB.AddToByteArray(m_szName);
	//	大小
	setWriteDB.AddToByteArray(m_dwSize);
	//	内容
	setWriteDB.AddToByteArray((void*)m_pContent,m_dwSize);

	return TRUE;
}