/////////////////////////////////////////////////////////////////////////////////
//FilesInfo.h
//Fun:用来统计客户端资源信息
//Create Time:2004.10.29
//Author:Joe
/////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include ".\filesinfo.h"
#include "ClientResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void PutDebugString(const char *msg);

CFilesInfo::CFilesInfo(HWND pWnd)
:m_pDisplayWnd(pWnd)
{
	m_FileName ="";
	m_PackageInfos.clear();
	m_pRootFile=new tagFileInfo;
	m_strVersion = "00.00.0000";

	m_dwFileNum = 0;

	//初始化
	OnIntial();
}

CFilesInfo::~CFilesInfo(void)
{
	//删除所有节点
	DelFileInfo(m_pRootFile);

	//删除
	OnDeleteAll();
}

//从文件中装载资源信息列表
BOOL	CFilesInfo::Load(string& strName)
{
	if(strName.compare("") ==0)	return false;
	m_FileName = strName;

	FILE *pf = fopen(m_FileName.c_str(),"rb");
	if(pf == NULL)
	{
		char strText[MAX_PATH]="";
		_snprintf(strText, MAX_PATH, "装载文件 %s 出错!",m_FileName.c_str());
		PutDebugString(strText);
		return false;
	}

	char strPath[MAX_PATH]="";
	DWORD bFolder;
	try
	{
		//读根节点
		if(!feof(pf))
		{
			fscanf(pf,"%s\n",m_strVersion.c_str());

			//装载包信息
			LoadPackInfo(pf);

			fscanf(pf,"%u ",&m_pRootFile->dwSize);
			fscanf(pf,"%u ",&m_pRootFile->dwOrginSize);
			fscanf(pf,"%u ",&m_pRootFile->dwValidSize);
			fscanf(pf,"%u ",&m_pRootFile->dwCrc32);
			fscanf(pf,"%d ",&m_pRootFile->dwPackageType);
			fscanf(pf,"%d ",&m_pRootFile->dwCompressType);	
			fscanf(pf,"%d\n",&bFolder);
			m_pRootFile->bFolder=true;
			OnUpdate(m_pRootFile);
			//装载资源
			LoadFolderInfo(pf,m_pRootFile);
		}
		fclose(pf);
	}
	catch (...)
	{
		Clear();
		PutDebugString("读取文件信息出错.");
		return false;
	}
	PutDebugString("CFilesInfo::Load() end!");
	return true;
}

BOOL	CFilesInfo::LoadPackInfo(FILE* pf)
{
	if(pf==NULL)	return FALSE;
	//读取个数
	char strName[100]="";
	DWORD dwSize = 0;
	fscanf(pf,"%u\n",&dwSize);
	for(DWORD i = 0;i<dwSize;i++)
	{
		tagPackFileInfo PackInfo;
		fscanf(pf,"%u ",&PackInfo.dwID);
		fscanf(pf,"%s ",strName);
		PackInfo.strFileName = strName;
		fscanf(pf,"%u ",&PackInfo.dwIndexNum);
		fscanf(pf,"%u\n",&PackInfo.dwEmptyIndexNum);
		m_PackageInfos.push_back(PackInfo);
	}

	return TRUE;
}

BOOL	CFilesInfo::LoadFolderInfo(FILE* pf,tagFileInfo* pParentagFileInfo)
{
	if(pf==NULL	|| pParentagFileInfo==NULL)	return false;

	char strName[MAX_PATH];
	memset(strName, 0, sizeof(strName));
	DWORD dwSize,dwOrginSize,dwValidSize,dwCrc32,dwPackageType,dwCompressType;
	DWORD bFolder;
	while(!feof(pf))
	{
		//fscanf(pf,"%s ",strName);
		long szPos = 0;
		char szFlag = 0;
		while(szFlag != ' ' && szFlag !='<')
			fread(&szFlag, 1, 1, pf); // 先循环到名字开始处'['
		while (szFlag != '>') // 记录文件名字
		{
			fread(&szFlag, 1, 1, pf); // 跳过空格' '
			if(szFlag != '>')
			{
				strName[szPos] = szFlag;
				szPos++;
			}
		}
		fread(&szFlag, 1, 1, pf); // 跳过空格' '
		strName[szPos] = '\0';

		fscanf(pf,"%u ",&dwSize);
		fscanf(pf,"%u ",&dwOrginSize);
		fscanf(pf,"%u ",&dwValidSize);
		fscanf(pf,"%u ",&dwCrc32);
		fscanf(pf,"%d ",&dwPackageType);
		fscanf(pf,"%d ",&dwCompressType);	
		fscanf(pf,"%d\n",&bFolder);
		string strFileName(strName);

		//返回
		if(bFolder==20)
			return true;

		//是文件夹
		if(bFolder != false)
		{
			if(strFileName == "*") // 是根目录,继续读
			{
				LoadFolderInfo(pf,pParentagFileInfo);
			}
			else
			{
				tagFileInfo* pTemptParentagInfo = AddFolderInfo(pParentagFileInfo,strFileName,
				dwSize,dwOrginSize,dwValidSize,dwPackageType,dwCompressType);
				LoadFolderInfo(pf,pTemptParentagInfo);
			}
		}
		else
		{
			AddFileInfo(pParentagFileInfo,strFileName,dwCrc32,dwSize,dwOrginSize,dwValidSize,
				dwPackageType,dwCompressType);
		}
	}
	return true;
}
//保存资源信息列表
BOOL	CFilesInfo::Save(string& strName)
{
	string strTemptName = strName;
	if(strTemptName.compare("")==0)
		strTemptName=m_FileName;
	if(strTemptName.compare("")==0)
		return false;

	FILE *pf = fopen(strTemptName.c_str(),"wb");
	if(pf)
	{
		//存版本号
		fprintf(pf,"%s\n",m_strVersion.c_str());

		//保存包信息
		SavePackInfo(pf);

		Save(pf,GetRootFile());
		fclose(pf);
	}
	else
	{
		char strText[MAX_PATH]="";
		_snprintf(strText, MAX_PATH, "保存文件 %s 出错!",strName.c_str());
		PutDebugString(strText);
		return false;
	}
	return true;
}

