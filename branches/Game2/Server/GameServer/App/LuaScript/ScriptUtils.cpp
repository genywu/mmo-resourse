///
/// @file ScriptUtils.cpp
/// @brief Some util functions for scripts.
///
#include "stdafx.h"
#include "ScriptUtils.h"

namespace ScriptUtils
{
	void UpdateValToWS( const char *varName, double val, long pos )
	{
		CMessage msg( MSG_S2W_SERVER_CHANGE_SCRIPT_VARIABLE );
		msg.Add( (char*)varName );
		msg.Add( (uchar) eVarNumber ); 
		msg.Add( pos );
		msg.Add( (LONG64)( *(LONG64*) &val ) );
		msg.Send();
	}

	void UpdateValToWS( const char *varName, const tString &val, long pos )
	{
		CMessage msg( MSG_S2W_SERVER_CHANGE_SCRIPT_VARIABLE );
		msg.Add( (char*)varName );
		msg.Add( (uchar) eVarString ); 
		//msg.Add( pos ); // not implemented.
		msg.Add( val.Str() );
		msg.Send();
	}

	void UpdateValToWS( const char *varName, const CGUID &val, long pos )
	{
		CMessage msg( MSG_S2W_SERVER_CHANGE_SCRIPT_VARIABLE );
		msg.Add( (char*)varName );
		msg.Add( (uchar) eVarGuid ); 
		//msg.Add( pos ); // not implemented.
		msg.Add( val );
		msg.Send();
	}
}

