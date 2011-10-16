// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "../Goods/CGoods.h"
#include "../Goods/CGoodsFactory.h"
#include "CVolumeLimitGoodsContainer.h"
#include "CContainer.h"
#include "../MessagePackage/CS2CContainerObjectMove.h"

#include "../../../../Public/Setup/GoodsSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CVolumeLimitGoodsContainer::CVolumeLimitGoodsContainer()
{
	m_dwVolume=0;
	m_vCells.clear();
}

CVolumeLimitGoodsContainer::~CVolumeLimitGoodsContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

void CVolumeLimitGoodsContainer::SetContainerVolume(DWORD dwVolume)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	Release();
	m_dwVolume=dwVolume;
	m_vCells.resize(dwVolume, NULL_GUID );
	SetGoodsAmountLimit(dwVolume);
}

void CVolumeLimitGoodsContainer::ChangeContainerVolume(DWORD dwVolume)
{
	m_dwVolume=dwVolume;
	m_vCells.resize(dwVolume, NULL_GUID );
	SetGoodsAmountLimit(dwVolume);
}


DWORD CVolumeLimitGoodsContainer::GetWidth()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return 5;
}


DWORD CVolumeLimitGoodsContainer::GetHeight()
{	
	double dVal=((double)m_dwVolume)/((double)5);
	dVal=::ceil(dVal);	
	DWORD dwHeight=(DWORD)dVal;
	return dwHeight;
}

DWORD CVolumeLimitGoodsContainer::GetVolume()
{
	return m_dwVolume;
}

//##ModelId=41AFF60100B6
bool CVolumeLimitGoodsContainer::FindPositionForGoods(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pGoods == NULL ) return false;
	goodsIt it=m_mGoods.find(pGoods->GetExID());
	if(it!=m_mGoods.end())
		return false;
	//如果是可叠加物品
	if( pGoods -> GetMaxStackNumber() > 1 )
	{
		goodsIt it = m_mGoods.begin();
		for( ; it != m_mGoods.end(); it ++ )
		{
			if( it -> second )
			{
				if( it -> second -> GetBasePropertiesIndex() == 
					pGoods -> GetBasePropertiesIndex() )
				{
					//可以再叠加
					if( it -> second -> GetAmount() + pGoods -> GetAmount() <=
						pGoods -> GetMaxStackNumber() )
					{
						if( QueryGoodsPosition(it -> second, dwPosition) )
						{
							return true;
						}
					}
				}
			}
		}
	}

	for( size_t i = 0; i < m_vCells.size(); i ++ )
	{
		if( m_vCells[i] == CGUID::GUID_INVALID )//该格上必须是空的
		{
			if( IsSpaceEnough( pGoods, static_cast<DWORD>(i) ) )
			{
				dwPosition = static_cast<DWORD>( i );
				return true;
			}
		}
	}
	return false;
}


//查找一个针对该物品的空区间,Joe
bool CVolumeLimitGoodsContainer::FindEmptySpaceForGoods(CGoods* pGoods, DWORD& dwPosition)
{
	for( size_t i = 0; i < m_vCells.size(); i ++ )
	{
		if( m_vCells[i] == CGUID::GUID_INVALID )
		{
			if( IsSpaceEnough( pGoods, static_cast<DWORD>(i) ) )
			{
				dwPosition = static_cast<DWORD>( i );
				return true;
			}
		}
	}
	return false;
}

