#include <iostream> 
#include <sstream> 
#include <strstream>

#include "stdafx.h"
#include "CSL_GoodsEditor.h"

#include "MainFrm.h"
#include "GoodAttrPage.h"
#include "AllGoodsPage.h"
#include "AllGoodsTree.h"
#include "ContainerPage.h"
#include "Search.h"
#include ".\mainfrm.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



int GoodID=0;
int MaxID=0;
vector<tagGoodAttr> vecGoodAttr;


// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//ON_COMMAND(ID_FILE_SAVE, OnFileSave)	
	ON_UPDATE_COMMAND_UI(ID_NEW, OnUpdateNew)
	ON_UPDATE_COMMAND_UI(ID_SAVE, OnUpdateSave)
	ON_COMMAND(ID_NEW, OnNew)
	ON_COMMAND(ID_SAVE, OnSave)
	ON_COMMAND(ID_CREATEOK, OnCreateok)
	ON_UPDATE_COMMAND_UI(ID_CREATEOK, OnUpdateCreateok)
	ON_UPDATE_COMMAND_UI(ID_EDITOK, OnUpdateEditok)
	ON_COMMAND(ID_32783, On32783)
	ON_WM_CLOSE()
	ON_COMMAND(ID_COUNT_EQUIP, OnCountEquip)
	ON_COMMAND(ID_COUNT_SHOW, OnCountShow)
	ON_COMMAND(ID_COUNT_RANDOM, OnCountRandom)
	ON_COMMAND(ID_COUNT_RANDOMNUM, OnCountRandomnum)
	ON_COMMAND(ID_COUNT_ADD20, OnCountAdd20)
	ON_COMMAND(ID_ADD_TYPEEX, OnAddTypeex)
	ON_COMMAND(ID_ConvertE, &CMainFrame::OnConvertE)
	ON_COMMAND(ID_CheckDataToEduce, &CMainFrame::OnCheckDataToEduce)
	ON_COMMAND(ID_CheckDataToEduceE, &CMainFrame::OnCheckDataToEduceE)
	ON_COMMAND(ID_AddAttrBatAdd, &CMainFrame::OnAddAttrByBatAdd)
	ON_COMMAND(ID_AddArrtBatDel, &CMainFrame::OnAddArrtBatDel)
	ON_COMMAND(ID_EduceOneAttribute, &CMainFrame::OnEduceOneAttribute)
	ON_COMMAND(ID_CalculateAddAttrValue2, &CMainFrame::OnCalculateAddAttrValue2)
	ON_COMMAND(ID_SEARCH, &CMainFrame::OnSearch)
END_MESSAGE_MAP()

// CMainFrame 构造/析构
CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码	
	//LoadGoods();	

	LoadGoodsB();
}

CMainFrame::~CMainFrame()
{
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
								CCreateContext* pContext)
{	
	/*if(!m_wndSplitter.CreateStatic(this,1,2)|| 
	!m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CAllGoodsTree),CSize(200,0),pContext) ||
	!m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CGoodAttrPage),CSize(0,0),pContext))
	return FALSE;*/	

	m_wndSplitter.CreateStatic(this,1,2);	
	m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CAllGoodsTree),CSize(200,0),pContext);
	//m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CGoodAttrPage),CSize(0,0),pContext);
	m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CContainerPage),CSize(0,0),pContext);
	SetTitle("光芒物品编辑");
	//m_wndGoodAttr.Create("FORMVIEW","",WS_CHILD | WS_VISIBLE,CRect(20, 20, 20, 20), this, 1234,NULL);	
	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式	
	return TRUE;
}


// CMainFrame 诊断
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG
// CMainFrame 消息处理程序

void CMainFrame::LoadGoodsB()
{
	FILE* fp;
	fp = fopen("goodslist.dat","rb");
	if( fp == NULL )
	{
		char str[256];
		sprintf_s(str, _T("file '%s' can't found!"), _T("goodslist.dat"));
		MessageBox(str,"error");
		return;
	}
	// 文件头和版本
	char szHeader[6];
	memset(szHeader, 0, 6);
	fread(szHeader, 5, 1, fp);
	if( strcmp(szHeader, "GOODS") != 0 )
	{
		char str[256];
		sprintf_s(str,_T("file '%s' is not goodslist file!"), "goodslist.dat");
		MessageBox(str, "error");		
	}
	long lVersion = 0;
	fread(&lVersion, sizeof(long), 1, fp);
	// 具体内容	

	//物品数量
	long lNum = 0;
	fread(&lNum, sizeof(long), 1, fp);
	for(int i=0; i<lNum; i++)
	{
		long lStrSize=0;
		char *pStr;
		tagGoodAttr stGoods;
		stGoods.dwSilverValue=0;
		stGoods.dwWeaponActType=0;

		fread(&stGoods.dwIndex, sizeof(stGoods.dwIndex), 1, fp);		//	编号

		//	原始名   < 20
		fread(&lStrSize,sizeof(long),1,fp);
		pStr=new char[lStrSize+1];
		fread(pStr,lStrSize,1,fp);
		pStr[lStrSize]='\0';
		stGoods.strOrigName=pStr;
		delete [] pStr;

		//	中文名称 < 20
		fread(&lStrSize,sizeof(long),1,fp);
		pStr=new char[lStrSize+1];
		fread(pStr,lStrSize,1,fp);
		pStr[lStrSize]='\0';
		stGoods.strName=pStr;
		delete [] pStr;

		fread(&stGoods.bSave, sizeof(stGoods.bSave), 1, fp);			//	是否需要存数据库
		fread(&stGoods.dwValue, sizeof(stGoods.dwValue), 1, fp);		//	价格
		fread(&stGoods.dwSilverValue, sizeof(stGoods.dwSilverValue), 1, fp);		//	价格
		//fread(&stGoods.lX_Size, sizeof(stGoods.lX_Size), 1, fp);		//	物品X方向大小
		//fread(&stGoods.lY_Size, sizeof(stGoods.lY_Size), 1, fp);		//	物品y方向大小
		fread(&stGoods.dwType, sizeof(stGoods.dwType), 1, fp);			//	物品类型
		//fread(&stGoods.dwWeight, sizeof(stGoods.dwWeight), 1, fp);		//	重量

		// 客户端专用属性
		fread(&stGoods.dwIconId, sizeof(stGoods.dwIconId), 1, fp);		//	界面图形ID
		fread(&stGoods.dwGroundId, sizeof(stGoods.dwGroundId), 1, fp);		//	落地后图形ID
		fread(&stGoods.dwEquipID, sizeof(stGoods.dwEquipID), 1, fp);		//	换装图形ID
		fread(&stGoods.dwSound, sizeof(stGoods.dwSound), 1, fp);		//	拾取声音
		fread(&stGoods.dwSoundID1, sizeof(stGoods.dwSoundID1), 1, fp);		//	挥动/弱伤害声音
		fread(&stGoods.dwSoundID2, sizeof(stGoods.dwSoundID2), 1, fp);		//	特殊攻击/击中声音
		fread(&stGoods.bSoundSwitch, sizeof(stGoods.bSoundSwitch), 1, fp);	//	攻击时是否混音
		fread(&stGoods.dwWeaponActType, sizeof(stGoods.dwWeaponActType), 1, fp);//	武器对应的动作类型

		//	中文描述 < 64
		fread(&lStrSize,sizeof(long),1,fp);
		pStr=new char[lStrSize+1];
		fread(pStr,lStrSize,1,fp);
		pStr[lStrSize]='\0';
		stGoods.strContent=pStr;
		delete [] pStr;

		// 扩展属性
		long lEffectNum = 0;
		fread(&lEffectNum, sizeof(long), 1, fp);
		for(int j=0; j<lEffectNum; j++)
		{
			//fread(&stEffect,sizeof(tagAddAttr)-sizeof(vector<tagModify>), 1, fp);
			tagAddAttr stEffect;
			fread(&(stEffect.wType),sizeof(WORD),1,fp);
			fread(&(stEffect.bEnable),sizeof(bool),1,fp);
			fread(&(stEffect.bHide),sizeof(bool),1,fp);
			fread(&(stEffect.lValue1),sizeof(DWORD),1,fp);
			fread(&(stEffect.lValue2),sizeof(DWORD),1,fp);
			//fread(&(stEffect.wEnableOdds),sizeof(WORD),1,fp);
			stGoods.vecAddAttr.push_back(stEffect);
		}

		// 套装属性
		lEffectNum = 0;
		fread(&lEffectNum, sizeof(long), 1, fp);
		for(int j=0; j<lEffectNum; j++)
		{
			//fread(&stEffect,sizeof(tagAddAttr)-sizeof(vector<tagModify>), 1, fp);
			tagSuitAttr stEffect;
			fread(&(stEffect.wType),sizeof(WORD),1,fp);			
			fread(&(stEffect.lValue1),sizeof(DWORD),1,fp);
			fread(&(stEffect.lValue2),sizeof(DWORD),1,fp);
			fread(&(stEffect.wSuitNum),sizeof(WORD),1,fp);
			stGoods.vecSuitAttr.push_back(stEffect);
		}

		//add by wang-xx 2009-6-30 begin
		//激发属性
		
		lEffectNum = 0;
		fread(&lEffectNum, sizeof(long), 1, fp);
		for(int j=0; j<lEffectNum; j++)
		{
			tagArouseAttr stEffect;
			fread(&(stEffect.wType),sizeof(WORD),1,fp);			
			fread(&(stEffect.lValue1),sizeof(DWORD),1,fp);
			fread(&(stEffect.lValue2),sizeof(DWORD),1,fp);
			fread(&(stEffect.wArouseCnt),sizeof(WORD),1,fp);
			stGoods.vecArouseAttr.push_back(stEffect);
		}
		//add by wang-xx 2009-6-30 end

		vecGoodAttr.push_back(stGoods);
	}
	fclose(fp);	
}

