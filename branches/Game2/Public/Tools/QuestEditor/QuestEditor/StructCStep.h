//定义一个任务步骤的结构体
#include "StructStepAim.h"
#include "StructStepEn.h"
#include "StructStepText.h"
#include "StructStepTrace.h"
#ifndef _StructCStep_H
#define _StructCStep_H

//描述任务步骤的结构体
struct tagRoleStep 
{
	//任务ID
	int			m_nRoleID;
	//步骤ID.一般与步数相同
	int			m_nID;	
	//NPC原始名
	string		m_cNPCID;
	//……		
	int			m_nNPCEffect_1;
	//……
	int			m_nNPCEffect_2;
	//就步任务奖励物品所占的格子数
	int			m_nEncouragementCell; 
	//任务追踪结构体
	vector<tagStepTrace*>   vcStepRoleTrace;
	//任务目标结构体
	vector<tagStepAim*>     vcStepAim;
	/***********************************************/
	//构造函数
	tagRoleStep()
		:m_nRoleID(0)
		,m_nID(1)
		,m_cNPCID(_T("00000"))
		,m_nNPCEffect_2(0)
		,m_nNPCEffect_1(0)
		,m_nEncouragementCell(1)
	{

	}

    tagRoleStep( const tagRoleStep& tagStep )
        :m_nRoleID(tagStep.m_nRoleID)
        ,m_nID(tagStep.m_nID)
        ,m_cNPCID(tagStep.m_cNPCID)
        ,m_nNPCEffect_2(tagStep.m_nNPCEffect_1)
        ,m_nNPCEffect_1(tagStep.m_nNPCEffect_2)
        ,m_nEncouragementCell(tagStep.m_nEncouragementCell)
    {

        for ( int i = 0 ; i < (int) tagStep.vcStepRoleTrace.size() ; i++ )
        {
            vcStepRoleTrace.push_back( new tagStepTrace(*tagStep.vcStepRoleTrace[i]));
        }
        for ( int i = 0 ; i < (int) tagStep.vcStepAim.size() ; i++ )
        {
            vcStepAim.push_back( new tagStepAim(*tagStep.vcStepAim[i]));
        }
    }

	//析构函数
	virtual~tagRoleStep()
	{
		vector<tagStepTrace*>::iterator Titer;
		Titer = vcStepRoleTrace.begin();
		for (;Titer!=vcStepRoleTrace.end();Titer++)
		{
			if (*Titer==NULL)
			{
			}
			else
			{
				delete (*Titer);
				*Titer=NULL;
			}
		}
		vcStepRoleTrace.clear();
		vector<tagStepAim*>::iterator Aiter;
		Aiter = vcStepAim.begin();
		for (;Aiter!=vcStepAim.end();Aiter++)
		{
			if (*Aiter==NULL)
			{
				break;
			}
			else
			{
				delete (*Aiter);
				*Aiter=NULL;
			}
		}
		vcStepAim.clear();	
	}
};
#endif