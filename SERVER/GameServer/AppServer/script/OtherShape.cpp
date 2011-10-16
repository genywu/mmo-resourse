//=============================================================================
/**
 *  file: OtherShapeFun.cpp
 *
 *  Brief:定义了其他实体(采集物、建物、城门等)相关的脚本函数
 *			
 *
 *  Authtor:wangqiao
 *	
 *	Create Datae:2008-11-06
 */
//=============================================================================

#include "StdAfx.h"
#include "mmsystem.h"
#include "script.h"
#include "variablelist.h"
#include "../serverregion.h"
#include "../Player.h"
#include "../RgnManager.h"
#include "../Collection.h"
#include "../../../setup/LogSystem.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"


double CScript::CreateCollectionEx(const char* CmdStr, char* retStr)
{
	CGUID ret; ret = NULL_GUID;

	// 返回的guid变量名
	char* szRetGUID = GetStringParam(CmdStr, 0);
	if(!szRetGUID) return -1;
	//##获取lRegionID的编号，如果是0，则设置为当前场景的id
	char* rgnID	= GetStringParam( CmdStr, 9 );
	if(!rgnID)
	{
		M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	CGUID lRegionID;

	if(CScript::GetGeneralVariableList())
		lRegionID = CScript::GetGeneralVariableList()->GetGuidValue(rgnID);
	if(lRegionID == NULL_GUID && p_SrcShape)
		lRegionID = ((CPlayer*)p_SrcShape)->GetVariableList()->GetGuidValue(rgnID);
	if(lRegionID == NULL_GUID)
		lRegionID = GetVariableList()->GetGuidValue(rgnID);

	if( lRegionID == NULL_GUID && m_pRegion )
	{
		lRegionID = m_pRegion->GetExID();
	}

	char* p			= GetStringParam( CmdStr, 1 );
	if( p != NULL )	
	{
		double nCounts 	= GetIntParam( CmdStr, 2 );
		double nLeft		= GetIntParam( CmdStr, 3 );
		double nTop		= GetIntParam( CmdStr, 4 );
		double nRight		= GetIntParam( CmdStr, 5 );
		double nBottom	= GetIntParam( CmdStr, 6 );
		char* strStart = GetStringParam( CmdStr, 7 );
		char* strEnd	= GetStringParam(CmdStr,8);
		double rgnType		= GetIntParam( CmdStr, 10 );
		int   iDir			= GetIntParam(CmdStr, 11);

		if( nCounts == ERROR_CODE || nLeft == ERROR_CODE || nRight == ERROR_CODE ||
			nTop == ERROR_CODE || nBottom == ERROR_CODE )
		{
			M_FREE( p, sizeof(char)*MAX_VAR_LEN );
			M_FREE( strStart, sizeof(char)*MAX_VAR_LEN );
			M_FREE( strEnd, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
			M_FREE( rgnID, sizeof(char)*MAX_VAR_LEN );
			return 0.0f;
		}
		if (iDir==ERROR_CODE || iDir== INVILID_VALUE)
		{
			iDir= -1;
		}

		CServerRegion* pRegion	= NULL;

		switch((eRgnType)(long)rgnType)
		{
		case RGN_NORMAL:
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_NORMAL, lRegionID);
			break;
		case RGN_PERSONAL_HOUSE:
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_PERSONAL_HOUSE, lRegionID);
			break;
		case RGN_PERSONAL:
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_PERSONAL, lRegionID);
			break;
		case RGN_TEAM:
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_TEAM, lRegionID);
			break;
		}

		if( pRegion == NULL )
			pRegion = static_cast<CServerRegion*>( m_pRegion );

		if( pRegion )
		{
			LONG lNewX = 0; 
			LONG lNewY = 0;
			pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );
			CCollection* pLeader = pRegion -> AddCollection( p, lNewX, lNewY,iDir );
			if( pLeader == NULL ) 
			{
				M_FREE( p, sizeof(char)*MAX_VAR_LEN );
				M_FREE( strStart, sizeof(char)*MAX_VAR_LEN );
				M_FREE( strEnd, sizeof(char)*MAX_VAR_LEN );
				M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
				M_FREE( rgnID, sizeof(char)*MAX_VAR_LEN );
				return 0;
			}

			//设置返回值guid
			if(szRetGUID)
			{
				int retFlag = SetScriptGuidValue((CMoveShape*)p_SrcShape, szRetGUID, pLeader->GetExID());
			}
			// 添加脚本
			if (strStart && strcmp(strStart,"0")!=0)
			{
				pLeader->SetStartCollectScript(strStart);
			}
			if (strEnd && strcmp(strEnd,"0")!=0)
			{
				pLeader->SetEndCollectScript(strEnd);
			}					

			ret = pLeader -> GetExID();

			for (int i=0; i<nCounts-1; i++)
			{
				pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );
				pLeader = pRegion -> AddCollection( p, lNewX, lNewY, rand()%8 );
				if (pLeader)
				{
					if (strStart)
					{
						pLeader->SetStartCollectScript(strStart);
					}
					if (strEnd)
					{
						pLeader->SetEndCollectScript(strEnd);
					}
				}
			}
		}


		M_FREE( p, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strStart, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strEnd, sizeof(char)*MAX_VAR_LEN );
	}

	M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( rgnID, sizeof(char)*MAX_VAR_LEN );
	return 0.0f;
}
double CScript::CreateCollection(const char* CmdStr, char* retStr)
{
	//static CGUID ret; ret = NULL_GUID;

	// 返回的guid变量名
	char* szRetGUID = GetStringParam(CmdStr, 0);
	if(!szRetGUID) return 0;

	//##获取lRegionID的编号，如果是0，则设置为当前场景的id
	double rgnID	= GetIntParam( CmdStr, 9 );

	char* p			= GetStringParam( CmdStr, 1 );
	if( p != NULL )	
	{
		double nCounts 	= GetIntParam( CmdStr, 2 );
		double nLeft		= GetIntParam( CmdStr, 3 );
		double nTop		= GetIntParam( CmdStr, 4 );
		double nRight		= GetIntParam( CmdStr, 5 );
		double nBottom	= GetIntParam( CmdStr, 6 );
		char* strStart = GetStringParam( CmdStr, 7 );
		char* strEnd	= GetStringParam(CmdStr,8);
		int	  iDir		= GetIntParam(CmdStr,10);

		if( nCounts == ERROR_CODE || nLeft == ERROR_CODE || nRight == ERROR_CODE ||
			nTop == ERROR_CODE || nBottom == ERROR_CODE )
		{
			M_FREE( p, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
			M_FREE( strStart, sizeof(char)*MAX_VAR_LEN );
			M_FREE( strEnd, sizeof(char)*MAX_VAR_LEN );
			return (long)(&NULL_GUID);
		}
		if (iDir== ERROR_CODE || iDir== INVILID_VALUE)
		{
			iDir= -1;
		}

		CServerRegion* pRegion = NULL;
		pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, rgnID);
		if(pRegion == NULL)
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL, rgnID);
		if(pRegion == NULL)
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL_HOUSE, rgnID);
		if(pRegion == NULL)
			pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_TEAM, rgnID);
		if( pRegion == NULL )
			pRegion = static_cast<CServerRegion*>( m_pRegion );
		if( pRegion )
		{
			LONG lNewX = 0; 
			LONG lNewY = 0;
			//定点刷,不管这个点上是否有其他Shape
			//pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );
			CCollection* pLeader = pRegion -> AddCollection( p, nLeft, nTop, iDir);
			if( pLeader == NULL ) 
			{
				M_FREE( p, sizeof(char)*MAX_VAR_LEN );
				M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
				M_FREE( strStart, sizeof(char)*MAX_VAR_LEN );
				M_FREE( strEnd, sizeof(char)*MAX_VAR_LEN );
				return 0.0;
			}

			//设置返回值guid
			if(szRetGUID)
			{
				int retFlag = SetScriptGuidValue((CMoveShape*)p_SrcShape, szRetGUID, pLeader->GetExID());
			}


			// 添加脚本
			if (strStart && strcmp(strStart,"0")!=0)
			{
				pLeader->SetStartCollectScript(strStart);
			}
			if (strEnd && strcmp(strEnd,"0")!=0)
			{
				pLeader->SetEndCollectScript(strEnd);
			}

			//			ret = pLeader -> GetExID();

			for (int i=0; i<nCounts-1; i++)
			{
				pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );
				pLeader = pRegion -> AddCollection( p, lNewX, lNewY, rand()%8 );
				if (pLeader)
				{
					if (strStart)
					{
						pLeader->SetStartCollectScript(strStart);
					}
					if (strEnd)
					{
						pLeader->SetEndCollectScript(strEnd);
					}
				}
			}
		}

		M_FREE( p, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strStart, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strEnd, sizeof(char)*MAX_VAR_LEN );
	}

	M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}