// 保存到文件
void CMainFrame::SaveGoodsB()
{
	FILE* fp;
	fp = fopen(_T("goodslist.dat"), "wb");
	if( fp == NULL )
	{
		char str[256];
		sprintf_s(str, _T("file '%s' can't write!"), "goodslist.dat");
		MessageBox(str, "error");	
		return;
	}
	// 文件头和版本
	fwrite("GOODS", 5, 1, fp);
	long lVersion = 1;
	fwrite(&lVersion, sizeof(long), 1, fp);
	// 具体内容

	//物品数量
	long lNum = (long)vecGoodAttr.size();
	fwrite(&lNum, sizeof(long), 1, fp);
	for(vector<tagGoodAttr>::iterator it=vecGoodAttr.begin(); it!=vecGoodAttr.end(); it++)
	{
		tagGoodAttr *pstGoodAttr = &(*it);

		fwrite(&pstGoodAttr->dwIndex, sizeof(pstGoodAttr->dwIndex), 1, fp);		//	编号

		lNum=long(pstGoodAttr->strOrigName.size());
		fwrite(&lNum,sizeof(long),1,fp);
		fwrite(pstGoodAttr->strOrigName.c_str(),lNum,1,fp);//原始名

		lNum=long(pstGoodAttr->strName.size());
		fwrite(&lNum,sizeof(long),1,fp);
		fwrite(pstGoodAttr->strName.c_str(),lNum,1,fp);//中文名

		fwrite(&pstGoodAttr->bSave, sizeof(pstGoodAttr->bSave), 1, fp);			//	是否需要存数据库
		fwrite(&pstGoodAttr->dwValue, sizeof(pstGoodAttr->dwValue), 1, fp);		//	价格
		fwrite(&pstGoodAttr->dwSilverValue, sizeof(pstGoodAttr->dwSilverValue), 1, fp);		//	价格

		//fwrite(&pstGoodAttr->lX_Size, sizeof(pstGoodAttr->lX_Size), 1, fp);		//	物品X方向大小
		//fwrite(&pstGoodAttr->lY_Size, sizeof(pstGoodAttr->lY_Size), 1, fp);		//	物品y方向大小
		fwrite(&pstGoodAttr->dwType, sizeof(pstGoodAttr->dwType), 1, fp);			//	物品类型
		//fwrite(&pstGoodAttr->dwWeight, sizeof(pstGoodAttr->dwWeight), 1, fp);		//	重量

		// 客户端专用属性
		fwrite(&pstGoodAttr->dwIconId, sizeof(pstGoodAttr->dwIconId), 1, fp);		//	界面图形ID
		fwrite(&pstGoodAttr->dwGroundId, sizeof(pstGoodAttr->dwGroundId), 1, fp);		//	落地后图形ID
		fwrite(&pstGoodAttr->dwEquipID, sizeof(pstGoodAttr->dwEquipID), 1, fp);		//	换装图形ID
		fwrite(&pstGoodAttr->dwSound, sizeof(pstGoodAttr->dwSound), 1, fp);		//	拾取声音
		fwrite(&pstGoodAttr->dwSoundID1, sizeof(pstGoodAttr->dwSoundID1), 1, fp);		//	挥动/弱伤害声音
		fwrite(&pstGoodAttr->dwSoundID2, sizeof(pstGoodAttr->dwSoundID2), 1, fp);		//	特殊攻击/击中声音
		fwrite(&pstGoodAttr->bSoundSwitch, sizeof(pstGoodAttr->bSoundSwitch), 1, fp);	//	攻击时是否混音
		fwrite(&pstGoodAttr->dwWeaponActType, sizeof(pstGoodAttr->dwWeaponActType), 1, fp);//	武器对应的动作类型

		lNum=long(pstGoodAttr->strContent.size());
		fwrite(&lNum,sizeof(long),1,fp);
		fwrite(pstGoodAttr->strContent.c_str(),lNum,1,fp);//描述

		// 扩展属性
		lNum = (long)pstGoodAttr->vecAddAttr.size();
		fwrite(&lNum, sizeof(long), 1, fp);
		for(int i=0; i<lNum; i++)
		{
			fwrite(&(pstGoodAttr->vecAddAttr[i].wType),sizeof(WORD),1,fp);
			fwrite(&(pstGoodAttr->vecAddAttr[i].bEnable),sizeof(bool),1,fp);
			fwrite(&(pstGoodAttr->vecAddAttr[i].bHide),sizeof(bool),1,fp);
			fwrite(&(pstGoodAttr->vecAddAttr[i].lValue1),sizeof(DWORD),1,fp);
			fwrite(&(pstGoodAttr->vecAddAttr[i].lValue2),sizeof(DWORD),1,fp);
			//fwrite(&(pstGoodAttr->vecAddAttr[i].wEnableOdds),sizeof(WORD),1,fp);
		}

		//套装属性
		lNum = (long)pstGoodAttr->vecSuitAttr.size();
		fwrite(&lNum, sizeof(long), 1, fp);
		for(int i=0;i<lNum;i++)
		{
			fwrite(&(pstGoodAttr->vecSuitAttr[i].wType),sizeof(WORD),1,fp);			
			fwrite(&(pstGoodAttr->vecSuitAttr[i].lValue1),sizeof(DWORD),1,fp);
			fwrite(&(pstGoodAttr->vecSuitAttr[i].lValue2),sizeof(DWORD),1,fp);
			fwrite(&(pstGoodAttr->vecSuitAttr[i].wSuitNum),sizeof(WORD),1,fp);
		}

		//add by wang-xx 2009-6-30 begin
		//激发属性
		lNum = (long)pstGoodAttr->vecArouseAttr.size();
		fwrite(&lNum, sizeof(long), 1, fp);
		for(int i=0;i<lNum;i++)
		{
			fwrite(&(pstGoodAttr->vecArouseAttr[i].wType),sizeof(WORD),1,fp);			
			fwrite(&(pstGoodAttr->vecArouseAttr[i].lValue1),sizeof(DWORD),1,fp);
			fwrite(&(pstGoodAttr->vecArouseAttr[i].lValue2),sizeof(DWORD),1,fp);
			fwrite(&(pstGoodAttr->vecArouseAttr[i].wArouseCnt),sizeof(WORD),1,fp);
		}
		//add by wang-xx 2009-6-30 end
	}
	fclose(fp);	
}

// 保存到文件client
void CMainFrame::SaveGoodsClient()
{
	FILE* fp;
	fp = fopen(_T("goods.dat"), "wb");
	if( fp == NULL )
	{
		char str[256];
		sprintf_s(str, _T("file '%s' can't write!"), _T("goods.dat"));
		MessageBox(str, "error");	
		return;
	}
	// 文件头和版本
	fwrite("GOODS", 5, 1, fp);
	long lVersion = 1;
	fwrite(&lVersion, sizeof(long), 1, fp);
	// 具体内容

	//物品数量
	long lNum = (long)vecGoodAttr.size();
	fwrite(&lNum, sizeof(long), 1, fp);
	for(vector<tagGoodAttr>::iterator it=vecGoodAttr.begin(); it!=vecGoodAttr.end(); it++)
	{
		tagGoodAttr *pstGoodAttr = &(*it);

		fwrite(&pstGoodAttr->dwIndex, sizeof(pstGoodAttr->dwIndex), 1, fp);		//	编号

		lNum=long(pstGoodAttr->strOrigName.size());
		fwrite(&lNum,sizeof(long),1,fp);
		fwrite(pstGoodAttr->strOrigName.c_str(),lNum,1,fp);//原始名

		lNum=long(pstGoodAttr->strName.size());
		fwrite(&lNum,sizeof(long),1,fp);
		fwrite(pstGoodAttr->strName.c_str(),lNum,1,fp);//中文名

		// 客户端专用属性
		fwrite(&pstGoodAttr->dwIconId, sizeof(pstGoodAttr->dwIconId), 1, fp);		//	界面图形ID
		fwrite(&pstGoodAttr->dwGroundId, sizeof(pstGoodAttr->dwGroundId), 1, fp);		//	落地后图形ID
		fwrite(&pstGoodAttr->dwEquipID, sizeof(pstGoodAttr->dwEquipID), 1, fp);		//	换装图形ID
		fwrite(&pstGoodAttr->dwSound, sizeof(pstGoodAttr->dwSound), 1, fp);		//	拾取声音
		fwrite(&pstGoodAttr->dwSoundID1, sizeof(pstGoodAttr->dwSoundID1), 1, fp);		//	挥动/弱伤害声音
		fwrite(&pstGoodAttr->dwSoundID2, sizeof(pstGoodAttr->dwSoundID2), 1, fp);		//	特殊攻击/击中声音
		fwrite(&pstGoodAttr->bSoundSwitch, sizeof(pstGoodAttr->bSoundSwitch), 1, fp);	//	攻击时是否混音
		fwrite(&pstGoodAttr->dwWeaponActType, sizeof(pstGoodAttr->dwWeaponActType), 1, fp);//	武器对应的动作类型
		fwrite(&pstGoodAttr->dwType, sizeof(pstGoodAttr->dwType), 1, fp);			//	物品类型
		fwrite(&pstGoodAttr->dwValue,sizeof(pstGoodAttr->dwValue),1,fp);
		fwrite(&pstGoodAttr->dwSilverValue,sizeof(pstGoodAttr->dwSilverValue),1,fp);
		//fwrite(&pstGoodAttr->dwWeight, sizeof(pstGoodAttr->dwWeight), 1, fp);		//	重量

		lNum=long(pstGoodAttr->strContent.size());
		fwrite(&lNum,sizeof(long),1,fp);
		fwrite(pstGoodAttr->strContent.c_str(),lNum,1,fp);//描述

		//基本附加属性
		lNum = (long)pstGoodAttr->vecAddAttr.size();
		fwrite(&lNum, sizeof(long), 1, fp);
		for(int i=0; i<lNum; i++)
		{
			fwrite(&(pstGoodAttr->vecAddAttr[i].wType),sizeof(WORD),1,fp);
			fwrite(&(pstGoodAttr->vecAddAttr[i].bEnable),sizeof(bool),1,fp);
			fwrite(&(pstGoodAttr->vecAddAttr[i].bHide),sizeof(bool),1,fp);
			fwrite(&(pstGoodAttr->vecAddAttr[i].lValue1),sizeof(DWORD),1,fp);
			fwrite(&(pstGoodAttr->vecAddAttr[i].lValue2),sizeof(DWORD),1,fp);
			//fwrite(&(pstGoodAttr->vecAddAttr[i].wEnableOdds),sizeof(WORD),1,fp);
		}

		//套装属性
		lNum = (long)pstGoodAttr->vecSuitAttr.size();
		fwrite(&lNum, sizeof(long), 1, fp);
		for(int i=0;i<lNum;i++)
		{
			fwrite(&(pstGoodAttr->vecSuitAttr[i].wType),sizeof(WORD),1,fp);			
			fwrite(&(pstGoodAttr->vecSuitAttr[i].lValue1),sizeof(DWORD),1,fp);
			fwrite(&(pstGoodAttr->vecSuitAttr[i].lValue2),sizeof(DWORD),1,fp);
			fwrite(&(pstGoodAttr->vecSuitAttr[i].wSuitNum),sizeof(WORD),1,fp);
		}

		//add by wang-xx 2009-6-30 begin
		//激发属性
		lNum = (long)pstGoodAttr->vecArouseAttr.size();
		fwrite(&lNum, sizeof(long), 1, fp);
		for(int i=0;i<lNum;i++)
		{
			fwrite(&(pstGoodAttr->vecArouseAttr[i].wType),sizeof(WORD),1,fp);			
			fwrite(&(pstGoodAttr->vecArouseAttr[i].lValue1),sizeof(DWORD),1,fp);
			fwrite(&(pstGoodAttr->vecArouseAttr[i].lValue2),sizeof(DWORD),1,fp);
			fwrite(&(pstGoodAttr->vecArouseAttr[i].wArouseCnt),sizeof(WORD),1,fp);
		}
		//add by wang-xx 2009-6-30 end
	}
	fclose(fp);	
}

void CMainFrame::OnUpdateNew(CCmdUI *pCmdUI)
{
	CContainerPage *pContainer=(CContainerPage*)m_wndSplitter.GetPane(0,1);
	if(pContainer->m_wndGoodAttrPage.GetAddFlag())
		pCmdUI->Enable(false);
	else
		pCmdUI->Enable(true);
}

void CMainFrame::OnUpdateSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);
}

void CMainFrame::OnNew()
{
	// TODO: 在此添加命令处理程序代码
	CContainerPage *pContainer=(CContainerPage*)m_wndSplitter.GetPane(0,1);
	CAllGoodsTree *pGoodsTree=(CAllGoodsTree*)m_wndSplitter.GetPane(0,0);
	pContainer->m_wndGoodAttrPage.SetEditFlag(false);	
	pContainer->m_wndGoodAttrPage.ClearCtrls();
	pContainer->m_wndGoodAttrPage.ClearAttr();
	pContainer->m_wndAllGoodsPage.ShowWindow(SW_HIDE);
	pContainer->m_wndGoodAttrPage.ShowWindow(SW_NORMAL);	
	tagGoodAttr stGoodAttr;
	int nMax=0;
	vector<tagGoodAttr>::iterator iter;
	for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++)
	{
		if(nMax<(int)iter->dwIndex)
			nMax=iter->dwIndex;
	}
	nMax++;
	char strTem[10];
	itoa(nMax,strTem,10);
	stGoodAttr.dwIndex=nMax;
	char strOrig[25];
	sprintf_s(strOrig,_T("原始名%d"),stGoodAttr.dwIndex);
	stGoodAttr.strOrigName=strOrig;
	stGoodAttr.strName="未命名";
	stGoodAttr.bSave=false;
	stGoodAttr.dwEquipID=0;
	stGoodAttr.dwGroundId=0;
	stGoodAttr.dwValue=0;
	stGoodAttr.dwSilverValue=0;
	stGoodAttr.dwSound=0;
	stGoodAttr.dwType=0;
	stGoodAttr.dwIconId=0;
	stGoodAttr.dwSoundID1=0;
	stGoodAttr.dwSoundID2=0;
	stGoodAttr.bSoundSwitch=false;
	stGoodAttr.dwWeaponActType=0;
	//stGoodAttr.dwWeight=0;
	//stGoodAttr.lX_Size=0;
	//stGoodAttr.lY_Size=0;
	stGoodAttr.strContent="";
	vecGoodAttr.push_back(stGoodAttr);

	char strVal[500];   
	int nCurItem=pContainer->m_wndAllGoodsPage.m_cAllGoodsList.InsertItem(0xffff,"");
	int nCnt=pContainer->m_wndAllGoodsPage.m_cAllGoodsList.GetItemCount();
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItemData(nCurItem,nCnt);
	sprintf_s(strVal,_T("%d"),stGoodAttr.dwIndex);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,0,LVIF_TEXT,strVal,NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,1,LVIF_TEXT,strOrig,NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,2,LVIF_TEXT,"未命名",NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,3,LVIF_TEXT,"否",NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,4,LVIF_TEXT,"0",NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,5,LVIF_TEXT,"0",NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,6,LVIF_TEXT,"0",NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,7,LVIF_TEXT,"0",NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,8,LVIF_TEXT,"0",NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,9,LVIF_TEXT,"0",NULL,0,0,0);	
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,11,LVIF_TEXT,"0",NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,12,LVIF_TEXT,"0",NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,13,LVIF_TEXT,"0",NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItem(nCurItem,14,LVIF_TEXT," ",NULL,0,0,0);
	pContainer->m_wndAllGoodsPage.m_cAllGoodsList.SetItemState(nCurItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);


	pContainer->m_wndGoodAttrPage.SetVecIndex((int)(vecGoodAttr.size()-1));
	pGoodsTree->m_cGoodsTree.Expand(pGoodsTree->m_hTreeRoot,TVE_EXPAND);
	HTREEITEM newItem;
	TV_INSERTSTRUCT stItem;
	stItem.hParent=pGoodsTree->m_cGoodsTree.GetRootItem();
	stItem.hInsertAfter=TVI_LAST;
	stItem.item.pszText=strOrig;
	stItem.item.mask=TVIF_TEXT| TVIF_PARAM;
	stItem.item.lParam=stGoodAttr.dwIndex;
	newItem=pGoodsTree->m_cGoodsTree.InsertItem(&stItem);
	pGoodsTree->SetNewItemFlag(true);
	pGoodsTree->m_cGoodsTree.SelectItem(newItem);
	pGoodsTree->SetNewItemFlag(false);
	pContainer->m_wndGoodAttrPage.m_cIDLab.SetWindowText(strTem);
}

