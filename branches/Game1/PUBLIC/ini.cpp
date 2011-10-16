//********************************************
//	Ini 相关函数
//********************************************

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tools.h"
#include "ini.h"
#include "RFile.h"

#ifdef	_CLIENT
#include "..\public\RFile.h"
#endif



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//初始化
CIni::CIni()
{
	m_lDataLen = 0;
	m_pData = NULL;
	m_lIndexNum=0;
	m_pIndexList = NULL;
}

//初始化
CIni::CIni(const char* filename, const char* pData)
{
	m_lDataLen = 0;
	m_pData = NULL;
	m_lIndexNum = 0;
	m_pIndexList = NULL;
	Open(filename, pData);
}

//析构释放
CIni::~CIni()
{
	if( m_lDataLen != 0 )
	{
		//SAFE_DELETE( m_pData );
		SAFE_DELETE_ARRAY( m_pData );
		m_lDataLen = 0;
	}

	if( m_lIndexNum != 0 )
	{
		SAFE_DELETE( m_pIndexList );
		m_lIndexNum = 0;
	}
}

//读入文件
bool CIni::Open(const char* filename, const char* pData)
{

//#ifdef _CLIENT
	if (filename)
		strcpy(m_strFileName, filename);
	SAFE_DELETE_ARRAY( m_pData );

	// 内存复制
	if (pData)
	{
		m_lDataLen=lstrlen(pData);	//获取文件长度

		m_pData = new char[m_lDataLen+1];

		strcpy(m_pData, pData);		//读数据

		//初始化索引
		InitIndex();
		return true;
	}
	else	// 读取文件
	{

		CRFile* prfile = rfOpen(filename);		
		//文件存在
		if( prfile)
		{
			m_lDataLen=prfile->GetDatalen();
			if(m_lDataLen>0)
			{
				m_pData = new char[m_lDataLen];
				prfile->ReadData(m_pData, m_lDataLen);
			}
			
			rfClose(prfile);
			//初始化索引
			InitIndex();
			return true;
		}
		else
		{
			m_lDataLen=GetFileLength(filename);	//获取文件长度
			//文件存在
			if(  m_lDataLen > 0 )
			{
				m_pData = new char[m_lDataLen];

				FILE *fp;
				fp=fopen(filename, "rb");
				assert( fp!=NULL );
				fread(m_pData, m_lDataLen, 1, fp);		//读数据
				fclose(fp);

				//初始化索引
				InitIndex();
				return true;
			}
			else	// 文件不存在
			{
				// 找不到文件
				m_lDataLen=1;
				m_pData = new char[m_lDataLen];
				memset(m_pData, 0, 1);
				InitIndex();
			}
		}
	}

	return false;


//#else
//
//	if (filename)
//		strcpy(m_strFileName, filename);
//	SAFE_DELETE_ARRAY( m_pData );
//
//	// 内存复制
//	if (pData)
//	{
//		m_lDataLen=lstrlen(pData);	//获取文件长度
//
//		m_pData = new char[m_lDataLen+1];
//
//		strcpy(m_pData, pData);		//读数据
//
//		//初始化索引
//		InitIndex();
//		return true;
//	}
//	else	// 读取文件
//	{
//
//			m_lDataLen=GetFileLength(filename);	//获取文件长度
//
//			//文件存在
//			if(  m_lDataLen > 0 )
//			{
//				m_pData = new char[m_lDataLen];
//
//
//				FILE *fp;
//				fp=fopen(filename, "rb");
//				assert( fp!=NULL );
//				fread(m_pData, m_lDataLen, 1, fp);		//读数据
//				fclose(fp);
//
//				//初始化索引
//				InitIndex();
//				return true;
//			}
//			else	// 文件不存在
//			{
//				// 找不到文件
//				m_lDataLen=1;
//				m_pData = new char[m_lDataLen];
//				memset(m_pData, 0, 1);
//				InitIndex();
//			}
//	}
//
//	return false;
//
//#endif
}

