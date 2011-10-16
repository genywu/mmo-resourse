#include "StdAfx.h"
#include "GUID.h"


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
bool CGUID::CopyConstrucutureCalFunc(const char* szValue, long& retValue)
{
	if(!szValue)return false;
	if(*szValue >= '0' && *szValue <= '9')
	{
		retValue = *szValue-0x30;
		return true;
	}
	else if(*szValue >= 'A' && *szValue <= 'F')
	{
		retValue = *szValue-0x41+10;
		return true;
	}
	else if(*szValue >= 'a' && *szValue <= 'f')
	{
		retValue = *szValue-0x61+10;
		return true;
	}
	return false;
}
CGUID::CGUID(const char* strGuid)
{
	memset(this, 0, sizeof(CGUID));
	if(strGuid != NULL && strlen(strGuid) == 38)
	{
		char strtempt[37] = "";
		memcpy(strtempt,strGuid+1,36);
		long curPos = 0;
		// data1
		for(int i=0; i<8; i++)
		{
			long value = strtempt[i];
			if(!CopyConstrucutureCalFunc(&strtempt[i], value))
			{
				memset(this, 0, sizeof(CGUID));
				return;
			}
			value = value << (28-(4*i));
			Data1 |= value;
		}
		curPos += 9;

		// data2
		for(int i=curPos; i<(curPos+4); i++)
		{
			long value = strtempt[i];
			if(!CopyConstrucutureCalFunc(&strtempt[i], value))
			{
				memset(this, 0, sizeof(CGUID));
				return;
			}
			value = value << (12-(4*(i-curPos)));
			Data2 |= value;
		}
		curPos += 5;

		// data3
		for(int i=curPos; i<(curPos+4); i++)
		{
			long value = strtempt[i];
			if(!CopyConstrucutureCalFunc(&strtempt[i], value))
			{
				memset(this, 0, sizeof(CGUID));
				return;
			}
			value = value << (12-(4*(i-curPos)));
			Data3 |= value;
		}
		curPos += 5;

		// data4 --- 1
		long tmpData4Pos = 0;
		for(int i=curPos; i<(curPos+4); i++)
		{
			// 高位
			long value = strtempt[i];
			if(!CopyConstrucutureCalFunc(&strtempt[i], value))
			{
				memset(this, 0, sizeof(CGUID));
				return;
			}
			Data4[tmpData4Pos] |= value << 4;
			i++;

			// 低位
			value = strtempt[i];
			if(!CopyConstrucutureCalFunc(&strtempt[i], value))
			{
				memset(this, 0, sizeof(CGUID));
				return;
			}
			Data4[tmpData4Pos] |= value;
			tmpData4Pos++;
		}
		curPos += 5;

		// data4 --- 2
		for(int i=curPos; i<(curPos+12); i++)
		{
			// 高位
			long value = strtempt[i];
			if(!CopyConstrucutureCalFunc(&strtempt[i], value))
			{
				memset(this, 0, sizeof(CGUID));
				return;
			}
			Data4[tmpData4Pos] |= value << 4;
			i++;

			// 低位
			value = strtempt[i];
			if(!CopyConstrucutureCalFunc(&strtempt[i], value))
			{
				memset(this, 0, sizeof(CGUID));
				return;
			}
			Data4[tmpData4Pos] |= value;
			tmpData4Pos++;
		}
	}
}

const char *CGUID::tostring() const
{
	static char s[48];
	tostring( s );
	return s;
}
