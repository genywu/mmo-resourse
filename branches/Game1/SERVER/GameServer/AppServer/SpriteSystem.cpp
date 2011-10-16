
#include "stdafx.h"
#include "SpriteSystem.h"


SpriteSystem::SpriteSystem(void)
{

}

SpriteSystem::~SpriteSystem(void)
{

}

map<LONG, DWORD> SpriteSystem::c_mapCanUseGoodsIndex;
vector<LONG> SpriteSystem::c_vectorCanUseFightSkill;
vector<LONG> SpriteSystem::c_vectorCanUseAssistSkill;
vector<LONG> SpriteSystem::c_vectorCanUseAssistGoods;

//! 配置编码
void SpriteSystem::SetupAddToByteArray(vector<BYTE> &ByteArray)
{
	_AddToByteArray(&ByteArray, (LONG)c_mapCanUseGoodsIndex.size());
	for (map<LONG, DWORD>::iterator ite = c_mapCanUseGoodsIndex.begin(); ite != c_mapCanUseGoodsIndex.end(); ++ite)
	{
		_AddToByteArray(&ByteArray, ite->first);
		_AddToByteArray(&ByteArray, ite->second);
	}
    //战斗技能
    _AddToByteArray(&ByteArray, (LONG)c_vectorCanUseFightSkill.size());
    for (vector<LONG>::iterator ite = c_vectorCanUseFightSkill.begin(); ite != c_vectorCanUseFightSkill.end(); ++ite)
    {
        _AddToByteArray(&ByteArray, *ite);
    }
    //辅助技能
    _AddToByteArray(&ByteArray, (LONG)c_vectorCanUseAssistSkill.size());
    for (vector<LONG>::iterator ite = c_vectorCanUseAssistSkill.begin(); ite != c_vectorCanUseAssistSkill.end(); ++ite)
    {
        _AddToByteArray(&ByteArray, *ite);
    }
    //辅助道具
    _AddToByteArray(&ByteArray, (LONG)c_vectorCanUseAssistGoods.size());
    for (vector<LONG>::iterator ite = c_vectorCanUseAssistGoods.begin(); ite != c_vectorCanUseAssistGoods.end(); ++ite)
    {
        _AddToByteArray(&ByteArray, *ite);
    }
}

//! 配置解码
void SpriteSystem::SetupGetFormByteArray(BYTE *pByteArray, long &lPos)
{
	LONG lGoodsNum = _GetLongFromByteArray(pByteArray, lPos);
	for (LONG i = 0; i < lGoodsNum; ++i)
	{
		LONG lIndex		= _GetLongFromByteArray(pByteArray, lPos);
		DWORD dwBound	= _GetLongFromByteArray(pByteArray, lPos); 
		c_mapCanUseGoodsIndex[lIndex] = dwBound;
	}
    LONG lFightSkillNum = _GetLongFromByteArray(pByteArray, lPos);
    for (LONG l = 0; l < lFightSkillNum; ++l)
    {
        LONG lID        = _GetLongFromByteArray(pByteArray, lPos);
        c_vectorCanUseFightSkill.push_back(lID);
    }
    LONG lAssistSkillNum = _GetLongFromByteArray(pByteArray, lPos);
    for (LONG l = 0; l < lAssistSkillNum; ++l)
    {
        LONG lID        = _GetLongFromByteArray(pByteArray, lPos);
        c_vectorCanUseAssistSkill.push_back(lID);
    }
    LONG lAssistGoodsNum = _GetLongFromByteArray(pByteArray, lPos);
    for (LONG l = 0; l < lAssistGoodsNum; ++l)
    {
        LONG lID        = _GetLongFromByteArray(pByteArray, lPos);
        c_vectorCanUseAssistGoods.push_back(lID);
    }
}

//!							配置编码
void SpriteSystem::CodeSetupToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray( (LONG)c_mapCanUseGoodsIndex.size());
	for (map<LONG, DWORD>::iterator ite = c_mapCanUseGoodsIndex.begin(); ite != c_mapCanUseGoodsIndex.end(); ++ite)
	{
		setWriteDB.AddToByteArray( ite->first);
		setWriteDB.AddToByteArray( ite->second);
	}
}
	//!							配置解码
void SpriteSystem::DecodeSetupFromDataBlock(DBReadSet& setReadDB)
{
	LONG lGoodsNum = setReadDB.GetLongFromByteArray();
	for (LONG i = 0; i < lGoodsNum; ++i)
	{
		LONG lIndex		= setReadDB.GetLongFromByteArray();
		DWORD dwBound	= setReadDB.GetLongFromByteArray(); 
		c_mapCanUseGoodsIndex[lIndex] = dwBound;
	}
}

