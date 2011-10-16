// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CGOODSSHADOWCONTAINER_41E5EEC90261_INCLUDED
#define _INC_CGOODSSHADOWCONTAINER_41E5EEC90261_INCLUDED

#include "CGoodsContainer.h"
#include "..\Listener\CContainerListener.h"

class CBaseObject;
class CGoods;
class CContainer;


//##ModelId=41E5EEC90261
class CGoodsShadowContainer 
: public CGoodsContainer
, public CContainerListener
{
public:
	//##ModelId=41E6371302EE
	struct tagGoodsShadow : public BaseMemObj
	{
		//##ModelId=41E6485E0148
		tagGoodsShadow();

		//##ModelId=41E64861006D
		~tagGoodsShadow();

		//##ModelId=41E63714006D
		LONG				lOriginalContainerType;
		//##ModelId=41E6371402FD
		CGUID				OriginalContainerID;
		//##ModelId=41E63714030D
		LONG				lOriginalContainerExtendID;
		//##ModelId=41EA1F760399
		DWORD				dwOriginalGoodsPosition;
		//##ModelId=41E63714031C
		//LONG				lGoodsID;
		CGUID				guGoodsID;
		//##ModelId=41E64EF802CE
		DWORD				dwGoodsBasePropertiesIndex;
		//##ModelId=41E63714032C
		DWORD				dwGoodsAmount;
		//##ModelId=41E63715000F
		//vector<BYTE>		vGoodsShadowStream;
	};

public:
	//##ModelId=41E5EFB4001F
	CGoodsShadowContainer();

	//##ModelId=41E5EFB702AF
	//CGoodsShadowContainer(LONG lID);

	//##ModelId=41E5EFBD008C
	~CGoodsShadowContainer();

	//##ModelId=41E656140271
	CGoodsContainer* GetGoodsSourceContainer(const CGUID& guid);

	//##ModelId=41E7635D0000
	VOID SetContainerExtendID(LONG lID);

	//##ModelId=41F0C6990048
	LONG GetContainerExtendID();

	//##ModelId=41ECAAB8035B
	BOOL GetGoodsOriginalContainerInformation(const CGUID& guid, tagPreviousContainer& tOriginalContainer);

	//##ModelId=41F46E6100A9
	BOOL GetShadowPosition(tagPreviousContainer* ptPreviousContainer, DWORD& dwPosition);

	//##ModelId=41E5EFF20213
	virtual BOOL Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41E5EFF901C5
	virtual BOOL Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41E5F0080177
	virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid = NULL);

	//##ModelId=41E735D901B5
	virtual BOOL RemoveShadow(const CGUID& guid);

	//##ModelId=41E5F00F0251
	virtual VOID Clear(LPVOID pVoid = NULL);

	//##ModelId=41E5F01503C8
	virtual VOID Release();

	//##ModelId=41E5F01B038A
	virtual VOID AI();

	//##ModelId=41E5F02203A9
	virtual BOOL IsFull();

	//##ModelId=41E5F76003D8
	virtual VOID TraversingContainer(CContainerListener* pListener);

	
	//virtual DWORD GetContentsWeight();

	//##ModelId=41E5F7760290
	virtual BOOL QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition);

	//##ModelId=41E5F77D0222
	virtual BOOL QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition);

	//##ModelId=41E5F78900FA
	virtual BOOL IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex);

	//##ModelId=41E5F791002E
	virtual CGoods* GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex);

	//##ModelId=41E5F797031C
	virtual CGoods* GetGoods(DWORD dwPosition);

	//##ModelId=41E5F79F0119
	virtual VOID GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut);

	//##ModelId=41E5F7A70196
	virtual DWORD GetGoodsAmount();

	virtual DWORD GetGoodsAmountByName(const char* strName);

	virtual DWORD GetGoodsAmountByType(int nType);

	//##ModelId=41E61B590109
	virtual BOOL OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41E61B6701F4
	virtual BOOL OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41E61B73036B
	virtual BOOL OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject);

	virtual BOOL Serialize( DBWriteSet& setWriteDB, BOOL b = TRUE ){return TRUE;}
	virtual BOOL Unserialize( DBReadSet& setReadDB, BOOL b = TRUE ) {return TRUE;}

	virtual CBaseObject* Find( CBaseObject* pObj );

	virtual CBaseObject* Find( LONG oType, const CGUID& guid );

	LONG GetGoodsSourceContainerExtendID(const CGUID& guid);

	tagGoodsShadow* RemoveByGoodsId(DWORD dwId);
	VOID AddShadowTag(CGoodsShadowContainer::tagGoodsShadow* pTag);
	CGoods* RemoveEx(const CGUID& PlayerId,LONG lContainerId,LONG lPos);
	LONG RemoveEx(const CGUID& PlayerId,LONG lContainerId,LONG lPos,LONG lNum);
	CBaseObject* FindEx( LONG oType, const CGUID& guid ,DWORD dwNum);




public:
	typedef	map<CGUID,tagGoodsShadow,hash_guid_compare>::iterator goodsshadowIt;
protected:
	//##ModelId=41E6385501B5
	map<CGUID,tagGoodsShadow,hash_guid_compare> m_mGoodsShadow;
	

	//##ModelId=41E745790186
	virtual BOOL AddShadow(const CGUID& guid);

	//##ModelId=41E5EFDF01A5
	virtual CBaseObject* Find(const CGUID& guid);

	//##ModelId=41E5F001029F
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

public:
	map<CGUID,tagGoodsShadow,hash_guid_compare>* GetGoodsShadowMap(){return &m_mGoodsShadow;}
	DWORD GetGoodsNumByIndex(LONG lId);
	VOID RemoveShadowByGoodsIndex(LONG lGoodsId);
	VOID RemoveShadowByGoodsGuid(CGUID& guid);
	CGoods* FindByGuid(LONG oType,const CGUID& guid,DWORD& dwNum);
	CGoods* GetGoodsByPos(LONG lContainerId,LONG lPos);
	//CGoods* GetGoodsById(LONG lContainerId,LONG lId);

private:
	//##ModelId=41E763CF037A
	LONG m_lContainerExtendID;
};

#endif /* _INC_CGOODSSHADOWCONTAINER_41E5EEC90261_INCLUDED */
