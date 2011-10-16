#pragma once

#include "MoveShape.h"

#include "../../../Public/Common/MonsterDef.h"
#include "../../../Public/Common/MultiHpBar.h"
#include "../../../Public/Common/DeputyOccu.h"
#include "../../../Public/Setup/MonsterList.h"
#include "../../../Public/Setup/GoodsSetup.h"
#include "../../../Public/Setup/GlobalSetup.h"
#include "../../../Public/Setup/PlayerList.h"

class CMonsterTaming;
class CVariableList;
class TauntHandler;

//技能类型
enum SKILLTYPE{SKILLTYPE_BASE=0,SKILLTYPE_NORMAL,SKILLTYPE_CYCLE,};

class CMonster :
	public CMoveShape
{
public:
	CMonster(void);
	virtual ~CMonster(void);

public:

	// 属性
	struct stProperty
	{
		string			strOrginName;		//	原始名称
		DWORD			dwHP;				//	当前生命值

		WORD			wSign;				//  所在地图的唯一标识
		WORD			wLeaderSign;		//  领导的唯一标识
		WORD			wLeaderDistance;	//  和领导者保持的最大距离
		long			lLeaderType;		//	领导者type
		long			lLeaderID;			//	领导者id

		long			lLiveTime;			//	生存时间（-1等于一直生存，倒计时，单位：秒）
		bool			bDiedRemove;		//	死亡后删除，不重生
		long			lRefeashIndex;		//	刷新编号(对应Region上的tagMonster)
	};
	struct tagSkill
	{
		WORD	wID;					//编号
		WORD	wLevel;					//等级
	};

	struct tagSkillRecord
	{
		tagSkillRecord(){wID=0; wLevel=0; dwCool=0;dwCoolDownTime=0;iActModify=0;}
		WORD wID;
		WORD wLevel;
		DWORD dwCool;
		DWORD dwCoolDownTime;
		int iActModify;
	};

public:
	const char* GetOriginName(void) { return m_strOrginName.c_str(); }
	void SetOriginName(const char* name) { m_strOrginName = name; }
	const char* GetTitleName(void)	{return m_strTitleName.c_str();}
private:
	string			m_strOrginName;		//	原始名称
	string			m_strTitleName;		//	称谓名
    map<string, tagPty>		    m_mapNameValue;		// 变量名字对应的变量地址
	typedef map<string, tagPty>::iterator mapNameValueItr; 

    // 脏属性数据 在向客户端发送时重新获取最新数据
    set<string>  m_mapDirtyAttrData;
	// 周围玩家需要更新的数据
	static set<string>  m_mapOtherUpdateDirtyAttrData;
    void    PushDirtyAttrData(string attrName) { m_mapDirtyAttrData.insert(attrName); }
    string  PopDirtyAttrData() 
    { 
        string tmp = *m_mapDirtyAttrData.begin(); 
        m_mapDirtyAttrData.erase(m_mapDirtyAttrData.begin());
        return tmp; 
    }
    long    DirtyAttrDataSize(void) { return (long)m_mapDirtyAttrData.size(); }

	tagMonsterProperty			m_tgMonsterProperty;			//怪物属性
	tagMonsterFightProperty		m_tgMonsterFightProperty;		//怪物战斗属性
	tagMonsterFightProperty		m_tgBaseMonsterFightProperty;	//怪物基本战斗属性
	tagPlayerElementProperty	m_arrElementProperty[MT_Max];	//元素属性
	


	string						m_strBornScript;				//出生脚本
	string						m_strFightScript;				//战斗脚本
	string						m_strGBHScript;					//重伤脚本
	string						m_strDeadScript;				//死亡脚本
	string						m_strCycleScript;				//周期触发脚本
	string						m_strSearchEnemyScript;			//寻目标脚本
	string						m_strFindTargetScript;			//找到目标脚本
	string						m_strKillTargetScript;			//杀死目标脚本
	string						m_strCatchScript;				//捕获脚本

public:
	void InitNameValueMap();	// 初始化
	static void InitUpdateDirtyAttrData();

    virtual long GetValue(const char* strName);
    virtual long SetValue(const char* strName, long dwValue);

	// 取得当前值的合法值
	virtual long GetCurrentValue(const tagPty& tagValue, long evalue/*改变值*/, long bvalue/*基本值*/);

	void Talk(const char* strContent);
	void Talk(LONG TextType,LONG TextIndex);
	void Init(); //怪物的初始化
	virtual void InitAI();	//AI的初始化
	void InitProperty(CMonsterList::tagMonster *p);  //初始化基本属性

	BOOL ChangeAI();
	void ResumeAll();
	BOOL Relive();

	void SetPosXY(float fX,float fY );
	void SetBlock(int x, int y, unsigned short block);
//发送脱离战斗消息
	void SendLeaveFightMessage(long lAimX,long lAimY,long lTime);
	void SendLeaveFightEndMessage();

	long OnTimeOut(DWORD timerid,DWORD curtime,const void* var);

//怪物的多血条属性	
private:
	CMultiHpBar					m_MultiHpBar;
public:
	void AddHpBar(int HpRange, const char* ScripntName);
	int  GetHpLevel(int Hp);
	bool OnHpChanged(int Hp, int ChangeHp);
	void CleanHpBar(){m_MultiHpBar.Clean();}
	
//////////////////////////////////////////////////////////////////////////
//	接口
//////////////////////////////////////////////////////////////////////////
    // [070608 AHC]
    virtual bool  SetAttribute(std::string& attrName, long value);
	virtual bool  SetAttribute(char* attrName, long value);
    virtual long  GetAttribute(std::string attrName);
    virtual bool  ChangeAttribute(std::string& attrName, long value);
    virtual long  UpdateAttributeToClient();
	long  UpdateAttribute(BOOL bImmediately=0);
	long  OnUpdateAttributeTimeOut();
	//void          AddChangeAttrToMsgArray(std::vector<BYTE>* bArray);
	void          CodeChangeAttrToDataBlock(DBWriteSet& setWriteDB);
	//添加变量名到胀数据
	void  AddAttributeToDirty(string attrName);
public:

	//virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	//virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

	virtual bool CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData = true);		// 添加到CByteArray
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);	// 解码

	void SetLockAttackImmediately(bool b)	{m_bLockAttackImmediately=b;}

