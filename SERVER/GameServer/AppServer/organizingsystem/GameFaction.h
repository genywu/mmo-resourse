#pragma once
#include "..\public\OrganizingDef.h"
#include "S2CSession.h"
//Faction.h
//Function:提供帮会操作接口、完成帮会功能。
//Authou:Joe
//Create Time:2004.6.11

class GameFaction : public BaseMemObj
{
public:
	GameFaction(void);
	~GameFaction(void);

public:
	//!							编码
	BOOL						AddToByteArray(vector<BYTE>& ByteArray);
	//!							解码(pMapApplicant是外部统计申请者是否重复所用的map)
	BOOL						DecodeFromMsgSet(DBReadSet &DbReadSet, map<CGUID, CGUID> *pMapApplicant = NULL);

	//!							编码会徽数据到buf末尾
	void						AddIconData_ToArray(vector<BYTE> *pByteArray);
	//!							编码公共信息到消息末尾
	void						AddPublicData_ToMsgBack(CMessage *pMsg);
	void						AddPublicData_ToArray(vector<BYTE> *pByteArray);
	void						AddPublicData_ToDataBlock(DBWriteSet& setWriteDB);
	//!							编码基本资料到消息末尾
	void						AddBaseData_ToMsgBack(CMessage *pMsg);
	//!							编码成员资料到消息末尾
	void						AddMember_ToMsgBack(CMessage *pMsg, const CGUID &AimGUID);
	//!							编码成员资料
	void						AddMember_ToMsgBack(CMessage* pMsg);
	//!							编码一个成员资料到消息末尾
	void						AddOneMember_ToMsgBack(const CGUID &MemeberGuid, CMessage *pMsg, const CGUID &AimGUID);
	//!							编码申请者资料到消息末尾
	void						AddApply_ToMsgBack(CMessage *pMsg);
	//!							编码公告到消息末尾
	void						AddPronounce_ToMsgBack(CMessage *pMsg);
	//!							编码留言到消息末尾
	void						AddLeaveWord_ToMsgBack(CMessage *pMsg);
	//!							得到申请加入相关信息
	void						GetApplyInfo(tagApplyInfo &ApplyInfo);

	//!							更新一个成员的Player对象的工会信息
	void						UpdateMemberFacInfo(map<CGUID, tagFacMemInfo>::const_iterator &iteMemInfo);
	void						UpdateMemberFacInfo(CPlayer *pPlayer);

	//!							响应加入联盟
	void						OnJoinUnion(GameUnion *pUnion);
	//!							响应退出联盟
	void						OnQuitUnion(void);
	//!							响应联盟解散
	void						OnDisbandUnion(void);
	//!							为成员初始化联盟信息
	void						SetMemberUnionInfo(const CGUID &UnionGuid);
	//!							响应执行福利操作
	void						OnBoonRun(void);
	

	//!							得到组织ID
	const		CGUID&			GetExID(void);					
	//!							设置组织ID
	const		CGUID&			SetExID(const CGUID& guid) ;
	//!							得到组织的名字
	const		char*			GetName(void)const;				
	//!							设置帮会的名字
	void						SetName(const char* pName);
	//!							得到国家
	BYTE						GetCountry(void);

	//!							成员判定
	const		CGUID&			IsMember(const CGUID& MemberGuid);
	bool						IsMember( const std::string &name ) const;
	//!							申请者判定
	BOOL						HasApplicant(const CGUID& ApplicantGuid);
	//!							获得会长
	const		CGUID&			GetMaster(void);
	//!							得到当前行会等级
	LONG						GetLevel(DWORD dwUpgradeType);
	//!							得到当前行会等级
	BOOL						SetLevel(LONG lLvl, DWORD dwUpgradeType);
	//!							得到成员官阶
	LONG						GetMemberJobLvl(const CGUID& MemberGuid);
	//!							设置招募状态
	BOOL						SetRecruitState(BOOL bOpen);

	//!							判断当前能否解散
	eFactionOptErrInfo			CanDisban(void);
	//!							解散
	void						Disband(void);
	//!							检测成员权限
	BOOL						TestPurview(const CGUID& MemberGuid, eMemberPurview ePurview);
	//!							得到成员权限
	LONG						GetPurview(const CGUID& MemberGuid);
	//!							得到成员的数据
	const tagFacMemInfo*		GetMemberData(const CGUID& MemberGuid);
	//!							添加成员
	void						AddMember(tagFacMemInfo &FacMemInfo);
	//!							删除成员
	void						DelMember(const CGUID& MemberGuid);
	//!							添加一个申请者
	BOOL						AddApplicant(tagFacApplyPerson &FacApplyPerson);
	//!							删除一个申请者
	BOOL						DelApplicant(const CGUID& ApplicantGuid);
	//!							设置一个职级的名称
	BOOL						SetJobName(LONG lJobLvl, const char *pNewName);
	//!							设置一个职级的权限
	BOOL						SetJobPurview(LONG lJobLvl, LONG lPurview);
	//!							设置公告
	BOOL						SetPronounce(tagOrgPronounceWord &OrgPronounceWord);
	//!							设置备忘
	BOOL						SetLeaveword(tagOrgPronounceWord &OrgPronounceWord);
	//!							设置成员的职级
	BOOL						SetMemJobLvl(const CGUID& MemberGuid, LONG lJobLvl);
	//! 						设置成员的称号
	BOOL						SetMemberTitle(const CGUID& MemberGuid, const char *pNewTitle);
	//! 						得到成员的称号
	const char*					GetMemberTitle(const CGUID& MemberGuid);
	//!							设置成员地图
	BOOL						SetMemberMap(const CGUID& MemberGuid, const char *pMapName);
	//!							设置成员等级
	BOOL						SetMemberLevel(const CGUID& MemberGuid, LONG lLevel);
	//!							设置成员隐匿
	BOOL						SetMemberHide(const CGUID& MemberGuid, BYTE cHide);
	//!							得到成员等级
	LONG						GetMemberLevel(const CGUID& MemberGuid);
	//!							设置成员职业
	BOOL						SetMemberOccu(const CGUID& MemberGuid, LONG lOccu);
	//!
	BOOL						SetMemberName(const CGUID& MemberGuid, const char *pName);
	//!							设置新会长
	BOOL						SetNewMaster(tagFacMemInfo &NewMaster, tagFacMemInfo &OldMaster);
	//!							在线状态改变
	BOOL						MemberOnlineChange(const CGUID& MemberGuid, BOOL bOnline, LONG lLastOnlineTime);
	//!							等级改变
	BOOL						MemberLvlChange(const CGUID& MemberGuid, LONG lLvl);
	//!							获得工会公告
	const tagOrgPronounceWord&	GetPronounceWord(void);

