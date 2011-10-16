// Step.cpp : 实现文件
//
#include "stdafx.h"
#include "QuestEditor.h"
#include "CStep.h"
#include "tinyxml.h"
#include <vector>
#include <string>
#include "CXmlEdit.h"
#include "CFileSave.h"
#include "Tools.h"
#include <algorithm>

IMPLEMENT_DYNAMIC(CStep, CDialog)
CStep::CStep(CWnd* pParent /*=NULL*/)
	: CDialog(CStep::IDD, pParent)
	, m_StepId(1)
	, m_StepNpcId(_T(""))
	, m_StepEffect_1(0)
	, m_StepEffect_2(0)
	, m_ShowNum(0)
	, m_Sum(0)
	, m_TraceID(_T(""))
	, m_Param(0)
	, m_TextID(0)
	, m_TextName(0)
	, m_StepEncouragementName(_T(""))
	, m_StepEncouragementNum(0)
	, m_StepEncouragementType(0)
    , m_QuestCell(1)
    , m_textDes(_T(""))
    , m_QuestId(0)
{
    isLoading = false;
    m_pSlectRoleStep = NULL;
}
CStep::~CStep()
{
}

BEGIN_MESSAGE_MAP(CStep, CDialog)
	//ON_BN_CLICKED(IDC_BUTTON8, &CStep::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON10, &CStep::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CStep::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON5, &CStep::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON1, &CStep::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON9, &CStep::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON4, &CStep::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON6, &CStep::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON12, &CStep::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CStep::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON2, &CStep::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CStep::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON7, &CStep::OnBnClickedButton7)
	ON_NOTIFY(NM_CLICK, IDC_LIST3, &CStep::OnNMClickList3)
	ON_NOTIFY(NM_CLICK, IDC_LIST5, &CStep::OnNMClickList5)
	ON_NOTIFY(NM_CLICK, IDC_LIST6, &CStep::OnNMClickList6)
	ON_NOTIFY(NM_CLICK, IDC_LIST4, &CStep::OnNMClickList4)

    ON_EN_CHANGE(IDC_EDIT16, &CStep::OnEnChangeEdit3)
    ON_EN_CHANGE(IDC_EDIT15, &CStep::OnEnChangeEdit1)

    ON_EN_CHANGE(IDC_EDIT7, &CStep::OnEnChangeEdit7)
    ON_EN_CHANGE(IDC_EDIT8, &CStep::OnEnChangeEdit8)
    ON_EN_CHANGE(IDC_EDIT9, &CStep::OnEnChangeEdit9)
    ON_EN_CHANGE(IDC_EDIT10, &CStep::OnEnChangeEdit10)
    ON_EN_CHANGE(IDC_EDIT11, &CStep::OnEnChangeEdit11)
    ON_EN_CHANGE(IDC_EDIT31, &CStep::OnEnChangeEdit31)
    ON_EN_CHANGE(IDC_EDIT27, &CStep::OnEnChangeEdit27)
    ON_EN_CHANGE(IDC_EDIT23, &CStep::OnEnChangeEdit23)
    ON_EN_CHANGE(IDC_EDIT22, &CStep::OnEnChangeEdit22)
    ON_EN_CHANGE(IDC_EDIT21, &CStep::OnEnChangeEdit21)
    ON_EN_CHANGE(IDC_EDIT19, &CStep::OnEnChangeEdit19)
    ON_EN_CHANGE(IDC_EDIT18, &CStep::OnEnChangeEdit18)
    ON_EN_CHANGE(IDC_EDIT20, &CStep::OnEnChangeEdit20)
