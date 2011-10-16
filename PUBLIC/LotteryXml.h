#pragma once

#include "tinystr.h"
#include "tinyxml.h"
#include <map>
#include <vector>
#include "tools.h"



//!		错误枚举
enum	eLotteryErrCode
{
	eLEC_Succeed		= 0,		//! 成功

	eLEC_NoEnoughSpace,				//! 背包空间不够
	eLEC_NoEnoughToken,				//! 代币不足
	eLEC_NoEnoughTimeInterVal,      //! 时间间隔不够
	eLEC_NotInit,					//! 没有进行初始化或则需要进行强制重置
};


//!		轮盘类型枚举
enum	eLotteryRouletteKind
{
	eLRK_Star,						//! 星
	eLRK_Moon,						//! 月
	eLRK_Sun,						//! 日

	MAXNUM_LRK
};


struct tagLotteryField   //抽奖栏位
{
	int iFieldIndex;
	int iFieldLevel;
	long lProbability;
	tagLotteryField()
	{
		iFieldIndex = 0;
		iFieldLevel = 0;
		lProbability = 0;
	}
};

struct tagLotteryGoods
{
	int iGoodsIndex;
	int iGoodsNum;
	long lProbability;
	tagLotteryGoods()
	{
		iGoodsIndex = 0;
		iGoodsNum = 0;
		lProbability = 0;
	}
};

struct tagCoronaType
{
	int FieldLevel;
	long lProbability;
	tagCoronaType()
	{
		FieldLevel = 0;
		lProbability = 0;
	}
};

//该结构用于分布随机事件的概率段
struct tagProbability
{
	DWORD Index;       // 物品索引
	long ProbabilityMin;    // 所在概率区间小值
	long ProbabilityMax;    // 所在概率区间大值
};

struct tagLottery
{
	int iFieldID;
	DWORD dwGoodsIndex;
	long lConstel;
	tagLottery()
	{
		iFieldID = 0;
		dwGoodsIndex = 0;
		lConstel = 0;
	}
};

class CLotteryXml
{
public:
	CLotteryXml(void);
	~CLotteryXml(void);
	bool LoadLotteryXml();
	bool LoadLotteryField(TiXmlElement* pElem);
	bool LoadLotteryGoods(TiXmlElement* pElem);	
	bool LoadCoronaType(TiXmlElement* pElem);	
	bool LoadConstySetup(TiXmlElement* pElem);
	DWORD RandGoodsFromLotteryGoods(int iLevel);              //随机抽取指定档次下配置的物品
	DWORD GetFieldSize(){return m_VecLotteryField.size();};
	int GetFieldLevel(int FieldID);      //返回一个指定栏位的档次
	int RandFieldLevel(int iLevel,int iLevelConsty=-1);   //通过轮盘类型随机出一个抽奖档次(参数二表示星座影响的档次)
	bool GetFieldByLevel(int iLevel,vector<tagLotteryField>& VecFieldByLevel);   //获得配置中该档次下所有栏位
	bool LoadLotterySetup(TiXmlElement* pElem);

