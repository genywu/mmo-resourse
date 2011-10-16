#include "stdafx.h"
#include "..\Player.h"
#include "../Pet.h"
//#include "../Skills/Skill.h"
#include "../BuffSkill/NewSkillBaseProperties.h"
#include "../BuffSkill/NewSkillFactory.h"
#include "..\nets\netserver\message.h"
#include "../ServerRegion.h"
#include "..\script\script.h"
#include "../../public/SkillRelated.h"
#include "../../public/StudySkillCondition.h"
#include "../../public/DeputyOccu.h"
#include "..\..\AppServer\Collection.h"

//返回技能失败消息
void OnSkillFailedMessage(CMessage* pMsg,long lID,const CGUID &pPlayerGuid)
{

#ifdef _SKILL_DEBUG
	char str[64]="";
	_snprintf(str,64, "SkillMessage发送技能失败消息:技能ID:%6d\n",lID);
	PutStringToFile("SKill.log",str);
#endif
	CMessage msg(MSG_S2C_SKILL_USE);
	msg.Add((CHAR)SKILL_USE_RESULT_FAILED);
	msg.Add( pPlayerGuid);
	//id和等级
	msg.Add((long)lID);
	msg.Add((BYTE)0);
	//全局id
	msg.Add((DWORD)0);
	//错误类型
	msg.Add((BYTE)SKILL_USE_FAILED_INVALID_UNKNOWN);
	msg.SendToSocket(pMsg->GetSocketID());

}