//保存包信息
BOOL	CFilesInfo::SavePackInfo(FILE* pf)
{
	if(pf==NULL)	return FALSE;
	//保存包的个数
	fprintf(pf,"%d\n",m_PackageInfos.size());
	itTypeInfo it = m_PackageInfos.begin();
	for(;it != m_PackageInfos.end();it++)
	{
		fprintf(pf,"%d ",(*it).dwID);
		fprintf(pf,"%s ",(*it).strFileName.c_str());
		fprintf(pf,"%d ",(*it).dwIndexNum);
		fprintf(pf,"%d\n",(*it).dwEmptyIndexNum);
	}
	return TRUE;
}

//保存资源信息列表
bool	CFilesInfo::Save(FILE* pf,tagFileInfo* pFileInfo)
{
	if(pFileInfo==NULL || pf == NULL)	return false;

	//存版本号
	if(pFileInfo->ParentFileInfo == NULL) // 根目录
		fprintf(pf,"<*> ");
	else
		fprintf(pf,"<%s> ",pFileInfo->strFileName.c_str());
	fprintf(pf,"%u ",pFileInfo->dwSize);
	fprintf(pf,"%u ",pFileInfo->dwOrginSize);
	fprintf(pf,"%u ",pFileInfo->dwValidSize);
	fprintf(pf,"%u ",pFileInfo->dwCrc32);
	fprintf(pf,"%d ",pFileInfo->dwPackageType);
	fprintf(pf,"%d ",pFileInfo->dwCompressType);
	fprintf(pf,"%d\n",pFileInfo->bFolder);

	if( pFileInfo->bFolder )
	{
		if(HaveChild(pFileInfo))
		{
			for(itFileInfo it = pFileInfo->ChildFilesInfo.begin();
				it != pFileInfo->ChildFilesInfo.end();it++)
			{
				Save(pf,(*it).second);
			}
		}

		//如果一个文件夹存储完毕，存一个返回表示
		fprintf(pf,"<%s> ","Return");
		fprintf(pf,"%u ",0);
		fprintf(pf,"%u ",0);
		fprintf(pf,"%u ",0);
		fprintf(pf,"%u ",0);
		fprintf(pf,"%d ",0);
		fprintf(pf,"%d ",0);
		fprintf(pf,"%d\n",20);
	}
	return true;
}

//寻找一个文件是否存在
bool	CFilesInfo::FindFileInfo(string& strFileName)
{
	if(GetFileInfoByText(strFileName)==NULL)	return true;
	return false;
}

tagFileInfo* 
CFilesInfo::AddFolderInfo(tagFileInfo* pParentFileInfo,string& strName,
						  DWORD dwSize,DWORD dwOrginSize,DWORD dwValidSize,
						  DWORD dwPackageType,DWORD dwCompressType)
{
	if(pParentFileInfo==NULL)	return NULL;

	char *newstring = _strlwr( _strdup( strName.c_str() ));
	strName = newstring;
	free(newstring);
	//插入新的节点
	tagFileInfo* pTemptFileInfo =
		new tagFileInfo(strName,dwSize,dwOrginSize,dwValidSize,0,dwPackageType,dwCompressType,true,pParentFileInfo);
	pParentFileInfo->ChildFilesInfo[strName] = pTemptFileInfo;

	//触发添加
	OnAdd(pTemptFileInfo);
	return pTemptFileInfo;
}

tagFileInfo* 
CFilesInfo::AddFileInfo(tagFileInfo* pParentFileInfo,string& strName,DWORD dwCrc32,
						DWORD dwSize,DWORD dwOrginSize,DWORD dwValidSize,
						DWORD dwPackageType,DWORD dwCompressType)
{
	if(pParentFileInfo==NULL)	return NULL;
	char *newstring = _strlwr( _strdup( strName.c_str() ));
	strName = newstring;
	free(newstring);
	//插入新的节点
	tagFileInfo* pTemptFileInfo =
		new tagFileInfo(strName,dwSize,dwOrginSize,dwValidSize,
						dwCrc32,dwPackageType,dwCompressType,false,pParentFileInfo);

	pParentFileInfo->ChildFilesInfo[strName] = pTemptFileInfo;
	//触发添加
	OnAdd(pTemptFileInfo);
	return pTemptFileInfo;
}

tagFileInfo* 
CFilesInfo::AddFolderInfo(string& strParentName,string& strName,
						  DWORD dwSize,DWORD dwOrginSize,DWORD dwValidSize,
						  DWORD dwPackageType,DWORD dwCompressType)
{
	tagFileInfo* pParentFileInfo = GetFileInfoByText(strParentName);
	if(pParentFileInfo==NULL)
		return NULL;
	return AddFolderInfo(pParentFileInfo,strName,dwSize,
						dwOrginSize,dwValidSize,dwPackageType,dwCompressType);
}

