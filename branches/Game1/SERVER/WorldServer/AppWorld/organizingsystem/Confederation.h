#pragma once
#include "organizing.h"

class CConfederation :
	public COrganizing
{
public:
	CConfederation(long lID,long lMastterID,string& strName = string(""));
	~CConfederation(void);

	bool Save();											//负责保存数据
	bool Load();											//负责装载数据
	bool SetMembers(BYTE* pByte,long lSize);				//设置成员信息
	bool GetMembersData(vector<BYTE>& ByteArray);			//得到成员信息数据
	bool AddToByteArray();									//添加到CByteArray
public:
	
	//属性(下边所涉及的ID是帮会的ID)
private:
	long	m_lID;											//该同盟的全局唯一ID
	string	m_strName;										//同盟名字

	long	m_lMastterID;									//盟主帮会ID	
	map<long,tagMemInfo> m_Members;							//该同盟所有帮会及该帮会的权限和称号
	long				m_ApplyPerson;						//当前申请加入的帮派

	tagTime	m_EstablishedTime;								//该盟成立时间32

	static	long MAX_MemberNum;								//最大成员数
	//相关的功能操作
public:
	bool ApplyForJoin(long lPlayerID,long lLvl,long lOrgMemID = 0);		//无帮派成员申请加入
	bool ClearApplyList(long lPlayerID);								//清除申请人员列表
	bool DoJoin(long lExecuteID,long lBeExecutedID,tagTime& Time);		//处理要加入联盟的组织
	bool Invite(long lInvitedID, long lBeInvitedID);					//邀请人员加入
	bool Exit(long lPlayerID);											//退出
	bool FireOut(long lFireID,long lBeFiredID);							//开除
	bool DubAndSetJobLvl(long lDubID,long lBeDubedID,string& strTitle,long lLvl);//授予称号
	bool EndueRightToMember(long lEndueID,long lBeEnduedID,ePurview ePV);//赋予成员权利
	bool AbolishRightToMember(long lEndueID,long lBeEnduedID,ePurview ePV);//取消成员某个权利
	bool Pronounce(long lPlayerID,string& strPro,tagTime& Time);		//宣告
	bool LeaveWord(long lPlayerID,string& strWords,tagTime& Time);		//留言
	bool Disband(long lID);												//解散
	bool Demise(long lMasterID,long lID);								//传主之位
	bool OperatorTax(long lPlayerID);									//操作税金

	//属性操作接口
	long GetID(){return m_lID;}								//得到组织的ID
	long GetMasterID(){return m_lMastterID;}				//得到掌门的ID
	//该玩家是盟主否
	//返回值:不是盟主返回0，否则返回该同盟的ID
	long IsMaster(long lID)							
	{
		if(m_lMastterID == lID)	return m_lID;
		else return 0;
	}
	//该玩家是否是该同盟成员否
	//返回值:不是成员返回0，否则返回该同盟的ID
	long IsMember(long lID)
	{
		if(m_Members.find(lID) != m_Members.end())	return m_lID;
		else return 0;
	}
	//判断该组织是否有上级组织
	//返回值:0表示没有，否则表示有上级组织
	long IsSuperiorOrganizing()
	{
		return false;
	}
	//设置上级组织
	void SetSuperiorOrganizing(long lID)
	{
		return;
	}
	//得到该帮会建立时间
	const tagTime* GetEstablishedTime() const {	return &m_EstablishedTime; }
	bool IsLWFunction() {return false;}												//是否具有留言功能
	void SetLWFunction(bool b){}													//设置留言功能


	long IsOwnedCity(long lCityID);													//判断该该帮是否拥有主城
	void AddOwnedCity(long lCityID);												//设置该帮会的主城ID
	bool DelOwnedCity(long lCityID);												//删除一个拥有的主城ID

	//对敌对帮会列表作处理
	bool IsHaveEnymyFaction();														//是否有敌对帮会存在
	list<long> GetFriendList();														//得到好友组织列表
	list<long> GetEnemyList();														//得到敌对组织列表
	void AddEnemyOrganizing(list<long>& TemptList);									//添加一个敌对组织列表
	void DelEnemyOrganizing(list<long>& TemptList);									//删除一个敌对组织列表
	void AddEnemyOrganizing(long lFactionID);										//添加一个敌对帮会
	void DelEnemyOrganizing(long lFactionID);										//删除一个敌对帮会
	void ClearEnemyFation();														//清除敌对帮会列表

	long RemoveApplyMember(long lID);												//移出一个申请列表中的成员
	void SetLogo(long lLogoID){}													//设置徽标
	long GetLogo(){return 0;}														//得到徽标

protected:
	bool Initial();																	//初始化
	bool SetName(string& strName);													//设置帮会的名字
	bool IsUsingPV(long lID,ePurview ePV);											//判断某人是否能使用此权限
	void SetMemPV(long lID,ePurview ePV);											//给某ID设置一个权限
	void AbolishMemPV(long lID,ePurview ePV);										//取消某个成员的权限
	bool DelMember(long lID);														//从变量中删除一个成员
	bool CheckOperValidate(long lExecuteID,long lBeExecutedID,ePurview ePV);		//检查某ID执行某个权限操作的合法性
	bool CheckOperValidate(long lExecuteID,ePurview ePV);							//检查某ID执行某个权限操作的合法性
};
