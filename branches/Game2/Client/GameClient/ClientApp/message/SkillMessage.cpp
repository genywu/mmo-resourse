#include "stdafx.h"
#include "Player.h"
#include "Message.h"
#include "Effect.h"
#include "ClientRegion.h"
#include "Skills/SkillXml.h"
#include "Skills/SkillListIni.h"
#include "../GameClient/GameControl.h"
#include "Other/AudioList.h"
#include "MainPlayerHand.h"
#include "PetSystem/Pet.h"
#include "Other/DeputyOccuSystemClient.h"
#include "../GameClient/Game.h"
#include "../../../Public/Common/SkillRelated.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern void UpdateHotKeyCoolDownEffect(long lSkillID);
// 响应SKILL消息
void OnSkillMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_S2C_SKILL_LOCK_GOODS:
		{
			long lExtendID = pMsg->GetLong();
			long lPos = pMsg->GetLong();
			CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
			if (pMainPlayer!=NULL)
			{
				CGoods* pGoods = pMainPlayer->GetGoodsByPosition(lExtendID,lPos);
				if (pGoods!=NULL&&!pGoods->IsLocked())
				{
					pGoods->Lock();
					//CItemPageEx* pPage = GetGame()->GetCGuiEx()->GetItemPageEx();
					//if (pPage!=NULL)
					//{
					//	pPage->UpdateGoodsShow(lExtendID,(int)lPos);
					//}
				}
			}
		}
		break;
	case MSG_S2C_SKILL_UNLOCK_GOODS:
		{
			DWORD lExtendID = pMsg->GetDWord();
			DWORD lPos = pMsg->GetDWord();
			CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
			if (pMainPlayer!=NULL)
			{
				CGoods* pGoods = pMainPlayer->GetGoodsByPosition(lExtendID,lPos);
				if (pGoods!=NULL&&pGoods->IsLocked())
				{
					pGoods->UnLock();
					CMainPlayerHand* pHand = 0;//GetGame()->GetCGuiEx()->GetPMainPlayerHand();
					if (pHand!=NULL&&pHand->GetPGoodsOfMainPlayerHand()!=NULL&&
						pHand->GetPGoodsOfMainPlayerHand()->GetExID()==pGoods->GetExID())
					{
						pGoods->SetHaveShadowState(true);
					}
				}
			}
		}
		break;
	case MSG_S2C_SKILL_STATE_CHANGE:
		//##同步状态消息
		{
			LONG lType	= pMsg -> GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);
			DWORD dwHP	= pMsg -> GetDWord();
			WORD wMP	= pMsg -> GetWord();
			WORD eg		= pMsg -> GetWord();
			if(lType==TYPE_PLAYER)
			{
				CPlayer* pMainPlayer = pMsg -> GetPlayer();
				//是自己则改变自己
				if(pMainPlayer->GetExID() == ID)
				{
					pMainPlayer -> SetHp( dwHP );
					pMainPlayer -> SetMp(wMP);
					pMainPlayer -> SetEnergy(eg);
					//pMainPlayer -> SetRP( wRP );
					//pMainPlayer -> SetYP( wYP );
				}
				else
				{
					//看是否是队友
					vector<CPlayer::tagTeammate>& vTeammate = pMainPlayer -> GetTeam();
					for( size_t i = 0; i < vTeammate.size(); i ++ )
					{
						if( vTeammate[i].ID == ID )
						{
							vTeammate[i].wHP = (WORD)dwHP;
							break;
						}
					}
				}
			}
			else
			{
				//改变怪物状态
				CMoveShape* pShape = (CMoveShape*)GetGame()->GetRegion()->FindChildObject(lType, ID);
				if (pShape)
				{
					pShape->SetHp(dwHP);
				}
			}
		}
		break;
	case MSG_S2C_SKILL_USE:
		{
			CHAR cResult = pMsg->GetChar();
			DWORD dwCurTime = GetCurTickCount();
			switch(cResult)
			{
			case SKILL_USE_RESULT_BEGIN:
				{
					CGUID ID;
					pMsg->GetGUID(ID);						

					float fDir = pMsg->GetFloat();			// 释放者朝向

					long lSkillKey = pMsg->GetDWord();		// 从服务器取得技能键值和ID
					long lSkillID = pMsg -> GetLong();
					long lLevel = pMsg->GetByte();			// 技能等级
					long lCurType = pMsg->GetByte();		// 技能的释放状态（吟唱/释放）
					long lTime = pMsg->GetLong();			// 技能的吟唱时间（吟唱阶段有用）
					long lRestoreTime = pMsg->GetLong();	// 技能的冷却时间

					long lDestX = pMsg->GetShort();			// 释放目标的坐标(x,y)
					long lDestY = pMsg->GetShort();	
					long lTypeSize = pMsg->GetByte();		// 释放目标的个数
					vector<long> vecType;					// 目标类型列表
					vector<CGUID> vecID;					// 目标类型的GUID
					CGUID destID;
					for (int i =0; i<lTypeSize; i++)
					{
						vecType.push_back(pMsg->GetLong());
						pMsg->GetGUID(destID);
						vecID.push_back(destID);
					}
					// 确定基本攻击技能
					if(lSkillID == SKILL_BASE_BUFF)
						lSkillID = GetGame()->GetMainPlayer()->GetDefaultAttackSkillID();
					// 技能释放者
					CMoveShape* pMoveShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(ID);
					if(pMoveShape)
					{
						long lType = pMoveShape->GetType();
						//判断是否是怪物，如果是怪物，播放怪物音效
						if(lType == TYPE_MONSTER)
						{
							GetGame()->GetAudioList()->Play(pMoveShape,TYPE_MONSTER,0xFFF0,Vector3(pMoveShape->GetPosY(),pMoveShape->GetHeight(),pMoveShape->GetPosX()));
						}

						if(lCurType == 1)		// 吟唱阶段
						{
							CPlayer::tagSkill *ptagS = GetGame()->GetMainPlayer()->GetSkill(lSkillID);
							if(ptagS)
							{
                                UpdateHotKeyCoolDownEffect(lSkillID);
							}
							if(pMoveShape->GetType() == TYPE_PET)
							{
								CPet *pPet = (CPet*)pMoveShape;
								//更新当前技能的开始冷却时间
								tagPetSkillInfo *pSkill = pPet->GetPetSkillInfoByID(lSkillID);
								if(pSkill)
									pSkill->lRestorTimeLength = lRestoreTime;

								pPet->SkillSuccReceived();
							}
							/***********************************************************************/
							/* 赵航 fix */
							//CSkillXml *m_pSkill = new CSkillXml();
							/***********************************************************************/
							ConcreteSkillComp* SkillCom = new ConcreteSkillComp();
							//SkillCom的内存交给了CSkillXml的析构来管理
							CSkillXml* pSkill = new ConcreteDecSkillXml(SkillCom); 
							CSkillListXml::tagSkill *pTagSkill = CSkillListXml::GetProperty(lSkillID);
							if (pTagSkill && pTagSkill->dwSkillType == CSkillListXml::SKILLTYPE_PLAY)	// 直接施放
							{
								// 引导类技能添加吟唱条显示时间
								DWORD dwlev = pTagSkill->vectorLevels.size() >= (DWORD)lLevel? (DWORD)lLevel-1:((DWORD)pTagSkill->vectorLevels.size() - 1);
								long lAiID = pTagSkill->vectorLevels[dwlev].dwSkillLevelAI;
								if(pTagSkill->bIsLead)
									pSkill->IntonateTime(lTime);
								// 直接从开始阶段执行
								if(pSkill->StepBegin(lSkillID, lLevel, lType, ID, lDestX, lDestY,fDir,vecType, vecID))
								{
									pSkill->SetSkillKey(lSkillKey);
									pMoveShape->AddCurSkillList(lSkillKey,pSkill);
								}
								else
									SAFE_DELETE(pSkill);
								// 如果开始阶段成功就直接执行施放阶段
								if(pSkill&&pSkill->StepRun(lSkillID, lLevel, lType, ID, lDestX, lDestY, fDir, vecType, vecID) == false)
								{
									//pSkill->SetIsDeleted(true);
									pMoveShape->DeleteCurSkill(lSkillKey);
								}
								if(lType == TYPE_PLAYER )
								{
									if (GetGame()->GetMainPlayer()->GetExID()==ID)
									{
										GetGame()->GetGameControl()->SetControlAble(true);
										GetGame()->GetGameControl()->SetMoveAble(true);
										/***********************************************************************/
										/* zhaohang fix */
										/***********************************************************************/
										// 引导类技能
										if(pTagSkill->bIsLead)
											GetGame()->GetMainPlayer()->SetIntonateSkillKey(lSkillKey);
										else
											GetGame()->GetMainPlayer()->SetIntonateSkillKey(0);
										// 自动攻击累计发送次数清0
										GetGame()->GetGameControl()->SetSendedUseSkillMsgCount(0);
										if(g_bDebug)
										{
											Log4c::Trace(ROOT_MODULE,"直接使用技能，次数清0：%d,key:%d",lSkillID,lSkillKey);
										}
									}else if (lSkillID>=50000&&lSkillID<=50006)
									{
										if (lSkillID>=50000&&lSkillID<=50002)
										{
											((CPlayer*)pMoveShape)->SetToolsID(CDeputyOccuSystemClient::OB_Collection);
										}else if (lSkillID>=50003&&lSkillID<=50005)
										{
											((CPlayer*)pMoveShape)->SetToolsID(CDeputyOccuSystemClient::OB_Facture);
										}else if (lSkillID==50006)
										{
											((CPlayer*)pMoveShape)->SetToolsID(CDeputyOccuSystemClient::OB_TaskCollection,lLevel);
										}
									}
									/////////////////////////////////////////////////////////////////
									//char strInfo[512]="";
									//_snprintf(strInfo,512,"SKILLTYPE_PLAY_Set Intonate Skill Key:%d",0);
									//PutStringToFile("IntonateSkillTest",strInfo);
									/////////////////////////////////////////////////////////////////
								}
								else if (lType == TYPE_PET)
								{
										CPet *pPet = (CPet*)pMoveShape;
										if (pPet->IsMainPlayerPet())
										{
											/***********************************************************************/
											/* zhaohang fix */
											/***********************************************************************/
											if(pTagSkill->bIsLead)
												pPet->SetIntonateSkillKey(lSkillKey);
											else
												pPet->SetIntonateSkillKey(0);
										}
								}
							}
							else if(pTagSkill && pTagSkill->dwSkillType == CSkillListXml::SKILLTYPE_INTONATE) // 吟唱施放
							{
								pSkill->IntonateTime(lTime);
								if(g_bDebug && lType == TYPE_PLAYER)
								{
									dwCurTime = GetCurTickCount();
									Log4c::Trace(ROOT_MODULE,"吟唱技能设置时间:%d,%d,%d",lSkillID,lTime,dwCurTime);
								}
								/***********************************************************************/
								/* zhaohang fix */
								/***********************************************************************/
								bool bIsLead = pTagSkill->bIsLead;
								pTagSkill->bIsLead = false;
								if(pSkill->StepBegin(lSkillID, lLevel, lType, ID, lDestX, lDestY,fDir,vecType, vecID))
								{
									/***********************************************************************/
									/* zhaohang fix */
									/***********************************************************************/
									pTagSkill->bIsLead = bIsLead;
									pSkill->SetSkillKey(lSkillKey);
									pMoveShape->AddCurSkillList(lSkillKey,pSkill);
								}
								else
									SAFE_DELETE(pSkill);
								if( lType == TYPE_PLAYER )
								{
									if (GetGame()->GetMainPlayer()->GetExID()==ID)
									{
										// 吟唱阶段允许玩家移动取消技能
										GetGame()->GetGameControl()->SetControlAble(true);
										GetGame()->GetGameControl()->SetMoveAble(true);
										GetGame()->GetMainPlayer()->SetIntonateSkillKey(lSkillKey);

										// 自动攻击累计发送次数清0
										GetGame()->GetGameControl()->SetSendedUseSkillMsgCount(0);
										/*if(g_bDebug)
										{
											char strInfo[512]="";
											_snprintf(strInfo,512,"技能吟唱中，次数清0：%d,key:%d",lSkillID,lSkillKey);
											PutStringToFile("SkillTimesTest",strInfo);
										}*/
									}else if (lSkillID>=50000&&lSkillID<=50006)
									{
										if (lSkillID>=50000&&lSkillID<=50002)
										{
											((CPlayer*)pMoveShape)->SetToolsID(CDeputyOccuSystemClient::OB_Collection);
										}else if (lSkillID>=50003&&lSkillID<=50005)
										{
											((CPlayer*)pMoveShape)->SetToolsID(CDeputyOccuSystemClient::OB_Facture);
										}else if (lSkillID==50006)
										{
											((CPlayer*)pMoveShape)->SetToolsID(CDeputyOccuSystemClient::OB_TaskCollection,lLevel);
										}
									}

									/////////////////////////////////////////////////////////////////
									//char strInfo[512]="";
									//_snprintf(strInfo,512,"SKILLTYPE_INTONATE_Set Intonate Skill Key:%d",lSkillKey);
									//PutStringToFile("IntonateSkillTest",strInfo);
									/////////////////////////////////////////////////////////////////
								}
							}
							else
							{
								SAFE_DELETE(pSkill);
								return;
							}
							pMoveShape->SetDirEx(fDir);
							// 更新快捷栏的冷却动画
							if(pMoveShape==GetGame()->GetMainPlayer())
							{
								GetGame()->GetMainPlayer()->UpdateSkillCoolDownTime(true);
								//CSkillListXml::tagSkill *pS = CSkillListXml::GetProperty(lSkillID);
								/***********************************************************************/
								/* zhaohang fix */
								/***********************************************************************/
								if(ptagS && pTagSkill && pTagSkill->dwSkillState != 0 && !pTagSkill->bLockSkill)
								{
									ptagS->dwUseingStartTime = timeGetTime();
								}
							}

						}
						else					// 施放阶段
						{
							/***********************************************************************/
							/* 赵航 fix */
							//CSkillXml *m_pSkill = new CSkillXml();
							/***********************************************************************/
							ConcreteSkillComp* SkillCom = new ConcreteSkillComp();
							//SkillCom的内存交给了CSkillXml的析构来管理
							CSkillXml* pSkill = new ConcreteDecSkillXml(SkillCom); 
							pSkill->IntonateTime(lTime);
							if(pSkill&&pSkill->StepRun(lSkillID, lLevel, lType, ID, lDestX, lDestY, fDir, vecType, vecID) )
							{
								pSkill->SetSkillKey(lSkillKey);
								pMoveShape->AddCurSkillList(lSkillKey,pSkill);
								CSkillListXml::tagSkill *pTagSkill = CSkillListXml::GetProperty(lSkillID);
								// 引导类技能
								if(pTagSkill && pTagSkill->bIsLead)
									GetGame()->GetMainPlayer()->SetIntonateSkillKey(lSkillKey);
							}
							else
							{
								SAFE_DELETE(pSkill);
							}

							if(lType == TYPE_PLAYER )
							{
								if (GetGame()->GetMainPlayer()->GetExID()==ID)
								{
									GetGame()->GetGameControl()->SetControlAble(true);

									GetGame()->GetMainPlayer()->SetIntonateSkillKey(0);

									// 自动攻击累计发送次数清0
									GetGame()->GetGameControl()->SetSendedUseSkillMsgCount(0);
									if(g_bDebug)
									{
										Log4c::Trace(ROOT_MODULE,"吟唱技能释放阶段，次数清0，吟唱清空：%d,key:%d",lSkillID,lSkillKey);
									}
								}
								if (lSkillID>=50000&&lSkillID<=50006)
								{
									((CPlayer*)pMoveShape)->SetToolsID(0);
								}
								/////////////////////////////////////////////////////////////////
								//char strInfo[512]="";
								//_snprintf(strInfo,512,"STEP_PLAY Set Intonate Skill Key:%d",0);
								//PutStringToFile("IntonateSkillTest",strInfo);
								/////////////////////////////////////////////////////////////////
							}
						}
					}
					else
					{
						/////////////////////////////////////////////////////////////start
						//test add or delete shape,(author:wangqiao)
						char pszTestInfo[1024]="";
						char pszGUID[64]="";
						ID.tostring(pszGUID);
						//_snprintf(pszTestInfo,1024,"When Use skill,the Firer is not exist.(type:%d,id:%s)",
						//	lType,pszGUID);
						//PutStringToFile("AddDelShapeTest",pszTestInfo);
						////////////////////////////////////////////////////////////end
					}
				}
				break;
			case SKILL_USE_RESULT_END:
				{
					CGUID ID;
					pMsg->GetGUID(ID);
					float fDir = pMsg->GetFloat();
					long lSkillKey = pMsg->GetDWord();				// 从服务器取得技能键值

					long lSkillID = pMsg -> GetLong();
					long lLevel = pMsg->GetByte();

					long lCurType = pMsg->GetByte();
					CMoveShape* pMoveShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(ID);
					if(pMoveShape)
					{
						long lType = pMoveShape->GetType();
						CSkillXml* pSkill = pMoveShape->GetCurSkill(lSkillKey);
						if(pSkill)
						{
							pSkill->StepEnd(lSkillID, lLevel, lType, ID, fDir);
							pMoveShape->DeleteCurSkill(lSkillKey);
						}

						if(lType == TYPE_PLAYER )
						{
							if (lSkillID>=50000&&lSkillID<=50006)
							{
								((CPlayer*)pMoveShape)->SetToolsID(0);
							}
							if(GetGame()->GetMainPlayer()->GetExID() == ID)
							{
								// 自动攻击累计发送次数清0
								GetGame()->GetGameControl()->SetSendedUseSkillMsgCount(0);
								GetGame()->GetGameControl()->SetControlAble(true);
								// 结束的技能是正在吟唱的技能，取消吟唱
								if(lSkillKey == GetGame()->GetMainPlayer()->GetIntonateKey())
									//|| lSkillID == GetGame()->GetMainPlayer()->GetIntonateState())
								{
									GetGame()->GetMainPlayer()->SetCurIntonateTime(0,0);
									GetGame()->GetMainPlayer()->SetIntonateSkillKey(0);
									if(g_bDebug)
									{
										Log4c::Trace(ROOT_MODULE,"技能结束，吟唱清空，次数清0：%d,key:%d",lSkillID,lSkillKey);
									}
								}
								else
								{
									// 光环类技能重设使用状态
									CPlayer::tagSkill *ptagS = GetGame()->GetMainPlayer()->GetSkill(lSkillID);
									if(ptagS)
										ptagS->dwUseingStartTime = 0;
								}
								// 当使用的是制作的技能时，进行特殊的处理
								if (lSkillID>=50003&&lSkillID<=50005)
								{
									//CFacturePageEx* pFacturePage = GetGame()->GetCGuiEx()->GetFacturePageEx();
									//if (lCurType==2&&pFacturePage!=NULL)
									//{
									//	pFacturePage->UpdateInputNum();
									//	pFacturePage->SetFactureState(false);
									//	if ((CFacturePageEx::eErrorNotice)pFacturePage->IsCanFacture()==CFacturePageEx::eErrorNotice_NoError)
									//	{
									//		pFacturePage->SendFactureOneMes();
									//	}
										//pFacturePage->UpdateFactureLevelExpProcessBar();
									//}		
								}
							}
						}
						if (lType == TYPE_PET)
						{
							CPet *pPet = (CPet*)pMoveShape;
							if (pPet->IsMainPlayerPet())
							{
								if(lSkillKey == pPet->GetIntonateKey()
									&& lSkillID == pPet->GetIntonateState())
								{
									GetGame()->GetMainPlayer()->SetCurIntonateTime(0,0);
									GetGame()->GetMainPlayer()->SetIntonateSkillKey(0);
								}
								tagPetSkillInfo *pPetSkill = pPet->GetPetSkillInfoByID(lSkillID);
								if(pPetSkill)
									pPetSkill->bIsSkillUsingEnd = true;
							}
						}
					}
				}
				break;
			case SKILL_USE_RESULT_FAILED:
				CGUID ID;
				pMsg->GetGUID(ID);

				long lSkillID = pMsg->GetLong();
				long lLevel = pMsg->GetByte();

				long lSkillKey = pMsg->GetLong();
				long lFaildRet = pMsg->GetByte();
				// 将该技能结束
				CMoveShape* pMoveShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(ID);
				if(pMoveShape)
				{
					long lType = pMoveShape->GetType();
					CSkillXml* pSkill = pMoveShape->GetCurSkill(lSkillKey);
					if(pSkill)
					{
						pSkill->StepEnd(lSkillID, lLevel, lType, ID, pMoveShape->GetDirEx());
						pMoveShape->DeleteCurSkill(lSkillKey);
					}
					if(pMoveShape->GetType() == TYPE_PET)
					{
						CPet *pPet = (CPet*)pMoveShape;
						tagPetSkillInfo *pPetSkill = pPet->GetPetSkillInfoByID(lSkillID);
						if(pPetSkill)
							pPetSkill->bIsSkillUsingEnd = true;
						pPet->SkillFailReceived();
					}
					// 其他对象的技能，不提示
					if( pMoveShape == GetGame()->GetMainPlayer())
					{
						// 自动攻击累计发送次数清0
						GetGame()->GetGameControl()->SetSendedUseSkillMsgCount(0);
						// 如果是本玩家释放的技能，且失败技能为连续技能，停止本玩家自动攻击状态
						CSkillListXml::tagSkill *pS = CSkillListXml::GetProperty(lSkillID);
						if(pS && pS->bLockSkill)
						{
							// 连续攻击技能的mp不足，切换到基本攻击
							if(lFaildRet == SKILL_USE_FAILED_INVALID_MP)
							{
								DWORD dwID = GetGame()->GetMainPlayer()->GetDefaultAttackSkillID();
								GetGame()->GetGameControl()->SetBufferSkillID(dwID);
								CPlayer::tagSkill * pPlayerSkill = GetGame()->GetMainPlayer()->GetSkill(dwID);
								if(pPlayerSkill)
									pPlayerSkill->dwUseingStartTime = timeGetTime();
							}
							if(lFaildRet == SKILL_USE_FAILED_INVALID_HP || lFaildRet == SKILL_USE_FAILED_INVALID_RP)
								GetGame()->GetGameControl()->AutoAttactStop();
						}
						// 光环类技能重设使用状态
						CPlayer::tagSkill *ptagS = GetGame()->GetMainPlayer()->GetSkill(lSkillID);
						if(ptagS && pS && !pS->bLockSkill)
							ptagS->dwUseingStartTime = 0;
						// 结束的技能是正在吟唱的技能，取消吟唱
						if(lSkillKey > 0 && lSkillKey == GetGame()->GetMainPlayer()->GetIntonateKey())
						{
							GetGame()->GetMainPlayer()->SetCurIntonateTime(0,0);
							GetGame()->GetMainPlayer()->SetIntonateSkillKey(0);
							if(g_bDebug)
							{
								Log4c::Trace(ROOT_MODULE,"技能使用失败，次数清0，吟唱清空：%d,类型：%d,key:%d",lSkillID,lFaildRet,lSkillKey);
							}
						}
						if (lSkillID>=50000&&lSkillID<=50006)
						{
							((CPlayer*)pMoveShape)->SetToolsID(0);
						}
						// 如果正处于制作过程中
						//CFacturePageEx* pFacturePage = GetGame()->GetCGuiEx()->GetFacturePageEx();
						/*if (pFacturePage!=NULL && pFacturePage->IsOpen())
						{
							pFacturePage->SetFactureState(false);
							if (pFacturePage->GetCurFactureNum()!=0 || pFacturePage->GetAllFactureState())
							{
								pFacturePage->ClearFactureNum();
							}
						}*/
					}
				}
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-48.wav");
				if(pMoveShape && (pMoveShape == GetGame()->GetMainPlayer() ||
					pMoveShape->GetType() == TYPE_PET &&
					((CPet*)pMoveShape)->IsMainPlayerPet() ))
				{
					switch (lFaildRet)
					{
					case SKILL_USE_FAILED_INVALID_STATE:
						{
							// 自动攻击技能
							if(GetGame()->GetGameControl()->GetAutoSkillID() == lSkillID)
								GetGame()->GetGameControl()->SetAutoSkillID(0);
							//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							//CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_STATE),
							//D3DCOLOR_ARGB(255,255,0,0));
						}
						break;
					case SKILL_USE_FAILED_INVALID_HP:
						//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
						//	CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_HP),
						//	D3DCOLOR_ARGB(255,255,0,0));
						break;

					case SKILL_USE_FAILED_INVALID_MP:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_MP),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;

					case SKILL_USE_FAILED_INVALID_RP:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_NP),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;

					case SKILL_USE_FAILED_INVALID_YP:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_YVALUE),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;

					case SKILL_USE_FAILED_INVALID_TARGET:
						{
							// 自动攻击技能
							if(GetGame()->GetGameControl()->GetAutoSkillID() == lSkillID)
								GetGame()->GetGameControl()->SetAutoSkillID(0);
							//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							//	CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_OBJERROR),
							//	D3DCOLOR_ARGB(255,255,0,0));
						}
						break;

					case SKILL_USE_FAILED_INVALID_DISTANT:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
                            CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_OUTAREA),
                            D3DCOLOR_ARGB(255,255,0,0));*/
						break;

					case SKILL_USE_FAILED_INVALID_SKILL:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_NOSKILL),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;

					case SKILL_USE_FAILED_INVALID_TIME:			//##允许再次使用技能的时间未到达
						{
							/*if(GetGame()->GetGameControl()->GetAutoSkillID() != lSkillID)
								GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
								CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_COOLTIME),
								D3DCOLOR_ARGB(255,255,0,0));
							else
								GetGame()->GetGameControl()->SetBufferSkillID(SKILL_BASE_BUFF);*/
							if( GetGame()->GetGameControl()->GetAutoSkillID() == lSkillID)
								GetGame()->GetGameControl()->SetBufferSkillID(SKILL_BASE_BUFF);
						}
						break;

					case SKILL_USE_FAILED_CANNOTCOLLECTION_STATE:	//当前状态(人物的状态)不能采集
						{
							/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
								CStringReading::LoadText(205,18),
								D3DCOLOR_ARGB(255,255,0,0));*/
						}
						break;

					case SKILL_USE_FAILED_CANNOTFACTURE_STATE:		//当前状态（人物的状态）不能制作
						{
							/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
								CStringReading::LoadText(205,19),
								D3DCOLOR_ARGB(255,255,0,0));*/
						}
						break;

					case SKILL_USE_FAILED_INVALID_EQUIP:		//##无效的装备
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_ARM),
							D3DCOLOR_ARGB(255,255,0,0));*/
						// 取消连续攻击技能
						GetGame()->GetGameControl()->SetAutoSkillID(0);
						break;

					case SKILL_USE_FAILED_BLOCKED:				//##被阻挡了
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_BLOCK),
							D3DCOLOR_ARGB(255,255,0,0));*/
						// 取消连续攻击技能
						GetGame()->GetGameControl()->SetAutoSkillID(0);
						break;
					case SKILL_USE_FAILED_BREAK:
						{
							if (lSkillID>=50000&&lSkillID<=50007)
							{
								/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
									CStringReading::LoadText(205,40),
									D3DCOLOR_ARGB(255,255,0,0));*/
							}
							else
							{
								/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
									CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_DELSKILL),
									D3DCOLOR_ARGB(255,255,0,0));*/
							}
							// 取消连续攻击技能
							if(lSkillID != GetGame()->GetGameControl()->GetAutoSkillID())
							{
								GetGame()->GetGameControl()->SetAutoSkillID(0);
								GetGame()->GetMainPlayer()->SetAction(CShape::ACT_STAND);
							}
						}
						break;
					case SKILL_USE_FAILED_NOOBJECT:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_NOOBJ),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;

					case SKILL_USE_FAILED_INVALID_INTONATE:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_INTTIME),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;

					case SKILL_USE_FAILED_INVALID_OCCUPATION:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_OUCC),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_INVALID_FACTURE:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_MADE),
							D3DCOLOR_ARGB(255,255,0,0));
						GetGame()->GetCGuiEx()->GetFacturePageEx()->Close();*/
						break;
					case SKILL_USE_FAILED_INVALID_COLLECT:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_COLLECTION),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_INVALID_ACT:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_ACTING),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_INVALID_MOVING:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_MOVEING),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;

					case SKILL_USE_FAILED_C_NOCOLLECTION:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_CONDITION_NOOBJ),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_C_STATE_ERROR:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_CONDITION_STATE),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_C_UNKNOW_COLLECTION:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_CONDITION_NOINFO),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_C_DOCCU_ERROR:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_CONDITION_OCCUEX),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_C_LEVEL_ERROR:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_CONDITION_LEV),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_C_GOODSCONDITION_ERROR:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_CONDITION_GOODS),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_C_NO_BAGSPACE:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_CONDITION_PACK),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_C_NOT_ROLECOLLECTION:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_CONDITION_NOTQUESTOBJ),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_C_QUESTSTEP_ERROR:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_CONDITION_SPECQUEST),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_C_UNKOWN_COLLECTTYPE:
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_CONDITION_TYPE),
							D3DCOLOR_ARGB(255,255,0,0));*/
						break;
					case SKILL_USE_FAILED_F_NO_FACTURE:
						{
							/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
								CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_MADE_NOOBJ),
								D3DCOLOR_ARGB(255,255,0,0));*/
							GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
						}
						break;
					case SKILL_USE_FAILED_F_DOCCU_ERROR:
						{
							/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
								CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_MADE_OCCUEX),
								D3DCOLOR_ARGB(255,255,0,0));*/
							GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
						}
						break;
					case SKILL_USE_FAILED_F_NOTHAS_FACTURE:
						{
							/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
								CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_MADE_NOSKILL),
								D3DCOLOR_ARGB(255,255,0,0));*/
							GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
						}
						break;
					case SKILL_USE_FAILED_F_GOODSCONDITION_ERROR:
						{
							/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
								CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_MADE_TOOL),
								D3DCOLOR_ARGB(255,255,0,0));*/
							GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
						}
						break;
					case SKILL_USE_FAILED_F_MATERIALS_ERROR:
						{
							/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
								CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_MADE_GOODS),
								D3DCOLOR_ARGB(255,255,0,0));*/
							GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
						}
						break;
					case SKILL_USE_FAILED_F_NPC_ERROR:
						{
							/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
								CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_MADE_NPCDEST),
								D3DCOLOR_ARGB(255,255,0,0));*/
							GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
						}
						break;
					case SKILL_USE_FAILED_F_NO_BAGSPACE:
						{
// 							GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
// 								CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_MADE_PACK),
// 								D3DCOLOR_ARGB(255,255,0,0));
							GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
						}
						break;
						// 场景错误
					case SKILL_USE_FAILED_INVALID_REGION:
