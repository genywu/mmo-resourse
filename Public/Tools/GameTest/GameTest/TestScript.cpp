//=============================================================================
/**
 *  file: TestScript.cpp
 *
 *  Brief:测试管理、脚本编辑
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================

#include "stdafx.h"
#include "GameTest.h"
#include "GameTestDlg.h"
#include "TestScript.h"
#include "WndMsgDef.h"
#include ".\testscript.h"
#include "../Script/ScriptSys.h"
#include "AddNameDlg.h"
#include "..\Script\MapScriptSystem.h"
#include <afxdlgs.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CTestScript 对话框
void DeleteFile(char* strDir);
IMPLEMENT_DYNAMIC(CTestScript, CDialog)
CTestScript::CTestScript(CWnd* pParent /*=NULL*/)
	: CDialog(CTestScript::IDD, pParent)
	, m_edScript(_T(""))
{
	m_lCaseNum = 0;
	m_csCurrtWorkPath = "";
}

CTestScript::~CTestScript()
{
	itTestCase it = m_TestCases.begin();
	for(;it != m_TestCases.end();it++)
	{
		delete (*it).second;
	}
	m_TestCases.clear();
}

void CTestScript::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_StartCDKey, m_edStartCDKey);
	DDX_Control(pDX, IDC_EndCDKey, m_edEndCDKey);
	DDX_Control(pDX, IDC_EDIT_Timer, m_edTimer);

	DDX_Control(pDX, IDC_TREE_TEST_CASE, m_treeTestCase);
	DDX_Control(pDX, IDC_EDIT_SCRIPT_EDIT, m_edScriptEdit);
	DDX_Control(pDX, IDC_EDIT_SCRIPT_ERROR_INFO, m_edScriptInfo);
	DDX_Control(pDX, IDC_ShowScriptPath,m_ctlShowScriptPath);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_EDIT, m_edScript);
	DDX_Text(pDX, IDC_StartCDKey,	m_lStartCDKey);
	DDX_Text(pDX, IDC_EndCDKey,		m_lEndCDKey);
	DDX_Control(pDX, IDC_COMBO_ServerList, m_ctrlServerList);
	DDX_Control(pDX, IDC_COMBO_RegionList, m_ctrlRegionList);
	DDX_Control(pDX, IDC_COMBO_OccuList, m_ctrlOcculist);

	DDX_Control(pDX, IDC_COMBO_TestType, m_ctrlTestType);
	DDX_Text(pDX, IDC_EDIT_Timer,m_lTimer);
	//DDX_Text(pDX, IDC_EDIT_DelayTime,m_lSendDelayTime);
	//DDV_MinMaxLong(pDX, m_lTimer,0,60000);
	DDX_Control(pDX, IDC_CHECK_PtToken, m_btnChkPtToken);
	DDX_Control(pDX, IDC_StartLogin, m_bnStartLogin);
	DDX_Control(pDX, IDC_PLAYERLEVEL, m_edPlayerLevel);
	//DDX_Text(pDX, IDC_PLAYERRGN, m_edPlayerRgn);
	DDX_Control(pDX, IDC_MAXSTATE, m_bnMaxState);
	DDX_Control(pDX, IDC_EQUIPWEAPON, m_bnEquipWeapon);
	DDX_Control(pDX, IDC_THIRDOCCU, m_bnThirdOccu);
	DDX_Text(pDX, IDC_PLAYERLEVEL, m_lPlayerLevel);
	DDX_Text(pDX, IDC_PLAYERRGNX, m_lPlayerX);
	DDX_Text(pDX, IDC_PLAYERRGNY, m_lPlayerY);
	DDX_Control(pDX, IDC_PLAYERRGNX, m_edPlayerX);
	DDX_Control(pDX, IDC_PLAYERRGNY, m_edPlayerY);

	//DDX_Text(pDX, IDC_PLAYERRGN, m_lPlayerRgn);
}


BEGIN_MESSAGE_MAP(CTestScript, CDialog)
	ON_WM_CTLCOLOR()
	//按钮
	ON_BN_CLICKED(IDC_StartLogin,					OnBnClickedButtonStartLogin)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_SCRIPT_SYNTAX,	OnCheckScriptSyntax)
	ON_BN_CLICKED(IDC_UpdateScript,					OnCheckUpdateScript)
	ON_BN_CLICKED(IDC_THIRDOCCU,					OnCheckThirdOccu)
	ON_BN_CLICKED(IDC_BUTTON_TESTDLG_CHOOSEPATH,	OnCheckSetWorkPath)
	ON_BN_CLICKED(IDC_CLOSE_GT,						OnCheckCloseGT)
	
	//ON_BN_CLICKED(IDC_OpenScript,					OnCheckOpenScript)
	//菜单类消息
	ON_NOTIFY(NM_RCLICK, IDC_TREE_TEST_CASE,		OnNMRclickTreeTestCase)
	ON_BN_CLICKED(IDC_SaveScript,					OnBnClickedSavescript)
	ON_COMMAND(ID_AddScript,						OnAddscript)
	ON_COMMAND(ID_AddCase,							OnAddCase)
	ON_COMMAND(ID_DelScript,						OnDelScript)
	ON_COMMAND(ID_CASE_START,						OnCaseStartTest)
	ON_COMMAND(ID_CASE_STOP,						OnCaseStopTest)
	//
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_TEST_CASE,OnTvnSelChangedTree)

	ON_BN_CLICKED(IDC_CHECK_PtToken, &CTestScript::OnBnClickedCheckPtToken)
	END_MESSAGE_MAP()




