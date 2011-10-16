//=============================================================================
/**
 *  file: TestScript.h
 *
 *  Brief:测试管理、脚本编辑
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class TiXmlElement;

struct tagTestCase
{
	string strCaseName;				//用例名
	string strStartScriptName;		//开始脚本名
	string strEndScriptName;		//结束脚本名
};

class CTestScript : public CDialog
{
	DECLARE_DYNAMIC(CTestScript)

public:
	CTestScript(CWnd* pParent = NULL);
	virtual ~CTestScript();

	enum { IDD = IDD_PROPPAGE_TESTSCRIPT };
public:
	struct tagServerList
	{
		DWORD dwPort;
		string strIp;
		string name;
	};
	struct tagRegionList
	{
		int	type;
		long id;
		long ResourceID;
		long AreaId;
		int expScale;
		int OccuExpScale;
		int spScale;
		int TYPE;
		int	noPK;
		int	no;
		string name;
		int GS_ID;
		int Country;
		long RgnInfo;
		int waterRgn;
		string RgnGuid;
		int LinedIdFlag;
	};


	//GameTest测试的类型选择
	enum GTTESTTYPE		
	{
		TEST_SKILL,		//测试技能使用
		TEST_GOODS,		//测试物品
		TEST_PK,		//测试PK
		TEST_MAIL,		//测试邮件
		TEST_COPYRGN,	//测试副本
		TEST_SPRITE,	//测试精灵
		TEST_LOGIN,		//测试登陆上下线
		TEST_INCREAMENT,//测试增值商店
		TEST_GUILD,		//测试行会
		TEST_TEAM,		//测试组队
		TEST_FRIEND,	//测试好友
		TEST_ALL,		//测试所有的功能
		TEST_TEMP,		//临时的测试脚本
	};
private:
	vector<tagServerList> m_vecServer;	//服务器的容器
	vector<tagRegionList> m_vecRegion;	//游戏场景的容器 by:LGR
	map<DWORD,DWORD> m_mapTestType;		//测试类型的容器
	map<int,string>  m_mapOccu;			//游戏职业的容器 by:LGR
	map<int,long>  m_mapTestConfigRecord;  //记录上一次测试环境配置

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg	void OnBnClickedButtonStartLogin();						//批登录

	afx_msg void OnCheckScriptSyntax();								//语法检测
	afx_msg void OnCheckOpenScript();								//打开文件
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnCaseStartTest();									//开始测试某个用例
	afx_msg void OnCaseStopTest();									//结束测试某个用例
	afx_msg void OnCheckUpdateScript();								//更新重读脚本
	afx_msg void OnCheckThirdOccu();
	afx_msg void OnCheckCloseGT();
	virtual BOOL OnInitDialog();
private:
	//	复选 是否盛大登录方式
	CButton m_btnChkPtToken;
	CTreeCtrl m_treeTestCase;
	long	m_lCaseNum;
	CEdit m_edScriptEdit;
	CEdit m_edScriptInfo;
	CStatic m_ctlShowScriptPath;
	long m_lStartCDKey;
	long m_lEndCDKey;
	CEdit m_edStartCDKey;
	CEdit m_edEndCDKey;
	CEdit m_edTimer;
	CEdit m_edPlayerLevel;
	//CString m_edPlayerRgn;
	CButton m_bnMaxState;
	CButton m_bnEquipWeapon;
	CButton m_bnThirdOccu;
	long m_lPlayerLevel;
	//long m_lPlayerRgn;
	long m_lPlayerX;
	long m_lPlayerY;
	CEdit m_edPlayerX;
	CEdit m_edPlayerY;

	CComboBox m_ctrlServerList;			//与服务器的容器所关联的ComboBox对象
	CComboBox m_ctrlTestType;			//与测试类型的容器所关联的ComboBox对象
	CComboBox m_ctrlRegionList;			//与测试场景的容器所关联的ComboBox对象
	CComboBox m_ctrlOcculist;			//与测试职业的容器所关联的ComboBox对象

	DWORD m_dwCurSelServer;					//当前的选择的服务器下拉列表控件中所选的序号
	DWORD m_dwCurSelTestType;				//当前的选择的测试类型下拉列表控件中所选的序号

	CButton m_bnStartLogin;
	CButton m_bnChosProj;

	CString m_csCurrtWorkPath;
private:
	typedef map<long,tagTestCase*> MultiTestCases;
	typedef map<long,tagTestCase*>::iterator itTestCase;
	//typedef hash_map<string,tagTestCase*> MultiTestCases;
	//typedef hash_map<string,tagTestCase*>::iterator itTestCase;
	MultiTestCases	m_TestCases;

	//=====
//	bool bLoadLuaRequireFile;
	//=====
public:
	void LoadCase(TiXmlElement *pElement);
	void LoadCaseSetup();
	void InitTree();

	bool SaveTestConfigInfo(void);
	void OutputScriptInfo(const char* strInfo);
	afx_msg void OnNMRclickTreeTestCase(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSavescript();
									
	CString m_edScript;
	afx_msg void OnAddcase();									//添加一个用例
	//
	afx_msg void OnAddscript();									//添加一个脚本
	afx_msg void OnDelScript();									//删除一个脚本
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCaseCheckInCase();
	CString m_strOutputInfo;
	//CComboBox	m_comTestType;
	//单击树状控件
	afx_msg void OnTvnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	//选中树状控件根节点 弹出菜单上的 添加用例
	afx_msg void OnAddCase();



	//int m_TestType;							//当前的测试类型
public:
	tagServerList* GetServer()	{ return &m_vecServer[m_dwCurSelServer]; }
	//获取IP
	const char* GetIp()	{return GetServer()->strIp.c_str();}
	const tagServerList* GetServerIpAddrAndPort()	
	{
		if( !m_vecServer.empty() )
		{
			DWORD dwSel = m_ctrlServerList.GetCurSel();
			return &m_vecServer[dwSel];
		}
		return NULL;
	}
	//获取测试类型TestType
	int GetTestType();

	BOOL InitServerList();
	BOOL InitRegionList();
	BOOL InitOccuList();
	BOOL InitProjList();
	bool InitTestType(void);
	bool InitTestConfigInfo(void);
	bool ReadTo(ifstream& inf,const char* flag);
	void OnCheckSetWorkPath();
	afx_msg void OnBnClickedButtonChangetesttype();


private:
	//该变量是控件变量,数值为机器人发送消息的时间间隔
	long m_lTimer;
	//该变量是控件变量,数值为机器人发送技能消息的时间间隔
   // long m_lSendDelayTime;
public:
	//inline long GetDelayTime()	{ return m_lSendDelayTime; }
	inline long GetTimer(void)	{ return m_lTimer; }
	//inline bool GetSaveStart()	{ return m_bSaveStart; }	
	afx_msg void OnBnClickedCheckPtToken();
	

};
