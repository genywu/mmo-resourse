#pragma once
#include "LogClient.h"

class CBaseMessage;
class CMoveShape;
class CPlayer;
class CFaction;
class WSTeam;
class GSTeam;
class CGoods;
class CServerRegion;

//! 逻辑日志接口封装类
//! 将日志参数封装为有逻辑意义的对象指针
class LogicLogInterface
{
public:
	LogicLogInterface();
    ~LogicLogInterface(void);

    void InitLogClient(CBaseMessage *pMsg, LogClientInfo &logClientInfo);

    LogClient*  GetLogClient()  { return m_pLogClient; }


    // lType对应数据表ID...不包括日志记录时间
  //  bool SendLogToServer(long lType,...);

public:
    //<!--物品交易日志-->
    //<Table name="t010_goods_log_trade" log_no="10" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="player"   head_name="sponsor"></item>
    //<item type="player"   head_name="participator"></item>
    //<item type="goods"    head_name=""></item>
    //<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--交易类型（0：普通；1个人商店）-->
    //<item type="alone"    Item_name="goods_price"     data_type="int32"></item><!--物品价格-->
    //<item type="alone"    Item_name="goods_num"       data_type="int32"></item><!--物品数量-->
    //</Table>
    bool logT010_goods_log_trade(CPlayer* pPlayer1,CPlayer* pPlayer2,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,long lActType,long lPrice,long lNum,DWORD dwMoneyType);

    //<Table name="t020_goods_log_pickup" log_no="20" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name=""></item>
    //<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：拾取；1：丢弃）-->
    //<item type="alone"    Item_name="goods_num"       data_type="int32"></item><!--物品数量-->
    //</Table>
    bool logT020_goods_log_pickup(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,long lActType,long lNum);

    //<!--物品镶嵌日志-->
    //<Table name="t021_goods_log_enchase" log_no="21" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name=""></item>
    //<item type="alone"    Item_name="act_type"        data_type="int32"></item>
    //<!--动作类型（0：镶；1：拆）-->
    //<item type="alone"    Item_name="hole_pos"       data_type="int32"></item>
    //<!--孔位置-->
    //<item type="alone"    Item_name="card_id"       data_type="int32"></item>
    //<!--卡ID-->
    //<item type="alone"    Item_name="card_name"       data_type="str128"></item>
    //<!--是否使用影拓（0：没有；1：使用了）-->
    //<item type="alone"    Item_name="reserve_card"  data_type="int32"></item>
    //<!--卡名字-->
    //</Table>
    bool logT021_goods_log_enchase(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,long lActType,DWORD dwHolePos,
        DWORD dwCardId,const char* strCardName,DWORD dwReserveCard);

    //<!--物品消耗日志-->
    //<Table name="t022_goods_log_consume" log_no="22" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name=""></item>    
    //<item type="alone"    Item_name="num"  data_type="int32"></item>
    //<item type="alone"    Item_name="used_num" data_type="int32"></item>
    //<!--数量-->
    //<item type="alone"    Item_name="container_id"       data_type="int32"></item>
    //<!--container id-->
    //<item type="alone"    Item_name="container_pos"       data_type="int32"></item>
    //<!--container pos-->
    //</Table>
    bool logT022_goods_log_consume(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,DWORD dwNum,DWORD dwUsedNum,DWORD dwPlace,DWORD dwPos);

    //<!--物品修理日志-->
    //<Table name="t023_goods_log_repair" log_no="23" log_pos="1">
    //<item type="client_head"     head_name=""></item>    
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name=""></item>
    //<item type="alone" Item_name="act_type"        data_type="int32"></item>
    //</Table>
    bool logT023_goods_log_repair(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,int nActType);

    //<!--奖励领取日志-->
    //<Table name="t026_goods_log_bouns" log_no="26" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>   
    //<item type="goods"    head_name=""></item> 
    //<item type="alone"    Item_name="goods_num"       data_type="int32"></item><!--物品数量-->
    //<item type="alone"    Item_name="bouns_id"       data_type="int32"></item><!--奖励编号-->
    //</Table>
    bool logT026_goods_log_bouns(CPlayer* pPlayer, DWORD dwGoodsId, const CGUID& goodsGuid, const char* strGoodsName, int nGoodsNum,int nBounsID);

    //<!--与NPC的物品交易日志-->
    //<Table name="t030_goods_log_npc" log_no="30" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name=""></item>
    //<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：玩家买；1：玩家卖）-->
    //<item type="alone"    Item_name="goods_price"     data_type="int32"></item><!--物品价格-->
    //<item type="alone"    Item_name="goods_num"       data_type="int32"></item><!--物品数量-->
    //</Table>
    //*/
    bool logT030_goods_log_npc(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,long lActType,long lPrice,long lNum);
    ///*<!--与NPC通过位面精华交易日志-->
    //<Table name="t031_goods_wm_npc" log_no="31" log_pos="1">	
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name=""></item>  
    //<item type="alone"	  Item_name="Goods_Num"		  data_type="int32"></item><!--获取物品的数量-->
    //<item type="alone"	  Item_name="wm_Num"		  data_type="int32"></item><!--位面数量-->
    //</Table>*/
    bool logT031_goods_wm_npc(CPlayer* pPlayer,DWORD dwGoodsID,const CGUID& guid,LONG lGoodsNum,LONG lWmNum);
    //<!--物品消失日志-->
    //<Table name="t040_goods_log_disappear" log_no="40" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name=""></item>
    //<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--类型（0：损坏；1：升级消失）-->
    //</Table>
    //*/
    bool logT040_goods_log_disappear(CPlayer* pPlayer,CGoods* pGoods,long lActType);
    ///*
    //<!--物品掉落日志-->
    //<Table name="t050_goods_log_fall" log_no="50" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name=""></item>
    //</Table>
    //*/
    bool logT050_goods_log_fall(CPlayer* pPlayer,CGoods* pGoods);

