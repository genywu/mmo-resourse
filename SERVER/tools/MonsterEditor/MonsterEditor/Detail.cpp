// Detail.cpp : 实现文件
//

#include "stdafx.h"
#include "MonsterEditor.h"
#include "Detail.h"


vector<tagMonster>::iterator iters;
vector<tagMonster>*vecMonDetail;
// CDetail 对话框
//DOWRD转换为CString
CString DwordToString1(DWORD n)
{

	CString str;

	str.Format("%d",n);
	return str;
}
IMPLEMENT_DYNAMIC(CDetail, CDialog)

CDetail::CDetail(CWnd* pParent /*=NULL*/)
	: CDialog(CDetail::IDD, pParent)
	, nIDCLevel(0)
	, dwIDCExp(0)
	, dwIDCWorkPoint(0)
	, dwIDCMaxHp(0)
	, dwIDCMinAttack(0)
	, dwIDCMaxAttack(0)
	, dwIDCMinMagicAttack(0)
	, dwIDCMaxMagicAttack(0)
	, dwIDCHitRating(0)
	, dwIDCStormHit(0)
	, dwIDCMagicStorm(0)
	, dwIDCDefense(0)
	, dwIDCMagicDefense(0)
	, dwIDCBlock(0)
	, dwIDCParry(0)
	, dwIDCDodge(0)
	, fIDCMoveSpd(0)
	, fIDCBatMoveSpd(0)
	, strIDCMonRace(_T(""))
	, strIDCElem(_T(""))
	, dwIDCVitality(0)
	, dwIDCHpRecoverSpd(0)
	, dwIDCMpRecoverSpd(0)
	, strIDCAIModel(_T(""))
	, strIDCBlock(_T(""))
	, strIDCSuper(_T(""))
	, strIDCTame(_T(""))
	, bIDCTalk(_T(""))
	, strIDCReturn(_T(""))
	, dwIDCReDistance(0)
	, dwIDCReTime(0)
	, fIDCInjurRate(0)
	, dwIDCAlertTime(0)
	, fIDCEscape(0)
	, dwIDCSkillRate(0)
	, dwIDCSkillInter(0)
	, dwIDCTameTimes(0)
	, dwIDCMinStayTimes(0)
	, dwIDCMaxStayTimes(0)
	, dwIDCMinWalkTimes(0)
	, dwIDCMaxWalkTimes(0)
	, dwIDCAlertRange(0)
	, dwIDCPurRange(0)
	, dwIDCPeaMoveRange(0)
	, dwIDCStayTime(0)
	, strIDCAIWay(_T(""))
	, strIDCNO(_T(""))
	, dwIDCIndex(0)
	, strIDCOrigName(_T(""))
	, strIDCName(_T(""))
	, strIDCCollection(_T(""))
	, strIDCTitle(_T(""))
	, dwIDCTileColor(0)
	, dwIDCTitlePicture(0)
	, strIDCCountryID(_T(""))
	, strIDCNpcSort(_T(""))
	, dwIDCPicID(0)
	, dwIDCPicLevel(0)
	, dwIDCNameColorID(0)
	, dwIDCLevelColorID(0)
	, dwIDCHpColor(0)
	, dwIDCSoundID(0)
	, dwIDCHurtHuman(0)
	, dwIDCHurtElem(0)
	, dwIDCHurtDiff(0)
	, dwIDCHurtAnimal(0)
	, dwIDCHurtDead(0)
	, dwIDCHurtDevil(0)
	, dwIDCHurtInsect(0)
	, dwIDCStormHurt(0)
	, dwIDCMagicStormHurt(0)
	, dwIDCHurtFire(0)
	, dwIDCHurtWater(0)
	, dwIDCHurtGround(0)
	, dwIDCHurtWind(0)
	, dwIDCHurtSacre(0)
	, dwIDCHurtDark(0)
	, dwIDCHurtNotElem(0)
	, dwIDCGroundAnti(0)
	, dwIDCWaterAnti(0)
	, dwIDCFireAnti(0)
	, dwIDCWindAnti(0)
	, dwIDCSacreAnti(0)
	, dwIDCDarkAnti(0)
	, dwIDCNotElemAnti(0)
	, dwIDCStormAnti(0)
	, dwIDCMagicStormAnti(0)
	, dwIDCWillAnti(0)
	, dwIDCBodyAnti(0)
	, dwIDCBloodAnti(0)
	, dwIDCDuskAnti(0)
	, dwIDCNumbAnti(0)
	, dwIDCSleepAnti(0)
	, dwIDCDrugAnti(0)
	, dwIDCNotMoveAnti(0)
	, strIDCRefe1(_T(""))
	, strIDCRefe2(_T(""))
	, strIDCRefe3(_T(""))
	, strIDCRefe4(_T(""))
	, strIDCRefe5(_T(""))
	, strIDCBirthSct(_T(""))
	, strIDCBattleSct(_T(""))
	, strIDCInjurSct(_T(""))
	, strIDCDeadSct(_T(""))
	, strIDCCycleSct(_T(""))
	, strIDCChoseSkillSct(_T(""))
	, strIDCFindTgtSct(_T(""))
	, strIDCKillTgtSct(_T(""))
	, strIDCShape(_T(""))
	, strIDCSkillType(_T(""))
	, dwIDCSkillID(0)
	, dwIDCSkillLevel(0)
	, dwIDCInterTime(0)
	{

}

CDetail::~CDetail()
{
}

