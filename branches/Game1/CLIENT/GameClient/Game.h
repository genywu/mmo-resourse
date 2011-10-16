#pragma once

#include "../AppClient/Player.h"
#include "../AppClient/shape.h"
#include "../AppClient/organizingsystem/Faction.h"
#include "../AppClient/organizingsystem/Union.h"
#include "..\..\GameEngine/shadowvolume.h"
#include "..\..\GameEngine/gameflatshadow.h"
#include "..\..\GameEngine/gamemodel.h"
#include "..\..\..\Public\GoodsExSetup.h"
//#ifndef  MOVE_IN_TILE
//#define MOVE_IN_TILE
//#endif
class CMouse;
class CKeyBoard;
class CClient;
class CPlayer;
class CClientRegion;
class CModelList;
class AudioEngine;
class CAudioList;
class CPicList;
class CFaceActionList;
class WeatherManager;
class CFmodMusic;
class CClientResource;
class CMsgBoxEvent;
class CGameControl;
class CEffect;
class CDupliRegionSetup;
//===================================================
class CGuiEx;
class CGuangMGUIApp;
class COccupSkillList;
class CDataBlockAllocator;
class CSkillXml;
class CMemClient;
extern HWND g_hRetLb;

// 游戏状态
enum
{
	GS_LOGIN,		// 游戲登陸狀態
	GS_PWD_CARD,	// 密保卡输入状态
	GS_SELECTWS,     //选择WS
	GS_SELECT,		// 游戲選擇角色狀態
	GS_CREATE,		// 游戲建立角色狀態
	GS_WAITING,		// 等待进入状态
	GS_BLACK,		// 黑屏状态
	GS_RUN,			// 游戲運行
	GS_STAFF		// 制作人员
};

// 鼠标当前状态
enum MOUSE_CURSOR_TYPE
{
	MC_NORMAL,				// 正常游戏
	MC_PLAYER_FOCUS,		// 指向玩家
	MC_NPC_FOCUS,			// 指向NPC(默认)
	MC_MONSTER_FOCUS,		// 指向怪物
	MC_GOODS_FOCUS,			// 指向物品
	
	MC_COLLECTION_CAO,		// 采集物（草）
	MC_COLLECTION_KUANG,	// 采集物（矿）
	MC_COLLECTION_PI,		// 采集物（皮）
	MC_COLLECTION_RENWU,	// 采集物（任务）
	MC_STATE_TRADE,			// 修理物品状态
	MC_TRYON_GOODS,			// 试装状态
	MC_NPC_QUEST,			// 任务
	MC_MONEY,				// 钱包
	MC_GEAR,				// 齿轮
	MC_GATE,				// 门

	MC_COUNT				// 数量
};

// 玩家容器编号 - 对应服务器的 CSCContainerObjectMove.h
enum PLAYER_EXTEND_ID
{
	PEI_PACKET					= 1,	// 背包
	PEI_EQUIPMENT,						// 装备	
	PEI_WALLET,							// 金币钱包
	PEI_SILVERWALLET,					// 银币钱包
	PEI_GEM,							// 宝石
	PEI_YUANBAO,						// 元宝
	PEI_JIFEN,							// 积分
	PEI_MEDAL,                        // 勋章容器 
	PEI_SUBPACK=10000,
	PEI_PACK1=10001,
	PEI_PACK2=10002,
	PEI_PACK3=10003,
	PEI_PACK4=10004,
	PEI_PACK5=10005,
	PEI_BUSSINESPACKFIELD = 20000,		// 放跑墒背包的栏位ID
	PEI_BUSSINESPACK = 20001			// 跑墒背包容器ID
};

// 判断容器是否属于背包类
#define IS_PACKETCONTAINER(type) ((type) == PEI_PACKET || ((type) >= PEI_PACK1 && (type) <= PEI_PACK5))

struct SHADOWVERTEX
{
    D3DXVECTOR4 p;
    D3DCOLOR    color;

    static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
};

struct stTopStrInfo
{
	D3DCOLOR	d3dColor;	//显示颜色
	string		strContend; //显示内容
};
//屏幕上端的提示
struct stTopInfo
{
	long				lID;
	long				lTimerFlag;			//倒计时方式(1:次数,2:时间)
	long				lParam;				//如果是时间则表示倒计时的描
	DWORD				dwStartTime;		//开始计时时间
	list<stTopStrInfo>	strInfo;			//提示信息部分内容
};

//当前提示信息内容
struct stCurTopInfo
{
	long	lX;					//显示坐标
	long	lY;
	long	lTotalLength;		//总长度
	
	list<stTopStrInfo>   listCurInfo;	//显示列表
};

enum eNetFlag
{
	NF_LS_Client=0x001,
	NF_GS_Client=0x010,
};
extern int ORGIN_GAME_WIDTH;
extern int ORGIN_GAME_HEIGHT;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern POINT g_pointMousePos;

class CBitmapX;

// 保存游戏全局数据类
class CGame
{
public:
	CGame(void);
	virtual ~CGame(void);

	static bool	s_bExit;						// 是否退出
	static bool IsExit()						{return s_bExit;}	

	static bool s_bReGame;						// 是否重新进入选人画面
	static bool IsReGame()						{return s_bReGame;}
	static void SetReGame(bool b)				{s_bReGame=b;}

	// 重新设置键盘设备
	bool ReSetKeyboard();

	// 王峰：添加 CGoodsDescPageEx 链表
	static vector<class CGoodsDescPageEx*>	m_vGoodsDescPageEx;

////////////////////////////////////////////////////
// 全局变量
////////////////////////////////////////////////////
private:	

