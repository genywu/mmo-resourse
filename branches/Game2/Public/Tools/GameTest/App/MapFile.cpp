//=============================================================================
/**
*  file: MapFile.cpp
*
*  Brief:通过Windows的文件影射机制，包装了简单的影射文件操作功能
*			用于异地的文件日志记录
*
*  Authtor:wangqiao
*	
*	Datae:2008-10-29
*/
//=============================================================================

#include "StdAfx.h"
#include "MapFile.h"


CMapFile::CMapFile(void)
:m_hFile(INVALID_HANDLE_VALUE)
,m_hMapObject(NULL)
,m_pszMapView(NULL)
,m_hFileAnti(INVALID_HANDLE_VALUE)
,m_hMapObjectAnti(NULL)
,m_pszMapViewAnti(NULL)
,m_eType(FMT_No)
{
}

CMapFile::~CMapFile(void)
{
}

//初始化
bool CMapFile::Initial(eFileMapServerType eFMType,const char* pszMapObjName,const char* pFileName)
{
	if(!pszMapObjName)	return false;
	m_eType = (eFileMapServerType)(m_eType|eFMType);

	char strVal[]="Anti";
	char pszMapObjNameAnti[128]="";
	char pszFileNameAnti[128]="";

	memcpy(pszMapObjNameAnti,pszMapObjName,strlen(pszMapObjName)+1);
	pszMapObjNameAnti[strlen(pszMapObjName)+1]='\0';
	strcat(pszMapObjNameAnti,strVal);



	//如果写操作
	if(m_eType&FMT_Server)
	{
		if(pFileName)
		{
			memcpy(pszFileNameAnti,pFileName,strlen(pFileName)+1);
			pszFileNameAnti[strlen(pFileName)+1]='\0';
			strcat(pszFileNameAnti,strVal);

			m_hFile = CreateFile(_TEXT(pFileName),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
				OPEN_ALWAYS,0,0);

			m_hFileAnti = CreateFile(_TEXT(pszFileNameAnti),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
				OPEN_ALWAYS,0,0);
			if(INVALID_HANDLE_VALUE == m_hFile || INVALID_HANDLE_VALUE == m_hFileAnti)
			{
				char pszErrorInfo[1024]="";
				_snprintf(pszErrorInfo,1024,"初始化MapFile时,创建文件(%s)出错.(ErrorID:%d)",pFileName,GetLastError());
				//				PutDebugString(pszErrorInfo);
				m_eType = FMT_No;
				return false;
			}
		}

		m_hMapObject = CreateFileMapping(m_hFile,NULL,
			PAGE_READWRITE, 0, MAX_MAP_VIEW_SIZE, _TEXT(pszMapObjName));

		m_hMapObjectAnti = CreateFileMapping(m_hFileAnti,NULL,
			PAGE_READWRITE, 0, MAX_MAP_VIEW_SIZE, _TEXT(pszMapObjNameAnti));
		if( !m_hMapObject || !m_hMapObjectAnti) 
		{ 
			CloseHandle(m_hFile); 
			CloseHandle(m_hFileAnti);
			m_eType = FMT_No;
			char pszErrorInfo[1024]="";
			_snprintf(pszErrorInfo,1024,"初始化MapFile时,创建影射文件(%s)出错.(ErrorID:%d)",pszMapObjName,GetLastError());
			//			PutDebugString(pszErrorInfo);
			return false;
		}
		m_pszMapView = MapViewOfFile(m_hMapObject,FILE_MAP_WRITE, 0, 0,0);
		m_pszMapViewAnti = MapViewOfFile(m_hMapObjectAnti,FILE_MAP_WRITE, 0, 0,0);
		if(!m_pszMapView || !m_pszMapViewAnti)
		{
			CloseHandle(m_hMapObject); 
			CloseHandle(m_hFile); 
			CloseHandle(m_hMapObjectAnti); 
			CloseHandle(m_hFileAnti);
			m_eType = FMT_No;
			char pszErrorInfo[1024]="";
			_snprintf(pszErrorInfo,1024,"初始化MapFile时,影射视域时(%s)出错.(ErrorID:%d)",pszMapObjName,GetLastError());
			///			PutDebugString(pszErrorInfo);
			return false;
		}

		char pszMutexObj[256]="";
		_snprintf(pszMutexObj,256,"%s_Mutex",pszMapObjName);
		m_hMutex = CreateMutex(NULL,FALSE,pszMutexObj); 

		memset(pszMutexObj,0,sizeof(pszMutexObj));
		_snprintf(pszMutexObj,256,"%s_Mutex",pszMapObjNameAnti);
		m_hMutexAnti = CreateMutex(NULL,FALSE,pszMutexObj);
		if(!m_hMutex || !m_hMutexAnti)
		{
			CloseHandle(m_hMapObject); 
			CloseHandle(m_hFile);	
			CloseHandle(m_hMapObjectAnti); 
			CloseHandle(m_hFileAnti);
			m_eType = FMT_No;
			char pszErrorInfo[1024]="";
			_snprintf(pszErrorInfo,1024,"初始化MapFile时,创建互斥对象(%s)出错.(ErrorID:%d)",pszMutexObj,GetLastError());
			//			PutDebugString(pszErrorInfo);
			return false;
		}

		//创建互斥事件句柄
		char pszEventObj[256]="";
		_snprintf(pszEventObj,256,"%s_Event",pszMapObjName);
		m_hEvent = CreateEvent(NULL,FALSE,FALSE,pszEventObj);

		memset(pszEventObj,0,sizeof(pszEventObj));
		_snprintf(pszEventObj,256,"%s_Event",pszMapObjNameAnti);
		m_hEventAnti = CreateEvent(NULL,FALSE,FALSE,pszEventObj);

		if(!m_hEvent || !m_hEventAnti)
		{
			UnmapViewOfFile(m_pszMapView);
			CloseHandle(m_hMapObject); 
			CloseHandle(m_hFile);	
			UnmapViewOfFile(m_pszMapViewAnti);
			CloseHandle(m_hMapObjectAnti); 
			CloseHandle(m_hFileAnti);
			m_eType = FMT_No;
			char pszErrorInfo[1024]="";
			_snprintf(pszErrorInfo,1024,"初始化MapFile时,创建事件对象(%s)出错.(ErrorID:%d)",pszEventObj,GetLastError());
			//			PutDebugString(pszErrorInfo);
			return false;
		}

		//赋初始读、写数据的位置值
		WaitForSingleObject(m_hMutex,INFINITE);
		WaitForSingleObject(m_hMutexAnti,INFINITE);
		//前4个字节表示开始写数据的位置
		*(long*)(m_pszMapView) = 8;
		*(long*)(m_pszMapViewAnti) = 8;
		//次后4个字节表示读数据的位置
		*(long*)((char*)m_pszMapView+4) = 8;
		*(long*)((char*)m_pszMapViewAnti+4) = 8;
		ReleaseMutex(m_hMutex);
		ReleaseMutex(m_hMutexAnti);
	}
	//如果是读操作
	else if(m_eType&FMT_Client)
	{
		m_hMapObject = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,_TEXT(pszMapObjName));
		m_hMapObjectAnti = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,_TEXT(pszMapObjNameAnti));	

		if( !m_hMapObject ||!m_hMapObjectAnti ) 
		{
			m_eType = FMT_No;
			char pszErrorInfo[1024]="";
			_snprintf(pszErrorInfo,1024,"初始化MapFile时,打开影射对象(%s)出错.(ErrorID:%d)",pszMapObjName,GetLastError());
			//			PutDebugString(pszErrorInfo);
			return false;
		}
		m_pszMapView = MapViewOfFile(m_hMapObject,FILE_MAP_ALL_ACCESS, 0, 0,0);
		m_pszMapViewAnti = MapViewOfFile(m_hMapObjectAnti,FILE_MAP_ALL_ACCESS, 0, 0,0);
		if(!m_pszMapView || !m_pszMapViewAnti)
		{
			CloseHandle(m_hMapObject); 
			CloseHandle(m_hMapObjectAnti); 
			m_eType = FMT_No;
			char pszErrorInfo[1024]="";
			_snprintf(pszErrorInfo,1024,"初始化MapFile时,影射视域时(%s)出错.(ErrorID:%d)",pszMapObjName,GetLastError());
			//			PutDebugString(pszErrorInfo);
			return false;
		}

		char pszMutexObj[256]="";
		_snprintf(pszMutexObj,256,"%s_Mutex",pszMapObjName);
		m_hMutex = OpenMutex(MUTEX_ALL_ACCESS,FALSE,pszMutexObj);

		memset(pszMutexObj,0,sizeof(pszMutexObj));
		_snprintf(pszMutexObj,256,"%s_Mutex",pszMapObjNameAnti);
		m_hMutexAnti = OpenMutex(MUTEX_ALL_ACCESS,FALSE,pszMutexObj);
		if(!m_hMutex ||!m_hMutexAnti)
		{
			CloseHandle(m_hMapObject); 
			CloseHandle(m_hMapObjectAnti); 
			m_eType = FMT_No;
			char pszErrorInfo[1024]="";
			_snprintf(pszErrorInfo,1024,"初始化MapFile时,打开互斥对象(%s)出错.(ErrorID:%d)",pszMutexObj,GetLastError());
			//			PutDebugString(pszErrorInfo);
			return false;
		}
		//创建互斥事件句柄
		char pszEventObj[256]="";
		_snprintf(pszEventObj,256,"%s_Event",pszMapObjName);
		m_hEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,pszEventObj);

		memset(pszEventObj,0,sizeof(pszEventObj));
		_snprintf(pszEventObj,256,"%s_Event",pszMapObjNameAnti);
		m_hEventAnti = OpenEvent(EVENT_ALL_ACCESS,FALSE,pszEventObj);
		if(!m_hEvent ||!m_hEventAnti)
		{
			UnmapViewOfFile(m_pszMapView);
			CloseHandle(m_hMapObject); 
			UnmapViewOfFile(m_pszMapViewAnti);
			CloseHandle(m_hMapObjectAnti); 
			m_eType = FMT_No;
			char pszErrorInfo[1024]="";
			_snprintf(pszErrorInfo,1024,"初始化MapFile时,打开事件对象(%s)出错.(ErrorID:%d)",pszEventObj,GetLastError());
			//			PutDebugString(pszErrorInfo);
			return false;
		}
	}

	return true;
}
//释放资源
void CMapFile::Release()
{
	if(m_pszMapView)
		UnmapViewOfFile(m_pszMapView);
	if(m_hMapObject)
		CloseHandle(m_hMapObject); 
	if(INVALID_HANDLE_VALUE != m_hFile)
		CloseHandle(m_hFile);

	if (m_pszMapViewAnti)
		UnmapViewOfFile(m_pszMapViewAnti);
	if (m_hMapObjectAnti)
		CloseHandle(m_hMapObjectAnti);
	if(INVALID_HANDLE_VALUE !=m_hFileAnti)
		CloseHandle(m_hFileAnti);
	return ;
}

