#include "StdAfx.h"
#include "Cosmetology.h"
#include "StrUnifyInput.h"
#include "Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct tagCosKey;
struct tagCosKeycmp;
map<tagCosKey,CCosmetology::tagCosme*,tagCosKeycmp>  CCosmetology::m_mpCosmetology;

//默认构造函数
CCosmetology::CCosmetology()
{

}
//默认析构函数
CCosmetology::~CCosmetology()
{

}
//解析美容系统的配置文件
bool CCosmetology::InitCosmetology(string strFileName)
{
	if (""==strFileName)
	{
		return false;
	}
	Release();
	char strError[256];
	//=================================================================//
	CRFile *prfile = rfOpen(strFileName.c_str());
	if(NULL == prfile)
	{
		//输出错误信息
		_snprintf(strError, 256, "LoadFileError:%s",strFileName);
		MessageBox(NULL,strError,"",MB_OK); 
		return false;
	}
	TiXmlNode *pNode = NULL; 		
	TiXmlDocument m_Tdoc(strFileName.c_str());
	//载入复活物品配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		_snprintf(strError, 256, CStringReading::LoadString(IDS_WS_SKILL,STR_WS_SKILL_FOURTH),strFileName.c_str());
		MessageBox(NULL,strError,"",MB_OK); 
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);
	//=================================================================//
	//找到根节点，如果无节点，输出错误提示
	pNode = m_Tdoc.FirstChild("Cosme");
	if (pNode==NULL)
	{
		string strNode = "Cosme";
		_snprintf(strError, 256, CStringReading::LoadString(IDS_WS_SKILL,STR_WS_SKILL_FIFTH),strFileName.c_str(),strNode.c_str());
		MessageBox(NULL,strError,"",MB_OK); 
		return false;
	}
	TiXmlElement* pElem = pNode->ToElement();


	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{

		if (!stricmp(pChildElem->Value(),"Type"))
		{			
			BYTE bType  = (BYTE)atoi(pChildElem->Attribute("id"));
			BYTE bSex = (BYTE)atoi(pChildElem->Attribute("sex"));
			if (!LoadMainConfig(pChildElem,bType,bSex))
			{
				_snprintf(strError, 256, "the file %s has the same id,please check it",strFileName.c_str());
				MessageBox(NULL,strError,"",MB_OK); 
				return false;
			}
			
		}
	}
	return true;
}

//获取位型
int CCosmetology::GetIntAttribute(TiXmlElement *pElem,string strName)
{
	int iValue;
	if(pElem->QueryIntAttribute(strName.c_str(),&iValue))
	{
		iValue = 0;
	}
	return iValue;
}
//读取配置主节点
bool CCosmetology::LoadMainConfig(TiXmlElement *pElem, BYTE bType, BYTE bSex)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"MainConfig"))
		{
			tagCosKey tgKey;
			tgKey.bType = bType;
			tgKey.bSex = bSex;
			tgKey.lID = atol(pChildElem->Attribute("id"));

			itCosmetology it = m_mpCosmetology.find(tgKey);
			if (it==m_mpCosmetology.end())
			{
				tagCosme *ptgCosme = new tagCosme;
				ptgCosme->lIndex = GetIntAttribute(pChildElem,"index");				//(脸，发)形编号
				ptgCosme->lGoodsIndex = GetIntAttribute(pChildElem,"goodsindex");	//物品索引
				ptgCosme->lGoodsNum = GetIntAttribute(pChildElem,"goodsnum");		//物品数量				
				ptgCosme->lPicID = GetIntAttribute(pChildElem,"pid");				//图片ID
				ptgCosme->bDisplay = (BYTE)(GetIntAttribute(pChildElem,"show"));	//物品显示标志符
				ptgCosme->lHairColor = GetIntAttribute(pChildElem,"haircolor");		//发型颜色
				m_mpCosmetology.insert(make_pair(tgKey,ptgCosme));				
			}
			else
			{
				//配置中有相同的ID,返回失败
				return false;
			}
		}
	}
	return true;
}
CCosmetology::tagCosme *CCosmetology::GetTgCosme(tagCosKey tgKey)
{
	itCosmetology it = m_mpCosmetology.find(tgKey);
	if (it!=m_mpCosmetology.end())
	{
		return (*it).second;
	}
	return NULL;
}

CCosmetology::tagCosme* CCosmetology::GetTgCosme(CSTTYPE e,BYTE sex,long index)
{
	tagCosKey _tgKey;
	_tgKey.bType = (BYTE)e;
	_tgKey.bSex = sex;
	_tgKey.lID = index;
	return GetTgCosme(_tgKey);
}

//编码
void CCosmetology::AddToByteArray(vector<BYTE> &pByteArray)
{
	//个数
	_AddToByteArray(&pByteArray, static_cast<long>(m_mpCosmetology.size()));
	itCosmetology it = m_mpCosmetology.begin();
	for (; it!=m_mpCosmetology.end(); ++it)
	{
		_AddToByteArray(&pByteArray,(*it).first.bType);
		_AddToByteArray(&pByteArray,(*it).first.lID);
		_AddToByteArray(&pByteArray,(*it).first.bSex);
		_AddToByteArray(&pByteArray,(*it).second,sizeof(tagCosme));
	}
}
//解码
void CCosmetology::DecordFromByteArray(BYTE *pByte, long& pos)
{
	Release();
	long lNum = _GetLongFromByteArray(pByte,pos);
	for (long l=0; l<lNum; ++l)
	{
		tagCosKey tgKey;
		tagCosme *ptgCosme = new tagCosme;
		tgKey.bType = _GetByteFromByteArray(pByte,pos);
		tgKey.lID   = _GetLongFromByteArray(pByte,pos);
		tgKey.bSex  = _GetByteFromByteArray(pByte,pos);
		_GetBufferFromByteArray(pByte,pos,ptgCosme,sizeof(tagCosme));
		m_mpCosmetology.insert(make_pair(tgKey,ptgCosme));
	}

}
//释放
void CCosmetology::Release()
{
	itCosmetology it = m_mpCosmetology.begin();
	for (; it!=m_mpCosmetology.end(); ++it)
	{
		SAFE_DELETE((*it).second);
	}
	m_mpCosmetology.clear();
}
