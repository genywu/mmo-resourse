#pragma once

//////////////////////////////////////////////////////////////////////////
//MakePatch.h
//Fun:负责制作下载升级包
//Create Time:2004.12.09
//Author:Joe
//////////////////////////////////////////////////////////////////////////

#include <string>

#include "../../PUBLIC/FilesInfo.h"
#include "../../PUBLIC/Package.h"
#include "../../PUBLIC/ClientResource.h"

class CMakePatch
{
public:
	CMakePatch(string& strVersion,string& strExeFolder,DWORD dwPackType,
				CFilesInfo* LastFilesInfo,CClientResource* pClientResource);
	~CMakePatch(void);
private:
	DWORD			m_dwPackType;						//写入更新文件的包
	string			m_strVersion;						//更新的版本号
	string			m_strExeFoler;						//当前文件夹
	CFilesInfo*		m_pLastFilesInfo;					//原始文件信息
	CClientResource* m_pOriginResource;					//当前资源

public: 
	void	MakePatch(bool bSelect);								//生成下载升级包
	bool	MakePatchPackage(tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,CPackage* pPackage);
	bool	UpdatePatch(string& strDir,string& strPackageName,string& strFilesInfoName);

	bool	UpdateAutoUpdateFolder(CFilesInfo& DifFilesInfo,CFilesInfo* pSouFilesInfo);	//更新自动更新的文件夹里的文件
};
