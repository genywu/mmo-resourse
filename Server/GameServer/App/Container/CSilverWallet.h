// Copyright (C) 1991 - 1999 Rational Software Corporation

#pragma once


#include "../Listener/CContainerListener.h"

#include "CGoodsContainer.h"

class CBaseObject;
class CGoods;
class CContainerListener;

class CContainer;

//##ModelId=41C92B7703A9
class CSilverWallet : public CGoodsContainer, public CContainerListener
  
{
public:	
	virtual BOOL OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);
	
	virtual BOOL OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);
	
	CSilverWallet();

	~CSilverWallet();
	
	virtual BOOL Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);
	
	virtual BOOL Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);
	
	virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid = NULL);
	
	virtual void Clear(LPVOID pVoid = NULL);
	
	virtual void Release();
	
	virtual BOOL IsFull();
	
	long GetMaxCapacity(void);
	
	virtual void TraversingContainer(CContainerListener* pListener);
	
	virtual DWORD GetContentsWeight();
	
	virtual BOOL QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition);
	
	virtual BOOL QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition);
	
	virtual BOOL IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex);

	virtual CGoods* GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex);

	virtual CGoods* GetGoods(DWORD dwPosition);
	
	virtual void GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut);
	
	virtual DWORD GetGoodsAmount();
	virtual DWORD GetGoodsAmountByName(const char* strName);
	virtual DWORD GetGoodsAmountByType(int nType);

	virtual BOOL			Serialize				( DBWriteSet& setWriteDB, BOOL b = TRUE );
	virtual BOOL			Unserialize				( DBReadSet& setReadDB, BOOL b = TRUE );
	
	DWORD GetSilverCoinsAmount();
	
	BOOL AddSilverCoins(DWORD dwAmount, LPVOID pVoid);
	
	BOOL DecreaseSilverCoins(DWORD dwAmount, LPVOID pVoid);

	void	SetoldSilverGood(CGoods* pGoods)	{m_pSilverCoins = pGoods;}


protected:	
	virtual CBaseObject* Find(const CGUID& guid);
	
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

private:	
	CGoods* m_pSilverCoins;

};