double CScript::DelCollection(const char* cmd, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	double ret = 0;
	char *pGuidName = GetStringParam( cmd, 0 );
	if (NULL == pGuidName)
	{
		return ret;
	}
	CGUID guid = GetVariableList()->GetGuidValue(pGuidName);

	if( guid == NULL_GUID )
	{
		M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
		return ret;
	}

	CServerRegion* pRegion = reinterpret_cast<CServerRegion*>( p_SrcShape -> GetFather() );
	CShape* pShape = NULL;
	if( pRegion ) 
		pShape = dynamic_cast<CShape*>( pRegion -> FindChildObject( TYPE_COLLECTION, guid) );
	if( pRegion && pShape )
	{
		pRegion->DelShapeToAround(pShape);

		pRegion->DeleteChildObject(pShape);
		ret = 1;
	}

	M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
	return 1;
}
//门操作
double CScript::SetDoor(const char* cmd, char* retStr)
{
	char *strTemp=GetStringParam(cmd,0);
	if (strTemp)
	{
		long id=GetIntParam(cmd,1);
		long value=GetIntParam(cmd,2);
		double dRegion=GetIntParam(cmd,3);
		CServerRegion* pRegion=NULL;
		if (id!=ERROR_CODE&&value!=ERROR_CODE)
		{
			//获取场景指针
			if (dRegion!=ERROR_CODE)
			{
				pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, dRegion);
				if(pRegion == NULL)
					pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL, dRegion);
				if(pRegion == NULL)
					pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL_HOUSE, dRegion);
				if(pRegion == NULL)
					pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_TEAM, dRegion);
			}
			else
			{
				CPlayer *pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
				if(pPlayer)
					pRegion=dynamic_cast<CServerRegion*>(pPlayer->GetFather());
			}
			//对门的操作
			if(pRegion)
			{
  				if (strcmp(strTemp,"State")==0)
				{
					if(value<0||value>3)
					{
						M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
						return 0;
					}
					pRegion->SetDoorState(id,value);
				}
				else if (strcmp(strTemp,"Switch")==0)
				{
					bool flag;
					if (value!=0)
						flag=true;
					else
						flag=false;
					pRegion->SetDoorDestroySwitch(id,flag);
				}
				else if (strcmp(strTemp,"Hp")==0)
				{
					pRegion->SetDoorHP(id,value);
				}
				else if (strcmp(strTemp,"MaxHp")==0)
				{
					pRegion->SetDoorMaxHP(id,value);
				}
			}
		}
		M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
	}
	return 0;
}