/*
=========================================================================
函数功能:控件颜色重载  By: LGR
=========================================================================
*/
HBRUSH CTestScript::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case
IDC_EDIT_SCRIPT_ERROR_INFO:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(LIGHTRED);
			HBRUSH b = CreateSolidBrush(WHITE);
			return (HBRUSH)b;
		}
	default:
		return CWnd::OnCtlColor(pDC,pWnd,nCtlColor);	
	}		
}


void CTestScript::LoadCase(TiXmlElement *caseElem)
{
	static int i = 0;
	tagTestCase* pCase = new tagTestCase;
	pCase->strCaseName = caseElem->Attribute("name");

	TiXmlElement* scriptElem = caseElem->FirstChildElement();
	for( ;scriptElem != NULL; scriptElem = scriptElem->NextSiblingElement())
	{
		if(!strcmp(scriptElem->Value(),"start"))
		{
			pCase->strStartScriptName = scriptElem->Attribute("scriptname");
		}
		else if(!strcmp(scriptElem->Value(),"end"))
		{
			pCase->strEndScriptName = scriptElem->Attribute("scriptname");
		}
	}
	m_TestCases[i++] = pCase;
}

void CTestScript::LoadCaseSetup()
{
	//载入配置文件
	char strfile[512] = "TestCase\\TestCases.xml";
	TiXmlDocument doc(strfile);
	if(!doc.LoadFile())
	{
		PutDebugString("装载配置文件<TestCases.xml>出错！");
		return;
	}
	//读配置
	TiXmlNode *pTestNode = NULL;
	pTestNode = doc.FirstChild("Test");
	if(pTestNode)
	{
		TiXmlElement* caseElem = pTestNode->FirstChildElement();
		for(;caseElem!=NULL;caseElem = caseElem->NextSiblingElement())
		{
			if(!strcmp(caseElem->Value(),"case"))
			{
				LoadCase(caseElem);
			}
		}
	}
}

//初始化树的内容
void CTestScript::InitTree()
{
	if(m_TestCases.empty())
	{
		return;
	}
	
	//Root节点
	char strRootText[256]="";
	sprintf(strRootText,"所有案例(%d)",m_TestCases.size());

	HTREEITEM hRootItem = m_treeTestCase.InsertItem(strRootText);
	m_treeTestCase.SetItemData(hRootItem,-1);
	HTREEITEM hCaseItem = NULL;
	HTREEITEM hScriptItem = NULL;
	//遍历case
	map<long,tagTestCase*>::iterator it = m_TestCases.begin();
	for(;it!=m_TestCases.end();++it,m_lCaseNum++)
	{
		tagTestCase* pCase = (*it).second;

		hCaseItem = m_treeTestCase.InsertItem(pCase->strCaseName.c_str(),hRootItem);
		m_treeTestCase.SetItemData(hCaseItem,m_lCaseNum);
		
		HANDLE	hFile;
		WIN32_FIND_DATA	dataFile;
		BOOL	bMoreFile = TRUE;
		//
		string strFilePath = "TestCase\\";
		strFilePath += pCase->strCaseName;
		hFile = FindFirstFileA((strFilePath + "\\*.*").c_str(),&dataFile);
		while( hFile != INVALID_HANDLE_VALUE && bMoreFile )
		{
			string strFileName = strFilePath + "\\" + dataFile.cFileName;
			size_t len = strlen(strFileName.c_str()) - 4;
			if( !strcmp(&strFileName.c_str()[len],".lua"))
			{
				hScriptItem = m_treeTestCase.InsertItem(strFileName.c_str(),hCaseItem);
				m_treeTestCase.SetItemData(hScriptItem,m_lCaseNum);
			}
			bMoreFile = FindNextFile(hFile,&dataFile);
		}
	}
}

