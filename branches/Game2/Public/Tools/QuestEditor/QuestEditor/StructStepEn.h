//描述任务单步奖励的结构体
#ifndef _Struct_StepEn_H
#define _Struct_StepEn_H
struct tagStepEncouragement 
{
	//奖励物品的类型
	int			m_nType;
	//奖励物品的名称
	string		m_cName;
	//奖励物品的数量
	int			m_nNum;
	tagStepEncouragement()
		:m_nType(0)
		,m_cName(_T(""))
		,m_nNum(0)
	{
	}
	~tagStepEncouragement()
	{
	}

};
#endif