//关闭文件
void CIni::Close()
{
	if( m_lDataLen != 0 )
	{
		SAFE_DELETE( m_pData );
		m_lDataLen = 0;
	}

	if( m_lIndexNum != 0 )
	{
		SAFE_DELETE( m_pIndexList );
		m_lIndexNum = 0;
	}
}

//写入文件
bool CIni::Save(const char *filename)
{
	if( filename==NULL )
	{
		filename=m_strFileName;
	}

	FILE *fp;
	fp=fopen(filename, "wb");
	if( fp )
	{
		//fprintf(fp,"%s",m_pData);
		fwrite(m_pData, m_lDataLen, 1, fp);
		fclose(fp);
	}
	else
	{
		assert( fp!=NULL );
		return false;
	}

	return true;
}

//计算出所有的索引位置
void CIni::InitIndex()
{
	m_lIndexNum=0;

	int i = 0;
	for(; i<m_lDataLen; i++)
	{
		//找到
		if( m_pData[i]=='[' && (m_pData[i-1]=='\n' || i==0) )
		{
			m_lIndexNum++;
		}
	}

	//申请内存
	SAFE_DELETE( m_pIndexList );
	if( m_lIndexNum>0 )
		m_pIndexList=new int[m_lIndexNum];

	int n=0;

	for(i=0; i<m_lDataLen; i++)
	{
		if( m_pData[i]=='[' && (m_pData[i-1]=='\n' || i==0) )
		{
			m_pIndexList[n]=i+1;
			n++;
		}
	}
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

//返回指定标题位置
int CIni::FindIndex(char *string)
{
	for(int i=0; i<m_lIndexNum; i++)
	{
		char *str=ReadText( m_pIndexList[i] );
		if( strcmp(string, str) == 0 )
		{
			SAFE_DELETE_ARRAY( str );
			return m_pIndexList[i];
		}
		SAFE_DELETE_ARRAY( str );
	}
	return -1;
}

//返回第几个标题
int CIni::FindIndexPos(char *string)
{
	for(int i=0; i<m_lIndexNum; i++)
	{
		char *str=ReadText( m_pIndexList[i] );
		if( strcmp(string, str) == 0 )
		{
			SAFE_DELETE_ARRAY( str );
			return i;
		}
		SAFE_DELETE_ARRAY( str );
	}
	return -1;
}

//返回第几个标题位置
int CIni::FindIndex(int nPos)
{
	if (nPos>=0 && nPos<m_lIndexNum)
	{
		return m_pIndexList[nPos];
	}
	return -1;
}

// 返回下一个标题位置
int CIni::FindNextIndex(int nPos)
{
	for(int i=0; i<m_lIndexNum; i++)
	{
		if (m_pIndexList[i] > nPos)
			return m_pIndexList[i];
	}
	return -1;
}

//返回指定数据的位置
int CIni::FindData(int index, char *string)
{
	int p=index;	//指针

	int endindex = FindNextIndex(index);
	if (endindex==-1)
		endindex = m_lDataLen;

	while(1)
	{
		p=GotoNextLine(p);
		char *name=ReadDataName(p);
		if( strcmp(string, name)==0 )
		{
			SAFE_DELETE_ARRAY( name );
			return p;
		}

		SAFE_DELETE_ARRAY( name );
		if( p>=endindex) return -1;
	}
	return -1;
}

//提行
int CIni::GotoNextLine(int p)
{
	int i = p;
	for(; i<m_lDataLen; i++)
	{
		if( m_pData[i]=='\n' )
			return i+1;
	}
	return i;
}

//在指定位置读一数据名称
char *CIni::ReadDataName(int &p)
{
	char chr;
	char *Ret;
	int m=0;

	Ret=new char[64];
	memset(Ret, 0, 64);

	for(int i=p; i<m_lDataLen; i++)
	{
		chr=m_pData[i];

		//结束
		if( chr == '\r' )
		{
			p=i+1;
			return Ret;
		}
		
		//结束
		if( chr == '=' || chr == ';' )
		{
			p=i+1;
			return Ret;
		}
		
		Ret[m]=chr;
		m++;
	}
	return Ret;
}

//在指定位置读一字符串
char *CIni::ReadText(int p)
{
	char chr;
	char *Ret;
	int n=p, m=0;

	int LineNum = GotoNextLine(p) - p + 1;
	Ret=new char[LineNum];
	memset(Ret, 0, LineNum);

	for(int i=0; i<m_lDataLen-p; i++)
	{
		chr=m_pData[n];

		//结束
		if( chr == ';' || chr == '\n' || chr == '\r' || chr == '\t' || chr == ']' )
		{
			//ShowMessage(Ret);
			return Ret;
		}
		
		Ret[m]=chr;
		m++;
		n++;
	}

	return Ret;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

//加入一个索引
bool CIni::AddIndex(char *index)
{
	char str[256];
	int n=FindIndex(index);

	if( n == -1 )	//新建索引
	{
		sprintf(str,"\r\n[%s]\r\n",index);

		// 从新分配内存
//		char* pTemp = new char[m_lDataLen+strlen(str)+1];
//		memcpy(pTemp, m_pData, m_lDataLen);
//		delete( m_pData );
//		m_pData = pTemp;

		m_pData = (char *)realloc(m_pData, m_lDataLen+strlen(str)+1);	//重新分配内存
		sprintf(&m_pData[m_lDataLen], "%s", str);
		m_lDataLen+=lstrlen(str);

		InitIndex();
		return true;
	}
	
	return false;	//已经存在
}

//在当前位置加入一个数据
bool CIni::AddData(int p, char *name, char *string)
{
	char *str;
	int len = lstrlen(string);
	str=new char[len+256];
	sprintf(str,"%s=%s\r\n",name,string);
	len=lstrlen(str);

	p=GotoNextLine(p);	//提行
	m_pData = (char *)realloc(m_pData, m_lDataLen+len+1);	//重新分配内存

	char *temp=new char[m_lDataLen-p];
	memcpy(temp, &m_pData[p], m_lDataLen-p);
	memcpy(&m_pData[p+len], temp, m_lDataLen-p);	//把后面的搬到末尾
	memcpy(&m_pData[p], str, len);
	m_lDataLen+=len;

	SAFE_DELETE( temp );
	SAFE_DELETE( str );
	return true;
}

//在当前位置修改一个数据的值
bool CIni::ModityData(int p, char *name, char *string)
{
	int n=FindData(p, name);

	char *t=ReadText(n);
	p=n+lstrlen(t);
	if( lstrlen(t)>0 ) free(t);

	int newlen=lstrlen(string);
	int oldlen=p-n;

	m_pData = (char *)realloc(m_pData, m_lDataLen+newlen-oldlen+1);	//重新分配内存

	char *temp=new char[m_lDataLen-p];
	memcpy(temp, &m_pData[p], m_lDataLen-p);
	memcpy(&m_pData[n+newlen], temp, m_lDataLen-p);			//把后面的搬到末尾
	memcpy(&m_pData[n], string, newlen);
	m_lDataLen+=newlen-oldlen;

	SAFE_DELETE( temp );
	return true;
}

//返回文件内容
char *CIni::GetData()
{
	return m_pData;
}

//获得文件的行数
int CIni::GetLines(int cur)
{
	int n=1;
	for(int i=0; i<cur; i++)
	{
		if( m_pData[i]=='\n' )
			n++;
	}
	return n;
}

/////////////////////////////////////////////////////////////////////
// 对外接口
/////////////////////////////////////////////////////////////////////

//以普通方式读一字符串数据
char *CIni::ReadText(char *index, char *name)
{
	int n=FindIndex(index);
	assert( n != -1 );

	int m=FindData(n, name);
	if( m == -1 )
	{
		char* temp = new char[1];
		temp[0] = 0;
		return temp;
	}

	return ReadText(m);
}
	
//在指定的行读一字符串
char *CIni::ReadText(char *index, int lines)
{
	int n=FindIndex(index);
	assert( n != -1 );

	//跳到指定行数
	n=GotoNextLine(n);
	for(int i=0; i<lines; i++)
	{
		if( n<m_lDataLen )
			n=GotoNextLine(n);
	}

	//读数据
	while( n<=m_lDataLen )
	{
		if( m_pData[n] == '=' )
		{
			n++;
			return ReadText(n);
		}
		if( m_pData[n] == '\r' )
		{
			return "";
		}
		n++;
	}

	return "";
}

//以普通方式读一整数
int CIni::ReadInt(char *index, char *name)
{
	int n=FindIndex(index);
	assert( n != -1 );

	int m=FindData(n, name);
	//assert( m != -1 );
	if( m == -1 )
	{
		return ERROR_DATA;
	}

	char *str=ReadText(m);
	int ret=atoi(str);
	free(str);
	return ret;
}

//在指定的行读一整数
int CIni::ReadInt(char *index, int lines)
{
	int n=FindIndex(index);
	assert( n != -1 );

	//跳到指定行数
	n=GotoNextLine(n);
	for(int i=0; i<lines; i++)
	{
		if( n<m_lDataLen )
			n=GotoNextLine(n);
	}

	//读数据
	while( n<m_lDataLen )
	{
		if( m_pData[n] == '=' )
		{
			n++;
			char *str=ReadText(n);
			int ret=atoi(str);
			free(str);
			return ret;
		}
		if( m_pData[n] == '\r' )
		{
			return ERROR_DATA;
		}
		n++;
	}

	return ERROR_DATA;
}

//在指定的行读一数据名称
char *CIni::ReadCaption(char *index, int lines)
{
	int n=FindIndex(index);
	assert( n != -1 );

	//跳到指定行数
	n=GotoNextLine(n);
	for(int i=0; i<lines; i++)
	{
		if( n<m_lDataLen )
			n=GotoNextLine(n);
	}

	return ReadDataName(n);
}

//以普通方式写一字符串数据
bool CIni::Write(char *index, char *name, char *string)
{
	int n=FindIndex(index);
	if( n == -1 )	//新建索引
	{
		AddIndex(index);
		n=FindIndex(index);
		n=GotoLastLine(index);
		AddData(n, name, string);	//在当前位置n加一个数据
		return true;
	}

	//存在索引
	int m=FindData(n, name);
	if( m==-1 )		//新建数据
	{
		n=GotoLastLine(index);
		AddData(n, name, string);	//在当前位置n加一个数据
		return true;
	}

	//存在数据
	ModityData(n, name, string);	//修改一个数据

	return true;
}

//以普通方式写一整数
bool CIni::Write(char *index, char *name, int num)
{
	char string[64];
	sprintf(string, "%d", num);

	int n=FindIndex(index);
	if( n == -1 )	//新建索引
	{
		AddIndex(index);
		n=FindIndex(index);
		n=GotoLastLine(index);
		AddData(n, name, string);	//在当前位置n加一个数据
		return true;
	}

	//存在索引
	int m=FindData(n, name);
	if( m==-1 )		//新建数据
	{
		n=GotoLastLine(index);
		AddData(n, name, string);	//在当前位置n加一个数据
		return true;
	}

	//存在数据
	ModityData(n, name, string);	//修改一个数据

	return true;
}

//返回连续的行数
int CIni::GetContinueDataNum(char *index)
{
	int num=0;
	int n=FindIndex(index);
	n=GotoNextLine(n);
	while(1)
	{
		if( n >= m_lDataLen || m_pData[n] == '\r' || m_pData[n] == EOF || m_pData[n] == ' ' || m_pData[n] == '/' || m_pData[n] == '\t' || m_pData[n] == '\n' )
		{
			return num;
		}
		else
		{
			num++;
			n=GotoNextLine(n);
			if( n >= m_lDataLen ) return num;
		}
	}
}

//把指针移动到本INDEX的最后一行
int CIni::GotoLastLine(char *index)
{
	int n=FindIndex(index);
	n=GotoNextLine(n);
	while(1)
	{
		if( m_pData[n] == '\r' || m_pData[n] == EOF || m_pData[n] == -3 || m_pData[n] == ' ' || m_pData[n] == '/' || m_pData[n] == '\t' || m_pData[n] == '\n' )
		{
			return n;
		}
		else
		{
			n=GotoNextLine(n);
			if( n >= m_lDataLen ) return n;
		}
	}
}
