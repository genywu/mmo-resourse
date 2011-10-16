//=============================================================================
/**
 *  file: QuestFun.cpp
 *
 *  Brief:定义跟任务相关的脚本函数
 *			
 *
 *  Authtor:wangqiao
 *	
 *	Create Datae:2008-11-06
 */
//=============================================================================

#include "StdAfx.h"
#include "mmsystem.h"
#include "script.h"
#include "variablelist.h"
#include "../player.h"
#include "../serverregion.h"
#include "../../../setup/QuestSystem.h"
#include "../../../setup/LogSystem.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../OrganizingSystem/GSTeam.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"


double CScript::AddQuest(const char* cmd, char* retStr)
{
	double lRet = 0;

	double lRoleID = GetIntParam( cmd, 0 );
	if(lRoleID == ERROR_CODE)
		return lRet;

	lRet = 1;
	//找到Player

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);//NULL;

	if(pPlayer)
	{
		pPlayer->AddQuest(lRoleID);

		GetGame()->UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRQUESTLIST);
	}
	return lRet;
}


double CScript::GetCurrentMasterRoleID(const char* cmd, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);//NULL;
	if (pPlayer== NULL) return 0;
	return (double)pPlayer->GetCurrentMasterRoleID();
}

double CScript::CompleteQuest(const char* CmdStr, char* retStr)
{
	double lRet = 0;

	DWORD dwQuestID = (DWORD)GetIntParam( CmdStr, 0);
	if(dwQuestID == ERROR_CODE)
		return lRet;
	//找到Player
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(pPlayer)
	{
		if(pPlayer->CompleteQuest(dwQuestID))
			lRet = 1;
	}
	return lRet;
}

double CScript::DisbandQuest(const char* CmdStr, char* retStr)
{
	double lRet = 0;

	DWORD dwQuestID = (DWORD)GetIntParam( CmdStr, 0 );
	if(dwQuestID == ERROR_CODE)
		return lRet;

	lRet = 1;
	//找到Player
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(pPlayer)
	{
		GetGameLogInterface()->logT460_quest_log(pPlayer,dwQuestID,2);
		pPlayer->RemoveQuest(dwQuestID);
	}
	return lRet;
}
double CScript::GetQuestState(const char* CmdStr, char* retStr)
{
	double lRet = -1;

	DWORD dwQuestID = GetIntParam( CmdStr, 0 );
	if(dwQuestID == ERROR_CODE)
		return lRet;
	CPlayer* pPlayer =dynamic_cast<CPlayer*>(p_SrcShape);
	if(pPlayer)
	{
		return pPlayer->GetQuestState(dwQuestID);
	}
	return lRet;
}

double CScript::GetValidQuestNum(const char* CmdStr, char* retStr)
{
	double lRet = -1;

	CPlayer* pPlayer =dynamic_cast<CPlayer*>(p_SrcShape);
	if( pPlayer )
	{
		lRet = pPlayer->GetValidQuestNum();
	}
	return lRet;
}

double CScript::UpdateQuestStep(const char* CmdStr, char* retStr)
{
	double lRet = 0;
	DWORD dwQuestID  = GetIntParam( CmdStr, 0 );
	if(dwQuestID == ERROR_CODE)	return lRet;
	double lStepID = GetIntParam( CmdStr, 1 );
	if(lStepID == ERROR_CODE)	return lRet;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);

	if( pPlayer )
	{
		pPlayer->UpdateQuestStep(dwQuestID,lStepID);
	}
	return lRet;
}

double CScript::UpdateQuestParamData(const char* CmdStr, char* retStr)
{
	double lRet = 0;

	DWORD dwQuestID  = GetIntParam( CmdStr, 0 );
	if(dwQuestID == ERROR_CODE)	return lRet;
	double lDataIndex = GetIntParam( CmdStr, 1 );
	if(lDataIndex == ERROR_CODE)	return lRet;
	double lParam=GetIntParam( CmdStr,2 );
	if(lParam == ERROR_CODE)	return lRet;
	double lRegionID=GetIntParam( CmdStr,3 );
	if(lRegionID == ERROR_CODE)	return lRet;
	double lRegionX=GetIntParam( CmdStr,4 );
	if(lRegionX == ERROR_CODE)	return lRet;
	double lRegionY=GetIntParam( CmdStr,5 );
	if(lRegionY == ERROR_CODE)	return lRet;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if( pPlayer )
	{
		pPlayer->UpdataQuestParamData(dwQuestID,lDataIndex-1,lParam,lRegionID,lRegionX,lRegionY);
	}
	return lRet;
}

