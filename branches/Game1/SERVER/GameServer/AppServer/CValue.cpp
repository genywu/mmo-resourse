// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "CValue.h"


//##ModelId=41A46E2602FD
CValue::CValue(INT64 iMin, INT64 iMax) : m_iValue(0), m_iModifier(0), m_iMinValue(iMin), m_iMaxValue(iMax)
{
	// TODO: Add your specialized code here.
}

CValue::CValue() : m_iValue(0), m_iModifier(0), m_iMinValue(0), m_iMaxValue(0)
{
}

//##ModelId=41A46E2B036B
CValue::~CValue()
{
	// TODO: Add your specialized code here.
}

//##ModelId=41FAEC43004E
CValue::operator CHAR()
{
	INT64 iResult	= 0;
	CHAR cMaxValue	= 0x7F;
	CHAR cMinValue	= 0;
	memset( &cMinValue, 0xFF, sizeof(CHAR) );

	if( m_iMaxValue < cMaxValue )
	{
		cMaxValue = static_cast<CHAR>( m_iMaxValue );
	}

	if( m_iMinValue > cMinValue )
	{
		cMinValue = static_cast<CHAR>( m_iMinValue );
	}

	if( m_iValue + m_iModifier < cMaxValue )
	{
		iResult = m_iValue + m_iModifier;
	}
	else 
	{
		iResult = cMaxValue;
	}

	if( iResult <= cMinValue )
	{
		iResult = cMinValue;
	}

	return static_cast<CHAR>( iResult );
}

//##ModelId=41FAE9C80399
CValue::operator SHORT()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult	= 0;
	SHORT sMaxValue	= 0x7FFF;
	SHORT sMinValue	= 0;
	memset( &sMinValue, 0xFF, sizeof(SHORT) );

	if( m_iMaxValue < sMaxValue )
	{
		sMaxValue = static_cast<SHORT>( m_iMaxValue );
	}

	if( m_iMinValue > sMinValue )
	{
		sMinValue = static_cast<SHORT>( m_iMinValue );
	}

	if( m_iValue + m_iModifier < sMaxValue )
	{
		iResult = m_iValue + m_iModifier;
	}
	else 
	{
		iResult = sMaxValue;
	}

	if( iResult <= sMinValue )
	{
		iResult = sMinValue;
	}

	return static_cast<SHORT>( iResult );
}

//##ModelId=41A473B601A5
CValue::operator LONG ()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult	= 0;
	LONG lMaxValue	= 0x7FFFFFFF;
	LONG lMinValue	= 0;
	memset( &lMinValue, 0xFF, sizeof(LONG) );


	if( m_iMaxValue < lMaxValue )
	{
		lMaxValue = static_cast<LONG>( m_iMaxValue );
	}

	if( m_iMinValue > lMinValue )
	{
		lMinValue = static_cast<LONG>( m_iMinValue );
	}

	if( m_iValue + m_iModifier < lMaxValue )
	{
		iResult = m_iValue + m_iModifier;
	}
	else 
	{
		iResult = lMaxValue;
	}

	if( iResult <= lMinValue )
	{
		iResult = lMinValue;
	}

	return static_cast<LONG>( iResult );
}

//##ModelId=41FAE9DB00AB
CValue::operator BYTE()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult	= 0;
	BYTE bMaxValue	= 0xFF;
	BYTE bMinValue	= 0x0;

	if( m_iMaxValue < bMaxValue )
	{
		bMaxValue = static_cast<BYTE>( m_iMaxValue );
	}

	if( m_iMinValue > bMinValue )
	{
		bMinValue = static_cast<BYTE>( m_iMinValue );
	}

	if( m_iValue + m_iModifier < bMaxValue )
	{
		iResult = m_iValue + m_iModifier;
	}
	else 
	{
		iResult = bMaxValue;
	}

	if( iResult <= bMinValue )
	{
		iResult = bMinValue;
	}

	return static_cast<BYTE>( iResult );
}

