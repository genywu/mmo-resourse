// QuestEditorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "QuestEditorDlg.h"
#include "tinyxml.h"
#include "CXmlEdit.h"
#include "CAttribute.h"
#include "CStep.h"
#include "CEncouragement.h"
#include "CFileSave.h"
#include "Input.h"
#include "CreateNewRes.h"
#include "QueryFrame.h"
#include "StickQuest.h"


#define TabOfAttribute		0     //任务属性标签
#define TabOfStep			1	  //任务步骤标签
#define TabOfEncouragement	2	  //任务奖励标签
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog 
{
public:
	CAboutDlg();
// 对话框数据
	enum { IDD = IDD_ABOUTBOX };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void Onload();
	afx_msg void OnSave();
};
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CQuestEditorDlg 对话框

CQuestEditorDlg::CQuestEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuestEditorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	Menu=::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MENU2));
    CopyId =0;
}

void CQuestEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, Tab);
	DDX_Control(pDX, IDC_TREE1, RoleList);
	DDX_Control(pDX, IDC_LIST1, m_Loglist);

}
CQuestEditorDlg::~CQuestEditorDlg()
{
}

BEGIN_MESSAGE_MAP(CQuestEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CQuestEditorDlg::OnNMRclickTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CQuestEditorDlg::OnTvnSelchangedTree1)
    ON_NOTIFY(TCN_SELCHANGE , IDC_TAB,   &CQuestEditorDlg::OnTvnSelchangedTab)
	ON_COMMAND(ID_32792, &CQuestEditorDlg::On32792)
	ON_COMMAND(ID_32791, &CQuestEditorDlg::OnExit)
    ON_BN_CLICKED(IDCANCEL, &CQuestEditorDlg::OnBnClickedCancel)
    ON_COMMAND(ID_32795, &CQuestEditorDlg::OnAddNewQuest)
    ON_COMMAND(ID_32794, &CQuestEditorDlg::OnDelQuest)
    ON_COMMAND(ID_32793, &CQuestEditorDlg::OnCopyQuest)
    ON_COMMAND(ID_32796, &CQuestEditorDlg::OnStickQuest)
    ON_COMMAND(ID_32797, &CQuestEditorDlg::OnCreateTextRes)
    ON_COMMAND(ID_32798, &CQuestEditorDlg::OnQuestQuery)
    ON_MESSAGE(WM_HOTKEY,&CQuestEditorDlg::OnHotKey)

    ON_COMMAND(ID_32802, &CQuestEditorDlg::OnOnlySaveQuest)
    ON_COMMAND(ID_32803, &CQuestEditorDlg::OnSaveAllRes)
    ON_COMMAND(ID_32804, &CQuestEditorDlg::OnReLoad)

END_MESSAGE_MAP()


// CQuestEditorDlg 消息处理程序

BOOL CQuestEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// 初始化界
    ShowTab();
	ShowRoleList();

    RegisterHotKey( GetSafeHwnd(),1001,NULL,VK_F3);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CQuestEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CQuestEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CQuestEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CQuestEditorDlg::OnNMClickTab(int TalNum)
{
	if (TalNum==TabOfAttribute)
	{
		Attribute.ShowWindow(SW_SHOW);
		Step.ShowWindow(SW_HIDE);
		Encouragement.ShowWindow(SW_HIDE);
	}
	if (TalNum==TabOfStep)
	{
		Attribute.ShowWindow(SW_HIDE);
		Step.ShowWindow(SW_SHOW);
		Encouragement.ShowWindow(SW_HIDE);
	}
	if (TalNum==TabOfEncouragement)
	{
		Attribute.ShowWindow(SW_HIDE);
		Step.ShowWindow(SW_HIDE);
		Encouragement.ShowWindow(SW_SHOW);
	}
}

