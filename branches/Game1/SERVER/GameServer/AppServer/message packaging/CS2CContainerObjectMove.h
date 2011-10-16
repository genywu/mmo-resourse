#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_MSG_S2C_CONTAINER_OBJECT_MOVE_INCLUDED
#define _INC_MSG_S2C_CONTAINER_OBJECT_MOVE_INCLUDED

class CShape;

class CS2CContainerObjectMove
{
public:
	enum OPERATION_TYPE
	{
		OT_ROLL_BACK				= 0,
		OT_MOVE_OBJECT,
		OT_NEW_OBJECT,
		OT_DELETE_OBJECT,
		OT_SWITCH_OBJECT,
		OT_SWITCH_FILL,
		OT_CHANGE_AMOUNT,
		OT_UNKNOWN
	};

	enum PLAYER_EXTEND_ID
	{
		PEI_PACKET					= 1,	//普通背包
		PEI_EQUIPMENT,						//装备栏		
		PEI_WALLET,							//金钱
		PEI_SILVERWALLET,					//银币钱包
		PEI_GEM,							//宝石
		PEI_YUANBAO,						//元宝
		PEI_JIFEN,							//积分
		PEI_MEDAL_CONTAINER,				// 勋章容器			
		PEI_PACK =10000,					//子背包容器
		PEI_PACK1=10001,					//子背包栏位1
		PEI_PACK2=10002,					//子背包栏位2
		PEI_PACK3=10003,					//子背包栏位3
		PEI_PACK4=10004,					//子背包栏位4
		PEI_PACK5=10005,					//子背包栏位5
		PEI_BUSINESS = 10006,				//商业背包
		PEI_BUSSINESPACKFIELD = 20000,		// 放跑墒背包的栏位ID
		PEI_BUSSINESPACK = 20001			// 跑墒背包容器ID

	};

public:
	OPERATION_TYPE						m_otType;

	LONG								m_lSourceContainerType;
	CGUID								m_SourceContainerID;
	long								m_lSourceContainerExtendID;
	DWORD								m_dwSourceContainerPosition;

	LONG								m_lDestinationContainerType;
	CGUID								m_DestinationContainerID;
	long								m_lDestinationContainerExtendID;
	DWORD								m_dwDestinationContainerPosition;

	LONG								m_lSourceObjectType;
	CGUID								m_guSourceObjectID;
	DWORD								m_dwSourceObjectAmount;

	LONG								m_lDestinationObjectType;
	CGUID								m_guDestinationObjectID;
	LONG								m_dwDestinationObjectAmount;

	vector<BYTE>						m_vStream;

public:
	CS2CContainerObjectMove				();
	~CS2CContainerObjectMove			();

	VOID								Send( const CGUID& Playerguid );
	VOID								SendToAround( CShape* pShape );
	VOID								SendToSession( const CGUID& Sessionguid );

	VOID								SetOperation( OPERATION_TYPE ot );

	VOID								SetSourceContainer( LONG lType, const CGUID& guid, DWORD dwPosition );
	VOID								SetDestinationContainer( LONG lType, const CGUID& guid, DWORD dwPosition );
	VOID								SetSourceContainerExtendID( long lID );
	VOID								SetDestinationContainerExtendID( long lID );
	
	VOID								SetSourceObject( LONG lType,const CGUID& guid );
	VOID								SetSourceObjectAmount( DWORD dwAmount );

	VOID								SetDestinationObject( LONG lType,const CGUID& guid);
	VOID								SetDestinationObjectAmount( DWORD dwAmount );
	
	OPERATION_TYPE						GetOperation();

	LONG								GetSourceContainerType();
	LONG								GetDestinationContainerType();
	LONG								GetSourceContainerExtendID();
	LONG								GetDestinationContainerExtendID();

	vector<BYTE>&						GetObjectStream();

	LONG								GetSourceObjectType();
	CGUID&								GetguSourceObjectID();
	DWORD								GetSourceObjectAmount();

	LONG								GetDestinationObjectType();
	CGUID&								GetguDestinationObjectID();
	DWORD								GetDestinationObjectAmount();

	CPlayer*							GetDestPlayer(void);
};


#endif