//##ModelId=41FAEC4300FA
CValue::operator WORD()
{
	INT64 iResult	= 0;
	WORD wMaxValue	= 0xFFFF;
	WORD wMinValue	= 0x0;

	if( m_iMaxValue < wMaxValue )
	{
		wMaxValue = static_cast<WORD>( m_iMaxValue );
	}

	if( m_iMinValue > wMinValue )
	{
		wMinValue = static_cast<WORD>( m_iMinValue );
	}

	if( m_iValue + m_iModifier < wMaxValue )
	{
		iResult = m_iValue + m_iModifier;
	}
	else 
	{
		iResult = wMaxValue;
	}

	if( iResult <= wMinValue )
	{
		iResult = wMinValue;
	}

	return static_cast<WORD>( iResult );
}

//##ModelId=41A4739102EE
CValue::operator DWORD ()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult		= 0;
	DWORD dwMaxValue	= 0xFFFFFFFF;
	DWORD dwMinValue	= 0x0;

	if( m_iMaxValue < dwMaxValue )
	{
		dwMaxValue = static_cast<DWORD>( m_iMaxValue );
	}

	if( m_iMinValue > dwMinValue )
	{
		dwMinValue = static_cast<DWORD>( m_iMinValue );
	}

	if( m_iValue + m_iModifier < dwMaxValue )
	{
		iResult = m_iValue + m_iModifier;
	}
	else 
	{
		iResult = dwMaxValue;
	}

	if( iResult <= dwMinValue )
	{
		iResult = dwMinValue;
	}

	return static_cast<DWORD>( iResult );
}

//##ModelId=41FAEA4403D8
CValue::operator FLOAT()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return static_cast<FLOAT>( m_iValue + m_iModifier );
}

CHAR CValue::operator =(CHAR cValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_iModifier = static_cast<INT64>( cValue ) - m_iValue;
	return cValue;
}

SHORT CValue::operator =(SHORT sValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_iModifier = static_cast<INT64>( sValue ) - m_iValue;
	return sValue;
}

//##ModelId=41A46C0B0167
LONG CValue::operator =(LONG lValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_iModifier = static_cast<INT64>( lValue ) - m_iValue;
	return lValue;
}

BYTE CValue::operator =(BYTE bValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_iModifier = static_cast<INT64>( bValue ) - m_iValue;
	return bValue;
}

WORD CValue::operator =(WORD wValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_iModifier = static_cast<INT64>( wValue ) - m_iValue;
	return wValue;
}

//##ModelId=41A46C8401B5
DWORD CValue::operator =(DWORD dwValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_iModifier = static_cast<INT64>( dwValue ) - m_iValue;
	return dwValue;
}

//##ModelId=41FAEC4302BF
FLOAT CValue::operator =(FLOAT fValue)
{
	m_iModifier = static_cast<INT64>( fValue ) - m_iValue;
	return fValue;
}

//##ModelId=41FB210D000F
INT64 CValue::BaseValue()
{
	return m_iValue;
}

//##ModelId=41FB210D0138
VOID CValue::SetBaseValue( INT64 iValue )
{
	m_iValue = iValue;
}

//##ModelId=41FB301B0399
VOID CValue::SetMinValue(INT64 iMinValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_iMinValue = iMinValue;
}

//##ModelId=41FB302C0222
VOID CValue::SetMaxValue(INT64 iMaxValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_iMaxValue = iMaxValue;
}

INT64 CValue::GetMinValue()
{
	return m_iMinValue;
}

INT64 CValue::GetMaxValue()
{
	return m_iMaxValue;
}

VOID CValue::Clear()
{
	m_iValue	= 0;
	m_iModifier	= 0;
}

