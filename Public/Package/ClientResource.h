#pragma once

/**
 * ClientResource.h
 * Fun:客户端资源类，管理客户端资源
 * Create Time:2004.11.7
 * Author:Joe
 * Modified by Joe,2006.10,23.为资源添加了多个包的功能。
 */

#define LOADRESOURCE_START		WM_USER+2000				//装载资源开始
#define LOADRESOURCE_END		WM_USER+2001				//装载资源结束
#define MAKEPACKAGE_START		WM_USER+2002				//打包开始
#define MAKEPACKAGE_ING			WM_USER+2003				//打包结束
#define MAKEPACKAGE_END			WM_USER+2004				//打包结束
#define REMAKEPACKAGE_START		WM_USER+2005				//整理包开始
#define REMAKEPACKAGE_END		WM_USER+2006				//整理包结束
#define RELOADRESOURCE_START	WM_USER+2007				//从新统计装载资源开始
#define RELOADRESOURCE_END		WM_USER+2008				//装载统计资源结束

#include <deque>
#include "FilesInfo.h"

class CPackage;
class CRFile;
using namespace std;

struct tagPackageInfo
{
	ulong		dwID;				//数据包ID编码
	string		strFileName;		//包对应的文件名
	ulong		dwIndexNum;		
	ulong		dwEmptyIndexNum;
	CPackage	*pPackage;
};

typedef map<ulong,tagPackageInfo>	Packages;
typedef map<ulong,tagPackageInfo>::iterator itPackage;

//异步打开文件的回调函数
typedef void (*AsynReadCallBack)(CRFile* prfFile,void* pData);

enum eResourceType
{
	NO_RES=0,
	ORIGIN_RES,
	GAME_RES,
};

class CClientResource
{
public:
	CClientResource(void);
	CClientResource(eResourceType eType,string& strWorkFolder,Packages& TemptPackages,
					string& strFileInfoName,HWND hwWnd=0);

	//补丁包使用
	CClientResource(eResourceType eType,string& strWorkFolder,string& strFileInfoName,HWND hwWnd=0);
	~CClientResource(void);

private:
	static CClientResource* instance;

public:
	static void InitialInstance();
	static CClientResource* GetInstance();
	static void ReleaseInstance();
private:	
	eResourceType	m_eType;
	//CPackage*		m_pPackage;				//客户端包
	Packages		m_pPackages;			//多个资源包
	CFilesInfo*		m_pFilesInfo;			//客户端综合资源


	string			m_strCurFolder;			//当前操作资源文件夹
	string			m_strPackageName;		//包名
	string			m_strFileInfoName;		//资源信息列表

	bool			m_bOver;				//是否覆盖
	bool			m_bAlwaysCheck;			//是否始终此操作

//#ifdef _CLENTTOTEST_
//	static map<string, ulong> s_mapNotFoundFiles;			//没有找到的文件<路径, ID>
//#endif

public:
	
	string& GetVersion()			{return m_pFilesInfo->GetVersion();}
	bool	GetIsOver()				{return m_bOver;}
	void	SetIsOver(bool b)		{m_bOver=b;	}

	bool	IsFileExist(const char* strFileName);
	bool	IsFileBodyExist(ulong dwPackage,const char* strFileName);

	bool	GetAlwaysCheck()		{return m_bAlwaysCheck;	}
	void	SetAlwaysCheck(bool b)	{m_bAlwaysCheck=b;	}

	//CPackage* GetPackage()			{return m_pPackage;	}
	//void	Setpackage(CPackage* pPackage)	{m_pPackage=pPackage;}
	CPackage* GetPackage(ulong dwPackID);
	CPackage* GetPackageForUpdate(ulong dwPackID);
	void	PackagesWriteIndexEx();

	CFilesInfo*	GetFilesInfo()	{return m_pFilesInfo;}

	string&  GetCurFolder()	{return m_strCurFolder;}
	void	SetCurFolder(string& strName){m_strCurFolder=strName;}