tagFileInfo* 
CFilesInfo::InsertFolderInfo(tagFileInfo* pParentFileInfo,string& strName,
							 DWORD dwSize,DWORD dwOrginSize,DWORD dwValidSize,
							 DWORD dwPackageType,DWORD dwCompressType)
{	
	return AddFolderInfo(pParentFileInfo,strName,dwSize,dwOrginSize,dwValidSize,
		dwPackageType,dwCompressType);
	
}
tagFileInfo* 
CFilesInfo::InsertFileInfo(tagFileInfo* pParentFileInfo,string& strName,
						   DWORD dwCrc32,DWORD dwSize,DWORD dwOrginSize,
						   DWORD dwValidSize,DWORD dwPackageType,DWORD dwCompressType)
{
	//插入新的节点
	tagFileInfo* pTemptFileInfo =AddFileInfo(pParentFileInfo,strName,
											dwCrc32,dwSize,dwOrginSize,dwValidSize,
											dwPackageType,dwCompressType);
	
	if(pTemptFileInfo)
	{
		AddFileSize(pParentFileInfo,pTemptFileInfo->dwSize);
	}
	return pTemptFileInfo;
}
//更新一个文件信息
tagFileInfo* 
CFilesInfo::UpdateFileInfo(string& strFileName,DWORD dwCrc32,DWORD dwSize,
							DWORD dwOrginSize,DWORD dwValidSize,
							DWORD dwPackageType,DWORD dwCompressType)
{
	char *newstring = _strlwr( _strdup( strFileName.c_str() ));
	strFileName = newstring;
	free(newstring);

	tagFileInfo* pParentFileInfo = pParentFileInfo = GetRootFile();

	string str = strFileName;
	tagFileInfo* pInFileInfo=NULL;
	int nPos = str.find("\\");

	string strTempt("");
	while(str.find("\\") ==0 && pParentFileInfo != NULL)
	{
		str = str.erase(0,str.find("\\")+1);
		nPos = str.find("\\");
		strTempt="";
		if(nPos < 0)
		{
			strTempt=str;
			str = "";
		}
		else
		{
			strTempt.insert(strTempt.begin(),str.begin(),str.begin()+str.find("\\"));
			str = str.erase(0,str.find("\\"));
		}
		pInFileInfo=pParentFileInfo;
		pParentFileInfo = FindChildFileInfoByText(pParentFileInfo,strTempt);
	}

	if(pParentFileInfo==NULL)
	{
		do
		{
			bool bFolder = false;
			nPos = str.find("\\");
			if(nPos < 0)
			{
				bFolder = false;
				pParentFileInfo = new tagFileInfo(	strTempt,dwSize,dwOrginSize,
													dwValidSize,dwCrc32,dwPackageType,
													dwCompressType,bFolder,pInFileInfo);
			}
			else
			{
				bFolder = true;
				pParentFileInfo = new tagFileInfo(strTempt,0,0,0,0,0,0,bFolder,pInFileInfo);
			}

			//插入新的节点
			pInFileInfo->ChildFilesInfo[strTempt] = pParentFileInfo;
			pInFileInfo = pParentFileInfo;

			if(bFolder==false)
			{
				AddFileSize(pInFileInfo->ParentFileInfo,pParentFileInfo->dwSize);
				UpdateParentFolderInfo(pInFileInfo->ParentFileInfo);
			}

			//触发添加
			OnAdd(pParentFileInfo);


			str = str.erase(0,str.find("\\")+1);
			nPos = str.find("\\");
			strTempt="";
			if(nPos < 0)
			{
				strTempt=str;
				str = "";
			}
			else
			{
				strTempt.insert(strTempt.begin(),str.begin(),str.begin()+str.find("\\"));
				str = str.erase(0,str.find("\\"));
			}
		}while(strTempt.compare("") != 0);
	}
	else
	{
		pParentFileInfo->dwPackageType=dwPackageType;
		pParentFileInfo->dwCompressType=dwCompressType;
		
		if(pParentFileInfo->bFolder==false)
		{
			pParentFileInfo->dwCrc32=dwCrc32;
			int nDirSize = dwSize-pParentFileInfo->dwSize;
			pParentFileInfo->dwSize = dwSize;
			pParentFileInfo->dwOrginSize = dwOrginSize;
			pParentFileInfo->dwValidSize = dwValidSize;
			AddFileSize(pParentFileInfo->ParentFileInfo,nDirSize);
			UpdateParentFolderInfo(pParentFileInfo->ParentFileInfo);
		}

		//触发更新
		OnUpdate(pParentFileInfo);
	}
	return pInFileInfo;
}

