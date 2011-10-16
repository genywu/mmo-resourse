#include "StdAfx.h"
#include "ClientResource.h"
#include "../Module/Crc32Static.h"
#include "Package.h"
#include "RFile.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct tagReLoadReSource
{
	string	strFolderName;
	CClientResource* pClientResour;
	bool	bWithChild;
	tagReLoadReSource(string& strName,CClientResource* pClientRes,bool bFlag)
	:strFolderName(strName)
	,pClientResour(pClientRes)
	,bWithChild(bFlag)
	{
	}
private:
	void operator=(tagReLoadReSource&);
};

struct tagMakePackPro
{
	CClientResource* pOriginResour;
	CClientResource* pClientResour;
	tagMakePackPro(CClientResource* pOriginRes,CClientResource* pClientRes)
		:pOriginResour(pOriginRes)
		,pClientResour(pClientRes)
	{
	}
private:
	void operator=(tagReLoadReSource&);
};

CClientResource* CClientResource::instance=NULL;

CClientResource::CClientResource(void)
:m_strCurFolder("")
,m_strFileInfoName("")
,m_hWnd(NULL)
,m_pFilesInfo(NULL)
,m_eType(NO_RES)
{
}

CClientResource::CClientResource(eResourceType eType,string& strWorkFolder,Packages& TemptPackages,
								 string& strFileInfoName,HWND hwWnd)
:m_strCurFolder(strWorkFolder)
,m_pPackages(TemptPackages)
,m_strFileInfoName(strFileInfoName)
,m_hWnd(hwWnd)
,m_eType(eType)
,ahThread(0)
,m_hAsynReadThread(NULL)
{
	m_pFilesInfo = new CFilesInfo(hwWnd);
	//m_pFilesInfo->m_pOriginResource=this;

	m_bOver = true;					//是否覆盖
	m_bAlwaysCheck=false;			//是否始终此操作

	//Initial();
}

CClientResource::CClientResource(eResourceType eType,string& strWorkFolder,string& strFileInfoName,HWND hwWnd)
:m_strCurFolder(strWorkFolder)
,m_strFileInfoName(strFileInfoName)
,m_hWnd(hwWnd)
,m_eType(eType)
,ahThread(0)
,m_hAsynReadThread(NULL)
{
	m_pFilesInfo = new CFilesInfo(m_hWnd);
	m_bOver = true;					//是否覆盖
	m_bAlwaysCheck=false;			//是否始终此操作

	//Initial();
}

CClientResource::~CClientResource(void)
{
	itPackage it = m_pPackages.begin();
	for(;it != m_pPackages.end();it++)
	{
		tagPackageInfo& PackageInfo = (*it).second;
		if( PackageInfo.pPackage != NULL )
		{
			delete PackageInfo.pPackage;
			PackageInfo.pPackage=NULL;
		}
	}
	m_pPackages.clear();
	if(m_pFilesInfo)
		delete m_pFilesInfo;

//#ifdef _CLENTTOTEST_
//	s_mapNotFoundFiles.clear();
//#endif
}

void  CClientResource::InitialInstance()
{
	instance =new CClientResource();
}


CClientResource* CClientResource::GetInstance()
{		
	return instance;
}
void  CClientResource::ReleaseInstance()
{
	delete instance;
}

void CClientResource::Initial(eResourceType eType,
                              string& strWorkFolder,
                              string& strFileInfoName,
                              HWND hwWnd)
{
	m_strCurFolder=strWorkFolder;
	m_strFileInfoName=strFileInfoName;
	m_hWnd=hwWnd;
	m_eType=eType;

	m_pFilesInfo = new CFilesInfo(m_hWnd);
	m_bOver = true;					//是否覆盖
	m_bAlwaysCheck=false;			//是否始终此操作
	ahThread = 0;

	//异步读取线程
	StartAsynReadFileThread();
}

void	CClientResource::Release()
{
	if(ahThread)
		::WaitForSingleObject( ahThread,INFINITE );

	ExitAsynReadFileThread();
}


HANDLE	CClientResource::LoadClientResource()
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return 0;
	}
	
	//建立一个线程来完成此操作
	unsigned threadID;
	ahThread = (HANDLE)_beginthreadex(NULL,0,LoadClientResourceProc,this,0,&threadID);
	return ahThread;
}

//重新统计资源数据
HANDLE	CClientResource::ReLoadClientResourece(string& strSeachFold,bool bWithChild)
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return 0;
	}

	tagReLoadReSource* pReloadReSource = new tagReLoadReSource(strSeachFold,this,bWithChild);
	unsigned threadID;
	ahThread = (HANDLE)_beginthreadex(NULL,0,ReLoadClientResoureceProc,pReloadReSource,0,&threadID);
	return ahThread;
}


bool	CClientResource::LoadPackage(bool bUpdate)
{
	//装载包
	string strPackPath = m_strCurFolder+"\\Data";
	itPackage it = m_pPackages.begin();
	for(;it != m_pPackages.end();it++)
	{
		tagPackageInfo& PackageInfo = (*it).second;
		if( PackageInfo.pPackage != NULL )
		{
			delete PackageInfo.pPackage;
			PackageInfo.pPackage=NULL;
		}
		PackageInfo.pPackage = new CPackage(this,strPackPath);
		if(PackageInfo.strFileName != "")
			PackageInfo.pPackage->Open( PackageInfo.strFileName.c_str(),bUpdate);
	}
	return true;
}

void	CClientResource::OpenPackages(bool bUpdate)
{
	itPackage it = m_pPackages.begin();
	for(;it != m_pPackages.end();it++)
	{
		tagPackageInfo& PackageInfo = (*it).second;
		if( PackageInfo.pPackage != NULL )
		{
			PackageInfo.pPackage->Open(PackageInfo.strFileName.c_str(),bUpdate);
		}
	}
}

void	CClientResource::PackagesCloseHandle()
{
	itPackage it = m_pPackages.begin();
	for(;it != m_pPackages.end();it++)
	{
		tagPackageInfo& PackageInfo = (*it).second;
		if( PackageInfo.pPackage != NULL )
		{
			PackageInfo.pPackage->CloseFileHandle();
		}
	}
}

//装载资源
bool	CClientResource::Load()
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return 0;
	}

	OnLoadResourceStart();

	if(this->LoadPackage(false) == false)
	{
		m_pFilesInfo->Clear();
		return false;
	}

	HWND hOldWnd = m_pFilesInfo->GetDisplayWnd();
	string strTempt = m_strCurFolder+"\\"+m_strFileInfoName;
	m_pFilesInfo->Clear();
	if(m_pFilesInfo->Load(strTempt)==false)
	{
		m_pFilesInfo->Clear();
		m_pFilesInfo->Save(strTempt);

		return false;
	}

	m_pFilesInfo->UpdateAllInfoToViewEx(hOldWnd);
	OnLoadResourceEnd();
	return true;
}

//bool	CClientResource::LoadORCreatePackages()
//{
//	//装载包
//	string strPackPath = m_strCurFolder+"\\Data";
//	itPackage it = m_pPackages.begin();
//	for(;it != m_pPackages.end();it++)
//	{
//		tagPackageInfo& PackageInfo = (*it).second;
//		if( PackageInfo.pPackage != NULL )
//		{
//			delete PackageInfo.pPackage;
//			PackageInfo.pPackage=NULL;
//		}
//		PackageInfo.pPackage = new CPackage(this,strPackPath);
//		if(PackageInfo.strFileName != "")
//		{
//			if( PackageInfo.pPackage->Open( PackageInfo.strFileName.c_str(),false) == false)
//				PackageInfo.pPackage->Create(PackageInfo.strFileName.c_str());
//		}
//	}
//	return true;
//}


ulong   CClientResource::GetFileContentCRC(string& strFileName)
{
	ulong dwReCRC32 = 0;
	tagFileInfo* ptagTemFileInfo = m_pFilesInfo->GetFileInfoByText(strFileName);
	if(ptagTemFileInfo)
	{
		if( (ptagTemFileInfo->dwPackageType&0x1) == 1)
		{
			string strTemptFile = m_strCurFolder+strFileName;
			CCrc32Static::FileCrc32Filemap(strTemptFile.c_str(),dwReCRC32);
		}
		else
		{
			CPackage* pPackage = this->GetPackage(ptagTemFileInfo->dwPackageType);
			if(pPackage)
			{
				ulong dwSize = pPackage->GetFileSize(strFileName.c_str());
				if(dwSize>0)
				{
					uchar* pBuf = new uchar[dwSize+1];
					ulong dwCrc32,dwCompress, dwOrginSize, dwValidSize;
					pPackage->ExtractToBuf(strFileName.c_str(),
						pBuf,dwSize,dwCrc32,dwCompress, dwOrginSize, dwValidSize);
					CCrc32Static::DataCrc32(pBuf,dwSize,dwReCRC32);
					delete[] pBuf;
				}
			}
		}
	}
	return dwReCRC32;
}
//提供给客户端和升级包使用的借口
bool	CClientResource::LoadEx()
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return 0;
	}

	OnLoadResourceStart();

	string strTempt = m_strCurFolder+"\\"+m_strFileInfoName;
	m_pFilesInfo->Clear();
	if(m_pFilesInfo->Load(strTempt)==false)
	{
		return false;
	}

	//初始化包信息
	PackTypeInfos& TemptPackageInfos = m_pFilesInfo->GetPackageTypeInfo();
	itTypeInfo it = TemptPackageInfos.begin();
	for(;it != TemptPackageInfos.end();it++)
	{
		tagPackageInfo PackageInfo = {(*it).dwID,(*it).strFileName,NULL };
		m_pPackages[(*it).dwID] = PackageInfo;
	}

	//打开包
	if(LoadPackage(false)==false)
	{
		return false;
	}

	OnLoadResourceEnd();
	return true;
}