    //<!--采集日志-->
    //<Table name="t055_collect" log_no="55" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name=""></item>
    //<item type="alone"    Item_name="opt_id"          data_type="guid"></item><!--操作ID-->
    //<item type="alone"    Item_name="goods_num"       data_type="int32"></item><!--物品数量-->
    //</Table>
    //*/
    bool logT055_collect( CPlayer* pPlayer,CGoods* pGoods,const CGUID& guid,int nAmount );

    //<!--制作日志-->
    //<Table name="t056_facture" log_no="56" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name=""></item>
    //<item type="alone"    Item_name="opt_id"          data_type="guid"></item><!--操作ID-->
    //<item type="alone"    Item_name="act_type"          data_type="int32"></item><!--操作类型,0：删除,1：添加-->
    //<item type="alone"    Item_name="goods_num"       data_type="int32"></item><!--物品数量-->
    //</Table>
    bool logT056_facture( CPlayer* pPlayer,CGoods* pGoods,const CGUID& guid,int nType, int nAmount );

    //<!--仓库操作日志-->
    //<Table name="t060_goods_log_depository" log_no="60" log_pos="1">
    //	<item type="client_head"     head_name=""></item>
    //	<item type="player"   head_name=""></item>
    //	<item type="goods"    head_name=""></item>
    //	<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--类型（0：存；1：取）-->
    //	<item type="alone"    Item_name="goods_num"       data_type="int32"></item><!--物品数量-->
    //</Table>
    bool logT060_goods_log_depository(CPlayer *pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,DWORD dwGoodsNum, long lActType);

    //<!--银行操作日志-->
    //<Table name="t070_goods_log_bank" log_no="70" log_pos="1">
    //	<item type="client_head"     head_name=""></item>
    //	<item type="player"   head_name=""></item>
    //	<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--类型（0：存；1：取）-->
    //	<item type="alone"    Item_name="number"          data_type="int32"></item><!--数量-->
    //</Table>
    bool logT070_goods_log_bank(CPlayer *pPlayer, long lActType, long lNum);

    //<!--玩家获得经验-->
    //<Table name="t080_player_getexp_log" log_no="80" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--获得目标类型（0：怪物；1：NPC：3：家族福利）-->
    //<item type="alone"    Item_name="delta_exp"       data_type="int32"></item><!--变化的经验值-->
    //<item type="alone"    Item_name="delta_expMul"    data_type="int32"></item><!--变化的经验倍数-->
    //<item type="alone"    Item_name="old_exp"         data_type="int32"></item><!--变化前的经验值-->
    //<item type="alone"    Item_name="old_expMul"	  data_type="int32"></item><!--变化前的经验倍数-->
    //<item type="alone"    Item_name="new_exp"         data_type="int32"></item><!--变化后的经验值-->
    //<item type="alone"    Item_name="new_expMul"	  data_type="int32"></item><!--变化后的经验倍数-->
    //<item type="alone"    Item_name="src_depict"      data_type="str128"></item><!--经验来源的说明-->
    //</Table>
    //*/
    bool logT080_player_getexp_log(CPlayer *pPlayer, DWORD dwType, LONG lChangeExp, LONG lChangeExpMul, LONG lOldExp, LONG lOldExpMul, LONG lNewExp, LONG lNewExpMul, const char *pSrcDepict);

    ///*<!--玩家获得经验-->
    //<Table name="t081_player_GetOccuExp_log" log_no="81" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--获得目标类型（0：怪物；1：NPC）-->
    //<item type="alone"    Item_name="cur_occu"        data_type="int32"></item><!--当前职业-->
    //<item type="alone"    Item_name="delta_exp"       data_type="int32"></item><!--变化的经验值-->
    //<item type="alone"    Item_name="old_exp"         data_type="int32"></item><!--变化前的经验值-->
    //<item type="alone"    Item_name="new_exp"         data_type="int32"></item><!--变化后的经验值-->
    //<item type="alone"    Item_name="src_depict"      data_type="str128"></item><!--经验来源的说明-->
    //</Table>*/
    bool logT081_player_getoccuexp_log(CPlayer *pPlayer,DWORD dwType, LONG byOccu, LONG lChangeExp, LONG lOldExp, LONG lNewExp, const char *pSrcDepict);

    //<!--玩家升级日志-->
    //<Table name="t100_player_level_log" log_no="100" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="alone"    Item_name="curr_exp"        data_type="int32"></item><!--经验值总量-->
    //<item type="alone"    Item_name="last_level"      data_type="int32"></item><!--之前级别-->
    //<item type="alone"    Item_name="curr_level"      data_type="int32"></item><!--升级后级别-->
    //<item type="alone"    Item_name="src_depict"      data_type="str128"></item><!--升级的说明-->
    //</Table>
    //*/
    bool logT100_player_level_log(CPlayer *pPlayer, LONG lLastLevel, const char *pSrcDepict);