END_MESSAGE_MAP()
void CStep::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT17, m_StepId);
    DDX_Text(pDX, IDC_EDIT2, m_StepNpcId);
    DDX_Text(pDX, IDC_EDIT3, m_StepEffect_1);
    DDX_Text(pDX, IDC_EDIT4, m_StepEffect_2);
    DDX_Text(pDX, IDC_EDIT30, m_QuestCell);
    DDV_MinMaxLong(pDX, m_QuestCell, 0, 10);
    //设置步骤的文本列表的格式

    if ( !isLoading )
    {
        DDX_Control(pDX, IDC_LIST3, m_StepTextList);
        m_StepTextList.SetExtendedStyle(
            LVS_EX_FLATSB
            |LVS_EX_FULLROWSELECT
            |LVS_EX_HEADERDRAGDROP
            |LVS_EX_ONECLICKACTIVATE
            |LVS_EX_GRIDLINES
            );
        m_StepTextList.InsertColumn(0,"索引",LVCFMT_LEFT,100,0);
        m_StepTextList.InsertColumn(1,"内容",LVCFMT_LEFT,573,0);
        //设置奖励列表的格式
        DDX_Control(pDX, IDC_LIST4, m_StepAimList);
        m_StepAimList.SetExtendedStyle(
            LVS_EX_FLATSB
            |LVS_EX_FULLROWSELECT
            |LVS_EX_HEADERDRAGDROP
            |LVS_EX_ONECLICKACTIVATE
            |LVS_EX_GRIDLINES
            );
        m_StepAimList.InsertColumn(0,"ID",LVCFMT_LEFT,50,0);
        m_StepAimList.InsertColumn(1,"AimModel",LVCFMT_LEFT,61,0);
        m_StepAimList.InsertColumn(2,"AimType",LVCFMT_LEFT,61,0);
        m_StepAimList.InsertColumn(3,"AimName",LVCFMT_LEFT,61,0);
        m_StepAimList.InsertColumn(4,"AimNum",LVCFMT_LEFT,61,0);
        m_StepAimList.InsertColumn(5,"DropFrom",LVCFMT_LEFT,61,0);
        m_StepAimList.InsertColumn(6,"DropOdds",LVCFMT_LEFT,61,0);
        m_StepAimList.InsertColumn(7,"param",LVCFMT_LEFT,51,0);
        //设置任务追踪列表格式
        DDX_Control(pDX, IDC_LIST6, m_StepTraceList);
        m_StepTraceList.SetExtendedStyle(
            LVS_EX_FLATSB
            |LVS_EX_FULLROWSELECT
            |LVS_EX_HEADERDRAGDROP
            |LVS_EX_ONECLICKACTIVATE
            |LVS_EX_GRIDLINES
            );
        m_StepTraceList.InsertColumn(0,"Desc",LVCFMT_LEFT,88,0);
        m_StepTraceList.InsertColumn(1,"ShowNum",LVCFMT_LEFT,88,0);
        m_StepTraceList.InsertColumn(2,"Sum",LVCFMT_LEFT,88,0);
        m_StepTraceList.InsertColumn(3,"Param",LVCFMT_LEFT,88,0);
        m_StepTraceList.InsertColumn(4,"ID",LVCFMT_LEFT,88,0);
        isLoading = true;
    }

    DDX_Control(pDX, IDC_EDIT15, m_TextIndex);
    DDX_Control(pDX, IDC_EDIT16, m_TextContext);
    DDX_Text(pDX, IDC_EDIT1, m_textDes);
    DDX_Text(pDX, IDC_EDIT5, m_QuestId);
    DDX_Control(pDX, IDC_EDIT7, m_TraceDes);
    DDX_Control(pDX, IDC_EDIT8, m_TraceShowNum);
    DDX_Control(pDX, IDC_EDIT9, m_TraceSum);
    DDX_Control(pDX, IDC_EDIT10, m_TraceParam);
    DDX_Control(pDX, IDC_EDIT11, m_TraceId);
    DDX_Control(pDX, IDC_EDIT31, m_AimId);
    DDX_Control(pDX, IDC_EDIT27, m_AimModel);
    DDX_Control(pDX, IDC_EDIT23, m_AimType);
    DDX_Control(pDX, IDC_EDIT22, m_AimName);
    DDX_Control(pDX, IDC_EDIT21, m_AimNum);
    DDX_Control(pDX, IDC_EDIT18, m_DropFrom);
    DDX_Control(pDX, IDC_EDIT19, m_DropOdds);
    DDX_Control(pDX, IDC_EDIT20, m_AimParam);
}
void CStep::OnOK()
{
}
/*功能:获取任务的步骤的信息。
*摘要:根据任务的ＩＤ读取出相应的任务的步骤信息
*参数:pItemText属性列表的文本信息，其中包含了任务的ＩＤ
*返回值:void
*作者：王 钦
*创建日期:2010-7-6
*/
void CStep::ReadRoleStep(CString *pItemText)
{
	int id = atoi(pItemText->GetString());
	m_nRoleID = id;
    m_QuestId = id;
    std::map<long,CString>::iterator FileIter = GetInst(CXmlEdit).m_AllQuestStrId.find ( m_nRoleID );
    if ( FileIter == GetInst(CXmlEdit).m_AllQuestStrId.end() )
    {
        return ;
    }
    CXmlEdit::MapFileTable::iterator  RoleIter = GetInst(CXmlEdit).m_AllQuestList.find( FileIter->second ) ;
    if ( RoleIter != GetInst(CXmlEdit).m_AllQuestList.end() )
    {
        vector<tagRole*>::iterator iter;
        iter = RoleIter->second.begin();
        for ( ; iter != RoleIter->second.end() ; iter ++ )
        {
            if ( (*iter)->m_nID == m_nRoleID )
            {
                m_pSlectRoleStep = (*iter)->vcRole_Step;
            }
        }
    }

    UpdateData( false );
}
/*功能:显示获取的任务步骤信息。
*摘要:根据任务步骤的ＩＤ显示相应的任务步骤的详细信息
*参数:StepID相应的任务的步骤的ID
*返回值:void
*作者：王 钦
*创建日期:2010-7-6
*/
void CStep::Showdetail(int StepID)
{
	m_StepAimList.DeleteAllItems();
	m_StepTextList.DeleteAllItems();
	m_StepTraceList.DeleteAllItems();
	m_StepId = StepID;
	char strVal[256];

	if ( m_pSlectRoleStep != NULL )
	{
		if (m_pSlectRoleStep->m_nID == m_StepId)
		{
			vector<tagStepAim*>Aim				= m_pSlectRoleStep->vcStepAim;
			vector<tagStepAim*>::iterator AimIter;

			vector<tagStepTrace*>Trace			= m_pSlectRoleStep->vcStepRoleTrace;
			vector<tagStepTrace*>::iterator TraceIter;

			 GetDlgItem(IDC_EDIT17)->SetWindowText(itoa(m_pSlectRoleStep->m_nID,strVal,10));
			 GetDlgItem(IDC_EDIT2)->SetWindowText((m_pSlectRoleStep->m_cNPCID).c_str());
			 GetDlgItem(IDC_EDIT3)->SetWindowText(itoa(m_pSlectRoleStep->m_nNPCEffect_1,strVal,10));
			 GetDlgItem(IDC_EDIT4)->SetWindowText(itoa(m_pSlectRoleStep->m_nNPCEffect_2,strVal,10));
             GetDlgItem(IDC_EDIT30)->SetWindowText(itoa(m_pSlectRoleStep->m_nEncouragementCell,strVal,10));

             char  QForMat [256];
             char  QQuestId[32];

             sprintf_s( QForMat , "textres/questtext%d.xml", m_nRoleID/1000 );
             sprintf_s( QQuestId , "Q%d", m_nRoleID);

             CXmlEdit::MapTextFileTable::iterator  MapIter = GetInst(CXmlEdit).m_AllQuestTextFile.find( QForMat );

             if ( MapIter!= GetInst(CXmlEdit).m_AllQuestTextFile.end() )
             {
                 map<CString,CString>::iterator  TextIter = MapIter->second.begin();

                 for (int i = 0 ; TextIter != MapIter->second.end() ; TextIter ++ )
                 {
                     if ( strstr( TextIter->first.GetString(),QQuestId) )
                     {
                         m_StepTextList.InsertItem(i,"");
                         m_StepTextList.SetItemText(i,0,TextIter->first);
                         m_StepTextList.SetItemText(i,1,TextIter->second);
                         i++;
                     }
                 }
             }

			///////////////
			AimIter = Aim.begin();
			for (int i = 0 ;i< (int)Aim.size();i++)
			{
				m_StepAimList.InsertItem(i,"");
				m_StepAimList.SetItemText(i,0,itoa((*AimIter)->m_nID,strVal,10));
				m_StepAimList.SetItemText(i,1,itoa((*AimIter)->m_nAimModel,strVal,10));
				m_StepAimList.SetItemText(i,2,itoa((*AimIter)->m_nAimType,strVal,10));
				m_StepAimList.SetItemText(i,3,((*AimIter)->m_cAimName).c_str());
				m_StepAimList.SetItemText(i,4,itoa((*AimIter)->m_nAimNum,strVal,10));
				m_StepAimList.SetItemText(i,5,((*AimIter)->m_cDropFrom).c_str());
				m_StepAimList.SetItemText(i,6,itoa((*AimIter)->m_nDropOdds,strVal,10));
				m_StepAimList.SetItemText(i,7,itoa((*AimIter)->m_nParam,strVal,10));
				AimIter++;
			}

			////////////////////////
			TraceIter = Trace.begin();
			for (int i = 0 ;i< (int)Trace.size();i++)
			{
				m_StepTraceList.InsertItem(i,"");
				m_StepTraceList.SetItemText(i,0,(*TraceIter)->m_nDesc.c_str());
				m_StepTraceList.SetItemText(i,1,itoa((*TraceIter)->m_nShowNum,strVal,10));
				m_StepTraceList.SetItemText(i,2,itoa((*TraceIter)->m_nSum,strVal,10));
				m_StepTraceList.SetItemText(i,3,itoa((*TraceIter)->m_nParam,strVal,10));
				m_StepTraceList.SetItemText(i,4,itoa((*TraceIter)->m_nID,strVal,10));
				TraceIter++;
			}
		}
	}
}
//确定控件的实现代码
void CStep::OnBnClickedButton8()
{

}
// 添加任务文本代码
void CStep::OnBnClickedButton10()
{
	POSITION   pos   =   m_StepTextList.GetFirstSelectedItemPosition();
	int ItemNum = m_StepTextList.GetNextSelectedItem(pos);
	if (ItemNum<0)
	{
		ItemNum = 0;
	}
	CString strID;
	CString strName;
	GetDlgItem(IDC_EDIT15)->GetWindowText(strID);
	GetDlgItem(IDC_EDIT16)->GetWindowText(strName);
	m_StepTextList.InsertItem(ItemNum,"");
	m_StepTextList.SetItemText(ItemNum,0,strID);
	m_StepTextList.SetItemText(ItemNum,1,strName);
	ItemNum+=1;
}
//删除任务文本代码
void CStep::OnBnClickedButton11()
{
	POSITION   pos   =   m_StepTextList.GetFirstSelectedItemPosition();
	int ItemNum = m_StepTextList.GetNextSelectedItem(pos);
	m_StepTextList.DeleteItem(ItemNum);

}
//修改任务文本代码
void CStep::OnBnClickedButton5()
{
	POSITION   pos   =   m_StepTextList.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{	return;		}
	CString InputId;
	CString InputName;
	CString id;
	CString name;
	int nSelectItem= m_StepTextList.GetNextSelectedItem(pos);
	id = m_StepTextList.GetItemText(nSelectItem,0);
	GetDlgItem(IDC_EDIT15)->GetWindowText(InputId);
	GetDlgItem(IDC_EDIT16)->GetWindowText(InputName);
	m_StepTextList.SetItemText(nSelectItem,0,InputId);
	m_StepTextList.SetItemText(nSelectItem,1,InputName);
}

