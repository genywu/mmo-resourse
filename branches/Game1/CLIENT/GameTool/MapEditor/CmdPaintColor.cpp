#include "StdAfx.h"
#include "cmdpaintcolor.h"
#include "terrainmesh.h"
#include "map.h"
#include "commdb.h"

CmdMapPaintColor::CmdMapPaintColor(void)
{
}

CmdMapPaintColor::~CmdMapPaintColor(void)
{
}

void CmdMapPaintColor::Execute(void)
{
	Cmd::Execute();
	//保存颜色

	EditMap *pMap = CommDB::GetInstance()->GetMap();
	DWORD dwMapWidth = pMap->GetWidth();
	DWORD dwMapDepth = pMap->GetDepth();
	
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagVertex *pTempVertex,*pVertexList = pMesh->GetVertexList();

	D3DXVECTOR3 *pCenterPosition = &pVertexList[m_dwVertexIndex].vPosition;//中心点坐标


    //求出要运算的顶点列表
	int iCenterRow = (int)(m_dwVertexIndex % (dwMapWidth + 1));
	int iCenterCol = (int)(m_dwVertexIndex / (dwMapWidth + 1));
	int iRadius = (int)m_fBrushRadius;
	for(int i = - iRadius; i <= iRadius; i++)
	{
		int iR = iCenterRow + i;
		if (iR < 0 || iR >= (int)(dwMapWidth +1)) 
		{
			continue;//越界
		}
		for(int j = -iRadius; j <=  iRadius; j++)
		{
			int iC =  iCenterCol + j;
			if (iC < 0 || iC >= (int)(dwMapDepth +1)) 
			{
				continue;//越界
			}

			DWORD dwVertexIndex = iR + iC * (int)(dwMapWidth + 1);

			pTempVertex = &pVertexList[dwVertexIndex];
			//计算顶点的距离是否在半径中
			float dx = pTempVertex->vPosition.x - pCenterPosition->x;
			float dz = pTempVertex->vPosition.z - pCenterPosition->z;

			float fDis = sqrtf(dx * dx + dz * dz);

			if (fDis <= m_fBrushRadius)
			{
				//保存要修改的定点索引表
				m_vVertexIndexArray.push_back(dwVertexIndex);
				m_vVertexColorArray.push_back(pTempVertex->cAddColor);
			}
		 }
	}

	std::vector<DWORD>::iterator it;
	//画刷类型
	if (m_dwBrushType == 0)
	{
		//铅笔。。。直接画
		for(it = m_vVertexIndexArray.begin(); it != m_vVertexIndexArray.end(); it++)
		{
			DWORD dwVertexIndex = *it;
			pTempVertex = &pVertexList[dwVertexIndex];
			DWORD &dwDestColor = pTempVertex->cAddColor;
			//混合运算
			DWORD sr = (m_dwColor & 0x00FF0000) >> 16;
			DWORD sg = (m_dwColor & 0x0000FF00) >> 8;
			DWORD sb = (m_dwColor & 0x000000FF);
			DWORD dr = (dwDestColor & 0x00FF0000) >> 16;
			DWORD dg = (dwDestColor & 0x0000FF00) >> 8;
			DWORD db = (dwDestColor & 0x000000FF);

			DWORD a = (DWORD)(m_fColorChroma * 255.0f);

			DWORD r = ((sr * a) + dr * (255 - a)) / 255;
			DWORD g = ((sg * a) + dg * (255 - a)) / 255;
			DWORD b = ((sb * a) + db * (255 - a)) / 255;

			dwDestColor = 0xFF000000 | r << 16 | g <<8 | b;
		}
	}
	else
	{
		//毛笔。。。渐变画__混合
		float fIntensity = 1.0f;
		
		for(it = m_vVertexIndexArray.begin(); it != m_vVertexIndexArray.end(); it++)
		{
			DWORD dwVertexIndex = *it;
			pTempVertex = &pVertexList[dwVertexIndex];		
			//计算光照
			DWORD &dwDestColor = pTempVertex->cAddColor;

			D3DXVECTOR3 L = pTempVertex->vPosition - *pCenterPosition;
			float fLightDistance = D3DXVec3Length(&L);		//光源到vertex的距离
			if (fLightDistance == 0.0f)
				fLightDistance = 0.001f;

			float fDivideRatio = fLightDistance * fLightDistance;

			float fValue = (m_fColorChroma * ((float)(m_fBrushRadius + 1)/ fDivideRatio));
			if (fValue < 0.0f)
				fValue = 0.0f;
			if (fValue > 1.0f)
				fValue = 1.0f;

			D3DXCOLOR color(m_dwColor);
			DWORD dwSrcColor = color;

			//混合运算
			DWORD sr = (dwSrcColor & 0x00FF0000) >> 16;
			DWORD sg = (dwSrcColor & 0x0000FF00) >> 8;
			DWORD sb = (dwSrcColor & 0x000000FF);
			DWORD dr = (dwDestColor & 0x00FF0000) >> 16;
			DWORD dg = (dwDestColor & 0x0000FF00) >> 8;
			DWORD db = (dwDestColor & 0x000000FF);

			DWORD a = (DWORD)(fValue * 255);

			DWORD r = ((sr * a) + dr * (255 - a)) / 255;
			DWORD g = ((sg * a) + dg * (255 - a)) / 255;
			DWORD b = ((sb * a) + db * (255 - a)) / 255;

			dwDestColor = 0xFF000000 | r << 16 | g <<8 | b;
		}
	}	
}

void CmdMapPaintColor::UnExecute(void)
{
	Cmd::UnExecute();

	//恢复颜色
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	DWORD dwMapWidth = pMap->GetWidth();
	DWORD dwMapHeight= pMap->GetDepth();
	
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagVertex *pVertexList = pMesh->GetVertexList();

	//恢复颜色
	for(int i = 0; i < (int)m_vVertexIndexArray.size();i++)
	{
		DWORD dwVertexIndex = m_vVertexIndexArray[i];
		pVertexList[dwVertexIndex].cAddColor = m_vVertexColorArray[i];
	}                     

	m_vVertexIndexArray.clear();
	m_vVertexColorArray.clear();
}

void CmdMapPaintColor::Dump(void)
{
	// 输出错误数据
	OutputConsole("Map_Paint_Color-");
}

void CmdMapPaintColor::Destroy()
{
	m_vVertexIndexArray.clear();
	m_vVertexColorArray.clear();
}

