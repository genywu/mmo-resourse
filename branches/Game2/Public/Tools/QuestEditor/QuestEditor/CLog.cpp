/*Clog.cpp
 *CLog类实现文件
*/
#include "StdAfx.h"
#include "CLog.h"
#include "QuestEditorDlg.h"
#pragma warning(disable:4996)
CLog::CLog()
{
}

CLog::~CLog()
{
}
/*功能:获取系统当前的时间和日期。
*摘要:获取系统当前的时间和日期,并分别保存在不同的变量中。
*参数:strTime用来保存时间.strDate来保存日期.
*返回值:void
*作者：王 钦
*创建日期:2010-7-6
*/
void GetTime(CString & strTime,CString& strDate)
{
	//定义一个时间的结构体
	SYSTEMTIME st;
	//获取系统时间
	GetLocalTime(&st); 
	strDate.Format("%d-%d-%d",st.wYear,st.wMonth,st.wDay);
	strTime.Format("%d-%d-%d",st.wHour,st.wMinute,st.wSecond);
}
/*功能:创建日志的。
*摘要:根据更新前的任务属性和更新后的任务属性来创建日志并保存在m_Log变量中.
*参数:pPre更新前的任务信息.pUped更新后的任务信息.
*返回值:void
*作者：王 钦
*创建日期:2010-7-6
*/
void CLog::CreatLogList(vector<tagRoleAttribute*>pPre,
						vector<tagRoleAttribute*>pUped )
{
	char strVal[256];
	CString cTime;
	CString cDate;
	//获取时间
	GetTime(cTime,cDate);
	string strTime = (LPCTSTR)cTime;
	int id ;
	vector<tagRoleAttribute*>::iterator IterPre;
	vector<tagRoleAttribute*>::iterator IterUped;
	IterPre = pPre.begin();
	IterUped = pUped.begin();
	for (;IterPre!=pPre.end();IterPre++)
	{
		for (;IterUped!=pUped.end();IterUped++)
		{
			id = (*IterPre)->m_nID;
			if (strcmp(((*IterPre)->m_cName).c_str(),((*IterUped)->m_cName).c_str()))
			{
				string  cPre = (*IterPre)->m_cName;
				string  cUpdated = (*IterUped)->m_cName;
				m_Log.push_back("Name:"+cPre+"--->"+cUpdated);
			}
		   if ((*IterPre)->m_QuestType!=(*IterUped)->m_QuestType)
			{
				string  cPre = (LPCTSTR)itoa((*IterPre)->m_QuestType,strVal,10);
				string  cUpdated = (LPCTSTR)(itoa((*IterUped)->m_QuestType,strVal,10));
				m_Log.push_back("RoleType:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nRoleEmbracer!=(*IterUped)->m_nRoleEmbracer)
			{
				string  cPre = (LPCTSTR)itoa((*IterPre)->m_nRoleEmbracer,strVal,10);
				string  cUpdated = (LPCTSTR)itoa((*IterUped)->m_nRoleEmbracer,strVal,10);
				m_Log.push_back("RoleEmbracer:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nHardFactor!=(*IterUped)->m_nHardFactor)
			{
				string  cPre = itoa((*IterPre)->m_nHardFactor,strVal,10);
				string  cUpdated = itoa((*IterPre)->m_nHardFactor,strVal,10);
				m_Log.push_back("HardFactor:"+cPre+"--->"+cUpdated);
			}
			 if(strcmp(((*IterPre)->m_cDeleteScript).c_str(),((*IterUped)->m_cDeleteScript).c_str()))
			{
				string  cPre = ((*IterPre)->m_cDeleteScript).c_str();
				string  cUpdated = ((*IterUped)->m_cDeleteScript).c_str();
				m_Log.push_back("DeleteScript"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nEmbracerEffect!=(*IterUped)->m_nEmbracerEffect)
			{
				string  cPre = itoa((*IterPre)->m_nEmbracerEffect,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nEmbracerEffect,strVal,10);
				m_Log.push_back("EmbracerEffect"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nFinishEmbracerEffect!=(*IterUped)->m_nFinishEmbracerEffect)
			{
				string  cPre = itoa((*IterPre)->m_nFinishEmbracerEffect,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nFinishEmbracerEffect,strVal,10);
				m_Log.push_back("FinishEmbracerEffect:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nEmbracerMapEffect!=(*IterUped)->m_nEmbracerMapEffect)
			{
				string  cPre = itoa((*IterPre)->m_nEmbracerMapEffect,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nEmbracerMapEffect,strVal,10);
				m_Log.push_back("EmbracerMapEffect:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nBeRecord!=(*IterUped)->m_nBeRecord)
			{
				string  cPre = itoa((*IterPre)->m_nBeRecord,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nBeRecord,strVal,10);
				m_Log.push_back("BeRecord:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nShowAble!=(*IterUped)->m_nShowAble)
			{
				string  cPre = itoa((*IterPre)->m_nShowAble,strVal,10);
				string  cUpdated =itoa((*IterUped)->m_nShowAble,strVal,10);
				m_Log.push_back("ShowAble:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nOdds!=(*IterUped)->m_nOdds)
			{
				string  cPre = itoa((*IterPre)->m_nOdds,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nOdds,strVal,10);
				m_Log.push_back("Odds:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nMassPoint!=(*IterUped)->m_nMassPoint)
			{
				string  cPre =itoa((*IterPre)->m_nMassPoint,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nMassPoint,strVal,10);
				m_Log.push_back("MassPoint:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nDiscribTextID!=(*IterUped)->m_nDiscribTextID)
			{
				string  cPre = (*IterPre)->m_nDiscribTextID;
				string  cUpdated = (*IterUped)->m_nDiscribTextID;
				m_Log.push_back("DiscribTextID:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nMiniGrade!=(*IterUped)->m_nMiniGrade)
			{
				string  cPre = itoa((*IterPre)->m_nMiniGrade,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nMiniGrade,strVal,10);
				m_Log.push_back("MiniGrade:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nMaxGrade!=(*IterUped)->m_nMaxGrade)
			{
				string  cPre = itoa((*IterPre)->m_nMaxGrade,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nMaxGrade,strVal,10);
				m_Log.push_back("MaxGrade:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nRepeated!=(*IterUped)->m_nRepeated)
			{
				string  cPre = itoa((*IterPre)->m_nRepeated,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nRepeated,strVal,10);
				m_Log.push_back("Repeated:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nDeadLost!=(*IterUped)->m_nDeadLost)
			{
				string  cPre = itoa((*IterPre)->m_nDeadLost,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nDeadLost,strVal,10);
				m_Log.push_back("DeadLost:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nLostRepeated!=(*IterUped)->m_nLostRepeated)
			{
				string  cPre = itoa((*IterPre)->m_nLostRepeated,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nLostRepeated,strVal,10);
				m_Log.push_back("LostRepeated:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nEngageArmsRepute!=(*IterUped)->m_nEngageArmsRepute)
			{
				string  cPre = itoa((*IterPre)->m_nEngageArmsRepute,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nEngageArmsRepute,strVal,10);
				m_Log.push_back("EngageArmsRepute"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nChapterNum!=(*IterUped)->m_nChapterNum)
			{
				string  cPre = itoa((*IterPre)->m_nChapterNum,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nChapterNum,strVal,10);
				m_Log.push_back("ChapterNum:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nPicture!=(*IterUped)->m_nPicture)
			{
				string  cPre =itoa((*IterPre)->m_nPicture,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nPicture,strVal,10);
				m_Log.push_back("Picture:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nIcon!=(*IterUped)->m_nIcon)
			{
				string  cPre = itoa((*IterPre)->m_nIcon,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nIcon,strVal,10);
				m_Log.push_back("Icon:"+cPre+"--->"+cUpdated);
			}
			 if((*IterPre)->m_nStoryDesc!=(*IterUped)->m_nStoryDesc)
			{
				string  cPre = (*IterPre)->m_nStoryDesc;
				string  cUpdated = (*IterUped)->m_nStoryDesc;
				m_Log.push_back("StoryDesc:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nNPCShowType!=(*IterUped)->m_nNPCShowType)
			{
				string  cPre = itoa((*IterPre)->m_nNPCShowType,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nNPCShowType,strVal,10);
				m_Log.push_back("NPCShowType:"+cPre+"--->"+cUpdated);
			}
			 if (strcmp(((*IterPre)->m_cNPCID).c_str(),((*IterUped)->m_cNPCID).c_str()))
			{
				string  cPre = ((*IterPre)->m_cNPCID).c_str();
				string  cUpdated = ((*IterUped)->m_cNPCID).c_str();
				m_Log.push_back("NPCID"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nFinishEffect!=(*IterUped)->m_nFinishEffect)
			{
				string  cPre = itoa((*IterPre)->m_nFinishEffect,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nFinishEffect,strVal,10);
				m_Log.push_back("FinishEffect:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nNPCClewType!=(*IterUped)->m_nNPCClewType)
			{
				string  cPre = itoa((*IterPre)->m_nNPCClewType,strVal,10);
				string  cUpdated = itoa((*IterUped)->m_nNPCClewType,strVal,10);
				m_Log.push_back("NPCClewType:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nBeSearchDesc!=(*IterUped)->m_nBeSearchDesc)
			{
				string  cPre = (*IterPre)->m_nBeSearchDesc;
				string  cUpdated =(*IterUped)->m_nBeSearchDesc;
				m_Log.push_back("BeSearchDesc:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nBeSearchSpDesc!=(*IterUped)->m_nBeSearchSpDesc)
			{
				string  cPre = (*IterPre)->m_nBeSearchSpDesc;
				string  cUpdated =(*IterUped)->m_nBeSearchSpDesc;
				m_Log.push_back("BeSearchSpDesc:"+cPre+"--->"+cUpdated);
			}
			 if ((*IterPre)->m_nRoleIndex!=(*IterUped)->m_nRoleIndex)
			{
				string  cPre = (*IterPre)->m_nRoleIndex;
				string  cUpdated = (*IterUped)->m_nRoleIndex;
				m_Log.push_back("RoleIndex:"+cPre+"--->"+cUpdated);
			}
			m_Log.push_back("--> "+strTime+"编辑了"+"任务id为:"+itoa(id,strVal,10)+"的任务属性将:");
		}
	}
}

/*功能:创建日志的。
*摘要:根据更新前的任务奖励和更新后的任务奖励来创建日志并保存在m_Log变量中.
*参数:pPre更新前的任务信息.pUped更新后的任务信息.
*返回值:void
*作者：王 钦
*创建日期:2010-7-6
*/
void CLog::CreatLogList(vector<tagRoleEncouragement*>pPre,
						vector<tagRoleEncouragement*>pUped)
{
	char strVal[256];
	CString cTime;
	CString cDate;
	//获取时间
	GetTime(cTime,cDate);
	string strTime = (LPCTSTR)cTime;
	int id ;
	vector<tagRoleEncouragement*>::iterator IterPre;
	IterPre = pPre.begin();
	vector<tagRoleEncouragement*>::iterator IterUped;
	IterUped = pUped.begin();
	id = (*IterUped)->m_nID;
	m_Log.push_back("--> "+strTime+"编辑了"+"任务id为:"+itoa(id,strVal,10)+"的任务奖励:");
	if (pPre.size()<pUped.size())
	{
		m_Log.push_back("添加了任务奖励");
	}
	 if (pPre.size()>pUped.size())
	{
		m_Log.push_back("删除了任务奖励");
	}
	 if (pPre.size()==pUped.size())
	{
		m_Log.push_back("编辑了任务奖励");
	}
}

/*功能:创建日志的。
*摘要:根据更新前的任务步骤和更新后的任务步骤来创建日志并保存在m_Log变量中.
*参数:pPre更新前的任务信息.pUped更新后的任务信息.
*返回值:void
*作者：王 钦
*创建日期:2010-7-6
*/
void CLog::CreatLogList(vector<tagRoleStep*> pPre,
						tagRoleStep* pUpdatedeStep)
{
	char strVal[256];
	CString cTime,cDate;
	//获取时间和日期
	GetTime(cTime,cDate);
	string strTime = (LPCTSTR)cTime;
	int id ;
	vector<tagRoleStep*>::iterator IterPre;
	IterPre = pPre.begin();
	id = pUpdatedeStep->m_nRoleID;
	m_Log.push_back("--> "+strTime+"编辑了"+"任务id为:"+itoa(id,strVal,10)+"的任务步骤:");
	//保存任务目标
	vector<tagStepAim*>pPreStepAim = (*IterPre)->vcStepAim;
	vector<tagStepAim*>pUpedStepAim = pUpdatedeStep->vcStepAim;
// 	//保存任务文本
// 	vector<tagStepText*>pPreStepText = (*IterPre)->vcStepText;
// 	vector<tagStepText*>pUpedStepText = pUpdatedeStep->vcStepText;
	//保存任务追踪
	vector<tagStepTrace*>pPreStepTrace = (*IterPre)->vcStepRoleTrace;
	vector<tagStepTrace*>pUpedStepTrace = pUpdatedeStep->vcStepRoleTrace;
// 	//保存任务奖励
// 	vector<tagStepEncouragement*>pPreStepEn = (*IterPre)->vcStepEncouragement;
// 	vector<tagStepEncouragement*>pUpedStepEn = pUpdatedeStep->vcStepEncouragement;
	if (pPreStepAim.size()<pUpedStepAim.size())
	{
		m_Log.push_back("添加了任务目标");
	}	
	if (pPreStepAim.size()>pUpedStepAim.size())
	{
		m_Log.push_back("删除了任务目标");
	}	
	if (pPreStepAim.size()==pUpedStepAim.size())
	{
		m_Log.push_back("编辑了任务目标");
	}	
// 	if (pPreStepText.size()<pUpedStepText.size())
// 	{
// 		m_Log.push_back("添加了任务文本");
// 	}	
// 	if (pPreStepText.size()>pUpedStepText.size())
// 	{
// 		m_Log.push_back("删除了任务文本");
// 	}	
// 	if (pPreStepText.size()==pUpedStepText.size())
// 	{
// 		m_Log.push_back("编辑了任务文本");
// 	}	
	if (pPreStepTrace.size()<pUpedStepTrace.size())
	{
		m_Log.push_back("添加了任务追踪");
	}	
	if (pPreStepTrace.size()>pUpedStepTrace.size())
	{
		m_Log.push_back("删除了任务追踪");
	}	
	if (pPreStepTrace.size()==pUpedStepTrace.size())
	{
		m_Log.push_back("编辑了任务追踪");
	}
// 	if (pPreStepEn.size()<pUpedStepEn.size())
// 	{
// 		m_Log.push_back("添加了任务就歨奖励");
// 	}	
// 	if (pPreStepEn.size()>pUpedStepEn.size())
// 	{
// 		m_Log.push_back("删除了任务就歨奖励");
// 	}	
// 	if (pPreStepEn.size()==pUpedStepEn.size())
// 	{
// 		m_Log.push_back("编辑了任务就歨奖励");
// 	}
}
/*功能:创建日志的。
*摘要:根据特定的敏感操作创建相应的日志,
*参数:strLog创建的日志的内容.
*返回值:void
*作者：王 钦
*创建日期:2010-7-6
*/
void CLog::CreatLogList(string strLog)
{
	CString strTime;
	CString strDate;
	GetTime(strTime,strDate);
	string LogText = (LPCTSTR)strTime+strLog;
	m_Log.push_back(LogText);

}
/*功能:保存日志。
*摘要:将生成的日志保存到txt文件中。
*参数:
*返回值:void
*作者：王 钦
*创建日期:2010-7-6
*/
void CLog::SaveLog()
{
	FILE *fp;
	CString cTime, cDate;
	//获取日期
	GetTime(cTime,cDate);
	CString strDate = cDate;
	CString cTitle = "log\\"+strDate+".txt"; 
	fp=fopen(cTitle,"a");
	vector<string>::iterator iter;
	iter = m_Log.begin();
	for (;iter!=m_Log.end();iter++)
	{
		fprintf(fp,(*iter).c_str());
		fprintf(fp,"\n");
	}
	fclose(fp);
}