//更新一个文件信息
tagFileInfo* 
CFilesInfo::UpdateFileInfo(string& strFileName,DWORD dwCrc32,DWORD dwSize,
						   DWORD dwOrginSize,DWORD dwValidSize,bool bFolder,
						   DWORD dwPackageType,DWORD dwCompressType)
{
	char *newstring = _strlwr( _strdup( strFileName.c_str() ));
	strFileName = newstring;
	free(newstring);

	tagFileInfo* pFileInfo = GetFileInfoByText(strFileName);
	if(pFileInfo)
	{		
		pFileInfo->dwPackageType=dwPackageType;
		pFileInfo->dwCompressType=dwCompressType;		
		//触发更新
		if(pFileInfo->bFolder==false)
		{
			pFileInfo->dwCrc32=dwCrc32;
			int nDirSize = dwSize-pFileInfo->dwSize;
			pFileInfo->dwSize = dwSize;
			pFileInfo->dwOrginSize=dwOrginSize;
			pFileInfo->dwValidSize=dwValidSize;
			AddFileSize(pFileInfo->ParentFileInfo,nDirSize);
			UpdateParentFolderInfo(pFileInfo->ParentFileInfo);
		}

		OnUpdate(pFileInfo);
	}
	else
	{
		string str(""),strTempt("");
		str.insert(str.begin(),strFileName.begin(),
					strFileName.begin()+strFileName.find_last_of("\\"));
		strTempt.insert(strTempt.begin(),
						strFileName.begin()+strFileName.find_last_of("\\")+1,
						strFileName.end());

		pFileInfo = GetFileInfoByText(str);
		if(pFileInfo==NULL)	return NULL;
		//插入新的节点
		tagFileInfo* pTemptFileInfo =
			new tagFileInfo(strTempt,dwSize,dwOrginSize,dwValidSize,dwCrc32,
							dwPackageType,dwCompressType,bFolder,pFileInfo);
		pFileInfo->ChildFilesInfo[strTempt] = pTemptFileInfo;
		pFileInfo=pTemptFileInfo;

		if(bFolder==false)
		{
			AddFileSize(pTemptFileInfo->ParentFileInfo,pTemptFileInfo->dwSize);
			UpdateParentFolderInfo(pTemptFileInfo->ParentFileInfo);
		}

		//触发添加
		OnAdd(pFileInfo);
	}
	return pFileInfo;

}

//需要更新父结点的大小和其他信息
tagFileInfo* 
CFilesInfo::UpdateFileInfo(tagFileInfo* ptagInfo,DWORD dwCrc32,DWORD dwSize,
							DWORD dwOrginSize,DWORD dwValidSize,bool bFolder,
							DWORD dwPackageType,DWORD dwCompressType)
{
	if(ptagInfo)
	{
		ptagInfo->dwPackageType=dwPackageType;
		ptagInfo->dwCompressType=dwCompressType;		
		//触发更新
		if(ptagInfo->bFolder==false)
		{
			ptagInfo->dwCrc32=dwCrc32;
			int nDirSize = dwSize-ptagInfo->dwSize;
			ptagInfo->dwSize = dwSize;
			ptagInfo->dwOrginSize=dwOrginSize;
			ptagInfo->dwValidSize=dwValidSize;
			AddFileSize(ptagInfo->ParentFileInfo,nDirSize);

			UpdateParentFolderInfo(ptagInfo->ParentFileInfo);
		}
		OnUpdate(ptagInfo);
	}
	return ptagInfo;
}

//更新打包信息
void	CFilesInfo::UpdateFileInfoWithChild(string& strFileName,DWORD dwPackageType,
									DWORD dwCompressType,
									bool bForcePack,
									bool bForceCompress)
{
	char *newstring = _strlwr( _strdup( strFileName.c_str() ));
	strFileName = newstring;
	free(newstring);

	tagFileInfo* pFileInfo = GetFileInfoByText(strFileName);
	if(pFileInfo)
	{		
		//更新子文件
		UpdateFileInfoWithChild(pFileInfo,dwPackageType,dwCompressType,bForcePack,bForceCompress);
		//if(pFileInfo->bFolder==false)
		//{
		//	pFileInfo->dwCrc32=dwCrc32;
		//	int nDirSize = dwSize-pFileInfo->dwSize;
		//	pFileInfo->dwSize = dwSize;
		//	pFileInfo->dwOrginSize=dwOrginSize;
		//	pFileInfo->dwValidSize=dwValidSize;
		//	AddFileSize(pFileInfo->ParentFileInfo,nDirSize);
		//	UpdateParentFolderInfo(pFileInfo->ParentFileInfo);
		//}

		UpdateParentFolderInfo(pFileInfo->ParentFileInfo);
	}
}

//更新打包信息
void	CFilesInfo::UpdateFileInfoWithChild(tagFileInfo* ptagInfo,DWORD dwPackageType,
									DWORD dwCompressType,
									bool bForcePack,
									bool bForceCompress)
{
	if(ptagInfo == NULL)	return;
	if(HaveChild(ptagInfo))
	{
		for(itFileInfo it = ptagInfo->ChildFilesInfo.begin();
			it != ptagInfo->ChildFilesInfo.end();it++)
		{
			UpdateFileInfoWithChild((*it).second,dwPackageType,dwCompressType,bForcePack,bForceCompress);
		}
	}
	if( ENABLEBIT(ptagInfo->dwPackageType,NOPACK) ||  bForcePack)
		ptagInfo->dwPackageType=dwPackageType;
	if( ENABLEBIT(ptagInfo->dwCompressType,NOCOMPRESS) ||  bForceCompress)
		ptagInfo->dwCompressType=dwCompressType;
	//更新这个消息
	OnUpdate(ptagInfo);
	return;
}

void	CFilesInfo::UpdateFilePackInfoWithChild(string& strFileName,DWORD dwPackageType)
{
	char *newstring = _strlwr( _strdup( strFileName.c_str() ));
	strFileName = newstring;
	free(newstring);

	tagFileInfo* pFileInfo = GetFileInfoByText(strFileName);
	if(pFileInfo)
	{		
		//更新子文件
		UpdateFilePackInfoWithChild(pFileInfo,dwPackageType);
		UpdateParentFolderInfo(pFileInfo->ParentFileInfo);
	}
}

