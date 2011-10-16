// GoodAttrPage.cpp : 实现文件
//

#include "stdafx.h"
#include "GoodsEditor.h"
#include "GoodAttrPage.h"
#include ".\goodattrpage.h"
#include "ContainerPage.h"
#include "AllGoodsPage.h"
#include "AllGoodsTree.h"
#include "mainfrm.h"


tagAddAttr g_stAddAttrCopy;			// 用来复制粘贴的属性

// CGoodAttrPage
extern int GoodID;
extern vector<tagGoodAttr> vecGoodAttr;
const char* strGoodType[]=
{
	"不能用",//0
	"普通",//1
	"消耗性道具",//2
	"卡片",//3
	"商品",//4
	"子背包",//5
	"拾取品",//6
    "称号",//7
	"金钱",//8
	"银币",//9
	"装备",//10
	"法宝",//11
	"无类别"
};

IMPLEMENT_DYNCREATE(CGoodAttrPage, CFormView)


CGoodAttrPage::CGoodAttrPage()
: CFormView(CGoodAttrPage::IDD)
, m_nSaveDB(0)
, m_strName(_T(""))
, m_strUIID(_T(""))
, m_strGroundID(_T(""))
, m_strCosplayID(_T(""))
, m_nPrice(0)
, m_nSilverPrice(0)
, m_nGoodType(0)
, m_strSound(_T(""))
, m_nWeight(0)
, m_strDesc(_T(""))
, m_nAttrSel(0)
, m_nVail(0)
, m_nHide(0)
, m_nVal1(0)
, m_nVal2(0)	
, m_nOdds(0)
, m_nMin(0)
, m_nMax(0)
, m_nOdds2(0)
, m_nValSel(0)
, m_lSoundID1(0)
, m_lSoundID2(0)
, m_bSoundSwitch(0)
, m_lWeaponActType(0)

{
	bInit=false;
	m_bEdit=false;
	m_bAdd=false;
	//m_cDescEdit;
	//m_cDescEdit.SetLimitText(5000);
}

CGoodAttrPage::~CGoodAttrPage()
{
}

void CGoodAttrPage::InitMyCtrls()
{
	std::ifstream TypeFile;
	TypeFile.open("ItemType.dat");
	if(TypeFile.fail())
	{
		MessageBox("读取文件ItemType.dat失败");
		exit(0);
	}
	int nTypeCnt=0;
	string strTem,strTypeName;
	TypeFile>>nTypeCnt;
	if(TypeFile>0)
	{
		for(int i=0;i<nTypeCnt;i++)
		{
			TypeFile>>strTem>>strTem>>strTypeName;
			m_cAttrCombo.InsertString(i,strTypeName.c_str());
			m_cAttrCombo2.InsertString(i,strTypeName.c_str());
			//add by wang-xx 2009-6-30 begin
			//激发属性
			m_cArouseAttrCombo.InsertString(i,strTypeName.c_str());
			//add by wang-xx 2009-6-30 end
			AttrNamesVec.push_back(strTypeName);
		}
	}
	//insert string to good type combo	
	for(int i=0;i<sizeof( strGoodType ) / sizeof( const char* );i++)
		m_cGoodType.InsertString(i,strGoodType[i]);

	//m_cSingleGoodList

	//m_cAttrList column
	//::SendMessage(m_cAttrList.m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	m_cAttrList.InsertColumn(0,"名称",LVCFMT_LEFT,115);
	m_cAttrList.InsertColumn(1,"有效",LVCFMT_LEFT,40);
	m_cAttrList.InsertColumn(2,"隐藏",LVCFMT_LEFT,40);

	m_cAttrList.InsertColumn(3,"值1",LVCFMT_LEFT,75);
	m_cAttrList.InsertColumn(4,"值2",LVCFMT_LEFT,75);
	//没有附加属性的出现几率了
	//m_cAttrList.InsertColumn(5,"出现几率",LVCFMT_LEFT,60);
	m_cAttrList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);


	m_cAttrList2.InsertColumn(0,"名称",LVCFMT_LEFT,120);		
	m_cAttrList2.InsertColumn(1,"值1",LVCFMT_LEFT,60);
	m_cAttrList2.InsertColumn(2,"值2",LVCFMT_LEFT,60);
	m_cAttrList2.InsertColumn(3,"套装件数",LVCFMT_LEFT,60);
	m_cAttrList2.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//m_cValList
	//m_cValList.InsertColumn(0,"值",LVCFMT_LEFT,60);
	//m_cValList.InsertColumn(1,"min",LVCFMT_LEFT,60);
	//m_cValList.InsertColumn(2,"max",LVCFMT_LEFT,60);
	//m_cValList.InsertColumn(3,"几率",LVCFMT_LEFT,40);	
	//m_cValList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//change combobox's selection state
	m_cSaveDBCombo.SetCurSel(0);
	m_cVailCombo.SetCurSel(1);
	m_cHideCombo.SetCurSel(0);	

	//add by wang-xx 2009-6-30 begin
	//激发属性
	m_cArouseAttrList.InsertColumn(0,"名称",LVCFMT_LEFT,120);		
	m_cArouseAttrList.InsertColumn(1,"值1",LVCFMT_LEFT,60);
	m_cArouseAttrList.InsertColumn(2,"值2",LVCFMT_LEFT,60);
	m_cArouseAttrList.InsertColumn(3,"激发次数",LVCFMT_LEFT,60);
	m_cArouseAttrList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	//add by wang-xx 2009-6-30 end
}

void CGoodAttrPage::ClearAttr()
{
	m_tagGoodAttr.vecAddAttr.clear();
}

void CGoodAttrPage::ClearCtrls()
{
	m_cSaveDBCombo.SetCurSel(0);
	m_cOrigName.SetWindowText("");
	m_cNameEdit.SetWindowText("未命名");
	m_cGroundEdit.SetWindowText("");
	m_cUIEdit.SetWindowText("");
	m_cCosplayEdit.SetWindowText("");
	m_cPriceEdit.SetWindowText("");
	m_cSilverPrice.SetWindowText("");
	//m_cXSizeEdit.SetWindowText("");
	//m_cYSizeEdit.SetWindowText("");
	m_cGoodType.SetCurSel(-1);
	m_cSoundEdit.SetWindowText("");
	//m_cWeightEdit.SetWindowText("");
	m_cDescEdit.SetWindowText("");
	m_cAttrCombo.SetCurSel(-1);
	m_cAttrCombo2.SetCurSel(-1);

	//add by wang-xx 2009-6-30 begin
	//激发属性
	m_cArouseAttrCombo.SetCurSel(-1);
	m_cArouseAttrList.DeleteAllItems();
	m_cArouseVal1.SetWindowText("");
	m_cArouseVal2.SetWindowText("");
	m_cArouseNum.SetWindowText("");
	//add by wang-xx 2009-6-30 end

	m_cVailCombo.SetCurSel(1);
	m_cHideCombo.SetCurSel(0);
	m_cAttrList.DeleteAllItems();	
	m_cAttrList2.DeleteAllItems();
	m_cValEdit1.SetWindowText("");
	m_cValEdit2.SetWindowText("");
	m_cSuitNumEdit.SetWindowText("");
	m_cSuitValEdit1.SetWindowText("");
	m_cSuitValEdit2.SetWindowText("");
	//m_cValList.DeleteAllItems();
	//m_cValSelCombo.SetCurSel(0);
	//m_cMinEdit.SetWindowText("");
	//m_cMaxEdit.SetWindowText("");
	//m_cOddsEdit.SetWindowText("");
	//m_cOddsEdit2.SetWindowText("");
	m_cSound1.SetWindowText("0");
	m_cSound2.SetWindowText("0");
	m_cSoundSwitch.SetCurSel(0);
	m_cWeaponActType.SetWindowText("");	
}

void CGoodAttrPage::FillCtrls(int nIndex)
{
	m_cAttrCombo.SetCurSel(-1);
	m_cAttrCombo2.SetCurSel(-1);
	//add by wang-xx 2009-6-30 begin
	//激发属性
	m_cArouseAttrCombo.SetCurSel(-1);
	//add by wang-xx 2009-6-30 end

	int nSize=(int)vecGoodAttr.size();
	if(nIndex<0 || nIndex>nSize-1)
		return;
	m_nVecIndex=nIndex;
	char strVal[500];
	sprintf_s(strVal,"%d",vecGoodAttr[nIndex].dwIndex);

	m_cIDLab.SetWindowText(strVal);
	m_cSaveDBCombo.SetCurSel(vecGoodAttr[nIndex].bSave?1:0);
	m_cOrigName.SetWindowText(vecGoodAttr[nIndex].strOrigName.c_str());
	m_cNameEdit.SetWindowText(vecGoodAttr[nIndex].strName.c_str());
	sprintf_s(strVal,"%d",vecGoodAttr[nIndex].dwIconId);
	m_cUIEdit.SetWindowText(strVal);
	sprintf_s(strVal,"%d",vecGoodAttr[nIndex].dwGroundId);
	m_cGroundEdit.SetWindowText(strVal);
	sprintf_s(strVal,"%d",vecGoodAttr[nIndex].dwEquipID);
	m_cCosplayEdit.SetWindowText(strVal);
	sprintf_s(strVal,"%d",vecGoodAttr[nIndex].dwValue);
	m_cPriceEdit.SetWindowText(strVal);
	sprintf_s(strVal,"%d",vecGoodAttr[nIndex].dwSilverValue);
	m_cSilverPrice.SetWindowText(strVal);
	//sprintf(strVal,"%d",vecGoodAttr[nIndex].lX_Size);
	//m_cXSizeEdit.SetWindowText(strVal);
	//sprintf(strVal,"%d",vecGoodAttr[nIndex].lY_Size);
	//m_cYSizeEdit.SetWindowText(strVal);
	m_cGoodType.SetCurSel(vecGoodAttr[nIndex].dwType);
	sprintf_s(strVal,"%d",vecGoodAttr[nIndex].dwSound);
	m_cSoundEdit.SetWindowText(strVal);

	//sprintf(strVal,"%d",vecGoodAttr[nIndex].dwWeight);
	//m_cWeightEdit.SetWindowText(strVal);

	m_cDescEdit.SetWindowText(vecGoodAttr[nIndex].strContent.c_str());

	sprintf_s(strVal,"%d",vecGoodAttr[nIndex].dwSoundID1);
	m_cSound1.SetWindowText(strVal);
	sprintf_s(strVal,"%d",vecGoodAttr[nIndex].dwSoundID2);
	m_cSound2.SetWindowText(strVal);
	m_cSoundSwitch.SetCurSel(vecGoodAttr[nIndex].bSoundSwitch?1:0);
	sprintf_s(strVal,"%d",vecGoodAttr[nIndex].dwWeaponActType);
	m_cWeaponActType.SetWindowText(strVal);
	int i=0;
	vector<tagAddAttr>::iterator iter;
	for(iter=vecGoodAttr[nIndex].vecAddAttr.begin();iter!=vecGoodAttr[nIndex].vecAddAttr.end();iter++,i++)
	{
		m_cAttrList.InsertItem(0xffff,"");
		m_cAttrList.SetItem(i,0,LVIF_TEXT,AttrNamesVec[iter->wType].c_str(),NULL,0,0,0);
		m_cAttrList.SetItem(i,1,LVIF_TEXT,iter->bEnable?"是":"否",NULL,0,0,0);
		m_cAttrList.SetItem(i,2,LVIF_TEXT,iter->bHide?"是":"否",NULL,0,0,0);
		sprintf_s(strVal,"%d",iter->lValue1);
		m_cAttrList.SetItem(i,3,LVIF_TEXT,strVal,NULL,0,0,0);
		sprintf_s(strVal,"%d",iter->lValue2);
		m_cAttrList.SetItem(i,4,LVIF_TEXT,strVal,NULL,0,0,0);

		//附加属性出现几率被屏蔽了
		//sprintf(strVal,"%d",iter->wEnableOdds);
		//m_cAttrList.SetItem(i,5,LVIF_TEXT,strVal,NULL,0,0,0);

	}

	vector<tagSuitAttr>::iterator suitAttrIter;
	i=0;
	for(suitAttrIter=vecGoodAttr[nIndex].vecSuitAttr.begin();suitAttrIter!=vecGoodAttr[nIndex].vecSuitAttr.end();suitAttrIter++,i++)
	{
		m_cAttrList2.InsertItem(0xffff,"");
		m_cAttrList2.SetItem(i,0,LVIF_TEXT,AttrNamesVec[suitAttrIter->wType].c_str(),NULL,0,0,0);

		sprintf_s(strVal,"%d",suitAttrIter->lValue1);
		m_cAttrList2.SetItem(i,1,LVIF_TEXT,strVal,NULL,0,0,0);
		sprintf_s(strVal,"%d",suitAttrIter->lValue2);
		m_cAttrList2.SetItem(i,2,LVIF_TEXT,strVal,NULL,0,0,0);
		sprintf_s(strVal,"%d",suitAttrIter->wSuitNum);
		m_cAttrList2.SetItem(i,3,LVIF_TEXT,strVal,NULL,0,0,0);

		//附加属性出现几率被屏蔽了
		//sprintf(strVal,"%d",iter->wEnableOdds);
		//m_cAttrList.SetItem(i,5,LVIF_TEXT,strVal,NULL,0,0,0);

	}

	//add by wang-xx 2009-6-30 begin
	//激发属性
	vector<tagArouseAttr>::iterator suitArouseIter;
	i=0;
	for(suitArouseIter=vecGoodAttr[nIndex].vecArouseAttr.begin();
		suitArouseIter!=vecGoodAttr[nIndex].vecArouseAttr.end();suitArouseIter++,i++)
	{
		m_cArouseAttrList.InsertItem(0xffff,"");
		m_cArouseAttrList.SetItem(i,0,LVIF_TEXT,AttrNamesVec[suitArouseIter->wType].c_str(),NULL,0,0,0);

		sprintf_s(strVal,"%d",suitArouseIter->lValue1);
		m_cArouseAttrList.SetItem(i,1,LVIF_TEXT,strVal,NULL,0,0,0);
		sprintf_s(strVal,"%d",suitArouseIter->lValue2);
		m_cArouseAttrList.SetItem(i,2,LVIF_TEXT,strVal,NULL,0,0,0);
		sprintf_s(strVal,"%d",suitArouseIter->wArouseCnt);
		m_cArouseAttrList.SetItem(i,3,LVIF_TEXT,strVal,NULL,0,0,0);
	}
	//add by wang-xx 2009-6-30 end

	m_tagGoodAttr=vecGoodAttr[nIndex];
}


