#include "StdAfx.h"
#include "MonsterList.h"
#include "GoodsSetup.h"
#include "../Common/Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

map<string, CMonsterList::tagMonster> CMonsterList::m_mapMonsterList;       // 原始Monster列表(strOrginName, tagMonster)
vector<CMonsterList::tagDropGoods*> CMonsterList::m_vecDropList;            // drop的管理
map<string , CMonsterList::tgMonsterDrop> CMonsterList::m_mapMonsterDrop;   // monster掉落
map<int, int> CMonsterList::m_mapRegionDrop;                                // region掉落
vector<int> CMonsterList::m_vecLevelDrop;                                   // 等级掉落
int CMonsterList::m_iWordDrop;                                              //世界掉落（索引）	



CMonsterList::CMonsterList(void)
{
}

CMonsterList::~CMonsterList(void)
{
    m_mapMonsterList.clear();
    //m_vecDropList;//.clear();
    for (vector<tagDropGoods*>::iterator it=m_vecDropList.begin(); it!=m_vecDropList.end(); it++)
    {
        delete *it;
    }
    m_vecDropList.clear();
}

// 从文件读取原始物品列表
BOOL CMonsterList::LoadMonsterList(const char* filename)
{
    m_mapMonsterList.clear();

    CRFile* prfile = rfOpen(filename);
    if(prfile == NULL)
    {
        Log4c::Error(ROOT_MODULE, "%-15s file '%s' can't found!",__FUNCTION__, filename);
        return FALSE;
    }

    stringstream stream;
    prfile->ReadToStream(stream);
    rfClose(prfile);

    while(ReadTo(stream, "*"))
    {
        tagMonster stMonster;
        int a=0;
        string strOriginalName;
        string strName;
        string strCollectionName;
        string strBornScript;
        string strFightScript;
        string strGBHScript;
        string strDeadScript;
        string strTitleName;
        string strCycleScript;
        string strSearchEnemyScript;
        string strFindTargetScriptName;
        string strKillTargetScriptName;
		string strCatchScriptName;

        stream	>>  stMonster.dwIndex
            >>  strOriginalName                 //原始名//..先读进到一个临时str变量中，判断在对
            >> 	strName                         //名字
            >> 	*(int*)(&stMonster.byBlockType)				//是否有阻挡
            >>	*(int*)(&stMonster.byIsunbeatable)
            >>  *(int*)&stMonster.DropFlag
            >>  strCollectionName				//采集物名字
            >>	strTitleName					//称谓名字
            >>  stMonster.dwTitleColor			//称谓显示颜色
            >>  stMonster.dwTitlePicID			//称谓图片
            >> 	*(int*)(&stMonster.byCountry)	//国家ID
            >> 	*(int*)(&stMonster.eNType)   //Npc类别 (NNPC,Monster,Guard)
            >>  stMonster.wBossLevel			//boss等级
            >> 	stMonster.dwPicID				//图形ID
            >> 	stMonster.dwPicLevel			//图形等级
            >> 	stMonster.dwNameColor			//名字颜色编号,对应到客户端配置]
            >> 	stMonster.dwColorOfLevel        //等级颜色
            >> 	stMonster.dwHpBarColor			//血条颜色，0：表示不显示血条 1：红色 2：黄色
            >> 	stMonster.dwSoundID				//声音ID
            >> 	*(int*)(&stMonster.byCatchable)	//是否可以捕获
            >> 	stMonster.dwMaxCatchFastness	//最大捕获抗性
			>>	stMonster.dwServantID			//对应侍从ID
            >> 	stMonster.wFigure				//体形(0:1x1 1:3*3 2:5*5)*
            >> 	stMonster.wLevel			    //等级
            >> 	stMonster.dwExp					//经验值*
			>>	stMonster.dwDExp				//修为经验值
            >>	stMonster.dwOccuPoint			//职业点数
            >> 	stMonster.dwMaxHP               //最大hp
            >> 	stMonster.wMinAtk				//最小攻击力		    
            >> 	stMonster.wMaxAtk				//最大攻击力
            >> 	stMonster.wMinMAtk				//最小法术攻击力
            >> 	stMonster.wMaxMAtk				//最大法术攻击力
            >> 	stMonster.wHit					//命中能力
            >> 	stMonster.wDef					//防御力
            >>  stMonster.wMDef					//魔法防御力
            >> 	stMonster.wBlock				//格挡
            >> 	stMonster.wParry				//招架
            >>  stMonster.wImmunity				//免伤
            >>  stMonster.wPierce				//穿透
            >> 	stMonster.wDodge				//闪避能力
            >> 	stMonster.fMoveSpeed			//移动速度
            >>	stMonster.fFightMoveSpeed		//战斗移动速度
            >>  stMonster.fTurningSpeed			//转身速度
            >> 	stMonster.wWillDef;				//意志抗性
        for(a=0;a<ET_Max;a++)
        {
            stream  >> stMonster.wElemDef[a];
        }

        stream	>> 	stMonster.wHpRecoverSpeed		//生命恢复速度
            >> 	stMonster.wMPRecoverSpeed		//魔法恢复速度
            >> 	stMonster.wBaseConstiDef;			//体质抗性

        for(a=0; a<DET_Max;a++)
        {
            stream >>stMonster.wDeBuffDef[a];
        }

        stream	>> 	stMonster.wMinStopTimes		//最少停留次数
            >> 	stMonster.wMaxStopTimes			//最多停留次数
            >> 	stMonster.wMinRunTimes			//最少移动步数
            >>  stMonster.wMaxRunTimes			//最多移动步数
            >> 	stMonster.GuardRange			//警戒范围*
            >> 	stMonster.ChaseRange			//追击范围*
            >>  stMonster.PeaceMoveRange		//和平移动范围
            >> 	stMonster.wAIModel              //AI模式
            >>	stMonster.wCanTalk				//是否可对话
            >> 	*(int*)(&stMonster.eClass_)//怪物类型
            >> 	*(int*)(&stMonster.eEType)	//元素类别
            >> 	stMonster.wStopTime				//停留的时间(在一个格子中间停留帧数)
            >> 	stMonster.wAIHangupType;			//AI间隙
        for(a=0; a<C_Max; a++)
        {
            stream   >>stMonster.wClassDam[a];
        }

        for(a=0; a<ET_Max; a++)
        {
            stream   >>stMonster.wElemDam[a];
        }

        stream>>stMonster.wMoveRandomValue   //活力 
            >>stMonster.wCri				 //物理爆击率
            >>stMonster.wCriDamg			 //物理暴击伤害
            >>stMonster.wCriDef				 //物理爆击抗性
            >>stMonster.wMCri				 //魔法暴击率
            >>stMonster.wMCriDamg			 //魔法暴击伤害
            >>stMonster.wMCriDef;			 //魔法爆击抗性


        stream>>stMonster.fHurtProportion    //重伤比率
            >>stMonster.dwGuardTime          //警戒时间
            >>stMonster.fEscapePoint         //逃跑底线
            >>stMonster.wSkillProportion     //技能概率
            >>stMonster.dwSkillSpace	     //技能间隔
            >>*(int*)(&stMonster.byIsFlyToPeace)//转换为和平状态是否飞回去
            >>*(int*)(&stMonster.byIsResumeAll)//转换为和平状态是否完全恢复 
            >>stMonster.ReturnRange		 //归位距离
            >>stMonster.dwChaseTime;		 //追击时间，在追击范围外的没受到攻击转化为和平的时间

        for( a=0; a<MONSTER_AI_REFERRENCE_NUM; a++)
        {
            stream>>stMonster.dwAIReferrence [a];//ai参考  1——5
        }
        stream
            >>strBornScript					//出生出发脚本
            >>strFightScript				//战斗触发脚本
            >>strGBHScript 					//重伤触发脚本
            >>strDeadScript					//死亡触发脚本
            >>strCycleScript				//周期触发脚本
            >>strSearchEnemyScript			//选择目标脚本
            >> strFindTargetScriptName		//寻找到目标脚本
            >> strKillTargetScriptName		//杀死目标脚本
			>> strCatchScriptName;			//捕获脚本

        stream
            >>stMonster.m_arrElementProperty[ME_Earth].uArrPro[AD_Attack]
        >>stMonster.m_arrElementProperty[ME_Water].uArrPro[AD_Attack]
        >>stMonster.m_arrElementProperty[ME_Fire].uArrPro[AD_Attack]
        >>stMonster.m_arrElementProperty[ME_Wind].uArrPro[AD_Attack]
        >>stMonster.m_arrElementProperty[ME_Bright].uArrPro[AD_Attack]
        >>stMonster.m_arrElementProperty[ME_Dark].uArrPro[AD_Attack]

        >>stMonster.m_arrElementProperty[ME_Earth].uArrPro[AD_Defend]
        >>stMonster.m_arrElementProperty[ME_Water].uArrPro[AD_Defend]
        >>stMonster.m_arrElementProperty[ME_Fire].uArrPro[AD_Defend]
        >>stMonster.m_arrElementProperty[ME_Wind].uArrPro[AD_Defend]
        >>stMonster.m_arrElementProperty[ME_Bright].uArrPro[AD_Defend]
        >>stMonster.m_arrElementProperty[ME_Dark].uArrPro[AD_Defend]
        >>stMonster.tauntSize;
        //读取结束

        size_t nLen = strOriginalName.size ();  //原始名字
        if( nLen<50)
        {
            memcpy(stMonster.cOriginalName,strOriginalName.c_str(),nLen);
            stMonster.cOriginalName[nLen]='\0';
        }
        else
        {
            strncpy(stMonster.cOriginalName,strOriginalName.c_str(),50-1);
            stMonster.cOriginalName[50-1]='\0';
            Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "OriginalName"));
        }

        nLen = strName.size ();   //名字
        if(nLen<50)
        {
            memcpy(stMonster.cName ,strName.c_str(),nLen);
            stMonster.cName[nLen] ='\0';
        }
        else
        {
            memcpy(stMonster.cName ,strName.c_str(),50-1);
            stMonster.cName[50-1] ='\0';
           Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "Name"));

        }

        nLen = strCollectionName.size ();   //名字
        if(nLen<50)
        {
            memcpy(stMonster.cCollectionName ,strCollectionName.c_str(),nLen);
            stMonster.cCollectionName[nLen] ='\0';
        }
        else
        {
            memcpy(stMonster.cCollectionName ,strCollectionName.c_str(),50-1);
            stMonster.cCollectionName[50-1] ='\0';
           Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "CollectionName"));

        }


        nLen = strTitleName.size ();   //名字
        if(nLen<50)
        {
            memcpy(stMonster.cTitleName ,strTitleName.c_str(),nLen);
            stMonster.cTitleName[nLen] ='\0';
        }
        else
        {
            memcpy(stMonster.cTitleName ,strTitleName.c_str(),50-1);
            stMonster.cTitleName[50-1] ='\0';
            Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "TitleName"));
        }
        nLen = strBornScript.size ();  //出生触发脚本
        if(nLen<MAX_PATH)
        {
            memcpy(stMonster.cBornScriptName ,strBornScript.c_str(),nLen);
            stMonster.cBornScriptName[nLen] ='\0';
        }
        else
        {
            memcpy(stMonster.cBornScriptName ,strBornScript.c_str(),MAX_PATH-1);
            stMonster.cBornScriptName[MAX_PATH-1] ='\0';
            Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "BornScriptName"));

        }

        nLen = strFightScript.size ();  //战斗触发脚本
        if(nLen<MAX_PATH)
        {
            memcpy(stMonster.cFightScriptName ,strFightScript.c_str(),nLen);
            stMonster.cFightScriptName[nLen] ='\0';
        }
        else
        {
            memcpy(stMonster.cFightScriptName ,strFightScript.c_str(),MAX_PATH-1);
            stMonster.cFightScriptName[MAX_PATH-1] ='\0';
           Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "FightScriptName"));

        }

        nLen = strGBHScript.size ();  //重伤触发
        if(nLen<MAX_PATH)
        {
            memcpy(stMonster.cGBHScriptName ,strGBHScript.c_str(),nLen);
            stMonster.cGBHScriptName[nLen] ='\0';
        }
        else
        {
            memcpy(stMonster.cGBHScriptName ,strGBHScript.c_str(),MAX_PATH-1);
            stMonster.cGBHScriptName[MAX_PATH-1] ='\0';
            Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "GBHScriptName"));

        }

        nLen = strDeadScript.size ();  //死亡触发脚本
        if(nLen<MAX_PATH)
        {
            memcpy(stMonster.cDeadScriptName ,strDeadScript.c_str(),nLen);
            stMonster.cDeadScriptName[nLen] ='\0';
        }
        else
        {
            memcpy(stMonster.cDeadScriptName ,strDeadScript.c_str(),MAX_PATH-1);
            stMonster.cDeadScriptName[MAX_PATH-1] ='\0';
           Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "DeadScriptName"));
        }

        nLen = strCycleScript.size ();  //周期触发脚本
        if(nLen<MAX_PATH)
        {
            memcpy(stMonster.cCycleScriptName ,strCycleScript.c_str(),nLen);
            stMonster.cCycleScriptName[nLen] ='\0';
        }
        else
        {
            memcpy(stMonster.cCycleScriptName ,strCycleScript.c_str(),MAX_PATH-1);
            stMonster.cCycleScriptName[MAX_PATH-1] ='\0';
            Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "CycleScriptName"));
        }

        nLen = strSearchEnemyScript.size ();  //选择技能脚本
        if(nLen<MAX_PATH)
        {
            memcpy(stMonster.cSearchEnemyScriptName ,strSearchEnemyScript.c_str(),nLen);
            stMonster.cSearchEnemyScriptName[nLen] ='\0';
        }
        else
        {
            memcpy(stMonster.cSearchEnemyScriptName ,strSearchEnemyScript.c_str(),MAX_PATH-1);
            stMonster.cSearchEnemyScriptName[MAX_PATH-1] ='\0';
            Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "SearchEnemyScriptName"));
        }
        //
        nLen = strFindTargetScriptName.size ();  //寻找到目标
        if(nLen<MAX_PATH)
        {
            memcpy(stMonster.cFindTargetScriptName ,strFindTargetScriptName.c_str(),nLen);
            stMonster.cFindTargetScriptName[nLen] ='\0';
        }
        else
        {
            memcpy(stMonster.cFindTargetScriptName ,strFindTargetScriptName.c_str(),MAX_PATH-1);
            stMonster.cFindTargetScriptName[MAX_PATH-1] ='\0';
            Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "FindTargetScriptName"));
        }
        //
        nLen = strKillTargetScriptName.size ();  //杀死目标
        if(nLen<MAX_PATH)
        {
            memcpy(stMonster.cKillTargetScriptName ,strKillTargetScriptName.c_str(),nLen);
            stMonster.cKillTargetScriptName[nLen] ='\0';
        }
        else
        {
            memcpy(stMonster.cKillTargetScriptName ,strKillTargetScriptName.c_str(),MAX_PATH-1);
            stMonster.cKillTargetScriptName[MAX_PATH-1] ='\0';
            Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "KillTargetScriptName"));
        }
		//
		nLen = strCatchScriptName.size ();  //捕获脚本
		if(nLen<MAX_PATH)
		{
			memcpy(stMonster.cCatchScriptName ,strCatchScriptName.c_str(),nLen);
			stMonster.cCatchScriptName[nLen] ='\0';
		}
		else
		{
			memcpy(stMonster.cCatchScriptName ,strCatchScriptName.c_str(),MAX_PATH-1);
			stMonster.cCatchScriptName[MAX_PATH-1] ='\0';
			Log4c::Warn(ROOT_MODULE,FormatText("WS_MONSTER_1", "CatchScriptName"));
		}
        //str类型判断结束
        //值判断部分
        if (stMonster.eNType<0 || stMonster.eNType>=NT_Max ||
            stMonster.eClass_<0 || stMonster.eClass_>=C_Max||
            stMonster.eEType<0 || stMonster.eEType>= ET_Max ||
            stMonster.wFigure>10)
        {
            //输出错误
            Log4c::Warn(ROOT_MODULE, GetText("WS_MONSTER_2"));
        }

        if (stMonster.byBlockType==1)
        {
            stMonster.byBlockType=4;
        }
        else if (stMonster.byBlockType==2)
        {
            stMonster.byBlockType=3;
        }
        else
            stMonster.byBlockType=0;

        m_mapMonsterList[stMonster.cOriginalName] = stMonster;
    }

    return TRUE;
}

