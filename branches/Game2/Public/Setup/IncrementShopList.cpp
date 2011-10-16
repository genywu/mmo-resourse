// 增值商店，只允许购买增值道具，不允许卖，不允许修理，而且无税收。
// bugs
#include "StdAfx.h"
#include "IncrementShoplist.h"
#include "GoodsSetup.h"
#include "../Common/Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CIncrementShopList::MAP_TOP10 CIncrementShopList::m_mTop10List;
CIncrementShopList::MAP_NEWGOODS CIncrementShopList::m_mNewGoodsList;
CIncrementShopList::MAP_GUIDEGOODS CIncrementShopList::m_mGuideGoodsList;
CIncrementShopList::MAP_FILTERGOODS CIncrementShopList::m_mFilterListOfWShop;
CIncrementShopList::MAP_FILTERGOODS CIncrementShopList::m_mFilterListOfXShop;
CIncrementShopList::MAP_FILTERGOODS CIncrementShopList::m_mFilterListOfTShop;
CIncrementShopList::MAP_INCGOODS CIncrementShopList::m_mIncGoodsList;
CIncrementShopList::MAP_INCCLASSGOODS CIncrementShopList::m_mIncClassGoodsList;

//string CIncrementShopList::m_sContactGM;
string CIncrementShopList::m_sAffiche;
//bool CIncrementShopList::m_bReload;


CIncrementShopList::CIncrementShopList(void)
{
}

CIncrementShopList::~CIncrementShopList(void)
{
	ClearIncShop();
	//m_sContactGM.clear();
}

