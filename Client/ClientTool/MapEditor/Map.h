#pragma once
#pragma warning(disable:4996)

#include "..\..\engine\render.h"
#include "..\..\EngineExtend\gamemodel.h"

#define AMBIENTCOLOR     0XFF808080
#define DIRECTIONALCOLOR 0XFFFFFFFF

class QNode;
class VisibleSet;
class EditMap;
class LightPoint;

class Grid
{
	friend class EditMap;

public:
	//建筑地物
	struct tagBuild
	{
		WORD wID;
		D3DXVECTOR3 vRotation;
		D3DXVECTOR3	vScaling;
		D3DXVECTOR3 vPosOffset;
		std::vector<DWORD> m_vHideFlagGridIndexArray;		//包含要隐藏自己的标志的格子数组
		void operator = (const tagBuild &tBuild)
		{
			wID = tBuild.wID;
			vRotation = tBuild.vRotation;
			vScaling = tBuild.vScaling;
			vPosOffset = tBuild.vPosOffset;

			m_vHideFlagGridIndexArray.clear();
			for(std::vector<DWORD>::const_iterator it = tBuild.m_vHideFlagGridIndexArray.begin(); it != tBuild.m_vHideFlagGridIndexArray.end(); it++)
				m_vHideFlagGridIndexArray.push_back(*it);
		}
		void AddHideFlagGridIndex(DWORD dwGridIndex)
		{
			std::vector<DWORD>::iterator it;
			for(it = m_vHideFlagGridIndexArray.begin(); it != m_vHideFlagGridIndexArray.end(); it++)
			{	
				if (dwGridIndex == *it)
					return;
			}
			m_vHideFlagGridIndexArray.push_back(dwGridIndex);			
		}
		void RemoveHideFlagGridIndex(DWORD dwGridIndex)
		{
			std::vector<DWORD>::iterator it;
			for(it = m_vHideFlagGridIndexArray.begin(); it != m_vHideFlagGridIndexArray.end(); it++)
			{	
				if (dwGridIndex == *it)
				{
					m_vHideFlagGridIndexArray.erase(it);
					return ;
				}
			}
		}
	};

	struct tagResverd
	{
		DWORD b;
		DWORD by;//
		DWORD f;
	};

	AnimInfo  m_cBuildModelAnimInfo;
	CDisplayModel *pDisplayModel;
	DWORD     m_dwBuildLightAnimTime;

	DWORD     m_dwTargetHideGridIndex;//指向隐藏的建筑格子标记

	//客户端标志
	//0000
	#define GRID_FLAG_VISIBLE       0x0001 //可见
	#define GRID_FLAG_WEATHEREFFECT 0x0002 //天气标志
	BOOL m_dwFlag;			//可见

private:
	//建筑
	tagBuild  m_tBuild;

	//服务器参数
	DWORD m_dwWar;
	DWORD m_dwSecurity;				 //安全区
	DWORD m_dwBlock;				 //阻挡
	DWORD m_dwMtl;					 //材质
	DWORD m_dwDoorIndex;			 //门索引0表示没有

public:
	Grid() { m_dwDoorIndex = 0; m_dwBlock = 0; m_dwMtl = 0; m_dwWar = 0; m_dwSecurity = 0; pDisplayModel = NULL;}
	~Grid() { CGameModelManager::GetInstance()->ReleaseDisplayModel(pDisplayModel);}
	void SetMtl(DWORD dwMtl)  { m_dwMtl = dwMtl;}
	inline DWORD GetMtl()	const  { return m_dwMtl;}

	void SetBlock(DWORD dwBlock) { m_dwBlock = dwBlock;}
	inline DWORD GetBlock()  const	  { return m_dwBlock;}

	void SetSecurity(DWORD dwSecurity) { m_dwSecurity = dwSecurity;}
	inline DWORD GetSecurity()	 const	{ return m_dwSecurity;}

