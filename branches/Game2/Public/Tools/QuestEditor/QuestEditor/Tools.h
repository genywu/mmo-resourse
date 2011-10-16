//********************************************
//	杂相函数
//********************************************
#pragma once
#include <list>
#include <map>
using namespace std;

#ifndef DELETE_SAFE
#define DELETE_SAFE(x) do{ delete (x); (x)=NULL; } while(0)
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) do{ delete (x); (x)=NULL; } while(0)
#endif

#ifndef SAFE_DECREF
#define SAFE_DECREF(x) do{ if (x) { (x)->DecRef(); (x)=NULL; } } while(0)
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) do{ delete[] (x); (x)=NULL; } while(0)
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(x) do{ free(x); (x)=NULL; } while(0)
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)	do{ if (x) { (x)->Release(); (x)=NULL; } } while(0)
#endif

#ifndef SAFE_DEL_RELEASE
#define SAFE_DEL_RELEASE(x)	do{ (x)->Release(); delete (x); (x)=NULL; } while(0)
#endif

#define WCHAR ushort

///  任务模块直接Hash到字符索引
#define QuestRet(QuestID)     QuestID/1000*100

// 用于日志输出
#define __FILE_DIR__	dirname(__FILE__)
#define __FILE_NAME__
#define DEBUGFMT  "<%s> %s(%d) : %s"
#define DEBUGARGS __FUNCTION__, __LINE__

