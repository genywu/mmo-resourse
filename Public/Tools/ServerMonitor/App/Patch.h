#pragma once
//#ifndef ZLIB_WINAPI
//#define ZLIB_WINAPI
//#endif
#include "../../../../Public/ziplib/zlib.h"
#include "../../../../Public/ziplib/unzip.h"

class CPatch
{
public:
	CPatch(void);
	~CPatch(void);
public:
	BOOL				Init(const char* filename);
private:
	CFilesInfo*			m_pPatchFilesInfo;		//升级的文件信息
	CPackage*			m_pPatchPackage;		//升级的包数据
	CClientResource*	m_pClientResource;		//客户端资源

	string				m_strWorkPath;
	string				m_strExePath;			//执行文件目录
	string				m_strFilesInfoName;		//文件信息名字

	string				m_strTemptPackageName;	//临时包名
	string				m_strTemptFilesInfoName;//临时文件姓名

	string				m_listStrUpdatedVersion;	//新的版本号

	string				m_strPatchName;			//包名
	DWORD				m_dwCrc32;				//当前包文件信息CRC32码
public:
	BOOL	GetPatchData();						//得到升级包数据
	BOOL	CheckGameRunFolder();				//检查是否是游戏运行目录
	
	void	SetCrc32(DWORD crc32)				{m_dwCrc32 = crc32;}
	string	Update(const char* filename,const char* patchname);

	bool	Release();
	static BOOL	 LoadClientResource(CPatch* pPatch);
	static BOOL  UpdateClientResource(tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,BYTE*& pBuf,DWORD& dwBuflen,
		CPackage* pPackage,CClientResource* pClientResource,HWND hwnd,int& nTotalSize);

public:
};