void CDetail::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT26, nIDCLevel);
	DDV_MinMaxInt(pDX, nIDCLevel, 1, 1000000);
	DDX_Control(pDX, IDC_LIST1, m_MonSkillList);

	DDX_Text(pDX, IDC_EDIT27, dwIDCExp);
	DDV_MinMaxUInt(pDX, dwIDCExp, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT28, dwIDCWorkPoint);
	DDV_MinMaxUInt(pDX, dwIDCWorkPoint, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT29, dwIDCMaxHp);
	DDV_MinMaxUInt(pDX, dwIDCMaxHp, 0, 10000000);
	DDX_Text(pDX, IDC_EDIT30, dwIDCMinAttack);
	DDX_Text(pDX, IDC_EDIT31, dwIDCMaxAttack);
	DDV_MinMaxUInt(pDX, dwIDCMaxAttack, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT32, dwIDCMinMagicAttack);
	DDX_Text(pDX, IDC_EDIT33, dwIDCMaxMagicAttack);
	DDX_Text(pDX, IDC_EDIT34, dwIDCHitRating);
	DDV_MinMaxUInt(pDX, dwIDCHitRating, 0, 1000000);
	DDV_MinMaxUInt(pDX, dwIDCMinMagicAttack, 0, 1000000);
	DDV_MinMaxUInt(pDX, dwIDCMaxMagicAttack, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT35, dwIDCStormHit);
	DDV_MinMaxUInt(pDX, dwIDCStormHit, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT36, dwIDCMagicStorm);
	DDV_MinMaxUInt(pDX, dwIDCMagicStorm, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT37, dwIDCDefense);
	DDV_MinMaxUInt(pDX, dwIDCDefense, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT38, dwIDCMagicDefense);
	DDV_MinMaxUInt(pDX, dwIDCMagicDefense, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT39, dwIDCBlock);
	DDV_MinMaxUInt(pDX, dwIDCBlock, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT40, dwIDCParry);
	DDV_MinMaxUInt(pDX, dwIDCParry, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT41, dwIDCDodge);
	DDV_MinMaxUInt(pDX, dwIDCDodge, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT42, fIDCMoveSpd);
	DDV_MinMaxUInt(pDX, fIDCMoveSpd, 0, 1);
	DDX_Text(pDX, IDC_EDIT43, fIDCBatMoveSpd);
	DDV_MinMaxDouble(pDX, fIDCBatMoveSpd, 0, 1);
	DDX_Text(pDX, IDC_EDIT44, strIDCMonRace);
	DDX_Text(pDX, IDC_EDIT45, strIDCElem);
	DDX_Text(pDX, IDC_EDIT46, dwIDCVitality);
	DDV_MinMaxUInt(pDX, dwIDCVitality, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT47, dwIDCHpRecoverSpd);
	DDV_MinMaxUInt(pDX, dwIDCHpRecoverSpd, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT48, dwIDCMpRecoverSpd);
	DDV_MinMaxUInt(pDX, dwIDCMpRecoverSpd, 0, 1000000);
	DDX_CBString(pDX, IDC_COMBO2, strIDCAIModel);
	DDX_CBString(pDX, IDC_COMBO3, strIDCBlock);
	DDX_CBString(pDX, IDC_COMBO4, strIDCSuper);
	DDX_CBString(pDX, IDC_COMBO5, strIDCTame);
	DDX_CBString(pDX, IDC_COMBO6, bIDCTalk);
	DDX_CBString(pDX, IDC_COMBO7, strIDCReturn);
	DDX_CBString(pDX, IDC_COMBO8, strIDCAllRecover);
	DDX_Text(pDX, IDC_EDIT55, dwIDCReDistance);
	DDV_MinMaxUInt(pDX, dwIDCReDistance, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT56, dwIDCReTime);
	DDV_MinMaxUInt(pDX, dwIDCReTime, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT57, fIDCInjurRate);
	DDV_MinMaxDouble(pDX, fIDCInjurRate, 0, 50000);
	DDX_Text(pDX, IDC_EDIT58, dwIDCAlertTime);
	DDV_MinMaxUInt(pDX, dwIDCAlertTime, 0, 10000000);
	DDX_Text(pDX, IDC_EDIT59, fIDCEscape);
	DDV_MinMaxDouble(pDX, fIDCEscape, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT60, dwIDCSkillRate);
	DDV_MinMaxUInt(pDX, dwIDCSkillRate, 0, 100);
	DDX_Text(pDX, IDC_EDIT61, dwIDCSkillInter);
	DDV_MinMaxUInt(pDX, dwIDCSkillInter, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT62, dwIDCTameTimes);
	DDV_MinMaxUInt(pDX, dwIDCTameTimes, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT63, dwIDCMinStayTimes);
	DDV_MinMaxUInt(pDX, dwIDCMinStayTimes, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT105, dwIDCMaxStayTimes);
	DDV_MinMaxUInt(pDX, dwIDCMaxStayTimes, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT64, dwIDCMinWalkTimes);
	DDV_MinMaxUInt(pDX, dwIDCMinWalkTimes, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT65, dwIDCMaxWalkTimes);
	DDV_MinMaxUInt(pDX, dwIDCMaxWalkTimes, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT66, dwIDCAlertRange);
	DDV_MinMaxUInt(pDX, dwIDCAlertRange, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT67, dwIDCPurRange);
	DDV_MinMaxUInt(pDX, dwIDCPurRange, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT69, dwIDCPeaMoveRange);
	DDV_MinMaxUInt(pDX, dwIDCPeaMoveRange, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT71, dwIDCStayTime);
	DDV_MinMaxUInt(pDX, dwIDCStayTime, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT72, strIDCAIWay);
	DDX_Text(pDX, IDC_EDIT1, strIDCNO);
	DDX_Text(pDX, IDC_EDIT2, dwIDCIndex);
	DDV_MinMaxUInt(pDX, dwIDCIndex, 0, 4000000000);
	DDX_Text(pDX, IDC_EDIT3, strIDCOrigName);
	DDX_Text(pDX, IDC_EDIT4, strIDCName);
	DDX_Text(pDX, IDC_EDIT5, strIDCCollection);
	DDX_Text(pDX, IDC_EDIT6, strIDCTitle);
	DDX_Text(pDX, IDC_EDIT7, dwIDCTileColor);
	DDV_MinMaxUInt(pDX, dwIDCTileColor, 0, 40000000);
	DDX_Text(pDX, IDC_EDIT8, dwIDCTitlePicture);
	DDV_MinMaxUInt(pDX, dwIDCTitlePicture, 0, 400000000);
	DDX_Text(pDX, IDC_EDIT9, strIDCCountryID);
	DDX_Text(pDX, IDC_EDIT10, strIDCNpcSort);
	DDX_Text(pDX, IDC_EDIT11, dwIDCPicID);
	DDV_MinMaxUInt(pDX, dwIDCPicID, 0, 40000000);
	DDX_Text(pDX, IDC_EDIT12, dwIDCPicLevel);
	DDV_MinMaxUInt(pDX, dwIDCPicLevel, 0, 40000000);
	DDX_Text(pDX, IDC_EDIT13, dwIDCNameColorID);
	DDV_MinMaxUInt(pDX, dwIDCNameColorID, 0, 40000000);
	DDX_Text(pDX, IDC_EDIT14, dwIDCLevelColorID);
	DDV_MinMaxUInt(pDX, dwIDCLevelColorID, 0, 40000000);
	DDX_Text(pDX, IDC_EDIT15, dwIDCHpColor);
	DDV_MinMaxUInt(pDX, dwIDCHpColor, 0, 40000000);
	DDX_Text(pDX, IDC_EDIT16, dwIDCSoundID);
	DDV_MinMaxUInt(pDX, dwIDCSoundID, 0, 40000000);
	DDX_Text(pDX, IDC_EDIT73, dwIDCHurtHuman);
	DDV_MinMaxUInt(pDX, dwIDCHurtHuman, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT74, dwIDCHurtElem);
	DDV_MinMaxUInt(pDX, dwIDCHurtElem, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT75, dwIDCHurtDiff);
	DDV_MinMaxUInt(pDX, dwIDCHurtDiff, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT76, dwIDCHurtAnimal);
	DDV_MinMaxUInt(pDX, dwIDCHurtAnimal, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT77, dwIDCHurtDead);
	DDV_MinMaxUInt(pDX, dwIDCHurtDead, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT78, dwIDCHurtDevil);
	DDV_MinMaxUInt(pDX, dwIDCHurtDevil, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT79, dwIDCHurtInsect);
	DDV_MinMaxUInt(pDX, dwIDCHurtInsect, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT86, dwIDCStormHurt);
	DDV_MinMaxUInt(pDX, dwIDCStormHurt, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT87, dwIDCMagicStormHurt);
	DDV_MinMaxUInt(pDX, dwIDCMagicStormHurt, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT80, dwIDCHurtFire);
	DDV_MinMaxUInt(pDX, dwIDCHurtFire, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT81, dwIDCHurtWater);
	DDV_MinMaxUInt(pDX, dwIDCHurtWater, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT88, dwIDCHurtGround);
	DDV_MinMaxUInt(pDX, dwIDCHurtGround, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT82, dwIDCHurtWind);
	DDV_MinMaxUInt(pDX, dwIDCHurtWind, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT83, dwIDCHurtSacre);
	DDV_MinMaxUInt(pDX, dwIDCHurtSacre, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT84, dwIDCHurtDark);
	DDV_MinMaxUInt(pDX, dwIDCHurtDark, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT85, dwIDCHurtNotElem);
	DDV_MinMaxUInt(pDX, dwIDCHurtNotElem, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT89, dwIDCGroundAnti);
	DDV_MinMaxUInt(pDX, dwIDCGroundAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT90, dwIDCWaterAnti);
	DDV_MinMaxUInt(pDX, dwIDCWaterAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT91, dwIDCFireAnti);
	DDV_MinMaxUInt(pDX, dwIDCFireAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT92, dwIDCWindAnti);
	DDV_MinMaxUInt(pDX, dwIDCWindAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT93, dwIDCSacreAnti);
	DDV_MinMaxUInt(pDX, dwIDCSacreAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT94, dwIDCDarkAnti);
	DDV_MinMaxUInt(pDX, dwIDCDarkAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT95, dwIDCNotElemAnti);
	DDV_MinMaxUInt(pDX, dwIDCNotElemAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT104, dwIDCStormAnti);
	DDV_MinMaxUInt(pDX, dwIDCStormAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT106, dwIDCMagicStormAnti);
	DDV_MinMaxUInt(pDX, dwIDCMagicStormAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT96, dwIDCWillAnti);
	DDV_MinMaxUInt(pDX, dwIDCWillAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT97, dwIDCBodyAnti);
	DDV_MinMaxUInt(pDX, dwIDCBodyAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT98, dwIDCBloodAnti);
	DDV_MinMaxUInt(pDX, dwIDCBloodAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT99, dwIDCDuskAnti);
	DDV_MinMaxUInt(pDX, dwIDCDuskAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT100, dwIDCNumbAnti);
	DDV_MinMaxUInt(pDX, dwIDCNumbAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT101, dwIDCSleepAnti);
	DDV_MinMaxUInt(pDX, dwIDCSleepAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT102, dwIDCDrugAnti);
	DDV_MinMaxUInt(pDX, dwIDCDrugAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT103, dwIDCNotMoveAnti);
	DDV_MinMaxUInt(pDX, dwIDCNotMoveAnti, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT114, strIDCRefe1);
	DDX_Text(pDX, IDC_EDIT115, strIDCRefe2);
	DDX_Text(pDX, IDC_EDIT116, strIDCRefe3);
	DDX_Text(pDX, IDC_EDIT117, strIDCRefe4);
	DDX_Text(pDX, IDC_EDIT118, strIDCRefe5);
	DDX_Text(pDX, IDC_EDIT107, strIDCBirthSct);
	DDX_Text(pDX, IDC_EDIT108, strIDCBattleSct);
	DDX_Text(pDX, IDC_EDIT109, strIDCInjurSct);
	DDX_Text(pDX, IDC_EDIT110, strIDCDeadSct);
	DDX_Text(pDX, IDC_EDIT111, strIDCCycleSct);
	DDX_Text(pDX, IDC_EDIT112, strIDCChoseSkillSct);
	DDX_Text(pDX, IDC_EDIT119, strIDCFindTgtSct);
	DDX_Text(pDX, IDC_EDIT113, strIDCKillTgtSct);
	DDX_CBString(pDX, IDC_COMBO1, strIDCShape);




	DDX_CBString(pDX, IDC_COMBO9, strIDCSkillType);
	DDX_Text(pDX, IDC_EDIT120, dwIDCSkillID);
	DDX_Text(pDX, IDC_EDIT121, dwIDCSkillLevel);
	DDX_Text(pDX, IDC_EDIT122, dwIDCInterTime);
	DDX_Control(pDX, IDC_COMBO9, m_combox);
}