	D3DCOLOR m_colorBack;						// 背景颜色
	UINT    m_uGameServerPort;					// GameServer服务器端口
	string	m_strGameServerIP;					// GameServerIP
	UINT    m_uLoginServerPort;					// LoginServer服务器端口
	string	m_strLoginServerIP;					// longinServerIP

	CMouse		*m_pMouse;						// 鼠标
	CKeyBoard	*m_pKeyboard;					// 键盘

	CAudioList	*m_pAudioList;					// 音效列表，管理所有shape的声音

	CDataBlockAllocator	*m_pDBAllocator;		// 数据块
    CClient* m_pNetClient;						// 客户端网络
	//===========================================================================================
	CGuiEx *m_pGuiEx;
	COccupSkillList *m_pOccupSkillList;			// 职业技能列表
	//===========================================================================================

	CPlayer* m_pMainPlayer;						// 主角
	CClientRegion* m_pRegion;					// 地图

	long	m_lGameState;						// 游戲當前狀態
	long	m_lMouseState;						// 鼠标状态
	string	m_strCDKEY;							// 玩家CDKEY
	vector<BYTE>	m_strPASSWORD;				// 玩家密码
	string	m_strWSName;						// 登陆的WorldServer名字

	CPicList	*m_pPicList;					// 图片资源管理

	CBitmapX	*m_pbitmapShapeTalk;			// 说话时头顶文字的背景图片
	CBitmapX	*m_pbitmapTeamTalk;				// 队友说话时的背景图片

	CBitmapX	*m_pbitmapIntanateTimePot;		// 吟唱条的时间点图片
	CBitmapX	*m_pbitmapIntanateBarBack;		// 吟唱条背景
	CBitmapX	*m_pbitmapIntanateBar;			// 吟唱条
	CBitmapX	*m_pbitmapHpBarBack;			// 血条的背景框贴图
	CBitmapX	*m_pbitmapPlayerHpBar;			// 血条贴图(绿)
	CBitmapX	*m_pbitmapMonsterHpBar;			// 怪物血条(红）
	CBitmapX	*m_pbitmapHpBar2;				// 血条贴图(黄)
	CBitmapX	*m_pbitmapHpBarPet;				// 宠物血条贴图
	CClientResource* m_pClientResource;			// 客户端资源
	CBitmapX	*m_pbitmapSuck;					// 吸收图标
	CBitmapX	*m_pbitmapBlastSuck;			// 吸收爆击图标
	CBitmapX	*m_pbitmapBlastAttack;			// 爆击图标
	CBitmapX	*m_pbitmapBlock;				// 格档图标
	CBitmapX	*m_pbitmapBlastBlock;			// 格档爆击图标
	CBitmapX	*m_pbitmapRebound;				// 反弹图标
	CBitmapX	*m_pbitmapBlastRebound;			// 反弹爆击图标
	CBitmapX	*m_pbitmapFullMiss;				// 闪避图标
	CBitmapX	*m_pbitmapBlastFullMiss;		// 闪避爆击图标
	CBitmapX	*m_pbitmapParry;				// 招架图标
	CBitmapX	*m_pbitmapBlastParry;			// 招架爆击图标
	CBitmapX	*m_pbitmapResist;				// 抵抗图标
	CBitmapX	*m_pbitmapBlastResist;			// 招架爆击图标
	CBitmapX	*m_pbitmapLost;					// 丢失图标
	CBitmapX	*m_pbitmapExpTip;				// 经验值提示图片
	CBitmapX	*m_pbitmapConTip;				// 功勋提示图片

	bool		m_bIsGM;						// GM标志
	CMsgBoxEvent*		m_pMsgBoxEvent;			// MsgBox 的事件
	CGameControl*		m_pGameControl;			// 游戏控制类

	CBitmapX*	m_pBitMapFactionWarEnemy;		// 帮战的敌对方图标
	CBitmapX*	m_pBitMapFactionWarUion;		// 帮战的同盟方图标

	CBitmapX*	m_pBitMapCityWarEnmey;			// 城战敌对方的图标
	CBitmapX*	m_pBitMapCityWarUnion;			// 城战同盟方的图标

	CBitmapX*	m_pBitMapOwnedCity;				// 拥有主城的显示图标
	CBitmapX*	m_pBitMapOwnedVillage;			// 拥有据点的显示图标

	CBitmapX*	m_pHandsBackIcon;				// 手上物品（技能）图标的背景

	GoodsExSetup		m_GoodsExSetup;			// 王峰：Goods 扩展设置算法
	CFactionManager		m_FactionManager;		// 王峰：行会管理对象
	CUnionManager		m_UnionManager;			// 王峰：联盟管理对象


	long		m_lLoadingTipsIndex;			// loading的提示文字索引
	long		m_lLoadingDelayTime;			// 进入场景的延迟开始时间
	CBitmapX*	m_pLoadingPic;					// 登陆的loading图片
	void		DisPlayLoading(long lRegionID);	// 根据场景ID来绘制Loading画面

public:
	DWORD	m_dwFaceTexture;	//表情图标

private:
	int			m_iCurrentRenderTriangleCount;  //当前游戏显示的多边形数量（不包括界面上的三维模型）

public:
	void        SetCurrentRenderTriangleCount(int iCount)  { m_iCurrentRenderTriangleCount = iCount;}

	CBitmapX*	m_pSelectShape;					// 选中了一个对象时显示在名字左右的图片
	CBitmapX*	m_pPKIcon;						// PK图标
	CEffect*	m_pMouseShapeFoot;				// 鼠标指向的对象脚底光标
	CEffect*	m_pMouseTeammerFoot;			// 鼠标指向的队友脚底光标
	CEffect*	m_pLockAttackShapeFoot;			// 锁定目标对象脚底光标

private:
	BOOL		m_bPlayerInWeatherEffectArea;	// 角色在天气效果区域吗？
public:
	void		SetPlayerInWeatherEffectArea(BOOL bValue) { m_bPlayerInWeatherEffectArea = bValue;}
private:
	//---------------------------------------------------------------------------------
	// 体积阴影，只有主角会产生阴影。