double CScript::UpdateQuestData(const char* CmdStr, char* cmd )
{
	double lRet = 0;

	DWORD dwQuestID  = GetIntParam( CmdStr, 0 );
	if(dwQuestID == ERROR_CODE)	return lRet;
	double lDataIndex = GetIntParam( CmdStr, 1 );
	if(lDataIndex == ERROR_CODE)	return lRet;
	double lVal=GetIntParam( CmdStr,2 );
	if(lVal == ERROR_CODE)	return lRet;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if( pPlayer )
	{
		pPlayer->UpdataQuestData(dwQuestID,lDataIndex- 1,lVal);
	}
	return lRet;
}


//取得佣兵任务id
double CScript::GetMercQuest(const char* cmd, char* retStr)
{
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player!=NULL)
	{
		return player->GetMercQuest();
	}
	return 0;

}
//设置当佣兵兵任务
double CScript::SetMercQuest(const char* cmd, char* retStr)
{

	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player!=NULL)
	{
		player->SetMercQuest(id);
		return 1;
	}
	return -1;
}
//寻找某任务的步骤。任务不存在返回0
double CScript::GetRoleStepNum(const char* cmd, char* retStr)
{

	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);

	if (player)
	{		
		return player->GetRoleStepNum(id);
	}
	else
	{
		return -1;
	}
}

//取得 某一任务某一步骤的Index对应的值
double CScript::GetParam(const char* cmd, char* retStr)//{return m_RoleQueue.GetParam(RoleID,Step,Index);}
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetRoleParam(id,step,index-1);
	}
	else
	{
		return -1;
	}

}

///////
//取得 某一任务某一步骤的Index对应的附加变量值
double CScript::GetRoleAccParam(const char* cmd, char* retStr)//{return m_RoleQueue.GetParam(RoleID,Step,Index);}
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetRoleAccParam(id,step,index-1);
	}
	else
	{
		return -1;
	}

}
//设置 某一任务某一步骤的Index对应的附加变量值
double CScript::SetRoleAccParam(const char* cmd, char* retStr)//{return m_RoleQueue.SetParam(RoleID,Step,Index,val);}
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	LONG val=GetIntParam(cmd,3);
	if (val==INVILID_VALUE || val==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->SetRoleAccParam(id,step,index-1,val);
	}
	else
	{
		return -1;
	}

}
//增加 某一任务某一步骤的Index对应的附加变量值
double CScript::AddRoleAccParam(const char* cmd, char* retStr)//{return m_RoleQueue.AddParam(RoleID,Step,Index,val);}
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}


	LONG val=GetIntParam(cmd,3);
	if (val==INVILID_VALUE || val==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->AddRoleAccParam(id,step,index-1,val);
	}
	else
	{
		return -1;
	}
}

//取得 某一任务某一步骤的Index对应的场景id
double CScript::GetRoleRegionID(const char* cmd, char* retStr)//{return m_RoleQueue.GetRegionID( RoleID,  Step , Index);}
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetRoleRegionID(id,step,index-1);
	}
	else
	{
		return -1;
	}
}
//设置 某一任务某一步骤的Index对应的场景id
double CScript::SetRoleRegionID(const char* cmd, char* retStr)//{return m_RoleQueue.SetRegionID( RoleID,   Step, Index,  val);}
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	LONG val=GetIntParam(cmd,3);
	if (val==INVILID_VALUE || val==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->SetRoleRegionID(id,step,index-1,val);
	}
	else
	{
		return -1;
	}

}
//取得 某一任务某一步骤的Index对应的场景x坐标
double CScript::GetRoleRegionX(const char* cmd, char* retStr)//{return m_RoleQueue.GetRegionX( RoleID,  Step , Index);}
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetRoleRegionX(id,step,index-1);
	}
	else
	{
		return -1;
	}
}
//设置 某一任务某一步骤的Index对应的场景x坐标
double CScript::SetRoleRegionX(const char* cmd, char* retStr)//{return m_RoleQueue.SetRegionX( RoleID,   Step, Index,  val);}
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	LONG val=GetIntParam(cmd,3);
	if (val==INVILID_VALUE || val==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->SetRoleRegionX(id,step,index-1,val);
	}
	else
	{
		return -1;
	}

}
//取得 某一任务某一步骤的Index对应的场景y坐标
double CScript::GetRoleRegionY(const char* cmd, char* retStr)//{return m_RoleQueue.GetRegionY( RoleID,  Step , Index);}
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetRoleRegionY(id,step,index-1);
	}
	else
	{
		return -1;
	}
}
//设置 某一任务某一步骤的Index对应的场景y坐标
double CScript::SetRoleRegionY(const char* cmd, char* retStr)//{return m_RoleQueue.SetRegionY( RoleID,   Step, Index,  val);}
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	LONG val=GetIntParam(cmd,3);
	if (val==INVILID_VALUE || val==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->SetRoleRegionY(id,step,index-1,val);
	}
	else
	{
		return -1;
	}

}
//取得任务列表的任务数量
double CScript::GetRoleSum(const char* cmd, char* retStr)//{return m_RoleQueue.GetRoleSum();}
{
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetRoleSum();
	}
	return -1;
}

