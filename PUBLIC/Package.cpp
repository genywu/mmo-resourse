
#include "stdafx.h"
#include "package.h"
#include "Crc32Static.h"
//#include "bitio.h"
//#include "compress_lzw15.h"
//#include "inputbitstream.h"
//#include "outputbitstream.h"
#include <fstream>
#include <assert.h>

#include "ClientResource.h"
#include "FilesInfo.h"

#include "ziplib/lzo1x.h"
#include "ziplib/zlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void PutDebugString(const char *msg);


// Lzo1x compress
void CompressData(const unsigned char* pInData,unsigned long lInDataSize,
				  unsigned char* pOutBuf,unsigned long* plOutDataSize,bool bFast){

	lzo_init();

	if(bFast){
		lzo_byte *lzowrkmem =(lzo_bytep) new unsigned char[LZO1X_1_MEM_COMPRESS]; //(lzo_bytep)_aligned_malloc(LZO1X_1_MEM_COMPRESS,16384);
		lzo1x_1_compress(pInData,lInDataSize,pOutBuf,(lzo_uint*)plOutDataSize,lzowrkmem);
		delete[] lzowrkmem;
		//_aligned_free(lzowrkmem);
	}
	else{
		lzo_byte *lzowrkmem =(lzo_bytep) new unsigned char[LZO1X_999_MEM_COMPRESS];//(lzo_bytep)_aligned_malloc(LZO1X_999_MEM_COMPRESS,16384);
		lzo1x_999_compress(pInData,lInDataSize,pOutBuf,(lzo_uint*)plOutDataSize,lzowrkmem);
		delete[] lzowrkmem;
		//_aligned_free(lzowrkmem);
	}
}

// Lzo1x uncompress
void DeCompressData(const unsigned char * pInData,unsigned long lInDataSize,
					unsigned char * pOutBuf,unsigned long* lOutDataSize)
{
	lzo_init();
	lzo1x_decompress(pInData,lInDataSize,pOutBuf,(lzo_uint*)lOutDataSize,NULL);
	//assert(LZO_E_OK==lzo1x_decompress_safe(pInData,lInDataSize,pOutBuf,(lzo_uint*)lOutDataSize,NULL));
}

// zip compress
int Compress(const unsigned char* pInData,unsigned long lInDataSize,
			 unsigned char* pOutBuf,unsigned long* plOutDataSize,int level)
{
	return compress2(pOutBuf,plOutDataSize, pInData,lInDataSize, level);
}

// zip uncompress
int  DeCompress(const unsigned char * pInData,unsigned long lInDataSize,
				unsigned char * pOutBuf,unsigned long* lOutDataSize)
{
	return uncompress(pOutBuf, lOutDataSize, pInData, lInDataSize);
}


CPackage::CPackage(CClientResource* pClientResource,string& strWorkPath,
				   DWORD dwMaxIndexNum,DWORD dwMaxEmptyIndexNum)
{ 
	m_pOriginResource = pClientResource;
	m_strWorkPath=strWorkPath;m_pFile = NULL ;

	m_dwMaxIndexNum = dwMaxIndexNum;
	m_dwMaxEmptyIndexNum = dwMaxEmptyIndexNum;
	//10W个文件
	m_dwIndexHeadSize = sizeof(tagFileIndex)*m_dwMaxIndexNum;
	m_dwEmptyIndexHeadSize = sizeof(tagFileIndex)*m_dwMaxEmptyIndexNum;

	//写文件数据的偏移量
	m_dwOffset = 4+4+4+m_dwIndexHeadSize+4+4+m_dwEmptyIndexHeadSize;
}
CPackage::~CPackage()
{
	CloseFileHandle();
}

void	CPackage::OpenFileHandle(const char* pszPackage)
{
	string strFullFileName = m_strWorkPath+"\\"+pszPackage+".dp";
	m_pFile = fopen(strFullFileName.c_str(), "r+b");

	if(!m_pFile)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "打开文件 %s 出错。",pszPackage);
		PutDebugString(str);
	}
	m_strPackName = pszPackage;
}

void	CPackage::CloseFileHandle()
{
	if (m_pFile)
	{
		fclose(m_pFile) ;
		m_pFile=NULL;
	}
}

bool	CPackage::OpenForUpdate( const char* pszPackage)
{
	string strFullFileName = m_strWorkPath+"\\"+pszPackage+".dp";
	m_pFile = fopen(strFullFileName.c_str(), "r+b");

	if(!m_pFile)
	{
		return Create(pszPackage);
	}
	if( Open(pszPackage,false) == false )
	{
		return Create(pszPackage);
	}
	return true;
}