//重新设置
bool CMapFile::Reset(eFileMapServerType eFMType,const char* pszMapObjName,const char* pFileName)
{
	//先释放
	Release();
	return Initial(eFMType,pszMapObjName,pFileName);
}

//检查空间大小是否满足
bool CMapFile::CheckViewSpace(long lNeedSize,bool bAnti)
{
	long lWritePos;
	long lReadPos;
	if (bAnti)
	{
		//写数据的位置
		lWritePos = *(long*)(m_pszMapViewAnti);
		//读数据位置
		lReadPos = *(long*)((char*)m_pszMapViewAnti+4);
	}
	else
	{
		//写数据的位置
		lWritePos = *(long*)(m_pszMapView);
		//读数据位置
		lReadPos = *(long*)((char*)m_pszMapView+4);
	}
	//超过大小
	if( lNeedSize > (MAX_MAP_VIEW_SIZE-sizeof(8)) )
		return false;
	//缓冲区满
	if( lWritePos < lReadPos && (lWritePos+lNeedSize) >= lReadPos)
		return false;
	//缓冲区满
	if( (lWritePos >=  lReadPos) && lNeedSize > (MAX_MAP_VIEW_SIZE-lWritePos+lReadPos-8) )
		return false;
	return true;
}

//返回下次写数据的位置
int CMapFile::WriteData(int nPos,const char *pData,int nLen,bool bAnti)
{
	char* pPos = NULL;
	if (bAnti)
	{
		pPos = (char*)m_pszMapViewAnti+nPos;
	}
	else
		pPos = (char*)m_pszMapView+nPos;

	if( (nPos+nLen) < MAX_MAP_VIEW_SIZE )
	{
		memcpy(pPos,pData,nLen);
		return nPos+nLen;
	}
	int nSize = MAX_MAP_VIEW_SIZE-nPos;
	memcpy(pPos,pData,nSize);
	//从头开始
	//pPos = (char*)m_pszMapView+8;
	if (bAnti)
	{
		pPos = (char*)m_pszMapViewAnti+8;
	}
	else
		pPos = (char*)m_pszMapView+8;
	memcpy(pPos,pData+nSize,nLen-nSize);
	return 8+nLen-nSize;
}
//范围下次读数据的位置
int CMapFile::ReadData(int nPos,char *pData,int nLen,bool bAnti)
{
	char *pPos=NULL;
	if (bAnti)
	{
		pPos = (char*)m_pszMapViewAnti+nPos;
	}
	else
		pPos = (char*)m_pszMapView+nPos;
	if( (nPos+nLen) < MAX_MAP_VIEW_SIZE )
	{
		memcpy(pData,pPos,nLen);
		return nPos+nLen;
	}

	int nSize = MAX_MAP_VIEW_SIZE-nPos;
	memcpy(pData,pPos,nSize);
	//从头开始
	//pPos = (char*)m_pszMapView+8;
	if (bAnti)
	{
		pPos = (char*)m_pszMapViewAnti+8;
	}
	else
		pPos = (char*)m_pszMapView+8;
	memcpy(pData+nSize,pPos,nLen-nSize);
	return 8+nLen-nSize;;
}

