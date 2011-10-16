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

#include "../public/OrganizingDef.h"
#include "..\..\..\public\WSUseMemManage.h"
class CPlayer;

class COrganizing:public BaseMemObj
{
public:
	COrganizing(void);
	virtual ~COrganizing(void);

	//提供给派生类继承使用的公共借口
public:
	virtual const CGUID&	GetExID(void) = 0;														//! 得到组织ID
	virtual const CGUID&	SetExID(const CGUID& guid)  = 0;										//! 设置组织ID
	virtual const char*		GetName(void)const = 0;													//! 得到组织的名字
	virtual void			SetName(const char* pName) = 0;											//! 设置帮会的名字

	virtual BOOL ApplyForJoin(const CGUID& PlayerGuid) = 0;											//! 申请加入
	virtual BOOL ClearApplyList(void) = 0;															//! 清除申请列表
	virtual BOOL AddMeber(CPlayer *pPlayer, const CGUID &InviterGuid = NULL_GUID) = 0;				//! 添加成员
	virtual BOOL Exit(const CGUID& PlayerGuid) = 0;													//! 退出
	virtual BOOL FireOut(const CGUID& FireID,const CGUID& BeFiredID) = 0;							//! 开除
	virtual BOOL Demise(const CGUID& MasterID,const CGUID& ID) = 0;									//! 传位
	virtual BOOL DubAndSetJobLvl(const CGUID& DubID,const CGUID& BeDubedID,string& strTitle) = 0;	//! 授予称号
	virtual const CGUID& GetMasterID(void) = 0;														//! 得到领导者ID
	virtual const CGUID& IsMember(const CGUID& guid) = 0;											//! 成员判定
	virtual LONG GetMemberNum(void) = 0;															//! 得到成员数

	

	virtual BOOL Disband(const CGUID& guid) = 0;													//! 解散
	virtual BYTE GetCountry(void) = 0;																//! 得到国家ID
	virtual void SetCountry(BYTE c) = 0;															//! 设置国家ID
	
	virtual LONG GetEstablishedTime(void)const = 0 ;												//! 得到该建立时间

	virtual BOOL Pronounce(CPlayer *pPlayer, const char *pPro) = 0;									//! 宣告
	virtual void Talk(const CGUID& PlayerGuid, const char *pSourceName, const char *pTxt) = 0;		//! 组织聊天

	virtual BOOL Initial(void) = 0;																	//! 初始化
};
