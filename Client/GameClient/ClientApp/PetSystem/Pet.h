/** 
* @file   Pet.h
* @author Nikai
* @brief  客户端宠物系统.
*
* @date   2008-11      
*/

#pragma once
#include "../../../GameClient/ClientApp/MoveShape.h"
#include "../../../Public/Common/BasePetDef.h"

/// 宠物技能结构
struct	tagPetSkillInfo:public tagSkill	
{
    long	lStartCoolTime;				///< 当前冷却时间
    long	lRestorTimeLength;			///< 技能总冷却时间长度
    long	lMinAtkDistance;			///< 最小攻击距离
    long	lMaxAtkDistance;			///< 最大攻击距离
    long	lCostMp;					///< 需要花费多少MP值
    long	lSkillStartTime;			///< 技能开始时间
    long	lSkillActTime;				///< 技能动作时间
    bool	bIsUsingByHotKey;			///< 当前是否由快捷键使用
    bool	bIsSkillUsingEnd;			///< 技能使用是否结束
    bool	bSkillIsChanged;			///< 当技能被改变标志 （设置或者取消自动释放，或者设置到动作条 被视为被改变）
    tagPetSkillInfo(void)
        :lStartCoolTime(0)
        ,bIsUsingByHotKey(false)
        ,bIsSkillUsingEnd(true)
        ,bSkillIsChanged(false)
        ,lSkillStartTime(-1)
        ,lSkillActTime(-1)
    {}
};

class CFloatMove;
class CPet:public CMoveShape
{
    //宠物的战斗状态（沉睡、待机、出战、激活）
public:
    PET_STATE GetPetState() const {return (PET_STATE)m_tagPetDiff->lStateType;}
    void SetPetState(PET_STATE s);

public:
    const static float FAR_FROM_TARGET_DISTANCE;		//与目标最大允许距离

    /// 在战斗模式下的返回值
    enum eFightModeRet
    {
        FMR_NO_SKILL				= 0,				///< 没可能用技能
        FMR_SKILL_CD_NOT_ARRIVAL	= 1,				///< 技能冷却时间未到
        FMR_SKILL_NOT_ENOUGH_MP		= 2,				///< 技能所需MP不足
        FMR_SKILL_ACTION_NOT_FINISH = 3,				///< 技能动作没完成
        FMR_SKILL_IS_NOT_AUTO		= 4,				///< 技能不是自动释放
        FMR_SKILL_LEVEL_FAIL		= 5,				///< 当前等级某种条件失败
        FMR_Trace_Near				= 10,				///< 继续追踪 太近
        FMR_Trace_Far				= 11,				///< 继续追踪 太远
        FMR_Attack					= 12,				///< 攻击
        FMR_No_TARGET				= 13,				///< 没有攻击目标 或者攻击目标死亡
        FMR_SKILL_NOT_FINISH		= 14,				///< 技能没结束
        FMR_Other,										///< 其他情况
    };

private:
    /// 宠物停留方向位置
    struct tagStandInfo			
    {
        bool			bOccupy;
        CShape::eDIR	eDirPos;
        PET_TYPE		ePetType;
        tagStandInfo():bOccupy(false),eDirPos(CShape::DIR_DOWN){}
    };

protected:
    static vector<tagStandInfo>	vecPetStandDirPos;

    PET_TYPE m_PetType;		///< 宠物类型
    CGUID m_MasterGUID;		///< 主人的GUID
    CPlayer* m_Host;		///< 主人
    bool m_bIsActived;		///< 是否被激活

    tagPetDiff* m_tagPetDiff;				///< 宠物差异属性数据
    tagPetTemp* m_tagPetDetail;				///< 宠物逻辑属性

    map<long ,tagPetSkillInfo> m_PetSkillMap;	//宠物技能表	lpos 代表 技能槽位置。 （0-3）天赋技能（4-11）普通技能

    long						m_lDistanceFromTarget;			//与目标之间的距离

    float						m_lStartDestX;			//  开始跟随的初始x位置
    float						m_lStartDestY;			//  开始跟随的初始y位置

    long						m_lLastPosX;		//上一次坐标X
    long						m_lLastPosY;		//上一次坐标Y
    long						m_lLastDestX;		//上一次目的地的X
    long						m_lLastDestY;		//上一次目的地的Y

    float						m_lStayModePosX;	//停留模式下记录的X
    float						m_lStayModePosY;	//停留模式下记录的Y

