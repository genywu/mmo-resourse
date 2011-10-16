#include "StdAfx.h"
#include "PetCtrl.h"
#include "Pet.h"
#include "Player.h"
#include "Monster.h"
#include "ServerRegion.h"
#include "../../Public/ConfigParse.h"
#include "../../GameServer/GameServer/GameManager.h"
#include "../../NETS/NetServer/Message.h"


// 注册消息响应函数
BEGIN_MSG_MAP(CPetCtrl)
	//ON_REGISTER_MEM_FUNC(GMSG_C2G_CONNECTED, &CPetCtrl::CheckLogic)
	//ON_REGISTER_MEM_FUNC(GMSG_C2C_TRANSMIT, &CPetCtrl::TransmitMsg)
END_MSG_MAP()

CPetCtrl::CPetCtrl(void)
{
}

CPetCtrl::~CPetCtrl(void)
{
}

// 配置解码
bool CPetCtrl::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	setReadDB.GetBufferFromByteArray(&m_ConfPet, sizeof(m_ConfPet));
	long lNum = setReadDB.GetLongFromByteArray();
	tagPetBase tPetConf;
	for( long l=0; l<lNum; ++l )
	{
		setReadDB.GetBufferFromByteArray(&tPetConf, sizeof(tPetConf));
		m_ConfPets[tPetConf.lIndex] = tPetConf;
	}

	lNum = setReadDB.GetLongFromByteArray();
	long lIndex = 0;
	long lSecs = 0;
	tagPetOrig tPetOrig;
	GROW_LIST PetOrigs;
	for( long l=0; l<lNum; ++l )
	{
		lIndex = setReadDB.GetLongFromByteArray();
		PetOrigs.clear();
		lSecs = setReadDB.GetLongFromByteArray(); 
		for( long m=0; m<lSecs; ++m )
		{
			setReadDB.GetBufferFromByteArray(&tPetOrig, sizeof(tPetOrig));
			PetOrigs.push_back(tPetOrig);
		}
		m_GrowConfPets[lIndex] = PetOrigs;
	}
	return true;
}

// GS==>Client编码
bool CPetCtrl::CodeToDataBlockForClient(DBWriteSet& setWriteDB,
										CPlayer* pPlayer)
{
	C_PETS crPets = pPlayer->GetPets();
	long lNum = (long)crPets.size();
	setWriteDB.AddToByteArray(lNum);
	if( 0 == lNum ) return true;

	CPet* pPet = NULL;
	C_ITR_PET itrPet = crPets.begin();
	for( ; itrPet != crPets.end(); ++itrPet )
	{
		pPet = itrPet->second;
		if( pPet != NULL )
		{
			if( pPet->m_BaseAttrs.lPetType != PET_TYPE_NULL )
			{
				setWriteDB.AddToByteArray(pPet->m_BaseAttrs.lPetType);
				setWriteDB.AddToByteArray(pPet->GetExID());
				setWriteDB.AddToByteArray(pPet->m_BaseAttrs.lPicID);
				setWriteDB.AddToByteArray(pPet->GetName());
			}
			else
			{
				setWriteDB.AddToByteArray(long(0));
			}
		}
		else
		{
			setWriteDB.AddToByteArray(long(0));
		}
	}

	return true;
}

// GS==>WS编码
bool CPetCtrl::CodeToDataBlock(DBWriteSet& setWriteDB,
							   CPlayer* pPlayer)
{
	CONDITION_CHECK(m_ConfPet.nFuncSwitch);

	char szLog[512];
	char szGUID[128];
	CPet* pPet = NULL;
	C_PETS rPets = pPlayer->GetPets();
	C_ITR_PET itrPet = rPets.begin();
	long lNum = (long)rPets.size();
	setWriteDB.AddToByteArray(lNum);
	for( ; itrPet != rPets.end(); ++itrPet )
	{
		if( NULL == (pPet = itrPet->second) )
		{
			setWriteDB.AddToByteArray(char(0));
			itrPet->first.tostring(szGUID);
			_snprintf(szLog, 512, "异常: 玩家%s没有GUID为%s的宠物.",
				pPlayer->GetName(),
				szGUID);
			PutoutLog("Pet", LT_ERROR, szLog);
			continue;
		}
		setWriteDB.AddToByteArray(char(1));
		pPet->CodeToDataBlock(setWriteDB);
	}
	return true;
}