bool	CClientResource::LoadExForAutoUpdate()
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return 0;
	}

	OnLoadResourceStart();

	string strTempt = m_strCurFolder+"\\"+m_strFileInfoName;
	m_pFilesInfo->Clear();
	if(m_pFilesInfo->Load(strTempt)==false)
	{
		return false;
	}

	//初始化包信息
	PackTypeInfos& TemptPackageInfos = m_pFilesInfo->GetPackageTypeInfo();
	itTypeInfo it = TemptPackageInfos.begin();
	for(;it != TemptPackageInfos.end();it++)
	{
		tagPackageInfo PackageInfo = {(*it).dwID,(*it).strFileName,NULL };
		m_pPackages[(*it).dwID] = PackageInfo;
	}

	OnLoadResourceEnd();
	return true;
}

void	CClientResource::ResetPackInfosForAutoUpdate(PackTypeInfos& TemptPackages)
{
	m_pPackages.clear();
	m_pFilesInfo->SetPackageTypeInfo(TemptPackages);

	//初始化包信息
	PackTypeInfos& TemptPackageInfos = m_pFilesInfo->GetPackageTypeInfo();
	itTypeInfo it = TemptPackageInfos.begin();
	for(;it != TemptPackageInfos.end();it++)
	{
		tagPackageInfo PackageInfo = {(*it).dwID,(*it).strFileName,
									(*it).dwIndexNum,(*it).dwEmptyIndexNum,NULL };
		m_pPackages[(*it).dwID] = PackageInfo;
	}
}

//提供给更新使用,不用及时打开包文件
bool	CClientResource::LoadEx(PackTypeInfos& PacksInfo)
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return 0;		
	}

	string strTempt = m_strCurFolder+"\\"+m_strFileInfoName;
	m_pFilesInfo->Clear();
	if(m_pFilesInfo->Load(strTempt)==false)
	{
		return false;
	}

	m_pFilesInfo->SetPackageTypeInfo(PacksInfo);
	//初始化包信息
	PackTypeInfos& TemptPackageInfos = m_pFilesInfo->GetPackageTypeInfo();
	itTypeInfo it = TemptPackageInfos.begin();
	for(;it != TemptPackageInfos.end();it++)
	{
		tagPackageInfo PackageInfo = {(*it).dwID,(*it).strFileName,
									(*it).dwIndexNum,(*it).dwEmptyIndexNum,NULL };
		m_pPackages[(*it).dwID] = PackageInfo;
	}

	OnLoadResourceEnd();
	return true;
}
//保存资源信息
bool	CClientResource::Save()
{
	string strTempt = m_strCurFolder+"\\"+m_strFileInfoName;
	//在资源目录保存最新资源文件信息
	m_pFilesInfo->Save(strTempt);

	//保存版本文件
	SaveVersionToFile();
	return true;
}

bool	CClientResource::UpdateSave()
{
	string strTempt = m_strCurFolder+"\\"+"PackRes.cfg";
	//在资源目录保存最新资源文件信息
	m_pFilesInfo->Save(strTempt);

	itPackage it = m_pPackages.begin();
	for(;it != m_pPackages.end();it++)
	{
		tagPackageInfo& PackageInfo = (*it).second;
		if( PackageInfo.pPackage != NULL )
		{
			PackageInfo.pPackage->WriteIndexEx();
		}
	}
	return true;
}

bool	CClientResource::SaveVersionToFile()
{
	string strFileName = m_strCurFolder+"\\Game.vs";
	if(FILE* pf =fopen(strFileName.c_str(),"wb"))
	{
		fprintf(pf,"%s",m_pFilesInfo->GetVersion().c_str());
		fclose(pf);
	}
	return true;
}

bool CClientResource::Clear()
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return 0;
	}

	m_pFilesInfo->Clear();

	itPackage it = m_pPackages.begin();
	for(;it != m_pPackages.end();it++)
	{
		tagPackageInfo& PackageInfo = (*it).second;
		if( PackageInfo.pPackage != NULL )
		{
			delete PackageInfo.pPackage;
			PackageInfo.pPackage = NULL;
		}
	}

	return true;
}

bool	CClientResource::Create()
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return 0;
	}

	m_pFilesInfo->Clear();

	CreatePackages();
	return true;
}


bool	CClientResource::CreatePackages()
{
	//创建包
	string strPackPath = m_strCurFolder+"\\Data";
	itPackage it = m_pPackages.begin();
	for(;it != m_pPackages.end();it++)
	{
		tagPackageInfo& PackageInfo = (*it).second;
		if( PackageInfo.pPackage != NULL )
		{
			delete PackageInfo.pPackage;
			PackageInfo.pPackage=NULL;
		}
		PackageInfo.pPackage = new CPackage(this,strPackPath,
								PackageInfo.dwIndexNum,PackageInfo.dwEmptyIndexNum);
		PackageInfo.pPackage->Create(PackageInfo.strFileName.c_str());
	}
	return true;
}

CPackage* CClientResource::GetPackage(ulong dwPackID)
{
	itPackage it = m_pPackages.find(dwPackID);
	if(it != m_pPackages.end())
		return (*it).second.pPackage;
	return NULL;
}

CPackage* CClientResource::GetPackageForUpdate(ulong dwPackID)
{
	string strPackPath = m_strCurFolder+"\\Data";
	itPackage it = m_pPackages.find(dwPackID);
	if(it != m_pPackages.end())
	{
		tagPackageInfo& PackageInfo = (*it).second;
		if(PackageInfo.pPackage == NULL)
		{
			PackageInfo.pPackage = new CPackage(this,strPackPath,
								PackageInfo.dwIndexNum,PackageInfo.dwEmptyIndexNum);
			PackageInfo.pPackage->OpenForUpdate( PackageInfo.strFileName.c_str());
		}
		return PackageInfo.pPackage;
	}
	return NULL;
}

void	CClientResource::PackagesWriteIndexEx()
{
	itPackage it = m_pPackages.begin();
	for(;it != m_pPackages.end();it++)
	{
		tagPackageInfo& PackageInfo = (*it).second;
		if( PackageInfo.pPackage != NULL )
		{
			PackageInfo.pPackage->WriteIndexEx();
		}
	}
}
//根据新的信息列表制作包
HANDLE	CClientResource::MakePakcage(CClientResource* pClientResource)
{
	if(ahThread)	
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return 0;
	}

	if(pClientResource==NULL)
		return 0;
	tagMakePackPro* pMakePackProc = new tagMakePackPro(this,pClientResource);

	unsigned threadID;
	ahThread = (HANDLE)_beginthreadex(NULL,0,MakePakcageProc,pMakePackProc,0,&threadID);

	return ahThread;
}

bool	CClientResource::MakePackage(tagFileInfo* pTagFileInfo,CFilesInfo* pFilesInfo,
									 CClientResource* pClientResource)
{
	if(pTagFileInfo==NULL || pFilesInfo == NULL ||pClientResource==NULL)	return false;

	string strTempt = pFilesInfo->GetFileTextWithParent(pTagFileInfo);
	if(pFilesInfo->HaveChild(pTagFileInfo))
	{
		//添加一个文件夹
		pClientResource->AddFolder(strTempt,pTagFileInfo->dwPackageType,
									pTagFileInfo->dwCompressType);

		for(itFileInfo it = pTagFileInfo->ChildFilesInfo.begin();
			it != pTagFileInfo->ChildFilesInfo.end();it++)
		{
			MakePackage((*it).second,pFilesInfo,pClientResource);
		}
	}
	else
	{
		if(pTagFileInfo->bFolder==false)
		{
			if( !(pTagFileInfo->dwPackageType & 0x1) )
			{
				OnMakePackageing(pTagFileInfo,pFilesInfo);
			}

			pClientResource->AddFile(	m_strCurFolder,strTempt,pTagFileInfo->dwSize,
										pTagFileInfo->dwCrc32,pTagFileInfo->dwPackageType,
										pTagFileInfo->dwCompressType);
		}		
	}
	return true;
}

//重新制作包
HANDLE	CClientResource::ReMakePackage()
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return 0;
	}

	itPackage it = m_pPackages.begin();
	for(;it != m_pPackages.end();it++)
	{
		tagPackageInfo& PackageInfo = (*it).second;
		if( PackageInfo.pPackage == NULL )
		{
			return 0;
		}
	}

	unsigned threadID;
	ahThread = (HANDLE)_beginthreadex(NULL,0,ReMakePakcageProc,this,0,&threadID);
	return ahThread;
}

//重新整理
bool	CClientResource::ReMakePackage(tagFileInfo* pTagFileInfo,CPackage* pNewPackage)
{
	if(pTagFileInfo==NULL || pNewPackage == NULL)	return false;

	if(m_pFilesInfo->HaveChild(pTagFileInfo))
	{
		for(itFileInfo it = pTagFileInfo->ChildFilesInfo.begin();
			it != pTagFileInfo->ChildFilesInfo.end();it++)
		{
			ReMakePackage((*it).second,pNewPackage);
		}
	}
	else
	{
		//是包文件
		if(pTagFileInfo->bFolder==false && !( pTagFileInfo->dwPackageType&0x1) )
		{
			CPackage* pPackage = GetPackage(pTagFileInfo->dwPackageType);
			if(pPackage)
			{
				ulong dwSize = pPackage->GetFileSize(
														m_pFilesInfo->GetFileTextWithParent(pTagFileInfo).c_str()
														);
				if(dwSize>0)
				{
					tagFileIndex FileIndex;
					uchar* pBuf = new uchar[dwSize];
					if(pPackage->ExtractToBuf(
						m_pFilesInfo->GetFileTextWithParent(pTagFileInfo).c_str(),pBuf,dwSize,FileIndex))
					{
						FileIndex.dwValidSize = FileIndex.dwSize;
						int nRet = pNewPackage->WriteData(FileIndex,pBuf);
						if(nRet == false)
						{
							return false;
						}
						
					}
					delete []pBuf;
				}
			}
		}
	}
	return true;
}

