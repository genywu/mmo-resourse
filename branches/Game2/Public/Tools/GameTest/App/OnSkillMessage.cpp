#include "stdafx.h"
#include "GameClient.h"
#include "RgnObjMgr.h"
#include "SkillMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



static const char szSkillUseRet[]	= "SkillUse";

void CGameClient::OnSkillMessage(CMessage* pMsg)
{
	long lType = pMsg->GetType();
	switch( lType )
	{
	case MSG_S2C_SKILL_USE:
		{
			//技能使用后服务器返回的S2C消息
			CHAR cRet = pMsg->GetChar();	//是否成功,如果是0表示成功
			switch( cRet )
			{
			case SKILL_USE_RESULT_FAILED:
				{
					long lType = pMsg->GetLong();
					CGUID guid;
					pMsg->GetGUID(guid);

					long lSkillID = pMsg->GetLong();		//使用的技能ID
					long lLevel = pMsg->GetLong();

					long lSkillKey = pMsg->GetLong();
					long lFaildRet = pMsg->GetLong();

					CMoveShape* pMoveShape = (CMoveShape*)m_pClientObjMgr->FindObject(guid);
					if( pMoveShape == m_pMainPlayer )
					{
						switch( lFaildRet)
						{
						case SKILL_USE_FAILED_INVALID_STATE:
							PutStringToFile(szSkillUseRet,"无效状态");
							break;
						case SKILL_USE_FAILED_INVALID_HP:
							PutStringToFile(szSkillUseRet,"Hp 不足");
							break;
						case SKILL_USE_FAILED_INVALID_MP:
							PutStringToFile(szSkillUseRet,"mp不够");
							break;
						case SKILL_USE_FAILED_INVALID_RP:
							PutStringToFile(szSkillUseRet,"Rp不足");
							break;
						case SKILL_USE_FAILED_INVALID_YP:
							PutStringToFile(szSkillUseRet,"yp不足");
							break;
						case SKILL_USE_FAILED_INVALID_TARGET:
							PutStringToFile(szSkillUseRet,"无效目标");
							break;
						case SKILL_USE_FAILED_INVALID_DISTANT:
							PutStringToFile(szSkillUseRet,"无效距离");
							break;
						case SKILL_USE_FAILED_INVALID_SKILL:
							PutStringToFile(szSkillUseRet,"无效技能");
							break;
						case SKILL_USE_FAILED_INVALID_TIME:
							PutStringToFile(szSkillUseRet,"冷却未到");
							break;
						case SKILL_USE_FAILED_INVALID_EQUIP:
							PutStringToFile(szSkillUseRet,"装备无效");
							break;
						case SKILL_USE_FAILED_BLOCKED:
							PutStringToFile(szSkillUseRet,"有阻挡");
							break;
						case SKILL_USE_FAILED_BREAK:
							PutStringToFile(szSkillUseRet,"休息一下");
							break;
						case SKILL_USE_FAILED_NOOBJECT:
							PutStringToFile(szSkillUseRet,"没有对象");
							break;
						case SKILL_USE_FAILED_INVALID_INTONATE:
							PutStringToFile(szSkillUseRet,"无效吟唱");
							break;
						case SKILL_USE_FAILED_INVALID_OCCUPATION:
							PutStringToFile(szSkillUseRet,"无效职业");
							break;
						default:
							PutStringToFile(szSkillUseRet,"采集错误");
							break;
						}
					}
				}
			}
		}
		break;
	case MSG_S2C_SKILL_STATE_CHANGE:
		{
			long lSocketID = pMsg->GetSocketID();
			LONG lType = pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			DWORD	dwHp = pMsg->GetDWord();
			WORD	wMp	 = pMsg->GetWord();
			WORD	eg = pMsg->GetWord();
			//
			if(lType == TYPE_PLAYER)
			{
				if( m_pMainPlayer->GetExID() == guid )
				{
// 					m_pMainPlayer->SetHp(dwHp);
// 					m_pMainPlayer->SetMp(wMp);
// 					m_pMainPlayer->SetEnergy(eg);
					if( dwHp <= 50)
					{
						CMessage* pSendMsg = (CMessage*)CBaseMessage::AllocMsg();
						pSendMsg->Reset();
						pSendMsg->Init(MSG_C2S_OTHER_TALK);

						pSendMsg->Add((BYTE)0);
						pSendMsg->Add((long)RUN_SCRIPT);
						pSendMsg->Add((long)TYPE_PLAYER);
						pSendMsg->Add(guid);
						pSendMsg->Add((long)0);
						pSendMsg->Add("SetMe(\"E_Hp\",60000");
						pSendMsg->SendBySocket(lSocketID);
						CBaseMessage::FreeMsg(pSendMsg);
					}
					if( wMp <= 50)
					{
						CMessage* pSendMsg = (CMessage*)CBaseMessage::AllocMsg();
						pSendMsg->Reset();
						pSendMsg->Init(MSG_C2S_OTHER_TALK);

						pSendMsg->Add((BYTE)0);
						pSendMsg->Add((long)RUN_SCRIPT);
						pSendMsg->Add((long)TYPE_PLAYER);
						pSendMsg->Add(guid);
						pSendMsg->Add((long)0);
						pSendMsg->Add("SetMe(\"E_Mp\",60000");
						pSendMsg->SendBySocket(lSocketID);
						CBaseMessage::FreeMsg(pSendMsg);
						pSendMsg->Reset();
					}
				}
			}
		}
		break;
	case MSG_S2C_OCCUSKILL_STUDY_BEGIN:
		{
			return;
			m_pMainPlayer->GetSkillMgr()->ClearOccuSkill();
//			(m_pMainPlayer->GetAllOccuSkillID()).clear();	//清空PLAYER中保存可以学习的副职业生产技能的技能ID容器
			CGUID npcguid;
			pMsg->GetGUID(npcguid);	//npc的GUID

			long num = pMsg->GetDWord();	//num为所有的生产技能的数目
//			m_pMainPlayer->SetOccuSkillNum(num);	
			for(DWORD i=0; i<num; ++i)
			{
				DWORD dwSkillID = pMsg->GetDWord();	//技能ID
				DWORD dwSkillCost = pMsg->GetDWord();	//学习技能所需要花费的金币

				m_pMainPlayer->GetSkillMgr()->AddOccuSkill(dwSkillID);
//				m_pMainPlayer->SetAllOccuSkillID(dwSkillID);
			}
		}
		break;
	case MSG_S2C_OCCUSKILL_STUDY:
		{
			long lBeSucceed = pMsg->GetLong();	//是否成功,0为成功
			DWORD dwFactureID = pMsg->GetDWord();	//生产技能的id
			if (lBeSucceed==1)
			{
//				m_pMainPlayer->SetOccuSkillHaveStudied();
			}
		}
		break;
	case MSG_S2C_SKILL_ADD_PET:
		{
			//CGUID guid;
			//pMsg->GetGUID(guid);

			//tagPetSkill skill;
			//pMsg->GetEx(&skill,sizeof(tagPetSkill));
			//pMsg->GetLong();
			//pMsg->GetLong();
			//pMsg->GetLong();
			//pMsg->GetLong();

			//CPet* pPet = dynamic_cast<CPet*>(m_pClientObjMgr->FindObject(TYPE_PET,guid));
			//if( pPet )
			//{
			//	if( skill.lPos == 0 )
			//		skill.bAuto = true;
			//	pPet->AddSkill(skill);
			//}
		}
		break;
	default:
		break;
	}
}