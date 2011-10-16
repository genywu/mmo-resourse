#include "StdAfx.h"
#include "DeputyOccu.h"

#ifdef _DEBUG
#define ENABLE_ALOADER
#include "../Package/ClientResLoader.h"
#endif


//采集列表
map<string,CCollectionLayout*>	CDeputyOccuSystem::m_mapCollectionList;
//制作列表
map<ulong,CFacture*>		CDeputyOccuSystem::m_mapFactureList;
//制作中高级获得物品的基础几率
ulong						CDeputyOccuSystem::m_dwAdvancedGainedBacePro;
//采集经验列表
list<CGapExp>				CDeputyOccuSystem::m_listCollectionGapExpList;
//制作经验列表
list<CGapExp>				CDeputyOccuSystem::m_listFacturenGapExpList;
//采集等级列表
vector<CGradeExp>			CDeputyOccuSystem::m_vecCollectionGradeExpList;
//制作等级列表
vector<CGradeExp>			CDeputyOccuSystem::m_vecFactureGradeExpList;
//制作学习列表
map<string,CFactureStudy*>	CDeputyOccuSystem::m_mapStudyList;
//制作学习提示列表
map<DOccuTypeLevel,vector<string>>		CDeputyOccuSystem::m_mapStudyClew;
//制作分类列表
map<ulong,string>			CDeputyOccuSystem::m_mapFactureSort;
//采集的最高等级
ulong						CDeputyOccuSystem::m_dwCollectMaxGrade;
//制作的最高等级
ulong						CDeputyOccuSystem::m_dwFactureMaxGrade;
//副职业技能记录
DeputyOccuSkillRecord		CDeputyOccuSystem::m_tgDOccuSkillRecord;
//采集成功系统触发
string						CDeputyOccuSystem::m_strCollectionSuccSpring;
//制作成功系统触发
string						CDeputyOccuSystem::m_strFactureSuccSpring;

bool CCollectionLayout::Init(TiXmlElement* p)
{
	//
	if (p==NULL)
	{
		return false;
	}
	//采集物名字
	m_strCollectionName=p->Attribute("name");	
	//采集物原始名
	m_strOrginName=p->Attribute("orginname");
	//采集物id
	m_dwCollectionID=atoi(p->Attribute("id"));
	//副职业id
	m_eOccuID=(eDeputyOccu)atoi(p->Attribute("artisanID"));
	//图形id
	m_dwPicID=atoi(p->Attribute("picID"));
	//小地图标志(雷达id)
	m_dwRadarID=atoi(p->Attribute("radarID"));;
	//最小采集次数
	m_dwMixTimes=atoi(p->Attribute("mintimes"));
	//最大采集次数
	m_dwMaxTimes=atoi(p->Attribute("maxtimes"));
	//采集等级
	m_dwCollectLevel=atoi(p->Attribute("skilllevel"));

	TiXmlElement* children=p->FirstChildElement();

	tagQuestGoods tagGoods;
	for (; children!=NULL; children=children->NextSiblingElement())
	{
		if (!strcmp(children->Value(),"StartScript"))
		{
			m_strStartScriptName= children->GetText();//Attribute("name");
		}
		else if (!strcmp(children->Value(),"EndScript"))
		{
			m_strEndScriptName= children->GetText();//Attribute("name");
		}
		else if (!strcmp(children->Value(),"SuccScript"))
		{
			m_strSuccScriptName= children->GetText();//Attribute("name");
		}
		//任务采集时ID  090309 by tanglei
		else if (!strcmp(children->Value(),"ActionID"))
		{
			m_dwActionID = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"BringBlock"))
		{
			m_byBlockType= atoi(children->GetText())!=0;//Attribute("name");
		}
		else if (!strcmp(children->Value(),"Restrict"))
		{
			////需要任务等级
			//m_iRoleID=atoi(children->Attribute("roolID"));
			////需要任务步骤
			//m_dwRoleStep=atoi(children->Attribute("roolstepID"));
			tagGoods.nQuestId = atoi(children->Attribute("roolID"));
			tagGoods.dwRoleStep = atoi(children->Attribute("roolstepID"));
			m_vecQuestGoods.push_back(tagGoods);
		}
		else if (!strcmp(children->Value(),"GoodsCondition"))
		{
			tgConditionGoods goods;
			string name=children->Attribute("goodsname");
			goods.iNumType=atoi(children->Attribute("numtype"));
			goods.iNum=atoi(children->Attribute("num"));
			if (name.size()<=48)
			{
				memcpy(goods.cGoodsName,name.c_str(),name.size()+1);
			}
			else
			{
				memcpy(goods.cGoodsName,name.c_str(),48);
				goods.cGoodsName[48]='\0';

				Log4c::Warn(ROOT_MODULE, "输出错误信息：采集物配置中名字过长");
			}
			//goods.dwGoodsIndex=CGoodsFactory::QueryGoodsIDByOriginalName(goods.cGoodsName);
			m_listGoodsCondition.push_back(goods);
		}
		else if (!strcmp(children->Value(),"GainedGoods"))
		{
			tgOddsGoods goods;
			string name=children->Attribute("name");
			goods.iGoodNum=atoi(children->Attribute("num"));
			goods.fOdds= (float)atof(children->Attribute("odds"));
			if (name.size()<=48)
			{
				memcpy(goods.cGoodsName,name.c_str(),name.size()+1);
			}
			else
			{
				memcpy(goods.cGoodsName,name.c_str(),48);
				goods.cGoodsName[48]='\0';
				Log4c::Warn(ROOT_MODULE, "输出错误信息：采集物配置中名字过长");
			}
			//goods.dwGoodsIndex=CGoodsFactory::QueryGoodsIDByOriginalName(goods.cGoodsName);
			m_listGainedGoods.push_back(goods);
		}
		else
		{

			Log4c::Warn(ROOT_MODULE, "输出错误信息：采集物配置中无效配置");
		}
	}
	return true;

}
bool CCollectionLayout::AddToByteArray(vector<uchar>* pByteArray)
{

	_AddToByteArray(pByteArray,m_strOrginName.c_str());
	_AddToByteArray(pByteArray,m_strCollectionName.c_str());
	_AddToByteArray(pByteArray,m_strStartScriptName.c_str());
	_AddToByteArray(pByteArray,m_strEndScriptName.c_str());
	_AddToByteArray(pByteArray,m_strSuccScriptName.c_str());

	_AddToByteArray(pByteArray,m_dwCollectionID);
	_AddToByteArray(pByteArray,(long)m_eOccuID);
	_AddToByteArray(pByteArray,m_dwPicID);
	_AddToByteArray(pByteArray,m_dwRadarID);
	_AddToByteArray(pByteArray,m_dwMixTimes);
	_AddToByteArray(pByteArray,m_dwMaxTimes);
	_AddToByteArray(pByteArray,m_dwCollectLevel);
	_AddToByteArray(pByteArray,m_dwActionID);
	/*_AddToByteArray(pByteArray,(long)m_iRoleID);
	_AddToByteArray(pByteArray,m_dwRoleStep);*/
	_AddToByteArray(pByteArray,(long)m_vecQuestGoods.size());
	QUESTGOODS_ITER _it=m_vecQuestGoods.begin();
	for (; _it!=m_vecQuestGoods.end();_it++)
	{
		_AddToByteArray(pByteArray,(uchar*)&(*_it),sizeof(tagQuestGoods));
	}
	_AddToByteArray(pByteArray,m_byBlockType);
	CONDITIONGOODS_ITTER it=m_listGoodsCondition.begin();
	_AddToByteArray(pByteArray,(ulong)m_listGoodsCondition.size());
	for (; it!=m_listGoodsCondition.end();it++)
	{
		_AddToByteArray(pByteArray,(uchar*)&(*it),sizeof(tgConditionGoods));
	}
	ODDSGOODS_ITTER it_=m_listGainedGoods.begin();
	_AddToByteArray(pByteArray,(ulong)m_listGainedGoods.size());
	for (; it_!=m_listGainedGoods.end(); it_++)
	{
		_AddToByteArray(pByteArray,(uchar*)&(*it_),sizeof(tgOddsGoods));
	}

	return true;
}
bool CCollectionLayout::DecordFromByteArray(uchar* pByte, long& pos)
{
	char str[256];
	m_strOrginName=_GetStringFromByteArray(pByte, pos,str);
	m_strCollectionName=_GetStringFromByteArray(pByte, pos,str);	
	m_strStartScriptName=_GetStringFromByteArray(pByte, pos,str);
	m_strEndScriptName=_GetStringFromByteArray(pByte, pos,str);
	m_strSuccScriptName=_GetStringFromByteArray(pByte,pos,str);

	m_dwCollectionID=_GetLongFromByteArray(pByte, pos);
	m_eOccuID=(eDeputyOccu)_GetLongFromByteArray(pByte, pos);
	m_dwPicID=_GetLongFromByteArray(pByte, pos);
	m_dwRadarID=_GetLongFromByteArray(pByte, pos);
	m_dwMixTimes=_GetLongFromByteArray(pByte, pos);
	m_dwMaxTimes=_GetLongFromByteArray(pByte, pos);
	m_dwCollectLevel=_GetLongFromByteArray(pByte, pos);
	m_dwActionID = _GetDwordFromByteArray(pByte,pos);
	/*m_iRoleID=_GetLongFromByteArray(pByte, pos);
	m_dwRoleStep=_GetLongFromByteArray(pByte, pos);*/
	int size=_GetLongFromByteArray(pByte,pos);
	for (int i=0;i<size;i++)
	{
		tagQuestGoods goods;
		_GetBufferFromByteArray(pByte,pos,&goods,sizeof(tagQuestGoods));
		m_vecQuestGoods.push_back(goods);
	}
	m_byBlockType=_GetByteFromByteArray(pByte,pos);

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		tgConditionGoods conditongoods;
		_GetBufferFromByteArray(pByte,pos,&conditongoods,sizeof(tgConditionGoods));
		m_listGoodsCondition.push_back(conditongoods);
	}

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		tgOddsGoods oddsgoods;
		_GetBufferFromByteArray(pByte,pos,&oddsgoods,sizeof(tgOddsGoods));
		m_listGainedGoods.push_back(oddsgoods);
	}

	return true;
}

