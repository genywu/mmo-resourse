//Function:	该类是一个基类，提供给帮会系统。也是一个接口类,
			//提供了对组织的基本操作，每个派生类必需重载这些操作。
			//例如，创建、邀请成员、解散、开除等
//Author:	Joe
//Create Time:2004.6.11

#pragma once
#include<list>
#include<map>
#include<vector>
#include <deque>
#include <queue>
#include <set>
using namespace std;

#include "..\public\OrganizingDef.h"


class COrganizing
{
public:
	COrganizing(void);
	virtual ~COrganizing(void);

	//提供给派生类继承使用的公共借口
public:
	virtual BOOL AddToByteArray(vector<BYTE>& ByteArray);											//! 编码
	virtual BOOL Decode(BYTE* pData, LONG &lPos);													//! 解码

	virtual BOOL ApplyForJoin(const CGUID& PlayerGuid) = 0;											//! 申请加入
	virtual BOOL ClearApplyList(void) = 0;															//! 清除申请列表
	virtual BOOL DoJoin(const CGUID& ExecuteGuid, const CGUID& BeExecutedGuid,LONG lAgree) = 0;		//! 处理一个申请成员
	virtual BOOL Invite(const CGUID& InvitedGuid, const CGUID& BeInvitedGuid) = 0;					//! 邀请加入
	virtual BOOL Exit(const CGUID& PlayerGuid) = 0;													//! 退出
	virtual BOOL FireOut(const CGUID& FireID,const CGUID& BeFiredID) = 0;							//! 开除
	virtual BOOL Demise(const CGUID& MasterID,const CGUID& ID) = 0;									//! 传位
	virtual BOOL DubAndSetJobLvl(const CGUID& DubID,const CGUID& BeDubedID,string& strTitle) = 0;	//! 授予称号
	virtual const CGUID& GetMasterID() = 0;															//! 得到领导者ID
	virtual const CGUID& IsMember(const CGUID& guid) = 0;											//! 成员判定
	virtual LONG GetMemberNum() = 0;																//! 得到成员数

	const CGUID&	GetExID();																		//! 得到组织ID
	const CGUID&	SetExID(const CGUID& guid) ;													//! 设置组织ID
	const char*		GetName()const;																	//! 得到组织的名字
	void			SetName(const char* pName);														//! 设置帮会的名字

	virtual BOOL Disband(const CGUID& guid) = 0;													//! 解散
	virtual BOOL Upgrade(const CGUID& PlayerGuid) = 0;												//! 升级
	virtual LONG GetLvl() = 0;																		//! 得到等级
	virtual LONG SetLvl(LONG lLvl) = 0;																//! 设置等级
	virtual BYTE GetCountry() = 0;																	//! 得到国家ID
	virtual void SetCountry(BYTE c) = 0;															//! 设置国家ID
	virtual const CGUID&  GetSuperior(void) = 0;													//! 得到上级
	virtual void  SetSuperior(const CGUID&) = 0;													//! 设置上级

	virtual LONG GetEstablishedTime()const = 0 ;													//! 得到该建立时间

	virtual BOOL Pronounce(const CGUID& PlayerGuid, const char *pPro) = 0;							//! 宣告
	virtual BOOL LeaveWord(const CGUID& PlayerGuid, const char *pWords) = 0;						//! 留言
	virtual BOOL EditLeaveWord(const CGUID& PlayerGuid,const CGUID& WordGuid,eOperator eOP) = 0;	//! 编辑留言
	virtual void Talk(const CGUID& PlayerGuid, const char *pSourceName, const char *pTxt) = 0;		//! 组织聊天

	virtual BOOL Initial() = 0;																		//! 初始化
private:
	CGUID m_guid;
	char  m_szName[16];

};