DWORD	CVolumeLimitGoodsContainer::DelGoods(eGoodsBaseType baseType,DWORD dwWeaponType,DWORD dwNum,DWORD dwPeiVal,const CGUID& playerId)
{
	DWORD dwDelNum=0;
	vector<CGoods*> vecGoodsRemove;
	goodsIt it=m_mGoods.begin();
	for(;it!=m_mGoods.end();it++)
	{
		
		if(it->second->GetGoodsBaseType()==baseType)
		{
			if(dwWeaponType!=0)
			{
				DWORD dwGoodsWeaponType=it->second->GetAddonPropertyValues(GAP_WEAPON_CATEGORY,1);
				if(dwWeaponType==dwGoodsWeaponType)
				{
					vecGoodsRemove.push_back(it->second);
					dwDelNum++;
				}
			}
			else
			{
				vecGoodsRemove.push_back(it->second);
				dwDelNum++;
			}
		}
		if(dwDelNum==dwNum)
			break;
	}
	vector<CGoods*>::iterator removeIt=vecGoodsRemove.begin();
	for(;removeIt!=vecGoodsRemove.end();removeIt++)
	{
		CS2CContainerObjectMove comMsg;
		if(Remove(*removeIt,&comMsg))
		{
			comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
			comMsg.SetSourceContainerExtendID(dwPeiVal);
			comMsg.Send( playerId);			
#ifdef _GOODSLOG1_
			if(*removeIt)
			{
				char pszGoodsLog[1024]="";			
				char srcGoodsGUID[50]="";
				(*removeIt)->GetExID().tostring(srcGoodsGUID);					
				_snprintf(pszGoodsLog,1024,"[物品回收:CVolumeLimitGoodsContainer::DelGoods(eGoodsBaseType baseType][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
					(*removeIt)->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName((*removeIt)->GetBasePropertiesIndex()),srcGoodsGUID,(*removeIt)->GetAmount());
				CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
			}			
#endif	
			CGoodsFactory::GarbageCollect(&(*removeIt),99);
		}
	}
	return dwDelNum;
}

DWORD CVolumeLimitGoodsContainer::DelGoods(DWORD dwGoodsId,int num,vector<CGoodsBaseProperties::tagGoodsDelInfo>* pVec,bool bDelete)
{	
	DWORD dwTargetNum=num;
	vector<CGoods*> vecGoods;
	goodsIt it=m_mGoods.begin();
	for(;it!=m_mGoods.end();it++)
	{
		vecGoods.push_back(it->second);
	}
	for(int i=0;i<(int)vecGoods.size();i++)
	{
		if(vecGoods[i]->GetBasePropertiesIndex()==dwGoodsId)
		{
			DWORD dwPos=0;
			if(QueryGoodsPosition(vecGoods[i],dwPos))
			{		
				DWORD dwGoodsAmount=vecGoods[i]->GetAmount();
				DWORD dwDelAmount=0;
				if(dwGoodsAmount>=dwTargetNum)
				{
					//OK 要求的数量全部都扣除完了
					CBaseObject* pBaseObj=Remove(dwPos,dwTargetNum);
					CGoods* pRemovedGoods=dynamic_cast<CGoods*>(pBaseObj);
					if(pVec)
					{
						CGoodsBaseProperties::tagGoodsDelInfo stInfo;
						stInfo.dwPos=dwPos;
						stInfo.dwNum=dwTargetNum;
						pVec->push_back(stInfo);
					}
					if(pRemovedGoods && dwGoodsAmount>dwTargetNum)
					{
						if(pRemovedGoods!=vecGoods[i])
							CGoodsFactory::GarbageCollect(&pRemovedGoods,14);
					}
					if(dwGoodsAmount==dwTargetNum && bDelete)
					{
						CGoodsFactory::GarbageCollect(&(vecGoods[i]),709);
					}
					return num;
				}
				else
				{
					CBaseObject* pBaseObj=Remove(dwPos,dwGoodsAmount);
					if(pVec)
					{
						CGoodsBaseProperties::tagGoodsDelInfo stInfo;
						stInfo.dwPos=dwPos;
						stInfo.dwNum=dwGoodsAmount;
						pVec->push_back(stInfo);
					}
					dwTargetNum-=dwGoodsAmount;
					CGoods* pDelGoods=dynamic_cast<CGoods*>(pBaseObj);
					if(pDelGoods && bDelete)
					{
						CGoodsFactory::GarbageCollect(&pDelGoods,709);
					}
				}
			}
		}		
	}
	
	return num-dwTargetNum;
}

DWORD CVolumeLimitGoodsContainer::GetEmptySpaceNum()
{
	DWORD dwRet=0;
	for(size_t i=0;i<m_vCells.size();i++)
	{
		if(m_vCells[i]==CGUID::GUID_INVALID)
		{
			dwRet++;
		}
	}
	return dwRet;
}

//##ModelId=41AFCFCF00D1
bool CVolumeLimitGoodsContainer::IsSpaceEnough(CGoods* pGoods, DWORD dwFirstCell)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	
	if( pGoods == NULL || dwFirstCell >= m_vCells.size() || IsFull() )
	{
		return false;
	}
	if(m_vCells[dwFirstCell]!=CGUID::GUID_INVALID)
		return false;
	return true;
}

