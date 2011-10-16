#ifndef	FILESINFO_H
#define FILESINFO_H

#pragma once
#include <map>
#include <string>
#include <list>

using namespace std;

#define TREELIST_ADD				WM_USER+1000				//向视图中添加
#define TREELIST_UPDATE				WM_USER+1001				//向视图中刷新项
#define TREELIST_DELETE				WM_USER+1002				//从视图中删除项
#define TREELIST_DELALL				WM_USER+1003				//从视图中删除所有项
#define TREELIST_INITIAL			WM_USER+1004				//初始化
#define TREELIST_UPDATEWITHCHILD	WM_USER+1005				//向视图中刷新项,包括所有子项

/////////////////////////////////////////////////////////////////////////////////
//FilesInfo.h
//Fun:用来统计客户端资源信息
//Create Time:2004.10.29
//Author:Joe
//
//
//Modified by Joe,2006.10,23.为资源添加了多个包的功能。
/////////////////////////////////////////////////////////////////////////////////
class CClientResource;

#define ENABLEBIT(NUM,BIT)	(NUM&BIT)
#define NOPACK		0x1
#define NOCOMPRESS	0x1


typedef struct tagFileInfo
{
	 string		strFileName;					//文件相对路径名
	 DWORD		dwSize;							//在硬盘上的大小，未压缩就是原始大小，压缩了就是压缩后大小
	 DWORD		dwOrginSize;					//文件原始大小，未压缩前size
	 DWORD		dwValidSize;					//当前有效的文件大小，主要用在断点续传记录已经下载的文件大小
	 DWORD		dwCrc32;						//CRC校验码
	 DWORD		dwPackageType;					//是否打包: 1, 未打包; 大于1, 表示打包、数据在数值相对应的包内
	 DWORD		dwCompressType;					//1 未压缩 2 zip压缩

	 bool		bFolder;						//是否是文件夹
	 tagFileInfo* ParentFileInfo;				//父文件夹信息
	 map<string,tagFileInfo*> ChildFilesInfo;	//子文件信息
	 
	 tagFileInfo()
	 {
		 strFileName="";dwSize=0;dwOrginSize=0;dwValidSize=0;
		 dwCrc32=0;dwPackageType=1;dwCompressType=1;
		 bFolder=true;ParentFileInfo=NULL;ChildFilesInfo.clear();
	 }

	 tagFileInfo(string& FileName,DWORD Size,DWORD OrginSize,
				DWORD ValidSize,DWORD Crc32,DWORD PackageType,
				DWORD CompressType,bool Folder,tagFileInfo* pParentFileInfo)
	 {
		 strFileName=FileName;dwSize=Size;dwOrginSize=OrginSize;
		 dwValidSize=ValidSize;dwCrc32=Crc32;
		 dwPackageType=PackageType;dwCompressType=CompressType;
		 bFolder=Folder;ParentFileInfo=pParentFileInfo;ChildFilesInfo.clear();
	 }

}*PTFILEINFO;

typedef map<string,tagFileInfo*>::iterator itFileInfo;

//包文件信息
struct tagPackFileInfo
{
	DWORD		dwID;				//数据包ID编码
	string		strFileName;		//包对应的文件名
	DWORD		dwIndexNum;		
	DWORD		dwEmptyIndexNum;
};

typedef list<tagPackFileInfo>	PackTypeInfos;
typedef list<tagPackFileInfo>	CompTypeInfos;


typedef list<tagPackFileInfo>::iterator itTypeInfo;

class CFilesInfo
{
public:
	CFilesInfo(HWND pWnd=0);
	virtual ~CFilesInfo(void);

private:
	string						m_FileName;												//资源信息存储文件名
	tagFileInfo					*m_pRootFile;											//根文件

	PackTypeInfos				m_PackageInfos;											//包文件的信息
	string						m_strVersion;											//文件版本号
	DWORD						m_dwFileNum;											//文件总个数
	
public:
	DWORD	GetFileNum();			//得到文件个数
	VOID	SetName(string& strName)		{m_FileName=strName;}
	string& GetName()						{return m_FileName;	}
	void	SetVersion(string strVersion)	{m_strVersion=strVersion;}
	string&	GetVersion()					{return m_strVersion;}