CHAR CValue::operator +(CHAR cValue)
{
	INT64 iResult	= 0;
	CHAR cMaxValue	= 0x7F;
	CHAR cMinValue	= 0;
	memset( &cMinValue, 0xFF, sizeof(CHAR) );

	if( m_iMaxValue < cMaxValue )
	{
		cMaxValue = static_cast<CHAR>( m_iMaxValue );
	}

	if( m_iMinValue > cMinValue )
	{
		cMinValue = static_cast<CHAR>( m_iMinValue );
	}

	if( m_iValue + m_iModifier + cValue < cMaxValue )
	{
		iResult = m_iValue + m_iModifier + cValue;
	}
	else 
	{
		iResult = cMaxValue;
	}

	if( iResult <= cMinValue )
	{
		iResult = cMinValue;
	}

	return static_cast<CHAR>( iResult );
}

SHORT CValue::operator +(SHORT sValue)
{
	INT64 iResult	= 0;
	SHORT sMaxValue	= 0x7FFF;
	SHORT sMinValue	= 0;
	memset( &sMinValue, 0xFFFF, sizeof(SHORT) );

	if( m_iMaxValue < sMaxValue )
	{
		sMaxValue = static_cast<SHORT>( m_iMaxValue );
	}

	if( m_iMinValue > sMinValue )
	{
		sMinValue = static_cast<SHORT>( m_iMinValue );
	}

	if( m_iValue + m_iModifier + sValue < sMaxValue )
	{
		iResult = m_iValue + m_iModifier + sValue;
	}
	else 
	{
		iResult = sMaxValue;
	}

	if( iResult <= sMinValue )
	{
		iResult = sMinValue;
	}

	return static_cast<CHAR>( iResult );
}

LONG CValue::operator +( LONG lValue )
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult	= 0;
	LONG lMaxValue	= 0x7FFFFFFF;
	LONG lMinValue	= 0;
	memset( &lMinValue, 0xFF, sizeof(LONG) );


	if( m_iMaxValue < lMaxValue )
	{
		lMaxValue = static_cast<LONG>( m_iMaxValue );
	}

	if( m_iMinValue > lMinValue )
	{
		lMinValue = static_cast<LONG>( m_iMinValue );
	}

	if( m_iValue + m_iModifier + lValue < lMaxValue )
	{
		iResult = m_iValue + m_iModifier + lValue;
	}
	else 
	{
		iResult = lMaxValue;
	}

	if( iResult <= lMinValue )
	{
		iResult = lMinValue;
	}

	return static_cast<LONG>( iResult );
}

BYTE CValue::operator +(BYTE bValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult	= 0;
	BYTE bMaxValue	= 0xFF;
	BYTE bMinValue	= 0x0;

	if( m_iMaxValue < bMaxValue )
	{
		bMaxValue = static_cast<BYTE>( m_iMaxValue );
	}

	if( m_iMinValue > bMinValue )
	{
		bMinValue = static_cast<BYTE>( m_iMinValue );
	}

	if( m_iValue + m_iModifier + bValue < bMaxValue )
	{
		iResult = m_iValue + m_iModifier + bValue;
	}
	else 
	{
		iResult = bMaxValue;
	}

	if( iResult <= bMinValue )
	{
		iResult = bMinValue;
	}

	return static_cast<BYTE>( iResult );
}

WORD CValue::operator +(WORD wValue)
{
	INT64 iResult	= 0;
	WORD wMaxValue	= 0xFFFF;
	WORD wMinValue	= 0x0;

	if( m_iMaxValue < wMaxValue )
	{
		wMaxValue = static_cast<WORD>( m_iMaxValue );
	}

	if( m_iMinValue > wMinValue )
	{
		wMinValue = static_cast<WORD>( m_iMinValue );
	}

	if( m_iValue + m_iModifier + wValue < wMaxValue )
	{
		iResult = m_iValue + m_iModifier + wValue;
	}
	else 
	{
		iResult = wMaxValue;
	}

	if( iResult <= wMinValue )
	{
		iResult = wMinValue;
	}

	return static_cast<WORD>( iResult );
}

