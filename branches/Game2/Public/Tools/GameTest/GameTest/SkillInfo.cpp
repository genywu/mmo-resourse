/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：CSkillInfo.cpp
*  功能：单个玩家技能显示列表
*  当前版本：1.1.0
*  作者：
*  完成日期：
*  取代版本：1.0.0
*  原作者：
*  完成日期： 
*
============================================================
*/

#include "stdafx.h"
#include "GameTest.h"
#include "SkillInfo.h"
#include ".\skillinfo.h"
#include "..\App\SkillMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSkillInfo, CDialog)
CSkillInfo::CSkillInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CSkillInfo::IDD, pParent)
	, m_strSkillInfo(_T(""))
{
	m_pDisplaySkill = NULL;
}
CSkillInfo::~CSkillInfo()
{
	list<tagOccupation*>::iterator it = m_OccupList.begin();
	for(; it != m_OccupList.end(); ++it)
	{
		SAFE_DELETE(*it);
		//*it = NULL;
	}
	m_OccupList.clear();

	vector<tagOccupSkill*>::iterator it2 = m_vecOccSkill.begin();
	for (;it2 != m_vecOccSkill.end();++it2)
	{
		SAFE_DELETE(*it2);
	}
	for( int i=0;i< (int)m_mapSkillListXml.size();++i)
	{
		m_mapSkillListXml[i];
	}
	m_vecOccSkill.clear();

	m_mapSkillListXml.clear();
}
void CSkillInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SKILL_LIST, m_CtrSkillList);
	DDX_Control(pDX, IDC_SKILL_EDIT, m_edSkillInfo);
	DDX_Text(pDX, IDC_SKILL_EDIT, m_strSkillInfo);
	DDX_Control(pDX, IDC_OccupSkillList, m_OccSkillList);
	DDX_Control(pDX, IDC_Occup, m_ctrlOccup);
}


BEGIN_MESSAGE_MAP(CSkillInfo, CDialog)
	//ON_WM_PAINT()
//	ON_NOTIFY(NM_CLICK, IDC_SKILL_LIST, OnNMClickSkillList)
//	ON_NOTIFY(NM_RCLICK, IDC_SKILL_LIST, OnNMRclickSkillList)
//	ON_COMMAND(ID_SkillInfo_USESKILL, OnSkillinfoUseskill)
END_MESSAGE_MAP()

BOOL CSkillInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	//LoadOccSkillListXml();
	//LoadOneSkillXml();
//TODO:  在此添加额外的初始化
	 m_CtrSkillList.InsertColumn(1,"名字",0,70,0);
	 m_CtrSkillList.InsertColumn(2,"等级",10,50,10);

	 m_OccSkillList.InsertColumn(1,"名字",0,70,0);
	 m_OccSkillList.InsertColumn(1,"MaxLve",0,50,0);
	 m_OccSkillList.InsertColumn(2,"会",0,25,0);

	   return TRUE;
}