//##ModelId=41B00B310242
bool CVolumeLimitGoodsContainer::IsSpaceEnough(vector<CGoods*> vGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = true;

	CVolumeLimitGoodsContainer cTestContainer;
	cTestContainer.SetContainerMode( GCM_TEST );
	Clone( &cTestContainer );

	for( size_t i = 0; i < vGoods.size(); i ++ )
	{
		if( cTestContainer.Add( vGoods[i], NULL ) == false )
		{
			bResult = false;
			break;
		}
	}

	//##退出前先清理掉所有的数据，但是不能删除物品指针
	cTestContainer.Clear();

	return bResult;
}

//! 得到可以装下的物品数量
LONG CVolumeLimitGoodsContainer::GetCanPutNum(vector<CGoods*> vGoods)
{
	LONG lResult = 0;

	CVolumeLimitGoodsContainer cTestContainer;
	cTestContainer.SetContainerMode( GCM_TEST );
	Clone( &cTestContainer );

	for( size_t i = 0; i < vGoods.size(); i ++ )
	{
		if( cTestContainer.Add( vGoods[i], NULL ) != false )
		{
			++lResult;
		}
	}

	//! 退出前先清理掉所有的数据，但是不能删除物品指针
	cTestContainer.Clear();

	return lResult;
}
//##ModelId=41BFA5430213
bool CVolumeLimitGoodsContainer::Swap(DWORD dwFirstCell, CGoods* pIn, CGoods** ppOut, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	
	bool bResult = false;

	if( pIn && ppOut )
	{
		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(
			pIn -> GetBasePropertiesIndex() );
		if( pProperties )
		{
			DWORD dwX = dwFirstCell % GetWidth();
			DWORD dwY = dwFirstCell / GetWidth();

			//##必须在容器的边界内
			if( GetWidth()  - dwX >= 0 &&
				GetHeight() - dwY >= 0)
			{
				//##他所覆盖的区域必须最多只有一个物品
				if( GetGoodsAmountInSquare( dwFirstCell, 1,1) <= 1 )
				{
					//##找到这个物品,并且找到他的位置
					CGoods* pOriginalGoods = GetFirstGoodsInSquare( dwFirstCell, 1,1);
                    DWORD dwOriginalGoodsPosition = 0;
					if( pOriginalGoods && QueryGoodsPosition(pOriginalGoods, dwOriginalGoodsPosition) )
					{
						if( Remove(pOriginalGoods) )
						{
							if( Add(dwFirstCell, pIn, NULL) )
							{
								CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
								if( pMsg )
								{
									pMsg -> SetSourceObject( pIn -> GetType(), pIn -> GetExID() );
									pMsg -> SetDestinationObject( pOriginalGoods -> GetType(), pOriginalGoods -> GetExID() );
								}
								*ppOut = pOriginalGoods;
								bResult = true;
							}
							else
							{
								//##回滚
								//if( Add(dwOriginalGoodsPosition, pOriginalGoods, NULL) == false )
								//{
									//##如果失败,则删除该物品.
#ifdef _GOODSLOG1_
									char pszGoodsLog[1024]="";			
									char srcGoodsGUID[50]="";
									pOriginalGoods->GetExID().tostring(srcGoodsGUID);					
									_snprintf(pszGoodsLog,1024,"[交换物品失败][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
										pOriginalGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pOriginalGoods->GetBasePropertiesIndex()),srcGoodsGUID,pOriginalGoods->GetAmount());
									CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
									//GoodsSetup::GarbageCollect( &pOriginalGoods,11 );
								//}
							}
						}
					}
				}
			}
		}
	}
	return bResult;
	
}

void CVolumeLimitGoodsContainer::ReFillCell()
{
	int size=(int)m_vCells.size();
	m_vCells.clear();
	m_vCells.resize(size);
	goodsIt it = m_mGoods.begin();
	int pos=0;
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second )
		{
			CGUID guid = it->second->GetExID();
			m_vCells[pos]=guid;
		    pos++;
		}
	}
}