bool CPackage::Open(const char* pszPackage ,bool bUpdate)
{
	CloseFileHandle();
	OpenFileHandle(pszPackage);

	if (!m_pFile)
		return false;

	try
	{
		fseek(m_pFile, 0, SEEK_SET);
		//读写数据的地址偏移量
		fread(&m_dwOffset, sizeof(DWORD), 1, m_pFile);
		//读文件索引头大小
		fread(&m_dwIndexHeadSize, sizeof(DWORD), 1, m_pFile);
		m_dwMaxIndexNum = m_dwIndexHeadSize/sizeof(tagFileIndex);
		//读文件索引数
		DWORD dwIndexCount;
		fread(&dwIndexCount, sizeof(DWORD), 1, m_pFile);
		//判断文件最大索引数量
		if(dwIndexCount > m_dwMaxIndexNum) return false;

		m_Indexs.clear();
		for (int i = 0; i < dwIndexCount; ++i)
		{
			tagFileIndex index;
			fread(&index, sizeof(tagFileIndex), 1, m_pFile);
			Crypto(&index, sizeof(index));

			_strlwr(index.szName );

			m_Indexs[index.szName] = index;
			if(bUpdate && m_pOriginResource)
			{
				string strName(index.szName);
				m_pOriginResource->GetFilesInfo()->UpdateFileInfo(	strName,index.dwCrc32,
																	index.dwSize,index.dwOrginSize,
																	index.dwValidSize,(DWORD)1,
																	index.dwCompressType);
			}
		}

		fseek(m_pFile, 4+4+4+m_dwIndexHeadSize, SEEK_SET);
		//读空文件索引头大小
		fread(&m_dwEmptyIndexHeadSize,sizeof(DWORD), 1, m_pFile);
		m_dwMaxEmptyIndexNum = m_dwEmptyIndexHeadSize/sizeof(tagFileIndex);
		//读取空索引数量
		fread(&dwIndexCount, sizeof(DWORD), 1, m_pFile);
		if(dwIndexCount>m_dwMaxEmptyIndexNum)
			return false;
		m_Emptys.clear();
		for (int i = 0; i < dwIndexCount; ++i)
		{
			tagFileIndex Empty;
			fread(&Empty, sizeof(tagFileIndex), 1, m_pFile);
			Crypto(&Empty, sizeof(Empty));
			m_Emptys.insert(Empty);
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}




bool CPackage::Create(const char* pszPackage)
{
	string strFullFileName = m_strWorkPath+"\\"+pszPackage+".dp";
	m_pFile = fopen(strFullFileName.c_str(), "w+b");
	if (m_pFile)
	{
		m_Indexs.clear();
		m_Emptys.clear();

		//10W个文件
		m_dwIndexHeadSize = sizeof(tagFileIndex)*m_dwMaxIndexNum;
		m_dwEmptyIndexHeadSize = sizeof(tagFileIndex)*m_dwMaxEmptyIndexNum;

		//写文件数据的偏移量
		m_dwOffset = 4+4+4+m_dwIndexHeadSize+4+4+m_dwEmptyIndexHeadSize;

		WriteIndexEx();

		m_strPackName = pszPackage;
		return true;
	}
	else
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "创建文件 %s 出错。",pszPackage);
		PutDebugString(str);
	}
	return false;
}

//清除索引和文件中的数据
bool	CPackage::ClearData()
{
	m_Indexs.clear();
	m_Emptys.clear();
	
	//10W个文件
	m_dwIndexHeadSize = sizeof(tagFileIndex)*m_dwMaxIndexNum;
	m_dwEmptyIndexHeadSize = sizeof(tagFileIndex)*m_dwMaxEmptyIndexNum;
	//写文件数据的偏移量
	m_dwOffset = 4+4+4+m_dwIndexHeadSize+4+4+m_dwEmptyIndexHeadSize;

	fseek(m_pFile,m_dwOffset,SEEK_SET);
	fflush(m_pFile);

	WriteIndexEx();
	return true;
}

