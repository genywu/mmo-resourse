//********************************************
//	杂相函数
//********************************************
#pragma once
#include <list>
#include <map>
using namespace std;

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if(x) { delete (x); (x)=NULL; }
#endif

#ifndef SAFE_DECREF
#define SAFE_DECREF(x) if(x) { (x)->DecRef(); (x)=NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) if(x) { delete[] (x); (x)=NULL; }
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(x) if(x) { free(x); (x)=NULL; }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if(x) { (x)->Release(); (x)=NULL; }
#endif

#define WCHAR unsigned short

//自释放运行
//exp: Do( m=MessageBox( NULL, _pTemp, "test", MB_OK ), Ini.ReadText("index", "test") );
//extern char *g_pTemp;
//#define Do(line, param) {g_pTemp=param; line; SAFE_FREE(g_pTemp);}

int random(int nMax);						//随机数
void Delay(long time);						//延时
int GetFileLength(const char *name);		//取文件长度
bool CheckFile(const char *name);			//判断文件是否存在
void ShowMessage(const char *msg,...);		//显示信息
void InitialDebugFileName();
void PutDebugString(const char *msg);		//输出调试信息
const char* GetDebugFileName();
void PutStringToFile(const char* filename, const char *msg);	//输出文件信息
void PutLogInfo(const char *msg);			//输出log信息
char *GetFilePath(const char *filename, char* out);	//由文件名获取文件路径
char *GetFileExtName(const char *filename, char* out);	//由文件名获取文件扩展名
WCHAR* WChar(char *str);					//char to wchar
char* Char(WCHAR* str);						//wchar to char
void DeleteDirectory(const char* strDir);	//删除目录
bool GetFileDialog(bool Load, const char* currentPath, char*, const char* strFilter="All Files\0*.*\0");	// 获取文件
bool GetPathDialog(const char* strRoot, char*);					// 获取路径
void ReplaceLine(char* str);		// 替换STR中的'\\'为'/';
bool GetString(const char* sour, char* dest, int num);	// 获取一个长字符串中的第NUM个串，空格为间隔

bool CreateDir(const char* dir); 
void FindFile(const char* strFile, const char* strExt, list<string>* plistFileName);	// 读取所有的指定扩展名的文件

inline bool IsInRect(long x, long y, RECT& rect)
{
	if( x > rect.left && x < rect.right && y > rect.top && y < rect.bottom )
	{
		return true;
	}
	return false;
}

bool FormatMoneyNumber(char *str,const int size);//格式化金钱显示,每三个之间加一个","

template<class T>
void QuickSort(T* a, int begin, int end);	// 快速排序

long GetLineDir(long sx, long sy, long dx, long dy);		// 获取从(sx,sy)到(dx,dy)的方向(0-7)


// 高精度时间函数,返回系统自开机以来的毫秒数,用RDTSC指令获取CPU自开机以来的指令周期，除以CPU频率得到
DWORD GetCurTickCount();
DWORD GetCurTime();

void IniEncoder(const char* pstrIn, char* pstrOut);		// 加密
void IniDecoder(const char* pstrIn, char* pstrOut, int len);		// 解密