    //<!--玩家职业等级升级日志-->
    //<Table name="t101_player_Occulevel_log" log_no="101" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="alone"    Item_name="curr_occu"       data_type="int32"></item><!--当前职业-->
    //<item type="alone"    Item_name="curr_exp"        data_type="int32"></item><!--经验值总量-->
    //<item type="alone"    Item_name="last_level"      data_type="int32"></item><!--之前级别-->
    //<item type="alone"    Item_name="curr_level"      data_type="int32"></item><!--升级后级别-->
    //<item type="alone"    Item_name="src_depict"      data_type="str128"></item><!--升级的说明-->
    //</Table>
    //*/
    bool logT101_player_Occulevel_log(CPlayer *pPlayer, LONG lLastLevel, const char *pSrcDepict);

    ///*
    //<!--================================================================-->
    //<!--玩家PK日志-->
    //<Table name="t130_player_attack_log" log_no="130" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="player"   head_name="sponsor"></item>
    //<item type="player"   head_name="participator"></item>
    //</Table>
    //*/
    bool logT130_player_attack_log(CPlayer *pSponsor, CPlayer *pParticipator);
    //<!--玩家致死日志-->
    //<Table name="t140_player_pkdie_log" log_no="140" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="player"   head_name="decedent"></item>
    //<item type="objcet"   head_name="attacker"></item>
    //</Table>
    //*/
    bool logT140_player_pkdie_log(CPlayer *pPlayer, CMoveShape *pMoveShape);
    ///* --------------------------------------------------------------------
    //<!--帮会创建/解散日志-->
    //<Table name="t150_faction_log" log_no="150" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="faction"  head_name=""></item>
    //<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：创建；1解散）-->
    //</Table>
    //*/
    bool LogT150_faction_log(LPCSTR pOprName, const CGUID &pOprGUID, CFaction *pFaction, LONG lAct);
    ///* --------------------------------------------------------------------
    //<!--成员申请加入/退出日志-->
    //<Table name="t160_faction_join_leave_log" log_no="160" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="faction"  head_name=""></item>
    //<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：申请；1退出）-->
    //</Table>
    //*/
    bool LogT160_faction_join_leave_log(const char *pOpterName, const CGUID *pOpterGuid, CFaction *pFaction, LONG lAct);
    ///* --------------------------------------------------------------------
    //<!--同意进入/开除成员日志-->
    //<Table name="t170_faction_agree_kick_log" log_no="170" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="player"   head_name="aim"></item>
    //<item type="faction"  head_name=""></item>
    //<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：同意；1开除）-->
    //</Table>
    //*/
    bool LogT170_faction_agree_kick_log(const char *pOpterName, const CGUID *pOpterGuid, const char *pAimName, const CGUID *pAimGuid, CFaction *pFaction, LONG lAct);
    ///* --------------------------------------------------------------------
    //<!--帮主变更-->
    //<Table name="t250_faction_master_log" log_no="250" log_pos="1">
    //<item type="client_head"    head_name=""></item>
    //<item type="faction"		head_name=""></item>
    //<item type="player"			head_name="old"></item>
    //<item type="player"			head_name="new"></item>
    //</Table>
    //*/
    bool LogT250_faction_master_log(CFaction *pFaction, tagFacMemInfo *pOld, tagFacMemInfo *pNew);
    ///*
    //<!--================================================================-->
    //<!--帮会经验变化-->
    //<Table name="t260_faction_experience_log" log_no="260" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="faction"  head_name=""></item>
    //<item type="alone"    Item_name="before_exp"      data_type="int32"></item><!--之前经验-->
    //<item type="alone"    Item_name="curr_exp"        data_type="int32"></item><!--当前经验-->
    //</Table>
    //*/
    bool LogT260_faction_experience_log(CFaction *pFaction, long lBeforeExp);
    ///*
    //<!--================================================================-->
    //<!--帮会等级变化-->
    //<Table name="t270_faction_level_log" log_no="270" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="faction"  head_name=""></item>
    //<item type="alone"    Item_name="before_level"    data_type="int32"></item><!--之前等级-->
    //<item type="alone"    Item_name="curr_level"      data_type="int32"></item><!--当前等级-->
    //<item type="alone"    Item_name="level_type"      data_type="int32"></item><!--等级类型 0=工会 1=福利 2=仓库 3=锻造 4炼金 5=缝纫-->
    //</Table>
    //*/
    bool LogT270_faction_level_log(CFaction *pFaction, long lCurrLevel, long lLvlType);
    ///* 
    //日志配置：
    //<!--队伍组建-->
    //<Table name="t340_team_create" log_no="34">
    //<item type="date"     head_name=""></item>
    //<item type="object"   head_name="team"></item>
    //<item type="player"   head_name="sponsor"></item>
    //<item type="player"   head_name="participator"></item>
    //</Table>
    //*/
    ////! 记录队伍组建日志
    bool LogT340_team_create(WSTeam *pTeam, CPlayer *pSponsor, CPlayer *pParticipator);

    //<!--队伍加入-->
    //<Table name="t350_team_invite" log_no="35">
    //<item type="date"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="object"   head_name="team"></item>
    //<item type="player"   head_name="sponsor"></item>
    //<item type="player"   head_name="participator"></item>
    //</Table>
    bool LogT350_team_join(GSTeam *pTeam, CPlayer *pPlayer);

