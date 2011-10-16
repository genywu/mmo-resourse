#ifndef _CLottery_H_
#define _CLottery_H_
#include "..\public\LotteryXml.h"

class CLottery
{
public:
	CLottery();
	~CLottery();

	int   GetHaveGoodsField();  //获得当前物品栏中还有物品的最底档次
	void  RefreshVecLottery(); //刷新物品栏位的值
	DWORD GetGoodsFromField(int iFieldID);
	DWORD GetLotteryReFromField(int iFieldID);
	bool  CanDoLottery(CPlayer *pPlayer);//条件判断
	bool  CanResetCredit(CPlayer *pPlayer);
	bool  DoLottery(DWORD& m_dwLotteryRe,int PlayerConsty,vector<tagLottery>& VecLottery,BYTE &FileId); //进行抽奖
	void  SetLotterylevel(int i){m_iLotterylevel=i;};
	int   GetLotterylevel(){return m_iLotterylevel;};
	bool  GetLotteryFieldInfo(vector<tagLottery>& VecLottery);
	DWORD GetLotteryRe(){return m_dwLotteryRe;};
	int   GetSurplusGoods();  
	int   GetConstNum();
	void  InitLottery();
	void  InitConstel();
	void  SetPlayerConsty(int i){m_iPlayerConsty = i;};
	int   FindFieldIDFromConsty(int iConsty);    //查找一个星座对应的栏位ID
	long  GetLotteryNum(){return m_lLotteryNum;};
	long  GetLevelNum(int i){return m_lLevelNum[i];};
	void  ClaerStatistics();
	int   GetLotteryFieldRe(){return m_iLotteryFieldRe;}; //返回抽中的栏位
	int   GetLotteryGoodsNum(int iFieldId,DWORD dwGoodsIndex);   //获得某个栏位物品数量
	void  SetVecLottery(vector<tagLottery> VecLottery);  //设置栏位和星座的信息

private:
	DWORD m_dwLotteryRe; 
	vector<tagLottery> m_VecLottery;
	CLotteryXml m_LotteryXml;
	int m_iLotterylevel;
	int ConstyArray[12];         // 星座的初始数组
	int m_iPlayerConsty;       // 玩家星座
	int m_iLotteryFieldRe;       //  抽中的栏位

	long m_lLotteryNum;
	long m_lLevelNum[4];
};
#endif