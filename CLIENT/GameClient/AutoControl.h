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

#pragma once
#include "..\public\MapFile.h"


class AutoControl
{
public:
	AutoControl(void);
	virtual ~AutoControl(void);

private:
	//对象实例
	CMapFile* m_pMapFile;

public:
	// 初始化
	bool InitAutoCtl();

	// 运行
	bool RunAutoCtl();

	// 释放
	void ReleaseAutoCtl();
};