    CGUID						m_curAtkID_Not_Find;		//宠物当前攻击目标的GUID （客户端找到对应shape的时候清除）
    CMoveShape				*	m_pCurAttackShape;			//当前攻击的目标
    CMoveShape				*	m_pPassiveAtkTarget;		//被动攻击模式下的目标

    bool						m_bPassiveForceToFight;				//宠物在被动模式强制攻击标志，由玩家控制状态

    bool						m_bPetIsRetaliateUponNextShape;		//宠物是否自动攻击下一个目标标志  （当宠物处于防御和主动模式下，主人或宠物收到伤害触发 ）

    long						m_lCurUseSkillPos;					//当前使用技能的位置
    long						m_lLastUseSkillPos;					//上一次使用技能的位置

    DWORD						m_dwSkillPublicBeginCoolTime;		//当前公共技能冷却时间

    DWORD						m_MoveMsgNum;				//客户端发送移动的消息数量
    DWORD						m_dwStopMoveMsgNum;			//客户端发送停止移动消息数量
    DWORD						m_dwSkillMsgSendNum;		//宠物使用技能发送消息数量
    DWORD						m_dwSkillSuccMsgRecvNum;	//宠物技能使用成功收到消息
    DWORD						m_dwSkillFailMsgRecvNum;	//技能使用失败消息
    DWORD						dwFightBackMsgNum;			//收到触发反击的消息（收到伤害 或者 变更为攻击模式）
    DWORD						dwForceMove;				//收到强制移动
    long						m_lFightAImode;				//宠物当前战斗AI类型

    bool						m_bPetSkillBechanged;		//宠物技能被改变标志（当技能表中有一个被改变被视为已经改变）
    bool						m_bIsInBornState;			//宠物出生状态。
public:
	CPet();
	virtual ~CPet();
	void	RendShadow(bool bsimple = false);
	long	AutoMoveAI_A(float fDestX,float fDestY);		//自动寻路到目标(A*)
	

	long	FollowObjectAI(CShape *pTarget);			//跟随目标 非A*
	long	FollowObjectAI(long lDx , long lDy);		//跟随目标 非A*

private:
    CFloatMove  *m_FloatMove;
    //在自动移动AMM_TO_DESTDIR模式下，记录曾经移动过的格子。
    //如果发现格子是曾经移动过的格子，则玩家自动停止
    typedef map<POINT,long,point_compare>::iterator itMovedCell;
    map<POINT,long,point_compare>	m_MovedCell;
    char    strMoveInfo[256];   //调试信息
    bool	AddMovedCell(long lTileX,long lTileY);		                    //添加移动过的格子
	void	MoveToCell(float fDestX, float fDestY);		//移动到指定格子  bFlag 是否需要发消息
	long	GetNextMoveDestCell(float fX,float fY,long lMaxDistance,float &fDestX,float &fDestY);//得到某个点方向上下一个移动目标点
    long	GetNextMoveDestCell(float fRadianDir,float fMaxDistance,float &fDestX,float &fDestY);//得到一个方向上的下一个移动目标点
public:
    void AI(void);
    virtual	bool Display();
    virtual void UpdateProperty(CMessage *pMsg);

    //主人GUID
    CGUID	GetMasterGUID()				{return m_MasterGUID;}
    //获得主人指针
    CPlayer *GetMaster();
    /// 设置主人
    void SetHost(CPlayer* pPlayer);

    /// 取得宠物名称
    const char* GetPetName(void) const { return m_tagPetDiff->szName; }
    /// 设置宠物名称
    void SetPetName(const char* name) { strcpy_s(m_tagPetDiff->szName, 32, name); }

    //激活
    void	SetPetIsActive(bool b)	{m_bIsActived = b;}
    const	bool	IsActive()				{return	m_bIsActived;}

    //行为模式
    void SetPetBehavior(PET_MOVE_STYLE  pms);
    PET_MOVE_STYLE GetPetBehavior()			{return (PET_MOVE_STYLE)m_tagPetDetail->lMoveType;}

    //攻击模式
    void SetPetFightMode(PET_FIGHT_STYLE pfs);
    PET_FIGHT_STYLE	GetPetFightMode()		{return (PET_FIGHT_STYLE)m_tagPetDetail->lFightType;}

    // 种族
    void SetPetRace(long lRace) { m_tagPetDetail->lRace = lRace; }
    long GetPetRace(void) const {return m_tagPetDetail->lRace;}

    // 生命
    void SetHp(DWORD dwHp)			{m_tagPetDiff->lHp = dwHp;}				
    DWORD GetHp() const				{return	m_tagPetDiff->lHp;}

