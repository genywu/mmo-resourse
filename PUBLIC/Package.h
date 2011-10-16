#ifndef		__packagefile__
#define		__packagefile__

////////////////////////////////////////////////////////
//Fun:实现打包功能
//Create Time:2004.11.1
//Author:Joe
////////////////////////////////////////////////////////

#include <vector>
#include <map>
#include <string>
#include <set>

using namespace std;

class CClientResource;


struct tagFileIndex
{
	char	szName[256];
	DWORD	dwOffset;
	DWORD	dwSize;							//在硬盘上的大小，未压缩就是原始大小，压缩了就是压缩后大小
	DWORD	dwOrginSize;					//文件原始大小，未压缩前size
	DWORD	dwValidSize;					//当前有效的文件大小，主要用在断点续传记录已经下载的文件大小
	DWORD	dwCrc32;
	DWORD   dwCompressType;					/// 0 未压缩 1 压缩
};

struct SortEmpty
{
	bool operator()	(tagFileIndex left,tagFileIndex right)
	{
		if(left.dwSize>=right.dwSize)
			return true;
		else
			return false;
	}
};

typedef map<string,tagFileIndex>	INDEXOFFILES;
typedef map<string,tagFileIndex>::iterator itIndex;
typedef map<string,tagFileIndex>::const_iterator const_itIndex;

typedef multiset<tagFileIndex,SortEmpty>		IndexEmptys;
typedef multiset<tagFileIndex,SortEmpty>::iterator itEmpty;

class CPackage
{
public:
	CPackage(CClientResource* pClientResource,string& strWorkPath,
		DWORD dwMaxIndexNum = 100000,DWORD dwMaxEmptyIndexNum=1);
	~CPackage();

	DWORD	GetID()				{return m_dwTypeID;}
	void	SetID(DWORD dwId)	{m_dwTypeID = dwId;}

	bool	Open( const char* pszPackage,bool bUpdate=true );
	bool	OpenForUpdate( const char* pszPackage);

	bool	Create(const char* pszPackage);
	DWORD	AddFile(const char* pszFile ,DWORD &dwCrc32,
					DWORD dwCompress, DWORD &dwOrginSize);

	DWORD	AddFile(const char* strFileName,BYTE* pBuf,int iSize,
					DWORD &dwCrc32,DWORD dwCompress, DWORD &dwOrginSize);

	DWORD	AddFile(FILE* pf ,const char* strFileName,DWORD &dwCrc32,
					DWORD dwCompress, DWORD &dwOrginSize);

	DWORD	DelFile(const char* pszFile);																	//删除包中文件

	DWORD	AddFileFromPackage(	CPackage* pPackage,const char* pszFile,
								DWORD dwCompress);

	DWORD	AdjustFile(const char* pszFile,DWORD dwCompress);												//内部调整文件

	DWORD	ExtractToFile(const char* pszFile,string strFolder="");											//提取到文件

	bool	ExtractToBuf(	const char* pszFile,BYTE *& pBuf,int iSize,
							DWORD& dwCrc32,DWORD& dwCompressType,
							DWORD &dwOrginSize,DWORD &dwValidSize);	//提取到缓存

	bool	ExtractToBuf(const char* pszFile,BYTE *& pBuf,int iSize,
							tagFileIndex& FileIndex);														//提取到缓存

	long	GetFileSize(const char* pszFile);																//得到文件大小

	//bool	WriteIndex();			//不用
	bool	WriteIndexEx();
	void	Crypto( void* pBuf, DWORD dwSize);

	void	Release();

	const INDEXOFFILES* GetIndexs()	{return & m_Indexs;	}

	void	OpenFileHandle(const char* pszPackage);
	void	CloseFileHandle();
	DWORD	WriteData(tagFileIndex& FileIndex,const void* pBuf);									//向包中写入数据
	DWORD	WriteDataEx(tagFileIndex& FileIndex,const void* pBuf);									//向包中写入数据

	//针对客户端的下载写数据操作
public:
	bool	ClearData();
	DWORD	AddFile(const char* strFileName,int nSize,DWORD dwCrc32,
					DWORD dwCompress, DWORD dwOrginSize);

	DWORD	AddFileData(const char* strFileName,BYTE* pBuf,
						DWORD& nValidSize,DWORD nSize);
	FILE*	GetFileHandle()		{return m_pFile;}

	bool	GetFirstFileInfo(tagFileIndex& FileIndex);

private:
	void	AddEmptyPart(tagFileIndex& FileIndex);													//添加一个空区间
	bool	GetEmptyPart(DWORD dwSize,tagFileIndex& FileIndex);										//根据大小得到一个空区间

private:
	DWORD			m_dwMaxIndexNum;
	DWORD			m_dwMaxEmptyIndexNum;
	
	DWORD			m_dwIndexHeadSize;								//文件索引头大小	
	DWORD			m_dwEmptyIndexHeadSize;							//空文件索引头的大小

	DWORD			m_dwTypeID;										//包ID
	FILE*			m_pFile;
	string			m_strWorkPath;
	string			m_strPackName;

	INDEXOFFILES	m_Indexs;										//当前包中文件索引
	IndexEmptys		m_Emptys;										//当前空区间目录
	DWORD			m_dwOffset;										//当前包中所有文件数据的的偏移量

	CClientResource*		m_pOriginResource;						//客户端资源类
};

// Lzo1x compress
void CompressData(const unsigned char* pInData,unsigned long lInDataSize,
				  unsigned char* pOutBuf,unsigned long* plOutDataSize,bool bFast);

// Lzo1x uncompress
void DeCompressData(const unsigned char * pInData,unsigned long lInDataSize,
					unsigned char * pOutBuf,unsigned long* lOutDataSize);

// zip compress
int Compress(const unsigned char* pInData,unsigned long lInDataSize,
			 unsigned char* pOutBuf,unsigned long* plOutDataSize,int level);

// zip uncompress
int  DeCompress(const unsigned char * pInData,unsigned long lInDataSize,
				unsigned char * pOutBuf,unsigned long* lOutDataSize);


#endif		//__packagefile__