DWORD CValue::operator +(DWORD dwValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult		= 0;
	DWORD dwMaxValue	= 0xFFFFFFFF;
	DWORD dwMinValue	= 0x0;

	if( m_iMaxValue < dwMaxValue )
	{
		dwMaxValue = static_cast<DWORD>( m_iMaxValue );
	}

	if( m_iMinValue > dwMinValue )
	{
		dwMinValue = static_cast<DWORD>( m_iMinValue );
	}

	if( m_iValue + m_iModifier + dwValue < dwMaxValue )
	{
		iResult = m_iValue + m_iModifier + dwValue;
	}
	else 
	{
		iResult = dwMaxValue;
	}

	if( iResult <= dwMinValue )
	{
		iResult = dwMinValue;
	}

	return static_cast<DWORD>( iResult );
}


CHAR CValue::operator -(CHAR cValue)
{
	INT64 iResult	= 0;
	CHAR cMaxValue	= 0x7F;
	CHAR cMinValue	= 0;
	memset( &cMinValue, 0xFF, sizeof(CHAR) );

	if( m_iMaxValue < cMaxValue )
	{
		cMaxValue = static_cast<CHAR>( m_iMaxValue );
	}

	if( m_iMinValue > cMinValue )
	{
		cMinValue = static_cast<CHAR>( m_iMinValue );
	}

	if( m_iValue + m_iModifier - cValue < cMaxValue )
	{
		iResult = m_iValue + m_iModifier - cValue;
	}
	else 
	{
		iResult = cMaxValue;
	}

	if( iResult <= cMinValue )
	{
		iResult = cMinValue;
	}

	return static_cast<CHAR>( iResult );
}

SHORT CValue::operator -(SHORT sValue)
{
	INT64 iResult	= 0;
	SHORT sMaxValue	= 0x7FFF;
	SHORT sMinValue	= 0;
	memset( &sMinValue, 0xFFFF, sizeof(SHORT) );

	if( m_iMaxValue < sMaxValue )
	{
		sMaxValue = static_cast<SHORT>( m_iMaxValue );
	}

	if( m_iMinValue > sMinValue )
	{
		sMinValue = static_cast<SHORT>( m_iMinValue );
	}

	if( m_iValue + m_iModifier - sValue < sMaxValue )
	{
		iResult = m_iValue + m_iModifier - sValue;
	}
	else 
	{
		iResult = sMaxValue;
	}

	if( iResult <= sMinValue )
	{
		iResult = sMinValue;
	}

	return static_cast<CHAR>( iResult );
}

LONG CValue::operator -( LONG lValue )
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult	= 0;
	LONG lMaxValue	= 0x7FFFFFFF;
	LONG lMinValue	= 0;
	memset( &lMinValue, 0xFF, sizeof(LONG) );


	if( m_iMaxValue < lMaxValue )
	{
		lMaxValue = static_cast<LONG>( m_iMaxValue );
	}

	if( m_iMinValue > lMinValue )
	{
		lMinValue = static_cast<LONG>( m_iMinValue );
	}

	if( m_iValue + m_iModifier - lValue < lMaxValue )
	{
		iResult = m_iValue + m_iModifier - lValue;
	}
	else 
	{
		iResult = lMaxValue;
	}

	if( iResult <= lMinValue )
	{
		iResult = lMinValue;
	}

	return static_cast<LONG>( iResult );
}

BYTE CValue::operator -(BYTE bValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult	= 0;
	BYTE bMaxValue	= 0xFF;
	BYTE bMinValue	= 0x0;

	if( m_iMaxValue < bMaxValue )
	{
		bMaxValue = static_cast<BYTE>( m_iMaxValue );
	}

	if( m_iMinValue > bMinValue )
	{
		bMinValue = static_cast<BYTE>( m_iMinValue );
	}

	if( m_iValue + m_iModifier - bValue < bMaxValue )
	{
		iResult = m_iValue + m_iModifier - bValue;
	}
	else 
	{
		iResult = bMaxValue;
	}

	if( iResult <= bMinValue )
	{
		iResult = bMinValue;
	}

	return static_cast<BYTE>( iResult );
}