bool CFacture::Init(TiXmlElement* p)
{
	if(p==NULL)return false;

	m_dwFactureID=atoi(p->Attribute("id"));
	m_strFactureName=p->Attribute("name");
	m_eArtisanID=(eDeputyOccu)atoi(p->Attribute("artisanID"));
	m_wType=atoi(p->Attribute("type"));
	m_dwFactureLevel=atoi(p->Attribute("skilllevel"));

	TiXmlElement* children=p->FirstChildElement();
	for (; children!=NULL; children=children->NextSiblingElement())
	{
		if (!strcmp(children->Value(),"Tool"))
		{
			tgGoods goods;
			string name=children->Attribute("goodsname");
			goods.iGoodNum=atoi(children->Attribute("num"));
			if (name.size()<=48)
			{
				memcpy(goods.cGoodsName,name.c_str(),name.size()+1);
			}
			else
			{
				memcpy(goods.cGoodsName,name.c_str(),48);
				goods.cGoodsName[48]='\0';

				Log4c::Warn(ROOT_MODULE, "输出错误信息：制作配置中名字过长");
			}
			//goods.dwGoodsIndex=CGoodsFactory::QueryGoodsIDByOriginalName(goods.cGoodsName);
			m_listTools.push_back(goods);
		}
		if (!strcmp(children->Value(),"NpcRange"))
		{
			tgNpcDistance npc;
			string name=children->Attribute("npcname");
			npc.iDistace=atoi(children->Attribute("distance"));
			if (name.size()<=48)
			{
				memcpy(npc.cNpcName,name.c_str(),name.size()+1);
			}
			else
			{
				memcpy(npc.cNpcName,name.c_str(),48);
				npc.cNpcName[48]='\0';

				Log4c::Warn(ROOT_MODULE, "输出错误信息：制作配置中名字过长");
			}
			m_listNpcDis.push_back(npc);
		}
		if (!strcmp(children->Value(),"Material"))
		{
			tgGoods goods;
			string name=children->Attribute("goodsname");
			goods.iGoodNum=atoi(children->Attribute("goodsnum"));
			if (name.size()<=48)
			{
				memcpy(goods.cGoodsName,name.c_str(),name.size()+1);
			}
			else
			{
				memcpy(goods.cGoodsName,name.c_str(),48);
				goods.cGoodsName[48]='\0';

				Log4c::Warn(ROOT_MODULE, "输出错误信息：制作配置中名字过长");
			}
			//goods.dwGoodsIndex=CGoodsFactory::QueryGoodsIDByOriginalName(goods.cGoodsName);
			m_listMaterial.push_back(goods);
		}
		if (!strcmp(children->Value(),"GainedGoods"))
		{
			tgGoods goods;
			string name=children->Attribute("goodsname");
			goods.iGoodNum=atoi(children->Attribute("goodsnum"));
			if (name.size()<=48)
			{
				memcpy(goods.cGoodsName,name.c_str(),name.size()+1);
			}
			else
			{
				memcpy(goods.cGoodsName,name.c_str(),48);
				goods.cGoodsName[48]='\0';

				Log4c::Warn(ROOT_MODULE, "输出错误信息：制作配置中名字过长");
			}
			TiXmlElement* childnode= children->FirstChildElement();
			for (; childnode!=NULL; childnode=childnode->NextSiblingElement())
			{
				if (!strcmp(childnode->Value(),"Maker"))
				{
					goods.bMaker = atoi(childnode->Attribute("bool"));
				}
			}
			
			//goods.dwGoodsIndex=CGoodsFactory::QueryGoodsIDByOriginalName(goods.cGoodsName);
			m_listGainedGoods.push_back(goods);
		}
		if (!strcmp(children->Value(),"AdvancedGainedGood"))
		{
			tgGoods goods;
			string name=children->Attribute("goodsname");
			goods.iGoodNum=atoi(children->Attribute("goodsnum"));
			if (name.size()<=48)
			{
				memcpy(goods.cGoodsName,name.c_str(),name.size()+1);
			}
			else
			{
				memcpy(goods.cGoodsName,name.c_str(),48);
				goods.cGoodsName[48]='\0';

				Log4c::Warn(ROOT_MODULE, "输出错误信息：制作配置中名字过长");
			}
			TiXmlElement* childnode= children->FirstChildElement();
			for (; childnode!=NULL; childnode=childnode->NextSiblingElement())
			{
				if (!strcmp(childnode->Value(),"Maker"))
				{
					goods.bMaker = atoi(childnode->Attribute("bool"));
				}
			}
			//goods.dwGoodsIndex=CGoodsFactory::QueryGoodsIDByOriginalName(goods.cGoodsName);
			m_listAdvancedGainedGood.push_back(goods);
		}
		if (!strcmp(children->Value(),"Quotiety"))
		{
			string str = children->Attribute("quotiety");
			m_fQuotiety = (float)atof(str.c_str());
		}
		else
		{
		}
	}
	return true;
}
bool CFacture::AddToByteArray(vector<uchar>* pByteArray)
{
	_AddToByteArray(pByteArray,m_dwFactureID) ;
	_AddToByteArray(pByteArray,m_strFactureName.c_str()) ;
	_AddToByteArray(pByteArray,(long)m_eArtisanID);
	_AddToByteArray(pByteArray,m_wType);
	_AddToByteArray(pByteArray,m_dwFactureLevel);
	_AddToByteArray(pByteArray,m_fQuotiety);
	_AddToByteArray(pByteArray,(long)m_listTools.size());
	for (GOODS_ITTER it=m_listTools.begin(); it!=m_listTools.end(); it++)
	{
		_AddToByteArray(pByteArray,(uchar*)&(*it),sizeof(tgGoods));
	}

	_AddToByteArray(pByteArray,(long)m_listNpcDis.size());// m_listNpcDis;
	for (NPC_ITTER it=m_listNpcDis.begin(); it!=m_listNpcDis.end(); it++)
	{
		_AddToByteArray(pByteArray,(uchar*)&(*it),sizeof(tgNpcDistance));
	}

	_AddToByteArray(pByteArray,(long)m_listMaterial.size());
	for (GOODS_ITTER it=m_listMaterial.begin(); it!=m_listMaterial.end(); it++)
	{
		_AddToByteArray(pByteArray,(uchar*)&(*it),sizeof(tgGoods));
	}

	_AddToByteArray(pByteArray,(long)m_listGainedGoods.size());
	for (GOODS_ITTER it=m_listGainedGoods.begin(); it!=m_listGainedGoods.end(); it++)
	{
		_AddToByteArray(pByteArray,(uchar*)&(*it),sizeof(tgGoods));
	}

	_AddToByteArray(pByteArray,(long)m_listAdvancedGainedGood.size());
	for (GOODS_ITTER it=m_listAdvancedGainedGood.begin(); it!=m_listAdvancedGainedGood.end(); it++)
	{
		_AddToByteArray(pByteArray,(uchar*)&(*it),sizeof(tgGoods));
	};

	return true;
}
bool CFacture::DecordFromByteArray(uchar* pByte, long& pos)
{
	char str[256];
	m_dwFactureID=_GetLongFromByteArray(pByte,pos);
	m_strFactureName=_GetStringFromByteArray(pByte,pos,str);
	m_eArtisanID=(eDeputyOccu)_GetLongFromByteArray(pByte,pos);
	m_wType=_GetDwordFromByteArray(pByte,pos);
	m_dwFactureLevel=_GetLongFromByteArray(pByte,pos);
	m_fQuotiety = _GetFloatFromByteArray(pByte,pos);
	int size=0;
	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		tgGoods goods;
		_GetBufferFromByteArray(pByte,pos,(uchar*)&goods, sizeof(tgGoods));
		m_listTools.push_back(goods);
	}

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		tgNpcDistance npcdistance;
		_GetBufferFromByteArray(pByte,pos,(uchar*)&npcdistance, sizeof(tgNpcDistance));
		m_listNpcDis.push_back(npcdistance);
	}

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		tgGoods goods;
		_GetBufferFromByteArray(pByte,pos,(uchar*)&goods, sizeof(tgGoods));
		m_listMaterial.push_back(goods);
	}

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		tgGoods goods;
		_GetBufferFromByteArray(pByte,pos,(uchar*)&goods, sizeof(tgGoods));
		m_listGainedGoods.push_back(goods);
	}

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		tgGoods goods;
		_GetBufferFromByteArray(pByte,pos,(uchar*)&goods, sizeof(tgGoods));
		m_listAdvancedGainedGood.push_back(goods);
	}

	return true;
}
//protected:
//	// <Facture id="11234" name="dadfa"  artisanID="1" type="1"  skilllevel="12">
//	//制造物id
//	ulong		m_dwFactureID;
//	//制造名字
//	string		m_dwFactureName;
//	//工匠id
//	eDeputyOccu 
//	//产生的物品类型
//	ushort		
//	//需要制作技能等级
//	ulong		
//protected:
//	//工具列表
//	list<tgGoods> m_listTools;
//	//npc距离列表
//	list<tgNpcDistance> m_listNpcDis;
//	//材料列表
//	list<tgGoods> m_listMaterial;
//	//获取物品列表
//	list<tgGoods> m_listGainedGoods;
//	//高级获取物品列表
//	list<tgGoods> m_listAdvancedGainedGood;
//Public:
//	typedef list<tgGoods>::iterator GOODS_ITTER;
//	typedef list<tgNpcDistance>::iterator NPC_ITTER;
//};