    //<!--队伍开除-->
    //<Table name="t360_team_fire" log_no="36">
    //<item type="date"     head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<item type="object"   head_name="team"></item>
    //<item type="player"   head_name="sponsor"></item>
    //<item type="player"   head_name="participator"></item>
    //</Table>
    bool LogT360_team_leave(GSTeam *pTeam, CPlayer *pPlayer);

    //日志配置：
    //<!--队伍解散-->
    //<Table name="t370_team_disband" log_no="37">
    //<item type="date"     head_name=""></item>
    //<item type="object"   head_name="team"></item>
    //</Table>
    //*/
    ////! 记录队伍解散日志
    bool LogT370_team_disband(WSTeam *pTeam);

    //<!--物品分解日志-->
    //	<item type="client_head"     head_name=""></item>
    //	<item type="player"   head_name=""></item>
    //	<item type="goods"    head_name=""></item><!--物品-->
    //	<item type="alone"    Item_name="opt_id"            data_type="guid"></item><!--分解操作的ID号-->
    //	<item type="alone"    Item_name="opt_type"          data_type="int32"></item><!--0=删除的，1=添加的-->
    //	<item type="alone"    Item_name="change_type"       data_type="int32"></item><!--1=添加新物品；2=修改数量-->
    //</Table>
    //*/
    bool logT380_goods_disassemble_log(CPlayer *pPlayer, CGoods *pGoods, long lAddType, const CGUID& guid, BOOL bIsAdd);

    //<!--物品合成日志-->
    //<Table name="t390_goods_synthesize_log" log_no="39" log_pos="1">
    //	<item type="client_head"     head_name=""></item>
    //	<item type="player"   head_name=""></item>
    //	<item type="goods"    head_name=""></item><!--物品-->
    //	<item type="alone"    Item_name="opt_id"            data_type="guid"></item><!--分解操作的ID号-->
    //	<item type="alone"    Item_name="opt_type"          data_type="int32"></item><!--0=删除的，1=添加的-->
    //	<item type="alone"    Item_name="change_type"       data_type="int32"></item><!--1=添加新物品；2=修改数量-->
    //</Table>
    //*/
    bool logT390_goods_synthesize_log(CPlayer *pPlayer, CGoods *pGoods, long lAddType, const CGUID& guid, BOOL bIsAdd);

    //<!--物品升级日志-->
    //<Table name="t400_goods_upgrade_log" log_no="40" log_pos="1">
    //	<item type="client_head"     head_name=""></item>
    //	<item type="player"   head_name=""></item>
    //	<item type="alone"    Item_name="opt_id"            data_type="guid"></item>  <!--操作的ID号-->
    //	<item type="goods"    head_name=""></item><!--被升级的物品-->
    //	<item type="alone"    Item_name="old_level"      data_type="int32"></item><!--升级前等级-->
    //	<item type="alone"    Item_name="new_level"      data_type="int32"></item><!--升级后等级-->
    //</Table>
    //*/
    bool logT400_goods_upgrade_log(CPlayer *pPlayer, const CGUID& guid, CGoods *pGoods, long lOldLevel, long lNewLevel);

    //<!--物品魔化日志-->
    //<Table name="t41_goods_plugevil_log" log_no="41" log_pos="1">
    //	<item type="client_head"     head_name=""></item>
    //	<item type="player"   head_name=""></item>
    //	<item type="alone"    Item_name="opt_id"            data_type="guid"></item>  <!--操作的ID号-->
    //	<item type="goods"    head_name=""></item><!--被升级的物品-->
    //	<item type="alone"    Item_name="item_index"     data_type="int32"></item> <!--修改的魔化栏位-->
    //	<item type="alone"    Item_name="old_level"      data_type="int32"></item><!--升级前等级-->
    //	<item type="alone"    Item_name="new_level"      data_type="int32"></item><!--升级后等级-->
    //</Table>
    //*/
    bool logT410_goods_plugevil_log(CPlayer *pPlayer, const CGUID& guid, CGoods *pGoods, long lItemIndex, long lOldLevel, long lNewLevel);

    //<!--魔化材料获得日志-->
    //<Table name="t420_evil_stuff_synthesize_log" log_no="42" log_pos="1">
    //	<item type="client_head"     head_name=""></item>
    //	<item type="player"   head_name=""></item>
    //	<item type="goods"    head_name=""></item><!--被合成的物品-->
    //	<item type="alone"    Item_name="add_type"      data_type="int32"></item><!--1=添加新物品；2=修改数量-->
    //	<item type="alone"    Item_name="get_num"      data_type="int32"></item><!--1=添加新物品；2=修改数量-->
    //</Table>
    //*/
    bool logT420_evil_stuff_synthesize_log(CPlayer *pPlayer, CGoods *pGoods, long lAddType, long lGetNum);

    //<!--物品绑定和冻结日志-->
    //<Table name="t421_goods_bind_and_frost" log_no="421" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name=""></item><!--被操作的物品-->
    //<item type="goods"    head_name=""></item><!--消耗的物品-->
    //<item type="alone"    Item_name="opt_type"          data_type="int32"></item><!--0=绑定，1=解除绑定， 2=冻结，3=解除冻结-->
    //</Table>
    //*/
    bool logT421_goods_bind_and_frost(CPlayer *pPlayer, CGoods *pAim, CGoods *pStuff, long lType);

    //<!--非副职业制作日志-->
    //<Table name="t422_goods_tailor_system" log_no="422" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name="aim"></item><!--被创建的物品-->
    //</Table>
    bool logT422_goods_tailor_system(CPlayer *pPlayer, CGoods *pGoods);