    //最大生命值
    void SetMaxHp(DWORD dwMaxHp)		{m_tagPetDetail->lMaxHp= dwMaxHp;}
    DWORD GetMaxHp() const {return m_tagPetDetail->lMaxHp;}

    // 魔力
    void SetMp(DWORD dwMp)			{m_tagPetDiff->lMp = dwMp;}				
    DWORD GetMp() const				{return m_tagPetDiff->lMp;}

    //最大魔力值
    void SetMaxMp(DWORD dwMaxMp)		{m_tagPetDetail->lMaxMp = dwMaxMp;}
    DWORD GetMaxMp() const			{return	m_tagPetDetail->lMaxMp;}

    // 内力值
    void SetEnergy(DWORD dwEnergy) { m_tagPetDiff->lEnergy = dwEnergy; }
    DWORD GetEnergy(void) const { return m_tagPetDiff->lEnergy; }

    // 最大内力值
    void SetMaxEnergy(DWORD dwMaxEnergy) { m_tagPetDetail->lMaxEnergy = dwMaxEnergy; }
    DWORD GetMaxEnergy(void) const { return m_tagPetDetail->lMaxEnergy; }

    //忠诚度
    DWORD	GetPetLoyalty() const {return	m_tagPetDiff->lLoyalty;}
    void	SetPetLoyalty(DWORD loyalty)		{m_tagPetDiff->lLoyalty = loyalty;}

    // 寿命
    long GetPetLife(void) const { return m_tagPetDiff->lLife; }
    void SetPetLife(long lLife) { m_tagPetDiff->lLife = lLife; }

    //宠物等级
    const	DWORD	GetPetLevel() const {return m_tagPetDiff->lLevel;}
    void	SetPetLevel(DWORD dwLv)				{m_tagPetDiff->lLevel = dwLv;}

    // 宠物道行等级
    long GetPetDLev(void) { return m_tagPetDiff->ServantAttr.lDLevel; }
    void SetPetDLev(long dLev) { m_tagPetDiff->ServantAttr.lDLevel = dLev; }

    //道行
    const	DWORD	GetPetDLevel() const {return m_tagPetDiff->ServantAttr.lDLevel;}
    void	SetPetDLevel(DWORD dwLv)				{m_tagPetDiff->ServantAttr.lDLevel = dwLv;}

    // 携带等级
    const	DWORD	GetTakeLev() const {return m_tagPetDetail->lTakeLev;}
    void	SetTakeLev(DWORD dwTakeLv)				{m_tagPetDetail->lTakeLev = dwTakeLv;}

    //宠物类型
    const	PET_TYPE	GetPetType() const {return m_PetType;}
	void SetPetType(PET_TYPE type) { m_PetType = type; } 

    //下一等级经验总值
    const	DWORD	GetPetNextLvExp() const {return m_tagPetDetail->lMaxExp;}
    void	SetPetNextLvExp(DWORD NextExp)		{m_tagPetDetail->lMaxExp = NextExp;}

    // 下一等级修为经验值
    long GetPetNextLvDExp() const { return m_tagPetDetail->lMaxDExp; }
    void SetPetNextLvDExp(long NextDExp) { m_tagPetDetail->lMaxDExp = NextDExp; }

    //经验值
    const	DWORD	GetPetExp() const {return	m_tagPetDiff->lExp;}
    void	SetPetExp(DWORD dwExp)				{m_tagPetDiff->lExp = dwExp;}

    // 修为值
    long GetPetDExp(void) const { return m_tagPetDiff->ServantAttr.lDExp; }
    void SetPetDExp(long dExp) { m_tagPetDiff->ServantAttr.lDExp = dExp; }

    // 物理伤害
    DWORD GetPetPhysiAtk(void) const {return m_tagPetDetail->lPhysicsAtk;}
    void SetPetPhysiAtk(DWORD dwAtk)			{m_tagPetDetail->lPhysicsAtk = dwAtk;}

    //魔法攻击力
    const	DWORD	GetPetMagicATK() const {return	m_tagPetDetail->lMAtk;}
    void	SetPetMagicATK(DWORD dwAtk)			{m_tagPetDetail->lMAtk = dwAtk;}

    //物理防御
    const	DWORD	GetPetPhysiDef() const {return	m_tagPetDetail->lPhysicsDef;}
    void	SetPetPhysiDef(DWORD dwDef)			{m_tagPetDetail->lPhysicsDef = dwDef;}

