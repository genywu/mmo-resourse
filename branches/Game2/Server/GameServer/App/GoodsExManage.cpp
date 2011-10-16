#include "StdAfx.h"

#include "Container/CTestContainer.h"
#include "Goods/CGoods.h"
#include "Goods/CGoodsFactory.h"
#include "GoodsExManage.h"
#include "Player.h"
//#include "Script/Script.h"
#include "../../../Public/Common/GoodsDef.h"

#include "../../../Public/Setup/GoodsSetup.h"
#include "../../../Public/Setup/GlobalSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//!**************************************************************************
//! 物品合成、分解
//!
//!
//!**************************************************************************

//! 响应物品分解预览请求
//! 
/*	客户端消息结构
	long 被分解物类型ID
*/
/*
void GoodsExManage::OnDisassemblePreview(CMessage *pMsg)
{
	CMessage reMsg(MSG_S2C_GOODS_RE_DISASSEMBLE_PREVIEW);
	if(TestSession(pMsg->GetPlayerID()))
	{
		long				lGoodsTypeID	= pMsg->GetLong();
		tagReValue			arr_result[MAX_DISASSEMBLE_NUM];


		long lOutNum = GetGoodsDisassembleResult(lGoodsTypeID, arr_result);
		if (0 < lOutNum)
		{
			reMsg.Add((long)eGTC_Succeed);
			reMsg.Add(lOutNum);
			for (long idx = 0; idx < lOutNum; ++idx)  
			{
				reMsg.Add(arr_result[idx].lGoodsIndex); //! 获得物种类ID
				reMsg.Add(arr_result[idx].lGoodsNum);	//! 获得物数量
			}	
		}
		else
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_CantDisassemble);
		}
	}
	else
	{
		reMsg.Add((long)eGTC_Failed);
		reMsg.Add((long)eGTC_SessionLapse);
	}

	reMsg.SendToPlayer(pMsg->GetPlayerID());
}
*/


//! 响应物品分解请求
//! 
/*	客户端消息结构
	long 被分解物对象GUID
	long 被分解物位置枚举（CS2CContainerObjectMove::PLAYER_EXTEND_ID）
	long 物品所在包裹位置
*/
void GoodsExManage::OnGoodsDisassemble(CMessage *pMsg)
{
	CMessage reMsg(MSG_S2C_GOODS_RE_DISASSEMBLE_DO);

	do 
	{
		if(!TestSession(pMsg->GetPlayerID(), eGWT_DisassembleWindow))
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_SessionLapse);
			break;
		}

		CPlayer* pPlayer = pMsg -> GetPlayer();
		if (NULL == pPlayer) return;
		if (CPlayer::PROGRESS_NONE != pPlayer->GetCurrentProgress()) return;

		CGUID goodsGuid;
		pMsg->GetGUID(goodsGuid);

		long lContianerId	= pMsg->GetLong(); //容器ID
		long lPos			= pMsg->GetLong(); //位置

		//! 取得被分解物品，并验证合法性
		CGoods *pGoods = TestGoodsHere(pPlayer, goodsGuid, lContianerId, lPos);
		if(NULL == pGoods)
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_CantFindAim);
			break;
		}
		
		long lGoodsTypeID = pGoods->GetBasePropertiesIndex();
		tagReValue arr_result[MAX_DISASSEMBLE_NUM];
		//! 如果物品不能分解
		long lOutNum = GetGoodsDisassembleResult(lGoodsTypeID, arr_result);
		if (0 == lOutNum || IsFrost(pGoods))
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_CantDisassemble);
			break;
		}

		//! 得到产物列表
		vector<CGoods*> vAddGoods;
		for (long i = 0; i < MAX_DISASSEMBLE_NUM; ++i)
		{
			if (0 < arr_result[i].lGoodsNum)
			{
				vector<CGoods*> vTmpAddGoods;
				CGoodsFactory::CreateGoods( arr_result[i].lGoodsIndex, arr_result[i].lGoodsNum, vTmpAddGoods,42);
				for (long j = 0; j < (int)vTmpAddGoods.size(); ++j)
				{
					vAddGoods.push_back(vTmpAddGoods[j]);
				}
			}
		}		

		//! 得到删除列表
		vector<tagGoodsOperate> vDelGoods;
		tagGoodsOperate GoodsOperate;
		GoodsOperate.lContianerPos	= lContianerId;
		GoodsOperate.lGoodsPos		= lPos;
		GoodsOperate.lOperateNum	= 1;
		vDelGoods.push_back(GoodsOperate);

		//! 测试玩家能否装下产物
		long lSpareGrilleNum = TestPutWay(pPlayer, vDelGoods, vAddGoods);
		if(0 > lSpareGrilleNum)
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_NoEnoughSpace);
			reMsg.Add(lSpareGrilleNum);

			for (LONG i = 0; i < (long)vAddGoods.size(); ++i)
			{
#ifdef _GOODSLOG1_
				if(vAddGoods[i])
				{
					char pszGoodsLog[1024]="";			
					char srcGoodsGUID[50]="";
					vAddGoods[i]->GetExID().tostring(srcGoodsGUID);					
					_snprintf(pszGoodsLog,1024,"[物品回收:exmanage206][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
						vAddGoods[i]->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(vAddGoods[i]->GetBasePropertiesIndex()),srcGoodsGUID,vAddGoods[i]->GetAmount());
					CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
				}			
#endif	
				CGoodsFactory::GarbageCollect(&vAddGoods[i], 520);
			}
			break;
		}

		//! 生成本次操作的ID
		CGUID optGuid;
		CGUID::CreateGUID(optGuid);

		//! 删除分解物
		if(!DelGoods(pPlayer, vDelGoods, 380, optGuid))
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_SrcIsLock);

			for (LONG i = 0; i < (long)vAddGoods.size(); ++i)
			{
#ifdef _GOODSLOG1_
				if(vAddGoods[i])
				{
					char pszGoodsLog[1024]="";			
					char srcGoodsGUID[50]="";
					vAddGoods[i]->GetExID().tostring(srcGoodsGUID);					
					_snprintf(pszGoodsLog,1024,"[物品回收:exmanage234][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
						vAddGoods[i]->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(vAddGoods[i]->GetBasePropertiesIndex()),srcGoodsGUID,vAddGoods[i]->GetAmount());
					CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
				}			
#endif	
				CGoodsFactory::GarbageCollect(&vAddGoods[i], 520);
			}
			break;
		}

		//! 添加物品
		vector<long> vOptTye(vAddGoods.size());
		vector<long> vOptNum(vAddGoods.size());
		AddGoods(pPlayer, vAddGoods, vOptTye, vOptNum);

		
		//! 获得物品日志
		for(LONG i = 0; i < (long)vAddGoods.size(); ++i)
		{
			GetLogicLogInterface()->logT380_goods_disassemble_log(pPlayer, vAddGoods[i], vOptNum[i], optGuid, TRUE);
		}			
		
		reMsg.Add((long)eGTC_Succeed);

	} while(false);

	////! 以前的代码在添加物品后进行了回收，不理解
	//for( size_t i = 0; i < vAddGoods.size(); i ++ )
	//{
	//	CGoodsFactory::GarbageCollect( &vAddGoods[i] );
	//}
	reMsg.SendToPlayer(pMsg->GetPlayerID(), false);
}