WORD CValue::operator -(WORD wValue)
{
	INT64 iResult	= 0;
	WORD wMaxValue	= 0xFFFF;
	WORD wMinValue	= 0x0;

	if( m_iMaxValue < wMaxValue )
	{
		wMaxValue = static_cast<WORD>( m_iMaxValue );
	}

	if( m_iMinValue > wMinValue )
	{
		wMinValue = static_cast<WORD>( m_iMinValue );
	}

	if( m_iValue + m_iModifier - wValue < wMaxValue )
	{
		iResult = m_iValue + m_iModifier - wValue;
	}
	else 
	{
		iResult = wMaxValue;
	}

	if( iResult <= wMinValue )
	{
		iResult = wMinValue;
	}

	return static_cast<WORD>( iResult );
}

DWORD CValue::operator -(DWORD dwValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult		= 0;
	DWORD dwMaxValue	= 0xFFFFFFFF;
	DWORD dwMinValue	= 0x0;

	if( m_iMaxValue < dwMaxValue )
	{
		dwMaxValue = static_cast<DWORD>( m_iMaxValue );
	}

	if( m_iMinValue > dwMinValue )
	{
		dwMinValue = static_cast<DWORD>( m_iMinValue );
	}

	if( m_iValue + m_iModifier - dwValue < dwMaxValue )
	{
		iResult = m_iValue + m_iModifier - dwValue;
	}
	else 
	{
		iResult = dwMaxValue;
	}

	if( iResult <= dwMinValue )
	{
		iResult = dwMinValue;
	}

	return static_cast<DWORD>( iResult );
}


CHAR CValue::operator *(CHAR cValue)
{
	INT64 iResult	= 0;
	CHAR cMaxValue	= 0x7F;
	CHAR cMinValue	= 0;
	memset( &cMinValue, 0xFF, sizeof(CHAR) );

	if( m_iMaxValue < cMaxValue )
	{
		cMaxValue = static_cast<CHAR>( m_iMaxValue );
	}

	if( m_iMinValue > cMinValue )
	{
		cMinValue = static_cast<CHAR>( m_iMinValue );
	}

	if( (m_iValue + m_iModifier) * cValue < cMaxValue )
	{
		iResult = (m_iValue + m_iModifier) * cValue;
	}
	else 
	{
		iResult = cMaxValue;
	}

	if( iResult <= cMinValue )
	{
		iResult = cMinValue;
	}

	return static_cast<CHAR>( iResult );
}

SHORT CValue::operator *(SHORT sValue)
{
	INT64 iResult	= 0;
	SHORT sMaxValue	= 0x7FFF;
	SHORT sMinValue	= 0;
	memset( &sMinValue, 0xFFFF, sizeof(SHORT) );

	if( m_iMaxValue < sMaxValue )
	{
		sMaxValue = static_cast<SHORT>( m_iMaxValue );
	}

	if( m_iMinValue > sMinValue )
	{
		sMinValue = static_cast<SHORT>( m_iMinValue );
	}

	if( (m_iValue + m_iModifier) * sValue < sMaxValue )
	{
		iResult = (m_iValue + m_iModifier) * sValue;
	}
	else 
	{
		iResult = sMaxValue;
	}

	if( iResult <= sMinValue )
	{
		iResult = sMinValue;
	}

	return static_cast<CHAR>( iResult );
}

