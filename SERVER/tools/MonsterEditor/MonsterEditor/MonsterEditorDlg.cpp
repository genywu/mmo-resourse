// MonsterEditorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MonsterEditor.h"
#include "MonsterEditorDlg.h"
#include "Detail.h"
#include "Recover.h"
#include "Part.h"
#include "Merger.h"
#define NEWRECORD 1			//新建了一条记录
#define UPDATERECORD 2		//修改了一条记录
#define DELETERECORD 3		//删除了一条记录
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
vector<tagMonster>vecMonster;//类型为tagMonseter的vector容器
vector<tagSkill>vecSkill;	 //类型为tagSkill的vector容器
string g_strfilename;			 //怪物属性文件名
string g_strfilenameskill;		 //怪物技能文件名
int ncount=0;		//编辑文件的记录数
bool bsaveflag=false;	//检查是否编辑了记录
char g_buf[1024];//保存程序开始运行时的路径
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


// CMonsterEditorDlg 对话框




CMonsterEditorDlg::CMonsterEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMonsterEditorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMonsterEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_MonTree);
	DDX_Control(pDX, IDC_LIST1, m_MonList);

	DDX_Control(pDX, IDC_LIST3, m_LogListBox);
}

BEGIN_MESSAGE_MAP(CMonsterEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CMonsterEditorDlg::OnNMDblclkList1)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CMonsterEditorDlg::OnNMDblclkTree1)
	ON_COMMAND(ID_NEW, &CMonsterEditorDlg::OnNew)
	ON_BN_CLICKED(IDC_BUTTON1, &CMonsterEditorDlg::OnBnClickedButton1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CMonsterEditorDlg::OnNMRclickList1)
	ON_COMMAND(ID_DELETE, &CMonsterEditorDlg::OnDelete)
	ON_COMMAND(ID_SAVE, &CMonsterEditorDlg::OnSaveToFile)
	ON_COMMAND(ID_RECOVER, &CMonsterEditorDlg::OnRecover)
	ON_COMMAND(ID_PART, &CMonsterEditorDlg::OnPartFile)
	ON_COMMAND(ID_MERGER, &CMonsterEditorDlg::OnMergerFile)
END_MESSAGE_MAP()


// CMonsterEditorDlg 消息处理程序