void	CFilesInfo::UpdateFilePackInfoWithChild(tagFileInfo* ptagInfo,DWORD dwPackageType)
{
	if(ptagInfo == NULL)	return;
	if(HaveChild(ptagInfo))
	{
		for(itFileInfo it = ptagInfo->ChildFilesInfo.begin();
			it != ptagInfo->ChildFilesInfo.end();it++)
		{
			UpdateFilePackInfoWithChild((*it).second,dwPackageType);
		}
	}
	if( !ENABLEBIT(ptagInfo->dwPackageType,NOPACK) )
		ptagInfo->dwPackageType=dwPackageType;
	//更新这个消息
	OnUpdate(ptagInfo);
	return;
}


//根据一个文件信息表包含的所有文件信息，来更新自己
void	CFilesInfo::UpdateFilePackInfoWithChild(CFilesInfo* pOtherFilesInfo)
{
	if(pOtherFilesInfo == NULL)	return;
	UpdateFilePackInfoWithChild(pOtherFilesInfo,pOtherFilesInfo->GetRootFile());
}
//更新包信息
void	CFilesInfo::UpdateFilePackInfoWithChild(CFilesInfo* pOtherFilesInfo,
									tagFileInfo* pOthertagInfo)
{
	if(pOtherFilesInfo->HaveChild(pOthertagInfo))
	{
		for(itFileInfo it = pOthertagInfo->ChildFilesInfo.begin();
			it != pOthertagInfo->ChildFilesInfo.end();it++)
		{
			UpdateFilePackInfoWithChild( pOtherFilesInfo ,(*it).second);
		}
	}
	if(pOthertagInfo->bFolder == false)
	{
		string strFileName = pOtherFilesInfo->GetFileTextWithParent(pOthertagInfo);
		tagFileInfo* pFileInfo = GetFileInfoByText(strFileName);
		if(pFileInfo)
		{
			pFileInfo->dwPackageType = pOthertagInfo->dwPackageType;
			OnUpdate(pFileInfo);
			UpdateParentFolderInfo(pFileInfo->ParentFileInfo);
		}
	}
}

 //删除一个文件信息
VOID	CFilesInfo::DelFileInfo(string& strFileName)
{
	char *newstring = _strlwr( _strdup( strFileName.c_str() ));
	strFileName = newstring;
	free(newstring);

	tagFileInfo* ptagInfo = GetFileInfoByText(strFileName);
	string strTemptName = ptagInfo->strFileName;
	tagFileInfo* pParentInfo = ptagInfo->ParentFileInfo;
	if(pParentInfo)
	{
		DelFileInfo(ptagInfo);
		pParentInfo->ChildFilesInfo.erase(strTemptName);
	}
}

//删除一个文件信息
VOID	CFilesInfo::DelFileInfo(tagFileInfo* pFileInfo)
{
	if(pFileInfo==NULL)	return;
	if(HaveChild(pFileInfo))
	{
		for(itFileInfo it = pFileInfo->ChildFilesInfo.begin();
			it != pFileInfo->ChildFilesInfo.end();)
		{
			DelFileInfo((*it).second);
			pFileInfo->ChildFilesInfo.erase(it++);
		}

		//触发删除
		OnDelete(pFileInfo);
		delete pFileInfo;pFileInfo=NULL;
	}
	else
	{
		//减去相关大小
		int nSize = pFileInfo->dwSize;
		AddFileSize(pFileInfo->ParentFileInfo,-nSize);
		//触发删除
		OnDelete(pFileInfo);
		delete pFileInfo;pFileInfo=NULL;
	}
}

//返回0，表示添加失败
DWORD	CFilesInfo::AddPackFileInfo(string& strPackFileName)
{
	//不能添加空的包名
	if( strPackFileName.length() == 0)	return 0;

	DWORD dwID = 0;
	//如果存在则返回ID，否则找出最大的ID，以便添加。
	itTypeInfo it = m_PackageInfos.begin();
	for(;it != m_PackageInfos.end();it++)
	{
		if( strPackFileName == (*it).strFileName )
			return (*it).dwID;
		else if( (*it).dwID > dwID )
			dwID = (*it).dwID;
	}
	if(dwID == 0xFFFFFFFF)
		return 0;

	dwID++;
	tagPackFileInfo Info = {dwID,strPackFileName};
	m_PackageInfos.push_back(Info);
	return dwID;
}

string	CFilesInfo::GetPackFileName(DWORD dwID)
{
	itTypeInfo it = m_PackageInfos.begin();
	for(;it != m_PackageInfos.end();it++)
	{
		if( (*it).dwID == dwID )
			return (*it).strFileName;
	}
	return "";
}


DWORD	CFilesInfo::GetPackID(string strFileName)
{
	itTypeInfo it = m_PackageInfos.begin();
	for(;it != m_PackageInfos.end();it++)
	{
		if( (*it).strFileName == strFileName )
			return (*it).dwID;
	}
	return 0;
}

//得到文件个数
DWORD	CFilesInfo::GetFileNum()
{
	return m_dwFileNum;
}

VOID	CFilesInfo::Clear()
{
	for(itFileInfo it = m_pRootFile->ChildFilesInfo.begin();
		it != m_pRootFile->ChildFilesInfo.end();it++)
	{
		DelFileInfo((*it).second);
	}
	m_pRootFile->ChildFilesInfo.clear();
	m_PackageInfos.clear();
}