//##ModelId=41BFA467029F
CGoods* CVolumeLimitGoodsContainer::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	if( dwPosition >= m_vCells.size() )
	{
		return NULL;
	}

	if( m_vCells[dwPosition] == CGUID::GUID_INVALID )
	{
		return NULL;
	}

	return dynamic_cast<CGoods*>( Find(m_vCells[dwPosition]) );
}

//##ModelId=41BFA407002E
bool CVolumeLimitGoodsContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pGoods == NULL )
	{
		return false;
	}

	//DWORD dwHashValue = CContainer::CalculateHashValue( pGoods -> GetType(), 
	//	pGoods -> GetID() );

	return QueryGoodsPosition( pGoods->GetExID(), dwPosition );
}

//##ModelId=41BFA36A0138
bool CVolumeLimitGoodsContainer::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods == NULL )
	{
		return false;
	}

	if(pGoods->GetBasePropertiesIndex()==GoodsSetup::GetSilverIndex() || 
		pGoods->GetBasePropertiesIndex()==GoodsSetup::GetGoldCoinIndex())
	{
		return false;
	}

	DWORD dwFirstCell = 0;
	if( FindPositionForGoods(pGoods, dwFirstCell) == false )
	{
		return false;
	}

	return Add( dwFirstCell, pGoods, pPreviousContainer, pVoid );
}

bool CVolumeLimitGoodsContainer::Add(CGoods* pGoods,vector<BYTE>* pData,DWORD dwContainerId)
{
	if(!pGoods)
		return false;

	if(pGoods->GetBasePropertiesIndex()==GoodsSetup::GetSilverIndex() || 
		pGoods->GetBasePropertiesIndex()==GoodsSetup::GetGoldCoinIndex())
	{
		return false;
	}

	DWORD dwPos=0;
	if(FindPositionForGoods(pGoods,dwPos)==false)
	{
		//找不到一个位置添加该物品
		return false;
	}
	CGoods* pPackGoods=GetGoods(dwPos);
	if(pPackGoods)
	{
		//只是改变数量
		if(pData)
		{
			_AddToByteArray(pData,(BYTE)CS2CContainerObjectMove::OT_CHANGE_AMOUNT);
			_AddToByteArray(pData,dwContainerId);
			_AddToByteArray(pData,dwPos);
			_AddToByteArray(pData,(DWORD)pGoods->GetAmount());
		}
	}
	else 
	{
		if(pData)
		{
			//添加新物品
			_AddToByteArray(pData,(BYTE)CS2CContainerObjectMove::OT_NEW_OBJECT);
			_AddToByteArray(pData,dwContainerId);
			pGoods->SerializeForOldClient(pData);
			_AddToByteArray(pData,dwPos);			
		}		
	}
	return Add( dwPos, pGoods, NULL, NULL );
}

//##ModelId=41BFA3A00157
void CVolumeLimitGoodsContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CAmountLimitGoodsContainer::Clear( pVoid );
	m_vCells.clear();
	m_vCells.resize(m_dwVolume, NULL_GUID );
}

//##ModelId=41BFA3A90196
void CVolumeLimitGoodsContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CAmountLimitGoodsContainer::Release();
	m_dwVolume=0;
	m_vCells.clear();
}

//##ModelId=41BFA55101E4
bool CVolumeLimitGoodsContainer::Clone(CGoodsContainer* pObject)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( CAmountLimitGoodsContainer::Clone(pObject) )
	{
		CVolumeLimitGoodsContainer* pContainer = 
			dynamic_cast<CVolumeLimitGoodsContainer*>( pObject );
		if( pContainer == NULL )
		{
			return false;
		}

		//pContainer -> m_dwWidth		= this -> m_dwWidth;
		//pContainer -> m_dwHeight	= this -> m_dwHeight;
		pContainer->m_dwVolume=this->m_dwVolume;
		pContainer -> m_vCells		= this -> m_vCells;
		return true;
	}
	return false;
}