BOOL CSkillInfo::LoadOccSkillListXml()
{
	char strError[128];
	char strFileName[] = "Data\\OccupSkillList.xml";
	TiXmlNode* pNode = NULL;
	TiXmlDocument doc(strFileName);
	if (!doc.LoadFile())
	{
		wsprintf(strError,"载入配置文件%s失败,请检查",strFileName);
		AfxMessageBox(strError,MB_OK,NULL);
		return FALSE;
	}
	pNode = doc.FirstChild("OccupSkillList");
	if (pNode==NULL)
	{
		wsprintf(strError,"没发现OccupSkillList节点,请检查");
		MessageBox(strError,MB_OK,NULL); 
		return FALSE;
	}

	//类型转换
	TiXmlElement* pElem = pNode->ToElement();
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if(!strcmp(pChildElem->Value(),"Occup")
			&& NULL!=pChildElem->Attribute("name")
			&& NULL!=pChildElem->Attribute("ID"))
		{
			tagOccupation * pOccup = new tagOccupation;
			pOccup->strOccupName = pChildElem->Attribute(_T("name"));
			pOccup->lOccupID = (long)atoi(pChildElem->Attribute(_T("ID")));
			// 职业描述
			if(NULL!=pChildElem->Attribute(_T("Desc")))
			{
				pOccup->strOccupDesc = pChildElem->Attribute(_T("Desc"));
			}
			// 读每个职业的技能列表
			if(Init(pChildElem))
				m_OccupList.push_back(pOccup);
			// 读取职业技能列表失败
			else
			{
				delete pOccup;
				pOccup = NULL;
			}
		}
	}
	doc.Clear();
	return TRUE;
}
BOOL CSkillInfo::Init(TiXmlElement* pElem)
{
	if ( pElem!=NULL)
	{
		for( TiXmlElement* pChildElem = pElem->FirstChildElement();
			pChildElem != NULL;
			pChildElem = pChildElem->NextSiblingElement() )
		{
			if ( !strcmp(pChildElem->Value(),_T("Skill")) &&
				NULL != pChildElem->Attribute(_T("name")) &&
				NULL != pChildElem->Attribute(_T("ID"))&&
				NULL != pChildElem->Attribute(_T("lev")) )
			{
				tagOccupSkill* ptagOccupSkill = new tagOccupSkill;
				ptagOccupSkill->strSkillName = pChildElem->Attribute("name");
				ptagOccupSkill->lSkillID = (long)atoi(pChildElem->Attribute("ID"));
				ptagOccupSkill->lSkillMaxLeve = (long)atoi(pChildElem->Attribute("lev"));
				m_vecOccSkill.push_back(ptagOccupSkill);

			}
		}
		return TRUE;
	}
	return FALSE;
}
BOOL CSkillInfo::LoadOneSkillXml()
{
	HANDLE hFind;
	CString strFileName;
	CString strDir = "Data\\skills\\";
	WIN32_FIND_DATA dataFind;
	BOOL bMoreFile = TRUE;
	hFind = FindFirstFile(strDir + "\\*.*",&dataFind);
	while(hFind!=INVALID_HANDLE_VALUE && bMoreFile ==TRUE)
	{
		strFileName = strDir + "\\" + dataFind.cFileName;
		if(strFileName.Right(4) == ".xml")
		{
		//	m_OneSkillXmlFileName.push_back(filename);
			LoadSkillConfigXml(strFileName);
		}
		bMoreFile = FindNextFile(hFind,&dataFind);
	}
	return true;
}
BOOL CSkillInfo::LoadSkillConfigXml(const char* filename)
{
	TiXmlElement	*m_pElem=NULL;
	TiXmlNode *pNode = NULL;
	TiXmlDocument doc(filename);
	doc.LoadFile();

	pNode = doc.FirstChild("Root");
	if(NULL==pNode)
	{
		return false;
	}
	m_pElem = pNode->ToElement();

	for(TiXmlElement *pChildElem = m_pElem->FirstChildElement(); pChildElem!=NULL; pChildElem = pChildElem->NextSiblingElement())
	{
		// 技能节点
		if(!strcmp(pChildElem->Value(),"Skill"))
		{
			tagOneSkill stSkill;
			// 技能编号
			if(NULL!=pChildElem->Attribute("id"))
				stSkill.dwSkillID = atoi( pChildElem->Attribute("id"));
			// 技能名称
			if(NULL!=pChildElem->Attribute("name"))
				stSkill.strSkillName = pChildElem->Attribute("name");
			// 技能状态（一般技能、光环类技能）
			if(NULL!=pChildElem->Attribute("skillstate"))
				stSkill.dwSkillState = atoi(pChildElem->Attribute("skillstate"));
			// 技能属性
			if(NULL!=pChildElem->Attribute("skillelement"))
				stSkill.dwSkillElement = atoi(pChildElem->Attribute("skillelement"));
			// 技能类型
			if(NULL!=pChildElem->Attribute("skilltype"))
				stSkill.dwSkillType = atoi(pChildElem->Attribute("skilltype"));
			// 技能使用类型(物理、魔法)
			if(NULL!=pChildElem->Attribute("skillusetype"))
				stSkill.dwSkillUseType = atoi(pChildElem->Attribute("skillusetype"));
			// 技能描述
			if(NULL!=pChildElem->Attribute("skilldesc"))
				stSkill.strSkillDesc = pChildElem->Attribute("skilldesc");
			// 是否锁定目标
			if(NULL!=pChildElem->Attribute("locktarget"))
				stSkill.bLockTarget = atoi(pChildElem->Attribute("locktarget")) == 0?true : false;
			// 是否是锁定技能
			if(NULL!=pChildElem->Attribute("lockskill"))
				stSkill.bLockSkill =atoi(pChildElem->Attribute("lockskill")) == 0?true:false;
			// 技能等级队列
			for (TiXmlElement *pSkillChildElem = pChildElem->FirstChildElement(); pSkillChildElem!=NULL; pSkillChildElem = pSkillChildElem->NextSiblingElement())
			{
				// 可用职业队列
				if(!strcmp(pSkillChildElem->Value(), "UseByOcc"))
				{
					for (TiXmlElement *pSkillOccElem = pSkillChildElem->FirstChildElement(); pSkillOccElem != NULL; pSkillOccElem = pSkillOccElem->NextSiblingElement())
					{
						tagSkillOcc stOcc;
						// 职业名称
						if(NULL != pSkillOccElem->Attribute("occname"))
							stOcc.strOccName = pSkillOccElem->Attribute("occname");
						// 职业编号
						if(NULL != pSkillOccElem->Attribute("occid"))
							stOcc.dwOccId = atoi(pSkillOccElem->Attribute("occid"));
						stSkill.vecUseByOcc.push_back(stOcc);
					}
				}
				else if(!strcmp(pSkillChildElem->Value(),"Level"))
				{
					tagLevel stlevel;
					// 技能等级
					if(NULL!=pSkillChildElem->Attribute("skilllevel"))
						stlevel.dwSkill_Level = atoi(pSkillChildElem->Attribute("skilllevel"));
					// 技能等级描述
					if(NULL!=pSkillChildElem->Attribute("skillleveldesc"))
						stlevel.strSkillLevelDesc = pSkillChildElem->Attribute("skillleveldesc");
					// 技能等级附加描述
					if(NULL!=pSkillChildElem->Attribute("skillleveldescEx"))
						stlevel.strSkillLevelDescEx = pSkillChildElem->Attribute("skillleveldescEx");
					if(NULL!=pSkillChildElem->Attribute("skilllevelai"))
						stlevel.dwSkillLevelAI = atoi(pSkillChildElem->Attribute("skilllevelai"));
					for(TiXmlElement *pStepChildElem = pSkillChildElem->FirstChildElement(); pStepChildElem!=NULL; pStepChildElem = pStepChildElem->NextSiblingElement())
					{
						tagLevelValue stLevelValue;
						// 发送技能验证阶段
						if(!strcmp(pStepChildElem->Value(),"StepSend"))
						{
							for(TiXmlElement *pStepSendElem = pStepChildElem->FirstChildElement(); pStepSendElem!=NULL; pStepSendElem = pStepSendElem->NextSiblingElement())
							{
								if(NULL!=pStepSendElem->Attribute("value"))
								{
									stLevelValue.strKeyWord = pStepSendElem->Value();
									stLevelValue.dwID = atoi(pStepSendElem->Attribute("id"));
									stLevelValue.dwValue = atoi(pStepSendElem->Attribute("value"));
									stlevel.vecStepSend.push_back(stLevelValue);
								}
							}
						}
						// 技能开始阶段
						else if(!strcmp(pStepChildElem->Value(),"StepBegin"))
						{
							for (TiXmlElement *pStepBeginElem = pStepChildElem->FirstChildElement(); pStepBeginElem!=NULL; pStepBeginElem = pStepBeginElem->NextSiblingElement())
							{
								if (NULL!=pStepBeginElem->Attribute("value"))
								{
									int ID = atoi(pStepBeginElem->Attribute("id"));
									if(INTONATE_ACT == ID || RELEASE_ACT == ID || INTONATE_MAINACT == ID)
										stSkill.bHasAct = true;
									stLevelValue.strKeyWord = pStepBeginElem->Value();
									stLevelValue.dwID = ID;
									stLevelValue.dwValue = atoi(pStepBeginElem->Attribute("value"));
									stlevel.vecStepBegin.push_back(stLevelValue);
								}
							}
						}
						// 技能使用成功阶段
						else if (!strcmp(pStepChildElem->Value(),"StepRun"))
						{
							for (TiXmlElement *pStepRunElem = pStepChildElem->FirstChildElement(); pStepRunElem!=NULL; pStepRunElem = pStepRunElem->NextSiblingElement())
							{
								if(NULL!= pStepRunElem->Attribute("value"))
								{
									int ID = atoi(pStepRunElem->Attribute("id"));
									if(INTONATE_ACT == ID || RELEASE_ACT == ID || INTONATE_MAINACT == ID)
										stSkill.bHasAct = true;
									stLevelValue.strKeyWord = pStepRunElem->Value();
									stLevelValue.dwID = ID;
									stLevelValue.dwValue = atoi(pStepRunElem->Attribute("value"));
									stlevel.vecStepRun.push_back(stLevelValue);
								}
							}
						}
						// 技能结束阶段
						else if(!strcmp(pStepChildElem->Value(),"StepEnd"))
						{
							for (TiXmlElement *pStepEndElem = pStepChildElem->FirstChildElement();pStepEndElem!=NULL; pStepEndElem = pStepEndElem->NextSiblingElement())
							{
								if(NULL!= pStepEndElem->Attribute("value"))
								{
									int ID =atoi(pStepEndElem->Attribute("id"));
									if(INTONATE_ACT == ID || RELEASE_ACT == ID || INTONATE_MAINACT == ID)
										stSkill.bHasAct = true;
									stLevelValue.strKeyWord = pStepEndElem->Value();
									stLevelValue.dwID = ID;
									stLevelValue.dwValue = atoi(pStepEndElem->Attribute("value"));
									stlevel.vecStepEnd.push_back(stLevelValue);
								}
							}
						}
						// 召唤技能阶段
						else
						{
							for (TiXmlElement *pStepSummonElem = pStepChildElem->FirstChildElement();pStepSummonElem!=NULL; pStepSummonElem = pStepSummonElem->NextSiblingElement())
							{
								if(NULL!= pStepSummonElem->Attribute("value"))
								{
									int ID = atoi(pStepSummonElem->Attribute("id"));
									if(INTONATE_ACT == ID || RELEASE_ACT == ID || INTONATE_MAINACT == ID)
										stSkill.bHasAct = true;
									stLevelValue.strKeyWord = pStepSummonElem->Value();
									stLevelValue.dwID = ID;
									stLevelValue.dwValue = atoi(pStepSummonElem->Attribute("value"));
									stlevel.vecStepSummon.push_back(stLevelValue);
								}
							}
						}
						/*else // 技能持有状态特效
						{
						for (TiXmlElement *pStepStateElem = pStepChildElem->FirstChildElement();pStepStateElem!=NULL; pStepStateElem = pStepStateElem->NextSiblingElement())
						{
						if(NULL!= pStepStateElem->Attribute("value"))
						{
						stLevelValue.strKeyWord = pStepStateElem->Value();
						stLevelValue.dwValue = CStrConverter::parseInt(pStepStateElem->Attribute("value"));
						stlevel.vecStepState.push_back(stLevelValue);
						}
						}
						}*/
					}
					stSkill.vectorLevels.push_back(stlevel);
				}
			}
			m_mapSkillListXml[stSkill.dwSkillID] = stSkill;
		}
	}
	doc.Clear();
	return true;
}



