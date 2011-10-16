//********************************************
//	公用
//********************************************
#pragma once
#include <windows.h>
#include <list>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
using namespace std;

#define BYTE unsigned char

class CGUID;

enum
{
	cPlayerGoodsPageID	= 1,	// 身上对话框
	cPlayerEquipPageID,			// 装备对话框
	cPlayerItemPageID,			// 快捷对话框
	cPlayerDepotPageID,			// 仓库对话框
	cNpcTradePageID,			// NPC交易对话框
};

// const int enTeamMemberCount = 9;		// 队伍一共人数

void _AddToByteArray(vector<BYTE>* pBA, void* pSource, long n);		// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, char* pStr);				// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, long l);					// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, short l);					// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, char l);					// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, float l);					// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, BYTE l);					// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, WORD l);					// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, DWORD l);					// 向CByteArray添加数据
void _AddToByteArray(vector<BYTE>* pBA, const CGUID& guid);			// 向CByteArray添加数据

bool  _GetBufferFromByteArray(BYTE* pSource, long &lPtr, CGUID& guid);				// 从CByteArray获取数据块
void* _GetBufferFromByteArray(BYTE* pSource, long &lPtr, void* pBuf, long lLen);	// 从CByteArray获取数据块
char* _GetStringFromByteArray(BYTE* pSource, long &lPtr, char* pStr);				// 从CByteArray获取字符串

// 从CByteArray获取long
inline long _GetLongFromByteArray(BYTE* pSource, long &lPtr)
{
	lPtr += sizeof(long);
	return *(long*)(&pSource[lPtr-sizeof(long)]);
}

// 从CByteArray获取short
inline short _GetShortFromByteArray(BYTE* pSource, long &lPtr)
{
	lPtr += sizeof(short);
	return *(short*)(&pSource[lPtr-sizeof(short)]);
}

// 从CByteArray获取char
inline char _GetCharFromByteArray(BYTE* pSource, long &lPtr)
{
	lPtr ++;
	return (char)pSource[lPtr-1];
}

inline float _GetFloatFromByteArray(BYTE* pSource, long &lPtr)
{
	lPtr += sizeof(float);
	return *(float*)(&pSource[lPtr-sizeof(float)]);
}

inline BYTE _GetByteFromByteArray(BYTE* pSource, long &lPtr)
{
	lPtr += sizeof(BYTE);
	return *(BYTE*)(&pSource[lPtr-sizeof(BYTE)]);
}

inline WORD _GetWordFromByteArray(BYTE* pSource, long &lPtr)
{
	lPtr += sizeof(WORD);
	return *(WORD*)(&pSource[lPtr-sizeof(WORD)]);
}

inline DWORD _GetDwordFromByteArray(BYTE* pSource, long &lPtr)
{
	lPtr += sizeof(DWORD);
	return *(DWORD*)(&pSource[lPtr-sizeof(DWORD)]);
}