//添加任务脚本
void CStep::OnBnClickedButton1()
{
	/// 
    if ( !m_pSlectRoleStep )
        return ;
    // TODO: 在此添加控件通知处理程序代码
    POSITION   pos   =   m_StepTraceList.GetFirstSelectedItemPosition();
    if ( pos == NULL  )
    {
        return ;
    }
    int nSelectItem     = m_StepTraceList.GetNextSelectedItem(pos);

}
//删除奖励
void CStep::OnBnClickedButton9()
{
	POSITION   pos   =   m_StepEncourageList.GetFirstSelectedItemPosition();
	int ItemNum = m_StepEncourageList.GetNextSelectedItem(pos);
	m_StepEncourageList.DeleteItem(ItemNum);
}

//修改步骤配置信息
void CStep::OnBnClickedButton4()
{
    if ( !m_pSlectRoleStep )
        return ;
    UpdateData( TRUE );

    vector<tagRole*>           vcAllRole;
    vector<tagRole*>::iterator AllRileIter;
    CXmlEdit::MapFileTable     AllFiletable;
    CXmlEdit::MapTextFileTable AllTextRes;
    std::map<long,CString>::iterator FileIter = GetInst(CXmlEdit).m_AllQuestStrId.find ( m_nRoleID );
    if ( FileIter == GetInst(CXmlEdit).m_AllQuestStrId.end() )
    {
        return ;
    }

    CXmlEdit::MapFileTable::iterator  RoleIter = GetInst(CXmlEdit).m_AllQuestList.find( FileIter->second ) ;
    if ( RoleIter != GetInst(CXmlEdit).m_AllQuestList.end() )
    {
        vector<tagRole*>::iterator iter;
        iter = RoleIter->second.begin();
        for ( ; iter != RoleIter->second.end() ; iter ++ )
        {
            if ( (*iter)->m_nID == m_nRoleID )
            {
               tagRole* Role = *iter;

               //// 更新
               Role->vcRole_Step->m_cNPCID  =  m_StepNpcId.GetBuffer();
               Role->vcRole_Step->m_nNPCEffect_1 = m_StepEffect_1;
               Role->vcRole_Step->m_nNPCEffect_2 = m_StepEffect_2;
               Role->vcRole_Step->m_nEncouragementCell = m_QuestCell;
            }
        }

    }
}
//添加任务追踪
void CStep::OnBnClickedButton6()
{
    if ( !m_pSlectRoleStep )
        return ;

    long num = m_pSlectRoleStep->vcStepRoleTrace.size();
    tagStepTrace*  tagTrace = new  tagStepTrace(num+1);
    tagTrace->m_nShowNum=0;
    tagTrace->m_nSum  = 0;
    tagTrace->m_nParam= 0;
    tagTrace->m_nID   = 0;
    m_pSlectRoleStep->vcStepRoleTrace.push_back( tagTrace );

    vector<tagStepTrace*>::iterator TraceIter = m_pSlectRoleStep->vcStepRoleTrace.begin() ;

    m_StepTraceList.DeleteAllItems();
    char strVal[ 256 ];
    for (int i = 0 ;i< (int)m_pSlectRoleStep->vcStepRoleTrace.size();i++)
    {
        m_StepTraceList.InsertItem(i,"");
        m_StepTraceList.SetItemText(i,0,(*TraceIter)->m_nDesc.c_str());
        m_StepTraceList.SetItemText(i,1,itoa((*TraceIter)->m_nShowNum,strVal,10));
        m_StepTraceList.SetItemText(i,2,itoa((*TraceIter)->m_nSum,strVal,10));
        m_StepTraceList.SetItemText(i,3,itoa((*TraceIter)->m_nParam,strVal,10));
        m_StepTraceList.SetItemText(i,4,itoa((*TraceIter)->m_nID,strVal,10));
        TraceIter++;
    }
}
//删除任务追踪
void CStep::OnBnClickedButton12()
{
    if ( !m_pSlectRoleStep )
        return ;
    // TODO: 在此添加控件通知处理程序代码
    POSITION   pos   =   m_StepTraceList.GetFirstSelectedItemPosition();
    if ( pos == NULL  )
    {
        return ;
    }
    int nSelectItem     = m_StepTraceList.GetNextSelectedItem(pos);

    if ( nSelectItem < (int)m_pSlectRoleStep->vcStepRoleTrace.size() )
    {
        CString Context =  m_StepTraceList.GetItemText(nSelectItem,0);

        if( strcmp( Context.GetBuffer(), m_pSlectRoleStep->vcStepRoleTrace[nSelectItem]->m_nDesc.c_str()) == 0 )
        {
            m_pSlectRoleStep->vcStepRoleTrace.erase( m_pSlectRoleStep->vcStepRoleTrace.begin() + nSelectItem );

            vector<tagStepTrace*>::iterator TraceIter = m_pSlectRoleStep->vcStepRoleTrace.begin() ;

            m_StepTraceList.DeleteAllItems();
            char strVal[ 256 ];
            for (int i = 0 ;i< (int)m_pSlectRoleStep->vcStepRoleTrace.size();i++)
            {
                m_StepTraceList.InsertItem(i,"");
                m_StepTraceList.SetItemText(i,0,(*TraceIter)->m_nDesc.c_str());
                m_StepTraceList.SetItemText(i,1,itoa((*TraceIter)->m_nShowNum,strVal,10));
                m_StepTraceList.SetItemText(i,2,itoa((*TraceIter)->m_nSum,strVal,10));
                m_StepTraceList.SetItemText(i,3,itoa((*TraceIter)->m_nParam,strVal,10));
                m_StepTraceList.SetItemText(i,4,itoa((*TraceIter)->m_nID,strVal,10));
                TraceIter++;
            }
        }
        return ;
    }
    return ;
}
//修改任务追踪
void CStep::OnBnClickedButton13()
{
	POSITION   pos   =   m_StepTraceList.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{	return;		}
	int nSelectItem = m_StepTextList.GetNextSelectedItem(pos);
	CString strDesc;
	CString strShowNum;
	CString strSum;
	CString strParam;
	CString strID;
	GetDlgItem(IDC_EDIT7)->GetWindowText(strDesc);
	GetDlgItem(IDC_EDIT8)->GetWindowText(strShowNum);
	GetDlgItem(IDC_EDIT9)->GetWindowText(strSum);
	GetDlgItem(IDC_EDIT10)->GetWindowText(strParam);
	GetDlgItem(IDC_EDIT11)->GetWindowText(strID);
	m_StepTraceList.SetItemText(nSelectItem,0,strDesc);
	m_StepTraceList.SetItemText(nSelectItem,1,strShowNum);
	m_StepTraceList.SetItemText(nSelectItem,2,strSum);
	m_StepTraceList.SetItemText(nSelectItem,3,strParam);
	m_StepTraceList.SetItemText(nSelectItem,4,strID);
}

