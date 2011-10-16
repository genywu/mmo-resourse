#include "stdafx.h"
#include "../Player.h"
#include "../Goods/CGoodsFactory.h"
#include "CLottery.h"
class CPlayer;
CLottery::CLottery()
{
	m_VecLottery.clear();
	m_dwLotteryRe=0;
	m_iLotterylevel=0;

	for(int i=0;i<12;i++)
	{
		ConstyArray[i] = i+1;
		tagLottery tagLottery;
		tagLottery.iFieldID = i;
		m_VecLottery.push_back(tagLottery);
	}

	m_iPlayerConsty = 1;
	m_lLotteryNum=0;
	m_iLotteryFieldRe=0;

	for(int i=0;i<4;i++) 
	{
		m_lLevelNum[i] = 0;
	}
}

CLottery::~CLottery()
{
	m_VecLottery.clear();
}

void CLottery::InitLottery()
{
	//m_LotteryXml.LoadLotteryXml();
}

void CLottery::RefreshVecLottery()
{
	m_dwLotteryRe = 0;
	for(int i=0;i<GetInst(CLotteryXml).GetFieldSize();i++)
	{
		int iLevel = GetInst(CLotteryXml).GetFieldLevel(i);
		DWORD dwGoodsIndex = GetInst(CLotteryXml).RandGoodsFromLotteryGoods(iLevel);
		m_VecLottery[i].dwGoodsIndex = dwGoodsIndex;
	}
}

int CLottery::GetHaveGoodsField()
{
	vector<tagLotteryField> VecFieldByLevel;
	//首先检查第4档次
	GetInst(CLotteryXml).GetFieldByLevel(4,VecFieldByLevel);
	for(int i=0;i<VecFieldByLevel.size();i++)
	{
       DWORD dwGoods = GetGoodsFromField(VecFieldByLevel[i].iFieldIndex);
	   if(dwGoods!=0)
		   return 4;
	}
	//检查第3档次
	VecFieldByLevel.clear();
	GetInst(CLotteryXml).GetFieldByLevel(3,VecFieldByLevel);
	for(int i=0;i<VecFieldByLevel.size();i++)
	{
       DWORD dwGoods = GetGoodsFromField(VecFieldByLevel[i].iFieldIndex);
	   if(dwGoods!=0)
		   return 3;
	}
	//检查第2档次
	VecFieldByLevel.clear();
	GetInst(CLotteryXml).GetFieldByLevel(2,VecFieldByLevel);
	for(int i=0;i<VecFieldByLevel.size();i++)
	{
       DWORD dwGoods = GetGoodsFromField(VecFieldByLevel[i].iFieldIndex);
	   if(dwGoods!=0)
		   return 2;
	}
	//检查第1档次
	VecFieldByLevel.clear();
	GetInst(CLotteryXml).GetFieldByLevel(1,VecFieldByLevel);
	for(int i=0;i<VecFieldByLevel.size();i++)
	{
       DWORD dwGoods = GetGoodsFromField(VecFieldByLevel[i].iFieldIndex);
	   if(dwGoods!=0)
		   return 1;
	}
    return 0;
}

DWORD CLottery::GetGoodsFromField(int iFieldID)
{
	for(int i=0;i<m_VecLottery.size();i++)
	{
		if(m_VecLottery[i].iFieldID==iFieldID)
		{
			DWORD dwGoodsIndex = m_VecLottery[i].dwGoodsIndex;
			return dwGoodsIndex;
		}
	}
	return 0;
}

DWORD CLottery::GetLotteryReFromField(int iFieldID)
{
	for(int i=0;i<m_VecLottery.size();i++)
	{
		if(m_VecLottery[i].iFieldID==iFieldID)
		{
			DWORD dwGoodsIndex = m_VecLottery[i].dwGoodsIndex;
		    m_VecLottery[i].dwGoodsIndex = 0;
			return dwGoodsIndex;
		}
	}
	return 0;
}