    //<!--聊天日志-->
    //<Table name="t430_chat_log" log_no="430" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name="sender"></item>
    //<item type="alone"	  Item_name="_accepter_name" data_type="str128"></item>
    //<item type="alone"	  Item_name="_chat_text" 	data_type="str512"></item>
    //<item type="alone"    Item_name="region_id" data_type="int32"></item>
    //<item type="alone"    Item_name="msg_type"      data_type="int32"></item>
    //<!--
    //消息类型
    //# 1. 屏幕
    //#	2. 区域
    //#	3. 帮会
    //#	4. 联盟
    //#	5. 组队
    //#	6. 密语
    //#	7. GM指令
    //-->
    //</Table>
    bool logT430_chat_log(CPlayer* pPlayer,const char* pAccepterName,const char* pChatText,long lType);

    /*<!--GM操作日志-->
    <Table name="t431_GM_log" log_no="431" log_pos="1">
    <item type="client_head"     head_name=""></item>
    <item type="player"   head_name="GM"></item>
    <item type="alone" Item_name="region_id" data_type="int32"></item>
    <item type="alone"	  Item_name="cmd_line" data_type="str128"></item>
    </Table>*/	
    bool logT431_GM_log(CPlayer* pPlayer,const char* pCommandLine);

    //<!--切换场景日志-->
    //<Table name="t440_change_map_log" log_no="440" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="alone"    Item_name="src_region_id"      data_type="int32"></item>
    //<item type="alone"    Item_name="aim_region_id"      data_type="int32"></item>
    //<item type="alone"    Item_name="switch_type"      data_type="int32"></item>
    //<!--
    //切换类型
    //# 1. 本场景
    //#	2. 本服务器
    //#	3. 其他服务器
    //-->
    //</Table>
    bool logT440_change_map_log(CPlayer *pPlayer, LONG lSrcRegionID, LONG lAimRegionID, LONG lType);
    //<!--联系人操作日志-->
    //<Table name="t450_linkman_opt_log" log_no="50" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name="opter"></item><!--操作者-->
    //<item type="player"   head_name="aim"></item><!--操作目标-->
    //<item type="alone"    Item_name="opt_type"      data_type="int32"></item><!--操作类型：1=添加、2=删除-->
    //<item type="alone"    Item_name="group_id"      data_type="int32"></item><!--所操作的联系人组的ID-->
    //</Table>
    bool logT450_linkman_opt_log(CPlayer *pOpter, const CGUID &AimGuid, const char *pAimName, long lOptType, long lGroupID);

    //<!--任务操作-->
    //<Table name="t460_quest_opt_log" log_no="460" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<!--操作目标-->
    //<item type="alone"    Item_name="quest_id"      data_type="int32"></item><!--操作任务id-->
    //<item type="alone"    Item_name="opt_type"      data_type="int32"></item><!--操作类型：1=添加、2=完成 3=删除-->
    //<item type="alone"    Item_name="TeamID"        data_type="guid"></item><!--队伍GUID-->
    //</Table>
    bool logT460_quest_log(CPlayer* pPlayer,DWORD dwQuestID ,DWORD dwQuestOperType);

    //<!--赏金任务刷新操作-->
    //<Table name="t461_mercquest_log" log_no="461" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="pos"      head_name=""></item>
    //<!--操作目标-->
    //<item type="alone"    Item_name="TeamID"         data_type="guid"></item><!--队伍GUID-->
    //<item type="alone"    Item_name="usegoods"       data_type="int32"></item><!--是否使用物品，1是，0否-->
    //<item type="alone"    Item_name="quest_id1"      data_type="int32"></item><!--刷出的任务1-->
    //<item type="alone"    Item_name="quest_id2"      data_type="int32"></item><!--刷出的任务2-->
    //<item type="alone"    Item_name="quest_id3"      data_type="int32"></item><!--刷出的任务3-->
    //<item type="alone"    Item_name="quest_id4"      data_type="int32"></item><!--刷出的任务4-->
    //</Table>
    bool logT461_mercquest_log(CPlayer* pPlayer,DWORD bUsegoods,DWORD quest1,DWORD quest2,DWORD quest3,DWORD quest4);

    //<!--点券消费日志-->
    //<Table name="t501_Ticket_consume_log" log_no="501" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="goods"    head_name=""></item>
    //<item type="alone"    Item_name="goods_price"     data_type="int32"></item>
    //<!--物品价格-->
    //<item type="alone"    Item_name="goods_num"       data_type="int32"></item>
    //<!--物品数量-->
    //<item type="alone"    Item_name="gold_remain"   data_type="int32"></item>
    //<!--剩余金币-->
    //<item type="alone"    Item_name="ticket_remain"   data_type="int32"></item>
    //<!--剩余点券-->
    //<item type="alone"    Item_name="act_type"        data_type="int32"></item>
    //<!--1：点券买；2:金币买 -->
    //</Table>
    bool logT501_Ticket_consume_log(CPlayer* pPlayer,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,DWORD dwPrice,DWORD dwGoodsNum,DWORD dwMoney,DWORD dwTicket,DWORD dwActType);

    //<!--点券获取日志-->
    //<Table name="t502_Ticket_gain_log" log_no="502" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="alone"    Item_name="gain_num"       data_type="int32"></item>
    //<item type="alone"    Item_name="ticket_num"       data_type="int32"></item>
    //</Table>
    bool logT502_Ticket_gain_log(CPlayer* pPlayer,int nGainNum);

