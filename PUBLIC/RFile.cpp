//////////////////////////////////////////////////////////////////////////
//RFile.cpp,资源文件类
//Fun:资源文件类句柄,负责读写操作
//Author:Joe
//Create Time:2004.11.26
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "rfile.h"
#include "ClientResource.h"
#include "FilesInfo.h"
#include "Package.h"
#include "tools.h"
#include "ziplib/lzo1x.h"
#include <string>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRFile::CRFile(const char* pszFileName,BYTE* pData,DWORD dwSize)
:m_pData(pData)
,m_dwSize(dwSize)
,m_dwPos(0)
{
	strncpy(m_pszFileName,pszFileName,MAX_PATH);
}

CRFile::CRFile(const char* pszFileName,FILE* pfile)
:m_pData(NULL)
,m_dwSize(0)
,m_dwPos(0)
{
	fseek(pfile, 0, SEEK_END);
	m_dwSize = ftell(pfile);
	fseek(pfile, 0, SEEK_SET);
	m_pData = new BYTE[m_dwSize+1];
	memset(m_pData,0,m_dwSize);
	int nNumRead = fread(m_pData,m_dwSize,1,pfile);
	m_pData[m_dwSize] = '\0';
	if( nNumRead != 1)
	{
		static char strErrorInfo[1024]="";
		_snprintf(strErrorInfo,1024,"读取文件(Name:%s,size:%d)出错.",pszFileName,m_dwSize);
		PutDebugString(strErrorInfo);		
	}
	strncpy(m_pszFileName,pszFileName,MAX_PATH);
}

CRFile::~CRFile(void)
{
	if(m_pData)
	{
		delete []m_pData;
	}
}

//读文件数据到缓冲区
bool CRFile::ReadData(void* pBuf,DWORD dwSize)
{
	assert(pBuf);	
	//包中文件
	if(m_pData)
	{
		if(m_dwPos+dwSize>m_dwSize)
			return false;
		memcpy(pBuf,m_pData+m_dwPos,dwSize);
		m_dwPos +=dwSize;
		return true;
	}
	return false;
}

//读数据到stringstream流
bool CRFile::ReadToStream(stringstream& stream)
{
	//包中文件
	if(m_pData)
	{
		stream << m_pData;
		//stream.write((char*)m_pData,m_dwSize);
		//stream.put((char*)m_pData,m_dwSize+1);
		stream.seekg(0, std::ios_base::beg);
		m_dwPos+=m_dwSize;
		return true;
	}
	return false;
}

//读int类新数据
bool CRFile::ReadData(int& nValue)
{
	return ReadData(&nValue,sizeof(int));
}

//读DWORD类型数据
bool CRFile::ReadData(DWORD& nValue)
{
	return ReadData(&nValue,sizeof(DWORD));
}

bool CRFile::ReadChar(unsigned char &c)
{
	return ReadData(&c,sizeof(unsigned char));
}

char * CRFile::ReadLine(char * pszLineBuffer, int iBufferLen)
{
	int  iLineLen = 0;
	BOOL bCount = FALSE;

	ZeroMemory(pszLineBuffer,iBufferLen);
	//从当前为止读取字符
	while(m_dwPos < m_dwSize && iLineLen < iBufferLen - 1)
	{
		char c = m_pData[m_dwPos++];
		if ( '\r' == c )
		{
			if (m_dwPos+1 < m_dwSize)
			{
				c = m_pData[m_dwPos+1];
				if ( '\n' == c )
				{
					m_dwPos++;
				}
			}
			break;
		}
		else
			if ( '\n' == c )
			{
				break;
			}

			pszLineBuffer[iLineLen++] = c;

	}
	return pszLineBuffer;
}

DWORD  CRFile::Seek(int iPosition,int iOrigin)
{
	DWORD dwCurPosition = m_dwPos;
	switch(iOrigin)
	{
	case SEEK_CUR:
		m_dwPos +=iPosition;
		break;
	case SEEK_END:
		assert(iPosition<0);
		assert( abs(iPosition)< (int)m_dwSize  );
		m_dwPos  = m_dwSize + iPosition;// 必须为负数
		break;
	case SEEK_SET:
	default:
		m_dwPos= iPosition;
	}
	return dwCurPosition;
}

////////////////////////////////////////////////
//功能：取得文件路径
///////////////////////////////////////////////
//不带"/" 和 "\\"
char *CRFile::GetFilePath(const char *szFileName, char* szOutput)
{
	int iLen=lstrlen(szFileName);
	strcpy(szOutput, szFileName);

	for(int i = iLen-1; i >= 0; i--)
	{
		szOutput[i]=0;

		if( szFileName[i] == '\\' || szFileName[i] == '/' )
			break;		
	}
	if (szOutput[0] == 0)
	{
		szOutput[0] = '.';
		szOutput[1] = 0;
	}

	return szOutput;
}