void CMainFrame::OnSave()
{
	// TODO: 在此添加命令处理程序代码
	SaveGoodsB();
	SaveGoodsClient();
	//	SaveGoods();
}

void CMainFrame::OnCreateok()
{
	// TODO: 在此添加命令处理程序代码
	char strID[100],strName[500],strUIID[500],strGroundID[500],strCosplay[500],
		strPrice[500],/*strXSize[500],strYSize[500],strWeight[500],*/strSound[500],strDesc[5000];
	CContainerPage *pContainer=(CContainerPage*)m_wndSplitter.GetPane(0,1);	

	pContainer->m_wndGoodAttrPage.m_tagGoodAttr.dwIndex=++GoodID;//
	sprintf_s(strID,_T("%d"),GoodID);
	pContainer->m_wndGoodAttrPage.m_cNameEdit.GetWindowText(strName,500);
	pContainer->m_wndGoodAttrPage.m_tagGoodAttr.strName=strName;//1
	pContainer->m_wndGoodAttrPage.m_tagGoodAttr.bSave=pContainer->m_wndGoodAttrPage.m_cSaveDBCombo.GetCurSel()==1?true:false;//2
	pContainer->m_wndGoodAttrPage.m_cUIEdit.GetWindowText(strUIID,500);
	pContainer->m_wndGoodAttrPage.m_tagGoodAttr.dwIconId=atoi(strUIID);//3
	pContainer->m_wndGoodAttrPage.m_cGroundEdit.GetWindowText(strGroundID,500);
	pContainer->m_wndGoodAttrPage.m_tagGoodAttr.dwGroundId=atoi(strGroundID);//4
	pContainer->m_wndGoodAttrPage.m_cCosplayEdit.GetWindowText(strCosplay,500);
	pContainer->m_wndGoodAttrPage.m_tagGoodAttr.dwEquipID=atoi(strCosplay);//5
	pContainer->m_wndGoodAttrPage.m_cPriceEdit.GetWindowText(strPrice,500);
	pContainer->m_wndGoodAttrPage.m_tagGoodAttr.dwValue=atoi(strPrice);
	//pContainer->m_wndGoodAttrPage.m_cXSizeEdit.GetWindowText(strXSize,500);
	//pContainer->m_wndGoodAttrPage.m_tagGoodAttr.lX_Size=atoi(strXSize);
	//pContainer->m_wndGoodAttrPage.m_cYSizeEdit.GetWindowText(strYSize,500);
	//pContainer->m_wndGoodAttrPage.m_tagGoodAttr.lY_Size=atoi(strYSize);	
	pContainer->m_wndGoodAttrPage.m_tagGoodAttr.dwType=pContainer->m_wndGoodAttrPage.m_cGoodType.GetCurSel();
	pContainer->m_wndGoodAttrPage.m_cSoundEdit.GetWindowText(strSound,500);
	pContainer->m_wndGoodAttrPage.m_tagGoodAttr.dwSound=atoi(strSound);
	//pContainer->m_wndGoodAttrPage.m_cWeightEdit.GetWindowText(strWeight,500);
	//pContainer->m_wndGoodAttrPage.m_tagGoodAttr.dwWeight=atoi(strWeight);
	pContainer->m_wndGoodAttrPage.m_cDescEdit.GetWindowText(strDesc,5000);
	pContainer->m_wndGoodAttrPage.m_tagGoodAttr.strContent=strDesc;
	vecGoodAttr.push_back(pContainer->m_wndGoodAttrPage.m_tagGoodAttr);	
	pContainer->m_wndGoodAttrPage.ShowWindow(SW_HIDE);
	pContainer->m_wndAllGoodsPage.ShowWindow(SW_NORMAL); 

}

void CMainFrame::OnUpdateCreateok(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CContainerPage *pContainer=(CContainerPage*)m_wndSplitter.GetPane(0,1);
	if(pContainer->m_wndGoodAttrPage.GetAddFlag())
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnUpdateEditok(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(true);
}

void CMainFrame::On32783()
{
	// TODO: 在此添加命令处理程序代码
}

void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int nRet=MessageBox("离开前要保存吗?","",MB_YESNO);
	if(nRet==IDYES)
		SaveGoodsB();
	CFrameWnd::OnClose();
}

//普通装备：
//
//原始名和ID均由“大类别”、“等级”、“颜色”、“类型”4部分组成。
//
//原始名中“大类别”由1位以上的字母组成。
//原始名中“等级”由2位数字组成，低于10的十位由0填补。
//原始名中“颜色”由1位字母组成。
//原始名中“类型”由2位数字组成，低于10的十位由0填补。
//
//剑类装备的大类别为“S”，等级10装备的等级部分为“01”，绿色2装备的“颜色”为“E”，珍贵类型的装备的“类型”为“03”。则一件10级绿2珍贵的剑的原始名为“S01E03”。
//
//ID中“大类别”、“等级”、“颜色”、“类型”4部分均为2位数字，每件装备的ID都是长度刚好8位的数值。
//
//
//原始名与ID的对应：（=前是原始名，=后是ID）
//大类别：
//S = 10 ， K = 11 ， B = 12 ， C = 13 ， W = 14
//WA = 15 ， WH = 16 ， WG = 17 ， WB = 18
//HA = 19 ， HH = 20 ， HG = 21 ， HB = 22
//TA = 23 ， TH = 24 ， TG = 25 ， TB = 26
//AMU = 27 ， RING = 28
//等级：
//直接转换，如原始名中等级05转为ID仍为05
//颜色：
//B = 01 ， C = 02 ， D = 03 ， E = 04 ， F = 05 ，G = 06 ，H = 07 ， I = 08
//类型：
//直接转换。
//
//
//例：60级橙2神器项链的原始名为“AMU06I04”，转换为ID则为“27060804”。
//
//
//
//
//未鉴定装备：
//
//未鉴定装备原始名和ID均由“首位标识”、“大类别”、“等级”、“颜色”4部分组成。
//
//原始名首位标识均为“U”。
//原始名中“大类别”由1位字母组成。
//原始名中“等级”由2位数字组成，低于10的十位由0填补。
//原始名中“颜色”由1位字母组成。
//原始名长度均为5。
//
//ID首位标识均为1。
//ID中“大类别”、“等级”、“颜色”3部分均为2位数字，每件装备的ID都是长度7位的数值。
//
//原始名与ID的对应：（=前是原始名，=后是ID）
//首位标识：
//U = 1
//大类别：
//S = 10 ， K = 11 ， B = 12 ， C = 13 ， W = 14
//A = 15 ， H = 16 ， G = 17 ， X = 18
//J = 19 ， R = 20
//等级：
//直接转换，如原始名中等级05转为ID仍为05
//颜色：
//A = 01 ， B = 02 ， C = 03 ，D = 04	
//
//例：未鉴定的60级橙2项链的原始名为“UJ06D”，转换为ID则为“1190604”。

bool CountEquipLevelLimit(const char* strOrginName, tagAddAttr *pAttr)
{
	if (pAttr && pAttr->wType == 6)	// 等级限制
	{
		char strRet[256];
		memset(strRet,0,256);

		char strName[256];
		strcpy(strName, strOrginName);

		// 未鉴定装备
		if (strName[0] == 'U')
		{
			strRet[0] = '1';

			char TYPE[][2] = {
				'S', 10, 'K', 11, 'B', 12, 'C', 13, 'W', 14,
				'A', 15, 'H', 16, 'G', 17, 'X', 18,
				'J', 19, 'R', 20
			};

			// 类型
			bool bVaild = false;
			for (int i=0; i<sizeof(TYPE)/2; i++)
			{
				if (strName[1] == TYPE[i][0])
				{
					sprintf(&strRet[1], _T("%02d"), TYPE[i][1]);
					bVaild = true;
					break;
				}
			}

			if (bVaild==false)	return false;	// 不符合规则

			// 等级
			strRet[3] = strName[2];
			strRet[4] = strName[3];

			// 颜色
			bVaild = false;
			char COLOR[][2] = {
				'A', 01, 'B', 02, 'C', 03, 'D', 04
			};

			for (int i=0; i<sizeof(COLOR)/2; i++)
			{
				if (strName[4] == COLOR[i][0])
				{
					sprintf(&strRet[5], _T("%02d"), COLOR[i][1]);
					bVaild = true;
					break;
				}
			}

			if (bVaild==false)	return false;	// 不符合规则

			pAttr->lValue2 = atoi(strRet);
		}
		else	// 普通装备
		{
			char *TYPE[] = {
				"S", "K", "B", "C", "W",
				"WA", "WH", "WG", "WB",
				"HA", "HH", "HG", "HB",
				"TA", "TH", "TG", "TB",
				"AMU", "RING"
			};
			long VALUE[] = {
				10, 11, 12, 13, 14,
				15, 16, 17, 18,
				19, 20, 21, 22,
				23, 24, 25, 26,
				27, 28
			};

			// 寻找第一个字符串
			char strType[256];
			memset(strType, 0, 256);
			int pos = 0;
			for (int i=0; i<lstrlen(strName); i++)
			{
				if (strName[i] >= '0' && strName[i] <= '9')
				{
					pos = i;
					break;
				}
				else
					strType[i] = strName[i];
			}

			// 类型
			bool bVaild = false;
			for (int i=0; i<19; i++)
			{
				if ( strcmp(strType, TYPE[i]) == 0)
				{
					sprintf(&strRet[0], _T("%02d"), VALUE[i]);
					bVaild = true;
					break;
				}
			}

			if (bVaild==false)	return false;	// 不符合规则

			// 等级
			strRet[2] = strName[pos++];
			strRet[3] = strName[pos++];

			// 颜色
			bVaild = false;
			char COLOR[][2] = {
				'S', 0, 'A', 01, 'B', 02, 'C', 03, 'D', 04, 'E', 05,
				'F', 06, 'G', 07, 'H', 8, 'I', 9
			};

			for (int i=0; i<sizeof(COLOR)/2; i++)
			{
				if (strName[pos] == COLOR[i][0])
				{
					sprintf(&strRet[4], _T("%02d"), COLOR[i][1]);
					bVaild = true;
					break;
				}
			}
			pos++;

			if (bVaild==false)	return false;	// 不符合规则

			// 类型
			strRet[6] = strName[pos++];
			strRet[7] = strName[pos++];

			// 不符合规则
			if (strName[pos])
				return false;

			pAttr->lValue2 = atoi(strRet);
		}
		return true;
	}

	return false;
}

// 重新计算所有装备的等级附加值
void CMainFrame::OnCountEquip()
{/*
 vector<tagGoodAttr>::iterator iter;
 vector<tagAddAttr>::iterator iterAttr;
 vector<tagModify>::iterator iterModify;

 int nTem=0;
 for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++)
 {
 tagGoodAttr *pGoods = &(*iter);
 if (pGoods && pGoods->dwType >= 2 )	// 装备类
 {
 for(iterAttr=iter->vecAddAttr.begin();iterAttr!=iter->vecAddAttr.end();iterAttr++)
 {
 tagAddAttr *pAttr = &(*iterAttr);
 CountEquipLevelLimit(pGoods->strOrigName.c_str(), pAttr);
 }
 }
 }*/
}