    ///*<!--金币获取日志-->
    //	  <Table name="t503_Gold_gain_log" log_no="503" log_pos="1">
    //		<item type="client_head"     head_name=""></item>
    //		<item type="player"   head_name=""></item>    
    //		<item type="alone"    Item_name="old_num"       data_type="int32"></item>
    //		<item type="alone"    Item_name="new_num"       data_type="int32"></item>
    //		<item type="alone"    Item_name="flag"          data_type="int32"></item>
    //	  </Table>*/
    bool logT503_Gold_gain_log(CPlayer* pPlayer,int nOldNum,int nNewNum,int nFlag);

    //<!--位面变化日志-->
    //<Table name="t504_weimian_log" log_no="504" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="alone"    Item_name="weimian_num"       data_type="int32"></item>
    //<!--0增加 1减少-->
    //<item type="alone"    Item_name="act_type"       data_type="int32"></item>
    //<item type="alone"    Item_name="flag"          data_type="int32"></item>
    //</Table>
    bool logT504_weimian_log(CPlayer* pPlayer,int nNum,int nActType,int nFlag);

    //<!--gs发送SNDA url计费请求-->
    //<Table name="t521_snda_url_send_log" log_no="521" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="alone"    Item_name="orderId" data_type="str128"></item>
    //<item type="alone"    Item_name="cdkey"   data_type="str128"></item>
    //<item type="alone"    Item_name="total_amount"    data_type="int32"></item> 
    //<item type="alone"    Item_name="goodsinfo"   data_type="str128"></item>    
    //</Table>
    bool logT521_snda_url_send_log(CPlayer* pPlayer,const char* szOrderId,int nTotalAmount,const char* szGoodsInfo);

    //<!--gs发送SNDA url计费请求-->
    //<Table name="t522_snda_ticket_consume_log" log_no="522" log_pos="1">
    //<item type="client_head"     head_name=""></item>
    //<item type="player"   head_name=""></item>
    //<item type="alone"    Item_name="orderId" data_type="str128"></item>
    //<item type="alone"    Item_name="cdkey"   data_type="str128"></item>
    //<item type="alone"    Item_name="sendTimes"    data_type="str128"></item> 
    //<item type="alone"    Item_name="goodsinfo"    data_type="str128"></item> 
    //<item type="alone"    Item_name="payerrormsg"  data_type="str128"></item> 
    //<item type="alone"    Item_name="retcode"  data_type="int32"></item> 
    //</Table>
    bool logT522_snda_ticket_consume_log(CPlayer* pPlayer,const char* szOrderId,const char* szSendTime,
        const char* szGoodsInfo,const char* payErrorMsg,int retcode);

    ///*
    //<!--客户端上下线日志-->
    //<Table name="t550_client_in_game_log" log_no="550" log_pos="1">
    //	<item type="client_head"     head_name=""></item>
    //	<item type="alone"    Item_name="cdkey"         data_type="str128"></item><!--IP-->
    //	<item type="alone"    Item_name="ip"            data_type="int32"></item><!--CDKEY-->
    //	<item type="player"   head_name=""></item>
    //	<item type="pos"      head_name=""></item>
    //	<item type="alone"    Item_name="region_guid"   data_type="int32"></item><!--level-->
    //	<item type="alone"    Item_name="level"         data_type="int32"></item><!--level-->
    //	<item type="alone"    Item_name="gold"          data_type="int32"></item><!--gold-->
    //	<item type="alone"    Item_name="silver"        data_type="int32"></item><!--silver-->
    //	<item type="alone"    Item_name="flag"          data_type="int32"></item><!--操作类型：1=上线、2=下线-->
    //</Table>
    //*/
    bool logT550_client_in_game_log(CPlayer *pPlayer, LONG lState);

    //<!--收取邮件日志-->
    //<Table name="t600_receive_mail_log" log_no="600" log_pos="1">
    //<item type="client_head"     head_name=""></item>    
    //<item type="player"   head_name=""></item>
    //<item type="alone"    Item_name="mail_guid"            data_type="guid"></item><!--邮件guid-->
    //<item type="alone"	  Item_name="writer"			   data_type="str128"></item><!--发信人-->
    //<item type="alone"	  Item_name="type"			   	   data_type="int32"></item><!--邮件类型-->
    //<item type="alone"	  Item_name="gold"			   	   data_type="int32"></item><!--金钱--> 
    //<item type="alone"	  Item_name="goodsnum"			   data_type="int32"></item><!--物品数量-->
    //<item type="alone"	  Item_name="incgoodsnum"		   data_type="int32"></item><!--增值物品数量-->    
    //<item type="alone"    Item_name="flag"          	   data_type="int32"></item><!--操作类型：1=上线收取、2=在线接收-->
    //</Table>
    bool logT600_Receive_mail_log(CPlayer *pPlayer,const CGUID &MailGuid,const char *strWriter,int nType,long lGold,long lGoodsNum/*,long lIncGoodsNum*/,int nFlag);