BOOL CTestScript::OnInitDialog()
{
	CDialog::OnInitDialog();
	//读取测试案例配置文件
	LoadCaseSetup();
	InitTree();
	InitTestConfigInfo();
	InitServerList();
	InitTestType();
	InitRegionList();
	InitOccuList();
	InitProjList();


//	this->GetDlgItem(IDC_UpdateScript)->EnableWindow(FALSE);

	m_edScriptInfo.LimitText(10);

	m_ctrlTestType.SetCurSel(m_mapTestConfigRecord[0]);
	m_ctrlServerList.SetCurSel(m_mapTestConfigRecord[1]);

	char cDisplay[256];
	sprintf(cDisplay,"%d",m_mapTestConfigRecord[2]);
	m_edStartCDKey.SetWindowText(cDisplay);

	sprintf(cDisplay,"%d",m_mapTestConfigRecord[3]);
	m_edEndCDKey.SetWindowText(cDisplay);

	sprintf(cDisplay,"%d",m_mapTestConfigRecord[4]);
	m_edTimer.SetWindowText(cDisplay);

	sprintf(cDisplay,"%d",m_mapTestConfigRecord[5]);
	m_edPlayerLevel.SetWindowText(cDisplay);

	m_ctrlOcculist.SetCurSel(m_mapTestConfigRecord[6]);

	m_bnThirdOccu.SetCheck(m_mapTestConfigRecord[7]);
	if (m_mapTestConfigRecord[7] == 1)
		m_ctrlOcculist.EnableWindow(0);

	m_ctrlRegionList.SetCurSel(m_mapTestConfigRecord[8]);

	sprintf(cDisplay,"%d",m_mapTestConfigRecord[9]);
	m_edPlayerX.SetWindowText(cDisplay);

	sprintf(cDisplay,"%d",m_mapTestConfigRecord[10]);
	m_edPlayerY.SetWindowText(cDisplay);


	m_bnEquipWeapon.SetCheck(m_mapTestConfigRecord[11]);

	m_bnMaxState.SetCheck(m_mapTestConfigRecord[12]);

	((CComboBox*)GetDlgItem(IDC_COMBO_TDLG_PROJNAME))->SetCurSel(m_mapTestConfigRecord[13]);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

bool CTestScript::ReadTo(ifstream& inf,const char* flag)
{
	string str;
	inf >> str;
	while( str != flag )
	{
		if(inf.eof())
			return false;
		inf >> str;
	}
	return true;
}


//初始化服务器列表
BOOL CTestScript::InitServerList()
{
	m_vecServer.clear();

	//读取配置文件
	string filename = "setup\\serverlist.ini";
	ifstream inf;
	inf.open(filename.c_str());
	if(inf.fail())
	{
		return FALSE;
	}
	while( ReadTo(inf,"#"))
	{
		string strIp, serverName;
		DWORD  dwPort;
		inf >> strIp
			>> dwPort
			>> serverName;

		tagServerList serList;
		serList.dwPort = dwPort;
		serList.name = serverName;
		serList.strIp = strIp;
		m_vecServer.push_back(serList);
	}
	//读入配置到combo
	int i=0;
	for(vector<tagServerList>::iterator it = m_vecServer.begin(); it != m_vecServer.end(); it++)
	{

		m_ctrlServerList.InsertString(i++,(*it).name.c_str());
	}
	
	inf.close();
	return TRUE;
}

//初始化测试的类型
bool CTestScript::InitTestType(void)
{
	m_mapTestType.clear();
	//读取配置文件
	string filename = "setup\\testtype.ini";
	ifstream inf;
	inf.open(filename.c_str());
	if(inf.fail())
	{
		return false;
	}

	int i = 0;
	while(ReadTo(inf,"#"))
	{
		string strTestName;
		DWORD dwTestType;
		inf >> strTestName
			>> dwTestType;

		m_ctrlTestType.InsertString(i,strTestName.c_str());
		m_mapTestType[i++] = dwTestType;
	}
	
	inf.close();
	return true;	
}

//新加界面记录 by:LGR
bool CTestScript::InitTestConfigInfo(void)
{
	m_mapTestConfigRecord.clear();
	//读取配置文件
	const string filename = "setup\\ConfigRecord.ini";
	ifstream inf;
	inf.open(filename.c_str());
	if(inf.fail())
	{
		return false;
	}

	int index = 0;
	while(ReadTo(inf,"#"))
	{
		long lTestNo;
		
		inf >> lTestNo;
		m_mapTestConfigRecord[index++] = lTestNo;
	}
	//m_mapTestConfigRecord.SetCurSel(0);
	inf.close();
	return true;
}


//初始化场景列表 by: LGR
BOOL CTestScript::InitRegionList()
{
	m_vecRegion.clear();
	//读取配置文件
	string filename = "setup\\regionlist.ini";
	ifstream inf;
	inf.open(filename.c_str());
	if(inf.fail())
	{
		return FALSE;
	}
	while( ReadTo(inf,"#"))
	{
		string name,RgnGuid;
		int type,expScale,OccuExpScale,spScale,TYPE,noPK,no,GS_ID,Country,waterRgn,LinedIdFlag;
		long id, ResourceID,AreaId,RgnInfo;

		inf >> type
			>> id
			>> ResourceID
			>> AreaId
			>> expScale
			>> OccuExpScale
			>> spScale
			>> TYPE
			>> noPK
			>> no
			>> name
			>> GS_ID
			>> Country
			>> RgnInfo
			>> waterRgn
			>> RgnGuid
			>> LinedIdFlag;
		tagRegionList rgnList;	
		rgnList.type = type;
		rgnList.id = id;
		rgnList.ResourceID = ResourceID;
		rgnList.AreaId = AreaId;
		rgnList.expScale = expScale;
		rgnList.OccuExpScale = OccuExpScale;
		rgnList.spScale =  spScale;
		rgnList.TYPE = TYPE;
		rgnList.noPK = noPK;
		rgnList.no = no;
		rgnList.name = name;
		rgnList.GS_ID = GS_ID;
		rgnList.Country = Country;
		rgnList.RgnInfo = RgnInfo;
		rgnList.waterRgn = waterRgn;
		rgnList.RgnGuid = RgnGuid;
		rgnList.LinedIdFlag = LinedIdFlag;
		m_vecRegion.push_back(rgnList);
	}
	//读入配置到combo
	int i=0;
	for(vector<tagRegionList>::iterator it = m_vecRegion.begin(); it != m_vecRegion.end(); it++)
	{
		m_ctrlRegionList.InsertString(i++,(*it).name.c_str());
	}

	inf.close();
	return TRUE;
}


BOOL CTestScript::InitProjList()
{
	((CComboBox*)GetDlgItem(IDC_COMBO_TDLG_PROJNAME))->InsertString(0,"XC");
	((CComboBox*)GetDlgItem(IDC_COMBO_TDLG_PROJNAME))->InsertString(1,"6P");
	return TRUE;
}

BOOL CTestScript::InitOccuList()
{
	m_mapOccu.clear();
	char strfile[512] = "setup\\ChangeOccu.xml";

	TiXmlNode *pNode = NULL;
	TiXmlDocument doc(strfile);

	///载入配置文件
	if(!doc.LoadFile())
	{
		return FALSE;
	}
	pNode = doc.FirstChild("ChangeOccu");
	if(pNode)
	{
		int i = 0;
		TiXmlElement *pChildElement = pNode->FirstChildElement();
		for(;pChildElement!=NULL;pChildElement=pChildElement->NextSiblingElement())
		{
			if(!strcmp(pChildElement->Value(),"Occu"))
			{
				m_mapOccu[i] = string(pChildElement->Attribute("name")); 
				m_ctrlOcculist.InsertString(i,m_mapOccu[i].c_str());
				i++;
			}
		}
	}
	return TRUE;
}

bool CTestScript::SaveTestConfigInfo(void)
{
	const char filename[128]  = "setup\\ConfigRecord.ini";
	FILE* fp = fopen(filename,"w");
	if( fp == NULL)
		return false;

	char sRecordInfo[128];
	sprintf(sRecordInfo,"#  %d  \r\n",m_ctrlTestType.GetCurSel());
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);
	
	sprintf(sRecordInfo,"#  %d  \r\n",m_ctrlServerList.GetCurSel());
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);
		
	sprintf(sRecordInfo,"#  %d  \r\n",m_lStartCDKey);
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);

	sprintf(sRecordInfo,"#  %d  \r\n",m_lEndCDKey);
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);

	sprintf(sRecordInfo,"#  %d  \r\n",m_lTimer);
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);

	sprintf(sRecordInfo,"#  %d  \r\n",m_lPlayerLevel);
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);

	sprintf(sRecordInfo,"#  %d  \r\n",m_ctrlOcculist.GetCurSel());
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);

	sprintf(sRecordInfo,"#  %d  \r\n",m_bnThirdOccu.GetCheck());
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);

	sprintf(sRecordInfo,"#  %d  \r\n",m_ctrlRegionList.GetCurSel());
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);

	sprintf(sRecordInfo,"#  %d  \r\n",m_lPlayerX);
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);

	sprintf(sRecordInfo,"#  %d  \r\n",m_lPlayerY);
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);

	sprintf(sRecordInfo,"#  %d  \r\n",m_bnEquipWeapon.GetCheck());
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);

	sprintf(sRecordInfo,"#  %d  \r\n",m_bnMaxState.GetCheck());
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);

	sprintf(sRecordInfo,"#  %d  \r\n",((CComboBox*)GetDlgItem(IDC_COMBO_TDLG_PROJNAME))->GetCurSel());
	fwrite(sRecordInfo,lstrlen(sRecordInfo),1,fp);

	fclose(fp);
	return true;
}