BOOL CMonsterEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

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

	// TODO: 在此添加额外的初始化代码
	m_hAccelTable=::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MENU1));
	//设置listcontrol样式
	m_MonList.SetExtendedStyle(
		LVS_EX_FLATSB
		|LVS_EX_FULLROWSELECT
		|LVS_EX_HEADERDRAGDROP
		|LVS_EX_ONECLICKACTIVATE
		|LVS_EX_GRIDLINES
		);
	//插入列
	m_MonList.InsertColumn(0,"自动序号",LVCFMT_LEFT,80,0);
	m_MonList.InsertColumn(1,"索引",LVCFMT_LEFT,80,1);
	m_MonList.InsertColumn(2,"原始名",LVCFMT_LEFT,100,2);
	m_MonList.InsertColumn(3,"名字",LVCFMT_LEFT,150,3);
	m_MonList.InsertColumn(4,"是否有阻挡",LVCFMT_LEFT,100,4);
	m_MonList.InsertColumn(5,"是否无敌",LVCFMT_LEFT,100,5);
	m_MonList.InsertColumn(6,"对应采集物名",LVCFMT_LEFT,100,6);
	m_MonList.InsertColumn(7,"称谓",LVCFMT_LEFT,100,7);
	m_MonList.InsertColumn(8,"称谓颜色",LVCFMT_LEFT,100,8);
	m_MonList.InsertColumn(9,"称谓图片",LVCFMT_LEFT,100,9);
	m_MonList.InsertColumn(10,"国家id",LVCFMT_LEFT,100,10);
	m_MonList.InsertColumn(11,"NPC类别",LVCFMT_LEFT,100,11);
	m_MonList.InsertColumn(12,"图形id",LVCFMT_LEFT,100,12);
	m_MonList.InsertColumn(13,"图形等级",LVCFMT_LEFT,100,13);
	m_MonList.InsertColumn(14,"名字颜色编号",LVCFMT_LEFT,100,14);
	m_MonList.InsertColumn(15,"等级颜色编号",LVCFMT_LEFT,100,15);
	m_MonList.InsertColumn(16,"血条颜色",LVCFMT_LEFT,100,16);
	m_MonList.InsertColumn(17,"声音id",LVCFMT_LEFT,100,17);
	m_MonList.InsertColumn(18,"是否可驯服",LVCFMT_LEFT,100,18);
	m_MonList.InsertColumn(19,"最大尝试驯服数",LVCFMT_LEFT,100,19);
	m_MonList.InsertColumn(20,"体形",LVCFMT_LEFT,100,20);
	m_MonList.InsertColumn(21,"等级",LVCFMT_LEFT,100,21);
	m_MonList.InsertColumn(22,"经验值",LVCFMT_LEFT,100,22);
	m_MonList.InsertColumn(23,"职业点数",LVCFMT_LEFT,100,23);
	m_MonList.InsertColumn(24,"最大HP",LVCFMT_LEFT,100,24);
	m_MonList.InsertColumn(25,"最小攻击力",LVCFMT_LEFT,100,25);
	m_MonList.InsertColumn(26,"最大攻击力",LVCFMT_LEFT,100,26);
	m_MonList.InsertColumn(27,"最小法力攻击",LVCFMT_LEFT,100,27);
	m_MonList.InsertColumn(28,"最大法力攻击",LVCFMT_LEFT,100,28);
	m_MonList.InsertColumn(29,"命中能力",LVCFMT_LEFT,100,29);
	m_MonList.InsertColumn(30,"防御力",LVCFMT_LEFT,100,30);
	m_MonList.InsertColumn(31,"魔法防御力",LVCFMT_LEFT,100,31);
	m_MonList.InsertColumn(32,"格挡",LVCFMT_LEFT,100,32);
	m_MonList.InsertColumn(33,"招架",LVCFMT_LEFT,100,33);
	m_MonList.InsertColumn(34,"闪避能力",LVCFMT_LEFT,100,34);
	m_MonList.InsertColumn(35,"移动速度",LVCFMT_LEFT,100,35);
	m_MonList.InsertColumn(36,"战斗移动速度",LVCFMT_LEFT,100,36);
	m_MonList.InsertColumn(37,"意志抗性",LVCFMT_LEFT,100,37);
	m_MonList.InsertColumn(38,"基本地元素抗性",LVCFMT_LEFT,100,38);
	m_MonList.InsertColumn(39,"基本水元素抗性",LVCFMT_LEFT,100,39);
	m_MonList.InsertColumn(40,"基本火元素抗性",LVCFMT_LEFT,100,40);
	m_MonList.InsertColumn(41,"基本风元素抗性",LVCFMT_LEFT,100,41);
	m_MonList.InsertColumn(42,"基本神圣元素抗性",LVCFMT_LEFT,100,42);
	m_MonList.InsertColumn(43,"基本黑暗元素抗性",LVCFMT_LEFT,100,43);
	m_MonList.InsertColumn(44,"基本无素抗性",LVCFMT_LEFT,100,44);
	m_MonList.InsertColumn(45,"生命恢复速度",LVCFMT_LEFT,100,45);
	m_MonList.InsertColumn(46,"魔法恢复速度",LVCFMT_LEFT,100,46);
	m_MonList.InsertColumn(47,"体质抗性",LVCFMT_LEFT,100,47);
	m_MonList.InsertColumn(48,"基本流血抗性",LVCFMT_LEFT,100,48);
	m_MonList.InsertColumn(49,"基本击晕抗性",LVCFMT_LEFT,100,49);
	m_MonList.InsertColumn(50,"基本麻痹抗性",LVCFMT_LEFT,100,50);
	m_MonList.InsertColumn(51,"基本催眠抗性",LVCFMT_LEFT,100,51);
	m_MonList.InsertColumn(52,"基本中毒抗性",LVCFMT_LEFT,100,52);
	m_MonList.InsertColumn(53,"基本定身抗性",LVCFMT_LEFT,100,53);
	m_MonList.InsertColumn(54,"最小停留次数",LVCFMT_LEFT,100,54);
	m_MonList.InsertColumn(55,"最大停留次数",LVCFMT_LEFT,100,55);
	m_MonList.InsertColumn(56,"最小行走次数",LVCFMT_LEFT,100,56);
	m_MonList.InsertColumn(57,"最大行走次数",LVCFMT_LEFT,100,57);
	m_MonList.InsertColumn(58,"警戒范围",LVCFMT_LEFT,100,58);
	m_MonList.InsertColumn(59,"追击范围",LVCFMT_LEFT,100,59);
	m_MonList.InsertColumn(60,"和平移动范围",LVCFMT_LEFT,100,60);
	m_MonList.InsertColumn(61,"AI模式",LVCFMT_LEFT,100,61);
	m_MonList.InsertColumn(62,"可否对话",LVCFMT_LEFT,100,62);
	m_MonList.InsertColumn(63,"怪物种族",LVCFMT_LEFT,100,63);
	m_MonList.InsertColumn(64,"元素类型",LVCFMT_LEFT,100,64);
	m_MonList.InsertColumn(65,"停留的时间",LVCFMT_LEFT,100,65);
	m_MonList.InsertColumn(66,"AI挂起方式",LVCFMT_LEFT,100,66);
	m_MonList.InsertColumn(67,"对人形类怪物基本伤害",LVCFMT_LEFT,120,67);
	m_MonList.InsertColumn(68,"对元素怪物基本伤害",LVCFMT_LEFT,120,68);
	m_MonList.InsertColumn(69,"对异族怪物基本伤害",LVCFMT_LEFT,120,69);
	m_MonList.InsertColumn(70,"对动物怪物基本伤害",LVCFMT_LEFT,120,70);
	m_MonList.InsertColumn(71,"对亡灵怪物基本伤害",LVCFMT_LEFT,120,71);
	m_MonList.InsertColumn(72,"对恶魔类怪物基本伤害",LVCFMT_LEFT,120,72);
	m_MonList.InsertColumn(73,"对昆虫类怪物基本伤害",LVCFMT_LEFT,120,73);
	m_MonList.InsertColumn(74,"对火系怪物基本伤害",LVCFMT_LEFT,120,74);
	m_MonList.InsertColumn(75,"对水系怪物基本伤害",LVCFMT_LEFT,120,75);
	m_MonList.InsertColumn(76,"对土系怪物基本伤害",LVCFMT_LEFT,120,76);
	m_MonList.InsertColumn(77,"对风系怪物基本伤害",LVCFMT_LEFT,120,77);
	m_MonList.InsertColumn(78,"对神圣系怪物基本伤害",LVCFMT_LEFT,120,78);
	m_MonList.InsertColumn(79,"对黑暗系列怪物基本伤害",LVCFMT_LEFT,120,79);
	m_MonList.InsertColumn(80,"对无元素系列怪物基本伤害",LVCFMT_LEFT,130,80);
	m_MonList.InsertColumn(81,"活力",LVCFMT_LEFT,100,81);
	m_MonList.InsertColumn(82,"物理暴击率",LVCFMT_LEFT,100,82);
	m_MonList.InsertColumn(83,"物理暴击伤害",LVCFMT_LEFT,100,83);
	m_MonList.InsertColumn(84,"物理暴击抗性",LVCFMT_LEFT,100,84);
	m_MonList.InsertColumn(85,"魔法暴击率",LVCFMT_LEFT,100,85);
	m_MonList.InsertColumn(86,"魔法暴击伤害",LVCFMT_LEFT,100,86);
	m_MonList.InsertColumn(87,"魔法暴击抗性",LVCFMT_LEFT,100,87);
	m_MonList.InsertColumn(88,"重伤比率",LVCFMT_LEFT,100,88);
	m_MonList.InsertColumn(89,"警戒时间",LVCFMT_LEFT,100,89);
	m_MonList.InsertColumn(90,"逃跑低线",LVCFMT_LEFT,100,90);
	m_MonList.InsertColumn(91,"技能概率",LVCFMT_LEFT,100,91);
	m_MonList.InsertColumn(92,"技能间隔",LVCFMT_LEFT,100,92);
	m_MonList.InsertColumn(93,"是否飞回",LVCFMT_LEFT,100,93);
	m_MonList.InsertColumn(94,"是否完全恢复",LVCFMT_LEFT,100,94);
	m_MonList.InsertColumn(95,"归位距离",LVCFMT_LEFT,100,95);
	m_MonList.InsertColumn(96,"追击时间",LVCFMT_LEFT,100,96);
	m_MonList.InsertColumn(97,"ai参考1",LVCFMT_LEFT,100,97);
	m_MonList.InsertColumn(98,"ai参考2",LVCFMT_LEFT,100,98);
	m_MonList.InsertColumn(99,"ai参考3",LVCFMT_LEFT,100,99);
	m_MonList.InsertColumn(100,"ai参考4",LVCFMT_LEFT,100,100);
	m_MonList.InsertColumn(101,"ai参考5",LVCFMT_LEFT,100,101);
	m_MonList.InsertColumn(102,"出生脚本",LVCFMT_LEFT,100,102);
	m_MonList.InsertColumn(103,"战斗脚本",LVCFMT_LEFT,100,103);
	m_MonList.InsertColumn(104,"重伤脚本",LVCFMT_LEFT,100,104);
	m_MonList.InsertColumn(105,"死亡脚本",LVCFMT_LEFT,100,105);
	m_MonList.InsertColumn(106,"周期脚本",LVCFMT_LEFT,100,106);
	m_MonList.InsertColumn(107,"选择技能脚本",LVCFMT_LEFT,100,107);
	m_MonList.InsertColumn(108,"寻找目标脚本",LVCFMT_LEFT,100,108);
	m_MonList.InsertColumn(109,"杀死目标脚本",LVCFMT_LEFT,100,109);
	
	ReadFile();
	MonList();
	MonTree();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMonsterEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMonsterEditorDlg::OnPaint()
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
HCURSOR CMonsterEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//将光标移动到“*”之后
int ReadTo(ifstream &stream,string name)
{
		string str;
	stream >> str;
	while (str != name)
	{
		//如果已到文件末尾，返回0
		if (stream.eof())
		{

			return 0;
		}
		//读取下一个串
		stream >> str;
	}
	return 1;
}

//将浮点型转换为字符型
CString ftoa(float f)
{
 
	stringstream ss; 
	ss << f; 
	return  ss.str( ).c_str( ); 

}
//DOWRD转换为CString

