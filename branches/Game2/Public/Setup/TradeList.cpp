#include "StdAfx.h"
#include "TradeList.h"
#include "GoodsSetup.h"
#include "../Common/Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// 静态成员初始化
map<string, TradeList::tagTrade> TradeList::m_mapTradeList;		// 所有的NPC交易列表 map<NPC原始名，交易列表>
const uchar MAXGOODSNUM = 3;

/// 构造
TradeList::TradeList(void)
{
}

/// 析构
TradeList::~TradeList(void)
{
	m_mapTradeList.clear();
}

/// 从文件加载交易列表
bool TradeList::LoadTradeList(const char* filename)
{
	m_mapTradeList.clear();

	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("file '%s' can't found!", filename));
		return false;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	string strGoodsName = "";
	string strChgGoodsName = "";
	DWORD dwGoodsNum = 0,
		dwChgAmount = 0,
		dwCredit = 0,
		dwCreditVal = 0;
	long lNum = 0;
	int nShopType = 0,
		nTradeType = 0,
		nCreditType = 0;

	while(ReadTo(stream, "*"))
	{
		tagTrade stTrade;
		//读NPC名字和物品数量
		stream >> stTrade.strNpcName >> lNum >> nShopType >> nTradeType >> nCreditType;		
		stTrade.shopType = static_cast<TradeList::eShopType>(nShopType);
		stTrade.tradeType = static_cast<TradeList::eTradeType>(nTradeType);
		stTrade._creditType = static_cast<TradeList::eCreditType>(nCreditType);
		for(int i=0; i<lNum; i++)
		{
			if(ReadTo(stream, "#"))
			{		
				tagTradeData stTradeData;
				if(stTrade.tradeType == TradeList::ETT_MONEY || stTrade.tradeType == TradeList::ETT_SILVER)
				{
					stream >> strGoodsName >> dwGoodsNum >> dwCredit >> dwCreditVal;
					// 获取物品编号
					DWORD dwGoodsIndex = 0;		
					dwGoodsIndex = GoodsSetup::QueryGoodsIDByOriginalName(strGoodsName.c_str());
					stTradeData.dwGoodsId = dwGoodsIndex;
					stTradeData.dwGoodsNum = dwGoodsNum;
					stTradeData.dwCreditId = dwCredit;
					stTradeData.dwCreditVal = dwCreditVal;
				}
				else if(stTrade.tradeType == TradeList::ETT_GOODS)
				{
					stream >> strGoodsName >> dwGoodsNum;
					DWORD dwGoodsIndex = 0;		
					dwGoodsIndex = GoodsSetup::QueryGoodsIDByOriginalName(strGoodsName.c_str());
					stTradeData.dwGoodsId = dwGoodsIndex;
					stTradeData.dwGoodsNum = dwGoodsNum;
					//交换物,最多3个
					tagTradeGoods tgGoods;
					for (uchar i=0; i<MAXGOODSNUM; i++)
					{
						stream >> strChgGoodsName >> dwChgAmount;
						if (strcmp(strChgGoodsName.c_str(),"0") != 0)
						{
							dwGoodsIndex = GoodsSetup::QueryGoodsIDByOriginalName(strChgGoodsName.c_str());
							tgGoods.dwGoodsChgId = dwGoodsIndex;
							tgGoods.dwChgAmount = dwChgAmount;
							stTradeData._vecTreadGoods.push_back(tgGoods);
						}	
					}
					stream >> dwCredit >> dwCreditVal;
					stTradeData.dwCreditId = dwCredit;
					stTradeData.dwCreditVal = dwCreditVal;
				}										
				stTrade.vecGoods.push_back(stTradeData);
			}			
		}
		m_mapTradeList[stTrade.strNpcName] = stTrade;
	}
	return true;
}


