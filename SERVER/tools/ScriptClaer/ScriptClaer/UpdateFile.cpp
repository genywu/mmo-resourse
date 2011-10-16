

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
		sprintf(strOut,"读取文件<%s>失败!",pFileName);
		::MessageBox(NULL,strOut,"提示",MB_OK);
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


const DWORD INVALID_IDX = 0xFFFFFFFF;

DWORD FindSub(const char *pTxt, DWORD TxtSize, const char *pSub)
{
	DWORD BeginPos = 0;
	DWORD SubSize = strlen(pSub);

	while (TxtSize - BeginPos >= SubSize)
	{
		

		if(pTxt[BeginPos] == pSub[0] && TxtSize - BeginPos >= SubSize)
		{
			bool bInHz = false;
			//! 判断是否再汉字中间
			if(127 < (UCHAR)pTxt[BeginPos - 1])
			{
				LONG lNum = 0;
				for(LONG lTmpIdx = BeginPos - 1; 0 <= lTmpIdx; --lTmpIdx)
				{
					if(127 > (UCHAR)pTxt[lTmpIdx])
						break;
					lNum++;
				}

				if(1 == (lNum % 2))
					bInHz = true;
			}

			if (!bInHz && 0 == memcmp(&pTxt[BeginPos], pSub, SubSize))
			{
				return BeginPos;
			}
		}
		++BeginPos;
	}

	return INVALID_IDX;
}

BOOL StrCmpChar(const char *pTxt, DWORD Pos, char CmpChar)
{
	if (pTxt[Pos] == CmpChar)
	{
		//! 判断是否再汉字中间
		if(0 < Pos && 127 < (UCHAR)pTxt[Pos - 1])
		{
			LONG lNum = 0;
			for(LONG lTmpIdx = Pos - 1; 0 <= lTmpIdx; --lTmpIdx)
			{
				if(127 > (UCHAR)pTxt[lTmpIdx])
					break;
				lNum++;
			}

			if(1 == (lNum % 2))
				return false;
		}

		return true;
	}

	return false;
}

BOOL CUpdateFile::WriteDataToPath(LPCSTR pDirPath) const
{
	if( pDirPath == NULL )
		return FALSE;
	//	无内容
	if( m_pContent == NULL )
		return FALSE;

	char szDirName[256];
	if('\\' == pDirPath[strlen(pDirPath) - 1])
		sprintf(szDirName,"%s%s",pDirPath,m_szName);
	else
		sprintf(szDirName,"%s\\%s",pDirPath,m_szName);
	//	文件句柄
	HANDLE hFile;
//	CreateDirectory(pDirPath,NULL);
	hFile = ::CreateFile(szDirName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
#ifdef USE_IN_CLIENT
		char strOut[128];
		sprintf(strOut,"写文件<%s>失败!",szDirName);
		::MessageBox(NULL,strOut,"提示",MB_OK);
#endif
		return FALSE;
	}

	//	这里开始写文件
	DWORD dwWritenSize = 0;

	//! 替换所有软回车为硬回车
	for (DWORD i = 0; i < m_dwSize; ++i)
	{
		if(StrCmpChar(m_pContent, i, '\r'))
			m_pContent[i] = '\n';
	}

	DWORD Pos = 0;



	while (Pos < m_dwSize)
	{
		//! 还原引号内所有内容
		if (StrCmpChar(m_pContent, Pos, '\"'))
		{
			DWORD EndPos = Pos + 1;
			do 
			{
				if (StrCmpChar(m_pContent, EndPos, '\"'))
					break;
				++EndPos;
			} while(EndPos < m_dwSize);

			++EndPos;
			WriteFile(hFile, &m_pContent[Pos], EndPos - Pos, &dwWritenSize, NULL);
			Pos = EndPos;
			continue;
		}
		//! 去掉//注释
		else if (Pos < m_dwSize && StrCmpChar(m_pContent, Pos, '/') && StrCmpChar(m_pContent, Pos + 1, '/'))
		{
			do 
			{
				++Pos;
			} while(!StrCmpChar(m_pContent, Pos, '\n') && !StrCmpChar(m_pContent, Pos, '\r'));

			continue;
		}
		//! 去掉/**/注释
		else if (Pos < m_dwSize && StrCmpChar(m_pContent, Pos, '/') && StrCmpChar(m_pContent, Pos + 1, '*'))
		{
			do 
			{
				++Pos;
				if (Pos < m_dwSize && StrCmpChar(m_pContent, Pos, '*') && StrCmpChar(m_pContent, Pos + 1, '/'))
				{
					Pos += 2;
					break;
				}
			} while(Pos < m_dwSize);

			continue;
		}
		//! 换行（连续多个保留一个）
		else if(StrCmpChar(m_pContent, Pos, '\n'))
		{
			while (Pos < m_dwSize && StrCmpChar(m_pContent, Pos + 1, '\n'))
			{
				++Pos;
			}
		}
		//! 空格（连续多个保留一个）
		else if(StrCmpChar(m_pContent, Pos, ' '))
		{
			while (Pos < m_dwSize && StrCmpChar(m_pContent, Pos + 1, ' '))
			{
				++Pos;
			}
		}
		//! 去掉缩进
		else if(StrCmpChar(m_pContent, Pos, '	'))
		{
			++Pos;
			continue;
		}

		WriteFile(hFile, &m_pContent[Pos++], 1, &dwWritenSize, NULL);
	}

	//	关闭句柄
	FlushFileBuffers(hFile);
	CloseHandle(hFile);

	printf("%s\r\n", szDirName);
	return TRUE;
}