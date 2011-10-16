#include "stdafx.h"
#include "LotteryXml.h"
#include "rfile.h"
#include "clientresource.h"

CLotteryXml::CLotteryXml()
{
	m_VecLotteryField.clear();
	m_MapLotteryGoods.clear();
	m_MapCoronaType.clear();

	m_iLotteryNumMin = 0;
	m_iResetNumLimit = 0;
	m_lConstyProbaToLevel = 0;
	m_lConstyProbaToField = 0;  
    m_lShinningInterval = 0;

	m_lCredit = 0;
	m_lPersonalCreditMax = 0;
	m_lWorldCreditMax = 0;
	m_dwTokenIndex = 0;
	m_iTokenCost = 0;
	m_iRefreshGoodsCost = 0;
	m_iRefreshConstCost = 0;
	m_dwTimeInterVal = 0;
	m_dwWorldCreditRefreshTime = 0;
	m_bLotteryShowSwitch = false;
}

CLotteryXml::~CLotteryXml()
{
    Reset();
}

bool CLotteryXml::LoadLotteryXml()
{
    bool bLoadSuccess = false;
    Reset();
    //读取配置文件
    CRFile *prfile = rfOpen("data\\Lottery.xml");
    TiXmlDocument doc;
    bool bLoadXMLSuccess = (NULL != prfile) && (doc.LoadData(prfile->GetData(),prfile->GetDatalen()));
    rfClose(prfile);
    if (bLoadXMLSuccess)
    {
        TiXmlNode *pNode = doc.FirstChild("Lottery"); 		
        //读取抽奖配置
        if(NULL != pNode)
        {
            TiXmlElement* pElement = NULL;
            pElement = pNode->ToElement();
            if ( NULL != pElement)
            {
                TiXmlElement* pElem = pElement->FirstChildElement("LotteryField");
                // 读取节点
                bLoadSuccess = (NULL != pElem) && 
                    LoadLotteryField(pElem) && 
                    LoadLotteryGoods(pElement) && 
                    LoadCoronaType(pElement) && 
                    LoadConstySetup(pElement) &&
                    LoadLotterySetup(pElement);
            }
        }
        if (!bLoadSuccess)
        {
            OutputErrorMsg("Lottery.xml:Frame");
        }
    }
    else
    {
        OutputErrorMsg("Lottery.xml");
    }

    return bLoadSuccess;
}

bool CLotteryXml::LoadLotteryField(TiXmlElement* pElem)
{
	for(TiXmlElement *pChildElem = pElem->FirstChildElement(); pChildElem!=NULL; pChildElem = pChildElem->NextSiblingElement())
	{	    
			if(strcmp(pChildElem->Value(),"Buff")==0)
			{
				tagLotteryField LotteryField;
				LotteryField.iFieldIndex = atoi(pChildElem->Attribute("Field"));
				LotteryField.iFieldLevel = atoi(pChildElem->Attribute("FieldLevel"));
				LotteryField.lProbability = (long)atoi(pChildElem->Attribute("Probability"));
	
				m_VecLotteryField.push_back(LotteryField);
		    }
	}
	//判断配置是否读取正确
	if(m_VecLotteryField.size()>0)
		return true;
	return false;
}

bool CLotteryXml::LoadLotteryGoods(TiXmlElement *pElem)
{
	TiXmlElement* pElemNode=pElem->FirstChildElement("LotteryGoods");
	if(pElemNode==NULL)
		return false;
	m_iLotteryNumMin = atoi(pElemNode->Attribute("LotteryNumMin"));
	m_iLotteryNumLimit = atoi(pElemNode->Attribute("LotteryNumLimit"));
	m_iResetNumLimit = atoi(pElemNode->Attribute("ReSetNumLimuni"));
	for(TiXmlElement *pChild = pElemNode->FirstChildElement(); pChild!=NULL; pChild = pChild->NextSiblingElement())
	{
		int iLevel = atoi(pChild->Attribute("Level"));
		for(TiXmlElement *pChildElem = pChild->FirstChildElement(); pChildElem!=NULL; pChildElem = pChildElem->NextSiblingElement())
		{	    
				if(strcmp(pChildElem->Value(),"Buff")==0)
				{
					tagLotteryGoods LotteryGoods;
					LotteryGoods.iGoodsIndex = atoi(pChildElem->Attribute("GoodsIndex"));
					LotteryGoods.iGoodsNum = atoi(pChildElem->Attribute("GoodsNum"));
					LotteryGoods.lProbability = (long)atoi(pChildElem->Attribute("Probability"));
					m_MapLotteryGoods[iLevel].push_back(LotteryGoods);
				}
		}
	}
	return true;
}

