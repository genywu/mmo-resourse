// Copyright (C) 1991 - 1999 Rational Software Corporation

#pragma once

#include "../MessagePackage/CS2CContainerObjectMove.h"

class CContainerListener;
class CBaseObject;

//##ModelId=41A16428030D
class CContainer
{
public:
	//##ModelId=41EA15720148
	struct tagPreviousContainer
	{
	public:
		//##ModelId=41EA158803B9
		tagPreviousContainer();
		//##ModelId=41EA1593006D
		~tagPreviousContainer();

		//##ModelId=41EA15720280
		LONG	lContainerType;
		//##ModelId=41EA157202EE
		CGUID	ContainerID;
		//##ModelId=41EA157202FD
		LONG	lContainerExtendID;
		//##ModelId=41EA1FCA003E
		DWORD	dwGoodsPosition;
	};

	
	CContainer										();
	

	//##ModelId=41A17F83000F
	virtual ~CContainer								();
	

	//##ModelId=41A1913202FD
	virtual CBaseObject*	Find					( CBaseObject* pObj );

	//##ModelId=41A17F7601D4
	virtual CBaseObject*	Find					( /*OBJECT_TYPE*/LONG oType,const CGUID& guid );

	//##ModelId=41A3256C0242
	virtual CBaseObject*	Find					( const CGUID& guid );

	//##ModelId=41A17D3B0186
	virtual bool			Add						( CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41A17EA20242
	virtual void			Clear					( LPVOID pVoid = NULL );

	//##ModelId=41A17D5702EE
	virtual CBaseObject*	Remove					( CBaseObject* pObject, LPVOID pVoid = NULL );

	//##ModelId=41A17DFC0213
	virtual CBaseObject*	Remove					( LONG oType, const CGUID& guid, LPVOID pVoid = NULL );

	//##ModelId=41A3E8020148
	virtual CBaseObject*	Remove					( const CGUID& guid, LPVOID pVoid = NULL );

	//##ModelId=41A191F100AB
	virtual void			Release					();

	//##ModelId=41A434660119
	bool					AddListener				( CContainerListener* pListener );

	//##ModelId=41A4348A02DE
	bool					RemoveListener			( CContainerListener* pListener );

	//##ModelId=41A4349E00CB
	void					RemoveAllListener		();

	//##ModelId=41A445E5032C
	virtual void			TraversingContainer		( CContainerListener* pListener ) = 0;

	virtual bool			Serialize				( DBWriteSet& setWriteDB, bool b = true ) = 0;
	virtual bool			Unserialize				( DBReadSet& setReadDB, bool b = true ) = 0;

protected:
	//##ModelId=41A434C501E4
	vector<CContainerListener*>						m_vListeners;

	////##ModelId=41A32B6F008C
	//virtual void			SetID					( LONG lID );

	////##ModelId=41A32B5E038A
	//virtual void			SetFather				( const CContainer* pFather );

};
