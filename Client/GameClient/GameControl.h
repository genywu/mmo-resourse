#pragma once
class CFindPath;
class CClientRegion;
class CShape;
class CMoveShape;
class CGoods;
class CPlayer;
class CFloatMove;
enum	MoveDir
{
	MOVE_NONE			= 0x0000,
	MOVE_FORWARD		= 0x0100,
	MOVE_LEFT			= 0x0010,	
	MOVE_RIGHT			= 0x0001,
	MOVE_BACK			= 0x1000,
	MOVE_LEFT_FORWARD	= 0x0110,
	MOVE_RIGHT_FORWARD  = 0x0101,
	MOVE_LEFT_BACK      = 0x1010,
	MOVE_RIGHT_BACK     = 0x1001,
};
// 游戏控制类
class CGameControl
{
	// 范围技能释放时的信息（跟随鼠标释放）
	struct tagAreaSkill 
	{
		DWORD dwSkillID;
		float fPosX;
		float fPosY;
		tagAreaSkill()
		{
			dwSkillID = 0;
			fPosX = 0.f;
			fPosY = 0.f;
		}
	};
	// 当前需要释放的范围技能
	tagAreaSkill m_stCurAreaSkill;
	CClientRegion*		pRegion;
	CPlayer*			pMainPlayer;
	//CMainPlayerHand*	pMainPlayerHand;
public:
	// 使用技能的返回值
	enum eUseSkillResult
	{
		USR_NONE_THIS_SKILL = 0,		// 无此技能
		USR_SUCCESS,					// 成功
		USR_COOLTIMING,					// 技能冷却中
	};

	CGameControl(void);
	virtual ~CGameControl(void);

	void	Run();
	void	ReNew();

	//--------------------条件判断函数--------------------------
	bool	IsEveryThingNotToDo();					// 是否不允许任何操作
	bool	IsCanControlCondition();				// 是否可以操作的状态
	bool	IsCanMoveCondition();					// 是否可以行走的状态，包括了不可操作状态
	bool	IsCanPickupGoodsCondition();			// 是否可以捡起物品的状态
	bool	IsCanUseGoodsCondition(DWORD dwGoodsIndex = 0);	// 是否可以使用物品的状态
	bool	IsCanUsePhysicsSkillCondition();		// 是否可以使用物理技能的状态
	bool	IsCanUseMagicSkillCondition();			// 是否可以使用魔法技能的状态
	//----------------------------------------------------------

	//---------------------控制流程三个阶段---------------------
	long	OnMouseEvent();					// 鼠标处理
	long	OnKeyboardEvent();				// 键盘处理
	long	OnAnotherEvent();				// 其它处理（自动处理等）
	//----------------------------------------------------------

	//---------------------鼠标处理函数-------------------------
	long	OnLeftButtonDown();				// 鼠标左键按下处理
	long	OnLeftButtonUp();				// 鼠标左键松开处理
	long	OnLeftButtonHolding();			// 鼠标左键按住处理
	long	OnLeftDoubleButtonDown();		// 鼠标左键双击
	long	OnLeftClickShape();				// 鼠标左健在SHAPE上点下处理

	long	OnRightButtonDown();			// 鼠标右键按下处理
	long	OnRightButtonUp();				// 鼠标右键松开处理
	long	OnRightButtonHolding();			// 鼠标右键按住处理
	long	OnRightDoubleButtonDown();		// 鼠标右键双击
	long	OnRightClickShape();			// 鼠标右健在SHAPE上点下处理
	//----------------------------------------------------------

	//---------------------键盘处理函数-------------------------
	long	ProcessHotKey();				// 处理快捷键操作
	//----------------------------------------------------------
	//----------------------------------------------------------

	//-------------------操作控制相关函数-----------------------
public:
	bool	IsControlAble()					{return m_bControlAble;}				// 是否能被控制
	void	SetControlAble(bool b)			{m_bControlAble=b; if(b==false) SetLastControlTimer(GetCurTickCount());}