	BOOL m_b2SidedStencils;
	static int CONST MAX_VOL = 32;							// 假设每个模型最多有10个组件会产生阴影
	SHADOWVERTEX m_pShadowVertex[4];
	BOOL m_bSupportStencils;

	DWORD m_dwNumOfVol, m_dwNumOfShape;

	// 平面阴影
	GameFlatShadow m_FlatShadow;
	//---------------------------------------------------------------------------------

public:

	CGameControl*			GetGameControl()	{return m_pGameControl;}
	CMsgBoxEvent*			GetMsgBoxEvent()	{return m_pMsgBoxEvent;	}

	CClientResource*		GetCleintResource()	{return m_pClientResource;}
	CPicList* GetPicList()					    {return m_pPicList;}
	D3DCOLOR	GetBackColor()					{return m_colorBack;}
	void SetBackColor(BYTE a, BYTE r, BYTE g, BYTE B)	{m_colorBack=D3DCOLOR_ARGB(a,r,g,B);}
	void SetBackColor(D3DCOLOR color) { m_colorBack = color;}     
	CMouse*		GetMouse()						{return m_pMouse;}
	CKeyBoard*  GetKeyboard()					{return m_pKeyboard;}
	CAudioList*	GetAudioList()					{return m_pAudioList;}
	UINT   GetGameServerPort()					{return m_uGameServerPort;}
	void   SetGameServerPort( UINT uPort)		{m_uGameServerPort = uPort;}
	const char*  GetGameServerIP()				{return m_strGameServerIP.c_str();}
	void   SetGameServerIP( const char* chIP)	{m_strGameServerIP = chIP;}
	UINT   GetLoginServerPort()				    {return m_uLoginServerPort;}
	void   SetLoginServerPort( UINT uPort)		{m_uLoginServerPort = uPort;}
	const char*  GetloginServerIP()				{return m_strLoginServerIP.c_str();}
	void   SetLoginServerIP( const char* chIP)	{m_strLoginServerIP = chIP;}

	void SetNetClient(CClient* pNetClient)		{m_pNetClient = pNetClient;}
	CClient* GetNetClient()						{return m_pNetClient;	}
	//====================获得界面扩展管理对象========================
	CGuiEx * GetCGuiEx()						{return m_pGuiEx;}
	COccupSkillList * GetOccupSkillList()		{return m_pOccupSkillList;}
	//================================================================
	CPlayer* GetMainPlayer()					{return m_pMainPlayer;	}
	void SetMainPlayer(CPlayer* t)				{m_pMainPlayer = t;		}
	CClientRegion	*GetRegion(void)			{return m_pRegion;}

	long	GetWarContendTime()					{return m_lWarContendTime;}
	void	SetWarContendTime(long lTime)		{m_lWarContendTime = lTime;}

	long	GetDiedStateTime()					{return m_lDiedStateTime;}
	void	SetDiedStateTime(long lTime)		{m_lDiedStateTime = lTime;}

	long	GetGameState()						{return m_lGameState;	}
	void	SetGameState(long t);//				{m_lGameState = t;m_lQueuePos=0;}
	long	GetMouseState()						{return m_lMouseState;	}
	void	SetMouseState(long t)				{m_lMouseState = t;		}
	void SetCDKEY( const char* t)				{ m_strCDKEY = t;}
	const char* GetCDKEY()						{return m_strCDKEY.c_str();}
	void SetPASSWORD(vector<BYTE> bytePass)		{m_strPASSWORD = bytePass;}
	const vector<BYTE>& GetPASSWORD()			{return m_strPASSWORD;}
	void SetWSName( const char* t)				{m_strWSName = t;}
	const char* GetWSName()						{return m_strWSName.c_str();}

	CBitmapX* GetIntanateTimePotBitmap()		{return m_pbitmapIntanateTimePot;}
	CBitmapX* GetIntanateBarBackBitmap()		{return m_pbitmapIntanateBarBack;}
	CBitmapX* GetIntanateBarBitmap()			{return m_pbitmapIntanateBar;}
	CBitmapX* GetHpBarBackBitmap()				{return m_pbitmapHpBarBack;}
	CBitmapX* GetPlayerHpBarBitmap()			{return m_pbitmapPlayerHpBar;}
	CBitmapX* GetMonsterHpBarBitmap()			{return m_pbitmapMonsterHpBar;}
	CBitmapX* GetHpBarBitmap2()					{return m_pbitmapHpBar2;}
	CBitmapX* GetHpBarPetBitmap()				{return m_pbitmapHpBarPet;}
	CBitmapX* GetSuckBitmap()					{return m_pbitmapSuck;}
	CBitmapX* GetBlastSuckBitmap()				{return m_pbitmapBlastSuck;}
	CBitmapX* GetBlastAttackBitmap()			{return m_pbitmapBlastAttack;}
	CBitmapX* GetBlock()						{return m_pbitmapBlock;}
	CBitmapX* GetBlastBlock()					{return m_pbitmapBlastBlock;}
	CBitmapX* GetRebound()						{return m_pbitmapRebound;}
	CBitmapX* GetBlastRebound()					{return m_pbitmapBlastRebound;}
	CBitmapX* GetFullMissBitmap()				{return m_pbitmapFullMiss;}
	CBitmapX* GetBlastFullMiss()				{return m_pbitmapBlastFullMiss;}
	CBitmapX* GetParry()						{return m_pbitmapParry;}
	CBitmapX* GetBlastParry()					{return m_pbitmapBlastParry;}
	CBitmapX* GetResist()						{return m_pbitmapResist;}
	CBitmapX* GetBlastResist()					{return m_pbitmapBlastResist;}
	CBitmapX* GetLost()							{return m_pbitmapLost;}
	CBitmapX* GetExpTip()						{return m_pbitmapExpTip;}
	CBitmapX* GetConTip()						{return m_pbitmapConTip;}
	CBitmapX* GetFactionWarEnemyBitmap()		{return m_pBitMapFactionWarEnemy;}
	CBitmapX* GetFactionWarUnionBitmap()		{return m_pBitMapFactionWarUion;}
	CBitmapX* GetCityWarEnemyBitmap()			{return m_pBitMapCityWarEnmey;}
	CBitmapX* GetCityWarUnionBitmap()			{return m_pBitMapCityWarUnion;}
	CBitmapX* GetOwnedCityBitmap()				{return m_pBitMapOwnedCity;	}
	CBitmapX* GetOwnedVillageBitmap()			{return m_pBitMapOwnedVillage;}

