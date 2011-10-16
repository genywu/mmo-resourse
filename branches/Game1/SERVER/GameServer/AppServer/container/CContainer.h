// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCONTAINER_41A16428030D_INCLUDED
#define _INC_CCONTAINER_41A16428030D_INCLUDED

#include "..\public\GUID.h"
#include "../message packaging/CS2CContainerObjectMove.h"
#include <vector>
using namespace std;

class CContainerListener;
class CBaseObject;

//##ModelId=41A16428030D
class CContainer : public BaseMemObj
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
	virtual BOOL			Add						( CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid = NULL);

	//##ModelId=41A17EA20242
	virtual VOID			Clear					( LPVOID pVoid = NULL );

	//##ModelId=41A17D5702EE
	virtual CBaseObject*	Remove					( CBaseObject* pObject, LPVOID pVoid = NULL );

	//##ModelId=41A17DFC0213
	virtual CBaseObject*	Remove					( LONG oType, const CGUID& guid, LPVOID pVoid = NULL );

	//##ModelId=41A3E8020148
	virtual CBaseObject*	Remove					( const CGUID& guid, LPVOID pVoid = NULL );

	//##ModelId=41A191F100AB
	virtual VOID			Release					();

	//##ModelId=41A434660119
	BOOL					AddListener				( CContainerListener* pListener );

	//##ModelId=41A4348A02DE
	BOOL					RemoveListener			( CContainerListener* pListener );

	//##ModelId=41A4349E00CB
	VOID					RemoveAllListener		();

	//##ModelId=41A445E5032C
	virtual VOID			TraversingContainer		( CContainerListener* pListener ) = 0;

	virtual BOOL			Serialize				( DBWriteSet& setWriteDB, BOOL b = TRUE ) = 0;
	virtual BOOL			Unserialize				( DBReadSet& setReadDB, BOOL b = TRUE ) = 0;

protected:
	//##ModelId=41A434C501E4
	vector<CContainerListener*>						m_vListeners;

	////##ModelId=41A32B6F008C
	//virtual VOID			SetID					( LONG lID );

	////##ModelId=41A32B5E038A
	//virtual VOID			SetFather				( const CContainer* pFather );

};

#endif /* _INC_CCONTAINER_41A16428030D_INCLUDED */
