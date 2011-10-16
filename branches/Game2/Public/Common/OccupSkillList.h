#pragma once

/**************************************************/
// 该类主要作用是保存每个职业所能拥有的技能配置表
// 并提供一些接口，便于查找某个职业的所有技能等
// 07.10.30 刘科
/**************************************************/

// 单个职业的技能类
class COccupSkill
{
public:
	// 职业技能结构
	struct tagOccupSkill
	{
		string strName;		// 技能的名字
		long lSkillID;		// 技能ID
		long lLeve;			// 最多能够升的级数
		tagOccupSkill()
		{
			strName = "";
			lSkillID = 0;
			lLeve = 0;
		}
	};
	COccupSkill();
	~COccupSkill();

	bool Init(TiXmlElement* pElem);		                                // 解析XML中关于技能的配置

	void SetOccupId(long lId)	{m_lOccupId = lId;}
	long GetOccupId()	{return m_lOccupId;}

	void SetName(const char * strName)	{m_strName = strName;}			// 职业名的设置和读取
	const char * GetName()	{return m_strName.c_str();}

	void SetDesc(const char * strDesc)	{m_strDesc = strDesc;}			// 职业描述的设置和读取
	const char * GetDesc()	{return m_strDesc.c_str();}

	long GetOccupSkillCount()	{return (long)(m_vecSkill.size()-1);}	// 得到该职业所拥有的技能总数（从0开始计算）

	vector<tagOccupSkill*> GetOccupSkill() {return m_vecSkill;}			// 获得该职业的技能表

	void SetSkill(tagOccupSkill *ptgOccupSkill) 	                    // 为该职业的技能表添加一个技能信息
    {       m_vecSkill.push_back(ptgOccupSkill);        }	

	long GetOccupSkillMaxLev(ulong dwSkillID);                          // 获得该职业下的指定技能的最大等级

private:

	string m_strName;		                                            // 职业名
	string m_strDesc;		                                            // 职业描述
	long m_lOccupId;		                                            // 职业ID(图片ID，或者对应enum eOccupation中的值)
	vector<tagOccupSkill*> m_vecSkill;	                                // 该职业对应的技能列表
	void Release();			                                            // 释放资源
};


// 所有职业技能列表
class COccupSkillList
{
private:
	static list<COccupSkill *> m_OccupSkillList;		                // 所有的职业技能列表
	

public:
	COccupSkillList();
	~COccupSkillList();

	static void Release();		                                        // 释放资源
	static bool Init(const char *filename);			                    // 初始化，读XML配置文件

	list<COccupSkill *>* GetOccupList() {return &m_OccupSkillList;}
	static COccupSkill * GetOccupSkill(long lOccupID);	                // 根据职业ID获得对应职业的技能列表类
	static bool OccuJudgeStudy(ulong dwID,ulong dwLv,long lOccupID );
		
	static bool AddToByteArray(vector<uchar> &pByteArray);		        // 添加到CByteArray
	static bool DecordFromByteArray(uchar* pByte, long& pos);	        // 解码
 
};


/*******************************************************************
* 转职系统配置
* 摘要: 客户端和服务器可读取配置的转职条件进行判断
*		客户端还可通过该类获得转职路径链表
* 作者: 刘科
* 创建日期: 2009.02.25
********************************************************************/

class CChangeOccu
{
private:
	// 职业结构
	struct tagChangeOccu
	{
		string strName;			// 职业名字
		string strGoodsName;	// 需要物品原始名
		int nID;				// 职业ID
		int nParentID;			// 父职业职业ID
		int nValue;				// 节点ID
		long lGoodsNum;			// 物品数量
		tagChangeOccu()
		{
			strName = "";
			strGoodsName = "";
			nID = 0;
			nParentID = 0;
			nValue = 0;
			lGoodsNum = 0;
		}
	};

	static vector<tagChangeOccu>	m_vecSubOccu;								    // 该职业的子职业列表

public:
	CChangeOccu();
	~CChangeOccu();

	static void Release();														    // 释放资源
	static bool Init(const char *filename);										    // 初始化，读XML配置文件

	static void GetChangeOccuCondision(uchar bOccu,string &str,int &num);		    // 根据传入的职业，获得转职条件（物品及其数量）
	static void GetChangeOccuPath(vector<uchar> &vecOccu,uchar bStart,uchar bEnd);	// 根据开始和结束的职业，查找转职路径
};