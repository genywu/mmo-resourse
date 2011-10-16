#include "stdafx.h"
#include "MasterInfo.h"


tagMasterInfo::tagMasterInfo()
{
	ZeroMemory( this, sizeof(tagMasterInfo) );
}

tagMasterInfo& tagMasterInfo::operator =( tagMasterInfo& tIn )
{
	memcpy( this, &tIn, sizeof(tagMasterInfo) );
	return ( *this );
}

BOOL tagMasterInfo::operator ==( tagMasterInfo& tIn )
{
	if( memcmp( this, &tIn, sizeof(tagMasterInfo) ) == 0 )
		return TRUE;
	return FALSE;
}