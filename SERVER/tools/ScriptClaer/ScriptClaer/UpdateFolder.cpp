

#include "stdafx.h"
#include "UpdateFolder.h"



UpdateFolder::UpdateFolder(VOID)
{

}
UpdateFolder::~UpdateFolder(VOID)
{

}

//! 把一个目录下的所有数据读入到对象中（覆盖以前的数据）
BOOL UpdateFolder::LoadDataFromDir(LPCSTR pDirPath)
{
	if(NULL == pDirPath)
		return FALSE;

	strcpy(m_Root.szName, pDirPath);
	return  (0 < LoadDataFromDir(pDirPath, m_Root));
}
//! 把包中的数据写入到指定的目录路径
BOOL UpdateFolder::WriteDataToDir(LPCSTR pDirPath)
{
	return (0 < WriteDataToDir(pDirPath, m_Root));
}

//! 递归函数，读取一个文件夹的所有内容到传入的节点中
DWORD UpdateFolder::LoadDataFromDir(LPCSTR pDirPath, tagDirectory &Directory)
{
	DWORD dwLoadFileNum = 0;
	string strFind(pDirPath);

	WIN32_FIND_DATA FindData;
	HANDLE hFind = ::FindFirstFile((strFind + "\\*").c_str(), &FindData);
	if(INVALID_HANDLE_VALUE != hFind)
	{
		do 
		{
			if((FILE_ATTRIBUTE_DIRECTORY == FindData.dwFileAttributes))
			{
				if(strcmp(FindData.cFileName, ".") && strcmp(FindData.cFileName, ".."))
				{			
					//! 创建新节点
					Directory.listDir.push_back(tagDirectory());
					tagDirectory &NewDir = *(-- Directory.listDir.end());
					strcpy(NewDir.szName, FindData.cFileName);
					//! 递归调用
					dwLoadFileNum += LoadDataFromDir((strFind + "\\" + FindData.cFileName).c_str(), NewDir);
				}
			}
			else
			{
				Directory.listFile.push_back(CUpdateFile(FindData.cFileName));
				CUpdateFile &NewFile = *(-- Directory.listFile.end());
				if(NewFile.LoadDataFromDir((strFind + "\\" + FindData.cFileName).c_str()))
					++ dwLoadFileNum;
			}
		} 
		//! 读取不到就退出
		while(FindNextFile(hFind, &FindData));

		FindClose(hFind);
	}

	return dwLoadFileNum;
}

//! 递归函数，把一个节点的所有内容写入指定的存盘位置
DWORD UpdateFolder::WriteDataToDir(LPCSTR pDirPath, tagDirectory &Directory)
{
	DWORD dwLoadFileNum = 0;
	//! 创建当前目录
	string strCurrDir(pDirPath);
	strCurrDir.append("\\");
	MakeSureDirectoryPathExists(strCurrDir.c_str());

	//! 创建当前文件
	list<CUpdateFile>::iterator iteFile = Directory.listFile.begin();
	for (; Directory.listFile.end() != iteFile; ++iteFile)
	{
		if(iteFile->WriteDataToPath(strCurrDir.c_str()))
			++ dwLoadFileNum;
	}

	//! 递归调用创建子目录
	list<tagDirectory>::iterator iteDir = Directory.listDir.begin();
	for (; Directory.listDir.end() != iteDir; ++iteDir)
	{
		dwLoadFileNum += WriteDataToDir((strCurrDir + iteDir->szName).c_str(), *iteDir);
	}

	return dwLoadFileNum;
}


//! 通过传入的相对路径和文件名，找到一个文件
const CUpdateFile* UpdateFolder::FindFile(LPCSTR pFilename)
{
	if(NULL != pFilename)
	{
		list<string> listPath;
		if(ParsePath(pFilename, listPath))
		{
			tagDirectory *pDirectory = &m_Root;
			list<string>::const_iterator iteLast = listPath.end();
			--iteLast;
			for (list<string>::iterator ite = listPath.begin(); listPath.end() != ite; ++ite)
			{
				if(iteLast != ite)//! 找文件夹
				{
					for (list<tagDirectory>::iterator iteDir = pDirectory->listDir.begin(); pDirectory->listDir.end() != iteDir; ++iteDir)
					{
						if(0 == stricmp(ite->c_str(), iteDir->szName))
						{
							pDirectory = &(*iteDir);
							break;
						}
					}
					return NULL;
				}
				else //! 找文件
				{
					for (list<CUpdateFile>::iterator iteFile = pDirectory->listFile.begin(); pDirectory->listFile.end() != iteFile; ++iteFile)
					{
						if(0 == stricmp(ite->c_str(), iteFile->GetName()))
						{
							return &(*iteFile);
						}
					}
				}
			}
		}
	}
	
	return NULL;
}


//! 解析路径
BOOL UpdateFolder::ParsePath(LPCSTR pFilename, list<string> &listPath)
{
	LONG lBeginPos = 0, lEndPos = 0, lSize = strlen(pFilename);

	char szTmp[MAX_PATH] = {0};
	do 
	{
		++lEndPos;
		if('\\' == pFilename[lEndPos] || '/' == pFilename[lEndPos] || lSize == lEndPos)
		{
			memcpy(szTmp, &pFilename[lBeginPos], lEndPos - lBeginPos);
			szTmp[lEndPos - lBeginPos] = 0;
			listPath.push_back(string(szTmp));
			lBeginPos = lEndPos;
		}
	} while(lEndPos <= lSize);

	if(0 != listPath.size())
		return TRUE;
	return FALSE;
}