//取得所在主线任务章节
double CScript::GetChapter(const char* cmd, char* retStr)//{return m_RoleQueue.GetChapter();}
{
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetChapter();
	}
	return -1;
}

//设置主线任务章节数
double CScript::SetChapter(const char* cmd, char* retStr)//{m_RoleQueue.SetChapter(num);}
{
	LONG num=GetIntParam(cmd,0);
	if (num==INVILID_VALUE || num==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->SetChapter(num);
	}
	return -1	;
}

//添加任务记录
double CScript::AddFinishedRoleRecord(const char* cmd, char* retStr)//{return m_RoleQueue.AddFinishedRoleRecord(index);}
{
	LONG index=GetIntParam(cmd,0);
	if (index==INVILID_VALUE || index== ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->AddFinishedRoleRecord(index);
	}
	return -1;
}
//取得一个索引的任务的状态（是否完成）
double CScript::GetRoleFinishedRecord(const char* cmd, char* retStr)//{return m_RoleQueue.GetRoleFinishedRecord(index);}
{
	LONG index=GetIntParam(cmd,0);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetRoleFinishedRecord(index);
	}
	return -1	;
}

double CScript::DelFinishedRoleRecord(const char* cmd, char* retStr)//{return m_RoleQueue.GetRoleFinishedRecord(index);}
{
	LONG index=GetIntParam(cmd,0);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->DelFinishedRoleRecord(index);
	}
	return -1	;
}

double CScript::GetStepText(const char* cmd, char* retStr)
{
	LONG lRole=GetIntParam(cmd,0);
	if (lRole==INVILID_VALUE || lRole==ERROR_CODE)
	{
		return -1;
	}

	LONG lTextID=GetIntParam(cmd,1);
	if (lTextID==INVILID_VALUE || lTextID==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetStepText(lRole,lTextID);
	}
	return -1;
}
double CScript::GetText(const char* cmd, char* retStr)
{
	LONG lRole=GetIntParam(cmd,0);
	if (lRole==INVILID_VALUE || lRole==ERROR_CODE)
	{
		return -1;
	}

	LONG lStep=GetIntParam(cmd,1);
	if (lStep==INVILID_VALUE || lStep==ERROR_CODE)
	{
		return -1;
	}

	LONG lTextID=GetIntParam(cmd,2);
	if (lTextID==INVILID_VALUE || lTextID==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetText(lRole,lStep,lTextID);
	}
	return -1;
}
//。。。。。。。。。。对任务系统的操作。。。。。。。。。。。。。
double CScript::GetTextType(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetTextType(id);
	}
	return -1;
}
double CScript::GetAttributeRoleName(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	char* szVarName = GetStringParam(cmd, 1);
	if (szVarName== NULL) return -1;

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player==NULL)
	{
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	char* name = (char*)player->GetAttributeRoleName(id);
	if (name==NULL)
	{
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	int ret = SetScriptVarValue((CMoveShape*)p_SrcShape, szVarName, name);
	M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
	return ret;
}

double CScript::GetAttributeBeRecord(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeBeRecord(id);
	}
	return -1;
}