CString DwordToString(DWORD n)
{

	CString str;

	str.Format("%d",n);
	return str;
}

/*功能：获取系统时间
*
*/
string GetTime()
{
	//定义一个时间的结构体
	SYSTEMTIME st;
	CString strDate,strTime;
	//获取系统时间
	GetLocalTime(&st); 
	strDate.Format("%d-%d-%d",st.wYear,st.wMonth,st.wDay);
	strTime.Format("%d-%d-%d",st.wHour,st.wMinute,st.wSecond);
	string str;
	str=strDate+"_"+strTime;
	return str;
}
/*功能：读取文件
*
*/
void CMonsterEditorDlg::ReadFile()
{
	ifstream file;
	string str1,strflag;
	//怪物属性文件的路径
	g_strfilename="monsterlist.ini";
	//怪物技能文件的路径
	g_strfilenameskill="monsterskilllist.ini";
	file.open(g_strfilename.c_str());
	if(!file.is_open())
	{
		MessageBox("打开文件失败");
		return;
	}

	::GetCurrentDirectory(1000,g_buf);
	g_buf[strlen(g_buf)]='\0';
	tagMonster monster;
	tagSkill skill;
	int nNum=1;
	int i=0;
	//从文件中读取数据到vector中
	while(ReadTo(file, "*")!=0)
	{
		monster.nNO=nNum;
		file>> monster.dwIndex
			>>monster.strOrigName
			>>monster.strName
			>>monster.strBlock
			>>monster.strSuper
			>>monster.wDropFlag
			>>monster.strCollection
			>>monster.strTitle
			>>monster.dwTitleColor
			>>monster.dwTitlePicture
			>>monster.strCountryID
			>>monster.strNpcSort
			>>monster.wBossLevel
			>>monster.dwPicID
			>>monster.dwPicLevel
			>>monster.dwNameColorID
			>>monster.dwLevelColorID
			>>monster.dwHpColor
			>>monster.dwSoundID
			>>monster.strTame
			>>monster.dwTameTimes
			>>monster.strShape
			>>monster.nLevel
			>>monster.dwExp
			>>monster.dwWorkPoint
			>>monster.dwMaxHp
			>>monster.wMinAttack
			>>monster.wMaxAttack
			>>monster.wMinMagicAttack
			>>monster.wMaxMagicAttack
			>>monster.wHitRating
			>>monster.wDefense
			>>monster.wMagicDefense
			>>monster.wBlock
			>>monster.wImmunity
			>>monster.wPierce
			>>monster.wParry
			>>monster.wDodge
			>>monster.fMoveSpd
			>>monster.fBatMoveSpd
			>>monster.fTurningSpeed
			>>monster.wWillAnti
			>>monster.wGroundAnti
			>>monster.wWaterAnti
			>>monster.wFireAnti
			>>monster.wWindAnti
			>>monster.wSacreAnti
			>>monster.wDarkAnti
			>>monster.wNotElemAnti
			>>monster.wHpRecoverSpd
			>>monster.wMpRecoverSpd
			>>monster.wBodyAnti
			>>monster.wBloodAnti
			>>monster.wDuskAnti
			>>monster.wNumbAnti
			>>monster.wSleepAnti
			>>monster.wDrugAnti
			>>monster.wNotMoveAnti
			>>monster.wSilenceAnti
			>>monster.wMinStayTimes
			>>monster.wMaxStayTimes
			>>monster.wMinWalkTimes
			>>monster.wMaxWalkTimes
			>>monster.wAlertRange
			>>monster.wPurRange
			>>monster.wPeaMoveRange
			>>monster.strAIModel
			>>monster.bTalk
			>>monster.strMonRace
			>>monster.strElem
			>>monster.wStayTime
			>>monster.strAIWay
			>>monster.wHurtHuman
			>>monster.wHurtElem
			>>monster.wHurtDiff
			>>monster.wHurtAnimal
			>>monster.wHurtDead
			>>monster.wHurtDevil
			>>monster.wHurtInsect
			>>monster.wHurtFire
			>>monster.wHurtWater
			>>monster.wHurtGround
			>>monster.wHurtWind
			>>monster.wHurtSacre
			>>monster.wHurtDark
			>>monster.wHurtNotElem
			>>monster.wVitality
			>>monster.wStormHit
			>>monster.wStormHurt
			>>monster.wStormAnti
			>>monster.wMagicStorm
			>>monster.wMagicStormHurt  
			>>monster.wMagicStormAnti
			>>monster.fInjurRate
			>>monster.wAlertTime
			>>monster.fEscape
			>>monster.wSkillRate
			>>monster.wSkillInter
			>>monster.strReturn
			>>monster.strAllRecover
			>>monster.wReDistance
			>>monster.wReTime
			>>monster.strRefe1
			>>monster.strRefe2
			>>monster.strRefe3
			>>monster.strRefe4
			>>monster.strRefe5
			>>monster.strBirthSct
			>>monster.strBattleSct
			>>monster.strInjurSct
			>>monster.strDeadSct
			>>monster.strCycleSct
			>>monster.strChoseSkillSct
			>>monster.strFindTgtSct
			>>monster.strKillTgtSct;
			
			if (monster.strName == "天空蚁后")
			{
				int i = 0;
			}
			TRACE(monster.strName.c_str());
			TRACE("\n");
			//到monsterskilllist.ini寻找是否有相应的技能记录，若有则读取
			//ifstream fileskill(g_strfilenameskill.c_str());
		//如果文件没有打开，则返回
		//if(!fileskill.is_open())
		//{
		//	MessageBox("打开技能文件失败");
		//	return;
		//
		//}//如果不为0则，有相应的技能记录
		//if(ReadTo(fileskill,monster.strOrigName)!=0)
		//{
		//	fileskill>>strflag;
		//	//有三种技能，“>”、“#”、“@”代表不同种类的技能
		//	while(strflag==">"||strflag=="#"||strflag=="@")		
		//	{
		//		//">"在文件中是表示基本技能
		//		if(strflag==">")
		//		{
		//			strflag="";
		//			skill.strSkillType=">";
		//			fileskill>>skill.dwSkillID
		//				>>skill.wSkillLevel
		//				>>strflag;
		//			//基本技能没有技能间隔，设置为0
		//			skill.wInterTime=0;
		//			monster.vecSkill.push_back(skill);

		//		}
		//		//"#"在文件中表示普通技能
		//		else if(strflag=="#")
		//		{
		//			strflag="";
		//			skill.strSkillType="#";
		//			fileskill>>skill.dwSkillID
		//				>>skill.wSkillLevel
		//				>>strflag;
		//			//普通技能没有技能间隔，设置为0
		//			skill.wInterTime=0;
		//			monster.vecSkill.push_back(skill);
		//		}
		//		//"@"在文件中表示周期技能
		//		else if(strflag=="@")
		//		{
		//			strflag="";
		//			skill.strSkillType="@";
		//			fileskill>>skill.dwSkillID
		//				>>skill.wSkillLevel
		//				>>skill.wInterTime
		//				>>strflag;
		//			//将技能skill插入到vecSkill中
		//			monster.vecSkill.push_back(skill);
		//		}
		//		//"*"表示下一个记录的开始，所以返回
		//		else if(strflag=="*")
		//		{
		//			break;
		//		}
		//	}
		//	
		//	
		//}
		//fileskill.close();
		//插入vector中
		nNum+=1;		
		vecMonster.push_back(monster);
		//清空技能容器，准备装下一个monster的技能
		monster.vecSkill.clear();
	}
	file.close();
}
/*功能：设置listcontrol中的数据
*
*/
void CMonsterEditorDlg::ListSetData(vector<tagMonster>::iterator iter,int i)
{
	char strVal[256];				
		//itoa(iter->nNO,strVal,10);
		itoa(iter->nNO,strVal,256);

		m_MonList.InsertItem(i,"");

		m_MonList.SetItemText(i,0,CString(itoa(iter->nNO,strVal,10)));

		m_MonList.SetItemText(i,1,DwordToString(iter->dwIndex));
		m_MonList.SetItemText(i,2,(CString)(iter->strOrigName).c_str());
		m_MonList.SetItemText(i,3,(CString)(iter->strName).c_str());

		m_MonList.SetItemText(i,4,(CString)(iter->strBlock).c_str());
		m_MonList.SetItemText(i,5,(CString)(iter->strSuper).c_str());
		
		m_MonList.SetItemText(i,6,(CString)(iter->strCollection).c_str());
		m_MonList.SetItemText(i,7,(CString)(iter->strTitle).c_str());
		m_MonList.SetItemText(i,8,DwordToString(iter->dwTitleColor));
		m_MonList.SetItemText(i,9,DwordToString(iter->dwTitlePicture));
		m_MonList.SetItemText(i,10,(CString)(iter->strCountryID).c_str());
		m_MonList.SetItemText(i,11,(CString)(iter->strNpcSort).c_str());
		m_MonList.SetItemText(i,12,DwordToString(iter->dwPicID));
		m_MonList.SetItemText(i,13,DwordToString(iter->dwPicLevel));
		m_MonList.SetItemText(i,14,DwordToString(iter->dwNameColorID));
		m_MonList.SetItemText(i,15,DwordToString(iter->dwLevelColorID));
		m_MonList.SetItemText(i,16,DwordToString(iter->dwHpColor));
		m_MonList.SetItemText(i,17,DwordToString(iter->dwSoundID));
		
		
		m_MonList.SetItemText(i,18,(CString)(iter->strTame).c_str());
		m_MonList.SetItemText(i,19,DwordToString(iter->dwTameTimes));
		m_MonList.SetItemText(i,20,(CString)(iter->strShape).c_str());

		m_MonList.SetItemText(i,21,(CString)(itoa(iter->nLevel,strVal,10)));
		m_MonList.SetItemText(i,22,DwordToString(iter->dwExp));
		m_MonList.SetItemText(i,23,DwordToString(iter->dwWorkPoint));
		m_MonList.SetItemText(i,24,DwordToString(iter->dwMaxHp));
		m_MonList.SetItemText(i,25,DwordToString(iter->wMinAttack));
		m_MonList.SetItemText(i,26,DwordToString(iter->wMaxAttack));
		m_MonList.SetItemText(i,27,DwordToString(iter->wMinMagicAttack));
		m_MonList.SetItemText(i,28,DwordToString(iter->wMaxMagicAttack));
		m_MonList.SetItemText(i,29,DwordToString(iter->wHitRating));
		m_MonList.SetItemText(i,30,DwordToString(iter->wDefense));
		m_MonList.SetItemText(i,31,DwordToString(iter->wMagicDefense));
		m_MonList.SetItemText(i,32,DwordToString(iter->wBlock));
		m_MonList.SetItemText(i,33,DwordToString(iter->wParry));

		m_MonList.SetItemText(i,34,DwordToString(iter->wDodge));
		m_MonList.SetItemText(i,35,ftoa(iter->fMoveSpd));
		m_MonList.SetItemText(i,36,ftoa(iter->fBatMoveSpd));
		m_MonList.SetItemText(i,37,DwordToString(iter->wWillAnti));
		m_MonList.SetItemText(i,38,DwordToString(iter->wGroundAnti));
		m_MonList.SetItemText(i,39,DwordToString(iter->wWaterAnti));
		m_MonList.SetItemText(i,40,DwordToString(iter->wFireAnti));
		m_MonList.SetItemText(i,41,DwordToString(iter->wWindAnti));
		m_MonList.SetItemText(i,42,DwordToString(iter->wSacreAnti));
		m_MonList.SetItemText(i,43,DwordToString(iter->wDarkAnti));
		m_MonList.SetItemText(i,44,DwordToString(iter->wNotElemAnti));
		m_MonList.SetItemText(i,45,DwordToString(iter->wHpRecoverSpd));
		m_MonList.SetItemText(i,46,DwordToString(iter->wMpRecoverSpd));
		m_MonList.SetItemText(i,47,DwordToString(iter->wBodyAnti));

		m_MonList.SetItemText(i,48,DwordToString(iter->wBloodAnti));
		m_MonList.SetItemText(i,49,DwordToString(iter->wDuskAnti));
		m_MonList.SetItemText(i,50,DwordToString(iter->wNumbAnti));
		m_MonList.SetItemText(i,51,DwordToString(iter->wSleepAnti));
		m_MonList.SetItemText(i,52,DwordToString(iter->wDrugAnti));
		m_MonList.SetItemText(i,53,DwordToString(iter->wNotMoveAnti));
		m_MonList.SetItemText(i,54,DwordToString(iter->wMinStayTimes));
		m_MonList.SetItemText(i,55,DwordToString(iter->wMaxStayTimes));
		m_MonList.SetItemText(i,56,DwordToString(iter->wMinWalkTimes));
		m_MonList.SetItemText(i,57,DwordToString(iter->wMaxWalkTimes));
		m_MonList.SetItemText(i,58,DwordToString(iter->wAlertRange));
		m_MonList.SetItemText(i,59,DwordToString(iter->wPurRange));
		m_MonList.SetItemText(i,60,DwordToString(iter->wPeaMoveRange));
		m_MonList.SetItemText(i,61,(CString)(iter->strAIModel).c_str());
		m_MonList.SetItemText(i,62,(CString)(itoa(iter->bTalk,strVal,10)));
		m_MonList.SetItemText(i,63,(CString)(iter->strMonRace).c_str());
		m_MonList.SetItemText(i,64,(CString)(iter->strElem).c_str());
		m_MonList.SetItemText(i,65,DwordToString(iter->wStayTime));
		m_MonList.SetItemText(i,66,(CString)(iter->strAIWay).c_str());

		m_MonList.SetItemText(i,67,DwordToString(iter->wHurtHuman));
		m_MonList.SetItemText(i,68,DwordToString(iter->wHurtElem));
		m_MonList.SetItemText(i,69,DwordToString(iter->wHurtDiff));
		m_MonList.SetItemText(i,70,DwordToString(iter->wHurtAnimal));
		m_MonList.SetItemText(i,71,DwordToString(iter->wHurtDead));
		m_MonList.SetItemText(i,72,DwordToString(iter->wHurtDevil));
		m_MonList.SetItemText(i,73,DwordToString(iter->wHurtInsect));
		m_MonList.SetItemText(i,74,DwordToString(iter->wHurtFire));
		m_MonList.SetItemText(i,75,DwordToString(iter->wHurtWater));
		m_MonList.SetItemText(i,76,DwordToString(iter->wHurtGround));
		m_MonList.SetItemText(i,77,DwordToString(iter->wHurtWind));
		m_MonList.SetItemText(i,78,DwordToString(iter->wHurtSacre));
		m_MonList.SetItemText(i,79,DwordToString(iter->wHurtDark));
		m_MonList.SetItemText(i,80,DwordToString(iter->wHurtNotElem));
		m_MonList.SetItemText(i,81,DwordToString(iter->wVitality));
		m_MonList.SetItemText(i,82,DwordToString(iter->wStormHit));

		m_MonList.SetItemText(i,83,DwordToString(iter->wStormHurt));
		m_MonList.SetItemText(i,84,DwordToString(iter->wStormAnti));
		m_MonList.SetItemText(i,85,DwordToString(iter->wMagicStorm));
		m_MonList.SetItemText(i,86,DwordToString(iter->wMagicStormHurt));
		m_MonList.SetItemText(i,87,DwordToString(iter->wMagicStormAnti));

		m_MonList.SetItemText(i,88,ftoa(iter->fInjurRate));
		m_MonList.SetItemText(i,89,DwordToString(iter->wAlertTime));
		m_MonList.SetItemText(i,90,ftoa(iter->fEscape));
		m_MonList.SetItemText(i,91,DwordToString(iter->wSkillRate));
		m_MonList.SetItemText(i,92,DwordToString(iter->wSkillInter));
		
		m_MonList.SetItemText(i,93,(CString)(iter->strReturn).c_str());
		
		m_MonList.SetItemText(i,94,(CString)(iter->strAllRecover).c_str());
		m_MonList.SetItemText(i,95,DwordToString(iter->wReDistance));
		m_MonList.SetItemText(i,96,DwordToString(iter->wReTime));
		
		m_MonList.SetItemText(i,97,(CString)(iter->strRefe1).c_str());
		m_MonList.SetItemText(i,98,(CString)(iter->strRefe2).c_str());
		m_MonList.SetItemText(i,99,(CString)(iter->strRefe3).c_str());
		m_MonList.SetItemText(i,100,(CString)(iter->strRefe4).c_str());
		m_MonList.SetItemText(i,101,(CString)(iter->strRefe5).c_str());
		m_MonList.SetItemText(i,102,(CString)(iter->strBirthSct).c_str());
		m_MonList.SetItemText(i,103,(CString)(iter->strBattleSct).c_str());
		m_MonList.SetItemText(i,104,(CString)(iter->strInjurSct).c_str());
		m_MonList.SetItemText(i,105,(CString)(iter->strDeadSct).c_str());
		m_MonList.SetItemText(i,106,(CString)(iter->strCycleSct).c_str());
		m_MonList.SetItemText(i,107,(CString)(iter->strChoseSkillSct).c_str());
		m_MonList.SetItemText(i,108,(CString)(iter->strFindTgtSct).c_str());
		m_MonList.SetItemText(i,109,(CString)(iter->strKillTgtSct).c_str());
		
}
/*功能：显示文件到listcontrol
*
*/
void CMonsterEditorDlg::MonList()
{
	vector<tagMonster>::iterator iter;
	int i=0;
	//读取全部记录
	for(iter=vecMonster.begin();iter!=vecMonster.end();iter++,i++)
	{
		
		ListSetData(iter,i);
			
	}
	
}
void CMonsterEditorDlg::MonTree()
{

	HTREEITEM m_Root;
	HTREEITEM m_Child;
	m_Root = m_MonTree.InsertItem("所有怪物记录",0,0);
	//m_hTreeRoot=m_MonTree.InsertItem(&childItem);
	int i=0;
	vector<tagMonster>::iterator iter;
	//读取全部记录
	for(iter=vecMonster.begin();iter!=vecMonster.end();iter++,i++)
	{
		//设置节点文件
		m_Child = m_MonTree.InsertItem(iter->strOrigName.c_str(),1,1,m_Root);
	} 	
	//展开节点
	m_MonTree.Expand(m_Root,TVE_EXPAND); 
}
/*功能：双击listcontrol中的记录，显示详细对话框
*
*/
void CMonsterEditorDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码	
	POSITION pos = m_MonList.GetFirstSelectedItemPosition();
      if (pos == NULL)
	  {
		   return;
	  }
      else
      {
		  string strVal;
		  //获取行
		  int nItem = m_MonList.GetNextSelectedItem(pos);
		  //获取指定行和列的数据
		  strVal=m_MonList.GetItemText(nItem,2);
		  vector<tagMonster>::iterator iter;
		  //在vecMonster查找原始名与strVal相等的元素
		  for(iter=vecMonster.begin();iter!=vecMonster.end();iter++)
		  {
			  //如果找到此元素，设置相应树结点
			  if(iter->strOrigName==strVal.c_str())
			  {
					HTREEITEM firstItem;
					firstItem=m_MonTree.GetNextItem(m_MonTree.GetRootItem(),TVGN_CHILD);
					//当结点不为空时循环
					while(firstItem!=NULL)
					{
						//找到树结点，将其加亮
						if(m_MonTree.GetItemText(firstItem).Compare(strVal.c_str())==0)
						{
							m_MonTree.SetFocus();
							m_MonTree.SelectItem(firstItem);
							
							break;
						}
						firstItem=m_MonTree.GetNextItem(firstItem,TVGN_NEXT);
					}
					//构造一个CDetail对象，用于显示详细对话框
				 	CDetail dlg;
					//设显示详细对话框中的值
					dlg.SetData(&vecMonster,iter);
					dlg.DoModal();
					//改变树节点的文本
					m_MonTree.SetItemText(firstItem,iter->strOrigName.c_str());
					//删除listcontrol中的原记录
					m_MonList.DeleteItem(nItem);
					//设置新记录
					ListSetData(iter,nItem);
					//将修改日志显示到listbox	
					LogList(iter,2);
					
				    break;

			  }

		  }
	  }
}
/*功能：双击树中记录，显示详细对话框
*
*/
void CMonsterEditorDlg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int nItem=0;
	HTREEITEM selectedItem=m_MonTree.GetSelectedItem();
	vector<tagMonster>::iterator iter;
	//当双击树控件中的记录时，查找并显示与被点击记录相符的详细对话框
	for(iter=vecMonster.begin();iter!=vecMonster.end();iter++,nItem++)
	{	
		//找到了相符的记录
		if(m_MonTree.GetItemText(selectedItem).Compare(iter->strOrigName.c_str())==0)
		{
			//构造一个CDetail对象，用于显示详细对话框
			CDetail dlgTree;
			//设置值
			dlgTree.SetData(&vecMonster,iter);
			dlgTree.DoModal();
			//删除listcontrol原记录
			m_MonList.DeleteItem(nItem);
			//重新插入行并显示记录
			ListSetData(iter,nItem);
			//更新树控件节点的文件
			m_MonTree.SetItemText(selectedItem,iter->strOrigName.c_str());
			//将修改日志显示到listbox
			LogList(iter,2);
			//将listcontrol设置为焦点
			m_MonList.SetFocus();
			m_MonList.SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
			//将listcontrol中滚动显示到相应记录
			m_MonList.EnsureVisible(nItem,FALSE);
			
			break;
		}
	}
}
//新建记录
void CMonsterEditorDlg::OnNew()
{
	// TODO: 在此添加命令处理程序代码
	CDetail dlgnew;
	//保存vecMonster原来的大小
	int nprimer=(int)vecMonster.size();
	//设置默认值	
	dlgnew.SetNewData(&vecMonster,nprimer);
	dlgnew.DoModal();

	

	vector<tagMonster>::iterator iter=vecMonster.end()-1;
	int n=(int)vecMonster.size();
	//现在的长度比原来的长度大，表示新建记录成功
	if(n>nprimer)
	{
		//设置listcontrol中的值
		ListSetData(iter,n-1);
		m_MonList.SetFocus();
		//先取消所有已被加亮的记录
		for(int j=0;j<m_MonList.GetItemCount();j++)
		m_MonList.SetItemState(j, 0, LVIS_SELECTED|LVIS_FOCUSED);
		//将新建的记录加亮
		m_MonList.SetItemState(m_MonList.GetItemCount()-1,LVIS_SELECTED,LVIS_SELECTED);
		m_MonList.EnsureVisible(m_MonList.GetItemCount()-1,FALSE);
		//在树中显示新建记录
		HTREEITEM newItem;
		TV_INSERTSTRUCT stItem;
		//根节点
		stItem.hParent=m_MonTree.GetRootItem();
		//设置新节点
		char strVal[100];
		sprintf_s(strVal,"原始名%s",m_MonList.GetItemText(m_MonList.GetItemCount()-1,1));
		stItem.hInsertAfter=TVI_LAST;
		stItem.item.pszText=strVal;
		stItem.item.mask=TVIF_TEXT| TVIF_PARAM;
		newItem=m_MonTree.InsertItem(&stItem);
		m_MonTree.SelectItem(newItem);
		//将编辑日志显示到listbox
		LogList(iter,1);
	}
}
//刷新按钮
void CMonsterEditorDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MonList.DeleteAllItems();
	m_MonTree.DeleteAllItems();
	MonList();
	MonTree();

}
/*功能：将编辑日志显示在listbox中
*摘要：nflag为1为添加记录，2为修改记录，3为删除记录
*/
void CMonsterEditorDlg::LogList(vector<tagMonster>::iterator iter,int nflag)
{
	char strVal[100];
	
	bsaveflag=true;
	string str;
	//获取时间
	str=GetTime();
	switch(nflag)
	{
		//当nflag为NEWRECORD时，表示新建了一条记录
		case NEWRECORD:
			str="> "+str+"       添加了记录：　"+"索引号：　"+itoa(iter->dwIndex,strVal,10)+
				" 　原始名：　"+iter->strOrigName;
			break;
		//当nflag为APPENDRECORD时，表示修改了一条记录
		case UPDATERECORD:
			str="> "+str+"       编辑了记录：　"+"索引号：　"+itoa(iter->dwIndex,strVal,10)+
				"　 原始名：　"+iter->strOrigName;
			break;
		//当nflag为DELETERECORD时，表示删除了一条记录
		case DELETERECORD:
			str="> "+str+"       删除了记录：　"+"索引号：　"+itoa(iter->dwIndex,strVal,10)+
				"　 原始名：　"+iter->strOrigName;
	}	
	//追加编辑日志
	m_LogListBox.AddString(str.c_str());
	
	//定义文件指针
	FILE *fp;
	fp=fopen("log.txt","a");
	//追中日志到文件
	fprintf(fp,str.c_str());
	fprintf(fp,"\n");
	fclose(fp);
	//ncount是记录修改文件的条数，当为5时，将文件备份，然后重置为0
	ncount+=1;
	if(ncount==5)
	{
		ncount=0;
		//获取系统时间
		string str1=GetTime();
		string filename,filenameskill;
		//备份文件名
		filename="monsterlist_backup/"+str1+"_backup.txt";
		filenameskill="monserskilllist_backup/"+str1+"_backupskill.txt";
		//备份文件
		SaveFile(filename.c_str(),filenameskill.c_str());
	}
}
//在listcontrol中右击，弹出删除菜单
void CMonsterEditorDlg::OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CMenu menu;
	//获取当前光标位置
	CPoint pt;
	//加载删除菜单
	menu.LoadMenu(IDR_MENU2);
	CMenu* pMenu=menu.GetSubMenu(0);
	GetCursorPos(&pt);
	//弹出删除菜单
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x,pt.y, this);
}
//删除记录
void CMonsterEditorDlg::OnDelete()
{
	// TODO: 在此添加命令处理程序代码
	POSITION pos = m_MonList.GetFirstSelectedItemPosition();
      if (pos == NULL)
	  {
		   return;
	  }
	  //获取当前选中的行
	  int nItem=m_MonList.GetNextSelectedItem(pos);
	  vector<tagMonster>::iterator iterdelete;
	  int i=0;
	  string str;
	  str=m_MonList.GetItemText(nItem,2);
	  //在vecMonster中查找单击的记录，将其删除
	  
	  for(iterdelete=vecMonster.begin();iterdelete!=vecMonster.end();iterdelete++,i++)
	  {
		  //找到记录，将其删除
		  if(iterdelete->strOrigName==str)
		  {
			  //将编辑记录日志显示到listbox
			  LogList(iterdelete,3);
			  //在listcontrol中删除记录
			  m_MonList.DeleteItem(nItem);
			  //在vecMonster中删除记录
			  
			  //删除树相应记录
			  HTREEITEM firstItem;
			  firstItem=m_MonTree.GetNextItem(m_MonTree.GetRootItem(),TVGN_CHILD);
			  while(firstItem!=NULL)
			  {
				  //找到子节点
				  if(m_MonTree.GetItemText(firstItem).Compare(str.c_str())==0)
 				  {
					  m_MonTree.DeleteItem(firstItem);
					  break;
				  }
				  //指向下一个节点
				  firstItem=m_MonTree.GetNextItem(firstItem,TVGN_NEXT);
			  }
			  vecMonster.erase(iterdelete);
			  break;
		  }
	  }	 
}
/*功能：保存文件
*
*/
void CMonsterEditorDlg::SaveFile(const char *filename,const char *filenameskill)
{
	//打开怪物属性文件
	ofstream file(filename);
	//如果打开失败，则返回
	if(!file.is_open())
	{
		MessageBox("无法写文件,文件属性可能为只读,请检查!");
		return;
	}
	//指向怪物记录
	vector<tagMonster>::iterator iter;
	//指向怪物记录的技能记录
	//vector<tagSkill>::iterator iterskill;
	int nItem=0;
	file<<"	索引	原始名	名字	是否有阻挡	是否无敌	掉宝标志（0，1）	对应采集物名	称谓	称谓颜色	称谓图片	国家id	npc类别	boss类别	图形id	图形等级	名字颜色编号	等级颜色编号	血条颜色	声音id	是否可驯服（byte）	最大尝试驯服次数	体形	等级	经验值	职业点数	最大hp	最小攻击力	最大攻击力	最小法力攻击	最大法力攻击	命中能力	防御力	魔法防御力	格挡	招架	免伤	穿透	闪避能力	移动速度	战斗移动速度	转身速度	意志抗性	基本地元素抗性	基本水元素抗性	基本火元素抗性	基本风元素抗性	基本神圣元素抗性	基本黑暗元素抗性	基本无素抗性	生命恢复速度	魔法恢复速度	体质抗性	基本流血抗性	基本击晕抗性	基本麻痹抗性	基本催眠抗性	基本中毒抗性	基本定身抗性	基本沉默抗性	最小停留次数	最大停留次数	最小行走次数	最大行走次数	警戒范围	追击范围	和平移动范围	AI模式	可否对话	怪物种族	元素类型	停留的时间	AI挂起方式	对人形类怪物基本伤害	对元素怪物基本伤害	对异族怪物基本伤害	对动物怪物基本伤害	对亡灵怪物基本伤害	对恶魔类怪物基本伤害　	对昆虫类怪物基本伤害	对火系怪物基本伤害	对水系怪物基本伤害	对土系怪物基本伤害	对风系怪物基本伤害	对神圣系怪物基本伤害	对黑暗系列怪物基本伤害	对无元素系列怪物基本伤害	活力	物理暴击率	物理暴击伤害	物理暴击抗性	魔法暴击率	魔法暴击伤害	魔法暴击抗性	重伤比率	警戒时间	逃跑底线	技能概率	技能间隔	是否飞回	是否完全恢复	归位距离	追击时间	ai参考1	ai参考2	ai参考3	ai参考4	ai参考5	出生脚本	战斗脚本	重伤脚本	死亡脚本	周期脚本	选择技能脚本	寻找到目标脚本	杀死目标脚本\n";
	for(iter=vecMonster.begin();iter!=vecMonster.end();iter++,nItem++)
	{
		file<<"*\t";
		file<<iter->dwIndex
			<<"\t"
			<<iter->strOrigName
			<<"\t"
			<<iter->strName
			<<"\t"
			<<iter->strBlock
			<<"\t"
			<<iter->strSuper
			<<"\t"
			<<iter->wDropFlag
			<<"\t"
			<<iter->strCollection
			<<"\t"
			<<iter->strTitle
			<<"\t"
			<<iter->dwTitleColor
			<<"\t"
			<<iter->dwTitlePicture
			<<"\t"
			<<iter->strCountryID
			<<"\t"
			<<iter->strNpcSort
			<<"\t"
			<<iter->wBossLevel
			<<"\t"
			<<iter->dwPicID
			<<"\t"
			<<iter->dwPicLevel
			<<"\t"
			<<iter->dwNameColorID
			<<"\t"
			<<iter->dwLevelColorID
			<<"\t"
			<<iter->dwHpColor
			<<"\t"
			<<iter->dwSoundID
			<<"\t"
			<<iter->strTame
			<<"\t"
			<<iter->dwTameTimes
			<<"\t"
			<<iter->strShape
			<<"\t"
			<<iter->nLevel
			<<"\t"
			<<iter->dwExp
			<<"\t"
			<<iter->dwWorkPoint
			<<"\t"
			<<iter->dwMaxHp
			<<"\t"
			<<iter->wMinAttack
			<<"\t"
			<<iter->wMaxAttack
			<<"\t"
			<<iter->wMinMagicAttack
			<<"\t"
			<<iter->wMaxMagicAttack
			<<"\t"
			<<iter->wHitRating
			<<"\t"
			<<iter->wDefense
			<<"\t"
			<<iter->wMagicDefense
			<<"\t"
			<<iter->wBlock
			<<"\t"
			<<iter->wImmunity
			<<"\t"
			<<iter->wPierce
			<<"\t"
			<<iter->wParry
			<<"\t"
			<<iter->wDodge
			<<"\t"
			<<iter->fMoveSpd
			<<"\t"
			<<iter->fBatMoveSpd
			<<"\t"
			<<iter->wWillAnti
			<<"\t"
			<<iter->wGroundAnti
			<<"\t"
			<<iter->wWaterAnti
			<<"\t"
			<<iter->wFireAnti
			<<"\t"
			<<iter->wWindAnti
			<<"\t"
			<<iter->wSacreAnti
			<<"\t"
			<<iter->wDarkAnti
			<<"\t"
			<<iter->wNotElemAnti
			<<"\t"
			<<iter->wHpRecoverSpd
			<<"\t"
			<<iter->wMpRecoverSpd
			<<"\t"
			<<iter->fTurningSpeed
			<<"\t"
			<<iter->wBodyAnti
			<<"\t"
			<<iter->wBloodAnti
			<<"\t"
			<<iter->wDuskAnti
			<<"\t"
			<<iter->wNumbAnti
			<<"\t"
			<<iter->wSleepAnti
			<<"\t"
			<<iter->wDrugAnti
			<<"\t"
			<<iter->wNotMoveAnti
			<<"\t"
			<<iter->wSilenceAnti
			<<"\t"
			<<iter->wMinStayTimes
			<<"\t"
			<<iter->wMaxStayTimes
			<<"\t"
			<<iter->wMinWalkTimes
			<<"\t"
			<<iter->wMaxWalkTimes
			<<"\t"
			<<iter->wAlertRange
			<<"\t"
			<<iter->wPurRange
			<<"\t"
			<<iter->wPeaMoveRange
			<<"\t"
			<<iter->strAIModel
			<<"\t"
			<<iter->bTalk
			<<"\t"
			<<iter->strMonRace
			<<"\t"
			<<iter->strElem
			<<"\t"
			<<iter->wStayTime
			<<"\t"
			<<iter->strAIWay
			<<"\t"
			<<iter->wHurtHuman
			<<"\t"
			<<iter->wHurtElem
			<<"\t"
			<<iter->wHurtDiff
			<<"\t"
			<<iter->wHurtAnimal
			<<"\t"
			<<iter->wHurtDead
			<<"\t"
			<<iter->wHurtDevil
			<<"\t"
			<<iter->wHurtInsect
			<<"\t"
			<<iter->wHurtFire
			<<"\t"
			<<iter->wHurtWater
			<<"\t"
			<<iter->wHurtGround
			<<"\t"
			<<iter->wHurtWind
			<<"\t"
			<<iter->wHurtSacre
			<<"\t"
			<<iter->wHurtDark
			<<"\t"
			<<iter->wHurtNotElem
			<<"\t"
			<<iter->wVitality
			<<"\t"
			<<iter->wStormHit
			<<"\t"
			<<iter->wStormHurt
			<<"\t"
			<<iter->wStormAnti
			<<"\t"
			<<iter->wMagicStorm
			<<"\t"
			<<iter->wMagicStormHurt  
			<<"\t"
			<<iter->wMagicStormAnti
			<<"\t"
			<<iter->fInjurRate
			<<"\t"
			<<iter->wAlertTime
			<<"\t"
			<<iter->fEscape
			<<"\t"
			<<iter->wSkillRate
			<<"\t"
			<<iter->wSkillInter
			<<"\t"
			<<iter->strReturn
			<<"\t"
			<<iter->strAllRecover
			<<"\t"
			<<iter->wReDistance
			<<"\t"
			<<iter->wReTime
			<<"\t"
			<<iter->strRefe1
			<<"\t"
			<<iter->strRefe2
			<<"\t"
			<<iter->strRefe3
			<<"\t"
			<<iter->strRefe4
			<<"\t"
			<<iter->strRefe5
			<<"\t"
			<<iter->strBirthSct
			<<"\t"
			<<iter->strBattleSct
			<<"\t"
			<<iter->strInjurSct
			<<"\t"
			<<iter->strDeadSct
			<<"\t"
			<<iter->strCycleSct
			<<"\t"
			<<iter->strChoseSkillSct
			<<"\t"
			<<iter->strFindTgtSct
			<<"\t"
			<<iter->strKillTgtSct;
			
		file<<"\n";
	}
	file.close();
	////打开怪物技能文件
	//ofstream fileskill(filenameskill);
	////如果打开失败，则返回
	//if(!fileskill.is_open())
	//{
	//	return;
	//}
	////查找有技能记录的属性
	//for(iter=vecMonster.begin();iter!=vecMonster.end();iter++)
	//{
	//	
	//	//如果有技能记录，则写入文件
	//	if((int)iter->vecSkill.size()>0)
	//	{
	//		fileskill<<"*\t"
	//				<<iter->strOrigName
	//				<<"\n";
	//		
	//		for(iterskill=iter->vecSkill.begin();iterskill!=iter->vecSkill.end();
	//			iterskill++)
	//		{
	//			
	//			//技能类型为">"，是基本技能
	//			if(iterskill->strSkillType==">")
	//			{
	//				fileskill<<">"
	//					<<"\t"
	//					<<iterskill->dwSkillID
	//					<<"\t"
	//					<<iterskill->wSkillLevel;
	//				fileskill<<"\n";
	//					
	//			}
	//			//技能类型为"#",是普通技能
	//			else if(iterskill->strSkillType=="#")
	//			{
	//				fileskill<<"#"
	//					<<"\t"
	//					<<iterskill->dwSkillID
	//					<<"\t"
	//					<<iterskill->wSkillLevel;
	//				fileskill<<"\n";

	//			}
	//			//技能类型为"@"，是周期技能
	//			else if(iterskill->strSkillType=="@")
	//			{
	//				fileskill<<"@"
	//					<<"\t"
	//					<<iterskill->dwSkillID
	//					<<"\t"
	//					<<iterskill->wSkillLevel
	//					<<"\t"
	//					<<iterskill->wInterTime;
	//				fileskill<<"\n";
	//			}
	//		}
	//	}
	//	else
	//		continue;

	//}
	//fileskill.close();

}
//选择了菜单上的保存文件
void CMonsterEditorDlg::OnSaveToFile()
{
	// TODO: 在此添加命令处理程序代码
	SaveFile(g_strfilename.c_str(),g_strfilenameskill.c_str());
	//文件已经保存
	bsaveflag=false;
}

