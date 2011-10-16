#pragma once

class CSkillListXml
{
public:
	/// 状态
	enum eSKILLTYPE
	{
		///直接施放
		SKILLTYPE_PLAY = 1,			
		/// 吟唱施放
		SKILLTYPE_INTONATE,			
		/// 被动
		SKILLTYPE_PASSIVE,			
	};

	CSkillListXml(void);
	virtual ~CSkillListXml(void);
	/// 职业数据描述
	struct tagSkillOcc
	{
		/// 职业名称
		string strOccName;						
		/// 职业编号
		DWORD	dwOccId;						
	};
	/// 技能基本数据描述
	struct tagLevelValue 
	{
		/// 关键字
		string	strKeyWord;						
		/// 字符值
		string	strValue;						
		/// 数据ID
		DWORD	dwID;							
		/// 值
		DWORD	dwValue;						
		tagLevelValue()
		{
			strKeyWord = "";
			strValue = "";
			dwID = 0;
			dwValue = 0;
		}
	};
	/// 技能等级信息
	struct tagLevel
	{
		/// 技能等级
		DWORD	dwSkill_Level;					
		/// 技能等级描述
		string	strSkillLevelDesc;
		/// 技能等级附加描述（描述下一级的基本信息等）
		string	strSkillLevelDescEx;
		/// 各等级技能AI
		DWORD	dwSkillLevelAI;					
		/// 发送技能验证阶段
		vector<tagLevelValue> vecStepSend;		
		/// 技能开始阶段
		vector<tagLevelValue> vecStepBegin;		
		/// 技能使用成功阶段
		vector<tagLevelValue> vecStepRun;	
		/// 技能结束阶段
		vector<tagLevelValue> vecStepEnd;	
		/// 召唤技能阶段
		vector<tagLevelValue> vecStepSummon;	
		/// 技能持有状态特效
		//vector<tagLevelValue> vecStepState;		
		tagLevel()
		{
			dwSkill_Level = 0;
			strSkillLevelDesc = "";
			dwSkillLevelAI = 0;
		}
		~tagLevel()
		{
			vecStepBegin.clear();
			vecStepSend.clear();
			vecStepEnd.clear();
			vecStepRun.clear();
			vecStepSummon.clear();
		}
		void UpdateValue(const char* strKey,long value);
	};
	/// 单个技能信息
	struct tagSkill
	{
		/// 技能编号
		DWORD	dwSkillID;	
		/// 技能名称
		string	strSkillName;	
		/// 技能描述
		string	strSkillDesc;
		/// 技能状态（一般技能、光环类技能）
		DWORD	dwSkillState;	
		/// 技能属性
		DWORD	dwSkillElement;	
		/// 技能类型(吟唱、释放)
		DWORD	dwSkillType;	
		/// 技能使用类型（物理、魔法）
		DWORD	dwSkillUseType;	
		/// 触发该技能时，在鼠标位置显示的特效ID（如配置了该值，表示为特殊的范围选择技能）
		DWORD	dwEffectID;		
		/// 是否锁定目标
		bool	bLockTarget;	
		/// 是否是锁定技能
		bool	bLockSkill;		
		/// 是否拥有动作
		bool	bHasAct;	
		/// 是否可以对已死亡的对象使用
		bool	bIsUseToDiedObj;	
		/// 是否是引导类技能(释放阶段倒读进度条)
		bool	bIsLead;		
		/// 可用职业列表
		vector<tagSkillOcc>		vecUseByOcc;	
		/// 技能等级队列
		vector<tagLevel>		vectorLevels;	
		tagSkill()
		{
			dwSkillID		= 0;
			strSkillName	= "";
			strSkillDesc	= "";
			dwSkillState	= 0;
			dwSkillElement	= 0;
			dwSkillType		= 0;
			dwSkillUseType	= 0;
			dwEffectID		= 0;
			bLockSkill		= false;
			bLockTarget		= false;
			bHasAct			= false;
			bIsUseToDiedObj = false;
			bIsLead			= false;
		}
		~tagSkill()
		{
			vectorLevels.clear();
			vecUseByOcc.clear();
		}
		///获取指定等级的数据结构
		tagLevel* GettagLevel(uint level);
	};
private:
	/// 所有技能数据队列
	static map<DWORD, tagSkill> m_mapSkillListXml;	
	///同步SkillAttrIniSetup数据到CSkillListXml
	static void SynchroData();
public:
	/// 读取所有技能配置文件,读完后再读取Ini配置，修改对应的数据值
	static BOOL LoadSkillListXml();	
	/// 读取单个技能配置文件
	//static BOOL LoadOneSkillXml(const char* filename);			
	static void Release();
	/// 通过编号(技能ID)取得属性
	static tagSkill* GetProperty(DWORD dwIndex);
	/// 通过名称取得属性
	static tagSkill* GetPropertyByName(const char* strName);	
	/// 判断传入的技能，是否在指定职业下可以使用
	static BOOL OccuJudgeStudy(DWORD dwID,long lOccupID );		
	
};

///全局函数，更新技能模块属性,skillID技能ID，skillleve技能等级，vecStr属性关键字列表，vecValue关键字列表对应的属性值列表
void UpdateSkillXmlList(uint skillID,uint skilllevel,std::vector<string>& vecStr,std::vector<long>& vecValue);