//! 响应物品合成请求
//! 
/*	客户端消息结构
	long				合成配方索引
	tagSynthesizeGoods	材料列表
*/
void GoodsExManage::OnGoodsSynthesize(CMessage *pMsg)
{
	CMessage reMsg(MSG_S2C_GOODS_RE_SYNTHESIZE_DO);
	do 
	{
		if(!TestSession(pMsg->GetPlayerID(), eGWT_PrescriptionWindow))
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_SessionLapse);
			break;
		}

		CPlayer* pPlayer = pMsg -> GetPlayer();
		if (NULL == pPlayer) return;
		if (CPlayer::PROGRESS_NONE != pPlayer->GetCurrentProgress()) return;
		
		//! 获得服务器配方
		long lPrescriptionIndex = pMsg->GetLong();
		const GoodsExSetup::tagSynthesizePrescription *pSynthesizePrescription 
			= m_GoodsExSetup.GetSynthesizePrescription(lPrescriptionIndex);
		if (NULL == pSynthesizePrescription)
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_NoPrescription);
			break;
		}

		//! 删除列表
		vector<tagGoodsOperate> vDelGoods;

		//! 验证客户端发来的物品
		tagSynthesizeGoods SynthesizeGoods;
		pMsg -> GetEx(&SynthesizeGoods, sizeof(tagSynthesizeGoods));

		bool pass = true;
		for (long i = 0; i < MAX_STUFF_NUM; ++i)
		{
			if (0 == SynthesizeGoods.arrContainerFlag[i]) break;

			CGoods *pGoods = TestGoodsHere(	pPlayer, 
								SynthesizeGoods.arrGoodsGuid[i], 
								SynthesizeGoods.arrContainerFlag[i],
								SynthesizeGoods.arrPos[i]);

			if(NULL == pGoods)
			{
				pass = false;
				break;
			}
			for (long j = i + 1; j < MAX_STUFF_NUM; ++j)
			{
				if(SynthesizeGoods.arrContainerFlag[i] == SynthesizeGoods.arrContainerFlag[j] && 
					SynthesizeGoods.arrPos[i] == SynthesizeGoods.arrPos[j])
				{
					reMsg.Add((long)eGTC_Failed);
					reMsg.Add((long)eGTC_ErrSP);
					reMsg.SendToPlayer(pMsg->GetPlayerID());
					return;
				}
			}

			tagGoodsOperate GoodsOperate;
			GoodsOperate.lContianerPos	= SynthesizeGoods.arrContainerFlag[i];
			GoodsOperate.lGoodsPos		= SynthesizeGoods.arrPos[i];
			if(SynthesizeGoods.arrNum[i] > (long)pGoods->GetAmount())
			{
				reMsg.Add((long)eGTC_Failed);
				reMsg.Add((long)eGTC_ErrSP);
				reMsg.SendToPlayer(pMsg->GetPlayerID());
				return;
			}
			GoodsOperate.lOperateNum	= SynthesizeGoods.arrNum[i];
			GoodsOperate.lReserved		= pGoods->GetBasePropertiesIndex();
			GoodsOperate.pGoods			= pGoods;
			vDelGoods.push_back(GoodsOperate);
		}
		if (!pass)
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_CantFindAim);
			break;
		}

		//! 验证配方、获取主材料属性
		LONG lGoodsLevel = 0;
		LONG lGoodsEvilLevel = 0;
		long arrPass[MAX_STUFF_NUM] = {0};
		for (long i = 0; i < MAX_STUFF_NUM; ++i)
		{
			if (0 == pSynthesizePrescription->arr_lStuffId[i])
			{
				break;
			}

			long lNeedNum = pSynthesizePrescription->arr_lStuffNum[i];
			for (long j = 0; j < (long)vDelGoods.size(); ++j)
			{
				if (vDelGoods[j].lReserved == pSynthesizePrescription->arr_lStuffId[i])
				{
					if (0 >= lNeedNum)
					{
						//! 配方错误
						reMsg.Add((long)eGTC_Failed);
						reMsg.Add((long)eGTC_ErrSP);
						reMsg.SendToPlayer(pMsg->GetPlayerID());
						return;
					}
					else if(vDelGoods[j].lOperateNum > lNeedNum)
					{
						vDelGoods[j].lOperateNum = lNeedNum;
						lNeedNum = 0;
						vDelGoods[j].lOperateWay = CS2CContainerObjectMove::OT_CHANGE_AMOUNT;
					}
					else
					{
						lNeedNum -= vDelGoods[j].lOperateNum;
						vDelGoods[j].lOperateWay = CS2CContainerObjectMove::OT_DELETE_OBJECT;
					}

					if(pSynthesizePrescription->bHasPrimary && 0 == i)
					{
						lGoodsLevel = vDelGoods[j].pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1);
						lGoodsEvilLevel = vDelGoods[j].pGoods->GetAddonPropertyValues(GAP_PLUG_EVIL_LEVEL, 1);
					}
				}
			}
			
			if(0 != lNeedNum)
			{
				//! 配方错误
				reMsg.Add((long)eGTC_Failed);
				reMsg.Add((long)eGTC_ErrSP);
				reMsg.SendToPlayer(pMsg->GetPlayerID(), false);
				return;
			}
		}

		//! 得到产物列表
		vector<CGoods*> vAddGoods;
		CGoodsFactory::CreateGoods( 
			pSynthesizePrescription->lOutGoodsId, 
			pSynthesizePrescription->lOutGoodsNum, vAddGoods,44);

		//! 测试玩家能否装下产物
		long lSpareGrilleNum = TestPutWay(pPlayer, vDelGoods, vAddGoods);
		if(0 > lSpareGrilleNum)
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_NoEnoughSpace);
			reMsg.Add(lSpareGrilleNum);

			for (LONG i = 0; i < (long)vAddGoods.size(); ++i)
			{
#ifdef _GOODSLOG1_
				if(vAddGoods[i])
				{
					char pszGoodsLog[1024]="";			
					char srcGoodsGUID[50]="";
					vAddGoods[i]->GetExID().tostring(srcGoodsGUID);					
					_snprintf(pszGoodsLog,1024,"[物品回收:exmanage424][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
						vAddGoods[i]->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(vAddGoods[i]->GetBasePropertiesIndex()),srcGoodsGUID,vAddGoods[i]->GetAmount());
					CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
				}			
#endif	
				CGoodsFactory::GarbageCollect(&vAddGoods[i], 520);
			}
			break;
		}

		//! 生成本次操作ID
		CGUID optGuid;
		CGUID::CreateGUID(optGuid);

		//! 删除分解物
		if(!DelGoods(pPlayer, vDelGoods, 390, optGuid))
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_SrcIsLock);

			for (LONG i = 0; i < (long)vAddGoods.size(); ++i)
			{
#ifdef _GOODSLOG1_
				if(vAddGoods[i])
				{
					char pszGoodsLog[1024]="";			
					char srcGoodsGUID[50]="";
					vAddGoods[i]->GetExID().tostring(srcGoodsGUID);					
					_snprintf(pszGoodsLog,1024,"[物品回收:exmanage452][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
						vAddGoods[i]->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(vAddGoods[i]->GetBasePropertiesIndex()),srcGoodsGUID,vAddGoods[i]->GetAmount());
					CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
				}			
#endif	
				CGoodsFactory::GarbageCollect(&vAddGoods[i], 520);
			}
			break;
		}

		//! 继承属性
		if(pSynthesizePrescription->bHasPrimary)
		{
			CGoodsFactory::SetEquipmentLevel(vAddGoods[0], lGoodsLevel);
			CGoodsFactory::UpdateEquipmentAttr(vAddGoods[0]);
		}

		//! 添加物品
		vector<long> vOptTye(vAddGoods.size());
		vector<long> vOptNum(vAddGoods.size());
		AddGoods(pPlayer, vAddGoods, vOptTye, vOptNum);


		//! 记录日志
		for (long i = 0; i < (long)vAddGoods.size(); ++i)
		{
			GetLogicLogInterface()->logT390_goods_synthesize_log(pPlayer, vAddGoods[i], vOptTye[i], optGuid, TRUE);
		}

		reMsg.Add((long)eGTC_Succeed);
		reMsg.Add((long)vAddGoods.size());
		for (LONG i = 0; i < (long)vAddGoods.size(); ++i)
		{
			reMsg.Add(vOptNum[i]);
			reMsg.Add(vAddGoods[i]->GetExID());
		}


	} while(false);
	
	reMsg.SendToPlayer(pMsg->GetPlayerID(), false);
}



//!**************************************************************************
//! 武器冲能
//!
//!
//!**************************************************************************


//! 补充能量物品作用到玩家
//! 物品合法性、使用后的物品删除，均在外部执行
bool GoodsExManage::ReinforcePower(CGoods *pArm, CGoods *pGoods, CPlayer *pPlayer)
{
	if (NULL == pGoods || NULL == pPlayer || NULL == pArm) return false;
	if (CPlayer::PROGRESS_NONE != pPlayer->GetCurrentProgress()) return false;

	const CGUID& PlayerGUID = pPlayer->GetExID();
	CMessage errMsg(MSG_S2C_GOODS_RE_REINFORCE_POWER);

	if (NULL == pArm || NULL == pGoods) 
	{
		errMsg.Add((long)eGTC_Failed);
		errMsg.Add((long)eGTC_NoArmInHand);
		errMsg.SendToPlayer(PlayerGUID, false);
		return false;
	}

	//! 武器充能上限			
	long lMaxPower_A = pArm->GetAddonPropertyValues(GAP_MAX_POWER, 1); 
	if (0 == lMaxPower_A) 
	{
		errMsg.Add((long)eGTC_Failed);
		errMsg.Add((long)eGTC_NoReinforceArm);
		errMsg.SendToPlayer(PlayerGUID, false);
		return false;
	}

	//! 物品充能强度
	long lPowerIntensity_G = pGoods->GetAddonPropertyValues(GAP_POWER_INTENSITY, 1);
	if (0 == lPowerIntensity_G) 
	{
		//! 于逻辑不合的错误，应记录错误日志
		return false;
	}

	//! 物品充能等级区间
	long lMinLevel_G = pGoods->GetAddonPropertyValues(GAP_MIN_REINFORCE_LEVEL, 1);
	long lMaxLevel_G = pGoods->GetAddonPropertyValues(GAP_MAX_REINFORCE_LEVEL, 1);

	//! 武器等级
	long lLevel_A = pArm->GetAddonPropertyValues(GAP_ROLE_LEVEL_LIMIT, 1);
	if (lMinLevel_G > lLevel_A || lMaxLevel_G < lLevel_A)
	{
		errMsg.Add((long)eGTC_Failed);
		errMsg.Add((long)eGTC_LevelError);
		errMsg.SendToPlayer(PlayerGUID, false);
		return false;
	}

	//! 武器充能强度
	long lPowerIntensity_A = pArm->GetAddonPropertyValues(GAP_POWER_INTENSITY, 1);

	//! 武器当前充能值
	long lCurrPower_A = pArm->GetAddonPropertyValues(GAP_CURR_POWER, 1);

	
	//! 判断是否需要充能
	long lMaxPercent = m_GoodsExSetup.GetReinforceMaxPercent();
	if (lPowerIntensity_A >= lPowerIntensity_G && lCurrPower_A * 100 >= lMaxPower_A * lMaxPercent)
	{
		errMsg.Add((long)eGTC_Failed);
		errMsg.Add((long)eGTC_ArmIsFull);
		errMsg.SendToPlayer(PlayerGUID, false);
		return false;
	}

	pArm->ChangeAttribute(GAP_POWER_INTENSITY, lPowerIntensity_G, 1, true);
	pArm->ChangeAttribute(GAP_POWER_INTENSITY, pGoods->GetBasePropertiesIndex(), 2, true);

	pArm->ChangeAttribute(GAP_CURR_POWER, lMaxPower_A, 1, true);
	pArm->UpdateAttribute(PlayerGUID);

	errMsg.Add((long)eGTC_Succeed);
	errMsg.Add(PlayerGUID);
	errMsg.Add(lPowerIntensity_G);
	errMsg.SendToAround(pPlayer);

	return true;
}