bool ReadTo(istream& stream, const char* name)
{
	string str;

	stream >> str;
	while (str != name)
	{
		if (stream.eof())
		{
			return false;
		}		
		stream >> str;
	}
	return true;
}

struct tagConvert
{
	//	string strSign;					//标识符 #
	string strOrgName;				// 原始名
	string strShowName;				// 显示名
	string lGraphicID;				// 图形ID
	string lGroundID;				// 落地ID
	string lEquipID;				// 换装ID
	string lPrice;					// 金币
	string lSilverPrice;			//银币价格
	string strContent;				//	中文描述 < 64
	string lLevelRest;				//等级限制
	string lRunScript;				//执行脚本
	string lColor;					//显示颜色
	string lSound;					//拾取声音

	string lClass;					//分类
	string lStudySkillID;			//	学习技能id
};

struct tagConvertE 
{
	string	OrgName;				//原始名
	string	WeaponStrong;			//武器强度
	string	WeaponStringAdd;		//武器强度成长
	string	MinAtk;					//最小攻击力
	string	MaxAtk;					//最大攻击力
	string	MoGong;					//魔法攻击
	string	WuGongSockPro;			//物理致命一击率
	string	WuBaoJiDamage;			//物理暴击伤害
	string	WuBaoJiFastness;		//物理暴击抗性
	string	MoGongSockPro;			//魔法致命一击率
	string	MoBaoJiDamage;			//魔法暴击伤害
	string	MoBaoJiFastness;		//魔法暴击抗性
	string	WuRecovery;				//物理防御
	string	MoRecovery;				//魔法防御
	string	AsunderValue1;			//分解价值1
	string	GoodsChar;				//物品品质
	string	ForceAdd;				//力量追加
	string	MinJieAdd;				//敏捷追加
	string	TiZhiAdd;				//体制追加
	string	MindAdd;				//智力追加
	string	ZhiHuiAdd;				//智慧追加
	string	EnergyAdd;				//精神追加
	string	MingZhongRevise;		//命中能力修正
	string	DuckRevise;				//闪避能力修正
	string	GeDangRevise;			//格挡能力修正
	string	ZhaoJiaRevise;			//招架能力修正
	string	MaxChongNeng;			//充能修正	
	string	WuImmunity;				//物理免伤值
	string	MoImmunity;				//魔法免伤值;
	string	GoodsUpPrice;			//物品升级价格
	string	BasicAperValue1;		//基本孔数值1
	string	BasicAperValue2;		//基本孔数值2
	string	AsunderValue2;			//分解价值2
	string	LianDuValue2;			//练度值2
	string	DecomBlueCrystalAmount;					//分解蓝色晶体数
	string	MainShang;				//免伤
	string	Pierce;					//穿透
	string	Const;					//星座
	string	Lucky;					//幸运
	string	AtkValueED;				//攻击ED值
	string	CurAkkValueED;			//当前攻击值
	string	RecoveryValueED;		//防御ed值
	string	CurRecoveryValueED;		//当前防御ed

	string	RecoveryValueED2;		//防御ed值2
	string	AtkValueED2;			//攻击ed值2
	string	EquipDengJie;				//等阶

	string	PhysMinAtkUp;			//	物理小攻成长
	string	PyhsMaxAtkUp;			//	物理大攻成长
	string	MagicAtkUp;				//	魔法攻击成长
	string	PhysRecoUp;				//	物理防御成长
	string	MagicRecoUp;			//	魔法防御成长

	string	SuperLifeMaxUp;			// 追加生命上限
	string	ModifyLifeMaxUp;		// 生命上限修正

	string	MinGongAdd;				//	小攻追加
	string	MaxGongAdd;				//	大攻追加
	string	MagicGongAdd;			//	墨攻追加
	string	PhysFangAdd;			//	物防追加
	string	MaigcFangAdd;			//	魔方追加
};

vector<tagConvert> g_vConvert;
vector<tagConvertE> vecConvertE;
//
//bool CMainFrame::CountConvetEGoodsProperty(tagGoodAttr* pGoods)
//{
//	for ( int i=0; i< (int)vecConvertE.size();i++)
//	{
//		if (pGoods->strOrigName == vecConvertE[i].OrgName)
//		{
//			vector<tagAddAttr>::iterator it=pGoods->vecAddAttr.begin();
//			for(;it!=pGoods->vecAddAttr.end();it++)
//			{
//				tagAddAttr *pAddAttr = &(*it);
//				string str = "";
//				const char * temp = "\\";
//				WORD wType = pAddAttr->wType;
//				//检查该道具的每个附加属性是否存在
//				if ( wType == 12 )												//武器强度
//				{
//					str = vecConvertE[i].WeaponStrong;
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( wType == 137 )										//武器强度成长			
//				{
//					str = vecConvertE[i].WeaponStringAdd;	
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( wType == 13) 											//最小攻击力
//				{
//					str = vecConvertE[i].MinAtk;
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 14 == wType )											//最大攻击力
//				{
//					str = vecConvertE[i].MaxAtk;
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 22 == wType )											//魔法攻击
//				{
//					str = vecConvertE[i].MoGong;
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 18 == wType )											//物理致命一击率
//				{
//					str = vecConvertE[i].WuGongSockPro;	
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}						
//				else if ( 120 == wType )										//物理暴击伤害
//				{
//					str = vecConvertE[i].WuBaoJiDamage;	
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}						
//				else if ( 121 == wType )										//物理暴击抗性
//				{
//					str = vecConvertE[i].WuBaoJiFastness;
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}	
//				else if ( 23 == wType )											//魔法致命一击率
//				{
//					str = vecConvertE[i].MoGongSockPro;	
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}						
//				else if ( 122 == wType )										//魔法暴击伤害
//				{
//					str = vecConvertE[i].MoBaoJiDamage;	
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}	
//				else if ( 123 == wType )										//魔法暴击抗性
//				{
//					str = vecConvertE[i].MoBaoJiFastness;
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}	
//				else if ( 15 == wType )											//物理防御
//				{
//					str = vecConvertE[i].WuRecovery;	
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}		
//				else if ( 26 == wType )											//魔法防御
//				{
//					str = vecConvertE[i].MoRecovery;	
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}		
//				else if ( 125 == wType )										//分解价值
//				{
//					str = vecConvertE[i].AsunderValue1;
//					if( strcmp(str.c_str(),"\\") )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					str = vecConvertE[i].AsunderValue2;
//					if( strcmp(str.c_str(),"\\") )
//						pAddAttr->lValue2 = atoi(str.c_str());
//					continue;
//				}			
//				else if ( 126 == wType )										//物品品质
//				{
//					str = vecConvertE[i].GoodsChar;
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 41 == wType )												//力量追加
//				{
//					str = vecConvertE[i].ForceAdd;
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 42 == wType )
//				{
//					str = vecConvertE[i].MinJieAdd;									//敏捷追加
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 43 == wType )	
//				{
//					str = vecConvertE[i].TiZhiAdd;									//体制追加
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 44 == wType )						
//				{
//					str = vecConvertE[i].MindAdd;									//智力追加
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 45 == wType )
//				{
//					str = vecConvertE[i].ZhiHuiAdd;									//智慧追加
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 46 == wType )
//				{
//					str = vecConvertE[i].EnergyAdd;									//精神追加
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 17 == wType )
//				{
//					str = vecConvertE[i].MingZhongRevise;							//命中能力修正
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 19 == wType )
//				{
//					str = vecConvertE[i].DuckRevise;								//闪避能力修正
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 20 == wType )
//				{
//					str = vecConvertE[i].GeDangRevise;								//格挡能力修正
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 21 == wType )
//				{
//					str = vecConvertE[i].ZhaoJiaRevise;								//招架能力修正
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 128 == wType )
//				{
//					str = vecConvertE[i].MaxChongNeng;								//充能修正	
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 159 == wType )
//				{
//					str = vecConvertE[i].WuImmunity;								//物理免伤值	
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 160 == wType )
//				{
//					str = vecConvertE[i].MoImmunity;								//魔法免伤值	
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 161 == wType	)
//				{
//					str = vecConvertE[i].GoodsUpPrice;								//物品升级价格
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if ( 90 == wType )
//				{
//					str = vecConvertE[i].BasicAperValue1;							//基本孔数值1
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue1 = atoi(str.c_str());
//					str = vecConvertE[i].BasicAperValue2;							//..........2
//					if ( strcmp( str.c_str(),temp ) )
//						pAddAttr->lValue2 = atoi(str.c_str());
//					continue;
//				}
//				else if( 172 == wType )
//				{
//					str = vecConvertE[i].LianDuValue2;								//练度值2
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue2 = atoi(str.c_str());
//					continue;
//				}
//				else if( 183 == wType )
//				{
//					str = vecConvertE[i].DecomBlueCrystalAmount;					//分解蓝色晶体数
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if( 184 == wType)										//攻击ED值
//				{
//					str = vecConvertE[i].AtkValueED;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//
//					str = vecConvertE[i].AtkValueED2;						//攻击ed值2
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue2 = atoi(str.c_str());
//
//					continue;
//				}
//				else if(185 == wType)
//				{
//					str = vecConvertE[i].CurAkkValueED;							//当前攻击ed值
//					if(strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if(186 == wType)
//				{
//					str = vecConvertE[i].RecoveryValueED;						//防御ED值
//					if(strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					
//					str = vecConvertE[i].RecoveryValueED2;						//值2
//					if(strcmp(str.c_str(),temp))
//						pAddAttr->lValue2 = atoi(str.c_str());
//					
//					continue;
//				}
//				else if(187 == wType)
//				{
//					str = vecConvertE[i].CurRecoveryValueED;						//当前防御ED值
//					if(strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if(188 == wType)
//				{
//					str = vecConvertE[i].MainShang;					//免伤
//					if(strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if(189 == wType)
//				{
//					str = vecConvertE[i].Pierce;				//c穿透
//					if(strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if(190 == wType)
//				{
//					str = vecConvertE[i].Const;				//星座
//					if(strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if(191 == wType)
//				{
//					str = vecConvertE[i].Lucky;
//					if(strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if(168 == wType )						//装备等阶
//				{
//					str = vecConvertE[i].EquipDengJie;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if(94 == wType )		//	物理小攻成长
//				{
//					str = vecConvertE[i].PhysMinAtkUp;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if(95 == wType )		//	物理大攻成长
//				{
//					str = vecConvertE[i].PyhsMaxAtkUp;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if(96 == wType )		//	魔法攻及成长
//				{
//					str = vecConvertE[i].MagicAtkUp;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if(97 == wType )		//	物理防御成长
//				{
//					str = vecConvertE[i].PhysRecoUp;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if(98 == wType )		// 魔法防御成长
//				{
//					str = vecConvertE[i].MagicRecoUp;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if( 203 == wType )		//	追加生命上限
//				{
//					str = vecConvertE[i].SuperLifeMaxUp;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if( 47 == wType )		//	生命上限修正
//				{
//					str = vecConvertE[i].ModifyLifeMaxUp;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if( 194 == wType )		//	小攻追加
//				{
//					str = vecConvertE[i].MinGongAdd;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if( 195 == wType )		//	大功追加
//				{
//					str = vecConvertE[i].MaxGongAdd;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if( 196 == wType )		//	墨攻追加
//				{
//					str = vecConvertE[i].MagicGongAdd;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if( 197 == wType )		//	物防追加
//				{
//					str = vecConvertE[i].PhysFangAdd;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//				else if( 198 == wType )		//	魔防追加
//				{
//					str = vecConvertE[i].MaigcFangAdd;
//					if( strcmp(str.c_str(),temp))
//						pAddAttr->lValue1 = atoi(str.c_str());
//					continue;
//				}
//			}
//		}
//	}
//	return true;
//}

