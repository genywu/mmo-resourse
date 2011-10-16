#include "StdAfx.h"
#include "Goodslist.h"
#include "Goods.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HWND g_hWnd;

map<ulong, CGoodsList::tagGoods2>		CGoodsList::m_mapGoodsList;			// 原始物品列表(wIndew, tagGoods)
map<string, CGoodsList::tagGoods2*>		CGoodsList::m_mapGoodsListByName;	// 以名字为索引的原始物品列表(strName, *tagGoods)
map<ulong,string>									CGoodsList::m_mapSuitName;			// 用套装ID对应存放所有套装的名称
map<ulong/*套装ID*/, CGoodsList::tagSuitAttribute>	CGoodsList::m_SuitList;				// 套装的属性表单，存放所有套装在各种件数条件下激发的总的属性
//map<string,ulong>									CGoodsList::m_mapEffects;			// 映射物品名称和附加属性
map<string,ulong>									CGoodsList::m_mapAllEffects;		// 映射附加属性名称和附加属性枚举值
map<ulong,string>									CGoodsList::m_mapEffectReverse;		// 映射物品附加属性和名称
map<ulong/*套卡的ID号*/,CGoodsList::tagSeriesCardProperty>	CGoodsList::m_mapSeriesCardList;	// 套卡属性列表
vector<CGoodsList::tagMoneyGraphics> CGoodsList::m_stMoneyGraphics;
vector<CGoodsList::tagMoneyGraphics> CGoodsList::m_stSilverMoneyGraphics;

CGoodsList::CGoodsList(void)
{
}

CGoodsList::~CGoodsList(void)
{
	m_mapGoodsList.clear();
	m_mapGoodsListByName.clear();
	m_mapAllEffects.clear();
	m_mapEffectReverse.clear();
	m_SuitList.clear();
	m_mapSeriesCardList.clear();
}

