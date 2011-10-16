///
/// @file tString.cpp
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "tString.h"
#include <string.h>
#include <stdlib.h>

/// Adjust 2^n if necessary, currently not adjust the size.
#define ADJUST_SIZE( size ) size

tString::tString()
{
	m_Len = 0;
	m_Str = 0;
}

tString::tString( const char *s )
{
	m_Len = 0;
	m_Str = 0;
	Assign( s );
}

tString::tString( const tString &other )
{
	m_Len = 0;
	m_Str = 0;
	Assign( other.m_Str );
}

tString::~tString()
{
	if( Valid() )
	{
		free( m_Str );
		m_Str = 0;
		m_Len = 0;
	}
}

const tString &tString::operator = ( const tString &other )
{
	return other.Valid() ? Assign( other.m_Str ) : *this;
}

const tString &tString::operator = ( const char *s )
{
	return Assign( s );
}

bool tString::operator == ( const tString &other ) const
{
	return m_Str == NULL ? other.m_Str == NULL :
		( other.m_Str == NULL ? false : strcmp( m_Str, other.m_Str ) == 0 );
}

bool tString::operator != ( const tString &other ) const
{
	return !operator == ( other );
}

tString::operator const char * () const
{
	return Str();
}

const tString &tString::Assign( const char *s )
{
	size_t len = strlen( s );
	if( len + 1 > m_Len )
	{
		m_Len = ADJUST_SIZE( len + 1 );
		m_Str = (char*) realloc( m_Str, m_Len );
	}
	memcpy( m_Str, s, len + 1 );
	return *this;
}

bool tString::Valid() const
{
	return m_Len > 0 && m_Str != NULL;
}

size_t tString::Length() const
{
	return strlen( m_Str );
}

const char *tString::Str() const
{
	return m_Str;
}