///  编码
bool TradeList::AddToByteArray(vector<uchar>* pByteArray)
{
	long lNum = (long)m_mapTradeList.size();
	_AddToByteArray(pByteArray, lNum);

	map<string, tagTrade>::iterator it = m_mapTradeList.begin();
	for(; it != m_mapTradeList.end(); ++it)
	{
		tagTrade *pTrade = &(*it).second;
		if( pTrade )
		{
			_AddToByteArray(pByteArray, (char*)pTrade->strNpcName.c_str());
			_AddToByteArray(pByteArray, (uchar)pTrade->shopType);
			_AddToByteArray(pByteArray, (uchar)pTrade->tradeType);
			_AddToByteArray(pByteArray, (uchar)pTrade->_creditType);
			long lGoodsNum = (long)pTrade->vecGoods.size();
			_AddToByteArray(pByteArray, lGoodsNum);			
			for(int i=0; i<lGoodsNum; i++)
			{		
				//物品列表
				tagTradeData& tradeData = pTrade->vecGoods[i];
				_AddToByteArray(pByteArray, tradeData.dwGoodsId);
				_AddToByteArray(pByteArray, tradeData.dwGoodsNum);
				_AddToByteArray(pByteArray, tradeData.dwCreditId);
				_AddToByteArray(pByteArray, tradeData.dwCreditVal); 
				//以物易物需求列表
				vector<tagTradeGoods>& tradeGoods = tradeData._vecTreadGoods;
				long request = static_cast<long>(tradeGoods.size());
				_AddToByteArray(pByteArray, request);
				for (int j=0; j<request; j++)
				{
					_AddToByteArray(pByteArray, &tradeGoods[j], sizeof(tagTradeGoods));
				}
			}
		}
	}
	return true;
}

/// 解码
bool TradeList::DecordFromByteArray(uchar* pByte, long& pos)
{
	m_mapTradeList.clear();

	tagTrade stTrade;		
	char strName[256];
	long lNum = _GetLongFromByteArray(pByte, pos);	
	uchar shopType = 0;
	for(int i=0; i<lNum; i++)
	{
		ZeroMemory(strName,256);
		stTrade.strNpcName = _GetStringFromByteArray(pByte, pos, strName);		
		shopType = _GetByteFromByteArray(pByte, pos);
		stTrade.shopType = static_cast<TradeList::eShopType>(shopType);
		shopType = _GetByteFromByteArray(pByte, pos);
		stTrade.tradeType = static_cast<TradeList::eTradeType>(shopType);
		uchar creditTy = _GetByteFromByteArray(pByte, pos);
		stTrade._creditType = static_cast<eCreditType>(creditTy);
		long lGoodsNum = _GetLongFromByteArray(pByte, pos);
		
		stTrade.vecGoods.clear();
		for(int j=0; j<lGoodsNum; j++)
		{
			tagTradeData stTradeData;	
			stTradeData.dwGoodsId = _GetDwordFromByteArray(pByte, pos);
			stTradeData.dwGoodsNum = _GetDwordFromByteArray(pByte, pos);
			stTradeData.dwCreditId = _GetDwordFromByteArray(pByte, pos);
			stTradeData.dwCreditVal = _GetDwordFromByteArray(pByte, pos);
			//需求物列表
			DWORD size = _GetDwordFromByteArray(pByte, pos);
			stTradeData._vecTreadGoods.clear();
			for (DWORD a=0; a<size; a++)
			{
				tagTradeGoods goods;
				_GetBufferFromByteArray(pByte, pos, &goods, sizeof(tagTradeGoods));
				stTradeData._vecTreadGoods.push_back(goods);
			}
			stTrade.vecGoods.push_back(stTradeData);
		}
		m_mapTradeList[stTrade.strNpcName] = stTrade;
		stTrade.vecGoods.clear();
	}
	return true;
}

/// 根据NPC商店名取得交易列表
TradeList::tagTrade* TradeList::GetTradeList(const char* strNpcName)
{
	return m_mapTradeList.find(strNpcName)!=m_mapTradeList.end()?
		(&m_mapTradeList[strNpcName]):NULL;
}