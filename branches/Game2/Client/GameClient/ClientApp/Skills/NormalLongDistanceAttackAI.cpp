#include "stdafx.h"
#include "NormalLongDistanceAttackAI.h"
#include "../../EngineExtend/DisplayModel.h"
#include "../../EngineExtend/GameModelManager.h"
#include "PetSystem/Pet.h"
#include "ClientRegion.h"
#include "../GameClient/Game.h"
#include "../ClientApp/Player.h"
#include "SkillXml.h"
#include "Effect.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNormalLongDistanceAttackAI::CNormalLongDistanceAttackAI()
{
	m_pSkill = NULL;
	m_emFlyMode = FLY_TRACK;
}

CNormalLongDistanceAttackAI::CNormalLongDistanceAttackAI(CSkillXml* pSkill)
{
	m_pSkill = pSkill;
	m_emFlyMode = FLY_TRACK;
}

CNormalLongDistanceAttackAI::CNormalLongDistanceAttackAI(CSkillXml* pSkill, FLY_MODE mode)
{
	m_pSkill = pSkill;
	m_emFlyMode = mode;
}

CNormalLongDistanceAttackAI::~CNormalLongDistanceAttackAI()
{
	Release();
}
void CNormalLongDistanceAttackAI::Release()
{

	/*m_vecObjInfo.clear();

	m_vecFStartX.clear();
	m_vecFStartY.clear();
	m_vecFStartHeight.clear();

	m_vecFEndX.clear();
	m_vecFEndY.clear();
	m_vecFEndHeight.clear();

	m_vecDwFlyingTime.clear();
	m_vecDwStartTime.clear();*/
	m_veceffect.clear();
}
void CNormalLongDistanceAttackAI::AI()
{
	//add by 彭谦 2008 - 5 - 5
	if (m_veceffect.size() > 0)
	{
		bool DeleteAllEffect = true;
		for (vector<tagEffect>::iterator it = m_veceffect.begin();it != m_veceffect.end();it++)
		{
			if (!(*it).barrived)
			{
				DeleteAllEffect = false;
				break;
			}
		}
		if (DeleteAllEffect)
		{
			StepEndAI();
			return;
		}
		long dwTime = (long)GetCurTickCount();
		// 获得当前步骤
		static long  lType = TYPE_REGION;
		static CGUID guidObj = GetGame()->GetRegion()->GetExID();
		static CGUID guidEffect = CGUID::GUID_INVALID;
		// 获得当前飞行技能在场景中对应的技能列表
		//list<CSkillXml::tagEffectIndex> *lsEffectIndex = m_pSkill->GetObjectEffectList(m_pSkill->GetCurStep(),lType,guidObj);

		CEffect* pEffect = NULL;
		CMoveShape * pObjShape = NULL;
		//list<CSkillXml::tagEffectIndex>::iterator itIndex = lsEffectIndex->begin();
		for(size_t index = 0; index < m_veceffect.size();index++)
		{				
			if(m_veceffect[index].llasttime == 0)
			{
				m_veceffect[index].llasttime = dwTime;
				m_veceffect[index].lstarttime = dwTime;
				if(index < m_pSkill->GetFlyDelay().size())
					m_veceffect[index].ldelaytime = m_pSkill->GetFlyDelay()[index];
				if(index < m_pSkill->GetFlyTime().size())
					m_veceffect[index].lspeed = m_pSkill->GetFlyTime()[index];
				if(index < m_pSkill->GetFlyEffect().size())
					m_veceffect[index].dweffectID = m_pSkill->GetFlyEffect()[index];
				if(index < m_pSkill->GetFlyEffectIndex().size())
					m_veceffect[index].dweffectindex = m_pSkill->GetFlyEffectIndex()[index];
				if(index < m_pSkill->GetFlyEffectLoc().size())
					m_veceffect[index].dwlocator = m_pSkill->GetFlyEffectLoc()[index];
			}
			switch (m_emFlyMode)
			{
			case		FLY_TRACK:
				if ((long)dwTime - m_veceffect[index].lstarttime < m_veceffect[index].ldelaytime||m_veceffect[index].barrived)
				{
					m_veceffect[index].fstartx = 0;
					m_veceffect[index].fstarty = 0;
					m_veceffect[index].fstarth = 0;
					m_veceffect[index].llasttime = dwTime;
					continue;
				}
				else
				{
					if (m_veceffect[index].bgone == false)
					{
						// 获得最新的飞行开始位置
						eLOCATOR eLoc = LOCATOR_WIND;
						if(index < m_pSkill->GetFlyEffectLoc().size())
							eLoc = m_pSkill->GetFlyEffectLoc()[index];
						float x, y, fHeight;
						// 更新特效绑定点
						m_pMoveshape->GetLocatorPos(eLoc, x, y, fHeight);
						m_veceffect[index].bgone = true;
						m_veceffect[index].fstartx = x;
						m_veceffect[index].fstarty = y;
						m_veceffect[index].fstarth = fHeight;
					}

					//pEffect = dynamic_cast<CEffect*>(GetGame()->GetRegion()->FindChildObject(TYPE_EFFECT,itIndex->guidEffect));
					pObjShape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_veceffect[index].objinfo.lType,m_veceffect[index].objinfo.guid));
					if(pObjShape)
					{
						float fx, fy, fH;
						pObjShape->GetLocatorPos(CShape::LOCATOR_WIND, fx, fy, fH);
						// 计算特效位置
						long ustime = (long)(dwTime - m_veceffect[index].llasttime);
						m_veceffect[index].llasttime = dwTime;
						// 重新计算特效对各个目标的飞行时间
						float lDistence = FM_Distance(
							D3DXVECTOR3(m_veceffect[index].fstartx,
										m_veceffect[index].fstarth,
										m_veceffect[index].fstarty),
							D3DXVECTOR3(fx,fH,fy));
						if(lDistence <= 0.5f||ustime > 500)
						{
							m_veceffect[index].barrived = true;

							continue;
						}
						D3DXVECTOR3 vdir = D3DXVECTOR3(fx,fH,fy) - D3DXVECTOR3(m_veceffect[index].fstartx,m_veceffect[index].fstarth,m_veceffect[index].fstarty);
						D3DXVec3Normalize(&vdir,&vdir);
						float x = (fabs(vdir.x) < 0.0001?0.0f:((float)ustime  / (float)m_veceffect[index].lspeed * vdir.x)) + m_veceffect[index].fstartx;
						float y = (fabs(vdir.z) < 0.0001?0.0f:((float)ustime  / (float)m_veceffect[index].lspeed * vdir.z)) + m_veceffect[index].fstarty;
						float h = (fabs(vdir.y) < 0.0001?0.0f:((float)ustime  / (float)m_veceffect[index].lspeed * vdir.y)) + m_veceffect[index].fstarth;

						D3DXVECTOR3 ddir;
						D3DXVECTOR3 RotVector;
						D3DXVECTOR3 sdir(0.0f,0.0f,1.0f);
						D3DXMATRIX matrot,matrotY;
						//特效到目标的方向
						ddir = D3DXVECTOR3(fy,fH,fx) - D3DXVECTOR3(m_veceffect[index].fstarty,m_veceffect[index].fstarth,m_veceffect[index].fstartx);
						float angle = FM_AngleBetweenVectors(sdir,ddir);
						D3DXVec3Cross(&RotVector,&sdir,&ddir);
						D3DXMatrixRotationAxis(&matrot,&RotVector,angle);
						/*pEffect->Set3DRotMatrix(matrot);
						pEffect->Set3DRotEnable(true);
						pEffect->SetPosXY(x, y);
						pEffect->SetHeight(h);*/
						m_veceffect[index].matrot = matrot;
						m_veceffect[index].fstartx = x;
						m_veceffect[index].fstarty = y;
						m_veceffect[index].fstarth = h;
					}
					// 目标消失（特效也消失）
					else
					{
						m_veceffect[index].barrived = true;
						m_veceffect[index].fstartx = 0;
						m_veceffect[index].fstarty = 0;
						m_veceffect[index].fstarth = 0;
						m_veceffect[index].llasttime = dwTime;
					}
				}
				break;
			case		FLY_LOCK_LINE:
				{
					// 获得最新的飞行开始位置
					// 获得最新的飞行开始位置
					eLOCATOR eLoc = LOCATOR_WIND;
					if(index < m_pSkill->GetFlyEffectLoc().size())
						eLoc = m_pSkill->GetFlyEffectLoc()[index];
					float x, y, fHeight;
					// 更新特效绑定点
					m_pMoveshape->GetLocatorPos(eLoc, x, y, fHeight);
					m_veceffect[index].bgone = true;
					m_veceffect[index].fstartx = x;
					m_veceffect[index].fstarty = y;
					m_veceffect[index].fstarth = fHeight;
				}
				break;
			default:
				break;
			}
		}
	}
}