// ItemConvert.ini
bool CMainFrame::CountEquipDisplayProperty(tagGoodAttr *pGoods)
{
	// 类型
	bool bVaild = false;
	for (int i=0;i<(int)g_vConvert.size(); i++)
	{
		if (pGoods->strOrigName == g_vConvert[i].strOrgName)
		{	
			const char * temp = "\\";
			string str = g_vConvert[i].strShowName;
			if( strcmp(str.c_str(),temp))
				pGoods->strName = str;
			if( strcmp( g_vConvert[i].lGraphicID.c_str(),temp ))
				pGoods->dwIconId = atoi((g_vConvert[i].lGraphicID).c_str());
			if( strcmp( g_vConvert[i].lGroundID.c_str(),temp ))
				pGoods->dwGroundId = atoi((g_vConvert[i].lGroundID).c_str());
			if( strcmp( g_vConvert[i].lEquipID.c_str(),temp) )
				pGoods->dwEquipID = atoi((g_vConvert[i].lEquipID).c_str());
			if( strcmp( g_vConvert[i].lPrice.c_str(),temp))
				pGoods->dwValue = atoi((g_vConvert[i].lPrice).c_str());
			if( strcmp( g_vConvert[i].strContent.c_str(),temp ))
			{
				const char* s = "=";
				if( strcmp( g_vConvert[i].strContent.c_str(),s) )
					pGoods->strContent = g_vConvert[i].strContent;
				else
					pGoods->strContent = "";
			}
			if( strcmp( g_vConvert[i].lSound.c_str(), temp ) )
				pGoods->dwSound = atoi((g_vConvert[i].lSound).c_str());
			if( strcmp( g_vConvert[i].lSilverPrice.c_str(),temp ))
				pGoods->dwSilverValue = atoi((g_vConvert[i].lSilverPrice).c_str());

			vector<tagAddAttr>::iterator iterAttr;
			for(iterAttr=pGoods->vecAddAttr.begin();iterAttr!=pGoods->vecAddAttr.end();iterAttr++)
			{
				tagAddAttr *pAttr = &(*iterAttr);
				if (pAttr)
				{
					string str = "";
					if (pAttr->wType == 7)					// 等级限制
					{
						str = g_vConvert[i].lLevelRest;
						if ( !strcmp( str.c_str(),temp )) continue ;
						pAttr->lValue1 = atoi(str.c_str());
						continue;
					}
					else if (pAttr->wType == 65)			//执行脚本
					{
						str = g_vConvert[i].lRunScript;
						if ( !strcmp( str.c_str(),temp ) ) continue ;
						pAttr->lValue1 = atoi(str.c_str());
						continue;
					}
					else if (pAttr->wType == 1) 
					{
						str = g_vConvert[i].lColor;			//显示颜色
						if ( !strcmp( str.c_str(),temp ) ) continue ;
						pAttr->lValue1 = atoi(str.c_str());
						continue;
					}
					else if(pAttr->wType == 171)			//物品分类
					{
						str = g_vConvert[i].lClass;
						if(!strcmp(str.c_str(),temp))	continue;
						pAttr->lValue1 = atoi(str.c_str());
						continue;
					}
					else if( pAttr->wType == 57 )			//	学习技能id
					{
						str = g_vConvert[i].lStudySkillID;
						if( !strcmp( str.c_str(),temp ) )	continue;
						pAttr->lValue1 = atoi( str.c_str() );
						continue;
					}
				}
			}
			return true;
		}
	}
	return false;
}

// 重新计算所有装备的显示属性，根据ItemConvert.ini
void CMainFrame::OnCountShow()
{
	std::ifstream file;
	file.open("ItemConvert.ini");
	if(file.fail())
	{
		MessageBox("读取文件ItemConvert.ini失败");
		return;
	}
	string line;
	g_vConvert.clear();

	//	读"ItemConvert.ini并将文件内记录压入g_vConvert"
	while (ReadTo(file,"#"))
	{
		//	ReadTo(file,"#");
		tagConvert stConvert;

		file>>stConvert.strOrgName				//原始名
			>>stConvert.strShowName
			>>stConvert.lGraphicID
			>>stConvert.lGroundID
			>>stConvert.lEquipID
			>>stConvert.lPrice
			>>stConvert.strContent
			>>stConvert.lLevelRest
			>>stConvert.lRunScript
			>>stConvert.lColor
			>>stConvert.lSound
			>>stConvert.lSilverPrice
			>>stConvert.lClass
			>>stConvert.lStudySkillID;
		//判断是否读正确
		if( !strcmp(stConvert.lStudySkillID.c_str(),"#") ||
			!strcmp(stConvert.lClass.c_str(),"#")	||	!strcmp(stConvert.lSilverPrice.c_str(),"#")	||	!strcmp(stConvert.lSound.c_str(),"#")		||
			!strcmp(stConvert.lColor.c_str(),"#")		||	!strcmp(stConvert.lRunScript.c_str(),"#")	||
			!strcmp(stConvert.lLevelRest.c_str(),"#")	||	!strcmp(stConvert.strContent.c_str(),"#")	||
			!strcmp(stConvert.lPrice.c_str(),"#")		||	!strcmp(stConvert.lEquipID.c_str(),"#")		||
			!strcmp(stConvert.lGroundID.c_str(),"#")	||	!strcmp(stConvert.strShowName.c_str(),"#")	||
			!strcmp(stConvert.strOrgName.c_str(),"#")	)
		{
			char str[128];
			sprintf_s(str,"警告：配置文件中 %s 附近有错误！",stConvert.strOrgName.c_str());
			MessageBox(str,NULL,0);
			file.close();
			return;
		}
		g_vConvert.push_back(stConvert);
	}

	file.close();

	vector<tagGoodAttr>::iterator iter;
	for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++)
	{
		tagGoodAttr *pGoods = &(*iter);
		if (pGoods)	// 装备类
			CountEquipDisplayProperty(pGoods);
	}
	CContainerPage *pContainer=(CContainerPage*)m_wndSplitter.GetPane(0,1);
	pContainer->m_wndAllGoodsPage.UpDataAllGoodsList();

	CAllGoodsTree *pGoodsTree=(CAllGoodsTree*)m_wndSplitter.GetPane(0,0);

	HTREEITEM hSelItem = pGoodsTree->GetSelGoodsTree()->GetSelectedItem();
	HTREEITEM hRootItem = pGoodsTree->GetRootItem();
	if ( hSelItem == hRootItem )
		return;
	int nSelFlag = 0;
	for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++,nSelFlag++)
	{			
		if(pGoodsTree->GetSelGoodsTree()->GetItemText(hSelItem).Compare(CString(iter->strOrigName.c_str()))==0)
			break;
	}
	pContainer->m_wndGoodAttrPage.FillCtrls(nSelFlag);		
}

struct tagBatAdd_E 
{
	//	值类型
	int nValueType;
	//	附加属性数量
	DWORD dwAddAttrCounts;
	//	附加属性索引
	vector<DWORD> vecAddAttrIndex;
	//	操作项
	map<string,vector<DWORD>>	mapAddAttrValue;
};
tagBatAdd_E myE;
//	原始名 
//map<string,vector<DWORD>> mapConverE;
//配置文件ItemConvert_E.ini 数据收集
void CMainFrame::OnConvertE()
{
	// TODO: 在此添加命令处理程序代码

	std::ifstream file;
	file.open("ItemConvert_E.ini");
	if(file.fail())
	{
		MessageBox("读取文件ItemConvert_E.ini失败");
		return;
	}

	//	读附加属性的数目和索引
	list<long>	listAddrIndex;
	long lAmount;
	long lType;
	//	读项
	while (ReadTo(file,"#"))
	{
		//	附加属性数目
		file>>lAmount;
		//	读附加属性的索引
		for(int i=0;i<lAmount;i++)
		{
			file>>lType;
			listAddrIndex.push_back(lType);
		}
		break;
	}
	
	//	读原始名和附加属性值
	map<long,long>		mapIndexMapValue;
	map<string,map<long,long>>	mapNameMapIndex;

	//	读物品
	string szOrgName;
	while ( ReadTo(file,"#"))
	{
		//	原始名
		file>>szOrgName;
		mapIndexMapValue.clear();

		//	遍历该名字所对应的附加属性index
		list<long>::iterator it = listAddrIndex.begin();
		for(; it!=listAddrIndex.end();it++)
		{
			long lIndex = (long)(*it);

			//	申明一个字符类型，可帮助检查人为的配置错误
			string szValue;
			file>>szValue;
			if( strcmp(szValue.c_str(),"#") == 0 )
			{
				AfxMessageBox("数据有错误");
				return;
			}

			mapIndexMapValue[lIndex] = atoi(szValue.c_str());
		}
		//
		mapNameMapIndex[szOrgName] = mapIndexMapValue;
	}

	//	遍历所有物品
	vector<tagGoodAttr>::iterator itGoods = vecGoodAttr.begin();
	for( ; itGoods != vecGoodAttr.end();itGoods++)
	{
		tagGoodAttr* pGoods = &(*itGoods);
		map<string,map<long,long>>::iterator itOrgName = mapNameMapIndex.find( pGoods->strOrigName );
		//	如果此物品需要更新属性
		if( itOrgName != mapNameMapIndex.end() )
		{
			//	遍历该物品的所有附加属性
			vector<tagAddAttr>::iterator itAddAttr = itGoods->vecAddAttr.begin();
			for(; itAddAttr != itGoods->vecAddAttr.end(); itAddAttr++ )
			{
				//	遍历需要修改的附加属性列表
				map<long,long>::iterator itValue = itOrgName->second.find(itAddAttr->wType);
				if( itValue != itOrgName->second.end() )
				{
					(*itAddAttr).lValue1 = itValue->second;
				}
			}
		}

	}
}