void CGoodAttrPage::DoDataExchange(CDataExchange* pDX)
{
	if(!bInit)
	{
		CFormView::DoDataExchange(pDX);
		DDX_CBIndex(pDX, IDC_SAVEDBCOMBO, m_nSaveDB);
		DDX_Text(pDX, IDC_NAMEEDIT, m_strName);
		DDV_MaxChars(pDX, m_strName, 30);
		DDX_Text(pDX, IDC_UIEDIT, m_strUIID);
		DDV_MaxChars(pDX, m_strUIID, 30);
		DDX_Text(pDX, IDC_GROUNDEDIT, m_strGroundID);
		DDV_MaxChars(pDX, m_strGroundID, 30);
		DDX_Text(pDX, IDC_COSPLAYEDIT, m_strCosplayID);
		DDV_MaxChars(pDX, m_strCosplayID, 30);
		DDX_Text(pDX, IDC_PRICEEDIT, m_nPrice);
		DDV_MinMaxInt(pDX, m_nPrice, 0, 1000000000);
		DDX_Text(pDX,IDC_SILVERPRICEEDIT,m_nSilverPrice);
		DDV_MinMaxInt(pDX,m_nSilverPrice,0,1000000000);
		//DDX_Text(pDX, IDC_XSIZEEDIT, m_nXSize);
		//DDV_MinMaxInt(pDX, m_nXSize, 0, 255);
		//DDX_Text(pDX, IDC_YSIZEEDIT, m_nYSize);
		//DDV_MinMaxInt(pDX, m_nYSize, 0, 255);
		DDX_CBIndex(pDX, IDC_GOODTYPECOMBO, m_nGoodType);
		DDX_Text(pDX, IDC_SOUNDEDIT, m_strSound);
		DDV_MaxChars(pDX, m_strSound, 30);
		//DDX_Text(pDX, IDC_WEIGHTEDIT, m_nWeight);
		//DDV_MinMaxInt(pDX, m_nWeight, 0, 1000000000);
		DDX_Text(pDX, IDC_DESCEDIT, m_strDesc);
		DDV_MaxChars(pDX, m_strDesc, 5000);
		DDX_CBIndex(pDX, IDC_ATTRCOMBO, m_nAttrSel);
		DDX_CBIndex(pDX, IDC_VAILCOMBO, m_nVail);
		DDX_CBIndex(pDX, IDC_HIDECOMBO, m_nHide);
		DDX_Text(pDX, IDC_VALEDIT1, m_nVal1);
		DDV_MinMaxInt(pDX, m_nVal1, 0, 1000000000);
		DDX_Text(pDX, IDC_VALEDIT2, m_nVal2);
		DDV_MinMaxInt(pDX, m_nVal2, 0, 1000000000);
		//DDX_Text(pDX, IDC_ODDSEDIT, m_nOdds);
		//DDV_MinMaxInt(pDX, m_nOdds, 0, 100);
		//DDX_Text(pDX, IDC_MINEDIT, m_nMin);
		//DDV_MinMaxInt(pDX, m_nMin, 0, 100000000);
		//DDX_Text(pDX, IDC_MAXEDIT, m_nMax);
		//DDV_MinMaxInt(pDX, m_nMax, 0, 1000000000);
		//DDX_Text(pDX, IDC_ODDSEDIT2, m_nOdds2);
		//DDV_MinMaxInt(pDX, m_nOdds2, 0, 100);
		DDX_Control(pDX, IDC_ATTRLIST, m_cAttrList);
		DDX_Control(pDX, IDC_LIST1, m_cAttrList2);
		//DDX_Control(pDX, IDC_VALLIST, m_cValList);
		DDX_Control(pDX, IDC_ATTRCOMBO, m_cAttrCombo);
		DDX_Control(pDX, IDC_COMBO1, m_cAttrCombo2);

		//add by wang-xx 2009-6-30 begin
		//激发属性
		DDX_Control(pDX, IDC_AROUSE_COMBO, m_cArouseAttrCombo);
		DDX_Control(pDX, IDC_AROUSE_LIST, m_cArouseAttrList);
		DDX_Control(pDX, IDC_AROUSE_VALUE1, m_cArouseVal1);
		DDX_Control(pDX, IDC_AROUSE_VALUE2, m_cArouseVal2);
		DDX_Control(pDX, IDC_AROUSE_NUM, m_cArouseNum);
		//add by wang-xx 2009-6-30 end

		DDX_Control(pDX, IDC_GOODTYPECOMBO, m_cGoodType);
		DDX_Control(pDX, IDC_SAVEDBCOMBO, m_cSaveDBCombo);
		DDX_Control(pDX, IDC_HIDECOMBO, m_cHideCombo);
		DDX_Control(pDX, IDC_VAILCOMBO, m_cVailCombo);		
		DDX_Control(pDX, IDC_VALEDIT1, m_cValEdit1);
		DDX_Control(pDX, IDC_VALEDIT2, m_cValEdit2);

		DDX_Control(pDX, IDC_EDIT1, m_cSuitValEdit1);
		DDX_Control(pDX, IDC_EDIT2, m_cSuitValEdit2);
		DDX_Control(pDX, IDC_EDIT3, m_cSuitNumEdit);
		//DDX_Control(pDX, IDC_VALSELCOMBO, m_cValSelCombo);	
		//DDX_CBIndex(pDX, IDC_VALSELCOMBO, m_nValSel);		
		DDX_Control(pDX, IDC_GOODIDLAB, m_cIDLab);	
		DDX_Control(pDX, IDC_NAMEEDIT, m_cNameEdit);
		DDX_Control(pDX, IDC_UIEDIT, m_cUIEdit);
		DDX_Control(pDX, IDC_GROUNDEDIT, m_cGroundEdit);
		DDX_Control(pDX, IDC_COSPLAYEDIT, m_cCosplayEdit);
		DDX_Control(pDX, IDC_PRICEEDIT, m_cPriceEdit);
		DDX_Control(pDX, IDC_SILVERPRICEEDIT, m_cSilverPrice);
		//DDX_Control(pDX, IDC_XSIZEEDIT, m_cXSizeEdit);
		//DDX_Control(pDX, IDC_YSIZEEDIT, m_cYSizeEdit);
		DDX_Control(pDX, IDC_SOUNDEDIT, m_cSoundEdit);
		//DDX_Control(pDX, IDC_WEIGHTEDIT, m_cWeightEdit);
		DDX_Control(pDX, IDC_DESCEDIT, m_cDescEdit);
		//DDX_Control(pDX, IDC_MINEDIT, m_cMinEdit);
		//DDX_Control(pDX, IDC_MAXEDIT, m_cMaxEdit);
		//DDX_Control(pDX, IDC_ODDSEDIT, m_cOddsEdit);
		//DDX_Control(pDX, IDC_ODDSEDIT2, m_cOddsEdit2);
		//DDX_Control(pDX, IDC_ADDBUTTON, m_cAddBtn);
		//DDX_Control(pDX, IDC_DELBUTTON, m_cDelBtn);
		DDX_Control(pDX, IDC_DELATTRBTN, m_cDelAttrBtn);
		DDX_Control(pDX, IDC_NAMEEDIT2, m_cOrigName);
		InitMyCtrls();
		bInit=true;
	}	


	DDX_Control(pDX, IDC_SOUND1, m_cSound1);
	DDX_Control(pDX, IDC_SOUND2, m_cSound2);
	DDX_Control(pDX, IDC_SOUNDSWITCH, m_cSoundSwitch);
	//DDX_Control(pDX, IDC_WEAPON_ACTTYPE, m_cWeaponActType);
	DDX_Text(pDX, IDC_SOUND1, m_lSoundID1);
	DDX_Text(pDX, IDC_SOUND2, m_lSoundID2);
	DDX_CBIndex(pDX, IDC_SOUNDSWITCH, m_bSoundSwitch);
	//DDX_Text(pDX, IDC_WEAPON_ACTTYPE, m_lWeaponActType);
	DDX_Control(pDX, IDC_SAVEASBUTTON, m_cSaveAsButton);
	DDX_Control(pDX, IDC_WEAPONACTEDIT, m_cWeaponActType);
	DDX_Text(pDX, IDC_WEAPONACTEDIT, m_lWeaponActType);	


}