	void SetGMMode( bool bIsGM = true )			{ m_bIsGM = bIsGM; }
	bool IsGM()									{ return m_bIsGM; }

	bool		IsFileExist(const char*	strFileName);
	void		FindFileList(const char* strFile, const char* strExt, list<string>& listFileName);

////////////////////////////////////////////////////
// 接口函数
	////////////////////////////////////////////////////

	// 游戏执行部分
	int Run(bool bControl=true);

	// 游戏消息处理
	void ProcessMessage();

	// 游戏显示部分
	int Display(void);

	// 游戏初始化
	int Init(void);

	// 游戏释放处理
	int Release(void);

	// 重新开始前的准备工作
	void ReNew(void);

	// 退出游戏
	void QuitGame(void);

	// 断开网络连接
	void CloseNetClient(void);

	//连接LoginServer服务器
	bool ConnectLoginServer(const char* const logIP,long logPort,
		const char* const cdKey,const vector<BYTE>& password,const char *wsName="");

	// 玩家收到 MSG_LOG_ANSWER_PLAYER 或 MSG_REGION_COMMOND_CHANGE 后请求进入场景
#ifdef MOVE_IN_TILE
	void QuestEnterRegion(long lRegionID,const CGUID& RegionID,long lResourceID, char* strRegionName, float lPlayerX, float lPlayerY, long lPlayerDir, long lUseGoods = 0);
#else
	void QuestEnterRegion(long lRegionID,const CGUID& RegionID,long lResourceID, char* strRegionName, long lPlayerX, long lPlayerY, long lPlayerDir, long lUseGoods = 0);
#endif

	//连接GameServer服务器
	CClient* ConnectGameServer(const char* const gsIP,long gsPort,const CGUID& PlayerID);

	void OnServerLost();

	// 获取输入信息
	void GetInputInfo();

	// new MainPlayer
	void NewMainPlayer();

	// 显示FPS
	int GetFps(void);

	void ShowFps(void);

	// 设置鼠标类型
	void SetMouseCursor(int mcType);

	// 获取鼠标类型
	MOUSE_CURSOR_TYPE GetMouseType();

	// 获取鼠标句柄
	HCURSOR GetMouseCursor();

	////---------------------------------------------------------------------------------

	// 抓取屏幕
	void PrintScreen(const char* strFileName);

	//
	//---------------------------------------------------------------------------------

	////////////////////////////向服务器发各种消息
	/*
	 *	LoginServer
	 */
	void C2L_CDKey_Send(const char *cdkey,const vector<BYTE>& password, const char *wsName="");//向loginserver CDKEY验证
	void C2L_PlayerBase_Send(const char *wsName,const char *cdkey);//向loginserver请求基本属性
	void C2L_DeleteRole_Send(const CGUID& ID);//向loginserver请求删除一个角色
	
	void C2L_RestoreRole_Send(const CGUID& PlayerID);

	void C2L_AddRole_Send(const char *strName,char nOccupation, char nSex, BYTE lHair, BYTE lFace,BYTE lCountry,BYTE lConstellation,BYTE bRandCountry);//向loginserver请求添加一个角色
	void C2L_QuestDetail_Send(const CGUID& ID);//向loginserver请求详细信息

////////////////////////////////////////////////////
// tick
////////////////////////////////////////////////////
public:
	DWORD GetAITick()				{return s_lAITick;}
	void SetAITick(long l)			{s_lAITick=l;}
	void SetLastPingTime(DWORD l)	{s_lLastPingTime=l;}

	void SetScriptTimer(long lTime)	{m_lScriptTimer=lTime;}
	long GetScriptTimer()			{return m_lScriptTimer;	}

