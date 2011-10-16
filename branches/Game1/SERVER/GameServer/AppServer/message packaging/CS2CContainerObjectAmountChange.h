#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_MSG_S2C_CONTAINER_OBJECT_AMOUNT_CHANGE_INCLUDED
#define _INC_MSG_S2C_CONTAINER_OBJECT_AMOUNT_CHANGE_INCLUDED

class CShape;

class CS2CContainerObjectAmountChange
{
private:
	LONG								m_lSourceContainerType;
	CGUID								m_SourceContainerID;
	LONG								m_lSourceContainerExtendID;
	DWORD								m_dwSourceContainerPosition;

	LONG								m_lObjectType;
	//LONG								m_lObjectID;
	CGUID								m_guObjectID;
	DWORD								m_dwAmount;

public:
	CS2CContainerObjectAmountChange		();
	~CS2CContainerObjectAmountChange	();

	VOID								Send( const CGUID& PlayerID );
	VOID								SendToSession( const CGUID& SessionID );

	VOID								SetSourceContainer( LONG lType, const CGUID& guid, DWORD dwPosition );
	VOID								SetSourceContainerExtendID( LONG lID );
	VOID								SetObject( LONG lType,const CGUID& guid );
	VOID								SetObjectAmount( DWORD dwAmount );
	LONG								GetSourceContainerType();
	const CGUID&						GetSourceContainerID();
	DWORD								GetSourceContainerPosition();
	LONG								GetSourceContainerExtendID();
	LONG								GetObjectType();
	//LONG								GetObjectID();
	CGUID&								GetguObjectID();
	DWORD								GetObjectAmount();
};


#endif