bool CLotteryXml::LoadCoronaType(TiXmlElement *pElem)
{
	TiXmlElement* pElemNode=pElem->FirstChildElement("LotteryType");
	if(pElemNode==NULL)
		return false;
	for(TiXmlElement *pChild = pElemNode->FirstChildElement(); pChild!=NULL; pChild = pChild->NextSiblingElement())
	{
		int iLevel = atoi(pChild->Attribute("Level"));
		long lProbabilityValue = atoi(pChild->Attribute("ProbabilityMax"));
		int iFieldLevelMax = atoi(pChild->Attribute("FieldLevelMax"));
		for(TiXmlElement *pChildElem = pChild->FirstChildElement(); pChildElem!=NULL; pChildElem = pChildElem->NextSiblingElement())
		{	    
				if(strcmp(pChildElem->Value(),"Buff")==0)
				{
					tagCoronaType CoronaType;
					CoronaType.FieldLevel = atoi(pChildElem->Attribute("FieldLevel"));
					if(CoronaType.FieldLevel!=iFieldLevelMax)
					   CoronaType.lProbability = (long)atoi(pChildElem->Attribute("Probability"));
					else{
					   CoronaType.lProbability = lProbabilityValue;
					}
					lProbabilityValue = lProbabilityValue-CoronaType.lProbability;
					m_MapCoronaType[iLevel].push_back(CoronaType);
				}
		}
	}
	return true;
}

DWORD CLotteryXml::RandGoodsFromLotteryGoods(int iLevel)
{
	vector<tagProbability> VecProbability;
	vector<tagLotteryGoods>::iterator it = m_MapLotteryGoods[iLevel].begin();
	tagProbability tagProbabilityTemp;
	long lProbabilityValue = 0;   //该值用于计算概率分布空间
	//将待定值分布于概率区间上
	for(;it!=m_MapLotteryGoods[iLevel].end();++it)
	{
		tagProbabilityTemp.Index = (*it).iGoodsIndex;
		tagProbabilityTemp.ProbabilityMin = lProbabilityValue;
		tagProbabilityTemp.ProbabilityMax = tagProbabilityTemp.ProbabilityMin + (*it).lProbability;
		VecProbability.push_back(tagProbabilityTemp);
		lProbabilityValue=lProbabilityValue+(*it).lProbability;
	}

	long lProbabilityRe = random(100); 

	for(int i=0;i<VecProbability.size();i++)
	{
		if(lProbabilityRe>=VecProbability[i].ProbabilityMin&&lProbabilityRe<VecProbability[i].ProbabilityMax)
		{
			return VecProbability[i].Index;
		}
	}
	return 0;
}

int CLotteryXml::GetFieldLevel(int FieldID)
{
	for(int i=0;i<m_VecLotteryField.size();i++)
	{
		if(m_VecLotteryField[i].iFieldIndex==FieldID)
		{
			return m_VecLotteryField[i].iFieldLevel;
		}
	}
	return 0;
}


int CLotteryXml::RandFieldLevel(int iLevel,int iLevelConsty)
{
	vector<tagProbability> VecProbability;
	vector<tagCoronaType>::iterator it = m_MapCoronaType[iLevel].begin();
	tagProbability tagProbabilityTemp;
	long lProbabilityValue = 0;   //该值用于计算概率分布空间
	//将待定值分布于概率区间上
	for(;it!=m_MapCoronaType[iLevel].end();++it)
	{
		tagProbabilityTemp.Index = (*it).FieldLevel;
		tagProbabilityTemp.ProbabilityMin = lProbabilityValue;
		if(iLevelConsty!=-1&&iLevelConsty==tagProbabilityTemp.Index)
		{
			tagProbabilityTemp.ProbabilityMax = tagProbabilityTemp.ProbabilityMin + (((*it).lProbability*m_lConstyProbaToLevel)/100);
			lProbabilityValue=lProbabilityValue+(((*it).lProbability*m_lConstyProbaToLevel)/100);
		}else{		    
			tagProbabilityTemp.ProbabilityMax = tagProbabilityTemp.ProbabilityMin + (*it).lProbability;
			lProbabilityValue=lProbabilityValue+(*it).lProbability;
		}
		VecProbability.push_back(tagProbabilityTemp);
	}

	long lProbabilityRe = random(10000); 

	for(int i=0;i<VecProbability.size();i++)
	{
		if(lProbabilityRe>=VecProbability[i].ProbabilityMin&&lProbabilityRe<VecProbability[i].ProbabilityMax)
		{
			return VecProbability[i].Index;
		}
	}
	return 0;
}

