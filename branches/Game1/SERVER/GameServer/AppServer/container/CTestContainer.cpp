#include "stdafx.h"
#include "ctestcontainer.h"
#include "../Player.h"
#include "..\goods\CGoodsBaseProperties.h"
#include "../goods/CGoodsFactory.h"




CTestContainer::CTestContainer()
{
	m_ContainerNum = 0;
}

CTestContainer::~CTestContainer()
{
	Release();
}

//Player 的背包和子背包物品放置信息导入到测试容器
VOID CTestContainer::Init(CPlayer* pPlayer)
{
	if(!pPlayer)
		return;
	
	//初始化原始背包
	m_ContainerInfo[0].dwContainerId=CS2CContainerObjectMove::PEI_PACKET;
	m_ContainerInfo[0].dwContainerVolume=pPlayer->getOriginPack()->GetVolume();
	m_ContainerInfo[0].pCellInfo=new tagCellInfo[pPlayer->getOriginPack()->GetVolume()];
	FillCell(0,pPlayer->getOriginPack());

	//初始化子背包
	for(int i=1;i<CONTAINER_NUM;i++)
	{
		m_ContainerInfo[i].dwContainerId=CS2CContainerObjectMove::PEI_PACK + i;
		m_ContainerInfo[i].dwContainerVolume=0;
		m_ContainerInfo[i].pCellInfo=NULL;
		if(pPlayer->getSubpackContainer()->GetSubpack(i-1)->pGoods)
		{
			CVolumeLimitGoodsContainer* pSubPack=pPlayer->getSubpackContainer()->GetSubpack(i-1)->pSubpackContainer;
			m_ContainerInfo[i].dwContainerVolume=pSubPack->GetVolume();
			m_ContainerInfo[i].pCellInfo=new tagCellInfo[pSubPack->GetVolume()];
			FillCell(i,pSubPack);
		}
		else
		{
			//玩家没有这个容器
		}
	}

	m_ContainerNum = 6;
}

VOID CTestContainer::AddContainer( DWORD dwContainerID, CVolumeLimitGoodsContainer* pVolumeContainer )
{
	if( pVolumeContainer == NULL || m_ContainerNum >= CONTAINER_NUM )
	{
		return;
	}

	m_ContainerInfo[m_ContainerNum].dwContainerId = dwContainerID;
	m_ContainerInfo[m_ContainerNum].dwContainerVolume = pVolumeContainer->GetVolume();
	m_ContainerInfo[m_ContainerNum].pCellInfo = new tagCellInfo[ pVolumeContainer->GetVolume() ];
	FillCell( m_ContainerNum, pVolumeContainer );

	m_ContainerNum ++;
}

VOID CTestContainer::FillCell(int nIndex,CVolumeLimitGoodsContainer* pVolumeContainer)
{
	if(!pVolumeContainer)
		return;
	if(pVolumeContainer->GetVolume()!=m_ContainerInfo[nIndex].dwContainerVolume)
	{
		//容器VOLUME不等于数组大小
		return;
	}
	vector<CGUID>& vecCellGuid=pVolumeContainer->GetCellGuid();
	tagCellInfo stCellInfo;
	for(int i=0;i<m_ContainerInfo[nIndex].dwContainerVolume;i++)
	{
		ZeroMemory(&stCellInfo,sizeof(tagCellInfo));
		ZeroMemory(&m_ContainerInfo[nIndex].pCellInfo[i],sizeof(tagCellInfo));
		if(vecCellGuid[i]!=NULL_GUID)
		{
#ifdef _GOODSLOG1_
			char pszGoodsLog[1024]="";			
			char srcGoodsGUID[50]="";
			vecCellGuid[i].tostring(srcGoodsGUID);					
			_snprintf(pszGoodsLog,1024,"[GUID2][容器ID:%d][容器位置:%d][物品GUID:%s]",
				m_ContainerInfo[nIndex].dwContainerId,i,srcGoodsGUID);
			CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
			CGoods* pGoods=dynamic_cast<CGoods*>(pVolumeContainer->Find(vecCellGuid[i]));
			if(pGoods)
			{
				//找到物品填充信息
				m_ContainerInfo[nIndex].pCellInfo[i].dwGoodsId=pGoods->GetBasePropertiesIndex();
				m_ContainerInfo[nIndex].pCellInfo[i].dwGoodsMaxNum=pGoods->GetMaxStackNumber();
				m_ContainerInfo[nIndex].pCellInfo[i].dwGoodsNum=pGoods->GetAmount();
				m_ContainerInfo[nIndex].pCellInfo[i].goodsGuid=pGoods->GetExID();
			}			
		}
	}
}

