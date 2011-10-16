#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "../message packaging/CS2CContainerObjectMove.h"

#define CONTAINER_NUM 6

class CGoods;
class CPlayer;
class CVolumeLimitGoodsContainer;

//测试容器的CELL信息
struct tagCellInfo 
{
	DWORD dwGoodsId;
	DWORD dwGoodsMaxNum;
	DWORD dwGoodsNum;
	CGUID goodsGuid;
};

//测试容器信息
struct tagContainerInfo
{
	DWORD dwContainerId;
	DWORD dwContainerVolume;
	tagCellInfo* pCellInfo;
};

//测试结果
struct tagTestResult
{
	DWORD dwContainerId;
	DWORD dwPos;
	CS2CContainerObjectMove::OPERATION_TYPE eOperType;
};

//根据原始名和数量进行删除测试的结构体
struct tagTestArg_ID_Num
{
	DWORD _dwGoodsID;
	DWORD _dwNum;
};

//删除测试结果,可用于记录删除指定背包位置物品的数量
struct tagDelTestResult
{
	DWORD dwContainerId;		
	DWORD dwPos;
	DWORD dwNum;//删除数量
	CS2CContainerObjectMove::OPERATION_TYPE eOperType;
};


class CTestContainer
{
private:
	tagContainerInfo m_ContainerInfo[CONTAINER_NUM];
	int m_ContainerNum;
public:		
	CTestContainer();
	~CTestContainer();	
public:
	VOID Init(CPlayer* pPlayer);
	/// 手动添加测试背包
	VOID AddContainer( DWORD dwContainerID, CVolumeLimitGoodsContainer* pVolumeContainer );
	VOID FillCell(int nIndex,CVolumeLimitGoodsContainer* pVolumeContainer);
	BOOL FillOneGoods(CGoods* pGoods,tagTestResult* pResult);
	BOOL ClearOneGoods(CGoods* pGoods);
	VOID Clear();
	VOID ClearCell(int nIndex,int nPos);
	BOOL DelTest(vector<CGoods*>& vecGoodsOut);
	BOOL DelTest(vector<tagTestArg_ID_Num>& vecArg,vector<tagDelTestResult>* vecResult);
	BOOL AddTest(vector<CGoods*>& vecGoodsIn,vector<tagTestResult>* vecResult=NULL);
	BOOL AddTest(CGoods* pGoods,vector<tagTestResult>* vecResult=NULL);
	VOID Release();
};