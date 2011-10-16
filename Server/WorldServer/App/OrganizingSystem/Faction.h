#pragma once
#include "organizing.h"
#include "OrganBuffSys.h"
//Faction.h
//Function:提供帮会操作接口、完成帮会功能。
//Authou:Joe
//Create Time:2004.6.11

class CMessage;
class CUnion;
class CEntityGroup;

class CFaction
	: public COrganizing 
{
public:
	//! 用于从DB数据构造以存在的对象
	CFaction(void);
	//! 用于游戏中玩家创建新的对象
	CFaction(const CGUID &FacGuid, const char *pName, CPlayer *pMaster,const char *pPro);

	~CFaction(void);

	//相关的操作
public:
	void Insert(void);
	void Save(CEntityGroup** ppTmpFaction);												//! 保存
	BOOL CodeToMsgSet(DBWriteSet &DbWriteSet);										//! 编码
	BOOL DecodeFromMsgSet(DBReadSet &DbReadSet);												//! 解码
	//! 继承的接口
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const CGUID&	GetExID(void);														//! 得到组织ID
	const CGUID&	SetExID(const CGUID& guid);											//! 设置组织ID
	const char*		GetName(void)const;													//! 得到组织的名字
	void			SetName(const char* pName);											//! 设置帮会的名字


	BOOL ApplyForJoin(const CGUID& PlayerGuid);											//! 申请加入
	BOOL ClearApplyList(void);															//! 清除申请列表
	BOOL AddMeber(CPlayer *pPlayer, const CGUID &InviterGuid = NULL_GUID);				//! 添加成员
	BOOL Exit(const CGUID& PlayerGuid);													//! 退出
	BOOL FireOut(const CGUID& FireID,const CGUID& BeFiredID);							//! 开除
	BOOL Demise(const CGUID& MasterID,const CGUID& ID);									//! 传位
	BOOL DubAndSetJobLvl(const CGUID& DubID,const CGUID& BeDubedID,string& strTitle);	//! 授予称号
	const CGUID& GetMasterID(void);														//! 得到领导者ID
	const CGUID& IsMember(const CGUID& guid);											//! 成员判定
	LONG GetMemberNum(void);															//! 得到成员数

	BOOL Disband(const CGUID& guid = NULL_GUID);										//! 解散

	BYTE GetCountry(void);																//! 得到国家ID
	void SetCountry(BYTE c);															//! 设置国家ID
	const CGUID&  GetSuperior(void);													//! 得到上级
	void  SetSuperior(CUnion *pUnion);													//! 设置上级
	LONG GetEstablishedTime(void) const;												//! 得到该建立时间

	BOOL Pronounce(CPlayer *pPlayer, const char *pPro);									//! 宣告
	BOOL Leaveword(CPlayer *pPlayer, const char *pPro);									//! 备忘
	void Talk(const CGUID& PlayerGuid, const char *pSourceName, const char *pTxt);		//! 组织聊天

	BOOL Initial(void);																	//! 初始化


	//!										得到领导者ID
	const char*								GetMasterName(void);													
	//!										得到公共信息
	tagFacPublicData&						GetPublicData(void){return m_FacPublicData;}
	//!										得到基本信息数据
	tagFacBaseData&							GetBaseData(void){return m_FacBaseData;}
	//!										设置会徽
	BOOL									SetIcon(BYTE* IconData, long size);
	//!										得到会徽
	void									GetIcon(vector<BYTE> &vIconData);
	//!										得到宣言
	tagOrgPronounceWord&					GetPronounceWord(void){return m_Pronounce;}
	//!										得到宣言
	tagOrgPronounceWord&					GetLeaveword(void){return m_LeaveWord;}

	//!										得到成员信息列表
	map<CGUID, tagFacMemInfo>&				GetMemberMap(void){return m_mapMemInfo;}
	//!										得到申请列表
	map<CGUID, tagFacApplyPerson>&			GetApplyMap(void){return m_mapFacApplyPerson;}

	//! 系统功能
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//!										发送对象到GS
	void									SendToGameServer(LONG lSocketID);

	//!										出发一个成员进入游戏
	void									OnMemberEnterGame(CPlayer *pPlayer);
	//!										触发一个成员离开游戏
	void									OnMemberExitGame(CPlayer *pPlayer);

	enum
	{
		Nothing,
		ImmediatelyDisband,
	};
	//!										执行工会验证操作
	DWORD									OnValidate(DWORD dwTime=0);



	//! 对象功能
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//!										重设成员数据
	VOID									ResetMember(VOID);
	//!										检查某ID执行某个操作的合法性
	BOOL									CheckOperValidate(const CGUID& ExecuteGuid,eMemberPurview eMP);				
	//!										设置一个职级的名称
	BOOL									SetJobName(LONG lJobLvl, const char *pNewName);
	//!										设置一个职级的权限
	BOOL									SetJobPurview(LONG lJobLvl, LONG lPurrview);
	//!										设置成员的职级
	BOOL									SetJobLvl(const CGUID& MemberGuid, LONG lJobLvl);
	//!										设置成员称号
	BOOL									SetMemberTitle(const CGUID& OptGuid, const CGUID& MemberGuid, const char *pNewTitle);
	//!										设置成员地图
	BOOL									MemberMapChange(const CGUID& MemberGuid, const char *pMapName);
	//!										设置成员等级
	BOOL									MemberLevelChange(const CGUID& MemberGuid, LONG lLevel);
	//!										设置成员隐匿状态
	BOOL									MemberHideChange(const CGUID& MemberGuid, BYTE cHide);
	//!										设置成员职业
	BOOL									MemberOccuChange(const CGUID& MemberGuid, LONG lOccu);
	//!										添加一个申请者
	BOOL									AddApplicant(const CGUID& ApplicantGuid);
	//!										找到一个申请者
	const	tagFacApplyPerson*				FindApplicant(const CGUID& ApplicantGuid);
	//!										删除一个申请者
	BOOL									DelApplicant(const CGUID& ApplicantGuid);
	//!										将申请者加入到一个外部传入的列表
	void									AddApplicantToMap(map<CGUID, CGUID> &mapaLLApplicant);

	//!										添加成员
	BOOL									AddMeber(const tagFacApplyPerson& FacApplyPerson, const CGUID &InviterGuid = NULL_GUID);

	//!										得到当前资源数
	LONG									GetRes(VOID);
	//!										得到当前经验数
	LONG									GetExp(VOID);
	//!										得到当前成员贡献数
	LONG									GetMemberContribute(const CGUID &MemberGuid);

	//!										增加资源（降低传负数）
	BOOL									AddRes(LONG lDeltaRes);
	//!										增加经验（降低传负数）
	BOOL									AddExp(LONG lDeltaExp);
	//!										增加成员贡献（降低传负数）
	BOOL									AddMemberContribute(const CGUID &MemberGuid, LONG lDeltaContribute);

	//!										得到等级
	LONG									GetLvl(DWORD dwUpgradeType);
	//!										设置等级
	BOOL									SetLvl(LONG lLvl, DWORD dwUpgradeType);
	//!										日常消耗
	DWORD									DailyConsume();
	//!										检查成员最后在线时间
	BOOL									CheckLastTime();
	//!										根据家族等级调整家族科技等级
	BOOL									ChangeTechLv();
	//!										添加某个据点的门票
	LONG									AddTickets(char* cBaseName,LONG lNum);
	//!										添加某所占战功值
	LONG									AddBaseBattle(char* cBaseName,LONG lNum);
	//!										修改战攻值
	LONG									AddBattle(long lValue,CGUID& PlayerGuid);	
	//!										修改军事投资值
	LONG									AddArmyInvest(long lValue,CGUID& PlayerGuid);
	//!										设置玩家参与权
	BYTE									SetJoin(CGUID& PlayerGuid,LONG lFlags);
	//!										增加一个科技等级
	BOOL									AddTechLv(LONG lTechNo,LONG Lv);									
	map<string,tagBaseInfo>&				GetBaseInfo(){return m_mapBaseInfo;}
	//										存储家族所占据点
	void									AddBaseInfoToByteArray(vector<BYTE>*pByte);
	void									DecodeBaseInfoFromByteArray(BYTE* pByte,long& pos);
	//										编码家庭科技
	void									AddTechLvToByteArray(vector<BYTE>* pByte);
	void									DecodeTechLvFromByteArray(BYTE* pBYTE,long& pos);
	//										增加或减少敌对家族战争信息,lFlags=0是增加
	BOOL									ChangeEnemy(eBattleType eType,char* cName,DWORD dwTime,LONG lFlags=0);
	//										修改一个据点的信息
	BOOL									ChangeBaseInfo(char* cName);
	//										获取已占领的据点数
	LONG									GetBaseNum();
	//										清空家族占领的据点
	void									ClearBase();
private:
	//!										添加一个新会员
	void									AddNewMember(CPlayer *pPlayer, BOOL bIsMaster = FALSE);
				
	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//帮会属性
private:
	//!									公共信息
	tagFacPublicData					m_FacPublicData;
	//!									基本信息数据
	tagFacBaseData						m_FacBaseData;
	//!									自定义会徽
	vector<BYTE>						m_vIconData;

	//!									成员信息列表
	map<CGUID, tagFacMemInfo>			m_mapMemInfo;
	//!									申请列表
	map<CGUID, tagFacApplyPerson>		m_mapFacApplyPerson;
	//!									留言列表
	//vector<tagOrgLeaveWord>				m_vFacLeaveWord;
	//!									宣告宣言
	tagOrgPronounceWord					m_Pronounce;
	//!									公共留言
	tagOrgPronounceWord					m_LeaveWord;
	//!									家族科技等级
	map<LONG,LONG>						m_TechLv;
	//!									拥有各个据点的信息
	map<string,tagBaseInfo>				m_mapBaseInfo;
	//!									家族战争信息
	map<eBattleType,list<tagEnemy>>		m_Enemy;
public:
	//!									数据变更标记
	DWORD								m_dwDataChanged;
	//!									解散计时
	DWORD								m_dwDisbandTime;
	//!									资源为负计时
	DWORD								m_dwNegativeTime;
	//!									减资源的时间
	DWORD								m_dwSubResTime;

};
