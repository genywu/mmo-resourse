//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年3月29日 10:21:59
// 更新时间 : 2004年5月14日 10:21:59
// 摘要     : 
//            1. 显示小地图，可以缩放。
//            2. 根据服务器传来的数据，在小地图上显示标记。
//            3. 以玩家为中心，显示的小地图随玩家移动。
//---------------------------------------------------------------------------------
#pragma once

// 小地图图标类型：（07.12.14刘科修改）
// 在小地图上显示的提示图标类型，如果类型大小超过MT_OTHER，
// 表示需要在相应的文件夹下加载对应ID的图标
// （08.10.18刘科修改，重新定义对象类型）

enum MODEL_TYPE
{
	MT_PLAYER = 0,			// 玩家自己
	MT_MONSTER,				// 怪物
	MT_LEAGNPC,				// 联盟NPC
	MT_UNLEAGNPC,			// 非联盟NPC
	MT_PARTNER,				// 队友
	MT_LEAGPLAYER,			// 联盟玩家
	MT_UNLEAGPLAYER,		// 非联盟玩家
	MT_RED,					// 红名玩家
	MT_PIRATES,				// 海盗玩家
	MT_TASK,				// 标记图标
	MT_PET,					// 宠物图标

	MT_OTHER,				// 其它类型（一般指特殊的图标）
	MT_COLLECTION = 10000,	// 采集物
	MT_QUEST = 100000,		// 任务
};

#define MAX_NUM 128
#define RADIUS_NUM 64

struct _tagShapeInfo
{
	float x, y;				// 坐标
	DWORD type;				// 类型
	string strName;			// 名字
};


class CBitmapX;

class CMiniMap
{
private:
	// 各种图标
	CBitmapX	*m_pMiniMap, 
				*m_PlayerIcon,				// 玩家图标
				*m_MonsterIcon,				// 怪物图标
				*m_LeagueNpcIcon,			// 联盟NPC图标
				*m_UnLeagueNpcIcon,			// 非联盟NPC图标
				*m_PartnerIcon,				// 队友图标
				*m_LeaguePlayerIcon ,		// 联盟其它玩家图标
				*m_UnLeaguePlayerIcon,		// 非联盟其它玩家图标
				*m_RedIcon,					// 红名图标
				*m_PiratesIcon,				// 海盗图标
				*m_TaskIcon,				// 标记图标
				*m_PetIcon,					// 宠物图标
				*m_pAlphaMap;				//迷你地图的Alpha蒙板纹理图

	// 小地图图片的高宽
	DWORD m_dwMiniMapWidth, m_dwMiniMapHeight;

	// 地图的高宽
	DWORD m_dwMapWidth, m_dwMapHeight;

	// 颜色
	DWORD m_dwColor;

	// 缩放比例
	FLOAT m_fScaleFactor;

	int m_iMainPlayerDir;
	int m_iMainPlayerX,m_iMainPlayerY;

	_tagShapeInfo m_pShapeInfo[MAX_NUM];		// 需要在小地图上标记的shape（不包括玩家自己）
	vector<_tagShapeInfo>	m_vecPartner;		// 队友
	vector<_tagShapeInfo>	m_vecRedName;		// 红名
	vector<_tagShapeInfo>	m_vecPirates;		// 海盗
	vector<_tagShapeInfo>	m_vecOther;			// 其它类型（任务问号->任务叹号->任务其他->采集物->ＮＰＣ称谓）
	vector<_tagShapeInfo>	m_vecNPC;			// NPC（联盟非联盟）
	vector<_tagShapeInfo>	m_vecUnLeagPlayer;	// 非联盟普通玩家
	vector<_tagShapeInfo>	m_vecLeagPlayer;	// 联盟普通玩家

	vector<_tagShapeInfo>	m_vecPartnerForUI;	// 队友的信息列表，保存的坐标位置是小地图上，相对于界面的，供界面提示信息用
	DWORD m_dwNumOfShape;