	void SetQueuePos(long lNum);
	long GetQueuePos()              {return m_lQueuePos;}

private:
	DWORD s_lAITick;				// 游戏AI从启动到现在共运行的次数
	DWORD s_lLastPingTime;			// 上次收到SERVER的PING时间
	long  m_lScriptTimer;			// 脚本的计时器
	long  m_lQueuePos;				// 登陆缓冲时候在队列中的位置
	DWORD m_dwLastUpdateQueueTime;	//上一次的时间
	DWORD m_dwCountDownTime;		// 连接倒计时
	long  m_lWarContendTime;		// 据点争霸战、城战进行的时间百分比
	long  m_lDiedStateTime;			//死亡状态剩余时间
public:
	void CreateGraphices(void);
	void DestroyGraphices(void);

///////////////////////////////////////////////////
//验证信息
///////////////////////////////////////////////////
public:
	void SetSignCode(long l) {m_lSignCode = l;};   //设置进入游戏的验证码
	long GetSignCode(){return m_lSignCode;};         // 获得进入游戏的验证码
	void SetChangeServerCode(long l) {m_lChangeServerCode = l;}; //设置切换服务器的验证码
	long GetChangeServerCode(){return m_lChangeServerCode;};  //获得切换服务器的验证码
	void SetGameCDKEY(const char* str) {m_strGameCDKEY = str;};     //设置m_strGameCDKEY
	const char* GetGameCDKEY() {return m_strGameCDKEY.c_str();};    //获得m_strGameCDKEY
	void SetReGameCode(DWORD l,int i){m_lReGameCode[i] = l;};    //设置验证小退的参数
private:
	long m_lSignCode;              //进入游戏的验证码
	long m_lChangeServerCode;      //切换服务器的验证码
	string m_strGameCDKEY;         //连接GAMESERVER时所需cdkey
	DWORD m_lReGameCode[4];                //验证小退的参数

////////////////////////////////////////////////////
// 功能屏蔽
////////////////////////////////////////////////////
public:
	void SetTradeAble(bool b)	{m_bTradeAble=b;}
	bool IsTradeAble()			{return m_bTradeAble;}

	void AddDisableChatter(const char* str)	{m_listDisableChatter.push_back(str);}
	void DelDisableChatter(const char* str)	{m_listDisableChatter.remove(str);}
	bool CheckDisableChatter(const char* str)	{return find(m_listDisableChatter.begin(), m_listDisableChatter.end(), str) != m_listDisableChatter.end();}

private:
	bool m_bTradeAble;							// 可否交易
	list<string>		m_listDisableChatter;	// 屏蔽聊天者名字

//////////////////////////////////////////////////////////////////////////
//	场景设置
//////////////////////////////////////////////////////////////////////////
public:
	struct tagRegion
	{
		string strBgMusic;	//场景背景音乐文件名
		string strLoadingPic;	//场景切换的图片文件名
		int bgMusicID;	//背景音效在声音引擎中的ID
		bool bgMusicPlaying;	//背景音效是否处于播放状态
		string strFightMusic;	//战斗音效文件名
		int FightMusicID;	//战斗音效在声音引擎中的ID
		bool bFightMusicPlaying;	//战斗音效是否处于播放状态
		bool bPlayerPointLight;		//主角是否添加点光源
		float fPlayerPointLightRadius;	//主角点光源的半径
		DWORD dwPlayerPointLightColor;	//主角点光源的颜色
		bool bUnderWater;			// 是否是水下场景
	};

private:
	map<long/*regionID*/,tagRegion> m_mapRegion;
	vector<string>		m_vectorLoadingTips;	// Loading时候的提示

/////////////////////////////////////////////////////////////////////////
// 游戏配置
/////////////////////////////////////////////////////////////////////////
public:
	struct tagSetup
	{
		tagSetup():bSound(true),bMusic(true),bTeamDisable(true){}

		bool bTeamDisable;
		bool bTradeDisable;
		bool bFriendDisable;

		BOOL	bFullscreen;
		long	lWidth;
		long	lHeight;
		long	lBitsPerPel;
		long	lFrequency;
		long	lTexturemode;
		bool	b2DLinear;		//是否开启2D线性纹理过滤模式
		bool	b3DLinear;		//是否开启3D线性纹理过滤模式
		long	lLight;
		long	lModel;
		long    lnotsimplemodelnum;
		long	lShadow;
		long	lTexture;
		bool	bRoleFlash;			//是否开启角色受伤闪光效果
		bool	bRoleShake;			//是否开启角色受伤震动效果
		bool	bShock;				//是否开启某些技能释放时,屏幕震动效果
		long	lWeather;
		long	lControl;
		long	lViewRange;
		long    lAntiAliasing;      //全屏抗锯齿
		bool	bSound;
		bool	bMusic;
		bool    bSpeech;
		float	fSoundVolume;
		float	fMusicVolume;
		float	fSpeechVolume;
		bool    bUseVSync;           //垂直同步
		long    lFrameControl;        //帧数控制
		bool    bupdateonlostfoucs;
		bool    bfreecamera;
		bool    b360viewrangecamera;
		bool    bAotuRestorecamera;     //是否自动恢复视角


		//声音性能提示
		bool	bOthersFootstep;			//是否播放其他玩家的脚步声
		long	l3DSoundsRange;				//可以播放其他玩家怪物声音的最大距离（按格子算）

		// 属性换算 - 连入服务器时发送过来
		float	fStr2MinAtk[OCC_Max];
		float	fStr2MaxAtk[OCC_Max];
		float	fStr2Burden[OCC_Max];
		float	fStr2Hit[OCC_Max];
		float	fDex2Hit[OCC_Max];
		float	fDex2Speed[OCC_Max];
		float	fDex2Flee[OCC_Max];
		float	fCon2MaxHp[OCC_Max];
		float	fCon2Def[OCC_Max];
		float	fInt2MaxMp[OCC_Max];
		float	fInt2Resistant[OCC_Max];

		//物品保值比
		float	fBasePriceRate;

		//物品折价率
		FLOAT	fTradeInRate;

		//物品修理价格系数
		float	fRepairFactor;

		long	lHitTime;	// 连击时间
		WORD	wBaseRp_Lv1;
		WORD	wBaseRp_Lv2;

		//杀一个人对应的PK值
		WORD	wPkCountPerKill;

		char	szTalkWorldGoodsName[32];	// 世界喊话物品原始名
		int		nTalkWorldGoodsNum;			// 世界喊话物品数量
		DWORD	dwTalkWorldMoney;			// 世界喊话金钱数量

