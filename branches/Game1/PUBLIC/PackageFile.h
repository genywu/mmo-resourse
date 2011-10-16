#pragma once

#ifndef			__PACKAGEFILE_HEADER__
#define			__PACKAGEFILE_HEADER__

#include "tools.h"
#include <string>

using namespace std;

struct PackageFileInfo
{
	string		strFileName;
	DWORD		dwSize;
	DWORD		dwRealSize;
	DWORD		dwStartOffset;
	DWORD		dwEndOffset;
	DWORD		dwCompress;
};


class CPackage;

class CPackageFile
{
	friend CPackage;

	enum seek_dir{ dir_beg, dir_cur, dir_end };

public:
	CPackageFile()
	{
		m_pFile		= NULL;
		m_dwSize	= 0;
		m_dwRealSize	= 0;
		m_dwStartOffset	= 0;
		m_dwEndOffset	= 0;
		m_pData			= NULL;
		m_dwPos			= 0;
	}

	~CPackageFile()
	{
		Close();
	}


	void	Close()
	{
		if (m_pFile)
		{
			fclose(m_pFile);
			m_pFile	= NULL;
		}	

		m_dwPos = 0;
		SAFE_DELETE( m_pData);
	}	


	DWORD	Read( void* pBuf, DWORD dwSize);

	void	Seek(int nOffset, seek_dir dir);


	DWORD	GetSize()
	{
		return m_dwRealSize;
	}

private:
	bool	Open(const char* pszFile, PackageFileInfo* info);


private:
	FILE*		m_pFile;
	DWORD		m_dwSize;
	DWORD		m_dwRealSize;
	DWORD		m_dwStartOffset;
	DWORD		m_dwEndOffset;
	BYTE*		m_pData;
	DWORD		m_dwPos;
};


#endif		//__PACKAGEFILE_HEADER__