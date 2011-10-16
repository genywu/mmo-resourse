//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年1月7日 11:29:18
// 更新时间 : 
// 摘要     :
//---------------------------------------------------------------------------------

//修正：2005/1/17 贾涛
#pragma once

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"

class EditMap;

// 纹理编码与UV的对应表
static FLOAT CodeUVTable[32][8] = 
{
	// v0            v1              v2              v3
	// 右边16块纹理块
	{0.002f, 0.002f, 0.123f, 0.002f, 0.123f, 0.248f, 0.002f, 0.248f},
	{0.127f, 0.002f, 0.248f, 0.002f, 0.248f, 0.248f, 0.127f, 0.248f},
	{0.252f, 0.002f, 0.373f, 0.002f, 0.373f, 0.248f, 0.252f, 0.248f},
	{0.377f, 0.002f, 0.498f, 0.002f, 0.498f, 0.248f, 0.377f, 0.248f},
														 		 	
	{0.002f, 0.252f, 0.123f, 0.252f, 0.123f, 0.498f, 0.002f, 0.498f},
	{0.127f, 0.252f, 0.248f, 0.252f, 0.248f, 0.498f, 0.127f, 0.498f},
	{0.252f, 0.252f, 0.373f, 0.252f, 0.373f, 0.498f, 0.252f, 0.498f},
	{0.377f, 0.252f, 0.498f, 0.252f, 0.498f, 0.498f, 0.377f, 0.498f},
														 		 	
	{0.002f, 0.502f, 0.123f, 0.502f, 0.123f, 0.748f, 0.002f, 0.748f},
	{0.127f, 0.502f, 0.248f, 0.502f, 0.248f, 0.748f, 0.127f, 0.748f},
	{0.252f, 0.502f, 0.373f, 0.502f, 0.373f, 0.748f, 0.252f, 0.748f},
	{0.377f, 0.502f, 0.498f, 0.502f, 0.498f, 0.748f, 0.377f, 0.748f},
														 		 	
	{0.002f, 0.752f, 0.123f, 0.752f, 0.123f, 0.998f, 0.002f, 0.998f},
	{0.127f, 0.752f, 0.248f, 0.752f, 0.248f, 0.998f, 0.127f, 0.998f},
	{0.252f, 0.752f, 0.373f, 0.752f, 0.373f, 0.998f, 0.252f, 0.998f},
	{0.377f, 0.752f, 0.498f, 0.752f, 0.498f, 0.998f, 0.377f, 0.998f},

	// 用于替换的纹理块UV
	//// 左边2块代码为1111的纹理块
	//{0.002f, 0.002f, 0.123f, 0.002f, 0.123f, 0.248f, 0.002f, 0.248f},
	//{0.377f, 0.752f, 0.498f, 0.752f, 0.498f, 0.998f, 0.377f, 0.998f},

	// 右边16块纹理块
	{0.502f, 0.002f, 0.623f, 0.002f, 0.623f, 0.248f, 0.502f, 0.248f},
	{0.627f, 0.002f, 0.748f, 0.002f, 0.748f, 0.248f, 0.627f, 0.248f},
	{0.752f, 0.002f, 0.873f, 0.002f, 0.873f, 0.248f, 0.752f, 0.248f},
	{0.877f, 0.002f, 0.998f, 0.002f, 0.998f, 0.248f, 0.877f, 0.248f},
								 								 	 	
	{0.502f, 0.252f, 0.623f, 0.252f, 0.623f, 0.498f, 0.502f, 0.498f},
	{0.627f, 0.252f, 0.748f, 0.252f, 0.748f, 0.498f, 0.627f, 0.498f},
	{0.752f, 0.252f, 0.873f, 0.252f, 0.873f, 0.498f, 0.752f, 0.498f},
	{0.877f, 0.252f, 0.998f, 0.252f, 0.998f, 0.498f, 0.877f, 0.498f},
								 								 	 	
	{0.502f, 0.502f, 0.623f, 0.502f, 0.623f, 0.748f, 0.502f, 0.748f},
	{0.627f, 0.502f, 0.748f, 0.502f, 0.748f, 0.748f, 0.627f, 0.748f},
	{0.752f, 0.502f, 0.873f, 0.502f, 0.873f, 0.748f, 0.752f, 0.748f},
	{0.877f, 0.502f, 0.998f, 0.502f, 0.998f, 0.748f, 0.877f, 0.748f},
						 		 								 	 	
	{0.502f, 0.752f, 0.623f, 0.752f, 0.623f, 0.998f, 0.502f, 0.998f},
	{0.627f, 0.752f, 0.748f, 0.752f, 0.748f, 0.998f, 0.627f, 0.998f},
	{0.752f, 0.752f, 0.873f, 0.752f, 0.873f, 0.998f, 0.752f, 0.998f},
	{0.877f, 0.752f, 0.998f, 0.752f, 0.998f, 0.998f, 0.877f, 0.998f},
};