void CTestScript::OutputScriptInfo(const char* strInfo)
{
 	CString str("");
 	m_edScriptInfo.GetWindowText(str);
 	str += strInfo;
 	str += "\r\n";
 	m_edScriptInfo.SetWindowText(str);
 
 	if (m_edScriptInfo.GetLineCount() > 2<<9 )
 	{
 		m_edScriptInfo.SetWindowText("");
 	}
	m_edScriptInfo.LineScroll(m_edScriptInfo.GetLineCount());
}

//点击右键
void CTestScript::OnNMRclickTreeTestCase(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码

//  右键选中节点
	POINT   _point; 
	::GetCursorPos(&_point);  
	HTREEITEM   hSelect   =   m_treeTestCase.GetSelectedItem();   
	HTREEITEM   hLight   =   m_treeTestCase.GetDropHilightItem();   
	if(hLight   !=   NULL   &&   hSelect   !=   hLight)   
	hSelect   =   hLight;   
	if(hSelect   ==   NULL)   
		return;   
	m_treeTestCase.SelectItem(hSelect);
	if( hSelect == m_treeTestCase.GetRootItem() )
	{
		CMenu menu;
		menu.LoadMenu(IDR_TreeRootMenu);
		menu.GetSubMenu(0)
			->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
			_point.x,_point.y,this);  

		*pResult = 0;  
		return;
	}