void CGoodsList::InitMapEffects()
{
	if (m_mapEffectReverse.empty())
	{
		//反转的属性映射Map
        m_mapEffectReverse[GAP_MAXHP_CORRECT] = AppFrame::GetText("Player_23");//生命上限
        m_mapEffectReverse[GAP_MAXMP_CORRECT]= AppFrame::GetText("Player_24");//法力上限 
        m_mapEffectReverse[GAP_HP_RESUME_CORRECT] = AppFrame::GetText("Player_25");//生命恢复速度
        m_mapEffectReverse[GAP_MP_RESUME_CORRECT] = AppFrame::GetText("Player_26");//法力恢复速度
        m_mapEffectReverse[GAP_ADD_STR] = AppFrame::GetText("Player_27");//力量
        m_mapEffectReverse[GAP_ADD_DEX] = AppFrame::GetText("Player_28");//敏捷
        m_mapEffectReverse[GAP_ADD_CON] = AppFrame::GetText("Player_29");//体质    
        m_mapEffectReverse[GAP_ADD_INT] = AppFrame::GetText("Player_30");//智力
        m_mapEffectReverse[GAP_ADD_WIS] = AppFrame::GetText("Player_31");//智慧 
        m_mapEffectReverse[GAP_ADD_SPI] = AppFrame::GetText("Player_32");//精神
        m_mapEffectReverse[GAP_PARRY_CORRECT] = AppFrame::GetText("Player_33");//格挡 
        m_mapEffectReverse[GAP_BLOCK_CORRECT] = AppFrame::GetText("Player_34");//招架 
        m_mapEffectReverse[GAP_FLEE_CORRECT] = AppFrame::GetText("Player_35");//闪避  
        m_mapEffectReverse[GAP_HIT_RATE_CORRECT] = AppFrame::GetText("Player_36");//命中
        m_mapEffectReverse[GAP_DEDICANT_CORRECT] = AppFrame::GetText("Player_37");//专注
        m_mapEffectReverse[GAP_NOR_DEF] = AppFrame::GetText("Player_38");//物理防御
        m_mapEffectReverse[GAP_MAGIC_DEF] = AppFrame::GetText("Player_39");//魔法抗性
        m_mapEffectReverse[GAP_NORATTACK_KILLRATE] = AppFrame::GetText("Player_40");//物理爆击
        m_mapEffectReverse[GAP_MAGIC_SPEED] = AppFrame::GetText("Player_41");//施法速度
        m_mapEffectReverse[GAP_MIN_ATTACK] = AppFrame::GetText("Player_42");//最小攻击力
        m_mapEffectReverse[GAP_MAX_ATTACK] = AppFrame::GetText("Player_43");//最大攻击力
        m_mapEffectReverse[GAP_FUMO_PROPERTY] = AppFrame::GetText("Player_44");//魔法攻击力
        m_mapEffectReverse[GAP_MAGIC_KILLRATE] = AppFrame::GetText("Player_45");//魔法爆击率
        m_mapEffectReverse[GAP_LEECH_HP] = AppFrame::GetText("Player_46");//生命吸收率
        m_mapEffectReverse[GAP_LEECH_MP] = AppFrame::GetText("Player_47");//法力吸收率
        m_mapEffectReverse[GAP_WILL_DEF] = AppFrame::GetText("Player_48");//意志抗性
        m_mapEffectReverse[GAP_CON_DEF] = AppFrame::GetText("Player_49");//体质抗性
        m_mapEffectReverse[GAP_BLED_DEF] = AppFrame::GetText("Player_50");//流血抗性
        m_mapEffectReverse[GAP_DIZZY_DEF] = AppFrame::GetText("Player_51");//击昏抗性
        m_mapEffectReverse[GAP_LULL_DEF] = AppFrame::GetText("Player_52");//麻痹抗性
        m_mapEffectReverse[GAP_HYPNOSIS_DEF] = AppFrame::GetText("Player_53");//催眠抗性
        m_mapEffectReverse[GAP_POISON_DEF] = AppFrame::GetText("Player_54");//中毒抗性
        m_mapEffectReverse[GAP_FIX_DEF] = AppFrame::GetText("Player_55");//定身抗性
        m_mapEffectReverse[GAP_FIREDEF_CORRECT] = AppFrame::GetText("Player_56");//火抗
        m_mapEffectReverse[GAP_WATERDEF_CORRECT] = AppFrame::GetText("Player_57");//水抗
        m_mapEffectReverse[GAP_EARTHDEF_CORRECT] = AppFrame::GetText("Player_58");//地抗
        m_mapEffectReverse[GAP_WINDDEF_CORRECT] = AppFrame::GetText("Player_59");//风抗
        m_mapEffectReverse[GAP_DARKDEF_CORRECT] = AppFrame::GetText("Player_60");//黑暗抗
        m_mapEffectReverse[GAP_HOLYDEF_CORRECT] = AppFrame::GetText("Player_61");//神圣抗
        m_mapEffectReverse[GAP_HURT_SUCK_PER] = AppFrame::GetText("Player_62");//伤害吸收比例
        m_mapEffectReverse[GAP_WEAPON_DAMAGE_LEVEL] = AppFrame::GetText("Player_101");//武器强度
        m_mapEffectReverse[GAP_LEECH_MP] = AppFrame::GetText("Player_102");//吸收Mp比例
        m_mapEffectReverse[GAP_HURT2MP] = AppFrame::GetText("Player_103");//损伤转Mp比例
        m_mapEffectReverse[GAP_CRI_DAMAGE] = AppFrame::GetText("Player_104");//物理暴击伤害
        m_mapEffectReverse[GAP_CRI_DEF] = AppFrame::GetText("Player_105");//物理暴击抗性
        m_mapEffectReverse[GAP_MCRI_DAMAGE] = AppFrame::GetText("Player_106");//魔法暴击伤害
        m_mapEffectReverse[GAP_MCRI_DEF] = AppFrame::GetText("Player_107");//魔法暴击抗性
        m_mapEffectReverse[GAP_MAX_ENERGY_COR] = AppFrame::GetText("Player_108");//能量上限修正
        m_mapEffectReverse[GAP_ENERGY_RESUME_COR] = AppFrame::GetText("Player_109");//能量恢复速度修正
        m_mapEffectReverse[GAP_ENERGY_SUCK] = AppFrame::GetText("Player_110");//吸收能量
        m_mapEffectReverse[GAP_PEOPLE_HURT_RATIO] = AppFrame::GetText("Player_111");//对人形类怪物伤害加成
        m_mapEffectReverse[GAP_ELEMENT_HURT_RATIO] = AppFrame::GetText("Player_112");//对元素类怪物伤害加成
        m_mapEffectReverse[GAP_DIFFRACE_HURT_RATIO] = AppFrame::GetText("Player_113");//对异族类怪物伤害加成
        m_mapEffectReverse[GAP_ANIM_HURT_RATIO] = AppFrame::GetText("Player_114");//对动物类怪物伤害加成
        m_mapEffectReverse[GAP_SOUL_HURT_RATIO] = AppFrame::GetText("Player_115");//对亡灵类怪物伤害加成
        m_mapEffectReverse[GAP_DEMON_HURT_RATIO] = AppFrame::GetText("Player_116");//对恶魔类怪物伤害加成
        m_mapEffectReverse[GAP_INSECT_HURT_RATIO] = AppFrame::GetText("Player_117");//对昆虫类怪物伤害加成
        m_mapEffectReverse[GAP_FIRE_HURT_RATIO] = AppFrame::GetText("Player_118");//对火系怪物伤害加成
        m_mapEffectReverse[GAP_WATER_HURT_RATIO] = AppFrame::GetText("Player_119");//对水系怪物伤害加成
        m_mapEffectReverse[GAP_EARTH_HURT_RATIO] = AppFrame::GetText("Player_120");//对土系怪物伤害加成
        m_mapEffectReverse[GAP_WIND_HURT_RATIO] = AppFrame::GetText("Player_121");//对风系怪物伤害加成
        m_mapEffectReverse[GAP_DARK_HURT_RATIO] = AppFrame::GetText("Player_122");//对暗系怪物伤害加成
        m_mapEffectReverse[GAP_HOLINESS_HURT_RATIO] = AppFrame::GetText("Player_123");//对圣系怪物伤害加成
        m_mapEffectReverse[GAP_HURT_RID] = AppFrame::GetText("Player_124");//免疫物理攻击
        m_mapEffectReverse[GAP_MAGIC_HURT_RID] = AppFrame::GetText("Player_125");//免疫魔法攻击
        m_mapEffectReverse[GAP_SHIELD_DEF] = AppFrame::GetText("Player_126");//盾牌物理防御
        m_mapEffectReverse[GAP_TIME_LIMIT] = AppFrame::GetText("Player_127");//限时
        m_mapEffectReverse[GAP_ADDON_CHARM] = AppFrame::GetText("Player_128");//魅力
        m_mapEffectReverse[GAP_HP_RESUME_RATE] = AppFrame::GetText("Player_129");//生命值恢复比例
        m_mapEffectReverse[GAP_BUFF_ATK_STRENGTH] = AppFrame::GetText("Player_130");//状态攻击强度
        m_mapEffectReverse[GAP_PENETRATE] = AppFrame::GetText("Player_214");//攻击穿透
        m_mapEffectReverse[GAP_EXCUSE_HURT] = AppFrame::GetText("Player_215");//伤害减免
        m_mapEffectReverse[GAP_LUCKY] = AppFrame::GetText("Player_223");//幸运
        m_mapEffectReverse[GAP_ADDON_PENETRATE] = AppFrame::GetText("Player_214");//附加穿透攻击
        m_mapEffectReverse[GAP_ADDON_EXCUSE_HURT] = AppFrame::GetText("Player_215");//附加伤害减免
        m_mapEffectReverse[GAP_ADDON_LUCKY] = AppFrame::GetText("Player_223");//附加幸运
        m_mapEffectReverse[GAP_CUR_ATK_ED] = AppFrame::GetText("Player_221");//强化攻击
        m_mapEffectReverse[GAP_CUR_DEF_ED] = AppFrame::GetText("Player_222");//强化防御
        m_mapEffectReverse[GAP_ADDON_ATK_ED] = AppFrame::GetText("Player_221");//附加强化攻击
        m_mapEffectReverse[GAP_ADDON_DEF_ED] = AppFrame::GetText("Player_222");//附加强化防御
        m_mapEffectReverse[GAP_MIN_ATK_ADDON] = AppFrame::GetText("Player_224");//小攻追加
        m_mapEffectReverse[GAP_MAX_ATK_ADDON] = AppFrame::GetText("Player_225");//大攻追加
        m_mapEffectReverse[GAP_MAGIC_ATK_ADDON] = AppFrame::GetText("Player_226");//魔攻追加
        m_mapEffectReverse[GAP_DEF_ADDON] = AppFrame::GetText("Player_227");		//物防追加
        m_mapEffectReverse[GAP_MAGIC_DEF_ADDON] = AppFrame::GetText("Player_228");//魔防追加
	}
	if (m_mapAllEffects.empty())
	{
		m_mapAllEffects["GAP_ADD_STATE"]=GAP_ADD_STATE;//激发的状态
		m_mapAllEffects["GAP_ADD_SKILL"]=GAP_ADD_SKILL;//附加技能
		m_mapAllEffects["GAP_BUFF_ATK_STRENGTH"]=GAP_BUFF_ATK_STRENGTH;//状态攻击强度
		m_mapAllEffects["GAP_HP_RESUME_RATE"]=GAP_HP_RESUME_RATE;//生命值恢复比例
		m_mapAllEffects["GAP_ADDON_CHARM"]=GAP_ADDON_CHARM;//魅力
		m_mapAllEffects["GAP_TIME_LIMIT"]=GAP_TIME_LIMIT;//限时
		m_mapAllEffects["GAP_SHIELD_DEF"]=GAP_SHIELD_DEF;//盾牌物理防御
		m_mapAllEffects["GAP_MAGIC_HURT_RID"]=GAP_MAGIC_HURT_RID;//免疫魔法攻击
		m_mapAllEffects["GAP_HURT_RID"]=GAP_HURT_RID;//免疫物理攻击
		m_mapAllEffects["GAP_HOLINESS_HURT_RATIO"]=GAP_HOLINESS_HURT_RATIO;//对圣系怪物伤害加成
		m_mapAllEffects["GAP_DARK_HURT_RATIO"]=GAP_DARK_HURT_RATIO;//对暗系怪物伤害加成
		m_mapAllEffects["GAP_WIND_HURT_RATIO"]=GAP_WIND_HURT_RATIO;//对风系怪物伤害加成
		m_mapAllEffects["GAP_EARTH_HURT_RATIO"]=GAP_EARTH_HURT_RATIO;//对土系怪物伤害加成
		m_mapAllEffects["GAP_WATER_HURT_RATIO"]=GAP_WATER_HURT_RATIO;//对水系怪物伤害加成
		m_mapAllEffects["GAP_FIRE_HURT_RATIO"]=GAP_FIRE_HURT_RATIO;//对火系怪物伤害加成
		m_mapAllEffects["GAP_INSECT_HURT_RATIO"]=GAP_INSECT_HURT_RATIO;//对昆虫类怪物伤害加成
		m_mapAllEffects["GAP_DEMON_HURT_RATIO"]=GAP_DEMON_HURT_RATIO;//对恶魔类怪物伤害加成
		m_mapAllEffects["GAP_SOUL_HURT_RATIO"]=GAP_SOUL_HURT_RATIO;//对亡灵类怪物伤害加成
		m_mapAllEffects["GAP_ANIM_HURT_RATIO"]=GAP_ANIM_HURT_RATIO;//对动物类怪物伤害加成
		m_mapAllEffects["GAP_DIFFRACE_HURT_RATIO"]=GAP_DIFFRACE_HURT_RATIO;//对异族类怪物伤害加成
		m_mapAllEffects["GAP_ELEMENT_HURT_RATIO"]=GAP_ELEMENT_HURT_RATIO;//对元素类怪物伤害加成
		m_mapAllEffects["GAP_PEOPLE_HURT_RATIO"]=GAP_PEOPLE_HURT_RATIO;//对人形类怪物伤害加成
		m_mapAllEffects["GAP_ENERGY_SUCK"]=GAP_ENERGY_SUCK;//吸收能量
		m_mapAllEffects["GAP_ENERGY_RESUME_COR"]=GAP_ENERGY_RESUME_COR;//能量恢复速度修正
		m_mapAllEffects["GAP_MAX_ENERGY_COR"]=GAP_MAX_ENERGY_COR;//能量上限修正
		m_mapAllEffects["GAP_MCRI_DEF"]=GAP_MCRI_DEF;//魔法暴击抗性
		m_mapAllEffects["GAP_MCRI_DAMAGE"]=GAP_MCRI_DAMAGE;//魔法暴击伤害
		m_mapAllEffects["GAP_CRI_DEF"]=GAP_CRI_DEF;//物理暴击抗性
		m_mapAllEffects["GAP_CRI_DAMAGE"]=GAP_CRI_DAMAGE;//物理暴击伤害 
		m_mapAllEffects["GAP_HURT2MP"]=GAP_HURT2MP;//损伤转Mp比例 
		m_mapAllEffects["GAP_LEECH_MP"]=GAP_LEECH_MP;//吸收Mp比例 
		m_mapAllEffects["GAP_HURT_SUCK_PER"]=GAP_HURT_SUCK_PER;//伤害吸收比例 
		m_mapAllEffects["GAP_WEAPON_DAMAGE_LEVEL"]=GAP_WEAPON_DAMAGE_LEVEL;//武器强度   
		m_mapAllEffects["GAP_MAXHP_CORRECT"]=GAP_MAXHP_CORRECT;//生命上限             
		m_mapAllEffects["GAP_MAXMP_CORRECT"]=GAP_MAXMP_CORRECT;//法力上限                     
		m_mapAllEffects["GAP_HP_RESUME_CORRECT"]=GAP_HP_RESUME_CORRECT;//生命恢复速度     
		m_mapAllEffects["GAP_MP_RESUME_CORRECT"]=GAP_MP_RESUME_CORRECT;//法力恢复速度                     
		m_mapAllEffects["GAP_ADD_STR"]=GAP_ADD_STR;//力量         
		m_mapAllEffects["GAP_ADD_DEX"]=GAP_ADD_DEX;//敏捷          
		m_mapAllEffects["GAP_ADD_CON"]=GAP_ADD_CON;//体质       
		m_mapAllEffects["GAP_ADD_INT"]=GAP_ADD_INT;//智力
		m_mapAllEffects["GAP_ADD_WIS"]=GAP_ADD_WIS;//智慧
		m_mapAllEffects["GAP_ADD_SPI"]=GAP_ADD_SPI;//精神         
		m_mapAllEffects["GAP_PARRY_CORRECT"]=GAP_PARRY_CORRECT;//格挡            
		m_mapAllEffects["GAP_BLOCK_CORRECT"]=GAP_BLOCK_CORRECT;//招架            
		m_mapAllEffects["GAP_FLEE_CORRECT"]=GAP_FLEE_CORRECT;//闪避           
		m_mapAllEffects["GAP_HIT_RATE_CORRECT"]=GAP_HIT_RATE_CORRECT;//命中
		m_mapAllEffects["GAP_DEDICANT_CORRECT"]=GAP_DEDICANT_CORRECT;//专注            
		m_mapAllEffects["GAP_NOR_DEF"]=GAP_NOR_DEF;//物理防御  
		m_mapAllEffects["GAP_MAGIC_DEF"]=GAP_MAGIC_DEF;//魔法抗性            
		m_mapAllEffects["GAP_NORATTACK_KILLRATE"]=GAP_NORATTACK_KILLRATE;//物理爆击         
		m_mapAllEffects["GAP_MAGIC_SPEED"]=GAP_MAGIC_SPEED;//施法速度          
		m_mapAllEffects["GAP_MIN_ATTACK"]=GAP_MIN_ATTACK;//最小攻击力
		m_mapAllEffects["GAP_MAX_ATTACK"]=GAP_MAX_ATTACK;//最大攻击力
		m_mapAllEffects["GAP_FUMO_PROPERTY"]=GAP_FUMO_PROPERTY;//魔法攻击力            
		m_mapAllEffects["GAP_MAGIC_KILLRATE"]=GAP_MAGIC_KILLRATE;//魔法爆击率          
		m_mapAllEffects["GAP_LEECH_HP"]=GAP_LEECH_HP;//生命吸收率            
		m_mapAllEffects["GAP_LEECH_MP"]=GAP_LEECH_MP;//法力吸收率      
		m_mapAllEffects["GAP_WILL_DEF"]=GAP_WILL_DEF;//意志抗性     
		m_mapAllEffects["GAP_CON_DEF"]=GAP_CON_DEF;//体质抗性
		m_mapAllEffects["GAP_BLED_DEF"]=GAP_BLED_DEF;//流血抗性
		m_mapAllEffects["GAP_DIZZY_DEF"]=GAP_DIZZY_DEF;//击昏抗性
		m_mapAllEffects["GAP_LULL_DEF"]=GAP_LULL_DEF;//麻痹抗性
		m_mapAllEffects["GAP_HYPNOSIS_DEF"]=GAP_HYPNOSIS_DEF;//催眠抗性
		m_mapAllEffects["GAP_POISON_DEF"]=GAP_POISON_DEF;//中毒抗性
		m_mapAllEffects["GAP_FIX_DEF"]=GAP_FIX_DEF;//定身抗性
		m_mapAllEffects["GAP_FIREDEF_CORRECT"]=GAP_FIREDEF_CORRECT;//火抗        
		m_mapAllEffects["GAP_WATERDEF_CORRECT"]=GAP_WATERDEF_CORRECT;//水抗
		m_mapAllEffects["GAP_EARTHDEF_CORRECT"]=GAP_EARTHDEF_CORRECT;//地抗 
		m_mapAllEffects["GAP_WINDDEF_CORRECT"]=GAP_WINDDEF_CORRECT;//风抗
		m_mapAllEffects["GAP_DARKDEF_CORRECT"]=GAP_DARKDEF_CORRECT;//黑暗抗
		m_mapAllEffects["GAP_HOLYDEF_CORRECT"]=GAP_HOLYDEF_CORRECT;//神圣抗
		m_mapAllEffects["GAP_HURT_SUCK_PER"] = GAP_HURT_SUCK_PER;//伤害吸收比例
		m_mapAllEffects["GAP_WEAPON_DAMAGE_LEVEL"] = GAP_WEAPON_DAMAGE_LEVEL;//武器强度
		m_mapAllEffects["GAP_LEECH_MP"] = GAP_LEECH_MP;//吸收Mp比例
		m_mapAllEffects["GAP_HURT2MP"] = GAP_HURT2MP;//损伤转Mp比例
		m_mapAllEffects["GAP_CRI_DAMAGE"] = GAP_CRI_DAMAGE;//物理暴击伤害
		m_mapAllEffects["GAP_CRI_DEF"] = GAP_CRI_DEF;//物理暴击抗性
		m_mapAllEffects["GAP_MCRI_DAMAGE"] = GAP_MCRI_DAMAGE;//魔法暴击伤害
		m_mapAllEffects["GAP_MCRI_DEF"] = GAP_MCRI_DEF;//魔法暴击抗性
		m_mapAllEffects["GAP_MAX_ENERGY_COR"] = GAP_MAX_ENERGY_COR;//能量上限修正
		m_mapAllEffects["GAP_ENERGY_RESUME_COR"] = GAP_ENERGY_RESUME_COR;//能量恢复速度修正
		m_mapAllEffects["GAP_ENERGY_SUCK"] = GAP_ENERGY_SUCK;//吸收能量
		m_mapAllEffects["GAP_PEOPLE_HURT_RATIO"] = GAP_PEOPLE_HURT_RATIO;//对人形类怪物伤害加成
		m_mapAllEffects["GAP_ELEMENT_HURT_RATIO"] = GAP_ELEMENT_HURT_RATIO;//对元素类怪物伤害加成
		m_mapAllEffects["GAP_DIFFRACE_HURT_RATIO"] = GAP_DIFFRACE_HURT_RATIO;//对异族类怪物伤害加成
		m_mapAllEffects["GAP_ANIM_HURT_RATIO"] = GAP_ANIM_HURT_RATIO;//对动物类怪物伤害加成
		m_mapAllEffects["GAP_SOUL_HURT_RATIO"] = GAP_SOUL_HURT_RATIO;//对亡灵类怪物伤害加成
		m_mapAllEffects["GAP_DEMON_HURT_RATIO"] = GAP_DEMON_HURT_RATIO;//对恶魔类怪物伤害加成
		m_mapAllEffects["GAP_INSECT_HURT_RATIO"] = GAP_INSECT_HURT_RATIO;//对昆虫类怪物伤害加成
		m_mapAllEffects["GAP_FIRE_HURT_RATIO"] = GAP_FIRE_HURT_RATIO;//对火系怪物伤害加成
		m_mapAllEffects["GAP_WATER_HURT_RATIO"] = GAP_WATER_HURT_RATIO;//对水系怪物伤害加成
		m_mapAllEffects["GAP_EARTH_HURT_RATIO"] = GAP_EARTH_HURT_RATIO;//对土系怪物伤害加成
		m_mapAllEffects["GAP_WIND_HURT_RATIO"] = GAP_WIND_HURT_RATIO;//对风系怪物伤害加成
		m_mapAllEffects["GAP_DARK_HURT_RATIO"] = GAP_DARK_HURT_RATIO;//对暗系怪物伤害加成
		m_mapAllEffects["GAP_HOLINESS_HURT_RATIO"] = GAP_HOLINESS_HURT_RATIO;//对圣系怪物伤害加成
		m_mapAllEffects["GAP_HURT_RID"] = GAP_HURT_RID;//免疫物理攻击
		m_mapAllEffects["GAP_MAGIC_HURT_RID"] = GAP_MAGIC_HURT_RID;//免疫魔法攻击
		m_mapAllEffects["GAP_SHIELD_DEF"] = GAP_SHIELD_DEF;//盾牌物理防御
		m_mapAllEffects["GAP_TIME_LIMIT"] = GAP_TIME_LIMIT;//限时
		m_mapAllEffects["GAP_ADDON_CHARM"] = GAP_ADDON_CHARM;//魅力
		m_mapAllEffects["GAP_HP_RESUME_RATE"] = GAP_HP_RESUME_RATE;//生命值恢复比例
		m_mapAllEffects["GAP_BUFF_ATK_STRENGTH"] = GAP_BUFF_ATK_STRENGTH;//状态攻击强度
		m_mapAllEffects["GAP_PENETRATE"] = GAP_PENETRATE;//攻击穿透
		m_mapAllEffects["GAP_EXCUSE_HURT"] = GAP_EXCUSE_HURT;//伤害减免
		m_mapAllEffects["GAP_LUCKY"] = GAP_LUCKY;//幸运
		m_mapAllEffects["GAP_ADDON_PENETRATE"] = GAP_ADDON_PENETRATE;//附加穿透攻击
		m_mapAllEffects["GAP_ADDON_EXCUSE_HURT"] = GAP_ADDON_EXCUSE_HURT;//附加伤害减免
		m_mapAllEffects["GAP_ADDON_LUCKY"] = GAP_ADDON_LUCKY;//附加幸运
		m_mapAllEffects["GAP_CUR_ATK_ED"] = GAP_CUR_ATK_ED;//强化攻击
		m_mapAllEffects["GAP_CUR_DEF_ED"] = GAP_CUR_DEF_ED;//强化防御
		m_mapAllEffects["GAP_ADDON_ATK_ED"] = GAP_ADDON_ATK_ED;//附加强化攻击
		m_mapAllEffects["GAP_ADDON_DEF_ED"] = GAP_ADDON_DEF_ED;//附加强化防御
		m_mapAllEffects["GAP_MIN_ATK_ADDON"] = GAP_MIN_ATK_ADDON;//小攻追加
		m_mapAllEffects["GAP_MAX_ATK_ADDON"] = GAP_MAX_ATK_ADDON;//大攻追加
		m_mapAllEffects["GAP_MAGIC_ATK_ADDON"] = GAP_MAGIC_ATK_ADDON;//魔攻追加
		m_mapAllEffects["GAP_DEF_ADDON"] = GAP_DEF_ADDON;		//物防追加
		m_mapAllEffects["GAP_MAGIC_DEF_ADDON"] = GAP_MAGIC_DEF_ADDON;//魔防追加
	}
}