	void SetWar(DWORD dwWar)  { m_dwWar = dwWar;}
	inline DWORD GetWar()	const	{ return m_dwWar;}

	void SetDoorIndex(DWORD dwDoorIndex)  { m_dwDoorIndex = dwDoorIndex;}
	inline DWORD GetDoorIndex()	const  { return m_dwDoorIndex;}

	void SetBuild(const tagBuild &tBuild) {if (m_tBuild.wID != tBuild.wID){CGameModelManager::GetInstance()->ReleaseDisplayModel(pDisplayModel);pDisplayModel = NULL;} m_tBuild = tBuild;}
	tagBuild *GetBuild() { return &m_tBuild;}

	void SetVisible(BOOL bValue) { m_dwFlag = (m_dwFlag & 0xFFFE) | bValue;}
	inline BOOL GetVisible() { return m_dwFlag & 0x0001;}

	void SetWeatherEffectFlag(BOOL bValue) { m_dwFlag = (m_dwFlag & 0xFFFD) | (bValue << 1);}
	BOOL GetWeatherEffectFlag() { return ((m_dwFlag & 0x0002) >> 1);}

	void  SetBuildRotationX(float fBuildRationX)						{ m_tBuild.vRotation.x = fBuildRationX;}
	void  SetBuildRotationY(float fBuildRationY)						{ m_tBuild.vRotation.y = fBuildRationY;}
	void  SetBuildRotationZ(float fBuildRationZ)						{ m_tBuild.vRotation.z = fBuildRationZ;}
	void  SetBuildScalingX(float fBuildScalingX)						{ m_tBuild.vScaling.x = fBuildScalingX;}
	void  SetBuildScalingY(float fBuildScalingY)						{ m_tBuild.vScaling.y = fBuildScalingY;}
	void  SetBuildScalingZ(float fBuildScalingZ)						{ m_tBuild.vScaling.z = fBuildScalingZ;}
	void  SetBuildScaling(D3DXVECTOR3 & vBuildScaling)					{ m_tBuild.vScaling = vBuildScaling;}
	void  SetBuildHeight(float fBuildHeight)							{ m_tBuild.vPosOffset.y  = fBuildHeight;}
	void  SetBuildOffsetXZ(float fBuildOffsetX, float fBuildOffsetZ)	{ m_tBuild.vPosOffset.x = fBuildOffsetX; m_tBuild.vPosOffset.z  = fBuildOffsetZ; }

	void  SetTargetHideGridIndex(DWORD dwGridIndex) { m_dwTargetHideGridIndex = dwGridIndex;}
	DWORD GetTargetHideGridIndex() { return m_dwTargetHideGridIndex;}

	BOOL GetFlag()	{return m_dwFlag;}

};

class EditMap
{
	AnimInfo *m_pGlobalAnimInfo;

	std::vector<int>m_vLightVertexIndexArray;

	//格子信息
	Grid * m_pGridArray;
	int  m_iWidth,m_iDepth;			//地图宽度
	DWORD m_dwVisibleTrianglesCount;

	//阳光
	DWORD		m_dwSunLightAmbientColor;	
	DWORD	    m_dwSunLightDiffuseColor;	
	float	    m_fSunLightIntensitisy;
	D3DXVECTOR3 m_vSunLightDirection;

	BOOL        m_bEnableFog;
	DWORD		m_dwFogStart;
	DWORD		m_dwFogEnd;
	DWORD		m_dwFogColor;

	//空间划分
	QNode *m_pQTRoot;
	DWORD m_dwTreeNodeCount;

	//可见集合
	VisibleSet *m_pVisibleSet;

	typedef std::pair<WORD,DWORD> REFERENCEBUILD_PAIR;
	std::map<WORD,DWORD> m_mapReferenceBuildArray;//建筑引用计数

	DWORD m_dwTotalPalacedBuildCount; //本地图放置所有建筑数量

public:
	EditMap(void);
	~EditMap(void);