//添加一个文件
bool	CClientResource::AddFile(string& strPos,string& strFileName)
{
	tagFileInfo* ptagTemFileInfo = m_pFilesInfo->GetFileInfoByText(strPos);
	if(ptagTemFileInfo == NULL || ptagTemFileInfo->bFolder == false)	return false;

	string strTemptName = strFileName;
	strTemptName = strTemptName.erase(0,strFileName.find_last_of("\\")+1);
	string strNewTemptFile = strPos+"\\"+strTemptName;

	ulong dwCrc32;
	CCrc32Static::FileCrc32Filemap(strFileName.c_str(),dwCrc32);

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = ::FindFirstFile(strFileName.c_str(), &FindFileData);

	if(hFind == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFind);

	ulong	dwPackType = 1;
	ulong	dwCompType = 1;
	tagFileInfo* ptagFileInfo = m_pFilesInfo->GetFileInfoByText(strNewTemptFile);
	/*if(ptagFileInfo && !(ptagFileInfo->dwPackageType&0x1))
	{
		FILE* pf= fopen(strFileName.c_str(),"rb");
		if(pf )
		{
			CPackage* pPackage = GetPackage(ptagFileInfo->dwPackageType);
			if(pPackage)
			{
				ulong dwOrginSize = 0;
				ulong dwSize = pPackage->AddFile(pf,strNewTemptFile.c_str(),dwCrc32,
					ptagFileInfo->dwCompressType, dwOrginSize);
				m_pFilesInfo->UpdateFileInfo(strNewTemptFile,dwCrc32,dwSize,
											dwOrginSize,dwSize,false,1,ptagFileInfo->dwCompressType);
			}
			fclose(pf);
		}
	}*/
	if(ptagFileInfo)
	{
		dwPackType = ptagFileInfo->dwPackageType;
		dwCompType = ptagFileInfo->dwCompressType;
	}
	//else
	{ 
		strNewTemptFile = m_strCurFolder+strNewTemptFile;
		::CopyFile(strFileName.c_str(),strNewTemptFile.c_str(),false);

		//CString str(strNewTemptFile.c_str());
		//str.Replace(m_strCurFolder.c_str(),"");

		string strName(strNewTemptFile.c_str());
		strName.erase(0,m_strCurFolder.length());

		m_pFilesInfo->UpdateFileInfo(strName,dwCrc32,FindFileData.nFileSizeLow,
			FindFileData.nFileSizeLow,FindFileData.nFileSizeLow,false,dwPackType,dwCompType);

	}
	return true;
}

//从缓冲区一个文件
bool	CClientResource::AddFile(string& strFileName,uchar* pBuf,long lSize)
{
	return true;
}