//! 测试一个物品能否用于自动补给
BOOL SpriteSystem::TestAutoSupplyGoods(LONG lGoodsIdx)
{
	return (c_mapCanUseGoodsIndex.end() != c_mapCanUseGoodsIndex.find(lGoodsIdx));
}

//! 测试一个技能和辅助道具能否使用
BOOL SpriteSystem::TestAutoFightSkill(LONG lSkillID)
{
    vector<LONG>::iterator ite = c_vectorCanUseFightSkill.begin();
    for (; ite != c_vectorCanUseFightSkill.end(); ++ite)
    {
        if ((*ite) == lSkillID)
        {
            return TRUE;
        }
    }
    return FALSE;
}

//!  测试一个战斗辅助技能能否使用
BOOL SpriteSystem::TestAutoAssistSkill(LONG lSkillID)
{
    vector<LONG>::iterator ite = c_vectorCanUseAssistSkill.begin();
    for (; ite != c_vectorCanUseAssistSkill.end(); ++ite)
    {
        if ( (*ite) == lSkillID)
        {
            return TRUE;
        }
    }
    return FALSE;
}

//!   测试一个辅助道具能否使用
BOOL SpriteSystem::TestAutoAssistGoods(LONG lGoodsID)
{
    vector<LONG>::iterator ite = c_vectorCanUseAssistGoods.begin();
    for (; ite != c_vectorCanUseAssistGoods.end(); ++ite)
    {
        if ( (*ite) == lGoodsID)
        {
            return TRUE;
        }
    }
    return FALSE;
}

//! 设置所有者
void SpriteSystem::SetOwner(CPlayer *pMainPlayer)
{
	m_AutoSupply.SetOwner(pMainPlayer);
    m_AutoFight.SetOwner(pMainPlayer);
}

//! 编码
void SpriteSystem::AddToByteArray(vector<BYTE> &ByteArray)
{
	m_AutoSupply.AddToByteArray(ByteArray);
}

//! 解码
void SpriteSystem::GetFormByteArray(BYTE *pByteArray, long &lPos)
{
	//! ----------------自动补给----------------
	m_AutoSupply.GetFormByteArray(pByteArray, lPos);
	//! 处理老设置与新配置之间的冲突
	const tagAotoSupplyNode *pAotoSupplyNode = NULL;
	for (LONG i = 0; i < AotoSupplyTypeNum; ++i)
	{
		pAotoSupplyNode = m_AutoSupply.GetOneSetup(i);
		if (TestAutoSupplyGoods(pAotoSupplyNode->lGoodsIndex))
		{
			//! 调用设置接口ChangeSetup才能保证数据改变标记被更新，因此这里作了多余的对象拷贝
			tagAotoSupplyNode AotoSupplyNode;
			memmove(&AotoSupplyNode, pAotoSupplyNode, sizeof(tagAotoSupplyNode));
			m_AutoSupply.ChangeSetup(i, AotoSupplyNode, TRUE);
		}
	}
	
	//! ----------------   ----------------
}

//!							编码
void SpriteSystem::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	m_AutoSupply.CodeToDataBlock(setWriteDB);
    //setWriteDB.AddToByteArray(m_AutoFight.GetStableTime());
    //编码稳定时间值发给WS
    m_AutoFight.CodeToDataBlock(setWriteDB);
}
//!							解码
void SpriteSystem::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	//! ----------------自动补给----------------
	m_AutoSupply.DecodeFromDataBlock(setReadDB);
	//! 处理老设置与新配置之间的冲突
	const tagAotoSupplyNode *pAotoSupplyNode = NULL;
	for (LONG i = 0; i < AotoSupplyTypeNum; ++i)
	{
		pAotoSupplyNode = m_AutoSupply.GetOneSetup(i);
		if (TestAutoSupplyGoods(pAotoSupplyNode->lGoodsIndex))
		{
			//! 调用设置接口ChangeSetup才能保证数据改变标记被更新，因此这里作了多余的对象拷贝
			tagAotoSupplyNode AotoSupplyNode;
			memmove(&AotoSupplyNode, pAotoSupplyNode, sizeof(tagAotoSupplyNode));
			m_AutoSupply.ChangeSetup(i, AotoSupplyNode, TRUE);
		}
	}
    //m_AutoFight.SetStableTime(setReadDB.GetLongFromByteArray());
    //从WS获得稳定时间
    m_AutoFight.DecodeFromDataBlock(setReadDB);
}