	bool	IsMoveAble()					{return m_bMoveAble;}					// 是否能被控制移动
	void	SetMoveAble(bool b)				{m_bMoveAble=b;}

	void	SetLastControlTimer(long l)		{m_lLastControlTimer=l;}				// 设置最后一次能被控制的时间
	long	GetLastControlTimer()			{return m_lLastControlTimer;}			// 获得最后一次能被控制的时间

	long	GetSendedMoveMsgCount()			{return m_lSendedMoveMsgCount;}			// 获得移动消息的数量
	void	SetSendedMoveMsgCount(long l)	{m_lSendedMoveMsgCount=l;}				// 设置移动消息的数量

	long	GetSendedUseSkillMsgCount()		{return m_lSendedUseSkillMsgCount;}
	void	SetSendedUseSkillMsgCount(long l){m_lSendedUseSkillMsgCount=l;}

	CShape* GetMouseShape()					{return m_pMouseShape;}					// 获得鼠标当前指向（悬停）的对象
	CShape* GetSelectedShape()				{return m_pSelectedShape;}				// 获得鼠标当前选择的对象
	CShape* GetCurAttractShape()			{return m_pCurAttractShape;}			// 获得鼠标当前正在攻击的对象
	CShape* GetMouseShapeGoods()			{return m_pMouseShapeGoods;}			// 获得鼠标当前指向（悬停）的物品

	void	SetMouseShape(CShape* pShape)		{m_pMouseShape=pShape;}				// 设置鼠标当前指向（悬停）的对象
	void	SetCurAttractShape(CShape* pShape)	{ m_pCurAttractShape=pShape;}
	void	SetMouseShapeGoods(CShape* pShape)	{m_pMouseShapeGoods=pShape;}		// 设置鼠标当前指向（悬停）的物品
	void	SetSelectedShape(CShape* pShape,bool bIsCToS = true);					// 设置鼠标当前选择的对象(默认需要向服务器发送选择消息)

	long	PickupGoods(CGoods* pGoods);						// 捡物品
	void	CancelSelectedTarget();								// 取消选中的目标

	void	SetIntonateBreak(bool bBreak)	{m_bIntonateBreak = bBreak;}
	bool	GetIntonateBreak();
	//CMainPlayerHand* GetPMainPlayerHand() {return pMainPlayerHand;}
	//发送消息改变移动模式
	void	ChangeMoveMode();
	bool	GetChangingMoveMode()	{return m_bChangingMoveMode;}
	void	SetChangingMoveMode(bool b){m_bChangingMoveMode=b;}

	// 设置和获得是否处于试装状态
	void	SetIsInTryOnState(bool bState)	{m_bIsInTryOnState = bState;}
	bool	GetIsInTryOnState()				{return m_bIsInTryOnState;}

	// 清空本次Tab选中对象列表
	void	ClearSelectedList()				{m_lsSelectedShape.clear();}

	// 设置和获得是否锁定队友选择
	void	SetIsTabTeamMate(bool bLock);
	bool	GetIsTabTeamMate()				{return m_bIsTabTeamMate;}

	void SetLocalIP(DWORD dwIP)	{m_dwLocalIP=dwIP;}
	//定时pingServer功能
	void PingServer();

    /// 跟随模式
    void    SetFollowLeader( bool flag=false) { m_FollowLeader = flag; ReSetFollowPoint();}
    bool    GetFollowType()const              { return m_FollowLeader; }

    void    ReSetFollowPoint()                { m_FollowDestX = 0.0f;m_FollowDestY = 0.0f;}

