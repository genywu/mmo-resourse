#pragma once

/**************************************************/
// 该类主要作用是保存每个职业所能在学习时的条件限制
// 并提供一些接口，便于查找这些限制条件
// 07.11.21 刘科
/**************************************************/


#include "tinystr.h"
#include "tinyxml.h"
#include "tools.h"
#include <list>
using namespace std;


// 技能学习条件类
class CStudySkillCondition
{
public:
	// 附加属性的类型
	enum eConditionEx
	{
		CONDITIONEX_NULL = -1,			// 无附加条件
		CONDITIONEX_GOODS,				// 附加道具
		CONDITIONEX_PRESKILL,			// 附加前置技能
		CONDITIONEX_CONFLICTSKILL,		// 附加限制技能
		CONDITIONEX_SCRIPT,			    // 触发脚本
        CONDITIONEX_EXP,                // 附加经验值 [chenqiang@20110317]
	};

	// 附加条件结构
	struct tagConditionEx
	{
		eConditionEx wConditionExType;	// 附加条件的类型
		string		 strValue;			// 第一个值（有可能是描述物品的原始名，也有可能描述技能ID，XML解析时都用字符串形式保存）
		LONG64		 lValue;			// 第二个值（对于附加道具来说，描述道具数，对于附加前置技能，描述技能等级，限制技能则无此项，始终为-1）
		tagConditionEx()
		{
			wConditionExType = CONDITIONEX_NULL;
			strValue = "";
			lValue = -1;
		}
	};

	// 条件结构，该结构处了描述学习指定技能的基本条件外，
	// 还存在一个附加条件的列表。
	struct tagCondition
	{
		DWORD dwSkillID;		// 技能ID
		WORD wSkillLev;			// 当前技能的等级
		WORD wStudyMode;		// 当前等级的学习模式
		WORD wPlayerLev;		// 要求玩家的等级限制
		DWORD dwSp;				// 学习需要消耗的SP值
		DWORD dwOccuLv;			//职业等级
		vector<tagConditionEx*>  vec_ConditionEx;	// 附加学习限制列表
		tagCondition()
		{
			dwSkillID	= 0;
			wSkillLev	= 0;
			wStudyMode	= 0;
			wPlayerLev	= 0;
			dwSp		= 0;
			dwOccuLv	= 0;
		}
	};
	//技能列表
	struct tagTeachSkill
	{
		string			strOcc;	//职业
		list<DWORD>		lOccList;//职业条件
		list<DWORD>		lSkillList;//技能列表
	};
	//活力值倍率列表
	struct tagActive
	{
		long  lMinValue;
		long  lMaxValue;
		long  lStateID;
		long  lStateLv;		
	};


	CStudySkillCondition();
	~CStudySkillCondition();
	
	// 解析XML中关于技能学习的配置
	static bool Init(const char *filename);
	static void Release();			// 释放资源
	static bool InitSkillList(const char *filename);	//解析学习技能列表
	static bool InitFactureSkillList(const char *filename);	//初始化专业制作技能列表

	// 根据要学习技能的ID以及需要提升的等级，找出这个技能的学习限制
	static tagCondition * GetCondition(DWORD wSkillID, WORD wSkillLev);
	static bool AddToByteArray(vector<BYTE> &pByteArray);		// 添加到CByteArray
	static bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码
	static DWORD GetDwordAttribute(TiXmlElement *pElem,string strName);
	static map<string,tagTeachSkill*> GetTeacherSkill() {return m_mapTeachSkill;}
	static bool CheckFactureSkill(long lID);
	static bool InitCondition(DWORD dwID, TiXmlElement* pElem);	// 解析技能的学习条件
	static bool InitConditionEx(vector<tagConditionEx*> & vec_ConditionEx, TiXmlElement* pElem);	// 解析技能的附加学习条件
	static list<tagActive*>	&GetActiveList()	{return m_lActiveRatio;}		//获取活力点倍率列表
private:
	static list<tagCondition*> m_StudyConditionList;			// 所有的技能学习条件列表
	static list<long>		   m_lFactureSkillList;				// 专业制作技能列表
	static map<string,tagTeachSkill*> m_mapTeachSkill;			// 教授技能列表
	static list<tagActive*>	   m_lActiveRatio;					// 活力点倍率配置列表			
};