BEGIN_MESSAGE_MAP(CGoodAttrPage, CFormView)
	ON_CBN_SELCHANGE(IDC_GOODTYPECOMBO, OnCbnSelchangeTypecombo)
	ON_CBN_SELCHANGE(IDC_ATTRCOMBO, OnCbnSelchangeAttrcombo)
	ON_CBN_SELCHANGE(IDC_VAILCOMBO, OnCbnSelchangeVailcombo)
	ON_CBN_SELCHANGE(IDC_HIDECOMBO, OnCbnSelchangeHidecombo)
	ON_EN_CHANGE(IDC_VALEDIT1, OnEnChangeValedit1)
	ON_EN_CHANGE(IDC_VALEDIT2, OnEnChangeValedit2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ATTRLIST, OnLvnItemchangedAttrlist)
	ON_CBN_SELCHANGE(IDC_SAVEDBCOMBO, OnCbnSelchangeSavedbcombo)
	ON_EN_CHANGE(IDC_NAMEEDIT, OnEnChangeNameedit)
	ON_EN_CHANGE(IDC_UIEDIT, OnEnChangeUiedit)
	ON_EN_CHANGE(IDC_GROUNDEDIT, OnEnChangeGroundedit)
	ON_EN_CHANGE(IDC_COSPLAYEDIT, OnEnChangeCosplayedit)
	ON_EN_CHANGE(IDC_PRICEEDIT, OnEnChangePriceedit)
	//ON_EN_CHANGE(IDC_XSIZEEDIT, OnEnChangeXsizeedit)
	//ON_EN_CHANGE(IDC_YSIZEEDIT, OnEnChangeYsizeedit)
	ON_EN_CHANGE(IDC_SOUNDEDIT, OnEnChangeSoundedit)
	//ON_EN_CHANGE(IDC_WEIGHTEDIT, OnEnChangeWeightedit)
	ON_EN_CHANGE(IDC_DESCEDIT, OnEnChangeDescedit)
	//ON_CBN_SELCHANGE(IDC_VALSELCOMBO, OnCbnSelchangeValselcombo)
	//ON_BN_CLICKED(IDC_ADDBUTTON, OnBnClickedAddbutton)	
	//ON_EN_CHANGE(IDC_MINEDIT, OnEnChangeMinedit)
	//ON_EN_CHANGE(IDC_MAXEDIT, OnEnChangeMaxedit)
	//ON_EN_CHANGE(IDC_ODDSEDIT, OnEnChangeOddsedit)
	//ON_EN_CHANGE(IDC_ODDSEDIT2, OnEnChangeOddsedit2)
	ON_BN_CLICKED(IDC_DELATTRBTN, OnBnClickedDelattrbtn)
	//ON_BN_CLICKED(IDC_DELBUTTON, OnBnClickedDelbutton)
	ON_EN_CHANGE(IDC_NAMEEDIT2, OnEnChangeNameedit2)
	ON_EN_CHANGE(IDC_GOODIDLAB, OnEnChangeGoodidlab)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_EN_CHANGE(IDC_SOUND1, OnEnChangeSound1)
	ON_EN_CHANGE(IDC_SOUND2, OnEnChangeSound2)
	ON_CBN_SELCHANGE(IDC_SOUNDSWITCH, OnCbnSelchangeSoundswitch)
	//ON_EN_CHANGE(IDC_WEAPON_ACTTYPE, OnEnChangeWeaponActtype)
	ON_EN_KILLFOCUS(IDC_NAMEEDIT2, OnEnKillfocusNameedit2)
	ON_BN_CLICKED(IDC_SAVEASBUTTON, OnBnClickedSaveasbutton)
	//ON_NOTIFY(LVN_ITEMCHANGED, IDC_VALLIST, OnLvnItemchangedVallist)
	ON_BN_CLICKED(IDC_COPYATTRBTN, OnBnClickedCopyattrbtn)
	ON_BN_CLICKED(IDC_PASTEATTRBTN, OnBnClickedPasteattrbtn)
	ON_BN_CLICKED(IDC_DELATTRBTN2, OnBnClickedDelattrbtn2)
	ON_BN_CLICKED(IDC_DELATTRBTN3, OnBnClickedDelattrbtn3)
	ON_COMMAND(ID_COUNT_EQUIP, OnCountEquip)
	ON_BN_CLICKED(IDC_COUNTLEVELLIMET, OnBnClickedCountlevellimet)
	ON_EN_CHANGE(IDC_WEAPONACTEDIT, &CGoodAttrPage::OnEnChangeWeaponactedit)
	ON_BN_CLICKED(IDC_BUTTON1, &CGoodAttrPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, &CGoodAttrPage::OnBnClickedButton4)
	ON_EN_CHANGE(IDC_EDIT1, &CGoodAttrPage::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &CGoodAttrPage::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &CGoodAttrPage::OnEnChangeEdit3)

	//add by wang-xx 2009-6-30 begin
	//激发属性
	ON_EN_CHANGE(IDC_AROUSE_VALUE1, &CGoodAttrPage::OnEnChangeArouseVal1)
	ON_EN_CHANGE(IDC_AROUSE_VALUE2, &CGoodAttrPage::OnEnChangeArouseVal2)
	ON_EN_CHANGE(IDC_AROUSE_NUM, &CGoodAttrPage::OnEnChangeArouseNum)
	ON_NOTIFY(NM_CLICK, IDC_AROUSE_LIST, &CGoodAttrPage::OnNMClickArouseList)
	//add by wang-xx 2009-6-30 end

	//	ON_NOTIFY(HDN_ITEMCLICK, 0, &CGoodAttrPage::OnHdnItemclickList1)
	//	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &CGoodAttrPage::OnLvnColumnclickList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CGoodAttrPage::OnNMClickList1)
	ON_EN_CHANGE(IDC_SILVERPRICEEDIT, &CGoodAttrPage::OnEnChangeSilverPrice)
	ON_BN_CLICKED(IDC_AROUSE_ADD, &CGoodAttrPage::OnBnClickedArouseAdd)
	ON_BN_CLICKED(IDC_AROUSE_DEL, &CGoodAttrPage::OnBnClickedArouseDel)
END_MESSAGE_MAP()


// CGoodAttrPage 诊断

#ifdef _DEBUG
void CGoodAttrPage::AssertValid() const
{
	CFormView::AssertValid();
}

void CGoodAttrPage::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

void CGoodAttrPage::SetGoodAttrTag(int nIndex)
{
	int nSize=(int)vecGoodAttr.size();
	if(nIndex<0 || nIndex>=nSize)
		return;
	m_tagGoodAttr=vecGoodAttr[nIndex];
}


// CGoodAttrPage 消息处理程序

void CGoodAttrPage::OnCbnSelchangeTypecombo()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_bEdit)
	{
		int nSelected=m_cGoodType.GetCurSel();
		vecGoodAttr[m_nVecIndex].dwType=nSelected;
		CContainerPage *pContainer=(CContainerPage*)GetParent();		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,9,const_cast<char*>(strGoodType[nSelected]));
	}

}

void CGoodAttrPage::OnCbnSelchangeAttrcombo()
{
	//// TODO: 在此添加控件通知处理程序代码
	//static int nSel=-1;
	//int nCurSel=m_cAttrCombo.GetCurSel();
	////if(nCurSel==nSel) //选择的ITEM不变，不做处理
	////	return;	
	///*
	//int nItemCnt=m_cAttrList.GetItemCount();
	//if(nItemCnt)
	//{
	//	char strTem[500];
	//	for(int i=0;i<nItemCnt;i++)
	//	{
	//		m_cAttrList.GetItemText(i,0,strTem,500);
	//		if(AttrNamesVec[nCurSel]==strTem)//已经添加过
	//		{							
	//			return;
	//		}
	//	}
	//}
	//*/
	//nSel=nCurSel;
	//tagAddAttr stAddAttr;
	//stAddAttr.wType=nSel;
	//stAddAttr.bEnable=true;
	//stAddAttr.bHide=false;
	//stAddAttr.lValue1=0;
	//stAddAttr.lValue2=0;
	//stAddAttr.wEnableOdds=10000;
	//   if(m_bEdit)
	//{
	//	vecGoodAttr[m_nVecIndex].vecAddAttr.push_back(stAddAttr);
	//	m_tagGoodAttr.vecAddAttr.push_back(stAddAttr);
	//}
	//else if(m_bAdd)
	//{		
	//	m_tagGoodAttr.vecAddAttr.push_back(stAddAttr);
	//}
	//int nIndex=m_cAttrList.InsertItem(0xffff,"");
	//m_cAttrList.SetItem(nIndex,0,LVIF_TEXT,AttrNamesVec[nCurSel].c_str(),NULL,0,0,0);
	//m_cAttrList.SetItem(nIndex,1,LVIF_TEXT,m_cVailCombo.GetCurSel()==1?"是":"否",NULL,0,0,0);
	//m_cAttrList.SetItem(nIndex,2,LVIF_TEXT,m_cHideCombo.GetCurSel()==1?"是":"否",NULL,0,0,0);
	//m_cAttrList.SetItem(nIndex,3,LVIF_TEXT,"0",NULL,0,0,0);
	//m_cAttrList.SetItem(nIndex,4,LVIF_TEXT,"0",NULL,0,0,0);
	//m_cAttrList.SetItem(nIndex,5,LVIF_TEXT,"10000",NULL,0,0,0);
	//m_cAttrList.SetItemState(nIndex,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);		
}

void CGoodAttrPage::OnCbnSelchangeVailcombo()
{
	// TODO: 在此添加控件通知处理程序代码
	/*
	*	更改选中的LIST ITEM
	*/
	POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nSelected=m_cAttrList.GetNextSelectedItem(pos);
		int nCurSel=m_cVailCombo.GetCurSel();
		if(m_bEdit)
		{	
			vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].bEnable=nCurSel==1?true:false;
			m_tagGoodAttr.vecAddAttr[nSelected].bEnable=nCurSel==1?true:false;
		}
		else if(m_bAdd)
		{
			m_tagGoodAttr.vecAddAttr[nSelected].bEnable=nCurSel==1?true:false;
		}
		m_cAttrList.SetItemText(nSelected,1,nCurSel==1?"是":"否");
	}
}

void CGoodAttrPage::OnCbnSelchangeHidecombo()
{
	// TODO: 在此添加控件通知处理程序代码
	/*
	*	更改选中的LIST ITEM
	*/
	POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nSelected=m_cAttrList.GetNextSelectedItem(pos);
		int nCurSel=m_cHideCombo.GetCurSel();
		if(m_bEdit)
		{
			vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].bHide=nCurSel==1?true:false;
			m_tagGoodAttr.vecAddAttr[nSelected].bHide=nCurSel==1?true:false;
		}
		else if(m_bAdd)
		{
			m_tagGoodAttr.vecAddAttr[nSelected].bHide=nCurSel==1?true:false;
		}
		m_cAttrList.SetItemText(nSelected,2,nCurSel==1?"是":"否");
	}
}

void CGoodAttrPage::OnEnChangeValedit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nSelected=m_cAttrList.GetNextSelectedItem(pos);
		char strVal[500];
		m_cValEdit1.GetWindowText(strVal,500);
		DWORD nVal=0;
		nVal = (DWORD)_atoi64(strVal);
		DWORD nn=-8126209;

		if(m_bEdit)
		{
			vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].lValue1=nVal;
			m_tagGoodAttr.vecAddAttr[nSelected].lValue1=nVal;
		}
		else if(m_bAdd)
			m_tagGoodAttr.vecAddAttr[nSelected].lValue1=nVal;
		m_cAttrList.SetItemText(nSelected,3,strVal);
	}
}

