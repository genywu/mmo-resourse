// Attribute.cpp : 实现文件
#include "stdafx.h"
#include "QuestEditor.h"
#include "CAttribute.h"
#include <string.h>
#include <vector>
#include "tinyxml.h"
#include "CXmlEdit.h"
#include "CFileSave.h"
#include "struct.h"
#include "Tools.h"
// Attribute 对话框
IMPLEMENT_DYNAMIC(CAttribute,CDialog)
CAttribute::CAttribute(CWnd* pParent /*=NULL*/)
	: CDialog(CAttribute::IDD, pParent)
	, m_RoleID(0)
	, m_RoleType(0)      // 携带宠物
	, m_RoleEmbracer(0)
	, m_HardFactor(-1)
	, m_DeleteScript(_T(""))
	, m_RoleName(_T(""))
	, m_EmbracerEffrct(-1)
	, m_FinishEmbracerEffect(-1)
	, m_EmbracerMapEffect(-1)
	, m_Odds(-1)
	, m_MassPoint(-1)
	, m_DiscribTextID(_T(""))
	, m_MiniGrade(1)
	, m_MaxGrade(999)
	, m_EngageArmsRepute(0)
	, m_NPCShowType(-1)
	, m_NPCID(_T(""))
	, m_FinishEffect(-1)
	, m_NPCClewType(-1)
	, m_BeSearchDesc(_T(""))
	, m_BeSearchSpDesc(_T(""))
	, m_ChapterNum(-1)
	, m_picture(-1)
	, m_Icon(-1)
	, m_StoryDesc(_T(""))
	, m_BeRecord(0)
	, m_ShowAble(1)
	, m_Repeated(0)
	, m_Deadlost(-1)
	, m_LostRepeated(-1)
    , m_Sex(_T("-1"))
    , m_ReqSkill(-1)
    , m_DDoc(_T(""))
    , m_Occupation(_T(""))
    , m_QuestType(_T("1"))
    , m_QuestShare(_T("0"))
    , m_DelScript(_T("scripts/questmodel/quest_model_del_3.lua"))
    , m_TotePet(-1)
//     , m_EncouragementCell(0)
//     , m_AwardName(_T(""))
//     , m_AwardNum(0)
//     , m_AwardOdds(0)
    , m_AwardTypeDes(_T(""))
{
    isLoading = false;
    m_pSelectedRoleAttribute = NULL;
    m_RoleIndex = "";
}