private:

	DWORD	m_dwAICount;
	bool	m_bLockAttackImmediately;	// 立即锁定攻击一次

	//////////////////////////////////////////////////////////////////////////
	//	技能
	//////////////////////////////////////////////////////////////////////////
private:
	typedef vector<tagSkill>::iterator itSkill;
	//怪物技能列表
	vector<tagSkillRecord> m_vBaseSkills;
	//怪物基本技能
	vector<tagSkillRecord> m_vSkills;
	//怪物周期固定技能列表
	vector<tagSkillRecord> m_vCycleSkills;
	//int		m_iBaseSkillNum;
public:
	virtual void AddSkill(CMonsterList::tagSkillInfo& info,SKILLTYPE SkillType);
	virtual bool DelSkill(long id);
	void CountAtkRange(vector<tagSkillRecord>& vSkills);
	void CountAtkRange(DisType ragMax,DisType ragMin);
	bool DelSkillOnList(vector<tagSkillRecord>& vSkills, long SkillID);
	long GetActModify(long SkillID);
public:
	vector<tagSkillRecord>& GetSkillList(){return m_vSkills;}
	vector<tagSkillRecord>& GetBaseSkillList(){return m_vBaseSkills;}
	vector<tagSkillRecord>& GetCycleSkillList(){return m_vCycleSkills;}
	void InitCycleSkill();

	// Fox@20081126 宠物相关
private:
	//DWORD									m_dwTameAttemptCount;
	/// 怪物的当前捕获抗性
	ulong m_dwCatchFastness;
	/// 怪物当前是否被捕获标志
	bool m_bCatchFlag;

public:
	/// 能否被捕获
	bool IsCatchable();
	//void IncreaseTameAttemptCount();
	/// 更新怪物的捕获抗性
	void DecreaseCatchFastness(int catchFastness);
	/// 取得怪物的当前捕获抗性
	ulong GetCatchFastness(void) {return m_dwCatchFastness;}
	/// 重置怪物的最大捕获抗性
	void ResetMaxCatchFastness(void) {m_dwCatchFastness = GetMaxCatchFastness();}
	/// 取得怪物对应的侍从ID
	virtual ulong GetServantID(void) {return m_tgMonsterProperty.dwServantID;}
	/// 设置捕获标志
	void SetCatchFlag(bool b) {m_bCatchFlag=b;}
	/// 取得捕获标志
	bool GetCatchFlag(void){return m_bCatchFlag;}
	/// 取得捕获脚本设定
	const string& GetCacthScript() {return m_strCatchScript;}
private:
	/// 设置怪物对应侍从ID
	virtual	void SetServantID(ulong l) {m_tgMonsterProperty.dwServantID = l;}

//////////////////////////////////////////////////////////////////////////
//	战斗
//////////////////////////////////////////////////////////////////////////
public:
	//怪物基本属性	

	//称谓颜色
	virtual	WORD		GetTitleColor()						{return m_tgMonsterProperty.wTitleColor;}
	//称谓图片id
	virtual	WORD		GetTitlePicID()						{return m_tgMonsterProperty.wTitlePicID;}
	//经验值
	virtual DWORD		GetExp()							{return  m_tgMonsterProperty.dwExp;}
	//杀死得到的经验值
	virtual DWORD		GetExpDead()						{return m_tgMonsterProperty.dwExp;}	
	//杀死得到的修为经验值
	virtual DWORD		GetDExpDead()						{return m_tgMonsterProperty.dwDExp;}
	//职业点数
	virtual DWORD		GetOccuPoint()						{return m_tgMonsterProperty.dwOccuPoint;}
	//种类索引编号
	virtual DWORD		GetIndex()							{return m_tgMonsterProperty.dwIndex;}
	//图形ID
	virtual DWORD		GetPicID()							{return	m_tgMonsterProperty.dwPicID;}
	//图形等级
	virtual	DWORD		GetPicLevel()						{return m_tgMonsterProperty.dwPicLevel;}
	//名字颜色编号,对应到客户端配置
	virtual	DWORD		GetNameColor()						{return m_tgMonsterProperty.dwNameColor;}
	//是否有阻挡
	virtual BYTE		GetBlockType()						{return m_tgMonsterProperty.byBlockType;}
	//血条颜色，0：表示不显示血条 1：红色 2：黄色
	virtual DWORD		GetHpBarColor()						{return m_tgMonsterProperty.dwHpBarColor;}
	//声音ID
	virtual	DWORD		GetSounID()							{return m_tgMonsterProperty.dwSoundID;}
	//等级
	virtual	WORD		GetLevel()							{return m_tgMonsterProperty.wLevel;}
	//国家ID
	virtual	BYTE		GetCountry()						{return m_tgMonsterProperty.byCountry;}
	//是否可以捕获
	virtual	BYTE		GetCatchable()						{return m_tgMonsterProperty.byCatchable;}
	//最大捕获抗性
	virtual	DWORD		GetMaxCatchFastness()				{return m_tgMonsterProperty.dwMaxCatchFastness;}
	//国家贡献值
	virtual	DWORD		GetCountryContriValue()				{return m_tgMonsterProperty.wCountryContriValue;}
	//SP贡献值
	virtual	WORD		GetSPContriValue()					{return m_tgMonsterProperty.wSPContriValue;}
	//元素类别
	virtual	eEleType	GetEleType()						{return m_tgMonsterProperty.eEType;}
	//Npc类别 (NNPC,Monster,Guard)
	virtual	eNpcType	GetNpcType()						{return m_tgMonsterProperty.eNType;}
	//boss等级
	virtual WORD		GetBossLevel()						{return m_tgMonsterProperty.wBossLevel;}
	//种类
	virtual	eClass		GetClass()							{return m_tgMonsterProperty.eClass_;}
	//族群(AI使用)
	virtual	WORD		GetRace()							{return m_tgMonsterProperty.wRace;}
	//产生移动的概率(活力)
	virtual	WORD		GetMoveRandomValue()				{return m_tgMonsterProperty.wMoveRandomValue;}
	//停留的时间(在一个格子中间停留帧数)
	virtual	WORD		GetStopTime()						{return m_tgMonsterProperty.wStopTime;}
	//AI挂起方式
	virtual	WORD		GetAIHangupType()						{return m_tgMonsterProperty.wAIHangupType;}			
	//AI间隙