//添加一个没文件内容的文件,下载是用
bool	CClientResource::AddEmptyFile(const char* strFileName,int nSize,ulong dwCrc32,
									  ulong dwPackage,ulong dwCompress,int dwOrginSize)
{
	if( !(dwPackage&0x1) )
	{
		CPackage *pPackage = this->GetPackage(dwPackage);
		if(pPackage == NULL)	return false;

		if( pPackage->AddFile(strFileName, nSize,dwCrc32,dwCompress, dwOrginSize) == false )
		{
			return false;
		}
	}
	else
	{
		string strTempt = m_strCurFolder+strFileName;
		FILE* pf = fopen(strTempt.c_str(),"w+b");
		if(pf==false)
		{
			Log4c::Warn(ROOT_MODULE, "[PACKAGE] 创建文件 %s 出错", strFileName);
			return false;
		}
		fclose(pf);
	}

	string strTemptFileName(strFileName);
	m_pFilesInfo->UpdateFileInfo(strTemptFileName,dwCrc32,nSize,
								dwOrginSize,0,false,dwPackage,dwCompress);
	return true;
}
bool	CClientResource::AddFileData(const char* strFileName,uchar* pBuf,
									 int nSize)
{
	if(nSize <= 0)
		return true;
	string strFile(strFileName);
	string strTemptFileName(strFileName);
	tagFileInfo* ptagFileInfo = m_pFilesInfo->GetFileInfoByText(strTemptFileName);
	if(ptagFileInfo==NULL)	return false;
	if(ptagFileInfo->dwValidSize+nSize > ptagFileInfo->dwSize)	return false;

	if( !(ptagFileInfo->dwPackageType&0x1) )
	{
		CPackage *pPackage = GetPackage(ptagFileInfo->dwPackageType);
		if(pPackage == NULL)	return false;

		if(pPackage->AddFileData(
			strFileName,pBuf,ptagFileInfo->dwValidSize,nSize))
		{
			m_pFilesInfo->UpdateFileInfo(strFile,ptagFileInfo->dwCrc32,
								ptagFileInfo->dwSize,ptagFileInfo->dwOrginSize,
								ptagFileInfo->dwValidSize+nSize,false,1,ptagFileInfo->dwCompressType);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if( (ptagFileInfo->dwPackageType&0x1)==1 )
	{
		strTemptFileName = m_strCurFolder+strTemptFileName;

		FILE* pf = fopen(strTemptFileName.c_str(), "r+b");
		if(pf)
		{
			int nRet = fseek(pf,ptagFileInfo->dwValidSize,SEEK_SET);
			if(nRet != 0)
			{
				Log4c::Warn(ROOT_MODULE, "[PACKAGE] fseek文件 %s 出错", strFileName);
				return false;
			}

			nRet = (int)fwrite(pBuf,nSize,1,pf);
			if(nRet != 1)
			{
				Log4c::Warn(ROOT_MODULE, "[PACKAGE] 写文件 %s 出错", strFileName);
				return false;
			}
			nRet = fflush(pf);
			if(nRet != 0)
			{
				Log4c::Warn(ROOT_MODULE, "[PACKAGE] fflush文件 %s 出错", strFileName);
				return false;
			}

			fclose(pf);
			m_pFilesInfo->UpdateFileInfo(strFile,ptagFileInfo->dwCrc32,
										ptagFileInfo->dwSize,ptagFileInfo->dwOrginSize,
										ptagFileInfo->dwValidSize+nSize,false,0,ptagFileInfo->dwCompressType);
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool	CClientResource::IsFileExist(const char* strFileName)
{
	string strName(strFileName);
	if(strName.find_first_of("\\") != 0 )
	{
		strName+="\\";
	}
	if(m_pFilesInfo->GetFileInfoByText(strName))
		return true;
	return false;
}

bool	CClientResource::IsFileBodyExist(ulong dwPackage,const char* strFileName)
{
	string strName(strFileName);
	if(strName.find_first_of("\\") != 0 )
	{
		strName+="\\";
	}

	if( (dwPackage&0x1) )
	{
		strName = m_strCurFolder+strName;
		WIN32_FIND_DATA	  FindFileData;
		HANDLE            hFind;
		// 寻找一个文件
		hFind = FindFirstFile ( strName.c_str(), &FindFileData );
		if(hFind == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		FindClose(hFind);
	}
	else
	{
		CPackage *pPackage = GetPackage(dwPackage);
		if(pPackage == NULL)	return false;

		if(pPackage->GetFileSize(strName.c_str()) == 0)
			return false;
	}
	return true;
}

bool CClientResource::AdjustFile(const char* strFileName,
									ulong dwTemptPackage,ulong dwTemptCompress)
{
	string strTemptFileName(strFileName);

	ulong dwSize=0,dwOrginSize=0,dwValidSzie=0,dwCrc32=2;
	ulong dwPackageType=1,dwCompressType=1;bool bFolder = false;
	if(m_pFilesInfo->GetFileInfo(strTemptFileName,dwSize,dwOrginSize,
		dwValidSzie,dwCrc32,dwPackageType,dwCompressType,bFolder))
	{			
		if(dwTemptPackage != dwPackageType)
		{// 提取文件
			if( (dwTemptPackage&0x1) && !(dwPackageType&0x1) )
			{
				CPackage *pPackage = GetPackage(dwPackageType);
				if(pPackage == NULL)	return false;

				dwSize=pPackage->ExtractToFile(strTemptFileName.c_str());
				pPackage->DelFile(strTemptFileName.c_str());
			}
			else if( !(dwTemptPackage&0x1) && (dwPackageType&0x1) )
			{
				CPackage *pPackage = GetPackage(dwTemptPackage);
				if(dwTemptPackage == NULL)	return false;

				dwSize = pPackage->AddFile(strTemptFileName.c_str(),
											dwCrc32,dwTemptCompress, dwOrginSize);
				if(dwSize == false)
				{
					return false;
				}
				string strFileName = string(m_strCurFolder)+strTemptFileName;
				if(::DeleteFile(strFileName.c_str())==false)
				{
					//AfxMessageBox("删除文件错误！");
				}

			}
			else//包之间的调整，暂不考虑
			{
				CPackage *pTemptPackage = GetPackage(dwTemptPackage);
				if(pTemptPackage == NULL)	return false;
				CPackage *pPackage = GetPackage(dwPackageType);
				if(pPackage == NULL)	return false;
			}

		}
		else 
		{
			//在包内，压缩不同调整
			if( (!(dwPackageType&0x1)) && dwTemptCompress != dwCompressType)
			{
				CPackage *pPackage = GetPackage(dwPackageType);
				if(pPackage == NULL)	return false;

				dwSize = pPackage->AdjustFile(strTemptFileName.c_str(),dwTemptCompress);
				if(dwSize == false)
					return false;
			}
		}
		m_pFilesInfo->UpdateFileInfo(strTemptFileName,dwCrc32,dwSize,
									dwOrginSize,dwSize,bFolder,dwTemptPackage,
									dwTemptCompress);
	}
	return true;
}

//添加一个文件夹
bool	CClientResource::AddFolder(string strFolderName,string strWorkPos)
{
	WIN32_FIND_DATA	  FindFileData;
	HANDLE            hFind;

	// 寻找第一个文件
	hFind = FindFirstFile ( "*.*", &FindFileData );

	// 搜索所有文件及子目录
	do
	{
		if ( hFind != INVALID_HANDLE_VALUE )
		{
			// 如果是当前目录或父目录，跳过
			if ( !( strcmp( FindFileData.cFileName, "." ) ) ||
				 !( strcmp( FindFileData.cFileName, ".." ) ) )
			{
				continue;
			}

			string strTempt = strFolderName+"\\"+FindFileData.cFileName;

			string strNewTemptFile = strWorkPos+"\\"+FindFileData.cFileName;
			
			// 如果是一个目录，递归调用继续搜索子目录
			if ( SetCurrentDirectory(strTempt.c_str()) )
			{				
				string strCreateFolder = m_strCurFolder+strNewTemptFile;
				::CreateDirectory(strCreateFolder.c_str(),NULL);

				m_pFilesInfo->UpdateFileInfo(strNewTemptFile,0,0,0,0,true,1,1);

				AddFolder(strTempt,strNewTemptFile);
			}
			else
			{

				//CString strTemptFileName = FindFileData.cFileName;
				//strTemptFileName = strTemptFileName.Right(strTemptFileName.GetLength()-strTemptFileName.Find(".")-1);
				//strTemptFileName.MakeUpper();
				////是包文件、不操作
				//strTemptFileName.MakeUpper();

				string strTemptFileName(FindFileData.cFileName);
				strTemptFileName.erase(0,strTemptFileName.find_last_of(".") + 1);	

				//strupr(strTemptFileName.c_str());
				
				if(strTemptFileName =="pak"  || strTemptFileName == "cfg" || strTemptFileName == "vs")	continue;

				AddFile(strWorkPos,strTempt);
			}
		}
	}
	while ( FindNextFile( hFind, &FindFileData ) && hFind != INVALID_HANDLE_VALUE );

	FindClose ( hFind );
	return true;
}

//提取一个文件夹或文件
bool	CClientResource::ExtrFileFolder(tagFileInfo* ptagFileInfo,string strFolder,
										CFilesInfo* pFilesInfo)
{
	CFilesInfo* pTemptFilesInfo = pFilesInfo;
	if(pTemptFilesInfo==NULL)
		pTemptFilesInfo = m_pFilesInfo;

	//if(ptagFileInfo==NULL || ptagFileInfo == pTemptFilesInfo->GetRootFile())	return false;
	if(ptagFileInfo==NULL)	return false;

	if(pTemptFilesInfo->HaveChild(ptagFileInfo))
	{
		string strDirectory = strFolder+"\\"+ptagFileInfo->strFileName;
		::CreateDirectory(strDirectory.c_str(),NULL);

		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			ExtrFileFolder((*it).second,strDirectory,pFilesInfo);	
		}
	}
	else
	{
		if(ptagFileInfo->bFolder==false)
		{
			string strFileName = pTemptFilesInfo->GetFileTextWithParent(ptagFileInfo);
			string strNewFileName = strFolder+"\\"+ptagFileInfo->strFileName;

			//打包并且是游戏资源
			if(	!(ptagFileInfo->dwPackageType&0x1) && m_eType == GAME_RES )
			{
				CPackage* pPackage = GetPackage(ptagFileInfo->dwPackageType);
				if(pPackage)
					pPackage->ExtractToFile(strFileName.c_str(),strNewFileName);
			}
			else
			{
				strFileName = m_strCurFolder+strFileName;
				::CopyFile(strFileName.c_str(),strNewFileName.c_str(),false);
			}
		}
	}
	return true;
}

//提取一个文件夹或文件，生成自动更新目录
bool	CClientResource::ExtrAutoUpdateFileFolder(tagFileInfo* ptagFileInfo,
												  string strFolder,CFilesInfo* pFilesInfo)
{
	CFilesInfo* pTemptFilesInfo = pFilesInfo;
	if(pTemptFilesInfo==NULL)
		pTemptFilesInfo = m_pFilesInfo;

	//if(ptagFileInfo==NULL || ptagFileInfo == pTemptFilesInfo->GetRootFile())	return false;
	if(ptagFileInfo==NULL)	return false;

	if(pTemptFilesInfo->HaveChild(ptagFileInfo))
	{
		string strDirectory = strFolder+"\\"+ptagFileInfo->strFileName;
		::CreateDirectory(strDirectory.c_str(),NULL);

		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			ExtrAutoUpdateFileFolder((*it).second,strDirectory,pFilesInfo);	
		}
	}
	else
	{
		if(ptagFileInfo->bFolder==false)
		{
			string strFileName = pTemptFilesInfo->GetFileTextWithParent(ptagFileInfo);
			strFileName = m_strCurFolder+strFileName;
			string strNewFileName = strFolder+"\\"+ptagFileInfo->strFileName;

			//无包或无压缩
			if( ENABLEBIT(ptagFileInfo->dwPackageType,NOPACK) || ENABLEBIT(ptagFileInfo->dwCompressType,NOCOMPRESS) )
			{
				::CopyFile(strFileName.c_str(),strNewFileName.c_str(),false);
			}
			else if(ptagFileInfo->dwOrginSize > 0)
			{
				FILE* fp = fopen(strFileName.c_str(), "rb");
				if(fp)
				{
					uchar *lpszIn = new uchar[ptagFileInfo->dwOrginSize];
					uchar *lpszOut = new uchar[ptagFileInfo->dwOrginSize*2];
					fread(lpszIn, ptagFileInfo->dwOrginSize, 1, fp);
					ulong nOutSize = ptagFileInfo->dwOrginSize*2;

					if( ENABLEBIT(ptagFileInfo->dwCompressType,0x4) )
						Compress(lpszIn, ptagFileInfo->dwOrginSize, lpszOut, &nOutSize, false);
					else
						CompressData(lpszIn, ptagFileInfo->dwOrginSize, lpszOut, &nOutSize, false);


					FILE* pnewf = fopen(strNewFileName.c_str(),"wb");
					if(pnewf)
					{
						fwrite(lpszOut, nOutSize, 1, pnewf);
						fclose(pnewf);
					}

					ptagFileInfo->dwSize = nOutSize;
					ptagFileInfo->dwValidSize = nOutSize;
				
					delete []lpszIn;
					delete []lpszOut;
					fclose(fp);
				}
			}
		}

		//if( !ENABLEBIT(ptagFileInfo->dwPackageType,NOPACK) )
		//{
		//	CPackage *pPackage = GetPackage(ptagFileInfo->dwPackageType);
		//	if(pPackage)
		//		pPackage->ExtractToFile(strFileName.c_str(),strNewFileName);
		//}
		//else
		//{
		//	strFileName = m_strCurFolder+strFileName;

		//	// 压缩
		//	if (  !ENABLEBIT(ptagFileInfo->dwCompressType,NOCOMPRESS) )
		//	{
		//		FILE* fp = fopen(strFileName.c_str(), "rb");
		//		if(!fp)
		//		{
		//			Log4c::Warn(ROOT_MODULE, "[PACKAGE] 创建文件 %s 出错", strFileName);
		//			return false;
		//		}
		//		assert(fp);

		//		int nResult = fseek(fp, 0, SEEK_END);
		//		if(nResult)
		//		{
		//			Log4c::Warn(ROOT_MODULE, "[PACKAGE] fseek文件 %s 出错", strFileName);
		//			return false;
		//		}

		//		ulong dwSize = ftell(fp);
		//		if(nResult == -1L)
		//		{
		//			Log4c::Warn(ROOT_MODULE, "[PACKAGE] ftell文件 %s 出错", strFileName);
		//			return false;
		//		}

		//		nResult = fseek(fp, 0, SEEK_SET);
		//		if(nResult)
		//		{
		//			Log4c::Warn(ROOT_MODULE, "[PACKAGE] fseek文件 %s 出错", strFileName);
		//			return 0;
		//		}

		//		if(dwSize > 0)
		//		{
		//			uchar *lpszIn = new uchar[dwSize];
		//			uchar *lpszOut = new uchar[dwSize*2];
		//			fread(lpszIn, dwSize, 1, fp);
		//			ulong nOutSize = 0;

		//			if( ENABLEBIT(ptagFileInfo->dwCompressType,0x4))
		//				Compress(lpszIn, dwSize, lpszOut, &nOutSize, 1);
		//			else
		//				CompressData(lpszIn, dwSize, lpszOut, &nOutSize, false);

		//			ptagFileInfo->dwOrginSize = dwSize;
		//			ptagFileInfo->dwSize = nOutSize;
		//			ptagFileInfo->dwValidSize = nOutSize;

		//			FILE* pf = fopen(strNewFileName.c_str(),"wb");
		//			if(pf)
		//			{
		//				fwrite(lpszOut, nOutSize, 1, pf);
		//			}
		//			fclose(pf);

		//			delete []lpszIn;
		//			delete []lpszOut;
		//		}
		//		else	// 空文件
		//		{
		//			::CopyFile(strFileName.c_str(),strNewFileName.c_str(),false);
		//		}

		//		fclose(fp);	
		//	}
		//	else	// 不压缩
		//	{
		//::CopyFile(strFileName.c_str(),strNewFileName.c_str(),false);
		//	}
		//}		
	}
	return true;
}

//删除一个文件夹或文件
bool	CClientResource::DelFileFolder(tagFileInfo* ptagFileInfo)
{
	if(ptagFileInfo==NULL || ptagFileInfo == m_pFilesInfo->GetRootFile())	return false;

	if(m_pFilesInfo->HaveChild(ptagFileInfo))
	{
		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();)
		{
			DelFileFolder((*it).second);

			ptagFileInfo->ChildFilesInfo.erase(it++);		
		}

		string strDirectory = m_strCurFolder+m_pFilesInfo->GetFileTextWithParent(ptagFileInfo);
		::RemoveDirectory(strDirectory.c_str());

		m_pFilesInfo->OnDelete(ptagFileInfo);
		delete ptagFileInfo;
	}
	else
	{
		string strFileName = m_strCurFolder+m_pFilesInfo->GetFileTextWithParent(ptagFileInfo);

		if( !ENABLEBIT(ptagFileInfo->dwPackageType,NOPACK) && ptagFileInfo->bFolder == false)
		{
			CPackage	*pPackage = GetPackage(ptagFileInfo->dwPackageType);
			if(pPackage)
				pPackage->DelFile(m_pFilesInfo->GetFileTextWithParent(ptagFileInfo).c_str());
		}
		else
		{
			if(ptagFileInfo->bFolder)
			{
				if(!::RemoveDirectory(strFileName.c_str()))
				{
					//ulong dwLastError = ::GetLastError();
				}
			}
			else
			{
				::DeleteFile(strFileName.c_str());
				//减去相关大小
				int nSize = ptagFileInfo->dwSize;
				m_pFilesInfo->AddFileSize(ptagFileInfo->ParentFileInfo,-nSize);
			}
		}

		m_pFilesInfo->OnDelete(ptagFileInfo);
		delete ptagFileInfo;
	}
	return true;
}

//删除一个文件夹或文件
bool	CClientResource::DelFileFolder(tagFileInfo* ptagFileInfo,CFilesInfo* pFileInfo)
{
	if(ptagFileInfo==NULL || ptagFileInfo == m_pFilesInfo->GetRootFile() || pFileInfo==NULL)
		return false;

	if(pFileInfo->HaveChild(ptagFileInfo))
	{
		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			DelFileFolder((*it).second,pFileInfo);		
		}

		string strDirectory = m_strCurFolder+m_pFilesInfo->GetFileTextWithParent(ptagFileInfo);
		::RemoveDirectory(strDirectory.c_str());

	}
	else
	{
		string strFileName = pFileInfo->GetFileTextWithParent(ptagFileInfo);
		if(ptagFileInfo->bFolder==false)
		{
			//删除一个文件
			OnDelFileFolder(strFileName,strFileName);
		}
		else
		{
			strFileName = m_strCurFolder+pFileInfo->GetFileTextWithParent(ptagFileInfo);
			::RemoveDirectory(strFileName.c_str());
		}
	}
	
	return true;
}

bool	CClientResource::OnAddFolder(string& strPos,string& strFolder)
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return false;
	}

	tagFileInfo* ptagFileInfo = m_pFilesInfo->GetFileInfoByText(strPos);
	if(ptagFileInfo == NULL || ptagFileInfo->bFolder == false)
	{
		return false;
	}

	// 设置为当前路径
	if(SetCurrentDirectory( strFolder.c_str() ))
	{
		string strFileName = strFolder;
		strFileName = strFileName.erase(0,strFileName.find_last_of("\\")+1);

		string strNewTemptFile = strPos+"\\"+strFileName;

		// 如果是一个目录，递归调用继续搜索子目录
		string strCreateFolder = m_strCurFolder+strNewTemptFile;
		::CreateDirectory(strCreateFolder.c_str(),NULL);
		m_pFilesInfo->UpdateFileInfo(strNewTemptFile,0,0,0,0,true,1,1);

		AddFolder(strFolder.c_str(),strNewTemptFile);
	}

	return true;
}
bool	CClientResource::OnAddFile(string& strPos,string& strFileName)
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return 0;
	}

	AddFile(strPos,strFileName);
	return true;
}
bool	CClientResource::OnDelFileFolder(string& strPos,string& strFolderFileName)
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return false;
	}

	string strDelPos = strPos;
	tagFileInfo* ptagFileInfo = m_pFilesInfo->GetFileInfoByText(strPos);
	if(ptagFileInfo==NULL || m_pFilesInfo->GetRootFile()==ptagFileInfo)
	{
		return false;
	}

	tagFileInfo* pParentFileInfo = ptagFileInfo->ParentFileInfo;
	string strName = ptagFileInfo->strFileName;
	DelFileFolder(ptagFileInfo);
	pParentFileInfo->ChildFilesInfo.erase(strName);
    return true;
}