// 响应SKILL消息
void OnSkillMessage(CMessage* pMsg)
{
	CPlayer* pPlayer = pMsg->GetPlayer();
	CPet* pPet = NULL;
	if( NULL == pPlayer ) return;

	switch(pMsg->GetType())
	{
	case MSG_C2S_SKILL_USE:
		/**## 格式： */
		{
			LONG lID		= static_cast<LONG>( ( pMsg -> GetDWord() ) & 0x7fffffff );
			LONG lSkillLevel	= 0;
			LONG lTargetType	= pMsg -> GetLong();
			CGUID TargetID;
			pMsg -> GetGUID(TargetID);
			LONG lTargetX		= pMsg -> GetLong();
			LONG lTargetY		= pMsg -> GetLong();
			long lGoodsIndex    = 0;
			long lContainerID =	 0;
			long lPos		  =  0;
			CGUID GoodsGuid	  = NULL_GUID;

#ifdef _RUNSTACKINFO_
			char pszStackInfo[1024]="";
			_snprintf(pszStackInfo,1024,"Use Skill(PlayerName:%s,SkillID:%d,Level:%d,TargetType:%d)",pPlayer->GetName(),lID,lSkillLevel,lTargetX);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif

			if (pPlayer && pPlayer->JudgeSkilllegality())
			{
				return;	
			}

			BYTE bType			= pMsg->GetByte();
			if (bType==1)
			{
				lContainerID = pMsg->GetLong();
				lPos		  = pMsg->GetLong();
				CGoods *pGoods = pPlayer->FindGoods(lContainerID,lPos);
				if (NULL==pGoods || pGoods->GetAddonPropertyValues(GAP_ADD_SKILL, 1)!=lID)
				{
					OnSkillFailedMessage(pMsg,lID,pPlayer->GetExID());
					return;
				}

				

				if(pPlayer)
				{
					CPlayer::EquipRestrict eRet = (CPlayer::EquipRestrict)(pPlayer->CanUseItem(pGoods));
					if (eRet!=CPlayer::EQUIPFIT )
					{
						//物品不符合使用条件
						OnSkillFailedMessage(pMsg,lID,pPlayer->GetExID());

						CMessage msg(MSG_S2C_PLAYER_USEITEM);
						msg.Add((char)PLAYER_USEITEM_FAILED);
						msg.Add((char)eRet);
						msg.SendToPlayer(pPlayer->GetExID());
						return;
					}

				}
				lGoodsIndex = pGoods->GetBasePropertiesIndex();
				GoodsGuid   = pGoods->GetExID();
			}		

#ifdef _SKILL_DEBUG
			char str[64]="";
			_snprintf(str, 64, "SkillMessage接收技能使用消息:技能ID:%6d\n",lID);
			PutStringToFile("SKill.log",str);
#endif


			if( pPlayer )
			{				
				lSkillLevel = pPlayer ->CheckSkill( lID );
				if( lSkillLevel == 0 )
				{
					if (bType==1 && GoodsGuid!=NULL_GUID)
					{
						lSkillLevel = 1;
					}
					else if (CStudySkillCondition::CheckFactureSkill(lID))
					{
						lSkillLevel = 1;
					}
					else
					{
						OnSkillFailedMessage(pMsg,lID,pPlayer->GetExID());
						return;
					}			
					
				}

				CBaseAI* pAI = pPlayer -> GetAI();
				if( pAI )
				{						

					CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pPlayer -> GetFather() );
					if( pRegion )
					{

						CMoveShape* pTarget = dynamic_cast<CMoveShape*>( pRegion -> 
							FindChildObject(lTargetType, TargetID) );

						if( pTarget || lTargetX!=0 || lTargetY!=0)
						{

							if(!pPlayer->AddActiveSkill(lID,lSkillLevel,pTarget,lGoodsIndex,lContainerID,lPos,GoodsGuid))
							{
								return;
							}
							long lActID=0;
							pAI -> Attack( lActID,lTargetX,lTargetY, pTarget );
						}
						else
						{
							OnSkillFailedMessage(pMsg,lID,pPlayer->GetExID());
						}
					}
					else
					{
						OnSkillFailedMessage(pMsg,lID,pPlayer->GetExID());
					}
				}
				else
				{
					OnSkillFailedMessage(pMsg,lID,pPlayer->GetExID());
				}

			}
			else
			{
				OnSkillFailedMessage(pMsg,lID,pPlayer->GetExID());
			}
		}
		break;
	case MSG_C2S_DOCCUSKILL_USE:
		{

			LONG lType		= static_cast<LONG>( ( pMsg -> GetDWord() ) & 0x7fffffff );
			LONG lSkillLevel	= 0;
			LONG lTargetType	= pMsg -> GetLong();
			CGUID TargetID;
			pMsg -> GetGUID(TargetID);
			LONG lTargetX		= 0;
			LONG lTargetY		= 0;
			LONG lID		= pMsg -> GetLong();

			pPlayer->ProduceEvent(ET_UseSkill);
			if( pPlayer==NULL )	
				return ;


			DWORD dwSkillID= CDeputyOccuSystem::GetDeputyOccuSkillID((CDeputyOccuSystem::OccuBeavioral)lType,(eDeputyOccu)pPlayer->GetDOccupation());
			if (dwSkillID== 0)	return ;
			if (lType== CDeputyOccuSystem::OB_Facture)
			{
				pPlayer->SetElectFactureID(lID);
			}

			lSkillLevel = pPlayer ->CheckSkill( dwSkillID );
			if( lSkillLevel == 0 )
			{
				lSkillLevel = 1;

			}
			//else
			{



				CBaseAI* pAI = pPlayer -> GetAI();
				if( pAI )
				{
					CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pPlayer -> GetFather() );
					if (pRegion==NULL)
					{
						return ;
					}
					CMoveShape* pTarget = dynamic_cast<CMoveShape*>( pRegion -> 
						FindChildObject(lTargetType, TargetID) );

					if( pTarget || lTargetX!=0 || lTargetY!=0 || lType==CDeputyOccuSystem::OB_Facture)
					{
						//任务采集动作090310 by tanglei
						if (pTarget != NULL)
						{
							const CCollectionLayout* pLayout = CDeputyOccuSystem::GetCollectionLayout(pTarget->GetCollectionName().c_str());
							if ( pLayout != NULL )
							{
								if (pLayout->GetOccuID() == DOccu_Other && pLayout->GetActionID() != 1)
								{
									lSkillLevel = pLayout->GetActionID();
								}
							}
						}//090310
						
						if(!pPlayer->AddActiveSkill(dwSkillID,lSkillLevel))
						{
							return;
						}
						long ActID = 0;

						pAI -> Attack( ActID,lTargetX,lTargetY, pTarget);
					}
					else
					{
						OnSkillFailedMessage(pMsg,lID,pPlayer->GetExID());
					}					
				}
			}

		}
		break;
	case MSG_C2S_SKILL_USE_END:
		{
			long lKeyID = pMsg->GetLong();
			long lID = pMsg->GetLong();
			pPlayer->ProduceEvent(ET_BreakSkill);
			pPlayer->ProduceEvent(ET_EventBreak);
			pPlayer->EndActiveSkill(lID,lKeyID);
		}
		break;
	case MSG_C2S_SKILL_STUDY_BEGIN:
		{

			string strName="战士导师";
			pPlayer->SendBackSkill(strName);		
		}
		break;
		// 学习技能
	case MSG_C2S_SKILL_STUDY:
		{
			long lSkillID = pMsg->GetLong();
			DWORD dwLv = 0;
			if (pPlayer->CheckStudySkill(lSkillID,dwLv))
			{
				pPlayer->StudySkill(lSkillID,dwLv);
			}
		}
		break;
	case MSG_C2S_OCCUSKILL_STUDY_BEGIN:// 向服务器发送打开副职业技能学习页面
		{
			CGUID NpcGuid;
			pMsg -> GetGUID(NpcGuid);
			if (pPlayer!=NULL)
			{
				CServerRegion* pRegion= dynamic_cast<CServerRegion*>(pPlayer->GetFather());
				if (pRegion!=NULL)
				{
					CMoveShape *moveshape=dynamic_cast<CMoveShape*>(pRegion->FindChildObject(TYPE_MONSTER, NpcGuid));
					pPlayer->FactureSkillStudyBegin(moveshape);
				}
			}

		}
		break;
	case MSG_C2S_BUFF_END:
		{
			//状态结束
			DWORD dwStateID = pMsg->GetDWord();
			pPlayer->EndBuff(dwStateID);
		}
		break;
	case MSG_C2S_OCCUSKILL_STUDY:// 向服务器发送学习副职业技能消息
		{
			CGUID NpcGuid;
			DWORD FactureID=pMsg->GetLong();
			pMsg->GetGUID(NpcGuid);
			CServerRegion* pRegion= dynamic_cast<CServerRegion*>(pPlayer->GetFather());
			if (pRegion!=NULL)
			{
				CMoveShape *moveshape=dynamic_cast<CMoveShape*>(pRegion->FindChildObject(TYPE_MONSTER, NpcGuid));
				pPlayer->StudyFactureSkill(moveshape,FactureID);
			}
		}
		break;

	case MSG_C2S_SKILL_DEL_PET:
		{
		}
		break;

	case MSG_C2S_SKILL_USE_PET:
		{
			long lType = pMsg->GetLong();
			pPet = pPlayer->GetActivePet(lType);
			if( NULL == pPet )
			{
				char szLog[512] = "";
				_snprintf(szLog, 512,
					"宠物技能使用失败: 玩家%s没有类型为%ld的激活宠物.",
					pPlayer->GetName(),
					lType);
				PutoutLog("Pet", LT_ERROR, szLog);
				OnSkillFailedMessage(pMsg, lType,pPlayer->GetExID());
				break;
			}

			long lPos = pMsg->GetLong();
			tagSkill* pSkill = pPet->GetSkill(lPos);
			if( NULL == pSkill )
			{
				char szLog[512] = "";
				_snprintf(szLog, 512,
					"宠物技能使用失败: 玩家%s类型为%ld的宠物位置%ld没有技能.",
					pPlayer->GetName(),
					lType,
					lPos);
				PutoutLog("Pet", LT_ERROR, szLog);
				OnSkillFailedMessage(pMsg, lType,pPlayer->GetExID());
				break;
			}

			long lTargetType = pMsg->GetLong();
			CGUID TargetID;
			pMsg->GetGUID(TargetID);
			long lTargetX = pMsg->GetLong();
			long lTargetY = pMsg->GetLong();

			CServerRegion* pRegion = dynamic_cast<CServerRegion*>(pPet->GetFather());
			if( NULL == pRegion )
			{
				OnSkillFailedMessage(pMsg, pSkill->lID,pPlayer->GetExID());
				break;
			}

			CMoveShape* pTarget = dynamic_cast<CMoveShape*>(pRegion->FindChildObject(lTargetType, TargetID));
			if( pTarget != NULL && lTargetX != 0 && lTargetY != 0 )
			{
				pPet->UseSkill(pSkill, pTarget);
			}
			else
			{
				OnSkillFailedMessage(pMsg, pSkill->lID,pPlayer->GetExID());
			}
		}
		break;
	}
}
