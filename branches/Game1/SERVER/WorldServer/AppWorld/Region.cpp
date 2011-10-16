#include "StdAfx.h"
#include ".\region.h"


CRegion::CRegion(void)
{
	SetType(TYPE_REGION);
	m_lRegionType = 0;
	m_lResourceID = 0;					// 场景资源ID
	m_lSvrResourceID = 0;
	m_fExpScale = 1.0;					// 获得经验系数
	m_fOccuExpScale = 1.0f;
	m_fSpScale = 1.0f;
	m_fMeriScale = 1.0f;
	m_lWidth = 0;
	m_lHeight = 0;
	m_pCell = NULL;

	m_lID = 0;
	m_lLinedIdFlag = -1;
	m_byRgnHideFlag = 0;
}

CRegion::~CRegion(void)
{
	m_strFileName = "";
	M_FREE(m_pCell,sizeof(tagCell)*m_lWidth*m_lHeight);
	m_vectorSwitch.clear();
}

// 读取
BOOL CRegion::Load()
{
	char strFileName[MAX_PATH];
	sprintf(strFileName, "regions/%d.rgn", GetID());
	
	CRFile *rfile = rfOpen(strFileName);
	if( rfile == NULL)
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
		return false;
	}

	rfile->ReadData(&lVersion, sizeof(long));
	if( lVersion != g_lRegionVersion )
	{
		assert(0);
		return false;
	}

	m_strFileName = strFileName;

	// 读具体数据
	rfile->ReadData(&m_lRegionType, sizeof(long));
	rfile->ReadData(&m_lWidth, sizeof(long));
	rfile->ReadData(&m_lHeight, sizeof(long));

	New();

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
	return true;
}

// 存储
BOOL CRegion::Save()
{
	char strFileName[MAX_PATH];
	sprintf(strFileName, "regions/%d.rgn", GetID());

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
	M_FREE(m_pCell,sizeof(tagCell)*m_lWidth*m_lHeight);
	m_vectorSwitch.clear();

	m_pCell = (tagCell*)M_ALLOC(sizeof(tagCell)*m_lWidth*m_lHeight);
	memset(m_pCell, 0, sizeof(tagCell)*m_lWidth*m_lHeight);

	return true;
}

// 添加到CByteArray
bool CRegion::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	CBaseObject::AddToByteArray(pByteArray, bExData);

	_AddToByteArray(pByteArray, GetID());
	_AddToByteArray(pByteArray, m_lLinedIdFlag);
	_AddToByteArray(pByteArray, m_lRgnType);
	_AddToByteArray(pByteArray, m_lRegionType);
	_AddToByteArray(pByteArray, m_lResourceID);
	_AddToByteArray(pByteArray, m_fExpScale);
	_AddToByteArray(pByteArray, m_fOccuExpScale);
	_AddToByteArray(pByteArray, m_fSpScale);
	_AddToByteArray(pByteArray, m_fMeriScale);
	_AddToByteArray(pByteArray, m_lWidth);
	_AddToByteArray(pByteArray, m_lHeight);
	_AddToByteArray(pByteArray, m_btCountry);
	_AddToByteArray(pByteArray, m_lNotify);
	_AddToByteArray(pByteArray, m_lACRgnType);
	_AddToByteArray(pByteArray, m_byRgnHideFlag);

	int len=sizeof(tagCell);
	_AddToByteArray(pByteArray, (BYTE*)m_pCell, sizeof(tagCell)*m_lWidth*m_lHeight);
	
	long n = (long)m_vectorSwitch.size();
	_AddToByteArray(pByteArray, n);
	for(int i=0; i<n; i++)
	{
		_AddToByteArray(pByteArray, (BYTE*)&m_vectorSwitch[i], sizeof(tagSwitch));
	}

	return true;
}

// 解码
bool CRegion::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	CBaseObject::DecordFromByteArray(pByte, pos, bExData);
	SetID(_GetLongFromByteArray(pByte, pos));
	m_lLinedIdFlag = _GetLongFromByteArray(pByte, pos);
	m_lRgnType = _GetLongFromByteArray(pByte, pos);
	m_lRegionType = _GetLongFromByteArray(pByte, pos);
	m_lResourceID = _GetLongFromByteArray(pByte, pos);
	m_fExpScale = _GetFloatFromByteArray(pByte, pos);
	m_fOccuExpScale = _GetFloatFromByteArray(pByte, pos);
	m_fSpScale = _GetFloatFromByteArray(pByte, pos);
	m_fMeriScale = _GetFloatFromByteArray(pByte, pos);
	m_lWidth = _GetLongFromByteArray(pByte, pos);
	m_lHeight = _GetLongFromByteArray(pByte, pos);
	m_btCountry = _GetByteFromByteArray(pByte, pos);
	m_lNotify	= _GetLongFromByteArray(pByte, pos);	
	m_lACRgnType = _GetLongFromByteArray(pByte, pos);
	m_byRgnHideFlag = _GetByteFromByteArray(pByte, pos);
	return true;
}

// 获取一个随机不是阻挡的点
bool CRegion::GetRandomPos( long &lX , long &lY )
{
	return GetRandomPosInRange(lX, lY, 0, 0, GetWidth(), GetHeight());
}

// 功能：在一个范围内获取一个随机不是阻挡的点
// 如果范围设置出错，那么在整个场景获取随机点
// 如果1000次都没有找到合适的点，那么依次遍历整个范围寻找不是阻挡的点
// 如果依次遍历都不行，那么把范围扩大一圈进行查找
// 如果整个场景都没有找到合适的点，那么随机返回一个点（不管阻挡与否）
bool CRegion::GetRandomPosInRange(long &lX , long &lY, long sx, long sy, long w, long h)
{

_Begin:

	// 区域边界判断，如果不符合就设置为整个场景
	if (sx>=GetWidth() || sy>=GetHeight() || w<0 || h<0 || sx+w<0 || sy+h<0)
	{
		sx = 0;
		sy = 0;
		w = GetWidth();
		h = GetHeight();
	}

	if (sx<0)
	{
		w += sx;
		sx = 0;
	}

	if (sy<0)
	{
		h += sy;
		sy = 0;
	}

	if (sx+w>GetWidth())
		w = GetWidth() - sx;

	if (sy+h>GetHeight())
		h = GetHeight() - sy;


	// 尝试1000次获取随机点
	for(int i=0; i<1000; i++)
	{
		lX	= sx + random( w );
		lY	= sy + random( h );

		tagCell* pCell = GetCell( lX, lY );
		if (pCell)
		{
			if (pCell->bBlock == BLOCK_NO)
			{
				if (pCell->lSwitch == 0)
				{
					return true;
				}
			}
		}
	}

	// 如果1000次找不到，那么依次遍历一遍
	for (lX=sx; lX<sx+w; lX++)
	{
		for (lY=sy; lY<sy+h; lY++)
		{
			tagCell* pCell = GetCell(lX, lY);
			if (pCell)
			{
				if (pCell->bBlock == BLOCK_NO)
				{
					if (pCell->lSwitch == 0)
					{
						return true;
					}
				}
			}
		}
	}


	// 如果已经是整张地图搜索过了，那么只有随机取一个点了-_-||
	if (sx<=0 && sy<=0 && w>=GetWidth() && h>=GetHeight())
	{
		lX = random(GetWidth());
		lY = random(GetHeight());
		return false;
	}


	// 如果依次遍历都不行，那么把范围扩大10圈进行查找
	sx -= 10;
	sy -= 10;
	w += 20;
	h += 20;
	goto _Begin;

	return false;
}
