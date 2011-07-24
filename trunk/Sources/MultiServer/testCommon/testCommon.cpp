// testCommon.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <process.h>

#pragma warning(disable:4786)

#include <iostream>

using namespace std;

#pragma message( "-------You must depend it on common.lib-------" )

#include "..\Common\Buffer.h"
#include "..\Common\Macro.h"
#include "..\Common\CriticalSection.h"

using OnlineGameLib::Win32::CCriticalSection;
using OnlineGameLib::Win32::CBuffer;

class a
{
	int a1;
	int a2;
};

class b
{
	a &aa;
	int bb;
};

int main(int argc, char* argv[])
{
	CBuffer::Allocator theAllocator( 1024*10, 1 );

	static const int nCount = 5000;
	CBuffer *pBuffer[nCount];

	getchar();
	cout << "Begin allocate" << endl;

	for ( int i=0; i<nCount; i++ )
	{
		pBuffer[i] = theAllocator.Allocate();
	}

	cout << "End allocate" << endl;

	getchar();

	cout << "Begin release" << endl;

	for ( int j=0; j<nCount; j++ )
	{
		SAFE_RELEASE( pBuffer[j] );
	}

	cout << "End release" << endl;

	getchar();

	return 0;
}
/*
void testBuffer_Packager();

typedef struct tagCS
{
	CCriticalSection cs;

}CS, NEAR *PCS, FAR *LPCS;

#define NUMBER_CS 1000

CS g_theCS[NUMBER_CS];

DWORD WINAPI ThreadFunc( LPVOID lpParam )
{
	int nBase = *(int *)( lpParam );

	int nNum = NUMBER_CS / 100;

	if ( ( nBase + nNum ) >= NUMBER_CS )
	{
		return 0;
	}

	cout << nBase << " ] " << ( nBase + nNum ) << endl;

	for ( int i=nBase; i<( nBase + nNum ); i ++ )
	{
		CCriticalSection::Owner lock( g_theCS[i].cs );

		ASSERT( g_theCS[i].cs.m_crit.OwningThread );
		ASSERT( g_theCS[i].cs.m_crit.LockCount >= 0 );
	}

    return 0; 
} 
*/
/*
int main(int argc, char* argv[])
{
	for ( int j=0; j<NUMBER_CS; j+=100 )
	{
		DWORD dwThreadId = 0;
		HANDLE handle = ::CreateThread( NULL, 0, ThreadFunc, &j, 0, &dwThreadId );
		
		for ( int i=j; i<(j+100); i++ )
		{
			CCriticalSection::Owner lock( g_theCS[i].cs );
			
			ASSERT( g_theCS[i].cs.m_crit.OwningThread );
			ASSERT( g_theCS[i].cs.m_crit.LockCount >= 0 );
			
			//cout << i << endl;
		}

		::CloseHandle( handle );
	}

	return 0;
}
*/

//
// Moves the cursor to x, y in console window
// ie x=left\right y=top\bottom
//
void move(int x, int y)
{
	COORD point;
	point.X = x; point.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), point);
}

/*int main(int argc, char* argv[])
{
	testBuffer_Packager();

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi );

	int x = csbi.dwCursorPosition.X;
	int y = csbi.dwCursorPosition.Y;

	move( 15, 15 );

	cout << "test" << endl;

	move( x, y );

	cout << "yes";

	return 0;
}*/

void testBuffer_Packager()
{
	using OnlineGameLib::Win32::CPackager;
	using OnlineGameLib::Win32::CBuffer;
	using OnlineGameLib::Win32::_tstring;

	_tstring sLargeBuffer;

	int index = 0;

	BYTE cContext = 'A';

	for ( int i=100; i<200; i+=100 )
	{
		sLargeBuffer.resize( i, cContext + index++ );
	}

	sLargeBuffer.append( "ZZZ" );

	cout << sLargeBuffer.size() << " : " << sLargeBuffer.c_str() << endl;

	CPackager theSend;
	
	CPackager theRecv;

	enum 
	{
		enumProtocolID = 30
	};

	theSend.AddData( enumProtocolID, sLargeBuffer.c_str(), sLargeBuffer.size() );

	/*
	 * Begin to split this buffer
	 */
	CBuffer *pBuffer = theSend.GetHeadPack( enumProtocolID, 10 );

	while ( pBuffer )
	{
		cout << "[Len: " << pBuffer->GetUsed() << "] ";

		/*
		 * Pack data into a received container
		 */
		CBuffer *pData = theRecv.PackUp( pBuffer->GetBuffer(), pBuffer->GetUsed() );

		/*
		 * Sample for show this data
		 */
		if ( pData )
		{
			/*
			 * {{ Begin to show infomation
			 */
			cout << "All : " << pData->GetUsed() << endl;
			
			size_t size = pData->GetUsed();

			char *pShow = new char[ size + 1 ];
			memcpy( pShow, pData->GetBuffer(), size );
			pShow[ size ] = '\0';

			cout << pData->GetUsed() << " : " << pShow << endl;

			SAFE_DELETE_ARRAY( pShow );
			/*
			 * }} Show infomation finally
			 */

			/*
			 * Your must relase this buffer
			 */
			SAFE_RELEASE( pData );
		}
		
		/*
		 * Your must relase this buffer
		 */
		SAFE_RELEASE( pBuffer );

		/*
		 * Get next package
		 */
		pBuffer = theSend.GetNextPack( enumProtocolID );
	}

	SAFE_RELEASE( pBuffer );
	
	theRecv.Empty();
	theSend.Empty();

}