//恢复文件
void CMonsterEditorDlg::OnRecover()
{
	// TODO: 在此添加命令处理程序代码
	CRecover recoverdlg;
	recoverdlg.DoModal();
}
/*功能：恢复文件
*摘要：先判断备份文件是否能打开，然后清空原文件，最后将备份文件
*复制到原文件
*/
void CMonsterEditorDlg::RecoverFile(string strfilename,string strfilenameskill)
{
	setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
	//分别开打开文件
	fstream backupfile,backupfileskill;
	backupfile.open(strfilename.c_str());
	backupfileskill.open(strfilenameskill.c_str());
	//判断文件是否能打开
	if(!backupfile.is_open()||!backupfileskill.is_open())
	{
		char str[256];
		//不能打开怪物属性文件
		if(!backupfile.is_open())
		{
			sprintf_s(str, "不能打开备份文件'%s'", strfilename);
			MessageBox(str, "error");	
			return;
		}
		//不能打怪物技能文件
		else if(!backupfileskill.is_open())
		{
			sprintf_s(str, "不能打开备份文件'%s'", strfilenameskill);
			MessageBox(str, "error");	
			return;
		}	
	}
	//设置文件路径
	::SetCurrentDirectory(g_buf);
	//打开monsterlist.ini并清空文件内容	
	CFile   m_file;   
	if(m_file.Open(g_strfilename.c_str(),CFile::modeReadWrite))
	{
		//如果文件长度不为0,清空文件
		if(m_file.GetLength()>0)
		m_file.SetLength(0); 
	}
	//不能打开文件
	else
	{
		char str[256];
		sprintf_s(str, "不能打开原文件'%s'", g_strfilename.c_str());
		MessageBox(str, "error");	
		return;
	} 
	//关闭文件
	m_file.Close();
	//打开monsterlistskill.ini并清空文件内容	 
	if(m_file.Open(g_strfilenameskill.c_str(),CFile::modeReadWrite))
	{
		//如果文件长度不为0,清空文件
		if(m_file.GetLength()>0)
		m_file.SetLength(0); 
	}
	//不能打开文件
	else
	{
		char str[256];
		sprintf_s(str, "不能打开原文件'%s'", g_strfilenameskill.c_str());
		MessageBox(str, "error");	
		return;
	} 
	//关闭文件
	m_file.Close();
	ofstream file;
	file.open(g_strfilename.c_str());
	if(!file.is_open())
	{
		MessageBox("无法打开原文件");
		return;
	}
	ofstream fileskill;
	fileskill.open(g_strfilenameskill.c_str());
	if(!fileskill.is_open())
	{
		MessageBox("无法打开原文件");
		return;
	}
	//设置不跳过空格
	backupfile.unsetf(ios::skipws);
	backupfileskill.unsetf(ios::skipws);
	char ch;
	//复制备份文件到目标文件monsterlist.ini
	while(backupfile>>ch)file<<ch;
	//复制备份文件到目标文件monsterlistskill.ini
	while(backupfileskill>>ch)fileskill<<ch;
	char str2[256];
	sprintf_s(str2, "恢复'%s'和'%s'成功", g_strfilename.c_str(),g_strfilenameskill.c_str());
	MessageBox(str2,"MonsterEditor");
	//关闭所有文件
	backupfile.close();
	backupfileskill.close();
	file.close();
	fileskill.close();
}
//菜单命令，拆分文件
void CMonsterEditorDlg::OnPartFile()
{
	// TODO: 在此添加命令处理程序代码
	CPart partdlg;
	//统计monsterlist.ini记录总数
	int nallrecords=(int)vecMonster.size();
	//统计monsterskilllist.ini记录总数
	vector<tagMonster>::iterator iter=vecMonster.begin();
	int nallskillrecords=0;
	for(;iter!=vecMonster.end();iter++)
	{
		//当怪物有技能时
		if(iter->vecSkill.size()>0)
		nallskillrecords+=1;
	}
	//初始化拆分对话框
	partdlg.SetData(&vecMonster,nallrecords,nallskillrecords);

	partdlg.DoModal();
}
//合并文件
void CMonsterEditorDlg::OnMergerFile()
{
	// TODO: 在此添加命令处理程序代码
	CMerger mergerdlg;
	mergerdlg.DoModal();
}
//关闭主程序
void CMonsterEditorDlg::OnClose()
{
	//没有保存文件
	if(bsaveflag)
	{
		int ret=MessageBox("是否保存文件","MonsterEditor",MB_YESNO);
		//选择保存文件
		if(ret==IDYES)
		{
			SaveFile(g_strfilename.c_str(),g_strfilenameskill.c_str());
		}
	}
	CDialog::OnClose();
}
BOOL CMonsterEditorDlg::PreTranslateMessage(MSG *pMsg)
{
	if(CWnd::PreTranslateMessage(pMsg))
		return TRUE;
	return((m_hAccelTable!=NULL)&&::TranslateAccelerator(m_hWnd,m_hAccelTable,pMsg));
}