//! 开始玩家的武器加成效果
bool GoodsExManage::ArmReinforceBegin(CPlayer *pPlayer, CGoods *pArm, bool isMagic, bool isAttack, pair<float, float>& ReturnReinforce)
{
	if (NULL == pArm || NULL == pPlayer) return false;

	//! 武器充能强度
	long lPowerIntensity = pArm->GetAddonPropertyValues(GAP_POWER_INTENSITY, 1);

	//! 武器当前充能值
	long lCurrPower = pArm->GetAddonPropertyValues(GAP_CURR_POWER, 1);

	//! 武器等级
	long lLevel = pArm->GetAddonPropertyValues(GAP_ROLE_LEVEL_LIMIT, 1);

	//! 配置
	const GoodsExSetup::tagPowerAccessorial* pPowerAccessorial = m_GoodsExSetup.GetPowerAccessorial(lLevel, lPowerIntensity);
	
	if (NULL != pPowerAccessorial) 
	{
		long lUsePower = 0;
		if (isMagic)
			lUsePower = pPowerAccessorial->lMExpend;
		else
			lUsePower = pPowerAccessorial->lPExpend;
		if (lCurrPower > 0)
		{
			//! 计算剩余点数
			long lLostPower = lCurrPower - lUsePower;
			lLostPower = (lLostPower > 0) ? lLostPower : 0;

			pArm->ChangeAttribute(GAP_CURR_POWER, lLostPower, 1, true);

			CMessage msg(MSG_S2C_GOODS_POWER_WARN);
			msg.Add(pPlayer->GetExID());
			msg.Add(lPowerIntensity);
			msg.Add(lLostPower);
			msg.SendToAround(pPlayer);


			if (isMagic)
			{
				ReturnReinforce.first = pPowerAccessorial->AmendCoefficient.fAmendIT;
				if(isAttack)
					ReturnReinforce.second = pPowerAccessorial->AmendCoefficient.fAmendMA;
				else
					ReturnReinforce.second = pPowerAccessorial->AmendCoefficient.fAmendMC;
			}
			else
			{
				ReturnReinforce.first = 1.0f;
				if(isAttack)
					ReturnReinforce.second = pPowerAccessorial->AmendCoefficient.fAmendPA;
				else
					ReturnReinforce.second = pPowerAccessorial->AmendCoefficient.fAmendPC;
			}
			return true;
		}
	}

	ReturnReinforce.first	= 1.0f;
	ReturnReinforce.second	= 1.0f;
	return false;
}

//!				根据输入的条件得到一个配置的加成值
float GoodsExManage::GetReinforceByEnum(long lAimLevel, long lPowerIntensity, long eValuePos)
{
	const GoodsExSetup::tagPowerAccessorial* pPowerAccessorial = m_GoodsExSetup.GetPowerAccessorial(lAimLevel, lPowerIntensity);
	if (NULL != pPowerAccessorial)
	{
		switch(eValuePos)
		{
		case GoodsExSetup::eACT_AmendIT:
			return pPowerAccessorial->AmendCoefficient.fAmendIT;
		case GoodsExSetup::eACT_AmendMA:
			return pPowerAccessorial->AmendCoefficient.fAmendMA;
		case GoodsExSetup::eACT_AmendPA:
			return pPowerAccessorial->AmendCoefficient.fAmendPA;
		case GoodsExSetup::eACT_AmendMC:
			return pPowerAccessorial->AmendCoefficient.fAmendMC;
		case GoodsExSetup::eACT_AmendPC:
			return pPowerAccessorial->AmendCoefficient.fAmendPC;
		}
	}
	return 1.0f;
}

//!				判断是否还可以减能量
long GoodsExManage::CanExpendPower(CGoods *pArm, BOOL isMagic)
{
	//! 武器充能强度
	long lPowerIntensity = pArm->GetAddonPropertyValues(GAP_POWER_INTENSITY, 1);

	//! 武器当前充能值
	long lCurrPower = pArm->GetAddonPropertyValues(GAP_CURR_POWER, 1);

	//! 武器等级
	long lLevel = pArm->GetAddonPropertyValues(GAP_ROLE_LEVEL_LIMIT, 1);

	//! 配置
	const GoodsExSetup::tagPowerAccessorial* pPowerAccessorial = m_GoodsExSetup.GetPowerAccessorial(lLevel, lPowerIntensity);

	long lUsePower = 0;
	if (NULL != pPowerAccessorial) 
	{
		if (isMagic)
			lUsePower = pPowerAccessorial->lMExpend;
		else
			lUsePower = pPowerAccessorial->lPExpend;
	}

	return lCurrPower >= lUsePower;
}

//!				扣除一次消耗的武器的能量
long GoodsExManage::GetExpendPower(CGoods *pArm, BOOL isMagic)
{
	//! 武器充能强度
	long lPowerIntensity = pArm->GetAddonPropertyValues(GAP_POWER_INTENSITY, 1);

	//! 武器当前充能值
	long lCurrPower = pArm->GetAddonPropertyValues(GAP_CURR_POWER, 1);

	//! 武器等级
	long lLevel = pArm->GetAddonPropertyValues(GAP_ROLE_LEVEL_LIMIT, 1);

	//! 配置
	const GoodsExSetup::tagPowerAccessorial* pPowerAccessorial = m_GoodsExSetup.GetPowerAccessorial(lLevel, lPowerIntensity);

	long lUsePower = 0;
	if (NULL != pPowerAccessorial) 
	{
		if (isMagic)
			lUsePower = pPowerAccessorial->lMExpend;
		else
			lUsePower = pPowerAccessorial->lPExpend;
	}

	if (lCurrPower < lUsePower)
		lUsePower = lCurrPower;

	return lUsePower;
}


//!**************************************************************************
//! 物品绑定、冻结
//!
//!
//!**************************************************************************

//! 设置物品的绑定属性
long GoodsExManage::SetGoodsBindProperty_Player(CGoods *pGoods, long lBindType, const CGUID& playerGuid)
{
	if(NULL == pGoods) return 1;
	if(!pGoods->HasAddonProperty(GAP_BIND)) return 10;

	//! 只有“未冻结”的物品才能进行“绑定”相关操作
	//if (!pGoods->HasAddonProperty(CGoodsBaseProperties::GAP_FREEZE)) return 15;
	//if (eFT_UnFrost != pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_FREEZE, 1)) return 20;

	if (eBT_Untie == lBindType || eBT_Bind == lBindType)
	{
		if (lBindType != pGoods->GetAddonPropertyValues(GAP_BIND, 1))
			pGoods->ChangeAttribute(GAP_BIND, lBindType, 1, true);
		else
			return 30;//! 操作值与当前值相同
	}
	else
		return 40;

	pGoods->UpdateAttribute(playerGuid);
	return 0;
}


//! 为脚本提供的绑定函数
//！参数1：物品拥有者
//! 参数2、3：物品所在包裹位置（只能是主背包和5个子背包）
//! 参数4：0=解绑定、1=绑定。其他值非法
//! 返回值：0=失败；1=成功；
long GoodsExManage::SetGoodsBindProperty_Script(CPlayer *pPlayer, long lContianerID, long lPos, long lBindType)
{
	if (NULL == pPlayer || (eBT_Untie != lBindType && eBT_Bind != lBindType)) return 0;

	CGoods *pGoods = FindGoodsFromPlayer(pPlayer, lContianerID, lPos);
	if (NULL == pGoods) return 0;

	if(0 == SetGoodsBindProperty_Player(pGoods, lBindType, pPlayer->GetExID())) return 1;

	return 0;
}


//! 系统设置物品的绑定属性
long GoodsExManage::SetGoodsBindProperty_System(CGoods *pGoods, long lBindType, const CGUID& playerGuid)
{
	if(NULL == pGoods || NULL_GUID == playerGuid) return 1;
	if(!pGoods->HasAddonProperty(GAP_BIND)) return 10;

	pGoods->ChangeAttribute(GAP_BIND, lBindType, 1, true);

	
	//! 若绑定属性被设为没绑定，并且冻结属性不为“0”，则修改冻结属性和解冻时间
	//if (eBT_Bind == lBindType || eBT_CantUntie == lBindType)
	//{	
	//	if (0 != pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_FREEZE, 1))
	//	{
	//		pGoods->ChangeAttribute(CGoodsBaseProperties::GAP_FREEZE, eFT_UnFrost, 1, true);
	//		pGoods->ChangeAttribute(CGoodsBaseProperties::GAP_THAWTIME, 0, 1, true);
	//	}
	//}
	pGoods->UpdateAttribute(playerGuid);

	return 0;
}




//! 玩家设置物品的冻结属性
long GoodsExManage::SetGoodsFrostProperty_Player(CGoods *pGoods, long lFrostType, const CGUID& playerGuid)
{
	if(NULL == pGoods || NULL_GUID == playerGuid) return 1;
	if(!pGoods->HasAddonProperty(GAP_BIND)) return 10;

	//! 玩家只能做冻结和解冻操作
	if (eFT_UnFrost == lFrostType || eFT_Frost == lFrostType)
	{
		//! 只有“绑定”属性为1或3的物品才能进行“冻结”相关操作
		//long lBindType = pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_BIND, 1);
		//if (eBT_Bind == lBindType || eBT_CantUntie == lBindType)
		{
			long lOldFrostType = pGoods->GetAddonPropertyValues(GAP_FREEZE, 1);
			if(lOldFrostType == lFrostType) return 30; //! 操作值与当前值相同

			if(eFT_CantThaw == lFrostType)
			{
				return 70;
			}
			//! 如果是冻结，则直接冻结物品
			else if (eFT_Frost == lFrostType)
			{
				pGoods->ChangeAttribute(GAP_FREEZE, eFT_Frost, 1, true);
				pGoods->ChangeAttribute(GAP_THAWTIME, 0, 1, true);
			}
			else 
			{
				//! 获得现在时间
				long lNow = 0;
				time(&lNow);

				//! 如果正在解冻
				if (eFT_Thawing == lOldFrostType)
				{
					//! 察看解冻过期时间
					long lThawTime = pGoods->GetAddonPropertyValues(GAP_THAWTIME, 1);
					if (lNow > lThawTime) //! 解冻时间已到
					{
						pGoods->ChangeAttribute(GAP_FREEZE, eFT_UnFrost, 1, true);
						pGoods->ChangeAttribute(GAP_THAWTIME, 0, 1, true);
					}
					else
					{
						return 40;//! 解冻时间未到
					}
				}
				else
				{
					//! 将7天后的现在设置为解冻过期时间
					pGoods->ChangeAttribute(GAP_FREEZE, eFT_Thawing, 1,true);
					pGoods->ChangeAttribute(GAP_THAWTIME, lNow + 7 * 24 * 60 * 60, 1, true);
				}
			}
		}
		//else
		//{
		//	return 50;
		//}
	}
	else
	{
		return 60;
	}

	pGoods->UpdateAttribute(playerGuid);
	return 0;
}

//! 为脚本提供的冻结函数
//！参数1：物品拥有者
//! 参数2、3：物品所在包裹位置（只能是主背包和5个子背包）
//! 参数4：0=解冻、1=冻结。其他值非法
//! 返回值：0=失败；1=成功；
long GoodsExManage::SetGoodsFrostProperty_Script(CPlayer *pPlayer, long lContianerID, long lPos, long lBindType)
{
	if (NULL == pPlayer || (eFT_UnFrost != lBindType && eFT_Frost != lBindType)) return 0;

	CGoods *pGoods = FindGoodsFromPlayer(pPlayer, lContianerID, lPos);
	if (NULL == pGoods) return 0;

	if(0 == SetGoodsFrostProperty_Player(pGoods, lBindType, pPlayer->GetExID())) return 1;

	return 0;
}