//	virtual	WORD		GetAIInterval()						{return m_tgMonsterProperty.wAIInterval;}
	//体形(0:1x1 1:3*3 2:5*5)*
	virtual	WORD		GetFigure(eDIR dir=DIR_UP)			{return m_tgMonsterProperty.wFigure;}
	//当前生命值
	virtual	DWORD		GetHP()								{return m_tgMonsterProperty.dwHp;}
	virtual WORD		GetMP() {return 0xFFFF;}
	//AI类型
	virtual DWORD		GetAIType()							{return m_tgMonsterProperty.dwAIType;}
	//最小停留次数
	virtual WORD		GetMinStopTimes()					{return m_tgMonsterProperty.wMinStopTimes;}	
		//最大停留次数
	virtual WORD		GetMaxStopTimes()					{return m_tgMonsterProperty.wMaxStopTimes;}
	//最少行走步数
	virtual WORD		GetMinRunTimes()					{return m_tgMonsterProperty.wMinRunTimes;}
	//最多行走步数
	virtual WORD		GetMaxRunTimes()					{return m_tgMonsterProperty.wMaxRunTimes;}
	//跟新索引
	virtual LONG		GetRefeashIndex()					{return m_tgMonsterProperty.lRefeashIndex;}
	//重伤比率
	virtual float       GetHurtProportion()                 {return m_tgMonsterProperty.fHurtProportion;}
	//警戒时间
	virtual DWORD       GetGuardTime()                      {return m_tgMonsterProperty.dwGuardTime;}
	//逃跑底线
	virtual float       GetEscapePoint()                    {return m_tgMonsterProperty.fEscapePoint;}
	//技能概率
	virtual WORD		GetSkillProportion()                {return m_tgMonsterProperty.wSkillProportion;}
	//技能间隔
	virtual DWORD       GetSkillSpace()                   {return m_tgMonsterProperty.dwSkillSpace;}
	//ai参考
	virtual	DWORD       GetAIReferrence(int lpos)         {if(lpos<0 || lpos>=MONSTER_AI_REFERRENCE_NUM) return 0;
	                                                         return m_tgMonsterProperty.dwAIReferrence[lpos];}
	//转换为和平时候是否飞回去
	virtual BYTE		GetIsFlyToPeace()					{return m_tgMonsterProperty.byIsFlyToPeace;}
	//转换为和平时候是否完全恢复
	virtual BYTE		GetIsResumeAll()					{return m_tgMonsterProperty.byIsResumeAll;}	
	virtual DisType		GetReturnRange()					{return m_tgMonsterProperty.ReturnRange;}
	virtual DisType		GetPeaceMoveRange()					{return m_tgMonsterProperty.PeaceMoveRange;}
	virtual DWORD		GetChaseTime()						{return m_tgMonsterProperty.dwChaseTime;}

////////////////////////////////////////////////////////////////////////////////////////////
//设置怪物属性
////////////////////////////////////////////////////////////////////////////////////////////
	//称谓颜色
	virtual	void		SetTitleColor(WORD l)						{m_tgMonsterProperty.wTitleColor=l;}
	//称谓图片id
	virtual	void		SetTitlePicID(WORD l)						{m_tgMonsterProperty.wTitlePicID=l;}
	//	经验值
	virtual void		SetExp(DWORD l)						{m_tgMonsterProperty.dwExp = l;}
	//  修为经验值
	virtual void		SetDExp(DWORD l)					{m_tgMonsterProperty.dwDExp = l;}
	//职业点数
	virtual void		SetOccuPoint(DWORD l)				{m_tgMonsterProperty.dwOccuPoint = l;}
	//种类索引编号
	virtual void		SetIndex(DWORD l)					{m_tgMonsterProperty.dwIndex = l;}
	//图形ID
	virtual void		SetPicID(DWORD l)					{m_tgMonsterProperty.dwPicID = l;}
	//图形等级
	virtual	void		SetPicLevel(DWORD l)				{m_tgMonsterProperty.dwPicLevel = l;}
	//设置是否有阻挡
	virtual void		SetBlockType(BYTE l)				{m_tgMonsterProperty.byBlockType = l;}
	//名字颜色编号,对应到客户端配置
	virtual	void		SetNameColor(DWORD l)				{m_tgMonsterProperty.dwNameColor = l;}
	//血条颜色，0：表示不显示血条 1：红色 2：黄色
	virtual void		SetHpBarColor(DWORD l)				{m_tgMonsterProperty.dwHpBarColor = l;}
	//声音ID
	virtual	void		SetSounID(DWORD l)					{m_tgMonsterProperty.dwSoundID = l;}
	//等级
	virtual	void		SetLevel(WORD l)					{m_tgMonsterProperty.wLevel = l;}
	//国家ID
	virtual	void		SetCountry(BYTE l)					{m_tgMonsterProperty.byCountry = l;}
	//是否可以捕获
	virtual	void		SetCatchable(BYTE l)					{m_tgMonsterProperty.byCatchable = l;}
	//最大捕获抗性
	virtual	void		SetMaxCatchFastness(DWORD l)			{m_tgMonsterProperty.dwMaxCatchFastness = l;}
	//国家贡献值
	virtual	void		SetCountryContriValue(DWORD l)		{m_tgMonsterProperty.wCountryContriValue = (ushort)l;}
	//SP贡献值
	virtual	void		SetSPContriValue(DWORD l)			{m_tgMonsterProperty.wSPContriValue = (ushort)l;}
	//元素类别
	virtual	void		SetEleType(eEleType l )				{m_tgMonsterProperty.eEType = l;}
	//Npc类别 (NNPC,Monster,Guard)
	virtual	void		SetNpcType(eNpcType l)				{m_tgMonsterProperty.eNType = l;}
	//boss等级
	virtual void		SetBossLevel(WORD l)				{m_tgMonsterProperty.wBossLevel=l;}
	//种类
	virtual	void		SetClass(eClass l)					{m_tgMonsterProperty.eClass_ = l;}
	//族群(AI使用)
	virtual	void		SetRace(WORD l)						{m_tgMonsterProperty.wRace = l;}
	//产生移动的概率(活力)
	virtual	void		SetMoveRandomValue(WORD l)			{m_tgMonsterProperty.wMoveRandomValue = l;}
	//停留的时间(在一个格子中间停留帧数)
	virtual	void		SetStopTime(WORD l)					{m_tgMonsterProperty.wStopTime = l;}
	//AI挂起方式
	virtual void		SetAIHangupType(WORD l)				{m_tgMonsterProperty.wAIHangupType=l;}			
	//AI间隙