//  响应右键菜单
	CMenu menu;
	menu.LoadMenu(IDR_TestCase_MENU);
	menu.GetSubMenu(0)
		->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
		_point.x,_point.y,this);  
	
	*pResult = 0;  
}

void CTestScript::OnAddscript()
{  
	HTREEITEM hSelItem = m_treeTestCase.GetSelectedItem();
	long lSel = (long)m_treeTestCase.GetItemData(hSelItem);
	itTestCase itCase = m_TestCases.find(lSel);

	string strCaseName = itCase->second->strCaseName;
	string strSelItemText = m_treeTestCase.GetItemText(hSelItem);

	string strAddName;
	//选中分类
	if( strcmp(strCaseName.c_str(),strSelItemText.c_str()))
	{
		hSelItem = m_treeTestCase.GetParentItem(hSelItem);
		lSel = (long)m_treeTestCase.GetItemData(hSelItem);
	}
	CAddNameDlg dlg;
	if( dlg.DoModal() == IDOK)
	{
		strAddName = dlg.GetAddName();
		strAddName = "TestCase\\" + strCaseName + "\\" + strAddName + ".lua";
		if( CheckFile(strAddName.c_str()) == FALSE )
		{
			CFile file;
			file.Open(strAddName.c_str(),CFile::modeCreate|CFile::modeWrite,NULL);
			file.Close();
			HTREEITEM hScriptItem = m_treeTestCase.InsertItem(strAddName.c_str(),hSelItem,NULL);
			m_treeTestCase.SetItemData(hScriptItem,lSel);
			return;
		}
	}
}

void CTestScript::OnAddCase()
{
	// TODO: 在此添加命令处理程序代码
	CAddNameDlg dlg;
	CString strCaseName;
	if(dlg.DoModal() == IDOK)
	{
		strCaseName = dlg.GetAddName();
		//载入配置文件
		char strfile[128] = "TestCase\\TestCases.xml";
		TiXmlDocument pDoc(strfile);		
		if(!pDoc.LoadFile())
		{
			PutDebugString("装载配置文件<TestCases.xml>出错！");
			AfxMessageBox("装载配置文件<TestCases.xml>出错！");
			return;
		}
		//添加控件
		HTREEITEM hRootItem = m_treeTestCase.GetRootItem();
		HTREEITEM hNewItem = m_treeTestCase.InsertItem(strCaseName, hRootItem);
		m_treeTestCase.SetItemData(hNewItem,m_lCaseNum);
	// Xml配置文件
		TiXmlNode *pRootNode = pDoc.FirstChild("Test");
		TiXmlElement* newCaseElem = new TiXmlElement( "case" );                /// 节点元素
		newCaseElem->SetAttribute("name",strCaseName);			 ///节点属性

		TiXmlElement* newStartScript = new TiXmlElement("start");
		TiXmlElement* newEndScript = new TiXmlElement("end");
		
		tagTestCase *pCase = new tagTestCase;
		pCase->strCaseName = newCaseElem->Attribute("name");
		m_TestCases[m_lCaseNum++] = pCase;

		char strText[256]="";
		sprintf(strText,"所有案例(%d)",m_TestCases.size());
		m_treeTestCase.SetItemText(hRootItem,strText);

		CString strDir = "TestCase\\";
		strDir += strCaseName;
		CreateDirectory(strDir,NULL);
		
		CString strFileName = strDir + "\\StartTest.lua";

		CFile file;
		file.Open(strFileName,CFile::modeCreate|CFile::modeWrite,NULL);
		file.Close();
		m_treeTestCase.InsertItem(strFileName,hNewItem,NULL);
		newStartScript->SetAttribute("scriptname",strFileName);
		//xml
		newCaseElem->LinkEndChild(newStartScript);
		pCase->strStartScriptName = strFileName;

		strFileName = strDir + "\\EndTest.lua";
		file.Open(strFileName,CFile::modeCreate|CFile::modeWrite,NULL);
		file.Close();
		m_treeTestCase.InsertItem(strFileName,hNewItem,NULL);
		newEndScript->SetAttribute("scriptname",strFileName);
		//xml
		newCaseElem->LinkEndChild(newEndScript);
		pCase->strEndScriptName = strFileName;

		pRootNode->LinkEndChild(newCaseElem);
		//xml 保存
		pDoc.SaveFile(strfile);
		return;
	}
}
//单击左键
void CTestScript::OnTvnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_treeTestCase.UpdateData(TRUE);
	HTREEITEM hSelItem = m_treeTestCase.GetSelectedItem();
	string strContent = m_treeTestCase.GetItemText(hSelItem);
	
	size_t len = lstrlen(strContent.c_str()) - 4;
	if( hSelItem) 
	{
		if( strcmp(&strContent.c_str()[len],".lua") == 0 )
	{
		CFile file;
		if( file.Open(strContent.c_str(),CFile::modeReadWrite|CFile::modeNoTruncate|CFile::modeCreate,0) )
		{
			long nByteRead = file.GetLength();
			char *bReadBuf = new char[nByteRead+1];
			file.Read(bReadBuf,nByteRead);
			bReadBuf[nByteRead] = 0;
			file.Close();

			m_edScriptEdit.SetWindowText(bReadBuf);
			m_ctlShowScriptPath.SetWindowText(strContent.c_str());
			delete [] bReadBuf;
		}
	}
	}
}