//Clear()函数只是把数据恢复到初始状态
VOID CTestContainer::Clear()
{
	for(int i=0;i<m_ContainerNum;i++)
	{
		m_ContainerInfo[i].dwContainerId=0;
		for(int j=0;j<m_ContainerInfo[i].dwContainerVolume;j++)
		{
			m_ContainerInfo[i].pCellInfo[j].dwGoodsId=0;
			m_ContainerInfo[i].pCellInfo[j].dwGoodsMaxNum=0;
			m_ContainerInfo[i].pCellInfo[j].dwGoodsNum=0;
			m_ContainerInfo[i].pCellInfo[j].goodsGuid=NULL_GUID;
		}
		m_ContainerInfo[i].dwContainerVolume=0;		
	}
}

//删除物品测试
BOOL CTestContainer::DelTest(vector<CGoods*>& vecGoodsOut)
{
	for(int i=0;i<vecGoodsOut.size();i++)
	{
		if(vecGoodsOut[i])
		{
			//清除一个物品数据
			BOOL bSucceed=ClearOneGoods(vecGoodsOut[i]);
			if(!bSucceed)
				return FALSE;
		}
	}
	return TRUE;
}

BOOL CTestContainer::DelTest(vector<tagTestArg_ID_Num>& vecArg,vector<tagDelTestResult>* vecResult)
{
	tagDelTestResult tagResult;
	for (int i=0;i<vecArg.size();i++)
	{
		DWORD dwGoodID = vecArg[i]._dwGoodsID;
		DWORD dwNum = vecArg[i]._dwNum;
		for(int m=0;m<m_ContainerNum;m++)
		{
			if(m_ContainerInfo[m].dwContainerVolume && m_ContainerInfo[m].pCellInfo)
			{
				//玩家有这个容器
				for(int n=0;n<m_ContainerInfo[m].dwContainerVolume;n++)
				{
					//继续查找物品
					if (dwNum > 0)
					{
						//有此物品
						if(m_ContainerInfo[m].pCellInfo[n].dwGoodsId == dwGoodID)
						{
							
							tagResult.dwContainerId = m_ContainerInfo[m].dwContainerId;
							tagResult.dwPos = n;	

							if (m_ContainerInfo[m].pCellInfo[n].dwGoodsNum <= dwNum)
							{
								dwNum -= m_ContainerInfo[m].pCellInfo[n].dwGoodsNum;
								tagResult.dwNum = m_ContainerInfo[m].pCellInfo[n].dwGoodsNum;
								tagResult.eOperType = CS2CContainerObjectMove::OT_DELETE_OBJECT;
								ClearCell(m,n);
							}
							else
							{
								tagResult.dwNum = dwNum;
								dwNum = 0;
								tagResult.eOperType = CS2CContainerObjectMove::OT_CHANGE_AMOUNT;
							}
							vecResult->push_back(tagResult);
						}
					}
					else
						break;
				}
			}
			//已找到足够的物品
			if (0 == dwNum)
				break;	
		}//for
		//物品数量不够
		if (dwNum !=0 )
		{
			return FALSE;
		}
		//测试完毕
		if ( i+1 == vecArg.size() )
		{
			return TRUE;
		}
	}//for
	return FALSE;
}

//添加物品测试
BOOL CTestContainer::AddTest(vector<CGoods*>& vecGoodsIn,vector<tagTestResult>* vecResult)
{	
	if(vecResult)
		vecResult->clear();
	for(int i=0;i<vecGoodsIn.size();i++)
	{
		tagTestResult stRet;
		ZeroMemory(&stRet,sizeof(tagTestResult));		
		if(vecGoodsIn[i])
		{	
			//是否能成功添加该物品
			BOOL bSucced=FillOneGoods(vecGoodsIn[i],&stRet);
			if(!bSucced)
			{
				//不能为该物品找到一个位置
				return FALSE;
			}
		}
		if(vecResult)
			vecResult->push_back(stRet);
	}
	return TRUE;
}

BOOL CTestContainer::AddTest(CGoods* pGoods,vector<tagTestResult>* vecResult)
{
	if(!pGoods)
		return FALSE;
	if(vecResult)
		vecResult->clear();
	tagTestResult stRet;
	ZeroMemory(&stRet,sizeof(tagTestResult));
	BOOL bSucced=FillOneGoods(pGoods,&stRet);
	if(bSucced)
	{
		if(vecResult)
			vecResult->push_back(stRet);
		return TRUE;
	}
	return FALSE;
}