///对文件进行变化操作
void CMapFile::DoFile(eFileOpType type,const char* pszFileName)
{
	if(!pszFileName)	return;

	WaitForSingleObject(m_hMutex,INFINITE);
	long lFileNameLen = strnlen(pszFileName,256);
	long lNeedSize = lFileNameLen+sizeof(short)*2;

	if(!CheckViewSpace(lNeedSize))
	{
		ReleaseMutex(m_hMutex); 
		//_snprintf(pszErrorInfo,1024,"对MapFile写内容(filename:%s),空间不足",pszFileName);
		//PutDebugString(pszErrorInfo);
		return;
	}	
	//写数据的位置
	long lWritePos = *(long*)(m_pszMapView);
	//读数据位置
	long lReadPos = *(long*)((char*)m_pszMapView+4);
	//如果读数据的位置等于写数据的位置，激活等候的读进程
	if(lWritePos == lReadPos)
		SetEvent(m_hEvent);
	//写操作类型
	lWritePos = WriteData(lWritePos,(char*)&type,sizeof(short));
	//写文件名长度
	lWritePos = WriteData(lWritePos,(char*)&lFileNameLen,sizeof(short));
	//写文件名内容
	lWritePos = WriteData(lWritePos,pszFileName,lFileNameLen);
	*(long*)(m_pszMapView) = lWritePos;	
	ReleaseMutex(m_hMutex);

}

