

#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")

#include "UpdateFile.h"


#pragma once


class UpdateFolder
{
public:
	UpdateFolder(VOID);
	~UpdateFolder(VOID);

public:
	//!						把一个目录下的所有数据读入到对象中（覆盖以前的数据）
	BOOL					LoadDataFromDir(LPCSTR pDirPath);
	//!						把包中的数据写入到指定的目录路径
	BOOL					WriteDataToDir(LPCSTR pDirPath);

	//!						通过传入的相对路径和文件名，找到一个文件
	const	CUpdateFile*	FindFile(LPCSTR pFilename);

protected:
	//! 保护的定义
	/////////////////////////////////////////////////////////////////////////////////////////

	//! 目录
	struct tagDirectory
	{
		char					szName[MAX_PATH];	//文件夹名
		list<tagDirectory>		listDir;
		list<CUpdateFile>		listFile;
		tagDirectory(VOID){memset(szName, 0, MAX_PATH);}
	};

protected:
	//! 保护的函数
	/////////////////////////////////////////////////////////////////////////////////////////
	//!		递归函数，读取一个文件夹的所有内容到传入的节点中
	DWORD	LoadDataFromDir(LPCSTR pDirPath, tagDirectory &Directory);
	//!		递归函数，把一个节点的所有内容写入指定的存盘位置
	DWORD	WriteDataToDir(LPCSTR pDirPath, tagDirectory &Directory);

	//!		解析路径
	BOOL	ParsePath(LPCSTR pFilename, list<string> &listPath);

protected:
	//!				根目录
	tagDirectory	m_Root;

private:
	//! 对象析构会释放数据，因此不允许对象拷贝
	UpdateFolder(const UpdateFolder&);
	UpdateFolder& operator=(const UpdateFolder&);
};