//得到文件的CRC信息码
//返回0 表示没有此文件
DWORD	CFilesInfo::GetFileCrc32(string& strFileName)
{
	char *newstring = _strlwr( _strdup( strFileName.c_str() ));
	strFileName = newstring;
	free(newstring);

	tagFileInfo* pFileInfo = GetFileInfoByText(strFileName);
	if(pFileInfo)
		return pFileInfo->dwCrc32;
	return 0;
}

//是否是文件夹
bool	CFilesInfo::IsFolder(string& strFileName)
{
	char *newstring = _strlwr( _strdup( strFileName.c_str() ));
	strFileName = newstring;
	free(newstring);

	tagFileInfo* pFileInfo = GetFileInfoByText(strFileName);
	if(pFileInfo)
		return pFileInfo->bFolder;
	return false;
}

//得到一个文件的信息
BOOL
CFilesInfo::GetFileInfo(const string& strFileName,DWORD& dwSize,
						DWORD& dwOrginSize,DWORD &dwValidSize,DWORD& dwCrc32,
						DWORD& dwPackageType,DWORD& dwCompressType,bool& bFolder)
{
	string strTemptFileName(strFileName);
	char *newstring = _strlwr( _strdup( strTemptFileName.c_str() ));
	strTemptFileName = newstring;
	free(newstring);

	dwCrc32=-1;dwPackageType=-1;dwCompressType=-1;
	tagFileInfo* pFileInfo = GetFileInfoByText(strTemptFileName);
	if(pFileInfo)
	{
		dwSize = pFileInfo->dwSize;
		dwOrginSize=pFileInfo->dwOrginSize;;
		dwValidSize=pFileInfo->dwValidSize;;
		dwCrc32= pFileInfo->dwCrc32;
		dwPackageType= pFileInfo->dwPackageType;
		dwCompressType= pFileInfo->dwCompressType;
		bFolder = pFileInfo->bFolder;
		return true;
	}
	return false;
}

//根据是否打包得到两个文件信息列表的差异
bool
CFilesInfo::GetDifferenceByPackage(CFilesInfo& filesInfo,CFilesInfo& temptFilesInfo)
{
	return GetDifferenceByPackage(GetRootFile(),filesInfo,temptFilesInfo,true);
}

bool
CFilesInfo::GetDifferenceByPackage(tagFileInfo* ptagFileInfo,CFilesInfo& filesInfo,
								   CFilesInfo& temptFilesInfo,bool bPackage)
{
	string strTempt = GetFileTextWithParent(ptagFileInfo);
	if(ptagFileInfo==NULL)	return false;
	if(HaveChild(ptagFileInfo))
	{
		temptFilesInfo.UpdateFileInfo(strTempt,0,0,0,0,true,ptagFileInfo->dwPackageType,
			ptagFileInfo->dwCompressType);

		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			GetDifferenceByPackage((*it).second,filesInfo,temptFilesInfo,bPackage);
		}
		
		//没有子文件，则删除该文件夹
		tagFileInfo* pTemptTagInfo = temptFilesInfo.GetFileInfoByText(strTempt);
		if(pTemptTagInfo->ChildFilesInfo.size()<=0)
		{
			temptFilesInfo.DelFileInfo(strTempt);
		}
	}
	else
	{
		if(ptagFileInfo->bFolder==false)
		{
			DWORD dwSize=0,dwOrginSize=0,dwValidSize=0;
			DWORD dwCrc32=2,dwPackageType=1,dwCompressType=1;
			bool bFolder=false;
			if(filesInfo.GetFileInfo(strTempt,dwSize,dwOrginSize,dwValidSize,
									dwCrc32,dwPackageType,dwCompressType,bFolder))
			{
				if(bPackage == true)
				{
					if(ptagFileInfo->dwCrc32 != dwCrc32  ||
						ptagFileInfo->dwPackageType != dwPackageType ||
						ptagFileInfo->dwCompressType != dwCompressType)
					{
						if( !ENABLEBIT(dwCompressType,NOCOMPRESS) ||
							ptagFileInfo->dwOrginSize != dwOrginSize)
						{
							temptFilesInfo.UpdateFileInfo(strTempt,
														ptagFileInfo->dwCrc32,
														ptagFileInfo->dwSize,
														ptagFileInfo->dwOrginSize,
														ptagFileInfo->dwValidSize,
														ptagFileInfo->dwPackageType,
														ptagFileInfo->dwCompressType);
						}
					}
				}
				else
				{
					if(ptagFileInfo->dwCrc32 != dwCrc32 ||
						(ptagFileInfo->dwCrc32 == dwCrc32 && dwValidSize != dwSize) )
					{
						temptFilesInfo.UpdateFileInfo(	strTempt,ptagFileInfo->dwCrc32,
														ptagFileInfo->dwSize,
														ptagFileInfo->dwOrginSize,
														ptagFileInfo->dwValidSize,
														ptagFileInfo->dwPackageType,
														ptagFileInfo->dwCompressType);
					}
				}
			}
			else
			{
				temptFilesInfo.UpdateFileInfo(strTempt,ptagFileInfo->dwCrc32,
												ptagFileInfo->dwSize,
												ptagFileInfo->dwOrginSize,
												ptagFileInfo->dwValidSize,
												ptagFileInfo->dwPackageType,
												ptagFileInfo->dwCompressType);
			}
		}
	}

	return true;
}

//根据是否变化得到两个文件信息列表的差异
bool CFilesInfo::GetDifferenceByCrc32(CFilesInfo& filesInfo,CFilesInfo& temptFilesInfo)
{	
	return GetDifferenceByPackage(GetRootFile(),filesInfo,temptFilesInfo,false);
}