// 						GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
// 							CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_BYREGION),
// 							D3DCOLOR_ARGB(255,255,0,0));
						break;
					default: break;
					}
				}
				break;
			}
		}
		break;

	case MSG_S2C_SKILL_ADD_EX_STATE:	// 加入一个异常状态
		{
			// 状态承受着的类型和GUID
			long lType = pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);
			// 状态ID
			DWORD dwStateID = pMsg->GetDWord();
			DWORD dwLevel = pMsg->GetDWord();			// 状态等级
			DWORD dwRemainedTime = pMsg->GetDWord();
			BYTE bFlag = pMsg->GetByte();

			CMoveShape *pMoveShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lType, ID);
			if (pMoveShape)
			{
				pMoveShape->AddStateEx(dwStateID, dwLevel, dwRemainedTime,0,bFlag);
			}
		}
		break;

	case MSG_S2C_SKILL_END_EX_STATE:	// 结束一个异常状态
		{
			long lType = pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);
			DWORD dwStateID = pMsg->GetDWord();
			DWORD dwlev = pMsg->GetDWord();
			CMoveShape *pMoveShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lType, ID);
			if (pMoveShape)
			{
				pMoveShape->RemoveStateEx(dwStateID,dwlev);
			}
		}
		break;

	case MSG_S2C_SKILL_EX_STATE_ADDITIONAL_DATA:
		{
			long lType = pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);
			long lStateID = pMsg->GetLong();
			DWORD dwValue = pMsg->GetDWord();
			if( lStateID == STATE_TEAM )
			{
				CPlayer *pPlayer = dynamic_cast<CPlayer*>(pMsg->GetRegion()->FindChildObject(lType, ID));
				if (pPlayer)
					pPlayer->SetTeammateAmount(dwValue&0xffff);
			}
		}
		break;
		// 打开技能学习页面
	case MSG_S2C_SKILL_STUDY_BEGIN:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			long num = pMsg->GetLong();
			if(num > 0)
			{
				//GetGame()->GetCGuiEx()->GetLearnSkillPage()->Close();
				for(long i=0; i<num; ++i)
				{
					BYTE bType = pMsg->GetByte();
					DWORD dwSkillID = pMsg->GetDWord();
					//if(dwSkillID > 0)
					//	GetGame()->GetCGuiEx()->GetLearnSkillPage()->PushSkillID(dwSkillID,bType);
				}
				//GetGame()->GetCGuiEx()->GetLearnSkillPage()->Open();
				//GetGame()->GetCGuiEx()->GetLearnSkillPage()->SetNpcGUID(guid);
				//GetGame()->GetCGuiEx()->GetLearnSkillPage()->OpenPage(CLearnSkillPage::TYPE_FIGHTSKILLPAGE);
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-20.wav");
			}
			// 职业不符，弹出警告对话框
			else
			{

			}
		}
		break;
		// 打开制作技能学习页面
	case MSG_S2C_OCCUSKILL_STUDY_BEGIN:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			//GetGame()->GetCGuiEx()->GetLearnSkillPage()->Close();
			long num = pMsg->GetDWord();
			for(WORD i=0; i<num; ++i)
			{
				DWORD dwSkillID = pMsg->GetDWord();
				DWORD dwSkillCost = pMsg->GetDWord();
				//if(dwSkillID > 0)
				//	GetGame()->GetCGuiEx()->GetLearnSkillPage()->AddOccuSkill(dwSkillID,dwSkillCost);
			}
			//GetGame()->GetCGuiEx()->GetLearnSkillPage()->SetNpcGUID(guid);
			//GetGame()->GetCGuiEx()->GetLearnSkillPage()->OpenPage(CLearnSkillPage::TYPE_ASSITSKILLPAGE);
		}
        break;
	case MSG_S2C_OCCUSKILL_STUDY:
		{
			long lBeSucceed = pMsg->GetLong();
			DWORD dwFactureID = pMsg->GetDWord();
			if (lBeSucceed!=0)
			{
				//GetGame()->GetCGuiEx()->GetLearnSkillPage()->ChoseNextSkill(dwFactureID);
			}
			//GetGame()->GetCGuiEx()->GetLearnSkillPage()->StudyOccuSkillResult(lBeSucceed,dwFactureID);
			//long lOccType = GetGame()->GetCGuiEx()->GetMainBarPageEx()->GetOuccType();
			//GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetOccuButtonSpecialState(true,lOccType);
			//GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-27.wav");
		}
        break;
	case MSG_S2C_OCCUSKILL_DELETE:
		{
			GetGame()->GetMainPlayer()->DeleteAllFactureSkill();
			//DWORD dwBeSucceed = pMsg->GetDWord();
			//DWORD dwFactureID = pMsg->GetDWord();
			//GetGame()->GetCGuiEx()->GetLearnSkillPage()->StudyOccuSkillResult(dwBeSucceed,dwFactureID);
		}break;
		// 技能学习失败
	case MSG_S2C_SKILL_STUDY_FAILED:
		{
			long lFaildRet = pMsg->GetLong();
			switch (lFaildRet)
			{
				//职业不符合学习条件
			case SKILL_STUDY_FAILED_OCCUPATION:
// 				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
// 					CStringReading::LoadText(eIDS_SKILL,eIDS_STUDY_OUCC),
// 					D3DCOLOR_ARGB(255,255,0,0));
				break;
				//等级不符合学习条件
			case SKILL_STUDY_FAILED_RANK:
// 				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
// 					CStringReading::LoadText(eIDS_SKILL,eIDS_STUDY_LEV),
// 					D3DCOLOR_ARGB(255,255,0,0));
				break;
				//SP不符合学习条件
			case SKILL_STUDY_FAILED_SP:
// 				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
// 					CStringReading::LoadText(eIDS_SKILL,eIDS_STUDY_SP),
// 					D3DCOLOR_ARGB(255,255,0,0));
				break;
				//前置技能不符合学习条件
			case SKILL_STUDY_FAILED_SKILL:
// 				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
// 					CStringReading::LoadText(eIDS_SKILL,eIDS_STUDY_PRESKILL),
// 					D3DCOLOR_ARGB(255,255,0,0));
				break;
				//前置限制技能不符合条件
			case SKILL_STUDY_FAILED_LIMIT_SKILL:
// 				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
// 					CStringReading::LoadText(eIDS_SKILL,eIDS_STUDY_LIMITSKILL),
// 					D3DCOLOR_ARGB(255,255,0,0));
				break;
				//物品
			case SKILL_STUDY_FAILED_GOOD:
// 				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
// 					CStringReading::LoadText(eIDS_SKILL,eIDS_STUDY_GOODS),
// 					D3DCOLOR_ARGB(255,255,0,0));
				break;
				//金币不足
			case SKILL_STUDY_FAILED_GOLD:
// 				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
// 					CStringReading::LoadText(eIDS_SKILL,eIDS_STUDY_NOGOLDS),
// 					D3DCOLOR_ARGB(255,255,0,0));
				break;
				//银币不足
			case SKILL_STUDY_FAILED_SILVER:
// 				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
// 					CStringReading::LoadText(eIDS_SKILL,eIDS_STUDY_NOSILVER),
// 					D3DCOLOR_ARGB(255,255,0,0));
				//职业等级不符合条件
			case SKILL_STUDY_FAILED_OCCULV:
// 				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
// 					CStringReading::LoadText(eIDS_SKILL,eIDS_STUDY_FAILED_OCCULV),
// 					D3DCOLOR_ARGB(255,255,0,0));
				break;
			}
		}
		break;
		// 宠物添加技能
	case MSG_S2C_SKILL_ADD_PET:
		{
			//得到宠物GUID
			CGUID	guid;
			pMsg->GetGUID(guid);

			tagPetSkillInfo skill;
			pMsg->GetEx(&skill, sizeof(tagSkill));
			
			//技能冷却总时间长度
			skill.lRestorTimeLength = pMsg->GetLong();

			//技能当前冷却时间
			skill.lStartCoolTime = 0;

            // 技能范围改为float TODO:修改客户端技能范围相关逻辑
			//最小攻击距离
			skill.lMinAtkDistance = (long)pMsg->GetFloat();
			
			//最大攻击距离
			skill.lMaxAtkDistance = (long)pMsg->GetFloat();
			
			//需要花费MP值
			skill.lCostMp = pMsg->GetLong();
			
			CPet* pPet = dynamic_cast<CPet*>(GetGame()->GetRegion()->FindChildObject(TYPE_PET,guid)) ;
			if (pPet == NULL)
				return;
			if (skill.lPos == 0)
			{
				skill.bAuto = true;	//如果是第一个技能 则打开自动释放
			}
			pPet->AddSkill(skill);

			//CPetPageEx *pPage = GetGame()->GetCGuiEx()->GetPetPageEx();
			
			//if (pPage)
			//	pPage->LearnedNewSkill(pPet,&skill);
		}
		break;
		/***********************************************************************/
		/*  zhaohang 2010-10-18 //技能免疫*/
		/***********************************************************************/
	//case MSG_S2C_SHAPE_ATK_IMMUNITY:
	//	{

	//	}
	//	break;
	}
}