BOOL CMonsterList::LoadMonsterDrop(const char* filename)
{
    CRFile* prFile = rfOpen(filename);
    if (prFile == NULL)
    {
        return FALSE;
    }
    TiXmlNode *pNode = NULL; 		

    TiXmlDocument T_dox(filename);
    TiXmlDocument* m_Tdoc = & T_dox; 

    if(!m_Tdoc->LoadData(prFile->GetData(),prFile->GetDatalen()))
    {
        //输出错误信息：错误的文件名
        return FALSE;
    }
    rfClose(prFile);
    pNode=m_Tdoc->FirstChild("DropList");

    if (pNode==NULL)
    {
        //无节点，输出错误
        return FALSE;
    }

    TiXmlElement* te=pNode->ToElement();//获取node 的指针
    if (te==NULL)
    {
        //输出错误信息
        return FALSE;
    }


    TiXmlElement* p=te->FirstChildElement();

    for (; p!=NULL; p=p->NextSiblingElement())
    {
        if (!strcmp(p->Value(),"Drop"))
        {
            TiXmlElement* children=p->FirstChildElement();
            int sharedrop=DROPINDEX_NULL;
            list<string>  sharemonstername;

            for (;children!=NULL; children=children->NextSiblingElement())
            {
                if (!strcmp(children->Value(),"Drop_Block"))
                {
                    tagDropGoods* dropgoods=LoadOneDrop(children);
                    if (dropgoods!=NULL)
                    {
                        m_vecDropList.push_back(dropgoods);
                        sharedrop = (int)(m_vecDropList.size()-1);
                    }
                }
                else if (!strcmp(children->Value(),"MonsterName"))
                {
                    string name=children->Attribute("name");
                    tagDropGoods* dropgoods=LoadOneDrop(children);
                    if (dropgoods!=NULL)
                    {
                        m_vecDropList.push_back(dropgoods);
                        int index = (int)(m_vecDropList.size()-1);
                        m_mapMonsterDrop[name].OwnDropIndex=index;
                    }
                    sharemonstername.push_back(name);
                }
            }

            for (list<string> ::iterator it= sharemonstername.begin(); it!= sharemonstername.end(); it++)
            {
                m_mapMonsterDrop[*it].ShareDropIndex=sharedrop;
            }
        }
    }
    return TRUE; 
}
BOOL CMonsterList::LoadActivityDrop(const char* filename)
{
    CRFile* prFile = rfOpen(filename);
    if (prFile == NULL)
    {
        return FALSE;
    }
    TiXmlNode *pNode = NULL; 		
    m_iWordDrop = DROPINDEX_NULL;
    TiXmlDocument T_dox(filename);
    TiXmlDocument* m_Tdoc = & T_dox; 

    if(!m_Tdoc->LoadData(prFile->GetData(),prFile->GetDatalen()))
    {
        //输出错误信息：错误的文件名
        return FALSE;
    }
    rfClose(prFile);
    pNode=m_Tdoc->FirstChild("DropList");

    if (pNode==NULL)
    {
        //无节点，输出错误
        return FALSE;
    }

    TiXmlElement* te=pNode->ToElement();//获取node 的指针
    if (te==NULL)
    {
        //输出错误信息
        return FALSE;
    }


    TiXmlElement* p=te->FirstChildElement();



    for (; p!=NULL; p=p->NextSiblingElement())
    {
        if (!strcmp(p->Value(),"World_Drop"))
        {
            tagDropGoods* dropgoods=LoadOneDrop(p);
            if (dropgoods!=NULL)
            {
                m_vecDropList.push_back(dropgoods);
                m_iWordDrop = (int)(m_vecDropList.size()-1);
            }
        }
        else if (!strcmp(p->Value(),"Grade_Drop"))
        {
            tagDropGoods* dropgoods=LoadOneDrop(p);
            if (dropgoods==NULL)
            {
                break;
            }
            m_vecDropList.push_back(dropgoods);
            int dropindex = (int)(m_vecDropList.size()-1);

            long MinLevel=0;
            long MaxLevel=0;
            if (!GetTwoNum(p->Attribute("levels"),MinLevel, MaxLevel))
            {
                break;
            }
            if ((long)m_vecLevelDrop.size() <= MaxLevel)
            {
                m_vecLevelDrop.resize(MaxLevel+1);
            }
            for (int a=MinLevel; a<=MaxLevel; a++)
            {
                m_vecLevelDrop[a]=dropindex;
            }
        }
        else if (!strcmp(p->Value(),"Monster_Drop"))
        {
            string name=p->Attribute("name");
            tagDropGoods* dropgoods=LoadOneDrop(p);
            if (dropgoods==NULL)
            {
                break;
            }
            m_vecDropList.push_back(dropgoods);
            int dropindex = (int)(m_vecDropList.size()-1);
            m_mapMonsterDrop[name].ActivityIndex=dropindex;
        }
        else
        {

        }
    }
    return TRUE; 
}
BOOL CMonsterList::LoadRegionDrop(const char* filename,long regionID)
{
    TiXmlNode *pNode = NULL; 		

    TiXmlDocument T_dox(filename);
    TiXmlDocument* m_Tdoc = & T_dox; 

    if(!m_Tdoc->LoadFile())
    {
        //输出错误信息：错误的文件名
        return FALSE;
    }

    pNode=m_Tdoc->FirstChild("RegionDrop");

    if (pNode==NULL)
    {
        //无节点，输出错误
        return FALSE;
    }

    TiXmlElement* te=pNode->ToElement();//获取node 的指针
    if (te==NULL)
    {
        //输出错误信息
        return FALSE;
    }


    TiXmlElement* p=te->FirstChildElement();



    for (; p!=NULL; p=p->NextSiblingElement())
    {
        tagDropGoods* dropgoods=LoadOneDrop(p);
        if (dropgoods!=NULL)
        {
            m_vecDropList.push_back(dropgoods);
            m_mapRegionDrop[regionID] = (int)(m_vecDropList.size()-1);
        }
    }
    return TRUE; 
}
CMonsterList::tagDropGoods* CMonsterList::LoadOneDrop(TiXmlElement* te)
{
    if (te==NULL)
    {
        return NULL;
    }


    TiXmlElement* p=te->FirstChildElement();
    if (p==NULL)
    {
        return NULL;
    }

    CMonsterList::tagDropGoods* md= new CMonsterList::tagDropGoods;

    for (; p!=NULL; p=p->NextSiblingElement() )
    {
        if (!strcmp(p->Value(),"DropGoods"))
        {
            tagDropGoods::tagDrop drop;

			drop.strName=p->Attribute("goodsname");
			if (!GetTwoNum(p->Attribute("odds"),drop.lOdds,drop.lMaxOdds))
			{
				//错误
				break;
			}
			
			if( !GetTwoNum( p->Attribute( "level" ), drop.lMinLevel, drop.lMaxLevel ) )
			{
				// failed, set default
				drop.lMinLevel = drop.lMaxLevel = 0;
			}
			drop.fLevelAttenuation = (float)atof(p->Attribute("levelattenuation"));
			drop.fLevelAttenuationLimit = (float)atof(p->Attribute("levelattenuationlimit"));
			drop.lGoodsIndex = GoodsSetup::QueryGoodsIDByOriginalName( drop.strName.c_str() );

            if( !GetTwoNum( p->Attribute( "level" ), drop.lMinLevel, drop.lMaxLevel ) )
            {
                // failed, set default
                drop.lMinLevel = drop.lMaxLevel = 0;
            }
            drop.fLevelAttenuation = (float)atof(p->Attribute("levelattenuation"));
            drop.fLevelAttenuationLimit = (float)atof(p->Attribute("levelattenuationlimit"));
            drop.lGoodsIndex = GoodsSetup::QueryGoodsIDByOriginalName( drop.strName.c_str() );

            md->vectorDrop.push_back(drop);
        }
        else if (!strcmp(p->Value(),"DropMoney"))
        {
            tagDropGoods::tagDrop drop;
            drop.strName=p->Attribute("goodsname");

			if (!GetTwoNum(p->Attribute("odds"),drop.lOdds,drop.lMaxOdds))
			{
				//错误
				break;
			}
			if (!GetTwoNum(p->Attribute("dropnum"),drop.lMinMoney,drop.lMaxMoney))
			{
				//错误
				break;
			}
			drop.fLevelAttenuation = (float)atof(p->Attribute("levelattenuation"));
			drop.fLevelAttenuationLimit = (float)atof(p->Attribute("levelattenuationlimit"));
			drop.lGoodsIndex = GoodsSetup::QueryGoodsIDByOriginalName( drop.strName.c_str() );

			md->vectorDrop.push_back(drop);
		}
		else if (!strcmp(p->Value(), "DropGroup"))
		{
			tagDropGoods::tagFixedDrop fa;
			int goodsnum=0;
			TiXmlElement* children=p->FirstChildElement();
			for (; children!=NULL; children=children->NextSiblingElement())
			{
				goodsnum++;
				if (!strcmp(children->Value(),"OneDrop"))
				{
					tagDropGoods::tagOneGoods onegoods;
					onegoods.strName=children->Attribute("dropname");
					onegoods.lGoodsIndex=GoodsSetup::QueryGoodsIDByOriginalName( onegoods.strName.c_str() );
					onegoods.lProbability=atoi(children->Attribute("dropodds"));
					
					if( !GetTwoNum( children->Attribute( "level" ), onegoods.lMinLevel, onegoods.lMaxLevel ) )
					{
						// failed, set default
						onegoods.lMinLevel = onegoods.lMaxLevel = 0;
					}

                    if( !GetTwoNum( children->Attribute( "level" ), onegoods.lMinLevel, onegoods.lMaxLevel ) )
                    {
                        // failed, set default
                        onegoods.lMinLevel = onegoods.lMaxLevel = 0;
                    }

                    fa.vecOneDropGroup.push_back(onegoods);
                }
                else
                {
                }
            }
            fa.iMaxDropNum=atoi(p->Attribute("maxdrop"));
            fa.iMinDropNum=atoi(p->Attribute("mindrop"));

            const char *attr = p->Attribute( "levelattenuation" );
            fa.fLevelAttenuation = attr != NULL ? (float)atof(attr) : 0.0f;
            attr = p->Attribute( "levelattenuationlimit" ); 
            fa.fLevelAttenuationLimit = attr != NULL ? (float)atof(attr) : 0.0f;

            // 获取组调用几率
            if( !GetTwoNum( p->Attribute( "odds" ), fa.lOdds, fa.lMaxOdds ) )
            {
                fa.lOdds = 1;
                fa.lMaxOdds = 1;
            }

            if (fa.iMinDropNum<0)
            {
                fa.iMaxDropNum=0;
            }

            if (fa.iMinDropNum>fa.iMaxDropNum)
            {
                fa.iMinDropNum=fa.iMaxDropNum;
            }

            md->vecFixedDrop.push_back(fa);
        }
        else
        {
            //错误信息
        }
    }

    if (md->vecFixedDrop.size()==0 && md->vectorDrop.size()==0)
    {
        delete md;
        return NULL;
    }
    return md;
}
BOOL CMonsterList::GetTwoNum(const char* str, long &first, long &second)
{
    if( str == 0 )
    {
        return FALSE;
    }

    int len = (int)strlen(str);
    if (len==0 || len>64)
    {
        return FALSE;
    }

    string firstnum;
    string secongnum;

    int place=0;

    for (; place<len ; place++)
    {
        char c=*(str+place);
        if (c=='/')
        {
            ++place;
            break;
        }
        else if (c>='0' && c<='9')
        {
            firstnum+=c;
        }
        else
        {
            return FALSE;
        }
    }

    if (place >=len)
    {
        return FALSE;
    }

    for (; place<len ; place++)
    {
        char c=*(str+place);
        if (c=='/')
        {
            ++place;
            break;
        }
        else if (c>='0' && c<='9')
        {
            secongnum+=c;
        }
        else
        {
            return FALSE;
        }
    }

    if (firstnum.size()==0 || secongnum.size()==0)
    {
        return FALSE;
    }

    first=atoi(firstnum.c_str());
    second=atoi(secongnum.c_str());
    return TRUE;
}