double CScript::GetAttributeExpRatio(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeExpRatio(id);
	}
	return -1;
}
//任务类型
double CScript::GetAttributeRoleType(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeRoleType(id);
	}
	return -1;
}
//任务接受npc的id
double CScript::GetAttributeEmbracerNPCID(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	char* szVarName = GetStringParam(cmd, 1);
	if (szVarName== NULL)	return -1;

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);

	if (player==NULL)
	{
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	char* name = (char*)player->GetAttributeEmbracerNPCID(id);
	if (name==NULL)
	{
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	int ret = SetScriptVarValue((CMoveShape*)p_SrcShape, szVarName, name);
	M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
	return ret;

}
//是否被显示
double CScript::GetAttributeShowAble(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeShowAble(id);
	}
	return -1;
}
//是否能反复接
double CScript::GetAttributeRepeated(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeRepeated(id);
	}
	return -1;
}
//死亡是否失败
double CScript::GetAttributeDeadLost(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeDeadLost(id);
	}
	return -1;
}
//任务失败后能否重新接
double CScript::GetAttributeLostRepeated(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeLostRepeated(id);
	}
	return -1;
}
//最小等级限制
double CScript::GetAttributeMinGrade(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeMinGrade(id);
	}
	return -1;
}
//最大等级限制
double CScript::GetAttributeMaxGrade(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeMaxGrade(id);
	}
	return -1;
}
//职业限制
double CScript::GetAttributeOccupationRestrict(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}
	LONG index=GetIntParam(cmd,1);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeOccupationRestrict(id,index);
	}
	return -1;
}

//需要佣兵声望
double CScript::GetAttributeEngageArmsRepute(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeEngageArmsRepute(id);
	}
	return -1;
}

//章节数
double CScript::GetAttributeChapterNum(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeChapterNum(id);
	}
	return -1;
}

//下一个任务ID
double CScript::GetAttributeNextRoleID(const char* cmd, char* retStr /* =NULL */)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAttributeNextRoleID(id);
	}
	return -1;
}

//取得index对应的前置任务
double CScript::GetLabelPrefix(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,1);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}


	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetLabelPrefix(id,index);
	}
	return -1;
}
//取得前置任务数量
double CScript::GetLabelSum(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetLabelSum(id);
	}
	return -1;
}

//目标模板类型
double CScript::GetAimAMTType(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG aim=GetIntParam(cmd,2);
	if (aim==INVILID_VALUE || aim==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAimAMTType(id,step,aim);
	}
	return -1;
}
//目标类型
double CScript::GetAimType(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG aim=GetIntParam(cmd,2);
	if (aim==INVILID_VALUE || aim==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAimType(id,step,aim);
	}
	return -1	;
}
//取得目标id
double CScript::GetAimId(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG aim=GetIntParam(cmd,2);
	if (aim==INVILID_VALUE || aim==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAimId(id,step,aim);
	}
	return -1;
}
//取得目标数量
double CScript::GetAimNum(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG aim=GetIntParam(cmd,2);
	if (aim==INVILID_VALUE || aim==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAimNum(id,step,aim);
	}
	return -1;
}
//取得 变量索引
double CScript::GetAimParameIndex(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG aim=GetIntParam(cmd,2);
	if (aim==INVILID_VALUE || aim==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetAimParameIndex(id,step,aim);
	}
	return -1;
}
//掉落这种物品的生物类型（对物品）
double CScript::GetDropFrom(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG aim=GetIntParam(cmd,2);
	if (aim==INVILID_VALUE || aim==ERROR_CODE)
	{
		return -1;
	}

	char* szVarName = GetStringParam(cmd, 3);
	if (szVarName == NULL)	return -1;

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player==NULL)	
	{
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	char* name = (char*)player->GetDropFrom(id,step,aim);
	if (name==NULL){
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	int ret = SetScriptVarValue((CMoveShape*)p_SrcShape, szVarName, name);
	M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
//几率
double CScript::GetDropOdds(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG aim=GetIntParam(cmd,2);
	if (aim==INVILID_VALUE || aim==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetDropOdds(id,step,aim);
	}
	return -1;
}

//取得目标名字
double CScript::GetAimName(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG aim=GetIntParam(cmd,2);
	if (aim==INVILID_VALUE || aim==ERROR_CODE)
	{
		return -1;
	}

	char* szVarName = GetStringParam(cmd, 3);
	if (szVarName== NULL) return -1;

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player==NULL)	
	{
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	char* name = (char*)player->GetAimName(id,step,aim);
	if (name==NULL)
	{
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	int ret = SetScriptVarValue((CMoveShape*)p_SrcShape, szVarName, name);
	M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
	return ret;
}

//取得备用属性
double CScript::GetPrepareParam(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG aim=GetIntParam(cmd,2);
	if (aim==INVILID_VALUE || aim==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,3);
	if (aim== INVILID_VALUE || aim==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetPrepareParam(id,step,aim,index-1);
	}
	return -1;
}
//CQusetEncouragement
//CQusetEncouragement
double CScript::GetStepEncouragementSum(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}


	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetStepEncouragementSum(id,step);
	}
	return -1;
}
double CScript::GetQuestEncouragementSum(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetQuestEncouragementSum(id);
	}
	return -1;
}
//取得奖励的名字
double CScript::GetSetpEncouragementName(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	char* szVarName = GetStringParam(cmd, 3);
	if (szVarName== NULL)	return -1;

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player==NULL)	
	{
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	char* name = (char*)player->GetSetpEncouragementName(id,step,index);
	if (name==NULL)	
	{
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	int ret = SetScriptVarValue((CMoveShape*)p_SrcShape, szVarName, name);
	M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
	return ret;
}

double CScript::GetQuestEncouragementName(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,1);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	char* szVarName = GetStringParam(cmd, 2);
	if (szVarName ==NULL)	return -1;

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player==NULL)	
	{
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return INVILID_VALUE;
	}

	char* name = (char*)player->GetQuestEncouragementName(id,index);
	if (name==NULL)	
	{
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	int ret = SetScriptVarValue((CMoveShape*)p_SrcShape, szVarName, name);
	M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
//取得奖励类型
double CScript::GetSetpEncouragementType(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		if (player)
		{
			return player->GetStepEncouragementType(id,step,index);
		}
	}
	return -1;
}