LONG CValue::operator *( LONG lValue )
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult	= 0;
	LONG lMaxValue	= 0x7FFFFFFF;
	LONG lMinValue	= 0;
	memset( &lMinValue, 0xFF, sizeof(LONG) );


	if( m_iMaxValue < lMaxValue )
	{
		lMaxValue = static_cast<LONG>( m_iMaxValue );
	}

	if( m_iMinValue > lMinValue )
	{
		lMinValue = static_cast<LONG>( m_iMinValue );
	}

	if( (m_iValue + m_iModifier) * lValue < lMaxValue )
	{
		iResult = (m_iValue + m_iModifier) * lValue;
	}
	else 
	{
		iResult = lMaxValue;
	}

	if( iResult <= lMinValue )
	{
		iResult = lMinValue;
	}

	return static_cast<LONG>( iResult );
}

BYTE CValue::operator *(BYTE bValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult	= 0;
	BYTE bMaxValue	= 0xFF;
	BYTE bMinValue	= 0x0;

	if( m_iMaxValue < bMaxValue )
	{
		bMaxValue = static_cast<BYTE>( m_iMaxValue );
	}

	if( m_iMinValue > bMinValue )
	{
		bMinValue = static_cast<BYTE>( m_iMinValue );
	}

	if( (m_iValue + m_iModifier) * bValue < bMaxValue )
	{
		iResult = (m_iValue + m_iModifier) * bValue;
	}
	else 
	{
		iResult = bMaxValue;
	}

	if( iResult <= bMinValue )
	{
		iResult = bMinValue;
	}

	return static_cast<BYTE>( iResult );
}

WORD CValue::operator *(WORD wValue)
{
	INT64 iResult	= 0;
	WORD wMaxValue	= 0xFFFF;
	WORD wMinValue	= 0x0;

	if( m_iMaxValue < wMaxValue )
	{
		wMaxValue = static_cast<WORD>( m_iMaxValue );
	}

	if( m_iMinValue > wMinValue )
	{
		wMinValue = static_cast<WORD>( m_iMinValue );
	}

	if( (m_iValue + m_iModifier) * wValue < wMaxValue )
	{
		iResult = (m_iValue + m_iModifier) * wValue;
	}
	else 
	{
		iResult = wMaxValue;
	}

	if( iResult <= wMinValue )
	{
		iResult = wMinValue;
	}

	return static_cast<WORD>( iResult );
}

DWORD CValue::operator *(DWORD dwValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult		= 0;
	DWORD dwMaxValue	= 0xFFFFFFFF;
	DWORD dwMinValue	= 0x0;

	if( m_iMaxValue < dwMaxValue )
	{
		dwMaxValue = static_cast<DWORD>( m_iMaxValue );
	}

	if( m_iMinValue > dwMinValue )
	{
		dwMinValue = static_cast<DWORD>( m_iMinValue );
	}

	if( (m_iValue + m_iModifier) * dwValue < dwMaxValue )
	{
		iResult = (m_iValue + m_iModifier) * dwValue;
	}
	else 
	{
		iResult = dwMaxValue;
	}

	if( iResult <= dwMinValue )
	{
		iResult = dwMinValue;
	}

	return static_cast<DWORD>( iResult );
}

CHAR CValue::operator /(CHAR cValue)
{
	INT64 iResult	= 0;
	CHAR cMaxValue	= 0x7F;
	CHAR cMinValue	= 0;
	memset( &cMinValue, 0xFF, sizeof(CHAR) );

	if( m_iMaxValue < cMaxValue )
	{
		cMaxValue = static_cast<CHAR>( m_iMaxValue );
	}

	if( m_iMinValue > cMinValue )
	{
		cMinValue = static_cast<CHAR>( m_iMinValue );
	}

	if( (m_iValue + m_iModifier) / cValue < cMaxValue )
	{
		iResult = (m_iValue + m_iModifier) / cValue;
	}
	else 
	{
		iResult = cMaxValue;
	}

	if( iResult <= cMinValue )
	{
		iResult = cMinValue;
	}

	return static_cast<CHAR>( iResult );
}

