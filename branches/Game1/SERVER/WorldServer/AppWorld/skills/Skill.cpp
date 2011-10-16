#include "StdAfx.h"
#include <assert.h>
#include "Skill.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CSkill::CSkill()
{
	m_dwSkillType	= SKILL_TYPE_UNKNOW;
	m_dwSkillID		= 0;
	m_dwLevel		= 0;
	m_dwIconID		= 0;
	m_bIsTargetSelf	= FALSE;
	ZeroMemory( m_strSkillName, sizeof( m_strSkillName ) );
	ZeroMemory( m_strDescription, sizeof( m_strDescription ) );
}

CSkill::CSkill( LONG lSkillID )
{
	m_dwSkillType	= SKILL_TYPE_UNKNOW;
	m_dwSkillID		= static_cast<DWORD>( lSkillID );
	m_dwLevel		= 0;
	m_dwIconID		= 0;
	m_bIsTargetSelf	= FALSE;
	ZeroMemory( m_strSkillName, sizeof( m_strSkillName ) );
	ZeroMemory( m_strDescription, sizeof( m_strDescription ) );
}

CSkill::~CSkill()
{
	Clear();
}

VOID CSkill::Clear()
{
	m_dwSkillType	= SKILL_TYPE_UNKNOW;
	m_dwSkillID		= 0;
	m_dwLevel		= 0;
	m_dwIconID		= 0;
	m_bIsTargetSelf	= FALSE;
	ZeroMemory( m_strSkillName, sizeof( m_strSkillName ) );
	ZeroMemory( m_strDescription, sizeof( m_strDescription ) );
	m_vUsages.clear();
}

VOID CSkill::AddUsage( tagUsage& rUsage )
{
	m_vUsages.push_back( rUsage );
}

VOID CSkill::SetSkillType( LONG lSkillType )
{
	m_dwSkillType = static_cast<DWORD>( lSkillType );
}

VOID CSkill::SetSkillID( LONG lSkillID )
{
	m_dwSkillID = static_cast<DWORD>( lSkillID );
}

VOID CSkill::SetLevel( LONG lLevel )
{
	m_dwLevel = static_cast<LONG>( lLevel );
}

VOID CSkill::SetSkillName( char* pName )
{
	if( pName )
	{
		strcpy( m_strSkillName, pName );
	}
}

VOID CSkill::SetDescription( char* pDescription )
{
	if( pDescription )
	{
		strcpy( m_strDescription, pDescription );
	}
}

VOID CSkill::SetIconID( DWORD dwIcon )
{
	m_dwIconID = dwIcon;
}

VOID CSkill::SetTargetType( BOOL bIsTargetSelf )
{
	m_bIsTargetSelf = bIsTargetSelf;
}

BOOL CSkill::Serialize( BYTE** ppBuffer, size_t* pLength )
{
	if( ppBuffer && pLength && 
		m_dwSkillType != SKILL_TYPE_UNKNOW &&
		m_dwSkillID != 0 )
	{
		//##类型，ID，等级，图标编号
		size_t lLength = 4 * sizeof( DWORD );
		lLength += sizeof( BOOL );
		//##两个字符串的长度标示
		lLength += sizeof( size_t ) * 2;
		lLength += strlen( m_strSkillName );
		lLength += strlen( m_strDescription );
		//##有多少个usage的标示
		lLength += sizeof( size_t );
		lLength += sizeof( DWORD ) * 2 * m_vUsages.size();

		*ppBuffer = new BYTE[lLength];
		if( *ppBuffer == NULL )
		{
			//ASSERT( false );
			return FALSE;
		}

		*pLength	= lLength;
		lLength		= 0;

		//##将类成员变量压入缓冲区
		//##技能的类型
		*( reinterpret_cast<DWORD*>( &(*ppBuffer)[lLength] ) )	= m_dwSkillType;
		lLength += sizeof( DWORD );
		//##技能的编号
		*( reinterpret_cast<DWORD*>( &(*ppBuffer)[lLength] ) )	= m_dwSkillID;
		lLength += sizeof( DWORD );
		//##技能的等级
		*( reinterpret_cast<DWORD*>( &(*ppBuffer)[lLength] ) )	= m_dwLevel;
		lLength += sizeof( DWORD );
		//##技能的图标
		*( reinterpret_cast<DWORD*>( &(*ppBuffer)[lLength] ) )	= m_dwIconID;
		lLength += sizeof( DWORD );
		//##技能的使用对象范围
		*( reinterpret_cast<BOOL*>( &(*ppBuffer)[lLength] ) )	= m_bIsTargetSelf;
		lLength += sizeof( BOOL );
		//##技能的名称长度
		*( reinterpret_cast<size_t*>( &(*ppBuffer)[lLength] ) )	= strlen( m_strSkillName );
		lLength += sizeof( size_t );
		//##技能的名称
		memcpy( &(*ppBuffer)[lLength], m_strSkillName, strlen( m_strSkillName ) );
		lLength += strlen( m_strSkillName );
		//##技能的描述长度
		*( reinterpret_cast<size_t*>( &(*ppBuffer)[lLength] ) )	= strlen( m_strDescription );
		lLength += sizeof( size_t );
		//##技能的描述
		memcpy( &(*ppBuffer)[lLength], m_strDescription, strlen( m_strDescription ) );
		lLength += strlen( m_strDescription );
		//##Usage的个数
		*( reinterpret_cast<size_t*>( &(*ppBuffer)[lLength] ) )	= m_vUsages.size();
		lLength += sizeof( size_t );
		//##所有的Usage
		for( size_t i = 0; i < m_vUsages.size(); i ++ )
		{
			if( lLength < *pLength )
			{
				*( reinterpret_cast<DWORD*>( &(*ppBuffer)[lLength] ) )	= m_vUsages[i].dwUsage;
				lLength += sizeof( DWORD );
				*( reinterpret_cast<DWORD*>( &(*ppBuffer)[lLength] ) )	= m_vUsages[i].dwCost;
				lLength += sizeof( DWORD );
			}
			else 
			{
				//##清理所有的数据，返回false
				SAFE_DELETE_ARRAY( *ppBuffer );
				*pLength = 0;
				return FALSE;
			}
		}

		return TRUE;
	}
	else return FALSE;
}