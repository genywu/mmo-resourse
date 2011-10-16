


#include "SM.h"
#include "../Setup/SMSetup.h"

#pragma once



class SmManage
{
public:
	SmManage(VOID);
	~SmManage(VOID);

public:
	//!
	static	SmManage&		GetInstance(VOID);

public:
	//!						初始化对象
	BOOL					Init(VOID);
	//!						释放对象
	VOID					Release(VOID);


public:
	//!						找到一个SM
	SM*						FindSM(DWORD dwSMID);

	//!						响应连接上SM
	DWORD					OnConnect(LONG lSocket, LONG lIP);
	//!						响应登陆SM
	VOID					OnLogin(LONG lSocket, LPCSTR pPwd, DWORD VersionNo);
	//!						响应断开连接
	VOID					OnClose(DWORD dwSMID);

	//!						发送所有信息到客户端
	VOID					SendInfoToSocket(LONG lSocket);
	//!						输出所有信息
	VOID					ShowAllInfo(VOID);

public:
	
	//!						得到所有需要更新到最新版本的GAPP的socket和类型列表
	VOID					GetNeedToHighestGapp(vector<tagUpdateInfo> &vUpdateInfo);


private:
	//!						读取配置
	BOOL					LoadSetup(VOID);

	//!						新加一个SM
	SM*						AddNewSM(DWORD dwSMID, const tagSMSetup *pSMSetup);
	//!						删除一个SM
	VOID					DelSM(DWORD dwSMID);
	

private:
	//!						已经登陆的，正在运行的SM
	map<DWORD, SM*>			m_mapRunningSM;

private:
	//!
	static	SmManage		c_SmManage;
};