DWORD CPackage::AddFile(const char* pszFile,DWORD &dwCrc32,
						DWORD dwCompress, DWORD &dwOrginSize)
{
	int nResult;
	tagFileIndex index;
	index.dwCrc32 = dwCrc32;
	strcpy_s(index.szName, 256, pszFile);
	string strFullFileName = m_strWorkPath+pszFile;

	FILE* fp = fopen(strFullFileName.c_str(), "rb");
	if(!fp)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "打开文件 %s 出错。",pszFile);
		PutDebugString(str);
		return 0;
	}
	assert(fp);
	if(!fp)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "fopen文件 %s 出错。",pszFile);
		PutDebugString(str);
		return 0;
	}

	nResult = fseek(fp, 0, SEEK_END);
	if(nResult)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "fseek文件 %s 出错。",pszFile);
		PutDebugString(str);
		return 0;
	}


	nResult = index.dwSize = ftell(fp);
	if(nResult == -1L)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "ftell文件 %s 出错。",pszFile);
		PutDebugString(str);
		return 0;
	}

	dwOrginSize = index.dwSize;

	nResult = fseek(fp, 0, SEEK_SET);
	if(nResult)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "fseek文件 %s 出错。",pszFile);
		PutDebugString(str);
		return 0;
	}
	
	if ( !ENABLEBIT(dwCompress,NOCOMPRESS) )
	{
		index.dwCompressType = dwCompress;
		if(index.dwSize > 0)
		{
			BYTE *lpszIn = new BYTE[index.dwSize];
			BYTE *lpszOut = new BYTE[index.dwSize*2];
			fread(lpszIn, index.dwSize, 1, fp);
			DWORD nOutSize = index.dwSize*2;

			if( ENABLEBIT(dwCompress,0x4) )
				Compress(lpszIn, index.dwSize, lpszOut, &nOutSize, false);
			else
				CompressData(lpszIn, index.dwSize, lpszOut, &nOutSize, false);
			

			index.dwOrginSize = index.dwSize;
			dwOrginSize = index.dwOrginSize;
			index.dwSize = nOutSize;
			index.dwValidSize = nOutSize;
			//CCrc32Static::DataCrc32(lpszOut, nOutSize, index.dwCrc32);
			//dwCrc32 = index.dwCrc32;

			nResult = WriteData(index,lpszOut);

			delete []lpszIn;
			delete []lpszOut;
		}
		else
		{
			index.dwSize=0;
			BYTE* pBuf = NULL;
			index.dwOrginSize = index.dwSize;
			index.dwValidSize = index.dwSize;
			nResult = WriteData(index,pBuf);
		}
	}
	else
	{
		index.dwCompressType = 1;
		BYTE* pBuf = new BYTE[index.dwSize];
		fread(pBuf, index.dwSize, 1, fp);
		index.dwOrginSize = index.dwSize;
		index.dwValidSize = index.dwSize;
		nResult = WriteData(index,pBuf);
		delete []pBuf;
	}

	fclose(fp);	
	if(nResult == false)
		return false;
	return index.dwSize;
}


DWORD	CPackage::AddFile(FILE* pf ,const char* strFileName,DWORD &dwCrc32,
						  DWORD dwCompress, DWORD &dwOrginSize)
{
	if(pf==NULL)	return 0;

	int nRet=0;
	tagFileIndex index;
	index.dwCrc32 = dwCrc32;
	strcpy_s(index.szName, 256, strFileName);

	nRet = fseek(pf, 0, SEEK_END);
	if(nRet)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "fseek文件 %s 出错。",strFileName);
		PutDebugString(str);
		return 0;
	}

	nRet = index.dwSize = ftell(pf);
	if(nRet == -1L)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "ftell文件 %s 出错。",strFileName);
		PutDebugString(str);
		return 0;
	}

	dwOrginSize = index.dwSize;

	nRet = fseek(pf, 0, SEEK_SET);
	if(nRet)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "fseek文件 %s 出错。",strFileName);
		PutDebugString(str);
		return 0;
	}

	if( !ENABLEBIT(dwCompress,NOCOMPRESS) )
	{
		index.dwCompressType = dwCompress;	

		if(index.dwSize>0)
		{
			BYTE *lpszIn = new BYTE[index.dwSize];
			BYTE *lpszOut = new BYTE[index.dwSize*2];
			fread(lpszIn, index.dwSize, 1, pf);
			DWORD nOutSize = index.dwSize*2;
			
			if( ENABLEBIT(dwCompress,0x4) )
				Compress(lpszIn, index.dwSize, lpszOut, &nOutSize, 1);
			else
				CompressData(lpszIn, index.dwSize, lpszOut, &nOutSize, false);


			index.dwOrginSize=index.dwSize;
			dwOrginSize = index.dwOrginSize;
			index.dwSize = nOutSize;
			index.dwValidSize = nOutSize;
			//CCrc32Static::DataCrc32(lpszOut, nOutSize, index.dwCrc32);
			//dwCrc32 = index.dwCrc32;

			nRet = WriteData(index,lpszOut);

			delete []lpszIn;
			delete []lpszOut;
		}

		else
		{
			index.dwSize=0;
			BYTE* pBuf = NULL;
			index.dwOrginSize = index.dwSize;
			index.dwValidSize = index.dwSize;
			nRet = WriteData(index,pBuf);
		}
	}
	else
	{
		index.dwCompressType = 1;
		BYTE* pBuf = new BYTE[index.dwSize];
		fread(pBuf, index.dwSize, 1, pf);

		index.dwOrginSize = index.dwSize;
		index.dwValidSize = index.dwSize;
		nRet = WriteData(index,pBuf);
		delete []pBuf;
	}

	if(nRet == false)
		return false;
	return index.dwSize;
}