	QNode * GetQTRoot(void);
	DWORD GetVisibleTirangle() const { return m_dwVisibleTrianglesCount;}
	//接口

	D3DXVECTOR3 * GetLightDirection() { return &m_vSunLightDirection;}
	void SetLightDirection(const D3DXVECTOR3 *pLightDirection) { m_vSunLightDirection = *pLightDirection;}
	//建立
	void Create(int iWidth,int iDepth);
	void Destroy();

	int GetWidth() const { return m_iWidth;}
	int GetDepth()const { return m_iDepth;}

	inline Grid * GetGrid(int iIndex) { return &m_pGridArray[iIndex];}
	inline Grid * GetGrid(int iX,int iZ) { return &m_pGridArray[iZ * m_iWidth + iX];}
	void   GetTagMapBuffer(int x,int z,DWORD size,D3DXVECTOR3* posbuff);
	VisibleSet * GetVisibleSet() const { return m_pVisibleSet;}
	
	void Render(DWORD dwRenderFlag, render::Camera * pCamera, render::Frustum * pFrustum);
	void Update(render::Frustum *pFrustum);
	
	void GetGridPosition(DWORD dwGridIndex,D3DXVECTOR3 * pPos,D3DXVECTOR3 *pCenter);
	void GetGridPosition(int iX,int iZ,D3DXVECTOR3 * pPos,D3DXVECTOR3 *pCenter);

	void Save(FILE *pFile);
	void Load(FILE * pFile, DWORD dwVersion);
	void CalculateSunLighting(void);
	void CalculateLightSourceLighting(void);


	void CalculateTerrainShadow();
	void DrawAll();

	void  SetSunLightAmbientColor(DWORD dwColor) { m_dwSunLightAmbientColor = dwColor;}
	void  SetSunLightDiffuseColor(DWORD dwColor) { m_dwSunLightDiffuseColor = dwColor;}
	DWORD GetSunLightAmbientColor() { return m_dwSunLightAmbientColor;}
	DWORD GetSunLightDiffuseColor() { return m_dwSunLightDiffuseColor;}
	D3DXVECTOR3 * GetSunLightDirection() { return &m_vSunLightDirection;}
	float GetSunLightIntensitisy() { return m_fSunLightIntensitisy;}

	void CalculateAllBaseLighting(void);
	void CalculateAllLightSourceLighting(void);

	void ImportVisibleMap(const char * pszFileName);
	void ExportVisibleMap(const char * pszFileName);

	void ImportBlockMap(const char * pszFileName);
	void ExportBlockMap(const char * pszFileName);
	void RecalculateBlock(void);
	void ImportWeatherEffectMap(const char * pszFileName);
	void ImportEnvCreateMap(const char * pszFileName);
	void ClearUnusedBuild(void);

	void AddRefenceBuildCount(WORD wBuildID);
	void SubRefenceBuildCount(WORD wBuildID);

	void AddPalaceBuildCount() { m_dwTotalPalacedBuildCount++;}
	void SubPalaceBuildCount() { if (m_dwTotalPalacedBuildCount >0) m_dwTotalPalacedBuildCount--;}

	DWORD GetPalaceBuildCount() { return m_dwTotalPalacedBuildCount;}
	DWORD GetRefrenceBuildCount() { return (DWORD)m_mapReferenceBuildArray.size();}

	void ImportMaterialMap(const char * pszFileName);

	void SetFogEnable(BOOL benable){m_bEnableFog = benable;}
	void SetFogColor(DWORD dwColor){m_dwFogColor=dwColor;}
	void SetFogStart(DWORD dwStart){m_dwFogStart=dwStart;}
	void SetFogEnd(DWORD dwEnd){m_dwFogEnd=dwEnd;}
	DWORD GetFogEnable(){return m_bEnableFog;}
	DWORD GetFogColor(void){return m_dwFogColor;}
	DWORD GetFogStart(void){return m_dwFogStart;}
	DWORD GetFogEnd(void){return m_dwFogEnd;}
};