// 从文件读取列表
bool CIncrementShopList::LoadIncrementShopList(const char* filename)
{
	if( !filename ) return false;

	ClearIncShop();

	char szError[128] = "";
	TiXmlNode *pNode = NULL;
	TiXmlDocument xmldoc;
	// 输出错误信息：错误的文件名
	CRFile* prFile = rfOpen(filename);
	if (prFile == NULL)
	{
		return FALSE;
	}
	if( !xmldoc.LoadData(prFile->GetData(),prFile->GetDatalen()) )
	{
        Log4c::Error(ROOT_MODULE,"%-15s 载入配置文件%s失败,请检查",__FUNCTION__, filename);
		return false;
	}
	rfClose(prFile);
	// 找到根节点，如果无节点，输出错误提示
	if ( !( pNode = xmldoc.FirstChild("IncShopList")) )
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"没发现IncShopList节点,请检查");
		return false;
	}

	// 用到的字段
	string	strAffiche;						//公告信息
	string	strOrignalName;					//物品原始名
	string	strTOrignalName;				//点券原始名
	DWORD	dwGoodsType;					//商品类型（杂货&药品&材料...）
	DWORD	dwShopType;						//商店类型，0 星钻商店 1  位面精华商店 2 点券商店
	DWORD	dwPrice;						//价格（星钻价格&位面价格 &点券价格）
	DWORD	dwDiscount;						//打折信息
	string	strTimeLimit;					//限时信息
	long	lQuantityLimit;					//限量信息
	long	lBuyLimit;						//限购信息
	string	strFilterType;					//筛选类型
	DWORD	dwGoodsIndex;					//商品物编ID
	DWORD	dwGoodsNum;						//物品数量

	double	dVal;

	// 类型转换
	TiXmlElement* pElem = pNode->ToElement();
	TiXmlElement *pChildElem=pElem->FirstChildElement();
	for ( ; pChildElem; pChildElem=pChildElem->NextSiblingElement() )
	{
		if( !strcmp(pChildElem->Value(), "ShopType" )
			&& TIXML_NO_ATTRIBUTE!=pChildElem->QueryDoubleAttribute("TypeID", &dVal) )
		{
			dwShopType = (DWORD)dVal;
			MAP_INCSHOP mapIncGoodsList;
			MAP_CLASSGOODS mapIncClassGoodsList;
			TiXmlElement* pGrandElem = pChildElem->FirstChildElement();
			for ( DWORD dwNo=0; pGrandElem; pGrandElem=pGrandElem->NextSiblingElement(), ++dwNo )
			{
				//解析新品推荐列表
				if ( !strcmp(pGrandElem->Value(), "NewGoods" ) )
				{
					VEC_GOODSIDX vecNewGoodIndex;
					ParseGoodsList( pGrandElem, vecNewGoodIndex );
					m_mNewGoodsList[dwShopType] = vecNewGoodIndex;
				}
				//解析热销商品列表
				else if( !strcmp(pGrandElem->Value(), "Top10Goods" ) )
				{
					VEC_GOODSIDX vecTop10GoodIndex;
					ParseGoodsList( pGrandElem, vecTop10GoodIndex );
					m_mTop10List[dwShopType] = vecTop10GoodIndex;
				}
				//解析导购标签
				else if ( !strcmp(pGrandElem->Value(), "ShopGuideTags" ) )
				{
					TiXmlElement* pShopGuideElem = pGrandElem->FirstChildElement();
					MAP_TYPEGOODS mapGuideGoods;
					for ( ; pShopGuideElem; pShopGuideElem=pShopGuideElem->NextSiblingElement() )
					{
						if( !strcmp(pShopGuideElem->Value(), "GuideTag" ) )
						{
							string strGuideType = pShopGuideElem->Attribute( "TypeID" );
							VEC_GOODSIDX vecGuideGoodIndex;
							ParseGoodsList( pShopGuideElem, vecGuideGoodIndex );
							mapGuideGoods[strGuideType] = vecGuideGoodIndex;
						}
					}
					m_mGuideGoodsList[dwShopType] = mapGuideGoods;
				}
				//解析商品分类标签
				else if ( !strcmp(pGrandElem->Value(), "GoodsType" ) 
					&& TIXML_NO_ATTRIBUTE!=pGrandElem->QueryDoubleAttribute("TypeID", &dVal) )
				{
					dwGoodsType = (DWORD)dVal;
					VEC_GOODSIDX vecGoodsIdx;
					MAP_TYPEGOODS mapFilterTypeList;
					TiXmlElement* pGoodTypeElem = pGrandElem->FirstChildElement();
					for ( ; pGoodTypeElem; pGoodTypeElem=pGoodTypeElem->NextSiblingElement() )
					{
						if ( !strcmp( pGoodTypeElem->Value(), "FilterTypeTag" ) )
						{
							string strFilterType = pGoodTypeElem->Attribute( "TypeID" );
							VEC_GOODSIDX vecFilterGoodIndex;
							ParseGoodsList( pGoodTypeElem, vecFilterGoodIndex );
							mapFilterTypeList[strFilterType] = vecFilterGoodIndex;
						}
						else if ( !strcmp( pGoodTypeElem->Value(), "Goods" ) )
						{
							//商品原始名
							strOrignalName	= pGoodTypeElem->Attribute("OrignalName");
							dwGoodsIndex	= GoodsSetup::QueryGoodsIDByOriginalName(strOrignalName.c_str());
							if( !dwGoodsIndex )
							{
                               Log4c::Warn(ROOT_MODULE,FormatText("WS_INCSHOP_40", strOrignalName.c_str()));
								continue;
							}

							//价格<以不同的商店分类为单位>
							pGoodTypeElem->Attribute("Price", &dVal);
							dwPrice	= (DWORD)dVal;

							//商品数量
							pGoodTypeElem->Attribute("GoodsNum",&dVal);
							if (dVal==0)
							{
								dwGoodsNum = 1;
							}
							else
							{
								dwGoodsNum = (DWORD)dVal;
							}
							//打折信息
							pGoodTypeElem->Attribute( "discount", &dVal );
							dwDiscount = (DWORD)dVal;
							//限时配置
							strTimeLimit = pGoodTypeElem->Attribute( "timelimit" );
							//限量配置
							int ival;
							pGoodTypeElem->Attribute( "quantitylimit", &ival );
							lQuantityLimit = ival;
							//限购配置
							pGoodTypeElem->Attribute( "buylimit", &ival );
							lBuyLimit = ival;

							IncrementShopElem	elemIncShop( dwGoodsType,dwGoodsIndex, dwPrice,dwGoodsNum,dwDiscount, 
								strTimeLimit,lQuantityLimit,lBuyLimit);

							mapIncGoodsList[dwGoodsIndex] = elemIncShop;
							vecGoodsIdx.push_back( dwGoodsIndex );
						}
					}
					if ( dwShopType == TM_YUANBAO )
					{
						m_mFilterListOfXShop[dwGoodsType] = mapFilterTypeList;
					}
					else if ( dwShopType == TM_WEIMIAN )
					{
						m_mFilterListOfWShop[dwGoodsType] = mapFilterTypeList;
					}
					else if ( dwShopType == TM_TICKET )
					{
						m_mFilterListOfTShop[dwGoodsType] = mapFilterTypeList;
					}
					mapIncClassGoodsList[dwGoodsType] = vecGoodsIdx;
				}
			}
			m_mIncGoodsList[dwShopType] = mapIncGoodsList;
			m_mIncClassGoodsList[dwShopType] = mapIncClassGoodsList;
		}
		//解析公告信息
		else if( !strcmp(pChildElem->Value(), "Affiche" ) )
		{
			TiXmlElement* pGrandElem = pChildElem->FirstChildElement();
			for ( ; pGrandElem; pGrandElem=pGrandElem->NextSiblingElement() )
			{
				if( !strcmp(pGrandElem->Value(), "line" ) )
				{
					strAffiche	= pGrandElem->Attribute("String");
					m_sAffiche += strAffiche + '\n';
				}
			}
		}
	}
	return true;
}