bool	CClientResource::OnExtrFileFolder(string& strPos,string& strFolderFileName)
{
	if(ahThread)
	{
		if(::WaitForSingleObject( ahThread,0 )==WAIT_TIMEOUT)
			return false;
	}

	string strDelPos = strPos;
	tagFileInfo* ptagFileInfo = m_pFilesInfo->GetFileInfoByText(strPos);
	if(ptagFileInfo==NULL)
	{
		return false;
	}
	ExtrFileFolder(ptagFileInfo,strFolderFileName);
	return true;
}

//搜索子文件夹
void CClientResource::SearchChildFolder(CHAR* strFolder,tagFileInfo* pParenttagFileInfo)
{
	if(pParenttagFileInfo==NULL)	return;

	WIN32_FIND_DATA	  FindFileData;
	HANDLE            hFind;

	// 寻找第一个文件
	hFind = FindFirstFile ( "*.*", &FindFileData );

	// 搜索所有文件及子目录
	do
	{
		if ( hFind != INVALID_HANDLE_VALUE )
		{
			// 如果是当前目录或父目录，跳过
			if ( ! ( strcmp( FindFileData.cFileName, "." ) ) ||
				 ! ( strcmp( FindFileData.cFileName, ".." ) ) )
			{
				continue;
			}

			CHAR strTempt[MAX_PATH];
			strcpy_s( strTempt, MAX_PATH, strFolder );
			strcat_s(strTempt, MAX_PATH, "\\");
			strcat_s(strTempt, MAX_PATH, FindFileData.cFileName );

			// 如果是一个目录，递归调用继续搜索子目录
			if ( SetCurrentDirectory(strTempt) )
			{
				string strName(strTempt);
				strName.erase(0,m_strCurFolder.length());

				string str(FindFileData.cFileName);
				tagFileInfo* pTemptParentInfo = m_pFilesInfo->InsertFolderInfo(pParenttagFileInfo,
					str,0,0,0,1,1);

				SearchChildFolder(strTempt,pTemptParentInfo);
			}
			else
			{
				string strTemptFileName(FindFileData.cFileName);
				strTemptFileName.erase( 0,strTemptFileName.find_last_of(".")+1 );		

				//工程信息不装入
				if(strTemptFileName=="pak"  || strTemptFileName == "cfg" || strTemptFileName == "vs")
					continue;

				//完成一个文件的搜索加入
				string str(FindFileData.cFileName);
				// Thumbs.db 文件不加入
				if(str == "Thumbs.db")
					continue;

				SearchOneFile(pParenttagFileInfo,strTempt,str,FindFileData.nFileSizeLow);				
			}
		}
	}
	while ( FindNextFile( hFind, &FindFileData ) && hFind != INVALID_HANDLE_VALUE );

	FindClose ( hFind );
}

//搜索子文件夹
void CClientResource::ReSearchChildFolder(CHAR* strFolder,
								tagFileInfo* pParenttagFileInfo,bool bWithChild)
{
	if(pParenttagFileInfo==NULL)	return;

	WIN32_FIND_DATA	  FindFileData;
	HANDLE            hFind;

	// 寻找第一个文件
	hFind = FindFirstFile ( "*.*", &FindFileData );

	// 搜索所有文件及子目录
	do
	{
		if ( hFind != INVALID_HANDLE_VALUE )
		{
			// 如果是当前目录或父目录，跳过
			if ( ! ( strcmp( FindFileData.cFileName, "." ) ) ||
				 ! ( strcmp( FindFileData.cFileName, ".." ) ) )
			{
				continue;
			}

			CHAR strTempt[MAX_PATH];
			strcpy_s( strTempt, MAX_PATH, strFolder );
			strcat_s(strTempt, MAX_PATH, "\\");
			strcat_s(strTempt, MAX_PATH, FindFileData.cFileName );

			// 如果是一个目录，递归调用继续搜索子目录
			if ( SetCurrentDirectory(strTempt) )
			{
				string strFileName1(FindFileData.cFileName);
				tagFileInfo* pTemptParentInfo =
					m_pFilesInfo->FindChildFileInfoByText(pParenttagFileInfo,strFileName1);
				
				if(pTemptParentInfo==NULL)
				{
					ulong dwPackage = GetFirstEnableBit(pParenttagFileInfo->dwPackageType);
					ulong dwCompress= GetFirstEnableBit(pParenttagFileInfo->dwCompressType);
					//ulong dwPackage = 1;
					//ulong dwCompress= 1;
					pTemptParentInfo =
						m_pFilesInfo->InsertFolderInfo(pParenttagFileInfo,strFileName1,0,0,0,
														dwPackage,dwCompress);
				}

				if(bWithChild)
					ReSearchChildFolder(strTempt,pTemptParentInfo);
			}
			else
			{
				string strTemptFileName(FindFileData.cFileName);
				strTemptFileName.erase( 0,strTemptFileName.find_last_of(".")+1 );		

				//工程信息不装入
				if(strTemptFileName=="pak"  || strTemptFileName == "cfg" || strTemptFileName == "vs")
					continue;

				//完成一个文件的搜索加入
				string str(FindFileData.cFileName);
				// Thumbs.db 文件不加入
				if(str == "Thumbs.db")
					continue;
				ReSearchOneFile(pParenttagFileInfo,strTempt,str,FindFileData.nFileSizeLow);				
			}
		}
	}
	while ( FindNextFile( hFind, &FindFileData ) && hFind != INVALID_HANDLE_VALUE );

	FindClose ( hFind );
}