bool CNormalLongDistanceAttackAI::Display()
{
	if(!m_pSkill)
		return false;
	static long  lType = TYPE_REGION;
	static CGUID guidObj = GetGame()->GetRegion()->GetExID();
	static CGUID guidEffect = CGUID::GUID_INVALID;
	CEffect *pEffect = NULL;
	// 将所有的飞行特效添加到场景中
	// 获得当前飞行技能在场景中对应的技能列表
	bool bAsyn = true;
	// 自己是技能释放者
	if(GetGame()->GetMainPlayer() == m_pMoveshape)
		bAsyn = false;
	
	for(size_t index=0; index<m_veceffect.size(); ++index)
	{
		if (m_veceffect[index].bgone && !m_veceffect[index].bloadinregion)
		{					
			// 目标保存起来（目标个数不能超过特效的个数，如果超过，以特效个数为准）
			CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_veceffect[index].objinfo.lType, m_veceffect[index].objinfo.guid));
			pEffect = GetGame()->GetRegion()->PlayEffect(m_veceffect[index].dweffectID, m_veceffect[index].fstartx, 
			m_veceffect[index].fstarty, m_veceffect[index].fstarth, 0.0f,bAsyn);
			if(pEffect && pMoveShape)
			{
				/// 特效飞行的初始方向
				float fdir = GetLineDirF(m_veceffect[index].fstartx, m_veceffect[index].fstarty, pMoveShape->GetPosX(), pMoveShape->GetPosY());
				pEffect->SetDirEx(fdir);
				pEffect->SetNowDir(fdir);

				// 特效GUID
				guidEffect = pEffect->GetExID();
				m_veceffect[index].guidmyguid = guidEffect;
				// 特效目标绑定点
				long lHosterType, lHosterID;
				eLOCATOR eLoc = LOCATOR_RIGHTHAND;
				pMoveShape->GetCurGraphicTypeID(lHosterType, lHosterID);
				if(index < m_pSkill->GetFlyEffectLoc().size())
					eLoc = m_pSkill->GetFlyEffectLoc()[index];
				pEffect->SetHost(lType, lHosterType, lHosterID, guidObj, eLoc);
				pEffect->SetLoop(m_pSkill->IsFlyEffectLoop());
				if(lType != 0 && guidEffect != CGUID::GUID_INVALID && guidObj != CGUID::GUID_INVALID)
				{
					// 构造一个新的特效索引结构
					CSkillXml::tagEffectIndex stEffectIndex;
					stEffectIndex.guidEffect = guidEffect;
					stEffectIndex.lEffectIndex = m_veceffect[index].dweffectindex;
					// 在本技能步骤下，查找关于本场景的特效列表，如果找到就直接添加进去，否则，构造一个新的对象特效结构添加到本阶段列表中去
					list<CSkillXml::tagEffectIndex> *lsEffectIndex = m_pSkill->GetObjectEffectList(CSkillXml::STEP_RUN,lType,guidObj);
					if(lsEffectIndex)
					{
						lsEffectIndex->push_back(stEffectIndex);
					}
					else
					{
						vector<CSkillXml::tagObjectEffect> *vecObjEffect = m_pSkill->GetEffectIndex(CSkillXml::STEP_RUN);
						if(vecObjEffect)
						{
							CSkillXml::tagObjectEffect stObjEffect;
							stObjEffect.guid = guidObj;
							stObjEffect.lType = lType;
							stObjEffect.lsEffectIndex.push_back(stEffectIndex);
							vecObjEffect->push_back(stObjEffect);
						}
					}
					m_veceffect[index].bloadinregion = true;
				}
			}
		}
 		if (m_veceffect[index].bloadinregion )
		{
			pEffect = dynamic_cast<CEffect*>(GetGame()->GetRegion()->FindChildObject(TYPE_EFFECT,m_veceffect[index].guidmyguid));
			if (pEffect)
			{
				CMoveShape	*pSrouceShape = NULL;
				CMoveShape	* pTargetShape = NULL;
				render::Camera *pCamera = NULL;
				switch (m_emFlyMode)
				{
				case FLY_TRACK:
					{
						pEffect->Set3DRotMatrix(m_veceffect[index].matrot);
						pEffect->Set3DRotEnable(true);
						pEffect->SetPosXY(m_veceffect[index].fstartx, m_veceffect[index].fstarty);
						pEffect->SetHeight(m_veceffect[index].fstarth);
					}
					break;
				case	FLY_LOCK_LINE:						//根据施法者和被攻击Shap的位置计算更新特效骨骼变化。
					{
						//技能释放者
						pSrouceShape = m_pMoveshape;
						//攻击目标
						pTargetShape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_veceffect[index].objinfo.lType,m_veceffect[index].objinfo.guid));
						pCamera = GetGame()->GetRegion()->GetCamera();
						if(pTargetShape)
						{
							eLOCATOR eLoc = m_pSkill->GetReleaseEffectPosition();
							if(index < m_pSkill->GetFlyEffectLoc().size())
								eLoc = m_pSkill->GetFlyEffectLoc()[index];
							float target_x = 0.0f, target_y = 0.0f, target_height = 0.f, src_x = 0.0f, src_y = 0.0f, src_height = 0.0f;
							pTargetShape->GetLocatorPos(CShape::LOCATOR_WIND,target_x,target_y,target_height);
							pSrouceShape->GetLocatorPos(eLoc,src_x,src_y,src_height);

							D3DXVECTOR3 vecSrcPos = D3DXVECTOR3(src_x,src_y,src_height);
							D3DXVECTOR3 vecTargetPos = D3DXVECTOR3(target_x,target_y,target_height);
							if (pEffect->GetDisplayModel() && 
								pEffect->GetAnimInfo() && 
								pEffect->GetDisplayModel()->GetLoadState() == Load_Did)
							{
								GameModel::Action * pCurAction = CGameModelManager::GetInstance()->GetActionAnim(pEffect->GetDisplayModel()->GetGameModelID(), pEffect->GetAnimInfo()->GetActionInfo()->dwCurAction);
								if (pCurAction)
								{
									model::AnimJoint	*pAnimJoint = pCurAction->GetAnimJoint();
									DWORD dwJointCount = pAnimJoint->GetJointCount();
									pEffect->GetAnimInfo()->CaculateJointPos(&vecSrcPos,&vecTargetPos,dwJointCount);
								}
								pEffect->GetAnimInfo()->SetupAnimInfo(src_x,src_y,src_height,0.0f,pCamera);
							}
						}
						break;
					default:
						break;
					}
				}
			}
		}
	}

	// 获得释放者
	CMoveShape* pMoveshape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_pSkill->GetSourType(), m_pSkill->GetSourID()));
	if(pMoveshape)
	{
		if(m_pSkill->GetCurStep() == CSkillXml::STEP_START && m_pSkill->GetIntonateMainAct())
		{
			// 如果处于吟唱阶段，那么在前期吟唱动作播放完之后，循环播放主要的吟唱动作
			AnimInfo::tagActionInfo *pActionInfo = pMoveshape->GetAnimInfo()->GetActionInfo();
			if(pMoveshape && pActionInfo &&
				(pMoveshape->GetAction() == CShape::ACT_IDLE ||
				pMoveshape->GetAction() == CShape::ACT_STAND))
			{
				pMoveshape->SetAction(m_pSkill->GetIntonateMainAct());
				pActionInfo->bCurActionLooped = TRUE;
			}
		}
		// 释放阶段
		else if(m_pSkill->GetCurStep() == CSkillXml::STEP_RUN && m_pSkill->GetRunStepActExQueue())
		{
			// 释放阶段续动作列表不为空，且当前动作是站立，将当前动作设置为后续动作列表的第一个动作
			if(!m_pSkill->GetRunStepActExQueue()->empty() && pMoveshape->GetAction() == CShape::ACT_STAND)
			{
				pMoveshape->SetAction(m_pSkill->GetRunStepActExQueue()->front());
				m_pSkill->GetRunStepActExQueue()->pop();
				// 已经是最后一个动作
				if(m_pSkill->GetRunStepActExQueue()->empty())
				{
					AnimInfo::tagActionInfo *pActionInfo = pMoveshape->GetAnimInfo()->GetActionInfo();
					if(pActionInfo)
						pActionInfo->bCurActionLooped = m_pSkill->IsActExCycle();
				}
			}
		}
	}
	return true;
}

