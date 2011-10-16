#ifndef		__packagefile__
#define		__packagefile__
//SM 编译时去掉下面的宏定义
//#ifndef ZLIB_WINAPI
//#define ZLIB_WINAPI
//#endif

////////////////////////////////////////////////////////
//Fun:实现打包功能
//Create Time:2004.11.1
//Author:Joe
////////////////////////////////////////////////////////

class CClientResource;

struct tagFileIndex
{
	char	szName[256];
	ulong	dwOffset;
	ulong	dwSize;							//在硬盘上的大小，未压缩就是原始大小，压缩了就是压缩后大小
	ulong	dwOrginSize;					//文件原始大小，未压缩前size
	ulong	dwValidSize;					//当前有效的文件大小，主要用在断点续传记录已经下载的文件大小
	ulong	dwCrc32;
	ulong   dwCompressType;					/// 0 未压缩 1 压缩
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
		ulong dwMaxIndexNum = 100000,ulong dwMaxEmptyIndexNum=1);
	~CPackage();

	ulong	GetID()				{return m_dwTypeID;}
	void	SetID(ulong dwId)	{m_dwTypeID = dwId;}

	bool	Open( const char* pszPackage,bool bUpdate=true );
	bool	OpenForUpdate( const char* pszPackage);

	bool	Create(const char* pszPackage);
	ulong	AddFile(const char* pszFile ,ulong &dwCrc32,
					ulong dwCompress, ulong &dwOrginSize);

	ulong	AddFile(const char* strFileName,uchar* pBuf,int iSize,
					ulong &dwCrc32,ulong dwCompress, ulong &dwOrginSize);

	ulong	AddFile(FILE* pf ,const char* strFileName,ulong &dwCrc32,
					ulong dwCompress, ulong &dwOrginSize);

	ulong	DelFile(const char* pszFile);																	//删除包中文件

	ulong	AddFileFromPackage(	CPackage* pPackage,const char* pszFile,
								ulong dwCompress);

	ulong	AdjustFile(const char* pszFile,ulong dwCompress);												//内部调整文件

	ulong	ExtractToFile(const char* pszFile,string strFolder="");											//提取到文件

	bool ExtractToBuf(const char* pszFile,
        uchar *& pBuf,uint iSize,
        ulong& dwCrc32,
        ulong& dwCompressType,
        ulong &dwOrginSize,
        ulong &dwValidSize);	// 提取到缓存

	bool ExtractToBuf(const char* pszFile,
        uchar *& pBuf,
        uint iSize,
        tagFileIndex& FileIndex);														//提取到缓存

	long	GetFileSize(const char* pszFile);																//得到文件大小

	//bool	WriteIndex();			//不用
	bool	WriteIndexEx();
	void	Crypto( void* pBuf, ulong dwSize);

	void	Release();

	const INDEXOFFILES* GetIndexs()	{return & m_Indexs;	}

	void	OpenFileHandle(const char* pszPackage);
	void	CloseFileHandle();
	ulong	WriteData(tagFileIndex& FileIndex,const void* pBuf);									//向包中写入数据
	ulong	WriteDataEx(tagFileIndex& FileIndex,const void* pBuf);									//向包中写入数据

	//针对客户端的下载写数据操作
public:
	bool	ClearData();
	ulong	AddFile(const char* strFileName,int nSize,ulong dwCrc32,
					ulong dwCompress, ulong dwOrginSize);

	ulong	AddFileData(const char* strFileName,uchar* pBuf,
						ulong& nValidSize,ulong nSize);
	FILE*	GetFileHandle()		{return m_pFile;}

	bool	GetFirstFileInfo(tagFileIndex& FileIndex);

private:
	void	AddEmptyPart(tagFileIndex& FileIndex);													//添加一个空区间
	bool	GetEmptyPart(ulong dwSize,tagFileIndex& FileIndex);										//根据大小得到一个空区间

private:
	ulong			m_dwMaxIndexNum;
	ulong			m_dwMaxEmptyIndexNum;
	
	ulong			m_dwIndexHeadSize;								//文件索引头大小	
	ulong			m_dwEmptyIndexHeadSize;							//空文件索引头的大小

	ulong			m_dwTypeID;										//包ID
	FILE*			m_pFile;
	string			m_strWorkPath;
	string			m_strPackName;

	INDEXOFFILES	m_Indexs;										//当前包中文件索引
	IndexEmptys		m_Emptys;										//当前空区间目录
	ulong			m_dwOffset;										//当前包中所有文件数据的的偏移量

	CClientResource*		m_pOriginResource;						//客户端资源类
};

// Lzo1x compress
void CompressData(const uchar* pInData,ulong lInDataSize,
				  uchar* pOutBuf,ulong* plOutDataSize,bool bFast);

// Lzo1x uncompress
void DeCompressData(const uchar * pInData,ulong lInDataSize,
					uchar * pOutBuf,ulong* lOutDataSize);

// zip compress
int Compress(const uchar* pInData,ulong lInDataSize,
			 uchar* pOutBuf,ulong* plOutDataSize,int level);

// zip uncompress
int  DeCompress(const uchar * pInData,ulong lInDataSize,
				uchar * pOutBuf,ulong* lOutDataSize);


#endif		//__packagefile__