CAttribute::~CAttribute()
{
}
// DDX/DDV 支持
void CAttribute::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_RoleID);
    DDX_Text(pDX, IDC_EDIT4, m_HardFactor);
    DDV_MinMaxInt(pDX, m_HardFactor, -1, 999999);
    DDX_Text(pDX, IDC_EDIT12, m_RoleName);
    DDX_Text(pDX, IDC_EDIT19, m_EmbracerEffrct);
    DDV_MinMaxInt(pDX, m_EmbracerEffrct,-1,999999);
    DDX_Text(pDX, IDC_EDIT26, m_FinishEmbracerEffect);
    DDV_MinMaxInt(pDX, m_FinishEmbracerEffect, -1, 999999);
    DDX_Text(pDX, IDC_EDIT6, m_EmbracerMapEffect);
    DDV_MinMaxInt(pDX, m_EmbracerMapEffect, -1, 999999);
    DDX_Text(pDX, IDC_EDIT27, m_Odds);
    DDV_MinMaxInt(pDX, m_Odds, -1, 999999);
    DDX_Text(pDX, IDC_EDIT7, m_MassPoint);
    DDV_MinMaxInt(pDX, m_MassPoint, -1, 999999);
    DDX_Text(pDX, IDC_EDIT14, m_DiscribTextID);
    DDX_Text(pDX, IDC_EDIT21, m_MiniGrade);
    DDV_MinMaxInt(pDX, m_MiniGrade, 0, 999999);
    DDX_Text(pDX, IDC_EDIT28, m_MaxGrade);
    DDV_MinMaxInt(pDX, m_MaxGrade, 0, 999999);
    DDX_Text(pDX, IDC_EDIT32, m_EngageArmsRepute);
    DDV_MinMaxInt(pDX, m_EngageArmsRepute, -1, 999999);
    DDX_Text(pDX, IDC_EDIT9, m_NPCShowType);
    DDV_MinMaxInt(pDX, m_NPCShowType, -1, 999999);
    DDX_Text(pDX, IDC_EDIT16, m_NPCID);
    DDX_Text(pDX, IDC_EDIT23, m_FinishEffect);
    DDV_MinMaxInt(pDX, m_FinishEffect, -1, 999999);
    DDX_Text(pDX, IDC_EDIT30, m_NPCClewType);
    DDV_MinMaxInt(pDX, m_NPCClewType, -1, 999999);
    DDX_Text(pDX, IDC_EDIT10, m_BeSearchDesc);
    DDX_Text(pDX, IDC_EDIT17, m_BeSearchSpDesc);
    DDX_Text(pDX, IDC_EDIT11, m_ChapterNum);
    DDV_MinMaxInt(pDX, m_ChapterNum, -1, 999999);
    DDX_Text(pDX, IDC_EDIT15, m_picture);
    DDV_MinMaxInt(pDX, m_picture, -1, 999999);
    DDX_Text(pDX, IDC_EDIT25, m_Icon);
    DDV_MinMaxInt(pDX, m_Icon, -1, 999999);
    DDX_Text(pDX, IDC_EDIT29, m_StoryDesc);
    DDX_Text(pDX, IDC_EDIT24, m_RoleIndex);
    DDX_Text(pDX, IDC_EDIT13, m_BeRecord);
    DDV_MinMaxInt(pDX, m_BeRecord, 0, 1);
    DDX_Text(pDX, IDC_EDIT18, m_ShowAble);
    DDV_MinMaxInt(pDX, m_ShowAble, 0, 1);
    DDX_Text(pDX, IDC_EDIT33, m_Repeated);
    DDV_MinMaxInt(pDX, m_Repeated, 0, 1);
    DDX_Text(pDX, IDC_EDIT22, m_Deadlost);
    DDV_MinMaxInt(pDX, m_Deadlost, -1, 1);
    DDX_Text(pDX, IDC_EDIT20, m_LostRepeated);
    DDV_MinMaxInt(pDX, m_LostRepeated, -1, 1);
    DDX_CBString(pDX, IDC_COMBO5, m_Sex);
    DDX_Text(pDX, IDC_EDIT31, m_ReqSkill);
    DDX_Text(pDX, IDC_EDIT34, m_DDoc);
    DDX_Text(pDX, IDC_EDIT35, m_Occupation);
    
    DDX_CBString(pDX, IDC_COMBO2, m_QuestType);
    DDX_CBString(pDX, IDC_COMBO4, m_QuestShare);
    DDX_CBString(pDX, IDC_COMBO3, m_DelScript);
    DDX_Text(pDX, IDC_EDIT39, m_TotePet);
    DDV_MinMaxLong(pDX, m_TotePet, -1, 100000000);
    DDX_Text(pDX, IDC_EDIT38, m_AwardTypeDes);
    DDX_Control(pDX, IDC_EDIT37, m_EncouragementCell);
    DDX_Control(pDX, IDC_COMBO6, m_AwardType);
    DDX_Control(pDX, IDC_EDIT3, m_AwardName);
    DDX_Control(pDX, IDC_EDIT36, m_AwardOdds);

    if( !isLoading )
    {
        DDX_Control(pDX, IDC_LIST1, m_EncouragementList);
        m_EncouragementList.SetExtendedStyle(
            LVS_EX_FLATSB
            |LVS_EX_FULLROWSELECT
            |LVS_EX_HEADERDRAGDROP
            |LVS_EX_ONECLICKACTIVATE
            |LVS_EX_GRIDLINES
            );
        m_EncouragementList.InsertColumn(0,"奖励类型",LVCFMT_LEFT,60,0);
        m_EncouragementList.InsertColumn(1,"奖励数量",LVCFMT_LEFT,60,0);
        m_EncouragementList.InsertColumn(2,"奖励名称",LVCFMT_LEFT,150,0);
        m_EncouragementList.InsertColumn(3,"奖励倍率",LVCFMT_LEFT,60,0);
        isLoading = true;   
    }



    DDX_Control(pDX, IDC_EDIT8, m_AwardNum);
}
//回车键响应
void CAttribute::OnOK()
{
}
BEGIN_MESSAGE_MAP(CAttribute, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CAttribute::OnBnClickedButton1)
    ON_NOTIFY(NM_CLICK, IDC_LIST1, &CAttribute::OnNMClickList)
    ON_CBN_SELCHANGE(IDC_COMBO6, &CAttribute::OnCbnSelchangeCombo1)
    ON_EN_CHANGE(IDC_EDIT37, &CAttribute::OnEnChangeEdit37)
    ON_EN_CHANGE(IDC_EDIT3, &CAttribute::OnEnChangeEdit3)
    ON_EN_CHANGE(IDC_EDIT8, &CAttribute::OnEnChangeEdit8)
    ON_EN_CHANGE(IDC_EDIT36, &CAttribute::OnEnChangeEdit36)
    ON_BN_CLICKED(IDC_BUTTON7, &CAttribute::OnBnClickedButton7)
    ON_BN_CLICKED(IDC_BUTTON8, &CAttribute::OnBnClickedButton8)
END_MESSAGE_MAP()