bool CLotteryXml::GetFieldByLevel(int iLevel,vector<tagLotteryField>& VecFieldByLevel)
{
	//vector<tagLotteryField> VecFieldByLevel;
	for(int i=0;i<m_VecLotteryField.size();i++)
	{
		if(m_VecLotteryField[i].iFieldLevel==iLevel)
		{
			tagLotteryField tagLotteryField;
			tagLotteryField.iFieldIndex = m_VecLotteryField[i].iFieldIndex;
			tagLotteryField.iFieldLevel = m_VecLotteryField[i].iFieldLevel;
			tagLotteryField.lProbability = m_VecLotteryField[i].lProbability;
			VecFieldByLevel.push_back(m_VecLotteryField[i]);
		}
	}
	return true;
}

bool CLotteryXml::LoadConstySetup(TiXmlElement* pElem)
{
	TiXmlElement* pElemNode=pElem->FirstChildElement("ConstySetup");
	if(pElemNode==NULL)
		return false;
	TiXmlElement *pChildElem = pElemNode->FirstChildElement();
	if(pChildElem==NULL)
		return false;
	m_lConstyProbaToLevel = (long)atoi(pChildElem->Attribute("lConstyProbaToLevel"));
	m_lConstyProbaToField = (long)atoi(pChildElem->Attribute("lConstyProbaToField"));
    m_lShinningInterval = (long)atoi(pChildElem->Attribute("lShinningInterval"));
	return true;
}

bool CLotteryXml::LoadLotterySetup(TiXmlElement* pElem)
{
	TiXmlElement* pElemNode=pElem->FirstChildElement("LotterySetup");
	if(pElemNode==NULL)
		return false;
	TiXmlElement *pChildElem = pElemNode->FirstChildElement();
	if(pChildElem==NULL)
		return false;
	m_lPersonalCreditMax = (long)atoi(pChildElem->Attribute("PersonalCreditMax"));
	m_lWorldCreditMax = (long)atoi(pChildElem->Attribute("WorldCreditMax"));
	m_lCredit  = (long)atoi(pChildElem->Attribute("Credit"));
	m_bLotteryShowSwitch = atoi(pChildElem->Attribute("LotteryShowSwitch")) != 0;
	TiXmlElement *pChild = pChildElem->FirstChildElement();
	for (;pChild!=NULL;pChild = pChild->NextSiblingElement())
	{
		if (!strcmp(pChild->Value(),"Token"))
		{
			 m_dwTokenIndex = (DWORD)atoi(pChild->Attribute("TokenIndex"));
			m_iTokenCost = atoi(pChild->Attribute("TokenCost"));
			m_iTokenCost_1 = atoi(pChild->Attribute("TokenCost_1"));
			m_iRefreshGoodsCost = atoi(pChild->Attribute("RefreshGoodsCost"));
			m_iRefreshConstCost = atoi(pChild->Attribute("RefreshConstCost"));
		}
		else if (!strcmp(pChild->Value(),"Time"))
		{
			m_dwTimeInterVal = (DWORD)atoi(pChild->Attribute("TimeInterVal"))*1000;
			m_dwWorldCreditRefreshTime = (DWORD)atoi(pChild->Attribute("WorldCreditRefreshTime"))*1000;
		}
		else if (!strcmp(pChild->Value(),"Effect"))
		{
			m_dwOneItemTime = (DWORD)atoi(pChild->Attribute("OneItemTime"));
			m_dwRouletteCircles = (DWORD)atoi(pChild->Attribute("RouletteCircles"));
			m_dwAddPowerCircles = (DWORD)atoi(pChild->Attribute("AddPowerCircles"));
			m_dwResetPrizesTime = (DWORD)atoi(pChild->Attribute("ResetPrizesTime"));
			m_dwResetConstellationsTime = (DWORD)atoi(pChild->Attribute("ResetConstellationsTime"));
			m_dwBlindTime = (DWORD)atoi(pChild->Attribute("BlindTime"));
		}
	}
	return true;
}

