#pragma once

//////////////////////////////////////////////////////////////////////////
//MakeAutoUpdate.h
//Fun:负责制作自动更新包
//Create Time:2004.11.16
//Author:Joe
//////////////////////////////////////////////////////////////////////////

#include <string>

#include "../../PUBLIC/FilesInfo.h"
#include "../../PUBLIC/Package.h"
#include "../../PUBLIC/ClientResource.h"

class CMakeAutoUpdate
{
public:
	CMakeAutoUpdate(string& strVersion,string& strExeFolder,DWORD dwPackType,
					CFilesInfo* LastFilesInfo,CClientResource* pClientResource);
	~CMakeAutoUpdate(void);
private:
	DWORD			m_dwPackType;						//写入更新文件的包
	string			m_strVersion;						//更新的版本号
	string			m_strExeFoler;						//当前文件夹
	CFilesInfo*		m_pLastFilesInfo;					//原始文件信息
	CClientResource* m_pOriginResource;					//当前资源

public: 
	void	MakeAutoUpdatePackage(long lCurMode/*制作模式 0：服务器，1：客户端*/, bool bIsSelcted/*0:从当前版本制作 1:从选择版本制作*/, bool bPatch=false);	//生成自动更新保 
	void	MakePacth(long lCurMode/*制作模式 0：服务器，1：客户端*/, CFilesInfo& DifFilesInfo);

	bool	MakePatchPackage(tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,CPackage* pPackage);
	bool	UpdatePatch(long lCurMode/*制作模式 0：服务器，1：客户端*/, string& strDir,string& strPackageName,string& strFilesInfoName);
};