void CVolumeLimitGoodsContainer::CloneEx(CVolumeLimitGoodsContainer* pContainer,vector<CGoods*>* pGoodsVec)
{
	pContainer->m_dwVolume=this->m_dwVolume;
	pContainer->SetGoodsAmountLimit(GetGoodsAmountLimit());
	pContainer->Clear();
	goodsIt goodsIt=m_mGoods.begin();
	for(;goodsIt!=m_mGoods.end();goodsIt++)
	{
		if(goodsIt->second->GetAmount()>0)
		{
			vector<CGoods*> vecGoods;
			CGoodsFactory::CreateGoods(goodsIt->second->GetBasePropertiesIndex(),
				goodsIt->second->GetAmount(),vecGoods,18);
			DWORD dwPos=0;

			if(vecGoods.size()>0)
			{
				QueryGoodsPosition(goodsIt->first,dwPos);
				pContainer->Add(dwPos,vecGoods[0],NULL,NULL);	
				if(pGoodsVec)
					pGoodsVec->push_back(vecGoods[0]);
			}
		}
		else
		{
			Log4c::Trace(ROOT_MODULE,"CloneEx Amount=0 : %s",GoodsSetup::QueryGoodsName(goodsIt->second->GetBasePropertiesIndex()));
		}
	}
}


//##ModelId=41B6730200EA
void CVolumeLimitGoodsContainer::GetGoodsInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight, vector<CGoods*> vGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( dwFirstCell < GetGoodsAmountLimit() )
	{
		DWORD dwY = dwFirstCell / GetWidth();
		DWORD dwX = dwFirstCell % GetWidth();
		for( DWORD i = 0; i < dwHeight; i ++ )
		{
			if( i + dwY >= GetHeight() )
			{
				continue;
			}
			for( DWORD j = 0; j < dwWidth; j ++ )
			{
				if( j + dwX >= GetWidth() )
				{
					continue;
				}
				
				//DWORD dwHashValue = m_vCells[(dwY + i) * GetContainerWidth() + (dwX + j)];
				CGUID guid = m_vCells[(dwY + i) * GetWidth() + (dwX + j)];
				if( guid != CGUID::GUID_INVALID )
				{
					CGoods* pGoods = dynamic_cast<CGoods*>( Find(guid) );
					if( pGoods )
					{
						bool bAlreadyAdded	= false;

						vector<CGoods*>::iterator it = vGoods.begin();
						for( ; it != vGoods.end(); it ++ )
						{
							if( *it == pGoods )
							{
								bAlreadyAdded = true;
								break;
							}
						}
						if( bAlreadyAdded == false )
						{
							vGoods.push_back( pGoods );
						}
					}
				}
			}
		}
	}
}

//##ModelId=41B673BE0261
CGoods* CVolumeLimitGoodsContainer::GetFirstGoodsInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( dwFirstCell < GetGoodsAmountLimit() )
	{
		DWORD dwY		= dwFirstCell / GetWidth();
		DWORD dwX		= dwFirstCell % GetWidth();
		bool  bFounded	= false;
		for( DWORD i = 0; i < dwHeight; i ++ )
		{
			if( i + dwY >= GetHeight() )
			{
				continue;
			}
			if( bFounded )
			{
				break;
			}
			for( DWORD j = 0; j < dwWidth; j ++ )
			{
				if( j + dwX >= GetWidth() )
				{
					continue;
				}
				//DWORD dwHashValue = m_vCells[(dwY + i) * GetContainerWidth() + (dwX + j)];
				CGUID guid = m_vCells[(dwY + i) * GetWidth() + (dwX + j)];
				if( guid != CGUID::GUID_INVALID )
				{
					pResult = dynamic_cast<CGoods*>( Find(guid) );
					if( pResult )
					{
						bFounded = true;
					}
				}
			}
		}
	}
	return pResult;
}

//##ModelId=41B6671E02BF
DWORD CVolumeLimitGoodsContainer::GetGoodsAmountInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.

#ifdef _DEBUG
	CHAR szDebug[1024];
	for( size_t i = 0; i < m_vCells.size(); i ++ )
	{
		char strGuid[40] = "";
		m_vCells[i].tostring(strGuid);
		_snprintf( szDebug, 40, "Cell %d: %s ", i,strGuid  );
		OutputDebugString( szDebug );
	}
