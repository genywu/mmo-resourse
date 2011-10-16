//描述任务追踪的结构体
#ifndef _Struct_StepTrace_H
#define _Struct_StepTrace_H
struct tagStepTrace 
{
	//任务追踪描述文本的ID
	string		m_nDesc;
	//显示模式
	int			m_nShowNum;
	//数量
	int			m_nSum;
	//任务步骤下的追踪索引
	int			m_nParam;
	//追踪任务物品ID
	int			m_nID;	
    tagStepTrace(long num=0):
		m_nShowNum(0)
		,m_nSum(0)
		,m_nParam(0)
		,m_nID(0)
	{	
        char szText[10];
        sprintf_s( szText ,"Q%d", num );
        m_nDesc = szText;
	}

    tagStepTrace( const tagStepTrace & trace)
    {
       m_nDesc = trace.m_nDesc;
       m_nShowNum = trace.m_nShowNum;
       m_nSum  = trace.m_nSum;
       m_nParam= trace.m_nParam;
       m_nID   = trace.m_nID;	 
    }

	~tagStepTrace()
	{
	}
};
#endif