void CGoodAttrPage::OnEnChangeEdit1()
{	
	POSITION pos=m_cAttrList2.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nSelected=m_cAttrList2.GetNextSelectedItem(pos);
		char strVal[500];
		m_cSuitValEdit1.GetWindowText(strVal,500);
		int nVal=atoi(strVal);
		if(m_bEdit)
		{
			vecGoodAttr[m_nVecIndex].vecSuitAttr[nSelected].lValue1=nVal;
			m_tagGoodAttr.vecSuitAttr[nSelected].lValue1=nVal;
		}
		else if(m_bAdd)
			m_tagGoodAttr.vecSuitAttr[nSelected].lValue1=nVal;
		m_cAttrList2.SetItemText(nSelected,1,strVal);
	}
}

void CGoodAttrPage::OnEnChangeEdit2()
{
	POSITION pos=m_cAttrList2.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nSelected=m_cAttrList2.GetNextSelectedItem(pos);
		char strVal[500];
		m_cSuitValEdit2.GetWindowText(strVal,500);
		int nVal=atoi(strVal);
		if(m_bEdit)
		{
			vecGoodAttr[m_nVecIndex].vecSuitAttr[nSelected].lValue2=nVal;
			m_tagGoodAttr.vecSuitAttr[nSelected].lValue2=nVal;
		}
		else if(m_bAdd)
			m_tagGoodAttr.vecSuitAttr[nSelected].lValue2=nVal;
		m_cAttrList2.SetItemText(nSelected,2,strVal);
	}
}

void CGoodAttrPage::OnEnChangeEdit3()
{
	POSITION pos=m_cAttrList2.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nSelected=m_cAttrList2.GetNextSelectedItem(pos);
		char strVal[500];
		m_cSuitNumEdit.GetWindowText(strVal,500);
		int nVal=atoi(strVal);
		if(m_bEdit)
		{
			vecGoodAttr[m_nVecIndex].vecSuitAttr[nSelected].wSuitNum=nVal;
			m_tagGoodAttr.vecSuitAttr[nSelected].wSuitNum=nVal;
		}
		else if(m_bAdd)
			m_tagGoodAttr.vecSuitAttr[nSelected].wSuitNum=nVal;
		m_cAttrList2.SetItemText(nSelected,3,strVal);
	}
}



void CGoodAttrPage::OnEnChangeValedit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nSelected=m_cAttrList.GetNextSelectedItem(pos);
		char strVal[500];
		m_cValEdit2.GetWindowText(strVal,500);
		DWORD nVal=0;
		nVal = (DWORD)_atoi64(strVal);
		if(m_bEdit)
		{
			vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].lValue2=nVal;
			m_tagGoodAttr.vecAddAttr[nSelected].lValue2=nVal;
		}
		else if(m_bAdd)
			m_tagGoodAttr.vecAddAttr[nSelected].lValue2=nVal;
		m_cAttrList.SetItemText(nSelected,4,strVal);
	}
}
/*
//选中列表项目的时候改变编辑控件的值
void CGoodAttrPage::OnLvnItemchangedVallist(NMHDR *pNMHDR, LRESULT *pResult)
{
LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
// TODO: 在此添加控件通知处理程序代码
if(pNMLV->uNewState & LVIS_SELECTED)
{
POSITION pos=m_cValList.GetFirstSelectedItemPosition();
if(pos)
{
int nSelected=m_cValList.GetNextSelectedItem(pos);
LVITEM lvitem;
ZeroMemory(&lvitem,sizeof(lvitem));
lvitem.iItem=nSelected;
lvitem.iSubItem=0;
lvitem.mask=LVIF_PARAM|LVIF_TEXT;
m_cValList.GetItem(&lvitem);
int nWhichVal=lvitem.lParam;
m_cValSelCombo.SetCurSel(nWhichVal);
char strVal[256];
m_cValList.GetItemText(nSelected,1,strVal,256);
m_cMinEdit.SetWindowText(strVal);
m_cValList.GetItemText(nSelected,2,strVal,256);
m_cMaxEdit.SetWindowText(strVal);
m_cValList.GetItemText(nSelected,3,strVal,256);
m_cOddsEdit.SetWindowText(strVal);

}
}
*pResult = 0;
}*/

//选中附加值列表项目时，改变相应编辑控件的值
void CGoodAttrPage::OnLvnItemchangedAttrlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	m_cAttrList.SetHotItem(-1);
	if(pNMLV->uNewState & LVIS_SELECTED)
	{
		POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
		//删除所有附加修正项目
		//m_cValList.DeleteAllItems();
		if(pos)
		{
			int nSelected=m_cAttrList.GetNextSelectedItem(pos);
			char strName[500],strVal1[500],strVal2[500],strVail[10],strHide[10],strEnableOdds[10];
			m_cAttrList.GetItemText(nSelected,0,strName,500);
			m_cAttrList.GetItemText(nSelected,1,strVail,10);
			m_cAttrList.GetItemText(nSelected,2,strHide,10);
			m_cAttrList.GetItemText(nSelected,3,strVal1,500);
			m_cAttrList.GetItemText(nSelected,4,strVal2,500);
			m_cAttrList.GetItemText(nSelected,5,strEnableOdds,10);
			m_cValEdit1.SetWindowText(strVal1);
			m_cValEdit2.SetWindowText(strVal2);
			//m_cOddsEdit2.SetWindowText(strEnableOdds);
			if(strcmp(strVail,"是")==0)
				m_cVailCombo.SetCurSel(1);
			else
				m_cVailCombo.SetCurSel(0);
			if(strcmp(strHide,"是")==0)
				m_cHideCombo.SetCurSel(1);
			else
				m_cHideCombo.SetCurSel(0);
			/*
			//填相应附加修正控件的值
			vector<tagModify>::iterator iter;
			int i=0;
			char strVal[500];
			for(iter=m_tagGoodAttr.vecAddAttr[nSelected].vecModify.begin();iter!=m_tagGoodAttr.vecAddAttr[nSelected].vecModify.end();iter++,i++)
			{
			m_cValList.InsertItem(0xffff,"");
			if(iter->lFixID==0)				    		
			m_cValList.SetItem(i,0,LVIF_TEXT|LVIF_PARAM,strVal1,NULL,0,0,0);
			else if(iter->lFixID==1)
			m_cValList.SetItem(i,0,LVIF_TEXT|LVIF_PARAM,strVal2,NULL,0,0,1);
			sprintf(strVal,"%d",iter->dwMin);
			m_cValList.SetItem(i,1,LVIF_TEXT,strVal,NULL,0,0,0);
			sprintf(strVal,"%d",iter->dwMax);
			m_cValList.SetItem(i,2,LVIF_TEXT,strVal,NULL,0,0,0);
			sprintf(strVal,"%d",iter->wOdds);				
			m_cValList.SetItem(i,3,LVIF_TEXT,strVal,NULL,0,0,0);				
			}*/
		}
	}
}

void CGoodAttrPage::OnCbnSelchangeSavedbcombo()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_tagGoodAttr.bSaveDB=nSelected?true:false;
	if(m_bEdit)
	{
		int nSelected=m_cSaveDBCombo.GetCurSel();	
		vecGoodAttr[m_nVecIndex].bSave=nSelected?true:false;
		CContainerPage *pContainer=(CContainerPage*)GetParent();		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,3,nSelected==1?"是":"否");
	}	
}

void CGoodAttrPage::OnCbnSelchangeSoundswitch()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_bEdit)
	{
		int nSelected=m_cSoundSwitch.GetCurSel();
		vecGoodAttr[m_nVecIndex].bSoundSwitch=nSelected?true:false;
		CContainerPage *pContainer=(CContainerPage*)GetParent();		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,14,nSelected==1?"是":"否");
	}
}

void CGoodAttrPage::OnEnChangeGoodidlab()
{
}

void CGoodAttrPage::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	char strID[500];
	m_cIDLab.GetWindowText(strID,500);
	int nid=atoi(strID);
	vector<tagGoodAttr>::iterator iter;
	for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++)
	{
		if(iter->dwIndex==nid)
		{
			MessageBox("物品ID有重复,无法更新");
			return;
		}
	}
	DWORD dwOldIndex=vecGoodAttr[m_nVecIndex].dwIndex;
	vecGoodAttr[m_nVecIndex].dwIndex=nid;

	char strNewIndex[32];

	sprintf_s(strNewIndex,"%d",nid);
	CContainerPage *pContainer=(CContainerPage*)GetParent();

	pContainer->m_wndAllGoodsPage.ChangeItemText(dwOldIndex,0,strNewIndex);
}



void CGoodAttrPage::OnEnChangeNameedit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	// TODO:  在此添加控件通知处理程序代码
	CContainerPage *pContainer=(CContainerPage*)GetParent();	
	if(m_bEdit)
	{
		char strName[500];
		m_cNameEdit.GetWindowText(strName,500);
		vecGoodAttr[m_nVecIndex].strName=strName;		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,2,strName);
	}	
}

void CGoodAttrPage::OnEnChangeUiedit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	// TODO:  在此添加控件通知处理程序代码
	if(m_bEdit)
	{
		char strUIID[500];
		m_cUIEdit.GetWindowText(strUIID,500);
		vecGoodAttr[m_nVecIndex].dwIconId=atoi(strUIID);
		CContainerPage *pContainer=(CContainerPage*)GetParent();		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,4,strUIID);
	}	
}

void CGoodAttrPage::OnEnChangeGroundedit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if(m_bEdit)
	{
		char strGroundID[500];
		m_cGroundEdit.GetWindowText(strGroundID,500);
		vecGoodAttr[m_nVecIndex].dwGroundId=atoi(strGroundID);
		CContainerPage *pContainer=(CContainerPage*)GetParent();		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,5,strGroundID);
	}
}

void CGoodAttrPage::OnEnChangeCosplayedit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	// TODO:  在此添加控件通知处理程序代码
	if(m_bEdit)
	{
		char strCosplayID[500];
		m_cCosplayEdit.GetWindowText(strCosplayID,500);
		vecGoodAttr[m_nVecIndex].dwEquipID=atoi(strCosplayID);
		CContainerPage *pContainer=(CContainerPage*)GetParent();		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,6,strCosplayID);
	}	
}

void CGoodAttrPage::OnEnChangePriceedit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if(m_bEdit)
	{
		char strPrice[500];
		m_cPriceEdit.GetWindowText(strPrice,500);
		vecGoodAttr[m_nVecIndex].dwValue=atoi(strPrice);
		CContainerPage *pContainer=(CContainerPage*)GetParent();		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,7,strPrice);
		//m_cSilverPrice.SetWindowText("0");
	}
}