bool CLotteryXml::CodeToByteArray(DBWriteSet& dbWriteSet)
{
	//栏位配置
	dbWriteSet.AddToByteArray((DWORD)m_VecLotteryField.size());
	vector<tagLotteryField>::iterator it=m_VecLotteryField.begin();
	int i=0;
	for (;it!=m_VecLotteryField.end();++it)
	{
		//dbWriteSet.AddToByteArray(&(m_tagQuestIndexForServer[i]),sizeof(tagQuestIndexForServer));
		dbWriteSet.AddToByteArray(&(*it),sizeof(tagLotteryField));
		i++;
	}
	//档次物品配置
	dbWriteSet.AddToByteArray((DWORD)m_MapLotteryGoods.size());
	map<int,vector<tagLotteryGoods>>::iterator itLotteryGoods=m_MapLotteryGoods.begin();
	for (;itLotteryGoods!=m_MapLotteryGoods.end();++itLotteryGoods)
	{
		//dbWriteSet.AddToByteArray(&(m_tagQuestIndexForServer[i]),sizeof(tagQuestIndexForServer));
		dbWriteSet.AddToByteArray((DWORD)(*itLotteryGoods).first);
		dbWriteSet.AddToByteArray((DWORD)(*itLotteryGoods).second.size());
		vector<tagLotteryGoods>::iterator itor=(*itLotteryGoods).second.begin();
		for(;itor!=(*itLotteryGoods).second.end();++itor)
		{
		    dbWriteSet.AddToByteArray(&(*itor),sizeof(tagLotteryGoods));
		}
		i++;
	}
	//轮盘类型配置
	dbWriteSet.AddToByteArray((DWORD)m_MapCoronaType.size());
	map<int,vector<tagCoronaType>>::iterator itCoronaType=m_MapCoronaType.begin();
	for (;itCoronaType!=m_MapCoronaType.end();++itCoronaType)
	{
		//dbWriteSet.AddToByteArray(&(m_tagQuestIndexForServer[i]),sizeof(tagQuestIndexForServer));
		dbWriteSet.AddToByteArray((DWORD)(*itCoronaType).first);
		dbWriteSet.AddToByteArray((DWORD)(*itCoronaType).second.size());
		vector<tagCoronaType>::iterator itor=(*itCoronaType).second.begin();
		for(;itor!=(*itCoronaType).second.end();++itor)
		{
		    dbWriteSet.AddToByteArray(&(*itor),sizeof(tagCoronaType));
		}
		i++;
	}
	//剩余奖品限制参数
	dbWriteSet.AddToByteArray((DWORD)m_iLotteryNumLimit);
	//剩余奖品最小数量限制
	dbWriteSet.AddToByteArray((DWORD)m_iLotteryNumMin);
	//星座影响档次的概率
	dbWriteSet.AddToByteArray((DWORD)m_lConstyProbaToLevel);
	//星座影响的栏位的概率
	dbWriteSet.AddToByteArray((DWORD)m_lConstyProbaToField);

	//单次抽奖所获得的个人积分
	dbWriteSet.AddToByteArray((DWORD)m_lCredit);
	//个人积分上限
	dbWriteSet.AddToByteArray((DWORD)m_lPersonalCreditMax);
	//世界积分上限
	dbWriteSet.AddToByteArray((DWORD)m_lWorldCreditMax);
	//代币索引
	dbWriteSet.AddToByteArray((DWORD)m_dwTokenIndex);
	//抽奖所需要的代币数量
	dbWriteSet.AddToByteArray((DWORD)m_iTokenCost);
	//小于剩余奖品限制参数时所需的代币数量
	dbWriteSet.AddToByteArray((DWORD)m_iTokenCost_1);
	//重置物品所需要的代币数量
	dbWriteSet.AddToByteArray((DWORD)m_iRefreshGoodsCost);
	//重置星座所需要的代币数量
	dbWriteSet.AddToByteArray((DWORD)m_iRefreshConstCost);
	//抽奖时间间隔
	dbWriteSet.AddToByteArray((DWORD)m_dwTimeInterVal);
	//世界积分刷新的时间间隔
	dbWriteSet.AddToByteArray((DWORD)m_dwWorldCreditRefreshTime);
	//重置限制参数
	dbWriteSet.AddToByteArray((DWORD)m_iResetNumLimit);
	return true;
}