ulong CGapExp::TranslateColorCode(const char* chr)
{
	ulong color=0;

	for (int i=0;i<8;i++)
	{
		color<<=4;

		if (*(chr+i)>='0' && *(chr+i)<='9')
			color=color+(*(chr+i)-'0');

		else if(*(chr+i)>='a' && *(chr+i)<='f')
			color=color+(*(chr+i)-'a')+10;

		else if(*(chr+i)>='A' && *(chr+i)<='F')
			color=color+(*(chr+i)-'A')+10;

	}
	return color;
}

//class CFactureStudy
//{
//Public:
CFactureStudy::CFactureStudy()
{
}
CFactureStudy::~CFactureStudy()
{
}
bool CFactureStudy::Init(TiXmlElement* p)
{
	if (p==NULL)return false;
	
	m_strFactureNpcName=p->Attribute("npcname");

	TiXmlElement* children=p->FirstChildElement();
	for (; children!=NULL; children=children->NextSiblingElement())
	{
		if (!strcmp(children->Value(),"MakeSkill"))
		{
			study stu;
			stu.FactureID=atoi(children->Attribute("id"));
			stu.Cost=atoi(children->Attribute("cost"));
			m_vecFactureList.push_back(stu);
		}
	}
	return true;
}
bool CFactureStudy::AddToByteArray(vector<uchar>* pByteArray)
{
	_AddToByteArray(pByteArray,m_strFactureNpcName.c_str());
	_AddToByteArray(pByteArray,(long)m_vecFactureList.size());
	for (size_t a=0; a<m_vecFactureList.size(); a++)
	{
		_AddToByteArray(pByteArray,m_vecFactureList[a].FactureID);
		_AddToByteArray(pByteArray,m_vecFactureList[a].Cost);
	}
	return true;
}
bool CFactureStudy::DecordFromByteArray(uchar* pByte, long& pos)
{
	char str[256];
	m_strFactureNpcName=_GetStringFromByteArray(pByte,pos,str);
	long size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		study stu;
		stu.FactureID=_GetLongFromByteArray(pByte,pos);
		stu.Cost=_GetLongFromByteArray(pByte,pos);
		m_vecFactureList.push_back(stu);
	}
	return true;
}

