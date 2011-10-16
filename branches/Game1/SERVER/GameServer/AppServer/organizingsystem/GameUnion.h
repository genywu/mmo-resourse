



#include "GameFaction.h"

#pragma once

class GameUnion : public BaseMemObj
{
public:
	//! 
	GameUnion(void);
	//! 
	~GameUnion(void);

	//!	基本功能
	///////////////////////////////////////////////////////////////////////////////////////
public:
	//!										编码
	BOOL									AddToByteArray(vector<BYTE>& ByteArray);
	//!										解码
	BOOL									Decode(BYTE* pData, LONG &lPos);

	//!										编码基本信息到消息末尾
	VOID									AddBaseToMsg(CMessage *pMsg);
	//!										编码成员信息到消息末尾
	BOOL									AddOneMemberToMsg(const CGUID &FactionGuid, CMessage *pMsg);
	BOOL									AddOneMemberToMsg(CPlayer *pPlayer, CMessage *pMsg);
	//!										编码成员信息到消息末尾
	VOID									AddAllMemberToMsg(CMessage *pMsg);

public:
	//!										得到组织ID
	const	CGUID&							GetExID(void)const;
	//!										得到组织的名字
	const	char*							GetName(void)const;
	//!										得到盟主ID
	const	CGUID&							GetMasterID(void)const;
	//!										得到盟主的名字
	const	char*							GetMasterName(void)const;

	//!										添加新成员
	BOOL									AddNewMember(GameFaction *pMember);
	//!										删除成员
	BOOL									DelMember(const CGUID& MemberGuid);
	//!										判断成员
	BOOL									IsMember(const CGUID& FactionGuid);
	//!										修改盟主
	BOOL									ChangeMaster(const CGUID& NewMasterFacGuid, const char *pNewMasterName);
	//!										设置成员权限
	BOOL									SetPurview(const CGUID& MemberGuid, DWORD dwNewPurview);
	//!										得到成员权限
	DWORD									GetPurview(const CGUID& MemberGuid);
	//!										解散
	BOOL									Disband(VOID);

	//!										响应联盟建立
	VOID									OnCreate(VOID);


private:
	//!										设置组织ID
	const	CGUID&							SetExID(const CGUID& guid);
	//!										设置帮会的名字
	void									SetName(const char* pName);
	//!										设置盟主ID
	const	CGUID&							SetMasterID(const CGUID& guid);
	//!										设置盟主的名字
	void									SetMasterName(const char* pName);


	//!	For DBManager
	///////////////////////////////////////////////////////////////////////////////////////
public:
	//!										得到基本信息
	const	tagUnionBaseData&				GetUnionBaseData(VOID);
	//!										得到成员列表
	const	map<CGUID, tagUnionMemInfo>&	GetUnionMemInfoMap(VOID);



	//!	
	///////////////////////////////////////////////////////////////////////////////////////
private:
	//!										基本资料
	tagUnionBaseData						m_UnionBaseData;
	//!										成员列表
	map<CGUID, tagUnionMemInfo>				m_mapUnionMemInfo;
};
