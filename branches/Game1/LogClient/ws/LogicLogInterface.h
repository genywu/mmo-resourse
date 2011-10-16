

#pragma once


class LogClient;
class WSTeam;
class CPlayer;
class CFaction;
struct tagFacApplyPerson;
struct tagFacMemInfo;

//! 逻辑日志接口封装类
//! 将日志参数封装为有逻辑意义的对象指针
class LogicLogInterface
{
public:
	LogicLogInterface(LogClient *pLogClient);


//! 日志记录逻辑接口申明
//! 具体的接口定义由逻辑模块自己完成，不要定义在LogicLogInterface.cpp中
///////////////////////////////////////////////////////////////////////////
public:
	/* --------------------------------------------------------------------
	日志配置：
	<!--队伍组建-->
	<Table name="t340_team_create" log_no="34">
		<item type="date"     head_name=""></item>
		<item type="object"   head_name="team"></item>
		<item type="player"   head_name="sponsor"></item>
		<item type="player"   head_name="participator"></item>
	</Table>
	*/
	//! 记录队伍组建日志
	//! 接口定义在：WSTeam.cpp
	bool LogT340_team_create(WSTeam *pTeam, CPlayer *pSponsor, CPlayer *pParticipator);


	/* --------------------------------------------------------------------
	日志配置：
	<!--队伍解散-->
	<Table name="t370_team_disband" log_no="37">
		<item type="date"     head_name=""></item>
		<item type="object"   head_name="team"></item>
	</Table>
	*/
	//! 记录队伍解散日志
	//! 接口定义在：WSTeam.cpp
	bool LogT370_team_disband(WSTeam *pTeam);

	/* --------------------------------------------------------------------
	<!--帮会创建/解散日志-->
		<Table name="t150_faction_log" log_no="150" log_pos="1">
		<item type="client_head"     head_name=""></item>
		<item type="player"   head_name=""></item>
		<item type="faction"  head_name=""></item>
		<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：创建；1解散）-->
	</Table>
	*/
	bool LogT150_faction_log(LPCSTR pOprName, const CGUID &pOprGUID, CFaction *pFaction, LONG lAct);

	
	/* --------------------------------------------------------------------
	<!--申请列表加入/退出日志-->
	<Table name="t160_faction_join_leave_log" log_no="160" log_pos="1">
		<item type="client_head"     head_name=""></item>
		<item type="player"   head_name=""></item>
		<item type="faction"  head_name=""></item>
		<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：加入；1：删除）-->
	</Table>
	*/
	bool LogT160_faction_join_leave_log(const char *pOpterName, const CGUID *pOpterGuid, CFaction *pFaction, LONG lAct);


	/* --------------------------------------------------------------------
	<!--成员进入/离开日志-->
	<Table name="t170_faction_agree_kick_log" log_no="170" log_pos="1">
		<item type="client_head"     head_name=""></item>
		<item type="player"   head_name=""></item>
		<item type="player"   head_name="aim"></item>
		<item type="faction"  head_name=""></item>
		<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：进入；1：离开）-->
	</Table>
	*/
	bool LogT170_faction_agree_kick_log(const char *pOpterName, const CGUID *pOpterGuid, const char *pAimName, const CGUID *pAimGuid, CFaction *pFaction, LONG lAct);


	/* --------------------------------------------------------------------
	<!--头衔变更-->
	<Table name="t230_faction_title_log" log_no="230" log_pos="1">
	<item type="client_head"     head_name=""></item>
	<item type="faction"  head_name=""></item>
	<item type="player"   head_name=""></item>
	<item type="alone"    Item_name="before_title"    data_type="int32"></item><!--之前头衔-->
	<item type="alone"    Item_name="curr_title"      data_type="int32"></item><!--当前头衔-->
	</Table>
	*/


	/* --------------------------------------------------------------------
	<!--权利变更-->
	<Table name="t240_faction_purview_log" log_no="240" log_pos="1">
	<item type="client_head"     head_name=""></item>
	<item type="faction"  head_name=""></item>
	<item type="player"   head_name=""></item>
	<item type="alone"    Item_name="before_purview"  data_type="int32"></item><!--之前权利-->
	<item type="alone"    Item_name="curr_purview"    data_type="int32"></item><!--当前权利-->
	</Table>
	*/


