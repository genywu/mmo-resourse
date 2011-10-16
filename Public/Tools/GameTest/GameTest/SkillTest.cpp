/*
=========================================================================
备注：暂未使用。2010.10.28  BY:LGR
=========================================================================
*/

/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：CSkillTest.cpp
*  功能：技能测试对话框界面
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
#include "SkillTest.h"

IMPLEMENT_DYNAMIC(CSkillTest, CDialog)

CSkillTest::CSkillTest(CWnd* pParent /*=NULL*/)
	: CDialog(CSkillTest::IDD, pParent)
	, m_RegionID(60101)
	, m_lPosX(280)
 	, m_lPosY(290)
	, m_lHighAttack(1)
	, m_lLowAttack(1)
	, m_bUseSkill1(TRUE)
	, m_bUseSkill2(TRUE)
	, m_bUseSkill3(TRUE)
	, m_lOccu(22)
	//, m_lWeapon(_T(S080E01))
	, m_lSkillAccountBegin(2101)
	, m_lSkillAccountEnd(2200)
	, m_lTimer(800)
	, m_lVersion(5008)
	, m_lCustomSkillID1(0)
	, m_lCustomSkillID2(0)
	, m_lCustomSkillID3(0)
	, m_lCustomSkillID4(0)
	, m_bRandomPos(TRUE)
	, m_bIsCanACCAnswer(FALSE)
	, m_bUseSDO(false)
{
	m_lWeapon = "";
}


CSkillTest::~CSkillTest()
{

}

void CSkillTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_REGIONID, m_RegionID);
	DDX_Text(pDX, IDC_EDIT_POSX, m_lPosX);
	DDV_MinMaxLong(pDX, m_lPosX, 0, 512);
	DDX_Text(pDX,IDC_EDIT_POSY,m_lPosY);
	DDV_MinMaxLong(pDX,m_lPosY,0,512);
	DDX_Check(pDX, IDC_CHECK_SKILL1, m_bUseSkill1);
	DDX_Check(pDX, IDC_CHECK_SKILL2, m_bUseSkill2);
	DDX_Check(pDX, IDC_CHECK_SKILL3, m_bUseSkill3);
	DDX_Control(pDX, IDC_COMBO_OCCU, m_CombOccu);
	DDX_Text(pDX, IDC_SkillStartCDKey, m_lSkillAccountBegin);
	DDX_Text(pDX, IDC_SkillEndCDKey, m_lSkillAccountEnd);
	DDX_Text(pDX, IDC_EDIT_SkillTimer, m_lTimer);
	DDX_Text(pDX, IDC_EDIT_SkillVersion, m_lVersion);
	DDX_Control(pDX, IDC_COMBO_SkillIP, m_CombIP);
	DDX_Text(pDX, IDC_EDIT_SKILLID1,m_lCustomSkillID1);
	DDX_Text(pDX, IDC_EDIT_SKILLID2,m_lCustomSkillID2);
	DDX_Text(pDX, IDC_EDIT_SKILLID3,m_lCustomSkillID3);
	DDX_Text(pDX, IDC_EDIT_SKILLID4,m_lCustomSkillID4);
	DDX_Check(pDX, IDC_CHECK1, m_bRandomPos);
	DDX_Check(pDX, IDC_CHECK_SKILL_ACCAnswer, m_bIsCanACCAnswer);
	DDX_Check(pDX, IDC_CHECK2 ,m_bUseSDO);
}