void CAttribute::OnCbnSelchangeCombo1()
{  
   UpdateData(true);

   int sel = m_AwardType.GetCurSel();
   if ( sel < 0 )
   {
       return ;
   }
   char  QText[256];
   m_AwardType.GetLBText( sel, QText );
   GetDlgItem(IDC_EDIT38)->SetWindowText( GetAwardDes(QText ) );

   POSITION   pos =  m_EncouragementList.GetFirstSelectedItemPosition();
   if(pos==NULL)
   {	
       return;		
   }

   int nSelectItem     = m_EncouragementList.GetNextSelectedItem(pos);
   CString selectNum   = m_EncouragementList.GetItemText(nSelectItem,0);

   if ( strcmp(selectNum.GetBuffer(),QText) == 0 )
   {
       return ;
   }

   std::map<long,CString>::iterator   AwardFile = GetInst(CXmlEdit).m_AwardIdFileList.find( m_QuestId );
   if ( AwardFile == GetInst(CXmlEdit).m_AwardIdFileList.end() )
   {
       return;
   } 
   //= *Awardtable 
   CXmlEdit::MapAwardFileTable::iterator fileIter= GetInst(CXmlEdit).m_AllAwardList.find( AwardFile->second );
   if ( fileIter == GetInst(CXmlEdit).m_AllAwardList.end())
   {
       return ;
   }
   CXmlEdit::MapAwardTable::iterator   AwardIter = (*(fileIter->second)).find( m_QuestId );
   if ( AwardIter != (*(fileIter->second)).end() )
   {
       long  AwardNum = (long)AwardIter->second.size() ;
       if( AwardNum < nSelectItem )
       {
           return ;           
       }
       AwardIter->second[nSelectItem]->m_nType = atoi(QText);
       m_EncouragementList.SetItemText(nSelectItem,0,QText);
   }

}