//! 响应自动补给设置修改
void SpriteSystem::OnChangeAutoSupply(CMessage *pMsg)
{
	LONG eSupplyType = pMsg->GetLong();
	tagAotoSupplyNode AotoSupplyNode;
	pMsg->GetEx(&AotoSupplyNode, sizeof(tagAotoSupplyNode));

	BOOL bReValue = eASET_NotErr;
	if(TestAutoSupplyGoods(AotoSupplyNode.lGoodsIndex) || 0 == AotoSupplyNode.lGoodsIndex)
	{
		bReValue = m_AutoSupply.ChangeSetup(eSupplyType, AotoSupplyNode);
	}
	else
	{
		bReValue = eASET_ValueErr;
	}

	CMessage msg(MSG_S2C_SPRITE_AutoSupply_ChangeSetup_Re);
	if(bReValue)
	{
		msg.Add((LONG)bReValue);
		msg.Add(eSupplyType);
		msg.AddEx(&AotoSupplyNode, sizeof(tagAotoSupplyNode));
	}
	else
	{
		msg.Add((LONG)bReValue);
		msg.Add(eSupplyType);
		//msg.Add(lReValue);
	}

	msg.SendToPlayer(pMsg->GetPlayerID(), false);
}

//! 响应自动打怪设置
void SpriteSystem::OnChangeAutoFight(CMessage *pMsg)
{
    LONG eFightType = pMsg->GetLong();

    tagAutoFightNode AutoFightNode;
    pMsg->GetEx(&AutoFightNode, sizeof(tagAutoFightNode));

    LONG bReValue = eASET_NotErr;
    switch((eAutoHookType)eFightType)
    {
    case eAHT_COMMON:
    case eAHT_STABLE:
    case eAHT_REPAIR:
        {
            //开启或者关闭辅助挂机
            bReValue = m_AutoFight.ChangeSetup(eFightType, AutoFightNode);

            CMessage msg(MSG_S2C_SPRITE_AutoFight_ChangeSetup_Re);
            if (bReValue == eASET_NotErr)
            {
                msg.Add( (LONG)bReValue);
                msg.Add(eFightType);
                msg.AddEx(&AutoFightNode, sizeof(tagAutoFightNode));
            }
            else
            {
                msg.Add((LONG)bReValue);
                msg.Add(eFightType);
            }
            msg.SendToPlayer(pMsg->GetPlayerID(), false);
        }
        break;
    case eAHT_FIGHTSKILL:
        {
            //检查自动战斗技能能否放置
            bReValue = TestAutoFightSkill(AutoFightNode.lFightTypeIndex);

            CMessage msg(MSG_S2C_SPRITE_AutoFight_ChangeSetup_Re);
            if (bReValue)
            {
                msg.Add((LONG)eASET_CanUse);
                msg.Add(eFightType);
                msg.AddEx(&AutoFightNode, sizeof(tagAutoFightNode));
            }
            else
            {
                msg.Add((LONG)eASET_NoCan);
                msg.Add(eFightType);
                msg.AddEx(&AutoFightNode, sizeof(tagAutoFightNode));
            }
            msg.SendToPlayer(pMsg->GetPlayerID(), false);
        }
        break;
    case eAHT_ASSISTSKILL:
        {
            //检查自动辅助技能能否放置
            bReValue = TestAutoAssistSkill(AutoFightNode.lFightTypeIndex);

            CMessage msg(MSG_S2C_SPRITE_AutoFight_ChangeSetup_Re);
            if (bReValue)
            {
                msg.Add((LONG)eASET_CanUse);
                msg.Add(eFightType);
                msg.AddEx(&AutoFightNode, sizeof(tagAutoFightNode));
            }
            else
            {
                msg.Add((LONG)eASET_NoCan);
                msg.Add(eFightType);
                msg.AddEx(&AutoFightNode, sizeof(tagAutoFightNode));
            }
            msg.SendToPlayer(pMsg->GetPlayerID(),false);
        }
        break;
    case eAHT_ASSISTGOODS:
        {
            //检查自动辅助道具能否放置
            bReValue = TestAutoAssistGoods(AutoFightNode.lFightTypeIndex);

            CMessage msg(MSG_S2C_SPRITE_AutoFight_ChangeSetup_Re);
            if (bReValue)
            {
                msg.Add((LONG)eASET_CanUse);
                msg.Add(eFightType);
                msg.AddEx(&AutoFightNode, sizeof(tagAutoFightNode));
            }
            else
            {
                msg.Add((LONG)eASET_NoCan);
                msg.Add(eFightType);
                msg.AddEx(&AutoFightNode, sizeof(tagAutoFightNode));
            }
            msg.SendToPlayer(pMsg->GetPlayerID(), false);
        }
        break;
    default:
        break;
    }
}

//! 判断自动修理装备的条件
bool SpriteSystem::CanAutoRepair(void)
{
   const tagAutoFightNode* pAutoFightNode;

   pAutoFightNode = m_AutoFight.GetOneSetup((LONG)eAHT_REPAIR);
    if (pAutoFightNode->bOpen)
    {
        //银币不足判断
        if (m_AutoFight.CanAutoRepair())
            return true;
        else
            return false;
    }
    return false;
}