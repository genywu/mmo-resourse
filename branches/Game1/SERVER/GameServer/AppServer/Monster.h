#pragma once
#include "moveshape.h"
#include "setup\monsterlist.h"
#include "..\..\..\public\monsterdef.h"
#include "MasterInfo.h"
#include <deque>
#include "..\..\..\public\\MultiHpBar.h"

class CMonsterTaming;
class CVariableList;

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
    long    DirtyAttrDataSize(void) { return m_mapDirtyAttrData.size(); }

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
	VOID ResumeAll();
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
public:
	virtual void AddSkill(CMonsterList::tagSkillInfo& info,SKILLTYPE SkillType);
	virtual bool DelSkill(long id);
	void CountAtkRange(vector<tagSkillRecord>& vSkills);
	void CountAtkRange(int iRagMax,int iRagMin);
	bool DelSkillOnList(vector<tagSkillRecord>& vSkills, long SkillID);
	long GetActModify(long SkillID);
public:
	vector<tagSkillRecord>& GetSkillList(){return m_vSkills;}
	vector<tagSkillRecord>& GetBaseSkillList(){return m_vBaseSkills;}
	vector<tagSkillRecord>& GetCycleSkillList(){return m_vCycleSkills;}
	VOID InitCycleSkill();

	// Fox@20081126 宠物相关
private:
	DWORD									m_dwTameAttemptCount;

public:
	BOOL			IsTamable				();
	VOID			IncreaseTameAttemptCount();

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
	//是否可以驯服
	virtual	BYTE		GetTamable()						{return m_tgMonsterProperty.byTamable;}
	//最大尝试驯服次数
	virtual	DWORD		GetMaxTameCount()					{return m_tgMonsterProperty.dwMaxTameCount;}
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
	virtual DWORD       GetSkillSpace()                     {return m_tgMonsterProperty.dwSkillSpace;}
	//ai参考
	virtual	DWORD       GetAIReferrence(int lpos)           {if(lpos<0 || lpos>=MONSTER_AI_REFERRENCE_NUM) return 0;
	                                                         return m_tgMonsterProperty.dwAIReferrence[lpos];}
	//转换为和平时候是否飞回去
	virtual BYTE		GetIsFlyToPeace()					{return m_tgMonsterProperty.byIsFlyToPeace;}
	//转换为和平时候是否完全恢复
	virtual BYTE		GetIsResumeAll()					{return m_tgMonsterProperty.byIsResumeAll;}	
	virtual WORD		GetReturnRange()					{return m_tgMonsterProperty.wReturnRange;}
	virtual WORD		GetPeaceMoveRange()					{return m_tgMonsterProperty.wPeaceMoveRange;}
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
	//是否可以驯服
	virtual	void		SetTamable(BYTE l)					{m_tgMonsterProperty.byTamable = l;}
	//最大尝试驯服次数
	virtual	void		SetMaxTameCount(DWORD l)			{m_tgMonsterProperty.dwMaxTameCount = l;}
	//国家贡献值
	virtual	void		SetCountryContriValue(DWORD l)		{m_tgMonsterProperty.wCountryContriValue = l;}
	//SP贡献值
	virtual	void		SetSPContriValue(DWORD l)			{m_tgMonsterProperty.wSPContriValue = l;}
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
	virtual void		SetMaxStopTimes(DWORD l)			{m_tgMonsterProperty.wMaxStopTimes=l;}	
	//最少行走步数
	virtual void		SetMinRunTimes(DWORD l)				{m_tgMonsterProperty.wMinRunTimes=l;}
	//最多行走步数
	virtual void		SetMaxRunTimes(DWORD l)				{m_tgMonsterProperty.wMaxRunTimes=l;}
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
	virtual void		SetReturnRange(WORD l)				{m_tgMonsterProperty.wReturnRange=l;}
	virtual	void		SetPeaceMoveRange(WORD l)			{m_tgMonsterProperty.wPeaceMoveRange=l;}
	virtual void		SetChaseTime(DWORD l)				{m_tgMonsterProperty.dwChaseTime=l;}
