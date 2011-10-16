/* CFaceActionList.h
 * 功能: 表情动作列表容器类
 * 摘要: 该类对象分别拥有玩家和怪物的表情列表,通过初始化时从配置文件载入数据,用自身提供的接口对表情动作进行操作
 * 作者: lpf
 * 创建日期: 2008-06-16
 */
class CMoveShape;
// 表情动作配置信息结构体
struct tagFaceAction
{
	DWORD  dwID;			//ID
	string sCallStr;		//调用字符串(动作代码,如果是怪物仪态列表则该成员值为空)
	string sNote;			//注释

	// 动作相关
	int	   iActionIndex;	//动作文件索引
	string sActionFileName;	//动作文件名

	bool bPlayInWater;		//是否可以在水下播放

	// 表情相关
	DWORD  dwFaceCount;		//表情次数
	DWORD  dwFaceStartIndex;//表情开始序号
	DWORD  dwFaceEndIndex;	//表情结束序号
	DWORD  dwFaceInterval;	//表情跳帧速度(毫秒)
	DWORD  dwFaceFileHandle;//表情文件句柄
	string sFaceFileName;	//表情文件名

	string sShowTextTarget;		//显示文本(有目标)
	string sShowTextNoTarget;	//显示文本(没有目标)

	tagFaceAction() : dwID(0), sCallStr(""), sNote(""), iActionIndex(0), sActionFileName(""), bPlayInWater(false), dwFaceCount(0), dwFaceStartIndex(0), dwFaceEndIndex(0), dwFaceInterval(0), dwFaceFileHandle(0), sFaceFileName(""), sShowTextTarget(""), sShowTextNoTarget("") {}
};

// 分解动作结构体
struct tagActionDec
{
	string sAction;				//动作编号
	int	   iLoopCount;			//动作循环次数
};

// 动作列表配置信息结构体
struct tagActionList
{
	DWORD dwIdEffect;			//特效ID
	DWORD dwStartTime;			//特效开始播放的时间
	bool  bIsLoop;				//特效是否循环播放
	DWORD eLocator;				//特效绑定点

	vector<tagActionDec> m_vecActionDecList;			//分解动作列表
};



// 表情动作列表类
class CFaceActionList
{
	vector<tagFaceAction> m_vecPlayerFaceAction;				//玩家仪态动作表
	vector<tagActionList> m_vecPlayerActionList;				//玩家仪态动作列表
	vector<tagFaceAction> m_vecMonsterFaceAction;				//怪物仪态动作表

public:
	CFaceActionList(void);
	~CFaceActionList(void);

	// 数据访问函数
	DWORD			GetPlayerFaceActionID(const char * pszStr);				//根据传入的字符串得到玩家仪态ID
	tagFaceAction * GetPlayerFaceAction(DWORD dwID);						//根据传入的ID得到玩家仪态动作
	tagFaceAction * GetMonsterFaceAction(DWORD dwID);						//根据传入的ID得到怪物仪态动作
	tagActionList * GetActionList(DWORD dwIndex);							//根据传入的动作文件索引得到动作列表

	// 操作接口
	bool LoadFaceAction(const char * pszFileDir);											//载入仪态配置文件
	bool LoadActionList(const string & sFilePathName);										//载入玩家动作配置
	void Release();																			//释放仪态列表
	void SetupFaceAction(DWORD dwFaceActionID, CMoveShape * pSourceObj, long lSourceObjType, CMoveShape * pTargetObj, long lTargetObjType);		//设定仪态动作
	bool IsCanPlay(DWORD dwFaceActionID, CMoveShape * pSourceObj, long lSourceObjType);		//检查是否可以播放指定的仪态动作
};