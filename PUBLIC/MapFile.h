//=============================================================================
/**
 *  file: MapFile.h
 *
 *  Brief:通过Windows的文件影射机制，包装了简单的影射文件操作功能
 *			用于文件日志记录
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2008-10-29
 */
//=============================================================================

#ifndef __MAP_FILE_H__
#define __MAP_FILE_H__
#pragma once


const long MAX_FILENAME_LEN = 256;
const long MAX_CONTENT_LEN = 1024*10;
const long MAX_MAP_VIEW_SIZE = 1048576*10;

//影射文件的打开方式
enum eFileMapServerType
{
	FMT_No = 0,
	FMT_Client=0x00000001,//客户方式
	FMT_Server=0x00000002,//服务方式
};


//消息中文件的操作方式
enum eFileOpType
{
	File_No=0,
	File_Open=1,//打开文件
	File_Write=2,//写文件
	File_Clear=3,//清空文件
	File_Close=4,//关闭文件
	End=0xFF,//结束处理,退出读数据线程
};


class CMapFile
{
public:
	CMapFile(void);
	~CMapFile(void);

private:
	//影射对象类型
	eFileMapServerType m_eType;
	//影射到地址空间的文件句柄
	HANDLE	m_hFile;
	//影射文件句柄
	HANDLE	m_hMapObject;
	//内存影射文件的地址空间
	LPVOID m_pszMapView;
	//用于读写的互斥体
	HANDLE m_hMutex;
	//互斥的事件句柄
	HANDLE m_hEvent;

	//封装了读写数据的功能
private:
	//检查空间大小是否满足
	bool CheckViewSpace(long lNeedSize);
	//返回下次写数据的位置
	int WriteData(int nPos,const char *pData,int nLen);
	//范围下次读数据的位置
	int ReadData(int nPos,char *pData,int nLen);	
public:
	//初始化
	bool Initial(eFileMapServerType eFMType,const char* pszMapObjName,const char* pFileName=NULL);
	//重新设置
	bool Reset(eFileMapServerType eFMType,const char* pszMapObjName,const char* pFileName=NULL);
	//释放资源
	void Release();
	//对文件进行变化操作
	void DoFile(eFileOpType type,const char* pszFileName);
	//打开文件
	inline void OpenFile(const char* pszFileName){	DoFile(File_Open,pszFileName);}
	//关闭文件
	inline void CloseFile(const char* pszFileName){DoFile(File_Close,pszFileName);}
	//清空文件
	inline void ClearFile(const char* pszFileName){DoFile(File_Clear,pszFileName);}
	//写数据
	void Write(const char* pszFileName,const char* pszText);
	//读数据
	eFileOpType Read(char* pszFileName,char* pszText);
};

#endif
