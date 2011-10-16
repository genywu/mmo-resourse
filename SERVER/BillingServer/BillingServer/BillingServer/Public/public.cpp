//********************************************
//	公用
//********************************************

#include "stdafx.h"
#include "public.h"
#include "guid.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, void* pSource, long n)
{
	for(int i=0; i<n; i++)
	{
		pBA->push_back(((BYTE*)pSource)[i]);
	}
}
	
// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, char* pStr)
{
	long size = lstrlen(pStr);
	for(int i=0; i<size; i++)
	{
		pBA->push_back(pStr[i]);
	}	

	pBA->push_back(0);
}

// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, long l)
{
	for(int i=0; i<sizeof(long); i++)
	{
		pBA->push_back( ((BYTE*)(&l))[i] );
	}
}

// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, short l)
{
	for(int i=0; i<sizeof(short); i++)
	{
		pBA->push_back( ((BYTE*)(&l))[i] );
	}
}

// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, char l)
{
	pBA->push_back(l);
}

// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, float l)
{
	for(int i=0; i<sizeof(float); i++)
	{
		pBA->push_back( ((BYTE*)(&l))[i] );
	}
}

// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, BYTE l)
{
	pBA->push_back(l);
}

// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, WORD l)
{
	for(int i=0; i<sizeof(WORD); i++)
	{
		pBA->push_back( ((BYTE*)(&l))[i] );
	}
}

// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, DWORD l)
{
	for(int i=0; i<sizeof(DWORD); i++)
	{
		pBA->push_back( ((BYTE*)(&l))[i] );
	}
}

// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, const CGUID& guid)
{
	long size = sizeof(CGUID);
	if(guid == CGUID::GUID_INVALID)
	{
		size = 0;
	}
	_AddToByteArray(pBA,(BYTE)size);
	_AddToByteArray(pBA,(void*)&guid,size);
}

// 从CByteArray获取数据块
bool _GetBufferFromByteArray(BYTE* pSource, long &lPtr, CGUID& guid)
{
	long size = 0;
	_GetBufferFromByteArray(pSource,lPtr,&size,1);
	if(size == 0)
	{
		guid = CGUID::GUID_INVALID;
		return false;
	}
	else
	{
		_GetBufferFromByteArray(pSource,lPtr,&guid,sizeof(CGUID));
		return true;
	}
	return true;
}

// 从CByteArray获取数据块
void* _GetBufferFromByteArray(BYTE* pSource, long &lPtr, void* pBuf, long lLen)
{
	// bugs 20080506 优化
	::memcpy(pBuf, pSource+lPtr, lLen);
	lPtr += lLen;
	return pBuf;
}

// 从CByteArray获取字符串
char* _GetStringFromByteArray(BYTE* pSource, long& lPtr, char* pStr)
{
	char c = 0;
	long n = 0;

	do
	{
		c = pSource[ lPtr++ ];
		pStr[n++] = c;
	}
	while( c!=0 );

	return pStr;
}