//删除
void CTestScript::OnDelScript()
{
	char strfile[128] = "TestCase\\TestCases.xml";
	TiXmlDocument pDoc(strfile);
	///载入配置文件
	if(!pDoc.LoadFile())
	{
		PutDebugString("装载配置文件<TestCases.xml>出错！");
		return;
	}
	else
	{
		HTREEITEM mSelItem = m_treeTestCase.GetSelectedItem();
		if( mSelItem == m_treeTestCase.GetRootItem())
		{
			AfxMessageBox("设定为:不能删除根节点下的所有磁盘文件",MB_OK,NULL);
			return;
		}
		long lSelID = (long)m_treeTestCase.GetItemData(mSelItem);
		itTestCase pCase = m_TestCases.find(lSelID);

		CString strSelItemText = m_treeTestCase.GetItemText(mSelItem);
		//如果不是某个lua文件
		if(strSelItemText.Right(4) != ".lua")
		{
			//分类节点
			TiXmlElement* rootElem = pDoc.RootElement();
			TiXmlNode	* rootNode = pDoc.FirstChild(rootElem->Value());

			TiXmlElement* caseElem = rootElem->FirstChildElement();
			TiXmlNode	* caseNode = rootElem->FirstChild(caseElem->Value());
			for( ; caseElem != NULL; caseElem = caseElem->NextSiblingElement(),
				caseNode = caseNode->NextSibling()	)
			{
				if( caseElem->Attribute("name") == strSelItemText )
				{
					rootNode->RemoveChild(caseElem);
					break;
				}
			}
			//删除磁盘文件
			DeleteFile(strSelItemText);
			//删除控件
			m_treeTestCase.DeleteItem(mSelItem);
			pDoc.SaveFile(strfile);
			//
			m_TestCases.erase(pCase);
			//显示
			char strText[256]="";
			sprintf(strText,"所有案例(%d)",m_TestCases.size());
			m_treeTestCase.SetItemText(m_treeTestCase.GetRootItem(),strText);
			return;
		}
		//是lua文件
		else
		{
			//删除xml配置文件
			TiXmlElement* RootEle = pDoc.RootElement();									//根
			TiXmlNode* pRootNode = pDoc.FirstChild(RootEle->Value());

			TiXmlElement* caseElem = RootEle->FirstChildElement();
			TiXmlNode* caseNode = RootEle->FirstChild(caseElem->Value());

			bool bDel = false;
			for(;caseElem != NULL; caseElem = caseElem->NextSiblingElement())
			{
				TiXmlElement* scriptElem = caseElem->FirstChildElement();
				TiXmlNode* scriptNode = caseElem->FirstChild(scriptElem->Value());
				for(;scriptElem!=NULL;scriptElem = scriptElem->NextSiblingElement(),
									scriptNode = scriptNode->NextSibling()  )
				{
					if(scriptElem->Attribute("scriptname") == strSelItemText)
					{
						caseElem->RemoveChild(scriptNode);
						bDel = true;
						break;
					}
				}
				if( bDel )	break;
			}
			//删除磁盘文件
			DeleteFile(strSelItemText);
			//删除控件
			m_treeTestCase.DeleteItem(mSelItem);
			//显示
			char strText[256]="";
			sprintf(strText,"所有案例(%d)",m_TestCases.size());
			m_treeTestCase.SetItemText(m_treeTestCase.GetRootItem(),strText);
		}
		pDoc.SaveFile(strfile);
		return;
	}
}