// 添加到CByteArray
bool CIncrementShopList::AddToByteArray(vector<uchar>* pByteArray)
{
	if (!pByteArray)
		return false;

	//编码热销商品列表
	CodeShopGoodsList( pByteArray, M_TOP10 );

	//编码新品推荐列表
	CodeShopGoodsList( pByteArray, M_NEWGOODS );

	//编码导购商品列表
	CodeTypeGoodsList( pByteArray,  M_GUIDETYPE );

	//编码星钻商店筛选商品列表
	CodeTypeGoodsList( pByteArray, M_FILTER_XINGZUAN );

	//编码位面商店筛选商品列表
	CodeTypeGoodsList( pByteArray, M_FILTER_WEIMIAN );

	//编码点券商店筛选商品列表
	CodeTypeGoodsList( pByteArray, M_FILTER_TICKET );

	//编码商店详细商品列表
	CodeDetailedGoodsList( pByteArray );

	//编码商品分类映射表
	CodeIncClassGoodsList( pByteArray );

	char buf[8192] = "";
	strcpy_s(buf, 8192, m_sAffiche.data());
	long lNum = (long)m_sAffiche.size();
	_AddToByteArray(pByteArray, lNum);
	_AddToByteArray(pByteArray,buf, lNum);

	return true;
}

//解码
bool CIncrementShopList::DecordFromByteArray(uchar* pByte, long& pos)
{
	if (!pByte)
		return false;

	//清除容器
	ClearIncShop();

	//解码热销商品列表
	DecordShopGoodsList( pByte, pos, M_TOP10 );

	//解码新品推荐列表
	DecordShopGoodsList( pByte, pos, M_NEWGOODS );

	//解码导购商品列表
	DecordTypeGoodsList( pByte, pos, M_GUIDETYPE );

	//解码星钻商店筛选商品列表
	DecordTypeGoodsList( pByte, pos, M_FILTER_XINGZUAN );

	//解码位面商店筛选商品列表
	DecordTypeGoodsList( pByte, pos, M_FILTER_WEIMIAN );

	//解码点券商店筛选商品列表
	DecordTypeGoodsList( pByte, pos, M_FILTER_TICKET );

	//解码商店详细商品列表
	DecordDetailedGoodsList( pByte, pos );

	//解码商品分类映射表
	DecordIncClassGoodsList( pByte, pos );

	//解码公告信息
	char buf[8192] = "";
	long lNum = _GetLongFromByteArray(pByte, pos);
	_GetBufferFromByteArray(pByte, pos, buf, lNum);
	m_sAffiche = buf;

	return true;
}