BEGIN_MESSAGE_MAP(CSkillTest, CDialog)
//	ON_EN_CHANGE(IDC_EDIT_REGIONID, &CSkillTest::OnEnChangeEditRegionid)
ON_BN_CLICKED(IDC_RADIO_ATTACK_HIGH, &CSkillTest::OnBnClickedRadioAttackHigh)
ON_BN_CLICKED(IDC_RADIO_ATTACK_LOW, &CSkillTest::OnBnClickedRadioAttackLow)
ON_BN_CLICKED(IDC_RADIO_WEAPON1, &CSkillTest::OnBnClickedRadioWeapon1)
ON_BN_CLICKED(IDC_RADIO_WEAPON2, &CSkillTest::OnBnClickedRadioWeapon2)
ON_BN_CLICKED(IDC_RADIO_WEAPON3, &CSkillTest::OnBnClickedRadioWeapon3)
ON_BN_CLICKED(IDC_RADIO_WEAPON4, &CSkillTest::OnBnClickedRadioWeapon4)
ON_BN_CLICKED(IDC_RADIO_WEAPON5, &CSkillTest::OnBnClickedRadioWeapon5)
ON_BN_CLICKED(IDC_RADIO_WEAPON6, &CSkillTest::OnBnClickedRadioWeapon6)
ON_BN_CLICKED(IDC_RADIO_WEAPON7, &CSkillTest::OnBnClickedRadioWeapon7)
ON_BN_CLICKED(IDC_BUTTON_SkillStart, &CSkillTest::OnBnClickedButtonSkillstart)
ON_BN_CLICKED(IDC_CHECK_SKILL_ACCAnswer, &CSkillTest::OnBnClickedACCAnswer)
ON_CBN_SELCHANGE(IDC_COMBO_OCCU, &CSkillTest::OnCbnSelchangeComboOccu)
ON_CBN_SELCHANGE(IDC_COMBO_SkillIP, &CSkillTest::OnCbnSelchangeComboSkillip)
ON_BN_CLICKED(IDC_CHECK2, &CSkillTest::OnBnClickedCheck2)
END_MESSAGE_MAP()

BOOL CSkillTest::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	//分别设置两组单选按钮的初始选项
	((CButton*)GetDlgItem(IDC_RADIO_ATTACK_LOW))->SetCheck(1);
	//((CButton*)GetDlgItem(IDC_RADIO_WEAPON1))->SetCheck(1);

	//设置"选择职业"的下拉列表
	m_CombOccu.InsertString(-1,"战士");
	m_CombOccu.InsertString(-1,"法师");
	m_CombOccu.InsertString(-1,"行者");
	m_CombOccu.InsertString(-1,"骑士");
	m_CombOccu.InsertString(-1,"斗士");
	m_CombOccu.InsertString(-1,"斥候");
	m_CombOccu.InsertString(-1,"艺人");
	m_CombOccu.InsertString(-1,"术士");
	m_CombOccu.InsertString(-1,"牧师");
	m_CombOccu.InsertString(-1,"圣骑士");
	m_CombOccu.InsertString(-1,"暗骑士");
	m_CombOccu.InsertString(-1,"剑士");
	m_CombOccu.InsertString(-1,"狂战士");
	m_CombOccu.InsertString(-1,"巡守");
	m_CombOccu.InsertString(-1,"刺客");
	m_CombOccu.InsertString(-1,"剑舞者");
	m_CombOccu.InsertString(-1,"吟游诗人");
	m_CombOccu.InsertString(-1,"巫师");
	m_CombOccu.InsertString(-1,"死灵法师");
	m_CombOccu.InsertString(-1,"主教");
	m_CombOccu.InsertString(-1,"贤者");
	m_CombOccu.InsertString(-1,"随机");
	//显示默认的条目
	m_CombOccu.SetCurSel(21);

	//设置"IP地址"的下拉列表
	m_CombIP.InsertString(-1,"21服务器");
	m_CombIP.InsertString(-1,"桥哥40");
	m_CombIP.InsertString(-1,"桥哥50");
	m_CombIP.InsertString(-1,"先进");
	m_CombIP.InsertString(-1,"张敏");
	m_CombIP.InsertString(-1,"科智");
	m_CombIP.InsertString(-1,"桥哥2");
	m_CombIP.InsertString(-1,"26服务器");
	m_CombIP.InsertString(-1,"相运峰");
	m_CombIP.InsertString(-1,"肖尧");
	m_CombIP.InsertString(-1,"海川");
	//显示默认的条目
	m_CombIP.SetCurSel(0);

	m_CombIP.ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_SERVER)->ShowWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