	PackTypeInfos& GetPackageTypeInfo()						{return m_PackageInfos;}
	void	SetPackageTypeInfo(PackTypeInfos& PackageInfos)	{m_PackageInfos = PackageInfos;}

	tagFileInfo* GetRootFile()				{return m_pRootFile;}

	DWORD	AddPackFileInfo(string& strPackFileName);
	string	GetPackFileName(DWORD dwID);
	DWORD	GetPackID(string strFileName);
		
	VOID	Clear();

	BOOL	Load(string& strName);														//从文件中装载资源息列表
	BOOL	LoadPackInfo(FILE* pf);
	BOOL	LoadFolderInfo(FILE* pf,tagFileInfo* pParentagFileInfo);					//装载一个文件夹的信息
	BOOL	Save(string& strName);														//保存资源信息列表
	bool	Save(FILE* pf,tagFileInfo* pFileInfo);										//保存资源信息列表
	BOOL	SavePackInfo(FILE* pf);

	bool	FindFileInfo(string& strFileName);											//寻找一个文件是否存在
	//不需要更新父节点信息
	tagFileInfo* AddFolderInfo(tagFileInfo* pParentFileInfo,string& strName,
								DWORD dwSize,DWORD dwOrginSize,DWORD dwValidSize,
								DWORD dwPackageType,DWORD dwCompressType);

	tagFileInfo* AddFileInfo(tagFileInfo* pParentFileInfo,string& strName,
								DWORD dwCrc32,DWORD dwSize,DWORD dwOrginSize,
								DWORD dwValidSize,DWORD dwPackageType,DWORD dwCompressType);

	tagFileInfo* AddFolderInfo(string& strParentName,string& strName,
								DWORD dwSize,DWORD dwOrginSize,DWORD dwValidSize,
								DWORD dwPackageType,DWORD dwCompressType);

	//需要更新父节点的大小
	tagFileInfo* InsertFolderInfo(tagFileInfo* pParentFileInfo,string& strName,
									DWORD dwSize,DWORD dwOrginSize,DWORD dwValidSize,
									DWORD dwPackageType,DWORD dwCompressType);

	tagFileInfo* InsertFileInfo(tagFileInfo* pParentFileInfo,string& strName,
								DWORD dwCrc32,DWORD dwSize,DWORD dwOrginSize,
								DWORD dwValidSize,DWORD dwPackageType,DWORD dwCompressType);

	//需要更新父结点的大小和其他信息
	tagFileInfo* UpdateFileInfo(tagFileInfo* ptagInfo,DWORD dwCrc32,DWORD dwSize,
								DWORD dwOrginSize,DWORD dwValidSize,bool bFolder,
								DWORD dwPackageType,DWORD dwCompressType);				//更新一个文件信息

	tagFileInfo* UpdateFileInfo(string& strFileName,DWORD dwCrc32,DWORD dwSize,
								DWORD dwOrginSize,DWORD dwValidSize,
								DWORD dwPackageType,DWORD dwCompressType);				//更新一个文件信息

	tagFileInfo* UpdateFileInfo(string& strFileName,DWORD dwCrc32,DWORD dwSize,
								DWORD dwOrginSize,DWORD dwValidSize,bool bFolder,
								DWORD dwPackageType,DWORD dwCompressType);				//更新一个文件信息


	//更新一个文件夹及其包含的子文件,如果该文件已经设置包信息，这不变
	void	UpdateFileInfoWithChild(string& strFileName,DWORD dwPackageType,
									DWORD dwCompressType,
									bool bForcePack,
									bool bForceCompress);								//更新打包，压缩信息
	void	UpdateFileInfoWithChild(tagFileInfo* ptagInfo,DWORD dwPackageType,
									DWORD dwCompressType,
									bool bForcePack,
									bool bForceCompress);								//更新打包，压缩信息

