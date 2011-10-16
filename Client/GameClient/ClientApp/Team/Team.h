/**
* @filename:Team.h
* @date: 2010/6/7
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 

* @purpose: 玩家队伍的逻辑层
*/
#include "../../../Public/Module/GUID.h"
#include "../../../Public/Common/TeamDef.h"
class CPlayer;
///队伍的数据结构
class TeamData
{
public:
	/// 状态结构
	struct tagState
	{
		long lID;						 
		long lIconID;
		long lLevel;					//	新增状态等级
		DWORD dwLeftTime;
		DWORD dwAlpha;					// 颜色
		bool IsDeBuff;					// 状态标志，是否是DeBuff状态
		tagState()
		{
			lID = 0;
			lIconID = 0;
			lLevel = 0;
			dwLeftTime = 0;
			dwAlpha = 0xffffffff;
			IsDeBuff = false;
		}
	};
	/// 队伍结构
	struct tagTeam
	{
		std::string	strPlayerName;			/// 玩家名字
		long	lNameColor;				/// 玩家名字的字体颜色
		CGUID	PlayerID;				/// 玩家ID
		long	lIconID;				/// 职业图标
		long	lSex;					/// 性别
		DWORD	dwMaxHp;				/// 最大HP
		DWORD	dwMaxMp;				/// 最大MP
		DWORD	dwMaxRp;				/// 最大Rp
		DWORD	dwHp;					/// HP
		DWORD	dwMp;					/// MP
		DWORD	dwRp;					/// Rp
		long	lLev;					/// 等级
		long	lRegionID;				/// 场景ID
		CGUID	RegionGuid;				/// 场景GUID
		float	fPosX;					/// 坐标
		float	fPosY;
		bool	bIsOnline;				/// 是否在线标志
		vector<tagState>	m_vectorState;		/// Buff状态列表
		///构造函数
		tagTeam()
		{
			strPlayerName = "";
			lNameColor = 0xff000000;
			lIconID = -1;
			dwMaxHp = 0;						// HP
			dwMaxMp = 0;						// MP
			dwMaxRp = 0;						// Rp
			dwHp = 0;
			dwMp = 0;
			dwRp = 0;
			lLev = 0;
			lRegionID = 0;
			RegionGuid = CGUID::GUID_INVALID;
			fPosX = 0.0f;
			fPosY = 0.0f;
			bIsOnline = true;
		};
	};
public:
	///构造函数
	TeamData();
	///析构函数
	~TeamData();
	///通知界面更新数据显示
	void FireUIUpdate();	
	///清空数据
	void Clear();							
	/// 添加被邀请人名字
	void AddNameBySrc(std::string& strName);
	///
	void AddGUIDSrc(CGUID guid);
	///
	void UpdateHeadPic(tagTeam sTeam,long index);
	/// 设置队伍队长头像
	void SetTeamCapPic(bool bIsCap = false);								
	bool UpdateTeamList();													/// 更新状态列表
	bool RemoveTeamMember(const CGUID& MemberID);							/// 将指定的队友从队伍中删除

	void SetMemberInfo(const CGUID& MemberID, tagMemberIdioinfo sMemberinfo); /// 为指定的队员设定相关信息
	void UpdateMemberPos(const CGUID& MemberID, float fPosX,float fPosY);	/// 更新队友的坐标
	void UpdateMemberInfo(const CGUID& MemberID, long lType, long lValue);	/// 更新指定玩家的指定属性的值
	void SetMemberIsOnline(const CGUID& PlayerID, long lState);				/// 设置指定队友的在线状态
	void SetTeamAllotMode(int iAllotMode);									/// 按照给定的队伍物品分配方式，发送消息

	void AddMemberState(const CGUID& PlayerID, DWORD dwState, DWORD dwLev);	/// 为指定的队友添加状态
	void RemoveMemberState(const CGUID& PlayerID, DWORD dwState);			/// 为指定的队友删除某种状态

	bool ChangeCaptain(const CGUID& CapID);									/// 改变队伍的队长

	void MemberChangeServer(const CGUID& MemID);							/// 指定ID的队员夸服

	tagTeam * GetMemberInfo(const CGUID& MemID);							/// 获得指定玩家在队伍中保存的信息
	vector<tagTeam> & GetTeamInfo() { return m_vectorTeam; }				/// 获得玩家队伍信息

	void SetSelMember(uint index) { CurSelMemberIndex = index ; }			/// 设置当前选中的成员
	uint GetSelMember() { return CurSelMemberIndex;	}						/// 获得当前选中的成员
	const CGUID& GetSelMemberGUID();										/// 获得当前选中的成员GUID
	bool AddTeamMember(tagWSMemberInfo& sWSMemberInfo);						/// 根据队伍信息添加成员
private:
	
	vector<tagTeam>	m_vectorTeam;		/// team列表
	vector<CGUID> m_vecIDSrc;			/// 邀请人的GUID列表
	vector<std::string> m_vecSrcName;	/// 被推荐人的名字列表

	uint CurSelMemberIndex;				///单前被选中的成员索引
};

///队伍的消息管理
class TeamMsgMgr
{
public:
	TeamMsgMgr(){}
	~TeamMsgMgr(){}

public:
	///
	static void  SendInviteJoinTeam(CPlayer* first,CPlayer* second);
	///
	static void  SendLeaveTeam(CPlayer* first);
	///
	static void  SendLeaveTeam(const CGUID& guid);
	///
	static void  SendSelfLeaveTeam();
	///
	static void  SendInviteJoinPhase(CPlayer* first,CPlayer* second);
	///
	static void  SendAddLinkMan(CPlayer* first,CPlayer* second);
	///
	static void  SendFollowLeader(CPlayer* first);
	///
	static void  SendSetCaptain(const CGUID& guid);
};