//添加无压缩的区间
DWORD	CPackage::AddFile(const char* strFileName,int nSize,DWORD dwCrc32,
						  DWORD dwCompress, DWORD dwOrginSize)
{
	tagFileIndex index;
	index.dwCrc32 = dwCrc32;
	strcpy_s(index.szName, 256, strFileName);
	index.dwCompressType=dwCompress;
	index.dwSize = nSize;
	index.dwOrginSize = dwOrginSize;
	index.dwValidSize = 0;

	BYTE* pBuf = new BYTE[index.dwSize];
	memset(pBuf,0,index.dwSize);

	index.dwValidSize = 0;
	int nRet =WriteData(index,pBuf);

	delete []pBuf;
	if(nRet == false)
		return false;
	return index.dwSize;
}
//向无压缩的数据中写数据
DWORD	CPackage::AddFileData(const char* strFileName,BYTE* pBuf,DWORD& nValidSize,
							  DWORD nSize)
{
	int nRet;
	itIndex it = m_Indexs.find(strFileName);
	if(it == m_Indexs.end())	return 0;
	tagFileIndex& index = (*it).second;

	//文件超过大小限制
	//index.dwValidSize = nValidSize;
	if(index.dwValidSize+nSize>index.dwSize)	return 0;
	//index.dwCompressType = dwCompress;

	DWORD dwOffset = index.dwOffset+index.dwValidSize;
	nRet = fseek(m_pFile,dwOffset,SEEK_SET);
	if(nRet != 0)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "fseek文件 %s 出错。",strFileName);
		PutDebugString(str);
		return 0;
	}

	nRet = fwrite(pBuf, nSize, 1, m_pFile);
	if(nRet != 1)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "fwrite文件 %s 出错。",strFileName);
		PutDebugString(str);
		return 0;
	}

	nRet = fflush(m_pFile);
	if(nRet != 0)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "fflush文件 %s 出错。",strFileName);
		PutDebugString(str);
		return 0;
	}
	index.dwValidSize += nSize;
	nValidSize = index.dwValidSize;
	return index.dwValidSize;
}
//删除包中文件
DWORD	CPackage::DelFile(const char* pszFile)														
{
	itIndex it = m_Indexs.find(pszFile);
	if(it == m_Indexs.end())	return 0;

	//增加空区间
	tagFileIndex temptFileIndex = {"",(*it).second.dwOffset,(*it).second.dwSize,0,0,0};
	AddEmptyPart(temptFileIndex);
	m_Indexs.erase(it);
	return 0;
}
//从包中提取文件加入该包 
DWORD CPackage::AddFileFromPackage(CPackage* pPackage,const char* pszFile ,DWORD dwCompress)
{
	if(pPackage==NULL || pszFile==NULL)	return 0;
	return 0;
}

void CPackage::Crypto(void* pBuf, DWORD dwSize)
{
	for (int i = 0; i < dwSize; ++i)
	{
		((BYTE*)pBuf)[i] ^= 0xFF;
	}
}

DWORD CPackage::ExtractToFile(const char* pszFile,string strFolder)
{
	DWORD dwSize = 0;
	if(m_pFile==NULL || pszFile == NULL)	return dwSize;
	
	itIndex it = m_Indexs.find(pszFile);
	if(it == m_Indexs.end())	return dwSize;

	tagFileIndex *pIndex = &(*it).second;

	int nRet = fseek(m_pFile,pIndex->dwOffset,SEEK_SET);
	if(nRet != 0)
	{
		char str[100]="fseek包文件出错。";
		PutDebugString(str);
		return 0;
	}

	BYTE* pBuf = new BYTE[pIndex->dwSize];
	nRet = fread(pBuf, pIndex->dwSize, 1, m_pFile);
	if(nRet != 1)
	{
		char str[100]="读包文件出错。";
		PutDebugString(str);
		delete []pBuf;
		return 0;
	}

	string strTemptFolder=strFolder;
	if(strTemptFolder=="")
	{
		strTemptFolder = m_strWorkPath;
		strTemptFolder += pIndex->szName;	
	}
	else
	{
		strTemptFolder = strFolder;
	}

	FILE* pf = fopen(strTemptFolder.c_str(),"wb");
	if(pf)
	{
		if( !ENABLEBIT(pIndex->dwCompressType,NOCOMPRESS) )
		{
			if(pIndex->dwSize > 0)
			{
				BYTE* pOutBuf = new BYTE[pIndex->dwOrginSize+1];
				DWORD dwOutSize=pIndex->dwOrginSize+1;

				if( ENABLEBIT(pIndex->dwCompressType,0x4) )
					DeCompress(pBuf, pIndex->dwSize, pOutBuf, &dwOutSize);
				else
					DeCompressData(pBuf, pIndex->dwSize, pOutBuf, &dwOutSize);

				nRet = fwrite(pOutBuf, dwOutSize, 1, pf);
				if(nRet != 1)
				{
					char str[MAX_PATH]="";
					_snprintf(str, MAX_PATH, "fwrite文件 %s 出错。",strTemptFolder.c_str());
					PutDebugString(str);
					delete []pBuf;
					return 0;
				}

				delete []pOutBuf;
				fflush(m_pFile);
				dwSize = dwOutSize;
			}
		}
		else
		{
			int nRet = fwrite(pBuf, pIndex->dwSize, 1, pf);
			if(nRet != 1)
			{
				char str[100]="fwrite包文件出错。";
				PutDebugString(str);
				delete []pBuf;
				return 0;
			}
			nRet = fflush(m_pFile);
			if(nRet != 0)
			{
				char str[100]="fflush包文件出错。";
				PutDebugString(str);
				delete []pBuf;
				return 0;
			}
			dwSize=pIndex->dwSize;
		}
		fclose(pf);
	}
	else
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "打开文件 %s 出错。",strTemptFolder.c_str());
		PutDebugString(str);
		return 0;
	}

	delete []pBuf;
	return dwSize;
}