//装载资源线程
unsigned __stdcall CClientResource::LoadClientResourceProc(void* pArguments)
{
	CClientResource* pClientResource = (CClientResource*)pArguments;
	if(pClientResource==NULL)
	{
		_endthreadex( 0 );
		return 0;
	}

	////装载资源开始
	pClientResource->OnLoadResourceStart();
	pClientResource->SetAlwaysCheck(false);
	
	string strFolder = pClientResource->GetCurFolder();
	::SetCurrentDirectory( strFolder.c_str() );

	pClientResource->SearchChildFolder( (CHAR*)strFolder.c_str(),
										pClientResource->GetFilesInfo()->GetRootFile() );

	ulong dwPackage=0,dwCompress=0;
	pClientResource->GetFilesInfo()->UpdateFolderInfo(pClientResource->GetFilesInfo()->GetRootFile(),
		dwPackage,dwCompress);	

	//保存信息
	pClientResource->Save();
	//装载资源结束
	pClientResource->OnLoadResourceEnd();

	_endthreadex( 0 );
	return 0;
}

unsigned __stdcall CClientResource::ReLoadClientResoureceProc(void* pArguments)
{
	tagReLoadReSource* pReLoad = (tagReLoadReSource*)pArguments;
	if(pReLoad==NULL)
	{
		_endthreadex( 0 );
		return 0;
	}

	//装载资源开始
	pReLoad->pClientResour->OnReLoadResourceStart();

	pReLoad->pClientResour->SetAlwaysCheck(false);

	string strTempt=pReLoad->pClientResour->GetCurFolder()+pReLoad->strFolderName;

	tagFileInfo* pParentFileInfo = NULL;
		
	if(SetCurrentDirectory( strTempt.c_str() ))
	{
		pParentFileInfo =
			pReLoad->pClientResour->GetFilesInfo()->GetFileInfoByText(pReLoad->strFolderName);

		pReLoad->pClientResour->ReSearchChildFolder((CHAR*)strTempt.c_str(),pParentFileInfo,
								pReLoad->bWithChild);
	}
	else
	{
		WIN32_FIND_DATA	  FindFileData;
		HANDLE            hFind;

		// 寻找一个文件
		hFind = FindFirstFile ( strTempt.c_str(), &FindFileData );
		if(hFind != INVALID_HANDLE_VALUE)
		{
			string strPath(pReLoad->strFolderName);
			int nPos = (int)strPath.find_last_of("\\");

			strPath.erase(nPos,strPath.length()-nPos);

			string strFileName(pReLoad->strFolderName);
			strFileName.erase(0,nPos+1);
			pParentFileInfo = pReLoad->pClientResour->GetFilesInfo()->GetFileInfoByText(strPath);

			pReLoad->pClientResour->ReSearchOneFile(pParentFileInfo,(CHAR*)strTempt.c_str(),strFileName,
				FindFileData.nFileSizeLow);

			FindClose(hFind);
		}
	}

	ulong dwPackage=0,dwCompress=0;
	CFilesInfo* pFilesInfo = pReLoad->pClientResour->GetFilesInfo();

	//删除磁盘上没有的文件或文件夹
	CFilesInfo* pTemptFilesInfo = new CFilesInfo;
	pTemptFilesInfo->Clear();
	pFilesInfo->CopyData(pTemptFilesInfo);
	tagFileInfo* pTemptTagFileInfo = pTemptFilesInfo->GetFileInfoByText(pReLoad->strFolderName);
	pReLoad->pClientResour->ClearEmptyFilesInfo(pTemptTagFileInfo,pTemptFilesInfo);

	pFilesInfo->UpdateFolderInfo(pFilesInfo->GetRootFile(),dwPackage,dwCompress);	
	pFilesInfo->OnUpdate(pFilesInfo->GetRootFile());

	pReLoad->pClientResour->Save();
	//装载资源结束
	pReLoad->pClientResour->OnReLoadResourceEnd();

	delete pReLoad;
	delete pTemptFilesInfo;

	_endthreadex( 0 );
	return 0;
}
unsigned __stdcall CClientResource::MakePakcageProc(void* pArguments)
{
	tagMakePackPro* pMakePackPro = (tagMakePackPro*)pArguments;

	if(pMakePackPro->pClientResour==NULL || pMakePackPro->pOriginResour==NULL)
	{
		delete pMakePackPro;
		_endthreadex( 0 );
		return 0;
	}

	//装载资源开始
	pMakePackPro->pOriginResour->OnMakePackageStart();

	CFilesInfo* pOriFilesInfo = pMakePackPro->pOriginResour->GetFilesInfo();
	CFilesInfo* pCliFilesInfo = pMakePackPro->pClientResour->GetFilesInfo();

	CFilesInfo* pTemptFilesInfo = new CFilesInfo;
	pOriFilesInfo->GetDifferenceByPackage(*pCliFilesInfo,*pTemptFilesInfo);

	//没有变化
	if(pTemptFilesInfo->GetRootFile()->ChildFilesInfo.size() <= 0)
	{
		pTemptFilesInfo->Clear();
		//查看是否有多余的文件
		pCliFilesInfo->GetDifferenceByPackage(*pOriFilesInfo,*pTemptFilesInfo);
		//没有变化
		if(pTemptFilesInfo->GetRootFile()->ChildFilesInfo.size() > 0)
		{
			pMakePackPro->pClientResour->DelFileFolder(pTemptFilesInfo->GetRootFile(),pTemptFilesInfo);
		}

		//保存信息
		pCliFilesInfo->SetVersion(pOriFilesInfo->GetVersion());
		string strTempt = pMakePackPro->pClientResour->GetCurFolder()+"\\"+
			pMakePackPro->pClientResour->GetFilesInfoName();
		pMakePackPro->pClientResour->GetFilesInfo()->Save(strTempt);		

		delete pMakePackPro;
		delete pTemptFilesInfo;
		_endthreadex( 0 );
		return 0;
	}

	/*CPackage *pPackage = pMakePackPro->pClientResour->GetPackage();
	if( pPackage==NULL )
	{
		delete pMakePackPro;
		delete pTemptFilesInfo;
		_endthreadex( 0 );
		return 0;
	}*/

	pMakePackPro->pOriginResour->MakePackage(pTemptFilesInfo->GetRootFile(),pTemptFilesInfo,
		pMakePackPro->pClientResour);

	//删除多余文件和目录
	pTemptFilesInfo->Clear();
	pCliFilesInfo->GetDifferenceByPackage(*pOriFilesInfo,*pTemptFilesInfo);
	//没有变化
	if(pTemptFilesInfo->GetRootFile()->ChildFilesInfo.size() > 0)
	{
		pMakePackPro->pClientResour->DelFileFolder(pTemptFilesInfo->GetRootFile(),pTemptFilesInfo);
	}

	string strTempt = pMakePackPro->pClientResour->GetCurFolder()+"\\"+
		pMakePackPro->pClientResour->GetFilesInfoName();

	pMakePackPro->pClientResour->PackagesWriteIndexEx();

	pMakePackPro->pClientResour->GetFilesInfo()->Save(strTempt);

	pMakePackPro->pOriginResour->OnMakePackageEnd();
	delete pMakePackPro;
	delete pTemptFilesInfo;
	_endthreadex( 0 );
	return 0;
}
unsigned __stdcall CClientResource::ReMakePakcageProc(void* pArguments)
{
	CClientResource* pClientResour = (CClientResource*)pArguments;
	if(pClientResour==NULL)
	{
		_endthreadex( 0 );
		return 0;
	}

	//从装载资源开始
	pClientResour->OnReMakePackageStart();

	string strNewPackageName = "New"+pClientResour->GetPackageName();

	//CPackage* pNewPackage =new CPackage(pClientResour,pClientResour->GetCurFolder());
	//pNewPackage->Create(strNewPackageName.c_str());

	//寻找包文件
	//pClientResour->ReMakePackage(pClientResour->GetFilesInfo()->GetRootFile(),pNewPackage);

	//pNewPackage->WriteIndexEx();

	//CPackage* pPackage = pClientResour->GetPackage();
	//delete pPackage;
	//pClientResour->Setpackage(pNewPackage);

	string strOldFileName = string(pClientResour->GetCurFolder())+"\\" + strNewPackageName;
	string strNewFileName = string(pClientResour->GetCurFolder())+"\\"+pClientResour->GetPackageName();

	if(::DeleteFile(strNewFileName.c_str())==false)
	{
		//AfxMessageBox("操作出错！");
	}

	pClientResour->PackagesCloseHandle();

	::MoveFile(strOldFileName.c_str(),strNewFileName.c_str());

	pClientResour->OpenPackages(false);
	
	//重装资源结束
	pClientResour->OnReMakePackageEnd();

	_endthreadex( 0 );
	return 0;
}

//搜索子文件
void	CClientResource::SearchOneFile(tagFileInfo* pParenttagInfo,
									   CHAR* strPath, string& strFileName,ulong dwSize)
{
	////文件中不能有空格
	//long lPos = strFileName.find_first_of(" ");
	//if(  lPos >= 0 )
	//{
	//	//AfxMessageBox(strFileName.c_str());
	//	return;
	//}

	if(pParenttagInfo==NULL)	return;
	//是否能找到该文件
	ulong dwCrc32;
	CCrc32Static::FileCrc32Filemap(strPath,dwCrc32);
	m_pFilesInfo->InsertFileInfo(pParenttagInfo,strFileName,dwCrc32,dwSize,dwSize,dwSize,1,1);
}