// Attribute 消息处理程序
/*功能:实现任务属性的显示。
*摘要:根据任务的ID获取任务属性然后显示，点击树型列表中的任务属性项的时候调用该函数。
*参数:pItemText是树型列表中节点的名称,包含了任务的ID
*返回值:void
*作者：王 钦
*创建日期:2010-7-6
*/
void CAttribute::ReadRoleAttribute(CString *pItemText)
{	
	int id = atoi(pItemText->GetString());
    m_QuestId = id;
  	m_RoleID = id;
    char strVal[256];

    m_RoleIndex ="";
    std::map<long,CString>::iterator FileIter = GetInst(CXmlEdit).m_AllQuestStrId.find ( m_RoleID );
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
            if ( (*iter)->m_nID == m_RoleID )
            {
                m_pSelectedRoleAttribute = (*iter)->vcRole_Attribute;
                
                for( int i =  0 ; i < (int)(*iter)->m_nRoleIndex.size(); i++   )
                {
                    m_RoleIndex  +=itoa( (*iter)->m_nRoleIndex[i] , strVal , 10 );
                    m_RoleIndex  +="/";
                }
                m_pRole = *iter  ;
                break;
            }
        }
    }

	if ( m_pSelectedRoleAttribute != NULL )
	{
		m_RoleType = m_pSelectedRoleAttribute->m_QuestType;
		m_RoleEmbracer = m_pSelectedRoleAttribute->m_nRoleEmbracer;
		m_HardFactor = m_pSelectedRoleAttribute->m_nHardFactor;
		m_DeleteScript = (m_pSelectedRoleAttribute->m_cDeleteScript).c_str();

        char  QText[256];
        sprintf_s( QText ,"Q%d" , m_RoleID );
        map<CString,CString>::iterator  sitr = GetInst(CXmlEdit).m_TextRes.find( QText );        
        m_RoleName = (m_pSelectedRoleAttribute->m_cName).c_str();  
        if( sitr != GetInst(CXmlEdit).m_TextRes.end() )
        {
             m_RoleName = sitr->second;
        }
		m_EmbracerEffrct = m_pSelectedRoleAttribute->m_nEmbracerEffect;
		m_EmbracerMapEffect = m_pSelectedRoleAttribute->m_nEmbracerMapEffect;
		m_FinishEffect = m_pSelectedRoleAttribute->m_nFinishEffect;
		m_FinishEmbracerEffect = m_pSelectedRoleAttribute->m_nFinishEmbracerEffect;
		m_BeRecord = m_pSelectedRoleAttribute->m_nBeRecord;
		m_ShowAble = m_pSelectedRoleAttribute->m_nShowAble;
		m_Deadlost = m_pSelectedRoleAttribute->m_nDeadLost;
		m_LostRepeated = m_pSelectedRoleAttribute->m_nLostRepeated;
		m_Repeated = m_pSelectedRoleAttribute->m_nRepeated;
		m_MassPoint = m_pSelectedRoleAttribute->m_nMassPoint;
		m_DiscribTextID = m_pSelectedRoleAttribute->m_nDiscribTextID;
		m_MiniGrade = m_pSelectedRoleAttribute->m_nMiniGrade;
		m_MaxGrade = m_pSelectedRoleAttribute->m_nMaxGrade;
		m_EngageArmsRepute = m_pSelectedRoleAttribute->m_nEngageArmsRepute;
		m_ChapterNum = m_pSelectedRoleAttribute->m_nChapterNum;
		m_picture = m_pSelectedRoleAttribute->m_nPicture;
		m_Icon = m_pSelectedRoleAttribute->m_nIcon;
		m_StoryDesc = m_pSelectedRoleAttribute->m_nStoryDesc;
		m_NPCClewType = m_pSelectedRoleAttribute->m_nNPCClewType;
		m_NPCID = (m_pSelectedRoleAttribute->m_cNPCID).c_str();
		m_NPCShowType = m_pSelectedRoleAttribute->m_nNPCShowType;
		m_BeSearchDesc = m_pSelectedRoleAttribute->m_nBeSearchDesc;
		m_BeSearchSpDesc = m_pSelectedRoleAttribute->m_nBeSearchSpDesc;
		m_Odds = m_pSelectedRoleAttribute->m_nOdds;
        m_Sex =  itoa(m_pSelectedRoleAttribute->m_QuestSex,strVal,10);
        m_ReqSkill = m_pSelectedRoleAttribute->m_ReqSkill;
        m_DDoc= m_pSelectedRoleAttribute->m_Doccu.c_str() ;
        m_Occupation = m_pSelectedRoleAttribute->m_Occupation.c_str();
        m_QuestType  = itoa(m_pSelectedRoleAttribute->m_QuestType,strVal,10);
        m_QuestShare = itoa(m_pSelectedRoleAttribute->m_ShareQuest,strVal,10);
        m_DelScript  = m_pSelectedRoleAttribute->m_cDeleteScript.c_str();
        m_TotePet    = m_pSelectedRoleAttribute->m_TotePet;

        m_EngageArmsRepute = m_pSelectedRoleAttribute->m_NextQuest;
	}

    UpdateData(false);
    m_EncouragementList.DeleteAllItems();
    
    std::map<long,CString>::iterator   AwardFile = GetInst(CXmlEdit).m_AwardIdFileList.find( m_QuestId );
    if ( AwardFile == GetInst(CXmlEdit).m_AwardIdFileList.end() )
    {
        return;
    } 
    //= *Awardtable 
    CXmlEdit::MapAwardFileTable::iterator fileIter= GetInst(CXmlEdit).m_AllAwardList.find( AwardFile->second );
    if ( fileIter == GetInst(CXmlEdit).m_AllAwardList.end())
    {
        return ;
    }
    CXmlEdit::MapAwardTable::iterator   AwardIter = (*(fileIter->second)).find( m_QuestId );
    if ( AwardIter != (*(fileIter->second)).end() )
    {
         long  AwardNum = (long)AwardIter->second.size() ;
         if( AwardNum > 0 )
         {
             GetDlgItem(IDC_EDIT37)->SetWindowText( itoa(AwardIter->second[0]->m_nEncouragementCell,strVal,10));
         }
         for ( long i = 0 ; i < AwardNum ; i ++ )
         {
             tagRoleEncouragement * tagEncour = AwardIter->second[i];
             m_EncouragementList.InsertItem(i,"");

             m_EncouragementList.SetItemText(i,0,itoa(tagEncour->m_nType,strVal,10));
             m_EncouragementList.SetItemText(i,1,itoa(tagEncour->m_nNum,strVal,10));
             m_EncouragementList.SetItemText(i,2,tagEncour->m_cName.c_str());

             sprintf_s( strVal,"%lf" , tagEncour->m_fOdds ); 
             m_EncouragementList.SetItemText(i,3,strVal);

         }
    }
}

//选中任务任务目标列表中的某一项进行编辑
void CAttribute::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
    if(!m_pSelectedRoleAttribute)
        return ;
    POSITION   pos =  m_EncouragementList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    int nSelectItem= m_EncouragementList.GetNextSelectedItem(pos);
    CString  Type		=  m_EncouragementList.GetItemText(nSelectItem,0);
    CString  Num	    =  m_EncouragementList.GetItemText(nSelectItem,1);
    CString  Name  	    =  m_EncouragementList.GetItemText(nSelectItem,2);
    CString  Odds   	=  m_EncouragementList.GetItemText(nSelectItem,3);

    GetDlgItem(IDC_COMBO6)->SetWindowText(Type);
    GetDlgItem(IDC_EDIT3)->SetWindowText(Num);
    GetDlgItem(IDC_EDIT8)->SetWindowText(Name);
    GetDlgItem(IDC_EDIT36)->SetWindowText(Odds);
    
    GetDlgItem(IDC_EDIT38)->SetWindowText( GetAwardDes(Type.GetString() ) );

}