//提取到缓存
bool CPackage::ExtractToBuf(const char* pszFile,BYTE *& pBuf,int iSize,
							DWORD& dwCrc32,DWORD& dwCompressType,
							DWORD &dwOrginSize, DWORD &dwValidSize)
{
	itIndex it = m_Indexs.find(pszFile);
	if(it == m_Indexs.end())	return false;
	if((*it).second.dwSize > iSize)	return false;

	dwCompressType = (*it).second.dwCompressType;
	dwCrc32 = (*it).second.dwCrc32;
	dwValidSize = (*it).second.dwValidSize;
	dwOrginSize = (*it).second.dwOrginSize;

	int nRet = fseek(m_pFile,(*it).second.dwOffset,SEEK_SET);
	if(nRet != 0)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "fseek文件 %s 出错。",pszFile);
		PutDebugString(str);
		return 0;
	}
	nRet = fread(pBuf, (*it).second.dwSize, 1, m_pFile);
	if(nRet != 1)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "fread文件 %s 出错。",pszFile);
		PutDebugString(str);
		return 0;
	}

	return true;
}

//提取到缓存
bool	CPackage::ExtractToBuf(const char* pszFile,BYTE *& pBuf,
							   int iSize,tagFileIndex& FileIndex)
{
	itIndex it = m_Indexs.find(pszFile);
	if(it == m_Indexs.end())	return false;
	if((*it).second.dwSize > iSize)	return false;

	FileIndex = (*it).second;
	int nRet = fseek(m_pFile,(*it).second.dwOffset,SEEK_SET);
	if(nRet != 0)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "fseek文件 %s 出错。",pszFile);
		PutDebugString(str);
		return 0;
	}
	nRet = fread(pBuf, (*it).second.dwSize, 1, m_pFile);
	if(nRet != 1)
	{
		char str[MAX_PATH]="";
		_snprintf(str, MAX_PATH, "fread文件 %s 出错。",pszFile);
		PutDebugString(str);
		return 0;
	}
	return true;
}

//得到文件大小
long CPackage::GetFileSize(const char* pszFile)
{
	itIndex it = m_Indexs.find(pszFile);
	if(it == m_Indexs.end())
		return 0;
	else
		return (*it).second.dwSize;
}

bool	CPackage::GetFirstFileInfo(tagFileIndex& FileIndex)
{
	if(m_Indexs.size() == 0)	return false;
	FileIndex = m_Indexs.begin()->second;
	return true;
}

void	CPackage::Release()
{
	fclose(m_pFile);
}

//添加一个空区间
void	CPackage::AddEmptyPart(tagFileIndex& FileIndex)
{
	if(m_Emptys.size() < m_dwMaxEmptyIndexNum)
		m_Emptys.insert(FileIndex);
}

//根据大小得到一个空区间
bool	CPackage::GetEmptyPart(DWORD dwSize,tagFileIndex& FileIndex)
{
	itEmpty it =m_Emptys.begin();
	for(;it != m_Emptys.end();it++)
	{
		if((*it).dwSize>=dwSize)
		{
			FileIndex = (*it);
			m_Emptys.erase(it);
			return true;
		}
	}
	return false;
}


