#include "StdAfx.h"
#include ".\tradelist.h"


#ifdef _GAME_SERVER
#include "..\GameServer\AppServer\goods\cgoodsfactory.h"
#endif

#ifdef _WORLD_SERVER_
#include "..\WorldServer\AppWorld\goods\cgoodsfactory.h"
#endif

#ifdef _MONSTER_SERVER
#include "..\MonsterServer\AppMonster\goods\cgoodsfactory.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HWND g_hWnd;

map<string, CTradeList::tagTrade>	CTradeList::m_mapTradeList;		// 所有的NPC交易列表 map<NPC原始名，交易列表>
const BYTE MAXGOODSNUM=3;
CTradeList::CTradeList(void)
{
}

CTradeList::~CTradeList(void)
{
	m_mapTradeList.clear();
}

// 从文件读取列表
BOOL CTradeList::LoadTradeList(const char* filename)
{
	m_mapTradeList.clear();

	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		char str[256];
		_snprintf(str, 256, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return FALSE;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	string strGoodsName="";
	string strChgGoodsName="";
	DWORD dwGoodsNum=0,dwChgAmount=0,dwCredit=0,dwCreditVal=0;
	long lNum=0;
	int nShopType=0;
	int nTradeType=0,nCreditType=0;
	while(ReadTo(stream, "*"))
	{
		tagTrade stTrade;
		//读NPC名字和物品数量
		stream >> stTrade.strNpcName >> lNum>>nShopType>>nTradeType>>nCreditType;		
		stTrade.shopType=static_cast<CTradeList::eShopType>(nShopType);
		stTrade.tradeType=static_cast<CTradeList::eTradeType>(nTradeType);
		stTrade._creditType=static_cast<CTradeList::eCreditType>(nCreditType);
		for(int i=0; i<lNum; i++)
		{
			if( ReadTo(stream, "#"))
			{		
				tagTradeData stTradeData;
				if(stTrade.tradeType==CTradeList::ETT_MONEY || stTrade.tradeType==CTradeList::ETT_SILVER)
				{
					stream>>strGoodsName>>dwGoodsNum>>dwCredit>>dwCreditVal;
					// 获取物品编号
					DWORD dwGoodsIndex = 0;		
					dwGoodsIndex = CGoodsFactory::QueryGoodsIDByOriginalName( strGoodsName.c_str() );
					stTradeData.dwGoodsId=dwGoodsIndex;
					stTradeData.dwGoodsNum=dwGoodsNum;
					stTradeData.dwCreditId=dwCredit;
					stTradeData.dwCreditVal=dwCreditVal;
				}
				else if(stTrade.tradeType==CTradeList::ETT_GOODS)
				{
					stream>>strGoodsName>>dwGoodsNum;
					DWORD dwGoodsIndex = 0;		
					dwGoodsIndex = CGoodsFactory::QueryGoodsIDByOriginalName( strGoodsName.c_str() );
					stTradeData.dwGoodsId=dwGoodsIndex;
					stTradeData.dwGoodsNum=dwGoodsNum;
					//交换物,最多3个
					tagTradeGoods tgGoods;
					for (BYTE i=0;i<MAXGOODSNUM;i++)
					{
						stream>>strChgGoodsName>>dwChgAmount;
						if (strcmp(strChgGoodsName.c_str(),"0")!=0)
						{
							dwGoodsIndex=CGoodsFactory::QueryGoodsIDByOriginalName(strChgGoodsName.c_str());
							tgGoods.dwGoodsChgId=dwGoodsIndex;
							tgGoods.dwChgAmount=dwChgAmount;
							stTradeData._vecTreadGoods.push_back(tgGoods);
						}	
					}
					stream>>dwCredit>>dwCreditVal;
					stTradeData.dwCreditId=dwCredit;
					stTradeData.dwCreditVal=dwCreditVal;
				}										
				stTrade.vecGoods.push_back(stTradeData);
			}			
		}
		m_mapTradeList[stTrade.strNpcName] = stTrade;
	}
	return TRUE;
}

// 保存到文件列表
BOOL CTradeList::SaveTradeList(const char* filename)
{
	return true;
}

// 添加到CByteArray
bool CTradeList::AddToByteArray(vector<BYTE>* pByteArray)
{
	long lNum = (long)m_mapTradeList.size();
	_AddToByteArray(pByteArray, lNum);

	for(map<string, tagTrade>::iterator it=m_mapTradeList.begin(); it!=m_mapTradeList.end(); it++)
	{
		tagTrade *pTrade = &(*it).second;
		if( pTrade )
		{
			_AddToByteArray(pByteArray, (char*)pTrade->strNpcName.c_str());
			_AddToByteArray(pByteArray, (BYTE)pTrade->shopType);
			_AddToByteArray(pByteArray,(BYTE)pTrade->tradeType);
			_AddToByteArray(pByteArray,(BYTE)pTrade->_creditType);
			long lGoodsNum = (long)pTrade->vecGoods.size();
			_AddToByteArray(pByteArray, lGoodsNum);			
			for(int i=0; i<lGoodsNum; i++)
			{		
				//物品列表
				tagTradeData& tradeData=pTrade->vecGoods[i];
				_AddToByteArray(pByteArray,tradeData.dwGoodsId);
				_AddToByteArray(pByteArray,tradeData.dwGoodsNum);
				_AddToByteArray(pByteArray,tradeData.dwCreditId);
				_AddToByteArray(pByteArray,tradeData.dwCreditVal); 
				//以物易物需求列表
				vector<tagTradeGoods>& tradeGoods=tradeData._vecTreadGoods;
				long request=tradeGoods.size();
				_AddToByteArray(pByteArray,request);
				for (int j=0;j<request;j++)
				{
					_AddToByteArray(pByteArray,&tradeGoods[j],sizeof(tagTradeGoods));
				}
			}
		}
	}
	return true;
}

// 解码
bool CTradeList::DecordFromByteArray(BYTE* pByte, long& pos)
{
	m_mapTradeList.clear();

	tagTrade stTrade;		
	char strName[256];
	long lNum = _GetLongFromByteArray(pByte, pos);	
	BYTE shopType=0;
	for(int i=0; i<lNum; i++)
	{
		ZeroMemory(strName,256);
		stTrade.strNpcName = _GetStringFromByteArray(pByte, pos, strName);		
		shopType=_GetByteFromByteArray(pByte,pos);
		stTrade.shopType=static_cast<CTradeList::eShopType>(shopType);
		shopType=_GetByteFromByteArray(pByte,pos);
		stTrade.tradeType=static_cast<CTradeList::eTradeType>(shopType);
		BYTE creditTy=_GetByteFromByteArray(pByte,pos);
		stTrade._creditType=static_cast<eCreditType>(creditTy);
		long lGoodsNum = _GetLongFromByteArray(pByte, pos);
		
		stTrade.vecGoods.clear();
		for(int j=0; j<lGoodsNum; j++)
		{
			tagTradeData stTradeData;	
			stTradeData.dwGoodsId=_GetDwordFromByteArray(pByte,pos);
			stTradeData.dwGoodsNum=_GetDwordFromByteArray(pByte,pos);
			stTradeData.dwCreditId=_GetDwordFromByteArray(pByte,pos);
			stTradeData.dwCreditVal=_GetDwordFromByteArray(pByte,pos);
			//需求物列表
			DWORD size=_GetDwordFromByteArray(pByte,pos);
			stTradeData._vecTreadGoods.clear();
			for (int a=0;a<size;a++)
			{
				tagTradeGoods goods;
				_GetBufferFromByteArray(pByte,pos,&goods,sizeof(tagTradeGoods));
				stTradeData._vecTreadGoods.push_back(goods);
			}
			stTrade.vecGoods.push_back(stTradeData);
		}
		m_mapTradeList[stTrade.strNpcName] = stTrade;
		stTrade.vecGoods.clear();
	}
	return true;
}
