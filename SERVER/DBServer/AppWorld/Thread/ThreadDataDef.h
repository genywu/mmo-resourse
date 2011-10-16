//ThreadDataDef.h/////////////////////////////////////////////////////////////////////
//对象类:该对象定义一个从主线程投递到工作线程使用的消息对象
//Author:安海川
//Create Time:2008.11.03
#ifndef THREAD_DATA_DEF_H
#define THREAD_DATA_DEF_H
#include "../public/GUID.h"
#include "../public/MsgType.h"
#include "../public/DBDefine.h"


class CBaseEntity;

// 多线程读取处理所需信息
struct ThreadData 
{
	LONG				lThreadOpType;
	LONG				retFlag;
	CBaseEntity*		pEntity;

	ThreadData();
	~ThreadData();
	ThreadData& operator=(const ThreadData& right);
};

#endif//THREAD_DATA_DEF_H