    //魔法防御
    const	DWORD	GetPetMagicDef() const {return	m_tagPetDetail->lMDef;}
    void	SetPetMagicDef(DWORD dwDef)			{m_tagPetDetail->lMDef = dwDef;}

    //命中率
    const	DWORD	GetPetHitRate() const {return	m_tagPetDetail->lHit;}
    void	SetHitRate(DWORD dwRate)			{m_tagPetDetail->lHit = dwRate;}

    //吟唱速度
    const	DWORD	GetPetIntonateSpeed() const {return	m_tagPetDetail->lMAtkSpeed;}
    void	SetPetIntonateSpeed(DWORD dwSpeed)	{m_tagPetDetail->lMAtkSpeed = dwSpeed;}

    //闪避
    const	DWORD	GetPetMiss() const {return m_tagPetDetail->lDodge;}
    void	SetPetMiss(DWORD	dwMiss)			{m_tagPetDetail->lDodge = dwMiss;}

    //水系元素抗性
    DWORD	GetPetElemDefWater() const {return m_tagPetDetail->lElemDef0;}

    //土系元素抗性
    DWORD	GetPetElemDefDust() const {return m_tagPetDetail->lElemDef1;}

    //木系元素抗性	
    DWORD	GetPetElemDefWood() const {return	m_tagPetDetail->lElemDef2;}

    //金系元素抗性
    DWORD	GetPetElemDefGold() const {return m_tagPetDetail->lElemDef3;}	

    //火系元素抗性
    DWORD	GetPetElemDefFire() const {return	m_tagPetDetail->lElemDef4;}

    //力量
    DWORD	GetPetStrenth() const {return m_tagPetDetail->lStrenth;}

    // 身法
    DWORD GetPetDexterity() const { return m_tagPetDetail->lDexterity; }

    // 根骨
    DWORD GetPetCon() const { return m_tagPetDetail->lCon; }

    // 意志
    DWORD GetPetIntellect() const { return m_tagPetDetail->lIntellect; }

    // 灵性
    DWORD GetPetSpiritualism() const { return m_tagPetDetail->lSpiritualism; }

    // 力量加点
    DWORD GetPetStrenthPoint() const { return m_tagPetDiff->ServantAttr.savvyVal.lStrenth; }
    // 身法加点
    DWORD GetPetDexterityPoint() const { return m_tagPetDiff->ServantAttr.savvyVal.lDexterity; }
    // 根骨加点
    DWORD GetPetConPoint() const { return m_tagPetDiff->ServantAttr.savvyVal.lCon; }
    // 意志加点
    DWORD GetPetIntellectPoint() const { return m_tagPetDiff->ServantAttr.savvyVal.lIntellect; }
    // 灵性加点
    DWORD GetPetSpiritualismPoint() const { return m_tagPetDiff->ServantAttr.savvyVal.lSpiritualism; }

    // 力量资质
    DWORD GetPetStrenthTalent() const { return m_tagPetDiff->ServantAttr.talentVal.lStrenth; }
    // 身法资质
    DWORD GetPetDexterityTalent() const { return m_tagPetDiff->ServantAttr.talentVal.lDexterity; }
    // 根骨资质
    DWORD GetPetConTalent() const { return m_tagPetDiff->ServantAttr.talentVal.lCon; }
    // 意志资质
    DWORD GetPetIntellectTalent() const { return m_tagPetDiff->ServantAttr.talentVal.lIntellect; }
    // 灵性资质
    DWORD GetPetSpiritualismTalent() const { return m_tagPetDiff->ServantAttr.talentVal.lSpiritualism; }

    ////技能点
    // const	DWORD	GetPetSkillPoint()		{return m_tagPetDiff->lSP;}
    // void	SetPetSkillPoint(DWORD	dwSp)		{m_tagPetDiff->lSP = dwSp;}

    const CFindPath* GetPetFindPath() {return &m_FindPath;}

    long			ContinueMoveTo(float fDestX, float fDestY);	