//添加任务目标
void CStep::OnBnClickedButton2()
{
     if ( !m_pSlectRoleStep )
        return ;

     vector<tagStepAim*>::iterator AimIter = m_pSlectRoleStep->vcStepAim.begin();

     long num = 0;
     for ( ; AimIter!= m_pSlectRoleStep->vcStepAim.end() ; AimIter++ )
     {
         if ( (*AimIter)->m_nID > num )
         {
             num = (*AimIter)->m_nID;
         }

     }
	 tagStepAim * tagAim = new tagStepAim(num+1);  
     m_pSlectRoleStep->vcStepAim.push_back( tagAim );

     
     AimIter = m_pSlectRoleStep->vcStepAim.begin();
     m_StepAimList.DeleteAllItems();

     char strVal[256];
     int  i = 0 ;

     for ( ; AimIter!= m_pSlectRoleStep->vcStepAim.end() ; AimIter++ )
     {
         m_StepAimList.InsertItem(i,"");
         m_StepAimList.SetItemText(i,0,itoa((*AimIter)->m_nID,strVal,10));
         m_StepAimList.SetItemText(i,1,itoa((*AimIter)->m_nAimModel,strVal,10));
         m_StepAimList.SetItemText(i,2,itoa((*AimIter)->m_nAimType,strVal,10));
         m_StepAimList.SetItemText(i,3,((*AimIter)->m_cAimName).c_str());
         m_StepAimList.SetItemText(i,4,itoa((*AimIter)->m_nAimNum,strVal,10));
         m_StepAimList.SetItemText(i,5,((*AimIter)->m_cDropFrom).c_str());
         m_StepAimList.SetItemText(i,6,itoa((*AimIter)->m_nDropOdds,strVal,10));
         m_StepAimList.SetItemText(i,7,itoa((*AimIter)->m_nParam,strVal,10));
         i++;
     }
}
//删除任务目标
void CStep::OnBnClickedButton3()
{
    if ( !m_pSlectRoleStep )
        return ;
    // TODO: 在此添加控件通知处理程序代码
    POSITION   pos   =   m_StepAimList.GetFirstSelectedItemPosition();
    if ( pos == NULL  )
    {
        return ;
    }
    int nSelectItem     = m_StepAimList.GetNextSelectedItem(pos);

    if ( nSelectItem < (int)m_pSlectRoleStep->vcStepAim.size() )
    {
        CString Context =  m_StepAimList.GetItemText(nSelectItem,0);

        long id = m_pSlectRoleStep->vcStepAim[nSelectItem]->m_nID;
        if( atoi(Context.GetBuffer()) == id )
        {
            m_pSlectRoleStep->vcStepAim.erase( m_pSlectRoleStep->vcStepAim.begin() + nSelectItem );

            vector<tagStepAim*>::iterator AimIter = m_pSlectRoleStep->vcStepAim.begin() ;
            char strVal[256];
            m_StepAimList.DeleteAllItems();
            for (int i = 0 ;i< (int)m_pSlectRoleStep->vcStepAim.size();i++)
            {
                m_StepAimList.InsertItem(i,"");
                m_StepAimList.SetItemText(i,0,itoa((*AimIter)->m_nID,strVal,10));
                m_StepAimList.SetItemText(i,1,itoa((*AimIter)->m_nAimModel,strVal,10));
                m_StepAimList.SetItemText(i,2,itoa((*AimIter)->m_nAimType,strVal,10));
                m_StepAimList.SetItemText(i,3,((*AimIter)->m_cAimName).c_str());
                m_StepAimList.SetItemText(i,4,itoa((*AimIter)->m_nAimNum,strVal,10));
                m_StepAimList.SetItemText(i,5,((*AimIter)->m_cDropFrom).c_str());
                m_StepAimList.SetItemText(i,6,itoa((*AimIter)->m_nDropOdds,strVal,10));
                m_StepAimList.SetItemText(i,7,itoa((*AimIter)->m_nParam,strVal,10));
                AimIter++;
            }
        }
        return ;
    }
    return ;
}
//修改任务目标
void CStep::OnBnClickedButton7()
{
	POSITION   pos   =   m_StepAimList.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{	return;		}
	int nSelectItem = m_StepAimList.GetNextSelectedItem(pos);
	CString strID;
	CString strAimModel;
	CString strAimType;
	CString strAimName;
	CString strAimNum;
	CString strDropFrom;
	CString strDropOdds;
	CString strParam;
	GetDlgItem(IDC_EDIT31)->GetWindowText(strID);
	GetDlgItem(IDC_EDIT27)->GetWindowText(strAimModel);
	GetDlgItem(IDC_EDIT23)->GetWindowText(strAimType);
	GetDlgItem(IDC_EDIT22)->GetWindowText(strAimName);
	GetDlgItem(IDC_EDIT21)->GetWindowText(strAimNum);
	GetDlgItem(IDC_EDIT18)->GetWindowText(strDropFrom);
	GetDlgItem(IDC_EDIT19)->GetWindowText(strDropOdds);
	GetDlgItem(IDC_EDIT20)->GetWindowText(strParam);
	//m_DropOdds = atoi(strDropOdds);

	if (atoi(strDropOdds)>=0&&atoi(strDropOdds)<10000)
	{
		m_StepAimList.SetItemText(nSelectItem,0,strID);
		m_StepAimList.SetItemText(nSelectItem,1,strAimModel);
		m_StepAimList.SetItemText(nSelectItem,2,strAimType);
		m_StepAimList.SetItemText(nSelectItem,3,strAimName);
		m_StepAimList.SetItemText(nSelectItem,4,strAimNum);
		m_StepAimList.SetItemText(nSelectItem,5,strDropFrom);
		m_StepAimList.SetItemText(nSelectItem,7,strParam);
		//m_StepAimList.SetItemText(nSelectItem,6,itoa(m_DropOdds,strVal,10));
	}
	else if(atoi(strDropOdds)<0||atoi(strDropOdds)>10000)
	{
		MessageBox("DropOdds的有效范围是0到10000整数");
		GetDlgItem(IDC_EDIT19)->SetFocus();
	}
	
	
}
//选中任务文本列表中的某一项进行编辑
void CStep::OnNMClickList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION   pos   =   m_StepTextList.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{	return;		}
	int nSelectItem    =  m_StepTextList.GetNextSelectedItem(pos);
	CString selectid   =  m_StepTextList.GetItemText(nSelectItem,0);
	CString selectname =  m_StepTextList.GetItemText(nSelectItem,1);

    if ( selectid.GetLength() == 7 )
    {
       GetDlgItem(IDC_EDIT1)->SetWindowText("任务名");
    }
    else if( selectid.GetLength() > 7 ) 
    {
        const char * QIndex = selectid.GetBuffer();
        int     Len = selectid.GetLength();
        int     Index = 0; 
        char   QText[4];
        strncpy( QText, QIndex+(Len-2) , 2 );
        QText[3] = '\0';
        Index = atoi( QText );

        GetDlgItem(IDC_EDIT1)->SetWindowText(GetTextResDes(Index));

    }
	GetDlgItem(IDC_EDIT15)->SetWindowText(selectid);
	GetDlgItem(IDC_EDIT16)->SetWindowText(selectname);
	*pResult = 0;
}
//选中任务奖励列表中的某一项进行编辑
void CStep::OnNMClickList5(NMHDR *pNMHDR, LRESULT *pResult)
{

}
//选中任务追踪列表中的某一项进行编辑
void CStep::OnNMClickList6(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION   pos   =   m_StepTraceList.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{	return;		}
	int nSelectItem	 =  m_StepTraceList.GetNextSelectedItem(pos);
	CString  Desc	 =  m_StepTraceList.GetItemText(nSelectItem,0);
	CString  ShowNum =  m_StepTraceList.GetItemText(nSelectItem,1);
	CString  Sum	 =  m_StepTraceList.GetItemText(nSelectItem,2);
	CString  Param	 =  m_StepTraceList.GetItemText(nSelectItem,3);
	CString  ID		 =  m_StepTraceList.GetItemText(nSelectItem,4);
	GetDlgItem(IDC_EDIT7)->SetWindowText(Desc);
	GetDlgItem(IDC_EDIT8)->SetWindowText(ShowNum);
	GetDlgItem(IDC_EDIT9)->SetWindowText(Sum);
	GetDlgItem(IDC_EDIT10)->SetWindowText(Param);
	GetDlgItem(IDC_EDIT11)->SetWindowText(ID);
	*pResult = 0;
}
//选中任务任务目标列表中的某一项进行编辑
void CStep::OnNMClickList4(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION   pos   =   m_StepAimList.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{	return;		}
	int nSelectItem= m_StepAimList.GetNextSelectedItem(pos);
	CString  ID			=  m_StepAimList.GetItemText(nSelectItem,0);
	CString AimModel	=  m_StepAimList.GetItemText(nSelectItem,1);
	CString AimType		=  m_StepAimList.GetItemText(nSelectItem,2);
	CString AimName		=  m_StepAimList.GetItemText(nSelectItem,3);
	CString AimNum		=  m_StepAimList.GetItemText(nSelectItem,4);
	CString DropFrom	=  m_StepAimList.GetItemText(nSelectItem,5);
	CString DropOdds	=  m_StepAimList.GetItemText(nSelectItem,6);
	CString param	    =  m_StepAimList.GetItemText(nSelectItem,7);
	GetDlgItem(IDC_EDIT31)->SetWindowText(ID);
	GetDlgItem(IDC_EDIT27)->SetWindowText(AimModel);
	GetDlgItem(IDC_EDIT23)->SetWindowText(AimType);
	GetDlgItem(IDC_EDIT22)->SetWindowText(AimName);
	GetDlgItem(IDC_EDIT21)->SetWindowText(AimNum);
	GetDlgItem(IDC_EDIT18)->SetWindowText(DropFrom);
	GetDlgItem(IDC_EDIT19)->SetWindowText(DropOdds);
	GetDlgItem(IDC_EDIT20)->SetWindowText(param);
	*pResult = 0;
}
/*功能:获取修改后的页面上的任务步骤。
*摘要:获取当前页面中的任务的步骤,并保存到相应的变量中去。
*参数:UpdatedStep用来保存更新后的任务步骤
*返回值:void
*作者：王 钦
*创建日期:2010-7-6
*/
bool CStep::GetUpdatedStep(tagRoleStep* UpdatedStep)
{
	pUpdatedStep = new tagRoleStep;
	CString strID ;
	CString strNPCID;
	CString strDiscrib;
	CString strNPCEffect_1;
	CString strNPCEffect_2;
	CString strEncouragementCell;
	CString strRoleTraceStartRow;
	///////////////////////////
	GetDlgItem(IDC_EDIT17)->GetWindowText(strID);
	GetDlgItem(IDC_EDIT2)->GetWindowText(strNPCID);
	GetDlgItem(IDC_EDIT3)->GetWindowText(strNPCEffect_1);
	GetDlgItem(IDC_EDIT4)->GetWindowText(strNPCEffect_2);
	GetDlgItem(IDC_EDIT5)->GetWindowText(strDiscrib);
	GetDlgItem(IDC_EDIT6)->GetWindowText(strRoleTraceStartRow);
	GetDlgItem(IDC_EDIT30)->GetWindowText(strEncouragementCell);
	////////////////////////////////
	pUpdatedStep->m_nRoleID = m_nRoleID;
	pUpdatedStep->m_cNPCID = strNPCID;
	pUpdatedStep->m_nID = atoi(strID);
	pUpdatedStep->m_nNPCEffect_1 = atoi(strNPCEffect_1);
	pUpdatedStep->m_nNPCEffect_2 = atoi(strNPCEffect_2);
	pUpdatedStep->m_nEncouragementCell = atoi(strEncouragementCell);

	int AimCount = m_StepAimList.GetItemCount();
	for (int i = 0; i<AimCount;i++)
	{
		tagStepAim *StepAim = new tagStepAim;
		StepAim->m_nID			= atoi(m_StepAimList.GetItemText(i,0));
		StepAim->m_nAimModel	= atoi(m_StepAimList.GetItemText(i,1));
		StepAim->m_nAimType		= atoi(m_StepAimList.GetItemText(i,2));
		StepAim->m_cAimName		=	  (m_StepAimList.GetItemText(i,3));
		StepAim->m_nAimNum		= atoi(m_StepAimList.GetItemText(i,4));
		StepAim->m_cDropFrom	=     (m_StepAimList.GetItemText(i,5));
		StepAim->m_nDropOdds	= atoi(m_StepAimList.GetItemText(i,6));
		StepAim->m_nParam		= atoi(m_StepAimList.GetItemText(i,7));
		pUpdatedStep->vcStepAim.push_back (StepAim);
	}

//读取页面中任务追踪列表中的信息
	int TraceCount = m_StepTraceList.GetItemCount();
	for (int i = 0; i<TraceCount;i++)
	{
		tagStepTrace* StepTrace = new tagStepTrace;
		StepTrace->m_nDesc		= atoi(m_StepTraceList.GetItemText(i,0));
		StepTrace->m_nShowNum	= atoi(m_StepTraceList.GetItemText(i,1));
		StepTrace->m_nSum		= atoi(m_StepTraceList.GetItemText(i,2));
		StepTrace->m_nParam		= atoi(m_StepTraceList.GetItemText(i,3));
		StepTrace->m_nID		= atoi(m_StepTraceList.GetItemText(i,4));
		pUpdatedStep->vcStepRoleTrace.push_back(StepTrace);
	}

	UpdatedStep = pUpdatedStep;
    return true;
}


