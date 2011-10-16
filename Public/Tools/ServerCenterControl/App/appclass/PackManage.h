

#include "../../../../SCMPublic/class/UpdatePack.h"


#pragma once


class PackManage
{
public:
	PackManage(VOID);
	~PackManage(VOID);
public:
	//!
	static		PackManage&					GetInstance(VOID) {return c_PackManage;}

public:
	//!										初始化对象
	BOOL									Init(VOID);
	//!										释放对象
	VOID									Release(VOID);

public:
	//!										响应用户上传更新包
	VOID									OnUserUploadData(DBReadSet &setReadDB, LONG lUserSocket);
	//!										响应用户上传文件
	VOID									OnUserUploadFile(DBReadSet &setReadDB, LONG lUserSocket);

	//!										发送所有更新包信息到客户端
	VOID									SendPackInfoListToClient(LONG lUserSocket);
	//!										发送一个类型GAPPP的更新文件信息到客户端
	VOID									SendFileInfoListToClient(DWORD dwGappID, LONG lUserSocket);

	//!										把一个GAPP更新数据压入数据集
	BOOL									GetUpdateGapp(DWORD dwGappType, LPCSTR pVersion, DBWriteSet &setWriteDB);
	//!										确认一个GAPP的版本是不是最高的
	BOOL									IsHigher(DWORD dwGappType, LPCSTR pVersion);
	//!										找到一个GAPP的最新更新包，压入数据集
	BOOL									FindAndGetHighestPack(DWORD dwGappType, DBWriteSet &setWriteDB);

public:
	//!										得到一个指定路径的文件
	const		CUpdateFile*				GetFile(DWORD dwGappType, LPCSTR pPathAndName);
	//!										刷新缓存文件
	VOID									RefreshFileCache(VOID);

private:
	//!										读取所有更新包的信息
	VOID									LoadAllPack(VOID);
	//!										读取所有更新文件的信息
	VOID									LoadAllFileInfo(VOID);
	//!										把一个包对应的子目录添加到传入的指针中
	BOOL									GetPackSubDir(DWORD dwServerType, LPCSTR pServerVersion, char *pOutPath);

private:
	//!										当前服务器保存的所有服务器的所有更新包
	map<DWORD, map<string, string>>			m_mapAllPack;
	//!										当前服务器保存的所有更新文件
	map<DWORD, set<string>>					m_mapAllFileInfo;
	//!										枚举值和服务器名字对应表
	map<DWORD, string>						m_mapServerName;
	//!										把每种GAPP的最高版本缓存在内存中
	map<DWORD, pair<string, UpdatePack*>>	m_mapCachePack;
	//!										提供给SM自助的文件的读取缓存
	map<string, CUpdateFile*>				m_mapSmDiyFile;

private:
	static		PackManage					c_PackManage;
	static		LPSTR						c_pPackPath;
	static		LPSTR						c_pFilePath;
	static		LPSTR						c_pDiyPath;
	static		LPSTR						c_pReadMeFileName;
};