enum LOCATION {BOTTOM_LEFT = 0, BOTTOM_RIGHT, TOP_RIGHT, TOP_LEFT};
static BYTE TexCode[4] = { 2, 1, 4, 8 };


// 笔刷半径
enum BRUSHSIZE 
{
	BS_TINY   = 0,                  
	BS_SMALL  = 1,                  
	BS_MEDIUM = 2,                  
	BS_LARGE  = 4,                  
	BS_HUGE   = 8                   
};

// 笔刷类型
enum BRUSHTYPE
{
	BT_CIRCLE = 0,                  // 圆形
	BT_QUAD                         // 方形
};

// 编辑类型
enum EDITTYPE
{
	ET_ADD,                         // 相加
	ET_REPLACE,                     // 替换
	ET_NOISE,                       // 噪音随机
	ET_SMOOTH                       // 平滑
};


#define MAXTERRAINTRIANGLECACHESIZE 131072

class TerrainMesh
{
	//-----------------------------------------------------------------------------
	// 成员变量
	//缓冲
	DWORD m_dwTextureCount;//纹理数量

	render::Interface::Layer3D::_tagTriangle * m_pTriangleCache;
	DWORD m_dwTriangleCacheUsage;

	struct tagRenderLayer
	{
		render::Interface::Layer3D::_tagTriangle * pRenderHeader;
	};

	struct tagRenderChain
	{
		tagRenderLayer pRenderLayer[16];//最大16个纹理
	};

	tagRenderChain m_pRenderChain[4];//4 层

	

public:
	struct _tagVertex
	{
		DWORD       dwVertexIndex;    // 顶点索引
		D3DXVECTOR3 vPosition;
		D3DXVECTOR3 vNormal;
		D3DCOLOR    cColor;
		D3DCOLOR	cAddColor;
		BYTE        byTexID;          // 每个顶点都有自己的纹理ID

		DWORD       dwBelongToNum;    // 被使用的次数
		DWORD       dwBelongToGrid[4];// 使用了该顶点的4个网格的索引
	};

	struct _tagGrid
	{
		DWORD       dwGridIndex;      // 网格索引
		DWORD       dwVertexIndex[4]; // 使用的顶点索引
		BYTE        byTexCode[4];     // 4层纹理的纹理编码，据此查表得到相应的纹理坐标。
		BYTE        byTexIndex[4];    // 4个顶点按纹理排序后的顺序
		BYTE        uv[4];			  // 4层纹理的UV编码索引
	};

	struct _tagProcessData            // 操作参数
	{
		BYTE  bBrushSize;             // 画刷大小
		BYTE  bBrushType;             // 画刷类型
		INT	  nCurrentTextureID;      // 当前选定的纹理
		DWORD dwSelectedGridIndex;    // 选中的网格索引
		DWORD dwSelectedVertexIndex;  // 选中网格中选中顶点的索引
	};

private:
	_tagVertex  *m_ptVertexList;        // 顶点列表	
	DWORD        m_dwNumVertex;			// 顶点数
	
	_tagGrid*    m_ptGrid;              // 网格列表
	DWORD        m_dwNumGrid;           // 网格数

	DWORD        m_dwWidth;             // 地表格数
	DWORD        m_dwDepth;

	DWORD		 m_dwShaderGroupHandle;
	DWORD        m_dwTexSetIndex;       // 记录使用的纹理集

	BOOL         m_bIntersectFlag;      // 标记鼠标是否位于网格上

    BOOL         m_bDrawBrushFlag;      // 是否绘制画刷

	INT          m_nDefaultBackTextureID;   // 默认的底层纹理ID

	_tagProcessData m_tProcessData; 

	D3DXVECTOR3 *QuadVertex, *GridVertex;

	//-----------------------------------------------------------------------------
	// 成员函数
public:
	TerrainMesh(void);
	~TerrainMesh(void);

	// 创建Shader
	DWORD CreateShader(TCHAR* pszFilename);

	// 创建地表网格
	BOOL CreateTerrain(DWORD dwWidth, DWORD dwDepth);

	// 销毁数据
	VOID Destroy();

	// 渲染网格
	void Render(BOOL bDrawNormal,BOOL bShowTexture, render::Camera * pCamera);
	void RenderAll();

	// 渲染小地图
	BOOL RenderMiniMap(TCHAR *filename);

	// 渲染笔刷
	VOID DrawBrush();