//	virtual	void		SetAIInterval(WORD l)				{m_tgMonsterProperty.wAIInterval = l;}
	//体形(0:1x1 1:3*3 2:5*5)*
	virtual	void		SetFigure(WORD l)					{m_tgMonsterProperty.wFigure = l;}
	//当前生命值
	virtual	void		SetHP(DWORD l)
	{
		m_tgMonsterProperty.dwHp= l;
		if(m_tgMonsterProperty.dwHp > GetMaxHP())
		{
			m_tgMonsterProperty.dwHp = GetMaxHP();
		}
	}
	//AI类型
	virtual void		SetAIType(DWORD l)					{m_tgMonsterProperty.dwAIType = l;}
	//最小停留次数
	virtual void		SetMinStopTimes(WORD l)				{m_tgMonsterProperty.wMinStopTimes=l;}		
	//最大停留次数
	virtual void		SetMaxStopTimes(DWORD l)			{m_tgMonsterProperty.wMaxStopTimes=(ushort)l;}	
	//最少行走步数
	virtual void		SetMinRunTimes(DWORD l)				{m_tgMonsterProperty.wMinRunTimes=(ushort)l;}
	//最多行走步数
	virtual void		SetMaxRunTimes(DWORD l)				{m_tgMonsterProperty.wMaxRunTimes=(ushort)l;}
	//更新索引
	virtual void		SetRefeashIndex(LONG l)				{m_tgMonsterProperty.lRefeashIndex=l;}
	//重伤比率
	virtual void        SetHurtProportion(float l)          {m_tgMonsterProperty.fHurtProportion=l;}
	//警戒时间
	virtual void        SetGuardTime(DWORD l)               {m_tgMonsterProperty.dwGuardTime=l;}
	//逃跑底线
	virtual void        SetEscapePoint(float l)             {m_tgMonsterProperty.fEscapePoint=l;}
	//技能概率
	virtual void		SetSkillProportion(WORD l)          {m_tgMonsterProperty.wSkillProportion=l;}
	//技能间隔
	virtual void        SetSkillSpace(DWORD l)            {m_tgMonsterProperty.dwSkillSpace=l;}
	//ai参考
	virtual	void        SetAIReferrence(int lpos,DWORD l) {if(lpos<0 || lpos>=MONSTER_AI_REFERRENCE_NUM) return ;
	                                                          m_tgMonsterProperty.dwAIReferrence[lpos]=l;}
	//转换为和平时候是否飞回去
	virtual void		SetIsFlyToPeace(BYTE l)				{m_tgMonsterProperty.byIsFlyToPeace=l;}
	//转换为和平的时候是否完全恢复
	virtual void		SetIsResumeAll(BYTE l)				{m_tgMonsterProperty.byIsResumeAll=l;}
	virtual void		SetReturnRange(DisType l)				{m_tgMonsterProperty.ReturnRange=l;}
	virtual	void		SetPeaceMoveRange(DisType l)			{m_tgMonsterProperty.PeaceMoveRange=l;}
	virtual void		SetChaseTime(DWORD l)				{m_tgMonsterProperty.dwChaseTime=l;}
//////////////////////////////////////////////////////////////////////////

	virtual long	GetStopFrame(); 							// 获取在格子中心停留的桢数
	virtual bool	IsAttackAble(CMoveShape* pAttacker);			// 可否攻击

	//////////////////////////////////////////////////////////////////////////
	//获取怪物战斗属性
	//////////////////////////////////////////////////////////////////////////
	//最近攻击距离
	virtual DisType GetAtkRangeMin()					{return m_tgMonsterFightProperty.NeaAtkDis;}
	//最远攻击距离
	virtual DisType GetAtkRangeMax()					{return m_tgMonsterFightProperty.FarAtkDis;}
	//攻击范围(以攻击目标为中心0:1x1 1:3*3 2:5*5)
//	virtual long GetFightRange()					{return m_tgMonsterFightProperty.wAtkRange;}
	//警戒范围
	virtual DisType GetGuardRange()					{return m_tgMonsterFightProperty.GuardRange;}
	//追击范围
	virtual DisType GetChaseRange()					{return m_tgMonsterFightProperty.ChaseRange;}	
	//生命上限
	virtual DWORD GetMaxHP()
	{
		long lMaxHp = m_tgMonsterFightProperty.dwMaxHp+m_tgBaseMonsterFightProperty.dwMaxHp;
		if(lMaxHp<0)	return 0;
		return lMaxHp;
	}	
	//生命恢复速度
	virtual WORD GetHpRecoverSpeed()				{return m_tgMonsterFightProperty.wHpRecoverSpeed;}
	//最小攻击力
	virtual WORD GetMinAtk()						{return m_tgMonsterFightProperty.wMinAtk;}
	//最大攻击力
	virtual WORD GetMaxAtk()						{return m_tgMonsterFightProperty.wMaxAtk;}
	//最小法术攻击力
	virtual WORD GetMinMAtk()						{return m_tgMonsterFightProperty.wMinMAtk;}
	//最大法术攻击力
	virtual WORD GetMaxMAtk()						{return m_tgMonsterFightProperty.wMaxMAtk;}
	//命中能力
	virtual WORD GetHit()							{return m_tgMonsterFightProperty.wHit;}
	//防御力
	virtual WORD GetDef()							{return m_tgMonsterFightProperty.wDef;}
	//魔法防御力
	virtual WORD GetMDef()							{return m_tgMonsterFightProperty.wMDef;}
	//闪避能力
	virtual WORD GetDodge()							{return m_tgMonsterFightProperty.wDodge;}
	//格挡
	virtual WORD GetBlock()							{return m_tgMonsterFightProperty.wBlock;}
	//招架
	virtual WORD GetParry()							{return m_tgMonsterFightProperty.wParry;}	
	//攻击速度