//确定控件的实现代码
void CAttribute::OnBnClickedButton1()
{
     UpdateData( true );

     if( !m_pSelectedRoleAttribute )
         return ;
     m_pSelectedRoleAttribute->m_QuestType=m_RoleType ;
     m_pSelectedRoleAttribute->m_nRoleEmbracer=m_RoleEmbracer ;
     m_pSelectedRoleAttribute->m_nHardFactor=m_HardFactor ;
     m_pSelectedRoleAttribute->m_cDeleteScript=m_DeleteScript.GetBuffer() ;
     m_pSelectedRoleAttribute->m_cName=m_RoleName.GetBuffer() ;
     m_pSelectedRoleAttribute->m_nEmbracerEffect=m_EmbracerEffrct ;
     m_pSelectedRoleAttribute->m_nEmbracerMapEffect=m_EmbracerMapEffect ;
     m_pSelectedRoleAttribute->m_nFinishEffect=m_FinishEffect ;
     m_pSelectedRoleAttribute->m_nFinishEmbracerEffect=m_FinishEmbracerEffect ;
     m_pSelectedRoleAttribute->m_nBeRecord=m_BeRecord ;
     m_pSelectedRoleAttribute->m_nShowAble=m_ShowAble ;
     m_pSelectedRoleAttribute->m_nDeadLost=m_Deadlost ;
     m_pSelectedRoleAttribute->m_nLostRepeated=m_LostRepeated ;
     m_pSelectedRoleAttribute->m_nRepeated=m_Repeated ;
     m_pSelectedRoleAttribute->m_nMassPoint=m_MassPoint ;
     m_pSelectedRoleAttribute->m_nDiscribTextID=m_DiscribTextID ;
     m_pSelectedRoleAttribute->m_nMiniGrade=m_MiniGrade ;
     m_pSelectedRoleAttribute->m_nMaxGrade=m_MaxGrade ;
     m_pSelectedRoleAttribute->m_nEngageArmsRepute=m_EngageArmsRepute ;
     m_pSelectedRoleAttribute->m_nChapterNum=m_ChapterNum ;
     m_pSelectedRoleAttribute->m_nPicture=m_picture ;
     m_pSelectedRoleAttribute->m_nIcon=m_Icon ;
     m_pSelectedRoleAttribute->m_nStoryDesc=m_StoryDesc ;
     m_pSelectedRoleAttribute->m_nNPCClewType=m_NPCClewType ;
     m_pSelectedRoleAttribute->m_cNPCID=m_NPCID.GetBuffer() ;
     m_pSelectedRoleAttribute->m_nNPCShowType=m_NPCShowType ;
     m_pSelectedRoleAttribute->m_nBeSearchDesc=m_BeSearchDesc ;
     m_pSelectedRoleAttribute->m_nBeSearchSpDesc=m_BeSearchSpDesc ;
     m_pSelectedRoleAttribute->m_nOdds=m_Odds ;
     m_pSelectedRoleAttribute->m_QuestSex= atoi(m_Sex.GetBuffer()) ;
     m_pSelectedRoleAttribute->m_ReqSkill=m_ReqSkill ;
     m_pSelectedRoleAttribute->m_Doccu =m_DDoc.GetBuffer();
     m_pSelectedRoleAttribute->m_Occupation=m_Occupation.GetBuffer() ;
     m_pSelectedRoleAttribute->m_QuestType=atoi(m_QuestType.GetBuffer())  ;
     m_pSelectedRoleAttribute->m_ShareQuest=atoi(m_QuestShare.GetBuffer()) ;
     m_pSelectedRoleAttribute->m_cDeleteScript=m_DelScript.GetBuffer()  ;
     m_pSelectedRoleAttribute->m_TotePet=m_TotePet    ;

     m_pSelectedRoleAttribute->m_NextQuest=m_EngageArmsRepute ;

     char *p1=NULL,*p2=m_RoleIndex.GetBuffer();
     m_pRole->m_nRoleIndex.clear();
     p1= strtok(p2,"/");
     while ( p1 != NULL )
     {
         int len = atoi(p1) ;
         m_pRole->m_nRoleIndex.push_back(  len );
         p1= strtok(NULL,"/");
     }
}
/*功能:获取修改后的页面上的任务属性。
*摘要:获取当前页面中的任务的信息,并保存到相应的变量中去。
*参数:vcUpdatedeAttribute用来保存更新后的任务信息
*返回值:void
*作者：王 钦
*创建日期:2010-7-6
*/
bool CAttribute::GetUpdatedAttribute(vector<tagRoleAttribute*>& vcUpdatedeAttribute)
{
    if(!m_pSelectedRoleAttribute)
        return false;
	//将控件中的值赋值给相应的变量.并保存
	UpdateData(true);
	tagRoleAttribute* pRoleAttribute = new tagRoleAttribute;
	pRoleAttribute->m_nID = m_RoleID;
	pRoleAttribute->m_QuestType = m_RoleType;
	pRoleAttribute->m_nRoleEmbracer = m_RoleEmbracer;
	pRoleAttribute->m_nHardFactor = m_HardFactor;
	pRoleAttribute->m_cDeleteScript = m_DeleteScript;
	pRoleAttribute->m_cName = m_RoleName;
	pRoleAttribute->m_nEmbracerEffect = m_EmbracerEffrct;
	pRoleAttribute->m_nEmbracerMapEffect = m_EmbracerMapEffect;
	pRoleAttribute->m_nFinishEffect = m_FinishEffect;
	pRoleAttribute->m_nFinishEmbracerEffect = m_FinishEmbracerEffect;
	pRoleAttribute->m_nBeRecord = m_BeRecord;
	pRoleAttribute->m_nShowAble = m_ShowAble;
	pRoleAttribute->m_nDeadLost = m_Deadlost;
	pRoleAttribute->m_nLostRepeated = m_LostRepeated;
	pRoleAttribute->m_nRepeated = m_Repeated;
	pRoleAttribute->m_nMassPoint = m_MassPoint;
	pRoleAttribute->m_nDiscribTextID = m_DiscribTextID;
	pRoleAttribute->m_nMiniGrade = m_MiniGrade;
	pRoleAttribute->m_nMaxGrade = m_MaxGrade;
	pRoleAttribute->m_nEngageArmsRepute = m_EngageArmsRepute;
	pRoleAttribute->m_nChapterNum = m_ChapterNum;
	pRoleAttribute->m_nPicture = m_picture;
	pRoleAttribute->m_nIcon = m_Icon;
	pRoleAttribute->m_nStoryDesc = m_StoryDesc;
	pRoleAttribute->m_nNPCClewType = m_NPCClewType;
	pRoleAttribute->m_cNPCID = m_NPCID;
	pRoleAttribute->m_nNPCShowType = m_NPCShowType;
	pRoleAttribute->m_nBeSearchDesc = m_BeSearchDesc;
	pRoleAttribute->m_nBeSearchSpDesc = m_BeSearchSpDesc;
	pRoleAttribute->m_nOdds = m_Odds;
	pRoleAttribute->m_nRoleIndex = m_RoleIndex;
	vcUpdatedeAttribute.push_back(pRoleAttribute);	
	return true;
}