//写数据
//缓冲区的数据格式为:
//写数据位置(4byte)+读数据位置(4byte)+操作类型(2byte)+文件名长度(2byte) +文件名内容+写的数据长度(4byte)+要写的数据
void CMapFile::Write(const char* pszFileName,const char* pszText)
{
	if( !(m_eType&FMT_Client) ) return;
	if(!pszText || !pszFileName)	return;

	WaitForSingleObject(m_hMutex,INFINITE);
	long lFileNameLen = strnlen(pszFileName,256);
	long lTextLen = strnlen(pszText,1048576);
	long lNeedSize = lFileNameLen+lTextLen+sizeof(long)+sizeof(short)*2;

	if(!CheckViewSpace(lNeedSize))
	{
		ReleaseMutex(m_hMutex); 
		//_snprintf(pszErrorInfo,1024,"对MapFile写内容(filename:%s),空间不足",pszFileName);
		//PutDebugString(pszErrorInfo);
		return;
	}

	//写数据的位置
	long lWritePos = *(long*)(m_pszMapView);
	//读数据位置
	long lReadPos = *(long*)((char*)m_pszMapView+4);
	//如果读数据的位置等于写数据的位置，激活等候的读进程
	if(lWritePos == lReadPos)
		SetEvent(m_hEvent);
	//写操作类型
	eFileOpType opType = File_Write;
	lWritePos = WriteData(lWritePos,(char*)&opType,sizeof(short),false);
	//写文件名长度
	lWritePos = WriteData(lWritePos,(char*)&lFileNameLen,sizeof(short),false);
	//写文件名内容
	lWritePos = WriteData(lWritePos,pszFileName,lFileNameLen,false);

	//写内容长度
	lWritePos = WriteData(lWritePos,(char*)&lTextLen,sizeof(long),false);
	//写内容
	lWritePos = WriteData(lWritePos,pszText,lTextLen,false);	
	*(long*)(m_pszMapView) = lWritePos;	
	ReleaseMutex(m_hMutex);
	return;
}