bool CNormalLongDistanceAttackAI::StepBeginAI(vector<long> vecType, vector<CGUID> vecID)
{
	return true;
}
bool CNormalLongDistanceAttackAI::StepRunAI(vector<long> vecType, vector<CGUID> vecID)
{
	long lType = TYPE_REGION;
	CGUID guidObj = GetGame()->GetRegion()->GetExID();
	CGUID guidEffect = CGUID::GUID_INVALID;
	// 删除该技能前一个阶段，在场景上的所有特效
	m_pSkill->DelAllEffectListByObject(CSkillXml::STEP_START,lType,guidObj);
	// 获得释放者
	m_pMoveshape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_pSkill->GetSourType(), m_pSkill->GetSourID()));
	if (m_pMoveshape == NULL)
		return false;
	//----判断目标个数与特效的最大个数，目标个数不能超过特效的最大个数
	// 否则，将按照特效的最大个数来针对攻击对象，顺序的添加特效(liuke07.11.2)----
	long lFlyEffectNum = (long)m_pSkill->GetFlyEffect().size();
	long lNum = 0;
	if(lFlyEffectNum > 0 && lFlyEffectNum < (long)vecType.size())
		lNum = lFlyEffectNum;
	else
		lNum = (long)vecType.size();
	if(lNum <= 0)
		return false;
	//-------------------------------------------------------
	for(int index=0; index<lNum; ++index)
	{
		// 目标保存起来（目标个数不能超过特效的个数，如果超过，以特效个数为准）
		CMoveShape* pMoveShape=NULL;
		pMoveShape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(vecType[0],vecID[0]));
		if(pMoveShape!=NULL)
		{
			tagObjectInfo stInfo;
			stInfo.lType = pMoveShape->GetType();
			stInfo.guid = pMoveShape->GetExID();
			tagEffect effect;
			effect.objinfo = stInfo;
			m_veceffect.push_back(effect);
		}
	}
	//====================================================
	return true;
}

