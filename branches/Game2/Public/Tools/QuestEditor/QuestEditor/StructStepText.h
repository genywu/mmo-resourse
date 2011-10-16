//描述任务步骤文本的结构体
#ifndef _Struct_StepText_H
#define _Struct_StepText_H
struct tagStepText 
{
	//类型标志ID.1为接任务显示的文本.2为交任务时的文本.
	int			m_nID;
	//文本ID
	int			m_nName;
	tagStepText()
		:m_nID(0)
		,m_nName(0)
	{
	}
	~tagStepText()
	{

	}
};
#endif