double CScript::GetQuestEncouragementType(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,1);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetQuestEncouragementType(id,index);
	}
	return -1;
}
//奖励的具体id
double CScript::GetSetpEncouragementID(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetStepEncouragementID(id,step,index);
	}
	return -1;
}

double CScript::GetQuestEncouragementID(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,1);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetQuestEncouragementID(id,index);
	}
	return -1;
}
//奖励数量
double CScript::GetSetpEncouragementNum(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,2);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetStepEncouragementNum(id,step,index);
	}
	return -1;
}
double CScript::GetQuestEncouragementNum(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,1);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetQuestEncouragementNum(id,index);
	}
	return -1;
}
//奖励所占的格子数
double CScript::GetStepEncouragementCell(const char* cmd, char* retStr)
{
	double id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG index=GetIntParam(cmd,1);
	if (index==INVILID_VALUE || index==ERROR_CODE)
	{
		return -1;
	}


	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetStepEncouragementCell(id,index);
	}
	return -1;
}
double CScript::GetQuestEncouragementCell(const char* cmd, char* retStr)
{
	double id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetQuestEncouragementCell(id);
	}
	return -1;
}


double CScript::GetStepSum(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetStepSum(id);
	}
	return -1;
}

double CScript::GetStepAimNum(const char* cmd, char* retStr)
{
	double id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG stepID=GetIntParam(cmd,1);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetStepAimNum(id,stepID);
	}
	return -1;

}

double CScript::FindRoleIsInSystem(const char* cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->FindRoleIsInSystem(id);
	}
	return -1;
}

double CScript::FindRoleStepIsInSystem(const char*cmd, char* retStr)
{
	LONG id=GetIntParam(cmd,0);
	if (id==INVILID_VALUE || id==ERROR_CODE)
	{
		return -1;
	}

	LONG step=GetIntParam(cmd,1);
	if (step==INVILID_VALUE || step==ERROR_CODE)
	{
		return -1;
	}

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->FindRoleStepIsInSystem(id,step);
	}
	return -1;
}