	//返回格子
	_tagGrid * GetGrid(DWORD dwGridIndex);
	_tagGrid * GetGridList() { return m_ptGrid; }

	DWORD GetGridNum() { return m_dwNumGrid; }

	//返回格子中心
	void GetGridCenter(DWORD dwGridIndex,D3DXVECTOR3 *pCenter);
	DWORD GetGridColor(DWORD dwGridIndex);

	//返回顶点
	_tagVertex * GetVertex(DWORD dwVertexIndex);
	_tagVertex * GetVertexList() { return  m_ptVertexList;}
	DWORD GetVertexNum() { return m_dwNumVertex; }

	// 判断点击时是否进行纹理处理
	BOOL ProcessLBDown(render::Camera *pCamera, BOOL bChangeTex = TRUE);

	// 设定当前选定的纹理
	VOID SetSelectedTextureID(INT nTextureID);

	// 取得ShaderHandle
	DWORD GetShaderHandle()	{ return m_dwShaderGroupHandle; }

	// 设置笔刷大小
	VOID SetBrushSize(BYTE bBrushSize);
	BYTE GetBrushSize() { return m_tProcessData.bBrushSize; }

	// 设置笔刷类型
	VOID SetBrushType(BYTE bBrushType);

	// 设置默认的底层纹理ID
	INT SetDefaultBackTextureID(INT nDefaultBackTextureID = 0);

	// 填充指定顶点的数据
	VOID FillVertex(std::vector<_tagVertex> vOldVertexArray, BOOL bRecalculateTex = TRUE);

	// 重新计算网格的纹理
	VOID RecalculateGridTexture(DWORD dwGridIndex, DWORD dwVertexIndex, std::vector<_tagVertex> &vOldVertexArray);

	// 获取处理数据
	VOID GetProcessData(_tagProcessData* pData);

	// 设置处理数据
	VOID SetProcessData(_tagProcessData* pData);

	// 设置是否绘制画刷
	BOOL SetDrawBrushFlag(BOOL bFlag = TRUE);

	// 调整地形高度
	VOID ModifyTerrainHeight(std::vector<_tagVertex> &vOldVertexArray, DWORD dwGridIndex, DWORD dwVertexIndex, 
		FLOAT fOffsetY = 0.0f, EDITTYPE et = ET_ADD);

	// 计算选定网格的法向
	VOID CalculateGridNormal(DWORD dwGridIndex);

	// 计算指定位置的顶点的法向
	VOID CalculateVertexNormal(DWORD row, DWORD column);

	// load and save file
	BOOL SaveTerrainMesh(FILE* pFile);
	BOOL LoadTerrainMesh(FILE* pFile,bool bnew);

	// 导入为客户端数据
	BOOL ImportTerrainMesh(FILE* pFile);

	// access texture set
	DWORD GetTexSetIndex() { return m_dwTexSetIndex; }
	VOID SetTexSetIndex(DWORD dwTexSetIndex) { m_dwTexSetIndex = dwTexSetIndex; }

	// 计算鼠标是否选中顶点
	BOOL CalculateIntersect();

	// 从贴图中取色，并应用到地形上。
	void ApplyColorMap(TCHAR *filename);

	// 从高度图得到地形数据
	void ImportHeightMap(TCHAR *filename);
	// 将地形数据导出为高度图
	void ExportHeightMap(TCHAR *filename);

private:
	// 查表取得随机UV值
	BYTE GetRandomUV();

	// 将网格的纹理块编码转换为UV。
	VOID Code2UV(_tagGrid* pGrid);

	// 根据选定顶点位置处理网格的纹理替换
	VOID ProcessGridTexture(_tagGrid* pGrid, INT nLocation);

	// 判断索引是否有效
	BOOL ValidIndex(INT nIndex, BOOL bGridFlag = TRUE);

	// 判断行列是否有效
	BOOL ValidLocation(INT nRow, INT nColumn, BOOL bGridFlag = TRUE);

	// 将输入的数调整到指定范围内
	FLOAT Clamp(FLOAT &num, FLOAT field);

	render::Interface::Layer3D::_tagTriangle * GetFreeTriangleInCache();

	void AddToRenderChain(DWORD dwLayer,DWORD dwTexIndex,render::Interface::Layer3D::_tagTriangle *pTriangle);
	void ClearRenderCache();

public:
	void ExportColorMap(const char * pszFileName);
	inline DWORD MultiplyColor(DWORD dwColorA,DWORD dwColorB);

	DWORD GetTriangleCacheUsage() const { return m_dwTriangleCacheUsage;}

	DWORD GetWidth(){return m_dwWidth;}
	DWORD GetDepth(){return m_dwDepth;}
};
