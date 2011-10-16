#include "StdAfx.h"
#include "OutputBitStream.h"

OutputBitStream::OutputBitStream(void)
{
	m_iStreamPosition = 0;
}

OutputBitStream::~OutputBitStream(void)
{
}

void OutputBitStream::OutputBit(int iBit)
{
	if ( iBit )
		m_iRack |= m_byMask;

    m_byMask = m_byMask >> 1;

    if ( m_byMask == 0 ) 
	{
		m_byStreamArray.push_back(m_iRack);

		if ( (m_iOutputDotCount++ & OUTPUT_DOT_COUNT ) == 0 )
		{
			printf(".");//打印进度
		}

		m_iRack = 0;
		m_byMask= 0x80;//1000 0000
    }

}

void OutputBitStream::OutputBits(unsigned int dwCode,int iCount)
{
    unsigned int mask;

    mask = 1L << ( iCount - 1 );
    
	while ( mask != 0)
	{
		if ( mask & dwCode )
			m_iRack |= m_byMask;

		m_byMask = m_byMask >> 1;

		if ( m_byMask == 0 ) 
		{
			m_byStreamArray.push_back(m_iRack);

			if ( (m_iOutputDotCount++ & OUTPUT_DOT_COUNT ) == 0 )
			{
				printf(".");//打印进度
			}

			m_iRack = 0;
			m_byMask= 0x80;
		}

        mask >>= 1;
    }
}
	


void OutputBitStream::CloseStream(void)
{
	if (m_byMask != 0x80 )
	{
		m_byStreamArray.push_back(m_iRack);
	}
}

void OutputBitStream::OpenStream(void)
{
	m_iRack = 0;
	m_byMask= 0x80;//1000 0000
}