bool CMonsterList::LoadSkillList(const char* filename)
{
    CRFile *prfile = rfOpen(filename);
    if(NULL == prfile)
    {
        //输出错误信息
        Log4c::Error(ROOT_MODULE,"%-15s CDeputyOccuSystem::Init(%s) failed",__FUNCTION__,filename);
        return FALSE;
    }

    TiXmlNode *pNode = NULL; 		
    TiXmlDocument T_dox;//TiXmlDocument T_dox(filename);
    TiXmlDocument* m_Tdoc = & T_dox;
    //载入技能配置文件
    if( !T_dox.LoadData(prfile->GetData(),prfile->GetDatalen()) )
    {
        //输出错误信息
        Log4c::Error(ROOT_MODULE,"%-15s CDeputyOccuSystem::Init(%s) failed",__FUNCTION__,filename);
        rfClose(prfile);
        return FALSE;
    }
    rfClose(prfile);

    pNode=m_Tdoc->FirstChild("MonsterSkillList");

    if (pNode==NULL)
    {
        return FALSE;
    }

    TiXmlElement* te=pNode->ToElement();//获取node 的指针
    if (te==NULL)
    {
        return FALSE;
    }
    TiXmlElement* p=te->FirstChildElement();

    for (; p!=NULL; p=p->NextSiblingElement())
    {
        if (!strcmp(p->Value(),"Monster"))
        {
            int BaseSkill=0;
            int CommonSkill=0;
            int CycleSkill=0;
            //tagMonster* point=0;
            const char * name=p->Attribute("OriginalName");
            tagMonster* point=GetPropertyByName(name);
            if (point== NULL) continue;

            for (TiXmlElement* skill= p->FirstChildElement(); 
                skill!= NULL; skill= skill->NextSiblingElement())
            {
                const char * ppp= skill->Value();

                if (!strcmp(skill->Value(),"BaseSkill"))
                {
                    if (BaseSkill<MONSTER_SKILL_SUM)
                    {
                        tagSkillInfo info;
                        skill->Attribute("id",(int*)&info.dwSkillID);
                        skill->Attribute("level",(int*)&info.dwSkillLevel);
                        skill->Attribute("cooltime",(int*)&info.dwCoolDownTime);
                        skill->Attribute("actmodify",&info.iActModifyTime);
                        point->BaseSkillList[BaseSkill]=info;
                        ++BaseSkill;
                    }
                    else
                        continue;
                }
                else if (!strcmp(skill->Value(),"CommonSkill"))
                {
                    if (CommonSkill<MONSTER_SKILL_SUM)
                    {
                        tagSkillInfo info;
                        skill->Attribute("id",(int*)&info.dwSkillID);
                        skill->Attribute("level",(int*)&info.dwSkillLevel);
                        skill->Attribute("cooltime",(int*)&info.dwCoolDownTime);
                        skill->Attribute("actmodify",&info.iActModifyTime);
                        point->CommonSkillList[CommonSkill]=info;
                        ++CommonSkill;
                    }
                    else
                        continue;
                }
                else if (!strcmp(skill->Value(),"CycleSkill"))
                {
                    if (CycleSkill<MONSTER_SKILL_SUM)
                    {
                        tagSkillInfo info;
                        skill->Attribute("id",(int*)&info.dwSkillID);
                        skill->Attribute("level",(int*)&info.dwSkillLevel);
                        skill->Attribute("cooltime",(int*)&info.dwCoolDownTime);
                        skill->Attribute("actmodify",&info.iActModifyTime);
                        point->CycleSkillList[CycleSkill]= info;
                        ++CycleSkill;
                    }
                    else
                        continue;
                }
                else
                {

                }

            }
        }

    }

    return true;

}