    void    SetCurLogSelectShape(CShape* shape){m_CurSelectedShape = shape ; }

private:
	// 和服务器进行同步的开关,表明一个消息发出去后是否收到了回应
	// true:服务器已经返回,或者超时3秒,可以行动
	// false:上个动作还在等待服务器的返回消息,这段时间内无法进行相关操作
	bool	m_bControlAble;
	// 客户端判断是否可以操作移动的标记,比如鼠标点击在界面上,那么就无法行走
	bool	m_bMoveAble;
	// 已经发送出去的行走消息数量,在未收到返回的前提下最多可以发送3条出去,也就是3步延迟
	long	m_lSendedMoveMsgCount;
	// 已经放松出去当未收到回复的使用技能消息的数量
	long	m_lSendedUseSkillMsgCount;
	// 上一次操作给服务器发消息的时间
	long	m_lLastControlTimer;
	// 鼠标左键按住的持续时间
	DWORD	m_dwLeftHoldingTime;
	// 鼠标右键键按住的持续时间
	DWORD	m_dwRightHoldingTime;
	bool	m_bIntonateBreak;				// 吟唱技能是否可以被中断
	//是否处在改变移动模型中
	bool	m_bChangingMoveMode;

	// 是否处于试装状态
	bool	m_bIsInTryOnState;

	// 选中目标的开始时间点，用于记录延迟发送选择消息
	DWORD	m_dwSelectedShapeStartTime;

	CShape* m_pMouseShapeGoods;				// 鼠标指向的物品
	CShape* m_pMouseShape;					// 鼠标指向的对象
	CShape* m_pSelectedShape;				// 当前选中的对象
	CShape* m_pCurAttractShape;				// 当前正在被攻击的对象
	CShape* m_pPressRBtnShape;				// 当前按下右键指向的对象
	long	m_lSelectedType;				// 选中对象的类型（向服务器发送的）
	CGUID	m_guidSelected;					// 选中对象的GUID（向服务器发送的）
    CShape* m_CurSelectedShape;             // 设置当前选中的Shape
	list<CShape *> m_lsSelectedShape;		// 本次Tab选择过的对象列表
	bool	m_bIsTabTeamMate;				// 是否锁定队友选择
	//----------------------------------------------------------

	//---------------------移动相关函数-------------------------

	//Ping服务器的参数控制
	//上次Ping时间
	DWORD m_dwLastPingTime;
	//本地IP
	DWORD m_dwLocalIP;
public:
	long	MoveDir(CMoveShape* pShape, long dir,short nDis=2);	// 向指定方向移动一步
	long	MoveToCell(float fDestX,float fDestY);//向某方向移动函数
	long	MoveToCellByHotKey(float fDestX,float fDestY);
	long	StopMove();//停止移动
	CFindPath* GetFindPath();									// 寻路

	long	FollowObjectAI();										// 跟随玩家移动
	void	SetFollowingObj(long lType,const CGUID& guid);				// 设置要跟随的obj
	bool	IsAutoMoving()		{return m_AutoMoveMode != AMM_NO;}			// 是否处于自动移动状态

	//MainPlayer的当前主行为模式
	typedef enum eMainPlayerActionMode
	{
		MPAM_No = 0,//无任何行为(站立)
		MPAM_AutoMove,//自动移动
		MPAM_AutoAttack,        //自动攻击
		MPAM_AutoPickGoods,     //自动拾取物品
        MPAM_AutoFollowLeader, // 跟随队长
		//MPAM_AutoCollect,	//自动采集
	}eMPAM;

	//自动移动模式
	typedef enum eAutoMoveMode
	{
		AMM_NO=0,				//无自动移动
		AMM_A_SEARCH=0x01,	//A*寻路
		AMM_TO_DESTDIR,	//向一个目标点方向移动
		AMM_FOLLOW_MOUSE,//跟随鼠标
		AMM_FOLLOW_OBJECT,//跟随一个目标对象
		AMM_STOP=0x1000,//结束
	}eAMM;

