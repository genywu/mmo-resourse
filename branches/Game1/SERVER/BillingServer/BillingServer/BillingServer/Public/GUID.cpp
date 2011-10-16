#include "StdAfx.h"
#include ".\guid.h"


CGUID CGUID::GUID_INVALID;

void CGUID::Initialize()
{
	::CoInitialize(NULL);
}
void CGUID::Uninitialize()
{
	::CoUninitialize();
}


bool CGUID::CreateGUID(CGUID &guid)
{
	if(FAILED(CoCreateGuid((GUID*)(&guid))))
		return false;
	return true;
}