void CStep::OnEnChangeEdit3()
{
    // TODO:  如果该控件是 RICHEDIT 控件，则它将不会
    // 发送该通知，除非重写 CDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    POSITION   pos   =   m_StepTextList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    int nSelectItem= m_StepTextList.GetNextSelectedItem(pos);
    CString selectType =  m_StepTextList.GetItemText(nSelectItem,0);
    CString selectNum =   m_StepTextList.GetItemText(nSelectItem,1);
   

    char  QForMat [256];
    char  QQuestId[32];

    sprintf_s( QForMat , "textres/questtext%d.xml", m_nRoleID/1000 );
    sprintf_s( QQuestId , "Q%d", m_nRoleID);

    CXmlEdit::MapTextFileTable::iterator  MapIter = GetInst(CXmlEdit).m_AllQuestTextFile.find( QForMat );

    if ( MapIter!= GetInst(CXmlEdit).m_AllQuestTextFile.end() )
    {
        UpdateData( TRUE );

        map<CString,CString>::iterator  TextIter1 = MapIter->second.find( selectType );

        char  QText[5120];
        m_TextContext.GetWindowText( QText , 5120 );
        map<CString,CString>::iterator  TextIter2 = MapIter->second.find( QText );

        if ( strcmp( selectType.GetBuffer(),QText) == 0 )
        {
            return ;
        }
        if ( TextIter1 != MapIter->second.end() )
        {
            TextIter1->second = QText;
        }

        m_StepTextList.SetItemText(nSelectItem,1,QText);
    }

}

