
#include "stdafx.h"
#include "SpriteSystem.h"
#include "goods/CGoodsFactory.h"
#include "skills/SkillFactory.h"


SpriteSystem::SpriteSystem(void)
{

}

SpriteSystem::~SpriteSystem(void)
{

}

map<LONG, DWORD> SpriteSystem::c_mapCanUseGoodsIndex;
vector<LONG> SpriteSystem::c_vectorCanUseFightSkill;
vector<LONG> SpriteSystem::c_vectorCanUseAssistSkill;
vector<LONG> SpriteSystem::c_vectorCanUseAssistGoods;

//! 读取配置
BOOL SpriteSystem::LoadSetup(void)
{
	BOOL bReturnValue = TRUE;
	//! ----------------------------根节点----------------------------
	TiXmlNode *pNode = NULL; 		
	
	CRFile* prFile = rfOpen("data/Sprite.xml");
	if (prFile == NULL)
	{
		return FALSE;
	}
	TiXmlDocument* m_Tdoc = new TiXmlDocument;

	if(!m_Tdoc->LoadData(prFile->GetData(),prFile->GetDatalen()))
		return FALSE;

	rfClose(prFile);
	pNode=m_Tdoc->FirstChild("Sprite");
	if (pNode==NULL) 
		return FALSE;

	TiXmlElement* pSprite = pNode->ToElement();//获取node 的指针
	if (pSprite==NULL) 
		return FALSE;

	//! ----------------------------获取配置----------------------------

	try
	{
		//! 自动补给物
		TiXmlElement* pAutoSupplyGoods = pSprite->FirstChildElement("AutoSupplyGoods");

		TiXmlElement* pGoods = pAutoSupplyGoods->FirstChildElement("Goods");
		for (; pGoods!=NULL; pGoods = pGoods->NextSiblingElement())
		{
			const char *pGoodsName = pGoods->Attribute("ItemName");
			LONG lGoodsBaseIndex = CGoodsFactory::QueryGoodsIDByOriginalName(pGoodsName);
			if (0 == lGoodsBaseIndex)
			{
				AddErrorLogText("读取自动补给配置失败：没有找到原始名为[%s]的物品！！！", pGoodsName);
				bReturnValue = FALSE;
				continue;
			}
			if (c_mapCanUseGoodsIndex.end() != c_mapCanUseGoodsIndex.find(lGoodsBaseIndex))
			{
				AddErrorLogText("读取自动补给配置失败：重复配置原始名为[%s]的物品！！！", pGoodsName);
				bReturnValue = FALSE;
				continue;
			}

			//! 物品应用范围
			DWORD dwBound = 0;
			const char *pBound = pGoods->Attribute("Bound");
			if (NULL == pBound)
			{
				AddErrorLogText("读取自动补给配置失败：原始名为[%s]的物品应用范围配置错误！！！", pGoodsName);
				bReturnValue = FALSE;
				continue;
			}
			LONG lBound = atoi(pBound);
			lBound = lBound % 10000;
			if(0 != lBound / 1000)	dwBound |= eGAB_POWER;
			lBound = lBound % 1000;
			if(0 != lBound / 100)	dwBound |= eGAB_SMP;
			lBound = lBound % 100;
			if(0 != lBound / 10)	dwBound |= eGAB_MP;
			lBound = lBound % 10;
			if(0 != lBound)			dwBound |= eGAB_HP;
			if(0 == dwBound)
			{
				AddErrorLogText("读取自动补给配置失败：原始名为[%s]的物品应用范围配置错误！！！", pGoodsName);
				bReturnValue = FALSE;
				continue;
			}
			
			c_mapCanUseGoodsIndex[lGoodsBaseIndex] = dwBound;
		}
        //加载战斗技能
        TiXmlElement* pAutoFightSkills = pSprite->FirstChildElement("AutoFightSkills");
        TiXmlElement* pFightSkills = pAutoFightSkills->FirstChildElement("Skills");
        for (; pFightSkills != NULL; pFightSkills = pFightSkills->NextSiblingElement("Skills"))
        {
            const char* pSkillID = pFightSkills->Attribute("ID");
            if (pSkillID == NULL)
            {
                AddErrorLogText("读取自动战斗技能配置失败：技能ID为[%s]配置错误！！！", pSkillID);
                bReturnValue = FALSE;
                continue;
            }
            LONG lID = atoi(pSkillID);
            if (FindExist(c_vectorCanUseFightSkill, lID))
            {
                AddErrorLogText("读取自动使用战斗技能配置失败：重复配置技能ID为[%s]！！！", pSkillID);
                bReturnValue = FALSE;
                continue;
            }
            c_vectorCanUseFightSkill.push_back(lID);
        }

        //加载辅助技能
        TiXmlElement* pAutoAssistSkills = pSprite->FirstChildElement("AutoAssistSkills");
        TiXmlElement* pAssistSkills = pAutoAssistSkills->FirstChildElement("Skills");
        for (; pAssistSkills != NULL; pAssistSkills = pAssistSkills->NextSiblingElement("Skills"))
        {
            const char* pSkillID = pAssistSkills->Attribute("ID");
            if (pSkillID == NULL)
            {
                AddErrorLogText("读取自动使用辅助技能配置失败：技能ID为[%s]配置错误！！！", pSkillID);
                bReturnValue = FALSE;
                continue;
            }
            LONG lAssistID = atoi(pSkillID);
            if (FindExist(c_vectorCanUseAssistSkill, lAssistID))
            {
                AddErrorLogText("读取自动使用辅助技能配置失败：重复配置技能ID为[%s]！！！", pSkillID);
                bReturnValue = FALSE;
                continue;
            }
            c_vectorCanUseAssistSkill.push_back(lAssistID);
        }
        //加载辅助道具
        TiXmlElement* pAutoAssistGoods = pSprite->FirstChildElement("AutoAssistGoods");
        TiXmlElement* pAssistGoods = pAutoAssistGoods->FirstChildElement("Goods");
        for (; pAssistGoods != NULL; pAssistGoods = pAssistGoods->NextSiblingElement("Goods"))
        {
            const char* pGoodsName = pAssistGoods->Attribute("ItemName");
            LONG lGoodsBaseIndex = CGoodsFactory::QueryGoodsIDByOriginalName(pGoodsName);
            if (lGoodsBaseIndex == 0)
            {
                AddErrorLogText("读取自动使用辅助道具配置失败：没有找到原始名为[%s]的物品道具！！！", pGoodsName);
                bReturnValue = FALSE;
                continue;
            }
            if (FindExist(c_vectorCanUseAssistGoods, lGoodsBaseIndex))
            {
                AddErrorLogText("读取自动使用辅助道具配置失败：重复配置原始名为[%s]的物品！！！", pGoodsName);
                bReturnValue = FALSE;
                continue;
            }
            c_vectorCanUseAssistGoods.push_back(lGoodsBaseIndex);
        }
	}
	catch (...)
	{
		AddErrorLogText("在读取小精灵配置的时候发生了错误，读取失败，可能的原因是配置不当。");
		if (NULL != m_Tdoc)
		{
			delete m_Tdoc;
			m_Tdoc = NULL;
		}
		return FALSE;
	}


	//! ----------------------------清除对象----------------------------
	delete m_Tdoc;
	m_Tdoc = NULL;
	return bReturnValue;
}