// 从文件读取原始物品列表
bool CGoodsList::LoadGoodsList(const char* filename)
{
	m_mapGoodsList.clear();
	m_mapGoodsListByName.clear();

	CRFile* prfile = rfOpen(filename);	
	if( prfile == NULL )
	{
		char str[256];
		sprintf(str, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return false;
	}

	// 文件头和版本
	char szHeader[6];
	memset(szHeader, 0, 6);
	prfile->ReadData(szHeader, 5);
	if( strcmp(szHeader, "GOODS") != 0 )
	{
		char str[256];
		sprintf(str, "file '%s' is not goodslist file!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		rfClose(prfile);
		return false;
	}

	long lVersion = 0;
	prfile->ReadData(&lVersion, sizeof(long));

	// 具体内容

	long lNum = 0;
	prfile->ReadData(&lNum, sizeof(long));
	for(int i=0; i<lNum; i++)
	{
		long lStrSize=0;
		tagGoods2 stGoods;

		prfile->ReadData(&stGoods.BaseProperty.dwIndex, sizeof(stGoods.BaseProperty.dwIndex));

		prfile->ReadData(&lStrSize, sizeof(long));
		char *pStrOrginName=new char[lStrSize+1];	
		prfile->ReadData(pStrOrginName, lStrSize);
		pStrOrginName[lStrSize]='\0';
		stGoods.BaseProperty.strOrginName=pStrOrginName;
		SAFE_DELETE(pStrOrginName);
		prfile->ReadData(&lStrSize, sizeof(long));
		char *pStrName=new char[lStrSize+1];

		prfile->ReadData(pStrName, lStrSize);
		pStrName[lStrSize]='\0';
		stGoods.BaseProperty.strName=pStrName;
		SAFE_DELETE(pStrName);

		prfile->ReadData(&stGoods.BaseProperty.dwIconId, sizeof(stGoods.BaseProperty.dwIconId));		//	界面图形ID
		prfile->ReadData(&stGoods.BaseProperty.dwGroundId, sizeof(stGoods.BaseProperty.dwGroundId));		//	落地后图形ID
		prfile->ReadData(&stGoods.BaseProperty.dwEquipID, sizeof(stGoods.BaseProperty.dwEquipID));		//	换装图形ID
		prfile->ReadData(&stGoods.BaseProperty.dwSound, sizeof(stGoods.BaseProperty.dwSound));		//	拾取声音
		prfile->ReadData(&stGoods.BaseProperty.dwSoundID1, sizeof(stGoods.BaseProperty.dwSoundID1));		//	挥动/弱伤害声音
		prfile->ReadData(&stGoods.BaseProperty.dwSoundID2, sizeof(stGoods.BaseProperty.dwSoundID2));		//	特殊攻击/击中声音
		prfile->ReadData(&stGoods.BaseProperty.bSoundSwitch, sizeof(stGoods.BaseProperty.bSoundSwitch));	//	攻击时是否混音
		prfile->ReadData(&stGoods.BaseProperty.dwWeaponActType, sizeof(stGoods.BaseProperty.dwWeaponActType));//	武器对应的动作类型
		prfile->ReadData(&stGoods.BaseProperty.dwType, sizeof(stGoods.BaseProperty.dwType));				// 类型
		prfile->ReadData(&stGoods.BaseProperty.dwPrice, sizeof(stGoods.BaseProperty.dwPrice));				// 金币价格
		prfile->ReadData(&stGoods.BaseProperty.dwSilverPrice, sizeof(stGoods.BaseProperty.dwSilverPrice));	// 银币价格
		//prfile->ReadData(&stGoods.BaseProperty.dwWeight, sizeof(stGoods.BaseProperty.dwWeight));

		//中文描述内容
		prfile->ReadData(&lStrSize, sizeof(long));
		char *pStrContent=new char[lStrSize+1];	
		prfile->ReadData(pStrContent, lStrSize);
		pStrContent[lStrSize]='\0';
		stGoods.BaseProperty.strContent=pStrContent;
		SAFE_DELETE(pStrContent);

		long lEffectNum = 0;
		prfile->ReadData(&lEffectNum, sizeof(long));
		ushort wType=0;
		for(int j=0;j<lEffectNum;j++)
		{
			tagEffect stEffect;

			prfile->ReadData(&wType/*&stEffect.wType*/,sizeof(ushort));
			stEffect.wType=wType+1;
			prfile->ReadData(&stEffect.bEnable,sizeof(bool));
			prfile->ReadData(&stEffect.bHide,sizeof(bool));
			prfile->ReadData(&stEffect.dwValue1,sizeof(ulong));
			prfile->ReadData(&stEffect.dwValue2,sizeof(ulong));
			//prfile->ReadData(&stEffect.wOdds,sizeof(ushort));
			stGoods.vectorEffect.push_back(stEffect);
		}		

		//////////////////读套装属性/////////////////////////
		prfile->ReadData(&lEffectNum, sizeof(long));
		for(int i=0;i<lEffectNum;i++)
		{
			tagGoodsBeControlledAttri stSuitSingleGoodsAttri;

			prfile->ReadData(&wType,sizeof(ushort));
			stSuitSingleGoodsAttri.wPropertyName = wType+1;
			prfile->ReadData(&(stSuitSingleGoodsAttri.dwValue1),sizeof(ulong));
			prfile->ReadData(&(stSuitSingleGoodsAttri.dwValue2),sizeof(ulong));
			prfile->ReadData(&(stSuitSingleGoodsAttri.wActiveNum),sizeof(ushort));

			stGoods.m_SuitProperty.push_back(stSuitSingleGoodsAttri);
		}

		////////////////属性激发的属性////////////////////
		prfile->ReadData(&lEffectNum, sizeof(long));
		for(int i=0;i<lEffectNum;i++)
		{
			tagGoodsBeControlledAttri stGoodsAktiviertAttri;

			prfile->ReadData(&wType,sizeof(ushort));
			stGoodsAktiviertAttri.wPropertyName = wType+1;
			prfile->ReadData(&(stGoodsAktiviertAttri.dwValue1),sizeof(ulong));
			prfile->ReadData(&(stGoodsAktiviertAttri.dwValue2),sizeof(ulong));
			prfile->ReadData(&(stGoodsAktiviertAttri.wActiveNum),sizeof(ushort));

			stGoods.m_AkiviertProperty.push_back(stGoodsAktiviertAttri);
		}

		//////////////////////////////////////////////////////
		m_mapGoodsList[stGoods.BaseProperty.dwIndex] = stGoods;
		/////////////测试信息////////////
		//if (stGoods.BaseProperty.dwIndex==32)
		//{
		//	char buf[256];
		//	vector<CGoodsList::tagEffect>::iterator Iter = stGoods.vectorEffect.begin();
		//	for(;Iter!=stGoods.vectorEffect.end();Iter++)
		//	{
		//		PutDebugString(ltoa(Iter->wType,buf,10));
		//		PutDebugString(ltoa(Iter->dwValue1,buf,10));
		//	}
		//}
		////////////////////////////////
		m_mapGoodsListByName[stGoods.BaseProperty.strOrginName] = &m_mapGoodsList[stGoods.BaseProperty.dwIndex];
	}

	rfClose(prfile);
	return true;
}

//从文件读取套装属性列表
void CGoodsList::LoadSuitList(const char* filename)
{
	InitMapEffects();
	m_SuitList.clear();
	CRFile* prfile = rfOpen(filename);

	if (prfile == NULL)
	{
		char str[256];
		sprintf(str, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return ;
	}
	stringstream  stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	ulong suitID = 0;						//存放套装的ID号
	string strSuitName;						//存放套装的名称 
	while (ReadTo(stream,"SUIT_ID"))
	{
		stream>>suitID;
		stream>>strSuitName;
		m_mapSuitName.insert(pair<ulong,string>(suitID,strSuitName));
		if(ReadTo(stream,"<SUIT_GOODS>"))
		{		
			tagSuitAttribute tempSuit;
			//获取tagSuitAttribute中vector<string> Suit_OriginName和uchar	wSuit_Count要存放的内容
			string tempString;
			while (true)				
			{					
				stream>>tempString;
				if (tempString == "</SUIT_GOODS>")
				{
					break;
				}
				tempSuit.wSuit_Count = (uchar)atoi(tempString.c_str());
				string tempSuitOriginName;
				stream>>tempSuitOriginName;
				tempSuit.Suit_OriginName.push_back(tempSuitOriginName);
			}
			//获取map<ulong,map<ulong,tagEffect>>中要存放的内容
			ulong SuitNum;				//套装可能的件数
			string SuitAttriName;		//套装在对应件数下激活的属性名字
			//ulong SuitAttriValue1,SuitAttriValue2;		//对应的附加属性的两个值
			while (ReadTo(stream,"NUM"))
			{
				stream>>SuitNum;			
				if (ReadTo(stream,"<ATTR>"))
				{	
					map<ulong,tagEffect> tempEffects;
					tagEffect tEffect;
					while (true)		
					{
						stream>>tempString;
						if (tempString == "</ATTR>"||tempString == "</ATTR_END>")
						{
							break;
						}
						SuitAttriName = tempString;
						stream>>tEffect.dwValue1>>tEffect.dwValue2;										//读取属性名和属性值
						ulong SuitAttriEnum;
						SuitAttriEnum = m_mapAllEffects[SuitAttriName];					//通过属性名找到对应的属性枚举值
						tempEffects.insert(pair<ulong,tagEffect>(SuitAttriEnum,tEffect));	
					}
					tempSuit.Suit_ActiveProperty[SuitNum] = tempEffects;
					if (tempString == "</ATTR_END>")
					{
						break;
					}
				}
			}
			m_SuitList.insert(pair<ulong,tagSuitAttribute>(suitID,tempSuit));
		}
	}
}

// 从文件读取套卡列表信息
void CGoodsList::LoadCardList(const char* filename)
{
	InitMapEffects();
	m_mapSeriesCardList.clear();
	CRFile* prfile = rfOpen(filename);

	if (prfile == NULL)
	{
		char str[256];
		sprintf(str, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return ;
	}
	stringstream  stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	ulong cardID = 0;							//存放套卡的ID号
	string CardAttriName = "";					//套卡激活的属性名字
	ulong CardAttriValue = 0;					//对应的属性的值
	ulong CardAttriEnum = 0;					//属性对应的枚举值

	while (ReadTo(stream,"<KIT_CARD>")&&!stream.eof())
	{
		char buf[256];
		int num = 0;
		tagSeriesCardProperty	tempCardProperty;
		if (ReadTo(stream,"KIT_CARD_ID"))
		{
			stream>>cardID;
			Log4c::Trace(ROOT_MODULE,ltoa(cardID,buf,10));
		}
		if (ReadTo(stream,"HOLE_NUM_REQUIRE"))
		{
			stream>>num;
			tempCardProperty.HOLE_NUM_REQUIRE = (uchar)num;
			Log4c::Trace(ROOT_MODULE,ltoa((long)tempCardProperty.HOLE_NUM_REQUIRE,buf,10));
		}
		if (ReadTo(stream,"CARD_NUM_REQUIRE"))
		{
			stream>>num;
			tempCardProperty.CARD_NUM_REQUIRE = (uchar)num;	
			Log4c::Trace(ROOT_MODULE,ltoa((long)tempCardProperty.CARD_NUM_REQUIRE,buf,10));
		}
		if(ReadTo(stream,"CARD_ID"))
		{		
			ulong tempCardID = 0;
			for (uchar i= 0;i<tempCardProperty.CARD_NUM_REQUIRE;i++)
			{
				stream>>tempCardID;
				Log4c::Trace(ROOT_MODULE,ltoa(tempCardID,buf,10));
				tempCardProperty.CardsID.push_back(tempCardID);
			}
		}
		if (ReadTo(stream,"<ATTR>"))
		{
			string tempString = "";
			while(true)
			{
				stream>>tempString;
				Log4c::Trace(ROOT_MODULE,tempString.c_str());
				if (tempString=="</ATTR>") break;
				if (tempString[0]=='/') continue;
				CardAttriName = tempString;
				stream>>CardAttriValue;
				Log4c::Trace(ROOT_MODULE,ltoa((long)CardAttriValue,buf,10));
				CardAttriEnum = m_mapAllEffects[CardAttriName];					//通过属性名找到对应的属性枚举值
				tempCardProperty.CardsEffects.insert(pair<ulong,int>(CardAttriEnum,CardAttriValue));
			}
		}
		if (ReadTo(stream,"</KIT_CARD>")) 
		{
			m_mapSeriesCardList[cardID] = tempCardProperty;
		}
	}
}

bool CGoodsList::FillGoodsInfo(ulong dwIndex,CGoods *pGood)
{	
	if(!dwIndex)return false;
	tagBaseProperty p=GetProperty(dwIndex)->BaseProperty;
	pGood->SetName(p.strName.c_str());
	pGood->m_dwIndex=dwIndex;
	//pGood->m_lNum=1;
	pGood->m_stBaseProperty.strName=p.strName;
	pGood->m_vectorEffect=GetProperty(dwIndex)->vectorEffect;
	pGood->m_stBaseProperty.dwValue = p.dwPrice;
	pGood->m_stBaseProperty.dwSilverPrice = p.dwSilverPrice;
	pGood->m_stBaseProperty.dwType = p.dwType;
	pGood->SetGoodsBaseType((eGoodsBaseType)p.dwType);
	return true;
}

long CGoodsList::GetEffectVal(ulong dwIndex,ushort wType,int nWhich)
{
	CGoodsList::tagGoods2* pstGoods2=CGoodsList::GetProperty(dwIndex);
	if (pstGoods2)
	{
		vector<CGoodsList::tagEffect>::iterator it=pstGoods2->vectorEffect.begin();
		for(;it!=pstGoods2->vectorEffect.end();it++)
		{
			if(it->wType==wType)
			{
				if(nWhich==1)
					return it->dwValue1;
				else if(nWhich==2)
					return it->dwValue2;
			}
		}
	}
	return 0;
}

CGoodsList::tagEffect* CGoodsList::GetEffectTag(ulong dwIndex,ushort wType)
{
	CGoodsList::tagGoods2* pstGoods=CGoodsList::GetProperty(dwIndex);
	if(pstGoods)
	{
		vector<CGoodsList::tagEffect>::iterator it=pstGoods->vectorEffect.begin();
		for(;it!=pstGoods->vectorEffect.end();it++)
		{
			if(it->wType == wType)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

bool CGoodsList::InitMoneyGraphics(const char* filename)
{
	m_stMoneyGraphics.clear();
	m_stSilverMoneyGraphics.clear();

	tagMoneyGraphics stMoney;

	stringstream stream;	
	CRFile* prfile = rfOpen(filename);
	if( !prfile )
	{
		char str[256];
		sprintf(str, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return false;
	}
	prfile->ReadToStream(stream);
	rfClose(prfile);
	int iLoopNum = 0;
	if (ReadTo(stream,"GoldMoney"))
	{
		stream>>iLoopNum;
		for (int i = 0;i<iLoopNum;i++)
		{
			if (ReadTo(stream,"#"))
			{
				stream >> stMoney.lMinNum >> stMoney.lMaxNum >> stMoney.lGraphicsID;
				m_stMoneyGraphics.push_back(stMoney);
			}
		}
	}
	if (ReadTo(stream,"SilverMoney"))
	{
		stream>>iLoopNum;
		for (int i = 0;i<iLoopNum;i++)
		{
			if (ReadTo(stream,"#"))
			{
				stream >> stMoney.lMinNum >> stMoney.lMaxNum >> stMoney.lGraphicsID;
				m_stSilverMoneyGraphics.push_back(stMoney);
			}
		}
	}

	return true;
}

long CGoodsList::GetMoneyGraphics(long lMoneyNum,int iMoneyType)
{
	vector<tagMoneyGraphics> t_vecMoney;
	// 金币
	if (iMoneyType==GBT_MONEY)
	{
		t_vecMoney = m_stMoneyGraphics;
	}
	// 银币
	else if (iMoneyType==GBT_SILVERMONEY)
	{
		t_vecMoney = m_stSilverMoneyGraphics;
	}
	vector<tagMoneyGraphics>::iterator iter = t_vecMoney.begin();
	for (;iter!=t_vecMoney.end();iter++)
	{
		if (lMoneyNum>=iter->lMinNum&&lMoneyNum<=iter->lMaxNum)
		{
			return iter->lGraphicsID;
		}
	}
	return 0;
}

// 根据装备换装编号获取物品属性
CGoodsList::tagGoods2*	CGoodsList::GetPropertyByEquipID(ulong dwEquipID)
{
	for (map<ulong, tagGoods2>::iterator it=m_mapGoodsList.begin(); it!=m_mapGoodsList.end(); it++)
	{
		if (it->second.BaseProperty.dwEquipID == dwEquipID)
		{
			return &it->second;
		}
	}
	return NULL;
}

//获取此ID的套装物品的自身属性(nWhich = 1 取套装激发物品的某个属性值1， = 2取属性值2，= 3取属性值3（第三个是激发此属性需达到的套装物品个数）)
long CGoodsList::GetSuitAttrOfGoods(ulong dwIndex,ulong wType,int nWhich)
{
	CGoodsList::tagGoods2* pstGoods2=CGoodsList::GetProperty(dwIndex);
	if (pstGoods2)
	{
		vector<CGoodsList::tagGoodsBeControlledAttri>::iterator it=pstGoods2->m_SuitProperty.begin();
		for(;it!=pstGoods2->m_SuitProperty.end();it++)
		{
			if(it->wPropertyName==wType)
			{
				if(nWhich==1)
					return it->dwValue1;
				else if(nWhich==2)
					return it->dwValue2;
				else if(nWhich == 3)
					return it->wActiveNum;
			}
		}
	}
	return 0;
}

//获取通过套装ID查找对应的套装名称
string& CGoodsList::GetSuitName(ulong dwSuitID)
{
	if (dwSuitID!=0&&m_mapSuitName.find(dwSuitID)!=m_mapSuitName.end())
	{
		return m_mapSuitName[dwSuitID];
	}
	return m_mapSuitName[0];
}

//获取此ID的套装的总件数
WORD CGoodsList::GetSuitCount(ulong SuitID)	
{
	if (SuitID!=0&&m_SuitList.find(SuitID)!=m_SuitList.end())
	{
		return m_SuitList[SuitID].wSuit_Count;
	}
	return 0;
}

//根据某个卡片的ID查找此类套卡的ID
ulong CGoodsList::GetSeriesCardIndexByCardID(ulong cardIndex)
{
	map<ulong/*套卡的ID号*/,tagSeriesCardProperty>::iterator it = m_mapSeriesCardList.begin();
	for (;it!=m_mapSeriesCardList.end();it++)
	{
		vector<ulong> temp = m_mapSeriesCardList[it->first].CardsID;
		for (size_t i = 0;i<temp.size();i++)
		{
			if (cardIndex==temp[i])
			{
				return it->first;
			}
		}
	}
	return 0;
}

/// 是否有此附加属性
bool CGoodsList::HaveEffect(ulong dwGoodsIndex,ushort wEffectType)
{
	tagGoods2* ptGoods = GetProperty(dwGoodsIndex);
	if (ptGoods!=NULL)
	{
		vector< CGoodsList::tagEffect >::iterator	iter;
		for( iter = ptGoods->vectorEffect.begin(); iter != ptGoods->vectorEffect.end(); iter++ )
		{
			if( iter->wType == wEffectType )
			{
				return true;
			}
		}
	}

	return false;
}

// 判断是否能装备
bool CGoodsList::IsCanEquip(CGoods *pGoods)
{
	if(pGoods)
	{
		ulong dwType = pGoods->GetProperty()->dwType;
        if( dwType == GBT_EQUIPMENT)
			return true;
	}
	return false;
}

/// 获取物品的时限
const string CGoodsList::GetGoodsLimitTime(ulong dwGoodsIndex)
{
	//消失时间
	long val1 = GetEffectVal(dwGoodsIndex,GAP_TIME_LIMIT,1);
	long val2 = GetEffectVal(dwGoodsIndex,GAP_TIME_LIMIT,2);		
	long val3 = 0;
	long val4 = 0;
	long val5 = 0;
	char scResult[64] = "";
	char strTime[64]="";char strHour[32] = "";char strSecond[32] = "";
	/// 值1为1时表示物品总的有效时限
	if(val1==1)
	{		
		val3 = val2/1440;  val4 = val2%1440;	/// 算天数
		if (val3!=0)
		{
            sprintf(strTime,AppFrame::GetText("Goods_447"),val3);
		}
		val3 = val4/60; val5 = val4%60;			/// 算小时、分
		if (val3!=0)
		{
            sprintf(strHour,AppFrame::GetText("Goods_448"),val3);
			strcat(strTime,strHour);
		}
		if (val5!=0)
		{
            sprintf(strSecond,AppFrame::GetText("Goods_449"),val5);
			strcat(strTime,strSecond);
		}
        sprintf(scResult,AppFrame::GetText("Goods_445"),strTime);
		//strResult = scResult;
	}
	return string(scResult);
}

// 根据传入的物品获得对应的品质图片以及颜色
void CGoodsList::GetGoodsQuality(ulong dwIndex,string & str, long & color)
{
	str = "";
	color = 0xffffffff;
	switch( GetEffectVal( dwIndex, GAP_QUALITY, 1 ) )
	{
	case 2:		// 蓝色品质
		{
			str = "datafiles/imagesets/public/icon_00A2FF.tga";
			color = 0xff00A2FF;
		}
		break;
	case 3:		// 绿色品质
		{
			str = "datafiles/imagesets/public/icon_00FF19.tga";
			color = 0xff00FF19;
		}
		break;
	case 4:		// 紫色品质
		{
			str = "datafiles/imagesets/public/icon_8400FF.tga";
			color = 0xff8400FF;
		}
		break;
	case 5:
		{
			str = "datafiles/imagesets/public/icon_FF00F0.tga";
			color = 0xffff00f0;
		}
		break;
	case 6:
		{
			str = "datafiles/imagesets/public/icon_dab26c.tga";
			color = 0xffdab26c;
		}
		break;
	case 7:
		{
			str = "datafiles/imagesets/public/icon_FF7200.tga";
			color = 0xffff7200;
		}
		break;
	case 8:
		{
			str = "datafiles/imagesets/public/icon_FF0000.tga";
			color = 0xffff0000;
		}
		break;
	case 9:		//家族跑商的物品专用边框
		{
			str = "datafiles/imagesets/public/icon_Tradables.tga";
			color = 0xffff0000;
		}
		break;
	}
}