//! 系统设置物品的冻结属性
//long GoodsExManage::SetGoodsFrostProperty_System(CGoods *pGoods, long lFrostType)
//{
//	if(NULL == pGoods) return 1;
//	if(!pGoods->HasAddonProperty(CGoodsBaseProperties::GAP_BIND)) return 2;
//
//}


//!		检测物品是否冻结
//!		如果物品解冻时间已到，触发解冻属性的设置
bool	GoodsExManage::IsFrost(CGoods *pGoods)
{
	long lFrostType = pGoods->GetAddonPropertyValues(GAP_FREEZE, 1);
	if(eFT_Frost == lFrostType || eFT_CantThaw == lFrostType)
	{
		return true;
	}
	else if (eFT_Thawing == lFrostType)
	{
		//! 察看解冻过期时间
		long lThawTime = pGoods->GetAddonPropertyValues(GAP_THAWTIME, 1);
		if (time(NULL) > lThawTime) //! 解冻时间已到
		{
			//! 这里更新了数据但不向客户端发送，客户端在时间的显示处理上已经动了手脚，玩家不会感觉到物品属性的更新被延迟了
			pGoods->ChangeAttribute(GAP_FREEZE, eFT_UnFrost, 1, true);
			pGoods->ChangeAttribute(GAP_THAWTIME, 0, 1, true);
		}
		else
			return true;
	}

	return false;
}

//!		检测物品是否绑定
bool	GoodsExManage::IsBind(CGoods *pGoods)
{
	long lBindType = pGoods->GetAddonPropertyValues(GAP_BIND, 1);
	if(eBT_Bind == lBindType || eBT_CantUntie == lBindType)
		return true;
	return false;
}

//! 响应物品绑定
void GoodsExManage::OnGoodsBind(CMessage *pMsg)
{
	CPlayer *pPlayer = pMsg->GetPlayer();
	if (NULL == pPlayer) return;
	if (CPlayer::PROGRESS_NONE != pPlayer->GetCurrentProgress()) return;
	BindAndFrost(true, pMsg);
}

//! 响应物品冻结
void GoodsExManage::OnGoodsFrost(CMessage *pMsg)
{
	CPlayer *pPlayer = pMsg->GetPlayer();
	if (NULL == pPlayer) return;
	if (CPlayer::PROGRESS_NONE != pPlayer->GetCurrentProgress()) return;
	BindAndFrost(false, pMsg);
}

//! 响应装备升级预览
void GoodsExManage::OnEquipmentUpgradeView(CMessage *pMsg)
{
	CMessage reMsg(MSG_S2C_GOODS_RE_UPGRADE_VIEW);
	do 
	{
		if(!TestSession(pMsg->GetPlayerID(), eGWT_UpgradeWindow))
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_SessionLapse);
			break;
		}

		//! 取得玩家
		CPlayer* pPlayer = pMsg -> GetPlayer();
		if (NULL == pPlayer) return;
		if (CPlayer::PROGRESS_NONE != pPlayer->GetCurrentProgress()) return;

		//! 取得操作目标，验证目标是否存在
		CGUID goodsGuid;
		pMsg->GetGUID(goodsGuid);

		long lContianerId	= pMsg->GetLong(); //容器ID
		long lPos			= pMsg->GetLong(); //位置

		CGoods *pGoods = TestGoodsHere(pPlayer, goodsGuid, lContianerId, lPos);
		if(NULL == pGoods)
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_CantFindAim);
			break;
		}

		//! 验证目标是否可以升级
		long lAimCurrLevel = pGoods->CanUpgrade();
		if (-1 == lAimCurrLevel || m_GoodsExSetup.GetGoodsMaxLevel() <= lAimCurrLevel)
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_LevelTiptop);
			break;
		}
		//! 获取配方
		long lLimitLevel = pGoods->GetAddonPropertyValues(GAP_ROLE_LEVEL_LIMIT, 1);
		const GoodsExSetup::tagUpgradeSet* pUpgradeSet = m_GoodsExSetup.GetUpgradeSet(lLimitLevel, lAimCurrLevel);
		if (NULL == pUpgradeSet)
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_LevelTiptop);
			break;
		}

		//! 克隆一个物品以便属性修改
		CGoods *pCloneGoods = CGoodsFactory::CreateGoods(pGoods->GetBasePropertiesIndex(),45);
		if(!pGoods->Clone((CBaseObject*)pCloneGoods))
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_UnknownErr);

			//! 回收临时物品
#ifdef _GOODSLOG1_
			if(pCloneGoods)
			{
				char pszGoodsLog[1024]="";			
				char srcGoodsGUID[50]="";
				pCloneGoods->GetExID().tostring(srcGoodsGUID);					
				_snprintf(pszGoodsLog,1024,"[物品回收:GoodsExManage::OnEquipmentUpgradeView][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
					pCloneGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pCloneGoods->GetBasePropertiesIndex()),srcGoodsGUID,pCloneGoods->GetAmount());
				CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
			}			
#endif
			CGoodsFactory::GarbageCollect(&pCloneGoods,32);
			break;
		}

		//! 升级克隆的物品，并编码到消息中
		if(!CGoodsFactory::ChangeEquipmentLevel(pCloneGoods, 1))
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_UnknownErr);

			//! 回收临时物品
#ifdef _GOODSLOG1_
			if(pCloneGoods)
			{
				char pszGoodsLog[1024]="";			
				char srcGoodsGUID[50]="";
				pCloneGoods->GetExID().tostring(srcGoodsGUID);					
				_snprintf(pszGoodsLog,1024,"[物品回收:GoodsExManage::OnEquipmentUpgradeView2][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
					pCloneGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pCloneGoods->GetBasePropertiesIndex()),srcGoodsGUID,pCloneGoods->GetAmount());
				CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
			}			
#endif
			CGoodsFactory::GarbageCollect(&pCloneGoods,33);
			break;
		}

		vector<BYTE> vMsgData;
		pCloneGoods->SerializeForOldClient(&vMsgData);

		reMsg.Add((long)eGTC_Succeed);
		reMsg.Add((long)vMsgData.size());
		reMsg.AddEx(&(vMsgData[0]), (long)vMsgData.size());

		//! 回收临时物品
#ifdef _GOODSLOG1_
		if(pCloneGoods)
		{
			char pszGoodsLog[1024]="";			
			char srcGoodsGUID[50]="";
			pCloneGoods->GetExID().tostring(srcGoodsGUID);					
			_snprintf(pszGoodsLog,1024,"[物品回收:GoodsExManage::OnEquipmentUpgradeView3][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
				pCloneGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pCloneGoods->GetBasePropertiesIndex()),srcGoodsGUID,pCloneGoods->GetAmount());
			CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
		}		
#endif
		CGoodsFactory::GarbageCollect(&pCloneGoods,34);

	}while (false);

	reMsg.SendToPlayer(pMsg->GetPlayerID(), false);
}