//初始化Tab控件函数
void CQuestEditorDlg::ShowTab()
{
	//为Tab添加3个标签
	Tab.InsertItem(0,_T("任务属性 + 任务奖励"));
	Tab.InsertItem(1,_T("任务步骤 + 任务文本"));
    Tab.InsertItem(2,_T("全局任务文本"));

	//创建3个对话框
	Attribute.Create(IDD_DIALOG1,&Tab);
	Step.Create(IDD_DIALOG2,&Tab);
	Encouragement.Create(IDD_DIALOG3,&Tab);
	//设定窗口的显示范围和显示的位置
	CRect rc;
	Tab.GetClientRect(rc);

	rc.top += 20;
	rc.bottom += -5;
	rc.left += 5;
	rc.right += -5;

	Attribute.MoveWindow(&rc);
	Step.MoveWindow(&rc);
	Encouragement.MoveWindow(&rc);
	//设置初始化窗口
	Attribute.ShowWindow(SW_SHOW);
	Step.ShowWindow(SW_HIDE);
	Encouragement.ShowWindow(SW_HIDE);
	m_CurSelTab = 0;
}
//显示树型列表函数并添加图标
void CQuestEditorDlg::ShowRoleList()
{
	char strVal[256];
	HTREEITEM	QuetRoote;
	HTREEITEM	TextRoote;
	HTREEITEM   RoleStep;
	QuetRoote = RoleList.InsertItem("任务列表",0,0);
	TextRoote = RoleList.InsertItem("任务文本资源",0,0);
	vector<tagRole*>           vcAllRole;
	vector<tagRole*>::iterator AllRileIter;
    CXmlEdit::MapFileTable     AllFiletable;
    CXmlEdit::MapTextFileTable AllTextRes;
    //vcAllRole   = GetInst(CXmlEdit).m_vcAllRole;
    AllFiletable= GetInst(CXmlEdit).m_AllQuestList;//;theApp.Filetable;
    AllTextRes  = GetInst(CXmlEdit).m_AllQuestTextFile;

    CXmlEdit::MapFileTable::iterator    fileIter = AllFiletable.begin();
    CXmlEdit::MapTextFileTable::iterator textIter = AllTextRes.begin();

    for ( ; fileIter!= AllFiletable.end(); fileIter ++  )
    {
        HTREEITEM RoleRoote = RoleList.InsertItem( fileIter->first,0,0,QuetRoote );
        vcAllRole   = (fileIter->second);
        AllRileIter = vcAllRole.begin();
        for (;AllRileIter!=vcAllRole.end();AllRileIter++)
        {
            tagRole* Role           = *AllRileIter;
            tagRoleAttribute* Attribute = Role->vcRole_Attribute;
            CString Rolename           = Attribute->m_cName.c_str();
            CString RoleId            = itoa(Attribute->m_nID,strVal,10);

            
            map<CString,CString>::iterator  sitr = GetInst(CXmlEdit).m_TextRes.find( "Q"+RoleId );        
            if( sitr != GetInst(CXmlEdit).m_TextRes.end() )
            {
                Rolename = sitr->second;
            }

            HTREEITEM RoleRootee = RoleList.InsertItem(RoleId+"("+Rolename+")",0,0,RoleRoote);
            RoleList.InsertItem("任务属性",0,0,RoleRootee);
            RoleStep = RoleList.InsertItem("任务步骤",0,0,RoleRootee);
        }
    }


    for ( ; textIter!= AllTextRes.end() ; textIter ++ )
    {
        HTREEITEM RoleRoote = RoleList.InsertItem( textIter->first,0,0,TextRoote );
    }
   
	RoleList.Expand(QuetRoote,TVE_EXPAND); 
	RoleList.Expand(TextRoote,TVE_EXPAND); 
}

//添加鼠标右键菜单
void CQuestEditorDlg::OnNMRclickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu menu;
	CPoint pt;
	menu.LoadMenu(IDR_MENU1);
	CMenu* pMenu=menu.GetSubMenu(0);
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x,pt.y, this);
	*pResult = 0;
}
//重载回车键响应
void CQuestEditorDlg::OnOK()
{
	//添加响应;
}

void CQuestEditorDlg::OnTvnSelchangedTab(NMHDR *pNMHDR, LRESULT *pResult)
{
     int CurSle = Tab.GetCurSel();

     switch( CurSle )
     {
     case 0:
         OnNMClickTab(TabOfAttribute);
         break;
     case 1:
         OnNMClickTab(TabOfStep);
         break;
     case 2:
         OnNMClickTab(TabOfEncouragement);
         break;
     }
}
//鼠标左键单击响应
void CQuestEditorDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM selectedItem =RoleList.GetSelectedItem();
	HTREEITEM parentItem = RoleList.GetParentItem(selectedItem);
	CString ItemText = RoleList.GetItemText(selectedItem);
	CString RoleID = RoleList.GetItemText(parentItem);
	CString *pRoleId = &RoleID;
	if (ItemText=="任务属性")
	{
		Attribute.ReadRoleAttribute(pRoleId);
		OnNMClickTab(TabOfAttribute);
        Tab.SetCurSel( 0 );
	}
	if (ItemText=="任务步骤")
	{
		Step.ReadRoleStep(pRoleId);
		OnNMClickTab(TabOfStep);
        Tab.SetCurSel( 1 );
        Step.Showdetail(1);
	}
 	if (RoleID=="任务文本资源")
 	{
 		Encouragement.ReadRoleEncouragement(&ItemText);
		OnNMClickTab(TabOfEncouragement);
        Tab.SetCurSel( 2 );
 	}
	*pResult = 0;
}
//日志显示
void CQuestEditorDlg::ShowLog(vector<string>strlog)
{
	int a  = m_Loglist.GetCount();
	if (!strlog.empty())
	{
		vector<string>::iterator iter;
		iter = strlog.begin();
		for (;iter!=strlog.end();iter++)
		{
			m_Loglist.InsertString(a,(*iter).c_str());
		}
		strlog.clear();
	}
}

