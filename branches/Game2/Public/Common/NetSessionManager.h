#pragma once
//Function:负责创建、销毁、管理会话实例
//Author:Joe
//CreateTime: 2004.6.17
#include "../Module/MulTiThMap.h"
class CGUID;
class CNetSession;

class CNetSessionManager
{
private:
	CNetSessionManager(void);
public:
	~CNetSessionManager(void);

public:

	CNetSession*	CreateSession(const CGUID& UserGuid);						                    //创建一个网络会话实例

	void OnDo(const CGUID& guid, const CGUID& UserGuid, long lPassWord,...);						//网络回调处理
	void OnSyncCallbackResult(const CGUID& guid, const CGUID& UserGuid, long lPassWord,...);		//网络回调结束触发处理

	void Run();

	static CNetSessionManager* getInstance();

	void	Release();								    //释放资源

private:

	static CNetSessionManager* instance;				//全局唯一的实例

	MulTiThMap<CGUID,CNetSession*>		m_Sessions;		//管理全局的会话实例
};

CNetSessionManager* GetNetSessionManager();
