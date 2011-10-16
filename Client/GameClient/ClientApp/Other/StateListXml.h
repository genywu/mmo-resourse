#pragma once
class CStateListXml
{
public:
	CStateListXml(void);
	virtual ~CStateListXml(void);
public:
	/*	2008.11.17 - 刘科修改
	 *		修改状态的特效处理方式，每个阶段一个特效列表
	 */
	struct tagStateEffect 
	{
		DWORD	dwEffectID;				// 状态的特效编号
		DWORD	dwEffectIndex;			// 状态的特效索引号（默认为0）
		DWORD	dwEffectPosition;		// 状态特效的绑定位置
		DWORD	dwEffectCycle;			// 状态特效是否循环
		bool	bIsOrderPlay;			// 状态特效是否按顺序播放
		tagStateEffect()
		{
			dwEffectID = 0;
			dwEffectIndex = 0;
			dwEffectPosition = 0;
			dwEffectCycle = 0;
			bIsOrderPlay = false;
		}
	};
	struct tagStateLevel
	{
		DWORD	dwAfettleLevel;				// 状态的等级
		string	strAfettleDesc;				// 状态的描述
		DWORD	dwAfettleContinueTime;		// 状态持续时间
		DWORD	AfettleDeathIsClear;		// 死亡时是否清除状态
		bool	bColorFade;					// 是否颜色渐变
		//---------开始阶段特效-----------
		vector<tagStateEffect> vecBeginEffect;	// 特效列表
		//---------中间阶段特效-----------
		vector<tagStateEffect> vecMidEffect;	// 特效列表
		//---------结束阶段特效-----------
		vector<tagStateEffect> vecEndEffect;	// 特效列表
		DWORD	dwInterval;					// 颜色变化间隔期（毫秒）
		vector<DWORD> vecBuffColor;			// 颜色数据队列
		//long	lIndex;						// 表示状态的索引（唯一）
		tagStateLevel()
		{
			dwAfettleLevel = 0;
			strAfettleDesc = "";
			dwAfettleContinueTime = 0;
			AfettleDeathIsClear = 0;
			dwInterval = 0;
		}
	};
	struct tagStateList
	{
		long	lAfettleID;					// 状态编号
		string	strAfettleName;				// 状态名称
		long	lAfettleICON;				// 状态图标
		DWORD	AfettleStateOnly;			// 状态是否唯一
		DWORD	AfettleIsBuff;				// 是否为有益状态
		BYTE	bStopMove;					// 是否让状态承受者停止移动
		BYTE	wAction;					// 对应的动作
		vector<tagStateLevel> vecStateLevel;	// 等级队列
		tagStateList()
		{
			lAfettleID = 0;
			strAfettleName = "";
			lAfettleICON = 0;
			AfettleStateOnly = 0;
			AfettleIsBuff = 0;
			bStopMove = 0;
			wAction = 0;
		}
	};
private:
	static map<long,tagStateList>	m_mapStateList;		// 所有的状态列表
public:
	static BOOL Load(const char* filename);				// 读取配置文件
	static map<long, tagStateList>* GetStateList(){return &m_mapStateList;}	// 取得状态队列
	static tagStateList* GetState(long lID);			// 根据状态编号取得状态
	static void Release();								// 释放队列
};