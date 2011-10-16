// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CVOLUMELIMITGOODSCONTAINER_41A6CD8700FA_INCLUDED
#define _INC_CVOLUMELIMITGOODSCONTAINER_41A6CD8700FA_INCLUDED

#include "CAmountLimitGoodsContainer.h"
#include "../../../../Public/ServerPublic/Server/Common/GoodsBaseProperties.h"
class CGoods;
class CBaseObject;

class CContainer;

//##ModelId=41A6CD8700FA
class CVolumeLimitGoodsContainer 
: public CAmountLimitGoodsContainer
{
public:
	//##ModelId=41BFBAF50213
	virtual bool OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41BFBAED0167
	virtual bool OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41BFA375002E
	virtual bool Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41BFA3910290
	virtual CBaseObject* Remove(const CGUID& guid, LPVOID pVoid = NULL);

	//##ModelId=41C0E6EC0196
	virtual CBaseObject* Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid = NULL);

	//##ModelId=41BFA5EA03B9
	virtual void AI();

	//##ModelId=41BFA3BB001F
	virtual bool IsFull();

	//##ModelId=41BFA40F0109
	virtual bool QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition);

	//##ModelId=41A6D1450290
	CVolumeLimitGoodsContainer();

	////##ModelId=41A6D15001D4
	//CVolumeLimitGoodsContainer(LONG lID);

	//##ModelId=41A6D158037A
	~CVolumeLimitGoodsContainer();

	//##ModelId=41A6D1E602CE
	void SetContainerVolume(DWORD dwVolume);
	void ChangeContainerVolume(DWORD dwVolume);

	vector<CGUID>& GetCellGuid() {return m_vCells;}


	DWORD GetWidth();
	
	DWORD GetHeight();

	DWORD GetVolume();

	//##ModelId=41AFF60100B6
	bool FindPositionForGoods(CGoods* pGoods, DWORD& dwPosition);

	//查找一个针对该物品的空区间,Joe
	bool FindEmptySpaceForGoods(CGoods* pGoods, DWORD& dwPosition);

	DWORD DelGoods(DWORD dwGoodsId,int num,vector<CGoodsBaseProperties::tagGoodsDelInfo>* pVec=NULL,bool bDelete=FALSE);
	DWORD	DelGoods(eGoodsBaseType baseType,DWORD dwWeaponType,DWORD dwNum,DWORD dwPeiVal,const CGUID& playerId);

	DWORD GetEmptySpaceNum();

	//##ModelId=41AFCFCF00D1
	bool IsSpaceEnough(CGoods* pGoods, DWORD dwFirstCell);

	//##ModelId=41B00B310242
	bool IsSpaceEnough(vector<CGoods*> vGoods);

	//! 得到可以装下的物品数量
	LONG GetCanPutNum(vector<CGoods*> vGoods);

	//##ModelId=41BFA5430213
	bool Swap(DWORD dwFirstCell, CGoods* pIn, CGoods** ppOut, LPVOID pVoid = NULL);

	void ReFillCell();
	void AddGuid2Msg(CMessage* pMsg);

	void ClearGoods(DWORD dwPos);
	//##ModelId=41BFA467029F
	virtual CGoods* GetGoods(DWORD dwPosition);

	//##ModelId=41BFA407002E
	virtual bool QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition);

	//##ModelId=41BFA36A0138
	virtual bool Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	bool Add(CGoods* pGoods,vector<BYTE>* pData,DWORD dwContainerId);

	//##ModelId=41BFA3A00157
	virtual void Clear(LPVOID pVoid = NULL);

	//##ModelId=41BFA3A90196
	virtual void Release();

	//克隆的容器将不再受锁定和监听器的约束。
	//##ModelId=41BFA55101E4
	virtual bool Clone(CGoodsContainer* pObject);

	void CloneEx(CVolumeLimitGoodsContainer* pContainer,vector<CGoods*>* pGoodsVec=NULL);

	virtual DWORD GetGoodsAmount();

	virtual CBaseObject*	Remove					( CBaseObject* pObject, LPVOID pVoid = NULL );

	virtual CBaseObject*	Remove					( LONG oType, const CGUID& guid , LPVOID pVoid = NULL );

	virtual CBaseObject*	Find					( CBaseObject* pObj );

	virtual CBaseObject*	Find					( LONG oType, const CGUID& guid );

	virtual CBaseObject*	Find					( const CGUID& guid );

	virtual bool			Serialize				( DBWriteSet& setWriteDB, bool b = true );
	virtual bool			Unserialize				( DBReadSet& setReadDB, bool b = true );

	virtual void			TraversingContainer		( CContainerListener* pListener );

protected:
	//##ModelId=41B6730200EA
	void GetGoodsInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight, vector<CGoods*> vGoods);

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
	//vector<DWORD> m_vCells;
	vector<CGUID> m_vCells;
	typedef vector<CGUID>::iterator itCell;

	class CSerializeContainer : public CContainerListener
	{
	public:
		DWORD			m_dwValidGoodsAmount;
		DBWriteSet* m_pDBWriteData;
		CSerializeContainer();
		~CSerializeContainer();

		virtual bool OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject);
	};
};

#endif /* _INC_CVOLUMELIMITGOODSCONTAINER_41A6CD8700FA_INCLUDED */