//搜索子文件
void	CClientResource::ReSearchOneFile(tagFileInfo* pParenttagInfo,
										 CHAR* strPath, string& strFileName,ulong dwSize)
{
	//文件中不能有空格
	//long lPos = strFileName.find_first_of(" ");
	//if( lPos >= 0 )
	//{
	//	//AfxMessageBox(strFileName.c_str());
	//	return;
	//}

	if(pParenttagInfo==NULL)	return;

	ulong dwCrc32;
	CCrc32Static::FileCrc32Filemap(strPath,dwCrc32);

	ulong dwPackage = 1;
	ulong dwCompress = 1;

	tagFileInfo* pTemptParentInfo = m_pFilesInfo->FindChildFileInfoByText(pParenttagInfo,strFileName);

	if(pTemptParentInfo)
	{
		dwPackage = pTemptParentInfo->dwPackageType;
		dwCompress= pTemptParentInfo->dwCompressType;
		m_pFilesInfo->UpdateFileInfo(pTemptParentInfo,dwCrc32,dwSize,
									dwSize,dwSize,false,dwPackage,dwCompress);
	}
	else
	{
		dwPackage = GetFirstEnableBit(pParenttagInfo->dwPackageType);
		dwCompress= GetFirstEnableBit(pParenttagInfo->dwCompressType);
		m_pFilesInfo->InsertFileInfo(pParenttagInfo,strFileName,dwCrc32,dwSize,dwSize,dwSize,
			dwPackage,dwCompress);
	}
}

//添加一个文件夹
void	CClientResource::AddFolder(string& strFolder,ulong dwPackage,ulong dwCompress)
{
	if( dwPackage&0x1 )
	{
		string strDirectory = m_strCurFolder+strFolder;
		::CreateDirectory(strDirectory.c_str(),NULL);
	}
	m_pFilesInfo->UpdateFileInfo(strFolder,0,0,0,0,true,dwPackage,dwCompress);
}

void	CClientResource::AddFile(string& strSouFolder,string& strFileName,
								 int nSize,ulong dwCrc32,ulong dwPackage,ulong dwCompress)
{
	tagFileInfo* ptagFileInfo = m_pFilesInfo->GetFileInfoByText(strFileName);
	if(ptagFileInfo)
	{
		OnDelFileFolder(strFileName,strFileName);
	}

	string strSouFileName = strSouFolder+strFileName;
	if( !ENABLEBIT(dwPackage,NOPACK) )
	{
		FILE* pf= fopen(strSouFileName.c_str(),"rb");
		if(pf)
		{
			CPackage *pPackage = GetPackage(dwPackage);
			if(pPackage)
			{
				ulong dwOrginSize = 0;
				ulong dwSize = pPackage->AddFile(pf,strFileName.c_str(),dwCrc32,
													dwCompress, dwOrginSize);
				m_pFilesInfo->UpdateFileInfo(strFileName,dwCrc32,dwSize,
												dwOrginSize,dwSize,false,dwPackage,dwCompress);
			}
			fclose(pf);
		}
	}
	else
	{ 
		string strNewTemptFile = m_strCurFolder+strFileName;
		::CopyFile(strSouFileName.c_str(),strNewTemptFile.c_str(),false);

		m_pFilesInfo->UpdateFileInfo(strFileName,dwCrc32,nSize,nSize,nSize,false,1,1);

	}
}

void	CClientResource::CreateDirectory(tagFileInfo* pTagFileInfo,CFilesInfo* pFilesInfo)
{
	if(pTagFileInfo==NULL || pFilesInfo==NULL)	return;

	if(pTagFileInfo != pFilesInfo->GetRootFile() && pTagFileInfo->bFolder==true &&
		pFilesInfo->ChildHaveUnPackFile(pTagFileInfo))
	{
		string strDirectory = m_strCurFolder+pFilesInfo->GetFileTextWithParent(pTagFileInfo);
		::CreateDirectory(strDirectory.c_str(),NULL);
	}

	if(pFilesInfo->HaveChild(pTagFileInfo))
	{
		for(itFileInfo it = pTagFileInfo->ChildFilesInfo.begin();
			it != pTagFileInfo->ChildFilesInfo.end();it++)
		{
			CreateDirectory((*it).second,pFilesInfo);
		}
	}
}
void	CClientResource::DeleteDirectory(tagFileInfo* pTagFileInfo,CFilesInfo* pFilesInfo)
{
	if(pTagFileInfo==NULL || pFilesInfo==NULL)	return;

	if(pFilesInfo->HaveChild(pTagFileInfo))
	{
		for(itFileInfo it = pTagFileInfo->ChildFilesInfo.begin();
			it != pTagFileInfo->ChildFilesInfo.end();it++)
		{
			DeleteDirectory((*it).second,pFilesInfo);
		}
	}

	if(pTagFileInfo != pFilesInfo->GetRootFile() && pTagFileInfo->bFolder==true  &&
		pFilesInfo->ChildHaveUnPackFile(pTagFileInfo) == false)
	{
		string strDirectory = m_strCurFolder+pFilesInfo->GetFileTextWithParent(pTagFileInfo);
		::RemoveDirectory(strDirectory.c_str());
	}
}

//清除空的文件信息
bool	CClientResource::ClearEmptyFilesInfo(tagFileInfo* pTagFileInfo,CFilesInfo* pFilesInfo)
{
	if(pTagFileInfo==NULL || pFilesInfo==NULL)
		return false;

	if(pFilesInfo->HaveChild(pTagFileInfo))
	{
		for(itFileInfo it = pTagFileInfo->ChildFilesInfo.begin();
			it != pTagFileInfo->ChildFilesInfo.end();it++)
		{
			ClearEmptyFilesInfo((*it).second,pFilesInfo);
		}

		string strTemptFile = pFilesInfo->GetFileTextWithParent(pTagFileInfo);
		tagFileInfo* pFolderInfo = m_pFilesInfo->GetFileInfoByText(strTemptFile);
		if(pFolderInfo && pFolderInfo->ChildFilesInfo.size()<=0)
			m_pFilesInfo->DelFileInfo(strTemptFile);
	}
	else
	{
		string strTemptFile = pFilesInfo->GetFileTextWithParent(pTagFileInfo);
		string strFilePath = m_strCurFolder+strTemptFile;

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = ::FindFirstFile(strFilePath.c_str(), &FindFileData);
		if(hFind == INVALID_HANDLE_VALUE)
		{
			m_pFilesInfo->DelFileInfo(strTemptFile);
		}
		else
		{
			FindClose(hFind);
		}
	}
	return true;
}

void CClientResource::FindFileList(string& strFile, string& strExt, list<string>& listFileName)
{
	listFileName.clear();
	if(m_pFilesInfo)
		m_pFilesInfo->FindFileList(strFile,strExt,listFileName);
}
//装载资源开始
void CClientResource::OnLoadResourceStart()
{
	if(m_hWnd)
	{
		::SendMessage(m_hWnd,LOADRESOURCE_START,(WPARAM)this,0);
    }
}
//装载资源结束
void CClientResource::OnLoadResourceEnd()
{
	if(m_hWnd)
	{
		::SendMessage(m_hWnd,LOADRESOURCE_END,(WPARAM)this,0);
	}
}

//从新装载资源开始
void CClientResource::OnReLoadResourceStart()
{
	if(m_hWnd)
	{
		::SendMessage(m_hWnd,RELOADRESOURCE_START,(WPARAM)this,0);
	}
}

//从新装载资源结束
void CClientResource::OnReLoadResourceEnd()
{
	if(m_hWnd)
	{
		::SendMessage(m_hWnd,RELOADRESOURCE_END,(WPARAM)this,0);
	}
}
//打包开始
void CClientResource::OnMakePackageStart()
{
	if(m_hWnd)
	{
		::SendMessage(m_hWnd,MAKEPACKAGE_START,(WPARAM)this,0);
	}
}

//打包中
void CClientResource::OnMakePackageing(tagFileInfo* pTagFileInfo,CFilesInfo* pFilesInfo)
{
	if(m_hWnd && pTagFileInfo && pFilesInfo)
	{
		::SendMessage(m_hWnd,MAKEPACKAGE_ING,(WPARAM)pTagFileInfo,(LPARAM)pFilesInfo);
	}
}

//打包结束
void CClientResource::OnMakePackageEnd()
{
	if(m_hWnd)
	{
		::SendMessage(m_hWnd,MAKEPACKAGE_END,(WPARAM)this,0);
	}
}

//整理包开始
void CClientResource::OnReMakePackageStart()
{
	if(m_hWnd)
	{
		::SendMessage(m_hWnd,REMAKEPACKAGE_START,(WPARAM)this,0);
	}
}
//整理包结束
void CClientResource::OnReMakePackageEnd()
{
	if(m_hWnd)
	{
		::SendMessage(m_hWnd,REMAKEPACKAGE_END,(WPARAM)this,0);
	} 
}

FILE*	CClientResource::OpenFile(const char* strFileName)
{
	string strName(strFileName);
	tagFileInfo* ptagFileInfo = m_pFilesInfo->GetFileInfoByText(strName);
	if(ptagFileInfo && ptagFileInfo->dwPackageType==0)
	{
		strName = m_strCurFolder+strName;
		return fopen(strName.c_str(),"rb");
	}
	return NULL;
}

bool	CClientResource::CloseFile(FILE* pf)
{
	if(pf)
		fclose(pf);
	return true;
}