//! 响应装备升级
void GoodsExManage::OnEquipmentUpgrade(CMessage *pMsg)
{
	CMessage reMsg(MSG_S2C_GOODS_RE_UPGRADE_DO);
	do 
	{
		if(!TestSession(pMsg->GetPlayerID(), eGWT_UpgradeWindow))
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_SessionLapse);
			break;
		}

		//! 取得玩家
		CPlayer* pPlayer = pMsg -> GetPlayer();
		if (NULL == pPlayer) return;
		if (CPlayer::PROGRESS_NONE != pPlayer->GetCurrentProgress()) return;

		//! 取得所有操作物，并生成删除物列表
		tagUpgradeStuff UpgradeStuff;
		pMsg->GetEx(&UpgradeStuff, sizeof(tagUpgradeStuff));

		vector<tagGoodsOperate> vDelGoods;
		
		//! 获得所有操作物
		set<CGUID> setGoodsGuid;
		CGoods *arr_pGoods[IN_TAGUPGRADESTUFF_GOODS_NUM] = {0};
		for (long i = 0; i < IN_TAGUPGRADESTUFF_GOODS_NUM; ++i)
		{
			arr_pGoods[i] = TestGoodsHere(pPlayer, UpgradeStuff.arrGoodsGuid[i], UpgradeStuff.arrContainerFlag[i], UpgradeStuff.arrPos[i]);
			if (0 < i && NULL != arr_pGoods[i])
			{
				//! 验证重复物品放置
				if(setGoodsGuid.end() != setGoodsGuid.find(arr_pGoods[i]->GetExID()))
					return;
				else
					setGoodsGuid.insert(arr_pGoods[i]->GetExID());

				for (long j = i + 1; j < IN_TAGUPGRADESTUFF_GOODS_NUM; ++j)
				{
					if (arr_pGoods[i] == arr_pGoods[j])
					{
						return;
					}
				}
				tagGoodsOperate GoodsOperate;
				GoodsOperate.lContianerPos	= UpgradeStuff.arrContainerFlag[i];
				GoodsOperate.lGoodsPos		= UpgradeStuff.arrPos[i];
				if(1 < arr_pGoods[i]->GetAmount())
					GoodsOperate.lOperateWay = CS2CContainerObjectMove::OT_CHANGE_AMOUNT;
				else if(1 != arr_pGoods[i]->GetAmount())
				{
					reMsg.Add((long)eGTC_Failed);
					reMsg.Add((long)eGTC_NumError);
					break;
				}
				GoodsOperate.lOperateNum	= 1;
				vDelGoods.push_back(GoodsOperate);
			}
		}

		//! 验证升级目标和主料是否存在
		if(NULL == arr_pGoods[0] || NULL == arr_pGoods[1])
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_CantFindAim);
			break;
		}

		//! 验证目标是否可以升级
		long lAimCurrLevel = arr_pGoods[0]->CanUpgrade();
		long lNewLevel = lAimCurrLevel;
		if (-1 == lAimCurrLevel || m_GoodsExSetup.GetGoodsMaxLevel() <= lAimCurrLevel)
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_LevelTiptop);
			break;
		}

		//! 获取配方
		long lLimitLevel = arr_pGoods[0]->GetAddonPropertyValues(GAP_ROLE_LEVEL_LIMIT, 1);
		const GoodsExSetup::tagUpgradeSet* pUpgradeSet = m_GoodsExSetup.GetUpgradeSet(lLimitLevel, lAimCurrLevel);
		if(NULL == pUpgradeSet)
		{
            //玩家[%s]使用[%s]升级物品[%s]时发生失败，没有找到升级配方!该物品当前等级为[%d]级!!
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_14", 
				        pPlayer->GetName(),
				        arr_pGoods[1]->GetName(),
				        arr_pGoods[0]->GetName(), 
				        lAimCurrLevel));
			break;
		}


		//! 验证目标和主料搭配是否合法
		if(pUpgradeSet->lStuffIndex != arr_pGoods[1]->GetBasePropertiesIndex())
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_ErrStuff);
			break;
		}

		//! 验证玩家的金钱
		long lType		= arr_pGoods[0]->GetGoodsBaseType();
		long lChidlType = 1;
		if (GBT_EQUIPMENT == lType)
		{
			lChidlType = arr_pGoods[0]->GetAddonPropertyValues(GAP_WEAPON_CATEGORY, 1);
		}

		//！ 将从配置中获取价格改为从物品属性中获取价格，单位金币
		//long lNeedMoney = m_GoodsExSetup.GetUpgradeMoney(lLimitLevel, lType, lChidlType);
		long lNeedMoney = arr_pGoods[0]->GetAddonPropertyValues(GAP_UPGRADE_PRICE, 1);

		if ( lNeedMoney > (long)pPlayer->GetMoney() )
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_NotEnoughMoney);
			break;
		}

		//! 获取辅助材料
		map<const GoodsExSetup::tagExtraStuff*, long> mapAssistantFlag;
		//for (long i = IN_TAGUPGRADESTUFF_GOODS_NUM - MAX_UPGRADE_ASSISTANT_NUM; i < IN_TAGUPGRADESTUFF_GOODS_NUM; ++i)
		//{
		//	if (NULL != arr_pGoods[i])
		//	{
		//		long lAssistantIndex = arr_pGoods[i]->GetBasePropertiesIndex();
		//		const GoodsExSetup::tagExtraStuff *tmpExtraStuff = m_GoodsExSetup.GetExtraStuffIni(lAssistantIndex);
		//		if(NULL != tmpExtraStuff)
		//		{
		//			map<const GoodsExSetup::tagExtraStuff*, long>::iterator ite = mapAssistantFlag.find(tmpExtraStuff);
		//			if (mapAssistantFlag.end() != ite)
		//			{
		//				ite->second = ite->second + 1;
		//			}
		//			else
		//			{
		//				mapAssistantFlag[tmpExtraStuff] = 1;
		//			}

		//			//! 验证特殊材料的数量是否超过限制
		//			if ((mapAssistantFlag.find(tmpExtraStuff))->first->lMaxUseCount < mapAssistantFlag[tmpExtraStuff])
		//			{
		//				//! 非法消息，不返回任何信息
		//				return;
		//			}
		//		}
		//		else
		//		{
		//			//! 非法消息，不返回任何信息
		//			return;
		//		}
		//	}
		//}

		//! 按几率得到这次升级的结果
		bool bSucceed = false;

		long lOdds = pUpgradeSet->lOdds;
		
		for (map<const GoodsExSetup::tagExtraStuff*, long>::iterator ite = mapAssistantFlag.begin(); ite != mapAssistantFlag.end(); ++ite)
		{
			lOdds += (ite->first->lAddOdds) * (ite->second);
		}
		if (0 > lOdds) lOdds = 0;
		else if(100 < lOdds) lOdds = 100;

		long lRandom = random(100);

		if (lRandom < lOdds) bSucceed = true;

		//! 按结果处理升级目标
		
		long lUpgradeResult = eUR_Succeed;

		if (bSucceed)
		{
			//! 升级物品
			if(!CGoodsFactory::ChangeEquipmentLevel(arr_pGoods[0], 1))
			{
				reMsg.Add((long)eGTC_Failed);
				reMsg.Add((long)eGTC_UnknownErr);
				
#ifdef _GOODSLOG1_
				if(arr_pGoods[0])
				{
					char pszGoodsLog[1024]="";			
					char srcGoodsGUID[50]="";
					arr_pGoods[0]->GetExID().tostring(srcGoodsGUID);					
					_snprintf(pszGoodsLog,1024,"[物品回收:GoodsExManage::OnEquipmentUpgrade][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
						arr_pGoods[0]->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(arr_pGoods[0]->GetBasePropertiesIndex()),srcGoodsGUID,arr_pGoods[0]->GetAmount());
					CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
				}				
#endif
				CGoodsFactory::GarbageCollect(&(arr_pGoods[0]),35);
				break;
			}
			lNewLevel += 1;

			//! 系统公告
			if(10 <= lAimCurrLevel)
			{
				if(20 > lAimCurrLevel + 1)
				{
					//! 普通公告
					char szShowInfo[512] = {0};
					//! "%s的【%s】受到光之女神的眷顾，成功将【%s】由【%d】级强化为【%d】级"
					_snprintf(szShowInfo, 512, AppFrame::GetText("80"),
						GetGame()->GetCountryName(pPlayer->GetCountry()),
						pPlayer->GetName(),
						arr_pGoods[0]->GetName(),
						lAimCurrLevel,
						lAimCurrLevel + 1
						);

					CMessage msg( MSG_S2W_GM_WORLD_MESSAGE );
					msg.Add( NULL_GUID );
					msg.Add( szShowInfo );
					msg.Add( GlobalSetup::GetSetup()->uUpgradeTo10FontColor );
					msg.Add( (long)0x00FFFFFF );
					msg.Send();
				}
				else
				{
					char szShowInfo[512] = {0};
					//! "%s的【%s】创造了奇迹，【%d】的【%s】发出了璀璨夺目的光芒！让我们一起为这位勇士喝彩吧！"
					_snprintf(szShowInfo, 512, AppFrame::GetText("81"),
						GetGame()->GetCountryName(pPlayer->GetCountry()),
						pPlayer->GetName(),
						lAimCurrLevel + 1,
						arr_pGoods[0]->GetName()
						);
					//! 走马灯
					CMessage msg(MSG_S2W_OTHER_NEWS_TOP_WORLD);
					msg.Add(1l);
					msg.Add(szShowInfo);
					msg.Add(GlobalSetup::GetSetup()->uUpgradeTo20BackIdx);
					msg.Send();
				}
			}
		}
		else
		{
			long lFailedOperate = pUpgradeSet->lFailedOperate;
			long lOperateNum	= pUpgradeSet->lOperateNum;

			bool bIsAmended				= false;
			long lAmendFailedOperate	= eFO_Null;
			long lAmendOperateNum		= 0;

			lUpgradeResult = eUR_Failed_And_LevelNotChange;

			//! 如果要对失败作出惩罚
			if (eFO_Null != lFailedOperate)
			{
				//! 则先查找有没有有修改惩罚结果的附加物品
				long lChangeResultGoodsNum = 0;
				for (map<const GoodsExSetup::tagExtraStuff*, long>::iterator ite = mapAssistantFlag.begin(); ite != mapAssistantFlag.end(); ++ite)
				{
					if(-1 < ite->first->lFailedMinishLevel)
					{
						lAmendFailedOperate = eFO_Reduce;
						lAmendOperateNum	= ite->first->lFailedMinishLevel;
						bIsAmended			= true;

						if (0 == lChangeResultGoodsNum)
						{
							++lChangeResultGoodsNum;
						}
						else
						{
							//! 出现了多个修改惩罚的物品，不返回信息
							return;
						}
					}
				}

				//! 比较出最好的惩罚结果
				long lResultLevel = 0;
				if (eFO_Reduce == lFailedOperate)
				{
					lResultLevel = lAimCurrLevel - lOperateNum;
				}
				else if (eFO_SetTo == lFailedOperate)
				{
					lResultLevel = lOperateNum;
				}

				if (bIsAmended)
				{
					long lAmendResult = lAimCurrLevel;
					
					if (eFO_Reduce == lAmendFailedOperate)
					{
						lAmendResult = lAimCurrLevel - lAmendOperateNum;
					}
					else if (eFO_SetTo == lAmendFailedOperate)
					{
						lAmendResult = lAmendOperateNum;
					}

					if (lAmendResult > lResultLevel) lResultLevel = lAmendResult;
				}

				//! 执行操作
				if (lAimCurrLevel != lResultLevel)
				{
					assert(lResultLevel < lAimCurrLevel);
					lUpgradeResult = eUR_Failed_And_Degraded;
					//! 物品降级
					if(!CGoodsFactory::SetEquipmentLevel(arr_pGoods[0], lResultLevel) && 0 != lAimCurrLevel)
					{
						reMsg.Add((long)eGTC_Failed);
						reMsg.Add((long)eGTC_UnknownErr);
						break;
					}
					lNewLevel = lResultLevel;
				}

				//! 系统公告
				if(10 <= lAimCurrLevel)
				{
					//! 普通公告
					char szShowInfo[512] = {0};
					//! "%s的【%s】屏气凝神，默默祈祷装备强化成功。然而事与愿违，【%s】由【%d】下降到【%d】级！"
					_snprintf(szShowInfo, 512, AppFrame::GetText("82"),
						GetGame()->GetCountryName(pPlayer->GetCountry()),
						pPlayer->GetName(),
						arr_pGoods[0]->GetName(),
						lAimCurrLevel,
						lResultLevel
						);

					CMessage msg( MSG_S2W_GM_WORLD_MESSAGE );
					msg.Add( NULL_GUID );
					msg.Add( szShowInfo );
					msg.Add( GlobalSetup::GetSetup()->uUpgrade10LostFontColor );
					msg.Add( (long)0x00FFFFFF );
					msg.Send();
				}
			}
		}

		//! 删除金钱
		pPlayer->SetMoney(pPlayer->GetMoney() - lNeedMoney,11);

		//! 生成本次操作的ID
		CGUID optGuid;
		CGUID::CreateGUID(optGuid);

		//! 删除材料
		if(!DelGoods(pPlayer, vDelGoods, 400, optGuid))
		{
			CGoodsFactory::SetEquipmentLevel(arr_pGoods[0], lAimCurrLevel);
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_SrcIsLock);
			break;
		}

		//! 记录日志
		GetLogicLogInterface()->logT400_goods_upgrade_log(pPlayer, optGuid, arr_pGoods[0], lAimCurrLevel, lNewLevel);

		vector<BYTE> vMsgData;
		arr_pGoods[0]->SerializeForOldClient(&vMsgData);

		reMsg.Add((long)eGTC_Succeed);
		reMsg.Add(lUpgradeResult);
		if (eUR_Failed_And_LevelNotChange != lUpgradeResult)
		{
			reMsg.Add((long)vMsgData.size());
			reMsg.AddEx(&(vMsgData[0]), (long)vMsgData.size());
		}
        bool bUpgradeSucceed = (eUR_Succeed == lUpgradeResult);
		PlayUpgradeEffect(pPlayer, m_GoodsExSetup.GetUpgrade3DEffectID(bUpgradeSucceed));

	}while (false);

	reMsg.SendToPlayer(pMsg->GetPlayerID(), false);
}