void CMapFile::WriteAnti(const char* pszFileName,const char* pszText)
{
	if( !(m_eType&FMT_Server) ) return;
	if(!pszText || !pszFileName)	return;

	WaitForSingleObject(m_hMutexAnti,INFINITE);
	long lFileNameLen = strnlen(pszFileName,256);
	long lTextLen = strnlen(pszText,1048576);
	long lNeedSize = lFileNameLen+lTextLen+sizeof(long)+sizeof(short)*2;

	if(!CheckViewSpace(lNeedSize,true))
	{
		ReleaseMutex(m_hMutexAnti); 
		//_snprintf(pszErrorInfo,1024,"对MapFile写内容(filename:%s),空间不足",pszFileName);
		//PutDebugString(pszErrorInfo);
		return;
	}

	//写数据的位置
	long lWritePos = *(long*)(m_pszMapViewAnti);
	//读数据位置
	long lReadPos = *(long*)((char*)m_pszMapViewAnti+4);
	//如果读数据的位置等于写数据的位置，激活等候的读进程
	if(lWritePos == lReadPos)
		SetEvent(m_hEventAnti);
	//写操作类型
	eFileOpType opType = File_Write;
	lWritePos = WriteData(lWritePos,(char*)&opType,sizeof(short),true);
	//写文件名长度
	lWritePos = WriteData(lWritePos,(char*)&lFileNameLen,sizeof(short),true);
	//写文件名内容
	lWritePos = WriteData(lWritePos,pszFileName,lFileNameLen,true);

	//写内容长度
	lWritePos = WriteData(lWritePos,(char*)&lTextLen,sizeof(long),true);
	//写内容
	lWritePos = WriteData(lWritePos,pszText,lTextLen,true);	
	*(long*)(m_pszMapViewAnti) = lWritePos;	
	ReleaseMutex(m_hMutexAnti);
	return;
}
//读数据
eFileOpType CMapFile::Read(char* pszFileName,char* pszText)
{
	eFileOpType eRet = File_No;
	if( !(m_eType&FMT_Server) ) return eRet;

	WaitForSingleObject(m_hMutex,INFINITE);
	//写数据的位置
	long lWritePos = *(long*)(m_pszMapView);
	//读数据位置
	long lReadPos = *(long*)((char*)m_pszMapView+4);

	if(lWritePos == lReadPos)
	{
		ReleaseMutex(m_hMutex); 
		//等候1秒
		WaitForSingleObject(m_hEvent,200);
		WaitForSingleObject(m_hMutex,INFINITE);
		//写数据的位置
		lWritePos = *(long*)(m_pszMapView);
		//读数据位置
		lReadPos = *(long*)((char*)m_pszMapView+4);
		if(lWritePos == lReadPos)
		{
			//直接返回
			ReleaseMutex(m_hMutex); 
			return eRet;
		}
	}

	long lFileNameLen = 0;
	long lTextLen = 0;
	//读操作类型
	lReadPos = ReadData(lReadPos,(char*)&eRet,sizeof(short));
	//读文件名
	lReadPos = ReadData(lReadPos,(char*)&lFileNameLen,sizeof(short));
	if(lFileNameLen > 0)
		lReadPos = ReadData(lReadPos,pszFileName,lFileNameLen);
	pszFileName[lFileNameLen]='\0';
	if(eRet == File_Write)
	{
		//读内容
		lReadPos = ReadData(lReadPos,(char*)&lTextLen,sizeof(long));
		if(lFileNameLen > 0)
			lReadPos = ReadData(lReadPos,pszText,lTextLen);
	}
	pszText[lTextLen]='\0';
	*(long*)((char*)m_pszMapView+4) = lReadPos;
	ReleaseMutex(m_hMutex); 
	return eRet;
}