void CStep::OnEnChangeEdit1()
{
    // TODO:  如果该控件是 RICHEDIT 控件，则它将不会
    // 发送该通知，除非重写 CDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    POSITION   pos   =   m_StepTextList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    int nSelectItem= m_StepTextList.GetNextSelectedItem(pos);
    CString selectType =  m_StepTextList.GetItemText(nSelectItem,0);
    CString selectNum =   m_StepTextList.GetItemText(nSelectItem,1);


    char  QForMat [256];
    char  QQuestId[32];

    sprintf_s( QForMat , "textres/questtext%d.xml", m_nRoleID/1000 );
    sprintf_s( QQuestId , "Q%d", m_nRoleID);

    CXmlEdit::MapTextFileTable::iterator  MapIter = GetInst(CXmlEdit).m_AllQuestTextFile.find( QForMat );

    if ( MapIter!= GetInst(CXmlEdit).m_AllQuestTextFile.end() )
    {
        UpdateData( TRUE );

        map<CString,CString>::iterator  TextIter1 = MapIter->second.find( selectType );

        char  QText[256];
        m_TextIndex.GetWindowText( QText , 256 );

        map<CString,CString>::iterator  TextIter2 = MapIter->second.find( QText );
        if ( strcmp( selectType.GetBuffer(),QText) == 0 )
        {
            return ;
        }
        if ( TextIter2 != MapIter->second.end() )
        {
            MessageBox("存在重复的索引文本！");
            return ;
        }
        if ( TextIter1 != MapIter->second.end() )
        {
            MapIter->second.erase( TextIter1 );
            MapIter->second.insert( std::make_pair(QText,selectNum) );
        }

        m_StepTextList.SetItemText(nSelectItem,0,QText);
    }
}
void CStep::OnEnChangeEdit7()
{
    POSITION   pos   =   m_StepTraceList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    vector<tagStepTrace*>::iterator TraceIter = m_pSlectRoleStep->vcStepRoleTrace.begin() ;

    int nSelectItem= m_StepTraceList.GetNextSelectedItem(pos);
    CString Context =  m_StepTraceList.GetItemText(nSelectItem,0);

    char  QText[256];
    m_TraceDes.GetWindowText( QText , 256 ); 

    if ( QText=="" || strcmp(Context.GetBuffer(),QText) == 0 )
    {
        return ;
    }

    for( ; TraceIter != m_pSlectRoleStep->vcStepRoleTrace.end() ; TraceIter ++  )
    {
        if ( strcmp( (*TraceIter)->m_nDesc.c_str(),QText) == 0  )
        {
            MessageBox("不允许存在相同的索引ＩＤ!");
            m_TraceDes.SetWindowText(Context);
            return ;
        }
    }

    TraceIter = m_pSlectRoleStep->vcStepRoleTrace.begin() ;
    for( ; TraceIter != m_pSlectRoleStep->vcStepRoleTrace.end() ; TraceIter ++  )
    {
        if ( strcmp( (*TraceIter)->m_nDesc.c_str(),Context.GetBuffer()) == 0  )
        {
               (*TraceIter)->m_nDesc = QText ;
               m_StepTraceList.SetItemText(nSelectItem,0,QText);
               break;
        }
    }
}