//! 播放一个玩家的升级物品特效
void GoodsExManage::PlayUpgradeEffect(CPlayer *pPlayer, LONG lEffectID)
{
	assert(NULL != pPlayer);
	if(NULL == pPlayer) return;
	map<CGUID, tagGoodsExWndSession>::iterator ite = m_mapOpendGEWnd.find(pPlayer->GetExID());
	assert(ite != m_mapOpendGEWnd.end());
	if(m_mapOpendGEWnd.end() == ite) return;

	
	CMessage msg(MSG_S2C_RGN_PLAYEFFECT);
	msg.Add((BYTE)0);
	msg.Add((LONG)lEffectID);

	msg.Add((LONG)TYPE_MONSTER);
	msg.Add(ite->second.NpcGuid);
		
	msg.Add((FLOAT)0.0f);
	msg.Add((FLOAT)0.0f);
	msg.Add(1L);
	msg.SendToAround(pPlayer);
	//msg.SendToAround((CServerRegion*)pPlayer->GetFather(), ite->second.lPosX, ite->second.lPosY);
}






//! 响应装备魔化
void GoodsExManage::OnEquipmentPlugEvil(CMessage *pMsg)
{
	CMessage reMsg(MSG_S2C_GOODS_RE_PLUG_EVIL_DO);
	do 
	{
		if(!TestSession(pMsg->GetPlayerID(), eGWT_PlusEvilWindow))
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_SessionLapse);
			break;
		}

		//! 取得玩家
		CPlayer* pPlayer = pMsg -> GetPlayer();
		if (NULL == pPlayer) return;
		if (CPlayer::PROGRESS_NONE != pPlayer->GetCurrentProgress()) return;

		//! 得到操作栏位
		DBReadSet dBReadSet;
		pMsg->GetDBReadSet(dBReadSet);
		long lItemType = dBReadSet.GetLongFromByteArray();
		if(GAP_ELEMENT_1 > lItemType || GAP_ELEMENT_3 < lItemType)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_SrcIsLock);
			break;
		}

		//! 得到操作物品
		CGUID OptGoodsGuid;
		dBReadSet.GetBufferFromByteArray(OptGoodsGuid);
		CGoods *pOptGoods = pPlayer->FindGoodsByGuidInPack(OptGoodsGuid);
		if(NULL == pOptGoods)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_CantFindAim);
			break;
		}

		//! 得到材料
		CGUID StuffGuid;
		dBReadSet.GetBufferFromByteArray(StuffGuid);
		CGoods *pStuff = pPlayer->FindGoodsByGuidInPack(StuffGuid);
		if(NULL == pStuff)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_ErrStuff);
			break;
		}
		const char *pClientStuffName = GoodsSetup::QueryGoodsOriginalName(pStuff->GetBasePropertiesIndex());
		if(NULL == pClientStuffName)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_ErrStuff);
			break;
		}

		//! 得到辅助材料
		CGUID HelpfulStuffGuid;
		CGoods *pHelpfulStuff = NULL;
		const char *pUseHelpfulStuffName = NULL;
		
		bool bUseHelpfulStuff = (1 == dBReadSet.GetByteFromByteArray());
		if (bUseHelpfulStuff)
		{
			dBReadSet.GetBufferFromByteArray(HelpfulStuffGuid);
			pHelpfulStuff = pPlayer->FindGoodsByGuidInPack(HelpfulStuffGuid);
			if (NULL == pHelpfulStuff)
			{
				reMsg.Add((long)eGTC_ErrOperation);
				reMsg.Add((long)eGTC_ErrAssistant);
				break;
			}
			pUseHelpfulStuffName = GoodsSetup::QueryGoodsOriginalName(pHelpfulStuff->GetBasePropertiesIndex());
			if(NULL == pUseHelpfulStuffName)
			{
				reMsg.Add((long)eGTC_ErrOperation);
				reMsg.Add((long)eGTC_ErrAssistant);
				break;
			}
		}

		//! 
		long lGoodsLevel = pOptGoods->GetAddonPropertyValues(GAP_ROLE_LEVEL_LIMIT, 1);
		//! 得到物品魔化属性
		vector<long> vPro;
		pOptGoods->GetAddonPropertyValues((eGoodsAddonProperties)lItemType, vPro);
		if (2 > vPro.size() || eGET_Disabled == vPro[0])
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_CantOperateAim);
			break;
		}
		bool bFristUp = eGET_Enabled == vPro[0];

		//! 得到已有魔化类型
		set<DWORD> setHasType;
		if (bFristUp)
		{
			for (DWORD i = GAP_ELEMENT_1; i <= GAP_ELEMENT_3; ++i)
			{
				vector<long> vTmpPro;
				pOptGoods->GetAddonPropertyValues((eGoodsAddonProperties)i, vTmpPro);
				if(eGET_Enabled < vTmpPro[0])
					setHasType.insert(vTmpPro[0] % 10);
			}
		}
		
		bool bIsAttEquipment = (TRUE == CGoodsFactory::IsAttEquipment(pOptGoods));

		//! 获取升级配置
		long lElementType	= vPro[0] % 10;
		long lNeedStuffNum	= 0;
		long lSucceedOdds	= 0;
		GoodsExSetup::LPReStuffName pStuffName = NULL;

		if(bFristUp)
		{
			vPro[1] = 0;
			lElementType = m_GoodsExSetup.GetElement(bIsAttEquipment, lGoodsLevel, pClientStuffName, lNeedStuffNum, lSucceedOdds);
			if(MT_Invalid == lElementType || setHasType.end() != setHasType.find(lElementType))
			{
				reMsg.Add((long)eGTC_ErrOperation);
				reMsg.Add((long)eGTC_CantOperateAim);
				break;
			}
		}
		else
		{
			if(!m_GoodsExSetup.GetElement(bIsAttEquipment, lGoodsLevel, vPro[1], lElementType, pStuffName, lNeedStuffNum, lSucceedOdds))
			{
				reMsg.Add((long)eGTC_ErrOperation);
				reMsg.Add((long)eGTC_CantOperateAim);
				break;
			}
		}
		
		//! 材料类型、材料数量判断
		if(!bFristUp)
		{
			if(0 != strcmp(pClientStuffName, pStuffName) || (long)pStuff->GetAmount() < lNeedStuffNum)
			{
				reMsg.Add((long)eGTC_ErrOperation);
				reMsg.Add((long)eGTC_ErrStuff);
				break;
			}
		}

		//! 获取辅助材料配置
		if (bUseHelpfulStuff)
		{
			long lAddOdds = m_GoodsExSetup.GetHelpfulStuffOdds(vPro[1], pUseHelpfulStuffName);
			if (0 > lAddOdds)
			{
				reMsg.Add((long)eGTC_ErrOperation);
				reMsg.Add((long)eGTC_ErrAssistant);
				break;
			}

			lSucceedOdds += lAddOdds;
		}
		
		//! 结果计算
		bool bSucceed = (random(10000) < lSucceedOdds);
		if(bSucceed)
		{
			if(bFristUp)
			{
				if(bIsAttEquipment)
					pOptGoods->SetAddonPropertyBaseValues((eGoodsAddonProperties)lItemType, 1, lElementType + eGET_Attack);
				else
					pOptGoods->SetAddonPropertyBaseValues((eGoodsAddonProperties)lItemType, 1, lElementType + eGET_Defend);
			}

			pOptGoods->SetAddonPropertyBaseValues((eGoodsAddonProperties)lItemType, 2, vPro[1] + 1);
		}


		//! 生成本次操作的ID
		CGUID optGuid;
		CGUID::CreateGUID(optGuid);

		//! 删除材料
		//! 先记录一个材料删除日志，若删除失败，则不会添加物品魔化等级改变的日志
		GetLogicLogInterface()->logT410_goods_plugevil_log(pPlayer, optGuid, pStuff, 0, pStuff->GetAmount(), pStuff->GetAmount() - lNeedStuffNum);
		if(!pPlayer->DelGoods(StuffGuid, lNeedStuffNum))
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_ErrStuff);
			break;
		}
		//! 辅助材料
		if (bUseHelpfulStuff)
		{
			//! 先记录一个材料删除日志，若删除失败，则不会添加物品魔化等级改变的日志
			GetLogicLogInterface()->logT410_goods_plugevil_log(pPlayer, optGuid, pHelpfulStuff, 0, pHelpfulStuff->GetAmount(), pHelpfulStuff->GetAmount() - 1);
			if(!pPlayer->DelGoods(HelpfulStuffGuid, 1))
			{
				reMsg.Add((long)eGTC_ErrOperation);
				reMsg.Add((long)eGTC_ErrStuff);
				break;
			}
		}

		//! 魔化结果日志，通过这条日志可以判断是否删除了材料又没有执行魔化结果
		GetLogicLogInterface()->logT410_goods_plugevil_log(pPlayer, optGuid, pOptGoods, lItemType, vPro[1], ((bSucceed) ? vPro[1] + 1 : vPro[1]));

		vector<BYTE> vMsgData;
		pOptGoods->SerializeForOldClient(&vMsgData);

		reMsg.Add((long)eGTC_Succeed);
		reMsg.Add((BYTE)bSucceed);
		reMsg.Add((long)vMsgData.size());
		reMsg.AddEx(&(vMsgData[0]), (long)vMsgData.size());

	}while (false);

	reMsg.SendToPlayer(pMsg->GetPlayerID(), false);
}