//解析物品列表
void CIncrementShopList::ParseGoodsList(TiXmlElement* pTiXmlElem, VEC_GOODSIDX& vecGoodsIndex )
{
	string	strOrignalName;
	DWORD	dwGoodsIndex;
	TiXmlElement* pGoodsElem = pTiXmlElem->FirstChildElement();
	for ( ; pGoodsElem; pGoodsElem=pGoodsElem->NextSiblingElement() )
	{
		if ( !strcmp( pGoodsElem->Value(), "Goods" ) )
		{
			strOrignalName = pGoodsElem->Attribute( "OrignalName" );
			dwGoodsIndex	= GoodsSetup::QueryGoodsIDByOriginalName(strOrignalName.c_str());
			if( !dwGoodsIndex )
			{
               Log4c::Warn(ROOT_MODULE,FormatText("WS_INCSHOP_41", strOrignalName.c_str()));
				continue;
			}
			vecGoodsIndex.push_back(dwGoodsIndex);
		}
	}
}


//编码热销&新品推荐列表
void CIncrementShopList::CodeShopGoodsList( vector<uchar>* pByteArray, eCodeMode eCodeMode )
{
	if ( !pByteArray ) return;
	if ( eCodeMode == M_TOP10 )
	{
		CodeShopGoodsListToByteArray( pByteArray, m_mTop10List );
	}
	else if ( eCodeMode == M_NEWGOODS )
	{
		CodeShopGoodsListToByteArray( pByteArray, m_mNewGoodsList );
	}
}

void CIncrementShopList::CodeShopGoodsListToByteArray( vector<uchar>* pByteArray,MAP_TOP10&  mapGoodsList )
{
	if ( !pByteArray ) return;

	long lNum = (long)mapGoodsList.size();
	_AddToByteArray( pByteArray, lNum );
	ITR_TOP10 itTop10 = mapGoodsList.begin();
	for ( ; itTop10 != mapGoodsList.end(); ++itTop10 )
	{
		DWORD dwShopType = itTop10->first;
		_AddToByteArray( pByteArray, dwShopType );
		VEC_GOODSIDX& vecTop10GoodsList = itTop10->second;
		lNum = (long)vecTop10GoodsList.size();
		_AddToByteArray( pByteArray, lNum );
		ITR_GOODSIDX it = vecTop10GoodsList.begin(); 
		for ( ; it != vecTop10GoodsList.end(); ++it )
		{
			_AddToByteArray( pByteArray, *it );
		}
	}
}

//编码导购&筛选列表
void CIncrementShopList::CodeTypeGoodsList( vector<uchar>* pByteArray, eCodeMode eCodeMode )
{
	if ( !pByteArray ) return;
	
	switch( eCodeMode )
	{
	case M_GUIDETYPE:
		CodeTypeGoodsListToByteArray( pByteArray, m_mGuideGoodsList );
		break;
	case M_FILTER_XINGZUAN:
		CodeTypeGoodsListToByteArray( pByteArray, m_mFilterListOfXShop );
		break;
	case M_FILTER_WEIMIAN:
		CodeTypeGoodsListToByteArray( pByteArray, m_mFilterListOfWShop );
	    break;
	case M_FILTER_TICKET:
		CodeTypeGoodsListToByteArray( pByteArray, m_mFilterListOfTShop );
	    break;
	default:
	    break;
	}
}

void CIncrementShopList::CodeTypeGoodsListToByteArray( vector<uchar>* pByteArray,MAP_GUIDEGOODS&  mapTypeGoodsList )
{
	if ( !pByteArray ) return;

	long lNum = (long)mapTypeGoodsList.size();
	_AddToByteArray( pByteArray, lNum );
	ITR_GUIDEGOODSLIST itTypeGoods = mapTypeGoodsList.begin();
	for ( ; itTypeGoods != mapTypeGoodsList.end(); ++itTypeGoods )
	{
		DWORD dwType = itTypeGoods->first;
		_AddToByteArray( pByteArray, dwType );
		MAP_TYPEGOODS& mapGoodsList = itTypeGoods->second;
		lNum = (long)mapGoodsList.size();
		_AddToByteArray( pByteArray, lNum );
		ITR_TYPEGOODS iter = mapGoodsList.begin();
		for ( ; iter != mapGoodsList.end(); ++iter )
		{
			string strType = iter->first;
			_AddToByteArray( pByteArray, strType.c_str() );
			VEC_GOODSIDX& vecGoodsList = iter->second;
			lNum = (long)vecGoodsList.size();
			_AddToByteArray( pByteArray, lNum );
			ITR_GOODSIDX it = vecGoodsList.begin();
			for ( ; it != vecGoodsList.end(); ++it )
			{
				_AddToByteArray( pByteArray, *it );
			}
		}
	}
}