	//!							设置会徽
	BOOL						SetIcon(BYTE* IconData, long size);
	//!							得到会徽
	void						GetIcon(vector<BYTE> &vIconData);
	//!
	DWORD						GetIconCRC32(VOID){return m_dwCRC32IconCode;}

public:
	//!							发送消息给所有成员
	void						SendToAll(CMessage &msg);

	//!							添加工会信息关注者，系统会对关注者的工会信息保持即时更新
	void						AddAttention(const CGUID &MemberGuid);
	//!							删除工会信息关注者
	void						DelAttention(const CGUID &MemberGuid);
	//!							发送消息给所有关注工会信息的成员
	void						SendToAllConcern(CMessage &msg);

	//!							得到当前资源数
	LONG						GetRes(VOID);
	//!							得到当前经验数
	LONG						GetExp(VOID);
	//!							得到当前成员贡献数
	LONG						GetMemberContribute(const CGUID &MemberGuid);

	//!							设置资源的值
	VOID						SetRes(LONG lRes);
	//!							设置经验
	VOID						SetExp(LONG lExp);
	//!							设置成员贡献
	VOID						SetMemberContribute(const CGUID &MemberGuid, LONG lContribute);
	//!							获取据点门票数
	LONG						GetTickets(char* cBaseName);
	//!							获取成员参与权
	BOOL						GetJoin(const CGUID& guid);
	//!							获取战功值
	LONG						GetBattle(CGUID guid=NULL_GUID);
	//!							获取投资值
	LONG						GetArmyInvest(CGUID guid=NULL_GUID);
	//!							改变战功值
	void						AddBattle(LONG lValue,CGUID PlayerGuid);
	//!							改变投资值
	void						AddArmyInvest(LONG lVlue,CGUID PlayerGuid);
	//!							设置成员参与权
	void						SetJoin(CGUID& PlayerGuid,LONG lFlags);
	//!							设置据点门票数
	void						SetTickets(char* cBaseName,long lNum);
	//!							设置据点战功
	void						SetBaseBattle(char* cBaseName,long lNum);
	//!							获取据点战功
	LONG						GetBaseBattle(char* cBaseName);
	//!							添加科技
	void						AddTechLv(LONG lTechNo,LONG Lv){m_TechLv[lTechNo]=Lv;}	
	//!							获取科技等级
	LONG						GetTechLv(LONG lTechNo){return m_TechLv.find(lTechNo)!=m_TechLv.end()?m_TechLv[lTechNo]:0;}
	typedef						map<CGUID, tagFacMemInfo>::iterator MEMBERITER;
	//!							编码家族敌对信息到客户端
	BOOL						AddEnemyInfoToMsg(CMessage* pMsg);
	//!							编码门票记录到客户端
	BOOL						AddBaseInfoToMsg(CMessage* pMsg);
	//!							修改敌对家族信息
	BOOL						ChangeEnemy(eBattleType eType,char* cName,DWORD dwTime,DWORD dwEndTime,LONG lFlags=0);
	//!							修改一个据点的信息
	BOOL						ChangeBaseInfo(char* cName);
	//!							发送据点信息到客户端
	void						SendBaseInfo();
	//!							获取已占领的据点
	LONG						GetBaseNum();
	//!							向玩家发送公会信息
	void						FacDisplay(const CGUID& guid,long lValue1,long lValue2);
	//!							清空据点
	void						ClearBase();
	//帮会属性
private:
	//!									公共信息
	tagFacPublicData					m_FacPublicData;
	//!									基本信息数据
	tagFacBaseData						m_FacBaseData;
	//!									自定义会徽
	vector<BYTE>						m_vIconData;
	//!									会徽验证码
	DWORD								m_dwCRC32IconCode;

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

	//!									打开了行会界面的成员
	S2CSession<CGUID>					m_OptingMember;
	//!									拥有各个据点的信息
	map<string,tagBaseInfo>				m_mapBaseInfo;
	//!									家族科技等级
	map<LONG,LONG>						m_TechLv; 
	//!									家族战争信息
	map<eBattleType,list<tagEnemy> >	m_Enemy;
};