// 由于飞行特效全部添加到场景中，所以在场景的AI循环中处理(07.12.19刘科修改)
bool CNormalLongDistanceAttackAI::StepEndAI()
{
	if(!m_pSkill)
		return false;
	CMoveShape* pMoveshape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_pSkill->GetSourType(), m_pSkill->GetSourID()));
	if(pMoveshape == GetGame()->GetMainPlayer())
	{
		AnimInfo::tagActionInfo *pActionInfo = pMoveshape->GetAnimInfo()->GetActionInfo();
		if(pActionInfo && pMoveshape->GetAction() >= CShape::ACT_PERFORM)
		{
			pActionInfo->bCurActionLooped = FALSE;
		}
	}
	if (pMoveshape->GetType() == TYPE_PET)
	{
		CPet *pPet = (CPet*)pMoveshape;
		if (pPet->IsMainPlayerPet())
		{
			tagPetSkillInfo *pPetSkill = pPet->GetPetSkillInfoByID(m_pSkill->GetSkillID());
			if(pPetSkill)
				pPetSkill->bIsSkillUsingEnd = true;
		}
	}
	// 删除该技能每一个阶段，在场景上的所有特效
	m_pSkill->DelAllEffectListByStep(CSkillXml::STEP_START);
	m_pSkill->DelAllEffectListByStep(CSkillXml::STEP_RUN);
	m_pSkill->DelAllEffectListByStep(CSkillXml::STEP_END);
	// 关闭飞行音效
    m_pSkill->StopFlySound();
	Release();
	m_pSkill->SetIsDeleted(true);
	//m_pMoveshape->SetSkill(NULL);
	return true;
}