		char	szTalkCountryGoodsName[32];	// 国家喊话物品原始名
		int		nTalkCountryGoodsNum;		// 国家喊话物品数量
		DWORD	dwTalkCountryMoney;			// 国家喊话金钱数量

		char	szTalkCountryUnionGoodsName[32];	// 国家喊话物品原始名
		int		nTalkCountryUnionGoodsNum;			// 国家喊话物品数量
		DWORD	dwTalkCountryUnionMoney;			// 国家喊话金钱数量
	};
	struct	tagAutoRestorPlayerSetup
	{
		string	HpGoods1OriginName;
		string	HpGoods2OriginName;
		string	MpGoods1OriginName;
		string	MpGoods2OriginName;
		bool	AllHpCheck;
		bool	HpCheck1;
		int		Hp1Value;
		bool	HpCheck2;
		int		Hp2Value;
		bool	AllMpCheck;
		bool	MpCheck1;
		int		Mp1Value;
		bool	MpCheck2;
		int		Mp2Value;
	};
	struct tagGameViewSetup
	{
		float	ViewFrustumRange;			//D3D视图截锥角度 影响场景视野
		float	ViewPlanformAngle;			//场景摄像机俯视高度
	};
	struct tagTabSet 
	{
		// Tab选择对象相关标记
		bool    bEnemyPlayer;			// 敌国玩家
		bool    bPiratePlayer;			// 海盗玩家
		bool    bMonster;				// 怪物
		bool    bNotTeamPlayer;			// 非本队玩家
	};
private:
	tagSetup		m_Setup;
	tagAutoRestorPlayerSetup	m_RestoreSetup;
	CDupliRegionSetup	*m_pDupliRegionSetup;
	tagGameViewSetup	m_GameViewSetup;
	tagTabSet		m_TabSet;

	//内存影射文件
	CMemClient	*m_pMemClient;
	//每次运新产生的记录堆栈的文件名字
	char m_pszRunStackFile[MAX_PATH];
public:
	tagSetup*	GetSetup()			{return &m_Setup;}
	tagTabSet*	GetTabSet()			{return &m_TabSet;}
	// Tab保存和装载
	void LoadTabSet();
	void SaveTabSet();

	void UpdateSetup();				// 刷新设置
	void PresentScence();
	void LoadSetup();
	void SaveSetup();
	tagAutoRestorPlayerSetup *GetPlayerRestoreSetup() {return &m_RestoreSetup;}
	void LoadRegionData();
	map<long,tagRegion>* GetRegionSetup()		{return & m_mapRegion;}
	CDupliRegionSetup* GetDupliRegionSetup()	{return m_pDupliRegionSetup;}
	const char* GetStatckFileName()	{return m_pszRunStackFile;}

	tagGameViewSetup	*GetViewSetup()			{return & m_GameViewSetup;}
	void	LoadGameViewSetup();

	GoodsExSetup*		GetGoodsExSetup()			{return &m_GoodsExSetup;}
	CFactionManager*	GetFactionManager()			{return &m_FactionManager;}
	CUnionManager*		GetUnionManager()			{return &m_UnionManager;}

//////////////////////////////////////////////////////////////////////////
// 自动登陆
//////////////////////////////////////////////////////////////////////////
public:
	struct tagAutoLogin
	{
		string strCdkey;
		string strPassword;
		string strWorldName;
		long lPlayerPos;
	};

	bool IsAutoLogin()	{return m_bAutoLogin;}
	void SetAutoLogin(bool b)	{m_bAutoLogin=b;}
	tagAutoLogin* GetAutoLogin()	{return &m_stAutoLogin;}

private:
	tagAutoLogin m_stAutoLogin;
	bool m_bAutoLogin;

public:
	// 多行文本显示
	HRESULT DrawString(int x, int y, char* pString, D3DCOLOR Color, int nLength, int nLineSpace);
	HRESULT DrawStringEx(int x, int y, char* pString, D3DCOLOR Color, D3DCOLOR BkColor, D3DCOLOR bkBarColor, int nLength, int nLineSpace);	//绘制带表情的字符串
	long GetStringLine(char* pString, int nLength);

	// modify by lpf 2007-11-27 - 取掉了fScale和bScale参数的传入,并增加了一个iStyleIndex参数
	void TextOut(int iX, int iY, const char * pszString, int iLen, D3DCOLOR color, int iStyleIndex = 0);

	void DrawText(int iX, int iY, const char * pszString, PFONTINFO pFontInfo = NULL);					//根据字体信息来输出文本
	void DrawText(int iX, int iY, const char * pszString, int iLen, PFONTINFO pFontInfo = NULL);		//根据字体信息来输出指定长度的文本

	void GetFontInfo(PFONTINFO pFontInfo);				//得到2D渲染层当前字体渲染信息
	void SetFontInfo(const PFONTINFO pFontInfo);		//设定2D渲染层当前字体渲染信息

	long GetFontWidth(int iStyleIndex);
	long GetFontHeight(int iStyleIndex);
	int	 CalcTextWidth(const char *pszTextBuffer, int iLen);

	void NumberOut(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale = 1.0f,bool bScale=true);
	void NumberOut1(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale = 1.0f,bool bScale=true);
	long GetNumberWidth();
	long GetNumberHeight();

	void DrawLine(int iStartX,int iStartY,int iEndX,int iEndY,D3DCOLOR color = 0XFFFFFFFF, bool bScale=true);
	void DrawSolidQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR color = 0XFFFFFFFF);
	void DrawSolidQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color = 0xFFFFFFFF);

	void DrawWireQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color = 0XFFFFFFFF);

	void DrawToolTip(int nX,int nY,int nWidth,int nHeight,const char* pStr,D3DCOLOR textColor);