// Tab 制表符
BOOL CTestScript::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			if(pMsg->wParam ==VK_TAB)
			{
				CString str;
				CPoint pt;
				pt = GetCaretPos();
				m_edScriptEdit.GetWindowText(str);
				m_edScriptEdit.ReplaceSel("\t");

				m_edScriptEdit.SetSel(-1,0); //取消正文的选择
				return TRUE;
			}
			else if (pMsg->wParam == VK_LEFT)
			{
				this->OnBnClickedButtonStartLogin();
				return true;
			}
		}

	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CTestScript::OnBnClickedButtonStartLogin()
{
	UpdateData(TRUE);

	CGameTestDlg* pMainDLg = CScriptSys::getInstance()->GetTestDialog();
	
	pMainDLg->GetTestResult()->SetAndOutPutTestResultInfo();

	pMainDLg->GetTestInfo()->ShowWindow(TRUE);
	ShowWindow(FALSE);
	pMainDLg->m_tabPage.SetCurSel(1);
	

	if (m_csCurrtWorkPath != "")
		SetCurrentDirectory(m_csCurrtWorkPath);

	if (!pMainDLg->LoadDifProjRes())
	{
		AfxMessageBox("资源加载出错");
		return;
	}

	pMainDLg->m_iProjName = 
		((CComboBox*)GetDlgItem(IDC_COMBO_TDLG_PROJNAME))->GetCurSel();

	pMainDLg->GetTestInfo()->SetTimer(1,1000,NULL);

	//用于判断测试类型容器中应返回的值
	long lType = GetTestType();

	char strScriptName[128];
	if( !m_btnChkPtToken.GetCheck() )
	{

		if ( m_lStartCDKey < 1000 || m_lEndCDKey > 9999)
		{
			MessageBox("《起始帐号》应该大于1000,小于9999","提示",MB_OK);
			return ;
		}
		if ( m_lEndCDKey < m_lStartCDKey || m_lEndCDKey > 9999 )
		{
			MessageBox("《结束帐号》应该大于或则等于《起始帐号》并且小于9999","提示",MB_OK);
			return ;
		}
		if (m_lTimer >= 200000 || m_lTimer <= 200)
		{
			MessageBox("间隔时间必须在200-200000ms之内","提示",MB_OK);
			return ;
		}

		if ( m_lStartCDKey == m_lEndCDKey )
		{
			m_lEndCDKey = m_lStartCDKey;
		}
		  sprintf(strScriptName,"LoginCDKey(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",m_lStartCDKey,m_lEndCDKey,lType,m_lTimer,m_lPlayerLevel,m_ctrlOcculist.GetCurSel(),m_vecRegion[m_ctrlRegionList.GetCurSel()].id,m_lPlayerX,m_lPlayerY,m_bnMaxState.GetCheck(),m_bnEquipWeapon.GetCheck(),m_bnThirdOccu.GetCheck());
		//sprintf(strScriptName,"LoginCDKey(%d,%d,%d,%d,%d);",m_lStartCDKey,m_lEndCDKey,lType,m_lTimer,m_lSendDelayTime);
	}
	else
	{
		sprintf(strScriptName,"LoginToken(%d,%d,%d);",m_lStartCDKey,lType,m_lTimer);
	}
	CScriptSys::getInstance()->PerformString(strScriptName);

	m_bnStartLogin.EnableWindow(FALSE);

	SaveTestConfigInfo();

}

//语法检测
void CTestScript::OnCheckScriptSyntax()
{
	m_edScriptEdit.GetWindowText(m_edScript);
	if( strcmp(m_edScript,"") == 0 )
		return;
	//得到文件路径
	CString strFilePath;
	m_ctlShowScriptPath.GetWindowText(strFilePath);
	int nRet;
	lua_State* tempL = lua_open();
	//
	if( strcmp( strFilePath,"") == 0 || strcmp(strFilePath,"位置") == 0 )
	{
		nRet = luaL_loadfile(tempL,m_edScript);
		if( nRet == 0)
			CScriptSys::getInstance()->LoadOneFileOrString(m_edScript);
	}
	else if( strFilePath.Right(4) != ".lua")
	{
		MessageBox("提示:\n 不是有效的 *.lua 文件","提示",MB_OK);
		return;
	}
	else
	{
		nRet = luaL_dofile(tempL,strFilePath);
		if(nRet ==0)
			CScriptSys::getInstance()->LoadOneFileOrString(strFilePath);
	}

	if( nRet != 0 )
	{
		char strInfo[102400]="";
		sprintf(strInfo,"<语法检测错误>(ErroNum:%d,info:%s)",
			nRet,lua_tostring(tempL,-1));
		MessageBox(strInfo,"警告",MB_OK);
	}
	else{
		MessageBox("<语法检测:正确>","提示",MB_OK);
	}
	lua_pop(tempL,1);
	lua_close(tempL);
	return;
}