    //<Table name="t601_send_mail_log" log_no="601" log_pos="1">
    //<item type="client_head"     head_name=""></item>    
    //<item type="alone"    Item_name="mail_guid"            data_type="guid"></item><!--邮件guid-->
    //<item type="alone"	  Item_name="receiver"			   data_type="str128"></item><!--收信人-->
    //<item type="alone"	  Item_name="writer"			   data_type="str128"></item><!--发信人-->
    //<item type="alone"	  Item_name="type"			   	   data_type="int32"></item><!--邮件类型-->
    //<item type="alone"	  Item_name="gold"			   	   data_type="int32"></item><!--金钱--> 
    //<item type="alone"	  Item_name="goodsnum"			   data_type="int32"></item><!--物品数量-->
    //<item type="alone"	  Item_name="incgoodsnum"		   data_type="int32"></item><!--增值物品数量--> 
    //<item type="alone"	  Item_name="goodsinfo"				   data_type="str512"></item><!--物品-->    
    //</Table>
    //*/
    bool logT601_Send_mail_log(const CGUID &MailGuid,const char *strReceiver,const char *strWriter,int nType,long lGold,long lGoodsNum/*,long lIncGoodsNum*/,const char *strGoodsInfo);

    ///*<!--从邮件中取物品日志-->
    //<Table name="t602_getgoods_mail_log" log_no="602" log_pos="1">
    //<item type="client_head"     head_name=""></item>    
    //<item type="alone"    Item_name="mail_guid"            data_type="guid"></item><!--邮件guid-->
    //<item type="alone"	  Item_name="receiver"			   data_type="str128"></item><!--收信人-->
    //<item type="alone"	  Item_name="writer"			   data_type="str128"></item><!--发信人-->
    //<item type="goods"    head_name=""></item>
    //<item type="alone"	  Item_name="type"			   	   data_type="int32"></item><!--邮件类型-->
    //<item type="alone"	  Item_name="gold"			   	   data_type="int32"></item><!--金钱--> 
    //<item type="alone"	  Item_name="goodsnum"			   data_type="int32"></item><!--物品数量-->
    //<item type="alone"	  Item_name="incgoodsnum"		   data_type="int32"></item><!--增值物品数量-->    
    //</Table>
    bool logT602_GetGoods_mail_log(const CGUID &MailGuid,const char *strReceiver,const char *strWriter,DWORD dwGoodsId,const CGUID& goodsGuid,const char* strGoodsName,int nType,long lGold,long lGoodsNum/*,long lIncGoodsNum*/);

    //<!--从邮件中取金币日志-->
    //<Table name="t603_getgold_mail_log" log_no="603" log_pos="1">
    //<item type="client_head"     head_name=""></item>    
    //<item type="alone"    Item_name="mail_guid"            data_type="guid"></item><!--邮件guid-->
    //<item type="alone"	  Item_name="receiver"			   data_type="str128"></item><!--收信人-->
    //<item type="alone"	  Item_name="writer"			   data_type="str128"></item><!--发信人-->    
    //<item type="alone"	  Item_name="type"			   	   data_type="int32"></item><!--邮件类型-->
    //<item type="alone"	  Item_name="gold"			   	   data_type="int32"></item><!--金钱--> 
    //<item type="alone"	  Item_name="goodsnum"			   data_type="int32"></item><!--物品数量-->
    //<item type="alone"	  Item_name="incgoodsnum"		   data_type="int32"></item><!--增值物品数量-->    
    //</Table>
    bool logT603_GetGold_mail_log(const CGUID &MailGuid,const char *strReceiver,const char *strWriter,int nType,long lGold,long lGoodsNum/*,long lIncGoodsNum*/);

    //<!--读取邮件日志-->
    //<Table name="t604_read_mail_log" log_no="604" log_pos="1">
    //<item type="client_head"     head_name=""></item>    
    //<item type="player"   head_name=""></item>
    //<item type="alone"    Item_name="mail_guid"            data_type="guid"></item><!--邮件guid-->		
    //<item type="alone"	  Item_name="type"			   	   data_type="int32"></item><!--邮件类型-->
    //<item type="alone"	  Item_name="gold"			   	   data_type="int32"></item><!--金钱--> 
    //<item type="alone"	  Item_name="goodsnum"			   data_type="int32"></item><!--物品数量-->
    //<item type="alone"	  Item_name="incgoodsnum"		   data_type="int32"></item><!--增值物品数量-->    
    //<item type="alone"    Item_name="flag"          	   data_type="int32"></item><!--操作类型：1=成功、0=失败-->
    //</Table>
    bool logT604_read_mail_log(CPlayer *pPlayer,const CGUID &MailGuid,int nType,long lGold,long lGoodsNum/*,long lIncGoodsNum*/,int nFlag);

    //<!--删除邮件日志-->
    //<Table name="t605_delete_mail_log" log_no="605" log_pos="1">
    //<item type="client_head"     head_name=""></item> 
    //<item type="alone"    Item_name="mail_guid"            data_type="guid"></item><!--邮件guid-->   
    //<item type="alone"	  Item_name="receiver"			   data_type="str128"></item><!--收信人-->
    //<item type="alone"	  Item_name="writer"			   data_type="str128"></item><!--发信人-->    
    //<item type="alone"	  Item_name="type"			   	   data_type="int32"></item><!--邮件类型-->
    //<item type="alone"	  Item_name="gold"			   	   data_type="int32"></item><!--金钱--> 
    //<item type="alone"	  Item_name="goodsnum"			   data_type="int32"></item><!--物品数量-->
    //<item type="alone"	  Item_name="incgoodsnum"		   data_type="int32"></item><!--增值物品数量-->    
    //</Table>*/
    bool logT605_delete_mail_log(const CGUID &MailGuid,const char *strReceiver,const char *strWriter,int nType,long lGold,long lGoodsNum/*,long lIncGoodsNum*/);