//	virtual WORD GetAtcSpeed()						{return m_tgMonsterFightProperty.wAtcSpeed;}
	//移动速度
	virtual float GetMoveSpeed()					{return m_tgMonsterProperty.fMoveSpeed;}
	//战斗移动速度
	virtual float GetFightMoveSpeed()				{return m_tgMonsterProperty.fFightMoveSpeed;}
	//转身速度
	virtual float GetTurningSpeed()					{return m_tgMonsterProperty.fTurningSpeed;}
	//意志抗性
	virtual WORD GetWillDef()						{return m_tgMonsterFightProperty.wWillDef;}
	//体质抗性
	virtual WORD GetConstiDef()						{return m_tgMonsterFightProperty.wConstiDef;}
	//Debuff抗性
	virtual WORD GetDeBuffDef(WORD lpos)			{if(lpos<0) {lpos = 0;}else if((lpos+1)>DET_Max) lpos = DET_Max -1; return m_tgMonsterFightProperty.wDeBuffDef[lpos];}
	//各个元素抗性
	virtual WORD GetElemDef(WORD lpos)				{if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1; return m_tgMonsterFightProperty.wElemDef[lpos];}
	//D对各个种族伤害
	virtual WORD GetClassDam(WORD lpos)				{if(lpos<0) {lpos = 0;}else if((lpos+1)>C_Max) lpos = C_Max -1; return m_tgMonsterFightProperty.wClassDam[lpos];}
	//对各个元素伤害
	virtual WORD GetElemDam(WORD lpos)				{if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1; return m_tgMonsterFightProperty.wElemDam[lpos];}
	//物理爆击率
	virtual WORD GetCri()							{return m_tgMonsterFightProperty.wCri;}
	//物理暴击伤害
	virtual WORD GetCriDamg()						{return m_tgMonsterFightProperty.wCriDamg;}
	//物理爆击抗性
	virtual WORD GetCriDef()						{return m_tgMonsterFightProperty.wCriDef;}
	//魔法暴击率
	virtual WORD GetMCri()							{return m_tgMonsterFightProperty.wMCri;}
	//魔法暴击伤害
	virtual WORD GetMCriDamg()						{return m_tgMonsterFightProperty.wMCriDamg;}
	//魔法爆击抗性
	virtual WORD GetMCriDef()						{return m_tgMonsterFightProperty.wMCriDef;}
	//是否能对话
	virtual WORD GetCanTalk()						{return m_tgMonsterProperty.wCanTalk;}

	//////////////////////////////////////////////////////////////////////////
	//设置怪物战斗属性
	//////////////////////////////////////////////////////////////////////////
	
	//最近攻击距离
	virtual void SetNearAtkDis(DisType l);//					{m_mapDirtyAttrData.push_back("wNeaAtkDis");m_tgMonsterFightProperty.wNeaAtkDis = l;}
	//最远攻击距离
	virtual void SetFarAtkDis(DisType l);//					{m_mapDirtyAttrData.push_back("wFarAtkDis");m_tgMonsterFightProperty.wFarAtkDis = l;}	
	//生命上限
	virtual void SetMaxHp(DWORD l);//						{m_mapDirtyAttrData.push_back("dwMaxHp");m_tgMonsterFightProperty.dwMaxHp = l; }		
	//最小攻击力
	virtual void SetMinAtk(WORD l);//						{m_mapDirtyAttrData.push_back("wMinAtk");m_tgMonsterFightProperty.wMinAtk = l; }
	//最大攻击力
	virtual void SetMaxAtk(WORD l);//						{m_mapDirtyAttrData.push_back("wMaxAtk");m_tgMonsterFightProperty.wMaxAtk = l;}
	//最小法术攻击力
	virtual void SetMinMAtk(WORD l);//						{m_mapDirtyAttrData.push_back("wMinMAtk");m_tgMonsterFightProperty.wMinMAtk = l;}
	//最大法术攻击力
	virtual void SetMaxMAtk(WORD l);//						{m_mapDirtyAttrData.push_back("wMaxMAtk");m_tgMonsterFightProperty.wMaxMAtk = l; }
	//生命恢复速度
	virtual void SetHpRecoverSpeed(WORD l);//				{m_mapDirtyAttrData.push_back("wHpRecoverSpeed");m_tgMonsterFightProperty.wHpRecoverSpeed = l;}
	//攻击范围(以攻击目标为中心0:1x1 1:3*3 2:5*5)