	string& GetPackageName(){return m_strPackageName;}
	void SetPackageName(string& strName){m_strPackageName=strName;}

	string& GetFilesInfoName(){return m_strFileInfoName;}
	ulong   GetFileContentCRC(string& strFileName);

	void	ResetPackInfosForAutoUpdate(PackTypeInfos& TemptPackages);

	bool	Load();
	bool	LoadEx();								//提供给打开游戏资源使用
	bool	LoadEx(PackTypeInfos& PacksInfo);		//提供给更新使用
	bool	LoadExForAutoUpdate();					//提供给自动更新打开游戏资源使用

	bool	Create();
	bool	Clear();
	bool	CreatePackages();
	//bool	LoadORCreatePackages();

	bool	Save();	
	bool	UpdateSave();
	bool	SaveVersionToFile();

	bool	LoadPackage(bool bUpdate=true);
	void	OpenPackages(bool bUpdate=true);
	void	PackagesCloseHandle();
	HANDLE	LoadClientResource();
	HANDLE	ReLoadClientResourece(string& strSeachFold,bool bWithChild = true); //重新统计资源数据
	HANDLE	MakePakcage(CClientResource* pClientResource);						//根据新的信息列表制作包
	HANDLE	ReMakePackage();													//重新整理
	
	bool	OnAddFolder(string& strPos,string& strFolder);
	bool	OnAddFile(string& strPos,string& strFileName);
	bool	OnDelFileFolder(string& strPos,string& strFolderFileName);
	bool	OnExtrFileFolder(string& strPos,string& strFolderFileName);
	bool	ExtrFileFolder(tagFileInfo* ptagFileInfo,string strFolder,
                           CFilesInfo* pFilesInfo=NULL);		                //提取一个文件夹或文件
	bool	ExtrAutoUpdateFileFolder(tagFileInfo* ptagFileInfo,
                                     string strFolder,CFilesInfo* pFilesInfo);	//提取一个文件夹或文件，生成自动更新目录

	void	AddFolder(string& strFolder,ulong dwPackage,ulong dwCompress);
	void	AddFile(string& strSouFolder,string& strFileName,int nSize,
					ulong dwCrc32,ulong dwPackage,ulong dwCompress);
	bool	DelFileFolder(tagFileInfo* ptagFileInfo,CFilesInfo* pFileInfo);		//删除一个文件夹或文件

	FILE*	OpenFile(const char* strFileName);
	bool	CloseFile(FILE* pf);

	void FindFileList(string& strFile, string& strExt, list<string>& listFileName);

	void Initial(eResourceType eType,
        string& strWorkFolder,
        string& strFileInfoName,
        HWND hwWnd=0);
	void	Release();

	bool	AddEmptyFile(	const char* strFileName,int nSize,ulong dwCrc32,
							ulong dwPackage,ulong dwCompress, int dwOrginSize);
	bool	AddFileData(const char* strFileName,uchar* pBuf,int nSize);
	bool	AdjustFile(const char* strFileName,ulong dwTemptPackage,ulong dwTemptCompress);
	bool	ClearEmptyFilesInfo(tagFileInfo* pTagFileInfo,CFilesInfo* pFilesInfo);			//清除空的文件信息

	static unsigned __stdcall LoadClientResourceProc(void* pArguments);
	static unsigned __stdcall ReLoadClientResoureceProc(void* pArguments);
	static unsigned __stdcall MakePakcageProc(void* pArguments);
	static unsigned __stdcall ReMakePakcageProc(void* pArguments);

private:
	
	bool	MakePackage(tagFileInfo* pTagFileInfo,CFilesInfo* pFilesInfo,
						CClientResource* pClientResource);	
	bool	ReMakePackage(tagFileInfo* pTagFileInfo,CPackage* pNewPackage);				//重新整理

	void	SearchChildFolder(CHAR* strFolder,tagFileInfo* pParenttagFileInfo);			//搜索子文件夹
	void	SearchOneFile(tagFileInfo* pParenttagInfo,CHAR* strPath,
                          string& strFileName,ulong dwSize);							//搜索子文件