	// 小地图界面的高宽
	DWORD m_dwFrameWidth, m_dwFrameHeight;

	int m_iMapMode;		// 小地图的图片模式（默认为1：512*512，2：1024*1024）

	// 大地图上的目标点标示
	int m_nDestX;
	int m_nDestY;

	LPDIRECT3DTEXTURE9 m_pRenderTexture128;
	LPDIRECT3DSURFACE9 m_pDepthStencilSurface128;

public:
	CMiniMap(void);
	~CMiniMap(void);

	// 创建和销毁
	void Create(DWORD dwFrameWidth, DWORD dwFrameHeight, CONST TCHAR *filename);
	void Destroy();
	void DestroyDirect3DResource();					//销毁D3D相关资源
	void ResetDirect3DResource();					//重设D3D相关资源

	// 载入小地图及相关数据
	BOOL LoadMiniMap(CONST CHAR *filename, DWORD width, DWORD height);

	// 显示
	void DrawMiniMap(DWORD x, DWORD y);
	//void DrawMiniMapOnWorldMap(DWORD x, DWORD y,CBitmapX *pBitmap=NULL,int index=0);
	void DrawAlphaMap();											//绘制Alpha蒙板
	void DrawFinalMiniMap(DWORD dwX, DWORD dwY);					//绘制最终的迷你地图纹理
	bool RenderMiniMapRotation(DWORD x,DWORD y);					//绘制旋转效果的迷你地图
	bool RenderMiniMapNoRotation(DWORD x,DWORD y);					//绘制没有旋转效果的迷你地图
	//void RenderMiniMapRotationOnWorldMap(DWORD x,DWORD y,CBitmapX *pBitmap=NULL,int index=0);
	//void RenderMiniMapNoRotationOnWorldMap(DWORD x,DWORD y,CBitmapX *pBitmap=NULL,int index=0);	

	//-----------------------08.3.12liuke----------------------
	// 绘制玩家所在场景的相关内容（参数：地图图片，效果图片，效果播放次数）
	/***********************************************************************/
	/*  zhaohang 2010-11-16 fix 位置参数修改为float型*/
	/***********************************************************************/
	void RenderOnWorldMap(CBitmapX *pMapBitmap,CBitmapX *pBitmap,int index, bool bShowPlayer=true,float x=0, float y=0);
	//---------------------------------------------------------

	// 操作shapelist
	void AddToShapeList(float x, float y, DWORD dwType,const char*szName = NULL);		// 增加一个Shape信息到显示数组中去
	void AddToPartnerList(float x, float y, const char*szName);							// 增加一个队友信息到显示列表中去
	_tagShapeInfo* GetPartnerFromPos(DWORD x, DWORD y);									// 根据传入的队友坐标获得队友信息（相对于小地图上的界面坐标）
	void CleanShapeList();
	DWORD GetShapeListCount();

	// 缩放小地图
	void ScaleMiniMap(FLOAT fScaleFactor);
	FLOAT GetScaleFactor() { return m_fScaleFactor; }

	// 操作小地图的颜色
	void SetMiniMapColor(DWORD color) { m_dwColor = color; }
	DWORD GetMiniMapColor() { return m_dwColor; }

	// (x, y) 是否在rc 中
	BOOL InTheRect(RECT *rc, int x, int y);
	// (x,y) 是否在圆中。centerX，centerY为中心点。angel返回偏移方向，半径为64
	BOOL InTheCircinal(float centerX, float centerY, float nr, float &x, float &y, int &iDir);

	// 获得当前地图的模式
	int GetMapMode()	{return m_iMapMode;}

	void SetMainPlayer(int x,int y,int dir);
	DWORD GetMapWidth(){return m_dwMapWidth;}
	DWORD GetMapHeight(){return  m_dwMapHeight;}
	DWORD GetPicMapWidth(){return m_dwMiniMapWidth;}
	bool IsRotationAble();		// 是否支持旋转显示

	// 设置在大地图上需要现实标示的目标点
	void SetDestPos(int x = -1,int y = -1);
};