	/* --------------------------------------------------------------------
	<!--帮主变更-->
	<Table name="t250_faction_master_log" log_no="250" log_pos="1">
	<item type="client_head"     head_name=""></item>
	<item type="faction"  head_name=""></item>
	<item type="player"   head_name="old"></item>
	<item type="player"   head_name="new"></item>
	</Table>
	*/
	bool LogT250_faction_master_log(CFaction *pFaction, tagFacMemInfo *pOld, tagFacMemInfo *pNew);
			
	/*
	<!--================================================================-->
	<!--帮会经验变化-->
	<Table name="t260_faction_experience_log" log_no="260" log_pos="1">
		<item type="client_head"     head_name=""></item>
		<item type="faction"  head_name=""></item>
		<item type="alone"    Item_name="before_exp"      data_type="int32"></item><!--之前经验-->
		<item type="alone"    Item_name="curr_exp"        data_type="int32"></item><!--当前经验-->
	</Table>
	*/
	bool LogT260_faction_experience_log(CFaction *pFaction, long lBeforeExp);
	
	/*
	<!--================================================================-->
	<!--帮会等级变化-->
	<Table name="t270_faction_level_log" log_no="270" log_pos="1">
		<item type="client_head"     head_name=""></item>
		<item type="faction"  head_name=""></item>
		<item type="alone"    Item_name="before_level"    data_type="int32"></item><!--之前等级-->
		<item type="alone"    Item_name="curr_level"      data_type="int32"></item><!--当前等级-->
		<item type="alone"    Item_name="level_type"      data_type="int32"></item><!--等级类型 0=工会 1=福利 2=仓库 3=锻造 4炼金 5=缝纫-->
	</Table>
	*/
	bool LogT270_faction_level_log(CFaction *pFaction, long lCurrLevel, long lLvlType);

	/* --------------------------------------------------------------------
	*/

	/* --------------------------------------------------------------------
	<!--联系人操作日志-->
	<Table name="t450_linkman_opt_log" log_no="50" log_pos="1">
	<item type="client_head"     head_name=""></item>
	<item type="player"   head_name="opter"></item><!--操作者-->
	<item type="player"   head_name="aim"></item><!--操作目标-->
	<item type="alone"    Item_name="opt_type"      data_type="int32"></item><!--操作类型：1=添加、2=删除-->
	<item type="alone"    Item_name="group_id"      data_type="int32"></item><!--所操作的联系人组的ID-->
	</Table>
	*/
	bool logT450_linkman_opt_log(CPlayer *pOpter, const CGUID &AimGuid, const char *pAimName, long lOptType, long lGroupID);


	/* --------------------------------------------------------------------
	<!--客户端上下线日志-->
	<Table name="t550_client_in_game_log" log_no="550" log_pos="1">
		<item type="client_head"     head_name=""></item>
		<item type="alone"    Item_name="cdkey"         data_type="str128"></item><!--IP-->
		<item type="alone"    Item_name="ip"            data_type="int32"></item><!--CDKEY-->
		<item type="player"   head_name=""></item>
		<item type="pos"      head_name=""></item>
		<item type="alone"    Item_name="region_guid"   data_type="int32"></item><!--level-->
		<item type="alone"    Item_name="level"         data_type="int32"></item><!--level-->
		<item type="alone"    Item_name="gold"          data_type="int32"></item><!--gold-->
		<item type="alone"    Item_name="flag"          data_type="int32"></item><!--silver-->
		<item type="alone"    Item_name="flag"          data_type="int32"></item><!--操作类型：1=上线、2=下线-->
		<item type="alone"    Item_name="online_time"   data_type="int32"></item><!--此次操作在线分钟数，上线类型，此处为0，下线类型，此处有值-->
	</Table>
	*/
	bool logT550_client_in_game_log(CPlayer *pPlayer, LONG lState);

private:

	//!							得到当前时间字符串（00-00-00 00:00:00格式）
	void						GetCurrTimeString(string &strTime);


private:

	//!							日志客户端对象
	LogClient					*m_pLogClient;

};