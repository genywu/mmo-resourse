// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CEQUIPMENTCONTAINER_41B3C94501A0_INCLUDED
#define _INC_CEQUIPMENTCONTAINER_41B3C94501A0_INCLUDED

#include "../Listener/CContainerListener.h"
#include "CGoodsContainer.h"

class CPlayer;
class CGoods;
class CBaseObject;

class CContainer;

//特殊的容器类，只能用于配置玩家的装备物品。
//##ModelId=41B3C94501A0
class CEquipmentContainer 
: public CGoodsContainer
, public CContainerListener
{
private:
	//##ModelId=41B3CA340295
	map<eEquipType,CGoods*> m_mEquipment;	
	CGoods* m_pGoodsBackToPack;

public:
	//##ModelId=41BFB9440119
	static DWORD s_dwEquipmentLimit;

	//##ModelId=41B3CA7F02A3
	CEquipmentContainer();

	////##ModelId=41B3CA9E02D2
	//CEquipmentContainer(LONG lID);

	//##ModelId=41B3CA960090
	~CEquipmentContainer();

	//##ModelId=41B3D0AE026C
	virtual bool Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41B3D0A1025D
	virtual CBaseObject* Find(const CGUID& guid);

	//##ModelId=41B3D0B80395
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

	//##ModelId=41C0E6DB0119
	virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid = NULL);

	//##ModelId=41B3D0C20191
	virtual void Clear(LPVOID pVoid = NULL);

	//##ModelId=41B3D0C9024D
	virtual void Release();

	//##ModelId=41B418F902D3
	virtual void TraversingContainer(CContainerListener* pListener);

	//##ModelId=41B5456901AB
	//virtual DWORD GetContentsWeight();

	//##ModelId=41B6917E00AB
	virtual bool IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex);

	//##ModelId=41BFB91C01D4
	virtual bool OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41BFB92601A5
	virtual bool OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41BF9DAB0109
	virtual bool Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41BFA0F30177
	virtual void AI();

	//##ModelId=41BFA1030271
	virtual bool IsFull();

	//##ModelId=41BFA16E03B9
	virtual bool QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition);

	//##ModelId=41BFA17A00BB
	virtual bool QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition);

	//##ModelId=41BFA197036B
	virtual CGoods* GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex);

	//##ModelId=41BFA1AC0242
	virtual CGoods* GetGoods(DWORD dwPosition);

	//##ModelId=41BFA1B8007D
	virtual void GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut);

	void GetGoodsByID(DWORD dwGoodsBasePropertiesIndex,vector<CGoods*>& vOut);

	void SetBackToPackGoods(CGoods* pGoods);
	void ClearBackToPackGoods(){m_pGoodsBackToPack=NULL;}
	CGoods* GetBackToPackGoods(){return m_pGoodsBackToPack;}

	

	//##ModelId=41BFEAD70138
	virtual DWORD GetGoodsAmount();
	virtual DWORD GetEquipAmount();

	virtual DWORD GetGoodsAmountByName(const char* strName);

	virtual DWORD GetGoodsAmountByType(int nType);
	map<eEquipType,CGoods*>* GetGoodsMap(){return &m_mEquipment;}

	//在指定位置替换一个物品,如果成功,返回被替换出的物品,否则
	//返回NULL。
	//##ModelId=41B570CD0068
	bool Swap(eEquipType ecColumn, CGoods* pIn, CGoods** ppOut, LPVOID pVoid = NULL);

	virtual CBaseObject*	Remove					( CBaseObject* pObject, LPVOID pVoid = NULL );

	virtual CBaseObject*	Remove					( LONG oType, const CGUID& guid, LPVOID pVoid = NULL );

	virtual CBaseObject*	Find					( CBaseObject* pObj );

	virtual CBaseObject*	Find					( LONG oType, const CGUID& guid );

	virtual bool			Serialize				( DBWriteSet& setWriteDB, bool b = true );
	virtual bool			Unserialize				( DBReadSet& setReadDB, bool b = true );
};

#endif /* _INC_CEQUIPMENTCONTAINER_41B3C94501A0_INCLUDED */