BEGIN_MESSAGE_MAP(CDetail, CDialog)
	ON_BN_CLICKED(IDC_BUTTON14, &CDetail::OnAppendSkill)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_EDIT26, &CDetail::OnEnChangeEdit26)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CDetail::OnNMClickList1)
	ON_BN_CLICKED(IDC_BUTTON15, &CDetail::OnUpdateSkill)
	ON_BN_CLICKED(IDC_BUTTON16, &CDetail::OnDeleteSkill)
	ON_BN_CLICKED(IDC_BUTTON1, &CDetail::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDetail::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDetail::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDetail::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDetail::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CDetail::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CDetail::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CDetail::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CDetail::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CDetail::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CDetail::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CDetail::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CDetail::OnBnClickedButton13)
END_MESSAGE_MAP()


// CDetail 消息处理程序
BOOL CDetail::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_MonSkillList.SetExtendedStyle(
		LVS_EX_FLATSB
		|LVS_EX_FULLROWSELECT
		|LVS_EX_HEADERDRAGDROP
		|LVS_EX_ONECLICKACTIVATE
		|LVS_EX_GRIDLINES
		);
	m_MonSkillList.InsertColumn(0,"技能类型",LVCFMT_LEFT,80,0);
	m_MonSkillList.InsertColumn(1,"技能ID",LVCFMT_LEFT,80,1);
	m_MonSkillList.InsertColumn(2,"技能等级",LVCFMT_LEFT,80,2);
	m_MonSkillList.InsertColumn(3,"技能间隔",LVCFMT_LEFT,100,3);
	FillData();
	return TRUE;
}
//设置对话框中各控件的值
void CDetail::SetData(vector<tagMonster>*vecMonster,vector<tagMonster>::iterator iter)
{
	char strVal[100];
	iters=iter;
	strIDCShape=(iter->strShape).c_str();
	nIDCLevel=iter->nLevel;
	dwIDCExp=(iter->dwExp);
	dwIDCWorkPoint=(iter->dwWorkPoint);
	dwIDCMaxHp=(iter->dwMaxHp);
	dwIDCMinAttack=(iter->wMinAttack);
	dwIDCMaxAttack=(iter->wMaxAttack);
	dwIDCMinMagicAttack=(iter->wMinMagicAttack);
	dwIDCMaxMagicAttack=(iter->wMaxMagicAttack);
	dwIDCHitRating=(iter->wHitRating);
	dwIDCStormHit=iter->wStormHit;
	dwIDCMagicStorm=(iter->wMagicStorm);
	dwIDCDefense=iter->wDefense;
	dwIDCMagicDefense=iter->wMagicDefense;
	dwIDCBlock=iter->wBlock;
	dwIDCParry=iter->wParry;
	dwIDCDodge=iter->wDodge;
	fIDCMoveSpd=iter->fMoveSpd;				
	fIDCBatMoveSpd=iter->fBatMoveSpd;
	strIDCMonRace=(iter->strMonRace).c_str();
	strIDCElem=(iter->strElem).c_str();
	dwIDCVitality=iter->wVitality;
	dwIDCHpRecoverSpd=iter->wHpRecoverSpd;
	dwIDCMpRecoverSpd=iter->wMpRecoverSpd;


	strIDCBlock=(iter->strBlock).c_str();
	strIDCSuper=(iter->strSuper).c_str();
	strIDCTame=(iter->strTame).c_str();
	bIDCTalk=itoa(iter->bTalk,strVal,10);
	strIDCReturn=(iter->strReturn).c_str();
	strIDCAllRecover=(iter->strAllRecover).c_str();
	dwIDCReDistance=iter->wReDistance;
	dwIDCReTime=iter->wReTime;
	fIDCInjurRate=iter->fInjurRate;
	dwIDCAlertTime=iter->wAlertTime;
	fIDCEscape=iter->fEscape;
	dwIDCSkillRate=iter->wSkillRate;
	dwIDCSkillInter=iter->wSkillInter;
	dwIDCTameTimes=iter->dwTameTimes;
	dwIDCMinStayTimes=iter->wMinStayTimes;
	dwIDCMaxStayTimes=iter->wMaxStayTimes;
	dwIDCMinWalkTimes=iter->wMinWalkTimes;
	dwIDCMaxWalkTimes=iter->wMaxWalkTimes;
	dwIDCAlertRange=iter->wAlertRange;
	dwIDCPurRange=iter->wPurRange;
	dwIDCPeaMoveRange=iter->wPeaMoveRange;
	strIDCAIModel=(iter->strAIModel).c_str();
	dwIDCStayTime=iter->wStayTime;
	strIDCAIWay=(iter->strAIWay).c_str();

	strIDCNO=itoa(iter->nNO,strVal,10);
	dwIDCIndex=iter->dwIndex;
	strIDCOrigName=(iter->strOrigName).c_str();
	strIDCName=(iter->strName).c_str();
	strIDCCollection=(iter->strCollection).c_str();
	strIDCTitle=(iter->strTitle).c_str();
	dwIDCTileColor=iter->dwTitleColor;
	dwIDCTitlePicture=iter->dwTitlePicture;
	strIDCCountryID=(iter->strCountryID).c_str();
	strIDCNpcSort=(iter->strNpcSort).c_str();
	dwIDCPicID=iter->dwPicID;
	dwIDCPicLevel=iter->dwPicLevel;
	dwIDCNameColorID=iter->dwNameColorID;
	dwIDCLevelColorID=iter->dwLevelColorID;
	dwIDCHpColor=iter->dwHpColor;
	dwIDCSoundID=iter->dwSoundID;

	dwIDCHurtHuman=iter->wHurtHuman;
	dwIDCHurtElem=iter->wHurtElem;
	dwIDCHurtDiff=iter->wHurtDiff;
	dwIDCHurtAnimal=iter->wHurtAnimal;
	dwIDCHurtDead=iter->wHurtDead;
	dwIDCHurtDevil=iter->wHurtDevil;
	dwIDCHurtInsect=iter->wHurtInsect;
	dwIDCStormHurt=iter->wStormHurt;
	dwIDCMagicStormHurt=iter->wMagicStormHurt;
	dwIDCHurtFire=iter->wHurtFire;
	dwIDCHurtWater=iter->wHurtWater;
	dwIDCHurtGround=iter->wHurtGround;
	dwIDCHurtWind=iter->wHurtWind;
	dwIDCHurtSacre=iter->wHurtSacre;
	dwIDCHurtDark=iter->wHurtDark;
	dwIDCHurtNotElem=iter->wHurtNotElem;

	dwIDCGroundAnti=iter->wGroundAnti;
	dwIDCWaterAnti=iter->wWaterAnti;
	dwIDCFireAnti=iter->wFireAnti;
	dwIDCWindAnti=iter->wWindAnti;
	dwIDCSacreAnti=iter->wSacreAnti;
	dwIDCDarkAnti=iter->wDarkAnti;
	dwIDCNotElemAnti=iter->wNotElemAnti;
	dwIDCStormAnti=iter->wStormAnti;
	dwIDCMagicStormAnti=iter->wMagicStormAnti;
	dwIDCWillAnti=iter->wWillAnti;
	dwIDCBodyAnti=iter->wBodyAnti;
	dwIDCBloodAnti=iter->wBloodAnti;
	dwIDCDuskAnti=iter->wDuskAnti;
	dwIDCNumbAnti=iter->wNumbAnti;
	dwIDCSleepAnti=iter->wSleepAnti;
	dwIDCDrugAnti=iter->wDrugAnti;
	dwIDCNotMoveAnti=iter->wNotMoveAnti;

	strIDCRefe1=(iter->strRefe1).c_str();
	strIDCRefe2=(iter->strRefe2).c_str();
	strIDCRefe3=(iter->strRefe3).c_str();
	strIDCRefe4=(iter->strRefe4).c_str();
	strIDCRefe5=(iter->strRefe5).c_str();
	strIDCBirthSct=(iter->strBirthSct).c_str();
	strIDCBattleSct=(iter->strBattleSct).c_str();
	strIDCInjurSct=(iter->strInjurSct).c_str();
	strIDCDeadSct=(iter->strDeadSct).c_str();
	strIDCCycleSct=(iter->strCycleSct).c_str();
	strIDCChoseSkillSct=(iter->strChoseSkillSct).c_str();
	strIDCFindTgtSct=(iter->strFindTgtSct).c_str();
	strIDCKillTgtSct=(iter->strKillTgtSct).c_str();
	vecMonDetail=vecMonster;
	
}
//将技能显示到listcontrol中
void CDetail::FillData()
{
	
	vector<tagSkill>::iterator iterskill;
	iterskill=iters->vecSkill.begin();
	//显示所有技能
	for(int i=0;iterskill!=iters->vecSkill.end();iterskill++,i++)
	{
		m_MonSkillList.InsertItem(i,"");
		//如果strSkillType为>，则为基本技能
		if(iterskill->strSkillType==">")
		{
			m_MonSkillList.SetItemText(i,0,"基本技能");
		}
		//如果strSkillType为#，则为普通技能
		if(iterskill->strSkillType=="#")
		{
			m_MonSkillList.SetItemText(i,0,"普通技能");
		}
		//如果strSkillType为@，则为周期技能
		if(iterskill->strSkillType=="@")
		{
			m_MonSkillList.SetItemText(i,0,"周期技能");
		}
		//设置技能ID,技能等级，技能间隔
		m_MonSkillList.SetItemText(i,1,DwordToString1(iterskill->dwSkillID));
		m_MonSkillList.SetItemText(i,2,DwordToString1(iterskill->wSkillLevel));
		m_MonSkillList.SetItemText(i,3,DwordToString1(iterskill->wInterTime));

	}
	
}
//将修改保存到内存
bool CDetail::Save()
{
	UpdateData();
	
	iters->nNO=atoi(strIDCNO);
	iters->dwIndex=dwIDCIndex;
	iters->strOrigName=strIDCOrigName;
	iters->strName=strIDCName;
	iters->strCollection=strIDCCollection;
	iters->strTitle=strIDCTitle;
	iters->dwTitleColor=dwIDCTileColor;
	iters->dwTitlePicture=dwIDCTitlePicture;
	iters->strCountryID=strIDCCountryID;
	iters->strNpcSort=strIDCNpcSort;
	iters->dwPicID=dwIDCPicID;
	iters->dwPicLevel=dwIDCPicLevel;
	iters->dwNameColorID=dwIDCNameColorID;
	iters->dwLevelColorID=dwIDCLevelColorID;
	iters->dwHpColor=dwIDCHpColor;
	iters->dwSoundID=dwIDCSoundID;
	
	string str;
	
	//保存基本属性页
	
	iters->strShape=strIDCShape;

	iters->nLevel=nIDCLevel;  
	iters->dwExp=dwIDCExp;
	iters->dwWorkPoint=dwIDCWorkPoint;
	iters->dwMaxHp=dwIDCMaxHp;
	iters->wMinAttack=dwIDCMinAttack;
	iters->wMaxAttack=dwIDCMaxAttack;
	iters->wMinMagicAttack=dwIDCMinMagicAttack;
	iters->wMaxMagicAttack=dwIDCMaxMagicAttack;
	iters->wHitRating=dwIDCHitRating;
	iters->wStormHit=dwIDCStormHit;
	iters->wMagicStorm=dwIDCMagicStorm;
	iters->wDefense=dwIDCDefense;
	iters->wMagicDefense=dwIDCMagicDefense;
	iters->wBlock=dwIDCBlock;
	iters->wParry=dwIDCParry;
	iters->wDodge=dwIDCDodge;
	iters->fMoveSpd=fIDCMoveSpd;
	iters->fBatMoveSpd=fIDCBatMoveSpd;
	iters->strMonRace=strIDCMonRace;
	iters->strElem=strIDCElem;
	iters->wVitality=dwIDCVitality;
	iters->wHpRecoverSpd=dwIDCHpRecoverSpd;
	iters->wMpRecoverSpd=dwIDCMpRecoverSpd;
	
	//保存抗性页
	iters->wGroundAnti=dwIDCGroundAnti;
	iters->wWaterAnti=dwIDCWaterAnti;
	iters->wFireAnti=dwIDCFireAnti;
	iters->wWindAnti=dwIDCWindAnti;
	iters->wSacreAnti=dwIDCSacreAnti;
	iters->wDarkAnti=dwIDCDarkAnti;
	iters->wNotElemAnti=dwIDCNotElemAnti;
	iters->wStormAnti=dwIDCStormAnti;
	iters->wMagicStormAnti=dwIDCMagicStormAnti;
	iters->wWillAnti=dwIDCWillAnti;
	iters->wBodyAnti=dwIDCBodyAnti;
	iters->wBloodAnti=dwIDCBloodAnti;
	iters->wDuskAnti=dwIDCDuskAnti;
	iters->wNumbAnti=dwIDCNumbAnti;
	iters->wSleepAnti=dwIDCSleepAnti;
	iters->wDrugAnti=dwIDCDrugAnti;
	iters->wNotMoveAnti=dwIDCNotMoveAnti;
	//保存伤害页
	iters->wHurtHuman=dwIDCHurtHuman;
	iters->wHurtElem=dwIDCHurtElem;
	iters->wHurtDiff=dwIDCHurtDiff;
	iters->wHurtAnimal=dwIDCHurtAnimal;
	iters->wHurtDead=dwIDCHurtDead;
	iters->wHurtDevil=dwIDCHurtDevil;
	iters->wHurtInsect=dwIDCHurtInsect;
	iters->wStormHurt=dwIDCStormHurt;
	iters->wMagicStormHurt=dwIDCMagicStormHurt;
	iters->wHurtFire=dwIDCHurtFire;
	iters->wHurtWater=dwIDCHurtWater;
	iters->wHurtGround=dwIDCHurtGround;
	iters->wHurtWind=dwIDCHurtWind;  
	iters->wHurtSacre=dwIDCHurtSacre;
	iters->wHurtDark=dwIDCHurtDark;
	iters->wHurtNotElem=dwIDCHurtNotElem;
	//保存脚本页
	iters->strRefe1=strIDCRefe1;
	iters->strRefe2=strIDCRefe2;
	iters->strRefe3=strIDCRefe3;
	iters->strRefe4=strIDCRefe4;
	iters->strRefe5=strIDCRefe5;
	iters->strBirthSct=strIDCBirthSct;
	iters->strBattleSct=strIDCBattleSct;
	iters->strInjurSct=strIDCInjurSct;
	iters->strDeadSct=strIDCDeadSct;
	iters->strCycleSct=strIDCCycleSct;
	iters->strChoseSkillSct=strIDCChoseSkillSct;
	iters->strFindTgtSct=strIDCFindTgtSct;
	iters->strKillTgtSct=strIDCKillTgtSct;
	//保存其他页
	
	iters->strBlock=strIDCBlock;

	
	iters->strSuper=strIDCSuper;

	
	iters->strTame=strIDCTame;

	iters->bTalk=atoi(bIDCTalk);

	
	iters->strReturn=strIDCReturn;

	
	iters->strAllRecover=strIDCAllRecover;

	iters->wReDistance=dwIDCReDistance;
	iters->wReTime=dwIDCReTime;
	iters->fInjurRate=fIDCInjurRate;
	iters->wAlertTime=dwIDCAlertTime;
	iters->fEscape=fIDCEscape;
	iters->wSkillRate=dwIDCSkillRate;
	iters->wSkillInter=dwIDCSkillInter;
	iters->dwTameTimes=dwIDCTameTimes;
	iters->wMinStayTimes=dwIDCMinStayTimes;
	iters->wMaxStayTimes=dwIDCMaxStayTimes;
	iters->wMinWalkTimes=dwIDCMinWalkTimes;
	iters->wMaxWalkTimes=dwIDCMaxWalkTimes;
	iters->wAlertRange=dwIDCAlertRange;
	iters->wPurRange=dwIDCPurRange;
	iters->wPeaMoveRange=dwIDCPeaMoveRange;
	
	
	iters->strAIModel=strIDCAIModel;
	iters->wStayTime=dwIDCStayTime;
	iters->strAIWay=strIDCAIWay;
	//清空原有技能
	iters->vecSkill.clear();
	tagSkill skill;
	//读取listcontrol中的每一行
	for(int i=0;i<m_MonSkillList.GetItemCount();i++)
	{
		//如果是基本技能，技能类型为">"
		if(m_MonSkillList.GetItemText(i,0)=="基本技能")
		skill.strSkillType=">";
		//如果是普通技能，技能类型为"#"
		else if(m_MonSkillList.GetItemText(i,0)=="普通技能")
		skill.strSkillType="#";
		//如果是周期技能，技能类型为"@"
		else if(m_MonSkillList.GetItemText(i,0)=="周期技能")
		skill.strSkillType="@";
		skill.dwSkillID=atoi(m_MonSkillList.GetItemText(i,1));
		skill.wSkillLevel=atoi(m_MonSkillList.GetItemText(i,2));
		skill.wInterTime=atoi(m_MonSkillList.GetItemText(i,3));
		iters->vecSkill.push_back(skill);
	}
	//有相同的记录，返回假
	if(IsSimple())
	{
		return false;
	}
	else 
		return true;
}


