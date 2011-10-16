//描述任务奖励的结构体
#ifndef  _Struct_CEn_H
#define  _Struct_CEn_H

struct tagRoleEncouragement
{
	//任务ID
	int			m_nID;
	//任务奖励物品所占的格子数
	int			m_nEncouragementCell;
	//奖励物品类型
	int			m_nType;
    //奖励物品数量
    int			m_nNum;
    //奖励倍率
    double      m_fOdds;
	//奖励物品名称
	string		m_cName;
	
	tagRoleEncouragement()
		:m_nID(0)
		,m_nEncouragementCell(0)
		,m_nType(0)
		,m_cName(_T(""))
		,m_nNum(0)
	{
	}
	virtual~tagRoleEncouragement()
	{
	}
};
#endif