//! 辅助函数，在给定的一组表示连续的区间长度的数中，得到一个随机点落在其中一个区间中，并返回区间索引
DWORD RandBoundIndex(const long lBound[], long lSize)
{
	struct tagBound
	{
		long lBegin;
		long lEnd;
	};
	long lAllOdds = 0;
	vector<tagBound> vBound;
	long lCurr = 0;
	for (long i = 0; i < lSize; ++i)
	{
		lAllOdds += lBound[i];
		tagBound Bound;
		Bound.lBegin	= lCurr;
		Bound.lEnd		= lCurr + lBound[i];
		lCurr			+= lBound[i];
		vBound.push_back(Bound);
	}

	if (0 < lAllOdds)
	{
		long lPointOfFall = random(lAllOdds);
		for (long i = 0; i < (long)vBound.size(); ++i)
		{
			if (vBound[i].lBegin != vBound[i].lEnd)
				if(vBound[i].lBegin <= lPointOfFall && vBound[i].lEnd > lPointOfFall)
					return i;
		}
	}

	return 0xFFFFFFFF;
}

//! 响应重置魔化类型
void GoodsExManage::OnEquipmentResetElementType(CMessage *pMsg)
{
	CMessage reMsg(MSG_C2S_GOODS_RE_RESET_ELEMENT_TYPE);
	do 
	{
		if(!TestSession(pMsg->GetPlayerID(), eGWT_ResetElementTypeWindow))
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_SessionLapse);
			break;
		}

		//! 取得玩家
		CPlayer* pPlayer = pMsg -> GetPlayer();
		if (NULL == pPlayer) return;
		if (CPlayer::PROGRESS_NONE != pPlayer->GetCurrentProgress()) return;

		//! 得到操作栏位
		DBReadSet dBReadSet;
		pMsg->GetDBReadSet(dBReadSet);
		long lItemType = dBReadSet.GetLongFromByteArray();
		if(GAP_ELEMENT_1 > lItemType || GAP_ELEMENT_3 < lItemType)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_SrcIsLock);
			break;
		}

		//! 得到操作物品
		CGUID OptGoodsGuid;
		dBReadSet.GetBufferFromByteArray(OptGoodsGuid);
		CGoods *pOptGoods = pPlayer->FindGoodsByGuidInPack(OptGoodsGuid);
		if(NULL == pOptGoods)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_CantFindAim);
			break;
		}

		//! 得到材料
		CGUID StuffGuid;
		dBReadSet.GetBufferFromByteArray(StuffGuid);
		CGoods *pStuff = pPlayer->FindGoodsByGuidInPack(StuffGuid);
		if(NULL == pStuff)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_ErrStuff);
			break;
		}
		const char *pClientStuffName = GoodsSetup::QueryGoodsOriginalName(pStuff->GetBasePropertiesIndex());
		if(NULL == pClientStuffName)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_ErrStuff);
			break;
		}

		//! 
		long lGoodsLevel = pOptGoods->GetAddonPropertyValues(GAP_ROLE_LEVEL_LIMIT, 1);
		//! 得到魔化栏位属性
		vector<long> vPro;
		pOptGoods->GetAddonPropertyValues((eGoodsAddonProperties)lItemType, vPro);
		if (2 > vPro.size() || eGET_Disabled == vPro[0] || eGET_Enabled == vPro[0])
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_CantOperateAim);
			break;
		}
		//! 得到已有魔化类型
		set<DWORD> setHasType;
		for (DWORD i = GAP_ELEMENT_1; i <= GAP_ELEMENT_3; ++i)
		{
			vector<long> vTmpPro;
			pOptGoods->GetAddonPropertyValues((eGoodsAddonProperties)i, vTmpPro);
			if(eGET_Enabled < vTmpPro[0])
				setHasType.insert(vTmpPro[0] % 10);
		}

		//! 获取重置配置
		const GoodsExSetup::tagResetStuff *pResetStuff = m_GoodsExSetup.GetResetElement(lGoodsLevel, pClientStuffName);
		if (NULL == pResetStuff)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_CantOperateAim);
			break;
		}

		
		DWORD lNewElementType = RandBoundIndex(pResetStuff->lOdds, MT_Max);
		long lCount = 0;
		while (setHasType.end() != setHasType.find(lNewElementType))
		{
			++lCount;
			if(20 <= lCount)
			{
				lNewElementType = 0xFFFFFFFF;
				break;
			}
			lNewElementType = RandBoundIndex(pResetStuff->lOdds, MT_Max);
		}
		if (0xFFFFFFFF == lNewElementType)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_ErrStuff);
			break;
		}
		//! 重置属性
		if(CGoodsFactory::IsAttEquipment(pOptGoods))
			lNewElementType = lNewElementType + eGET_Attack;
		else
			lNewElementType = lNewElementType + eGET_Defend;

		pOptGoods->SetAddonPropertyBaseValues((eGoodsAddonProperties)lItemType, 1, lNewElementType);

		//! 生成本次操作的ID
		CGUID optGuid;
		CGUID::CreateGUID(optGuid);

		//! 删除材料
		//! 先记录一个材料删除日志，若删除失败，则不会添加物品魔化等级改变的日志
		GetLogicLogInterface()->logT410_goods_plugevil_log(pPlayer, optGuid, pStuff, 0, pStuff->GetAmount(), pStuff->GetAmount() - 1);
		if(!pPlayer->DelGoods(StuffGuid, 1))
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_ErrStuff);
			break;
		}

		GetLogicLogInterface()->logT410_goods_plugevil_log(pPlayer, optGuid, pOptGoods, lItemType, vPro[0], lNewElementType);

		vector<BYTE> vMsgData;
		pOptGoods->SerializeForOldClient(&vMsgData);

		reMsg.Add((long)eGTC_Succeed);
		reMsg.Add((long)vMsgData.size());
		reMsg.AddEx(&(vMsgData[0]), (long)vMsgData.size());

	}while (false);

	reMsg.SendToPlayer(pMsg->GetPlayerID(), false);
}

//! 响应重置魔化相性
void GoodsExManage::OnEquipmentResetXiangXing(CMessage *pMsg)
{
	CMessage reMsg(MSG_C2S_GOODS_RE_RESET_XIANG_XING);
	do 
	{
		if(!TestSession(pMsg->GetPlayerID(), eGWT_ResetElementXiangXingWindow))
		{
			reMsg.Add((long)eGTC_Failed);
			reMsg.Add((long)eGTC_SessionLapse);
			break;
		}

		//! 取得玩家
		CPlayer* pPlayer = pMsg -> GetPlayer();
		if (NULL == pPlayer) return;
		if (CPlayer::PROGRESS_NONE != pPlayer->GetCurrentProgress()) return;

		//! 得到操作栏位
		DBReadSet dBReadSet;
		pMsg->GetDBReadSet(dBReadSet);
		long lItemType = dBReadSet.GetLongFromByteArray();
		if(GAP_ELEMENT_XIANG_XING_1 > lItemType || GAP_ELEMENT_XIANG_XING_3 < lItemType)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_SrcIsLock);
			break;
		}

		//! 得到操作物品
		CGUID OptGoodsGuid;
		dBReadSet.GetBufferFromByteArray(OptGoodsGuid);
		CGoods *pOptGoods = pPlayer->FindGoodsByGuidInPack(OptGoodsGuid);
		if(NULL == pOptGoods)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_CantFindAim);
			break;
		}

		//! 得到材料
		CGUID StuffGuid;
		dBReadSet.GetBufferFromByteArray(StuffGuid);
		CGoods *pStuff = pPlayer->FindGoodsByGuidInPack(StuffGuid);
		if(NULL == pStuff)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_ErrStuff);
			break;
		}
		const char *pClientStuffName = GoodsSetup::QueryGoodsOriginalName(pStuff->GetBasePropertiesIndex());
		if(NULL == pClientStuffName)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_ErrStuff);
			break;
		}

		//! 
		long lGoodsLevel = pOptGoods->GetAddonPropertyValues(GAP_ROLE_LEVEL_LIMIT, 1);
		//! 得到魔化栏位属性
		vector<long> vPro;
		pOptGoods->GetAddonPropertyValues((eGoodsAddonProperties)lItemType, vPro);
		if (2 > vPro.size() || eGET_Disabled == vPro[0] || eGET_Enabled == vPro[0])
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_CantOperateAim);
			break;
		}

		//! 得到已有魔化类型
		set<DWORD> setHasType;
		for (DWORD i = GAP_ELEMENT_1; i <= GAP_ELEMENT_3; ++i)
		{
			vector<long> vTmpPro;
			pOptGoods->GetAddonPropertyValues((eGoodsAddonProperties)i, vTmpPro);
			if(eGET_Enabled < vTmpPro[0])
				setHasType.insert(vTmpPro[0] % 10);
		}

		//! 获取重置配置
		GoodsExSetup::LPCXiangXingOdds pLPCXiangXingOdds = NULL;
		const GoodsExSetup::tagResetStuff *pResetStuff = m_GoodsExSetup.GetResetXiangXing(lGoodsLevel, pClientStuffName, pLPCXiangXingOdds);
		if (NULL == pResetStuff || NULL == pLPCXiangXingOdds)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_CantOperateAim);
			break;
		}

		DWORD lNewXiangXingType = RandBoundIndex(pResetStuff->lOdds, MT_Max);
		long lCount = 0;
		while (setHasType.end() != setHasType.find(lNewXiangXingType))
		{
			++lCount;
			if(20 <= lCount)
			{
				lNewXiangXingType = 0xFFFFFFFF;
				break;
			}
			lNewXiangXingType = RandBoundIndex(pResetStuff->lOdds, MT_Max);
		}
		DWORD lNewXiangXingValue = RandBoundIndex(pLPCXiangXingOdds->lOdds, eXXV_NUM);
		if (0xFFFFFFFF == lNewXiangXingType || 0xFFFFFFFF == lNewXiangXingValue)
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_ErrStuff);
			break;
		}
		//! 重置属性
		if(CGoodsFactory::IsAttEquipment(pOptGoods))
			lNewXiangXingType = lNewXiangXingType + eGET_Attack;
		else
			lNewXiangXingType = lNewXiangXingType + eGET_Defend;

		pOptGoods->SetAddonPropertyBaseValues((eGoodsAddonProperties)lItemType, 1, lNewXiangXingType);
		pOptGoods->SetAddonPropertyBaseValues((eGoodsAddonProperties)lItemType, 2, s_XiangXingValus[lNewXiangXingValue]);

		//! 生成本次操作的ID
		CGUID optGuid;
		CGUID::CreateGUID(optGuid);

		//! 删除材料
		//! 先记录一个材料删除日志，若删除失败，则不会添加物品魔化等级改变的日志
		GetLogicLogInterface()->logT410_goods_plugevil_log(pPlayer, optGuid, pStuff, 0, pStuff->GetAmount(), pStuff->GetAmount() - 1);
		if(!pPlayer->DelGoods(StuffGuid, 1))
		{
			reMsg.Add((long)eGTC_ErrOperation);
			reMsg.Add((long)eGTC_ErrStuff);
			break;
		}

		GetLogicLogInterface()->logT410_goods_plugevil_log(pPlayer, optGuid, pOptGoods, lItemType, vPro[0], lNewXiangXingType);
		GetLogicLogInterface()->logT410_goods_plugevil_log(pPlayer, optGuid, pOptGoods, lItemType, vPro[1], s_XiangXingValus[lNewXiangXingValue]);

		vector<BYTE> vMsgData;
		pOptGoods->SerializeForOldClient(&vMsgData);

		reMsg.Add((long)eGTC_Succeed);
		reMsg.Add((long)vMsgData.size());
		reMsg.AddEx(&(vMsgData[0]), (long)vMsgData.size());

	}while (false);

	reMsg.SendToPlayer(pMsg->GetPlayerID(), false);
}