//void CSkillInfo::ReleaseSkill()
//{
//	m_mapSkillListXml.clear();
//	//if( !m_OccupList.empty())
//	//{
//	//	if (!m_vecOccupSkillList.empty())
//	//	{
//	//		vector<tagOccupSkill*>::iterator pSkill = m_vecOccupSkillList.begin();
//	//		vector<tagOccupSkill*>::iterator pTemp;
//	//		for (;pSkill!=m_vecOccupSkillList.end();)
//	//		{
//	//			pTemp = pSkill;
//	//			++pSkill;
//	//			delete *pTemp;
//	//		}
//	//		m_vecOccupSkillList.clear();
//	//	}
//
//	//	list<tagOccup*>::iterator pOccup = m_OccupList.begin();
//	//	list<tagOccup*>::iterator pTemp2;
//	//	for (;pOccup!=m_OccupList.end();)
//	//	{
//	//		pTemp2 = pOccup;
//	//		++pOccup;
//	//		delete *pTemp2;
//	//	}
//	//	m_OccupList.clear();
//	//}
//
//}
//// 初始化，读XML配置文件
//bool CSkillInfo::Init(const char *filename)
//{
//	if(!filename)	return false;
//	Release();
//	char strError[128];
//	TiXmlNode *pNode = NULL;
//	TiXmlDocument m_Tdoc;
//	//输出错误信息：错误的文件名
//	if(!m_Tdoc.LoadFile(filename))
//	{
//		wsprintf(strError,"载入配置文件%s失败,请检查",filename);
//		MessageBox(NULL,strError,"",MB_OK);
//		return false;
//	}
//	//找到根节点，如果无节点，输出错误提示
//	pNode = m_Tdoc.FirstChild("OccupSkillList");
//	if (pNode==NULL)
//	{
//		wsprintf(strError,"没发现OccupSkillList节点,请检查");
//		MessageBox(NULL,strError,"",MB_OK); 
//		return false;
//	}
//	//类型转换
//	TiXmlElement* pElem = pNode->ToElement();
//	// 便利子节点，设置属性
//	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
//		pChildElem=pChildElem->NextSiblingElement())
//	{
//		if(!strcmp(pChildElem->Value(),"Occup")
//			&& NULL!=pChildElem->Attribute("name")
//			&& NULL!=pChildElem->Attribute("ID"))
//		{
//			COccupSkill * pOccupSkill = new COccupSkill;
//			pOccupSkill->SetName(pChildElem->Attribute("name"));
//			pOccupSkill->SetOccupId((long)atoi(pChildElem->Attribute("ID")));
//			// 职业描述
//			if(NULL!=pChildElem->Attribute("Desc"))
//			{
//				pOccupSkill->SetDesc(pChildElem->Attribute("Desc"));
//			}
//			// 读每个职业的技能列表
//			if(pOccupSkill->Init(pChildElem))
//				m_OccupSkillList.push_back(pOccupSkill);
//			// 读取职业技能列表失败
//			else
//			{
//				delete pOccupSkill;
//				pOccupSkill = NULL;
//			}
//		}
//	}
//	m_Tdoc.Clear();
//	return true;
//}
//

