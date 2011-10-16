#pragma once
//AddBy: nikai	2008.11
//游戏QA模块
class CGameQASys
{
public:
	//问答类型
	enum QA_TYPE
	{
		QA_BEGINER	= 0,	//新手入门
		QA_OCC		= 1,	//职业相关
		QA_TASK		= 2,	//任务相关
		QA_EQUIP	= 3,	//装备相关
		QA_MAP		= 4,	//地图怪物
		QA_ITEM		= 5,	//物品道具
		QA_SPECIAL	= 6,	//特色系统
		QA_TYPE_TOTAL = 7
	};

	struct tagQAConfig
	{
		QA_TYPE		type;						//配置类型
		string		strDesc;					//描述
		string		strQuestionTreeFileName;	//用于加载树形控件的配置文件名路径
		string		strAnswerContentFileName;	//用于对应此种类型问题的答案内容的文件名
		tagQAConfig():type(QA_BEGINER)
		{}
	};


	//内容组
	struct tagQaDoc 
	{
		bool			isTreeLoaded;		//是否已经加载成功树形控件内容。
		TiXmlDocument	m_QaTreeDoc;		//树形控件内容配置
		
		map<string,DWORD>	mapQuestionTree;	//问题描述

		bool			isAnswerLoaded;		//是否已经加载成功答案内容
		map<DWORD,string>	mapQaAnswerDB;		//答案内容配置
		tagQaDoc():isTreeLoaded(false),isAnswerLoaded(false)
		{}
	};

private:

	//保存配置文件名
	static tagQAConfig		m_tagQaConfig[QA_TYPE_TOTAL];
	static tagQaDoc			m_tagQaDoc[QA_TYPE_TOTAL];
	
public:
	CGameQASys();
	~CGameQASys();
	static void InitialGameQaSys();
	static tagQAConfig	*GetQaConfigFile()	{return m_tagQaConfig;}
	static tagQaDoc		*GetQaDoc()			{return m_tagQaDoc;}
	
	static void LoadQaDoc(QA_TYPE type);
};