//	virtual void SetAtkRange(WORD l);//					{m_mapDirtyAttrData.push_back("wAtkRange");m_tgMonsterFightProperty.wAtkRange = l;}
	//警戒范围
	virtual void SetGuardRange(DisType l);//					{m_mapDirtyAttrData.push_back("wGuardRange");m_tgMonsterFightProperty.wGuardRange = l;}
	//追击范围
	virtual void SetChaseRange(DisType l);//					{m_mapDirtyAttrData.push_back("wChaseRange");m_tgMonsterFightProperty.wChaseRange = l;}	
	//命中能力
	virtual void SetHit(WORD l);//							{m_mapDirtyAttrData.push_back("wHit");m_tgMonsterFightProperty.wHit = l;}
	//防御力
	virtual void SetDef(WORD l)	;//						{m_mapDirtyAttrData.push_back("wDef");m_tgMonsterFightProperty.wDef = l;}
	//魔法防御力
	virtual void SetMDef(WORD l);//
	//闪避能力
	virtual void SetDodge(WORD l);//						{m_mapDirtyAttrData.push_back("wDodge");m_tgMonsterFightProperty.wDodge = l;}
	//格挡
	virtual void SetFightBlock(WORD l);//						{m_mapDirtyAttrData.push_back("wBlock");m_tgMonsterFightProperty.wBlock = l;}
	//招架
	virtual void SetParry(WORD l);//						{m_mapDirtyAttrData.push_back("wParry");m_tgMonsterFightProperty.wParry = l;}	
	//攻击速度
	//virtual void SetAtcSpeed(WORD l);//					{m_mapDirtyAttrData.push_back("wAtcSpeed");m_tgMonsterFightProperty.wAtcSpeed = l;}
	//移动速度
	virtual void SetMoveSpeed(float f);//					{m_mapDirtyAttrData.push_back("wMoveSpeed");m_tgMonsterFightProperty.wMoveSpeed = l;}
	//战斗移动速度
	virtual void SetFightMoveSpeed(float f);//				{m_mapDirtyAttrData.push_back("wFightMoveSpeed");m_tgMonsterFightProperty.wFightMoveSpeed = l;}
	//转身速度
	virtual void SetTurningSpeed(float f)					{m_tgMonsterProperty.fTurningSpeed=f;}
	//意志抗性
	virtual void SetWillDef(WORD l)	;//					{m_mapDirtyAttrData.push_back("wWillDef");m_tgMonsterFightProperty.wWillDef = l;}
	//体质抗性
	virtual void SetConstiDef(WORD l);//					{m_mapDirtyAttrData.push_back("wConstiDef");m_tgMonsterFightProperty.wConstiDef = l;}
	//Debuff抗性
	virtual void SetDeBuffDef(WORD lpos,WORD l);			

	//各个元素抗性
	virtual void SetElemDef(WORD lpos,WORD l);			

	//D对各个种族伤害
	virtual void SetClassDam(WORD lpos,WORD l);	

	//对各个元素伤害
	virtual void SetElemDam(WORD lpos,WORD l);	

	//物理爆击率
	virtual void SetCri(WORD l);//							{m_tgMonsterFightProperty.wCri = l;}
	//物理暴击伤害
	virtual void SetCriDamg(WORD l);
	//物理爆击抗性
	virtual void SetCriDef(WORD l);//						{m_tgMonsterFightProperty.wCriDef = l;}
	//魔法暴击率
	virtual void SetMCri(WORD l);
	//魔法暴击伤害
	virtual void SetMCriDamg(WORD l);
	//魔法爆击抗性
	virtual void SetMCriDef(WORD l);//						{m_tgMonsterFightProperty.wMCriDef = l;}
	//是否能对话
	virtual void SetCanTalk(WORD l);


	//////////////////////////////////////////////////////////////////////////
	//获取怪物基本战斗属性
	//////////////////////////////////////////////////////////////////////////
	//最近攻击距离
	virtual DisType GetBaseNearAtkDis()						{return m_tgBaseMonsterFightProperty.NeaAtkDis;}
	//最远攻击距离
	virtual DisType GetBaseFarAtkDis()						{return m_tgBaseMonsterFightProperty.FarAtkDis;}
	//攻击范围(以攻击目标为中心0:1x1 1:3*3 2:5*5)
//	virtual long GetBaseAtkRange()						{return m_tgBaseMonsterFightProperty.wAtkRange;}
	//警戒范围
	virtual DisType GetBaseGuardRange()					{return m_tgBaseMonsterFightProperty.GuardRange;}
	//追击范围
	virtual DisType GetBaseChaseRange()					{return m_tgBaseMonsterFightProperty.ChaseRange;}	
	//生命上限
	virtual long GetBaseMaxHp()							{return m_tgBaseMonsterFightProperty.dwMaxHp;}	
	//生命恢复速度
	virtual WORD GetBaseHpRecoverSpeed()				{return m_tgBaseMonsterFightProperty.wHpRecoverSpeed;}
	//最小攻击力
	virtual WORD GetBaseMinAtk()						{return m_tgBaseMonsterFightProperty.wMinAtk;}
	//最大攻击力
	virtual WORD GetBaseMaxAtk()						{return m_tgBaseMonsterFightProperty.wMaxAtk;}
	//最小法术攻击力
	virtual WORD GetBaseMinMAtk()						{return m_tgBaseMonsterFightProperty.wMinMAtk;}
	//最大法术攻击力
	virtual WORD GetBaseMaxMAtk()						{return m_tgBaseMonsterFightProperty.wMaxMAtk;}
	//命中能力
	virtual WORD GetBaseHit()							{return m_tgBaseMonsterFightProperty.wHit;}
	//防御力
	virtual WORD GetBaseDef()							{return m_tgBaseMonsterFightProperty.wDef;}
	//魔法防御力
	virtual WORD GetBaseMDef()							{return m_tgBaseMonsterFightProperty.wMDef;}
	//闪避能力
	virtual WORD GetBaseDodge()							{return m_tgBaseMonsterFightProperty.wDodge;}
	//格挡
	virtual WORD GetBaseBlock()							{return m_tgBaseMonsterFightProperty.wBlock;}
	//招架
	virtual WORD GetBaseParry()							{return m_tgBaseMonsterFightProperty.wParry;}	
	//攻击速度