//////////////////////////////////////////////////////////////////////////

	virtual long	GetStopFrame(); 							// 获取在格子中心停留的桢数
	virtual bool	IsAttackAble(CMoveShape* pAttacker);			// 可否攻击

	//////////////////////////////////////////////////////////////////////////
	//获取怪物战斗属性
	//////////////////////////////////////////////////////////////////////////
	//最近攻击距离
	virtual long GetAckRangeMin()					{return m_tgMonsterFightProperty.wNeaAtkDis;}
	//最远攻击距离
	virtual long GetAckRangeMax()					{return m_tgMonsterFightProperty.wFarAtkDis;}
	//警戒范围
	virtual long GetGuardRange()					{return m_tgMonsterFightProperty.wGuardRange;}
	//追击范围
	virtual long GetChaseRange()					{return m_tgMonsterFightProperty.wChaseRange;}	
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
	virtual WORD GetDef()							{return m_tgMonsterFightProperty.lDef;}
	//魔法防御力
	virtual WORD GetMDef()							{return m_tgMonsterFightProperty.lMDef;}
	//闪避能力
	virtual WORD GetDodge()							{return m_tgMonsterFightProperty.wDodge;}
	//格挡
	virtual WORD GetBlock()							{return m_tgMonsterFightProperty.wBlock;}
	//招架
	virtual WORD GetParry()							{return m_tgMonsterFightProperty.wParry;}	

	//移动速度
	virtual float GetMoveSpeed()					{return m_tgMonsterProperty.fMoveSpeed;}
	//战斗移动速度
	virtual float GetFightMoveSpeed()				{return m_tgMonsterProperty.fFightMoveSpeed;}
	//转身速度
	virtual float GetTurningSpeed()					{return m_tgMonsterProperty.fTurningSpeed;}
	//意志抗性
	virtual long GetWillDef()						{return m_tgMonsterFightProperty.lWillDef;}
	//体质抗性
	virtual long GetConstiDef()						{return m_tgMonsterFightProperty.lConstiDef;}
	//Debuff抗性
	virtual long GetDeBuffDef(WORD lpos)			{if(lpos<0) {lpos = 0;}else if((lpos+1)>DET_Max) lpos = DET_Max -1; return m_tgMonsterFightProperty.lDeBuffDef[lpos];}
	//各个元素抗性
	virtual long GetElemDef(WORD lpos)				{if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1; return m_tgMonsterFightProperty.lElemDef[lpos];}
	//D对各个种族伤害
	virtual WORD GetClassDam(WORD lpos)				{if(lpos<0) {lpos = 0;}else if((lpos+1)>C_Max) lpos = C_Max -1; return m_tgMonsterFightProperty.wClassDam[lpos];}
	//对各个元素伤害
	virtual WORD GetElemDam(WORD lpos)				{if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1; return m_tgMonsterFightProperty.wElemDam[lpos];}
	//物理爆击率
	virtual WORD GetCri()							{return m_tgMonsterFightProperty.wCri;}
	//物理暴击伤害
	virtual WORD GetCriDamg()						{return m_tgMonsterFightProperty.wCriDamg;}
	//物理爆击抗性
	virtual long GetCriDef()						{return m_tgMonsterFightProperty.lCriDef;}
	//魔法暴击率
	virtual WORD GetMCri()							{return m_tgMonsterFightProperty.wMCri;}
	//魔法暴击伤害
	virtual WORD GetMCriDamg()						{return m_tgMonsterFightProperty.wMCriDamg;}
	//魔法爆击抗性
	virtual long GetMCriDef()						{return m_tgMonsterFightProperty.lMCriDef;}
	//是否能对话
	virtual WORD GetCanTalk()						{return m_tgMonsterProperty.wCanTalk;}

	//////////////////////////////////////////////////////////////////////////
	//设置怪物战斗属性
	//////////////////////////////////////////////////////////////////////////
	
	//最近攻击距离
	virtual void SetNeaAtkDis(WORD l);					
	//最远攻击距离
	virtual void SetFarAtkDis(WORD l);					
	//生命上限
	virtual void SetMaxHp(DWORD l);						
	//最小攻击力
	virtual void SetMinAtk(WORD l);						
	//最大攻击力
	virtual void SetMaxAtk(WORD l);						
	//最小法术攻击力
	virtual void SetMinMAtk(WORD l);						
	//最大法术攻击力
	virtual void SetMaxMAtk(WORD l);						
	//生命恢复速度
	virtual void SetHpRecoverSpeed(WORD l);
	//警戒范围
	virtual void SetGuardRange(WORD l);					
	//追击范围
	virtual void SetChaseRange(WORD l);					
	//命中能力
	virtual void SetHit(WORD l);							
	//防御力
	virtual void SetDef(long l)	;
	//魔法防御力
	virtual void SetMDef(long l);
	//闪避能力
	virtual void SetDodge(WORD l);						
	//格挡
	virtual void SetFightBlock(WORD l);					
	//招架
	virtual void SetParry(WORD l);						
	//移动速度
	virtual void SetMoveSpeed(float f);					
	//战斗移动速度
	virtual void SetFightMoveSpeed(float f);				
	//转身速度
	virtual void SetTurningSpeed(float f)					{m_tgMonsterProperty.fTurningSpeed=f;}
	//意志抗性
	virtual void SetWillDef(long l)	;
	//体质抗性
	virtual void SetConstiDef(long l);
	//Debuff抗性
	virtual void SetDeBuffDef(WORD lpos,long l);			
  
	//各个元素抗性
	virtual void SetElemDef(WORD lpos,long l);			

	//D对各个种族伤害
	virtual void SetClassDam(WORD lpos,WORD l);	
 
	//对各个元素伤害
	virtual void SetElemDam(WORD lpos,WORD l);	
  
	//物理爆击率
	virtual void SetCri(WORD l);
	//物理暴击伤害
	virtual void SetCriDamg(WORD l);
	//物理爆击抗性
	virtual void SetCriDef(long l);
	//魔法暴击率
	virtual void SetMCri(WORD l);
	//魔法暴击伤害
	virtual void SetMCriDamg(WORD l);
	//魔法爆击抗性
	virtual void SetMCriDef(long l);
	//是否能对话
	virtual void SetCanTalk(WORD l);


	//////////////////////////////////////////////////////////////////////////
	//获取怪物基本战斗属性
	//////////////////////////////////////////////////////////////////////////
	//最近攻击距离
	virtual long GetBaseNeaAtkDis()						{return m_tgBaseMonsterFightProperty.wNeaAtkDis;}
	//最远攻击距离
	virtual long GetBaseFarAtkDis()						{return m_tgBaseMonsterFightProperty.wFarAtkDis;}
	//警戒范围
	virtual long GetBaseGuardRange()					{return m_tgBaseMonsterFightProperty.wGuardRange;}
	//追击范围
	virtual long GetBaseChaseRange()					{return m_tgBaseMonsterFightProperty.wChaseRange;}	
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
	virtual WORD GetBaseDef()							{return m_tgBaseMonsterFightProperty.lDef;}
	//魔法防御力
	virtual WORD GetBaseMDef()							{return m_tgBaseMonsterFightProperty.lMDef;}
	//闪避能力
	virtual WORD GetBaseDodge()							{return m_tgBaseMonsterFightProperty.wDodge;}
	//格挡
	virtual WORD GetBaseBlock()							{return m_tgBaseMonsterFightProperty.wBlock;}
	//招架
	virtual WORD GetBaseParry()							{return m_tgBaseMonsterFightProperty.wParry;}	
	//意志抗性
	virtual long GetBaseWillDef()						{return m_tgBaseMonsterFightProperty.lWillDef;}
	//体质抗性
	virtual long GetBaseConstiDef()						{return m_tgBaseMonsterFightProperty.lConstiDef;}
	//Debuff抗性
	virtual long GetBaseDeBuffDef(WORD lpos)			{if(lpos<0) {lpos = 0;}else if((lpos+1)>DET_Max) lpos = DET_Max -1; return m_tgMonsterFightProperty.lDeBuffDef[lpos];}
	//各个元素抗性
	virtual long GetBaseElemDef(WORD lpos)				{if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1; return m_tgMonsterFightProperty.lElemDef[lpos];}
	//D对各个种族伤害
	virtual WORD GetBaseClassDam(WORD lpos)				{if(lpos<0) {lpos = 0;}else if((lpos+1)>C_Max) lpos = C_Max -1; return m_tgMonsterFightProperty.wClassDam[lpos];}
	//对各个元素伤害
	virtual WORD GetBaseElemDam(WORD lpos)				{if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1; return m_tgMonsterFightProperty.wElemDam[lpos];}
	//物理爆击率
	virtual WORD GetBaseCri()							{return m_tgBaseMonsterFightProperty.wCri;}
	//物理暴击伤害
	virtual WORD GetBaseCriDamg()						{return m_tgBaseMonsterFightProperty.wCriDamg;}
	//物理爆击抗性
	virtual long GetBaseCriDef()						{return m_tgBaseMonsterFightProperty.lCriDef;}
	//魔法暴击率
	virtual WORD GetBaseMCri()							{return m_tgBaseMonsterFightProperty.wMCri;}
	//魔法暴击伤害
	virtual WORD GetBaseMCriDamg()						{return m_tgBaseMonsterFightProperty.wMCriDamg;}
	//魔法爆击抗性
	virtual long GetBaseMCriDef()						{return m_tgBaseMonsterFightProperty.lMCriDef;}
	//能否对话
	//////////////////////////////////////////////////////////////////////////
	//设置怪物基本战斗属性
	//////////////////////////////////////////////////////////////////////////
	//最近攻击距离
	virtual void SetBaseNeaAtkDis(WORD l)					{m_tgBaseMonsterFightProperty.wNeaAtkDis = l; }
	//最远攻击距离
	virtual void SetBaseFarAtkDis(WORD l)					{m_tgBaseMonsterFightProperty.wFarAtkDis = l;}	
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

	//警戒范围
	virtual void SetBaseGuardRange(WORD l)					{m_tgBaseMonsterFightProperty.wGuardRange = l;}
	//追击范围
	virtual void SetBaseChaseRange(WORD l)					{m_tgBaseMonsterFightProperty.wChaseRange = l;}	
	//命中能力
	virtual void SetBaseHit(WORD l)							{m_tgBaseMonsterFightProperty.wHit = l;}
	//防御力
	virtual void SetBaseDef(long l)							{m_tgBaseMonsterFightProperty.lDef = l;}
	//魔法防御力
	virtual void SetBaseMDef(long l)						{m_tgBaseMonsterFightProperty.lMDef = l;}
	//闪避能力
	virtual void SetBaseDodge(WORD l)						{m_tgBaseMonsterFightProperty.wDodge = l;}
	//格挡
	virtual void SetBaseBlock(WORD l)						{m_tgBaseMonsterFightProperty.wBlock = l;}
	//招架
	virtual void SetBaseParry(WORD l)						{m_tgBaseMonsterFightProperty.wParry = l;}	
	//免伤
	virtual void SetBaseImmunity( long value )				{m_tgBaseMonsterFightProperty.lImmunity = value; }
	//穿透
	virtual void SetBasePierce( short value )				{m_tgBaseMonsterFightProperty.wPierce = value; }
	//意志抗性
	virtual void SetBaseWillDef(long l)						{m_tgBaseMonsterFightProperty.lWillDef = l;}
	//体质抗性
	virtual void SetBaseConstiDef(long l)					{m_tgBaseMonsterFightProperty.lConstiDef = l;}
	
	//Debuff抗性
	virtual void SetBaseDeBuffDef(WORD lpos,long l)			
    {
		if(lpos<0) {lpos = 0;}else if((lpos+1)>DET_Max) lpos = DET_Max -1;
        m_tgBaseMonsterFightProperty.lDeBuffDef[lpos] = l;
    }
	//各个元素抗性
	virtual void SetBaseElemDef(WORD lpos,long l)	
    {
		if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1;
        m_tgBaseMonsterFightProperty.lElemDef[lpos] = l;
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
	virtual void SetBaseCriDef(long l)						{m_tgBaseMonsterFightProperty.lCriDef = l;}
	//魔法暴击率
	virtual void SetBaseMCri(WORD l)						{m_tgBaseMonsterFightProperty.wMCri = l;}
	//魔法暴击伤害
	virtual void SetBaseMCriDamg(WORD l)					{m_tgBaseMonsterFightProperty.wMCriDamg = l;}
	//魔法爆击抗性
	virtual void SetBaseMCriDef(long l)						{m_tgBaseMonsterFightProperty.lMCriDef = l;}
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

	//##经验值分配的实现函数
	LONG		CalculateExperienceQuota			( CPlayer* pPlayer, LONG lPosX, LONG lPosY );
	//计算职业经验分配函数
	LONG		CalculateOccuExpQuota				( CPlayer* pPlayer, LONG lPosX, LONG lPosY );
	LONG		CalculateOccuPointQuota				( CPlayer* pPlayer, LONG lPosX, LONG lPosY );
	//##获取受益人
	CPlayer*	GetBeneficiary						();

	virtual VOID OnBeenHurted( LONG lAttackerType, const CGUID& AttackerGuid ); //被伤害的处理

	DWORD GetAICount(){return m_dwAICount;}
	void SetAICount(DWORD dwCount){m_dwAICount=dwCount;}
	virtual void GetBeAttackedPoint(long lAttackerX,long lAttackerY,long& lTileX,long& lTileY);

//////////////////////////////////////////////////////////////////////////
//	死亡脚本
//////////////////////////////////////////////////////////////////////////
public:
	const char* GetScriptFile()			{return m_strScriptFile.c_str();}
	void SetScriptFile(const char* str)	{m_strScriptFile=str;}

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
	virtual VOID			UpdateCurrentState()	{}

//---------------------------------------------------------------------
//  在场景中刷新时候是否重生
//---------------------------------------------------------------------
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
	DWORD	GetColonyID(){return m_dwColonyID;}
	WORD	GetMemberType(){return m_wMemberType;}
	VOID	AddToColony(DWORD dwColonyID, WORD wMemberType);
	VOID	ExitColony();
	WORD	GetColonyState(){return m_bColonyState;}
	BOOL	HasColony(){return m_dwColonyID==0? FALSE:TRUE;}
	DWORD	GetColonyDistance(){return m_dwColonyDistance;}

	VOID	SetColonyDistance(DWORD dis){m_dwColonyDistance= dis;}
	VOID	SetColonyID(DWORD dwColonyID){m_dwColonyID= dwColonyID;}
	VOID	SetMemberType(WORD type){m_wMemberType= type;}
	VOID	SetColonyState(BOOL bState){m_bColonyState= bState;}
private:
	DWORD m_dwColonyID;
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
	// 重载接口
	virtual int SetScriptVarValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName, const char* value);
	virtual int SetScriptVarValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName, long lArray=0, double value=0);
	virtual const char* GetScriptVarValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName);
	virtual double GetScriptVarValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName, long lArray);
	virtual int SetScriptGuidValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName, const CGUID& guid);
	virtual const CGUID& GetScriptGuidValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName);
	virtual CVariableList* GetVariableList();
private:
	CVariableList* m_pVariableList;
};