VOID CTestContainer::ClearCell(int nIndex,int nPos)
{
	if(nIndex<0 || nIndex>=m_ContainerNum)
		return;
	if(nPos<0 || nPos>=m_ContainerInfo[nIndex].dwContainerVolume)
		return;
	if(m_ContainerInfo[nIndex].pCellInfo)
	{
		m_ContainerInfo[nIndex].pCellInfo[nPos].dwGoodsId=0;
		m_ContainerInfo[nIndex].pCellInfo[nPos].dwGoodsMaxNum=0;
		m_ContainerInfo[nIndex].pCellInfo[nPos].dwGoodsNum=0;
		m_ContainerInfo[nIndex].pCellInfo[nPos].goodsGuid=NULL_GUID;
	}
}

BOOL CTestContainer::ClearOneGoods(CGoods* pGoods)
{
	if(!pGoods)
		return FALSE;

	for(int i=0;i<m_ContainerNum;i++)
	{
		if(m_ContainerInfo[i].dwContainerVolume && m_ContainerInfo[i].pCellInfo)
		{
			//玩家有这个容器
			for(int j=0;j<m_ContainerInfo[i].dwContainerVolume;j++)
			{
				if(m_ContainerInfo[i].pCellInfo[j].dwGoodsId==pGoods->GetBasePropertiesIndex() && 
					m_ContainerInfo[i].pCellInfo[j].goodsGuid==pGoods->GetExID())
				{
					//清除格子数据
					ClearCell(i,j);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL CTestContainer::FillOneGoods(CGoods* pGoods,tagTestResult* pResult)
{
	if(!pGoods || !pResult)
		return FALSE;
	DWORD dwMaxStack=pGoods->GetMaxStackNumber();
	//第一个空位的容器ID
	int nContainerIndex=-1;
	//第一个空位的位置
	int nPos=-1;
	
	for(int i=0;i<m_ContainerNum;i++)
	{
		if(m_ContainerInfo[i].dwContainerVolume && m_ContainerInfo[i].pCellInfo)
		{
			//玩家有这个容器
			for(int j=0;j<m_ContainerInfo[i].dwContainerVolume;j++)
			{
				
				if(nContainerIndex==-1 && m_ContainerInfo[i].pCellInfo[j].dwGoodsId==0)
				{
					//记下空格位置以便之后无法查到其它位置时不用重新查找
					nContainerIndex=i;
					nPos=j;
				}
				
				if(dwMaxStack>1)
				{
					//叠加物品
					if(m_ContainerInfo[i].pCellInfo[j].dwGoodsId==pGoods->GetBasePropertiesIndex() && 
						(m_ContainerInfo[i].pCellInfo[j].dwGoodsNum+pGoods->GetAmount())<=dwMaxStack)
					{
						//可以叠加
						m_ContainerInfo[i].pCellInfo[j].dwGoodsNum+=pGoods->GetAmount();
						pResult->dwContainerId=m_ContainerInfo[i].dwContainerId;
						pResult->dwPos=j;
						pResult->eOperType=CS2CContainerObjectMove::OT_CHANGE_AMOUNT;
						return TRUE;
					}
				}
				else
				{
					//不能叠加物品
					if(m_ContainerInfo[i].pCellInfo[j].dwGoodsId==0)
					{
						m_ContainerInfo[i].pCellInfo[j].dwGoodsId=pGoods->GetBasePropertiesIndex();
						m_ContainerInfo[i].pCellInfo[j].dwGoodsMaxNum=1;
						m_ContainerInfo[i].pCellInfo[j].dwGoodsNum=1;
						m_ContainerInfo[i].pCellInfo[j].goodsGuid=pGoods->GetExID();
						pResult->dwContainerId=m_ContainerInfo[i].dwContainerId;
						pResult->dwPos=j;
						pResult->eOperType=CS2CContainerObjectMove::OT_NEW_OBJECT;
						return TRUE;
					}
				}
			}
		}
		else
		{
			//玩家没有这个容器
		}
	}

	//直接添加到空位置
	if(dwMaxStack>1 && nContainerIndex>=0 && nPos>=0)
	{
		m_ContainerInfo[nContainerIndex].pCellInfo[nPos].dwGoodsId=pGoods->GetBasePropertiesIndex();
		m_ContainerInfo[nContainerIndex].pCellInfo[nPos].dwGoodsMaxNum=dwMaxStack;
		m_ContainerInfo[nContainerIndex].pCellInfo[nPos].dwGoodsNum=pGoods->GetAmount();
		m_ContainerInfo[nContainerIndex].pCellInfo[nPos].goodsGuid=pGoods->GetExID();
		pResult->dwContainerId=m_ContainerInfo[nContainerIndex].dwContainerId;
		pResult->dwPos=nPos;
		pResult->eOperType=CS2CContainerObjectMove::OT_NEW_OBJECT;
		return TRUE;
	}
	return FALSE;
}

VOID CTestContainer::Release()
{
	for(int i=0;i<m_ContainerNum;i++)
	{			
		SAFE_DELETE_ARRAY(m_ContainerInfo[i].pCellInfo);	
	}
}