//写文件和空区间索引
//bool CPackage::WriteIndex()
//{
//	if(!m_pFile)
//		return false;
//
//	DWORD nOffset = m_dwOffset;
//	DWORD nIndexStartOff=nOffset;
//	int nRet = fseek(m_pFile, nOffset, SEEK_SET);
//	if(nRet != 0)
//	{
//		char str[100]="fseek包文件出错。";
//		PutDebugString(str);
//		return 0;
//	}
//
//	//写文件索引
//	DWORD dwCount = m_Indexs.size();
//	nRet = fwrite(&dwCount, sizeof(DWORD), 1, m_pFile);
//	if(nRet != 1)
//	{
//		char str[100]="fwrite包文件出错。";
//		PutDebugString(str);
//		return 0;
//	}
//
//	nRet = fflush(m_pFile);
//	if(nRet != 0)
//	{
//		char str[100]="fflush包文件出错。";
//		PutDebugString(str);
//		return 0;
//	}
//	nOffset +=sizeof(DWORD);
//
//	for (itIndex it = m_Indexs.begin(); it != m_Indexs.end(); it++)
//	{
//		tagFileIndex index = (*it).second;
//		Crypto(&index, sizeof(index));
//
//		nRet = fseek(m_pFile, nOffset, SEEK_SET);
//		if(nRet != 0)
//		{
//			char str[100]="fseek包文件出错。";
//			PutDebugString(str);
//			return 0;
//		}
//		nRet = fwrite(&index, sizeof(tagFileIndex),1, m_pFile);
//		if(nRet != 1)
//		{
//			char str[100]="fwrite包文件出错。";
//			PutDebugString(str);
//			return 0;
//		}
//		nRet = fflush(m_pFile);
//		if(nRet != 0)
//		{
//			char str[100]="fflush包文件出错。";
//			PutDebugString(str);
//			return 0;
//		}
//		nOffset +=sizeof(tagFileIndex);
//	}
//
//	//写空区间索引
//	dwCount = m_Emptys.size();
//
//	nRet = fseek(m_pFile, nOffset, SEEK_SET);
//	if(nRet != 0)
//	{
//		char str[100]="fseek包文件出错。";
//		PutDebugString(str);
//		return 0;
//	}
//	nRet = fwrite(&dwCount, sizeof(DWORD), 1, m_pFile);
//	if(nRet != 1)
//	{
//		char str[100]="fwrite包文件出错。";
//		PutDebugString(str);
//		return 0;
//	}
//	nRet = fflush(m_pFile);
//	if(nRet != 0)
//	{
//		char str[100]="fflush包文件出错。";
//		PutDebugString(str);
//		return 0;
//	}
//	nOffset +=sizeof(DWORD);
//
//	for (itEmpty it = m_Emptys.begin(); it != m_Emptys.end(); it++)
//	{
//		tagFileIndex index = (*it);
//		Crypto(&index, sizeof(index));
//
//		nRet = fseek(m_pFile, nOffset, SEEK_SET);
//		if(nRet != 0)
//		{
//			char str[100]="fseek包文件出错。";
//			PutDebugString(str);
//			return 0;
//		}
//		nRet = fwrite(&index, sizeof(tagFileIndex),1, m_pFile);
//		if(nRet != 1)
//		{
//			char str[100]="fwrite包文件出错。";
//			PutDebugString(str);
//			return 0;
//		}
//		nRet = fflush(m_pFile);
//		if(nRet != 0)
//		{
//			char str[100]="fflush包文件出错。";
//			PutDebugString(str);
//			return 0;
//		}
//		nOffset +=sizeof(tagFileIndex);
//	}
//
//	//写索引偏移量
//
//	Crypto(&nIndexStartOff, sizeof(DWORD));
//
//	//保证是在文件末尾倒数第4个字节写索引偏移量
//	nRet = fseek(m_pFile,0,SEEK_END);
//	if(nRet != 0)
//	{
//		char str[100]="fseek包文件出错。";
//		PutDebugString(str);
//		return 0;
//	}
//	DWORD nFilLenght = ftell(m_pFile);
//	if(nFilLenght-4>nOffset)
//	{
//		nOffset = nFilLenght-4;
//	}
//
//	nRet = fseek(m_pFile, nOffset, SEEK_SET);
//	if(nRet != 0)
//	{
//		char str[100]="fseek包文件出错。";
//		PutDebugString(str);
//		return 0;
//	}
//	nRet = fwrite(&nIndexStartOff, sizeof(DWORD),1, m_pFile);
//	if(nRet != 1)
//	{
//		char str[100]="fwrite包文件出错。";
//		PutDebugString(str);
//		return 0;
//	}
//	nRet = fflush(m_pFile);
//	if(nRet != 0)
//	{
//		char str[100]="fflush包文件出错。";
//		PutDebugString(str);
//		return 0;
//	}
//	return true;
//}

bool	CPackage::WriteIndexEx()
{
	if(!m_pFile)
		return false;

	//声情20M空间
	BYTE *TemptBuff = new BYTE[4+4+4+m_dwIndexHeadSize+4+4+m_dwEmptyIndexHeadSize];
	int nPos = 0;
	//写数据的地址偏移量
	memcpy(&TemptBuff[nPos],&m_dwOffset,sizeof(DWORD));
	nPos += sizeof(DWORD);
	//写文件索引头大小
	memcpy(&TemptBuff[nPos],&m_dwIndexHeadSize,sizeof(DWORD));
	nPos += sizeof(DWORD);
	//写文件索引数量
	DWORD dwCount = m_Indexs.size();
	memcpy(&TemptBuff[nPos],&dwCount,sizeof(DWORD));
	nPos += sizeof(DWORD);
	for(itIndex it = m_Indexs.begin(); it != m_Indexs.end(); it++)
	{
		tagFileIndex index = (*it).second;
		Crypto(&index, sizeof(index));
		memcpy(&TemptBuff[nPos],&index, sizeof(tagFileIndex));
		nPos += sizeof(tagFileIndex);
	}

	nPos = 4+4+4+m_dwIndexHeadSize;
	//写空区间头大小
	memcpy(&TemptBuff[nPos],&m_dwEmptyIndexHeadSize,sizeof(DWORD));
	nPos += sizeof(DWORD);
	//写空区间数量
	dwCount = m_Emptys.size();
	memcpy(&TemptBuff[nPos],&dwCount,sizeof(DWORD));
	nPos += sizeof(DWORD);
	for (itEmpty it = m_Emptys.begin(); it != m_Emptys.end(); it++)
	{
		tagFileIndex index = (*it);
		Crypto(&index, sizeof(index));
		memcpy(&TemptBuff[nPos],&index, sizeof(tagFileIndex));
		nPos += sizeof(tagFileIndex);
	}
	
	int nRet = fseek(m_pFile, 0, SEEK_SET);
	if(nRet != 0)
	{
		char str[100]="fseek包文件出错。";
		PutDebugString(str);
		return 0;
	}
	//写文件索引
	nRet = fwrite(TemptBuff, nPos, 1, m_pFile);
	if(nRet != 1)
	{
		char str[100]="fwrite包文件出错。";
		PutDebugString(str);
		delete[] TemptBuff;
		return 0;
	}

	nRet = fflush(m_pFile);
	if(nRet != 0)
	{
		char str[100]="fflush包文件出错。";
		PutDebugString(str);
		delete[] TemptBuff;
		return 0;
	}
	delete[] TemptBuff;
	return true;
}