bool CLotteryXml::DecodeToByteArray(DBReadSet& dbReadSet)
{
	m_VecLotteryField.clear();
	m_MapLotteryGoods.clear();
	m_MapCoronaType.clear();
	//栏位配置
	DWORD dwsize=dbReadSet.GetDwordFromByteArray();	
	tagLotteryField tagLotteryField;
	for (int i=0;i<dwsize;++i)
	{
		dbReadSet.GetBufferFromByteArray(&tagLotteryField,sizeof(tagLotteryField));
		m_VecLotteryField.push_back(tagLotteryField);
	}
	//档次物品配置
	dwsize=dbReadSet.GetDwordFromByteArray();
	for(int i=0;i<dwsize;i++)
	{
		int index = dbReadSet.GetDwordFromByteArray();
		tagLotteryGoods tagLotteryGoods;
    	DWORD dwsizeToLotteryGoods=dbReadSet.GetDwordFromByteArray();	
		for(int j=0;j<dwsizeToLotteryGoods;j++)
		{
			dbReadSet.GetBufferFromByteArray(&tagLotteryGoods,sizeof(tagLotteryGoods));
			m_MapLotteryGoods[index].push_back(tagLotteryGoods);
		}
	}
	//轮盘类型配置
	dwsize=dbReadSet.GetDwordFromByteArray();
	for(int i=0;i<dwsize;i++)
	{
		int index = dbReadSet.GetDwordFromByteArray();
		tagCoronaType tagCoronaType;
    	DWORD dwsizeToCoronaType=dbReadSet.GetDwordFromByteArray();	
		for(int j=0;j<dwsizeToCoronaType;j++)
		{
			dbReadSet.GetBufferFromByteArray(&tagCoronaType,sizeof(tagCoronaType));
			m_MapCoronaType[index].push_back(tagCoronaType);
		}
	}
	//剩余奖品限制参数
	m_iLotteryNumLimit = dbReadSet.GetDwordFromByteArray();
	//剩余奖品最小数量限制
    m_iLotteryNumMin = dbReadSet.GetDwordFromByteArray();
	//星座影响档次的概率
    m_lConstyProbaToLevel = dbReadSet.GetDwordFromByteArray();
	//星座影响的栏位的概率
	m_lConstyProbaToField = dbReadSet.GetDwordFromByteArray();

	//单次抽奖所获得的个人积分
	m_lCredit = dbReadSet.GetDwordFromByteArray();
	//个人积分上限
	m_lPersonalCreditMax = dbReadSet.GetDwordFromByteArray();
	//世界积分上限
	m_lWorldCreditMax = dbReadSet.GetDwordFromByteArray();
	//代币索引
	m_dwTokenIndex = dbReadSet.GetDwordFromByteArray();
	//抽奖所需要的代币数量
	m_iTokenCost = dbReadSet.GetDwordFromByteArray();
	//小于剩余奖品限制参数时所需的代币数量
	m_iTokenCost_1 = dbReadSet.GetDwordFromByteArray();
	//重置物品所需要的代币数量
	m_iRefreshGoodsCost = dbReadSet.GetDwordFromByteArray();
	//重置星座所需要的代币数量
	m_iRefreshConstCost = dbReadSet.GetDwordFromByteArray();
	//抽奖时间间隔
	m_dwTimeInterVal = dbReadSet.GetDwordFromByteArray();
	//世界积分刷新的时间间隔
	m_dwWorldCreditRefreshTime = dbReadSet.GetDwordFromByteArray();
	//重置参数限制
	m_iResetNumLimit = dbReadSet.GetDwordFromByteArray();
	return true;
}
 //获得某个栏位对应物品的数量
int CLotteryXml::GetLotteryGoodsNum(int iLevel,DWORD dwIndex)
{
	vector<tagLotteryGoods>::iterator it = m_MapLotteryGoods[iLevel].begin();
	for(;it!=m_MapLotteryGoods[iLevel].end();++it)
	{
		if((*it).iGoodsIndex==dwIndex)
		{
			return (*it).iGoodsNum;
		}
	}
	return 0;
}

void CLotteryXml::OutputErrorMsg(char* pMsg)
{
    char strError[128];
    wsprintf(strError,"LoadFileError:%s",pMsg);
    MessageBox(NULL,strError,"",MB_OK); 
}

void CLotteryXml::Reset()
{
    m_VecLotteryField.clear();
    m_MapLotteryGoods.clear();
    m_MapCoronaType.clear();
}