//保存
void  CTestScript::OnBnClickedSavescript()
{
	m_treeTestCase.UpdateData(TRUE);
	CString filename;
	m_ctlShowScriptPath.GetWindowText(filename);
	m_edScriptEdit.GetWindowText(m_edScript);
	FILE* fp = fopen(filename,"w");
	if( fp == NULL)
		return;
	MessageBox("保存成功","!!!!!!",MB_OK);
	fwrite(m_edScript,m_edScript.GetLength(),1,fp);
	fclose(fp);
}
//开始某个用例的测试
void CTestScript::OnCaseStartTest()
{
	HTREEITEM hSelItem = m_treeTestCase.GetSelectedItem();
	if(hSelItem)
	{
		long lSel = (long)m_treeTestCase.GetItemData(hSelItem);
		itTestCase it = m_TestCases.find(lSel);
		if( it != m_TestCases.end())
		{
			CScriptSys::getInstance()->LoadOneFileOrString((*it).second->strStartScriptName.c_str());
		}
	}
}
//结束某个测试
void CTestScript::OnCaseStopTest()
{
	HTREEITEM hSelItem = m_treeTestCase.GetSelectedItem();
	if(hSelItem)
	{
		long lSel = (long)m_treeTestCase.GetItemData(hSelItem);
		itTestCase it = m_TestCases.find(lSel);
		if( it != m_TestCases.end())
		{
			CScriptSys::getInstance()->LoadOneFileOrString((*it).second->strEndScriptName.c_str());
		}
	}
}
//打开lua文件
void CTestScript::OnCheckOpenScript()
{
	m_edScriptEdit.UpdateData(TRUE);
	m_edScriptEdit.SetFocus();
	CFileDialog fileDlg(true, ".lua", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"lua Files (*.lua)|*.lua|All Files (*.*)|*.*||", NULL);
	if( fileDlg.DoModal() == IDOK )
	{
		string strPath = fileDlg.GetPathName();
		string strName = fileDlg.GetFileName();
		CFile file;
		if( file.Open(strName.c_str(),CFile::modeReadWrite | CFile::modeNoTruncate | CFile::modeCreate,0) )
		{
			long nByteRead = file.GetLength();
			char *bReadBuf = new char[nByteRead+1];
			file.Read(bReadBuf,nByteRead);
			bReadBuf[nByteRead] = 0;
			file.Close();

			m_edScriptEdit.SetWindowText(bReadBuf);
			m_ctlShowScriptPath.SetWindowText(strPath.c_str());
			delete [] bReadBuf;
		}
	}
}


void CTestScript::OnCheckThirdOccu()
{
	if (m_bnThirdOccu.GetCheck())
		m_ctrlOcculist.EnableWindow(0);
	else
		m_ctrlOcculist.EnableWindow(1);
}

//更新重读脚本
void CTestScript::OnCheckUpdateScript()
{
	std::ifstream inf;
	if( inf.fail())
		return;
	inf.open("Script/UpdateFile.ini");
	while( true)
	{
		string filename;
		if( inf.eof())
			break;
		inf>>filename;
		if( filename.empty() )
		{
			MessageBox("LoadScriptFileSuccessful","警告",MB_OK);
			return;
		}
		if( CScriptSys::getInstance()->LoadOneFileOrString(filename.c_str()) == FALSE )
			MessageBox("更新重读脚本出错","警告",MB_OK);
		else
			MessageBox("更新重读脚本出错","警告",MB_OK);

		
	}
}

//获取本次测试的类型
int CTestScript::GetTestType(void)
{
	//用于判断测试类型容器中应返回的值
	m_dwCurSelTestType = m_ctrlTestType.GetCurSel();
	return m_mapTestType.find( m_dwCurSelTestType )->second;
	return 0;
}

void CTestScript::OnBnClickedCheckPtToken()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bToken = m_btnChkPtToken.GetCheck() != 0 ? TRUE:FALSE;
	if( !bToken )
	{
		GetDlgItem(IDC_STATIC_CDKeyStart)->SetWindowText("起始帐号:gm");

		GetDlgItem(IDC_STATIC_CDKeyEnd)->ShowWindow(TRUE);
		GetDlgItem(IDC_EndCDKey)->ShowWindow(TRUE);	
	}
	else
	{
		GetDlgItem(IDC_STATIC_CDKeyStart)->SetWindowText("机器人数量");

		GetDlgItem(IDC_STATIC_CDKeyEnd)->ShowWindow(FALSE);
		GetDlgItem(IDC_EndCDKey)->ShowWindow(FALSE);
	}
}

void CTestScript::OnCheckCloseGT()
{
	CScriptSys::getInstance()->GetTestDialog()->OnCancel();
}


void CTestScript::OnCheckSetWorkPath()
{
	LPITEMIDLIST rootLocation;
	SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOP,&rootLocation);
	if (rootLocation == NULL)
	{
		ASSERT(0);
		return;
	}
	BROWSEINFOW bi;
	ZeroMemory(&bi,sizeof(bi));
	bi.pidlRoot = rootLocation;
	LPITEMIDLIST targetLocation = SHBrowseForFolder((LPBROWSEINFOA)&bi);
	if (targetLocation!=NULL)
	{
		TCHAR targetPath[MAX_PATH];
		SHGetPathFromIDList(targetLocation,targetPath);
		m_csCurrtWorkPath.Format("%s",targetPath);
	}
	GetDlgItem(IDC_STATIC_WORKPATH)->SetWindowText("项目资源路径:"+m_csCurrtWorkPath);
}