eFileOpType CMapFile::ReadAnti(char* pszFileName,char* pszText)
{
	eFileOpType eRet = File_No;
	if( !(m_eType&FMT_Client) ) return eRet;

	WaitForSingleObject(m_hMutexAnti,INFINITE);
	//写数据的位置
	long lWritePos = *(long*)(m_pszMapViewAnti);
	//读数据位置
	long lReadPos = *(long*)((char*)m_pszMapViewAnti+4);

	if(lWritePos == lReadPos)
	{
		ReleaseMutex(m_hMutexAnti); 
		//等候1秒
		WaitForSingleObject(m_hEventAnti,200);
		WaitForSingleObject(m_hMutexAnti,INFINITE);
		//写数据的位置
		lWritePos = *(long*)(m_pszMapViewAnti);
		//读数据位置
		lReadPos = *(long*)((char*)m_pszMapViewAnti+4);
		if(lWritePos == lReadPos)
		{
			//直接返回
			ReleaseMutex(m_hMutexAnti); 
			return eRet;
		}
	}

	long lFileNameLen = 0;
	long lTextLen = 0;
	//读操作类型
	lReadPos = ReadData(lReadPos,(char*)&eRet,sizeof(short),true);
	//读文件名
	lReadPos = ReadData(lReadPos,(char*)&lFileNameLen,sizeof(short),true);
	if(lFileNameLen > 0)
		lReadPos = ReadData(lReadPos,pszFileName,lFileNameLen,true);
	pszFileName[lFileNameLen]='\0';
	if(eRet == File_Write)
	{
		//读内容
		lReadPos = ReadData(lReadPos,(char*)&lTextLen,sizeof(long),true);
		if(lFileNameLen > 0)
			lReadPos = ReadData(lReadPos,pszText,lTextLen,true);
	}
	pszText[lTextLen]='\0';
	*(long*)((char*)m_pszMapViewAnti+4) = lReadPos;
	ReleaseMutex(m_hMutexAnti); 
	return eRet;
}