void CAttribute::OnEnChangeEdit37()
{
    // TODO:  如果该控件是 RICHEDIT 控件，则它将不会
    // 发送该通知，除非重写 CDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码

    if(!m_pSelectedRoleAttribute)
        return ;
    POSITION   pos =  m_EncouragementList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }
    
    char  QText[256];
    m_EncouragementCell.GetWindowText( QText , 256 );

    std::map<long,CString>::iterator   AwardFile = GetInst(CXmlEdit).m_AwardIdFileList.find( m_QuestId );
    if ( AwardFile == GetInst(CXmlEdit).m_AwardIdFileList.end() )
    {
        return;
    } 
    //= *Awardtable 
    CXmlEdit::MapAwardFileTable::iterator fileIter= GetInst(CXmlEdit).m_AllAwardList.find( AwardFile->second );
    if ( fileIter == GetInst(CXmlEdit).m_AllAwardList.end())
    {
        return ;
    }
    CXmlEdit::MapAwardTable::iterator   AwardIter = (*(fileIter->second)).find( m_QuestId );
    if ( AwardIter != (*(fileIter->second)).end() )
    {
        long  AwardNum = (long)AwardIter->second.size() ;
        if( AwardNum > 0 )
        {
            AwardIter->second[0]->m_nEncouragementCell = atoi( QText );
        }

    }

}

void CAttribute::OnEnChangeEdit3()
{
    if(!m_pSelectedRoleAttribute)
        return ;
    POSITION   pos =  m_EncouragementList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    int nSelectItem     = m_EncouragementList.GetNextSelectedItem(pos);
    CString selectNum   = m_EncouragementList.GetItemText(nSelectItem,1);

    char  QText[256];
    m_AwardName.GetWindowText( QText , 256 );
    if ( strcmp(selectNum.GetBuffer(),QText) == 0 )
    {
        return ;
    }
    std::map<long,CString>::iterator   AwardFile = GetInst(CXmlEdit).m_AwardIdFileList.find( m_QuestId );
    if ( AwardFile == GetInst(CXmlEdit).m_AwardIdFileList.end() )
    {
        return;
    } 
    //= *Awardtable 
    CXmlEdit::MapAwardFileTable::iterator fileIter= GetInst(CXmlEdit).m_AllAwardList.find( AwardFile->second );
    if ( fileIter == GetInst(CXmlEdit).m_AllAwardList.end())
    {
        return ;
    }
    CXmlEdit::MapAwardTable::iterator   AwardIter = (*(fileIter->second)).find( m_QuestId );
    if ( AwardIter != (*(fileIter->second)).end() )
    {
        long  AwardNum = (long)AwardIter->second.size() ;
        if( AwardNum < nSelectItem )
        {
            return ;           
        }
        AwardIter->second[nSelectItem]->m_nNum = atoi(QText);
        m_EncouragementList.SetItemText(nSelectItem,1,QText);
    }
}