void CGoodAttrPage::OnEnChangeSilverPrice()
{
	if(m_bEdit)
	{
		char strSilverPrice[500];
		m_cSilverPrice.GetWindowText(strSilverPrice,500);
		vecGoodAttr[m_nVecIndex].dwSilverValue=atoi(strSilverPrice);
		CContainerPage* pContainer=(CContainerPage*)GetParent();
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,8,strSilverPrice);
		//m_cPriceEdit.SetWindowText("0");
	}
}
/*
void CGoodAttrPage::OnEnChangeXsizeedit()
{
// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
// 发送该通知，除非重写 CFormView::OnInitDialog()
// 函数并调用 CRichEditCtrl().SetEventMask()，
// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

// TODO:  在此添加控件通知处理程序代码
if(m_bEdit)
{
char strXSize[500];
m_cXSizeEdit.GetWindowText(strXSize,500);
vecGoodAttr[m_nVecIndex].lX_Size=atoi(strXSize);
CContainerPage *pContainer=(CContainerPage*)GetParent();		
pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,8,strXSize);
}	
}*/
/*
void CGoodAttrPage::OnEnChangeYsizeedit()
{
// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
// 发送该通知，除非重写 CFormView::OnInitDialog()
// 函数并调用 CRichEditCtrl().SetEventMask()，
// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

// TODO:  在此添加控件通知处理程序代码
if(m_bEdit)
{
char strYSize[500];
m_cYSizeEdit.GetWindowText(strYSize,500);
vecGoodAttr[m_nVecIndex].lY_Size=atoi(strYSize);
CContainerPage *pContainer=(CContainerPage*)GetParent();		
pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,9,strYSize);
}	
}*/


void CGoodAttrPage::OnEnChangeSoundedit()
{
	if(m_bEdit)
	{
		char strSound[500];
		m_cSoundEdit.GetWindowText(strSound,500);
		vecGoodAttr[m_nVecIndex].dwSound=atoi(strSound);
		CContainerPage *pContainer=(CContainerPage*)GetParent();		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,10,strSound);
	}	
}

void CGoodAttrPage::OnEnChangeSound1()
{
	if(m_bEdit)
	{
		char strSound[500];
		m_cSound1.GetWindowText(strSound,500);
		vecGoodAttr[m_nVecIndex].dwSoundID1=atoi(strSound);
		CContainerPage *pContainer=(CContainerPage*)GetParent();		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,12,strSound);
	}
}

void CGoodAttrPage::OnEnChangeSound2()
{
	if(m_bEdit)
	{
		char strSound[500];
		m_cSound2.GetWindowText(strSound,500);
		vecGoodAttr[m_nVecIndex].dwSoundID2=atoi(strSound);
		CContainerPage *pContainer=(CContainerPage*)GetParent();		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,13,strSound);
	}
}

void CGoodAttrPage::OnEnChangeWeaponactedit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if(m_bEdit)
	{
		char strVal[500];
		m_cWeaponActType.GetWindowText(strVal,500);
		vecGoodAttr[m_nVecIndex].dwWeaponActType=atoi(strVal);
		CContainerPage *pContainer=(CContainerPage*)GetParent();		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,17,strVal);
	}
}


/*
void CGoodAttrPage::OnEnChangeWeaponActtype()
{
if(m_bEdit)
{
char strVal[500];
m_cWeaponActType.GetWindowText(strVal,500);
vecGoodAttr[m_nVecIndex].dwWeaponActType=atoi(strVal);
CContainerPage *pContainer=(CContainerPage*)GetParent();		
pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,17,strVal);
}
}*/

/*
void CGoodAttrPage::OnEnChangeWeightedit()
{
// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
// 发送该通知，除非重写 CFormView::OnInitDialog()
// 函数并调用 CRichEditCtrl().SetEventMask()，
// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

// TODO:  在此添加控件通知处理程序代码
if(m_bEdit)
{
char strWeight[500];
m_cWeightEdit.GetWindowText(strWeight,500);
vecGoodAttr[m_nVecIndex].dwWeight=atoi(strWeight);
CContainerPage *pContainer=(CContainerPage*)GetParent();		
pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,12,strWeight);
}	
}*/

void CGoodAttrPage::OnEnChangeDescedit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if(m_bEdit)
	{
		char strDesc[5000];
		m_cDescEdit.GetWindowText(strDesc,5000);
		vecGoodAttr[m_nVecIndex].strContent=strDesc;
		CContainerPage *pContainer=(CContainerPage*)GetParent();		
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,11,strDesc);
	}
}

/*
void CGoodAttrPage::OnCbnSelchangeValselcombo()
{
// TODO: 在此添加控件通知处理程序代码
POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
if(pos)
{	
int nCurSel=m_cValSelCombo.GetCurSel();
int nSelected=m_cAttrList.GetNextSelectedItem(pos);
pos=m_cValList.GetFirstSelectedItemPosition();
if(pos)
{
int nSelected2 =m_cValList.GetNextSelectedItem(pos);
if(m_bEdit)
{
vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].vecModify[nSelected2].lFixID=nCurSel;
m_tagGoodAttr.vecAddAttr[nSelected].vecModify[nSelected2].lFixID=nCurSel;
}
else if(m_bAdd)
{
m_tagGoodAttr.vecAddAttr[nSelected].vecModify[nSelected2].lFixID=nCurSel;
}
char strVal[500];
m_cAttrList.GetItemText(nSelected,3+nCurSel,strVal,500);
//m_cValList.SetItemText(nSelected2,0,strVal);
m_cValList.SetItem(nSelected2,0,LVIF_TEXT|LVIF_PARAM,strVal,NULL,0,0,nCurSel);
}
}
}*/
/*
void CGoodAttrPage::OnEnChangeMinedit()
{
// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
// 发送该通知，除非重写 CFormView::OnInitDialog()
// 函数并调用 CRichEditCtrl().SetEventMask()，
// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

// TODO:  在此添加控件通知处理程序代码
POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
if(pos)
{
int nSelected=m_cAttrList.GetNextSelectedItem(pos);
pos=m_cValList.GetFirstSelectedItemPosition();
if(pos)
{
int nSelected2=m_cValList.GetNextSelectedItem(pos);
char strVal[500];
int nVal=0;
if(m_bEdit)
{
m_cMinEdit.GetWindowText(strVal,500);
nVal=atoi(strVal);
vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].vecModify[nSelected2].dwMin=nVal;
m_tagGoodAttr.vecAddAttr[nSelected].vecModify[nSelected2].dwMin=nVal;
}
else if(m_bAdd)
{
m_cMinEdit.GetWindowText(strVal,500);
nVal=atoi(strVal);
m_tagGoodAttr.vecAddAttr[nSelected].vecModify[nSelected2].dwMin=nVal;
}
m_cValList.SetItemText(nSelected2,1,strVal);
}
}
}*/
/*
void CGoodAttrPage::OnEnChangeMaxedit()
{	
POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
if(pos)
{
int nSelected=m_cAttrList.GetNextSelectedItem(pos);
pos=m_cValList.GetFirstSelectedItemPosition();
if(pos)
{
int nSelected2=m_cValList.GetNextSelectedItem(pos);
char strVal[500];
int nVal=0;
if(m_bEdit)
{
m_cMaxEdit.GetWindowText(strVal,500);
nVal=atoi(strVal);
vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].vecModify[nSelected2].dwMax=nVal;
m_tagGoodAttr.vecAddAttr[nSelected].vecModify[nSelected2].dwMax=nVal;
}
else if(m_bAdd)
{
m_cMaxEdit.GetWindowText(strVal,500);
nVal=atoi(strVal);
m_tagGoodAttr.vecAddAttr[nSelected].vecModify[nSelected2].dwMax=nVal;
}
m_cValList.SetItemText(nSelected2,2,strVal);
}
}
}*/
/*
void CGoodAttrPage::OnEnChangeOddsedit()
{
POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
if(pos)
{
int nSelected=m_cAttrList.GetNextSelectedItem(pos);
pos=m_cValList.GetFirstSelectedItemPosition();
if(pos)
{
int nSelected2=m_cValList.GetNextSelectedItem(pos);
char strVal[500];
int nVal=0;
if(m_bEdit)
{
m_cOddsEdit.GetWindowText(strVal,500);
nVal=atoi(strVal);
vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].vecModify[nSelected2].wOdds=nVal;
m_tagGoodAttr.vecAddAttr[nSelected].vecModify[nSelected2].wOdds=nVal;
}
else if(m_bAdd)
{
m_cOddsEdit.GetWindowText(strVal,500);
nVal=atoi(strVal);
m_tagGoodAttr.vecAddAttr[nSelected].vecModify[nSelected2].wOdds=nVal;
}
m_cValList.SetItemText(nSelected2,3,strVal);
}
}
}*/
/*
void CGoodAttrPage::OnEnChangeOddsedit2()
{
POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
if(pos)
{
int nSelected=m_cAttrList.GetNextSelectedItem(pos);

char strVal[500];
int nVal=0;
if(m_bEdit)
{
m_cOddsEdit2.GetWindowText(strVal,500);
nVal=atoi(strVal);
vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].wEnableOdds=nVal;
m_tagGoodAttr.vecAddAttr[nSelected].wEnableOdds=nVal;
}
else if(m_bAdd)
{
m_cOddsEdit2.GetWindowText(strVal,500);
nVal=atoi(strVal);
m_tagGoodAttr.vecAddAttr[nSelected].wEnableOdds=nVal;
}
m_cAttrList.SetItemText(nSelected,5,strVal);
}

}*/

void CGoodAttrPage::OnEnKillfocusNameedit2()
{
	// TODO: 在此添加控件通知处理程序代码
	/*
	*原始名是否有重复的检测
	*/
	char strOrigName[256];
	m_cIDLab.GetWindowText(strOrigName,256);
	DWORD dwID=atoi(strOrigName);
	m_cOrigName.GetWindowText(strOrigName,256);
	vector<tagGoodAttr>::iterator it=vecGoodAttr.begin();
	for(;it!=vecGoodAttr.end();it++)
	{
		if(strcmp(it->strOrigName.c_str(),strOrigName)==0 && it->dwIndex!=dwID)
		{
			MessageBox("原始名已经存在","警告",MB_OK);
			m_cOrigName.SetFocus();
		}
	}
}

void CGoodAttrPage::OnEnChangeNameedit2()
{	
	CContainerPage* pContainer=(CContainerPage*)GetParent();
	CSplitterWnd* pSplitter=(CSplitterWnd*)pContainer->GetParent();
	CAllGoodsTree* pGoodsTree=(CAllGoodsTree*)pSplitter->GetPane(0,0);
	HTREEITEM selectedItem=pGoodsTree->m_cGoodsTree.GetSelectedItem();
	if(m_bEdit)
	{
		char strName[500];
		CContainerPage *pContainer=(CContainerPage*)GetParent();
		m_cOrigName.GetWindowText(strName,500);
		vecGoodAttr[m_nVecIndex].strOrigName=strName;
		pGoodsTree->m_cGoodsTree.SetItemText(selectedItem,strName);
		pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,1,strName);
	}
	else if(m_bAdd)
	{
		if(selectedItem!=NULL)
		{
			char strName[500];
			m_cOrigName.GetWindowText(strName,500);
			pGoodsTree->m_cGoodsTree.SetItemText(selectedItem,strName);
		}
	}	
}

void CGoodAttrPage::OnBnClickedDelattrbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
	int nSel=0;
	if(pos)
	{
		nSel=m_cAttrList.GetNextSelectedItem(pos);
		vector<tagAddAttr>::iterator iter=vecGoodAttr[m_nVecIndex].vecAddAttr.begin();
		vector<tagAddAttr>::iterator iter2=m_tagGoodAttr.vecAddAttr.begin();
		iter+=nSel;
		iter2+=nSel;
		if(m_bEdit)
		{			
			vecGoodAttr[m_nVecIndex].vecAddAttr.erase(iter);			
			m_tagGoodAttr.vecAddAttr.erase(iter2);			
		}
		else if(m_bAdd)
		{			
			m_tagGoodAttr.vecAddAttr.erase(iter2);			
		}
		m_cAttrList.DeleteItem(nSel);
		//m_cValList.DeleteAllItems();
	}
}