void CMainFrame::UpdateChangeGoodsAddAttr(tagGoodAttr* pGoods,map<string,map<long,string>> & mapTemp)
{
	map<string,map<long,string>>::iterator itOrgName = mapTemp.find(pGoods->strOrigName);
	//	如果此物品需要更新属性
	if( itOrgName != mapTemp.end() )
	{
		//	遍历该物品的所有附加属性
		vector<tagAddAttr>::iterator itAddAttr = pGoods->vecAddAttr.begin();
		for(; itAddAttr != pGoods->vecAddAttr.end(); itAddAttr++ )
		{
			tagAddAttr pAddAttr = (*itAddAttr);
			//	遍历需要修改的附加属性列表
			map<long,string> mapValueTemp = itOrgName->second;
			map<long,string>::iterator itFind = mapValueTemp.find(pAddAttr.wType);
			if( itFind != mapValueTemp.end())
			{
				pAddAttr.lValue1 = atoi(itFind->second.c_str());
			}
		}
	}

	return;
	//////////////////////////////////////////////////////////////////////////
	//vecConvertE.clear();
	////读"ItemConvert.ini并将文件内记录压入g_vConvert"
	//while (ReadTo(file, "#"))
	//{
	//	tagConvertE stConvertE;
	//	file>>stConvertE.OrgName					//原始名
	//		>>stConvertE.WeaponStrong				//武器强度
	//		>>stConvertE.WeaponStringAdd			//武器强度成长
	//		>>stConvertE.MinAtk						//最小攻击力
	//		>>stConvertE.MaxAtk						//最大攻击力
	//		>>stConvertE.MoGong						//魔法攻击
	//		>>stConvertE.WuGongSockPro				//物理致命一击率
	//		>>stConvertE.WuBaoJiDamage				//物理暴击伤害
	//		>>stConvertE.WuBaoJiFastness			//物理暴击抗性
	//		>>stConvertE.MoGongSockPro				//魔法致命一击率
	//		>>stConvertE.MoBaoJiDamage				//魔法暴击伤害
	//		>>stConvertE.MoBaoJiFastness			//魔法暴击抗性
	//		>>stConvertE.WuRecovery					//物理防御
	//		>>stConvertE.MoRecovery					//魔法防御
	//		>>stConvertE.AsunderValue1				//分解价值
	//		>>stConvertE.GoodsChar					//物品品质
	//		>>stConvertE.ForceAdd					//力量追加
	//		>>stConvertE.MinJieAdd					//敏捷追加
	//		>>stConvertE.TiZhiAdd					//体制追加
	//		>>stConvertE.MindAdd					//智力追加
	//		>>stConvertE.ZhiHuiAdd					//智慧追加
	//		>>stConvertE.EnergyAdd					//精神追加
	//		>>stConvertE.MingZhongRevise			//命中能力修正
	//		>>stConvertE.DuckRevise					//闪避能力修正
	//		>>stConvertE.GeDangRevise				//格挡能力修正
	//		>>stConvertE.ZhaoJiaRevise				//招架能力修正
	//		>>stConvertE.MaxChongNeng				//充能上限
	//		>>stConvertE.WuImmunity					//物理免伤值
	//		>>stConvertE.MoImmunity					//魔法免伤值
	//		>>stConvertE.GoodsUpPrice				//物品升级价格
	//		>>stConvertE.BasicAperValue1			//基本孔数值1
	//		>>stConvertE.BasicAperValue2			//基本孔数值2
	//		>>stConvertE.AsunderValue2				//分解价值2
	//		>>stConvertE.LianDuValue2				//练度值2
	//		>>stConvertE.DecomBlueCrystalAmount		//分解蓝色晶体数
	//		>>stConvertE.MainShang					//免伤
	//		>>stConvertE.Pierce						//穿透
	//		>>stConvertE.Const						//星座
	//		>>stConvertE.Lucky						//幸运
	//		>>stConvertE.AtkValueED					//攻击ED值
	//		>>stConvertE.CurAkkValueED				//当前攻击ED值
	//		>>stConvertE.RecoveryValueED			//防御ED值
	//		>>stConvertE.CurRecoveryValueED			//当前防御ed值

	//		>>stConvertE.RecoveryValueED2			//防御ed值2
	//		>>stConvertE.AtkValueED2				//攻击ed值2
	//		>>stConvertE.EquipDengJie				//装备等阶

	//		>>stConvertE.PhysMinAtkUp			//	物理小攻成长
	//		>>stConvertE.PyhsMaxAtkUp			//	物理大攻成长
	//		>>stConvertE.MagicAtkUp				//	魔法攻击成长
	//		>>stConvertE.PhysRecoUp				//	物理防御成长
	//		>>stConvertE.MagicRecoUp			//	魔法防御成长
	//		>>stConvertE.SuperLifeMaxUp			//	追加生命上限
	//		>>stConvertE.ModifyLifeMaxUp		//	生命上限修正
	//		>>stConvertE.MinGongAdd					//	小攻追加
	//		>>stConvertE.MaxGongAdd					//	大功追加
	//		>>stConvertE.MagicGongAdd				//	墨攻追加
	//		>>stConvertE.PhysFangAdd				//	物防追加
	//		>>stConvertE.MaigcFangAdd;				//	魔防追加

	//		//判断是否读错
	//	if( !strcmp(stConvertE.MaigcFangAdd.c_str(),"#") || !strcmp(stConvertE.PhysFangAdd.c_str(),"#")	||
	//		!strcmp(stConvertE.MagicGongAdd.c_str(),"#") || !strcmp(stConvertE.MaxGongAdd.c_str(),"#")	|| !strcmp(stConvertE.MinGongAdd.c_str(),"#") ||
	//		!strcmp(stConvertE.ModifyLifeMaxUp.c_str(),"#") || !strcmp(stConvertE.SuperLifeMaxUp.c_str(),"#") || 
	//		!strcmp(stConvertE.MagicRecoUp.c_str(),"#")	|| !strcmp(stConvertE.PhysRecoUp.c_str(),"#")	|| !strcmp(stConvertE.MagicAtkUp.c_str(),"#")	||
	//		!strcmp(stConvertE.PhysMinAtkUp.c_str(),"#") || !strcmp(stConvertE.PhysMinAtkUp.c_str(),"#") ||
	//		!strcmp(stConvertE.EquipDengJie.c_str(),"#")					||	!strcmp(stConvertE.AtkValueED2.c_str(),"#")	||	!strcmp(stConvertE.RecoveryValueED2.c_str(),"#") ||
	//		!strcmp(stConvertE.CurRecoveryValueED.c_str(),"#")		||	!strcmp(stConvertE.RecoveryValueED.c_str(),"#")			||
	//		!strcmp(stConvertE.CurAkkValueED.c_str(),"#")			||	!strcmp(stConvertE.AtkValueED.c_str(),"#")				||
	//		!strcmp(stConvertE.Lucky.c_str(),"#")					||	!strcmp(stConvertE.Const.c_str(),"#")					||
	//		!strcmp(stConvertE.Pierce.c_str(),"#")					||	!strcmp(stConvertE.MainShang.c_str(),"#")				||
	//		!strcmp(stConvertE.DecomBlueCrystalAmount.c_str(),"#") ||
	//		!strcmp(stConvertE.LianDuValue2.c_str(),"#")	||	!strcmp(stConvertE.AsunderValue2.c_str(),"#")	||	
	//		!strcmp(stConvertE.BasicAperValue2.c_str(),"#")	||	!strcmp(stConvertE.BasicAperValue1.c_str(),"#")	||
	//		!strcmp(stConvertE.GoodsUpPrice.c_str(),"#")	||	!strcmp(stConvertE.MoImmunity.c_str(),"#")		||
	//		!strcmp(stConvertE.MaxChongNeng.c_str(),"#")	||	!strcmp(stConvertE.ZhaoJiaRevise.c_str(),"#")	||
	//		!strcmp(stConvertE.GeDangRevise.c_str(),"#")	||	!strcmp(stConvertE.DuckRevise.c_str(),"#")		||
	//		!strcmp(stConvertE.MingZhongRevise.c_str(),"#")	||	!strcmp(stConvertE.EnergyAdd.c_str(),"#")		||
	//		!strcmp(stConvertE.ZhiHuiAdd.c_str(),"#")		||	!strcmp(stConvertE.MindAdd.c_str(),"#")			||
	//		!strcmp(stConvertE.TiZhiAdd.c_str(),"#")		||	!strcmp(stConvertE.MinJieAdd.c_str(),"#")		||
	//		!strcmp(stConvertE.ForceAdd.c_str(),"#")		||	!strcmp(stConvertE.GoodsChar.c_str(),"#")		||
	//		!strcmp(stConvertE.AsunderValue1.c_str(),"#")	||	!strcmp(stConvertE.MoRecovery.c_str(),"#")		||
	//		!strcmp(stConvertE.WuRecovery.c_str(),"#")		||	!strcmp(stConvertE.MoBaoJiFastness.c_str(),"#")	||
	//		!strcmp(stConvertE.MoBaoJiDamage.c_str(),"#")	||	!strcmp(stConvertE.MoGongSockPro.c_str(),"#")	||
	//		!strcmp(stConvertE.WuBaoJiFastness.c_str(),"#")	||	!strcmp(stConvertE.WuBaoJiDamage.c_str(),"#")	||
	//		!strcmp(stConvertE.WuGongSockPro.c_str(),"#")	||	!strcmp(stConvertE.MoGong.c_str(),"#")			||
	//		!strcmp(stConvertE.MaxAtk.c_str(),"#")			||	!strcmp(stConvertE.MinAtk.c_str(),"#")			||
	//		!strcmp(stConvertE.WeaponStringAdd.c_str(),"#")	||	!strcmp(stConvertE.WeaponStrong.c_str(),"#")	||
	//		!strcmp(stConvertE.OrgName.c_str(),"#")	)
	//	{
	//		char str[128];
	//		sprintf_s(str,"警告：配置文件中 %s 附近有错误！",stConvertE.OrgName.c_str());
	//		MessageBox(str,NULL,0);
	//		return;
	//	}
	//	vecConvertE.push_back(stConvertE);
	//}

	//file.close();

	//vector<tagGoodAttr>::iterator iter;
	//for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++)
	//{
	//	tagGoodAttr *pGoods = &(*iter);
	//	if (pGoods)	// 装备类
	//		CountConvetEGoodsProperty(pGoods);
	//}
}

// 获取物品附加属性值一
long GetValue1(tagGoodAttr * pGoods, WORD wType)
{
	vector<tagAddAttr>::iterator iterAttr;
	for(iterAttr=pGoods->vecAddAttr.begin();iterAttr!=pGoods->vecAddAttr.end();iterAttr++)
	{
		tagAddAttr *pAttr = &(*iterAttr);
		if (pAttr && pAttr->wType == wType)
		{
			return pAttr->lValue1;
		}
	}
	return 0;
}

// 设置物品附加属性值一
long SetValue1(tagGoodAttr * pGoods, WORD wType, long value)
{
	vector<tagAddAttr>::iterator iterAttr;
	for(iterAttr=pGoods->vecAddAttr.begin();iterAttr!=pGoods->vecAddAttr.end();iterAttr++)
	{
		tagAddAttr *pAttr = &(*iterAttr);
		if (pAttr && pAttr->wType == wType)
		{
			pAttr->lValue1 = value;
			return value;
		}
	}
	return -1;
}

// 
long AddValue(tagGoodAttr * pGoods, WORD wType, long value1, long value2)
{
	vector<tagAddAttr>::iterator iterAttr;
	for(iterAttr=pGoods->vecAddAttr.begin();iterAttr!=pGoods->vecAddAttr.end();iterAttr++)
	{
		tagAddAttr *pAttr = &(*iterAttr);
		if (pAttr && pAttr->wType == wType)
		{
			pAttr->lValue1 = value1;
			pAttr->lValue2 = value2;
			return 1;
		}
	}

	tagAddAttr sta;
	sta.bEnable = true;
	sta.bHide = false;
	sta.lValue1 = value1;
	sta.lValue2 = value2;
	//sta.wEnableOdds = 10000;
	sta.wType = wType;

	pGoods->vecAddAttr.push_back(sta);

	return 1;
}

// 获取物品附加属性值一，随机1
long GetValue1Random(tagGoodAttr * pGoods, WORD wType)
{/*
 vector<tagAddAttr>::iterator iterAttr;
 for(iterAttr=pGoods->vecAddAttr.begin();iterAttr!=pGoods->vecAddAttr.end();iterAttr++)
 {
 tagAddAttr *pAttr = &(*iterAttr);
 if (pAttr && pAttr->wType == wType)
 {
 if (pAttr->vecModify.size())
 {
 return 1;
 }
 else
 return 0;
 }
 }*/
	return 0;
}

// 设置物品附加属性值一，随机1
long SetValue1Random(tagGoodAttr * pGoods, WORD wType, long min, long max)
{/*
 vector<tagAddAttr>::iterator iterAttr;
 for(iterAttr=pGoods->vecAddAttr.begin();iterAttr!=pGoods->vecAddAttr.end();iterAttr++)
 {
 tagAddAttr *pAttr = &(*iterAttr);
 if (pAttr && pAttr->wType == wType)
 {
 if (pAttr->vecModify.size())
 {
 pAttr->vecModify[0].dwMin = min;
 pAttr->vecModify[0].dwMax = max;
 }
 else
 return 0;
 }
 }*/
	return 0;
}


//
//1．如果有属性“最大攻击”，同时该属性下有“随机属性1”，则修改随机属性1的“值1”＝1，“值2”＝最大攻击×0.2
//
//2．如果有属性“最小攻击”，同时该属性下有“随机属性1”，则修改随机属性1的“值1”＝1，“值2”＝最小攻击×0.2
//
//3．同时该武器如果有“最小攻击成长”，则修改该属性的值1＝最大攻击×0.05
//
//4．同时该武器如果有“最大攻击成长”，则修改该属性的值1＝最大攻击×0.05
//
//5．如果有道具有“物理防御力”属性，同时该属性下有“随机属性1”，则修改随机属性1的“值1”＝1，“值2”＝物理防御力×0.2
//
//6．同时该武器如果有“物理防御成长”，则修改该属性的值1＝物理防御力×0.05
//
//7．如果有道具有“法术抗性修正”属性，同时该属性下有“随机属性1”，则修改随机属性1的“值1”＝1，“值2”＝法术抗性修正×0.2
//
//8．同时该武器如果有“法术抗性修正成长”，则修改该属性的值1＝法术抗性修正×0.05
//
//

