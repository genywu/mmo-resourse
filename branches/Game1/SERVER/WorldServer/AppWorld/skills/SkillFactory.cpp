#include "StdAfx.h"
#include "SkillFactory.h"
#include "Skill.h"
#include <ASSERT.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//##全局变量的申明
map<DWORD, CSkill*> CSkillFactory::g_mSkillMap;
map<string, tagSkillUsage> CSkillFactory::g_mSkillUsage;

CSkill* CSkillFactory::QuerySkill( LONG lSkillID, LONG lLevel )
{
	//##需要先做lSkillID的有效性验证
	DWORD dwKey = static_cast<DWORD>( lSkillID << 16 ) | 
		static_cast<DWORD>( lLevel & 0x0000ffff );

	map<DWORD, CSkill*>::iterator it = g_mSkillMap.find( dwKey );
	if( it != g_mSkillMap.end() && it -> second != NULL )
	//##找到了
	{
		return it -> second;
	}
	else 
	//##没有找到
	{
		//##生成相应的技能，并且返回
		CSkill* pSkill = new CSkill;
		if( pSkill )
		{
			if( SUCCEEDED( LoadConfigration( lSkillID, lLevel, pSkill ) ) )
			{
				g_mSkillMap[dwKey] = pSkill;
			}
			else 
			{
				SAFE_DELETE( pSkill );
			}
		}
		return pSkill;
	}
}