bool CFactureStudy::FindFacture(ulong dwFactureID)const
{
	for (size_t a=0; a< m_vecFactureList.size(); a++)
	{
		if (m_vecFactureList[a].FactureID == dwFactureID)
		{
			return true;
		}
	}
	return false;
}

ulong CFactureStudy::FindFactureCost(ulong dwFactureID)const
{
	for (size_t a=0; a< m_vecFactureList.size(); a++)
	{
		if (m_vecFactureList[a].FactureID== dwFactureID)
		{
			return m_vecFactureList[a].Cost;
		}
	}
	return 0;
}

//private:
//	struct study  
//	{
//		ulong FactureID;
//		ulong Cost;
//	};
//	string m_strFactureNpcName;
//	vector<study> m_vecFactureList;
//};

//class CDeputyOccuSystem
//{
//Public:
CDeputyOccuSystem::CDeputyOccuSystem(){}
CDeputyOccuSystem::~CDeputyOccuSystem(){}
bool CDeputyOccuSystem::Init(const char* filename)
{
	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		//输出错误信息
		Log4c::Error(ROOT_MODULE,"%-15s CDeputyOccuSystem::Init(%s) failed",__FUNCTION__,filename); 
		return false;
	}

	TiXmlNode *pNode = NULL; 		
	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox;
	//载入技能配置文件
	if( !T_dox.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		Log4c::Error(ROOT_MODULE,"%-15s CDeputyOccuSystem::Init(%s) failed",__FUNCTION__,filename);
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	Relese();
	pNode=m_Tdoc->FirstChild("DeputyOccu");

	if (pNode==NULL)
	{
		//无节点，输出错误

		Log4c::Warn(ROOT_MODULE, "输出错误信息：副职业系统配置文件配置出错");
		return false;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息

		Log4c::Warn(ROOT_MODULE, "输出错误信息：副职业系统配置文件配置出错");
		return false;
	}


	TiXmlElement* p=te->FirstChildElement();



	for (; p!=NULL; p=p->NextSiblingElement())
	{
		string file;

		if (!strcmp(p->Value(),"CollectionListFile"))
		{
			file=p->Attribute("name");
			InitCollectionList(file.c_str());
		}
		else if (!strcmp(p->Value(),"CollectionExpFile"))
		{
			file=p->Attribute("name");
			InitCollectionGapExpList(file.c_str());
		}
		else if (!strcmp(p->Value(),"CollectionGradeFile"))
		{
			file=p->Attribute("name");
			InitCollectionGradeExpList(file.c_str());
		}
		else if (!strcmp(p->Value(),"FactureListFile"))
		{
			file=p->Attribute("name");
			InitFactureList(file.c_str());
		}
		else if (!strcmp(p->Value(),"FactureExpFile"))
		{
			file=p->Attribute("name");
			InitFactureGapExpList(file.c_str());
		}
		else if (!strcmp(p->Value(),"FactureGradeFile"))
		{
			file=p->Attribute("name");
			InitFactureGradeExpList(file.c_str());
		}
		else if (!strcmp(p->Value(),"FactureStudyFile"))
		{
			file=p->Attribute("name");
			InitFactureStudyList(file.c_str());
		}
		else if (!strcmp(p->Value(),"FactureSortFile"))
		{
			file=p->Attribute("name");
			InitFactureSortList(file.c_str());
		}
		else if (!strcmp(p->Value(),"DOccuSkillFile"))
		{
			file=p->Attribute("name");
			InitDeputyOccuSkillID(file.c_str());
		}
		else if (!strcmp(p->Value(),"CollectionSuccSpring"))
		{
			m_strCollectionSuccSpring=p->Attribute("name");
		}
		else if (!strcmp(p->Value(),"FactureSuccSpring"))
		{
			m_strFactureSuccSpring=p->Attribute("name");
		}
		else
		{

			Log4c::Warn(ROOT_MODULE, "输出错误信息：无效的单个文件名");
		}
	}
	return true;
}
bool CDeputyOccuSystem::Relese()
{
	ReleseCollectionList();
	ReleseFactureList();
	ReleseCollectionGapExpList();
	ReleseFactureGapExpList();
	ReleseCollectionGradeExpList();
	ReleseFactureGradeExpList();
	ReleseFactureStudyList();
	ReleseDeputyOccuSkillID();
	return true;
}
bool CDeputyOccuSystem::AddToByteArray(vector<uchar>* pByteArray)
{	

	_AddToByteArray(pByteArray,(long)m_mapCollectionList.size());
	for (map<string,CCollectionLayout*>::iterator it=m_mapCollectionList.begin();
		it!=m_mapCollectionList.end(); it++)
	{
		//_GetBufferFromByteArray(pByteArray,(*it).second,sizeof(CCollection));
		(*it).second->AddToByteArray(pByteArray);
	}

	_AddToByteArray(pByteArray,(long)m_dwAdvancedGainedBacePro);

	_AddToByteArray(pByteArray,(long)m_mapFactureList.size());
	for (map<ulong,CFacture*>::iterator it= m_mapFactureList.begin();
		it!=m_mapFactureList.end(); it++)
	{
		//_GetBufferFromByteArray(pByteArray,(*it).second,sizeof(CFacture));
		(*it).second->AddToByteArray(pByteArray);
	}

	_AddToByteArray(pByteArray,(long)m_listCollectionGapExpList.size());
	for (list<CGapExp>::iterator it= m_listCollectionGapExpList.begin();
		it!= m_listCollectionGapExpList.end(); it++)
	{
		_AddToByteArray(pByteArray,&(*it),sizeof(CGapExp));
	}

	_AddToByteArray(pByteArray,(long)m_listFacturenGapExpList.size());
	for (list<CGapExp>::iterator it= m_listFacturenGapExpList.begin();
		it!=m_listFacturenGapExpList.end(); it++)
	{
		_AddToByteArray(pByteArray,&(*it),sizeof(CGapExp));
	}

	_AddToByteArray(pByteArray,(long)m_vecCollectionGradeExpList.size());
	for (vector<CGradeExp>::iterator it= m_vecCollectionGradeExpList.begin();
		it!=m_vecCollectionGradeExpList.end(); it++)
	{
		_AddToByteArray(pByteArray,&(*it),sizeof(CGradeExp));
	}

	_AddToByteArray(pByteArray,(long)m_vecFactureGradeExpList.size());
	for (vector<CGradeExp>::iterator it= m_vecFactureGradeExpList.begin();
		it!=m_vecFactureGradeExpList.end(); it++)
	{
		_AddToByteArray(pByteArray,&(*it),sizeof(CGradeExp));
	}

	_AddToByteArray(pByteArray,(long)m_mapStudyList.size());
	for (map<string,CFactureStudy*>::iterator it= m_mapStudyList.begin();
		it!=m_mapStudyList.end(); it++)
	{
		(*it).second->AddToByteArray(pByteArray);
	}
	
	_AddToByteArray(pByteArray,(long)m_mapFactureSort.size());
	for (map<ulong,string>::iterator it=m_mapFactureSort.begin();
		it!=m_mapFactureSort.end(); it++)
	{
		_AddToByteArray(pByteArray,(*it).first);
		_AddToByteArray(pByteArray,(*it).second.c_str());
	}

	_AddToByteArray(pByteArray,m_dwCollectMaxGrade);
	_AddToByteArray(pByteArray,m_dwFactureMaxGrade);

	_AddToByteArray(pByteArray,&m_tgDOccuSkillRecord,sizeof(DeputyOccuSkillRecord));

	_AddToByteArray(pByteArray,m_strCollectionSuccSpring.c_str());
	_AddToByteArray(pByteArray,m_strFactureSuccSpring.c_str());

	return true;
}