// 计算攻防随机和成长
void CMainFrame::OnCountRandom()
{
	vector<tagGoodAttr>::iterator iter;
	vector<tagAddAttr>::iterator iterAttr;
	//vector<tagModify>::iterator iterModify;

	int nTem=0;
	for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++)
	{
		tagGoodAttr *pGoods = &(*iter);
		if (pGoods && pGoods->dwType >= 2 )	// 装备类
		{
			long lAttackMin = GetValue1(pGoods, 13);				//最小攻击
			long lAttackMax = GetValue1(pGoods, 14);				//最大攻击
			long lWuRecovery	= GetValue1(pGoods, 15);			//物理防御
			long lMagicAtk	= GetValue1(pGoods, 22);				//魔法攻击
			long lMagicRecovery = GetValue1(pGoods,26);				//魔法防御
			//long lAttackMinUp = GetValue1(pGoods, 52);
			//long lAttackMaxUp = GetValue1(pGoods, 53);
			//long lMagicAtk = GetValue1(pGoods, 15);
			//long lMagicAtkUp = GetValue1(pGoods, 54);
			//long lDef = GetValue1(pGoods, 16);
			//long lDefUp = GetValue1(pGoods, 55);
			//long lMagicDefUp = GetValue1(pGoods, 72);

			// 修正攻击成长min
			if (lAttackMin>0)
			{
				float f = lAttackMin / 20.f;
				long n = (long)f;

				// 5入
				if (f-(float)n >= 0.5f)
				{
					n++;
				}

				if( n<1)
				{
						n = 1;
				}
				SetValue1(pGoods, 94, n);
			}

			// 修正攻击成长max
			if (lAttackMax>0)
			{
				float f = lAttackMax / 20.f;
				long n = (long)f;

				// 5入
				if (f-(float)n >= 0.5f)
				{
					n++;
				}
				if (n<1)
				{
						n=1;
				}
				SetValue1(pGoods, 95, n);
			}

			// 修正法攻成长max
			if (lMagicAtk>0)			
			{
				float f = lMagicAtk / 20.f;
				long n = (long)f;

				// 5入
				if (f-(float)n >= 0.5f)
				{
					n++;
				}
				if (n<1) 
				{
						n=1;
				}
				SetValue1(pGoods, 96, n);					//魔法攻击力成长值
			}

			// 修正物防成长
			if (lWuRecovery>0)
			{
				float f = lWuRecovery / 20.f;
				long n = (long)f;

				// 5入
				if (f-(float)n >= 0.5f)
				{
					n++;
				}
				if (n<1) 
				{
						n=1;
				}
				SetValue1(pGoods, 97, n);					//物理防御力成长值
			}

			// 修正魔防成长
			if (lMagicRecovery>0)
			{
				float f = lMagicRecovery / 20.f;
				long n = (long)f;

				// 5入
				if (f-(float)n >= 0.5f)
				{
					n++;
				}
				if (n<1)
				{
						n=1;
				}
				SetValue1(pGoods, 98, n);					//魔法防御力成长值
			}

			//// 修正攻击随机min
			//if (lAttackMin && GetValue1Random(pGoods, 14))
			//{
			//	float f = lAttackMin / 5.f;
			//	long n = (long)f;

			//	// 5入
			//	if (f-(float)n > 0.5f)
			//	{
			//		n++;
			//	}
			//	if (n<1) n=1;
			//	SetValue1Random(pGoods, 14, 1, n);
			//}

			//// 修正攻击随机max
			//if (lAttackMax && GetValue1Random(pGoods, 15))
			//{
			//	float f = lAttackMax / 5.f;
			//	long n = (long)f;

			//	// 5入
			//	if (f-(float)n > 0.5f)
			//	{
			//		n++;
			//	}
			//	if (n<1) n=1;
			//	SetValue1Random(pGoods, 15, 1, n);
			//}

			//// 修正法攻随机
			//if (lMagicAtk && GetValue1Random(pGoods, 16))
			//{
			//	float f = lMagicAtk / 5.f;
			//	long n = (long)f;

			//	// 5入
			//	if (f-(float)n > 0.5f)
			//	{
			//		n++;
			//	}
			//	if (n<1) n=1;
			//	SetValue1Random(pGoods, 16, 1, n);
			//}

			//// 修正物防随机min
			//if (lDef && GetValue1Random(pGoods, 17))
			//{
			//	float f = lDef / 5.f;
			//	long n = (long)f;

			//	// 5入
			//	if (f-(float)n > 0.5f)
			//	{
			//		n++;
			//	}
			//	if (n<1) n=1;
			//	SetValue1Random(pGoods, 17, 1, n);
			//}

			//// 修正魔防随机min
			//if (lMagicDef && GetValue1Random(pGoods, 23))
			//{
			//	float f = lMagicDef / 5.f;
			//	long n = (long)f;

			//	// 5入
			//	if (f-(float)n > 0.5f)
			//	{
			//		n++;
			//	}
			//	if (n<1) n=1;
			//	SetValue1Random(pGoods, 23, 1, n);
			//}
		}
	}
}

long ClearRandom(tagGoodAttr * pGoods)
{/*
 vector<tagAddAttr>::iterator iterAttr;
 for(iterAttr=pGoods->vecAddAttr.begin();iterAttr!=pGoods->vecAddAttr.end();iterAttr++)
 {
 tagAddAttr *pAttr = &(*iterAttr);
 if (pAttr)
 {
 while (pAttr->vecModify.size() > 1)
 {
 pAttr->vecModify.pop_back();
 }
 }
 }*/
	return 0;
}

// 清除所有装备附加属性的随机值，只保留第一条
void CMainFrame::OnCountRandomnum()
{
	vector<tagGoodAttr>::iterator iter;
	vector<tagAddAttr>::iterator iterAttr;
	//vector<tagModify>::iterator iterModify;

	int nTem=0;
	for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++)
	{
		tagGoodAttr *pGoods = &(*iter);
		if (pGoods && pGoods->dwType >= 2 )	// 装备类
		{
			ClearRandom(pGoods);
		}
	}
}

// 武器强度+20
void CMainFrame::OnCountAdd20()
{
	vector<tagGoodAttr>::iterator iter;
	vector<tagAddAttr>::iterator iterAttr;
	//vector<tagModify>::iterator iterModify;

	int nTem=0;
	for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++)
	{
		tagGoodAttr *pGoods = &(*iter);
		if (pGoods && pGoods->dwType >= 2 )	// 装备类
		{
			long val = GetValue1(pGoods, 90);
			if (val != -1)
			{
				SetValue1(pGoods, 90, val + 20);
			}
		}
	}
}

void CMainFrame::OnAddTypeex()
{
	vector<tagGoodAttr>::iterator iter;
	vector<tagAddAttr>::iterator iterAttr;
	//vector<tagModify>::iterator iterModify;

	int nTem=0;
	for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++)
	{
		tagGoodAttr *pGoods = &(*iter);
		if (pGoods && pGoods->dwType >= 2 )	// 装备类
		{
			long val = GetValue1(pGoods, 13);
			if (val == 0)
			{
				AddValue(pGoods, 13, 0, 0);
			}
		}
	}
}
//数据导出到 ItemEduce.ini
void CMainFrame::OnCheckDataToEduce()
{
	// TODO: 在此添加命令处理程序代码
	std::ofstream ofs("ItemEduce.ini",ios::out);

	ofs.write("原始名	", sizeof("原始名"));
	ofs.write("显示名	", sizeof("显示名"));
	ofs.write("界面图形	", sizeof("界面图形"));
	ofs.write("落地图形	", sizeof("落地图形"));
	ofs.write("换装图形	", sizeof("换装图形"));
	ofs.write("价格	", sizeof("价格"));
	ofs.write("描述	", sizeof("描述"));
	ofs.write("等级限制	", sizeof("等级限制"));
	ofs.write("执行脚本\n	", sizeof("执行脚本"));

	vector<tagGoodAttr>::iterator pAttr = vecGoodAttr.begin();
	for (; pAttr!=vecGoodAttr.end();pAttr++)
	{
		tagConvert pOutAttr;

		string str = pAttr->strOrigName.c_str();
		ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
		ofs.put('	');

		str = pAttr->strName.c_str();
		ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
		ofs.put('	');	

		char str1[16];
		str = _itoa(pAttr->dwIconId,str1,10);			
		ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
		ofs.put('	');

		str = _itoa(pAttr->dwGroundId,str1,10);
		ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
		ofs.put('	');

		str = _itoa(pAttr->dwEquipID,str1,10);
		ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
		ofs.put('	');

		str = _itoa(pAttr->dwValue,str1,10);
		ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
		ofs.put('	');

		str = pAttr->strContent.c_str();
		ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
		ofs.put('	');

		vector<tagAddAttr>::iterator pAdd = pAttr->vecAddAttr.begin();
		if (pAttr->vecAddAttr.empty())
		{
			for (int i=0;i<2;i++)
			{
				ofs.write("\\",4);
				ofs.put('	');
			}
			ofs.write("\n",1);
			ofs.close();
			return;
		}

		bool bFound;

		for (;pAdd!=pAttr->vecAddAttr.end();pAdd++)
		{
			if(pAdd->wType == 7)						//等级限制
			{
				str = itoa(pAdd->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put('	');
				bFound = true;
				break;
			}
			bFound = false;
		}
		if(!bFound)
		{
			ofs.write("\\",4);
			ofs.put('	');
		}

		bFound = true;
		pAdd = pAttr->vecAddAttr.begin();
		for (;pAdd!=pAttr->vecAddAttr.end();pAdd++)
		{
			if (pAdd->wType == 65)						//执行脚本
			{
				str = _itoa(pAdd->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put('	');
				bFound = true;
				break;
			}
			bFound = false;
		}
		if(!bFound)
		{
			ofs.write("\\",4);
			ofs.put('	');
		}
		ofs.write("\n",1);
	}
	ofs.close();
}
//数据导出到 ItemEduce_E.ini
void CMainFrame::OnCheckDataToEduceE()
{
	//	 TODO: 在此添加命令处理程序代码
	std::ofstream ofs("ItemEduce_E.ini",ios::out);

	ofs.write("原始名	", sizeof("原始名"));
	ofs.write("武器强度	", sizeof("武器强度"));
	ofs.write("武器强度成长	", sizeof("武器强度成长"));
	ofs.write("最小攻击力	", sizeof("最小攻击力"));
	ofs.write("最大攻击力	", sizeof("最大攻击力"));
	ofs.write("魔法攻击	", sizeof("魔法攻击"));
	ofs.write("物理致命一击率	", sizeof("物理致命一击率"));
	ofs.write("物理暴击伤害	", sizeof("物理暴击伤害"));
	ofs.write("物理暴击抗性	", sizeof("物理暴击抗性"));
	ofs.write("魔法致命一击率	", sizeof("魔法致命一击率"));
	ofs.write("魔法暴击伤害	", sizeof("魔法暴击伤害"));
	ofs.write("魔法暴击抗性	", sizeof("魔法暴击抗性"));
	ofs.write("物理防御	", sizeof("物理防御"));
	ofs.write("魔法防御	", sizeof("魔法防御"));
	ofs.write("基本孔数	", sizeof("基本孔数"));
	ofs.write("分解价值	", sizeof("分解价值"));
	ofs.write("物品品质	", sizeof("物品品质"));
	ofs.write("力量追加	", sizeof("力量追加"));
	ofs.write("敏捷追加	", sizeof("敏捷追加"));
	ofs.write("体质追加	", sizeof("体质追加"));
	ofs.write("智力追加	", sizeof("智力追加"));
	ofs.write("智慧追加	", sizeof("智慧追加"));
	ofs.write("精神追加	", sizeof("精神追加"));
	ofs.write("命中能力修正	", sizeof("命中能力修正"));
	ofs.write("闪避能力修正	", sizeof("闪避能力修正"));
	ofs.write("格挡能力修正	", sizeof("格挡能力修正"));
	ofs.write("招架能力修正	", sizeof("招架能力修正"));
	ofs.write("充能上限", sizeof("充能上限"));
	ofs.write("物理免伤值	", sizeof("物理免伤值"));
	ofs.write("魔法免伤值	", sizeof("魔法免伤值"));
	ofs.write("物品升级价格\n", sizeof("物品升级价格"));

	vector<tagGoodAttr>::iterator pAttr = vecGoodAttr.begin();
	for (; pAttr!=vecGoodAttr.end();pAttr++)
	{
		tagConvertE pOutAttr;
		char s = '	';
		char str1[16];
		//原始名
		string str = pAttr->strOrigName.c_str();
		ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
		ofs.put(s);

		if (pAttr->vecAddAttr.empty())
		{
			for (int i=0;i<30;i++)
			{
				ofs.write("\\",4);
				ofs.put(s);
			}
			ofs.write("\n",1);
			continue;
		}

		vector<tagAddAttr>::iterator pAddAttr = pAttr->vecAddAttr.begin();
		bool bFind = false;
		for (;pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType==12)								//武器强度
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 137)							//武器强度成长
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(	pAddAttr->wType ==	13)							//最小攻击力
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 14)							//最大攻击力
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 22)							//魔法攻击
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(	pAddAttr->wType ==	18)							//物理致命一击率
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 120)							//物理暴击伤害
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 121)							//物理暴击抗性
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(	pAddAttr->wType ==	23)							//魔法致命一击率
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 122)							//魔法暴击伤害
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 123)							//魔法暴击抗性
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(	pAddAttr->wType ==	15)							//物理防御
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 26)							//魔法防御
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		//for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		//{
		//	if(pAddAttr->wType == 90)							//基本孔数
		//	{
		//		str = itoa(pAddAttr->lValue1,str1,10);
		//		ofs.write(str.c_str(),strlen(str.c_str()));
		//		ofs.put(s);
		//		bFind = true;
		//		break;
		//	}
		//	bFind = false;
		//}
		//if (!bFind)
		//{
		//	ofs.write("\\",4);
		//	ofs.put(s);
		//}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(	pAddAttr->wType ==	125)							//分解价值
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 126)							//物品品质
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 41)							//力量追加
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(	pAddAttr->wType ==	42)							//敏捷追加
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 43)							//体质追加
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 44)							//智力追加
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(	pAddAttr->wType ==	45)							//智慧追加
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 46)							//精神追加
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 17)							//命中能力修正
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(	pAddAttr->wType ==	19)							//闪避能力修正
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 20)							//格挡能力修正
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 21)							//招架能力修正
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(	pAddAttr->wType ==	128)							//充能上限
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 159)							//物理免伤值
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 160)							//魔法免伤值
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}
		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		for(pAddAttr = pAttr->vecAddAttr.begin();pAddAttr!=pAttr->vecAddAttr.end();pAddAttr++)
		{
			if(pAddAttr->wType == 161)							//物品升级价格
			{
				str = itoa(pAddAttr->lValue1,str1,10);
				ofs.write(str.c_str(),static_cast<int>(strlen(str.c_str())));
				ofs.put(s);
				bFind = true;
				break;
			}
			bFind = false;
		}

		if (!bFind)
		{
			ofs.write("\\",4);
			ofs.put(s);
		}

		ofs.write("\n",1);
	}
	ofs.close();
}
void CMainFrame::OnAddAttrByBatAdd()
{
//	 TODO: 在此添加命令处理程序代码
	list<string> m_listBatOrgName;
	list<tagAddAttr> m_listBatAddAttrType;
	m_listBatOrgName.clear();
	m_listBatAddAttrType.clear();

	std::ifstream in_f,in_f2;
	in_f.open("OrgName.ini");	in_f2.open("AddAttr.ini");
	if(in_f.fail() || in_f2.fail())
	{
		MessageBox("读取文件失败");
		return;
	}
	//读原始名
	while(true)
	{
		string strOrgName;
		in_f>>strOrgName;				//原始名
		if( strOrgName.empty())
		{
			break;
		}
		m_listBatOrgName.push_back(strOrgName);
	}
	//读附加属性
	while(true)
	{
		tagAddAttr addData;
		WORD wShowType;
		long lValue1,lValue2;
		in_f2>>wShowType				//附加属性值
			>>lValue1					//值1
			>>lValue2;				//值2

		addData.wType = wShowType;
		addData.lValue1 = lValue1;
		addData.lValue2 = lValue2;
		addData.bEnable = true;
		addData.bHide = false;

		m_listBatAddAttrType.push_back(addData);
		if( in_f2.eof())
			break;
	}

	in_f.close();		in_f2.close();

	list<string>::iterator itOrgNameIter = m_listBatOrgName.begin();
	//遍历原始名
	for(;itOrgNameIter != m_listBatOrgName.end();itOrgNameIter++)
	{
		DWORD dwIndex = -1;
		GetGoodsAddrByOrgName((*itOrgNameIter).c_str(),dwIndex);
		if( dwIndex != -1 )
		{
			//提取每项附加属性
			list<tagAddAttr>::iterator it = m_listBatAddAttrType.begin();
			for(; it != m_listBatAddAttrType.end();it++)
			{
				WORD wType = (WORD)(*it).wType -1;

				tagAddAttr addAttr;
				addAttr.wType = wType;
				addAttr.lValue1 = it->lValue1;
				addAttr.lValue2 = it->lValue2;
				addAttr.bEnable = true;
				addAttr.bHide = false;

				//如果为空,则直接添加
				if( vecGoodAttr[dwIndex].vecAddAttr.empty())
				{
					vecGoodAttr[dwIndex].vecAddAttr.push_back(addAttr);
					if( m_wndGoodAttr.GetEditFlag() )
					{
						m_wndGoodAttr.UpDataOwnAttrList(dwIndex);
					}
					break;

				}
				//如果不为空
				else
				{					
					//是否需要添加
					bool bAdd = true;			
				
					vector<tagAddAttr>::iterator itGoodsAttrIter = vecGoodAttr[dwIndex].vecAddAttr.begin();
					//遍历已经有的附加属性
					for(;itGoodsAttrIter != vecGoodAttr[dwIndex].vecAddAttr.end();itGoodsAttrIter++)
					{
						//如果相等,则不添加
						if(itGoodsAttrIter->wType == wType)
						{
							bAdd = false;
							break;
						}
						//第一个比自己大的Type前面添加
						if( itGoodsAttrIter->wType > wType)
						{
							vecGoodAttr[dwIndex].vecAddAttr.insert(itGoodsAttrIter,addAttr);
							//不添加
							bAdd = false;	
							if( m_wndGoodAttr.GetEditFlag() )
							{
								m_wndGoodAttr.UpDataOwnAttrList(dwIndex);
							}
							break;
						}
					}

					if( bAdd )
					{
						vecGoodAttr[dwIndex].vecAddAttr.push_back(addAttr);
						if( m_wndGoodAttr.GetEditFlag() )
						{
							m_wndGoodAttr.UpDataOwnAttrList(dwIndex);
						}
					}
				}
			}
		}
	}
	return;
}