//内部调整文件
DWORD	CPackage::AdjustFile(const char* pszFile,DWORD dwCompress)
{
	itIndex it = m_Indexs.find(pszFile);
	if(it == m_Indexs.end()) return false;

	if((*it).second.dwCompressType == dwCompress)	return false;

	//现在大小和文件实际大小不符时候，不能调整
	if((*it).second.dwSize != (*it).second.dwValidSize)	return false;

	tagFileIndex index = (*it).second;
	index.dwCompressType = dwCompress;

	BYTE* pBuf = new BYTE[(*it).second.dwSize];

	int nRet = fseek(m_pFile,(*it).second.dwOffset,SEEK_SET);
	if(nRet != 0)
	{
		char str[100]="fseek包文件出错。";
		PutDebugString(str);
		return 0;
	}
	nRet = fread(pBuf, (*it).second.dwSize, 1, m_pFile);
	if(nRet != 1)
	{
		char str[100]="fread包文件出错。";
		PutDebugString(str);
		return 0;
	}

	DelFile(pszFile);

	if( !ENABLEBIT(dwCompress,NOCOMPRESS) && ENABLEBIT(index.dwCompressType,NOCOMPRESS) )
	{
		if(index.dwSize > 0)
		{
			BYTE *lpszOut = new BYTE[index.dwSize*2];
			DWORD nOutSize = index.dwSize*2;

			if( ENABLEBIT(dwCompress,0x4) )//zip压缩
				Compress(pBuf, index.dwSize, lpszOut, &nOutSize, 1);
			else
				CompressData(pBuf, index.dwSize, lpszOut, &nOutSize, false);

			index.dwOrginSize = index.dwSize;
			index.dwValidSize = nOutSize;
			index.dwSize = nOutSize;
			nRet = WriteData(index,lpszOut);

			delete []lpszOut;
		}
		else
		{
			index.dwSize=0;
			BYTE* pBuf = NULL;
			index.dwValidSize=index.dwSize;
			nRet = WriteData(index,pBuf);
		}
	}
	else if((dwCompress&0x1) && !(index.dwCompressType&0x1))
	{
		BYTE* pOutBuf = new BYTE[index.dwOrginSize];
		DWORD dwOutSize=index.dwOrginSize;

		if( ENABLEBIT(index.dwCompressType,0x4) )
			DeCompress(pBuf, index.dwSize, pOutBuf, &dwOutSize);
		else
			DeCompressData(pBuf, index.dwSize, pOutBuf, &dwOutSize);
		index.dwSize = dwOutSize;
		index.dwValidSize = dwOutSize;

		nRet = WriteData(index,pOutBuf);

		delete []pOutBuf;
	}
	//压缩之间的切换暂时不考虑
	else
	{
	}

	delete []pBuf;
	if(nRet == false)
		return false;
	return index.dwSize;
}

