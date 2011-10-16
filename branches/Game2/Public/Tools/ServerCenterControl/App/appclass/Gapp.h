

#include "../../../../SCMPublic/class/CGameInfo.h"
#include "DbOperator.h"

#pragma once

const DWORD MAX_GAME_INFO_NUM = 8L;

class Gapp : public tagGappBaseInfo
{
public:
	static		Gapp*				CreateGapp(tagGappBaseInfo &GappBaseInfo, DWORD dwSMID);
	static		VOID				ReleaseGapp(Gapp **ppGapp);

public:
	//!								更新基本信息
	VOID							UpdateInfo(tagGappBaseInfo &GappBaseInfo){memcpy(this, &GappBaseInfo, sizeof(tagGappBaseInfo));}
	//!
	DWORD							GetID(VOID) {return _dwGappID;}
	//!
	const char*						GetVersion(VOID){return _szGappVersion;}
	VOID							SetVersion(LPCTSTR pNewVersion);
	//!
	DWORD							GetType(VOID){return _dwGappType;}

	//!								更新状态
	BOOL							ChangeRunState(DWORD dwNewState);
	//!								更新echo延迟时间
	VOID							ChangeDelayTime(DWORD dwNewDelayTime);

	//!								发送自身信息到客户端
	VOID							SendInfoToSocket(LONG lSocket);
	//!								发送自身信息到所有客户端
	VOID							SendInfoToAll(VOID);

	//!								输出所有信息
	VOID							ShowAllInfo(VOID);

	//!								进入、离开控制台
	BOOL							InCtrlRoom(LONG lUserID, BOOL bIn);
	//!								得到操作者ID
	LONG							GetRoomUser(VOID){return m_lRoomUser;}

public:
	//!								添加一条运行信息
	VOID							AddGameInfo(tagDbGameInfo &GameInfoData);
	//!								响应DB操作完成
	static		VOID				OnDBWriteFinish(tagDbGameInfo *pGameInfoData, DWORD dwNum, BOOL bExecuteOK);

	//!								查找一段时间内的运行信息给用户
	VOID							FindGameInfoToUser(DWORD dwInfoType, DWORD dwBeginTime, DWORD dwEndTime, LONG lUserID);

private:
	//!								将现有信息写入DB
	VOID							WriteGameInfoToDB(VOID);

private:
	DWORD							m_dwSMID;
	LONG							m_lRoomUser;

	//!								游戏运行信息数目
	DWORD							m_dwNum;
	//!								游戏运行信息
	tagDbGameInfo					*m_pDbGameInfo;
	

private:
	Gapp(tagGappBaseInfo &GappBaseInfo, DWORD dwSMID);
	~Gapp(VOID);
};