double CScript::GetDoor(const char* cmd, char* retStr)
{
	char *strTemp=GetStringParam(cmd,0);
	if (strTemp)
	{
		long id=GetIntParam(cmd,1);
		double dRegion=GetIntParam(cmd,2);
		if (id!=ERROR_CODE)
		{
			CServerRegion* pRegion=NULL;
			if (dRegion!=ERROR_CODE)
			{
				pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, dRegion);
				if(pRegion == NULL)
					pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL, dRegion);
				if(pRegion == NULL)
					pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL_HOUSE, dRegion);
				if(pRegion == NULL)
					pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_TEAM, dRegion);
			}
			else
			{	
				CPlayer *pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
				if(pPlayer)
					pRegion=dynamic_cast<CServerRegion*>(pPlayer->GetFather());	
			}
			if (pRegion)
			{
				if (strcmp("State",strTemp)==0)
				{
					M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
					return pRegion->GetDoorState(id);
				} 
				else if (strcmp("Switch",strTemp)==0)
				{
					M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
					return pRegion->GetDoorDestroySwitch(id);
				}
				else if (strcmp("Hp",strTemp)==0)
				{
					M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
					return pRegion->GetDoorHP(id);
				}
				else if (strcmp("MaxHp",strTemp)==0)
				{
					M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
					return pRegion->GetDoorMaxHP(id);
				}
			}
		}		
	M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
	}
	return 0;
}