//创建读取线程
void CClientResource::StartAsynReadFileThread()
{
	//创建同步读事件
	m_hSyncRead = CreateEvent(NULL,0,0,0);
	//创建条件变量
	m_hRequestReadFile =  CreateEvent(NULL,0,0,0);
	InitializeCriticalSection(&m_CSRequestReadFile);

	InitializeCriticalSection(&m_CSCompletedReadFile);

	m_pSyncReadFile = new tagFileAsynOper;

	unsigned threadID;
	m_hAsynReadThread = (HANDLE)_beginthreadex(NULL,0,AsynReadFileProc,this,0,&threadID);
}
//退出读取线程
void CClientResource::ExitAsynReadFileThread()
{
	if(!m_hAsynReadThread)	return;

	PushRequestReadFile(NULL);
	WaitForSingleObject(m_hAsynReadThread,INFINITE);
	CloseHandle(m_hAsynReadThread);

	CloseHandle(m_hSyncRead);
	DeleteCriticalSection(&m_CSRequestReadFile);
	CloseHandle(m_hRequestReadFile);
	DeleteCriticalSection(&m_CSCompletedReadFile);

    Log4c::Trace(ROOT_MODULE, "[PACKAGE] Release CSCompletedReadFile OK!");
	//释放已经读取的资源列表
	itAsynOpFile it = m_CompletedReadFiles.begin();
	for(;it != m_CompletedReadFiles.end();it++)
	{
		tagFileAsynOper* pCompletedFile = (*it);
		if(pCompletedFile)
		{
			if(pCompletedFile->prFile && pCompletedFile->prFile->GetFileName())
                Log4c::Trace(ROOT_MODULE, "[PACKAGE] Release %s Start", pCompletedFile->prFile->GetFileName());
			if(pCompletedFile->pCallBack)
			{
				rfClose(pCompletedFile->prFile);
				pCompletedFile->prFile = NULL;
				(*pCompletedFile->pCallBack)(NULL,pCompletedFile->pData);
			}
            if(pCompletedFile->prFile)
			    Log4c::Trace(ROOT_MODULE, "[PACKAGE] Release %s End", pCompletedFile->prFile->GetFileName());
			delete pCompletedFile;
		}
	}
	delete m_pSyncReadFile;
}

void CClientResource::PushRequestReadFile(CClientResource::tagFileAsynOper* pAsynReadFile)
{
	EnterCriticalSection(&m_CSRequestReadFile);
	if(m_RequestedReadFiles.size() == 0)
		SetEvent(m_hRequestReadFile);
	m_RequestedReadFiles.push_back(pAsynReadFile);
	LeaveCriticalSection(&m_CSRequestReadFile);
}

CClientResource::tagFileAsynOper* CClientResource::PopRequestReadFile()
{
	tagFileAsynOper* pFileAsynOper = NULL;
	EnterCriticalSection(&m_CSRequestReadFile);
	while(m_RequestedReadFiles.size() == 0)
	{
		LeaveCriticalSection(&m_CSRequestReadFile);
		WaitForSingleObject(m_hRequestReadFile,INFINITE);
		EnterCriticalSection(&m_CSRequestReadFile);
	}
	pFileAsynOper = m_RequestedReadFiles.front();
	m_RequestedReadFiles.pop_front();
	LeaveCriticalSection(&m_CSRequestReadFile);
	return pFileAsynOper;
}

void CClientResource::PushCompletedReadFile(CClientResource::tagFileAsynOper* pAsynReadFile)
{
	EnterCriticalSection(&m_CSCompletedReadFile);
	m_CompletedReadFiles.push_back(pAsynReadFile);
	LeaveCriticalSection(&m_CSCompletedReadFile);
}

CClientResource::tagFileAsynOper* CClientResource::PopCompletedReadFile()
{
	tagFileAsynOper* pFileAsynOper = NULL;
	EnterCriticalSection(&m_CSCompletedReadFile);
	if(m_CompletedReadFiles.size() > 0)
	{
		pFileAsynOper = m_CompletedReadFiles.front();
		m_CompletedReadFiles.pop_front();
	}
	LeaveCriticalSection(&m_CSCompletedReadFile);
	return pFileAsynOper;
}

CRFile* CClientResource::OpenRFile(const char* strFileName)
{
	CRFile* rfile=NULL;
	ulong dwSize = 0;
	string strName(strFileName);
	CheckRFileStr(strName);
	//资源信息列表中有该文件
	tagFileInfo* ptagFileInfo =
		GetFilesInfo()->GetFileInfoByText(strName);
	if(ptagFileInfo)
	{		
		//在包中
		if( !ENABLEBIT( ptagFileInfo->dwPackageType,NOPACK) )
		{
			CPackage *pPackage = GetPackage(ptagFileInfo->dwPackageType);
			if(pPackage)
			{
				dwSize = pPackage->GetFileSize(strName.c_str());
				if(dwSize>0)
				{
					uchar* pBuf = new uchar[dwSize+1];
					ulong dwCrc32,dwCompress,dwOrginSize, dwValidSize;
					pPackage->ExtractToBuf(strName.c_str(),
						pBuf,dwSize,dwCrc32,dwCompress, dwOrginSize, dwValidSize);

					//解压
					if( !ENABLEBIT( dwCompress,NOCOMPRESS) )
					{
						ulong dwOutSize = ptagFileInfo->dwOrginSize+1;
						uchar* pOutBuf = new uchar[dwOutSize];
						if( ENABLEBIT(dwCompress,0x4) )
							DeCompress(pBuf, dwSize, pOutBuf, &dwOutSize);
						else
							DeCompressData(pBuf, dwSize, pOutBuf, &dwOutSize);

						pOutBuf[dwOutSize]='\0';
						rfile = new CRFile(strFileName,pOutBuf,dwOutSize);
						delete[] pBuf;
					}
					else
					{
						pBuf[dwSize]='\0';
						rfile = new CRFile(strFileName,pBuf,dwSize);
					}
				}
			}
		}
		//没有在包中的数据
		else
		{
			string strTempt(GetCurFolder());
			strTempt +=strName;

			FILE* pf = fopen(strTempt.c_str(),"rb");
			if(pf)
			{
				rfile = new CRFile(strFileName,pf);
				fclose(pf);
			}
		}
	}
	//从外部读文件
	else
	{
		FILE* pf = fopen(strFileName,"rb");
		if(pf)
		{
			rfile = new CRFile(strFileName,pf);
			fclose(pf);
		}
	}
	return rfile;
}



/*
 * 功能: 同步打开文件
 * 摘要: -
 * 参数: strFileName - 文件名
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2009.04.11 - lpf
 *		增加了限定_CLENTTOTEST_版本,未找到文件才输出日志
 */
CRFile* CClientResource::SyncReadFile(const char* strFileName)
{
	if(strFileName)
		strncpy(m_pSyncReadFile->strFileName,strFileName,MAX_PATH);
	else 
		strncpy(m_pSyncReadFile->strFileName,"",MAX_PATH);
	m_pSyncReadFile->prFile = NULL;
	PushRequestReadFile(m_pSyncReadFile);
	WaitForSingleObject(m_hSyncRead,INFINITE);
	if (!m_pSyncReadFile->prFile)
	{
		//Log4c::Error(m_pSyncReadFile->strFileName);

#ifdef _CLENTTOTEST_
		//string sFileName = strFileName;
		//if (s_mapNotFoundFiles.find(sFileName) == s_mapNotFoundFiles.end())
		//{
		//	s_mapNotFoundFiles[sFileName] = (ulong)(s_mapNotFoundFiles.size() + 1);

            Log4c::Warn(ROOT_MODULE,"同步载入文件:%s没找到", m_pSyncReadFile->strFileName);
		//}
#endif
	}
	return m_pSyncReadFile->prFile;
}



//异步打开文件
bool	CClientResource::AsynReadFile(const char* strFileName,AsynReadCallBack pCallBack,void* pData)
{
	tagFileAsynOper* pFileAsynOper  = new tagFileAsynOper;
	strncpy(pFileAsynOper->strFileName,strFileName,MAX_PATH);
	pFileAsynOper->pCallBack = pCallBack;
	pFileAsynOper->pData = pData;
	pFileAsynOper->prFile = NULL;
	PushRequestReadFile(pFileAsynOper);
	return true;
}



/*
 * 功能: 异步读取文件的线程函数
 * 摘要: -
 * 参数: pArguments - 资源参数
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2009.04.11 - lpf
 *		增加了限定_CLENTTOTEST_版本,未找到文件才输出日志
 */
unsigned __stdcall CClientResource::AsynReadFileProc(void * pArguments)
{
	CClientResource* pClientResource = (CClientResource*)pArguments;
	if(pClientResource)
	{
		while(true)
		{
			tagFileAsynOper* pAsynReadFile = pClientResource->PopRequestReadFile();
			//退出
			if(NULL == pAsynReadFile)
				break;

			//读操作
			pAsynReadFile->prFile = pClientResource->OpenRFile(pAsynReadFile->strFileName);
			if (!pAsynReadFile->prFile)
			{
				//Log4c::Error(pAsynReadFile->strFileName);

#ifdef _CLENTTOTEST_
				//string sFileName = pAsynReadFile->strFileName;
				//if (s_mapNotFoundFiles.find(sFileName) == s_mapNotFoundFiles.end())
				//{
				//	s_mapNotFoundFiles[sFileName] = (ulong)(s_mapNotFoundFiles.size() + 1);

                    Log4c::Warn(ROOT_MODULE, "异步载入文件:%s没找到", pAsynReadFile->strFileName);
				//}
#endif
			}

			//完成操作
			if(pAsynReadFile == pClientResource->m_pSyncReadFile)
				SetEvent(pClientResource->m_hSyncRead);
			else
				pClientResource->PushCompletedReadFile(pAsynReadFile);
		}
	}
	//退出
	_endthreadex( 0 );
	return 0;
}



//完成读取数据的驱动函数
void CClientResource::Run()
{
	tagFileAsynOper* pCompletedFile = PopCompletedReadFile();
	if(pCompletedFile)
	{
		if(pCompletedFile->pCallBack)
			(*pCompletedFile->pCallBack)(pCompletedFile->prFile,pCompletedFile->pData);
		delete pCompletedFile;
	}
}


//打开资源文件的全局函数

//同步打开文件函数,调用rfClose关闭
CRFile* rfOpen(const char* strFileName)
{
	return CClientResource::GetInstance()->SyncReadFile(strFileName);
}

//异步打开一个文件
bool rfAsynOpen(const char* strFileName,AsynReadCallBack pCallBack,void* pData)
{
	return CClientResource::GetInstance()->AsynReadFile(strFileName,pCallBack,pData);
}

//关闭CRFile文件句柄
void rfClose(CRFile* rfile)
{
	if(rfile)
		delete rfile;
}