	// 攻击模式
	typedef enum eAutoAttactMode
	{
		AAM_NO=0,			// 无攻击模式
		AAM_FIND_ATTACTER,	// 追踪目标的攻击模式
		AAM_ATTACT,			// 攻击状态模式
		AAM_STOP=0x1000,			// 停止
	}eAAM;

private:
	//在攻击模式下的返回值
	enum eAAMRet
	{
		AAMR_Stop=-1,//停止该模式
		AAMR_Trace=0,//继续追踪
		AAMR_Attack=1,//攻击
		AAMR_Other,//其他情况
	};
	
	long	m_lFollowingObjType;	//　跟随目标的type
	CGUID	m_FollowingObjID;		//  跟随的玩家ID
	float	m_fStartDestX;			//  开始跟随的初始x位置
	float	m_fStartDestY;			//  开始跟随的初始y位置
	long	m_lFollowMaxDistance;	//　跟随最大距离
	CGUID	m_PickGoodsID;			// 自动拾取物品的ID

	//CGUID	m_CollectGoodsID;		// 自动采集的目标GUID

	float	m_fDestX;				// 目的地x
	float	m_fDestY;				// 目的地y
	//在自动移动AMM_TO_DESTDIR模式下，记录曾经移动过的格子。
	//如果发现格子是曾经移动过的格子，则玩家自动停止
	typedef map<POINT,long,point_compare>::iterator itMovedCell;
	map<POINT,long,point_compare>	m_MovedCell;

	eMPAM m_MainPlayerActionMode;	// 主行为模式
	long m_AutoMoveMode;			// 自动移动模式
	bool m_bResetAutoMoveMode;		// 在行走模式下点击鼠标,重置移动模式
	long m_AutoAttactMode;			// 自动攻击模式

    bool m_FollowLeader;            // 跟随队长模式
    float m_FollowDestX;
    float m_FollowDestY;
    long m_FollowRegionID;
	

	//主要用于当前有一个主行为模式,但是又要求一个不同的行为模式
	//等到没有主行为模式的时候,执行缓存的行为模式
	eMPAM m_NextMainPlayerActionMode;	// 缓冲主行为模式

	//保存主角向服务器发送的更新位置
	float m_fUpdateX;
	float m_fUpdateY;
	//----------------------------------------------------------

	//---------------------技能相关函数-------------------------
public:
	
	//---------------------自动处理函数-------------------------
	bool	IsMoveWithCurAction();						// 判断当前动作是否可以移动
	bool	IsActWithCurAction();						// 判断当前动作是否可以攻击
	bool	AddMovedCell(long lTileX,long lTileY);		// 添加移动过的格子
	void	AutoMoveTo(float DestX, float DestY);		// 开始自动向目标点行走
	void	AutoMoveToEx(float fDestX, float fDestY);		// 开始自动行走,直接向目标点的方向移动
	void	FollowObject(long lType,const CGUID& guid,long lMaxDistance);	// 跟随一个目标对象
	void	DoFollowResult();//处理跟随的结果
	long	ContinueMoveTo(float fDestX, float fDestY);	// 过程中间被中断后继续寻路
	void	ContinueMoveToOtherRegion();				// 跨场景后的继续寻路
	void	StartAttack(CShape* pShape);				// 开始攻击并设置攻击模式
	void	AutoMoveStop();								// 终止自动移动
	void	AutoAttactStop();							// 终止自动攻击
	long	AutoMoveAI();								// 自动移动的AI
	long	AutoMoveAIEx();								// 到一个点的自动移动
	void	AutoPickGood(const CGUID& guGoodID);		// 自动拾取物品
	void	AutoPickGoodStop();							// 终止自动拾取物品
	void	AutoPickGoodEx();							// 自动拾取玩家外圈的物品（需要自动移动到物品处拾取）

	//void	AutoCollect(const CGUID& guCollectID);			// 目标采集物的GUID
	//得到某个点方向上下一个移动目标点
	long	GetNextMoveDestCell(float fX,float fY,long lMaxDistance,float &fDestX,float &fDestY);
	//得到一个方向上的下一个移动目标点
	long	GetNextMoveDestCell(float fRadianDir,float fMaxDistance,float &fDestX,float &fDestY);
	long	FollowMouseMove();							//	自动跟随鼠标移动
	