SHORT CValue::operator /(SHORT sValue)
{
	INT64 iResult	= 0;
	SHORT sMaxValue	= 0x7FFF;
	SHORT sMinValue	= 0;
	memset( &sMinValue, 0xFFFF, sizeof(SHORT) );

	if( m_iMaxValue < sMaxValue )
	{
		sMaxValue = static_cast<SHORT>( m_iMaxValue );
	}

	if( m_iMinValue > sMinValue )
	{
		sMinValue = static_cast<SHORT>( m_iMinValue );
	}

	if( (m_iValue + m_iModifier) / sValue < sMaxValue )
	{
		iResult = (m_iValue + m_iModifier) / sValue;
	}
	else 
	{
		iResult = sMaxValue;
	}

	if( iResult <= sMinValue )
	{
		iResult = sMinValue;
	}

	return static_cast<CHAR>( iResult );
}

LONG CValue::operator /( LONG lValue )
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult	= 0;
	LONG lMaxValue	= 0x7FFFFFFF;
	LONG lMinValue	= 0;
	memset( &lMinValue, 0xFF, sizeof(LONG) );


	if( m_iMaxValue < lMaxValue )
	{
		lMaxValue = static_cast<LONG>( m_iMaxValue );
	}

	if( m_iMinValue > lMinValue )
	{
		lMinValue = static_cast<LONG>( m_iMinValue );
	}

	if( (m_iValue + m_iModifier) / lValue < lMaxValue )
	{
		iResult = (m_iValue + m_iModifier) / lValue;
	}
	else 
	{
		iResult = lMaxValue;
	}

	if( iResult <= lMinValue )
	{
		iResult = lMinValue;
	}

	return static_cast<LONG>( iResult );
}

BYTE CValue::operator /(BYTE bValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult	= 0;
	BYTE bMaxValue	= 0xFF;
	BYTE bMinValue	= 0x0;

	if( m_iMaxValue < bMaxValue )
	{
		bMaxValue = static_cast<BYTE>( m_iMaxValue );
	}

	if( m_iMinValue > bMinValue )
	{
		bMinValue = static_cast<BYTE>( m_iMinValue );
	}

	if( (m_iValue + m_iModifier) / bValue < bMaxValue )
	{
		iResult = (m_iValue + m_iModifier) / bValue;
	}
	else 
	{
		iResult = bMaxValue;
	}

	if( iResult <= bMinValue )
	{
		iResult = bMinValue;
	}

	return static_cast<BYTE>( iResult );
}

WORD CValue::operator /(WORD wValue)
{
	INT64 iResult	= 0;
	WORD wMaxValue	= 0xFFFF;
	WORD wMinValue	= 0x0;

	if( m_iMaxValue < wMaxValue )
	{
		wMaxValue = static_cast<WORD>( m_iMaxValue );
	}

	if( m_iMinValue > wMinValue )
	{
		wMinValue = static_cast<WORD>( m_iMinValue );
	}

	if( (m_iValue + m_iModifier) / wValue < wMaxValue )
	{
		iResult = (m_iValue + m_iModifier) / wValue;
	}
	else 
	{
		iResult = wMaxValue;
	}

	if( iResult <= wMinValue )
	{
		iResult = wMinValue;
	}

	return static_cast<WORD>( iResult );
}

DWORD CValue::operator /(DWORD dwValue)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	INT64 iResult		= 0;
	DWORD dwMaxValue	= 0xFFFFFFFF;
	DWORD dwMinValue	= 0x0;

	if( m_iMaxValue < dwMaxValue )
	{
		dwMaxValue = static_cast<DWORD>( m_iMaxValue );
	}

	if( m_iMinValue > dwMinValue )
	{
		dwMinValue = static_cast<DWORD>( m_iMinValue );
	}

	if( (m_iValue + m_iModifier) / dwValue < dwMaxValue )
	{
		iResult = (m_iValue + m_iModifier) / dwValue;
	}
	else 
	{
		iResult = dwMaxValue;
	}

	if( iResult <= dwMinValue )
	{
		iResult = dwMinValue;
	}

	return static_cast<DWORD>( iResult );
}