// WS==>GS解码
bool CPetCtrl::DecodeFromDataBlock(DBReadSet& setReadDB,
								   CPlayer* pPlayer)
{
	CONDITION_CHECK(m_ConfPet.nFuncSwitch);

	pPlayer->ReleaseAllPets();	// 清空宠物列表
	long lNum = setReadDB.GetLongFromByteArray();
	if( 0 == lNum ) return true;
	CGUID guID;
	char cFlag = 0;
	CPet* pPet = NULL;
	for( long i=0; i<lNum; ++i )
	{
		cFlag = setReadDB.GetCharFromByteArray();
		if( 0 == cFlag )
			continue;

		pPet = pPlayer->CreatePet(NULL_GUID);

		// 如果宠物解码成功, 则为玩家添加宠物.
		// 否则, 直接删除宠物对象.
		if( pPet->DecodeFromDataBlock(setReadDB) )
		{
			pPlayer->AddPet(pPet);
		}
		else // 解码失败(生命周期终止)
		{
			MP_DELETE(pPet);
		}
	}

	return true;
}

// 激活宠物
void CPetCtrl::ActivatePet(CPet* pPet)
{
	CPlayer* pPlayer = pPet->m_pHost;
	if( NULL == pPlayer )
	{
		PutoutLog("Pet", LT_ERROR, "激活失败, 无主人.");
		return;
	}

	m_Pets[pPet->GetExID()] = pPet;
	pPet->InitialByHost(IT_LOGIN_GS);
	InformClient(pPlayer, pPet, true);
	pPet->OnActive();
}

// 宠物删除处理
void CPetCtrl::OnDeletePet(void)
{
	if( m_DetectPets.empty() ) return;

	char szLog[512];
	char szGUID[64];
	static long lCount = 0;
	vector<CPet*>::iterator itr = m_DetectPets.begin();
	CPet* pPet = NULL;
	while( !m_DetectPets.empty() )
	{
		PutoutLog("PetCD", LT_DEFAULT, "Del pet.");
		if( (pPet = *itr) != NULL )
		{
			pPet->GetExID().tostring(szGUID);
			_snprintf(szLog, 512, "Pet(%s: %s) will be delete.",
				pPet->GetName(),
				szGUID);
			PutoutLog("PetCD", LT_DEFAULT, szLog);
			DELETE_SAFE(pPet);
		}
		m_DetectPets.erase(itr++);
	}
}

CPet* CPetCtrl::GetPet(const CGUID& rID)
{
	CPet* pPet = NULL;
	C_ITR_PET itr = m_Pets.find(rID);
	if( itr != m_Pets.end() )
	{
		pPet = itr->second;
	}
	return pPet;
}

// 删除(未)激活宠物
void CPetCtrl::DelPet(CPet* pPet, long lOpType)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pPet->GetHost());
	assert(pPlayer != NULL);

	if( pPet->IsActive() )
	{
		pPet->OnHouse(lOpType);
		if(lOpType != PET_OT_BEEN_KILLED)
		{
			InformClient(pPlayer, pPet, false);
		}
	}
	else
	{
		pPet->DelFromRegion(lOpType);
		InformClient(pPlayer, pPet, false);
	}

	m_Pets.erase(pPet->GetExID());

	if( lOpType & PET_OT_DELETE )
	{
		pPlayer->DelPet(pPet->GetExID());
		m_DetectPets.push_back(pPet);
		pPet->SetHost(NULL);

		if( pPet->NeedBeSaved() )
		{
			InformWS(pPlayer);
		}
	}
}

// 复活宠物
bool CPetCtrl::RelivePet(CPet* pPet, long lInitType)
{
	// 检查宠物配置
	CPlayer* pPlayer = (CPlayer*)(pPet->GetHost());
	long lIndexID = pPet->GetIndexID();
	C_ITR_PET_LIST itrPet = m_ConfPets.find(lIndexID);
	if( m_ConfPets.end() == itrPet ||
		NULL == pPlayer )
	{
		PutoutLog("PET", LT_ERROR,
			"主人为NULL || 查表失败, 未找到对应宠物信息, 宠物索引编号: %ld.",
			lIndexID);
		return false;
	}

	return pPet->InitialDiffAttrs(lInitType);
}

// 发送消息给WS, 此处仅设标志位, 到达存盘点方保存宠物.
void CPetCtrl::InformWS(CPlayer* pPlayer)
{
	GetGame()->UpdateSavePlayer(pPlayer, SAVE_DETAIL_PETS);
}

// 发送消息给主人
void CPetCtrl::InformClient(CPlayer* pPlayer, CPet* pPet, bool bAdd)
{
	assert(pPlayer != NULL);
	assert(pPet != NULL);

	if( bAdd )
	{
		CMessage MsgHost(MSG_S2C_RGN_ADDSHAPE);
		MsgHost.Add(pPet->GetType());
		MsgHost.Add(pPet->GetExID());
		MsgHost.Add(pPet->m_BaseAttrs.lPetType);
		MsgHost.Add(char(1));		// 主人标志位
		DBWriteSet setWriteDBHost;
		MsgHost.GetDBWriteSet(setWriteDBHost);
		pPet->CodeToDataBlock_ForClient(setWriteDBHost);
		MsgHost.Add(char(0));		// 是否出现特效
		MsgHost.SendToPlayer(pPlayer->GetExID());
	}
	else
	{
		CMessage MsgAround(MSG_S2C_RGN_DELSHAPE);
		MsgAround.Add(pPet->GetType());
		MsgAround.Add(pPet->GetExID());
		MsgAround.Add((char)0);		// 是否出现特效
		MsgAround.SendToPlayer(pPlayer->GetExID());
	}
}