HRESULT CSkillFactory::LoadConfigration( LONG lSkillID, LONG lLevel, CSkill* pSkill )
{
	if( pSkill == NULL ) 
		return E_FAIL;

	//##根据lSkillID生成文件名，并且读取相应的文件
	char strSkillFile[MAX_PATH];
	sprintf( strSkillFile, "Skills\\%d.skill", lSkillID );

	//##打开文件，并且获取相应的信息
	CRFile* prfile = rfOpen(strSkillFile);
	if(prfile)
	{
		MessageBox( NULL, strSkillFile, "文件不存在", MB_OK );
		return E_FAIL;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	char	strSkillName[256];
	char	strSkillDesc[256];
	DWORD	dwIsTargetSelf;
	DWORD	dwSkillType;
	DWORD	dwIconID;
	
	Read( stream, "类型", dwSkillType );
	ReadTo( stream, "名称" );
	stream >> strSkillName;
	ReadTo( stream, "描述" );
	stream >> strSkillDesc;
	Read( stream, "图标", dwIconID );
	Read( stream, "目标", dwIsTargetSelf );

	pSkill -> SetSkillType( dwSkillType );
	pSkill -> SetSkillID( lSkillID );
	pSkill -> SetLevel( lLevel );
	pSkill -> SetSkillName( strSkillName );
	pSkill -> SetDescription( strSkillDesc );
	if( dwIsTargetSelf > 0 )
	{
		pSkill -> SetTargetType( TRUE );
	}
	pSkill -> SetIconID( dwIconID );

	char strTemp[256];
	LONG lCurrentLevel = -1;
	BOOL bFinded = FALSE;
	while( TRUE )
	{
		if( stream.eof() ) 
		{
			break;
		}

		stream >> strTemp;
		if( strcmp( strTemp, "等级" ) == 0 )
		{
			stream >> lCurrentLevel;
		}
		if( lCurrentLevel == lLevel )
		{
			bFinded = TRUE;
			CSkill::tagUsage sUsage;
			sUsage.dwUsage = static_cast<DWORD>( StringToUsage(strTemp) );
			stream >> sUsage.dwCost;
			if( sUsage.dwUsage != SKILL_USAGE_UNKNOW )
				pSkill -> AddUsage( sUsage );
		}
	}

	if( bFinded ) return S_OK;
	else 
	{
		pSkill -> Clear();
		return E_FAIL;
	}
}

//##清理所有的Skill
VOID CSkillFactory::ClearSkillCache()
{
	for( map<DWORD, CSkill*>::iterator it = g_mSkillMap.begin(); 
		it != g_mSkillMap.end(); it ++ )
	{
		SAFE_DELETE( it -> second );
	}
	g_mSkillMap.clear();
}

BOOL CSkillFactory::LoadSkillCache()
{
	ClearSkillCache();

	// 读取技能文件
	list<string> listFileName;
	string strTemptFolder("SKILLS/*.*");
	CheckRFileStr(strTemptFolder);
	string strTempt(strTemptFolder);
	int nPos = strTemptFolder.find_last_of("\\");
	strTempt.erase(0,nPos+1);
	if(strTempt=="*.*")
		strTemptFolder.erase(nPos,strTemptFolder.length()-nPos);
	if(CClientResource::GetInstance()->IsFileExist(strTemptFolder.c_str()) )
	{
		string strTemptExt(".skill");
		GetDefaultClientResource()->FindFileList(strTemptFolder,strTemptExt,listFileName);
	}
	else
	{
		FindFile( ".\\SKILLS\\*.*", ".skill", &listFileName );
	}

	BOOL bResult = TRUE;
	for( list<string>::iterator it = listFileName.begin(); it != listFileName.end(); it++ )
	{
		const char* pFileName = it -> c_str();
		if( FAILED( LoadConfigration( pFileName ) ) )
		{
			bResult = FALSE;
			ClearSkillCache();
			break;
		}
	}

	return bResult;
}

HRESULT CSkillFactory::LoadConfigration( const char* pFileName )
{
	if( pFileName == NULL ) 
		return E_FAIL;

	//##打开文件，并且获取相应的信息
	CRFile* prfile = rfOpen(pFileName);
	if(prfile == NULL)
	{
		MessageBox( NULL, pFileName, "文件不存在", MB_OK );
		return E_FAIL;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	char	strSkillName[256];
	char	strSkillDesc[256];
	DWORD	dwIsTargetSelf;
	DWORD	dwSkillType;
	DWORD	dwSkillID;
	DWORD	dwIconID;
	
	Read( stream, "类型", dwSkillType );
	Read( stream, "编号", dwSkillID );
	ReadTo( stream, "名称" );
	stream >> strSkillName;
	ReadTo( stream, "描述" );
	stream >> strSkillDesc;
	Read( stream, "图标", dwIconID );
	Read( stream, "目标", dwIsTargetSelf );

	char strTemp[256];
	LONG lCurrentLevel	= -1;
	//BOOL bFinded		= FALSE;
	CSkill*	pSkill		= NULL;
	while( TRUE )
	{
		if( stream.eof() ) 
		{
			break;
		}

		stream >> strTemp;
		if( strcmp( strTemp, "等级" ) == 0 )
		{
			stream >> lCurrentLevel;
			//##需要先做lSkillID的有效性验证
			DWORD dwKey = static_cast<DWORD>( dwSkillID << 16 ) | 
				static_cast<DWORD>( lCurrentLevel & 0x0000ffff );
			pSkill = new CSkill( dwSkillID );
			assert( pSkill );
			g_mSkillMap[dwKey] = pSkill;
			//##设置好基本信息
			pSkill -> SetSkillType( dwSkillType );
			pSkill -> SetLevel( lCurrentLevel );
			pSkill -> SetSkillName( strSkillName );
			pSkill -> SetDescription( strSkillDesc );
			if( dwIsTargetSelf > 0 )
			{
				pSkill -> SetTargetType( TRUE );
			}
			pSkill -> SetIconID( dwIconID );
		}
		else if( lCurrentLevel != -1 )
		{
			//bFinded = TRUE;
			CSkill::tagUsage sUsage;
			sUsage.dwUsage = static_cast<DWORD>( StringToUsage(strTemp) );
			stream >> sUsage.dwCost;
			if( sUsage.dwUsage != SKILL_USAGE_UNKNOW )
				pSkill -> AddUsage( sUsage );
		}
	}

	return S_OK;
	//if( bFinded ) return S_OK;
	//else 
	//{
	//	pSkill -> Clear();
	//	return E_FAIL;
	//}
}

BOOL CSkillFactory::Serialize( vector<BYTE>& vRet )
{
	//##多少个技能/等级
	//##可能需要事先确定有没有null的指针
	_AddToByteArray( &vRet, static_cast<DWORD>( g_mSkillMap.size() ) );
	
	size_t	lLength	= 0;
	BYTE*	pBuffer	= NULL;
	for( map<DWORD, CSkill*>::iterator it = g_mSkillMap.begin(); 
		it != g_mSkillMap.end(); it ++ )
	{
		if( it -> second )
		{
			it -> second -> Serialize( &pBuffer, &lLength );
			if( pBuffer )
			{
				_AddToByteArray( &vRet, static_cast<DWORD>( lLength ) );
				_AddToByteArray( &vRet, pBuffer, static_cast<LONG>(lLength) );
				SAFE_DELETE_ARRAY( pBuffer );
				lLength = 0;
			}
			else 
			{
				_AddToByteArray( &vRet, static_cast<DWORD>( 0 ) );
			}
		}
		else 
		{
			_AddToByteArray( &vRet, static_cast<DWORD>( 0 ) );
		}
	}

	return TRUE;
}

BOOL CSkillFactory::LoadUsageCache()
{
	ClearUsageCache();

	// 读取技能设置文件
	list<string> listFileName;
	string strTemptFolder("SKILLS/*.*");
	CheckRFileStr(strTemptFolder);
	string strTempt(strTemptFolder);
	int nPos = strTemptFolder.find_last_of("\\");
	strTempt.erase(0,nPos+1);
	if(strTempt=="*.*")
		strTemptFolder.erase(nPos,strTemptFolder.length()-nPos);
	if(CClientResource::GetInstance()->IsFileExist(strTemptFolder.c_str()) )
	{
		string strTemptExt(".usage");
		GetDefaultClientResource()->FindFileList(strTemptFolder,strTemptExt,listFileName);
	}
	else
	{
		FindFile( ".\\SKILLS\\*.*", ".usage", &listFileName );
	}

	BOOL bResult = TRUE;
	for( list<string>::iterator it = listFileName.begin(); it != listFileName.end(); it++ )
	{
		const char* pFileName = it -> c_str();
		if( FAILED( LoadUsage( pFileName ) ) )
		{
			bResult = FALSE;
			ClearUsageCache();
			break;
		}
	}

	return bResult;
}

HRESULT CSkillFactory::LoadUsage( const char* pFileName )
{
	if( pFileName == NULL ) 
		return E_FAIL;

	//##打开文件，并且获取相应的信息
	CRFile* prfile = rfOpen(pFileName);
	if(prfile == NULL)
	{
		MessageBox( NULL, pFileName, "文件不存在", MB_OK );
		return E_FAIL;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	char	strTemp[256];
	DWORD	dwUsage;
	while( TRUE )
	{
		if( stream.eof() ) 
		{
			break;
		}

		stream >> strTemp;
		stream >> dwUsage;

		string str( strTemp );
		g_mSkillUsage[str] = static_cast<tagSkillUsage>( dwUsage );
	}

	return S_OK;
}

VOID CSkillFactory::ClearUsageCache()
{
	g_mSkillUsage.clear();
}

tagSkillUsage CSkillFactory::StringToUsage( CHAR* pStr )
{
	//##首先判断容器是否为空，如果为空，则在配置文件中加载。
	if( pStr == NULL )
		return SKILL_USAGE_UNKNOW;

	string strKey( pStr );

	map<string, tagSkillUsage>::iterator it = g_mSkillUsage.find( strKey );
	if( it != g_mSkillUsage.end() )
	//##找到了
	{
		return it -> second;
	}
	return SKILL_USAGE_UNKNOW;
}