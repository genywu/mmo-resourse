#include "StdAfx.h"
#include ".\region.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRegion::CRegion(void)
{
	SetType(TYPE_REGION);
	m_lRegionType = 0;
	m_lResourceID = 0;					// 资源ID
	m_fExpScale = 1.0;					// 获得的经验倍数
	m_lWidth = 0;
	m_lHeight = 0;
	m_pCell = NULL;
	m_pCellObj = NULL;
}

CRegion::~CRegion(void)
{
	m_strFileName = "";
	SAFE_DELETE_ARRAY(m_pCell);
	SAFE_DELETE_ARRAY(m_pCellObj);
	m_vectorSwitch.clear();
}

// 读取
BOOL CRegion::Load()
{
	char strFileName[MAX_PATH];
	sprintf(strFileName, "data/regions/%d.rgn", GetResourceID());

	FILE* file = fopen(strFileName,"r");
	if( file == NULL )
	{
		char szInfo[256];
		sprintf(szInfo,"CRegion::Load(%d.rgn) Failed!",GetResourceID());
		MessageBox(NULL,szInfo,"Warning",MB_OK);
		return false;
	}

	// 读文件头信息
	char strHead[MAX_PATH];
	memset(strHead, 0, MAX_PATH);
	long lVersion = 0;
	fread(strHead,strlen(g_strRegionHead),1,file);
	if( strcmp(strHead, g_strRegionHead) != 0 )
	{
		fclose(file);
		return false;
	}

	fread(&lVersion,sizeof(long),1,file);
	if( lVersion != g_lRegionVersion )
	{
		fclose(file);
		return false;
	}

	// 读具体数据
	SAFE_DELETE_ARRAY(m_pCell);
	m_vectorSwitch.clear();
	m_strFileName = strFileName;

	fread(&m_lRegionType,sizeof(long),1,file);
	fread(&m_lWidth,sizeof(long),1,file);
	fread(&m_lHeight,sizeof(long),1,file);


	SAFE_DELETE_ARRAY(m_pCell);
	m_pCell = new tagCell[m_lWidth*m_lHeight];

	fread(m_pCell,sizeof(tagCell)*m_lWidth*m_lHeight,1,file);

	long lSwitchNum = 0;
	tagSwitch stSwitch;
	fread(&lSwitchNum,sizeof(long),1,file);
	for(int i=0; i<lSwitchNum; i++)
	{
		fread(&stSwitch,sizeof(tagSwitch),1,file);
		m_vectorSwitch.push_back(stSwitch);
	}

	fclose(file);
	SAFE_DELETE_ARRAY(m_pCellObj);
	m_pCellObj = new tagCellObject[m_lWidth*m_lHeight];
	memset(m_pCellObj,0,sizeof(tagCellObject)*m_lHeight*m_lWidth);
	return true;
}

// 新建
BOOL CRegion::New()
{
	SAFE_DELETE_ARRAY(m_pCell);
	m_vectorSwitch.clear();

	m_pCell = new tagCell[m_lWidth*m_lHeight];
	memset(m_pCell, 0, sizeof(tagCell)*m_lWidth*m_lHeight);

	return true;
}
//BYTE CRegion::GetBlock(int x, int y)	
//{
//	return (x>-1&&x<GetWidth()&&y>-1&&y<GetHeight())?m_pCell[GetWidth()*y+x].bBlock:BLOCK_UNFLY;
//}
//long CRegion::GetWidth()	
//{
//	return m_lWidth;
//}
//long CRegion::GetHeight()	
//{
//	return  m_lHeight;
//}