bool CLottery::DoLottery(DWORD& m_dwLotteryRe,int PlayerConsty,vector<tagLottery>& VecLottery,BYTE &FileId)
{
	SetPlayerConsty(PlayerConsty);
	long lFieldID=0;   //最后中奖的栏位ID 
	int iLevel = 0;
	//找到玩家星座对应的栏位ID
    int iLuckField = FindFieldIDFromConsty(m_iPlayerConsty);
	//查找该栏位的档次
	int iLuckLevel = GetInst(CLotteryXml).GetFieldLevel(iLuckField);
	if(m_iLotterylevel!=0&&(m_iLotterylevel>=0&&m_iLotterylevel<3))
       iLevel = GetInst(CLotteryXml).RandFieldLevel(m_iLotterylevel,iLuckLevel);
	else
       iLevel = GetInst(CLotteryXml).RandFieldLevel(0,iLuckLevel);
	//获得该档次下所有栏位
    vector<tagLotteryField> VecLotteryFieldTemp;
	vector<tagLotteryField> VecLotteryField;
Begin:
	GetInst(CLotteryXml).GetFieldByLevel(iLevel,VecLotteryFieldTemp);
	//剔除该档次下没有物品的栏位
	for(int i=0;i<VecLotteryFieldTemp.size();i++)
	{
		DWORD deGoodsIndex = GetGoodsFromField(VecLotteryFieldTemp[i].iFieldIndex);
		if(deGoodsIndex!=0)
		{
			VecLotteryField.push_back(VecLotteryFieldTemp[i]);
		}
	}
	if(VecLotteryField.size()==0)
	{
        VecLotteryFieldTemp.clear();
		iLevel = GetHaveGoodsField();    //查找一个还有物品的最低档次
		if(iLevel != 0)
		   goto Begin;
		else
			return false;
	}

	vector<tagProbability> VecProbability;
	tagProbability tagProbabilityTemp;
	long lProbabilityValue = 0;   //该值用于计算概率分布空间
	//将该档次栏位概率放入概率计算区
	for(int i=0;i<VecLotteryField.size();i++)
	{
		tagProbabilityTemp.Index = VecLotteryField[i].iFieldIndex;
		tagProbabilityTemp.ProbabilityMin = lProbabilityValue;
		if(iLuckField!=tagProbabilityTemp.Index)
		{
			tagProbabilityTemp.ProbabilityMax = tagProbabilityTemp.ProbabilityMin + VecLotteryField[i].lProbability;		
			lProbabilityValue=lProbabilityValue+VecLotteryField[i].lProbability;
		}else{
			tagProbabilityTemp.ProbabilityMax = tagProbabilityTemp.ProbabilityMin + (VecLotteryField[i].lProbability*GetInst(CLotteryXml).GetConstyProbaToField())/100;		
			lProbabilityValue=lProbabilityValue+(VecLotteryField[i].lProbability*GetInst(CLotteryXml).GetConstyProbaToField())/100;
		}
		VecProbability.push_back(tagProbabilityTemp);

	}

	long lProbabilityRe = random(lProbabilityValue); 

	for(int i=0;i<VecProbability.size();i++)
	{
		if(lProbabilityRe>=VecProbability[i].ProbabilityMin&&lProbabilityRe<VecProbability[i].ProbabilityMax)
		{
			lFieldID = VecProbability[i].Index;
			break;
		}
	}
	//输出中奖物品
	m_dwLotteryRe = GetLotteryReFromField(lFieldID);
	FileId = (BYTE)lFieldID;
	m_iLotteryFieldRe = lFieldID;
	m_lLotteryNum++;
	int iShowLevel = GetInst(CLotteryXml).GetFieldLevel(lFieldID)-1;
	m_lLevelNum[iShowLevel]++;
	GetLotteryFieldInfo(VecLottery);
    return true;
}

bool CLottery::GetLotteryFieldInfo(vector<tagLottery>& VecLottery)
{
	VecLottery.clear();
	for(int i=0;i<m_VecLottery.size();i++)
	{
		VecLottery.push_back(m_VecLottery[i]);
	}
	return true;
}

int CLottery::GetSurplusGoods()
{
	int iNum = 0;
	for(int i=0;i<m_VecLottery.size();i++)
	{
		if(m_VecLottery[i].dwGoodsIndex!=0)
		{
			iNum++;
		}
	}
	return iNum;
}
int CLottery::GetConstNum()
{
	int iNum = 0;
	for(int i=0;i<m_VecLottery.size();i++)
	{
		if(m_VecLottery[i].lConstel!=0)
		{
			iNum++;
		}
	}
	return iNum;
}

void CLottery::InitConstel()
{
	//将原始星座排列加入抽奖列表
   for(int i=0;i<12;i++)
   {
	   m_VecLottery[i].lConstel = ConstyArray[i];
   }
   //随机改变抽奖列表中星座的排列
   for(int i=0;i<12;i++)
   {
	   //随机产生一个下标
	   int iIndex = rand()%12;
	   //交换当前循环位置和随机下标位置的值
	   int iTemp = 0;
	   iTemp = m_VecLottery[i].lConstel;
	   m_VecLottery[i].lConstel = m_VecLottery[iIndex].lConstel;
	   m_VecLottery[iIndex].lConstel = iTemp;
   }
}

int CLottery::FindFieldIDFromConsty(int iConsty)
{
	for(int i=0;i<m_VecLottery.size();i++)
	{
		if(m_VecLottery[i].lConstel == iConsty)
			return m_VecLottery[i].iFieldID;
	}
	return -1;
}

void CLottery::ClaerStatistics()
{
	m_lLotteryNum=0;

	for(int i=0;i<4;i++)
	{
		m_lLevelNum[i] = 0;
	}
}