bool CDeputyOccuSystem::DecordFromByteArray(uchar* pByte, long& pos)
{
	long size=0;
	Relese();

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		CCollectionLayout* collection= new CCollectionLayout;
		collection->DecordFromByteArray(pByte,pos);
		map<string,CCollectionLayout*>::iterator it= m_mapCollectionList.find(collection->GetOrginName());
		if (it==m_mapCollectionList.end())
		{
			m_mapCollectionList[collection->GetOrginName()]=collection;
		}
		else
		{
			delete collection;
		}
	}

	m_dwAdvancedGainedBacePro= _GetDwordFromByteArray(pByte,pos);

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0 ; a<size; a++)
	{
		CFacture* facture= new CFacture;
		facture->DecordFromByteArray(pByte,pos);
		map<ulong,CFacture*>::iterator it=m_mapFactureList.find(facture->GetFactureID());
		if (it==m_mapFactureList.end())
		{
			m_mapFactureList[facture->GetFactureID()]=facture;
		}
		else
		{
			delete facture;
		}
	}

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		CGapExp fapexp;
		_GetBufferFromByteArray(pByte,pos,&fapexp,sizeof(CGapExp));
		m_listCollectionGapExpList.push_back(fapexp);
	}

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		CGapExp fapexp;
		_GetBufferFromByteArray(pByte,pos,&fapexp,sizeof(CGapExp));
		m_listFacturenGapExpList.push_back(fapexp);
	}

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		CGradeExp gradeexp;
		_GetBufferFromByteArray(pByte,pos,&gradeexp,sizeof(CGradeExp));
		m_vecCollectionGradeExpList.push_back(gradeexp);
	}

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		CGradeExp gradeexp;
		_GetBufferFromByteArray(pByte,pos,&gradeexp,sizeof(CGradeExp));
		m_vecFactureGradeExpList.push_back(gradeexp);
	}

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0 ; a<size; a++)
	{
		CFactureStudy* stud= new CFactureStudy;
		stud->DecordFromByteArray(pByte,pos);
		map<string, CFactureStudy*>::iterator it=m_mapStudyList.find(stud->GetNpcName());
		if (it==m_mapStudyList.end())
		{
			m_mapStudyList[stud->GetNpcName()]=stud;
		}
		else
		{
			delete stud;
		}
	}

	size=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<size; a++)
	{
		ulong FactureType=_GetLongFromByteArray(pByte,pos);
		char str[1024];
		string name=_GetStringFromByteArray(pByte,pos,str);
		map<ulong,string>::iterator it= m_mapFactureSort.find(FactureType);
		if (it==m_mapFactureSort.end())
		{
			m_mapFactureSort[FactureType]=name;
		}
	}

	m_dwCollectMaxGrade=_GetDwordFromByteArray(pByte,pos);
	m_dwFactureMaxGrade=_GetDwordFromByteArray(pByte,pos);

	_GetBufferFromByteArray(pByte,pos,&m_tgDOccuSkillRecord,sizeof(DeputyOccuSkillRecord));
	char str[1024];
	m_strCollectionSuccSpring=_GetStringFromByteArray(pByte,pos,str);
	m_strFactureSuccSpring=_GetStringFromByteArray(pByte,pos,str);
	
	for (map<string,CFactureStudy*>::iterator it= m_mapStudyList.begin();
		it!= m_mapStudyList.end(); it++)
	{
		CFactureStudy* pFS= it->second;
		if(pFS==NULL) continue;
		set<DOccuTypeLevel> FactureLevelecord;
		for (size_t a=0; a< pFS->GetStudySum(); a++)
		{
			
			ulong FactureID= pFS->GetFactureID(a);
			const CFacture* pFacture= CDeputyOccuSystem::GetFacture(FactureID);
			if (pFacture==NULL) continue;
			ulong FactureLevel= pFacture->GetFactureLevel();
			ulong FactureType= pFacture->GetArtisanID();
			DOccuTypeLevel TypeAndLevel;
			TypeAndLevel.Set(FactureType,FactureLevel);
			if (FactureLevelecord.find(TypeAndLevel)== FactureLevelecord.end())
			{
				FactureLevelecord.insert(TypeAndLevel);
				m_mapStudyClew[TypeAndLevel].push_back(it->first);
			}
		}
	}
	return true;
}
//Public
const CCollectionLayout*	CDeputyOccuSystem::GetCollectionLayout(const char* Name)
{
	map<string,CCollectionLayout*>::iterator it=m_mapCollectionList.find(Name);
	if (it!= m_mapCollectionList.end())
	{
		return (*it).second;
	}
	else
		return NULL;
}
const CFacture*				CDeputyOccuSystem::GetFacture(ulong ID)
{
	map<ulong,CFacture*>::iterator it= m_mapFactureList.find(ID);
	if (it!= m_mapFactureList.end())
	{
		return (*it).second;
	}
	else
		return NULL;
}
long						CDeputyOccuSystem::GetCollectionExp(long level)
{
	list<CGapExp>::iterator it= m_listCollectionGapExpList.begin();

	for (; it!= m_listCollectionGapExpList.end(); it++)
	{
		if ( level>= (*it).MinLevelGap && level <= (*it).MaxLevelGap)
		{
			return (*it).GapExp;
		}
	}
	return 0;
}
ulong						CDeputyOccuSystem::GetCollectionColor(long level)
{
	list<CGapExp>::iterator it= m_listCollectionGapExpList.begin();

	for (; it!= m_listCollectionGapExpList.end(); it++)
	{
		if ( level>= (*it).MinLevelGap && level <= (*it).MaxLevelGap)
		{
			return (*it).Color;
		}
	}
	return 0;
}
ulong						CDeputyOccuSystem::GetCollectionNeededExp(ulong level)
{
	if (m_vecCollectionGradeExpList.size()<=level)
	{
		return 0;
	}
	return m_vecCollectionGradeExpList[level].NeededExp;
}
long						CDeputyOccuSystem::GetFactureExp(long level)
{
	list<CGapExp>::iterator it= m_listFacturenGapExpList.begin();

	for (; it!= m_listFacturenGapExpList.end(); it++)
	{
		if ( level>= (*it).MinLevelGap && level <= (*it).MaxLevelGap)
		{
			return (*it).GapExp;
		}
	}
	return 0;
}
long						CDeputyOccuSystem::GetFactureCredit(long level)
{
	list<CGapExp>::iterator it= m_listFacturenGapExpList.begin();

	for (; it!= m_listFacturenGapExpList.end(); it++)
	{
		if ( level>= (*it).MinLevelGap && level <= (*it).MaxLevelGap)
		{
			return (*it).GapCredit;
		}
	}
	return 0;
}
ulong						CDeputyOccuSystem::GetFactureColor(long level)
{
	list<CGapExp>::iterator it= m_listFacturenGapExpList.begin();

	for (; it!= m_listFacturenGapExpList.end(); it++)
	{
		if ( level>= (*it).MinLevelGap && level <= (*it).MaxLevelGap)
		{
			return (*it).Color;
		}
	}
	return 0;
}
ulong						CDeputyOccuSystem::GetFactureNeededExp(ulong level)
{
	if (m_vecFactureGradeExpList.size()<=level)
	{
		return 0;
	}
	return m_vecFactureGradeExpList[level].NeededExp;
}
 const CFactureStudy*		CDeputyOccuSystem::GetFactureStudy(const char* NpcName)
{
	map<string, CFactureStudy*> ::iterator it= m_mapStudyList.find(NpcName);
	if (it!= m_mapStudyList.end())
	{
		return (*it).second;
	}
	return NULL;
}

 const vector<string>*		CDeputyOccuSystem::GetStudyStudyClew(ulong dwDOccuType,ulong dwLevel)
 {
	 DOccuTypeLevel TypeAndLevel;
	 TypeAndLevel.Set(dwDOccuType,dwLevel);
	 static map<DOccuTypeLevel,vector<string>>::iterator it=m_mapStudyClew.find(TypeAndLevel);
	 if (it!= m_mapStudyClew.end())
	 {
		 return &(it->second);
	 }
	 return NULL;
 }

 const string*				CDeputyOccuSystem::GetFactureSortName(ulong FactureType)
 {
	 map<ulong , string>::iterator it= m_mapFactureSort.find(FactureType);
	 if (it!= m_mapFactureSort.end())
	 {
		 return &(*it).second;
	 }
	 it= m_mapFactureSort.find(0);
	 if (it!= m_mapFactureSort.end())
	 {
		 return &(*it).second;
	 }
	 return NULL;
 }

 //获取副职业技能
 ulong CDeputyOccuSystem::GetDeputyOccuSkillID(OccuBeavioral type,eDeputyOccu DOccuType)
 {
	 if (type== CDeputyOccuSystem::OB_TaskCollection)
		 return m_tgDOccuSkillRecord.TaskSkill;
	 if (DOccuType>0 && DOccuType<=3)
	 {
		 if (type== CDeputyOccuSystem::OB_Collection)
		 {
			 return m_tgDOccuSkillRecord.CollectiontSkill[DOccuType-1];
		 }
		 else if (type==  CDeputyOccuSystem::OB_Facture)
		 {
			 return m_tgDOccuSkillRecord.FactureSkill[DOccuType-1];
		 }
	 }
	 return 0;
 }