//向包中写入数据
DWORD	CPackage::WriteData(tagFileIndex& FileIndex,const void* pBuf)
{
	if(pBuf==NULL || FileIndex.dwSize <= 0)	return false;

	DWORD dwOffset = 0;
	bool bFlag = false;
	//处理包中存在该文件的情况
	itIndex it = m_Indexs.find(FileIndex.szName);
	if(it != m_Indexs.end())
	{
		if(FileIndex.dwSize <= (*it).second.dwSize)
		{
			dwOffset = (*it).second.dwOffset;
			bFlag = true;
		
			if((*it).second.dwSize > FileIndex.dwSize)
			{
				//添加新的空区间
				tagFileIndex temptFileIndex1 = {"",dwOffset+FileIndex.dwSize,
					((*it).second.dwSize-FileIndex.dwSize),0,0,0};
				AddEmptyPart(temptFileIndex1);
			}
		}
		else
		{
			//把已经有的文件区间添加到空区间
			tagFileIndex temptFileIndex1 = {"",(*it).second.dwOffset,(*it).second.dwSize,0,0,0};
			AddEmptyPart(temptFileIndex1);
		}
	}
	else
	{
		//判断最大数量
		if(m_Indexs.size() >= m_dwMaxIndexNum)
		{
			char str[512]="";
			_snprintf(str, 512, "向包<%s>中写入文件<%s>出错，超过最大文件个数%d.",
				m_strPackName.c_str(),FileIndex.szName,m_dwMaxIndexNum);
			PutDebugString(str);
			return false;
		}

		//寻找包中是否有空区间
		tagFileIndex temptFileIndex;
		if(GetEmptyPart(FileIndex.dwSize,temptFileIndex))
		{
			dwOffset = temptFileIndex.dwOffset;
			bFlag = true;
		
			if(temptFileIndex.dwSize > FileIndex.dwSize)
			{
				//添加新的空区间
				tagFileIndex temptFileIndex2 = {"",dwOffset+FileIndex.dwSize,
					temptFileIndex.dwSize-FileIndex.dwSize,0,0,0};
				AddEmptyPart(temptFileIndex2);
			}
		}
	}

	if(bFlag == false || dwOffset>=(m_dwOffset-FileIndex.dwSize))
	{
		dwOffset = m_dwOffset;
		m_dwOffset += FileIndex.dwSize;
	}

	FileIndex.dwOffset = dwOffset;
	_strlwr(FileIndex.szName);
	m_Indexs[FileIndex.szName] = FileIndex;

	int nRet = fseek(m_pFile,dwOffset,SEEK_SET);
	if(nRet != 0)
	{
		char str[100]="fread包文件出错。";
		PutDebugString(str);
		return false;
	}
	nRet = fwrite(pBuf, FileIndex.dwSize, 1, m_pFile);
	if(nRet != 1)
	{
		char str[100]="fwrite包文件出错。";
		PutDebugString(str);
		return false;
	}
	nRet = fflush(m_pFile);
	if(nRet != 0)
	{
		char str[100]="fflush包文件出错。";
		PutDebugString(str);
		return false;
	}

	return FileIndex.dwSize;
}

DWORD	CPackage::WriteDataEx(tagFileIndex& FileIndex,const void* pBuf)
{
	if(pBuf==NULL || FileIndex.dwSize <= 0)	return false;

	DWORD dwOffset = 0;
	bool bFlag = false;
	//处理包中存在该文件的情况
	itIndex it = m_Indexs.find(FileIndex.szName);
	if(it != m_Indexs.end())
	{
		if(FileIndex.dwSize <= (*it).second.dwSize)
		{
			dwOffset = (*it).second.dwOffset;
			bFlag = true;
		
			if((*it).second.dwSize > FileIndex.dwSize)
			{
				//添加新的空区间
				tagFileIndex temptFileIndex1 = {"",dwOffset+FileIndex.dwSize,
					((*it).second.dwSize-FileIndex.dwSize),0,0,0};
				AddEmptyPart(temptFileIndex1);
			}
		}
		else
		{
			//把已经有的文件区间添加到空区间
			tagFileIndex temptFileIndex1 = {"",(*it).second.dwOffset,(*it).second.dwSize,0,0,0};
			AddEmptyPart(temptFileIndex1);
		}
	}
	else
	{
		//判断最大数量
		if(m_Indexs.size() >= m_dwMaxIndexNum)
		{
			char str[512]="";
			_snprintf(str, 512, "向包<%s>中写入文件<%s>出错，超过最大文件个数%d.",
				m_strPackName.c_str(),FileIndex.szName,m_dwMaxIndexNum);
			PutDebugString(str);
			return false;
		}
		//寻找包中是否有空区间
		tagFileIndex temptFileIndex;
		if(GetEmptyPart(FileIndex.dwSize,temptFileIndex))
		{
			dwOffset = temptFileIndex.dwOffset;
			bFlag = true;
		
			if(temptFileIndex.dwSize > FileIndex.dwSize)
			{
				//添加新的空区间
				tagFileIndex temptFileIndex2 = {"",dwOffset+FileIndex.dwSize,
					temptFileIndex.dwSize-FileIndex.dwSize,0,0,0};
				AddEmptyPart(temptFileIndex2);
			}
		}
	}

	if(bFlag == false)
	{
		dwOffset = m_dwOffset;
		m_dwOffset += FileIndex.dwSize;
	}

	FileIndex.dwOffset = dwOffset;
	_strlwr(FileIndex.szName);
	m_Indexs[FileIndex.szName] = FileIndex;

	WriteIndexEx();

	int nRet = fseek(m_pFile,dwOffset,SEEK_SET);
	if(nRet != 0)
	{
		char str[100]="fread包文件出错。";
		PutDebugString(str);
		return false;
	}
	nRet = fwrite(pBuf, FileIndex.dwSize, 1, m_pFile);
	if(nRet != 1)
	{
		char str[100]="fwrite包文件出错。";
		PutDebugString(str);
		return false;
	}
	nRet = fflush(m_pFile);
	if(nRet != 0)
	{
		char str[100]="fflush包文件出错。";
		PutDebugString(str);
		return false;
	}

	return FileIndex.dwSize;
}