//编码物品详细信息列表
bool CIncrementShopList::CodeDetailedGoodsList( vector<uchar>* pByteArray )
{
	if (!pByteArray) return false;

	long lNum = (long)m_mIncGoodsList.size();
	_AddToByteArray( pByteArray, lNum );
	ITR_INCGOODS itIncGoodsList = m_mIncGoodsList.begin();
	for ( ; itIncGoodsList != m_mIncGoodsList.end(); ++itIncGoodsList )
	{
		DWORD dwShopType = itIncGoodsList->first;
		_AddToByteArray( pByteArray, dwShopType );
		MAP_INCSHOP& mapIncGoodsList = itIncGoodsList->second;
		lNum = (long)mapIncGoodsList.size();
		_AddToByteArray(pByteArray, lNum);
		ITR_INCSHOP iter = mapIncGoodsList.begin();
		for ( ; iter != mapIncGoodsList.end(); ++iter )
		{
			IncrementShopElem *pElem = &(iter->second);
			if( pElem )
			{
				_AddToByteArray(pByteArray, pElem->dwGoodsType);
				_AddToByteArray(pByteArray, pElem->dwGoodsIndex);
				_AddToByteArray( pByteArray, pElem->dwPrice );
				_AddToByteArray(pByteArray, pElem->dwGoodsNum);
				_AddToByteArray( pByteArray, pElem->dwDiscount );
				_AddToByteArray( pByteArray, pElem->strTimeLimit.c_str() );
				_AddToByteArray( pByteArray, pElem->lQuantityLimit );
				_AddToByteArray( pByteArray, pElem->lBuyLimit );
			}
			else
				return false;
		}
	}
	return true;
}

//编码商品分类映射表
bool CIncrementShopList::CodeIncClassGoodsList( vector<uchar>* pByteArray )
{
	if (!pByteArray) return false;

	long lNum = (long)m_mIncClassGoodsList.size();
	_AddToByteArray( pByteArray, lNum );
	ITR_INCCLASSGOODS itIncClassGoodsList = m_mIncClassGoodsList.begin();
	for ( ; itIncClassGoodsList != m_mIncClassGoodsList.end(); ++itIncClassGoodsList )
	{
		DWORD dwShopType = itIncClassGoodsList->first;
		_AddToByteArray( pByteArray, dwShopType );
		MAP_CLASSGOODS& mapClassGoodsList = itIncClassGoodsList->second;
		lNum = (long)mapClassGoodsList.size();
		_AddToByteArray(pByteArray, lNum);
		ITR_CLASSGOODS iter = mapClassGoodsList.begin();
		for ( ; iter != mapClassGoodsList.end(); ++iter )
		{
			DWORD dwClassType = iter->first;
			_AddToByteArray( pByteArray, dwClassType );
			VEC_GOODSIDX& vecGoodsIdx = iter->second;
			lNum = (long)vecGoodsIdx.size();
			_AddToByteArray( pByteArray, lNum );
			ITR_GOODSIDX it = vecGoodsIdx.begin();
			for ( ; it != vecGoodsIdx.end(); ++it )
			{
				_AddToByteArray(pByteArray, *it );
			}
		}
	}
	return true;
}

//解码物品列表
void CIncrementShopList::DecordGoodsList( uchar* pByte, long& pos, VEC_GOODSIDX& vecGoodsList )
{
	if (!pByte) return;

	long lGoodsNum = _GetLongFromByteArray( pByte, pos );
	vecGoodsList.reserve( lGoodsNum );
	for ( int i = 0; i < lGoodsNum; ++i )
	{
		DWORD dwGoodIndex = _GetDwordFromByteArray( pByte, pos );
		vecGoodsList.push_back( dwGoodIndex );
	}
}