//保存
void CQuestEditorDlg::On32792()
{
	CFileSave objSave;
	objSave.OnReWrite();
    objSave.OnSaveRes();
}
//退出
void CQuestEditorDlg::OnExit()
{
	// TODO: 在此添加命令处理程序代码
	SendMessage(WM_CLOSE);
}

void CQuestEditorDlg::OnBnClickedCancel()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    int nRet=MessageBox("离开前确定保存了或者不需要保存吗?","",MB_YESNO);
    if( nRet == IDYES )
    {
        OnCancel();
    }   
}

void CQuestEditorDlg::OnAddNewQuest()
{
    // TODO: 在此添加命令处理程序代码
    char strVal[256];
    CInput in;
    INT_PTR  Res =in.DoModal() ;
    if( Res == IDCANCEL )
        return ;
    bool bIsSame = false;
    std::map<long,CString>::iterator iter;

    iter = GetInst(CXmlEdit).m_AllQuestStrId.find( in.m_nInPutRoleID );

    //对ID的有效性进行判断
    if ( iter != GetInst(CXmlEdit).m_AllQuestStrId.end() )
    {
        bIsSame = true;
    }
    if (!bIsSame && in.m_nInPutRoleID > 100000)
    {
        sprintf_s( strVal , "quest/Quest%d.xml" , in.m_nInPutRoleID/1000 );
        CXmlEdit::MapFileTable::iterator itr = GetInst(CXmlEdit).m_AllQuestList.find( strVal ); 
        if ( itr != GetInst(CXmlEdit).m_AllQuestList.end() )
        {
            tagRole* quest = new  tagRole;
            quest->m_nID = in.m_nInPutRoleID;
            tagRoleAttribute* Attribute = new tagRoleAttribute;
            tagRoleStep*     Step = new tagRoleStep ;
            Attribute->m_nID = in.m_nInPutRoleID;
            Step->m_nID      = in.m_nInPutRoleID;
            quest->vcRole_Attribute = Attribute;
            quest->vcRole_Step      = Step;
            itr->second.push_back( quest );
            GetInst(CXmlEdit).m_AllQuestStrId[ in.m_nInPutRoleID ]  = strVal; 
        }
        else
        {
            tagRole* quest = new  tagRole;
            quest->m_nID = in.m_nInPutRoleID;
            tagRoleAttribute* Attribute = new tagRoleAttribute;
            tagRoleStep*     Step = new tagRoleStep ;
            Attribute->m_nID = in.m_nInPutRoleID;
            Step->m_nID      = in.m_nInPutRoleID;
            quest->vcRole_Attribute = Attribute;
            quest->vcRole_Step      = Step;
         
            GetInst(CXmlEdit).m_AllQuestList[ strVal ].push_back( quest );
            GetInst(CXmlEdit).m_AllQuestStrId[ in.m_nInPutRoleID ]  = strVal;

        }
        RoleList.DeleteAllItems();
        ShowRoleList();
    }
    else if (bIsSame)
    {
        MessageBox("新建任务ID冲突");
    }
    else if(in.m_nInPutRoleID < 100000 )
    {
        MessageBox("新建任务ID不符合规范");
    }
}