    /*
    <!--跑商玩家在NPC处买卖日志-->
    <Table name="t700_business_npc_log" log_no="700" log_pos="1">
    <item type="client_head" head_name=""></item>
    <item type="pos" head_name=""></item>
    <item type="player" head_name=""></item>
    <item type="goods" head_name=""></item>
    <item type="alone" Item_name="act_type" data_type="int32"></item> <!--操作类型，1：买，2：卖-->
    <item type="alone" Item_name="goods_amount" data_type="int32"></item> <!--交易数量-->
    <item type="alone" Item_name="trade_spot" data_type="int32"></item> <!--贸易点ID-->
    <item type="alone" Item_name="spent_money" data_type="int32"></item> <!--消费的钱-->
    </Table>
    */
    bool logT700_business_npc( CPlayer *pPlayer, CGoods *pGoods, int iTradeAmount, 
        int iTradeMoney, int iActType, int iTradeSpot );

    //<!--================================================================-->
    //<!--每张图上的玩家数记录-->
    //<Table name="t800_player_num_in_region" log_no="800" log_pos="1">
    //<item type="alone" Item_name="gs_id" data_type="int32"></item>
    //<item type="client_head" head_name=""></item>
    //<item type="alone" Item_name="region_name" data_type="int32"></item>
    //<item type="alone" Item_name="region_id" data_type="int32"></item>
    //<item type="alone" Item_name="region_guid" data_type="int32"></item>
    //<item type="alone" Item_name="player_num" data_type="int32"></item> <!--玩家数量-->
    //</Table>
    //*/
    //
    bool logT800_player_num_in_region(LONG lGsID, CServerRegion *pCServerRegion);

    //<!--反外挂日志记录：T下线及刚解封日志-->
    //<Table name="t900_anticheat_player_log" log_no="900" log_pos="1">
    //<item type="client_head" head_name=""></item>			<!-- 时间 -->
    //<item type="pos" head_name=""></item>				<!-- 位置 -->
    //<item type="player" head_name=""></item>			<!-- 玩家 -->	
    //<item type="alone" Item_name="log_type" data_type="str128"></item> <!-- 操作类型 -->
    //<item type="alone" Item_name="ACC_WRONG" data_type="int32"></item> <!--累计外挂值-->
    //<item type="alone" Item_name="CUR_WRONG" data_type="int32"></item> <!--当前外挂值-->
    //<item type="alone" Item_name="CUR_BONUS" data_type="int32"></item> <!--当天累计奖励次数-->
    //</Table>
    bool logT900_anticheat_player_log(CPlayer * player, const char *log_type, int acc_wrong, int cur_wrong, int acc_bonus);

    //<!--反外挂日志记录：答题情况日志-->
    //<Table name="t901_anticheat_player_log" log_no="901" log_pos="1">
    //<item type="client_head" head_name=""></item>			<!-- 时间 -->
    //<item type="pos" head_name=""></item>				<!-- 位置 -->
    //<item type="player" head_name=""></item>			<!-- 玩家 -->	
    //<item type="alone" Item_name="oper_type" data_type="str128"></item> <!-- 操作类型:ANSWER, TIMEOUT, REFUSED, EXIT -->
    //<item type="alone" Item_name="ACC_WRONG" data_type="int32"></item> <!--累计外挂值-->
    //<item type="alone" Item_name="CUR_WRONG" data_type="int32"></item> <!--当前外挂值-->
    //<item type="alone" Item_name="CUR_RIGHT" data_type="int32"></item> <!--当前连续答对次数-->
    //<item type="alone" Item_name="CUR_BONUS" data_type="int32"></item> <!--当天累计奖励次数-->
    //<item type="alone" Item_name="PK" data_type="int32"></item> <!--当前PK值-->
    //<item type="alone" Item_name="RHAnswered" data_type="int32"></item> <!--是否在黄金时间答题正确过-->
    //</Table>
    bool logT901_anticheat_answer_log( CPlayer *player, const char *log_type, 
        int acc_wrong, int cur_wrong, int cur_right, int acc_bonus, int pk, bool RHAnswered );

    //<!--反外挂日志记录：答题情况日志-->
    //<Table name="t902_player_progress_chg_log" log_no="902" log_pos="1">
    //<item type="client_head" head_name=""></item>    
    //<item type="player" head_name=""></item>    
    //<!--老状态-->
    //<item type="alone" Item_name="old_progress" data_type="int32"></item>
    //<!--新状态-->
    //<item type="alone" Item_name="new_progress" data_type="int32"></item>    
    //</Table>
    bool logT902_player_progress_chg_log(CPlayer* pPlayer,int nOldProgress,int nNewProgress);

    /* <!-- 仙贝寄售日志记录 -->
    <Table name="t987_auction_house" log_no="987" log_pos="1">
    <item type="client_head" head_name=""> </item>
    <item type="player" head_name=""> </item>
    <item type="alone" Item_name="op" data_type="str128"> </item>
    <item type="alone" Item_name="count" data_type="int32"> </item>
    <item type="alone" Item_name="price" data_type="int32"> </item>
    </Table> 
    */
    bool logT987_auction_house_log( const CGUID &playerID, const char *opDesc, int cnt, int price );

private:

	//!	得到当前时间字符串（00-00-00 00:00:00格式）
	void    GetCurrTimeString(string &strTime);

private:

	//!	日志客户端对象
	LogClient   *m_pLogClient;

};