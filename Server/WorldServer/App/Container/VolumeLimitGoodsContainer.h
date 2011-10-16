// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CVOLUMELIMITGOODSCONTAINER_41A6CD8700FA_INCLUDED
#define _INC_CVOLUMELIMITGOODSCONTAINER_41A6CD8700FA_INCLUDED

#include "AmountLimitGoodsContainer.h"

class CBaseObject;
class CContainer;
class CGoods;

//##ModelId=41A6CD8700FA
class CVolumeLimitGoodsContainer 
: public CAmountLimitGoodsContainer
{
public:
	//##ModelId=41BFBAF50213
	virtual BOOL OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid);

	//##ModelId=41BFBAED0167
	virtual BOOL OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid);

	//##ModelId=41BFA375002E
	virtual BOOL Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid = NULL);

	virtual BOOL AddFromDB( CGoods* pObject, DWORD dwPosition = 0 );

	//##ModelId=41BFA3910290
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

	//##ModelId=41C0E6EC0196
	virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid = NULL);

	//##ModelId=41BFA5EA03B9
	virtual VOID AI();

	//##ModelId=41BFA3BB001F
	virtual BOOL IsFull();

	//##ModelId=41BFA40F0109
	virtual BOOL QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition);

	virtual DWORD GetGoodsAmount();

	//##ModelId=41A6D1450290
	CVolumeLimitGoodsContainer();

	////##ModelId=41A6D15001D4
	//CVolumeLimitGoodsContainer(LONG lID);

	//##ModelId=41A6D158037A
	~CVolumeLimitGoodsContainer();

	//##ModelId=41A6D1E602CE
	VOID SetContainerVolume(DWORD dwVolume);

	VOID SetContainerVolume(CGoods* pGoods);

	//##ModelId=41A6D20F0177
	DWORD GetWidth();

	//##ModelId=41A6D231032C
	DWORD GetHeight();

	//##ModelId=41AFF60100B6
	BOOL FindPositionForGoods(CGoods* pGoods, DWORD& dwPosition);

	//##ModelId=41AFCFCF00D1
	BOOL IsSpaceEnough(CGoods* pGoods, DWORD dwFirstCell);

	//##ModelId=41B00B310242
	BOOL IsSpaceEnough(vector<CGoods*> vGoods);

	//##ModelId=41BFA5430213
	BOOL Swap(DWORD dwFirstCell, CGoods* pIn, CGoods** ppOut, LPVOID pVoid = NULL);

	//##ModelId=41BFA467029F
	virtual CGoods* GetGoods(DWORD dwPosition);

	//##ModelId=41BFA407002E
	virtual BOOL QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition);

	//##ModelId=41BFA36A0138
	virtual BOOL Add(CBaseObject* pObject, LPVOID pVoid = NULL);

	

	//##ModelId=41BFA3A00157
	virtual VOID Clear(LPVOID pVoid = NULL);

	//##ModelId=41BFA3A90196
	virtual VOID Release();

	//克隆的容器将不再受锁定和监听器的约束。
	//##ModelId=41BFA55101E4
	virtual BOOL Clone(CGoodsContainer* pObject);

	virtual CBaseObject*	Remove					( CBaseObject* pObject, LPVOID pVoid = NULL );

	virtual CBaseObject*	Remove					( LONG oType, const CGUID& guid , LPVOID pVoid = NULL );

	virtual CBaseObject*	Find					( CBaseObject* pObj );

	virtual CBaseObject*	Find					( LONG oType, const CGUID& guid );

	virtual CBaseObject*	Find					( const CGUID& guid );

	virtual BOOL			Serialize				( vector<BYTE>* pStream, BOOL b = TRUE );

	virtual BOOL			Unserialize				( BYTE* pStream, LONG& lOffset, BOOL b = TRUE );

	virtual BOOL			Serialize				( DBWriteSet& setWriteDB, BOOL b = TRUE );
	virtual BOOL			Unserialize				( DBReadSet& setReadDB, BOOL b = TRUE );

protected:
	//##ModelId=41B6730200EA
	VOID GetGoodsInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight, vector<CGoods*> vGoods);

	//##ModelId=41B673BE0261
	CGoods* GetFirstGoodsInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight);

	//##ModelId=41B6671E02BF
	DWORD GetGoodsAmountInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight);

private:
	//##ModelId=41A6D0200271
	//DWORD m_dwWidth;

	//##ModelId=41A6D0320138
	//DWORD m_dwHeight;
	DWORD m_dwVolume;

	//##ModelId=41AFCF1700E0
	vector<CGUID> m_vCells;
	typedef vector<CGUID>::iterator itCell;

	class CSerializeContainer : public CContainerListener
	{
	public:
		DWORD			m_dwValidGoodsAmount;
		vector<BYTE>*	m_pStream;
		DBWriteSet* m_pDBWriteData;
		bool m_bStream;

		CSerializeContainer();
		~CSerializeContainer();

		virtual BOOL OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject);
	};
};

#endif /* _INC_CVOLUMELIMITGOODSCONTAINER_41A6CD8700FA_INCLUDED */