//! 配置编码
void SpriteSystem::SetupAddToByteArray(vector<BYTE> &ByteArray)
{
	_AddToByteArray(&ByteArray, (LONG)c_mapCanUseGoodsIndex.size());
	for (map<LONG, DWORD>::iterator ite = c_mapCanUseGoodsIndex.begin(); ite != c_mapCanUseGoodsIndex.end(); ++ite)
	{
		_AddToByteArray(&ByteArray, ite->first);
		_AddToByteArray(&ByteArray, ite->second);
	}
    //战斗技能
    _AddToByteArray(&ByteArray, (LONG)c_vectorCanUseFightSkill.size());
    for (vector<LONG>::iterator ite = c_vectorCanUseFightSkill.begin(); ite != c_vectorCanUseFightSkill.end(); ++ite)
    {
        _AddToByteArray(&ByteArray, *ite);
    }
    //辅助技能
    _AddToByteArray(&ByteArray, (LONG)c_vectorCanUseAssistSkill.size());
    for (vector<LONG>::iterator ite = c_vectorCanUseAssistSkill.begin(); ite != c_vectorCanUseAssistSkill.end(); ++ite)
    {
        _AddToByteArray(&ByteArray, *ite);
    }
    //辅助道具
    _AddToByteArray(&ByteArray, (LONG)c_vectorCanUseAssistGoods.size());
    for (vector<LONG>::iterator ite = c_vectorCanUseAssistGoods.begin(); ite != c_vectorCanUseAssistGoods.end(); ++ite)
    {
        _AddToByteArray(&ByteArray, *ite);
    }
}

//! 编码
void SpriteSystem::AddToByteArray(vector<BYTE> &ByteArray)
{
	m_AutoSupplyData.AddToByteArray(ByteArray);
}

//! 解码
void SpriteSystem::GetFormByteArray(BYTE *pByteArray, long &lPos)
{
	m_AutoSupplyData.GetFormByteArray(pByteArray, lPos);
}

//!	编码
void SpriteSystem::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	m_AutoSupplyData.CodeToDataBlock(setWriteDB);
    //编码稳定稳定时间
    //setWriteDB.AddToByteArray(m_AutoFightData.GetStableTime());
    m_AutoFightData.CodeToDataBlock(setWriteDB);
}
//!	解码
void SpriteSystem::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	m_AutoSupplyData.DecodeFromDataBlock(setReadDB);
    //解码稳定时间
    //m_AutoFightData.SetStableTime(setReadDB.GetLongFromByteArray());
    m_AutoFightData.DecodeFromDataBlock(setReadDB);
}

//! 设置稳定挂机时间
void SpriteSystem::SetStableTime(DWORD dwTime)
{
    m_AutoFightData.SetStableTime(dwTime);
}

DWORD SpriteSystem::GetStableTime(void)
{
    return m_AutoFightData.GetStableTime();
}

bool SpriteSystem::FindExist(vector<LONG> &refVector, LONG lValue)
{
    if (lValue)
    {
        vector<LONG>::iterator ite = refVector.begin();
        for (; ite != refVector.end(); ++ite)
        {
            if ((*ite) == lValue)
            {
                return true;
            }
        }
    }
    return false;
}