	void	ReSearchChildFolder(CHAR* strFolder,
								tagFileInfo* pParenttagFileInfo,bool bWithChild = true);//重搜索搜索子文件夹
	void	ReSearchOneFile(tagFileInfo* pParenttagInfo,CHAR* strPath,
                            string& strFileName,ulong dwSize);												//搜索子文件

	bool	AddFile(string& strPos,string& strFileName);								//添加一个文件
	bool	AddFile(string& strFileName,uchar* pBuf,long lSize);						//从缓冲区一个文件	
	bool	AddFolder(string strFolderName,string strWorkPos);							//添加一个文件夹

	bool	DelFileFolder(tagFileInfo* ptagFileInfo);									//删除一个文件夹或文件

	void	CreateDirectory(tagFileInfo* pTagFileInfo,CFilesInfo* pFilesInfo);
	void	DeleteDirectory(tagFileInfo* pTagFileInfo,CFilesInfo* pFilesInfo);

private:
	HWND	m_hWnd;																		//显示相关信息的窗口
	HANDLE  ahThread;																	//一个执行线程
public:

	void OnLoadResourceStart();															//装载资源开始
	void OnLoadResourceEnd();															//装载资源结束
	void OnReLoadResourceStart();														//从新装载资源开始
	void OnReLoadResourceEnd();															//从新装载资源结束

	void OnMakePackageStart();															//打包开始
	void OnMakePackageing(tagFileInfo* pTagFileInfo,CFilesInfo* pFilesInfo);			//打包中
	void OnMakePackageEnd();															//打包结束
	
	void OnReMakePackageStart();														//整理包开始
	void OnReMakePackageEnd();															//整理包结束


//异步读取文件操作
private:
	struct tagFileAsynOper
	{
		//文件名
		char strFileName[MAX_PATH];
		//回调函数
		AsynReadCallBack pCallBack;
		//异步数据
		void* pData;
		//文件句柄
		CRFile* prFile;
	};

	typedef deque<tagFileAsynOper*>	AsynOpFiles;
	typedef deque<tagFileAsynOper*>::iterator itAsynOpFile;

	//请求的异步操作
	AsynOpFiles m_RequestedReadFiles;
	//条件互斥变量
	HANDLE	m_hRequestReadFile;
	CRITICAL_SECTION m_CSRequestReadFile;

	//完成的异步操作
	AsynOpFiles m_CompletedReadFiles;
	CRITICAL_SECTION m_CSCompletedReadFile;

	//一个执行线程
	HANDLE  m_hAsynReadThread;
	//同步打开文件时候的同步信号
	HANDLE  m_hSyncRead;

	//同步读取文件的信息
	tagFileAsynOper* m_pSyncReadFile;

	void PushRequestReadFile(tagFileAsynOper* pAsynReadFile);
	tagFileAsynOper* PopRequestReadFile();

	void PushCompletedReadFile(tagFileAsynOper* pAsynReadFile);
	tagFileAsynOper* PopCompletedReadFile();

	CRFile* OpenRFile(const char* strFileName);
public:
	void StartAsynReadFileThread();
	void ExitAsynReadFileThread();
	//同步打开文件
	CRFile* SyncReadFile(const char* strFileName);
	//异步打开文件
	bool	AsynReadFile(const char* strFileName,AsynReadCallBack pCallBack,void* pData);
	//异步读取文件的线程函数
	static unsigned __stdcall AsynReadFileProc(void* pArguments);
	//完成读取数据的驱动函数
	void Run();
};


//打开资源文件的全局函数
//同步打开文件函数,调用rfClose关闭
CRFile* rfOpen(const char* strFileName);
//异步打开一个文件
bool rfAsynOpen(const char* strFileName,AsynReadCallBack pCallBack,void* pData);
//关闭CRFile文件句柄
void rfClose(CRFile* rfile);
