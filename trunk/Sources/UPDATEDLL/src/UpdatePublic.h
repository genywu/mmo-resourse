//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   UpdatePublic.h
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-1-17 11:14:17
//  Comment     :   Define the comment interface of update system
//
//////////////////////////////////////////////////////////////////////////////////////


#ifndef __UPDATE_PUBLIC_H__
#define __UPDATE_PUBLIC_H__ 1

#include "DataDefine.h"
#include "ProcessIndex.h" 

extern CProcessIndex g_ProcessIndex;

//------------------------------------------------------------------------------------


int GetSerialNumberFromRegistry(HKEY hKey, const char cszKeyName[]);

int CheckSerialNumberValidity();

int GetInstallComponetInfo(const char cszIndexTxtFile[]);
int ProcessIndexFile(const char cszFileName[]);
int UpdateFiles();
int UpdateSelf();

int GetHostURL(const char cszFileName[], const char cszHostName[], char szHostURL[]);

///////////////////////////////////////////////////////////////////////////////
//更新包合并 Add By Fellow 2003.08.11
//一个Pack文件具有下面的结构:
//首先是四个字节的文件的头标志:字符串'PACK',然后是项的数目然后是索引开始的偏移量\数据开始的偏移量,然后是校验和,然后是保留的字节:
typedef struct {
	unsigned char signature[4];			//"PACK"
	unsigned long count;				//数据的条目数
	unsigned long index_offset;			//索引的偏移量
	unsigned long data_offset;			//数据的偏移量
	unsigned long crc32;
	unsigned char reserved[12];
} t_pack_header;

typedef struct {		//索引信息
	unsigned long id;
	unsigned long offset;
	long size;
	long compress_size;
} t_index_info;
void GetHeader(t_pack_header* aHeader, FILE* aFile);
void GetIndexInfo(t_index_info* aIndexInfo, const t_pack_header* aHeader, FILE* aFile);
bool PackComb(const char* stdFilePath,const char* addFilePath);
///////////////////////////////////////////////////////////////////////////

#endif  //__UPDATE_PUBLIC_H__