//俑兵任务
//重置俑兵任务刷新时间
double CScript::ResetMercQuestUpdateTime(const char*cmd, char* retStr)
{
	long lRet = 0;
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		//player->ResetMercQuestUpdateTime();
	}
	return lRet;
}
//获得俑兵列表ID任务数据
double CScript::AcquireMercQuest(const char*cmd, char* retStr)
{
	long lRet = 0;
	long lQuestListID = GetIntParam(cmd,0);
	if (lQuestListID != INVILID_VALUE && lQuestListID!=ERROR_CODE)
	{
		CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
		if (player)
		{
			if (!player->IsMerQuestInit(lQuestListID))
			{
				player->AcquireMercQuestList(lQuestListID);
			}
		}
		lRet = 1;
	}
	return lRet;
}
//发送俑兵任务数据到客户端，$俑兵列表ID==0表示只发送刷新时间
double CScript::SendMercQuestToClient(const char*cmd, char* retStr)
{
	long lRet = 0;
	long lQuestListID = GetIntParam(cmd,0);
	if (lQuestListID != INVILID_VALUE && lQuestListID!=ERROR_CODE)
	{
		CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
		if (player)
		{
			player->SendMercQuestDataToClient(lQuestListID);
		}
		lRet = 1;
	}
	return lRet;
}
//打开俑兵任务界面
double CScript::OpenMercQuestInterface(const char*cmd, char* retStr)
{
	long lRet = 0;
	long lQuestListID = GetIntParam(cmd,0);
	if (lQuestListID != INVILID_VALUE && lQuestListID!=ERROR_CODE)
	{
		CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
		if (player)
		{
			player->OpenMercQuestInterface(lQuestListID);
		}
		lRet = 1;
	}
	return lRet;
}
//队员打开接受赏金任务界面
double CScript::OpenShareQuestInterface(const char* cmd, char* retStr/* =NULL */)
{
	long lRet = 0;
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		player->OpenShareQuestInterface();
		lRet = 1;
	}
	return lRet;
}
//队伍当前进行的任务
double CScript::GetTeamCurrQuest(const char* cmd, char* retStr/* =NULL */)
{
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		return player->GetTeamCurrQuest();
	}
	return 0;
}

//队伍当前进行的任务
double CScript::SetTeamCurrQuest(const char* cmd, char* retStr/* =NULL */)
{
	long lRet = 0;
	long lQuestID=GetIntParam(cmd,0);
	if (lQuestID!=ERROR_CODE)
	{
		CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
		if (player)
		{
			player->SetTeamCurrQuest(lQuestID);
			lRet = 1;
		}	
	}
	return lRet;
}
//当玩家接受的赏金任务在共享列表时,删除共享列表中的任务
double CScript::DelTeamShareQuest(const char* cmd,char* retStr/* =NULL */)
{
	long lRet = 0;
	long lQuestID=GetIntParam(cmd,0);
	if (lQuestID!=ERROR_CODE)
	{
		CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
		if (player)
		{
			player->DeleteTeamQuest(lQuestID);
			lRet = 1;
		}	
	}
	return lRet;
}
//在接任务时判断要接的任务是否是在队伍中已完成过的
double CScript::IsCompleteQuest(const char* cmd,char* retStr/* =NULL */)
{
	long lQuestID=GetIntParam(cmd,0);
	if (lQuestID!=ERROR_CODE)
	{
		CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
		if (player)
		{
			if( player->IsCompleteQuest(lQuestID) )
				return 0;	
		}	
	}
	return 1;
}
//得道俑兵任务数据
double CScript::GetMercQuestData(const char*cmd, char* retStr)
{
	long lRet = 0;
	long lQuestListID = GetIntParam(cmd,0);
	if (lQuestListID != INVILID_VALUE && lQuestListID!=ERROR_CODE)
	{
		long lPos = GetIntParam(cmd,1);
		if (lPos != INVILID_VALUE && lPos!=ERROR_CODE)
		{
			CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
			if (player)
			{
				lRet = player->GetMercQuestData(lQuestListID,lPos);
			} 
		}
	}
	return lRet;
}
//设置俑兵任务数据
double CScript::SetMercQuestData(const char*cmd, char* retStr)
{
	long lRet = 0;
	long lQuestListID = GetIntParam(cmd,0);
	if (lQuestListID != INVILID_VALUE && lQuestListID!=ERROR_CODE)
	{
		long lPos = GetIntParam(cmd,1);
		if (lPos != INVILID_VALUE && lPos!=ERROR_CODE)
		{
			long lValue = GetIntParam(cmd,2);
			if (lValue != INVILID_VALUE && lValue!=ERROR_CODE)
			{
				CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
				if (player)
				{
					player->SetMercQuestData(lQuestListID,lPos,lValue);
					lRet = 1;
				}
			}
		}
	}
	return lRet;
}

