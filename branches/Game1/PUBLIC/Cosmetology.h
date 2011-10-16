///////////////////////////////////////////////////////////////////////////////
///  Cosmetology.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\PUBLIC
///
///  CREATED: 02/28/2008 13:15:01 PM by 陈先进
///
///  PURPOSE:   美容系统配置
//////////////////////////////////////////////////////////////////////
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_COSMETOLOGY_INCLUDED
#define _INC_COSMETOLOGY_INCLUDED
#include "tinyxml.h"
#include <map>


using namespace std;
struct tagCosKey 
{
	BYTE bType;	//美容类型1表示脸形2表示发型
	BYTE bSex;	//性别
	long lID;   //序列编号
};

typedef enum CONSMETOLOGYTYPE
{
	CSTTYPE_FACE = 1,		// 脸型
	CSTTYPE_HAIR = 2,		// 发型
	CSTTYPE_UNKNOW = -1,

	CSTTYPE_NUM
}CSTTYPE;

struct tagCosKeycmp
{
	bool operator()(const tagCosKey& left,const tagCosKey& right) const
	{
		if(left.bType<right.bType)
		{
			return true;
		}
		else if (left.bType==right.bType)
		{
			if (left.bSex<right.bSex)
			{
				return true;
			} 
			else if(left.bSex==right.bSex)
			{
				if (left.lID < right.lID)
				{
					return true;
				}				
			}
		}
		return false;
	}
};

class CCosmetology
{
public:
	struct tagCosme 
	{
		BYTE bDisplay;	 //物品显示标志符
		long lPicID;	 //图片ID
		long lIndex;	 //(脸，发)形编号
		long lGoodsIndex;//物品索引
		long lGoodsNum;	 //物品数量		
		long lHairColor; //头发颜色
	};

public:
	CCosmetology();
	~CCosmetology();

public:
	static bool InitCosmetology(string strFileName);
	static bool LoadMainConfig(TiXmlElement *pElem, BYTE bType, BYTE bSex);
	static void AddToByteArray(vector<BYTE> &pByteArray);
	static void DecordFromByteArray(BYTE *pByte, long& pos);
	//获取整型
	static int	GetIntAttribute(TiXmlElement *pElem,string strName);	
	static tagCosme *GetTgCosme(tagCosKey tgKey);
	static tagCosme* GetTgCosme(CSTTYPE,BYTE,long);
	static void	Release();

	static map<tagCosKey,tagCosme*,tagCosKeycmp> m_mpCosmetology;			//美容配置列表

	typedef map<tagCosKey,tagCosme*,tagCosKeycmp> mapCTy;					

	static mapCTy& GetCosTologyList() { return m_mpCosmetology; }
};		

typedef map<tagCosKey,CCosmetology::tagCosme*,tagCosKeycmp>::iterator itCosmetology;
typedef CCosmetology::tagCosme* CTPCosme;
typedef CCosmetology::tagCosme& CTRCosme;	

#endif//_INC_COSMETOLOGY_INCLUDED
