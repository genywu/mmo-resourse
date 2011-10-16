#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_MSG_C2S_CONTAINER_OBJECT_MOVE_INCLUDED
#define _INC_MSG_C2S_CONTAINER_OBJECT_MOVE_INCLUDED

class CBaseObject;
class CGoods;
class CS2CContainerObjectMove;

class CC2SContainerObjectMove
{
public:
	LONG								m_lSourceContainerType;
	CGUID								m_SourceContainerID;
	LONG								m_lSourceContainerExtendID;
	DWORD								m_dwSourceContainerPosition;

	LONG								m_lDestinationContainerType;
	CGUID								m_DestinationContainerID;
	LONG								m_lDestinationContainerExtendID;
	DWORD								m_dwDestinationContainerPosition;

	LONG								m_lObjectType;
	
	CGUID								m_guObjectID;
	DWORD								m_dwAmount;

	CPlayer*	pClientPlayer;
	//##先前的容器信息，用于rollback时使用。
	//CContainer::tagPreviousContainer	m_tPreviousContainer;

public:	
	CC2SContainerObjectMove				();
	~CC2SContainerObjectMove			();

	BOOL								Receive( CMessage* pMessage );
	BOOL								Move( CS2CContainerObjectMove* pResult);

private:
	CBaseObject*						GetSource( LONG lType = 0, const CGUID& guid = NULL_GUID );
	CBaseObject*						GetDestination();
	
	//##该函数检验是否存在该物品，该物品的数量是否符合，另外是否被锁定，然后把该物品移出容器。
	CGoods*								GetGoods( CS2CContainerObjectMove* pResult );

	CGoods*								RemoveSrcGoods(CS2CContainerObjectMove* pMessage);

	CGoodsContainer*					GetCGoodsContainer(CPlayer *pPlayer, long lContainerExtendID);

};

#endif
