#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "changewnd.h"
#include "dooreditwnd.h"

const char *szChangePointType[] = { "直接切换","随机切换","脚本触发"};

WndPopupDoorEdit::WndPopupDoorEdit(void)
{
	m_bIsPopupCheckFlag = FALSE;
	m_bIsOK = FALSE;
}

WndPopupDoorEdit::~WndPopupDoorEdit(void)
{
	
}

BOOL WndPopupDoorEdit::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	//create control
	RECT rcType = { 10,74,97,211};
	m_pListType = new ui::ListBox;
	m_pListType->Create("SwitchTypeLIST",&rcType,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG,this);
	m_pListType->InsertItem(szChangePointType[0]);
	m_pListType->InsertItem(szChangePointType[1]);
	m_pListType->InsertItem(szChangePointType[2]);

	RECT rcReginoID = { 83,224,193,255};
	m_pEditRegionID = new ui::EditBox;
	m_pEditRegionID->Create("RegionIDEDIT",&rcReginoID,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG,this);

	RECT rcX = { 37,270,96,301};
	m_pEditX = new ui::EditBox;
	m_pEditX->Create("XEDIT",&rcX,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG,this);

	RECT rcY = { 128,271,191,302};
	m_pEditY = new ui::EditBox;
	m_pEditY->Create("YEDIT",&rcY,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG,this);

	RECT rcDirect = { 102,73,192,211};
	m_pListDirect = new ui::ListBox;
    m_pListDirect->Create("DirectLIST",&rcDirect,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG,this);
	char szTemp[32];
	for(int i = 0; i < 8; i++)
	{
		sprintf(szTemp,"%d",i);
		m_pListDirect->InsertItem(szTemp);
	}

	RECT rcOK = { 37,318,97,340};
	m_pBtnOK = new ui::Button;
	m_pBtnOK->Create("确定",&rcOK,FWS_VISIBLE,this);

	RECT rcCancel = { 116,318,176,339};
	m_pBtnCancel = new ui::Button;
	m_pBtnCancel->Create("取消",&rcCancel,FWS_VISIBLE,this);
	
	RECT rcStaticType = { 12, 49, 43 ,80};
	m_pStaticType = new ui::Static;
	m_pStaticType->Create("类型",&rcStaticType,FWS_VISIBLE,this);
	
	RECT rcStaticRegionID = { 30, 223, 61 ,254};
	m_pStaticRegionID = new ui::Static;
	m_pStaticRegionID->Create("地图编号",&rcStaticRegionID,FWS_VISIBLE,this);
	
	RECT rcStaticX = { 7, 266, 38 ,297};
	m_pStaticX = new ui::Static;
	m_pStaticX->Create("X",&rcStaticX,FWS_VISIBLE,this);
	
	RECT rcStaticY = { 101,269,132,300};
	m_pStaticY = new ui::Static;
	m_pStaticY->Create("Y",&rcStaticY,FWS_VISIBLE,this);
	
	RECT rcStaticDirect = { 130,49,134,80};
	m_pStaticDirect = new ui::Static;
	m_pStaticDirect->Create("方向",&rcStaticDirect,FWS_VISIBLE,this);  

	RECT rcEditName = { 61,18,190,43};
	m_pEditName = new ui::EditBox;
	m_pEditName->Create("NameEDIT",&rcEditName,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG,this);

	RECT rcStaticName = { 19,19,61,38};
	m_pStaticName = new ui::Static;
	m_pStaticName->Create("Name",&rcStaticName,FWS_VISIBLE,this);


	return TRUE;
}

void WndPopupDoorEdit::Destroy()
{
	SAFEDESTROY(m_pStaticName);
	SAFEDESTROY(m_pStaticType);
	SAFEDESTROY(m_pStaticRegionID);
	SAFEDESTROY(m_pStaticX);
	SAFEDESTROY(m_pStaticY);
	SAFEDESTROY(m_pStaticDirect);

	SAFEDESTROY(m_pEditName);
	SAFEDESTROY(m_pBtnCancel);
	SAFEDESTROY(m_pBtnOK);
	SAFEDESTROY(m_pListType);
	SAFEDESTROY(m_pEditRegionID);
	SAFEDESTROY(m_pEditX);
	SAFEDESTROY(m_pEditY);
	SAFEDESTROY(m_pListDirect);
	ui::Wnd::Destroy();
}

void WndPopupDoorEdit::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_pBtnOK->IsClicked())
		{
			EndPopUp();
			m_bIsOK = TRUE;
			m_bIsPopupCheckFlag= TRUE;
		}
		if (m_pBtnCancel->IsClicked())
		{
			EndPopUp();
			m_bIsOK = FALSE;
			m_bIsPopupCheckFlag = TRUE;
		}
		ui::Wnd::UpdateChild();
	}
}

void WndPopupDoorEdit::BeginPopUp(void)
{
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}

void WndPopupDoorEdit::EndPopUp()
{
	ModifyStyle(0,FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
}

void WndPopupDoorEdit::GetDoor(tagDoor *pDoor)
{
	m_pEditName->GetEditText(pDoor->szName,63);
	char szTemp[256];
	m_pEditX->GetEditText(szTemp,255);
	pDoor->tSwitch.lCoordX = atoi(szTemp);
	m_pEditY->GetEditText(szTemp,255);
	pDoor->tSwitch.lCoordY = atoi(szTemp);
	m_pEditRegionID->GetEditText(szTemp,255);
	pDoor->tSwitch.lRegionID = atoi(szTemp);
	
	int iDirect = m_pListDirect->GetSelItem();
	if (iDirect < 0)
		iDirect = 0;
	pDoor->tSwitch.lDir = iDirect;

	int iType = m_pListType->GetSelItem();
	if (iType < 0)
		iType = 0;
	pDoor->tSwitch.lState = iType;
}

void WndPopupDoorEdit::SetDoor(const tagDoor *pDoor)
{
	m_pEditName->Clear();
	m_pEditX->Clear();
	m_pEditY->Clear();
	m_pEditRegionID->Clear();

	m_pEditName->InsertText(0,pDoor->szName,63);
	char szTemp[256];   
	m_pEditX->InsertText(0,itoa(pDoor->tSwitch.lCoordX,szTemp,10),255);
	m_pEditY->InsertText(0,itoa(pDoor->tSwitch.lCoordY,szTemp,10),255);
	m_pEditRegionID->InsertText(0,itoa(pDoor->tSwitch.lRegionID,szTemp,10),255);
	m_pListDirect->SetSelItem(pDoor->tSwitch.lDir);
	m_pListType->SetSelItem(pDoor->tSwitch.lState);
	
}