void CGoodAttrPage::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码

	POSITION pos=m_cAttrList2.GetFirstSelectedItemPosition();
	int nSel=0;
	if(pos)
	{
		nSel=m_cAttrList2.GetNextSelectedItem(pos);
		vector<tagSuitAttr>::iterator iter=vecGoodAttr[m_nVecIndex].vecSuitAttr.begin();
		vector<tagSuitAttr>::iterator iter2=m_tagGoodAttr.vecSuitAttr.begin();
		iter+=nSel;
		iter2+=nSel;
		if(m_bEdit)
		{			
			vecGoodAttr[m_nVecIndex].vecSuitAttr.erase(iter);			
			m_tagGoodAttr.vecSuitAttr.erase(iter2);			
		}
		else if(m_bAdd)
		{			
			m_tagGoodAttr.vecSuitAttr.erase(iter2);			
		}
		m_cAttrList2.DeleteItem(nSel);
		//m_cValList.DeleteAllItems();
	}
}

/*
void CGoodAttrPage::OnBnClickedDelbutton()
{
POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();	
int nSel=0,nSel2=0;
if(pos)
{
nSel=m_cAttrList.GetNextSelectedItem(pos);
pos=m_cValList.GetFirstSelectedItemPosition();
if(pos)
{
nSel2=m_cValList.GetNextSelectedItem(pos);
vector<tagModify>::iterator iter=vecGoodAttr[m_nVecIndex].vecAddAttr[nSel].vecModify.begin();
vector<tagModify>::iterator iter2=m_tagGoodAttr.vecAddAttr[nSel].vecModify.begin();
iter+=nSel2;
iter2+=nSel2;
if(m_bEdit)
{
vecGoodAttr[m_nVecIndex].vecAddAttr[nSel].vecModify.erase(iter);
m_tagGoodAttr.vecAddAttr[nSel].vecModify.erase(iter2);
}
else if(m_bAdd)
{
m_tagGoodAttr.vecAddAttr[nSel].vecModify.erase(iter2);
}
m_cValList.DeleteItem(nSel2);
}
}
}*/
/*
void CGoodAttrPage::OnBnClickedAddbutton()
{
// TODO: 在此添加控件通知处理程序代码
POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
if(pos)
{
int nCurSel=m_cValSelCombo.GetCurSel();
int nSelected=m_cAttrList.GetNextSelectedItem(pos);
int nSubItem=3+nCurSel;
char strVal[500],strMin[500],strMax[500],strOdds[500];
m_cAttrList.GetItemText(nSelected,nSubItem,strVal,500);
m_cMinEdit.GetWindowText(strMin,500);
m_cMaxEdit.GetWindowText(strMax,500);
m_cOddsEdit.GetWindowText(strOdds,500);


tagModify stModify;
stModify.lFixID=nCurSel;
stModify.dwMin=atoi(strMin);
stModify.dwMax=atoi(strMax);
stModify.wOdds=atoi(strOdds);		
if(m_bEdit)
{
vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].vecModify.push_back(stModify);
m_tagGoodAttr.vecAddAttr[nSelected].vecModify.push_back(stModify);
}
else if(m_bAdd)
{
m_tagGoodAttr.vecAddAttr[nSelected].vecModify.push_back(stModify);
}
int nCurItem=m_cValList.InsertItem(0xffff,"");
m_cValList.SetItem(nCurItem,0,LVIF_TEXT|LVIF_PARAM,strVal,NULL,0,0,nCurSel);
m_cValList.SetItem(nCurItem,1,LVIF_TEXT,strMin,NULL,0,0,0);
m_cValList.SetItem(nCurItem,2,LVIF_TEXT,strMax,NULL,0,0,0);
m_cValList.SetItem(nCurItem,3,LVIF_TEXT,strOdds,NULL,0,0,0);

m_cValList.SetItemState(nCurItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);		
}
}*/

void CGoodAttrPage::EnableCtrlGroup()
{
	m_cVailCombo.EnableWindow(TRUE);
	m_cHideCombo.EnableWindow(TRUE);
	m_cValEdit1.EnableWindow(TRUE);
	m_cValEdit2.EnableWindow(TRUE);

	//m_cOddsEdit2.EnableWindow(TRUE);
	//m_cValList.EnableWindow(TRUE);
	//m_cValSelCombo.EnableWindow(TRUE);
	//m_cMinEdit.EnableWindow(TRUE);
	//m_cMaxEdit.EnableWindow(TRUE);
	//m_cOddsEdit.EnableWindow(TRUE);
	//m_cAddBtn.EnableWindow(TRUE);
	//m_cDelBtn.EnableWindow(TRUE);
}

void CGoodAttrPage::DisableCtrlGroup()
{
	m_cVailCombo.EnableWindow(FALSE);
	m_cHideCombo.EnableWindow(FALSE);
	m_cValEdit1.EnableWindow(FALSE);
	m_cValEdit2.EnableWindow(FALSE);
	//m_cOddsEdit2.EnableWindow(FALSE);
	//m_cValList.EnableWindow(FALSE);
	//m_cValSelCombo.EnableWindow(FALSE);
	//m_cMinEdit.EnableWindow(FALSE);
	//m_cMaxEdit.EnableWindow(FALSE);
	//m_cOddsEdit.EnableWindow(FALSE);
	//m_cAddBtn.EnableWindow(FALSE);
	//m_cDelBtn.EnableWindow(FALSE);
}


void CGoodAttrPage::OnBnClickedSaveasbutton()
{
	// TODO: 在此添加控件通知处理程序代码
	CContainerPage *pContainer=(CContainerPage*)GetParent();
	CMainFrame* pMainFrame=(CMainFrame*)(pContainer->GetParent()->GetParent());
	DWORD dwOldIndex=m_tagGoodAttr.dwIndex;
	pMainFrame->OnNew();	
	//vecGoodAttr
	vector<tagGoodAttr>::iterator iter=vecGoodAttr.begin();
	for(;iter!=vecGoodAttr.end();iter++)
	{
		if(iter->dwIndex==dwOldIndex)
		{
			//属性拷贝
			vecGoodAttr[m_nVecIndex].bSave=iter->bSave;
			vecGoodAttr[m_nVecIndex].bSoundSwitch=iter->bSoundSwitch;
			vecGoodAttr[m_nVecIndex].dwEquipID=iter->dwEquipID;
			vecGoodAttr[m_nVecIndex].dwGroundId=iter->dwGroundId;
			vecGoodAttr[m_nVecIndex].dwIconId=iter->dwIconId;
			vecGoodAttr[m_nVecIndex].dwSound=iter->dwSound;
			vecGoodAttr[m_nVecIndex].dwSoundID1=iter->dwSoundID1;
			vecGoodAttr[m_nVecIndex].dwSoundID2=iter->dwSoundID2;
			vecGoodAttr[m_nVecIndex].dwType=iter->dwType;
			vecGoodAttr[m_nVecIndex].dwValue=iter->dwValue;
			vecGoodAttr[m_nVecIndex].dwSilverValue=iter->dwSilverValue;
			vecGoodAttr[m_nVecIndex].dwWeaponActType=iter->dwWeaponActType;
			//vecGoodAttr[m_nVecIndex].dwWeight=iter->dwWeight;
			//vecGoodAttr[m_nVecIndex].lX_Size=iter->lX_Size;
			//vecGoodAttr[m_nVecIndex].lY_Size=iter->lY_Size;
			vecGoodAttr[m_nVecIndex].strContent=iter->strContent;
			vecGoodAttr[m_nVecIndex].strName=iter->strName;
			vecGoodAttr[m_nVecIndex].vecAddAttr.clear();
			vecGoodAttr[m_nVecIndex].vecAddAttr.assign(iter->vecAddAttr.begin(),iter->vecAddAttr.end());

			//添加套装属性和激发属性属性保存
			vecGoodAttr[m_nVecIndex].vecSuitAttr.clear();
			vecGoodAttr[m_nVecIndex].vecSuitAttr.assign( iter->vecSuitAttr.begin(), iter->vecSuitAttr.end() );
			vecGoodAttr[m_nVecIndex].vecArouseAttr.clear();
			vecGoodAttr[m_nVecIndex].vecArouseAttr.assign( iter->vecArouseAttr.begin(), iter->vecArouseAttr.end() );

			FillCtrls(m_nVecIndex);
			pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,3,iter->bSave?"是":"否");
			pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,9,const_cast<char*>(strGoodType[iter->dwType]));
			pContainer->m_wndAllGoodsPage.ChangeItemText(vecGoodAttr[m_nVecIndex].dwIndex,16,iter->bSoundSwitch?"是":"否");
			break;
		}
	}
}



void CGoodAttrPage::OnBnClickedCopyattrbtn()
{
	POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
	int nSel=0;
	if(pos)
	{
		nSel=m_cAttrList.GetNextSelectedItem(pos);
		vector<tagAddAttr>::iterator iter=vecGoodAttr[m_nVecIndex].vecAddAttr.begin();
		vector<tagAddAttr>::iterator iter2=m_tagGoodAttr.vecAddAttr.begin();
		iter+=nSel;
		iter2+=nSel;
		//if(m_bEdit)
		//{			
		//	vecGoodAttr[m_nVecIndex].vecAddAttr.erase(iter);
		//	m_tagGoodAttr.vecAddAttr.erase(iter2);
		//}
		//else if(m_bAdd)
		//{			
		//	m_tagGoodAttr.vecAddAttr.erase(iter2);
		//}

		g_stAddAttrCopy = *iter2;
		//m_cAttrList.DeleteItem(nSel);
		//m_cValList.DeleteAllItems();
	}
}

void CGoodAttrPage::OnBnClickedPasteattrbtn()
{
	if(m_bEdit)
	{
		vector<tagAddAttr>::iterator pAddAttr = vecGoodAttr[m_nVecIndex].vecAddAttr.begin();
		bool b1 = false;
		bool b2 = false;
		for (;pAddAttr != vecGoodAttr[m_nVecIndex].vecAddAttr.end();pAddAttr++)
		{
			if( g_stAddAttrCopy.wType > pAddAttr->wType)
				continue;
			if( g_stAddAttrCopy.wType <= pAddAttr->wType)
			{
				vecGoodAttr[m_nVecIndex].vecAddAttr.insert(pAddAttr,1,g_stAddAttrCopy);
				b1 = true;

				vector<tagAddAttr>::iterator pTagGoodsAddAttr = m_tagGoodAttr.vecAddAttr.begin();
				for (;pTagGoodsAddAttr!=m_tagGoodAttr.vecAddAttr.end();pTagGoodsAddAttr++)
				{
					if ( g_stAddAttrCopy.wType > pTagGoodsAddAttr->wType)
						continue;
					if (g_stAddAttrCopy.wType <= pTagGoodsAddAttr->wType)
					{
						m_tagGoodAttr.vecAddAttr.insert(pTagGoodsAddAttr,1,g_stAddAttrCopy);
						b2 = true;
						break;
					}
				}
				break;
			}
		}
		if (b1==false)
		{
			vecGoodAttr[m_nVecIndex].vecAddAttr.push_back(g_stAddAttrCopy);
		}
		if(b2 == false)
		{
			m_tagGoodAttr.vecAddAttr.push_back(g_stAddAttrCopy);
		}
	}
	else if(m_bAdd)
	{		
		m_tagGoodAttr.vecAddAttr.push_back(g_stAddAttrCopy);
	}

	int nIndex=m_cAttrList.InsertItem(0xffff,"");
	m_cAttrList.SetItem(nIndex,0,LVIF_TEXT,AttrNamesVec[g_stAddAttrCopy.wType].c_str(),NULL,0,0,0);

	m_cAttrList.SetItem(nIndex,1,LVIF_TEXT,g_stAddAttrCopy.bEnable?"是":"否",NULL,0,0,0);
	m_cAttrList.SetItem(nIndex,2,LVIF_TEXT,g_stAddAttrCopy.bHide?"是":"否",NULL,0,0,0);

	char str[256];
	sprintf_s(str, "%d", g_stAddAttrCopy.lValue1);
	m_cAttrList.SetItem(nIndex,3,LVIF_TEXT,str,NULL,0,0,0);
	sprintf_s(str, "%d", g_stAddAttrCopy.lValue2);
	m_cAttrList.SetItem(nIndex,4,LVIF_TEXT,str,NULL,0,0,0);
	//附加属性出现几率被屏蔽了
	//sprintf(str, "%d", g_stAddAttrCopy.wEnableOdds);
	//m_cAttrList.SetItem(nIndex,5,LVIF_TEXT,str,NULL,0,0,0);

	//	m_cAttrList.SetItemState(nIndex,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);	

	UpDataOwnAttrList(m_nVecIndex);

}

