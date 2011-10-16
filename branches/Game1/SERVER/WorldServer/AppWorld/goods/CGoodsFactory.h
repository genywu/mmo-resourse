// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CGOODSFACTORY_41A59A7103B9_INCLUDED
#define _INC_CGOODSFACTORY_41A59A7103B9_INCLUDED

#include "CGoodsBaseProperties.h"
class CGoods;

//##ModelId=41A59A7103B9
class CGoodsFactory 
{
public:
	//##ModelId=41A688EC0109
	static VOID Release();

	//##ModelId=41A5A2A0004E
	CGoodsFactory();

	//##ModelId=41A5A2A802EE
	~CGoodsFactory();

	static VOID CreateAddonProperties(CGoods* pGoods);

	//##ModelId=41A5A2AC00FA
	static CGoodsBaseProperties* QueryGoodsBaseProperties(DWORD dwID);

	//##ModelId=41A5A3BE0399
	static CGoodsBaseProperties* QueryGoodsBasePropertiesByOriginalName(const CHAR* pName);

	//##ModelId=41A5A3D800FA
	static CGoodsBaseProperties* QueryGoodsBasePropertiesByName(const CHAR* pName);

	//##ModelId=41A5A319004E
	static const CHAR* QueryGoodsOriginalName(DWORD dwID);

	//##ModelId=41A5A3400128
	static const CHAR* QueryGoodsName(DWORD dwID);

	//##ModelId=41A5A356008C
	static DWORD QueryGoodsIDByOriginalName(const CHAR* pName);

	//##ModelId=41A5A37D0119
	static DWORD QueryGoodsIDByName(const CHAR* pName);

	//##ModelId=41A5A45B0280
	static BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);

	//##ModelId=41A5A473033C
	static BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);

	//序列化到DataBlock
	static BOOL Serialize(DBWriteSet& setWriteDB, BOOL b = TRUE);

	//通过DataBlock还原
	static BOOL Unserialize(DBReadSet& setReadDB, BOOL b = TRUE);

	//##ModelId=41A5A7660251
	static CGoods* CreateGoods(DWORD dwID,long lType);	

	//创建多个物品
	//##ModelId=41B55322019C
	static VOID CreateGoods(DWORD dwID, DWORD dwAmount, vector<CGoods*>& vOut,long lType);

	//##ModelId=41B7F08B02D6
	static DWORD GetGoldCoinIndex();
	static const char* GetGoldOrginName();
	static const char* GetGoldName();

	static DWORD GetSilverIndex();
	static const char* GetSilverOrginName();
	static const char* GetSilverName();

	static DWORD GetGemIndex();

	//##ModelId=41B7F08B02D6
	static DWORD GetYuanBaoIndex();
	


	//##ModelId=41C632EA0387
	static BOOL GarbageCollect(CGoods** ppGoods);

	//##ModelId=41C632940182
	static CGoods* UnserializeGoods(BYTE* pStream, LONG& lOffset,long lType);

	//通过ByteArray还原物品
	static CGoods* UnserializeGoods(DBReadSet& setReadDB,long lType);

	//##ModelId=41AD288C003E
	static BOOL Load(const CHAR* StrPath);

	static BOOL LoadAttrDBSetup(const char* strpath);

	

	static void AddCreateGoodsLog(CGoods* pGoods,long lType);

	
public:
	static BYTE s_GoodsAttrDBSetup[256][2];

private:
	//##ModelId=41AAC08E00BB
	static map<string, DWORD> s_mOriginalNameIndex;

	//##ModelId=41AAC0BF00DA
	static map<string, DWORD> s_mNameIndex;

	static DWORD s_dwAddonPropertyNum;

	static map<long,set<CGoods*> >	s_UsingGoods;

	

	

	//##ModelId=41A59BA702EE
	static map<DWORD,CGoodsBaseProperties*> s_mGoodsBaseProperties;
	static BOOL Upgrade(CGoods* pGoods, GOODS_ADDON_PROPERTIES gapSource, GOODS_ADDON_PROPERTIES gapDest, BOOL bUpgrade = TRUE );

	//! 聊天中的物品信息管理
	////////////////////////////////////////////////////////////////////////////////
public:
	//!							添加一个物品信息
	static		void			AddGoodsInfo(const CGUID &Key, const vector<BYTE> &vGoodsInfo);
	//!							取得一个物品信息
	static		BOOL			GetGoodsInfo(const CGUID &Key, vector<BYTE> &vGoodsInfo);

	static		long			OutputUseGoodsInfo(const char *pszFileName);

private:
	//!							清理
	static		void			ClearInfo(LONG lClearNum);


	//!									保存的物品信息
	static	map<CGUID, vector<BYTE>>	m_mapChatGoodsInfo;
	//!									保存的时间先后顺序
	static	list<pair<CGUID, LONG>>		m_listSaveTimeOrder;

};

#endif /* _INC_CGOODSFACTORY_41A59A7103B9_INCLUDED */
