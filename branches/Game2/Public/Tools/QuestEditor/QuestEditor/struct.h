#include <string>
#include<vector>
#include <cstring>
#include "StructCA.h"
#include "StructCStep.h"
#include "StructCEn.h"
#include "StructStepText.h"
#include "StructStepEn.h"
#include "StructStepTrace.h"
#include "StructStepAim.h"
using namespace std;

#ifndef _STRUCT_H
#define _STRUCT_H
//描述任务的结构体
struct tagRole
{
	//任务ID
	int			m_nID;
	//任务所在文件名
	string		m_strFileName;
	//任务属性
	tagRoleAttribute*       vcRole_Attribute;
	//任务步骤
	tagRoleStep*            vcRole_Step;
	//前序任务索引
	vector<int>             m_nRoleIndex ;
	//任务所占的格子数
	int			            m_nEncouragementCell;
	//构造函数
	tagRole()
		:m_nID(0)
		,m_nEncouragementCell(0)
		,m_strFileName(_T("新建任务"))
	{
        m_nRoleIndex.clear();
	}

    tagRole( const tagRole & role):m_nID(role.m_nID),
                                   m_strFileName(role.m_strFileName),
                                   m_nEncouragementCell(role.m_nEncouragementCell),
                                   m_nRoleIndex( role.m_nRoleIndex )
    {
         vcRole_Attribute = new tagRoleAttribute(*role.vcRole_Attribute);
         vcRole_Step      = new tagRoleStep( * role.vcRole_Step );
    }

	//析构函数
	virtual	~tagRole()
	{
        if (vcRole_Attribute!=NULL)
        {
            delete vcRole_Attribute;
            vcRole_Attribute=NULL;
        }
        if (vcRole_Step!=NULL)
        {
            delete vcRole_Step;
            vcRole_Step=NULL;
        }
	}
};
#endif