//protected:
bool CDeputyOccuSystem::InitCollectionList(const char* filename)
{
	
	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		return false;
	}

	TiXmlNode *pNode = NULL; 		
	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox;
	//载入技能配置文件
	if( !T_dox.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	//ReleseCollectionList();

	pNode=m_Tdoc->FirstChild("CollectionList");

	if (pNode==NULL)
	{
		//无节点，输出错误

		Log4c::Warn(ROOT_MODULE, "输出错误信息：采集文件出错");
		return false;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息

		Log4c::Warn(ROOT_MODULE, "输出错误信息：采集文件出错");
		return false;
	}

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{
		if (!strcmp(p->Value(),"Collection"))
		{
			CCollectionLayout * collection=new CCollectionLayout;
			collection->Init(p);
			map<string,CCollectionLayout*>::iterator it=m_mapCollectionList.find(collection->GetOrginName());
			if (it==m_mapCollectionList.end())
			{
				m_mapCollectionList[collection->GetOrginName()]=collection;
			}
			else
			{

				Log4c::Warn(ROOT_MODULE, "输出错误信息：重复原始名的采集物配置");
				delete collection;
			}

			
		}
		else
		{

			Log4c::Warn(ROOT_MODULE, "输出错误信息：采集文件有无效配置");
		} 
	}
	return true;
}
bool CDeputyOccuSystem::InitFactureList(const char* filename)
{
	TiXmlNode *pNode = NULL; 
	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		return false;
	}
		
	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox;
	//载入技能配置文件
	if( !T_dox.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	//ReleseFactureList();

	pNode=m_Tdoc->FirstChild("FactureList");

	if (pNode==NULL)
	{
		//无节点，输出错误

		Log4c::Warn(ROOT_MODULE, "输出错误信息：制作文件出错");
		return false;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息

		Log4c::Warn(ROOT_MODULE, "输出错误信息：制作文件出错");
		return false;
	}

	m_dwAdvancedGainedBacePro= atoi(te->Attribute("AdvancedGainedBacePro"));

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{
		if (!strcmp(p->Value(),"Facture"))
		{
			CFacture * facture=new CFacture;
			facture->Init(p);

			map<ulong,CFacture*>::iterator it=m_mapFactureList.find(facture->GetFactureID());
			if (it==m_mapFactureList.end())
			{
				m_mapFactureList[facture->GetFactureID()]=facture;
			}
			else
			{

				Log4c::Warn(ROOT_MODULE, "输出错误信息：重复原始名的制作条目");
				delete facture; 
			}


		}
		else
		{

			Log4c::Warn(ROOT_MODULE, "输出错误信息：制作文件有无效配置");
		} 

	}
	return true;
}
bool CDeputyOccuSystem::InitCollectionGapExpList(const char* filename)
{
	TiXmlNode *pNode = NULL; 		

	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		return false;
	}

	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox;
	//载入技能配置文件
	if( !T_dox.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	//ReleseCollectionGapExpList();

	pNode=m_Tdoc->FirstChild("ExpList");

	if (pNode==NULL)
	{
		//无节点，输出错误

		Log4c::Warn(ROOT_MODULE, "输出错误信息：采集获得经验值列表错误");
		return false;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息

		Log4c::Warn(ROOT_MODULE, "输出错误信息：采集获得经验值列表错误");
		return false;
	}

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{
		if (!strcmp(p->Value(),"ExpStep"))
		{
			CGapExp gapexp;
			gapexp.MinLevelGap=atoi(p->Attribute("minlevel"));
			gapexp.MaxLevelGap=atoi(p->Attribute("maxlevel"));
			gapexp.GapExp=atoi(p->Attribute("exp"));
			gapexp.Color=gapexp.TranslateColorCode(p->Attribute("color"));//atoi(p->Attribute("color"));
			m_listCollectionGapExpList.push_back(gapexp);
		}
		else
		{

			Log4c::Warn(ROOT_MODULE,"输出错误信息：采集获得经验值列表有无效配置");
		}

	}
	return  true;
}
bool CDeputyOccuSystem::InitFactureGapExpList(const char* filename)
{
	TiXmlNode *pNode = NULL; 		

	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		return false;
	}
	
	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox;
	//载入技能配置文件
	if( !T_dox.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	//ReleseFactureGapExpList();

	pNode=m_Tdoc->FirstChild("ExpList");

	if (pNode==NULL)
	{
		//无节点，输出错误

		Log4c::Warn(ROOT_MODULE, "输出错误信息：制作获得经验值列表错误");
		return false;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息

		Log4c::Warn(ROOT_MODULE, "输出错误信息：制作获得经验值列表错误");
		return false;
	}

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{
		if (!strcmp(p->Value(),"ExpStep"))
		{
			CGapExp gapexp;
			gapexp.MinLevelGap=atoi(p->Attribute("minlevel"));
			gapexp.MaxLevelGap=atoi(p->Attribute("maxlevel"));
			gapexp.GapExp=atoi(p->Attribute("exp"));
			gapexp.GapCredit=atoi(p->Attribute("credit"));
			gapexp.Color=gapexp.TranslateColorCode(p->Attribute("color"));

			m_listFacturenGapExpList.push_back(gapexp);
		}
		else
		{

			Log4c::Warn(ROOT_MODULE, "输出错误信息：制作获得经验值列表有无效配置");
		}
	}
	return true;
}
bool CDeputyOccuSystem::InitCollectionGradeExpList(const char* filename)
{
	TiXmlNode *pNode = NULL; 		

	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		return false;
	}
		
	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox;
	//载入技能配置文件
	if( !T_dox.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	//ReleseCollectionGradeExpList();

	pNode=m_Tdoc->FirstChild("GradeList");

	if (pNode==NULL)
	{
		//无节点，输出错误

		Log4c::Warn(ROOT_MODULE, "输出错误信息：采集等级经验值列表错误");
		return false;
	}


	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息

		Log4c::Warn(ROOT_MODULE, "输出错误信息：采集等级经验值列表错误");
		return false;
	}
	m_dwCollectMaxGrade=atoi(te->Attribute("maxgrade"));
	m_vecCollectionGradeExpList.resize(m_dwCollectMaxGrade+1);

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{
		if (!strcmp(p->Value(),"GradeExp"))
		{
			CGradeExp gradeexp;
			gradeexp.Level=atoi(p->Attribute("level"));
			gradeexp.NeededExp=atoi(p->Attribute("exp"));
			if (m_vecCollectionGradeExpList.size()<gradeexp.Level)
			{
				continue;
			}
			if (m_vecCollectionGradeExpList[gradeexp.Level].NeededExp!=0)
			{
				continue;
			}
			m_vecCollectionGradeExpList[gradeexp.Level]=gradeexp;
		}
		else
		{

			Log4c::Warn(ROOT_MODULE, "输出错误信息：采集等级经验值列表有无效配置");
		}
	}
	return true;
}
bool CDeputyOccuSystem::InitFactureGradeExpList(const char* filename)
{
	TiXmlNode *pNode = NULL; 		

	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		return false;
	}

	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox;
	//载入技能配置文件
	if( !T_dox.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	//ReleseFactureGradeExpList();

	pNode=m_Tdoc->FirstChild("GradeList");

	if (pNode==NULL)
	{
		//无节点，输出错误

		Log4c::Warn(ROOT_MODULE, "输出错误信息：制作等级经验值列表错误");
		return false;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息

		Log4c::Warn(ROOT_MODULE,"输出错误信息：制作等级经验值列表错误");
		return false;
	}
	m_dwFactureMaxGrade=atoi(te->Attribute("maxgrade"));
	m_vecFactureGradeExpList.resize(m_dwFactureMaxGrade+1);

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{
		if (!strcmp(p->Value(),"GradeExp"))
		{
			CGradeExp gradeexp;
			gradeexp.Level=atoi(p->Attribute("level"));
			gradeexp.NeededExp=atoi(p->Attribute("exp"));
			if (m_vecFactureGradeExpList.size()<gradeexp.Level)
			{
				continue;
			}
			if (m_vecFactureGradeExpList[gradeexp.Level].NeededExp!=0)
			{
				continue;
			}
			m_vecFactureGradeExpList[gradeexp.Level]=gradeexp;
		}
		else
		{

			Log4c::Warn(ROOT_MODULE, "输出错误信息：制作等级经验值列表有无效配置");
		}
	}

	return true;
}
bool CDeputyOccuSystem::InitFactureStudyList(const char* filename)
{
	TiXmlNode *pNode = NULL; 		

	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		return false;
	}

	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox;
	//载入技能配置文件
	if( !T_dox.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	//ReleseFactureStudyList();

	pNode=m_Tdoc->FirstChild("MakeStudyList");

	if (pNode==NULL)
	{
		//无节点，输出错误

		Log4c::Warn(ROOT_MODULE, "输出错误信息：制作学习列表错误");
		return false;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息

		Log4c::Warn(ROOT_MODULE, "输出错误信息：制作学习列表错误");
		return false;
	}

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{
		if (!strcmp(p->Value(),"List"))
		{
			CFactureStudy* stud=new CFactureStudy;
			stud->Init(p);
			map<string,CFactureStudy*>::iterator it=m_mapStudyList.find(stud->GetNpcName());
			if (it==m_mapStudyList.end())
			{
				m_mapStudyList[stud->GetNpcName()]=stud;
			}
			else
			{

				Log4c::Warn(ROOT_MODULE,"输出错误信息：制作学习列表npc重复配置");
				delete stud;
			}
				
		}
		else
		{

			Log4c::Warn(ROOT_MODULE, "输出错误信息：制作学习列表有无效配置");
		}
	}
	

	return true;
}