//	virtual WORD GetBaseAtcSpeed()						{return m_tgBaseMonsterFightProperty.wAtcSpeed;}
	//移动速度
	//virtual WORD GetBaseMoveSpeed()						{return m_tgBaseMonsterFightProperty.wMoveSpeed;}
	//战斗移动速度
	//virtual WORD GetBaseFightMoveSpeed()				{return m_tgBaseMonsterFightProperty.wFightMoveSpeed;}
	//意志抗性
	virtual WORD GetBaseWillDef()						{return m_tgBaseMonsterFightProperty.wWillDef;}
	//体质抗性
	virtual WORD GetBaseConstiDef()						{return m_tgBaseMonsterFightProperty.wConstiDef;}
	//Debuff抗性
	virtual WORD GetBaseDeBuffDef(WORD lpos)			{if(lpos<0) {lpos = 0;}else if((lpos+1)>DET_Max) lpos = DET_Max -1; return m_tgMonsterFightProperty.wDeBuffDef[lpos];}
	//各个元素抗性
	virtual WORD GetBaseElemDef(WORD lpos)				{if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1; return m_tgMonsterFightProperty.wElemDef[lpos];}
	//D对各个种族伤害
	virtual WORD GetBaseClassDam(WORD lpos)				{if(lpos<0) {lpos = 0;}else if((lpos+1)>C_Max) lpos = C_Max -1; return m_tgMonsterFightProperty.wClassDam[lpos];}
	//对各个元素伤害
	virtual WORD GetBaseElemDam(WORD lpos)				{if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1; return m_tgMonsterFightProperty.wElemDam[lpos];}
	//物理爆击率
	virtual WORD GetBaseCri()							{return m_tgBaseMonsterFightProperty.wCri;}
	//物理暴击伤害
	virtual WORD GetBaseCriDamg()						{return m_tgBaseMonsterFightProperty.wCriDamg;}
	//物理爆击抗性
	virtual WORD GetBaseCriDef()						{return m_tgBaseMonsterFightProperty.wCriDef;}
	//魔法暴击率
	virtual WORD GetBaseMCri()							{return m_tgBaseMonsterFightProperty.wMCri;}
	//魔法暴击伤害
	virtual WORD GetBaseMCriDamg()						{return m_tgBaseMonsterFightProperty.wMCriDamg;}
	//魔法爆击抗性
	virtual WORD GetBaseMCriDef()						{return m_tgBaseMonsterFightProperty.wMCriDef;}
	//能否对话
	//virtual	WORD GetBaseCanTalk()						{return m_tgBaseMonsterFightProperty.wCanTalk;}
	//////////////////////////////////////////////////////////////////////////
	//设置怪物基本战斗属性
	//////////////////////////////////////////////////////////////////////////
	//最近攻击距离
	virtual void SetBaseNearAtkDis(DisType l)					{m_tgBaseMonsterFightProperty.NeaAtkDis = l; }
	//最远攻击距离
	virtual void SetBaseFarAtkDis(DisType l)					{m_tgBaseMonsterFightProperty.FarAtkDis = l;}	
	//生命上限
	virtual void SetBaseMaxHp(DWORD l)						{m_tgBaseMonsterFightProperty.dwMaxHp = l; }		
	//最小攻击力
	virtual void SetBaseMinAtk(WORD l)						{m_tgBaseMonsterFightProperty.wMinAtk = l; }
	//最大攻击力
	virtual void SetBaseMaxAtk(WORD l)						{m_tgBaseMonsterFightProperty.wMaxAtk = l;}
	//最小法术攻击力
	virtual void SetBaseMinMAtk(WORD l)						{m_tgBaseMonsterFightProperty.wMinMAtk = l;}
	//最大法术攻击力
	virtual void SetBaseMaxMAtk(WORD l)						{m_tgBaseMonsterFightProperty.wMaxMAtk = l;}
	//生命恢复速度
	virtual void SetBaseHpRecoverSpeed(WORD l)				{m_tgBaseMonsterFightProperty.wHpRecoverSpeed = l;}
	//攻击范围(以攻击目标为中心0:1x1 1:3*3 2:5*5)
//	virtual void SetBaseAtkRange(WORD l)					{m_tgBaseMonsterFightProperty.wAtkRange = l;}
	//警戒范围
	virtual void SetBaseGuardRange(DisType l)					{m_tgBaseMonsterFightProperty.GuardRange = l;}
	//追击范围
	virtual void SetBaseChaseRange(DisType l)					{m_tgBaseMonsterFightProperty.ChaseRange = l;}	
	//命中能力
	virtual void SetBaseHit(WORD l)							{m_tgBaseMonsterFightProperty.wHit = l;}
	//防御力
	virtual void SetBaseDef(WORD l)							{m_tgBaseMonsterFightProperty.wDef = l;}
	//魔法防御力
	virtual void SetBaseMDef(WORD l)						{m_tgBaseMonsterFightProperty.wMDef = l;}
	//闪避能力
	virtual void SetBaseDodge(WORD l)						{m_tgBaseMonsterFightProperty.wDodge = l;}
	//格挡
	virtual void SetBaseBlock(WORD l)						{m_tgBaseMonsterFightProperty.wBlock = l;}
	//招架
	virtual void SetBaseParry(WORD l)						{m_tgBaseMonsterFightProperty.wParry = l;}	
	//免伤
	virtual void SetBaseImmunity( short value )				{m_tgBaseMonsterFightProperty.wImmunity = value; }
	//穿透
	virtual void SetBasePierce( short value )				{m_tgBaseMonsterFightProperty.wPierce = value; }
	//攻击速度
//	virtual void SetBaseAtcSpeed(WORD l)					{m_tgBaseMonsterFightProperty.wAtcSpeed = l;}
	//移动速度
	//virtual void SetBaseMoveSpeed(WORD l)					{m_tgBaseMonsterFightProperty.wMoveSpeed = l;}
	//战斗移动速度
	//virtual void SetBaseFightMoveSpeed(WORD l)				{m_tgBaseMonsterFightProperty.wFightMoveSpeed = l;}
	//意志抗性
	virtual void SetBaseWillDef(WORD l)						{m_tgBaseMonsterFightProperty.wWillDef = l;}
	//体质抗性
	virtual void SetBaseConstiDef(WORD l)					{m_tgBaseMonsterFightProperty.wConstiDef = l;}
	
	//Debuff抗性
	virtual void SetBaseDeBuffDef(WORD lpos,WORD l)			
    {
		if(lpos<0) {lpos = 0;}else if((lpos+1)>DET_Max) lpos = DET_Max -1;
        m_tgBaseMonsterFightProperty.wDeBuffDef[lpos] = l;
    }
	//各个元素抗性
	virtual void SetBaseElemDef(WORD lpos,WORD l)	
    {
		if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1;
        m_tgBaseMonsterFightProperty.wElemDef[lpos] = l;
    }
	//D对各个种族伤害
	virtual void SetBaseClassDam(WORD lpos,WORD l)	
    {
		if(lpos<0) {lpos = 0;}else if((lpos+1)>C_Max) lpos = C_Max -1;
        m_tgBaseMonsterFightProperty.wClassDam[lpos] = l;
    }
	//对各个元素伤害
	virtual void SetBaseElemDam(WORD lpos,WORD l)	
    {
		if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1;
        m_tgBaseMonsterFightProperty.wElemDam[lpos] = l;
    }
	//物理爆击率
	virtual void SetBaseCri(WORD l)							{m_tgBaseMonsterFightProperty.wCri = l;}
	//物理暴击伤害
	virtual void SetBaseCriDamg(WORD l)						{m_tgBaseMonsterFightProperty.wCriDamg = l;}
	//物理爆击抗性
	virtual void SetBaseCriDef(WORD l)						{m_tgBaseMonsterFightProperty.wCriDef = l;}
	//魔法暴击率
	virtual void SetBaseMCri(WORD l)						{m_tgBaseMonsterFightProperty.wMCri = l;}
	//魔法暴击伤害
	virtual void SetBaseMCriDamg(WORD l)					{m_tgBaseMonsterFightProperty.wMCriDamg = l;}
	//魔法爆击抗性
	virtual void SetBaseMCriDef(WORD l)						{m_tgBaseMonsterFightProperty.wMCriDef = l;}
	//是否能对话
	//virtual void SetBaseCanTalk(WORD l)						{m_tgBaseMonsterFightProperty.wCanTalk=l;}
	//脚本名字的获取
	const string& GetBornScriptName()						{return m_strBornScript;}
	const string& GetFightScriptName()						{return m_strFightScript;}
	const string& GetGBHScriptName()						{return m_strGBHScript;}
	const string& GetDeadScriptName()						{return m_strDeadScript;}
	const string& GetCycleScript()							{return m_strCycleScript;}				//周期触发脚本
	const string& GetSearchEnemyScript()					{return m_strSearchEnemyScript;}//寻目标脚本
	const string& GetFindTargetScript()						{return m_strFindTargetScript;}			//找到目标脚本
	const string& GetKillTargetScript()						{return m_strKillTargetScript;}			//杀死目标脚本

	virtual void OnDied();								// 死亡处理	