// 加入属性
void CGoodAttrPage::OnBnClickedDelattrbtn2()
{
	// TODO: 在此添加控件通知处理程序代码
	static int nSel=-1;
	int nCurSel=m_cAttrCombo.GetCurSel();		//选择哪一条？
	if(nCurSel<0)
		return;

	nSel=nCurSel;
	tagAddAttr stAddAttr;
	stAddAttr.wType=nSel;
	stAddAttr.bEnable=true;
	stAddAttr.bHide=false;
	stAddAttr.lValue1=0;
	stAddAttr.lValue2=0;
	//stAddAttr.wEnableOdds=10000;

	if(m_bEdit)
	{
		bool b = false;
		vector<tagAddAttr>::iterator pTemp = vecGoodAttr[m_nVecIndex].vecAddAttr.begin();
		for (;pTemp!=vecGoodAttr[m_nVecIndex].vecAddAttr.end();pTemp++)
		{
			if (stAddAttr.wType > pTemp->wType)
				continue;
			if (stAddAttr.wType <= pTemp->wType)
			{
				vecGoodAttr[m_nVecIndex].vecAddAttr.insert(pTemp,1,stAddAttr);
				b = true;
				break;
			}
		}
		if (b == false)
		{
			vecGoodAttr[m_nVecIndex].vecAddAttr.push_back(stAddAttr);
		}

	}

	bool b = false;
	vector<tagAddAttr>::iterator pTemp = m_tagGoodAttr.vecAddAttr.begin();
	for (;pTemp!=m_tagGoodAttr.vecAddAttr.end();pTemp++)
	{
		if(stAddAttr.wType > pTemp->wType )
			continue;
		if (stAddAttr.wType <= pTemp->wType)
		{
			m_tagGoodAttr.vecAddAttr.insert(pTemp,1,stAddAttr);
			b = true;
			break;
		}
	}
	if (b == false)
	{
		m_tagGoodAttr.vecAddAttr.push_back(stAddAttr);
	}
	// 注释控件设置 刷新 m_cAttrList 和 m_cAttrList2 有下面这个函数
	UpDataOwnAttrList(m_nVecIndex);
	//int nIndex=m_cAttrList.InsertItem(0xffff,"");
	//m_cAttrList.SetItem(nIndex,0,LVIF_TEXT,AttrNamesVec[nCurSel].c_str(),NULL,0,0,0);
	//m_cAttrList.SetItem(nIndex,1,LVIF_TEXT,m_cVailCombo.GetCurSel()==1?"是":"否",NULL,0,0,0);
	//m_cAttrList.SetItem(nIndex,2,LVIF_TEXT,m_cHideCombo.GetCurSel()==1?"是":"否",NULL,0,0,0);
	//m_cAttrList.SetItem(nIndex,3,LVIF_TEXT,"0",NULL,0,0,0);
	//m_cAttrList.SetItem(nIndex,4,LVIF_TEXT,"0",NULL,0,0,0);
	//m_cAttrList.SetItem(nIndex,5,LVIF_TEXT,"10000",NULL,0,0,0);
	//m_cAttrList.SetItemState(nIndex,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
}

void CGoodAttrPage::UpDataOwnAttrList(int mIndex)
{
	//	vector<tagAddAttr>::iterator pAddAttr = m_tagGoodAttr.vecAddAttr.begin();
	vector<tagAddAttr>::iterator pAddAttr = vecGoodAttr[mIndex].vecAddAttr.begin();
	int i=0;
	char strVal[500];
	m_cAttrList.DeleteAllItems();
	for (;pAddAttr != vecGoodAttr[mIndex].vecAddAttr.end();pAddAttr++,i++)
	{
		m_cAttrList.InsertItem(0xffff,"");
		m_cAttrList.SetItem(i,0,LVIF_TEXT,AttrNamesVec[pAddAttr->wType].c_str(),NULL,0,0,0);
		m_cAttrList.SetItem(i,1,LVIF_TEXT,pAddAttr->bEnable?"是":"否",NULL,0,0,0);
		m_cAttrList.SetItem(i,2,LVIF_TEXT,pAddAttr->bHide?"是":"否",NULL,0,0,0);
		sprintf_s(strVal,"%d",pAddAttr->lValue1);
		m_cAttrList.SetItem(i,3,LVIF_TEXT,strVal,NULL,0,0,0);
		sprintf_s(strVal,"%d",pAddAttr->lValue2);
		m_cAttrList.SetItem(i,4,LVIF_TEXT,strVal,NULL,0,0,0);
	}

	vector<tagSuitAttr>::iterator pSuitAttr =vecGoodAttr[mIndex].vecSuitAttr.begin();
	i=0;
	for(;pSuitAttr!=vecGoodAttr[mIndex].vecSuitAttr.end();pSuitAttr++,i++)
	{
		m_cAttrList2.InsertItem(0xffff,"");
		m_cAttrList2.SetItem(i,0,LVIF_TEXT,AttrNamesVec[pSuitAttr->wType].c_str(),NULL,0,0,0);

		sprintf_s(strVal,"%d",pSuitAttr->lValue1);
		m_cAttrList2.SetItem(i,1,LVIF_TEXT,strVal,NULL,0,0,0);
		sprintf_s(strVal,"%d",pSuitAttr->lValue2);
		m_cAttrList2.SetItem(i,2,LVIF_TEXT,strVal,NULL,0,0,0);
		sprintf_s(strVal,"%d",pSuitAttr->wSuitNum);
		m_cAttrList2.SetItem(i,3,LVIF_TEXT,strVal,NULL,0,0,0);
	}

	//add by wang-xx 2009-6-30 begin
	//激发属性
	vector<tagArouseAttr>::iterator pArouseAttr =vecGoodAttr[mIndex].vecArouseAttr.begin();
	i=0;
	for(;pArouseAttr!=vecGoodAttr[mIndex].vecArouseAttr.end();pArouseAttr++,i++)
	{
		m_cArouseAttrList.InsertItem(0xffff,"");
		m_cArouseAttrList.SetItem(i,0,LVIF_TEXT,AttrNamesVec[pArouseAttr->wType].c_str(),NULL,0,0,0);

		sprintf_s(strVal,"%d",pArouseAttr->lValue1);
		m_cArouseAttrList.SetItem(i,1,LVIF_TEXT,strVal,NULL,0,0,0);
		sprintf_s(strVal,"%d",pArouseAttr->lValue2);
		m_cArouseAttrList.SetItem(i,2,LVIF_TEXT,strVal,NULL,0,0,0);
		sprintf_s(strVal,"%d",pArouseAttr->wArouseCnt);
		m_cArouseAttrList.SetItem(i,3,LVIF_TEXT,strVal,NULL,0,0,0);
	}
	//add by wang-xx 2009-6-30 end
}
void CGoodAttrPage::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	static int nSel=-1;
	int nCurSel=m_cAttrCombo2.GetCurSel();
	if(nCurSel<0)
		return;

	nSel=nCurSel;	
	tagSuitAttr stSuitAttr;
	stSuitAttr.wType=nSel;
	stSuitAttr.lValue1=0;
	stSuitAttr.lValue2=0;
	stSuitAttr.wSuitNum=0;		

	if(m_bEdit)
	{	
		vecGoodAttr[m_nVecIndex].vecSuitAttr.push_back(stSuitAttr);
		m_tagGoodAttr.vecSuitAttr.push_back(stSuitAttr);
	}
	else if(m_bAdd)
	{				
		m_tagGoodAttr.vecSuitAttr.push_back(stSuitAttr);
	}

	int nIndex=m_cAttrList2.InsertItem(0xffff,"");
	m_cAttrList2.SetItem(nIndex,0,LVIF_TEXT,AttrNamesVec[nCurSel].c_str(),NULL,0,0,0);	
	m_cAttrList2.SetItem(nIndex,1,LVIF_TEXT,"0",NULL,0,0,0);
	m_cAttrList2.SetItem(nIndex,2,LVIF_TEXT,"0",NULL,0,0,0);
	m_cAttrList2.SetItem(nIndex,3,LVIF_TEXT,"0",NULL,0,0,0);
	//m_cAttrList.SetItem(nIndex,5,LVIF_TEXT,"10000",NULL,0,0,0);
	m_cAttrList2.SetItemState(nIndex,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);	
}

