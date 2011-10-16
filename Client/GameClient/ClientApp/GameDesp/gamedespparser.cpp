#include "stdafx.h"
#include "GameDespParser.h"
#include "../Goods/Goods.h"
#include "../../Game.h"
#include "../Skills/SkillIni.h"
#include "../../Public/Common/CStrConverter.h"
#include "../../ui/UIManager.h"
#include "../../GameClient/ClientApp/Other/GlobeSetup.h"
#define STR_MARGIN  "   "
#define GoodsConfigFile "data/GameDescription/GoodsDescription.xml"

char GameDespParser::BaseAtt[5][64] = {"BP_INDEX",                //物品编号
"BP_NAME",                 //物品名字
"BP_ORIGINAL_NAME",        //物品原始名
"BP_PRICE",                //物品价格
"BP_DESCRIBE"              //物品概述
};


GameDespParser::GameDespParser():sourceUI((SOURCE_UI)0)
{
    LoadDespCfg();
}

GameDespParser::~GameDespParser()
{

}

const char *GameDespParser::AppendImage(const char *ImageFileNamePath,long d_width,long d_height)
{
    //name 图片文件名  path 图片路径 imagesetName 图像集名
    //imageSize图像尺寸 imageProperty图像属性
    char name[128],path[128],imagesetName[128],imageSize[128],imageProperty[128];;
    GetFileName(ImageFileNamePath,name);
    GetFilePath(ImageFileNamePath,path);
    sprintf_s<128>(imagesetName,"GoodIcon/%s",name);
    if(!GetInst(UIManager).isImageSetDefined(imagesetName))
    {
        GetInst(UIManager).CreateImageSetFromFile(imagesetName,ImageFileNamePath);
    }
    sprintf_s<128>(imageProperty,"[IMG set:%s image:full_image]",imagesetName);
    if(!d_width && !d_height)
    {
        d_width = GetInst(UIManager).GetImageWidth(imagesetName);
        d_height = GetInst(UIManager).GetImageHeight(imagesetName);
    }
    sprintf_s<128>(imageSize,"[image-size w:%d h:%.0d]", d_width
        , d_height);
    strPARSETEXT += imageSize;
    strPARSETEXT += imageProperty;
    return strPARSETEXT.c_str();
}
const char *GameDespParser::AppendGoodIcon(string &ImageSetProperty)
{
    if (ImageSetProperty.empty())
        return 0;

    char imageSet[128];
    char imageName[128];
    sscanf(ImageSetProperty.c_str(), " set:%127s image:%127s", imageSet, imageName);
    long d_width = GetInst(UIManager).GetImageWidth(imageSet);
    long d_height = GetInst(UIManager).GetImageHeight(imageSet);
    char imageSize[128];
    sprintf_s<128>(imageSize,"[image-size w:%.0f h:%.0f]",d_width,d_height);

    strPARSETEXT += imageSize;
    strPARSETEXT += "[IMG ";
    strPARSETEXT += ImageSetProperty;
    strPARSETEXT += "]";
    return strPARSETEXT.c_str();
}

const char *GameDespParser::AppendText(const char *text)
{
    if(text && strlen(text))
        strPARSETEXT += text;
    return strPARSETEXT.c_str();
}

