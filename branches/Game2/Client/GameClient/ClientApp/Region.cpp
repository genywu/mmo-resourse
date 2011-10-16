#include "StdAfx.h"
#include "Region.h"
#include "../../Engine/utility.h"
#include "../../Public/Common/BaseDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRegion::CRegion(void)
{
	SetType(TYPE_REGION);
	m_lRegionType = 0;
	m_lConfigID = 0;
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
	SAFE_DELETE(m_pCell);
	SAFE_DELETE_ARRAY(m_pCellObj);
	m_vectorSwitch.clear();
}

// 读取
BOOL CRegion::Load()
{
	char strFileName[MAX_PATH];
	sprintf(strFileName, "regions/%d.rgn", GetResourceID());
	CRFile* rfile;
	rfile = rfOpen(strFileName);
	if( rfile == NULL )
	{
		assert(0);
		return false;
	}

	// 读文件头信息
	char strHead[MAX_PATH];
	memset(strHead, 0, MAX_PATH);
	long lVersion = 0;
	rfile->ReadData(strHead, lstrlen(g_strRegionHead));
	if( strcmp(strHead, g_strRegionHead) != 0 )
	{
		assert(0);
		rfClose(rfile);
		return false;
	}

	rfile->ReadData(&lVersion, sizeof(long));
	if( lVersion != g_lRegionVersion )
	{
		assert(0);
		rfClose(rfile);
		return false;
	}

	// 读具体数据
	SAFE_DELETE(m_pCell);
	m_vectorSwitch.clear();
	m_strFileName = strFileName;

	rfile->ReadData(&m_lRegionType, sizeof(long));
	rfile->ReadData(&m_lWidth, sizeof(long));
	rfile->ReadData(&m_lHeight, sizeof(long));


	m_pCell = new tagCell[m_lWidth*m_lHeight];

	rfile->ReadData(m_pCell, sizeof(tagCell)*m_lWidth*m_lHeight);

	long lSwitchNum = 0;
	tagSwitch stSwitch;
	rfile->ReadData(&lSwitchNum, sizeof(long));
	for(int i=0; i<lSwitchNum; i++)
	{
		rfile->ReadData(&stSwitch, sizeof(tagSwitch));
		m_vectorSwitch.push_back(stSwitch);
	}

	rfClose(rfile);

	m_pCellObj = new tagCellObj[m_lWidth*m_lHeight];
	memset(m_pCellObj, 0, sizeof(tagCellObj)*m_lWidth*m_lHeight);
	return true;
}

// 存储
BOOL CRegion::Save()
{
	char strFileName[MAX_PATH];
	sprintf(strFileName, "regions/%d.rgn", GetResourceID());

	FILE* file;
	file = fopen(strFileName, "wb");
	if( file == NULL )
	{
		assert(0);
		return false;
	}

	m_strFileName = strFileName;

	fwrite(g_strRegionHead, strlen(g_strRegionHead), 1, file);
	fwrite(&g_lRegionVersion, sizeof(long), 1, file);

	fwrite(&m_lRegionType, sizeof(long), 1, file);
	fwrite(&m_lWidth, sizeof(long), 1, file);
	fwrite(&m_lHeight, sizeof(long), 1, file);

	fwrite(m_pCell, sizeof(tagCell)*m_lWidth*m_lHeight, 1, file);

	long lSwitchNum = (long)m_vectorSwitch.size();
	fwrite(&lSwitchNum, sizeof(long), 1, file);
	for(int i=0; i<lSwitchNum; i++)
	{
		fwrite(&m_vectorSwitch[i], sizeof(tagSwitch), 1, file);
	}

	fclose(file);
	return true;
}

// 新建
BOOL CRegion::New()
{
	SAFE_DELETE(m_pCell);
	m_vectorSwitch.clear();

	m_pCell = new tagCell[m_lWidth*m_lHeight];
	memset(m_pCell, 0, sizeof(tagCell)*m_lWidth*m_lHeight);

	return true;
}
