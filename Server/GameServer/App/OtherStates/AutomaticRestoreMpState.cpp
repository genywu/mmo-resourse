#include "stdafx.h"
#include <MMSystem.h>
#include "AutomaticRestoreMpState.h"
#include "..\MoveShape.h"
#include "..\Player.h"
#include "..\Monster.h"
#include "../setup/GlobeSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAutomaticRestoreMpState::CAutomaticRestoreMpState( DWORD dwFrequency, FLOAT fVolume )
{
	//##
	m_lType			= STATE_TYPE_STANDARD;
	m_lID			= STATE_AUTOMATIC_RESTORE_MP;
	m_dwFrequency	= dwFrequency;
	m_fVolume		= fVolume;
}

CAutomaticRestoreMpState::CAutomaticRestoreMpState()
{
	//##
	m_lType			= STATE_TYPE_STANDARD;
	m_lID			= STATE_AUTOMATIC_RESTORE_MP;
	m_dwFrequency	= 10000;
	m_fVolume		= 0.01f;
}


CAutomaticRestoreMpState::~CAutomaticRestoreMpState()
{
}

BOOL CAutomaticRestoreMpState::Begin( CMoveShape* pUser, CMoveShape* pSufferer )
{
	CState::Begin( pUser, pSufferer );
	m_pCurrentVisualEffect = new CAutomaticRestoreMpStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	return TRUE;
}

BOOL CAutomaticRestoreMpState::Begin( CMoveShape* pUser, LONG lX, LONG lY )
{
	CState::Begin( pUser, lX, lY );
	m_pCurrentVisualEffect = new CAutomaticRestoreMpStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	return TRUE;
}

BOOL CAutomaticRestoreMpState::Begin( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion )
{
	CState::Begin( pUser, otType, lID, lRegion );
	m_pCurrentVisualEffect = new CAutomaticRestoreMpStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	return TRUE;
}

VOID CAutomaticRestoreMpState::End()
{
	CMoveShape* pShape = GetSufferer();
	if( pShape )
	{
		pShape -> RemoveState( this );
	}
	else
	{
		//##获取不到承载者，有可能。
	}
}

VOID CAutomaticRestoreMpState::AI()
{
	//##获取承受者
	CPlayer* pSufferer = dynamic_cast<CPlayer*>( GetSufferer() );
	if( pSufferer == NULL )
	{
		End();
		return;
	}

	if( pSufferer -> IsDied() || pSufferer -> GetMP() == 
		pSufferer -> GetMaxMP() ) 
	{
		return;
	}

	//##加Mp
	if( timeGetTime() > m_dwTimeStamp + m_dwFrequency )
	{
		// 处理和平状态才加血 soft 2006.5.31
		if (pSufferer->GetState() == CShape::STATE_PEACE)
		{
			float fAdd = m_fVolume - (float)pSufferer->GetLevel() / CGlobeSetup::GetSetup()->lResumeFactor;

			// 保底系数 soft 2006.12.04
			if (fAdd < CGlobeSetup::GetSetup()->fResumeFactorMini)
				fAdd = CGlobeSetup::GetSetup()->fResumeFactorMini;

			if (fAdd > 0.f)
			{
				LONG lRestoreMp = static_cast<LONG>( pSufferer -> GetMaxMP() * fAdd );
				if( lRestoreMp < 1 ) lRestoreMp = 1;

				LONG lMp = pSufferer -> GetMP() + lRestoreMp;
				if( lMp > pSufferer -> GetMaxMP() )
				{
					lMp = pSufferer -> GetMaxMP();
				}

				pSufferer -> SetMP( static_cast<WORD>(lMp) );
				pSufferer -> OnChangeStates();
			}
		}

		m_dwTimeStamp = timeGetTime();
	}
}

VOID CAutomaticRestoreMpState::OnChangeRegion( LONG lRegionID )
{
	//##这个状态存在于Sufferer身上，只改变Sufferer的RID
	m_lSuffererRegion = lRegionID;
}

//##类的序列化，用于在游戏服务器之间传输数据
VOID CAutomaticRestoreMpState::Serialize( vector<BYTE>& vOut )
{
	//##是否需要将技能的id和等级都压入？
	//##这个状态只有技能ID，不需要压入等级，但是有些其他的状态需要等级
	_AddToByteArray( &vOut, m_lID );

	//##频率
	_AddToByteArray( &vOut, m_dwFrequency );

	//##加MP量
	_AddToByteArray( &vOut, m_fVolume );
	
}

VOID CAutomaticRestoreMpState::Unserialize( BYTE* pBytes, LONG& lPos )
{
	//##状态的技能ID已经被事先在CStateFactory中取出，所以不需要再次取出了

	//##恢复时间
	m_dwTimeStamp	= timeGetTime();

	//##频率
	m_dwFrequency	= _GetDwordFromByteArray( pBytes, lPos );

	//##MP
	m_fVolume		= _GetFloatFromByteArray( pBytes, lPos );

	//##之后直接调用Begin就可以启动这个状态
}

VOID CAutomaticRestoreMpStateVisualEffect::UpdateVisualEffect( CState* pParent, DWORD dwCmd )
{
	CVisualEffect::UpdateVisualEffect( pParent );
}