const char * GameDespParser::ParseGoods(CGoods *pGoods,bool isSelf)
{
    strPARSETEXT = "";
    vector<tagParse>::iterator  iterParser = tagGoodsVector.begin();
    for (;iterParser != tagGoodsVector.end();++iterParser)
    {
        tagParse &parTag = *iterParser;
        //基本属性
        long lindex = pGoods->GetIndex();
        map<string,ulong>::iterator  AdditIter = CGoodsList::GetMapAllEffect().find(parTag.strAtt);
        CGoodsList::tagBaseProperty &tagBasePP = CGoodsList::GetProperty(lindex)->BaseProperty;
        if(parTag.strAtt.compare("BP_NAME") == 0)
        {
            AppendText(parTag.Desp.c_str());
            AppendText(pGoods->GetName());
        }
        else if(parTag.strAtt.compare("BP_PRICE") == 0)
        {
            AppendText(parTag.Desp.c_str());
            char strPrice[32];
            sprintf_s<32>(strPrice,"%u",pGoods->GetPrice());
            AppendText(strPrice);
        }
        else if(parTag.strAtt.compare("BP_DESCRIBE") == 0)
        {
            AppendText(parTag.Desp.c_str());
            long lIndex = pGoods->GetIndex();
            string &strDesp = tagBasePP.strContent;
            AppendText(strDesp.c_str());
        }
        else if (parTag.ImgInfo.compare("GOODS_ICON") == 0)
        {
            char imagePath[128];
            sprintf_s<128>(imagePath,"pictures/goods/%d.jpg",tagBasePP.dwIconId);
            AppendImage(imagePath);
        }
        else if (AdditIter != CGoodsList::GetMapAllEffect().end())
        {
            
            ushort Name = (ushort)(*AdditIter).second;
            long  lValue1 = pGoods->GetEffectVal(Name,1);
            long  lValue2 = pGoods->GetEffectVal(Name,2);
            char  addstr[128] = "";
            if(parTag.val == 0)
                sprintf_s<128>(addstr,"%u - %u",lValue1,lValue2);
            else if(parTag.val == 1 && lValue1)
                sprintf_s<128>(addstr,"%u",lValue1);
            else if(parTag.val == 2 && lValue2)
                sprintf_s<128>(addstr,"%u",lValue2);
            if(strcmp(addstr,"") == 0)
                continue;
            AppendText(parTag.Desp.c_str());
            AppendText(addstr);
        }
    }

    return strPARSETEXT.c_str();
}

const char *GameDespParser::ParseSkill(int skillID, int iskillLv)
{
    //技能概述
    
    //对应等级描述
    tagSkillDoc *pSkillDoc = GetGame()->GetSkillDocConfig()->GetSkillDoc(skillID);
    tagSkillLD *pSkillLv = pSkillDoc->GetLevel(iskillLv);
    string skillLvDesp = pSkillLv->GetDesc();
    AppendText(skillLvDesp.c_str());
    return strPARSETEXT.c_str();
}

// 读取配置文件(data/Description.xml)
bool  GameDespParser::LoadDespCfg()
{
    CRFile *prfile = rfOpen(GoodsConfigFile);
	if(NULL == prfile)
	{
		//输出错误信息
		char strError[128];
		wsprintf(strError,"LoadFileError:%s",GoodsConfigFile);
		MessageBox(NULL,strError,"",MB_OK); 
		return false;
	}
	TiXmlNode *pNode = NULL; 		
	TiXmlDocument doc;
	//载入技能配置文件
	if( !doc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		char strError[128];
		wsprintf(strError,"LoadFileError:%s",GoodsConfigFile);
		MessageBox(NULL,strError,"",MB_OK); 
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);
	//=================================================================//
	TiXmlElement	*m_pElem=NULL;
	pNode = doc.FirstChild("GameDescription");
	if(NULL==pNode)
	{
		return false;
	}
	m_pElem = pNode->ToElement();
    tagGoodsVector.clear();
	for(TiXmlElement *pChildElem = m_pElem->FirstChildElement(); pChildElem!=NULL; pChildElem = pChildElem->NextSiblingElement())
	{
		// 物品描述节点
		if(!strcmp(pChildElem->Value(),"GoodsDesp"))
		{
            TiXmlElement *pElem = pChildElem->FirstChildElement();
            for(; pElem!=NULL; pElem = pElem->NextSiblingElement())
            {
                if(!strcmp(pElem->Value(),"Property"))
                {
                    tagParse parTag;
                    if(NULL != pElem->Attribute("Desp"))
                        parTag.Desp = pElem->Attribute("Desp");
                    if(NULL != pElem->Attribute("EmuID"))
                        parTag.strAtt = pElem->Attribute("EmuID");
                    if(NULL != pElem->Attribute("Val"))
                        parTag.val = CStrConverter::parseInt(pElem->Attribute("Val"));
                    if (NULL != pElem->Attribute("Img"))
                        parTag.ImgInfo = pElem->Attribute("Img");
                    tagGoodsVector.push_back(parTag);
                }
            }
        }
    }
    return true;
}