void CQuestEditorDlg::OnDelQuest()
{
    //获取相应节点的信息
    HTREEITEM selectedItem = RoleList.GetSelectedItem();
    CString ItemText = RoleList.GetItemText(selectedItem);
    int SlecteRoleId  = atoi(ItemText);

    char  strVal[256];
    if ( SlecteRoleId > 100000 )
    {
        sprintf_s( strVal ," 确定删除任务id:%d(%s)" , SlecteRoleId,ItemText.GetBuffer() );
        int nRet=MessageBox(strVal,"",MB_YESNO);
        if( nRet == IDYES )
        {
            //删除任务
            std::map<long,CString>::iterator FileIter = GetInst(CXmlEdit).m_AllQuestStrId.find ( SlecteRoleId ); 
            if ( FileIter != GetInst(CXmlEdit).m_AllQuestStrId.end() )
            {
                CXmlEdit::MapFileTable::iterator  RoleIter = GetInst(CXmlEdit).m_AllQuestList.find( FileIter->second ) ;
                if ( RoleIter != GetInst(CXmlEdit).m_AllQuestList.end() )
                {
                    vector<tagRole*>::iterator iter;
                    iter = RoleIter->second.begin();
                    for ( ; iter != RoleIter->second.end() ; iter ++ )
                    {
                        if ( (*iter)->m_nID == SlecteRoleId )
                        {
                            RoleIter->second.erase( iter );
                            break;
                        }
                    }
                }
            }
            //删除奖励
            std::map<long,CString>::iterator   AwardFile = GetInst(CXmlEdit).m_AwardIdFileList.find( SlecteRoleId );
            if ( AwardFile == GetInst(CXmlEdit).m_AwardIdFileList.end() )
            {
                RoleList.DeleteAllItems();
                ShowRoleList();
                return;
            } 
            //= *Awardtable 
            CXmlEdit::MapAwardFileTable::iterator fileIter= GetInst(CXmlEdit).m_AllAwardList.find( AwardFile->second );
            if ( fileIter == GetInst(CXmlEdit).m_AllAwardList.end())
            {
                RoleList.DeleteAllItems();
                ShowRoleList();
                return ;
            }
            CXmlEdit::MapAwardTable::iterator   AwardIter = (*(fileIter->second)).find( SlecteRoleId );
            if ( AwardIter != (*(fileIter->second)).end() )
            {
                 (*(fileIter->second)).erase( AwardIter );
            }
            RoleList.DeleteAllItems();
            ShowRoleList();
        }

    }
    else
    {
        MessageBox("请先选中具体需要删除的任务!");
    }

}

void CQuestEditorDlg::OnCopyQuest()
{
    //获取相应节点的信息
    HTREEITEM selectedItem = RoleList.GetSelectedItem();
    CString ItemText = RoleList.GetItemText(selectedItem);
    int SlecteRoleId  = atoi(ItemText);
    char  strVal[256];
    if ( SlecteRoleId > 100000 )
    {
        sprintf_s( strVal ," 确定复制任务id:%d(%s)" , SlecteRoleId,ItemText.GetBuffer() );
        int nRet=MessageBox(strVal,"",MB_YESNO);
        if( nRet == IDYES )
        {
            //复制
            CopyId = SlecteRoleId;
        }
    }
    else
    {
        MessageBox("请先选中具体需要复制的任务!");
    }
}