	long	OnMainPlayerMove();				// 主角移动到达目标点的自动处理
	eAAMRet	OnMainPlayerAttack();			// 主角攻击的自动处理
	long	OnNextPlayerActionMode();			// 执行缓存的行为模式
	eAAMRet	AutoLockAttackAI();				// 攻击当前正在被攻击对象的AI
	eAAMRet	AutoAttackMoveAI();				// 追击当前正在被攻击对象的AI
	long	OnAutoPickupGoods();

	//long	OnAutoCollect();				

	void	DoCollect(CMoveShape* pMoveShape);

	//得到当前玩家的主行为模式
	eMPAM GetMPActionMode()	{return m_MainPlayerActionMode;}
	void SetMPActionMode(eMPAM eActionMode) {m_MainPlayerActionMode = eActionMode;}

	eMPAM GettMPNexActionMode()	{return m_NextMainPlayerActionMode;}
	void SetMPNexActionMode(eMPAM eActionMode) {m_NextMainPlayerActionMode = eActionMode;}

	long GetAutoMoveMode()	{return m_AutoMoveMode;}
	void SetAutoMoveMode(eAMM eMoveMode) {m_AutoMoveMode = eMoveMode;}

	void SetResetAutoMoveMode(bool b) {m_bResetAutoMoveMode  = b;} 
	bool IsResetAutoMoveMode();

	long GetAutoAttackMode()	{return m_AutoAttactMode;}
	void SetAutoAttackMode(eAAM eAttackMode){ m_AutoAttactMode = eAttackMode;}

	bool	StartSkill(DWORD dwIndex, CShape* pAttackShape);		// 对指定目标发起技能
	bool	StartSkill(DWORD dwIndex, long lType, const CGUID& ID);	// 对指定目标发起技能
	bool	UseOccuSkill(DWORD dwIndex,CShape* pTagShape);			// 使用副职业技能

	void	SetBufferSkillID(DWORD dw);
	void	SetLastPressHotKey(int n)	{m_nLastPressHotKey=n;}			// 设置最后一次使用的快捷键
	void	SetAutoSkillID(DWORD dw);									// 设置自动释放技能的ID

	DWORD	GetBufferSkillID()			{return m_dwBufferSkillID;}		// 获得缓冲技能ID
	int		GetLastPressHotKey()		{return m_nLastPressHotKey;}	// 获得最后一次使用的快捷键
	DWORD	GetAutoSkillID()			{return m_dwAutoSkillID;}		// 获得自动释放技能的ID
	//void	SetGoodsIndexForSkill(DWORD dwGoodsIndex) { m_dwGoodsIndexForSkill = dwGoodsIndex; } //设置触发技能的物品的ID
	//DWORD	GetGoodsIndexForSkill()		{return m_dwGoodsIndexForSkill;}	// 获取触发技能的物品的ID
	void SetSkillGoods(long lExtendID,long lPos)	{m_lSkillGoodsExtendID = lExtendID;m_lSkillGoodsPos = lPos;}
	void SetUpdateXY(float fX,float fY)	{m_fUpdateX=fX;m_fUpdateY=fY;}
	void GetUpdateXY(float &fX,float &fY);

	void SendTouchNpcMes(CGUID NpcGUID);
	eAAMRet AddSkillByGoods(DWORD m_dwCurSkillID);							// 检测一个由物品触发的技能
	
	void ProcessMoveKey();								//处理按键移动

