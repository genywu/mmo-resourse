
#pragma once

#include "region.h"

#include "../../EngineExtend/Gamebuildlist.h"
#include "../../EngineExtend/Gameskybox.h"
#include "RegionCamera.h"
#include "Other/AlphaFadeObjectList.h"
#include "../../EngineExtend/CBitmapX.h"

class CRegionCamera;
class CMoveShape;
class CMonster;
class CMiniMap;
class CSkill;
class CEffect;
class GameMap;
class GameBaseEnvEffect;
class GameBaseEnvCreature;
class CFindPath;
class CPlayer;
class CGoods;

typedef enum eRegionWarType
{
	RWT_Normal=0,	//普通场景
	RWT_NormalWar,	//普通战场
	RWT_VillageWar,	//村战
	RWT_CityWar		//城战
}eRWT;

typedef enum eRegionPKType
{
	RPKT_None = 0,	//该功能未开启
	RPKT_No_PK,		//开启PK功能,但不可PK
	RPKT_PK,		//开启PK功能,但可PK,为特殊PK区域
}eRPKT;

//场景空间类型
typedef enum eRegiogSpaceType
{
	RST_Land=0,	//陆地
	RST_Warter,	//水下
	RST_Sky,	//天空
}eRST;

enum REGION_LIGHT
{
	RL_OUTDOOR,			//室外
	RL_CAVE				//洞穴
};

enum CONTROL_CAMEAR_OPERATION{ CCO_NONE,CCO_MOVE,CCO_ROTATE,CCO_SCALE,CCO_FLATMOVE};

struct PLAYERDIST 
{
	CPlayer* p;
	float dist;
	bool operator < (PLAYERDIST p){return dist < p.dist;}
};

