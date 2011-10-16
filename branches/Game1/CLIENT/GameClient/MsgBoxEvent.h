#pragma once
#include <vector>
#include <list>

using namespace std;

//////////////////////////////////////////////////////////////////////////
//MsgBoxEvent.h
//Func:MsgBox运行的事件类
//Author:joe
//Create Time:2005.2.2
//////////////////////////////////////////////////////////////////////////

enum eMsgBoxEvent
{
	MBE_DIEDSELECT,				//死亡之后选择的复活事件
};

class CMsgBoxEvent
{
public:
	struct tagMsgBoxEvent
	{
		eMsgBoxEvent			eEvent;		//时间类型
		vector<LONG_PTR>		Params;		//参数列表
	};
	list<tagMsgBoxEvent*>::iterator itEvent;

public:
	CMsgBoxEvent(void);
	~CMsgBoxEvent(void);
private:
	list<tagMsgBoxEvent*>		m_MsgBoxEvents;
public:
	void	AddEvent(eMsgBoxEvent eEvent,...);
	void	ClearAllEvent();
	
	void	Run();							//运行 MsgBox 并处理结果
	void	Display();						//显示MsgBox
};