void CAttribute::OnEnChangeEdit8()
{
    if(!m_pSelectedRoleAttribute)
        return ;
    POSITION   pos =  m_EncouragementList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    int nSelectItem     = m_EncouragementList.GetNextSelectedItem(pos);
    CString selectNum   = m_EncouragementList.GetItemText(nSelectItem,2);

    char  QText[256];
    m_AwardNum.GetWindowText( QText , 256 );
    if ( strcmp(selectNum.GetBuffer(),QText) == 0 )
    {
        return ;
    }
    std::map<long,CString>::iterator   AwardFile = GetInst(CXmlEdit).m_AwardIdFileList.find( m_QuestId );
    if ( AwardFile == GetInst(CXmlEdit).m_AwardIdFileList.end() )
    {
        return;
    } 
    //= *Awardtable 
    CXmlEdit::MapAwardFileTable::iterator fileIter= GetInst(CXmlEdit).m_AllAwardList.find( AwardFile->second );
    if ( fileIter == GetInst(CXmlEdit).m_AllAwardList.end())
    {
        return ;
    }
    CXmlEdit::MapAwardTable::iterator   AwardIter = (*(fileIter->second)).find( m_QuestId );
    if ( AwardIter != (*(fileIter->second)).end() )
    {
        long  AwardNum = (long)AwardIter->second.size() ;
        if( AwardNum < nSelectItem )
        {
            return ;           
        }
        AwardIter->second[nSelectItem]->m_cName = QText;
        m_EncouragementList.SetItemText(nSelectItem,2,QText);
    }
}

void CAttribute::OnEnChangeEdit36()
{
    if(!m_pSelectedRoleAttribute)
        return ;
    POSITION   pos =  m_EncouragementList.GetFirstSelectedItemPosition();
    if(pos==NULL)
    {	
        return;		
    }

    int nSelectItem     = m_EncouragementList.GetNextSelectedItem(pos);
    CString selectNum   = m_EncouragementList.GetItemText(nSelectItem,3);

    char  QText[256];
    m_AwardOdds.GetWindowText( QText , 256 );

    if ( strcmp(selectNum.GetBuffer(),QText) == 0 )
    {
        return ;
    }
    std::map<long,CString>::iterator   AwardFile = GetInst(CXmlEdit).m_AwardIdFileList.find( m_QuestId );
    if ( AwardFile == GetInst(CXmlEdit).m_AwardIdFileList.end() )
    {
        return;
    } 
    //= *Awardtable 
    CXmlEdit::MapAwardFileTable::iterator fileIter= GetInst(CXmlEdit).m_AllAwardList.find( AwardFile->second );
    if ( fileIter == GetInst(CXmlEdit).m_AllAwardList.end())
    {
        return ;
    }
    CXmlEdit::MapAwardTable::iterator   AwardIter = (*(fileIter->second)).find( m_QuestId );
    if ( AwardIter != (*(fileIter->second)).end() )
    {
        long  AwardNum = (long)AwardIter->second.size() ;
        if( AwardNum < nSelectItem )
        {
            return ;           
        }
        AwardIter->second[nSelectItem]->m_fOdds = atof(QText);
        m_EncouragementList.SetItemText(nSelectItem,3,QText);
    }
}