void CQuestEditorDlg::OnStickQuest()
{
    //获取相应节点的信息
    HTREEITEM selectedItem = RoleList.GetSelectedItem();
    CString ItemText = RoleList.GetItemText(selectedItem);

    char  strVal[256] = "quest/Quest";
    if ( CopyId > 0 && strstr( ItemText.GetBuffer(),strVal) )
    {
        //sprintf_s( strVal ," 确定粘贴任务id:%d？" , CopyId);
        int nRet=MessageBox(strVal,"",MB_YESNO);
        if( nRet == IDYES )
        {
            //删除
            CStickQuest  stick;
            if( stick.DoModal() == IDOK  )
            {
                long   NId = stick.m_QuestID;

                std::map<long,CString>::iterator FileIter = GetInst(CXmlEdit).m_AllQuestStrId.find ( NId ); 
                if ( FileIter != GetInst(CXmlEdit).m_AllQuestStrId.end() )
                {
                    MessageBox("有重复的任务ＩＤ");
                    return ;
                }

                FileIter = GetInst(CXmlEdit).m_AllQuestStrId.find ( CopyId ); 
                tagRole* role = NULL;
                if ( FileIter != GetInst(CXmlEdit).m_AllQuestStrId.end() )
                {
                    CXmlEdit::MapFileTable::iterator  RoleIter = GetInst(CXmlEdit).m_AllQuestList.find( FileIter->second ) ;
                    if ( RoleIter != GetInst(CXmlEdit).m_AllQuestList.end() )
                    {
                        vector<tagRole*>::iterator iter;
                        iter = RoleIter->second.begin();
                        for ( ; iter != RoleIter->second.end() ; iter ++ )
                        {
                            if ( (*iter)->m_nID == CopyId )
                            {
                                role = (*iter);
                                break;
                            }
                        }
                    }
                }
                if( !role )
                    return ;

                sprintf_s( strVal , "quest/Quest%d.xml" , NId/1000 );
                CXmlEdit::MapFileTable::iterator itr = GetInst(CXmlEdit).m_AllQuestList.find( strVal ); 
                if ( itr != GetInst(CXmlEdit).m_AllQuestList.end() )
                {
                       tagRole * newRole = new tagRole( *role);
                       newRole->m_nID    = NId;
                       newRole->vcRole_Attribute->m_nID = NId;
                       newRole->vcRole_Step->m_nRoleID      = NId;
                       newRole->vcRole_Step->m_nID      = 1;
                       itr->second.push_back( newRole );
                       GetInst(CXmlEdit).m_AllQuestStrId[ NId ]  = strVal; 
                }
                else
                {
                     tagRole * newRole = new tagRole( *role);
                     newRole->m_nID    = NId;
                     newRole->vcRole_Attribute->m_nID = NId;
                     newRole->vcRole_Step->m_nRoleID      = NId;
                     newRole->vcRole_Step->m_nID      = 1;
                     GetInst(CXmlEdit).m_AllQuestList[ strVal ].push_back( newRole );
                     GetInst(CXmlEdit).m_AllQuestStrId[ NId]  = strVal;

                }
                RoleList.DeleteAllItems();
                ShowRoleList();
                
            }
        }
    }
    else if ( CopyId <= 0 )
    {
        int nRet=MessageBox("粘贴前需要复制具体的任务!");
    }
    else
    {
        MessageBox("请先选中具体需要粘贴到具体文件名!");
    }
}

void  CQuestEditorDlg::OnCreateTextRes()
{
     CCreateNewRes  res;
     if( res.DoModal() == IDOK )
     {
         HTREEITEM selectedItem = RoleList.GetSelectedItem();
         CString ItemText = RoleList.GetItemText(selectedItem);


         CString  filename = res.m_fileresname;
         if ( filename.GetLength() > 6 )
         {
             filename = "textres/" + filename;
             CXmlEdit::MapTextFileTable::iterator  MapIter = GetInst(CXmlEdit).m_AllQuestTextFile.find( filename);
             if ( MapIter != GetInst(CXmlEdit).m_AllQuestTextFile.end() )
             {
                 MessageBox("以及存在次文件名，请换一个!");
                 return ;
             }
             map<CString,CString> mp;
             GetInst(CXmlEdit).m_AllQuestTextFile[filename] = mp ;
             RoleList.DeleteAllItems();
             ShowRoleList();
         }
         else
         {
             MessageBox("你输入的文件名可能有误，需要xml文件后缀名!");
         }
     }
}

void  CQuestEditorDlg::OnQuestQuery()
{
    CQueryFrame  query;
    if( query.DoModal() == IDOK )
    {
        long Sel = query.m_Sel;
        if ( Sel < 0 )
        {
            return ;
        }
        CString filename = query.m_questname;

        if ( filename.GetLength() > 0 )
        {
            m_VcCurrItem.clear();
            m_Findtext = filename;
            HTREEITEM hRoot = RoleList.GetRootItem();
            if ( RoleList.ItemHasChildren(hRoot) )
            {
                HTREEITEM  hNextItem;
                HTREEITEM  hChildItem = RoleList.GetChildItem( hRoot ) ;

                CString  str =  RoleList.GetItemText(hChildItem).GetBuffer();
                if ( strstr( str.GetBuffer(),filename.GetBuffer() ))
                {
                       RoleList.SelectItem(hChildItem);
                       RoleList.Expand(hChildItem,TVE_EXPAND);
                       m_VcCurrItem[hChildItem] = 1;// .push_back( hChildItem );
                       return ;
                }

                while( hChildItem != NULL )
                {
                    if ( RoleList.ItemHasChildren(hChildItem) )
                    {
                        HTREEITEM  hGrandChildItem = RoleList.GetChildItem( hChildItem ) ;
                        HTREEITEM  hGrandNextItem;

                        str =  RoleList.GetItemText(hGrandChildItem).GetBuffer();
                        if ( strstr( str.GetBuffer(),filename.GetBuffer() ))
                        {
                            RoleList.SelectItem(hGrandChildItem);
                            RoleList.Expand(hGrandChildItem,TVE_EXPAND);
                            m_VcCurrItem[hGrandChildItem] = 1;
                            return ;
                        }
                        while ( hGrandChildItem!= NULL )
                        {
                            hGrandNextItem = RoleList.GetNextItem( hGrandChildItem , TVGN_NEXT );

                            str =  RoleList.GetItemText(hGrandNextItem).GetBuffer();
                            if ( strstr( str.GetBuffer(),filename.GetBuffer() ))
                            {
                                RoleList.SelectItem(hGrandNextItem);
                                RoleList.Expand(hGrandNextItem,TVE_EXPAND);
                                m_VcCurrItem[hGrandNextItem] = 1;
                                return ;
                            }
                            hGrandChildItem = hGrandNextItem;               
                        }

                        hNextItem = RoleList.GetNextItem( hChildItem , TVGN_NEXT );
                        hChildItem = hNextItem;
                    }
                }
            }
        }
    }
}