// 添加到CByteArray
bool CMonsterList::AddToByteArray(vector<uchar>* pByteArray)
{
    // 怪物属性
    _AddToByteArray(pByteArray, (long)m_mapMonsterList.size());

    for(map<string, tagMonster>::iterator it=m_mapMonsterList.begin(); it!=m_mapMonsterList.end(); it++)
    {
        tagMonster *pMonster = &(*it).second;
        if( pMonster )
        {
            _AddToByteArray(pByteArray, (uchar*)pMonster, (long)sizeof(tagMonster));
        }
    }

    // 掉物品就部分
    // droplist:
    _AddToByteArray(pByteArray, (long)m_vecDropList.size());

    for(vector<tagDropGoods*>::iterator it=m_vecDropList.begin(); it!=m_vecDropList.end(); it++)
    {
        tagDropGoods *pDrop =*it;
        if( pDrop )
        {
            //_AddToByteArray(pByteArray, index);
            _AddToByteArray(pByteArray, (long)pDrop->vectorDrop.size());
            for(int i=0; i<(long)pDrop->vectorDrop.size(); i++)
            {
                _AddToByteArray(pByteArray, (char*)pDrop->vectorDrop[i].strName.c_str());
                _AddToByteArray(pByteArray, (uchar*)&pDrop->vectorDrop[i], sizeof(tagDropGoods::tagDrop)-sizeof(string));
            }
            //.....add by chenaly..07/6/22
            _AddToByteArray(pByteArray, (long)pDrop->vecFixedDrop.size());

            for (int a=0; a<(long)pDrop->vecFixedDrop.size(); a++)
            {
                _AddToByteArray(pByteArray, (long)pDrop->vecFixedDrop[a].iMinDropNum);
                _AddToByteArray(pByteArray, (long)pDrop->vecFixedDrop[a].iMaxDropNum);
                _AddToByteArray(pByteArray, (long)pDrop->vecFixedDrop[a].lOdds );
                _AddToByteArray(pByteArray, (long)pDrop->vecFixedDrop[a].lMaxOdds );
                _AddToByteArray(pByteArray, (float)pDrop->vecFixedDrop[a].fLevelAttenuation );
                _AddToByteArray(pByteArray, (float)pDrop->vecFixedDrop[a].fLevelAttenuationLimit );

                _AddToByteArray(pByteArray, (long)pDrop->vecFixedDrop[a].vecOneDropGroup.size());

                for (int b=0; b< (long)pDrop->vecFixedDrop[a].vecOneDropGroup.size(); b++)
                {
                    _AddToByteArray(pByteArray, (char*)pDrop->vecFixedDrop[a].vecOneDropGroup[b].strName.c_str());
                    _AddToByteArray(pByteArray, (long)pDrop->vecFixedDrop[a].vecOneDropGroup[b].lProbability);
                    _AddToByteArray(pByteArray, (long)pDrop->vecFixedDrop[a].vecOneDropGroup[b].lGoodsIndex);
                    _AddToByteArray(pByteArray,(long)pDrop->vecFixedDrop[a].vecOneDropGroup[b].lMinLevel);
                    _AddToByteArray(pByteArray,(long)pDrop->vecFixedDrop[a].vecOneDropGroup[b].lMaxLevel);
                }
            }
            //....end ...................
        }
    }
    // m_mapMonsterDrop
    _AddToByteArray(pByteArray, (long)m_mapMonsterDrop.size());
    for (map<string,tgMonsterDrop>::iterator it=m_mapMonsterDrop.begin(); it!=m_mapMonsterDrop.end();it++)
    {
        string name=(*it).first;
        _AddToByteArray(pByteArray,(*it).first.c_str());
        _AddToByteArray(pByteArray,&(*it).second,sizeof((*it).second));
    }
    //m_mapRegionDrop
    _AddToByteArray(pByteArray,(long)m_mapRegionDrop.size());

    for( map<int,int>::iterator it=m_mapRegionDrop.begin(); it!=m_mapRegionDrop.end(); it++)
    {
        _AddToByteArray(pByteArray,(long)(*it).first);
        _AddToByteArray(pByteArray,(long)(*it).second);
    }
    //m_vecLevelDrop
    _AddToByteArray(pByteArray,(long)m_vecLevelDrop.size());
    for (vector<int>::iterator it= m_vecLevelDrop.begin(); it!=m_vecLevelDrop.end(); it++)
    {
        _AddToByteArray(pByteArray,(long)*it);
    }
    //
    _AddToByteArray(pByteArray,(long)m_iWordDrop);
    return TRUE;
}