	void	UpdateFilePackInfoWithChild(string& strFileName,DWORD dwPackageType);
	void	UpdateFilePackInfoWithChild(tagFileInfo* ptagInfo,DWORD dwPackageType);		//更新包信息

	//根据一个文件信息表包含的所有文件信息，来更新自己
	void	UpdateFilePackInfoWithChild(CFilesInfo* pOtherFilesInfo);
	void	UpdateFilePackInfoWithChild(CFilesInfo* pOtherFilesInfo,
										tagFileInfo* pOthertagInfo);					//更新包信息

	VOID	DelFileInfo(string& strFileName);											//删除一个文件信息
	VOID	DelFileInfo(tagFileInfo* pFileInfo);										//删除一个文件信息
	
	DWORD	GetFileCrc32(string& strFileName);											//得到文件的CRC信息码
	BOOL	GetFileInfo(const string& strFileName,DWORD& dwSize,DWORD& dwOrginSize,
						DWORD &dwValidSize,DWORD& dwCrc32,DWORD& dwPackageType,
						DWORD& dwCompressType,bool& bFolder);							//得到一个文件的信息

	bool	GetDifferenceByPackage(CFilesInfo& filesInfo,CFilesInfo& temptFilesInfo);	//根据是否打包得到两个文件信息列表的差异

	bool	GetDifferenceByPackage(tagFileInfo* pFileInfo,CFilesInfo& filesInfo,
									CFilesInfo& temptFilesInfo,bool bPackage);

	bool	GetDifferenceByCrc32(CFilesInfo& filesInfo,CFilesInfo& temptFilesInfo);		//得到两个文件信息列表的差异

	bool	GetDifferenceByCrc32(tagFileInfo* pFileInfo,CFilesInfo& filesInfo,
									CFilesInfo& temptFilesInfo);

	bool	CopyData(CFilesInfo* pFileInfo);
	bool	CopyData(tagFileInfo* ptagFileInfo,CFilesInfo* pFileInfo);


	bool	IsFolder(string& strFileName);												//是否是文件夹
	bool	HaveChild(tagFileInfo* pFieInfo);											//是否有子节点
	bool	ChildHaveUnPackFile(tagFileInfo* ptagFieInfo);								//是否有未打包文件
	tagFileInfo* GetFileInfoByText(string strFileName);									//根据字符串得到一个节点
	tagFileInfo* FindChildFileInfoByText(tagFileInfo* pParent,string& str);				//找到一个匹配字符子节点
	string GetFileTextWithParent(tagFileInfo* pFileInfo);								//得到一个节点的和父结点连接字符串


	void AddFileSize(tagFileInfo* pFileInfo,int iSize);									//添加文件大小不
	void UpdateParentFolderInfo(tagFileInfo* ptagFileInfo);								//统计父文件夹信息
	void UpdateFolderInfo(tagFileInfo* pFileInfo,DWORD& dwPackage,DWORD& dwCompress);	//统计文件夹信息

	void FindFileList(string& strFile, string& strExt, list<string>& listFileName);
	void FindFileList(tagFileInfo* ptagFileInfo, string& strExt,
						list<string>& listFileName);
private:
	HWND		m_pDisplayWnd;															//显示数据的窗口
public:
	HWND SetDisplayWnd(HWND hWnd)
	{ HWND hOldWnd = m_pDisplayWnd;m_pDisplayWnd=hWnd;return hOldWnd;}
	HWND GetDisplayWnd(void) { return m_pDisplayWnd; }
	void OnAdd(tagFileInfo* ptagFileInfo);
	void OnUpdate(tagFileInfo* ptagFileInfo);
	void OnDelete(tagFileInfo* ptagFileInfo);
	void OnDeleteAll();
	void OnIntial();

	void UpdateAllInfoToView(HWND hWnd);
	void UpdateAllInfoToViewEx(HWND hWnd);
	void UpdateAllInfoToView(tagFileInfo* ptagFileInfo,HWND hWnd);
};

DWORD	GetFirstEnableBit(DWORD dwNum);

#endif