bool CDeputyOccuSystem::InitFactureSortList(const char* filename)
{
	TiXmlNode *pNode = NULL; 		

	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		return false;
	}

	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox;
	//载入技能配置文件
	if( !T_dox.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	//ReleseFactureSortList();

	pNode=m_Tdoc->FirstChild("FactureType");

	if (pNode==NULL)
	{
		//无节点，输出错误
		Log4c::Warn(ROOT_MODULE, "输出错误信息：制作分类列表错误");
   
		return false;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息

		Log4c::Warn(ROOT_MODULE, "输出错误信息：制作分类列表错误");
		return false;
	}

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{
		if (!strcmp(p->Value(),"OccuType"))
		{
			TiXmlElement* children=p->FirstChildElement();
			for (; children!=NULL; children=children->NextSiblingElement())
			{
				ulong OneFactureType= atoi(children->Attribute("id"));
				map<ulong, string>::iterator it= m_mapFactureSort.find(OneFactureType);
				if (it== m_mapFactureSort.end())
				{
					m_mapFactureSort[OneFactureType]= children->Attribute("name");
				}
				else
				{

					Log4c::Warn(ROOT_MODULE, "输出错误信息：制作分类列表重复配置");
				}
			}

		}
		else
		{

			Log4c::Warn(ROOT_MODULE, "输出错误信息：制作分类列表有无效配置");
		}
	}
	return true;
}