// 解码
bool CMonsterList::DecordFromByteArray(uchar* pByte, long& pos)
{
    Release();//清除数据，修正重读BUG
    m_mapMonsterList.clear();
    //m_mapDropGoods.clear();
    //m_vecRoadsList.clear();

    char strTemp[256];

    // 怪物属性

    long lMonsterNum = _GetLongFromByteArray(pByte, pos);
    for(int i=0; i<lMonsterNum; i++)
    {
        tagMonster stMonster;
        _GetBufferFromByteArray(pByte, pos, &stMonster, sizeof(tagMonster));

        m_mapMonsterList[string(stMonster.cOriginalName)] = stMonster;
    }

    // 掉物品部分
    //droplist
    long lDropNum = _GetLongFromByteArray(pByte, pos);
    for(int i=0; i<lDropNum; i++)
    {
        tagDropGoods* stDrop=new tagDropGoods;

        //stDrop.strMonsterOrginName = _GetStringFromByteArray(pByte, pos, strTemp);
        long lGoodsNum = _GetLongFromByteArray(pByte, pos);
        for(int j=0; j<lGoodsNum; j++)
        {
            tagDropGoods::tagDrop stGoods;
            stGoods.strName = _GetStringFromByteArray(pByte, pos, strTemp);
            _GetBufferFromByteArray(pByte, pos, &stGoods, sizeof(tagDropGoods::tagDrop)-sizeof(string));
            stDrop->vectorDrop.push_back(stGoods);
        }
        //add by chenaly .....07/6/22.......
        lGoodsNum=_GetLongFromByteArray(pByte, pos);
        for (int a=0; a< lGoodsNum; a++)
        {
            tagDropGoods::tagFixedDrop stFix;
            stFix.iMinDropNum=_GetLongFromByteArray(pByte, pos);
            stFix.iMaxDropNum=_GetLongFromByteArray(pByte, pos);
            stFix.lOdds = _GetLongFromByteArray(pByte, pos);
            stFix.lMaxOdds = _GetLongFromByteArray(pByte, pos);
            stFix.fLevelAttenuation = _GetFloatFromByteArray(pByte, pos);
            stFix.fLevelAttenuationLimit = _GetFloatFromByteArray(pByte, pos);
            long dropgroup=_GetLongFromByteArray(pByte, pos);
            for (int b=0; b<dropgroup; b++)
            {
                tagDropGoods::tagOneGoods onegoods;
                onegoods.strName=_GetStringFromByteArray(pByte, pos, strTemp);
                onegoods.lProbability=_GetLongFromByteArray(pByte, pos);
                onegoods.lGoodsIndex=_GetLongFromByteArray(pByte, pos);
                onegoods.lMinLevel=_GetLongFromByteArray(pByte,pos);
                onegoods.lMaxLevel=_GetLongFromByteArray(pByte,pos);
                stFix.vecOneDropGroup.push_back(onegoods);
            }
            stDrop->vecFixedDrop.push_back(stFix);
        }
        m_vecDropList.push_back(stDrop);
    }
    //tgMonsterDrop
    long MonsterNum= _GetLongFromByteArray(pByte, pos);
    for (int a=0; a<MonsterNum; a++)
    {
        string name=_GetStringFromByteArray(pByte,pos,strTemp);
        tgMonsterDrop monsterdrop;
        _GetBufferFromByteArray(pByte,pos,&monsterdrop,sizeof(tgMonsterDrop));
        m_mapMonsterDrop[name]=monsterdrop;
    }
    //m_mapRegionDrop;
    long RegionDropNum=_GetLongFromByteArray(pByte,pos);
    for (int a=0; a<RegionDropNum; a++)
    {
        long reiondIndex=_GetLongFromByteArray(pByte,pos);
        long dropindex=_GetLongFromByteArray(pByte,pos);
        m_mapRegionDrop[reiondIndex]=dropindex;
    }
    //m_vecLevelDrop;					//等级掉落
    long WorldDropNum=_GetLongFromByteArray(pByte,pos);
    for (int a=0; a<WorldDropNum; a++)
    {
        long dropindex=_GetLongFromByteArray(pByte,pos);
        m_vecLevelDrop.push_back(dropindex);
    }
    //m_iWordDrop;					//世界掉落（索引）	
    m_iWordDrop=_GetLongFromByteArray(pByte,pos);

    return TRUE;
}