bool CFilesInfo::CopyData(CFilesInfo* pFileInfo)
{
	CopyData(GetRootFile(),pFileInfo);
	//pFileInfo->m_pDisplayWnd = m_pDisplayWnd;
	return true;
}

bool CFilesInfo::CopyData(tagFileInfo* ptagFileInfo,CFilesInfo* pFileInfo)
{
	if(pFileInfo==NULL || ptagFileInfo==NULL)	return false;

	if(ptagFileInfo != GetRootFile())
	{
		string strName(GetFileTextWithParent(ptagFileInfo));
		pFileInfo->UpdateFileInfo(	strName,ptagFileInfo->dwCrc32,ptagFileInfo->dwSize,
									ptagFileInfo->dwOrginSize,ptagFileInfo->dwValidSize,
									ptagFileInfo->bFolder,ptagFileInfo->dwPackageType,
									ptagFileInfo->dwCompressType);
	}

	if(HaveChild(ptagFileInfo))
	{
		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			CopyData((*it).second,pFileInfo);
		}
	}
	return true;
}

//是否有子节点
bool	CFilesInfo::HaveChild(tagFileInfo* pFieInfo)
{
	if(pFieInfo==NULL)	return false;
	if(pFieInfo->ChildFilesInfo.size() > 0)	return true;
	return false;
}

//是否有未打包文件
bool	CFilesInfo::ChildHaveUnPackFile(tagFileInfo* ptagFieInfo)
{
	for(itFileInfo it = ptagFieInfo->ChildFilesInfo.begin();
		it != ptagFieInfo->ChildFilesInfo.end();it++)
	{
		if((*it).second->bFolder==true || (*it).second->dwPackageType==0)
			return true;
	}
	return false;
}

//找到一个匹配字符子节点
tagFileInfo* CFilesInfo::FindChildFileInfoByText(tagFileInfo* pParent,string& str)
{
	char *newstring = _strlwr( _strdup( str.c_str() ));
	str = newstring;
	free(newstring);

	if(pParent == NULL)	return NULL;
	if(str.compare("")==0)
		return m_pRootFile;

	if(HaveChild(pParent) == false)	return NULL;

	itFileInfo it = pParent->ChildFilesInfo.find(str);
	if(it != pParent->ChildFilesInfo.end())
		return (*it).second;
	return NULL;
}

//根据字符串得到一个节点
tagFileInfo* CFilesInfo::GetFileInfoByText(string strFileName)
{
	string str(strFileName);

	if(str.compare("") ==0 )
		return m_pRootFile;

	tagFileInfo* pFileInfo = GetRootFile();
	int nPos = str.find("\\");

	while(str.find("\\") ==0 && pFileInfo != NULL)
	{
		str = str.erase(0,str.find("\\")+1);
		string strTempt("");
		nPos = str.find("\\");
		if(nPos < 0)
		{
			strTempt=str;
			str = "";
		}
		else
		{
			strTempt.insert(strTempt.begin(),str.begin(),str.begin()+str.find("\\"));

			str = str.erase(0,str.find("\\"));
		}
		pFileInfo = FindChildFileInfoByText(pFileInfo,strTempt);
	}
	return pFileInfo;
}

//得到一个节点的和父结点连接字符串
string CFilesInfo::GetFileTextWithParent(tagFileInfo* pFileInfo)
{
	
	string str("");
	while(pFileInfo && pFileInfo != GetRootFile())
	{
		str= pFileInfo->strFileName+str;
		pFileInfo = pFileInfo->ParentFileInfo;
		str="\\"+str;
	}
	return str;
}

//添加文件大小不
void CFilesInfo::AddFileSize(tagFileInfo* pFileInfo,int iSize)
{
	if(pFileInfo==NULL)	return;
	pFileInfo->dwSize += iSize;

	//触发更新
	OnUpdate(pFileInfo);

	if(pFileInfo->ParentFileInfo)
		AddFileSize(pFileInfo->ParentFileInfo,iSize);
}


//统计父文件夹信息
void CFilesInfo::UpdateParentFolderInfo(tagFileInfo* ptagFileInfo)
{
	if(ptagFileInfo==NULL)	return;

	if(HaveChild(ptagFileInfo))
	{
		DWORD dwTemptPackage = 0,dwTemptCompress = 0;

		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			dwTemptPackage |= (*it).second->dwPackageType;
			dwTemptCompress |= (*it).second->dwCompressType;
		}
		if( ptagFileInfo->dwPackageType!=dwTemptPackage || ptagFileInfo->dwCompressType!=dwTemptCompress )
		{
			ptagFileInfo->dwPackageType=dwTemptPackage;
			ptagFileInfo->dwCompressType=dwTemptCompress;
			//触发更新
			OnUpdate(ptagFileInfo);
		}
	}

	UpdateParentFolderInfo(ptagFileInfo->ParentFileInfo);
}
//统计文件夹信息
void CFilesInfo::UpdateFolderInfo(tagFileInfo* ptagFileInfo,DWORD& dwPackage,DWORD& dwCompress)
{
	if(ptagFileInfo==NULL)	return;

	if(ptagFileInfo->bFolder==true)
	{
		DWORD dwTemptPackage = 0,dwTemptCompress=0;

		if(HaveChild(ptagFileInfo))
		{
			for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
				it != ptagFileInfo->ChildFilesInfo.end();it++)
			{
				UpdateFolderInfo((*it).second,dwTemptPackage,dwTemptCompress);
			}

			if( ptagFileInfo->dwPackageType!=dwTemptPackage ||
				ptagFileInfo->dwCompressType!=dwTemptCompress )
			{
				ptagFileInfo->dwPackageType=dwTemptPackage;ptagFileInfo->dwCompressType=dwTemptCompress;
				//触发更新
				OnUpdate(ptagFileInfo);
			}
		}
		else
		{
			ptagFileInfo->dwPackageType=1;ptagFileInfo->dwCompressType=1;
		}

		dwPackage = dwPackage|dwTemptPackage;dwCompress = dwCompress|dwTemptCompress;
	}
	else
	{
		//if( ptagFileInfo->dwPackageType != 1 ||
		//	ptagFileInfo->dwCompressType != 1 )
		//{
		//	ptagFileInfo->dwPackageType=1;
		//	ptagFileInfo->dwCompressType=1;

		//	//触发更新
		//	OnUpdate(ptagFileInfo);
		//}

		dwPackage = (ptagFileInfo->dwPackageType|dwPackage);
		dwCompress= (ptagFileInfo->dwCompressType|dwCompress);
	}
}

