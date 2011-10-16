#include "stdafx.h"
#include <MMSystem.h>
#include "AutomaticRestoreHpState.h"
#include "..\MoveShape.h"
#include "..\Player.h"
#include "..\Monster.h"
#include "../setup/GlobeSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CAutomaticRestoreHpState::CAutomaticRestoreHpState( DWORD dwFrequency, FLOAT fVolume )
{
	//##
	m_lType			= STATE_TYPE_STANDARD;
	m_lID			= STATE_AUTOMATIC_RESTORE_HP;
	m_dwFrequency	= dwFrequency;
	m_fVolume		= fVolume;
	m_pCurrentVisualEffect = NULL;
}

CAutomaticRestoreHpState::CAutomaticRestoreHpState()
{
	//##
	m_lType			= STATE_TYPE_STANDARD;
	m_lID			= STATE_AUTOMATIC_RESTORE_HP;
	m_dwFrequency	= 10000;
	m_fVolume		= 0.01f;
	m_pCurrentVisualEffect = NULL;
}


CAutomaticRestoreHpState::~CAutomaticRestoreHpState()
{
	SAFE_DELETE(m_pCurrentVisualEffect);
}

BOOL CAutomaticRestoreHpState::Begin( CMoveShape* pUser, CMoveShape* pSufferer )
{
	CState::Begin( pUser, pSufferer );
	m_pCurrentVisualEffect = new CAutomaticRestoreHpStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	return TRUE;
}

BOOL CAutomaticRestoreHpState::Begin( CMoveShape* pUser, LONG lX, LONG lY )
{
	CState::Begin( pUser, lX, lY );
	m_pCurrentVisualEffect = new CAutomaticRestoreHpStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	return TRUE;
}

BOOL CAutomaticRestoreHpState::Begin( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion )
{
	CState::Begin( pUser, otType, lID, lRegion );
	m_pCurrentVisualEffect = new CAutomaticRestoreHpStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	return TRUE;
}

VOID CAutomaticRestoreHpState::End()
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

VOID CAutomaticRestoreHpState::AI()
{
	//##获取承受者
	CMoveShape* pSufferer = GetSufferer();
	if( pSufferer == NULL )
	{
		End();
		return;
	}

	if( pSufferer -> IsDied() || pSufferer -> GetHP() == 
		pSufferer -> GetMaxHP() ) 
	{
		return;
	}

	//##加血
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
				LONG lRestoreHp = static_cast<LONG>( pSufferer -> GetMaxHP() * fAdd );
				if( lRestoreHp < 1 ) lRestoreHp = 1;

				LONG lHp = pSufferer -> GetHP() + lRestoreHp;
				if( lHp > pSufferer -> GetMaxHP() )
				{
					lHp = pSufferer -> GetMaxHP();
				}

				pSufferer -> SetHP( static_cast<DWORD>(lHp) );
				pSufferer -> OnChangeStates();
			}
		}

		m_dwTimeStamp = timeGetTime();
	}
}

VOID CAutomaticRestoreHpState::OnChangeRegion( LONG lRegionID )
{
	//##这个状态存在于Sufferer身上，只改变Sufferer的RID
	m_lSuffererRegion = lRegionID;
}

//##类的序列化，用于在游戏服务器之间传输数据
VOID CAutomaticRestoreHpState::Serialize( vector<BYTE>& vOut )
{
	//##是否需要将技能的id和等级都压入？
	//##这个状态只有技能ID，不需要压入等级，但是有些其他的状态需要等级
	_AddToByteArray( &vOut, m_lID );

	//##频率
	_AddToByteArray( &vOut, m_dwFrequency );

	//##加血量
	_AddToByteArray( &vOut, m_fVolume );
	
}

VOID CAutomaticRestoreHpState::Unserialize( BYTE* pBytes, LONG& lPos )
{
	//##状态的技能ID已经被事先在CStateFactory中取出，所以不需要再次取出了

	//##恢复时间
	m_dwTimeStamp	= timeGetTime();

	//##频率
	m_dwFrequency	= _GetDwordFromByteArray( pBytes, lPos );

	//##血
	m_fVolume		= _GetFloatFromByteArray( pBytes, lPos );

	//##之后直接调用Begin就可以启动这个状态
}

VOID CAutomaticRestoreHpStateVisualEffect::UpdateVisualEffect( CState* pParent, DWORD dwCmd )
{
	CVisualEffect::UpdateVisualEffect( pParent );
}