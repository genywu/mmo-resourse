//=============================================================================
/**
 *  file: MoveShapeFun.cpp
 *
 *  Brief:定义MoveShape(角色、怪物、采集物等)的公用脚本函数
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
#include "../Player.h"
#include "../Monster.h"
#include "../serverregion.h"
#include "../../../setup/LogSystem.h"
#include "../../GameServer/GameManager.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"
#include "../AI/AIUtilities.h"



double CScript::SetAttrInt(const char* CmdStr, char* retStr)
{
	OBJECT_TYPE type;
	CGUID id;
	double value   = 0;

	if(!GetSrcShape()) return -1.0f;

	int param = GetIntParam(CmdStr, 0);
	if(param != ERROR_CODE && param != INVILID_VALUE)
	{
		type = (OBJECT_TYPE)param;

		char* pIdName = GetStringParam(CmdStr, 1);
		if(!pIdName) return -1.0f;

		id = GetScriptGuidValue((CMoveShape*)GetSrcShape(), pIdName);

		CMoveShape* shape = NULL;

		char* name = GetStringParam(CmdStr, 2);

		if(!name)
		{
			M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
			return -1.0f;
		}

		value = GetIntParam(CmdStr, 3);
		if(value == ERROR_CODE || value == INVILID_VALUE)
		{
			M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( name, sizeof(char)*MAX_VAR_LEN );
			return -1.0f;
		}

		if(NULL_GUID == id) // 默认id=0时，直接调用SrcShape的SetAttr接口
		{
			shape = (CMoveShape*)GetSrcShape();
		}
		else
		{
			shape = GetGame()->FindShape(type, id);
		}

		if(shape)
		{
			if(CVariableList::VAR_NOT_FOUND == SetScriptVarValue(shape, name, 0, value))
			{
				shape->SetAttribute(name, value);
				shape->UpdateAttribute();
			}
		}
	
		M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( name, sizeof(char)*MAX_VAR_LEN );
	}
	else // 参数是字符串
	{
		AddErrorLogText(CStringReading::LoadString(IDS_GS_FUNCTION, STR_GS_FUNCTION_SETATTRFIRSTVARISSTRING));
	}

	return -1.0f;
}
double CScript::SetAttrStr(const char* CmdStr, char* retStr)
{
	OBJECT_TYPE type;
	CGUID id;

	if(!GetSrcShape()) return -1.0f;

	int param = GetIntParam(CmdStr, 0);
	if(param != ERROR_CODE && param != INVILID_VALUE)
	{
		type = (OBJECT_TYPE)param;

		char* pIdName = GetStringParam(CmdStr, 1);
		if(!pIdName) return -1.0f;

		id = GetScriptGuidValue((CMoveShape*)GetSrcShape(), pIdName);

		CMoveShape* shape = NULL;

		char* name = GetStringParam(CmdStr, 2);

		if(!name)
		{
			M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
			return -1.0f;
		}

		const char* strValue = GetStringParam(CmdStr, 3);
		if(!strValue)
		{
			M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( name, sizeof(char)*MAX_VAR_LEN );
			return -1.0f;
		}

		if(NULL_GUID == id) // 默认id=0时，直接调用SrcShape的SetAttr接口
		{
			shape = (CMoveShape*)GetSrcShape();
		}
		else
		{
			shape = GetGame()->FindShape(type, id);
		}

		if(shape)
		{
			SetScriptVarValue(shape, name, strValue);
		}

		M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( name, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strValue, sizeof(char)*MAX_VAR_LEN );
	}
	else // 参数是字符串
	{
		AddErrorLogText(CStringReading::LoadString(IDS_GS_FUNCTION, STR_GS_FUNCTION_SETATTRFIRSTVARISSTRING));
	}

	return -1.0f;
}
double CScript::SetAttrGuid(const char* CmdStr, char* retStr)
{
	OBJECT_TYPE type;
	CGUID id;
	CGUID varId;

	if(!GetSrcShape()) return -1.0f;

	int param = GetIntParam(CmdStr, 0);
	if(param != ERROR_CODE && param != INVILID_VALUE)
	{
		type = (OBJECT_TYPE)param;

		char* pIdName = GetStringParam(CmdStr, 1);
		if(!pIdName) return -1.0f;

		id = GetScriptGuidValue((CMoveShape*)GetSrcShape(), pIdName);

		CMoveShape* shape = NULL;

		char* name = GetStringParam(CmdStr, 2);

		if(!name)
		{
			M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
			return -1.0f;
		}

		const char* guName = GetStringParam(CmdStr, 3);
		if(!guName)
		{
			M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( name, sizeof(char)*MAX_VAR_LEN );
			return -1.0f;
		}

		varId = GetScriptGuidValue((CMoveShape*)GetSrcShape(), guName);

		if(NULL_GUID == id) // 默认id=0时，直接调用SrcShape的SetAttr接口
		{
			shape = (CMoveShape*)GetSrcShape();
		}
		else
		{
			shape = GetGame()->FindShape(type, id);
		}

		if(shape)
		{
			SetScriptGuidValue(shape, name, varId);
		}

		M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( name, sizeof(char)*MAX_VAR_LEN );
		M_FREE( guName, sizeof(char)*MAX_VAR_LEN );
	}
	else // 参数是字符串
	{
		AddErrorLogText(CStringReading::LoadString(IDS_GS_FUNCTION, STR_GS_FUNCTION_SETATTRFIRSTVARISSTRING));
	}

	return -1.0f;
}
double CScript::GetAttr(const char* CmdStr, char* retStr)
{
	OBJECT_TYPE type;
	CGUID id;
	double value   = 0;

	if(!GetSrcShape()) return -1.0f;

	int param = GetIntParam(CmdStr, 0);
	if(param != ERROR_CODE && param != INVILID_VALUE)
	{
		type = (OBJECT_TYPE)param;

		char* pIdName = GetStringParam(CmdStr, 1);
		if(!pIdName) return -1.0f;

		id = GetScriptGuidValue((CMoveShape*)GetSrcShape(), pIdName);

		char* name = GetStringParam(CmdStr, 2);
		if(!name)
		{
			M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
			return -1.0f;
		}

		CMoveShape* shape = NULL;
		if(NULL_GUID == id) // 默认id=0时，直接调用SrcShape的SetAttr接口
		{
			shape = (CMoveShape*)GetSrcShape();
		}
		else
			shape = GetGame()->FindShape(type, id);

		if(shape)
		{
			long lRet = shape->GetAttribute(std::string(name));
			M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( name, sizeof(char)*MAX_VAR_LEN );
			return lRet;
		}
		
		M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( name, sizeof(char)*MAX_VAR_LEN );
	}
	else // 该参数是字符串
	{
		AddErrorLogText(CStringReading::LoadString(IDS_GS_FUNCTION, STR_GS_FUNCTION_SETATTRFIRSTVARISSTRING));
	}

	return 0.0f;
}
double CScript::GetScriptAttr(const char* CmdStr, char* retStr)
{
	OBJECT_TYPE type;
	CGUID id;
	double value   = 0;

	if(!GetSrcShape()) return -1.0f;

	int param = GetIntParam(CmdStr, 0);
	if(param != ERROR_CODE && param != INVILID_VALUE)
	{
		type = (OBJECT_TYPE)param;

		char* pIdName = GetStringParam(CmdStr, 1);
		if(!pIdName) return -1.0f;

		id = GetScriptGuidValue((CMoveShape*)GetSrcShape(), pIdName);

		char* name = GetStringParam(CmdStr, 2);
		if(!name)
		{
			M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
			return -1.0f;
		}

		char* retName = GetStringParam(CmdStr, 3);

		if(!retName)
		{
			M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( name, sizeof(char)*MAX_VAR_LEN );
			return -1.0f;
		}

		CMoveShape* shape = NULL;
		if(NULL_GUID == id) // 默认id=0时，直接调用SrcShape的SetAttr接口
		{
			shape = (CMoveShape*)GetSrcShape();
		}
		else
			shape = GetGame()->FindShape(type, id);

		if(shape)
		{
			if(name[0] == '$') // 整型
			{
				double llRet = GetScriptVarValue(shape, name, 0);
				SetScriptVarValue((CMoveShape*)GetSrcShape(), retName, 0, llRet);
			}
			else if(name[0] == '#') // 字符串
			{
				const char* sRet = GetScriptVarValue(shape, name);
				if(sRet)
					SetScriptVarValue((CMoveShape*)GetSrcShape(), retName, sRet);
			}
			else if(name[0] == '@') // GUID
			{
				const CGUID& gRet = GetScriptGuidValue(shape, name);
				SetScriptGuidValue((CMoveShape*)GetSrcShape(), retName, gRet);
			}
		}

		M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( name, sizeof(char)*MAX_VAR_LEN );
		M_FREE( retName, sizeof(char)*MAX_VAR_LEN );
	}
	else // 该参数是字符串
	{
		AddErrorLogText(CStringReading::LoadString(IDS_GS_FUNCTION, STR_GS_FUNCTION_SETATTRFIRSTVARISSTRING));
	}

	return 1.0f;
}
double CScript::ChangeAttr(const char* CmdStr, char* retStr)
{
	OBJECT_TYPE type;
	CGUID id;
	double value  = 0;
	double ret = INVILID_VALUE;

	int param = GetIntParam(CmdStr, 0);
	if(param != ERROR_CODE && param != INVILID_VALUE)
	{
		type = (OBJECT_TYPE)param;

		char* pIdName = GetStringParam(CmdStr, 1);
		if(NULL == pIdName) return ret;
		id = GetVariableList()->GetGuidValue(pIdName);
		if(id == NULL_GUID)
			id = GetGeneralVariableList()->GetGuidValue(pIdName);
		if(id == NULL_GUID)
			id = ((CPlayer*)p_SrcShape)->GetVariableList()->GetGuidValue(pIdName);

		CMoveShape* shape = GetGame()->FindShape(type, id);

		char* name = GetStringParam(CmdStr, 2);
		if(name)
		{
			value = GetIntParam(CmdStr, 3);
			if(value != ERROR_CODE && value != INVILID_VALUE)
			{

				if(NULL == shape) // 默认id=0时，直接调用SrcShape的SetAttr接口
				{
					CShape* cShape = GetSrcShape();
					if(cShape)
					{
						((CMoveShape*)cShape)->ChangeAttribute(string(name), value);
						((CMoveShape*)cShape)->UpdateAttribute();
						ret = 1;
					}
				}
				else
				{
					shape->ChangeAttribute(string(name), value);
					shape->UpdateAttribute();
					ret = 1;
				}
			}
		}

		M_FREE( pIdName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( name, sizeof(char)*MAX_VAR_LEN );
	}
	else // 参数是字符串
	{
		AddErrorLogText(CStringReading::LoadString(IDS_GS_FUNCTION, STR_GS_FUNCTION_CHANGEATTRFIRSTVARISSTRING));
	}

	return ret;
}


//为当前对象注册一个定时器
double CScript::RegisterTimer(const char* cmd, char* retStr)
{
	int nRet = 0;
	long lfuturetime = static_cast<long>(GetIntParam(cmd, 0));
	if(lfuturetime != ERROR_CODE && lfuturetime != INVILID_VALUE)
	{
		long lInterTime = static_cast<long>(GetIntParam(cmd, 1));
		if(lInterTime != ERROR_CODE && lInterTime != INVILID_VALUE)
		{
			char* pszScriptName=GetStringParam(cmd,2);
			if( pszScriptName )
			{
				long lUserData = static_cast<long>(GetIntParam(cmd, 3));
				if(lUserData != ERROR_CODE && lUserData != INVILID_VALUE)
				{
					CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_SrcShape);
					if( pMoveShape )
					{
						//添加提示
						nRet = pMoveShape->RegisterScriptTimer(lfuturetime,lInterTime,
							pszScriptName,lUserData);
					}
				}
				M_FREE( pszScriptName, sizeof(char)*MAX_VAR_LEN );
			}
		}
	}
	return nRet;
}

//为当前对象注销一个定时器
double CScript::UnRegisterTimer(const char* cmd, char* retStr)
{
	int nRet = 0;
	long lUserData = static_cast<long>(GetIntParam(cmd, 0));
	if(lUserData != ERROR_CODE && lUserData != INVILID_VALUE)
	{
		long lTimerID = static_cast<long>(GetIntParam(cmd, 1));
		if(lTimerID != ERROR_CODE && lTimerID != INVILID_VALUE)
		{
			CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_SrcShape);
			if( pMoveShape )
			{
				//添加提示
				nRet = pMoveShape->UnRegisterScriptTimer(lUserData,lTimerID);
			}
		}
	}
	return nRet;
}




//播放表情
double CScript::PlayMotion(const char* cmd, char* retStr)
{
	int nRet = 0;
	long lMotionNum  = static_cast<long>(GetIntParam(cmd, 0));
	if(lMotionNum != ERROR_CODE && lMotionNum != INVILID_VALUE)
	{
		long lAround = static_cast<long>(GetIntParam(cmd, 1));
		if(lAround != ERROR_CODE && lAround != INVILID_VALUE)
		{
			if( p_SrcShape )
			{			
				CMessage msg( MSG_S2C_SHAPE_EMOTION );
				msg.Add( p_SrcShape->GetType() );
				msg.Add( p_SrcShape->GetExID() );
				msg.Add( lMotionNum );
				//发送给周围玩家
				if(lAround)
				{
					msg.SendToAround(p_SrcShape);
				}
				else if(p_SrcShape->GetType() == TYPE_PLAYER)
				{
					msg.SendToPlayer(p_SrcShape->GetExID());
				}
			}
		}
	}
	return nRet;
}

//目标执行一个脚本
double CScript::DesRunScript(const char* cmd, char* retStr)
{
	char * cScriptName= GetStringParam(cmd,0);
	if (cScriptName== NULL)	return -1;

	stRunScript st;
	st.pszFileName = cScriptName;
	st.srcShape = p_DesShape;
	st.pRegion = (CRegion*)(p_SrcShape->GetFather());
	st.desShape = NULL;
	RunScript(&st, (char*)GetGame()->GetScriptFileData(cScriptName));
	M_FREE( cScriptName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

//------//返回执行者周围一定范围内的玩家数量
double	CScript::GetPlayerNumByAround(const char* cmd, char* retStr)
{
	if (p_SrcShape == NULL)	return -1;

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( p_SrcShape -> GetFather() );
	if( pRegion == NULL ) return -1;

	double range= GetIntParam(cmd,0);
	if (range==INVILID_VALUE && range==ERROR_CODE )
	{
		return -1;
	}

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( p_SrcShape, TYPE_PLAYER, vOut );

	// 死亡判定标志
	int iDiedFlag = GetIntParam( cmd, 1 );
	// 隐身判定标志
	int iHideFlag = GetIntParam( cmd, 2 );

#define ADJUST_V( v ) v = ( v == INVILID_VALUE || v == ERROR_CODE ) ? 0 : v

	ADJUST_V( iDiedFlag );
	ADJUST_V( iHideFlag );

	int iNum=0;
	for (int a=0; a<vOut.size(); a++)
	{
		if (p_SrcShape->Distance(vOut[a])> range)	continue;
		if( !iDiedFlag && static_cast<CPlayer*>( vOut[a] )->IsDied() ) continue;
		if( !iHideFlag && !static_cast<CMoveShape*>( vOut[a] )->IsVisible( static_cast<CMoveShape*>( p_SrcShape ) ) ) continue;
		if( !iHideFlag && AIUtilities::IsInGodState( vOut[a] ) ) continue;
		iNum++;
	}

	return iNum;
}

//------//返回执行者周围一定范围内的怪物数量
double	CScript::GetMonsterNumByAround(const char* cmd, char* retStr)
{
	if (p_SrcShape == NULL)	return -1;

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( p_SrcShape -> GetFather() );
	if( pRegion == NULL ) return -1;

	double range= GetIntParam(cmd,0);
	if (range==INVILID_VALUE && range==ERROR_CODE )
	{
		return -1;
	}

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( p_SrcShape, TYPE_MONSTER, vOut );
	int iNum=0;
	for (int a=0; a<vOut.size(); a++)
	{
		if (p_SrcShape->Distance(vOut[a])> range)	continue;
		iNum++;
	}

	return iNum;
}

//------//返回执行者周围一定范围内的同类怪物数量
double	CScript::GetSameMonsterNumByAround(const char* cmd, char* retStr)
{
	CMonster* pMonster= dynamic_cast<CMonster*>(p_SrcShape);
	if (pMonster == NULL)	return -1;

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pMonster -> GetFather() );
	if( pRegion == NULL ) return -1;

	double range= GetIntParam(cmd,0);
	if (range==INVILID_VALUE && range==ERROR_CODE )
	{
		return -1;
	}

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( pMonster, TYPE_MONSTER, vOut );
	int iSameTypeNum=0;
	for (int a=0; a<vOut.size(); a++)
	{
		if (pMonster->Distance(vOut[a]) > range) continue;
		if (pMonster->GetIndex()!= ((CMonster*)vOut[a])->GetIndex()) continue;

		iSameTypeNum++;
	}
	return iSameTypeNum;
}

double CScript::SetDelTime(const char* cmd, char* retStr)
{
	if(!p_SrcShape) return -1.0f;

	int time= GetIntParam(cmd,0);
	if (time==INVILID_VALUE || time == ERROR_CODE || time<0)
	{
		return -1.0f;
	}
	CMoveShape* pMoveShape= dynamic_cast<CMoveShape*>(p_SrcShape);
	if (pMoveShape== NULL ) return -1.0f;

	pMoveShape->AddDelEvent(time);
	return 0;

}

double CScript::SetDelTimeByGuid(const char* cmd, char* retStr)
{
	if(!p_SrcShape) return -1.0f;

	char* szGUID = GetStringParam(cmd, 0); 
	int type=GetIntParam(cmd,1);
	int iDelTime= GetIntParam(cmd,2);

	if (szGUID==NULL || 
		type== ERROR_CODE || type== INVILID_VALUE)
	{
		M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}
	if (iDelTime== ERROR_CODE || iDelTime== INVILID_VALUE)
	{
		iDelTime=0;
	}


	CGUID monsterGUID = GetVariableList()->GetGuidValue(szGUID);
	CServerRegion* pRegion=dynamic_cast<CServerRegion*>(p_SrcShape->GetFather());
	if (pRegion!= NULL)
	{
		CMoveShape* pMoveShape= dynamic_cast<CMoveShape*>(pRegion->FindChildObject(type,monsterGUID));
		if (pMoveShape!= NULL)
		{
			pMoveShape->AddDelEvent(iDelTime);
		}
	}
	M_FREE( szGUID, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

//取得源的guid
double CScript::GetSrcShapeGUID(const char* cmd, char* retStr)
{
	if (p_SrcShape==NULL)
	{
		return -1;
	}
	char* cGuidName= GetStringParam(cmd,0);
	if (cGuidName== NULL)	return -1;

	SetScriptGuidValue((CMoveShape*)(p_SrcShape), cGuidName, p_SrcShape->GetExID());

	M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

//取得目标guid
double CScript::GetDesShapeGUID(const char* cmd, char* retStr)
{
	if (p_DesShape==NULL)
	{
		return -1;
	}
	char* cGuidName= GetStringParam(cmd,0);
	if (cGuidName== NULL)	return -1;

	SetScriptGuidValue((CMoveShape*)(p_SrcShape), cGuidName, p_DesShape->GetExID());

	M_FREE( cGuidName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::ChangeMoveSpeed(const char* cmd, char* retStr)
{
	double dbRet = 1;
	int nSpeed = GetIntParam(cmd, 0);
	if(nSpeed != ERROR_CODE && nSpeed != INVILID_VALUE)
	{
		CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_SrcShape);
		if(pMoveShape)
		{
			pMoveShape->ChangeSpeed(nSpeed);
			pMoveShape->SendChangedSpeedToClients();
		}
	}
	else // 参数是整数
	{
		AddErrorLogText("ERR: ChangeMoveSpeed(): 1st Arg is int!");
	}
	return dbRet;
}

double CScript::SetMoveSpeed(const char* cmd, char* retStrL)
{
	double dbRet = 1;
	int nSpeed = GetIntParam(cmd, 0);
	if(nSpeed != ERROR_CODE && nSpeed != INVILID_VALUE)
	{
		CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_SrcShape);
		if(pMoveShape)
		{
			pMoveShape->SetChangeSpeed(nSpeed);
			pMoveShape->SendChangedSpeedToClients();
		}
	}
	else // 参数是整数
	{
		AddErrorLogText("ERR: SetMoveSpeed(): 1st Arg is int!");
	}
	return dbRet;
}
double CScript::GetCurrentSpeed(const char* cmd, char* retStr)
{
	double dbRet = 0.0;
	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_SrcShape);
	if(pMoveShape)
	{
		dbRet =  pMoveShape->GetSpeed();// SetChangeSpeed(nSpeed);
	}
	return dbRet;
}

double CScript::ChangeBaseSpeed(const char* cmd, char* retStr)
{
	double dbRet = 1;
	int nRideState = GetIntParam(cmd, 0);
	DWORD dwSrc = GetIntParam(cmd,1);

	if(nRideState != ERROR_CODE && nRideState != INVILID_VALUE && dwSrc != ERROR_CODE && dwSrc != INVILID_VALUE)
	{
		CMoveShape* pMoveShape = (CMoveShape*)(dwSrc);
		if(pMoveShape && pMoveShape->GetType()==TYPE_PLAYER)
		{ 
			if (!pMoveShape->GetIsRide())
			{
				//骑状态
				if(!pMoveShape->GetIsRun())
				{
					pMoveShape->SetSpeed(CGlobeSetup::GetSetup()->fRidePlayerWalkSpeed);
				}
				else
				{
					pMoveShape->SetSpeed(CGlobeSetup::GetSetup()->fRidePlayerRunSpeed);
				}
			} 
			else
			{
				if(!pMoveShape->GetIsRun())
				{
					pMoveShape->SetSpeed(CGlobeSetup::GetSetup()->fPlayerWalkSpeed);
				}
				else
				{
					pMoveShape->SetSpeed(CGlobeSetup::GetSetup()->fPlayerRunSpeed);
				}
			}


			pMoveShape->SendChangedSpeedToClients();
		}
	}
	return dbRet;
}



//得到目标整型变量的值,$bound默认为0
double CScript::GetDestIntVar(const char* cmd, char* retStr)
{
	double dbRet = 0;
	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_DesShape);
	if(!pMoveShape)	return dbRet;

	char* szValueName= GetStringParam(cmd,0);
	if(szValueName)
	{
		if( strlen(szValueName) >= 2 && szValueName[0]=='$')
		{
			long lBound = GetIntParam(cmd,1);
			if(lBound == ERROR_CODE)
			{
				lBound = 0;
			}
			dbRet = pMoveShape->GetVariableList()->GetVarValue(szValueName,0);
		}
		M_FREE( szValueName, sizeof(char)*MAX_VAR_LEN );
	}
	return dbRet;
}
//设置目标整形变量的值,$bound默认为0
double CScript::SetDestIntVar(const char* cmd, char* retStr)
{
	double dbRet = 0;
	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_DesShape);
	if(!pMoveShape)	return dbRet;

	char* szValueName= GetStringParam(cmd,0);
	if(szValueName)
	{
		if( strlen(szValueName) >= 2 && szValueName[0]=='$')
		{
			long lValue =  GetIntParam(cmd,1);
			if(lValue != ERROR_CODE)
			{
				long lBound = GetIntParam(cmd,2);
				if(lBound == ERROR_CODE)
				{
					lBound = 0;
				}
				dbRet = pMoveShape->GetVariableList()->SetVarValue(szValueName,0,lValue);
			}
		}
		M_FREE( szValueName, sizeof(char)*MAX_VAR_LEN );
	}
	
	return dbRet;
}
//得到目标字符串变量的值
double CScript::GetDestStrVar(const char* cmd, char* retStr)
{
	double dbRet = 0;
	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_DesShape);
	if(!pMoveShape)	return dbRet;
	
	char* szValueName= GetStringParam(cmd,0);
	if(szValueName)
	{
		if( strlen(szValueName) >= 2 && szValueName[0]=='#')
		{
			char* szRetName = GetStringParam(cmd,1);
			if(szRetName)
			{
				const char* szValue = pMoveShape->GetVariableList()->GetVarValue(szValueName);
				if(szValue && p_SrcShape)
				{
					SetScriptVarValue(dynamic_cast<CMoveShape*>(p_SrcShape),szRetName,szValue);
				}
				M_FREE( szRetName, sizeof(char)*MAX_VAR_LEN );
			}
		}
		M_FREE( szValueName, sizeof(char)*MAX_VAR_LEN );
	}

	return dbRet;
}
//设置目标字符串变量的值
double CScript::SetDestStrVar(const char* cmd, char* retStr)
{
	double dbRet = 0;
	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_DesShape);
	if(!pMoveShape)	return dbRet;

	char* szValueName= GetStringParam(cmd,0);
	if(szValueName)
	{
		if( strlen(szValueName) >= 2 && szValueName[0]=='#')
		{
			char* szValue = GetStringParam(cmd,1);
			if(szValue)
			{			
				pMoveShape->GetVariableList()->SetVarValue(szValueName,szValue);
				M_FREE( szValue, sizeof(char)*MAX_VAR_LEN );
			}
		}
		M_FREE( szValueName, sizeof(char)*MAX_VAR_LEN );
	}
	return dbRet;
}
//得到目标Guid变量的值
double CScript::GetDestGuidVar(const char* cmd, char* retStr)
{
	double dbRet = 0;
	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_DesShape);
	if(!pMoveShape)	return dbRet;

	char* szValueName= GetStringParam(cmd,0);
	if(szValueName)
	{
		if( strlen(szValueName) >= 2 && szValueName[0]=='@')
		{
			char* szRetName = GetStringParam(cmd,1);
			if(szRetName)
			{
				const CGUID& guid =  pMoveShape->GetVariableList()->GetGuidValue(szValueName);
				if(p_SrcShape)
				{
					SetScriptGuidValue(dynamic_cast<CMoveShape*>(p_SrcShape),szRetName,guid);
				}
				M_FREE( szRetName, sizeof(char)*MAX_VAR_LEN );
			}
		}
		M_FREE( szValueName, sizeof(char)*MAX_VAR_LEN );
	}

	return dbRet;
}
//设置目标变量GUID的值
double CScript::SetDestGuidVar(const char* cmd, char* retStr)
{
	double dbRet = 0;
	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_DesShape);
	if(!pMoveShape)	return dbRet;

	char* szValueName= GetStringParam(cmd,0);
	if(szValueName)
	{
		if( strlen(szValueName) >= 2 && szValueName[0]=='@')
		{
			char* szGuidName = GetStringParam(cmd,1);
			if(szGuidName )
			{
				if(p_SrcShape)
				{
					const CGUID& guid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szGuidName);
					pMoveShape->GetVariableList()->SetGuidValue(szValueName,&guid);
				}
				M_FREE( szGuidName, sizeof(char)*MAX_VAR_LEN );
			}
		}
		M_FREE( szValueName, sizeof(char)*MAX_VAR_LEN );
	}

	return dbRet;
}

double CScript::IsShapeValid( const char *cmd, char *retStr )
{
	int type = GetIntParam( cmd, 0 );
	if( type == ERROR_CODE || type == INVILID_VALUE )
	{
		return -1;
	}
	char *strGUID = GetStringParam( cmd, 1 );
	if( strGUID == NULL )
	{
		return -1;
	}

	const CGUID &guid = GetScriptGuidValue( (CMoveShape*) p_SrcShape, strGUID );
	CMoveShape *pShape = GetGame()->FindShape( (OBJECT_TYPE) type, guid );
	
	M_FREE( strGUID, sizeof(char)*MAX_VAR_LEN );

	return pShape != NULL;
}