//! 私有方法
///////////////////////////////////////////////////////////////////////////////////
/*
得到一个物品的分解物类型ID和数量
返回result参数：pair<物品ID, 物品数量>
不检测参数pGoods的合法性
*/
long GoodsExManage::GetGoodsDisassembleResult(long lGoodsID,  tagReValue arrReValue[MAX_DISASSEMBLE_NUM])
{
	long re = 0;

	CGoodsBaseProperties* pGoodsBaseProperties = GoodsSetup::QueryGoodsBaseProperties(lGoodsID);
	if (NULL == pGoodsBaseProperties) return re;
	if (!pGoodsBaseProperties->IsHasAddonPropertie(GAP_CAN_DISASSEMBLE)) return re;

	//! 取得可否分解
	vector<CGoodsBaseProperties::tagAddonPropertyValue> vOut;
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_CAN_DISASSEMBLE, vOut);
	long lAbleDisassemble = vOut[0].lBaseValue;
	if (0 == lAbleDisassemble) return re;

	//! 取得分解价值
	vOut.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_DISASSEMBLE_VALUE, vOut);
	long lDisassembleValue1 = vOut[0].lBaseValue;
	long lDisassembleValue2 = vOut[1].lBaseValue;

	if (0 == lDisassembleValue1 || 0 == lDisassembleValue2) return re;

	//! 取得物品品质
	vOut.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_QUALITY, vOut);
	if (1 > vOut.size()) return re;
	long lQuality = vOut[0].lBaseValue;


	//! 取得配置
	const GoodsExSetup::tagDisassemblePrescription  *pDisassemblePrescription = m_GoodsExSetup.GetDisassemblePrescription(lQuality);

	if (NULL == pDisassemblePrescription) 
		return re;
	
	for (long idx = 0; idx < MAX_DISASSEMBLE_NUM; ++idx)
	{
		if (0 != pDisassemblePrescription->arr_lOutGoodsId[idx])
		{
			//! 得到生成物ID
			arrReValue[idx].lGoodsIndex		= pDisassemblePrescription->arr_lOutGoodsId[idx];
			if(0 == idx)
			{
				//! 第一中产物
				arrReValue[idx].lGoodsNum	= m_GoodsExSetup.GetDisassembleNum(
																	lDisassembleValue1,
																	lDisassembleValue2,
																	lQuality,
																	idx
																	);
			}
			else if(1 == idx)
			{
				//! 第二中产物
				vOut.clear();
				pGoodsBaseProperties->GetAddonPropertyValues(GAP_BLUE_CRYSTAL, vOut);
				if (0 < vOut.size())
					arrReValue[idx].lGoodsNum = vOut[0].lBaseValue;
				else
					arrReValue[idx].lGoodsNum = 0;
				
			}
			else if(2 == idx)
			{
				arrReValue[idx].lGoodsNum = 0;
			}
			++re;
		}
		else
		{
			arrReValue[idx].lGoodsIndex		= 0;
			arrReValue[idx].lGoodsNum		= 0;
		}
	}

	return re;
}

//!		真正的消息响应在此执行函数
void	GoodsExManage::BindAndFrost(bool isBind, CMessage *pMsg)
{
	CMessage msg;
	if (isBind)
		msg.Init(MSG_S2C_GOODS_BIND);
	else
		msg.Init(MSG_S2C_GOODS_FROST);
	
	do 
	{
		//! 测试会话
		if (!TestSession(pMsg->GetPlayerID(), eGWT_PrescriptionWindow))
		{
			msg.Add((long)eGTC_Failed);
			msg.Add((long)eGTC_SessionLapse);
			break;
		}

		//! 首先得到操作手段
		long lOperation = pMsg->GetLong();
		if (eFT_Frost != lOperation && eFT_UnFrost != lOperation)
		{
			msg.Add((long)eGTC_Failed);
			msg.Add((long)eGTC_ErrOperation);
			break;
		}

		//! 得到操作玩家
		CPlayer *pPlayer = pMsg->GetPlayer();
		if(NULL == pPlayer) return;

		CGUID	arrGoodGuid[2];				//! 物品ID
		CGoods	*arr_pGoods[2]		= {NULL};
		long	arrlContianerID[2]	= {0};	//! 物品所在容器
		long	arrlPos[2]			= {0};	//! 物品所在位置

		//! 第一个物品是消耗品
		pMsg->GetGUID(arrGoodGuid[0]);
		arrlContianerID[0]	= pMsg->GetLong();
		arrlPos[0]			= pMsg->GetLong();

		arr_pGoods[0] = TestGoodsHere(pPlayer, arrGoodGuid[0], arrlContianerID[0], arrlPos[0]);
		if (NULL == arr_pGoods[0])
		{
			msg.Add((long)eGTC_Failed);
			msg.Add((long)eGTC_ErrAssistant);
			break;
		}

		//! 第二个物品是目标
		pMsg->GetGUID(arrGoodGuid[1]);
		arrlContianerID[1]	= pMsg->GetLong();
		arrlPos[1]			= pMsg->GetLong();

		arr_pGoods[1] = TestGoodsHere(pPlayer, arrGoodGuid[1], arrlContianerID[1], arrlPos[1]);
		if (NULL == arr_pGoods[1] || arr_pGoods[1] == arr_pGoods[0])
		{
			msg.Add((long)eGTC_Failed);
			msg.Add((long)eGTC_CantFindAim);
			break;
		}

		//！验证操作可行性
		LONG lLogType = 0;
		if (isBind)
		{
			if (eBT_Bind == lOperation)
			{
				if (!m_GoodsExSetup.TestBindGoodsIndex(arr_pGoods[0]->GetBasePropertiesIndex()))
				{
					msg.Add((long)eGTC_Failed);
					msg.Add((long)eGTC_ErrAssistant);
					break;
				}
			}
			else
			{
				if (!m_GoodsExSetup.TestUntieGoodsIndex(arr_pGoods[0]->GetBasePropertiesIndex()))
				{
					msg.Add((long)eGTC_Failed);
					msg.Add((long)eGTC_ErrAssistant);
					break;
				}
				lLogType = 1;
			}
		}
		else
		{	
			if (eFT_Frost == lOperation)
			{
				if (!m_GoodsExSetup.TestFrostGoodsIndex(arr_pGoods[0]->GetBasePropertiesIndex()))
				{
					msg.Add((long)eGTC_Failed);
					msg.Add((long)eGTC_ErrAssistant);
					break;
				}
				lLogType = 2;
			}
			else
			{
				if (!m_GoodsExSetup.TestThawGoodsIndex(arr_pGoods[0]->GetBasePropertiesIndex()))
				{
					msg.Add((long)eGTC_Failed);
					msg.Add((long)eGTC_ErrAssistant);
					break;
				}
				lLogType = 3;
			}
		}
		

		//! 执行操作
		if (isBind)
		{
			if(0 != SetGoodsBindProperty_Player(arr_pGoods[1], lOperation, pMsg->GetPlayerID()))
			{
				msg.Add((long)eGTC_Failed);
				msg.Add((long)eGTC_CantOperateAim);
				break;
			}
		}
		else
		{
			if(0 != SetGoodsFrostProperty_Player(arr_pGoods[1], lOperation, pMsg->GetPlayerID()))
			{
				msg.Add((long)eGTC_Failed);
				msg.Add((long)eGTC_CantOperateAim);
				break;
			}
		}
		
		GetLogicLogInterface()->logT421_goods_bind_and_frost(pPlayer, arr_pGoods[1], arr_pGoods[0], lLogType);

		//! 删除消耗物
		vector<tagGoodsOperate> vDelGoods;
		tagGoodsOperate GoodsOperate;
		GoodsOperate.lContianerPos	= arrlContianerID[0];
		GoodsOperate.lGoodsPos		= arrlPos[0];
		GoodsOperate.lOperateNum	= 1;
		GoodsOperate.lOperateWay	= (arr_pGoods[0]->GetAmount() > 1) ? CS2CContainerObjectMove::OT_CHANGE_AMOUNT : CS2CContainerObjectMove::OT_DELETE_OBJECT;
		vDelGoods.push_back(GoodsOperate);

		if (!DelGoods(pPlayer, vDelGoods, 0, NULL_GUID))
		{
			msg.Add((long)eGTC_Failed);
			msg.Add((long)eGTC_DelAssistantFailed);
			break;
		}

		msg.Add((long)eGTC_Succeed);


	} while(false);

	msg.SendToPlayer(pMsg->GetPlayerID(), false);
}