void CQuestEditorDlg::GoOnFindTreeItem()
{
   if ( m_VcCurrItem.size() < 1 )
        return ;
   
   HTREEITEM hRoot = RoleList.GetRootItem();
   if ( RoleList.ItemHasChildren(hRoot) )
   {
       HTREEITEM  hNextItem;
       HTREEITEM  hChildItem = RoleList.GetChildItem( hRoot ) ;

       CString  str =  RoleList.GetItemText(hChildItem).GetBuffer();
       if ( m_VcCurrItem[hChildItem] < 1 && strstr( str.GetBuffer(),m_Findtext.GetBuffer() ))
       {
           RoleList.SelectItem(hChildItem);
           RoleList.Expand(hChildItem,TVE_EXPAND);
           m_VcCurrItem[hChildItem] = 1;
           return ;
       }

       while( hChildItem != NULL )
       {
           if ( RoleList.ItemHasChildren(hChildItem) )
           {
               HTREEITEM  hGrandChildItem = RoleList.GetChildItem( hChildItem ) ;
               HTREEITEM  hGrandNextItem;

               str =  RoleList.GetItemText(hGrandChildItem).GetBuffer();
               if ( m_VcCurrItem[hGrandChildItem] < 1 && strstr( str.GetBuffer(),m_Findtext.GetBuffer() ))
               {
                   RoleList.SelectItem(hGrandChildItem);
                   RoleList.Expand(hGrandChildItem,TVE_EXPAND);
                   m_VcCurrItem[hGrandChildItem] = 1;
                   //m_VcCurrItem.push_back( hGrandChildItem );
                   return ;
               }
               while ( hGrandChildItem!= NULL )
               {
                   hGrandNextItem = RoleList.GetNextItem( hGrandChildItem , TVGN_NEXT );

                   str =  RoleList.GetItemText(hGrandNextItem).GetBuffer();
                   if ( m_VcCurrItem[hGrandNextItem] < 1 &&  strstr( str.GetBuffer(),m_Findtext.GetBuffer() ))
                   {
                       RoleList.SelectItem(hGrandNextItem);
                       RoleList.Expand(hGrandNextItem,TVE_EXPAND);
                       m_VcCurrItem[hGrandNextItem] = 1;
                       //m_VcCurrItem.push_back( hGrandNextItem );
                       return ;
                   }
                   hGrandChildItem = hGrandNextItem;               
               }

               hNextItem = RoleList.GetNextItem( hChildItem , TVGN_NEXT );
               hChildItem = hNextItem;
           }
       }
   }
   m_VcCurrItem.clear();
   return ;
}

LRESULT  CQuestEditorDlg::OnHotKey(WPARAM wPararm,LPARAM lPararm)
{
    if ( wPararm == 1001 )
    {
          if ( m_Findtext.GetLength() < 1)
            return 0;
          
          if ( m_VcCurrItem.size() < 1 )
            return 0;
          
          GoOnFindTreeItem();
    }
    return 0;
}

void CQuestEditorDlg::OnOnlySaveQuest()
{
    CFileSave objSave;
    objSave.OnReWrite();
}

void CQuestEditorDlg::OnSaveAllRes()
{
    CFileSave objSave;
    objSave.OnSaveRes();
}

void CQuestEditorDlg::OnReLoad()
{
    RoleList.DeleteAllItems();
    ShowRoleList();
}