// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CEQUIPMENTCONTAINER_41B3C94501A0_INCLUDED
#define _INC_CEQUIPMENTCONTAINER_41B3C94501A0_INCLUDED

#include "..\Listener\CContainerListener.h"
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
public:
	//##ModelId=41B3D000001B
	enum EQUIPMENT_COLUMN 
	{	
		EC_HEAD,				//	0：头盔
		EC_NECKLACE,			//	1：项链
		EC_WING,				//	2：翅膀
		EC_BODY,				//	3：盔甲
		EC_BACK,				//	4：腰带
		EC_GLOVE,			//5：手套	
		EC_BOOT,			//6：鞋子
		EC_HEADGEAR,			//	7：头饰
		EC_FROCK,			//	8：外套		
		EC_LRING,			//	9：左戒指
		EC_RRING,			//	10：右戒指		
		EC_MEDAL1,			//	11：勋章1
		EC_MEDAL2,			//	12	勋章2
		EC_MEDAL3,			//	13	勋章3
		EC_FAIRY,			//	14：小精灵
		EC_WEAPON,			//	15：武器
		EC_WEAPON2,			//	16：副手武器
		EC_CLOAK,			//17披风
		EC_HORSE,			//18坐骑
		EC_DECORATION1,		//19饰品1
		EC_DECORATION2,		//20饰品2
		EC_ADORN,			//佩饰
		EC_GUARDIAN,		//星座守护石
		EC_TOTAL
	};


private:
	//##ModelId=41B3CA340295
	map<EQUIPMENT_COLUMN,CGoods*> m_mEquipment;

public:
	//map<EQUIPMENT_COLUMN,CGoods*> &GetGoodsMap() {return m_mEquipment;}
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
	virtual BOOL Add(CBaseObject* pObject, LPVOID pVoid = NULL);

	//##ModelId=41B3D0A1025D
	virtual CBaseObject* Find(const CGUID& guid);

	//##ModelId=41B3D0B80395
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

	//##ModelId=41C0E6DB0119
	virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid = NULL);

	//##ModelId=41B3D0C20191
	virtual VOID Clear(LPVOID pVoid = NULL);

	//##ModelId=41B3D0C9024D
	virtual VOID Release();

	//##ModelId=41B418F902D3
	virtual VOID TraversingContainer(CContainerListener* pListener);

	//##ModelId=41B5456901AB
	virtual DWORD GetContentsWeight();

	//##ModelId=41B6917E00AB
	virtual BOOL IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex);

	//##ModelId=41BFB91C01D4
	virtual BOOL OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid);

	//##ModelId=41BFB92601A5
	virtual BOOL OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid);

	//##ModelId=41BF9DAB0109
	virtual BOOL Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid = NULL);

	//从DBPlayer到CPlayer转换物品数据时
	virtual BOOL AddFromDB( CGoods* pObject, DWORD dwPosition = 0 );

	//##ModelId=41BFA0F30177
	virtual VOID AI();

	//##ModelId=41BFA1030271
	virtual BOOL IsFull();

	//##ModelId=41BFA16E03B9
	virtual BOOL QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition);

	//##ModelId=41BFA17A00BB
	virtual BOOL QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition);

	//##ModelId=41BFA197036B
	virtual CGoods* GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex);

	//##ModelId=41BFA1AC0242
	virtual CGoods* GetGoods(DWORD dwPosition);

	//##ModelId=41BFA1B8007D
	virtual VOID GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut);

	//##ModelId=41BFEAD70138
	virtual DWORD GetGoodsAmount();

	//在指定位置替换一个物品,如果成功,返回被替换出的物品,否则
	//返回NULL。
	//##ModelId=41B570CD0068
	BOOL Swap(EQUIPMENT_COLUMN ecColumn, CGoods* pIn, CGoods** ppOut, LPVOID pVoid = NULL);

	virtual CBaseObject*	Remove					( CBaseObject* pObject, LPVOID pVoid = NULL );

	virtual CBaseObject*	Remove					( LONG oType, const CGUID& guid, LPVOID pVoid = NULL );

	virtual CBaseObject*	Find					( CBaseObject* pObj );

	virtual CBaseObject*	Find					( LONG oType, const CGUID& guid );

	virtual BOOL			Serialize				( vector<BYTE>* pStream, BOOL b = TRUE );

	virtual BOOL			Unserialize				( BYTE* pStream, LONG& lOffset, BOOL b = TRUE );

	virtual BOOL			Serialize				( DBWriteSet& setWriteDB, BOOL b = TRUE );
	virtual BOOL			Unserialize				( DBReadSet& setReadDB, BOOL b = TRUE );

	map<EQUIPMENT_COLUMN,CGoods*>* GetGoodsMap(){return &m_mEquipment;}
};

#endif /* _INC_CEQUIPMENTCONTAINER_41B3C94501A0_INCLUDED */