#endif

	vector<CGUID> vGoods;
	if( dwFirstCell < GetGoodsAmountLimit() )
	{
		DWORD dwY = dwFirstCell / GetWidth();
		DWORD dwX = dwFirstCell % GetWidth();
		for( DWORD i = 0; i < dwHeight; i ++ )
		{
			if( i + dwY >= GetHeight() )
			{
				continue;
			}
			for( DWORD j = 0; j < dwWidth; j ++ )
			{
				if( j + dwX >= GetWidth() )
				{
					continue;
				}
				
				//DWORD dwHashValue = m_vCells[(dwY + i) * GetContainerWidth() + (dwX + j)];
				CGUID guid = m_vCells[(dwY + i) * GetWidth() + (dwX + j)];
				if( guid != CGUID::GUID_INVALID )
				{
					bool bAlreadyAdded	= false;

					vector<CGUID>::iterator it = vGoods.begin();
					for( ; it != vGoods.end(); it ++ )
					{
						if( *it == guid )
						{
							bAlreadyAdded = true;
							break;
						}
					}
					if( bAlreadyAdded == false )
					{
						vGoods.push_back( guid );
					}
				}
			}
		}
	}
	return static_cast<DWORD>( vGoods.size() );
}

CBaseObject* CVolumeLimitGoodsContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	CGoods* pGoods = dynamic_cast<CGoods*>( CAmountLimitGoodsContainer::Remove(guid, pVoid) );
	if( pGoods == NULL )
	{
		return NULL;
	}

	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( 
		pGoods -> GetBasePropertiesIndex() );
	if( pProperties == NULL )
	{
		return NULL;
	}

	DWORD dwFirstCell = 0;
	if( QueryGoodsPosition( guid, dwFirstCell ) == false )
	{
		return NULL;
	}
	m_vCells[dwFirstCell]=CGUID::GUID_INVALID;
	
	return pGoods;
}

bool CVolumeLimitGoodsContainer::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{	
	bool bResult = false;

	//if( IsFull() == false )
	//{
		if( pObject )
		{
			if(pObject->GetBasePropertiesIndex()==GoodsSetup::GetSilverIndex() || 
				pObject->GetBasePropertiesIndex()==GoodsSetup::GetGoldCoinIndex())
			{
				return false;
			}

			//取得pGoods的GUID，如果背包中已经有同一GUID物品不能添加
			goodsIt it=m_mGoods.find(pObject->GetExID());
			if(it!=m_mGoods.end())
				return false;



			CGoods* pGoods = GetGoods( dwPosition );
			if( pGoods )
			{
				bResult = CGoodsContainer::Add( dwPosition, pObject, pPreviousContainer, pVoid );
				/*
				if( pGoods -> GetBasePropertiesIndex() ==
					pObject -> GetBasePropertiesIndex() &&
					pGoods -> GetMaxStackNumber() > 1 )
				{
					long nRemainedSlot = pGoods -> GetMaxStackNumber() -
						pGoods -> GetAmount();
					long lAddedAmount = pObject -> GetAmount();
					//##如果加入的数量少于等于这个，则直接加入。
					if(lAddedAmount>=0 && lAddedAmount <= nRemainedSlot )
					{
						//##如果是正常模式，就删除该物品。如果是测试模式，就不删除源物品
						if( GetContainerMode() == GCM_NORMAL )
						{								
							pObject->SetAmount(pGoods->GetAmount()+lAddedAmount);
							pObject->SetExID(pGoods->GetExID());
							m_mGoods[pGoods->GetExID()]=pObject;
							GoodsSetup::GarbageCollect(&pGoods,12);
						}

						for( size_t i = 0; i < m_vListeners.size(); i ++ )
						{
							if( m_vListeners[i] )
							{
								//##该合并消息必须设置目标物品
								m_vListeners[i] -> OnObjectAdded( this, pObject, lAddedAmount, pVoid );
							}
						}
						bResult = true;
					}					
				}*/
			}
			else 
			{
				if( IsSpaceEnough(pObject, dwPosition) )
				{
					CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( 
						pObject -> GetBasePropertiesIndex() );
					if( pProperties )
					{
						//##暂时在这儿添加物品。
						//DWORD dwHashValue = CContainer::CalculateHashValue( pObject -> GetType(), pObject -> GetID() );
						CGUID guid = pObject->GetExID();
						m_mGoods[guid] = pObject;
						//##暂时屏蔽一下，否则QueryGoodsPosition会出错。
						//if( CAmountLimitGoodsContainer::Add(pObject, pVoid)  )	
						//{
						DWORD dwY			= dwPosition / GetWidth();
						DWORD dwX			= dwPosition % GetWidth();
						//DWORD dwHashValue	= CContainer::CalculateHashValue( pObject -> GetType(), pObject -> GetID() );
						
						
						m_vCells[dwY*GetWidth() + dwX] = guid;
						

						for( size_t i = 0; i < m_vListeners.size(); i ++ )
						{
							if( m_vListeners[i] )
							{
								m_vListeners[i] -> OnObjectAdded( this, pObject, pObject -> GetAmount(), pVoid );
							}
						}
						bResult = true;
						//}
					}
				}
			}
		}
	//}
	return bResult;
}