void CMainFrame::GetGoodsAddrByOrgName(const char*strOrgName,DWORD &dwIndex)
{
	dwIndex = -1;
	for(int i = 0; i < (int)vecGoodAttr.size();i++)
	{
		tagGoodAttr goodsAttr = vecGoodAttr[i];
		if( strcmp(goodsAttr.strOrigName.c_str(),strOrgName) == 0 )
		{
			dwIndex = i;
			return;
		}
	}
}

void CMainFrame::OnAddArrtBatDel()
{
	// TODO: 在此添加命令处理程序代码
	list<string> m_listBatOrgName;
	list<WORD> m_listBatAddAttrType;
	m_listBatOrgName.clear();
	m_listBatAddAttrType.clear();

	std::ifstream in_f,in_f2;
	in_f.open("OrgName.ini");	in_f2.open("AddAttr.ini");

	if(in_f.fail() || in_f2.fail())
	{
		MessageBox("读取文件失败");
		return;
	}
	//读原始名
	while(true)
	{
		string strOrgName;
		in_f>>strOrgName;				//原始名
		if( strOrgName.empty())
		{
			break;
		}
		m_listBatOrgName.push_back(strOrgName);
	}
	//读附加属性
	while(true)
	{
		WORD dwAddAttrType;
		in_f2>>dwAddAttrType;				//附加属性值
		m_listBatAddAttrType.push_back(dwAddAttrType);
		if( in_f2.eof())
			break;
	}
	in_f.close();		in_f2.close();
	for(list<string>::iterator it = m_listBatOrgName.begin(); it != m_listBatOrgName.end();it++)
	{
		DWORD dwIndex = -1;
		GetGoodsAddrByOrgName((*it).c_str(),dwIndex);
		if( dwIndex != -1 )
		{
			//遍历需要删除的附加属性
			for(list<WORD>::iterator itType = m_listBatAddAttrType.begin(); itType != m_listBatAddAttrType.end();itType++)
			{
				WORD wDelType = (WORD)(*itType) - 1;
				//遍历现有的所有附加属性
				for( vector<tagAddAttr>::iterator it = vecGoodAttr[dwIndex].vecAddAttr.begin(); it != vecGoodAttr[dwIndex].vecAddAttr.end();it++)
				{
					//如果有，则删除
					if( it->wType == wDelType)
					{
						vecGoodAttr[dwIndex].vecAddAttr.erase(it);
						if( m_wndGoodAttr.GetEditFlag() )
						{
							m_wndGoodAttr.UpDataOwnAttrList(dwIndex);
						}
						//跳出，继续下一条
						break;
					}
				}
			}
		}
	}
}

void CMainFrame::OnEduceOneAttribute()
{
	// TODO: 在此添加命令处理程序代码
	std::ifstream in_f;
	in_f.open("EduceSetup.ini");

	long lFlag = 0;			//	1:基本属性，2：附加属性
	long lAttrIndex = 0;	//	属性索引
	long lValueFlag = 0;	//	值几
	while (ReadTo(in_f,"#"))
	{
		in_f>>lFlag
			>>lAttrIndex
			>>lValueFlag;
	}
	in_f.close();

	std::ofstream ofs("EduceData.ini",ios::out);

	//	遍历所有物品
	vector<tagGoodAttr>::iterator pAttr = vecGoodAttr.begin();
	for(; pAttr != vecGoodAttr.end(); pAttr++)
	{
		tagGoodAttr *pstGoodAttr = &(*pAttr);
		if( lFlag == 2 )
		{
			char szValue[128] = "\\";
			vector<tagAddAttr>::iterator it = pstGoodAttr->vecAddAttr.begin(); 
			for(; it != pstGoodAttr->vecAddAttr.end();it++)
			{
				tagAddAttr *pAttr = &(*it);
				//	属性索引
				if( pAttr->wType == lAttrIndex)
				{
					//	值几
					if( lValueFlag == 1 )
					{
						sprintf(szValue,"%d",pAttr->lValue1);
					}
					else
					{
						sprintf(szValue,"%d",pAttr->lValue2);
					}

					ofs.write(pstGoodAttr->strName.c_str(),strlen(pstGoodAttr->strName.c_str()));
					ofs.write("	",1);
					ofs.write(pstGoodAttr->strOrigName.c_str(),strlen(pstGoodAttr->strOrigName.c_str()));
					ofs.write("	",1);
					ofs.write(szValue,strlen(szValue));
					ofs.write("\n",1);
				}
			}
		}
	}
	ofs.close();
}

void CMainFrame::OnCalculateAddAttrValue2()
{
	// TODO: 在此添加命令处理程序代码

	std::ifstream file;
	file.open("Value2.ini");
	if(file.fail())
	{
		MessageBox("读取文件ItemConvert_E.ini失败");
		return;
	}

	list<long>	listFuJiaType;
	long lAmount;
	long lType;
	//	读项
	while (ReadTo(file,"#"))
	{
		//	附加属性数目
		file>>lAmount;
		//	读附加属性的类别
		for(int i=0;i<lAmount;i++)
		{
			file>>lType;
			listFuJiaType.push_back(lType);
		}
		break;
	}

	map<long,string>	mapFujiaValue;
	map<string,map<long,string>> mapOrgNameMapToValue;
	//	读物品
	string szOrgName;
	while ( ReadTo(file,"#"))
	{
		//	原始名
		file>>szOrgName;
		mapFujiaValue.clear();
		//	某条附加属性对应的值
		list<long>::iterator it = listFuJiaType.begin();
		for(; it!=listFuJiaType.end();it++)
		{
			long key = (long)(*it);
			string szValue;
			file>>szValue;
			if( strcmp(szValue.c_str(),"#") == 0 )
			{
				AfxMessageBox("数据有错误");
				return;
			}
			mapFujiaValue[key] = szValue;
		}
		mapOrgNameMapToValue[szOrgName] = mapFujiaValue;
	}
	file.close();

	//	遍历所有物品
	vector<tagGoodAttr>::iterator itAllGoods = vecGoodAttr.begin();
	for( ; itAllGoods != vecGoodAttr.end();itAllGoods++)
	{
		map<string,map<long,string>>::iterator itOrgName = mapOrgNameMapToValue.find(itAllGoods->strOrigName);
		//	如果此物品需要更新属性
		if( itOrgName != mapOrgNameMapToValue.end() )
		{
			//	遍历该物品的所有附加属性
			vector<tagAddAttr>::iterator itAddAttr = itAllGoods->vecAddAttr.begin();
			for(; itAddAttr != itAllGoods->vecAddAttr.end(); itAddAttr++ )
			{
				//	遍历需要修改的附加属性列表
				map<long,string> mapValueTemp = itOrgName->second;
				map<long,string>::iterator itFind = mapValueTemp.find(itAddAttr->wType);
				if( itFind != mapValueTemp.end())
				{
					itAddAttr->lValue2 = atoi(itFind->second.c_str());
				}
			}
		}

	}
}

void CMainFrame::OnSearch()
{
	// TODO: 在此添加命令处理程序代码
	Search search(this);
	search.DoModal();
	return;
}
