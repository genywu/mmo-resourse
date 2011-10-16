/// 
/// @file ScriptOrganFunc.h
/// @brief Script内的公会操作函数
///

// class Script {

///
/// \brief 获取队伍的人数
///
long GetTeamNum();

///
/// \brief 获取队伍在一个地图中的人数
///
long GetTeamNumInRgn();

///
/// \brief 获取队友名字
/// \param pos 队友在队伍中的位置
///
const char* GetTeamerName(int pos);

///
/// \brief 获取队伍的GUID
///
CGUID GetTeamGUID();

///
/// \brief 通过玩家的GUID获取队伍的GUID
///
CGUID GetPlayerTeamGUID(const CGUID& playerGuid);

///
/// \brief 判定自己是否是队长
///
bool IsTeamCaptain();

///
/// \brief 获取队长在队伍中的位置
///
long GetTeamCaptainPos();

///
/// \brief 传送队伍（条件成立整队传送，否则都不传送）
///
long SetRegionForTeam(int type, int rgnType, const CGUID& regionGuid, long x=-1, long y=-1, long dir=-1, long range=-1);

///
/// \brief 传送队伍（在范围内的传送，范围外没有操作）
///
void SetTeamRegion(int rgnType, const CGUID& regionGuid, long x=-1, long y=-1, long radius=3, long dir=-1, long range=2);

///
/// \brief 判定队友是否在自己周围
/// \param type 0：radius参数有效，1：同场景，2：同服务器
/// \param radius 半径大小
///
bool IsTeammatesAroundMe(long type=0, long radius=3);

///
/// \brief 为一个队伍的玩家触发脚本
/// \param distance 距离限制，为0表示无距离限制
///
bool RunScriptForTeam(const char* scriptName, long distance, const CGUID& playerGuid=NULL_GUID);

///
/// \brief 创建帮会
///
bool CreateFaction();

///
/// \brief 打开申请加入帮会面板
///
void OpenApplyBoard();

///
/// \brief 打开帮会图标上传面板
///
void OpenFacIconBoard();

///
/// \brief 获取帮会等级
///
long GetFactionLevel();

///
/// \brief 设置帮会招募状态
/// \param inPut 0：关，非0：开
///
bool SetFactionRecruitState(long inPut);

///
/// \brief 通过玩家的名字获取帮会的GUID
///
CGUID GetFactionIDByPlayerName(const char* playerName=NULL);

///
/// \brief 通过玩家的名字获取联盟的GUID
///
CGUID GetUinionIDByPlayerName(const char* playerName=NULL);

///
/// \brief 当前玩家是否是帮会会长
///
bool IsFacMaster();

///
/// \brief 获取某个据点的门票的数量
///
long GetFacTickets(const CGUID& factionGuid, const char* footholdName);

///
/// \brief 获取帮会或帮会成员的战功值
/// \param flag 1：玩家的GUID，2：帮会的GUID
///
long GetBattleVal(int flag, const CGUID& guid);

///
/// \brief 获取帮会或帮会成员的投资值
/// \param flag 1：玩家的GUID，2：帮会的GUID
///
long GetArmyInvest(int flag, const CGUID& guid);

///
/// \brief 判定某个帮会成员是否有参与权
///
bool GetJoinBattle(const CGUID& playerGuid);

///
/// \brief 获取某个科技的等级
///
long GetTechLv(const CGUID& factionGuid, long techNo);

///
/// \brief 获取帮会的名字
///
const char* GetFactionName(const CGUID& factionGuid);

///
/// \brief 获取某个据点的战功值
///
long GetFacBaseBattle(const CGUID& factionGuid, const char* footholdName);

///
/// \brief 发送已更新的据点信息数据到客户端
///
void SendBaseInfo(const CGUID& factionGuid);

///
/// \brief 获取帮会的某个项目的等级
/// \param type 0：帮会等级，1：福利等级，2：专业等级，3：战争等级，4：商业等级，5：贵族等级
///
long GetFacLevel(ulong type, const CGUID& playerGuid=NULL_GUID);

///
/// \brief 获取某个帮会成员的贡献值
///
long GetMemContribute(const CGUID& playerGuid=NULL_GUID);

///
/// \brief 创建帮会界面
///
void OpenCreateFactionUi();


// };