	long GetLotteryCredit(){return m_lCredit;};
	int GetLotteryNumMin(){return m_iLotteryNumMin;};           //获得剩余物品最小数量限制参数
	int GetLotteryNumLimit(){return m_iLotteryNumLimit;};		//获得剩余物品限制参数.
	void SetRestNumLimit(int i ){ m_iResetNumLimit = i;};
	int  GetResetNumLimit(){return m_iResetNumLimit;};
	long GetConstyProbaToLevel(){ return m_lConstyProbaToLevel;};
	long GetConstyProbaToField(){return m_lConstyProbaToField;};
    long GetShinningInterval(){return m_lShinningInterval;};
	long GetPersonalCreditMax(){return m_lPersonalCreditMax;};  //获得个人积分上限
	long GetWorldCreditMax() {return m_lWorldCreditMax;};       //获得世界积分上限
	DWORD GetTokenIndex() {return m_dwTokenIndex;};             //获得代币物品
	int GetTokenCost() {return m_iTokenCost;};                   //获得抽奖所需要的代币数量
	int GetTokenCost_1(){return m_iTokenCost_1;};				//获得奖品数量小于限制参数时所需要的代币数量
	int GetRefreshGoodsCost(){return m_iRefreshGoodsCost;};    //获得重置物品所需代币数量
	int GetRefreshConstCost(){return m_iRefreshConstCost;};    //获得重置星座所需代币数量
    DWORD GetTimeInterVal(){return m_dwTimeInterVal;};			//获得抽奖时间间隔
	DWORD GetWorldCreditRefreshTime(){return m_dwWorldCreditRefreshTime;};//获得世界积分刷新时间
	int GetLotteryGoodsNum(int iLevel,DWORD dwIndex);
	long GetCredit(){return m_lCredit;};

	bool IsLotteryShowSwitch(){return m_bLotteryShowSwitch;};   //奖品是否显示
	DWORD GetOneItemTime(){return m_dwOneItemTime;};							//轮盘转动一格的时间
	DWORD GetRouletteCircles(){return m_dwRouletteCircles;};					//轮盘转动的最少圈数
	DWORD GetAddPowerCircles(){return m_dwAddPowerCircles;};					//力量槽的总畜力圈数
	DWORD GetResetPrizesTime(){return m_dwResetPrizesTime;};					//重置奖品的特效时间
	DWORD GetResetConstellationsTime(){return m_dwResetConstellationsTime;};	//重置星座的特效时间
	DWORD GetBlindTime(){return m_dwBlindTime;};								//中奖闪烁的特效时间

	bool CodeToByteArray(DBWriteSet& dbWriteSet);
	bool DecodeToByteArray(DBReadSet& dbReadSet);

private:
    //输出错误信息
    void OutputErrorMsg(char* pMsg);
    //清除所有容器
    void Reset();

private:
	vector<tagLotteryField> m_VecLotteryField;
	map<int,vector<tagLotteryGoods>> m_MapLotteryGoods;
	map<int,vector<tagCoronaType>> m_MapCoronaType;

	int m_iLotteryNumMin;			//剩余物品最小数量限制参数
	int m_iLotteryNumLimit;			//剩余物品限制参数
	int m_iResetNumLimit;
	long m_lConstyProbaToLevel;    //星座影响的档次的概率
	long m_lConstyProbaToField;    //星座影响的栏位的概率
    long m_lShinningInterval;      //UI闪烁特效的间隔时间（毫秒）

	long m_lCredit;			       //单次抽奖所获得的个人积分
	long m_lPersonalCreditMax;     //个人积分上限
	long m_lWorldCreditMax;        //世界积分上限
	DWORD m_dwTokenIndex;          //代币索引
	int m_iTokenCost;		       //抽奖所需要的代币数量
	int m_iTokenCost_1;				//奖品数量小于限制参数时所需要的代币数量
	int m_iRefreshGoodsCost;        //重置物品所需要的代币数量
	int m_iRefreshConstCost;        //重置星座所需要的代币数量

	DWORD m_dwTimeInterVal;		   //抽奖时间间隔
	DWORD m_dwWorldCreditRefreshTime; //世界积分刷新的时间间隔
	bool m_bLotteryShowSwitch;    //奖品显示开关

	DWORD m_dwOneItemTime;		   //轮盘转动一格的时间
	DWORD m_dwRouletteCircles;		   //轮盘转动的最少圈数
	DWORD m_dwAddPowerCircles;		   //力量槽的总畜力圈数
	DWORD m_dwResetPrizesTime;		   //重置奖品的特效时间
	DWORD m_dwResetConstellationsTime;	//重置星座的特效时间
	DWORD m_dwBlindTime;				//中奖闪烁的特效时间
};