#define PutoutLog(FILE_NAME, TYPE, FORMAT, ...) \
{ \
	char szLogType0x[32]; \
	LOG_TYPE type0x = TYPE; \
	switch( type0x ) \
{ \
	case LT_WARNING:	strcpy_s(szLogType0x, 32, "WARNING"); break; \
	case LT_ERROR:		strcpy_s(szLogType0x, 32, "ERROR"); break; \
	case LT_FATAL:		strcpy_s(szLogType0x, 32, "FATAL"); break; \
	default:			strcpy_s(szLogType0x, 32, "INFO"); break; \
} \
	char szLog0xFmt[512]; \
	sprintf_s(szLog0xFmt, 512, FORMAT,##__VA_ARGS__); \
	char szLog0x[2048]; \
	sprintf_s(szLog0x, 512, DEBUGFMT, \
	szLogType0x, DEBUGARGS, szLog0xFmt); \
	PutStringToFile(FILE_NAME, szLog0x); \
}

enum LOG_TYPE
{
	LT_DEFAULT		= 0,
	LT_WARNING		= 1,
	LT_ERROR		= 2,
	LT_FATAL		= 4
};

int random(int nMax);						//随机数
int Round(float f);							//四舍五入
int Round(double f);						//四舍五入
void Delay(long time);						//延时
int GetFileLength(const char *name);		//取文件长度
bool CheckFile(const char *name);			//判断文件是否存在
void ShowMessage(const char *msg,...);		//显示信息
void InitialDebugFileName(bool bIsTimed = false,bool bIsClear = true);	// 是否按当前时间来重新命名文件
void PutDebugString(const char *fmt, ...);	//输出调试信息
const char* GetDebugFileName();
void PutWarDebugString(const char *msg);	//输出城战调试信息
void PutStringToFile(const char* filename, const char *msg);	//输出文件信息
bool OverloadFile(const char* filename, const char *msg);		//! 更新一个文件内容
void PutLogInfo(const char *msg);			//输出log信息
char *GetFilePath(const char *filename, char* out);	//由文件名获取文件路径
char *GetFileName(const char *filename, char* out); //由文件路径名获取文件名
char *GetFileExtName(const char *filename, char* out);	//由文件名获取文件扩展名
void DeleteDirectory(const char* strDir);	//删除目录
bool GetFileDialog(bool Load, const char* currentPath, char*, const char* strFilter="All Files\0*.*\0");	// 获取文件
bool GetPathDialog(const char* strRoot, char*);					// 获取路径
void ReplaceLine(char* str);		// 替换STR中的'\\'为'/';
bool GetString(const char* sour, char* dest, int num);	// 获取一个长字符串中的第NUM个串，空格为间隔

bool CreateDir(const char* dir); 
void FindFile(const char* strFile, const char* strExt, list<string>* plistFileName);	// 读取所有的指定扩展名的文件

// 输出文件信息，并在指定的目录下保存文件,最后一个参数表示是否根据当前系统时间重新创建文件（false继续输入）
void PutStrToFileSaveToDir(const char* dir,const char* filename,const char *msg,bool IsRePut=false);

inline bool IsInRect(long x, long y, RECT& rect)
{
	if( x > rect.left && x < rect.right && y > rect.top && y < rect.bottom )
	{
		return true;
	}
	return false;
}

// 判断两个矩形是否相交
// 如源矩形的任其一点被目标矩形包含了，则相交
inline bool IsInstersect(RECT sRect, RECT dRect)
{
	//求两个矩形交集:
	//所求交集的左上角坐标,等于两个矩形的左上角坐标之中较大的一个
	//所求交集的右下角坐标,等于两个矩形的右下角坐标之中较小的一个
	long  lx,ly,rx,ry;
	//相交区域的左上角坐标
	lx = max(sRect.left,dRect.left);
	ly = max(sRect.top,dRect.top);
	//相交区域的右下角坐标
	rx = min(sRect.right,dRect.right);
	ry = min(sRect.bottom,dRect.bottom);
	//相交区域的宽和高都大于0表示相交
	return ((rx - lx > 0) && (ry - ly > 0));
}

bool FormatMoneyNumber(char *str,const int size);//格式化金钱显示,每三个之间加一个","

template<class T>
void QuickSort(T* a, int begin, int end);	// 快速排序

long Distance(long sx,long sy,long dx,long dy);
float Distance(float sx,float sy,float dx,float dy);
long GetLineDir(long sx, long sy, long dx, long dy);		// 获取从(sx,sy)到(dx,dy)的方向(0-7)
float GetRadianDir(float sx,float sy,float dx,float dy);
//通过方向得到线段上所有点
long GetLinePointByDir(list<POINT>& listPos,long lX,long lY,long lDir,long lDistance);
//得到通过目标点的所有线段上的点
long GetLinePointToDest(list<POINT>& listPos,long lX,long lY,long lDestX,long lDestY,long lDistance);
//得到一个圆里的所有点
long GetCircPoint(list<POINT>& listPos,long lCenterX,long lCenterY,long lRadius);
//得到一个正方形里的所有点
long GetSquarePoint(list<POINT>& listPos,long lCenterX,long lCenterY,long lOffLen);
//得到源与目标直线一离源最远同时离目标最近的点坐标
void GetLineBack(long &x,long &y,long lX,long lY,long lDestX,long lDestY,long lFigure);
//得到源与目标直线一离源最近同时离目标最近的坐标点
void GetLineFront(long &x,long &y,long lSrcX,long lSrcY,long lDestX,long lDestY,long lDistance);
//去掉字符串右边的空格
char* TrimRight(char* pszStr);


// 高精度时间函数,返回系统自开机以来的毫秒数,用RDTSC指令获取CPU自开机以来的指令周期，除以CPU频率得到
ulong GetCurTickCount();
ulong GetCurTime();

void IniEncoder(const char* pstrIn, char* pstrOut);		// 加密
void IniDecoder(const char* pstrIn, char* pstrOut, int len);		// 解密

#ifdef _RUNSTACKINFO_
//输出运行栈信息
void StartStackInfo();
void OutputStackInfo(const char* pszInfo);
void EmptyStackInfo();
void EndStackInfo();
#endif


// 提供给hash使用
class point_compare
{
public:
	bool operator( )( const POINT& Key1, const POINT& Key2 ) const
	{
		if(Key1.x < Key2.x)
			return true;
		else if(Key1.x == Key2.x)
			if(Key1.y < Key2.y)
				return true;
		return false;
	}

};
extern bool g_bOutPut;	// 是否输出

///
/// calculate the rest seconds in this day.
///
ulong GetRestSeconds();

///
/// Calculate the elapsed day between two time values.
///
ulong GetElapsedDay( time_t time1, time_t time2 );


/* 
*功能：生成简略字符串
*细节描述： 将源字符串 按照指定长度截断，超出部分以 省略符号表示
*参数： szString	源字符串
*		iLen		指定长度
*返回值： std::string
*添加：王峰	
*/
std::string	GetShortString( LPCTSTR szString, int iLen );


/*2010 5 10 用于程序运行时 调试*/
extern SYSTEMTIME g_startTime;     //开始运行时间
extern SYSTEMTIME g_endTime;       //结束时间
void PutDebugStrCurRunTime(DWORD  endtime);   //输出当前运行时间到调试栏


const char*  GetAwardDes( const char* type );
const char*  GetTextResDes(int  Num);