//设置为很高的攻击力
void CSkillTest::OnBnClickedRadioAttackHigh()
{
	m_lHighAttack = 3000;
	m_lLowAttack = 3000;
}

//设置为很低的攻击力
void CSkillTest::OnBnClickedRadioAttackLow()
{
	m_lHighAttack = 1;
	m_lLowAttack = 1;
}

//选择的武器
void CSkillTest::OnBnClickedRadioWeapon1()
{
	m_lWeapon = "S080E01";
}

void CSkillTest::OnBnClickedRadioWeapon2()
{
	m_lWeapon = "T060E01";
}

void CSkillTest::OnBnClickedRadioWeapon3()
{
	m_lWeapon = "E100A01";
}

void CSkillTest::OnBnClickedRadioWeapon4()
{
	m_lWeapon = "A100A01";
}

void CSkillTest::OnBnClickedRadioWeapon5()
{
	m_lWeapon = "B080A00";
}

void CSkillTest::OnBnClickedRadioWeapon6()
{
	m_lWeapon = "X100A01";
}



void CSkillTest::OnBnClickedRadioWeapon7()
{
	m_lWeapon = "D060A00";
}

//玩家选择职业
void CSkillTest::OnCbnSelchangeComboOccu()
{
	
	long Occu = m_CombOccu.GetCurSel();
	
	switch(Occu)
	{
	case 0:
		m_lOccu = 1;
		break;
	case 1:
		m_lOccu = 2;
		break;
	case 2:
		m_lOccu = 3;
		break;
	case 3:
		m_lOccu = 4;
		break;
	case 4:
		m_lOccu = 5;
		break;
	case 5:
		m_lOccu = 6;
		break;
	case 6:
		m_lOccu = 7;
		break;
	case 7:
		m_lOccu = 8;
		break;
	case 8:
		m_lOccu = 9;
		break;
	case 9:
		m_lOccu = 10;
		break;
	case 10:
		m_lOccu = 11;
		break;
	case 11:
		m_lOccu = 12;
		break;
	case 12:
		m_lOccu = 13;
		break;
	case 13:
		m_lOccu = 14;
		break;
	case 14:
		m_lOccu = 15;
		break;
	case 15:
		m_lOccu = 16;
		break;
	case 16:
		m_lOccu = 17;
		break;
	case 17:
		m_lOccu = 18;
		break;
	case 18:
		m_lOccu = 19;
		break;
	case 19:
		m_lOccu = 20;
		break;
	case 20:
		m_lOccu = 21;
		break;
	case 21:
		m_lOccu = 22;
	}
}

void CSkillTest::OnCbnSelchangeComboSkillip()
{

	long IP = m_CombIP.GetCurSel();
	//m_CombIP.InsertString("21服务器");
	//m_CombIP.InsertString("桥哥40");
	//m_CombIP.InsertString("桥哥50");
	//m_CombIP.InsertString("先进");
	//m_CombIP.InsertString("张敏");
	//m_CombIP.InsertString("科智");
	//m_CombIP.InsertString("桥哥2");
	//m_CombIP.InsertString("26服务器");
	//m_CombIP.InsertString("相运峰");
	//m_CombIP.InsertString("肖尧");
	//m_CombIP.InsertString("海川");
	switch(IP)
	{
	case 0:
		m_strIP = "192.168.220.21";
		break;
	case 1:
		m_strIP = "192.168.220.40";
		break;
	case 2:
		m_strIP = "192.168.220.50";
		break;
	case 3:
		m_strIP = "192.168.220.3";
		break;
	case 4:
		m_strIP = "192.168.220.31";
		break;
	case 5:
		m_strIP = "192.168.220.9";
		break;
	case 6:
		m_strIP = "192.168.220.2";
		break;
	case 7:
		m_strIP = "192.168.220.26";
		break;
	case 8:
		m_strIP = "192.168.220.24";
		break;
	case 9:
		m_strIP = "192.168.220.27";
		break;
	case 10:
		m_strIP = "192.168.220.8";
		break;
	}

}