void CStep::OnEnChangeEdit8()
{
    POSITION   pos   =   m_StepTraceList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    vector<tagStepTrace*>::iterator TraceIter = m_pSlectRoleStep->vcStepRoleTrace.begin() ;

    int nSelectItem= m_StepTraceList.GetNextSelectedItem(pos);
    CString Context =  m_StepTraceList.GetItemText(nSelectItem,0);

    char  QText[256];
    m_TraceShowNum.GetWindowText( QText , 256 ); 

    for( ; TraceIter != m_pSlectRoleStep->vcStepRoleTrace.end() ; TraceIter ++  )
    {
        if ( strcmp( (*TraceIter)->m_nDesc.c_str(),Context.GetBuffer()) == 0  )
        {
            (*TraceIter)->m_nShowNum = atoi(QText)  ;
            m_StepTraceList.SetItemText(nSelectItem,1,QText);
            break;
        }
    }
}

void CStep::OnEnChangeEdit9()
{
    POSITION   pos   =   m_StepTraceList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    vector<tagStepTrace*>::iterator TraceIter = m_pSlectRoleStep->vcStepRoleTrace.begin() ;

    int nSelectItem= m_StepTraceList.GetNextSelectedItem(pos);
    CString Context =  m_StepTraceList.GetItemText(nSelectItem,0);

    char  QText[256];
    m_TraceSum.GetWindowText( QText , 256 ); 

    for( ; TraceIter != m_pSlectRoleStep->vcStepRoleTrace.end() ; TraceIter ++  )
    {
        if ( strcmp( (*TraceIter)->m_nDesc.c_str(),Context.GetBuffer()) == 0  )
        {
            (*TraceIter)->m_nSum = atoi(QText) ;
            m_StepTraceList.SetItemText(nSelectItem,2,QText);
            break;
        }
    }
}

void CStep::OnEnChangeEdit10()
{
    POSITION   pos   =   m_StepTraceList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }
    vector<tagStepTrace*>::iterator TraceIter = m_pSlectRoleStep->vcStepRoleTrace.begin() ;

    int nSelectItem= m_StepTraceList.GetNextSelectedItem(pos);
    CString Context =  m_StepTraceList.GetItemText(nSelectItem,0);

    char  QText[256];
    m_TraceParam.GetWindowText( QText , 256 ); 

    for( ; TraceIter != m_pSlectRoleStep->vcStepRoleTrace.end() ; TraceIter ++  )
    {
        if ( strcmp( (*TraceIter)->m_nDesc.c_str(),Context.GetBuffer()) == 0  )
        {
            (*TraceIter)->m_nParam = atoi(QText) ;
            m_StepTraceList.SetItemText(nSelectItem,3,QText);
            break;
        }
    }
}

void CStep::OnEnChangeEdit11()
{
    POSITION   pos   =   m_StepTraceList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

     vector<tagStepTrace*>::iterator TraceIter = m_pSlectRoleStep->vcStepRoleTrace.begin() ;

    int nSelectItem= m_StepTraceList.GetNextSelectedItem(pos);
    CString Context =  m_StepTraceList.GetItemText(nSelectItem,0);

    char  QText[256];
    m_TraceId.GetWindowText( QText , 256 ); 

    for( ; TraceIter != m_pSlectRoleStep->vcStepRoleTrace.end() ; TraceIter ++  )
    {
        if ( strcmp( (*TraceIter)->m_nDesc.c_str(),Context.GetBuffer()) == 0  )
        {
            (*TraceIter)->m_nID = atoi(QText) ;
            m_StepTraceList.SetItemText(nSelectItem,4,QText);
            break;
        }
    }
}