    /// by:expter 进行相位阻挡判断
    bool IsPhaseBlock(float xNew,float yNew);             //相位阻挡

private:
	//DWORD   m_dwGoodsIndexForSkill;				// 触发技能的物品ID
	long	m_lSkillGoodsExtendID;				// 触发技能的物品所在容器ID
	long	m_lSkillGoodsPos;					// 触发技能的物品在容器中的位置
	DWORD	m_dwBufferSkillID;					// 在移动状态时按下技能快捷键会缓冲在这里，等到达下一个格子的时候使用
	DWORD	m_dwAutoSkillID;					// 当前自动释放的技能编号
	DWORD	m_dwCurSkillID;						// 当前正在使用的技能
	int		m_nLastPressHotKey;					// 上一次按下的快捷键(专门提供给 七星汇芒、绝情箭 等续力技能使用)
	//----------------------------------------------------------	

// ==============================跨场景移动（刘科08.5.6）=================================
private:
	long m_lTargetRegionID;					//目标场景ID
	//CFindPath::stPos m_stSPos;				//跨场景寻路时的原始起始点
	//CFindPath::stPos m_stTPos;				//跨场景寻路时的原始目标点（上面定义的目标点和起始点，只是在当前场景下的目标和起始点）
	long m_lSPosX;							// 原始开始点
	long m_lSPosY;
	long m_lTPosX;							// 最终的目标点
	long m_lTPosY;

public:
	/*
	* 功能: 查找指定场景下的指定位置到目标场景的指定位置的路径。
    * 摘要: 该方法首先会找出场景路径列表，然后根据每个场景节点标示的坐标，来进行寻路；
    在一个场景中寻路的时候需要根据该场景所配置的路标点来查找。
    * 参数: 依次为源场景ID和坐标（x，y），目标场景ID和坐标（x，y）
    * 返回值: 查找成功为true,失败为false
    * 作者: 刘科
    * 创建日期: 2008.5.6
    */	
    bool FindPathToOtherRegion(long lSourRegion, long lSourx, long lSoury, long lTargRegion, long lTargx, long lTargy);

    // 获得目标点的场景ID
    long GetTargetRegionID()	{return m_lTargetRegionID;}

    // 获得目标点
    //CFindPath::stPos GetTargetPos()		{return m_stTPos;}

    DWORD	m_dwMoveMsgCounter;	//玩家移动发送的消息数

    float	m_fParameterOfDisFromCamara;	//摄像机到主人的距离
 //浮点移动功能
    CFloatMove * m_FloatMove;
    char  strMoveInfo[256]; //调试消息
    //移动开始时间
    DWORD dwBegionMoveTime;
};

/*
功能：浮点移动模块
作者：倪凯
日期：2010.7.7
*/
class CFloatMove
{
public:
    CFloatMove();
    ~CFloatMove();
    /*
    功能: 对A*寻路后的路径点 转换成 需要转向的线段点。
    作者：倪凯
    日期：2010.7.7
    */
    struct tagMovPos 
    {
        float x;
        float y;
        tagMovPos():x(0.f),y(0.f){}
        const tagMovPos& operator = (const tagMovPos &other)
        {
            if (&other == this)
            {
                return *this;
            }
            x = other.x;
            y = other.y;
        }
    };
    /*玩家操作的最小距离
    说明:(玩家当前位置和目标的距离超过此值 才移动)
    */
    const float MINIMUS_OP_DISTANCE;
    
    /*直线移动上的最大距离 (超过此值的两点将被拆分)*/
    const float MAX_LINE_DISTANCE;

    //A*寻路后 拆分转向点和超长路径点后的线段路径点 
    list<tagMovPos>  A_MoveList;       
    //直线移动拆分过长路径后的线段点
    list<tagMovPos>  LineMoveList;
    //转换A*路径为转向线段路径
    void UpdateA_MoveList(CFindPath *pFindPath,const float fPosX,
        const float fPosY,const float fDestX,
        const float fDestY);
    
    //直线移动时超过最大步长的拆分处理
    void UpdatelinearMoveList(const float fPosX,const float fPosY,
        const float fDestX, const float fDestY);
    //是否是直线移动
    bool  bIsBeelineMove;

    //上一个路径点是否是A*中的
    bool  bIsPreviousMoveByA_Star;

};