void CFilesInfo::FindFileList(string& strFile, string& strExt, list<string>& listFileName)
{
	//转换成小写
	char *newstring = _strlwr( _strdup( strFile.c_str() ));
	strFile = newstring;
	free(newstring);

	newstring = _strlwr( _strdup( strExt.c_str() ));
	strExt = newstring;
	free(newstring);
	
	string strTempt(strFile);
	tagFileInfo* ptagFileInfo = GetFileInfoByText(strTempt);
	if(ptagFileInfo)
	{
		FindFileList(ptagFileInfo,strExt,listFileName);
	}
}

void CFilesInfo::FindFileList(tagFileInfo* ptagFileInfo, string& strExt,
							  list<string>& listFileName)
{
	if(ptagFileInfo==NULL)	return;
	if(HaveChild(ptagFileInfo))
	{
		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			FindFileList((*it).second,strExt,listFileName);
		}
	}
	else
	{	
		if(ptagFileInfo->bFolder==false)
		{
			string strTempt = ptagFileInfo->strFileName;
			strTempt.erase(0,strTempt.find_last_of("."));
			if(strTempt==strExt)
			{
				strTempt = GetFileTextWithParent(ptagFileInfo);
				listFileName.push_back(strTempt);
			}
		}
	}
}

void CFilesInfo::OnAdd(tagFileInfo* ptagFileInfo)
{
	if(ptagFileInfo->bFolder == false)
		m_dwFileNum++;
	if(ptagFileInfo==NULL)	return;
	if(m_pDisplayWnd)
	{
		::SendMessage(m_pDisplayWnd,TREELIST_ADD,(WPARAM)this,(LPARAM)ptagFileInfo);
	}
}

void CFilesInfo::OnUpdate(tagFileInfo* ptagFileInfo)
{
	if(ptagFileInfo==NULL)	return; 
	if(m_pDisplayWnd)
	{
		::SendMessage(m_pDisplayWnd,TREELIST_UPDATE,(WPARAM)this,(LPARAM)ptagFileInfo);
	}
}

void CFilesInfo::OnDelete(tagFileInfo* ptagFileInfo)
{
	if(ptagFileInfo->bFolder == false)
		m_dwFileNum++;

	if(ptagFileInfo==NULL)	return;
	if(m_pDisplayWnd)
	{
		::SendMessage(m_pDisplayWnd,TREELIST_DELETE,(WPARAM)this,(LPARAM)ptagFileInfo);
	}
}

void CFilesInfo::OnDeleteAll()
{
	m_dwFileNum = 0;

	if(m_pDisplayWnd)
	{
		::SendMessage(m_pDisplayWnd,TREELIST_DELALL,0,0);
	}
}

void CFilesInfo::OnIntial()
{
	if(m_pDisplayWnd)
	{
		::SendMessage(m_pDisplayWnd,TREELIST_INITIAL,0,0);
	}
}

void CFilesInfo::UpdateAllInfoToView(HWND hWnd)
{
	if(hWnd==NULL)
		hWnd=m_pDisplayWnd;

	::SendMessage(hWnd,TREELIST_INITIAL,0,0);
	UpdateAllInfoToView(GetRootFile(),hWnd);
}

void CFilesInfo::UpdateAllInfoToViewEx(HWND hWnd)
{
	if(hWnd==NULL)
		hWnd=m_pDisplayWnd;

	::SendMessage(hWnd,TREELIST_INITIAL,0,0);
	::SendMessage(hWnd,TREELIST_UPDATEWITHCHILD,(WPARAM)this,(LPARAM)GetRootFile());
}

void CFilesInfo::UpdateAllInfoToView(tagFileInfo* ptagFileInfo,HWND hWnd)
{
	if(ptagFileInfo==NULL)	return;
	::SendMessage(hWnd,TREELIST_ADD,(WPARAM)this,(LPARAM)ptagFileInfo);

	if(HaveChild(ptagFileInfo))
	{
		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			UpdateAllInfoToView((*it).second,hWnd);
		}
	}
}

DWORD	GetFirstEnableBit(DWORD dwNum)
{
	int i=0;
	for(;i< sizeof(DWORD);i++)
	{
		if( ENABLEBIT(dwNum,1<<i) )
			break;
	}
	return 1<<i;
}