void CAttribute::OnBnClickedButton7()
{
     if(!m_pSelectedRoleAttribute)
        return ;
    if ( m_QuestId < 100000 )
    {
        return ;
    }

    char strVal[256];
    sprintf_s( strVal , "quest/Award%d.xml" , m_QuestId/1000 );
    std::map<long,CString>::iterator   AwardFile = GetInst(CXmlEdit).m_AwardIdFileList.find( m_QuestId );
    if ( AwardFile == GetInst(CXmlEdit).m_AwardIdFileList.end() )
    {
        GetInst(CXmlEdit).m_AwardIdFileList[ m_QuestId ] =  strVal;
        AwardFile = GetInst(CXmlEdit).m_AwardIdFileList.find( m_QuestId );
    } 
    //= *Awardtable 
    CXmlEdit::MapAwardFileTable::iterator fileIter= GetInst(CXmlEdit).m_AllAwardList.find( AwardFile->second );
    if ( fileIter == GetInst(CXmlEdit).m_AllAwardList.end())
    {
        CXmlEdit::MapAwardTable*  Award = new  CXmlEdit::MapAwardTable;
        GetInst(CXmlEdit).m_AllAwardList[ AwardFile->second ] = Award;

        fileIter= GetInst(CXmlEdit).m_AllAwardList.find( AwardFile->second );
    }
    CXmlEdit::MapAwardTable::iterator   AwardIter = (*(fileIter->second)).find( m_QuestId );
    if ( AwardIter != (*(fileIter->second)).end() )
    {
        tagRoleEncouragement * tagEncour = new  tagRoleEncouragement;

        tagEncour->m_nID =  m_QuestId;
        //tagEncour->m_nEncouragementCell = 0;
        tagEncour->m_cName = "";
        tagEncour->m_nNum  = 0;
        tagEncour->m_nType = 0;
        tagEncour->m_fOdds = 0.0;

        AwardIter->second.push_back( tagEncour );
    }
    else
    {
        tagRoleEncouragement * tagEncour = new  tagRoleEncouragement;
        tagEncour->m_nID =  m_QuestId;
        //tagEncour->m_nEncouragementCell = 0;
        tagEncour->m_cName = "";
        tagEncour->m_nNum  = 0;
        tagEncour->m_nType = 0;
        tagEncour->m_fOdds = 0.0;

        (*(fileIter->second) )[ m_QuestId ].push_back ( tagEncour );
        AwardIter = (*(fileIter->second)).find( m_QuestId );
    }

    m_EncouragementList.DeleteAllItems();
    if ( AwardIter != (*(fileIter->second)).end() )
    {
        long  AwardNum = (long)AwardIter->second.size() ;
        if( AwardNum > 0 )
        {
            GetDlgItem(IDC_EDIT37)->SetWindowText( itoa(AwardIter->second[0]->m_nEncouragementCell,strVal,10));
        }
        for ( long i = 0 ; i < AwardNum ; i ++ )
        {
            AwardIter->second[i]->m_nEncouragementCell = AwardIter->second[0]->m_nEncouragementCell;

            tagRoleEncouragement * tagEncour = AwardIter->second[i];
            m_EncouragementList.InsertItem(i,"");

            m_EncouragementList.SetItemText(i,0,itoa(tagEncour->m_nType,strVal,10));
            m_EncouragementList.SetItemText(i,1,itoa(tagEncour->m_nNum,strVal,10));
            m_EncouragementList.SetItemText(i,2,tagEncour->m_cName.c_str());

            sprintf_s( strVal,"%lf" , tagEncour->m_fOdds ); 
            m_EncouragementList.SetItemText(i,3,strVal);

        }
    }
}

void CAttribute::OnBnClickedButton8()
{
    if(!m_pSelectedRoleAttribute)
        return ;
    // TODO: 在此添加控件通知处理程序代码
    POSITION   pos   =   m_EncouragementList.GetFirstSelectedItemPosition();
    if ( pos == NULL  )
    {
        return ;
    }

    int nSelectItem     = m_EncouragementList.GetNextSelectedItem(pos);

    std::map<long,CString>::iterator   AwardFile = GetInst(CXmlEdit).m_AwardIdFileList.find( m_QuestId );
    if ( AwardFile == GetInst(CXmlEdit).m_AwardIdFileList.end() )
    {
        return;
    } 
    //= *Awardtable 
    CXmlEdit::MapAwardFileTable::iterator fileIter= GetInst(CXmlEdit).m_AllAwardList.find( AwardFile->second );
    if ( fileIter == GetInst(CXmlEdit).m_AllAwardList.end())
    {
        return ;
    }
    CXmlEdit::MapAwardTable::iterator   AwardIter = (*(fileIter->second)).find( m_QuestId );
    if ( AwardIter != (*(fileIter->second)).end() )
    {
        long  AwardNum = (long)AwardIter->second.size() ;
        if( AwardNum < nSelectItem )
        {
            return ;           
        }
        AwardIter->second.erase( AwardIter->second.begin() + nSelectItem );
    }                                                                      
    int ItemNum = m_EncouragementList.GetNextSelectedItem(pos);
    m_EncouragementList.DeleteItem(ItemNum);

    char strVal[256];
    m_EncouragementList.DeleteAllItems();
    if ( AwardIter != (*(fileIter->second)).end() )
    {
        long  AwardNum = (long)AwardIter->second.size() ;
        if( AwardNum > 0 )
        {
            GetDlgItem(IDC_EDIT37)->SetWindowText( itoa(AwardIter->second[0]->m_nEncouragementCell,strVal,10));
        }
        for ( long i = 0 ; i < AwardNum ; i ++ )
        {
            AwardIter->second[i]->m_nEncouragementCell = AwardIter->second[0]->m_nEncouragementCell;

            tagRoleEncouragement * tagEncour = AwardIter->second[i];
            m_EncouragementList.InsertItem(i,"");

            m_EncouragementList.SetItemText(i,0,itoa(tagEncour->m_nType,strVal,10));
            m_EncouragementList.SetItemText(i,1,itoa(tagEncour->m_nNum,strVal,10));
            m_EncouragementList.SetItemText(i,2,tagEncour->m_cName.c_str());

            sprintf_s( strVal,"%lf" , tagEncour->m_fOdds ); 
            m_EncouragementList.SetItemText(i,3,strVal);

        }
    }
}
