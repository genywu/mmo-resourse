#ifndef _UPDATE_FILE_H_
#define _UPDATE_FILE_H_

#pragma once

//! 内容拷贝
//#define IN_DEPTH_COPY

class CUpdateFile
{
public:
	CUpdateFile(const char *pFileName = NULL);
	~CUpdateFile();

	//! 对象拷贝
	//! 出于效率考虑，不希望在读取了文件数据后进行拷贝
	//! 因此当前的实现，如果m_pContent成员不为空，拷贝时会抛出异常，要取消这个设置，需要定义IN_DEPTH_COPY宏
	CUpdateFile(const CUpdateFile &SrcObj);
	CUpdateFile& operator=(const CUpdateFile&);

public:
	//!		把一个文件数据读入到对象中（覆盖以前的数据）
	BOOL	LoadDataFromDir(LPCSTR pFileName);
	//!		把内存中的数据写入到指定的位置路径（不指定文件名）
	BOOL	WriteDataToPath(LPCSTR pDirPath) const;

	VOID	SetName(LPCSTR pName);
	LPCSTR	GetName(VOID){return m_szName;}
	char*	GetData()	 {return m_pContent;}
	DWORD	GetDataLen() {return m_dwSize;}

private:
	char				m_szName[MAX_PATH];				//文件名
	char				*m_pContent;					//文件内容
	DWORD				m_dwSize;						//文件大小

};
#endif//_UPDATE_FILE_H_