////用相对路径打开文件,必须用rfClose关闭
//CRFile* rfOpen(const char* strFileName, CClientResource* pClientResource)
//{
//	CRFile* rfile=NULL;
//	DWORD dwSize = 0;
//	
//	if (pClientResource == NULL)
//		pClientResource = GetDefaultClientResource();
//
//	if(pClientResource)
//	{
//		string strName(strFileName);
//		CheckRFileStr(strName);
//		//资源信息列表中有该文件
//		tagFileInfo* ptagFileInfo =
//			pClientResource->GetFilesInfo()->GetFileInfoByText(strName);
//		if(ptagFileInfo)
//		{
//			//在包中
//			if( !ENABLEBIT( ptagFileInfo->dwPackageType,NOPACK) )
//			{
//				CPackage *pPackage = pClientResource->GetPackage(ptagFileInfo->dwPackageType);
//				if(pPackage)
//				{
//					dwSize = pPackage->GetFileSize(strName.c_str());
//					if(dwSize>0)
//					{
//						BYTE* pBuf = new BYTE[dwSize+1];
//						DWORD dwCrc32,dwCompress,dwOrginSize, dwValidSize;
//						pPackage->ExtractToBuf(strName.c_str(),
//							pBuf,dwSize,dwCrc32,dwCompress, dwOrginSize, dwValidSize);
//
//						//解压
//						if( !ENABLEBIT( dwCompress,NOCOMPRESS) )
//						{
//							DWORD dwOutSize = ptagFileInfo->dwOrginSize+1;
//							BYTE* pOutBuf = new BYTE[dwOutSize];
//							if( ENABLEBIT(dwCompress,0x4) )
//								DeCompress(pBuf, dwSize, pOutBuf, &dwOutSize);
//							else
//								DeCompressData(pBuf, dwSize, pOutBuf, &dwOutSize);
//
//							pOutBuf[dwOutSize]='\0';
//							rfile = new CRFile(pOutBuf,dwOutSize);
//							delete[] pBuf;
//						}
//						else
//						{
//							pBuf[dwSize]='\0';
//							rfile = new CRFile(pBuf,dwSize);
//						}
//					}
//				}
//			}
//			//没有在包中的数据
//			else
//			{
//				string strTempt(pClientResource->GetCurFolder());
//				strTempt +=strName;
//
//				FILE* pf = fopen(strTempt.c_str(),"rb");
//				if(pf)
//				{
//					fseek(pf, 0, SEEK_END);
//					dwSize = ftell(pf);
//					fseek(pf, 0, SEEK_SET);
//					rfile = new CRFile(pf,dwSize);
//				}
//			}
//		}
//		//从外部读文件
//		else
//		{
//			string strTempt(pClientResource->GetCurFolder());
//			strTempt +=strName;
//
//			FILE* pf = fopen(strTempt.c_str(),"rb");
//			if(pf)
//			{
//				fseek(pf, 0, SEEK_END);
//				dwSize = ftell(pf);
//				fseek(pf, 0, SEEK_SET);
//				rfile = new CRFile(pf,dwSize);
//			}
//		}
//	}
//	return rfile;
//}
//
////关闭文件句柄
//void rfClose(CRFile* rfile)
//{
//	if(rfile)
//	{
//		delete rfile;
//	}
//}

void CheckRFileStr(string& strFileName)
{
	static string strReplace("\\");
	static string strEmpty("");
	char *newstring = _strlwr( _strdup( strFileName.c_str() ));
	strFileName = newstring;
	free(newstring);

	int nPos = strFileName.find_first_of("/");
	while(nPos>=0)
	{
		strFileName.replace(nPos,1,strReplace);
		int nPos1 = strFileName.find_first_of("/");
		if(nPos == nPos1-1)
		{
			strFileName.replace(nPos1,1,strEmpty);
			nPos = strFileName.find_first_of("/");
		}
		else
			nPos = nPos1;
	}

	if(strFileName.find_first_of("\\") != 0)
	{
		strFileName="\\"+strFileName;
	}
}


// 缺省的客户端资源
CClientResource* g_pDefaultClientResource = NULL;

CClientResource* GetDefaultClientResource()
{
	return g_pDefaultClientResource;
}

void SetDefaultClientResource(CClientResource* p)
{
	g_pDefaultClientResource = p;
}