// 修改属性类型
void CGoodAttrPage::OnBnClickedDelattrbtn3()
{
	int nCurSel=m_cAttrCombo.GetCurSel();
	if(nCurSel>=0)
	{
		POSITION pos=m_cAttrList.GetFirstSelectedItemPosition();
		if(pos)
		{
			int nSelected=m_cAttrList.GetNextSelectedItem(pos);
			//if(m_bEdit)
			//{
			//	vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].wType = nCurSel;
			//	m_tagGoodAttr.vecAddAttr[nSelected].wType = nCurSel;
			//}
			//else if(m_bAdd)
			//{
			//	m_tagGoodAttr.vecAddAttr[nSelected].wType = nCurSel;
			//}
			if(m_bEdit)
			{
				vector<tagAddAttr>::iterator it = vecGoodAttr[m_nVecIndex].vecAddAttr.begin();
				bool b=false;
				bool bFirst = false;				// 是否在选中项前加入
				for (;it!=vecGoodAttr[m_nVecIndex].vecAddAttr.end();it++)
				{
					if( nCurSel > it->wType)
						continue;
					if (nCurSel <= it->wType)
					{
						if(nCurSel < vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].wType)
							bFirst = true;
						tagAddAttr pAdd;
						pAdd.wType  = nCurSel;
						pAdd.bEnable = vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].bEnable;
						pAdd.bHide = vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].bHide;
						pAdd.lValue1 = vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].lValue1;
						pAdd.lValue2 = vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].lValue2;
						vecGoodAttr[m_nVecIndex].vecAddAttr.insert(it,1,pAdd);
						b=true;
						break;
					}
				}
				if (b ==false)
				{
					tagAddAttr pAdd;
					pAdd.wType  = nCurSel;
					pAdd.bEnable = vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].bEnable;
					pAdd.bHide = vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].bHide;
					pAdd.lValue1 = vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].lValue1;
					pAdd.lValue2 = vecGoodAttr[m_nVecIndex].vecAddAttr[nSelected].lValue2;
					vecGoodAttr[m_nVecIndex].vecAddAttr.push_back(pAdd);
				}

				for (it=vecGoodAttr[m_nVecIndex].vecAddAttr.begin();it!=vecGoodAttr[m_nVecIndex].vecAddAttr.end();it++)
				{
					int num = nSelected;
					if (bFirst==true)
						num +=1;
					if (it->wType == vecGoodAttr[m_nVecIndex].vecAddAttr[num].wType)
					{
						vecGoodAttr[m_nVecIndex].vecAddAttr.erase(it);
						break;
					}
				}
			}


			vector<tagAddAttr>::iterator it = m_tagGoodAttr.vecAddAttr.begin();
			bool b = false;
			bool bFirst = false;
			for (;it!=m_tagGoodAttr.vecAddAttr.end();it++)
			{
				if( nCurSel > it->wType)
					continue;
				if ( nCurSel <= it->wType)
				{
					if (m_tagGoodAttr.vecAddAttr[nSelected].wType > nCurSel)
						bFirst = true;
					tagAddAttr pAdd;
					pAdd.wType = nCurSel;
					pAdd.bEnable = m_tagGoodAttr.vecAddAttr[nSelected].bEnable;
					pAdd.bHide = m_tagGoodAttr.vecAddAttr[nSelected].bHide;
					pAdd.lValue1 = m_tagGoodAttr.vecAddAttr[nSelected].lValue1;
					pAdd.lValue2 = m_tagGoodAttr.vecAddAttr[nSelected].lValue2;
					m_tagGoodAttr.vecAddAttr.insert(it,1,pAdd);
					b = true;
					break;
				}
			}
			if (b == false)
			{
				tagAddAttr pAdd;
				pAdd.wType = nCurSel;
				pAdd.bEnable = m_tagGoodAttr.vecAddAttr[nSelected].bEnable;
				pAdd.bHide = m_tagGoodAttr.vecAddAttr[nSelected].bHide;
				pAdd.lValue1 = m_tagGoodAttr.vecAddAttr[nSelected].lValue1;
				pAdd.lValue2 = m_tagGoodAttr.vecAddAttr[nSelected].lValue2;
				m_tagGoodAttr.vecAddAttr.push_back(pAdd);
			}

			for (it = m_tagGoodAttr.vecAddAttr.begin();it!=m_tagGoodAttr.vecAddAttr.end();it++)
			{
				int num = nSelected;
				if (bFirst==true)
					num+=1;
				if (it->wType == m_tagGoodAttr.vecAddAttr[num].wType)
				{
					m_tagGoodAttr.vecAddAttr.erase(it);
					break;
				}
			}

			m_cAttrList.SetItemText(nSelected,0,AttrNamesVec[nCurSel].c_str());
			UpDataOwnAttrList(m_nVecIndex);
		}
	}	
}


void CGoodAttrPage::OnCountEquip()
{
}

void CGoodAttrPage::OnBnClickedCountlevellimet()
{
	extern bool CountEquipLevelLimit(const char* strOrginName, tagAddAttr *pAttr);

	vector<tagAddAttr>::iterator iterAttr;
	tagGoodAttr *pGoods = &vecGoodAttr[m_nVecIndex];
	if (pGoods && pGoods->dwType >= 2 )	// 装备类
	{
		for(iterAttr=pGoods->vecAddAttr.begin();iterAttr!=pGoods->vecAddAttr.end();iterAttr++)
		{
			tagAddAttr *pAttr = &(*iterAttr);
			CountEquipLevelLimit(pGoods->strOrigName.c_str(), pAttr);

			//int i=0;
			//vector<tagAddAttr>::iterator iter;
			//char strVal[500];
			//for(iter=vecGoodAttr[m_nVecIndex].vecAddAttr.begin();iter!=vecGoodAttr[m_nVecIndex].vecAddAttr.end();iter++,i++)
			//{
			//	m_cAttrList.InsertItem(0xffff,"");
			//	m_cAttrList.SetItem(i,0,LVIF_TEXT,AttrNamesVec[iter->wType].c_str(),NULL,0,0,0);
			//	m_cAttrList.SetItem(i,1,LVIF_TEXT,iter->bEnable?"是":"否",NULL,0,0,0);
			//	m_cAttrList.SetItem(i,2,LVIF_TEXT,iter->bHide?"是":"否",NULL,0,0,0);
			//	sprintf(strVal,"%d",iter->lValue1);
			//	m_cAttrList.SetItem(i,3,LVIF_TEXT,strVal,NULL,0,0,0);
			//	sprintf(strVal,"%d",iter->lValue2);
			//	m_cAttrList.SetItem(i,4,LVIF_TEXT,strVal,NULL,0,0,0);
			//	sprintf(strVal,"%d",iter->wEnableOdds);
			//	m_cAttrList.SetItem(i,5,LVIF_TEXT,strVal,NULL,0,0,0);

			//}
		}
	}
}
//左键点击套装属性list的某条记录
void CGoodAttrPage::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	m_cAttrList2.SetHotItem(-1);
	POSITION pos=m_cAttrList2.GetFirstSelectedItemPosition();
	int nSelected=m_cAttrList2.GetNextSelectedItem(pos);
	if (nSelected>=0)
	{
		char strName[500],strVal1[500],strVal2[500],strSuitAmount[10];
		m_cAttrList2.GetItemText(nSelected,0,strName,500);
		m_cAttrList2.GetItemText(nSelected,1,strVal1,500);
		m_cAttrList2.GetItemText(nSelected,2,strVal2,500);
		m_cAttrList2.GetItemText(nSelected,3,strSuitAmount,100);
		m_cSuitValEdit1.SetWindowText(strVal1);
		m_cSuitValEdit2.SetWindowText(strVal2);
		m_cSuitNumEdit.SetWindowText(strSuitAmount);
	}
	//	*pResult = 0;
}
void CGoodAttrPage::OnBnClickedArouseAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	static int nSel=-1;
	int nCurSel=m_cArouseAttrCombo.GetCurSel();
	if(nCurSel<0)
		return;

	nSel=nCurSel;	
	tagArouseAttr stArouseAttr;
	stArouseAttr.wType=nSel;
	stArouseAttr.lValue1=0;
	stArouseAttr.lValue2=0;
	stArouseAttr.wArouseCnt=0;		

	if(m_bEdit)
	{	
		vecGoodAttr[m_nVecIndex].vecArouseAttr.push_back(stArouseAttr);
		m_tagGoodAttr.vecArouseAttr.push_back(stArouseAttr);
	}
	else if(m_bAdd)
	{				
		m_tagGoodAttr.vecArouseAttr.push_back(stArouseAttr);
	}

	int nIndex=m_cArouseAttrList.InsertItem(0xffff,"");
	m_cArouseAttrList.SetItem(nIndex,0,LVIF_TEXT,AttrNamesVec[nCurSel].c_str(),NULL,0,0,0);	
	m_cArouseAttrList.SetItem(nIndex,1,LVIF_TEXT,"0",NULL,0,0,0);
	m_cArouseAttrList.SetItem(nIndex,2,LVIF_TEXT,"0",NULL,0,0,0);
	m_cArouseAttrList.SetItem(nIndex,3,LVIF_TEXT,"0",NULL,0,0,0);
	m_cArouseAttrList.SetItemState(nIndex,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);	
}

void CGoodAttrPage::OnBnClickedArouseDel()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos=m_cArouseAttrList.GetFirstSelectedItemPosition();
	int nSel=0;
	if(pos)
	{
		nSel=m_cArouseAttrList.GetNextSelectedItem(pos);
		vector<tagArouseAttr>::iterator iter=vecGoodAttr[m_nVecIndex].vecArouseAttr.begin();
		vector<tagArouseAttr>::iterator iter2=m_tagGoodAttr.vecArouseAttr.begin();
		iter+=nSel;
		iter2+=nSel;
		if(m_bEdit)
		{			
			vecGoodAttr[m_nVecIndex].vecArouseAttr.erase(iter);			
			m_tagGoodAttr.vecArouseAttr.erase(iter2);			
		}
		else if(m_bAdd)
		{			
			m_tagGoodAttr.vecArouseAttr.erase(iter2);			
		}
		m_cArouseAttrList.DeleteItem(nSel);
	}
}

void CGoodAttrPage::OnEnChangeArouseVal1()
{	
	POSITION pos=m_cArouseAttrList.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nSelected=m_cArouseAttrList.GetNextSelectedItem(pos);
		char strVal[500];
		m_cArouseVal1.GetWindowText(strVal,500);
		int nVal=atoi(strVal);
		if(m_bEdit)
		{
			vecGoodAttr[m_nVecIndex].vecArouseAttr[nSelected].lValue1=nVal;
			m_tagGoodAttr.vecArouseAttr[nSelected].lValue1=nVal;
		}
		else if(m_bAdd)
			m_tagGoodAttr.vecArouseAttr[nSelected].lValue1=nVal;
		m_cArouseAttrList.SetItemText(nSelected,1,strVal);
	}
}

void CGoodAttrPage::OnEnChangeArouseVal2()
{	
	POSITION pos=m_cArouseAttrList.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nSelected=m_cArouseAttrList.GetNextSelectedItem(pos);
		char strVal[500];
		m_cArouseVal2.GetWindowText(strVal,500);
		int nVal=atoi(strVal);
		if(m_bEdit)
		{
			vecGoodAttr[m_nVecIndex].vecArouseAttr[nSelected].lValue2=nVal;
			m_tagGoodAttr.vecArouseAttr[nSelected].lValue2=nVal;
		}
		else if(m_bAdd)
			m_tagGoodAttr.vecArouseAttr[nSelected].lValue2=nVal;
		m_cArouseAttrList.SetItemText(nSelected,2,strVal);
	}
}

void CGoodAttrPage::OnEnChangeArouseNum()
{	
	POSITION pos=m_cArouseAttrList.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nSelected=m_cArouseAttrList.GetNextSelectedItem(pos);
		char strVal[500];
		m_cArouseNum.GetWindowText(strVal,500);
		int nVal=atoi(strVal);
		if(m_bEdit)
		{
			vecGoodAttr[m_nVecIndex].vecArouseAttr[nSelected].wArouseCnt=nVal;
			m_tagGoodAttr.vecArouseAttr[nSelected].wArouseCnt=nVal;
		}
		else if(m_bAdd)
			m_tagGoodAttr.vecArouseAttr[nSelected].wArouseCnt=nVal;
		m_cArouseAttrList.SetItemText(nSelected,3,strVal);
	}
}
//左键点击激发属性list的某条记录
void CGoodAttrPage::OnNMClickArouseList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	m_cArouseAttrList.SetHotItem(-1);
	POSITION pos=m_cArouseAttrList.GetFirstSelectedItemPosition();
	int nSelected=m_cArouseAttrList.GetNextSelectedItem(pos);
	if (nSelected>=0)
	{
		char strName[500],strVal1[500],strVal2[500],strArouseNum[10];
		m_cArouseAttrList.GetItemText(nSelected,0,strName,500);
		m_cArouseAttrList.GetItemText(nSelected,1,strVal1,500);
		m_cArouseAttrList.GetItemText(nSelected,2,strVal2,500);
		m_cArouseAttrList.GetItemText(nSelected,3,strArouseNum,100);
		m_cArouseVal1.SetWindowText(strVal1);
		m_cArouseVal2.SetWindowText(strVal2);
		m_cArouseNum.SetWindowText(strArouseNum);
	}
	//	*pResult = 0;
}