//	virtual void DropGoods();						

	//##经验值分配的实现函数
	LONG		CalculateExperienceQuota			( CPlayer* pPlayer, LONG lPosX, LONG lPosY );
	//计算职业经验分配函数
	LONG		CalculateOccuExpQuota				( CPlayer* pPlayer, LONG lPosX, LONG lPosY );
	LONG		CalculateOccuPointQuota				( CPlayer* pPlayer, LONG lPosX, LONG lPosY );
	//DWORD		CalculateExperienceCorrective		( CPlayer* pPlayer, DWORD dwExperience );
	//##获取受益人
	CPlayer*	GetBeneficiary						();

	virtual void OnBeenHurted( LONG lAttackerType, const CGUID& AttackerGuid ); //被伤害的处理

	DWORD GetAICount(){return m_dwAICount;}
	void SetAICount(DWORD dwCount){m_dwAICount=dwCount;}
	virtual void GetBeAttackedPoint(long lAttackerX,long lAttackerY,long& lTileX,long& lTileY);

//////////////////////////////////////////////////////////////////////////
//	死亡脚本
//////////////////////////////////////////////////////////////////////////
public:
	const char* GetScriptFile()			{return m_strScriptFile.c_str();}
	void SetScriptFile(const char* str)	{m_strScriptFile=str;}

	virtual void Evanish();

private:
	string m_strScriptFile;		// 脚本文件名
//////////////////////////////////////////////////////////////////////////
//  谈话脚本
//////////////////////////////////////////////////////////////////////////
public:
	const char* GetTalkScriptFile(){return m_strTalkScriptFile.c_str();}
	void SetTalkScriptFile(const char* str){m_strTalkScriptFile=str;}
private:
	string m_strTalkScriptFile;
//////////////////////////////////////////////////////////////////////////
//  谈话脚本
//////////////////////////////////////////////////////////////////////////
public:
	void ExpAction(long ActID,long DesType=0,CGUID DesGUID=NULL_GUID);
////////////////////////////////////////////////////////////////////////////
////	受益者
////////////////////////////////////////////////////////////////////////////
public:
	void OnLoseBeneficiary();			//丢失受益者事件
	void StartRecordBeneficiary(CGUID guidFirst,OBJECT_TYPE typeFirst,DWORD time);				//开始记录受益者
	bool CheckDelete();
//------------------------------------------------------------------------
//	继承于CMoveShape需要实例化的虚函数
//------------------------------------------------------------------------
//	virtual void			EnterCombatState()		{}
//	virtual void			EnterPeaceState()		{}
	virtual void			UpdateCurrentState()	{}


	//##获取挥拳的时间
	DWORD GetStrikeOutTime();
//---------------------------------------------------------------------
//  在场景中刷新时候是否重生
//---------------------------------------------------------------------
//public:
//	BOOL	GetCanReburn(){return m_bCanReburn;}
//	void	SetCanReburn(BOOL canreburn){m_bCanReburn=canreburn;}
//private:
//	BOOL m_bCanReburn;
public:
	virtual BOOL	BeenCollectedAll();
//为npc提供
public:
		void SetShowList(bool b)		{m_bShowList=b;}
		bool IsShowList()				{return m_bShowList;}
private:
	bool m_bShowList;


//---------------------------------------------------------------------
//  集群
//---------------------------------------------------------------------
public:
	//enum{COLONY_STATE_NULL,COLONY_STATE_NOLEADER, COLONY_STATE_PEACE, COLONY_STATE_FIGHT};
	DWORD	GetColonyID(){return m_dwColonyID;}
	//DWORD	GetMemberID(){return m_dwMemberID;}
	WORD	GetMemberType(){return m_wMemberType;}
	void	AddToColony(DWORD dwColonyID, WORD wMemberType);
	void	ExitColony();
	WORD	GetColonyState(){return m_bColonyState;}
	BOOL	HasColony(){return m_dwColonyID==0? FALSE:TRUE;}
	DWORD	GetColonyDistance(){return m_dwColonyDistance;}

	void	SetColonyDistance(DWORD dis){m_dwColonyDistance= dis;}
	void	SetColonyID(DWORD dwColonyID){m_dwColonyID= dwColonyID;}
	//void	SetMemberID(DWORD dwMemberID){m_dwMemberID= dwMemberID;}
	void	SetMemberType(WORD type){m_wMemberType= type;}
	void	SetColonyState(BOOL bState){m_bColonyState= bState;}
private:

private:
	DWORD m_dwColonyID;
	//DWORD m_dwMemberID;
	DWORD m_dwColonyDistance;
	WORD  m_wMemberType;
	BOOL  m_bColonyState;
//改变虚弱状态
public:
	void						ChangeWeaknessState(bool l);

//---------------------------------------------------------------------
// 注册消失
//---------------------------------------------------------------------
public:
	//void OnDelEvent();			// 响应删除事件


public:

private:
	static BOOL CheckDropGoods(string MonsterName, INT RegionIndex, INT Level,
		INT LevelBalances,vector<CGoods*> &pOut, BYTE drop_flag = 0 );
	static BOOL CheckOneDrop(CMonsterList::tagDropGoods* pDrop,int LevelBalance, vector<CGoods*> &pOut);

public:
    TauntHandler &GetTauntHandler() { return *m_tauntHandler; }
private:
    TauntHandler *m_tauntHandler;
};
