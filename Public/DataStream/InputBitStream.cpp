#include "StdAfx.h"
#include "inputbitstream.h"

InputBitStream::InputBitStream(void)
{
}

InputBitStream::~InputBitStream(void)
{
}

void InputBitStream::LoadFromFile(FILE *fp,int iSize)
{
	m_iSize = iSize;

	m_pData = new uchar[m_iSize];

	fread(m_pData,m_iSize,1,fp);

    m_iRack  = 0;
    m_byMask = 0x80;
	m_iPosition = 0;
}

int InputBitStream::InputBit()
{
	int value;

    if ( m_byMask == 0x80 ) 
	{
        m_iRack = m_pData[m_iPosition++];
        
		if ( m_iPosition > m_iSize)
		{
            printf( "Fatal error in InputBit!\n" );
			assert(0);
		}
	}

    value = m_iRack & m_byMask;
    m_byMask >>= 1;
    if ( m_byMask == 0 )
	{
		m_byMask = 0x80;
	}

    return  (value) ? 1 : 0 ;
}

unsigned int InputBitStream::InputBits(int iCount)
{
	unsigned int mask;
	unsigned int return_value;

	mask = 1L << ( iCount - 1 );
	return_value = 0;
	
	while ( mask != 0)
	{
		if ( m_byMask == 0x80 ) 
		{
			m_iRack = m_pData[m_iPosition++];
			if ( m_iPosition > m_iSize)
			{
				printf( "Fatal error in InputBit!\n" );
				assert(0);
			}
		}

		if ( m_iRack & m_byMask )
		{
			return_value |= mask;
		}

		mask >>= 1;
		m_byMask >>= 1;
		if ( m_byMask == 0 )
		{
			m_byMask = 0x80;
		}
	}
	return( return_value );
}