//##ModelId=41BFA5EA03B9
void CVolumeLimitGoodsContainer::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CAmountLimitGoodsContainer::AI();
}

//##ModelId=41BFA3BB001F
bool CVolumeLimitGoodsContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = true;
	if( CAmountLimitGoodsContainer::IsFull() == false )
	{
		itCell it = m_vCells.begin();
		for( ; it < m_vCells.end(); it ++ )
		{
			if( *it == CGUID::GUID_INVALID )
			{
				bResult = false;
				break;
			}
		}
	}
	return bResult;
}

void CVolumeLimitGoodsContainer::ClearGoods(DWORD dwPos)
{
	if(dwPos>=0 && dwPos<m_vCells.size())
	{
		m_mGoods.erase(m_vCells[dwPos]);
		m_vCells[dwPos]=CGUID::GUID_INVALID;
	}
}

//##ModelId=41BFA40F0109
bool CVolumeLimitGoodsContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	for( size_t i = 0; i < m_vCells.size(); i ++ )
	{
		if( m_vCells[i] == guid )
		{
			dwPosition = static_cast<DWORD>( i );
			return true;
		}
	}
	return false;
}

DWORD CVolumeLimitGoodsContainer::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwAmount = 0;
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second )
		{
			//##如果是一个有效的物品
			if( GoodsSetup::QueryGoodsBaseProperties( 
				it -> second -> GetBasePropertiesIndex() ) )
			{
				DWORD dwPosition = 0;
				if( QueryGoodsPosition(it -> second, dwPosition) )
				{
					++ dwAmount;
				}
			}
		}
	}
	return dwAmount;
}

//##ModelId=41BFBAF50213
bool CVolumeLimitGoodsContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer* pGoodsContainer	= dynamic_cast<CGoodsContainer*>( pContainer );
	CGoods* pGoods						= dynamic_cast<CGoods*>( pObj );
	CS2CContainerObjectMove* pMsg		= reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	
	if( pGoodsContainer && pGoods && pMsg )
	{
		DWORD dwPosition = 0;
		if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
		{
			pMsg -> SetDestinationContainer( GetOwnerType(), GetOwnerID(), dwPosition );
			pMsg -> SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
			pMsg -> SetDestinationObjectAmount( pGoods -> GetAmount() );
		}
	}
	return true;
}

//##ModelId=41BFBAED0167
bool CVolumeLimitGoodsContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer* pGoodsContainer	= dynamic_cast<CGoodsContainer*>( pContainer );
	CGoods* pGoods						= dynamic_cast<CGoods*>( pObj );
	CS2CContainerObjectMove* pMsg		= reinterpret_cast<CS2CContainerObjectMove*>( pVoid );

	if( pGoodsContainer && pGoods && pMsg )
	{
		DWORD dwPosition = 0;
		if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
		{
			pMsg -> SetSourceContainer( GetOwnerType(), GetOwnerID(), dwPosition );
			pMsg -> SetSourceObject( pGoods -> GetType(), pGoods -> GetExID() );
			pMsg -> SetSourceObjectAmount( dwAmount );
		}
	}
	return true;
}

void CVolumeLimitGoodsContainer::AddGuid2Msg(CMessage* pMsg)
{
	DWORD dwAmount=GetGoodsAmount();
	pMsg->Add(dwAmount);
	for(WORD i=0;i<dwAmount;i++)
	{
		pMsg->Add(m_vCells[i]);
		pMsg->Add(i);
	}
}