// 返回指定的NPC属性
CMonsterList::tagMonster* CMonsterList::GetProperty(DWORD dwPicID)
{
    for(map<string, tagMonster>::iterator it=m_mapMonsterList.begin(); it!=m_mapMonsterList.end(); it++)
    {
        if( (*it).second.dwPicID == dwPicID )
        {
            return &(*it).second;
        }
    }
    return NULL;
}

CMonsterList::tagMonster* CMonsterList::GetPropertyByName(const char* strName)
{
    map<string, tagMonster>::iterator it=m_mapMonsterList.find(strName);
    if (it!=m_mapMonsterList.end())
    {
        return &(*it).second;
    }
    return NULL;
}

CMonsterList::tagMonster* CMonsterList::GetPropertyByOrginName(const char* strName)
{
    return (m_mapMonsterList.find(strName)!=m_mapMonsterList.end()) ? (&m_mapMonsterList[strName]) : NULL;
}

CMonsterList::tagMonster* CMonsterList::GetPropertyByOrginIndex(DWORD dwIndex)
{
    for(map<string, tagMonster>::iterator it=m_mapMonsterList.begin(); it!=m_mapMonsterList.end(); it++)
    {
        if( (*it).second.dwIndex == dwIndex )
        {
            return &(*it).second;
        }
    }
    return NULL;
}

/// 取得怪物掉落集合
map<string , CMonsterList::tgMonsterDrop>* CMonsterList::GetMonsterDropList()
{
	return &m_mapMonsterDrop;
}

/// 取得Drop管理集合
vector<CMonsterList::tagDropGoods*>* CMonsterList::GetDropGoodsList()
{
	return &m_vecDropList;
}

/// 取得region掉落集合
map<int, int>* CMonsterList::GetRegionDropList()
{
	return &m_mapRegionDrop;
}

/// 取得等级掉落集合
vector<int>* CMonsterList::GetLevelDropList()
{
	return &m_vecLevelDrop;
}

/// 取得世界掉落（索引）
int CMonsterList::GetWorldDropIdx()
{
	return m_iWordDrop;
}


void CMonsterList::Release()
{
    m_mapMonsterList.clear();
    for (vector<tagDropGoods*>::iterator it=m_vecDropList.begin(); it!=m_vecDropList.end(); it++)
    {
        delete *it;
    }
    m_vecDropList.clear();
}