//
//BOOL CSkillInfo::LoadSkillListXml()
//{
//	ReleaseSkill();
//	string strFile = "Data\\OccupSkillList.xml";
//	TiXmlNode *pNode = NULL;
//	TiXmlDocument doc(strFile.c_str());
//	if( !doc.LoadFile())
//	{
//		PutDebugString("装载配置文件<<OccupSkillList.xml>>出错！");
//		return FALSE;
//	}
//	//找到根节点，如果无节点，输出错误提示
//	pNode = doc.FirstChild("OccupSkillList");
//	if (pNode==NULL)
//	{
//		PutDebugString("没发现OccupSkillList节点,请检查！");
//		return FALSE;
//	}
//	//类型转换
//	TiXmlElement* pElem = pNode->ToElement();
//	// 便利子节点，设置属性
//	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
//		pChildElem=pChildElem->NextSiblingElement())
//	{
//		if(!strcmp(pChildElem->Value(),_T("Occup"))
//			&& NULL!=pChildElem->Attribute(_T("name"))
//			&& NULL!=pChildElem->Attribute(_T("ID")))
//		{
//			tagOccup * pOccupSkill = new tagOccup;
//			pOccupSkill->strOccName = pChildElem->Attribute("name");
//			pOccupSkill->dwOccID = (long)atoi(pChildElem->Attribute("ID"));
//			// 职业描述
//			if(NULL!=pChildElem->Attribute("Desc"))
//			{
//				pOccupSkill->strOccDesc = (pChildElem->Attribute("Desc"));
//			}
//			// 读每个职业的技能列表
//			if(LoadSkillListXml())
//				m_OccupList.push_back(pOccupSkill);
//			// 读取职业技能列表失败
//			else
//			{
//				delete pOccupSkill;
//				pOccupSkill = NULL;
//			}
//		}
//	}
//	doc.Clear();
//}
//
//// 读取所有技能配置文件
//BOOL CSkillInfo::LoadSkillListXml()
//{
//	HANDLE hFind;
//	CString filename;
//	CString filepath = "Data\\skills\\";
//	WIN32_FIND_DATA dataFind;
//	BOOL bMoreFile = TRUE;
//	hFind = FindFirstFile(filepath+"\\*.*",&dataFind);
//	while(hFind!=INVALID_HANDLE_VALUE && bMoreFile ==TRUE)
//	{
//		filename = filepath +"\\" + dataFind.cFileName;
//		if(filename.Right(4) == ".xml")
//		{
//			m_OneSkillXmlFileName.push_back(filename);
//			LoadOneSkillXml(filename);
//		}
//		bMoreFile = FindNextFile(hFind,&dataFind);
//	}
//	return true;
//}
//
//// 读取单个技能配置文件
//BOOL CSkillInfo::LoadOneSkillXml(const char* filename)
//{
//	
//	TiXmlElement	*m_pElem=NULL;
//	TiXmlNode *pNode = NULL;
//	TiXmlDocument doc(filename);
//	doc.LoadFile();
//
//	pNode = doc.FirstChild("Root");
//	if(NULL==pNode)
//	{
//		return false;
//	}
//	m_pElem = pNode->ToElement();
//	TiXmlElement *pChildElem = m_pElem->FirstChildElement();
//	for(; pChildElem!=NULL; pChildElem = pChildElem->NextSiblingElement())
//	{
//		// 技能节点
//		if(!strcmp(pChildElem->Value(),_T("Skill")))
//		{
//			tagSkill stSkill;
//			// 技能编号
//			if(NULL!=pChildElem->Attribute(_T("id")))
//				stSkill.dwSkillID = atoi(pChildElem->Attribute(_T("id")));
//			// 技能名称
//			if(NULL!=pChildElem->Attribute(_T("name")))
//				stSkill.strSkillName = pChildElem->Attribute(_T("name"));
//			//// 技能状态（一般技能、光环类技能）
//			if(NULL!=pChildElem->Attribute(_T("skillstate")))
//				stSkill.dwSkillState = atoi(pChildElem->Attribute(_T("skillstate")));
//			// 技能属性
//			if(NULL!=pChildElem->Attribute(_T("skillelement")))
//				stSkill.dwSkillElement = atoi(pChildElem->Attribute(_T("skillelement")));
//			// 技能类型
//			if(NULL!=pChildElem->Attribute(_T("skilltype")))
//				stSkill.dwSkillType = atoi(pChildElem->Attribute(_T("skilltype")));
//			// 技能使用类型(物理、魔法)
//			if(NULL!=pChildElem->Attribute(_T("skillusetype")))
//				stSkill.dwSkillUseType = atoi(pChildElem->Attribute(_T("skillusetype")));
//			// 技能描述
//			if(NULL!=pChildElem->Attribute(_T("skilldesc")))
//				stSkill.strSkillDesc = pChildElem->Attribute(_T("skilldesc"));
//			// 是否锁定目标
//			if(NULL!=pChildElem->Attribute(_T("locktarget")))
//				stSkill.bLockTarget = pChildElem->Attribute(_T("locktarget"))==0?false:true;
//			// 是否是锁定技能
//			if(NULL!=pChildElem->Attribute(_T("lockskill")))
//				stSkill.bLockSkill = pChildElem->Attribute(_T("lockskill"))==0?true:false;
//			// 技能等级队列
//			for (TiXmlElement *pSkillChildElem = pChildElem->FirstChildElement(); pSkillChildElem!=NULL; pSkillChildElem = pSkillChildElem->NextSiblingElement())
//			{
//				// 可用职业队列
//				if(!strcmp(pSkillChildElem->Value(), _T("UseByOcc")))
//				{
//					for (TiXmlElement *pSkillOccElem = pSkillChildElem->FirstChildElement(); pSkillOccElem != NULL; pSkillOccElem = pSkillOccElem->NextSiblingElement())
//					{
//						tagSkillOcc stOcc;
//						// 职业名称
//						if(NULL != pSkillOccElem->Attribute(_T("occname")))
//							stOcc.strOccName = pSkillOccElem->Attribute(_T("occname"));
//						// 职业编号
//						if(NULL != pSkillOccElem->Attribute(_T("occid")))
//							stOcc.dwOccId = atoi(pSkillOccElem->Attribute(_T("occid")));
//						stSkill.vecUseByOcc.push_back(stOcc);
//					}
//				}
//				else if(!strcmp(pSkillChildElem->Value(),_T("Level")))
//				{
//					tagLevel stlevel;
//					// 技能等级
//					if(NULL!=pSkillChildElem->Attribute(_T("skilllevel")))
//						stlevel.dwSkill_Level = atoi(pSkillChildElem->Attribute(_T("skilllevel")));
//					// 技能等级描述
//					if(NULL!=pSkillChildElem->Attribute(_T("skillleveldesc")))
//						stlevel.strSkillLevelDesc = pSkillChildElem->Attribute(_T("skillleveldesc"));
//					// 技能等级附加描述
//					if(NULL!=pSkillChildElem->Attribute(_T("skillleveldescEx")))
//						stlevel.strSkillLevelDescEx = pSkillChildElem->Attribute(_T("skillleveldescEx"));
//					if(NULL!=pSkillChildElem->Attribute(_T("skilllevelai")))
//						stlevel.dwSkillLevelAI =atoi(pSkillChildElem->Attribute(_T("skilllevelai")));
//					for(TiXmlElement *pStepChildElem = pSkillChildElem->FirstChildElement(); pStepChildElem!=NULL; pStepChildElem = pStepChildElem->NextSiblingElement())
//					{
//						tagLevelValue stLevelValue;
//						// 发送技能验证阶段
//						if(!strcmp(pStepChildElem->Value(),_T("StepSend")))
//						{
//							for(TiXmlElement *pStepSendElem = pStepChildElem->FirstChildElement(); pStepSendElem!=NULL; pStepSendElem = pStepSendElem->NextSiblingElement())
//							{
//								if(NULL!=pStepSendElem->Attribute(_T("value")))
//								{
//									stLevelValue.strKeyWord = pStepSendElem->Value();
//									stLevelValue.dwID = atoi(pStepSendElem->Attribute(_T("id")));
//									stLevelValue.dwValue = atoi(pStepSendElem->Attribute(_T("value")));
//									stlevel.vecStepSend.push_back(stLevelValue);
//								}
//							}
//						}
//						// 技能开始阶段
//						else if(!strcmp(pStepChildElem->Value(),_T("StepBegin")))
//						{
//							for (TiXmlElement *pStepBeginElem = pStepChildElem->FirstChildElement(); pStepBeginElem!=NULL; pStepBeginElem = pStepBeginElem->NextSiblingElement())
//							{
//								if (NULL!=pStepBeginElem->Attribute(_T("value")))
//								{
//									stLevelValue.strKeyWord = pStepBeginElem->Value();
//									stLevelValue.dwID = atoi(pStepBeginElem->Attribute(_T("id")));
//									stLevelValue.dwValue = atoi(pStepBeginElem->Attribute(_T("value")));
//									stlevel.vecStepBegin.push_back(stLevelValue);
//								}
//							}
//						}
//						// 技能使用成功阶段_T(
//						else if (!strcmp(pStepChildElem->Value(),_T("StepRun")))
//						{
//							for (TiXmlElement *pStepRunElem = pStepChildElem->FirstChildElement(); pStepRunElem!=NULL; pStepRunElem = pStepRunElem->NextSiblingElement())
//							{
//								if(NULL!= pStepRunElem->Attribute(_T("value")))
//								{
//									stLevelValue.strKeyWord = pStepRunElem->Value();
//									stLevelValue.dwID = atoi(pStepRunElem->Attribute(_T("id")));
//									stLevelValue.dwValue = atoi(pStepRunElem->Attribute(_T("value")));
//									stlevel.vecStepRun.push_back(stLevelValue);
//								}
//							}
//						}
//						// 技能结束阶段
//						else if(!strcmp(pStepChildElem->Value(),_T("StepEnd")))
//						{
//							for (TiXmlElement *pStepEndElem = pStepChildElem->FirstChildElement();pStepEndElem!=NULL; pStepEndElem = pStepEndElem->NextSiblingElement())
//							{
//								if(NULL!= pStepEndElem->Attribute(_T("value")))
//								{
//									stLevelValue.strKeyWord = pStepEndElem->Value();
//									stLevelValue.dwID = atoi(pStepEndElem->Attribute(_T("id")));
//									stLevelValue.dwValue = atoi(pStepEndElem->Attribute(_T("value")));
//									stlevel.vecStepEnd.push_back(stLevelValue);
//								}
//							}
//						}
//						// 召唤技能阶段
//						else
//						{
//							for (TiXmlElement *pStepSummonElem = pStepChildElem->FirstChildElement();pStepSummonElem!=NULL; pStepSummonElem = pStepSummonElem->NextSiblingElement())
//							{
//								if(NULL!= pStepSummonElem->Attribute(_T("value")))
//								{
//									stLevelValue.strKeyWord = pStepSummonElem->Value();
//									stLevelValue.dwID = atoi(pStepSummonElem->Attribute(_T("id")));
//									stLevelValue.dwValue = atoi(pStepSummonElem->Attribute(_T("value")));
//									stlevel.vecStepSummon.push_back(stLevelValue);
//								}
//							}
//						}
//					}
//					stSkill.vectorLevels.push_back(stlevel);
//				}
//			}
//			m_SkillListXml.push_back(stSkill);
//		}
//	}
//	doc.Clear();
//	return true;
//}
//
void CSkillInfo::SetDisplaySkill(CPlayer* pPlayer)
{
	if ( pPlayer == NULL ) return;
	m_CtrSkillList.DeleteAllItems();
	CString strSkill;
	CString strOccupType1("");
	CString strOccupType2("");
	//职业
	long lOccupType = pPlayer->GetOccupation();
	strOccupType1.Format("%d",lOccupType);

	//list<tagSkill>::iterator OccSkill;																			//配置文件中的所有技能

	list<tagPlayerSkill>::iterator pStudyedSkill = pPlayer->GetSkillMgr()->GetSkillList().begin();					//玩家学会的技能

	int i =0 ;

	for ( ; pStudyedSkill!=pPlayer->GetSkillMgr()->GetSkillList().end(); ++pStudyedSkill)
	{
// 		for( OccSkill = GetOneSkillXmlProperty().begin() ;OccSkill!=GetOneSkillXmlProperty().end();++OccSkill)
// 		{
// 			if ( pStudyedSkill->dwID == OccSkill->dwSkillID )
// 			{
// 				if (pStudyedSkill->dwID>=10002 && pStudyedSkill->dwID <=10010)
// 					continue;
// 				m_CtrSkillList.InsertItem(i,OccSkill->strSkillName.c_str());
// 				char str[8] = "";
// 				sprintf(str,"%d",pStudyedSkill->wLevel);
// 				m_CtrSkillList.SetItemText(i,1,str);
// 				break;
// 			}
//		}
		char str[8] = "";
		sprintf(str,"%d",pStudyedSkill->dwID);
		m_CtrSkillList.SetItemText(i,1,str);
	}
}

BOOL CSkillInfo::QuerySkill(tagPlayerSkill* pSkill)
{
	for (vector<tagOccupSkill*>::iterator it = m_vecOccSkill.begin(); it!= m_vecOccSkill.end(); ++it )
	{
		if ( (*it)->lSkillID == pSkill->dwID )
		{
			return TRUE;
		}
	}
	return FALSE;
}