CBaseObject* CVolumeLimitGoodsContainer::Remove( CBaseObject* pObject, LPVOID pVoid )
{
	return CAmountLimitGoodsContainer::Remove( pObject, pVoid );
}

CBaseObject* CVolumeLimitGoodsContainer::Remove( LONG oType, const CGUID& guid , LPVOID pVoid )
{
	return CAmountLimitGoodsContainer::Remove( oType, guid, pVoid );
}

CBaseObject* CVolumeLimitGoodsContainer::Find( CBaseObject* pObj )
{
	return CAmountLimitGoodsContainer::Find( pObj );
}

CBaseObject* CVolumeLimitGoodsContainer::Find( LONG oType, const CGUID& guid  )
{
	return CAmountLimitGoodsContainer::Find( oType, guid );
}

CBaseObject* CVolumeLimitGoodsContainer::Find( const CGUID& guid )
{
	return CAmountLimitGoodsContainer::Find( guid );
}

void CVolumeLimitGoodsContainer::TraversingContainer( CContainerListener* pListener )
{
	return CAmountLimitGoodsContainer::TraversingContainer( pListener );
}


bool CVolumeLimitGoodsContainer::Serialize( DBWriteSet& setWriteDB, bool b  )
{
	bool bResult = false;

	CSerializeContainer scSerializer;
	DWORD dwValidGoodsAmount = GetGoodsAmount();
	setWriteDB.AddToByteArray( dwValidGoodsAmount );
 	scSerializer.m_pDBWriteData = &setWriteDB;
	TraversingContainer( &scSerializer );

	if( dwValidGoodsAmount == scSerializer.m_dwValidGoodsAmount )
	{
		bResult = true;
	}

	return bResult;
}

bool CVolumeLimitGoodsContainer::Unserialize( DBReadSet& setReadDB, bool b )
{
	CVolumeLimitGoodsContainer::Clear();

	DWORD dwNumGoods = setReadDB.GetDwordFromByteArray( );
	for( DWORD i = 0; i < dwNumGoods; i ++ )
	{
		DWORD dwPosition = setReadDB.GetDwordFromByteArray(  );
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB ,19);
		if( pGoods && pGoods->GetAmount()>0)
		{
			CGoodsFactory::UpdateEquipmentAttr(pGoods);
			Add( dwPosition, pGoods, NULL );
		}
	}

	return true;
}


//##ModelId=41C0E6EC0196
CBaseObject* CVolumeLimitGoodsContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	return CGoodsContainer::Remove( dwPosition, dwAmount, pVoid );
}

CVolumeLimitGoodsContainer::CSerializeContainer::CSerializeContainer()
{
	m_dwValidGoodsAmount	= 0;
	m_pDBWriteData			= NULL;
}

CVolumeLimitGoodsContainer::CSerializeContainer::~CSerializeContainer()
{
	m_dwValidGoodsAmount	= 0;
	m_pDBWriteData			= NULL;
}

bool CVolumeLimitGoodsContainer::CSerializeContainer::OnTraversingContainer( CContainer* pContainer, CBaseObject* pObject )
{
	bool bResult	= false;
	CGoods* pGoods	= dynamic_cast<CGoods*>( pObject );

	CVolumeLimitGoodsContainer* pVolumeLimitGoodsContainer	= 
		dynamic_cast<CVolumeLimitGoodsContainer*>( pContainer );

	if( pVolumeLimitGoodsContainer && pGoods && m_pDBWriteData )
	{
		if( GoodsSetup::QueryGoodsBaseProperties( pGoods -> GetBasePropertiesIndex() ) )
		{
			DWORD dwPosition = 0;
			if( pVolumeLimitGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
			{				
				m_pDBWriteData->AddToByteArray(dwPosition );
/*
#ifdef _GOODSLOG_
				if(pGoods)
				{
					char pszGoodsLog[1024]="";			
					char srcGoodsGUID[50]="";
					pGoods->GetExID().tostring(srcGoodsGUID);					
					_snprintf(pszGoodsLog,1024,"[玩家物品编码][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
						pGoods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
					CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
				}			
#endif	
				*/
				pGoods -> Serialize( *m_pDBWriteData );
				m_dwValidGoodsAmount ++;
				bResult = true;
			}
		}
	}

	return bResult;
}