// testIOCPServer.cpp : Defines the entry point for the console application.
//

#include <winsock2.h>

#include "IOCPServer\Exception.h"
#include "IOCPServer\Utils.h"
#include "IOCPServer\ManualResetEvent.h"

#include "GameServer.h"

using OnlineGameLib::Win32::_tstring;
using OnlineGameLib::Win32::CException;
using OnlineGameLib::Win32::Output;
using OnlineGameLib::Win32::CManualResetEvent;

int main(int argc, char* argv[])
{
	try
	{
		CGameServer server(
			"Welcome to jx.online server! What are you doing now?",
			10,                           // max number of sockets to keep in the pool
			10,                           // max number of buffers to keep in the pool
			1024);                        // buffer size 

		server.Open( 
			INADDR_ANY,                   // address to listen on
			5001                          // port to listen on
			);
	
		server.StartAcceptingConnections();
	
		CManualResetEvent shutdownEvent(_T("OnlineGameServerShutdown"), false);
		CManualResetEvent pauseResumeEvent(_T("OnlineGameServerPauseResume"), false);
		
		HANDLE handlesToWaitFor[2];
		
		handlesToWaitFor[0] = shutdownEvent.GetEvent();
		handlesToWaitFor[1] = pauseResumeEvent.GetEvent();
		
		bool accepting = true;
		bool done = false;
		
		while (!done)
		{
			DWORD waitResult = ::WaitForMultipleObjects(2, handlesToWaitFor, false, INFINITE);

			if (waitResult == WAIT_OBJECT_0)
			{
				done = true;
			}
			else if (waitResult == WAIT_OBJECT_0 + 1)
			{
				if (accepting)
				{
					server.StopAcceptingConnections();
				}
				else
				{
					server.StartAcceptingConnections();
				}
				
				accepting = !accepting;
			}
			else
			{
				Output(_T("Unexpected result from WaitForMultipleObjects - exiting"));
				done = true;
			}
		}

		server.WaitForShutdownToComplete();
	}
	catch(const CException &e)
	{
		Output(_T("Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage());
	}
	catch(...)
	{
		Output(_T("Unexpected exception"));
	}
	
	return 0;
}