int CLottery::GetLotteryGoodsNum(int iFieldId,DWORD dwGoodsIndex)
{
	//获得指定栏位的档次
	int iLevel = GetInst(CLotteryXml).GetFieldLevel(iFieldId);
	//获得该栏位上的物品
	//DWORD dwGoodsIndex = GetGoodsFromField(iFieldId);

	return GetInst(CLotteryXml).GetLotteryGoodsNum(iLevel,dwGoodsIndex);
}

void CLottery::SetVecLottery(vector<tagLottery> VecLottery)
{
	for(int i=0;i<VecLottery.size();i++)
	{
		m_VecLottery[i].iFieldID = VecLottery[i].iFieldID;
		m_VecLottery[i].dwGoodsIndex = VecLottery[i].dwGoodsIndex;
		m_VecLottery[i].lConstel = VecLottery[i].lConstel;
	}
}
bool CLottery::CanDoLottery(CPlayer *pPlayer)
{
	//上次抽奖到现在的时间间隔
	DWORD dwInterVal = timeGetTime() - pPlayer->GetLastLotteryTime();
	//抽奖的时间间隔
	DWORD dwTimeInterVal = GetInst(CLotteryXml).GetTimeInterVal();
	//剩余背包数量
	DWORD dwEmptySpaceNum = pPlayer->GetTotalEmptySpaceNum();
	//获取代币索引
	DWORD dwTokenIndex = GetInst(CLotteryXml).GetTokenIndex();
	//根据代币索引获取代币的原始名
	const char* strGoodOriName = CGoodsFactory::QueryGoodsOriginalName(dwTokenIndex);
	if(strGoodOriName==NULL)
		return false;
	//获取所需代币数量
	int iTokenCost = GetInst(CLotteryXml).GetTokenCost();
	int iTokenCost_1 = GetInst(CLotteryXml).GetTokenCost_1();
	//玩家的代币数量
	int iTokenNum = pPlayer->GetGoodsAmountByOriginName(strGoodOriName);
	//物品栏位最小数量限制
	int iLotteryNumMin = GetInst(CLotteryXml).GetLotteryNumMin();
	//物品栏位数量限制
	int iLotteryNumLimit = GetInst(CLotteryXml).GetLotteryNumLimit();

	if (dwInterVal < dwTimeInterVal)
	{
		CMessage msg(MSG_S2C_LOTTERY_Startup);
		msg.Add((long)eLEC_NoEnoughTimeInterVal);
		msg.SendToPlayer(pPlayer->GetExID());
		return false;
	}
	else if (iTokenNum < iTokenCost&&GetSurplusGoods()>iLotteryNumLimit)
	{
		CMessage msg(MSG_S2C_LOTTERY_Startup);
		msg.Add((long)eLEC_NoEnoughToken);
		msg.SendToPlayer(pPlayer->GetExID());
		return false;	 //消耗品数量不够
	}
	else if (iTokenNum < iTokenCost_1&&GetSurplusGoods()<=iLotteryNumLimit)
	{
		CMessage msg(MSG_S2C_LOTTERY_Startup);
		msg.Add((long)eLEC_NoEnoughToken);
		msg.SendToPlayer(pPlayer->GetExID());
		return false;	 //消耗品数量不够
	}
	else if (dwEmptySpaceNum< 1)
	{
		CMessage msg(MSG_S2C_LOTTERY_Startup);
		msg.Add((long)eLEC_NoEnoughSpace);
		msg.SendToPlayer(pPlayer->GetExID());
		return false;	//背包空间不够
	}
	else if (GetSurplusGoods()<= iLotteryNumMin)
	{
		CMessage msg(MSG_S2C_LOTTERY_Startup);
		msg.Add((long)eLEC_NotInit);
		msg.SendToPlayer(pPlayer->GetExID());
		return false;//物品栏位数量不够或者没有初始化
	}
	else
	{
		//设置轮盘的星座
		SetPlayerConsty(pPlayer->GetConst());
		//扣除代币
		if (GetSurplusGoods()<iLotteryNumLimit)
		{
			pPlayer->DelGoodsAmountByOriginName(strGoodOriName,iTokenCost_1);
			return true;
		}
		else
		{
			pPlayer->DelGoodsAmountByOriginName(strGoodOriName,iTokenCost);
			return true;
		}
	}
}
bool CLottery::CanResetCredit(CPlayer *pPlayer)
{
	DWORD dwCurrTime = time(NULL);
	static tm CurrTm;
	tm *pTm = localtime((time_t*)&dwCurrTime);
	memcpy(&CurrTm, pTm, sizeof(tm));
	pTm = localtime((time_t*)&(pPlayer->GetProperty().lLastResetActiveTime));

	if (pTm->tm_year == CurrTm.tm_year && pTm->tm_mon == CurrTm.tm_mon && pTm->tm_mday == CurrTm.tm_mday)
		return true;
	else
		return false;
}