//解码热销商品&新品推荐列表
void CIncrementShopList::DecordShopGoodsList( uchar* pByte, long& pos, eMode eDecordMode )
{
	if (!pByte) return;

	long lNum = _GetLongFromByteArray( pByte, pos );
	for ( int i = 0; i < lNum; ++i )
	{
		DWORD dwShopType = _GetDwordFromByteArray( pByte, pos );
		VEC_GOODSIDX vecGoodsList;
		DecordGoodsList( pByte, pos, vecGoodsList );
		if ( eDecordMode == M_TOP10 )
		{
			m_mTop10List.insert( make_pair( dwShopType, vecGoodsList ) );
		}
		else if ( eDecordMode == M_NEWGOODS )
		{	
			m_mNewGoodsList.insert( make_pair( dwShopType, vecGoodsList ) );
		}
	}
}

//解码导购&筛选物品列表
void CIncrementShopList::DecordTypeGoodsList( uchar* pByte, long& pos, eMode eDecordMode )
{
	if (!pByte) return;

	long lNum = _GetLongFromByteArray( pByte, pos );
	for ( int i = 0; i < lNum; ++i )
	{
		DWORD dwType = _GetDwordFromByteArray( pByte, pos );
		MAP_TYPEGOODS mapGoods;
		long lTypeNum = _GetLongFromByteArray( pByte, pos );
		for ( int j = 0; j < lTypeNum; ++j )
		{
			char strTemp[256];
			_GetStringFromByteArray( pByte, pos, strTemp );
			string strType = strTemp;
			VEC_GOODSIDX vecGoodsList;
			DecordGoodsList( pByte, pos, vecGoodsList );
			mapGoods.insert( make_pair( strType, vecGoodsList ) );
		}

		switch( eDecordMode )
		{
		case M_GUIDETYPE:
			m_mGuideGoodsList.insert( make_pair( dwType, mapGoods ) );
			break;
		case M_FILTER_WEIMIAN:
			m_mFilterListOfWShop.insert( make_pair( dwType, mapGoods ) );
			break;
		case M_FILTER_XINGZUAN:
			m_mFilterListOfXShop.insert( make_pair( dwType, mapGoods ) );
		    break;
		case M_FILTER_TICKET:
			m_mFilterListOfTShop.insert( make_pair( dwType, mapGoods ) );
		    break;
		default:
		    break;
		}
	}
}

//解码物品详细信息列表
void CIncrementShopList::DecordDetailedGoodsList( uchar* pByte, long& pos )
{
	if (!pByte) return;

	long lNum = _GetLongFromByteArray(pByte, pos);
	for ( int i = 0; i < lNum; ++i )
	{
		DWORD dwShopType = _GetDwordFromByteArray( pByte, pos );
		MAP_INCSHOP mapIncElem;
		long lGoodsNum = _GetLongFromByteArray( pByte, pos );
		for ( int j = 0; j < lGoodsNum; ++j )
		{
			char strTemp[256];
			IncrementShopElem Elem;
			Elem.dwGoodsType		= _GetDwordFromByteArray(pByte, pos);
			Elem.dwGoodsIndex		= _GetDwordFromByteArray(pByte, pos);
			Elem.dwPrice = _GetDwordFromByteArray( pByte, pos );
			Elem.dwGoodsNum			= _GetDwordFromByteArray(pByte, pos);
			Elem.dwDiscount = _GetDwordFromByteArray( pByte, pos );
			_GetStringFromByteArray( pByte, pos, strTemp );
			Elem.strTimeLimit = strTemp;
			Elem.lQuantityLimit = _GetDwordFromByteArray( pByte, pos );
			Elem.lBuyLimit = _GetDwordFromByteArray( pByte, pos );

			mapIncElem[Elem.dwGoodsIndex] = Elem;
		}
		m_mIncGoodsList[dwShopType] = mapIncElem;
	}
}

//解码商品分类映射表
void CIncrementShopList::DecordIncClassGoodsList( uchar* pByte, long& pos )
{
	if (!pByte) return;

	long lNum = _GetLongFromByteArray(pByte, pos);
	for ( int i = 0; i < lNum; ++i )
	{
		DWORD dwShopType = _GetDwordFromByteArray( pByte, pos );
		MAP_CLASSGOODS mapIncClass;
		long lGoodsNum = _GetLongFromByteArray( pByte, pos );
		for ( int j = 0; j < lGoodsNum; ++j )
		{
			DWORD dwClassType = _GetDwordFromByteArray( pByte, pos );
			long lIdxNum = _GetLongFromByteArray( pByte, pos );
			VEC_GOODSIDX vecGoodsIdx;
			for ( int k = 0; k < lIdxNum; ++k )
			{
				DWORD dwGoodsIdx = _GetDwordFromByteArray( pByte, pos );
				vecGoodsIdx.push_back( dwGoodsIdx );
			}
			mapIncClass[dwClassType] = vecGoodsIdx;
		}
		m_mIncClassGoodsList[dwShopType] = mapIncClass;
	}
}

