
#include "stdafx.h"
#include "package.h"
#include "bitio.h"
#include "compress_lzw15.h"
#include "inputbitstream.h"
#include "outputbitstream.h"
#include <fstream>
#include "packagefile.h"




bool CPackageFile::Open(const char* pszFile, PackageFileInfo* info)
{
	assert(pszFile);
	assert(info);

	Close();

	m_pFile = fopen(pszFile, "rb");
	if (!m_pFile)
		return false;

	m_dwSize	= info->dwSize;
	m_dwRealSize = info->dwRealSize;
	m_dwStartOffset = info->dwStartOffset;
	m_dwEndOffset = info->dwEndOffset;


	m_pData = new BYTE[m_dwSize];
	if (!m_pFile)
		return false;

	fseek(m_pFile, m_dwStartOffset, SEEK_SET);
	fread(m_pData, m_dwSize, 1, m_pFile);
	fclose(m_pFile);

	if (info->dwCompress)
	{
		BYTE* pRealData = new BYTE[info->dwRealSize];

		//解压数据
		utility::MemFile::InputBitStream inputBits(m_pData,m_dwSize);

		utility::MemFile::Compress_LZW15 decoder;
		decoder.ExpandData(&inputBits,pRealData);

		//交换缓冲
		SAFEDELETE(m_pData);
		m_pData = pRealData;
	}

	m_dwPos = 0;
	return true;
}




DWORD CPackageFile::Read(void* pBuf, DWORD dwSize)
{
	assert(pBuf);

	if ( m_dwPos + dwSize > m_dwRealSize)
		dwSize = m_dwRealSize - m_dwPos;

	memcpy(pBuf, m_pData + m_dwPos, dwSize);

	m_dwPos += dwSize;
	return dwSize;
}



void CPackageFile::Seek(int nOffset, seek_dir dir)
{
	switch( dir )
	{
	case dir_beg:
		if ( nOffset < 0 )
			nOffset = 0;
		if ( nOffset > m_dwRealSize )
			nOffset = m_dwRealSize;

		m_dwPos = nOffset;
		break;

	case dir_cur:
		if ((int)m_dwPos + nOffset < 0 )
			nOffset = -m_dwPos;
		if ( m_dwPos + nOffset > m_dwRealSize )
			nOffset = m_dwRealSize - m_dwPos;
			
		m_dwPos += nOffset;
		break;

	case dir_end:
		if (nOffset > 0)
			nOffset = 0;
		if (nOffset + m_dwRealSize < 0 )
			nOffset = -m_dwRealSize;

		m_dwPos = m_dwRealSize + nOffset;
		break;

	default:
		break;
	}
}






















