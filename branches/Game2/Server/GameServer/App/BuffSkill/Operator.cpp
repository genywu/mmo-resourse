#include "StdAfx.h"
#include "Operator.h"
#include "Expressions.h"
#include "../MoveShape.h"
#include "../ServerRegion.h"
#include "../../../../Public/ServerPublic/Server/Common/VariableList.h"
#include "../../../../Public/ServerPublic/Server/Common/VarListUtils.h"
#include "../LuaScript/Script.h"
#include "../LuaScript/ScriptSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COperator::COperator(stModuParam *pParam)
:m_pParam(pParam)
{
}

COperator::~COperator(void)
{
	
}

CMoveShape* COperator::GetOpShape(CMoveShape* pDest)
{
	list<stDestObj*>::iterator it = m_pParam->OperObjs.begin();
	for(;it!=m_pParam->OperObjs.end();++it)
	{
		if ((*it)->DestID==m_pParam->pUser->GetExID() && (*it)->nDestType==m_pParam->pUser->GetType() && m_pParam->OperObjs.size()==1)
		{
			return m_pParam->pUser;
		}
	}

	if(m_Obj == Obj_Self)
		return m_pParam->pUser;
	else if(m_Obj == Obj_Dest)
		return pDest;
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

COpProperty::COpProperty(stModuParam *pParam)
:COperator(pParam)
{
}
COpProperty::COpProperty(void* pParam)
:COperator((stModuParam*)pParam)
{

}
COpProperty::~COpProperty(void)
{

}

//执行
void COpProperty::Execute(CMoveShape* pShape)
{
	if(m_strVariaName != "")
	{
		//  [5/4/2009 chenxianj]	
		m_nValue = (long)m_pParam->pVariableList->GetVarValue<double>(m_strVariaName.c_str(),0);	
		//  [5/4/2009 chenxianj]
		
	}
    if (m_ParamName!="")
    {
        /// by:expter 2010/08/31 技能增加一个参数
        m_nValue = m_pParam->pFirer->GetSkillValue(m_pParam->nID,m_pParam->nLvl,m_ParamName);
    }

	if (m_nValue<0)
	{
		m_nValue = 0;
	}
   
	if (m_strName=="Hp" && pShape!=m_pParam->pUser && m_pParam->eType==ModuType_Skill_Active && m_pOpFun==AddOper && m_pParam->eAddSubType==eAdd_Type)
	{
		//增益技能设定
		pShape->OnBeenAddAppendSkill(m_pParam->pUser);
		if (pShape->GetState()==1)
		{
			m_pParam->pUser->ChangeStateTo((CShape::eSTATE)pShape->GetState());
		}
	}
#ifdef _SKILL_INFO_D_   
    Log4c::Trace(SKILL_MODULE, "COpProperty:修改属性名:%s: 修改属性值:%d\n",m_strName.c_str(),m_nValue);    
#endif

	if(m_pOpFun)
		m_pOpFun(GetOpShape(pShape),m_strName,m_nValue);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

COpBuff::COpBuff(stModuParam *pParam)
:COperator(pParam)
{
}
COpBuff::COpBuff(void* pParam)
:COperator((stModuParam*)pParam)
{

}
COpBuff::~COpBuff(void)
{ 
	         
}

//执行
void COpBuff::Execute(CMoveShape* pShape)
{
	if (m_Obj==Obj_Self)
	{
		pShape = m_pParam->pUser;
	}
    if ( m_strBuffId!="" )
    {
        /// by:expter 2010/08/31 技能增加一个参数
        m_nBuffID = m_pParam->pFirer->GetSkillValue(m_pParam->nID,m_pParam->nLvl,m_strBuffId);
    }
    if ( m_strBuffLvl!="" )
    {
        /// by:expter 2010/08/31 技能增加一个参数
        m_nBuffLvl = m_pParam->pFirer->GetSkillValue(m_pParam->nID,m_pParam->nLvl,m_strBuffLvl);
    }
    if ( m_strBuffNum!="" )
    {
        /// by:expter 2010/08/31 技能增加一个参数
        m_nBuffCount = m_pParam->pFirer->GetSkillValue(m_pParam->nID,m_pParam->nLvl,m_strBuffNum);
    }

	if (m_Op==MODOP_Add)
	{
		pShape->AddNewState(m_pParam->pUser,m_nBuffID,m_nBuffLvl,11);
	} 
	else if(m_Op==MODOP_Sub)
	{
		pShape->DelNewState(m_nBuffID,m_nBuffLvl,m_nBuffCount);
	}
    else if (m_Op==MODOP_Clear)
    {
        ///@by:expter 清空指定技能类型
        pShape->ClearState(m_SkillType,m_nBuffCount);
#ifdef _SKILL_INFO_D_     
            Log4c::Trace(SKILL_MODULE, "ClearBuf:技能=id,清除技能类型%d,清除技能数量%d\n", m_pParam->nID,m_SkillType,m_nBuffCount );    
#endif
    }
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
COpSkill::COpSkill(stModuParam *pParam)
:COperator(pParam)
{

}
COpSkill::COpSkill(void *pParam)
:COperator((stModuParam*)pParam)
{

}
COpSkill:: ~COpSkill(void)
{

}
//执行
void COpSkill::Execute(CMoveShape* pShape)
{
	if (m_Obj==Obj_Self)
	{
		pShape = m_pParam->pUser;
	}
	if (m_strOperator=="Sub" && pShape)
	{
		pShape->StopSkills(m_lID,m_lLv,m_eType);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

COpGood::COpGood(stModuParam *pParam)
:COperator(pParam)
{
}
COpGood::COpGood(void* pParam)
:COperator((stModuParam*)pParam)
{

}
COpGood::~COpGood(void)
{

}

//执行
void COpGood::Execute(CMoveShape* pShape)
 {
	 CMoveShape *pMoveShape = NULL;
	 if (m_Obj==Obj_Self)
	 {
		 pMoveShape = m_pParam->pUser;
	 }
	 else
	 {
		 pMoveShape = pShape;
	 }
	 if (m_Op == MODOP_Sub)
	 {
		 if (m_strOrigName.size()>0)
		 {
			 //根据原始名删除物品
			 pMoveShape->DelGoods((char*)m_strOrigName.c_str(),m_nValue);

		 } 
		 else if(m_nPlace!=0&&m_nPos!=0)
		 {
			 //根据容器id和pos删除物品
			 pMoveShape->DelGoodsAndSendMsg(m_nPlace,m_nPos);			
		 }
		 else
		 {
			 //根据容器id和物品类别删除物品
			 pMoveShape->DeleteGoods((CS2CContainerObjectMove::PLAYER_EXTEND_ID)m_nPlace,(eGoodsBaseType)m_nGoodType,m_nWeaponType,m_nValue);
		 }

	 }
 }


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//位置操作
COpPosition::COpPosition(stModuParam *pParam)
:COperator(pParam)
{
}
COpPosition::COpPosition(void* pParam)
:COperator((stModuParam*)pParam)
{

}
COpPosition::~COpPosition(void)
{
	
}
//随机获取目标四周一个无阻挡点的坐标
void COpPosition::GetRandomPos(CMoveShape *pShape,LONG &x, LONG &y)
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(pShape->GetFather());
	if (pRegion -> GetBlock(pShape->GetTileX()+ pShape->GetFigure() +1,pShape->GetTileY()) == CRegion::BLOCK_NO)
	{
		x = pShape->GetTileX() + pShape->GetFigure() + 1 ;
		y = pShape->GetTileY();
	}
	else if (pRegion -> GetBlock(pShape->GetTileX()-pShape->GetFigure()-1,pShape->GetTileY()) == CRegion::BLOCK_NO)
	{
		x = pShape->GetTileX()-pShape->GetFigure()-1;
		y = pShape->GetTileY();
	}
	else if (pRegion -> GetBlock(pShape->GetTileX(),pShape->GetTileY() + pShape->GetFigure()  + 1) == CRegion::BLOCK_NO)
	{
		x = pShape->GetTileX();
		y = pShape->GetTileY()+pShape->GetFigure()+1;
	}
	else if (pRegion -> GetBlock(pShape->GetTileX(),pShape->GetTileY()-pShape->GetFigure()-1) == CRegion::BLOCK_NO)
	{
		x = pShape->GetTileX();
		y = pShape->GetTileY()-pShape->GetFigure()-1;
	}
}
//执行
void COpPosition::Execute(CMoveShape* pShape)
{
	LONG x=0,y=0;
	LONG lDistance=0;
	list<POINT> listPos;
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(m_pParam->pUser->GetFather());
	

	if (m_refObj==Obj_Mouse)
	{
		x=m_pParam->nMouseX;
		y=m_pParam->nMouseY;
	}
	else if(m_pParam->pUser && pShape)
	{
		if (m_eDriect==DIREC_Front)
		{	
			long lDistance = m_pParam->pUser->RealDistance(pShape);
			if (lDistance<0)
			{
				lDistance = 0;
			}
			GetLineFront(x,y,m_pParam->pUser->GetTileX(),m_pParam->pUser->GetTileY(),
				pShape->GetTileX(),pShape->GetTileY(),lDistance);
		}
		else if (m_eDriect==DIRECT_Back)
		{			
			GetLineBack(x,y,m_pParam->pUser->GetTileX(),m_pParam->pUser->GetTileY(),
				pShape->GetTileX(),pShape->GetTileY(),pShape->GetFigure());
		}

	}


	if (x==0 && y==0)
		return;

	if( pRegion && m_refObj!=Obj_Mouse)
	{
		if( x > pRegion -> GetWidth() || 
			x < 0 ||
			y >= pRegion -> GetHeight() ||
			y < 0 || 
			pRegion -> GetBlock(x, y) != CRegion::BLOCK_NO)
		{			
			GetRandomPos(pShape,x,y);
		}
	}
	if (m_Obj==Obj_Self)
	{
		pShape = m_pParam->pUser;
	}
	

	if (pShape && pShape->CanMove())
	{
		pShape->ProduceEvent(ET_Move);
		pShape->ProduceEvent(ET_EventBreak);
		if (m_ePosStyle==POS_SIMPLE)
		{
			pShape->SetPosition(x,y);
		} 
		else if (m_ePosStyle==POS_SLIP)
		{
			pShape->SetSlipPos(x,y,m_fSpeed,m_pParam->pUser->GetDirEx());

#ifdef _SKILL_INFO_D_    
            Log4c::Trace(SKILL_MODULE, "位移坐标x:%d,y:%d,目标坐标:x:%d,y:%d\n",x,y,m_pParam->pMouseShape->GetTileX(),m_pParam->pMouseShape->GetTileY());
#endif
		}
		
	}
	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//方向操作
COpDirec::COpDirec(stModuParam *pParam)
:COperator(pParam)
{
}
COpDirec::COpDirec(void* pParam)
:COperator((stModuParam*)pParam)
{

}
COpDirec::~COpDirec(void)
{
	
}
void COpDirec::SendChangeDir(CMoveShape* pShape)
{

	CMessage Msg( MSG_S2C_SHAPE_CHANGEDIR );
	Msg.Add( static_cast<BYTE>(pShape->GetDir()) );
	Msg.Add( pShape->GetDirEx());
	Msg.Add( pShape->GetType() );
	Msg.Add( pShape->GetExID() );
	Msg.SendToAround( pShape );

}
//执行
void COpDirec::Execute(CMoveShape* pShape)
{
	if (pShape == m_pParam->pUser && m_eDir!=eDir_Mouse)
	{
		return;
	}
	CMoveShape *pMoveShape = GetOpShape(pShape);
	long lX = 0,lY = 0;

	if(pMoveShape == NULL)	
	{
		lX = m_pParam->nMouseX;
		lY = m_pParam->nMouseY;
		float fDir =GetRadianDir((float)m_pParam->pUser->GetTileX(),(float)m_pParam->pUser->GetTileY(),(float)lX,(float)lY);
		m_pParam->pUser->SetDir((long)fDir);
		//SendChangeDir(m_pParam->pUser);		
		return;
	}
	
	if(m_eDir == eDir_Mouse)
	{
		lX = m_pParam->nMouseX;
		lY = m_pParam->nMouseY;
	}
	else if(m_eDir == eDir_Self)
	{
		lX = m_pParam->pUser->GetTileX();
		lY = m_pParam->pUser->GetTileY();
	}
	else if(m_eDir == eDir_Dest && pShape )//&& 
	{
		lX = pShape->GetTileX();
		lY = pShape->GetTileY();
		if (m_pParam->pMouseShape)
		{
			lX = m_pParam->pMouseShape->GetTileX();
			lY = m_pParam->pMouseShape->GetTileY();
		}
	}
	if((lX !=0 || lY !=0) && pMoveShape)
	{
		float fDir =GetRadianDir((float)pMoveShape->GetTileX(),(float)pMoveShape->GetTileY(),(float)lX,(float)lY);
		pMoveShape->SetDirEx(fDir);
		//SendChangeDir(pMoveShape);	
	}

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Cooldown

COpTimeChange::COpTimeChange(stModuParam *pParam)
:COperator(pParam)
{
}
COpTimeChange::COpTimeChange(void* pParam)
:COperator((stModuParam*)pParam)
{

}
COpTimeChange::~COpTimeChange(void)
{
	
}                                    
//执行
void COpTimeChange::Execute(CMoveShape* pShape)
{
	if(NULL == pShape)	return;

	if(m_strVariaName!="")
	{
		//  [5/4/2009 chenxianj]
		m_nValue = (int)m_pParam->pVariableList->GetVarValue<double>(m_strVariaName.c_str(),0);
		//  [5/4/2009 chenxianj]
	}
    if (m_ParamName!="")
    {
        /// by:expter 2010/08/31 技能增加一个参数
        m_nValue = m_pParam->pFirer->GetSkillValue(m_pParam->nID,m_pParam->nLvl,m_ParamName,m_pParam->nMouseX,m_pParam->nMouseY);
    }

	if(m_eOpCd == OP_TS_Self)
	{
		//技能本身		
		m_pParam->pUser->SetSkillCooldown(m_pParam->nID,m_nValue);
	}
	else if (m_eOpCd == OP_TS_Act)
	{
		//动作	
		DWORD dwCurTime = timeGetTime();
		m_pParam->pUser->SetActTime(dwCurTime,m_nValue);
	}
	else if (m_eOpCd == OP_TS_Inton)
	{
		//吟唱时间
		DWORD dwStartTime = timeGetTime();
		DWORD dwIntonateTime = m_nValue;
		m_pParam->pUser->SetIntonateTime(dwStartTime,dwIntonateTime);
	}
    else if (m_eOpCd == OP_TS_CDControl)
    {
        DWORD dwStartTime = timeGetTime();
        m_pParam->pUser->SetCDTime( dwStartTime , m_nValue );
    }

#ifdef _SKILL_INFO_D_     
    Log4c::Trace(SKILL_MODULE, "COpTimeChange:m_strVariaName=%s,m_ParamName=%s,m_eOpCd=%d,Time=%d",m_strVariaName.c_str(),m_ParamName.c_str(),m_eOpCd,m_nValue);
#endif

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//COpDestObjs

COpDestObjs::COpDestObjs(stModuParam *pParam)
:COperator(pParam)
{
}
COpDestObjs::COpDestObjs(void *pParam)
:COperator((stModuParam*)pParam)
{

}
COpDestObjs::~COpDestObjs(void)
{
	
}
//执行
void COpDestObjs::Execute(CMoveShape* pShape)
{
	if (m_Obj == Obj_Self)
	{
		pShape = m_pParam->pUser;
	}
	//添加目标
	if(m_Op == OP_Dest_Obj_Add && pShape)
	{	
		list<stDestObj*>::iterator it = m_pParam->OperObjs.begin();
		for(;it!=m_pParam->OperObjs.end();++it)
		{
			if ((*it)->DestID==pShape->GetExID() && (*it)->nDestType==pShape->GetType())
			{
				return;
			}
		}
		stDestObj *obj = OBJ_CREATE(stDestObj);
		obj->DestID = pShape->GetExID();
		obj->nDestType = pShape->GetType();		
		m_pParam->OperObjs.push_back(obj);
	}
	//清除一个目标
	else if(m_Op == OP_Dest_Obj_Del)
	{
	}
	//清除所有目标
	else if(m_Op == OP_Dest_Obj_Clear)
	{
		m_pParam->OperObjs.clear();
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

COpScript::COpScript(stModuParam *pParam)
:COperator(pParam)
{
}
COpScript::COpScript(void *pParam)
:COperator((stModuParam*)pParam)
{
}
COpScript::~COpScript(void)
{
	itExpress it = m_Expressions.begin();
	for(;it != m_Expressions.end();it++)
	{
		OBJ_RELEASE(CExpressions, *it);
	}
	m_Expressions.clear();
}

//执行
void COpScript::Execute(CMoveShape* pShape)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "COpScript::Execute() begin");
#endif
	//  [5/4/2009 chenxianj]
	//CScript *pTempScript =  PopFreeScript("COpScript::Execute");
	//if (pTempScript)
	//{
	//	pTempScript->SetVariableList(m_pParam->pVariableList);
	//	pTempScript->SetUsedItemID(m_pParam->GoodsGuid);
	//	pTempScript->SetSrcShapeEx(m_pParam->pUser);
	//	//  [5/4/2009 chenxianj]
	//	itExpress it = m_Expressions.begin();
	//	for(;it != m_Expressions.end();it++)
	//	{			
	//		if(m_pParam->pUser)
	//			pTempScript->SetRegion(dynamic_cast<CRegion*>(m_pParam->pUser->GetFather()));
	//		else
	//			pTempScript->SetRegion(NULL);


	//		(*it)->Execute(pTempScript,pShape);

	//	}
	//	//  [5/4/2009 chenxianj]
	//	PushFreeScript(pTempScript);
	//}
	Script::RunEnv env( m_pParam->pUser, pShape, static_cast<CRegion*>( m_pParam->pUser->GetFather() ),
		m_pParam->pVariableList );
	VariableList *varList = m_pParam->pVariableList;
	if( !varList->AddVar( "$ParamA", (double)(DWORD)m_pParam->pUser ) )
	{
		varList->SetVarValue( "$ParamA", (double)(DWORD) m_pParam->pUser );
	}
	if( !varList->AddVar( "$ParamB", (double)(DWORD)pShape ) )
	{
		varList->SetVarValue( "$ParamB", (double)(DWORD) pShape );
	}
	
	Script *script = GetInst( ScriptSys ).NewScript( env );
	for( itExpress it = m_Expressions.begin(); it != m_Expressions.end(); ++ it )
	{
		CExpressions *exp = *it;
		int ret = exp->Execute( script, pShape );
		if( !script->CanDelete( ret ) )
		{
			// yield script, cannot run.
			assert( "Yield script" && 0 );
		}
	}
	GetInst( ScriptSys ).DelScript( script );

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "COpScript::Execute() end");
#endif
}

COpGenius::COpGenius(stModuParam *pParam):COperator(pParam)
{

}

COpGenius::COpGenius(void *pParam):COperator((stModuParam*)pParam)
{

}

COpGenius::~COpGenius()
{

}

void  COpGenius::Execute(CMoveShape* pShape)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "COpGenius::Execute() begin");
#endif
    if( !pShape )
        return ;
    if( m_pParam->SendGeniusList.find(pShape->GetExID()) != m_pParam->SendGeniusList.end() )
        return ;

    m_pParam->SendGeniusList.insert( pShape->GetExID() );

    long  nId = 0 ;
    if ( m_strGenius!="" )
    {
        nId = atol(m_strGenius.c_str());
    }
    m_pParam->pFirer->AddGeniusStates( m_pParam->nID , pShape ,nId );

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "COpGenius::Execute() end");
#endif
}

void AddOper(CMoveShape* pShape,string& strName,int nValue)
{
	if(pShape)
    {
		pShape->ChangeStateAttribute(strName,nValue);
    }
}
void SubOper(CMoveShape* pShape,string& strName,int nValue)
{
	if(pShape)
    {
		pShape->ChangeStateAttribute(strName,-nValue);
    }
}
void SetOper(CMoveShape* pShape,string& strName,int nValue)
{
	if(pShape)
    {
		pShape->SetStateAttribute(strName,nValue);
    }
}
void BoolOper(CMoveShape* pShape,string& strName,int nValue)
{
	if (pShape)
	{
		pShape->SetBoolAttribute(strName,static_cast<BOOL>(nValue));
	}
}

void UpdateOper(CMoveShape *pShape,string& strName,int nValue)
{
	nValue = 1;
	if (pShape)
	{
		pShape->UpdateAttribute(nValue);
	}
}

void ResetOper(CMoveShape *pShape,string& strName,int nValue)
{
    if(pShape)
        pShape->SetStateAttribute(strName,nValue);
#ifdef _SKILL_INFO_D_  
    Log4c::Trace(SKILL_MODULE, "ResetOper:属性%s=%d\n",strName.c_str(),nValue);    
#endif
}