double CScript::GetQuestOdds(const char* cmd,char* retStr/* =NULL */)
{
	long lRet = 0;
	long lQuestID = GetIntParam(cmd,0);
	if (lQuestID != INVILID_VALUE && lQuestID!=ERROR_CODE)
	{	
		CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
		if (player)
		{
			lRet=player->GetQuestOdds(lQuestID);
		}	
	}
	return lRet;
}
//判断是否能够接受佣兵任务
double CScript::CanAcceptMercQuest(const char* cmd,char* retStr/* =NULL */)
{
	long lRet = 0;
	long lQuestID = GetIntParam(cmd,0);
	if (lQuestID != INVILID_VALUE && lQuestID!=ERROR_CODE)
	{	
		CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
		if (player)
		{
			lRet=player->CanAcceptMercQuest(lQuestID);
		}	
	}
	return lRet;
}
//返回任务集结点
double CScript::GetQuestMassPoint(const char*cmd, char* retStr)
{
	long lRet = 0;
	long lQuestID = GetIntParam(cmd,0);
	if (lQuestID != INVILID_VALUE && lQuestID!=ERROR_CODE)
	{
		const CQuest * quest=CQuestSystem::GetQuest(lQuestID);
		if (quest)
		{	
			lRet = quest->GetAttribute().GetMassPoint();
		}
	}	
	return lRet;
}

//重置所有俑兵任务数据
double CScript::ResetAllMercQuestData(const char*cmd, char* retStr)
{
	long lRet = 0;
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		//player->ResetAllMercQuestData();
		lRet = 1;
	}
	return lRet;
}

//查看某一任务是否在佣兵任务刷新表里面
double CScript::IsInMercQuestList(const char*cmd, char* retStr)
{
	long lRet = 0;
	long lQuestID = GetIntParam(cmd,0);
	if (lQuestID != INVILID_VALUE && lQuestID!=ERROR_CODE)
	{
		CPlayer* pPlayer= dynamic_cast<CPlayer*>(p_SrcShape);
		if (pPlayer)
		{
			lRet= pPlayer->IsMercListQuest(lQuestID);
		}
	}	
	return lRet;
}

double CScript::SetMasterQuest(const char* cmd, char* retStr/* =NULL */)
{
	long lQuestID = GetIntParam(cmd,0);
	if (lQuestID != INVILID_VALUE && lQuestID!=ERROR_CODE)
	{
		CPlayer* pPlayer= dynamic_cast<CPlayer*>(p_SrcShape);
		if (pPlayer)
		{
			pPlayer->SetMasterQuest(lQuestID);
		}
	}
	return 1;
}

double CScript::SetRoleProtect(const char* cmd, char* retStr)
{
	if (p_SrcShape == NULL)	return 0;

	int iRoleProtect = GetIntParam(cmd, 0);
	if (iRoleProtect==ERROR_CODE || iRoleProtect==INVILID_VALUE)	return 0;

	if (p_SrcShape->GetType()== TYPE_MONSTER || p_SrcShape->GetType()== TYPE_COLLECTION)
	{
		dynamic_cast<CMoveShape*>(p_SrcShape)->SetRoleProtect(iRoleProtect!=0);
	}
	return 0;
}

double CScript::OpenIndexQuestInterface(const char *cmd, char *retStr)
{
	long lRet = 0;
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		player->OpenIndexQuestInterface();
		lRet = 1;
	}
	return lRet;
}

double CScript::SetIndexQuestParamNum(const char *cmd, char *retStr)
{
	long lRet = 0;
	long lQuestID = GetIntParam(cmd,0);
	long lParamNum = GetIntParam(cmd,1);
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		player->SetIndexQuestParamNum(lQuestID,lParamNum);
		lRet = 1;
	}
	return lRet;
}

double CScript::SetIndexQuestState(const char *cmd, char *retStr)
{
	long lRet = 0;
	long lQuestID = GetIntParam(cmd,0);
	long lParamNum = GetIntParam(cmd,1);
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		player->SetIndexQuestState(lQuestID,lParamNum);
		lRet = 1;
	}
	return lRet;
}

double CScript::ResetIndexQuest(const char *cmd, char *retStr)
{
	long lRet = 0;
	long lQuestID = GetIntParam(cmd,0);
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		player->ResetQuestIndex(lQuestID);
		lRet = 1;
	}
	return lRet;
}
