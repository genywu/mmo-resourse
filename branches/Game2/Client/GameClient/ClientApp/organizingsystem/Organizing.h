//Function:	该类是一个基类，提供给行会系统。也是一个接口类,
			//提供了对组织的基本操作，每个派生类必需重载这些操作。
			//例如，创建、邀请成员、解散、开除等
//Author:	Joe
//Create Time:2004.6.11

#pragma once
#include "../../../Public/Common/OrganizingDef.h"
class COrganizing
{
public:
	COrganizing(void);
	virtual ~COrganizing(void);

	virtual bool Save() = 0;											//负责保存数据
	virtual bool Load() = 0;											//负责装载数据
public:

	//提供给派生类继承使用的公共借口
public:
	virtual bool ApplyForJoin(CGUID& PlayerID,long lLvl,long lOrgMemID = 0) = 0;		//无行会成员申请加入
	virtual bool DoJoin(const CGUID& ExecuteID,const CGUID& BeExecutedID,bool lAgree,tagTime& Time) = 0;		//处理要加入组织的人员
	virtual bool Invite(const CGUID& InvitedID, const CGUID& BeInvitedID) = 0;					//邀请人员加入
	virtual bool Exit(const CGUID& PlayerID) = 0;											//退出
	virtual bool FireOut(const CGUID& FireID,const CGUID& BeFiredID) = 0;							//开除
	virtual bool DubAndSetJobLvl(const CGUID& DubID,const CGUID& BeDubedID,string& strTitle,long lLvl) = 0;//授予称号和设置职级
	virtual bool EndueRightToMember(const CGUID& EndueID,const CGUID& BeEnduedID,eFactionPurviewIndex ePV) = 0;//赋予成员权利
	virtual bool AbolishRightToMember(const CGUID& EndueID,const CGUID& BeEnduedID,eFactionPurviewIndex ePV) = 0;//取消成员某个权利
	virtual bool Pronounce(const CGUID& PlayerID,string& strPro,tagTime& Time) = 0;		//宣告
	virtual bool LeaveWord(const CGUID& PlayerID,string& strWords,tagTime& Time) = 0;		//留言
	virtual bool EditLeaveWord(const CGUID& PlayerID,long lID,eOperator eOP)=0;			//编辑留言
	virtual bool Disband(const CGUID& ID) = 0;												//解散
	virtual bool Demise(const CGUID& MasterID,const CGUID& ID) = 0;								//传位
	virtual bool OperatorTax(const CGUID& PlayerID) = 0;									//操作税金

	virtual const CGUID& GetID() = 0;														//得到组织的ID
	virtual const CGUID& GetMasterID() = 0;													//得到掌门的ID
	virtual long IsOwnedCity(long lCityID) = 0;										//该帮是否拥有主城
	virtual	void AddOwnedCity(long lCityID)= 0;										//给该行会添加主城ID
	virtual	bool DelOwnedCity(long lCityID)= 0;										//删除一个拥有的主城ID
	virtual bool IsHaveEnymyFaction() = 0;											//是否有敌对行会存在
	virtual list<CGUID> GetFriendList() = 0;											//得到好友组织列表
	virtual	void AddEnemyOrganizing(list<long>& TemptList) = 0;						//添加一个敌对组织列表
	virtual	void DelEnemyOrganizing(list<long>& TemptList) = 0;						//删除一个敌对组织列表
	virtual	void AddEnemyOrganizing(const CGUID& FactionID) = 0;							//添加一个敌对行会
	virtual	void DelEnemyOrganizing(const CGUID& FactionID) = 0;							//删除一个敌对行会
	virtual	void ClearEnemyFation() = 0;											//清除敌对行会列表
	virtual long IsMaster(const CGUID& ID) = 0;											//该玩家是帮主否
	virtual long IsMember(const CGUID& ID) = 0;											//该玩家是否是该帮成员否
	virtual long IsSuperiorOrganizing() = 0;										//判断该组织是否有上级组织
	virtual void SetSuperiorOrganizing(const CGUID& ID) = 0;								//设置上级组织
	virtual	long RemoveApplyMember(const CGUID& ID) = 0;									//移出一个申请成员
	virtual const tagTime* GetEstablishedTime() const = 0;							//得到该行会建立时间
	//virtual void SetLogo(long lLogoID) = 0;											//设置徽标
	//virtual long GetLogo() = 0;														//得到徽标
	virtual bool IsLWFunction() = 0;												//是否具有留言功能
	virtual void SetLWFunction(bool b) = 0;											//设置留言功能
	virtual bool GetIsPermit()=0;													//得到是否允许同盟、宣战、投降、议和
	virtual void SetIsPermit(long lPlayer,bool b)=0;								//设置是否允许同盟、宣战、投降、议和

	virtual bool IsControbute(const CGUID& MemID) = 0;										//得到是否是贡献者
	virtual void SetControbuter(const CGUID& MemID,bool bControbute) = 0;					//设置贡献者

	virtual void AddMember(tagFacMemInfo& Memnfo)=0;									//添加一个成员
	virtual bool DelMember(const CGUID& ID)=0;												//从变量中删除一个成员

	virtual long GetMemberNum() = 0;												//得到成员数量

protected:
	virtual bool Initial() = 0;														//初始化
	virtual bool IsUsingPV(const CGUID& ID,eFactionPurviewIndex ePV) = 0;								//判断某ID是否能使用此权限
	virtual void SetMemPV(const CGUID& ID,eFactionPurviewIndex ePV) = 0;								//给某ID设置一个权限
	virtual void AbolishMemPV(const CGUID& ID,eFactionPurviewIndex ePV) = 0;							//取消某个成员的权限
	virtual bool SetName(string& strName) = 0;										//设置组织的名字
	virtual bool CheckOperValidate(const CGUID& ExecuteID,const CGUID& BeExecutedID,eFactionPurviewIndex ePV)=0;//检查某ID执行某个权限操作的合法性
	virtual bool CheckOperValidate(const CGUID& ExecuteID,eFactionPurviewIndex ePV)=0;					//检查某ID执行某个权限操作的合法性
	
};