// 客户端专用的场景类
class CClientRegion :
	public CRegion
{
public:
	// 场景建筑物声音播放测试
	typedef struct tagRegionSound
	{	
		float xpos;
		float zpos;
		bool bPlaying;
		string filename;
		int channel;
		GameBuildList::Build::tagSoundSource Sound;

	}REGION_SOUND,*REGION_SOUND_PTR;

	// 环境音效
	typedef struct tagEnvSound
	{
		bool bLoop;	
		bool bPlaying;
		int channel;
		string strFileName;
		long lMinTime;
		long lMaxTime;
		DWORD dwStartTime;
		long lIntervalTime;
		int	nLoops;

	}ENV_SOUND,*ENV_SOUND_PTR;

	// ***************************据点信息（09.9.29刘科）************************
	struct tagSPointInfo 
	{
		BYTE		bID;				// 该据点ID
		BYTE		bState;				// 该据点的状态
		long		lPosX;				// 该据点在本场景中的位置
		long		lPosY;
		string		strPointName;		// 该据点的名字（同服务器ID）
	};
	// **************************************************************************

public:
	CClientRegion(void);
	virtual ~CClientRegion(void);

//////////////////////////////////////////////////////////////////////////
//	图形部分
//////////////////////////////////////////////////////////////////////////
public:

	void EnableFog(DWORD Color, float fDensity=0.05);	// 开雾
	void DisableFog();									// 关雾

	void UpdateCamera(void);							// 自由视角
	void UpdateRegionCamera(float x, float y);			// 更新场景相机
	//摄像机操作方式
	void TransformCamer(long lOperaMode,long lParam1,long lParam2,long lParam3);

	float CalcDirection(float sx, float sy, float sh, int dx, int dy);	// 计算指定点到鼠标的方向
	DWORD CalcMoveDirection(POINT &ptMouse);
	void  CalculateFinalTransformMatrix(D3DXMATRIX &matFinal);	// 计算主角头顶文字的位置
	bool CalcMousePoint(long mx, long my, float &x, float &y);	// 获取鼠标指到的坐标
	CShape* GetHoverShape(void);								// 获取鼠标指向的Shape
	render::Camera * GetCamera() { return m_pCamera;}			// 获得摄象机

	CRegionCamera * GetRegionCamera()							{ return m_pRegionCamera; }
	void			SetRegionCamera(CRegionCamera * pRegionCamera)		{ memcpy(m_pRegionCamera, pRegionCamera, sizeof(CRegionCamera)); }

	DWORD m_dwHoverGrid;

	render::Frustum* GetFrustum()	{return m_pFrustum;}
	float GetCellHeight(float x, float y);					// 计算格子的高度
	void  Coordinate3DTo2D(float f3DX,float f3DY,float f3DH,long& l2DX,long &l2DY);//3维坐标转到2维坐标

	/*
	*	功能：添加一个据点信息
	*	细节描述： 当玩家进入场景的时候，接到服务器发送的该场景的据点信息后调用
	*	参数： stSPInfo - 据点信息
	*	返回值： 无
	*	作者: 刘科
	*	创建日期: 09.09.29
	*	修改日志: -
	*/
	void	AddSPointInfo(tagSPointInfo stSPInfo);

	/*
	*	功能：改变一个据点的状态
	*	细节描述： 当某个据点的状态发生变化之后，服务器通知客户端改变
	*	参数： szPointName - 据点名字
	*			bState		- 据点状态
	*	返回值： 无
	*	作者: 刘科
	*	创建日期: 09.09.29
	*	修改日志: -
	*/
	void	ChangeSPointState(const char* szPointName, BYTE bState);
	
	
	/*
	*	功能：得到某个据点的描述
	*	细节描述： 根据传入的据点名字，得到当前玩家看到的该据点描述
	*	参数： szPointName - 据点名字
	*	返回值： 据点描述
	*	作者: 刘科
	*	创建日期: 09.09.29
	*	修改日志: -
	*/
	void	GetDescTxtBySPState(string &strDesc, const char* szPointName);

	/*
	*	功能：获得本场景下的所有据点信息
	*	细节描述： 当需要显示场景下的据点信息时候调用
	*	参数： 无
	*	返回值： m_vecSPoint - 据点列表
	*	作者: 刘科
	*	创建日期: 09.09.29
	*	修改日志: -
	*/
	map<string,tagSPointInfo> &	GetSPointInfo()		{return m_mapSPoint;}
	
	/*
	*	功能：获得本场景下的所有据点信息
	*	细节描述： 当需要显示场景下的据点信息时候调用
	*	参数： 无
	*	返回值： true：开启		false：关闭
	*	作者: 刘科
	*	创建日期: 09.09.29
	*	修改日志: -
	*/
	bool	IsOpenSPoint()		{return m_bOpenSPoint;}
	
	/*
	*	功能：获得本场景下的所有据点信息
	*	细节描述： 当需要显示场景下的据点信息时候调用
	*	参数： bFlag = true：开启		bFlag = false：关闭
	*	返回值： -
	*	作者: 刘科
	*	创建日期: 09.09.29
	*	修改日志: -
	*/
	void	SetIsOpenSPoint(bool bFlag)		{m_bOpenSPoint = bFlag;}

private:
	CBitmapX * m_pBossBackImage;							// Boss的大血条背景图片
	CBitmapX * m_pBossImage;								// Boss的大血条图片

	//图形部分
	CRegionCamera * m_pRegionCamera;						//场景相机

	//图形接口
	CONTROL_CAMEAR_OPERATION m_eCamOpType;					//摄像机操作类型
	render::Camera  * m_pCamera;							//摄像机
	render::Frustum * m_pFrustum;							

	GameMap * m_pGameMap;
	GameSkybox* m_pGameSkyBox;
	DWORD m_dwSkyBoxID;
	GameBaseEnvEffect   * m_pGameEnvEffect;
	GameBaseEnvCreature * m_apGameEnvCreature[4];			//环境生物指针数组

	list<REGION_SOUND> m_RegionSoundList;
	list<ENV_SOUND> m_EnvSoundList;

	D3DLIGHT9 m_RegionLight;

	REGION_LIGHT	m_eRegionLightType;
	//---------------------------------------------------------------------------------
	// 体积阴影
	static int CONST MAX_SHAPE = 128;					//假设屏幕内最多有50个模型
	CShape *m_pShapeList[MAX_SHAPE];					//需要处理的模型列表
	DWORD m_dwNumOfShape;
	//---------------------------------------------------------------------------------
	long m_lTaxRate;									//税率

	eRegiogSpaceType    m_eSpaceType;
	eRegionWarType		m_WarRegionType;				//战斗场景类型
	eRegionPKType		m_ePKRegionType;				//战斗场景类型

	bool m_bChangeEquip;								//是否可以改变装备(目前主要针对外套的显示,披风不显示)

	CAlphaFadeObjectList m_AlphaFadeObjectList;			//透明渐变对象列表

	map<string,tagSPointInfo>	m_mapSPoint;			// 据点列表（09.9.29刘科）
	bool	m_bOpenSPoint;								// 本场景是否开启据点

//////////////////////////////////////////////////////////////////////////
//	接口
//////////////////////////////////////////////////////////////////////////
public:
	virtual	void Run();
	virtual	void AI();
	virtual	bool Display();
	virtual	void ScrollCenter(float x, float y);
	virtual BOOL Load();								//读取客户端资源
	virtual BOOL Save();								//存储
	void Init(void);
	void SyncUpdate();
	void Exit(void);

	// 移出OBJECT
	void RemoveObject(CBaseObject* pObject);
	void ShowShopName(CPlayer * pPlayer, float fPos);

	//#新增加的物品管理的函数
	bool AddGoods(CGoods *pGoods,DWORD dwPos,LONG lExtendID);
	CGoods *RemoveGoods(CGUID& guID,DWORD dwPos,LONG lExtendID,DWORD dwAmount);
	bool ClearGoods();

	//得到一个中心点附近没有MoveShape的格子
	void GetNoShapeCell(long lCenterX,long lCenterY,long lRadius,long lFirstDir,long& lX,long& lY);
	//得到一个空间里某方向上的一个格子，该格子上的MoveShape对象最少
	void GetLeastShapesCellFromSpace(int nObjCenterX,int nObjCenterY,
		int nObjFigureX,int nObjFigureY,int nMaxDistance,int nDiretion,
		long& lX,long& lY);

	//得到一个圆形空间里某方向上的一个格子，该格子上的MoveShape对象最少
	void GetLeastShapesCellFromCircularSpace(int nObjCenterX,int nObjCenterY,
		int nObjFigureX,int nObjFigureY,float nMaxDistance,long lDir,
		long& lX,long& lY);
	
	// 获取一个点上是否有Shape
	CShape* GetShape(long x, long y);

	// 获取一个点上是否有Shape
	CGoods* GetShapeGoods(long x, long y);

	// 获取一个点的指定范围内是否有Shape
	CShape* GetShapeFromSpace(long x, long y, long lSize);


	// 获得一个点上的所有对象    ADDBY：刘科（09.2.9）
	CShape* GetAllShapeByType(list<CShape *> & lsShape, long x, long y, long type = -1);

	/****************************************************************************************
	* ADDBY：刘科（09.2.5）
	* 获取一定范围内，一个离指定点最近的指定类型的shape
	* 如果有多个shape离目标点的距离都最小，按顺时针从目标点指定的方向上的shape开始查找
	****************************************************************************************/
	CShape* GetLeastShape(list<CShape *> & lsSelectedShape,long x,long y,long lDir,long lOffLen,long lType = -1);

	// 更新玩家周围的所有NPC的任务状态
	void UpdataAllNpcQuestState();

	CAlphaFadeObjectList & GetAlphaFadeObjectList()		{ return m_AlphaFadeObjectList; }			//得到Alpha渐变对象表

	//==============07.10.16liuke添加NPC头上显示任务提示信息方法==================
	void DisplayQuestInfoByNpc(CMonster *pMonster, long lEffectID);
	// 显示对象的称号以及图片
	void DisplayMonsterTitle(CMonster* pMonster, bool bDisplayTitle=false, float fDrawHeight = 2.5f);
	//============================================================================

	// 玩家隐藏信息后的显示内容
	float DisplayHideInfo(CPlayer* pPlayer);

	float DisplayShapeName(CShape* pShape, bool bDisplayName=false, bool bDisplayBloodBar = false);		//显示Shape名字
	void DisplayShapeHoverArrow(CShape* pShape);
	void DisplayShapeCountryPirate(CPlayer * pPlayer, bool bDisplayCountry, bool bPirate);				//显示Shape国家和海盗标志

	void DisplayTextureProjective();


	GameMap * GetGameMap() { return m_pGameMap;}
	GameSkybox * GetGameSkyBox() { return m_pGameSkyBox;}
	void	SwitchDayOrNight();
	inline	REGION_LIGHT	GetClientReionLightType()	{return	m_eRegionLightType;}
	void	SetPointLight();
	void	SetDirectLight();
	list<REGION_SOUND>* GetRegionSoundList(){return &m_RegionSoundList;}
	list<ENV_SOUND>* GetEnvSoundList(){return &m_EnvSoundList;}

	D3DLIGHT9* GetRegionLight(){return &m_RegionLight;}
	void SetRegionLight(D3DLIGHT9 light){m_RegionLight = light;}

	// 播放特效
	CEffect *PlayEffect(long lGraphicsID, float x, float y, float hight=0.f, float fDir=-1.f,bool bAsyn=true);
	void PlayEffectFoot(CPlayer * pPlayer);			//播放足底特效

	//==================07.12.20liuke====================
	// 查找指定索引的特效对象
	CEffect	*FindEffectByIndex();
	//===================================================

	void	SetTaxRate(long lTaxRate)		{m_lTaxRate = lTaxRate;}
	long	GetTaxRate()					{return m_lTaxRate;	}
	
	void	SetWarType(eRWT eType)			{m_WarRegionType = eType;}
	eRWT GetWarType()						{return m_WarRegionType;}

	// add by Lpf at 2009.10.13
	void  SetPKRegionType(eRPKT eType)		{ m_ePKRegionType = eType; }
	eRPKT GetPKRegionType()					{ return m_ePKRegionType; }

	bool IsChangeEquip()					{ return m_bChangeEquip; }
	void SetChangeEquip(bool b)				{ m_bChangeEquip = b; }

	eRST GetSpaceType(){return m_eSpaceType;}
	void SetSpaceType(eRST eType){m_eSpaceType=eType;}

	//当主角色进入场景
	void OnMainPlayerEnter();
/////////////////////////////////////////////////////////////////////
// 消息响应
/////////////////////////////////////////////////////////////////////
public:
	virtual void OnMessage(CMessage*);

//////////////////////////////////////////////////////////////////////////
//	小地图
//////////////////////////////////////////////////////////////////////////
public:
	void DisplayMiniMap(int x, int y);
	CMiniMap* GetMiniMap()	{return m_pMiniMap;}

protected:
	CMiniMap *m_pMiniMap;

public:
	void _Debug_RenderMap(void);

//////////////////////////////////////////////////////////////////////////
//	技能
//////////////////////////////////////////////////////////////////////////
public:

//显示组队招募
	void ShowPlayerRecruiting(CPlayer * pPlayer, float fPos);

//显示玩家头顶信息
	void ShowPlayerInfo(CPlayer *pPlayer);

	const char * CClientRegion::ResolveNameStr(const char * pszOriNameStr);			//解析名字字符串

//////////////////////////////////////////////////////////////////////////
//	NPC列表
//////////////////////////////////////////////////////////////////////////
public:
	void SetNpcListLoaded(bool b)		{m_bNpcListLoaded=b;}
	bool IsNpcListLoaded()				{return m_bNpcListLoaded;}

private:
	bool m_bNpcListLoaded;		// NPC列表是否已经获取

//////////////////////////////////////////////////////////////////////////
//	寻路模块
//////////////////////////////////////////////////////////////////////////
public:
	CFindPath* GetFindPath();

private:
	CFindPath *	m_pFindPath;	// 寻路模块
};
