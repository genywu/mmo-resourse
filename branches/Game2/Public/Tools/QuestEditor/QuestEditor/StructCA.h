//定义一个任务属性的结构体
#ifndef _StructCA_H
#define _StructCA_H
struct tagRoleAttribute 
{
	//任务ID
	int			m_nID;
	//
	int			m_nRoleEmbracer;
	//任务难度
	int			m_nHardFactor;
	//删除任务的脚本
	string		m_cDeleteScript;
	//任务名称
	string		m_cName;
	//接受任务者效果
	int			m_nEmbracerEffect;
	//完成任务后接受者的效果
	int			m_nFinishEmbracerEffect;
	//任务接受者地图效果
	int			m_nEmbracerMapEffect;
	//是否被记录
	int			m_nBeRecord;
	//是否能显示
	int			m_nShowAble;
	//稀有度
	int			m_nOdds;
	//集结点
	int			m_nMassPoint;
	//描述任务的文本id
	CString		m_nDiscribTextID;
	//最小等级限制
	int			m_nMiniGrade;
	//最大等级限制
	int			m_nMaxGrade;
	//是否能重复
	int			m_nRepeated;
	//死亡后任务是否失败
	int			m_nDeadLost;
	//死亡后是否能够重新接任务
	int			m_nLostRepeated;
	//需要的佣兵声望
	int			m_nEngageArmsRepute;
	//章数_____主线特有
	int			m_nChapterNum;
	//图片_______主线特有
	int			m_nPicture;
	//图标_________主线特有
	int			m_nIcon;
	//描述文本ID_______主线任务文本
	CString		m_nStoryDesc;
	//提示类型
	int			m_nNPCShowType;
	//NPC的原始名
	string		m_cNPCID;
	//任务完成时的特效
	int			m_nFinishEffect;
	//任务线索类型
	int			m_nNPCClewType;
	//查询任务描述索引
	CString			m_nBeSearchDesc;
	//查询任务简术索引
	CString			m_nBeSearchSpDesc;
	//前序任务索引
	CString			m_nRoleIndex;
    //后序任务
    int         m_NextQuest;

    // add by:Expter
    //任务类型
    long        m_QuestType;
    //可否共享任务
    long        m_ShareQuest;
    //携带宠物
    long        m_TotePet;
    //任务性别限制
    long        m_QuestSex;
    //任务稀有度
    long        m_Odds;
    //技能限定
    long        m_ReqSkill;
    //职业限定
    string      m_Occupation;    
    //副职业限定
    string      m_Doccu;
    
	tagRoleAttribute()
		:m_nID(0)
		,m_QuestType(1)
		,m_nRoleEmbracer(1)
		,m_nHardFactor(-1)
		,m_cDeleteScript(_T("scripts/questmodel/quest_model_del_3.lua"))
		,m_cName(_T("未知任务名"))
		,m_nEmbracerEffect(-1)
		,m_nFinishEmbracerEffect(-1)
		,m_nEmbracerMapEffect(-1)
		,m_nBeRecord(0)
		,m_nShowAble(1)
		,m_nOdds(-1)
		,m_nMassPoint(-1)
		,m_nMiniGrade(1)
		,m_nMaxGrade(999)
		,m_nRepeated(0)
		,m_nDeadLost(-1)
		,m_nLostRepeated(-1)
		,m_nEngageArmsRepute(0)
		,m_nChapterNum(-1)
		,m_nPicture(-1)
		,m_nIcon(-1)
		,m_nStoryDesc(_T(""))
		,m_nNPCShowType(-1)
		,m_cNPCID(_T(""))
		,m_nFinishEffect(-1)
		,m_nNPCClewType(0)
        ,m_NextQuest(-1)
	{
        m_ShareQuest = 0;
        m_TotePet = m_QuestSex = m_Odds  = m_ReqSkill = -1;
        m_nBeSearchDesc="";
        m_nBeSearchSpDesc="";
        m_nDiscribTextID="";
        m_nRoleIndex="";
	}

    tagRoleAttribute(const tagRoleAttribute & Attribute)
        :m_nID(Attribute.m_nID)
        ,m_QuestType(Attribute.m_QuestType)
        ,m_nRoleEmbracer(Attribute.m_nRoleEmbracer)
        ,m_nHardFactor(Attribute.m_nHardFactor)
        ,m_cDeleteScript(Attribute.m_cDeleteScript)
        ,m_cName(Attribute.m_cName)
        ,m_nEmbracerEffect(Attribute.m_nEmbracerEffect)
        ,m_nFinishEmbracerEffect(Attribute.m_nFinishEmbracerEffect)
        ,m_nEmbracerMapEffect(Attribute.m_nEmbracerMapEffect)
        ,m_nBeRecord(Attribute.m_nBeRecord)
        ,m_nShowAble(Attribute.m_nShowAble)
        ,m_nOdds(Attribute.m_nOdds)
        ,m_nMassPoint(Attribute.m_nMassPoint)
        ,m_nMiniGrade(Attribute.m_nMiniGrade)
        ,m_nMaxGrade(Attribute.m_nMaxGrade)
        ,m_nRepeated(Attribute.m_nRepeated)
        ,m_nDeadLost(Attribute.m_nDeadLost)
        ,m_nLostRepeated(Attribute.m_nLostRepeated)
        ,m_nEngageArmsRepute(Attribute.m_nEngageArmsRepute)
        ,m_nChapterNum(Attribute.m_nChapterNum)
        ,m_nPicture(Attribute.m_nPicture)
        ,m_nIcon(Attribute.m_nIcon)
        ,m_nStoryDesc(Attribute.m_nStoryDesc)
        ,m_nNPCShowType(Attribute.m_nNPCShowType)
        ,m_cNPCID(Attribute.m_cNPCID)
        ,m_nFinishEffect(Attribute.m_nFinishEffect)
        ,m_nNPCClewType(Attribute.m_nNPCClewType)
        ,m_NextQuest(Attribute.m_NextQuest)
    {
        m_ShareQuest = Attribute.m_ShareQuest;
        m_TotePet    = Attribute.m_TotePet;
        m_QuestSex   = Attribute.m_QuestSex;
        m_Odds       = Attribute.m_Odds;
        m_ReqSkill   = Attribute.m_ReqSkill;
        m_nBeSearchDesc=Attribute.m_nBeSearchDesc;
        m_nBeSearchSpDesc=Attribute.m_nBeSearchSpDesc;
        m_nDiscribTextID=Attribute.m_nDiscribTextID;
        m_nRoleIndex=Attribute.m_nRoleIndex;
    }

	~tagRoleAttribute()
	{
	}

    /// 任务编辑器的初始化映射模块
    void  Init();

};
#endif