void CDetail::OnEnChangeEdit26()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	
}
//新建文件时设置默认值
bool CDetail::SetNewData(vector<tagMonster>*vecMonster,size_t n)
{
	tagMonster monster;

	char strVal[10];
	strIDCNO=itoa((int)n+1,strVal,10);


	CString str1="Index";
	str1=str1+itoa((int)n+1,strVal,10);
	dwIDCIndex=(DWORD)n+1;

	string str="原始名";	
	str=str+itoa((int)n+1,strVal,10);	
	strIDCOrigName=str.c_str();

	strIDCName="未命名";
	strIDCCollection="0";
	strIDCTitle="0";
	dwIDCTileColor=0;
	dwIDCTitlePicture=0;
	strIDCCountryID="1";
	strIDCNpcSort="1";
	dwIDCPicID=0;
	dwIDCPicLevel=0;
	dwIDCNameColorID=0;
	dwIDCLevelColorID=0;
	dwIDCHpColor=0;
	dwIDCSoundID=0;
	
	//基本属性页
	strIDCShape="0";
	nIDCLevel=1;
	dwIDCExp=0;
	dwIDCWorkPoint=0;
	dwIDCMaxHp=0;
	dwIDCMinAttack=0;
	dwIDCMaxAttack=0;
	dwIDCMinMagicAttack=0;
	dwIDCMaxMagicAttack=0;
	dwIDCHitRating=0;
	dwIDCStormHit=0;
	dwIDCMagicStorm=0;
	dwIDCDefense=0;
	dwIDCMagicDefense=0;
	dwIDCBlock=0;
	dwIDCParry=0;
	dwIDCDodge=0;
	fIDCMoveSpd=0.001;			
	fIDCBatMoveSpd=0.002;
	strIDCMonRace="0";
	strIDCElem="0";
	dwIDCVitality=5000;
	dwIDCHpRecoverSpd=0;
	dwIDCMpRecoverSpd=0;
	
	//抗性页
	dwIDCGroundAnti=0;
	dwIDCWaterAnti=0;
	dwIDCFireAnti=0;
	dwIDCWindAnti=0;
	dwIDCSacreAnti=0;
	dwIDCDarkAnti=0;
	dwIDCNotElemAnti=0;
	dwIDCStormAnti=0;
	dwIDCMagicStormAnti=0;
	dwIDCWillAnti=0;
	dwIDCBodyAnti=0;
	dwIDCBloodAnti=0;
	dwIDCDuskAnti=0;
	dwIDCNumbAnti=0;
	dwIDCSleepAnti=0;
	dwIDCDrugAnti=0;
	dwIDCNotMoveAnti=0;
	//伤害页
	dwIDCHurtHuman=0;
	dwIDCHurtElem=0;
	dwIDCHurtDiff=0;
	dwIDCHurtAnimal=0;
	dwIDCHurtDead=0;
	dwIDCHurtDevil=0;
	dwIDCHurtInsect=0;
	dwIDCStormHurt=0;
	dwIDCMagicStormHurt=0;
	dwIDCHurtFire=0;
	dwIDCHurtWater=0;
	dwIDCHurtGround=0;
	dwIDCHurtWind=0;
	dwIDCHurtSacre=0;
	dwIDCHurtDark=0;
	dwIDCHurtNotElem=0;
	//脚本页
	strIDCRefe1="0";
	strIDCRefe2="0";
	strIDCRefe3="0";
	strIDCRefe4="0";
	strIDCRefe5="0";
	strIDCBirthSct="0";
	strIDCBattleSct="0";
	strIDCInjurSct="0";
	strIDCDeadSct="0";
	strIDCCycleSct="0";
	strIDCChoseSkillSct="0";
	strIDCFindTgtSct="0";
	strIDCKillTgtSct="0";
	//其他页
	strIDCBlock="0";
	strIDCSuper="0";
	strIDCTame="0";
	bIDCTalk="0";
	strIDCReturn="1";
	strIDCAllRecover="1";
	dwIDCReDistance=0;
	dwIDCReTime=0;
	fIDCInjurRate=0;
	dwIDCAlertTime=0;
	fIDCEscape=0;
	dwIDCSkillRate=50;
	dwIDCSkillInter=0;
	dwIDCTameTimes=0;
	dwIDCMinStayTimes=0;
	dwIDCMaxStayTimes=0;
	dwIDCMinWalkTimes=0;
	dwIDCMaxWalkTimes=0;
	dwIDCAlertRange=0;
	dwIDCPurRange=0;
	dwIDCPeaMoveRange=0;
	strIDCAIModel="1";
	dwIDCStayTime=0;
	strIDCAIWay="2";
	

	//保存到vector中
	
	monster.nNO=atoi(strIDCNO);


	monster.dwIndex=dwIDCIndex;
	
	monster.strOrigName=strIDCOrigName;

	monster.strName=strIDCName;
	monster.strCollection=strIDCCollection;
	monster.strTitle=strIDCTitle;
	monster.dwTitleColor=dwIDCTileColor;
	monster.dwTitlePicture=dwIDCTitlePicture;
	monster.strCountryID=strIDCCountryID; 
	monster.strNpcSort=strIDCNpcSort;
	monster.dwPicID=dwIDCPicID;
	monster.dwPicLevel=dwIDCPicLevel;
	monster.dwNameColorID=dwIDCNameColorID;
	monster.dwLevelColorID=dwIDCLevelColorID;
	monster.dwHpColor=dwIDCHpColor;
	monster.dwSoundID=dwIDCSoundID;
	
	//基本属性页
	monster.strShape=strIDCShape;
	monster.nLevel=nIDCLevel;
	monster.dwExp=dwIDCExp;
	monster.dwWorkPoint=dwIDCWorkPoint;
	monster.dwMaxHp=dwIDCMaxHp;
	monster.wMinAttack=dwIDCMinAttack;
	monster.wMaxAttack=dwIDCMaxAttack;
	monster.wMinMagicAttack=dwIDCMinMagicAttack;
	monster.wMaxMagicAttack=dwIDCMaxMagicAttack;
	monster.wHitRating=dwIDCHitRating;
	monster.wStormHit=dwIDCStormHit;
	monster.wMagicStorm=dwIDCMagicStorm;
	monster.wDefense=dwIDCDefense;
	monster.wMagicDefense=dwIDCMagicDefense;
	monster.wBlock=dwIDCBlock;
	monster.wParry=dwIDCParry;
	monster.wDodge=dwIDCDodge;
	monster.fMoveSpd=fIDCMoveSpd;			
	monster.fBatMoveSpd=fIDCBatMoveSpd;
	monster.strMonRace=strIDCMonRace;
	monster.strElem=strIDCElem;
	monster.wVitality=dwIDCVitality;
	monster.wHpRecoverSpd=dwIDCHpRecoverSpd;
	monster.wMpRecoverSpd=dwIDCMpRecoverSpd;
	
	//抗性页
	monster.wGroundAnti=dwIDCGroundAnti;;
	monster.wWaterAnti=dwIDCWaterAnti;
	monster.wFireAnti=dwIDCFireAnti;
	monster.wWindAnti=dwIDCWindAnti;
	monster.wSacreAnti=dwIDCSacreAnti;
	monster.wDarkAnti=dwIDCDarkAnti;
	monster.wNotElemAnti=dwIDCNotElemAnti;
	monster.wStormAnti=dwIDCStormAnti;
	monster.wMagicStormAnti=dwIDCMagicStormAnti;
	monster.wWillAnti=dwIDCWillAnti;
	monster.wBodyAnti=dwIDCBodyAnti;
	monster.wBloodAnti=dwIDCBloodAnti;
	monster.wDuskAnti=dwIDCDuskAnti;
	monster.wNumbAnti=dwIDCNumbAnti;
	monster.wSleepAnti=dwIDCSleepAnti;
	monster.wDrugAnti=dwIDCDrugAnti;
	monster.wNotMoveAnti=dwIDCNotMoveAnti;
	//伤害页
	monster.wHurtHuman=dwIDCHurtHuman;
	monster.wHurtElem=dwIDCHurtElem;
	monster.wHurtDiff=dwIDCHurtDiff;
	monster.wHurtAnimal=dwIDCHurtAnimal;
	monster.wHurtDead=dwIDCHurtDead;
	monster.wHurtDevil=dwIDCHurtDevil;
	monster.wHurtInsect=dwIDCHurtInsect;
	monster.wStormHurt=dwIDCStormHurt;
	monster.wMagicStormHurt=dwIDCMagicStormHurt;
	monster.wHurtFire=dwIDCHurtFire;
	monster.wHurtWater=dwIDCHurtWater;
	monster.wHurtGround=dwIDCHurtGround;
	monster.wHurtWind=dwIDCHurtWind;
	monster.wHurtSacre=dwIDCHurtSacre;
	monster.wHurtDark=dwIDCHurtDark;
	monster.wHurtNotElem=dwIDCHurtNotElem;
	//脚本页
	monster.strRefe1=strIDCRefe1;
	monster.strRefe2=strIDCRefe2;
	monster.strRefe3=strIDCRefe3;
	monster.strRefe3=strIDCRefe4;
	monster.strRefe4=strIDCRefe5;
	monster.strBirthSct=strIDCBirthSct;
	monster.strBattleSct=strIDCBattleSct;
	monster.strInjurSct=strIDCInjurSct;
	monster.strDeadSct=strIDCDeadSct;
	monster.strCycleSct=strIDCCycleSct;
	monster.strChoseSkillSct=strIDCChoseSkillSct;
	monster.strFindTgtSct=strIDCFindTgtSct;
	monster.strKillTgtSct=strIDCKillTgtSct;
	//其他页
	monster.strBlock=strIDCBlock;
	monster.strSuper=strIDCSuper;
	monster.strTame=strIDCTame;
	monster.bTalk=atoi(bIDCTalk);
	monster.strReturn=strIDCReturn;
	monster.strAllRecover=strIDCAllRecover;
	monster.wReDistance=dwIDCReDistance;
	monster.wReTime=dwIDCReTime;
	monster.fInjurRate=fIDCInjurRate;
	monster.wAlertTime=dwIDCAlertTime;
	monster.fEscape=fIDCEscape;
	monster.wSkillRate=dwIDCSkillRate=50;
	monster.wSkillRate=dwIDCSkillInter;
	monster.dwTameTimes=dwIDCTameTimes;
	monster.wMinStayTimes=dwIDCMinStayTimes;
	monster.wMaxStayTimes=dwIDCMaxStayTimes;
	monster.wMinWalkTimes=dwIDCMinWalkTimes;
	monster.wMaxWalkTimes=dwIDCMaxWalkTimes;
	monster.wAlertRange=dwIDCAlertRange;
	monster.wPurRange=dwIDCPurRange;
	monster.wPeaMoveRange=dwIDCPeaMoveRange;
	monster.strAIModel=strIDCAIModel;
	monster.wStayTime=dwIDCStayTime;
	monster.strAIWay=strIDCAIWay;
	monster.wDropFlag = 0;
	monster.wBossLevel = 0;
	monster.wImmunity = 0;
	monster.wPierce = 0;
	vector<tagMonster>::iterator iter;

	(*vecMonster).push_back(monster);


	vecMonDetail=vecMonster;
	
	//将新插入元素的指针赋值给全局变量
	for(iter=(*vecMonster).begin();iter!=(*vecMonster).end();iter++)
	{
		iters=iter;
	}	
 	return true;
}
//判断是索引或原始名是否有重复
bool CDetail::IsSimple()
{
	
	vector<tagMonster>::iterator iter;
	//判断是索引或原始名是否有重复
	int n=0;
	for(iter=(*vecMonDetail).begin();iter!=(*vecMonDetail).end();iter++)
	{
		//相同
		if(dwIDCIndex==iter->dwIndex||strIDCOrigName==iter->strOrigName.c_str())
		{
			//索引相同
			if(iter->dwIndex==dwIDCIndex)
			{
				n+=1;
				
			}
			//有两个相同的记录
			if(n>1)
			{
				char strVal[100];
				sprintf_s(strVal,"'%s'，已有的索引号，请修改",DwordToString1(dwIDCIndex));
				MessageBox(strVal,"error");
				return true;
			}
		}
	}
	return false;
}
//关闭详细对话框
void CDetail::OnClose()
{
	
	if(!Save())
	{
		return;
	
	
	}
	CDialog::OnClose();
}
/*功能：单击了listcontrol中的记录，显示信息到相应EDITBOX中
*
*/
void CDetail::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION   pos   =   m_MonSkillList.GetFirstSelectedItemPosition();   
	  if   (pos   ==   NULL)   //未选中 
	  {
		
		   return; 
	  }
	    
      else
      {
		  string str,str1;
		  int i=0;
		  int nItem=0;
		  nItem= m_MonSkillList.GetNextSelectedItem(pos);
		  str1=m_MonSkillList.GetItemText(nItem,1);
		  str=m_MonSkillList.GetItemText(nItem,0);
		  if(str=="基本技能")
		  m_combox.SelectString(0,"基本技能");
		  else if(str=="普通技能")
		  m_combox.SelectString(0,"普通技能");
		  else if(str=="周期技能")
		   m_combox.SelectString(0,"周期技能");
		  dwIDCSkillID=atoi(str1.c_str());
		  //显示信息到editbox
		  GetDlgItem(IDC_EDIT120)->SetWindowText(str1.c_str());
		  GetDlgItem(IDC_EDIT121)->SetWindowText(m_MonSkillList.GetItemText(nItem,2));
		  GetDlgItem(IDC_EDIT122)->SetWindowText(m_MonSkillList.GetItemText(nItem,3));
			
	  }
}
//添加技能
void CDetail::OnAppendSkill()
{
	// TODO: 在此添加控件通知处理程序代码
	//技能总数
	int nNum;
	char strVal[100];
	CString str;
	//获取总行数，每一行表示一个技能，最多为6
	nNum=m_MonSkillList.GetItemCount();

	UpdateData();

	if(dwIDCSkillID==0)
	{
		MessageBox("技能ID不能为空");
		return;
	}
	//如果技能数超过六个,不能添加
	if(nNum<6)
	{

	 m_MonSkillList.InsertItem(nNum,"");
	 int nindex=m_combox.GetCurSel();
	 m_combox.GetLBText(nindex,str);
	 m_MonSkillList.SetItemText(nNum,0,str);
	 m_MonSkillList.SetItemText(nNum,1,itoa(dwIDCSkillID,strVal,10));
	 m_MonSkillList.SetItemText(nNum,2,itoa(dwIDCSkillLevel,strVal,10));
	 m_MonSkillList.SetItemText(nNum,3,itoa(dwIDCInterTime,strVal,10));
	 nNum+=1;
	}
	else
	{
		MessageBox("最多6个技能");
	}
	
}
//修改了技能
void CDetail::OnUpdateSkill()
{
	// TODO: 在此添加控件通知处理程序代码
	//获取当前选中的行的pos
	POSITION   pos   =   m_MonSkillList.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{

		return;
	}
	char strVal[100];
	int i=0;


	UpdateData();
	tagSkill newskill;
	int nItem=0;
	nItem= m_MonSkillList.GetNextSelectedItem(pos);
	//重新显示修改后的记录
	 CString str;
	 int nindex=m_combox.GetCurSel();
	 m_combox.GetLBText(nindex,str);
	 m_MonSkillList.SetItemText(nItem,0,str);
	 m_MonSkillList.SetItemText(nItem,1,itoa(dwIDCSkillID,strVal,10));
	 m_MonSkillList.SetItemText(nItem,2,itoa(dwIDCSkillLevel,strVal,10));
	 m_MonSkillList.SetItemText(nItem,3,itoa(dwIDCInterTime,strVal,10));
}
//删除记录
void CDetail::OnDeleteSkill()
{
	// TODO: 在此添加控件通知处理程序代码
	//获取当前选中的行的pos
	POSITION   pos   =   m_MonSkillList.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{
		
		return;
	}
	int nItem=0;
	int i=0;
	//在listcontrol中删除记录
	nItem= m_MonSkillList.GetNextSelectedItem(pos);
	m_MonSkillList.DeleteItem(nItem);
}

//获取脚本路径
CString CDetail::GetScript()
{
	//合并后另存为文件名
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	FilePathName=dlg.GetPathName();
	return FilePathName;
}
void CDetail::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT107)->SetWindowText(str);
}

void CDetail::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT108)->SetWindowText(str);
}

void CDetail::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT109)->SetWindowText(str);
}

void CDetail::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT110)->SetWindowText(str);
}

void CDetail::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT111)->SetWindowText(str);
}

void CDetail::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT112)->SetWindowText(str);
}

void CDetail::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT119)->SetWindowText(str);
}

void CDetail::OnBnClickedButton8()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT113)->SetWindowText(str);
}

void CDetail::OnBnClickedButton9()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT114)->SetWindowText(str);
}

void CDetail::OnBnClickedButton10()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT115)->SetWindowText(str);
}

void CDetail::OnBnClickedButton11()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT116)->SetWindowText(str);
}

void CDetail::OnBnClickedButton12()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT117)->SetWindowText(str);
}

void CDetail::OnBnClickedButton13()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str=GetScript();
	GetDlgItem(IDC_EDIT118)->SetWindowText(str);
}