    /// 解码激活宠物属性
    virtual bool	DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData = true);
	/// 解码宠物属性
	bool DecodeAttrFromDataBlock(DBReadSet& setReadDB);

    void			DecodeChangePropertyFromDataBlock(DBReadSet& setReadDB);		//更新宠物属性

    void			SetPetAttibute(WORD wAttrType,long lData);		//设置宠物某项属性

    bool			IsMainPlayerPet();			//是否是自己的宠物

    void			EnterRegion(bool isSelf = false);				//进入场景

    void			PetStopMove(bool Flag = false);

    DWORD			GetMoveMsg()		{return	m_MoveMsgNum;}
    DWORD			GetStopMoveMsg()	{return	m_dwStopMoveMsgNum;}	

    void	AddSkill(tagPetSkillInfo &skill);	//添加技能
    tagPetSkillInfo * GetPetSkillInfoByPos(long lPos);		//根据技能槽获取技能	（0-3）天赋技能（4-11）普通技能
    tagPetSkillInfo * GetPetSkillInfoByID(long lSkillID);

    long			GetPetSkillCount()	{return (long)m_PetSkillMap.size();}		//得到技能数				

    eFightModeRet	SkillAI();	//轮询技能列表 加载到技能释放缓冲列表
    void	UsePetSkill(long lSkillPos);	//使用技能

    void	SetCurAttackShape(CMoveShape *pTarget);		//设置当前攻击的目标
    void	CPet::SetCurAttackShape(CGUID & guid);

    CMoveShape*	GetCurAttackShape(){return m_pCurAttackShape;}		//获取当前攻击的目标
    void	SetSpeed(float fSpeed);		//设置宠物移动速度

    ///////------------AI 相关-----------------////////////
    eFightModeRet	PetFightAI();			//宠物攻击目标控制AI
    void	AutoRetaliateUponAI();	//被打还击 

    void	SetPetCurUseSkillPos(long lPos)	{m_lCurUseSkillPos = lPos;}				//设置当前使用的技能
    long	GetPetCurUseSkillPos()			{return m_lCurUseSkillPos;}
    void	UpdateTargetPos(long lPosX,long lPosY);							//更新跟随目标信息
    void	UpdateTargetPos(CMoveShape *pTarget);

    void	SetForceToAttack()	{m_bPassiveForceToFight = true;}		//设置强制攻击

    void	SetIsRetaliateUponNextShape(bool b)		{m_bPetIsRetaliateUponNextShape  = b;}	//是否自动反击

    bool	CheckAttackTargetIsValidate(CMoveShape * pAtkShape);		//检查攻击目标是否合法

    bool	IsPetMoveWithCurAction();						// 判断当前动作是否可以移动

    bool	IsPetActWithCurAction();						// 判断当前动作是否可以攻击

    long	GetPetSkillSendMsgCount()			{return m_dwSkillMsgSendNum;}

    long	GetPetSkillSuccRecvMsgCount()		{return m_dwSkillSuccMsgRecvNum;}

    long	GetPetSkillFailRecvMsgCount()		{return m_dwSkillFailMsgRecvNum;}
    void	SkillSuccReceived()					{++m_dwSkillSuccMsgRecvNum;}

    void	SkillFailReceived()					{++m_dwSkillFailMsgRecvNum;}

    void	PetContAttRecved()					{++dwFightBackMsgNum;}

    long	GetPetCountAttMsgCount()			{return dwFightBackMsgNum;}

    bool	IsSkillBeBlcoked();					//判断技能是否被阻挡了。

    long	GetPetFightAI_Mode()				{return	m_lFightAImode;}		//得到当前宠物战斗AI类型

    void	ClearFindPath()						{m_FindPath.GetPathList()->clear();}		//清空A*路径表

    void	ProcessPetHotKey();					//处理宠物快捷键

    void	LetMeAttack();						//让宠物去攻击

    void	SetPetSkillChanged(bool b)			{m_bPetSkillBechanged = b;}

    bool	IsPetSkillChanged()					{return m_bPetSkillBechanged;}

    bool	IsInBornState()						{return m_bIsInBornState;}

    void	SetInBornState(bool b)				{m_bIsInBornState = b;}

    bool	CheckBornIsComplete();				//宠物出生是否结束

    void	ChangeToNextSkill();				//技能使用成功切换到下一个技能

    bool	IsPreviousSkillActionFinish();		//上一个技能是否结束

    CPet::eFightModeRet	PetSkillIsCanRelease(tagPetSkillInfo *pSkillInfo);	//判断宠物技能是否达到释放条件

    CShape::eDIR	GetRandomPlaceBesideMaster();	//获取主人身边可停留的位置

    static void	ConfusePetStandInfo();			// 打乱停留站点位置

    //sends messages
    void AskforChangeName(const char* newName);
    void AskforChangeState(PET_STATE newState) const;
    void AskforDelete() const;
    void AskforLvUp() const;
    void AskforDLvUp() const;

	void OnHostDied(void);					///< 主人死亡
	void OnDied(void);						///< 宠物死亡
};