void CSkillTest::OnBnClickedACCAnswer()
{
	m_bIsCanACCAnswer = ((CButton*)GetDlgItem(IDC_CHECK_SKILL_ACCAnswer))->GetCheck() ? true : false ;
}


//机器人开始测试按钮
void CSkillTest::OnBnClickedButtonSkillstart()
{
	UpdateData(TRUE);

	//判断是不是用盛大方式登陆
	if (!m_bUseSDO)
	{
		//判断需要使用的技能的范围
		if(m_bUseSkill1 == TRUE && m_bUseSkill2 == TRUE && m_bUseSkill3 == TRUE)
		{
			//使用1转2转3转技能
			m_lSkillRange = 1;
		}
		else if (m_bUseSkill1 == TRUE && m_bUseSkill2 == TRUE && m_bUseSkill3 == FALSE)
		{
			//使用1转2转技能,不使用3转技能
			m_lSkillRange = 2;
		}
		else if (m_bUseSkill1 == TRUE && m_bUseSkill2 == FALSE && m_bUseSkill3 == TRUE)
		{
			//使用1转3转技能,不使用2转技能
			m_lSkillRange = 3;
		}
		else if (m_bUseSkill1 == TRUE && m_bUseSkill2 == FALSE && m_bUseSkill3 == FALSE)
		{
			//使用1转技能,不使用2转3转技能
			m_lSkillRange = 4;
		}
		else if(m_bUseSkill1 == FALSE && m_bUseSkill2 == TRUE && m_bUseSkill3 == TRUE)
		{
			//使用2转3转技能,不使用1转技能
			m_lSkillRange = 5;
		}
		else if (m_bUseSkill1 == FALSE && m_bUseSkill2 == TRUE && m_bUseSkill3 == FALSE)
		{
			//使用2转技能,不使用1转3转技能
			m_lSkillRange = 6;
		}
		else if (m_bUseSkill1 == FALSE && m_bUseSkill2 == FALSE && m_bUseSkill3 == TRUE)
		{	
			//使用3转技能,不使用1转2转技能
			m_lSkillRange = 7;
		}
		else if (m_bUseSkill1 == FALSE && m_bUseSkill2 == FALSE && m_bUseSkill3 == FALSE)
		{
			//不使用任何技能,错误
			MessageBox("没有指定使用的技能的范围,请重新选择","错误",MB_OK);
			return;
		}

		//用于判断测试类型容器中应返回的值
		long lType = 0;

		if ( m_lSkillAccountBegin < 1000 || m_lSkillAccountEnd > 9999)
		{
			MessageBox("《起始帐号》应该大于1000,小于9999","提示",MB_OK);
			return ;
		}
		if ( m_lSkillAccountEnd < m_lSkillAccountBegin || m_lSkillAccountEnd > 9999 )
		{
			MessageBox("《结束帐号》应该大于或则等于《起始帐号》并且小于9999","提示",MB_OK);
			return ;
		}
		if (m_lTimer >= 200000 || m_lTimer <= 200)
		{
			MessageBox("间隔时间必须在200-200000ms之内","提示",MB_OK);
			return ;
		}

		if ( m_lSkillAccountBegin == m_lSkillAccountEnd )
		{
			m_lSkillAccountEnd = m_lSkillAccountBegin;
		}

		//sprintf(strScriptName,"SkillLoginCDKey(%d,%d,%d,%d%s",m_lSkillAccountBegin,m_lSkillAccountEnd,lType,m_lTimer,");");

		//CScriptSys::getInstance()->PerformString(strScriptName);

		//将所有的参数压入栈中
		lua_State* L = CScriptSys::getInstance()->GetLuaState();

		lua_settop(L,1);
		lua_pushvalue(L,LUA_GLOBALSINDEX);
		lua_pushstring(L, "SkillLoginCDKey");
		lua_gettable(L,-2);
		lua_pushnumber(L,m_lSkillAccountBegin);			//机器人起始账号 
		lua_pushnumber(L,m_lSkillAccountEnd);			//机器人结束账号
		lua_pushnumber(L,lType);							//测试类型(技能)
		lua_pushnumber(L,m_lTimer);						//发消息的时间间隔
		lua_pushnumber(L,m_lVersion);					//服务器版本号
		lua_pushnumber(L,m_RegionID);					//场景ID
		lua_pushnumber(L,m_lPosX);						//X坐标
		lua_pushnumber(L,m_lPosY);						//Y坐标
		lua_pushnumber(L,m_lHighAttack);					//最大攻击力
		lua_pushnumber(L,m_lLowAttack);					//最小攻击力
		lua_pushstring(L,m_lWeapon.c_str());					//使用的武器的原始名
		lua_pushnumber(L,m_lOccu);						//职业编号
		lua_pushnumber(L,m_lSkillRange);					//使用的技能的范围
		lua_pushnumber(L,m_lCustomSkillID1);				//自定义的技能ID1
		lua_pushnumber(L,m_lCustomSkillID2);				//自定义的技能ID2
		lua_pushnumber(L,m_lCustomSkillID3);				//自定义的技能ID3
		lua_pushnumber(L,m_lCustomSkillID4);					//自定义的技能ID4
		lua_pushboolean(L,m_bRandomPos);					//随机的坐标点
		//
		lua_pushboolean(L,m_bIsCanACCAnswer);				//是否要答题
		//
		int nRet = lua_pcall(L,19,0,0);
		if(nRet != 0)
		{
			char strInfo[512]="";
			sprintf(strInfo,"error in function <SkillLoginCDKey>(L:%d,info:%s)",
				nRet,/*lua_tostring(L,-1,0));*/lua_tolstring(L,-1,0)); //修改：谭浩文
			PutDebugString(strInfo);
			MessageBox(strInfo,"错误",MB_OK);
			lua_pop(L,1);
			return;
		}
	}
	else
	{
		//判断需要使用的技能的范围
		if(m_bUseSkill1 == TRUE && m_bUseSkill2 == TRUE && m_bUseSkill3 == TRUE)
		{
			//使用1转2转3转技能
			m_lSkillRange = 1;
		}
		else if (m_bUseSkill1 == TRUE && m_bUseSkill2 == TRUE && m_bUseSkill3 == FALSE)
		{
			//使用1转2转技能,不使用3转技能
			m_lSkillRange = 2;
		}
		else if (m_bUseSkill1 == TRUE && m_bUseSkill2 == FALSE && m_bUseSkill3 == TRUE)
		{
			//使用1转3转技能,不使用2转技能
			m_lSkillRange = 3;
		}
		else if (m_bUseSkill1 == TRUE && m_bUseSkill2 == FALSE && m_bUseSkill3 == FALSE)
		{
			//使用1转技能,不使用2转3转技能
			m_lSkillRange = 4;
		}
		else if(m_bUseSkill1 == FALSE && m_bUseSkill2 == TRUE && m_bUseSkill3 == TRUE)
		{
			//使用2转3转技能,不使用1转技能
			m_lSkillRange = 5;
		}
		else if (m_bUseSkill1 == FALSE && m_bUseSkill2 == TRUE && m_bUseSkill3 == FALSE)
		{
			//使用2转技能,不使用1转3转技能
			m_lSkillRange = 6;
		}
		else if (m_bUseSkill1 == FALSE && m_bUseSkill2 == FALSE && m_bUseSkill3 == TRUE)
		{	
			//使用3转技能,不使用1转2转技能
			m_lSkillRange = 7;
		}
		else if (m_bUseSkill1 == FALSE && m_bUseSkill2 == FALSE && m_bUseSkill3 == FALSE)
		{
			//不使用任何技能,错误
			MessageBox("没有指定使用的技能的范围,请重新选择","错误",MB_OK);
			return;
		}

		//用于判断测试类型容器中应返回的值
		long lType = 0;

		if ( m_lSkillAccountBegin <= 0)
		{
			MessageBox("玩家数量需要大于0","提示",MB_OK);
			return ;
		}
		if (m_lTimer >= 200000 || m_lTimer <= 200)
		{
			MessageBox("间隔时间必须在200-200000ms之内","提示",MB_OK);
			return ;
		}

		//将所有的参数压入栈中
		lua_State* L = CScriptSys::getInstance()->GetLuaState();

		lua_settop(L,1);
		lua_pushvalue(L,LUA_GLOBALSINDEX);
		lua_pushstring(L, "SkillLoginToken");
		lua_gettable(L,-2);
		lua_pushnumber(L,m_lSkillAccountBegin);			//机器人数量
		lua_pushnumber(L,lType);							//测试类型(技能)
		lua_pushnumber(L,m_lTimer);						//发消息的时间间隔
		lua_pushnumber(L,m_lVersion);					//服务器版本号
		lua_pushnumber(L,m_RegionID);					//场景ID
		lua_pushnumber(L,m_lPosX);						//X坐标
		lua_pushnumber(L,m_lPosY);						//Y坐标
		lua_pushnumber(L,m_lHighAttack);					//最大攻击力
		lua_pushnumber(L,m_lLowAttack);					//最小攻击力
		lua_pushstring(L,m_lWeapon.c_str());					//使用的武器的原始名
		lua_pushnumber(L,m_lOccu);						//职业编号
		lua_pushnumber(L,m_lSkillRange);					//使用的技能的范围
		lua_pushnumber(L,m_lCustomSkillID1);				//自定义的技能ID1
		lua_pushnumber(L,m_lCustomSkillID2);				//自定义的技能ID2
		lua_pushnumber(L,m_lCustomSkillID3);				//自定义的技能ID3
		lua_pushnumber(L,m_lCustomSkillID4);					//自定义的技能ID4
		lua_pushboolean(L,m_bRandomPos);					//随机的坐标点
		//
		lua_pushboolean(L,m_bIsCanACCAnswer);				//是否要答题
		//
		int nRet = lua_pcall(L,18,0,0);
		if(nRet != 0)
		{
			char strInfo[512]="";
			sprintf(strInfo,"error in function <SkillLoginCDKey>(L:%d,info:%s)",
				nRet,/*lua_tostring(L,-1,0));*/lua_tolstring(L,-1,0)); //修改：谭浩文
			PutDebugString(strInfo);
			MessageBox(strInfo,"错误",MB_OK);
			lua_pop(L,1);
			return;
		}		
	}

}
void CSkillTest::OnBnClickedCheck2()
{
	UpdateData(TRUE);

	//判断是否使用盛大的登陆方式
	if (!m_bUseSDO)
	{
		GetDlgItem(IDC_STATIC_SKILL_START_CDKEY)->SetWindowText("起始帐号:gm");
		GetDlgItem(IDC_STATIC_SKILL_END_CDKEY)->ShowWindow(TRUE);
		GetDlgItem(IDC_SkillEndCDKey)->ShowWindow(TRUE);	
	}
	else
	{
		GetDlgItem(IDC_STATIC_SKILL_START_CDKEY)->SetWindowText("机器人数量");
		GetDlgItem(IDC_STATIC_SKILL_END_CDKEY)->ShowWindow(FALSE);
		GetDlgItem(IDC_SkillEndCDKey)->ShowWindow(FALSE);
	}
}
