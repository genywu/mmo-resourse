//////////////////////////////////////////////////////////////////////////
//RFile.h,资源文件类
//Fun:资源文件类句柄,负责读写操作
//Author:Joe
//Create Time:2004.11.26
//////////////////////////////////////////////////////////////////////////
#pragma once

class CClientResource;
#include "../DefType.h"
class CRFile
{
public:
	CRFile(const char* pszFileName,uchar* pData,ulong dwSize);
	CRFile(const char* pszFileName,FILE* pfile);
	~CRFile(void);
private:
	uchar*		m_pData;			//数据
	ulong		m_dwSize;
	ulong		m_dwPos;

	//文件名
	char m_pszFileName[MAX_PATH];

public:
	const char* GetFileName()	{return m_pszFileName;}
	ulong GetPosition()	{return m_dwPos;}
	void SetPosition(ulong dwPosition) { m_dwPos = dwPosition;}

	uchar* GetData()	{return m_pData;}
	long GetDatalen()	{return m_dwSize;}					//返回数据长度
	bool ReadData(void* pBuf,ulong dwSize);					//读数据到缓冲区
	bool ReadData(int& nValue);								//读ini类型数据
	bool ReadData(ulong& nValue);							//读ulong类型数据
	bool ReadChar(uchar &c);
	char * ReadLine(char * pszLineBuffer, int iBufferLen);
	bool ReadToStream(stringstream& stream);				//读数据到stringstream流

	ulong  Seek(int iPosition,int iOrigin = 0);

	static char *GetFilePath(const char *szFileName, char* szOutput);//不带"/" 和 "\\"
};

////打开资源文件的全局函数
//CRFile* rfOpen(const char* strFileName, CClientResource* pClientResource=NULL);	//打开文件函数,调用rfClose关闭
//void rfClose(CRFile* rfile);						//关闭CRFile文件句柄

//使用合理的文件路径规格
void CheckRFileStr(string& strFileName);


// 缺省的客户端资源
extern CClientResource* g_pDefaultClientResource;
CClientResource* GetDefaultClientResource();
void SetDefaultClientResource(CClientResource* p);