//////////////////////////////////////////////////////////////////////////
//	天气
//////////////////////////////////////////////////////////////////////////
public:
	WeatherManager* GetWeatherManager()		{return m_pWeatherManager;}

protected:
	WeatherManager* m_pWeatherManager;

//////////////////////////////////////////////////////////////////////////
//	鼠标上的技能图标
//////////////////////////////////////////////////////////////////////////
public:
	void SetHandSkillIcon(WORD w)	{m_wHandSkillIcon=w;}
	WORD GetHandSkillIcon()			{return m_wHandSkillIcon;}
	GameModel::AnimInfo * GetHandSkillIconAnimInfo() { return &m_tAnimInfo;}

protected:
	WORD m_wHandSkillIcon;		// 鼠标上拿的技能图标
	GameModel::AnimInfo m_tAnimInfo;
	
//////////////////////////////////////////////////////////////////////////
//	屏幕中心点
//////////////////////////////////////////////////////////////////////////
public:
	float GetScrollCenterX()		{return m_fScrollCenterX;}
	void SetScrollCenterX(float f)	{m_fScrollCenterX=f;}
	float GetScrollCenterY()		{return m_fScrollCenterY;}
	void SetScrollCenterY(float f)	{m_fScrollCenterY=f;}

protected:
	float	m_fScrollCenterX;
	float	m_fScrollCenterY;


//////////////////////////////////////////////////////////////////////////
//	最后一次点的NPC id
//////////////////////////////////////////////////////////////////////////
public:
	const CGUID&	GetLastTouchNpcID()			{return m_LastTouchNpcID;}
	void	SetLastTouchNpcID(const CGUID& ID)	{m_LastTouchNpcID=ID;}

private:
	CGUID	m_LastTouchNpcID;

//////////////////////////////////////////////////////////////////////////
//	更新Shape的坐标
//////////////////////////////////////////////////////////////////////////
public:
	//void UpdateMoveShapePosition();
	void UpdateMoveShapePositionEx();
	void UpdateAnimation();					//更新动画,防止失去焦点后,所有对象动画会同步播放

//////////////////////////////////////////////////////////////////////////
//	猎手武器和特效的对应列表
//////////////////////////////////////////////////////////////////////////
public:
	struct tagArcheryEffect
	{
		DWORD	dwWeaponGroundID;		//	武器地面图形
		DWORD	dwArcheryEffect;		//	射出去的特效
	};

	void LoadArcheryEffect(const char* filename);
	DWORD GetArcheryEffect(DWORD dwWeaponGroundID);

protected:
	list<tagArcheryEffect>		m_listArcheryEffect;	// 武器和特效的对应列表

//////////////////////////////////////////////////////////////////////////
//	表情文件对应列表
//////////////////////////////////////////////////////////////////////////
public:
	struct tagFace
	{
		string strFaceCode;		//表情代码
		int iStartIndex;		//开始序号
		int iEndIndex;			//结束序号
		int iCurCycle;			//起始动画周期
		int iCycle;				//动画周期
		int iCurIndex;			//当前帧
	};
	void LoadFace(const char* filename);
	void UpdateFace();
	list<tagFace> m_listFace;	//表情对应列表

	//输出的表情结构
	struct tagFaceInfo
	{
		int iX;			//x,y坐标
		int iY;
		int iWidth;		//宽和高
		int iHeight;
		RECT rc;		//矩形的坐标
	};

	// Server的时间
	public:
		bool m_bMsgTimeStart;
		unsigned long m_ulServerTime;
		unsigned long m_ulClientTime;

//顶部提示信息
private:
	bool				m_bDisplayInfo;			// 是否是文字显示阶段
	list<stTopInfo>		m_TopInfos;				// 顶部提示信息
	stCurTopInfo		m_stCurTopInfo;			// 顶部的当前提示内容
	long				m_lDisplaySpeed;		// 显示文字的速度
	long				m_lDisplayIntel;		//显示时间间隔
	DWORD				m_dwDisplayTime;		// 显示提示信息时间点
public:
	//客户端顶部提示信息运行
	void RunTopInfo();
	//添加提示信息
	void AddTopInfo(long lTopInfoID,long lFlag,long lParam,string strInfo);
	//得到提示信息的内容
	bool GetCurTopInfo(stCurTopInfo& TopInfo);
	//显示顶部信息
	void DrawTopInfos();

//======================================================================================
private:
	struct tagWinMessageParam
	{
		UINT message;
		WPARAM wParam;
		LPARAM lParam;
	};
	//struct t
	list<tagWinMessageParam> m_quMessage;
public:
	bool PushWinMessage(UINT message, WPARAM wParam, LPARAM lParam);
	bool PopWinMessage();
//======================================================================================



//////////////////////////////////////////////////////////////////////////
// 仪态相关(即表情动作) add by lpf(2008.04.09)
// - 包含表情和动作两个方面
//////////////////////////////////////////////////////////////////////////
private:
	CFaceActionList * m_pFaceActionList;						//仪态动作列表

public:
	CFaceActionList * GetFaceActionList()		{ return m_pFaceActionList; }		//得到仪态动作列表

//=======================当前玩家聊天信息中的物品信息=====================
public:
	struct stChatGoodsInfo 
	{
		CGUID    idEx;
		CGoods * pGoods;
		stChatGoodsInfo()
		{
			idEx	= CGUID::GUID_INVALID;
			pGoods	= NULL;
		}
	};
private:
	list<stChatGoodsInfo> m_lsChatGoodsInfo;
public:
	list<stChatGoodsInfo> *GetChatGoodsInfoList()	{return &m_lsChatGoodsInfo;}
	CGoods * FindGoodsInfo(const CGUID &guid);
	//========================================================================