void CStep::OnEnChangeEdit31()
{
    POSITION   pos   =   m_StepAimList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    vector<tagStepAim*>::iterator AimIter = m_pSlectRoleStep->vcStepAim.begin() ;

    int nSelectItem= m_StepAimList.GetNextSelectedItem(pos);

    /// 获得列表值
    CString Context =  m_StepAimList.GetItemText(nSelectItem,0);

    /// 获得Edith值
    char  QText[256];
    m_AimId.GetWindowText( QText , 256 ); 

    if ( QText=="" || strcmp(Context.GetBuffer(),QText) == 0 )
    {
        return ;
    }

    for( ; AimIter != m_pSlectRoleStep->vcStepAim.end() ; AimIter ++  )
    {
        if ( (*AimIter)->m_nID == atoi(QText) )
        {
            MessageBox("不允许存在相同的ＩＤ!");
            m_AimId.SetWindowText(Context);
            return ;
        }
    }

    AimIter = m_pSlectRoleStep->vcStepAim.begin();
    int i = 0;
    for( ; AimIter != m_pSlectRoleStep->vcStepAim.end() ; AimIter ++  )
    {
        if ( (*AimIter)->m_nID == atoi(Context.GetBuffer()) )
        {
            (*AimIter)->m_nID = atoi(QText) ;
            m_StepAimList.SetItemText(nSelectItem,0,QText);
            break;
        }
        i++;
    }
}

void CStep::OnEnChangeEdit27()
{
    POSITION   pos   =   m_StepAimList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    vector<tagStepAim*>::iterator AimIter = m_pSlectRoleStep->vcStepAim.begin() ;

    int nSelectItem= m_StepAimList.GetNextSelectedItem(pos);
    CString Context =  m_StepAimList.GetItemText(nSelectItem,0);

    char  QText[256];
    m_AimModel.GetWindowText( QText , 256 ); 

    for( ; AimIter != m_pSlectRoleStep->vcStepAim.end() ; AimIter ++  )
    {
        if ( (*AimIter)->m_nID == atoi(Context.GetBuffer()) )
        {
            (*AimIter)->m_nAimModel = atoi(QText) ;
            m_StepAimList.SetItemText(nSelectItem,1,QText);
            break;
        }
    }
}

void CStep::OnEnChangeEdit23()
{
    POSITION   pos   =   m_StepAimList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    vector<tagStepAim*>::iterator AimIter = m_pSlectRoleStep->vcStepAim.begin() ;

    int nSelectItem= m_StepAimList.GetNextSelectedItem(pos);
    CString Context =  m_StepAimList.GetItemText(nSelectItem,0);

    char  QText[256];
    m_AimType.GetWindowText( QText , 256 ); 

    for( ; AimIter != m_pSlectRoleStep->vcStepAim.end() ; AimIter ++  )
    {
        if ( (*AimIter)->m_nID == atoi(Context.GetBuffer()) )
        {
            (*AimIter)->m_nAimType = atoi(QText) ;
            m_StepAimList.SetItemText(nSelectItem,2,QText);
            break;
        }
    }
}

void CStep::OnEnChangeEdit22()
{
    POSITION   pos   =   m_StepAimList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    vector<tagStepAim*>::iterator AimIter = m_pSlectRoleStep->vcStepAim.begin() ;

    int nSelectItem= m_StepAimList.GetNextSelectedItem(pos);
    CString Context =  m_StepAimList.GetItemText(nSelectItem,0);

    char  QText[256];
    m_AimName.GetWindowText( QText , 256 ); 

    for( ; AimIter != m_pSlectRoleStep->vcStepAim.end() ; AimIter ++  )
    {
        if ( (*AimIter)->m_nID == atoi(Context.GetBuffer()) )
        {
            (*AimIter)->m_cAimName = QText ;
            m_StepAimList.SetItemText(nSelectItem,3,QText);
            break;
        }
    }
}

void CStep::OnEnChangeEdit21()
{
    POSITION   pos   =   m_StepAimList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    vector<tagStepAim*>::iterator AimIter = m_pSlectRoleStep->vcStepAim.begin() ;

    int nSelectItem= m_StepAimList.GetNextSelectedItem(pos);
    CString Context =  m_StepAimList.GetItemText(nSelectItem,0);

    char  QText[256];
    m_AimNum.GetWindowText( QText , 256 ); 

    for( ; AimIter != m_pSlectRoleStep->vcStepAim.end() ; AimIter ++  )
    {
        if ( (*AimIter)->m_nID == atoi(Context.GetBuffer()) )
        {
            (*AimIter)->m_nAimNum = atoi(QText) ;
            m_StepAimList.SetItemText(nSelectItem,4,QText);
            break;
        }
    }
}

void CStep::OnEnChangeEdit19()
{
    POSITION   pos   =   m_StepAimList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    vector<tagStepAim*>::iterator AimIter = m_pSlectRoleStep->vcStepAim.begin() ;

    int nSelectItem= m_StepAimList.GetNextSelectedItem(pos);
    CString Context =  m_StepAimList.GetItemText(nSelectItem,0);

    char  QText[256];
    m_DropOdds.GetWindowText( QText , 256 ); 

    for( ; AimIter != m_pSlectRoleStep->vcStepAim.end() ; AimIter ++  )
    {
        if ( (*AimIter)->m_nID == atoi(Context.GetBuffer()) )
        {
            (*AimIter)->m_nDropOdds = atoi(QText) ;
            m_StepAimList.SetItemText(nSelectItem,6,QText);
            break;
        }
    }
}

void CStep::OnEnChangeEdit18()
{
    POSITION   pos   =   m_StepAimList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    vector<tagStepAim*>::iterator AimIter = m_pSlectRoleStep->vcStepAim.begin() ;

    int nSelectItem= m_StepAimList.GetNextSelectedItem(pos);
    CString Context =  m_StepAimList.GetItemText(nSelectItem,0);

    char  QText[256];
    m_DropFrom.GetWindowText( QText , 256 ); 

    for( ; AimIter != m_pSlectRoleStep->vcStepAim.end() ; AimIter ++  )
    {
        if ( (*AimIter)->m_nID == atoi(Context.GetBuffer()) )
        {
            (*AimIter)->m_cDropFrom = QText ;
            m_StepAimList.SetItemText(nSelectItem,5,QText);
            break;
        }
    }
}

void CStep::OnEnChangeEdit20()
{
    POSITION   pos   =   m_StepAimList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    vector<tagStepAim*>::iterator AimIter = m_pSlectRoleStep->vcStepAim.begin() ;

    int nSelectItem= m_StepAimList.GetNextSelectedItem(pos);
    CString Context =  m_StepAimList.GetItemText(nSelectItem,0);

    char  QText[256];
    m_AimParam.GetWindowText( QText , 256 ); 

    for( ; AimIter != m_pSlectRoleStep->vcStepAim.end() ; AimIter ++  )
    {
        if ( (*AimIter)->m_nID == atoi(Context.GetBuffer()) )
        {
            (*AimIter)->m_nParam = atoi(QText) ;
            m_StepAimList.SetItemText(nSelectItem,7,QText);
            break;
        }
    }
}
