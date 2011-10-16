///
/// @file FileNameListManager.cpp
/// @author Kevin Lynx
/// @brief to manage some file name lists in AntiCheat module.
///
#include "stdafx.h"
#include "FileListManager.h"
#include "AntiCheatCommon.h"

namespace AntiCheat
{

#define FILE_EXT ".bmp"

	static CFileListManager::Buffer s_invalid = { 0, 0 };
#define LOG_REQUEST_INVALID( t ) PutoutLog( LOG_FILE, LT_WARNING, "Request NULL file list [%s].", (const char*)(t) )

	bool CFileListManager::LoadFile( const std::string &file, FileBufferList &file_list )
	{
		CRFile *rf = rfOpen( file.c_str() );
		if( rf == NULL )
		{
			PutoutLog( LOG_FILE, LT_ERROR, "Load file [%s] failed.", file.c_str() );
			return false;
		}
		Buffer buf;
		buf.size = (size_t)rf->GetDatalen();
		buf.buf = malloc( buf.size );
#ifdef RECORD_NAME
		buf.name = file;
#endif
		memcpy( buf.buf, rf->GetData(), buf.size );
		rfClose( rf );

		file_list.push_back( buf );
		return true;
	}

	bool CFileListManager::LoadFileList( const std::list<std::string> &file_name_list, FileBufferList &file_list )
	{
		bool ret = true;
		for( std::list<std::string>::const_iterator it = file_name_list.begin(); 
			it != file_name_list.end(); ++ it )
		{
			ret = LoadFile( *it, file_list ) && ret;
		}
		return ret;
	}

	bool CFileListManager::Init()
	{
		bool ret = true;
		CClientResource *cr = CClientResource::GetInstance();		
		std::list<std::string> file_list;
		const char *back_folder = "\\anticheat\\back";
		const char *small_folder = "\\anticheat\\small";
		const char *x_folder = "\\anticheat\\x";

		// load back picture file list
		if( cr->IsFileExist( back_folder ) )
		{
			cr->FindFileList( std::string( back_folder ), std::string( FILE_EXT ), file_list ); 
		}
		else
		{
			FindFile( "anticheat/back/*.*", FILE_EXT, &file_list );
		}
		ret = LoadFileList( file_list, m_BackList ) && ret;
		

		// load small pictures file list
		file_list.clear();
		if( cr->IsFileExist( small_folder ) )
		{
			cr->FindFileList( std::string( small_folder ), std::string( FILE_EXT ), file_list );
		}
		else
		{
			FindFile( "anticheat/small/*.*", FILE_EXT, &file_list );
		}
		ret = LoadFileList( file_list, m_SmallList ) && ret;

		// load 'X' pictures file list
		file_list.clear();
		if( cr->IsFileExist( x_folder ) )
		{
			cr->FindFileList( std::string( x_folder ), std::string( FILE_EXT ), file_list );
		}
		else
		{
			FindFile( "anticheat/x/*.*", FILE_EXT, &file_list );
		}
		ret = LoadFileList( file_list, m_XList ) && ret;

		// load 'A''B''C''D' sign files
		ret = LoadFile( "anticheat/a.bmp", m_SelList ) && ret;
		ret = LoadFile( "anticheat/b.bmp", m_SelList ) && ret;
		ret = LoadFile( "anticheat/c.bmp", m_SelList ) && ret;
		ret = LoadFile( "anticheat/d.bmp", m_SelList ) && ret;

		// load alpha file
		ret = LoadFile( "anticheat/alpha.bmp", m_AlphaList ) && ret;

		return ret;
	}

	void CFileListManager::Release()
	{
		ReleaseFileList( m_BackList );
		ReleaseFileList( m_SmallList );
		ReleaseFileList( m_XList );
		ReleaseFileList( m_SelList );
		ReleaseFileList( m_AlphaList );
	}

	void CFileListManager::ReleaseFileList( FileBufferList &file_list )
	{
		for( FileBufferList::iterator it = file_list.begin(); it != file_list.end(); ++ it )
		{
			free( (*it).buf );
		}
		file_list.clear();
	}

	bool CFileListManager::ReInit()
	{
		Release();
		return Init();
	}

	const CFileListManager::Buffer &CFileListManager::GetRandBack() const
	{
		if( m_BackList.size() == 0 )
		{
			LOG_REQUEST_INVALID( "BACK" );
			return s_invalid;
		}
		return m_BackList[random( m_BackList.size() )];
	}

	const CFileListManager::Buffer &CFileListManager::GetRandSmall() const
	{
		if( m_SmallList.size() == 0 )
		{
			LOG_REQUEST_INVALID( "SMALL" );
			return s_invalid;
		}
		return m_SmallList[random( m_SmallList.size() )];
	}

	const CFileListManager::Buffer &CFileListManager::GetRandX() const
	{
		if( m_XList.size() == 0 )
		{
			LOG_REQUEST_INVALID( "X" );
			return s_invalid;
		}
		return m_XList[random( m_XList.size() )];
	}
	
	const CFileListManager::Buffer &CFileListManager::GetSelSign( int sel ) const
	{
		if( m_SelList.size() == 0 || sel > m_SelList.size() )
		{
			LOG_REQUEST_INVALID( "SIGN" );
			return s_invalid;
		}
		return m_SelList[sel - 1];
	}

	const CFileListManager::Buffer &CFileListManager::GetAlpha() const
	{
		if( m_AlphaList.size() == 0 )
		{
			LOG_REQUEST_INVALID( "ALPHA" );
			return s_invalid;
		}
		
		return m_AlphaList[0];
	}
}