//////////////////////////////////////////////////////////////////////////
// NPC任务特效优先级 liuke (2008.12.03)
// - 任务特效的优先级以及各自对应的特效ID
//////////////////////////////////////////////////////////////////////////
	private:
		vector<long>	m_vecMonsterQuestEffect;		// 任务特效优先级列表
		void LoadMonsterQuestEffect(const char *filename);
	public:
		long GetMonsterQuestEffectID(long lIndex);			// 根据传入的优先级来得到对应的特效ID
		long GetMonsterQuestEffectLevel(long lEffectID);	// 根据传入的特效ID来得到对应的优先级

///////////////////////////////////////////////////////////////////////////
//写入注册表信息
//-将客户端目录等信息写入注册表 zw(2008.12.10)
///////////////////////////////////////////////////////////////////////////
	private:
		bool m_bReg;                            //写入注册表的标志（初始化为false,写入后为true）
	public:
		bool WriteReg(string strPath,string KeyValue);                        //将信息写入注册表

	//------------测试程序消息处理------------
		void ProcessTestMessage();
//反外挂标志
	private:
		bool m_bIsBeCheateForbid;				//是否被反外挂 禁止登陆	
	public:
		void SetIsBeCheatForbid(bool bForbid)	{m_bIsBeCheateForbid = bForbid;}
		bool IsBeCheatForbid()					{return m_bIsBeCheateForbid;}
		void ResetCheatSystem();		//重置反外挂系统为初始状态
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
	public:
		void CloseIGW();                       //关闭IGW
		void OpenIGW();                        //初始化IGW
		LRESULT OnWindowProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);     //IGW相关WINDOOWS消息回调
		void ShowLoginDialog();                     //显示圈圈登陆对话框
		void IGWBalance();                          //查询余额
		void IGWOnLostDevice();                     //丢失设备
		void IGWOnResetDevice();                    //重设设备
		void NodifyAudioChanged();                  //声音改变
		void ConnectLoginServer(const char* const logIP,long logPort,const char* SonId,string wsName="");   //盛大通行证登陆ls的消息发送
		void SetPlayerInfo(const char* Name,int iSex);       //设置玩家姓名
		void OpenUrl(const char* UrlId);            //打开消费操作的反馈
		void SetSndaid(const char* SndaId){m_strSndaid = SndaId;};   //设置盛大数字帐号
		const char *GetSndaid(){return m_strSndaid.c_str();};        //获得盛大数字帐号
		void SetIsSadoLog(bool l){m_bIsSadoLog=l;};                  //设置此次登陆是否是盛大通行证登陆
		bool GetIsSadoLog(){return m_bIsSadoLog;}                    //获得此次登陆是否是盛大通行证登陆
		void SetSonID(const char *t){m_strSonId = t;};               //设置用于后台验证的token串
		const char *GetSonID(){return m_strSonId.c_str();};          //获得用于后台验证的token串
		void SDOLogOut();                                             //igw的登出操作
		void SetSDOFocus();		                                      //设置igw的焦点
		void SetSadoLogPageOpen(bool l){m_bSadoLogPageOpen = l;};     //设置圈圈登陆界面是否打开
		bool GetSadoLogPageOpen(){return m_bSadoLogPageOpen;};        //获得圈圈登陆界面是否打开
		void ReleaseIGW();
		void ReadLogMode();                                           //从配置中读取登陆模式
		void ReLoadGame();
		void SetAreaId(int i){m_iAreaID=i;};                          //设置游戏区ID
		int GetAreaId(){return m_iAreaID;};                           //返回游戏区ID
		void SetGroupId(int i){m_iGroupID=i;};                          //设置游戏区ID
		int GetGroupId(){return m_iGroupID;};                           //返回游戏区ID
		bool GetIGWFocus();                                           //IGW是否有焦点
		bool GetIsShowSdo(){return m_bIsShowSdo;};
		void SetIsShowSdo(bool b){m_bIsShowSdo = b;};                    //设置是否显示sdo界面
		bool ServerlistDat(){return m_bServerlistDat;};
		bool AutoQuestTrace(){return m_bQuestTrace;};      

		void SaveLastVersion();
		void LoadLastVersion();
		bool CompareVersion();

		void LoadOtherExeIni();                                   //用于读取其他exe启动程序的配置文件
		void RunOtherExe();                                       //运行其他的启动程序

	private:
		string m_strSonId;                         //用于后台验证的token串
		string m_strSndaid;                        //盛大通行证的数字帐号
		bool m_bIsSadoLog;                         //本次登陆是否是盛大通行证登陆
		bool m_bSadoLogPageOpen;                   //盛大通行证登陆界面是否打开
		int  m_iAreaID;                            //游戏区ID
		int  m_iGroupID;                           //游戏组ID
		bool m_bIsShowSdo;                         //是否显示sdo界面

		bool m_bServerlistDat;                     //是否读取serverlist的编码文件 
		bool m_bQuestTrace;                        //是否自动显示任务追踪
		bool m_bShowUserTreaty;                    //是否显示用户协议

		string m_LastVersion;                      //上次可户端版本号

		vector<string>   m_VecOtherExeFile;        //其它exe文件名列表
};



////////////////////////////////////////////////////
//	通用接口
////////////////////////////////////////////////////
extern bool CreateGame();
extern bool DeleteGame();
extern CGame* GetGame();

extern CMouse* GetMouse();
extern CKeyBoard* GetKeyboard();
extern CGuangMGUIApp* GetGuangMGUIApp();

extern bool g_bDebug;
extern DWORD g_LoginTime;
extern bool g_bLogin;