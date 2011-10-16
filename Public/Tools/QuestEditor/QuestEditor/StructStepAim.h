//描述任务目标的结构体
#ifndef _Struct_StepAim_H
#define _Struct_StepAim_H
struct tagStepAim 
{
	//目标ID:个位数表示目标是物品,从1开始.逐步增加.100表示任务开始的NPC,101表示任务结束的NPC.
	int			m_nID;
	//当AimModel为个位数时,表示任务的类型.
	int			m_nAimModel;
	//目标类型
	int			m_nAimType;
	//与RoleTrace的param关联,确定计数的索引
	int			m_nParam;
	//目标数量
	int			m_nAimNum;
	//掉落目标物品的NPC的原始名
	string		m_cDropFrom;
	//目标物品的掉落几率___DropOdds/10000。
	int			m_nDropOdds;
    //目标名称
	string		m_cAimName;
	
	tagStepAim(long nID=0)
		:m_nID(nID)
		,m_nAimModel(0)
		,m_nAimType(0)
		,m_cAimName(_T(""))
		,m_nAimNum(0)
		,m_cDropFrom(_T("NULL"))
		,m_nDropOdds(0)
		,m_nParam(0)
	{
	}

    tagStepAim(const tagStepAim & aim)
        :m_nID(aim.m_nID)
        ,m_nAimModel(aim.m_nAimModel)
        ,m_nAimType(aim.m_nAimType)
        ,m_cAimName(aim.m_cAimName)
        ,m_nAimNum(aim.m_nAimNum)
        ,m_cDropFrom(aim.m_cDropFrom)
        ,m_nDropOdds(aim.m_nDropOdds)
        ,m_nParam(aim.m_nParam)
    {

    }

	~tagStepAim()
	{

	}
};

inline bool  AimCMp( const tagStepAim * rs,const tagStepAim * rm)
{
    return rs->m_nID < rm->m_nID;
}

#endif