bool CDeputyOccuSystem::InitDeputyOccuSkillID(const char* filename)
{
	TiXmlNode *pNode = NULL; 		

	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		return false;
	}
	
	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox;
	//载入技能配置文件
	if( !T_dox.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	//ReleseDeputyOccuSkillID();

	pNode=m_Tdoc->FirstChild("DoccuSkillRecord");

	if (pNode==NULL)
	{
		//无节点，输出错误

		Log4c::Warn(ROOT_MODULE, "输出错误信息：副职业技能列表错误");
		return false;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息

		Log4c::Warn(ROOT_MODULE,"输出错误信息：副职业技能列表错误");
		return false;
	}

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{
		if (!strcmp(p->Value(),"Collection_Smithing"))
		{
			int id=0;
			p->Attribute("id",&id); 
			m_tgDOccuSkillRecord.CollectiontSkill[0]= id;
		}
		else if (!strcmp(p->Value(),"Collection_Metallurgy"))
		{
			int id=0;
			p->Attribute("id",&id); 
			m_tgDOccuSkillRecord.CollectiontSkill[1]= id;
		}
		else if (!strcmp(p->Value(),"Collection_Sewing"))
		{
			int id=0;
			p->Attribute("id",&id); 
			m_tgDOccuSkillRecord.CollectiontSkill[2]= id;
		}
		else if (!strcmp(p->Value(),"Facture_Smithing"))
		{
			int id=0;
			p->Attribute("id",&id); 
			m_tgDOccuSkillRecord.FactureSkill[0]= id;
		}
		else if (!strcmp(p->Value(),"Facture_Metallurgy"))
		{
			int id=0;
			p->Attribute("id",&id); 
			m_tgDOccuSkillRecord.FactureSkill[1]= id;
		}
		else if (!strcmp(p->Value(),"Facture_Sewing"))
		{
			int id=0;
			p->Attribute("id",&id); 
			m_tgDOccuSkillRecord.FactureSkill[2]= id;
		}
		else if (!strcmp(p->Value(),"Other_Task"))
		{
			int id=0;
			p->Attribute("id",&id); 
			m_tgDOccuSkillRecord.TaskSkill= id;
		}
		else
		{

			Log4c::Warn(ROOT_MODULE,"输出错误信息：副职业技能列表有无效配置");
		}
	}
	return true;
}

bool CDeputyOccuSystem::ReleseCollectionList()
{	
	if (m_mapCollectionList.size()==0)
	{
		return true;
	}
	for (map<string,CCollectionLayout*>::iterator it=m_mapCollectionList.begin();
		it!=m_mapCollectionList.end(); it++)
	{
		delete (*it).second;
	}
	m_mapCollectionList.clear();
	return true;
}
bool CDeputyOccuSystem::ReleseFactureList()
{
	if (m_mapFactureList.size()==0)
	{
		return true;
	}
	for (map<ulong,CFacture*>::iterator it= m_mapFactureList.begin();
		it!=m_mapFactureList.end(); it++)
	{
		delete (*it).second;
	}
	m_mapFactureList.clear();
	return true;
}
bool CDeputyOccuSystem::ReleseCollectionGapExpList()
{
	m_listCollectionGapExpList.clear();
	return true;
}
bool CDeputyOccuSystem::ReleseFactureGapExpList()
{
	m_listFacturenGapExpList.clear();
	return true;
}
bool CDeputyOccuSystem::ReleseCollectionGradeExpList()
{
	m_vecCollectionGradeExpList.clear();
	return true;
}
bool CDeputyOccuSystem::ReleseFactureGradeExpList()
{
	m_vecFactureGradeExpList.clear();
	return true;
}
bool CDeputyOccuSystem::ReleseFactureStudyList()
{
	if (m_mapStudyList.size()==0)
	{
		return true;
	}
	for (map<string,CFactureStudy*>::iterator it= m_mapStudyList.begin();
		it!=m_mapStudyList.end(); it++)
	{
		delete (*it).second;
	}
	m_mapStudyList.clear();
	return true;
}
bool CDeputyOccuSystem::ReleseFactureSortList()
{
	m_mapFactureSort.clear();
	return true;
}
bool CDeputyOccuSystem::ReleseDeputyOccuSkillID()
{
	return true;
}

//protected:
//	//采集列表
//	static map<string,CCollection*>		m_mapCollectionList;
//	//制作列表
//	static map<ulong,CFacture*>			m_mapFactureList;
//	//采集经验列表
//	static list<CGapExp>				m_listCollectionGapExpList;
//	//制作经验列表
//	static list<CGapExp>				m_listFacturenGapExpList;
//	//采集等级列表
//	static vector<CGradeExp>			m_vecCollectionGradeExpList;
//	//制作等级列表
//	static vector<CGradeExp>			m_vecFactureGradeExpList;
//	//制作学习列表
//	static map<string,CFactureStudy*>	m_mapStudyList;
//};
