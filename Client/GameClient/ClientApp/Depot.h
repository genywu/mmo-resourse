#pragma once
#include "Player.h"
#include "../../Public/Common/DepotDef.h"
class CGoods;
class CMessage;
// 子背包的横向最大数
#define		SUBPACK_ITEMS_H_MAXNUM					5U
// 子背包的纵向格子总数
#define		SUBPACK_ITEMS_V_MAXNUM					4U
// 子背包的所有格子总数
#define		SUBPACK_ITEMS_MAXNUM					SUBPACK_ITEMS_H_MAXNUM * SUBPACK_ITEMS_V_MAXNUM


// 仓库的子背包栏位总数
#define		DEPOT_SUBPACK_MAXNUM					DEPOT_SUBPACK_NUM
// 仓库的扩展栏位 首位置
#define		DEPOT_SUBPACK_BASEPOS					10001U

// 仓库的所有子背包格子总数
#define		DEPOT_SUBPACK_ITEMS_MAXNUM				DEPOT_SUBPACK_MAXNUM * SUBPACK_ITEMS_MAXNUM

// 仓库的主容器格子总数
#define		DEPOT_PRIMARY_MAXNUM					PRIMARY_SIZE

// 仓库整合的横向格子总数
#define		DEPOT_CONFORMITY_H_MAXNUM				10U
// 仓库整合的纵向格子总数
#define		DEPOT_CONFORMITY_V_MAXNUM				13U
// 仓库整合的格子总数
#define		DEPOT_CONFORMITY_ITEMS_MAXNUM			DEPOT_PRIMARY_MAXNUM + DEPOT_SUBPACK_ITEMS_MAXNUM

// 仓库的所有格子总数
#define		DEPOT_ALL_ITEMS_MAXNUM					DEPOT_PRIMARY_MAXNUM + DEPOT_SUBPACK_ITEMS_MAXNUM + DEPOT_SUBPACK_MAXNUM

//**************************************************************************************
// 物品容器
//**************************************************************************************
class	CGoodsContainer
{
    std::map<DWORD,CGoods*>		m_mapGoods;					// Goods 实体表

    DWORD						m_dwContainerID;			// 容器ID
    DWORD						m_numCubage;				// 容量

public:
    CGoodsContainer();
    virtual ~CGoodsContainer();

    DWORD		GetContainerID()								{ return m_dwContainerID;			}
    void		SetContainerID( DWORD dwContainerID )			{ m_dwContainerID = dwContainerID;	}

    DWORD		GetCubage()										{ return m_numCubage;				}
    void		SetCubage( DWORD numCubage )					{ m_numCubage = numCubage;			}

    CGoods*		GetGoods( DWORD pos );
    CGoods*		GetGoods( const CGUID& guid );

    bool		AddGoods( CGoods* pGoods, DWORD pos );
    CGoods*		DelGoods( const CGUID& guidSrc, const CGUID& guidDest, long num );

    DWORD		GetPos( const CGUID& guid );
    DWORD		GetAmount();

    void		Decode( DBReadSet& setReadDB );
    void		Neaten( CMessage* pMsg );

    void		Clear();
};


//**************************************************************************************
// 物品容器管理
//**************************************************************************************
class	CGCMana
{
    std::map<DWORD,CGoodsContainer*>		m_mapGC;		// Goods容器表

public:
    bool				AddGC( DWORD id, DWORD numCubage );
    bool				DelGC( DWORD id );
    CGoodsContainer*	GetGC( DWORD id );

    bool				AddGoods( CGoods* pGoods, DWORD id, DWORD pos );
    CGoods*				DelGoods( const CGUID& guidSrc, const CGUID& guidDest, DWORD num );
    CGoods*				GetGoods( DWORD id, DWORD pos );
    CPlayer::tagGoods	GetGoods( const CGUID& guid );

    DWORD				GetAmount( DWORD id );

    void				Clear( bool bGoodsOnly = false );

    // 单件访问
    static CGCMana*		Inst()		{ return &m_Instance; }

private:
    CGCMana();
    virtual ~CGCMana();

    static	CGCMana		m_Instance;		// 单件实例
};



//**************************************************************************************
// 银行管理
//**************************************************************************************
class	CDepot
{
    bool					m_bHasPassport;							// 是否已拿到通行证
    tagPlayerDepotDataHead	m_stDepotInfo;							// 仓库信息结构

    char					m_szOldPW[ DEPOT_PASSWORD_ARRAY_LEN ];	// 保存老密码
    char					m_szNewPW[ DEPOT_PASSWORD_ARRAY_LEN ];	// 保存新密码

public:
    void					SetPassport( bool bPassport )	{ m_bHasPassport = bPassport;					}
    bool					HasPassport()					{ return m_bHasPassport;						}
    bool					HasPassWord()					{ return m_stDepotInfo.lHasPassword != 0;		}
    long					GetFreezeTime()					{ return m_stDepotInfo.lThawDate;				}
    long					GetNumOfSubItem()				{ return m_stDepotInfo.lUsableSubbackNum;		}
    long					GetPriceOfSubItem( long c )		{ return m_stDepotInfo.arr_lSubbackPrice[ c ];	}

    bool					IsDepotGC( DWORD id );

    void					SetOldPW( LPCTSTR szPW )		{ lstrcpy( m_szOldPW, szPW );					}
    void					SetNewPW( LPCTSTR szPW )		{ lstrcpy( m_szNewPW, szPW );					}
    LPCTSTR					GetOldPW()						{ return m_szOldPW;								}
    LPCTSTR					GetNewPW()						{ return m_szNewPW;								}

    //
    CGUID					GetGoldGUID();
    CGUID					GetSilverGUID();

    long					GetGoldAmount();
    long					GetSilverAmount();

    DWORD					GetSubPackCubage( DWORD idSubPack );

    // 发送\接收 消息
    void					SendEnterMsg( LPCTSTR pText );
    void					RecvEnterMsg( CMessage* pMsg );

    void					SendSetPWMsg( LPCTSTR pText );
    void					RecvSetPWMsg( CMessage* pMsg );

    void					SendBuyItemMsg();
    void					RecvBuyItemMsg( CMessage* pMsg );

    void					RecvFreezeMsg( CMessage* pMsg );

    void					DecodeDepotHeadInfo( DBReadSet& setReadDB );
    void					DecodeDepotInfo( DBReadSet& setReadDB );

    //
    static bool				Compare( const CPlayer::tagGoods* pstGoods1, const CPlayer::tagGoods* pstGoods2 );

    // 单件访问
    static CDepot*			Inst()		{ return &m_Instance; }

private:
    CDepot();
    virtual ~CDepot();

    static	CDepot			m_Instance;		// 单件实例
};