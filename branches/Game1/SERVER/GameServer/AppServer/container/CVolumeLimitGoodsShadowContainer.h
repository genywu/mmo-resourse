// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CVOLUMELIMITGOODSSHADOWCONTAINER_41EF40080186_INCLUDED
#define _INC_CVOLUMELIMITGOODSSHADOWCONTAINER_41EF40080186_INCLUDED

#include "CAmountLimitGoodsShadowContainer.h"

class CBaseObject;
class CGoods;

//##ModelId=41EF40080186
class CVolumeLimitGoodsShadowContainer 
: public CAmountLimitGoodsShadowContainer
{
public:
	//##ModelId=41EF4251036B
	CVolumeLimitGoodsShadowContainer();

	//##ModelId=41EF425503C8
	//CVolumeLimitGoodsShadowContainer(LONG lID);

	//##ModelId=41EF4257029F
	~CVolumeLimitGoodsShadowContainer();

	//##ModelId=41EF44C70177
	virtual BOOL Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41EF44CF0148
	virtual BOOL Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41EF44F20196
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

	//##ModelId=41EF44F802AF
	virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid = NULL);

	//##ModelId=41F0C14A01FA
	virtual BOOL RemoveShadow(const CGUID& guid);

	//##ModelId=41EF44D60196
	virtual VOID Clear(LPVOID pVoid = NULL);

	//##ModelId=41EF44DC00AB
	virtual VOID Release();

	//##ModelId=41EF4682034B
	virtual BOOL Clone(CBaseObject* pObject);

	//##ModelId=41EF44E202BF
	virtual BOOL IsFull();

	//##ModelId=41EF450303D8
	virtual BOOL QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition);

	//##ModelId=41EF450B0399
	virtual BOOL QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition);

	//##ModelId=41EF451602DE
	virtual CGoods* GetGoods(DWORD dwPosition);

	//##ModelId=41EF4645005D
	VOID SetContainerVolume(DWORD dwWidth, DWORD dwHeight);

	//##ModelId=41EF46520157
	DWORD GetContainerWidth();

	//##ModelId=41EF465A007D
	DWORD GetContainerHeight();

	//##ModelId=41EF46600290
	BOOL FindPositionForGoods(CGoods* pGoods, DWORD& dwPosition);

	//##ModelId=41EF466C0148
	BOOL IsSpaceEnough(CGoods* pGoods, DWORD dwFirstCell);

	//##ModelId=41EF4674006D
	BOOL IsSpaceEnough(vector<CGoods*> vGoods);

	//##ModelId=41EF468E0128
	VOID GetGoodsInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight, vector<CGoods*> vGoods);

	//##ModelId=41EF4697031C
	CGoods* GetFirstGoodsInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight);

	//##ModelId=41EF46A00196
	DWORD GetGoodsAmountInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight);

	DWORD GetCellsSize(){return m_vCells.size();}
	BOOL RemoveShadowEx(CGUID& guid);

	virtual CBaseObject* Remove( CBaseObject* pObject, LPVOID pVoid = NULL );
	virtual CBaseObject* Remove( LONG oType, const CGUID& guid, LPVOID pVoid = NULL );

private:
	//##ModelId=41EF428E0177
	DWORD m_dwWidth;

	//##ModelId=41EF429602EE
	DWORD m_dwHeight;

	//##ModelId=41EF428101A5
	//vector<DWORD> m_vCells;
	vector<CGUID> m_vCells;
	typedef vector<CGUID>::iterator itCell;

};

#endif /* _INC_CVOLUMELIMITGOODSSHADOWCONTAINER_41EF40080186_INCLUDED */