// 通知客户端AI事件
void CPetCtrl::InformClientAI(CPet* pPet, long lOpType)
{
	CPlayer* pPlayer = (CPlayer*)pPet->GetHost();

	// 发送至主人
	CMessage msg(MSG_S2C_PET_AI);
	msg.Add(pPet->GetExID());
	msg.Add(lOpType);
	switch( lOpType )
	{
	case PET_OT_AI_SET_TARGET:
		{
			const CMoveShape* pTarget = pPet->GetTarget();
			if( pTarget != NULL )
			{
				msg.Add(pTarget->GetType());
				msg.Add(pTarget->GetExID());
			}
			else
			{
				msg.Add((long)0);
				msg.Add(NULL_GUID);
			}
		}
		break;
	}

	msg.SendToPlayer(pPlayer->GetExID());
}

// 创建宠物
CPet* CPetCtrl::CreatePet(CPlayer* pPlayer, long lType, long lIndexID)
{
	if( NULL == pPlayer )
	{
		PutoutLog("Pet", LT_ERROR, "Player == NULL.");
		return NULL;
	}

	// 如果存在同类型激活宠物
	CPet* pOldPet = pPlayer->GetActivePet(lType);
	if( pOldPet != NULL && pOldPet->IsActive() )
	{
		char szGUID[64];
		pOldPet->GetExID().tostring(szGUID);
		PutoutLog("Pet", LT_ERROR,
			"宅藏玩家%s已经激活拥有的宠物<%ld:%ld:%s>.",
			pPlayer->GetName(),
			lType,
			lIndexID,
			szGUID);
		DelPet(pOldPet, PET_OT_HOUSE);
	}

	// 创建宠物
	// 1) 创建陷阱
	// 2) 所创建类型宠物不在列表中
	long lInitType = IT_RELIVE;
	CPet* pPet = pPlayer->GetPet(lIndexID);
	if( pPet == NULL )
	{
		CGUID guID;
		CGUID::CreateGUID(guID);
		pPet = pPlayer->CreatePet(guID);
		pPet->SetIndexID(lIndexID);
		pPet->SetPetType(lType);
		lInitType = IT_CREATE;
	}

	if( NULL == pPet )
	{
		PutoutLog("Pet", LT_ERROR,
			"玩家%s创建宠物<%ld>失败.",
			pPlayer->GetName(),
			lIndexID);
		return NULL;
	}
	else
	{
		pPet->m_DiffAttrs.lActive = 1;
	}

	// 重置宠物
	if( RelivePet(pPet, lInitType) )
	{
		// 激活宠物
		ActivatePet(pPet);
		if( pPet->NeedBeSaved() )
		{
			InformWS(pPlayer);
		}

		return pPet;
	}

	return NULL;
}

// 删除宠物(为HOUSE)
bool CPetCtrl::DelPet(const CGUID& rID)
{
	// 如果存在同类型激活宠物
	CPet* pPet = GetPet(rID);
	if( pPet != NULL )
	{
		DelPet(pPet, PET_OT_HOUSE);
	}

	return true;
}

// 复活宠物
bool CPetCtrl::RelivePet(CPlayer* pPlayer, long lType, long lIndexID)
{
	CPet* pPet = CreatePet(pPlayer, lType, lIndexID);
	if (pPet != NULL)
	{
		pPet->SetHP(pPet->GetMaxHP() >> 1);
		pPet->SetMP(pPet->GetMaxMP() >> 1);
		return true;
	}

	return false;
}

// 添加宠物技能
bool CPetCtrl::AddPetSkill(CPlayer* pPlayer,
						   long lType,
						   long lID,
						   long lLevel,
						   long lPos)
{
	if( NULL == pPlayer )
	{
		PutoutLog("Pet", LT_ERROR, "Player == NULL.");
		return false;
	}

	char szLog[512];
	// 检查该类型是否存在被激活宠物
	CPet* pPet = pPlayer->GetActivePet(lType);
	if( NULL == pPet )
	{
		sprintf_s(szLog, sizeof(szLog),
			"玩家%s不存在类型为%ld的宠物.",
			pPlayer->GetName(), lType);
		PutoutLog("PET", LT_ERROR, szLog);
		return false;
	}

	if( lType == PET_TYPE_ELF ||			// 精灵追加天赋技能是自动追加, 此处只要为测试目的
		0 == lID ||							// 默认为追加配置技能
		0 == lLevel ||
		-1 == lPos )
	{
		pPet->AppendGeniusSkill();
		return true;
	}
	return pPet->ModifySkill(lID, lLevel, lPos);
}