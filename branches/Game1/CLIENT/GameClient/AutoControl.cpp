/************************************************************
*  file: AutoControl.h
*
*  Brief:自动控制类，为客户端提供自动处理一系列事件的接口，
*		 主要用来从mapfile上读去数据，然后进行操作
*
*  Authtor:liuke
*
*  Datae:2009-01-15
*************************************************************/

#include "StdAfx.h"
#include "AutoControl.h"

AutoControl::AutoControl(void)
{
}

AutoControl::~AutoControl(void)
{
}

// 初始化
bool AutoControl::InitAutoCtl()
{
	//创建目录,如果要写文件，就放到该目录下面
	CreateDirectory("AutoControl",NULL);
	m_pMapFile = new CMapFile();
	// 初始化mapfile，服务方式，映射对象名，映射文件名
	if( !m_pMapFile->Initial(FMT_Server,"AutoControl","mapfile"))
	{
		SAFE_DELETE(m_pMapFile);
		return false;
	}
	return true;
}

// 运行
bool AutoControl::RunAutoCtl()
{
	if(m_pMapFile)
	{
		eFileOpType OpType = File_No;
		//运新Read函数时候，用来取数据的buff
		char pszFileName[MAX_FILENAME_LEN]="";
		char pszContent[MAX_CONTENT_LEN]="";
		OpType = m_pMapFile->Read(pszFileName,pszContent);
		// 有写入的内容，解析内容数据
		if(OpType == File_Write)
		{
		}
		return true;
	}
	return false;
}

// 释放
void AutoControl::ReleaseAutoCtl()
{
	if(m_pMapFile)
	{
		m_pMapFile->DoFile(End,"");
		m_pMapFile->Release();
		SAFE_DELETE(m_pMapFile);
	}
}