//时间比较
bool CIncrementShopList::TimeLimited(const string& strTime)
{
	if(strTime.length()!=8)
		return false;
	string strYear=strTime.substr(0,4);
	string strMonth=strTime.substr(4,2);
	string strDay=strTime.substr(6);
	WORD wYear=static_cast<WORD>(atoi(strYear.c_str()));
	WORD wMonth=static_cast<WORD>(atoi(strMonth.c_str()));
	WORD wDay=static_cast<WORD>(atoi(strDay.c_str()));
	SYSTEMTIME	stTime;	
	GetLocalTime(&stTime);
	if(stTime.wYear<wYear)
		return true;
	if(stTime.wYear>wYear)
		return false;
	if(stTime.wMonth<wMonth)
		return true;
	if(stTime.wMonth>wMonth)
		return false;
	if(stTime.wDay<wDay)
		return true;
	if(stTime.wDay>wDay)
		return false;
	return true;
}

//取得限购数目
long CIncrementShopList::GetLimitNumByID( DWORD dwShopType, DWORD dwGoodsIndex )
{
	long lNum=0;
	ITR_INCGOODS itIncGoodsList = m_mIncGoodsList.find( dwShopType );
	if( itIncGoodsList != m_mIncGoodsList.end() )
	{
		MAP_INCSHOP& mapIncGoodsList = itIncGoodsList->second;
		ITR_INCSHOP it = mapIncGoodsList.find( dwGoodsIndex );
		if (  it != mapIncGoodsList.end() )
		{
			lNum = it->second.lBuyLimit;
		}
	}
	return lNum;
}

//取得限量数目
long CIncrementShopList::GetWorldBuyLimitNum( DWORD dwShopType, DWORD dwGoodsIndex )
{
	long lNum=0;
	ITR_INCGOODS itIncGoodsList = m_mIncGoodsList.find( dwShopType );
	if( itIncGoodsList != m_mIncGoodsList.end() )
	{
		MAP_INCSHOP& mapIncGoodsList = itIncGoodsList->second;
		ITR_INCSHOP it = mapIncGoodsList.find( dwGoodsIndex );
		if (  it != mapIncGoodsList.end() )
		{
			lNum = it->second.lQuantityLimit;
		}
	}
	return lNum;
}

//取得限时信息
string CIncrementShopList::GetLimitTimeByID( DWORD dwShopType, DWORD dwGoodsIndex )
{
	string strTimeLimit = "";
	ITR_INCGOODS itIncGoodsList = m_mIncGoodsList.find( dwShopType );
	if( itIncGoodsList != m_mIncGoodsList.end() )
	{
		MAP_INCSHOP& mapIncGoodsList = itIncGoodsList->second;
		ITR_INCSHOP it = mapIncGoodsList.find( dwGoodsIndex );
		if (  it != mapIncGoodsList.end() )
		{
			strTimeLimit = it->second.strTimeLimit;
		}
	}
	return strTimeLimit;
}

//清除容器
void CIncrementShopList::ClearIncShop()
{
	m_mTop10List.clear();											//清除热销商品列表
	m_mNewGoodsList.clear();										//清除新品推荐列表
	m_mGuideGoodsList.clear();										//清除导购商品列表
	m_mFilterListOfWShop.clear();									//清除位面商店筛选商品列表
	m_mFilterListOfXShop.clear();									//清除星钻商店筛选商品列表
	m_mFilterListOfTShop.clear();									//清除点券商店筛选商品列表
	m_mIncGoodsList.clear();										//清除商店详细商品列表
	m_mIncClassGoodsList.clear();									//清除商品分类映射表
	m_sAffiche.clear();												//清除公告
}
