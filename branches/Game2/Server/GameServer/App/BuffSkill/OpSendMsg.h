//=============================================================================
/**
 *  file: OpSendMsg.h
 *
 *  Brief:包装了发送消息的操作
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-9-5
 */
//=============================================================================

#ifndef __OPERATOR_SENDMSG_H__
#define __OPERATOR_SENDMSG_H__
#pragma once

#include "Operator.h"
#include "BuffSkillDef.h"

class CMoveShape;
class CScript;

class COpSendMsg :
	public COperator
{
public:
	COpSendMsg(stModuParam *pParam);
	COpSendMsg(void * pParam);
	virtual ~COpSendMsg(void);

	enum eSendMsgType
	{
		SMT_ProcNo,	  //proce未知
		SMT_ProcBegin,//proce开始
		SMT_ProcEnd,//proce结束
		SMT_ProcFail,//失败
	};
	//消息的发送范围
	enum eMsgRange
	{
		MsgRange_Single,//单个
		MsgRange_Around,//周围
		MsgRange_Region,//场景
	};
private:
	//类型
	eSendMsgType m_eMsgType;
	//范围
	eMsgRange m_eRange;
	//特殊值
	long m_lValue;
	//特殊值脚本
	string m_strVariaName;
	//更新标示符
	BYTE  m_bUpdate;
	//私有接口
private:
	//发送技能开始消息
	void CreateSkillBeginMsg();
	//发送技能结束消息
	void CreateSkillEndMsg();
	//发送技能失败消息
	void CreateSKillFailMsg();

	//发送buff开始消息
	void CreateBuffBeginMsg();
	//发送状态更新消息
	void CreateBuffUpdateMsg();
	//发送buff结束消息
	void CreateBuffEndMsg();
	//锁定物品消息
	void SendLockGoodsMsg();

	typedef void (COpSendMsg::*CREATEMSG)(void);

	CREATEMSG m_pCreateMsg;

public:
	//初始化
	void InitBaseData(eObj Obj,ModuleType eMType, eSendMsgType eType,eMsgRange eRange, long value,string strVname,int nAdd)
	{
		m_eMsgType = eType;
		m_eRange = eRange;
		m_lValue = value;
		m_strVariaName = strVname;
		m_bUpdate = nAdd;
		m_pCreateMsg = GetCreateMsg(eMType,m_eMsgType);

	}
	CREATEMSG GetCreateMsg(ModuleType eMType,eSendMsgType eMsgType)
	{
		CREATEMSG Msg;
		if (eMsgType==SMT_ProcBegin && (eMType==ModuType_Buff || eMType==ModuType_DeBuff))
		{
			Msg = &COpSendMsg::CreateBuffBeginMsg;
		} 
		else if(eMsgType==SMT_ProcEnd && (eMType==ModuType_Buff || eMType==ModuType_DeBuff))
		{
			Msg = &COpSendMsg::CreateBuffEndMsg;
		}
		else if (eMsgType==SMT_ProcBegin && (eMType==ModuType_Skill_Active 
										  || eMType==ModuType_Skill_Passive
										  || eMType==ModuType_Skill_OnOff
										 ))// || eMType==ModuType_Skill_Light
		{
			Msg = &COpSendMsg::CreateSkillBeginMsg;
		}
		else if (eMsgType==SMT_ProcEnd && (eMType==ModuType_Skill_Active 
										 || eMType==ModuType_Skill_Passive
										 || eMType==ModuType_Skill_OnOff
										 ))//|| eMType==ModuType_Skill_Light
		{
			Msg = &COpSendMsg::CreateSkillEndMsg;
		}
		else if (eMsgType==SMT_ProcFail && (eMType==ModuType_Skill_Active 
										 || eMType==ModuType_Skill_Passive
										 || eMType==ModuType_Skill_OnOff										 
										 || eMType==ModuType_Buff 
										 || eMType==ModuType_DeBuff))//|| eMType==ModuType_Skill_Light
		{
			Msg = &COpSendMsg::CreateSKillFailMsg;
		}
		return Msg;
	}

	
	//执行
    void Execute(CMoveShape* pShape);
};


//发送攻击产生的效果消息
class COpSendDamageMsg :
	public COperator
{
public:
	COpSendDamageMsg(stModuParam *pParam);
	virtual ~COpSendDamageMsg(void);

	enum eSendMsgType
	{
		SMT_SelfDamage,//普通受伤消息
		SMT_DestDamage,//受伤消息
		SMT_DestDodge,//发送闪避消息
		SMT_DestFightBlock,//发送格挡消息
		SMT_DestParry,//发送招架消息
		SMT_DestResist,//发送抵抗消息
		SMT_DestSuck,//发送吸收消息
		SMT_DestRebound,//反弹消息
	};

private:
	//普通减血消息
	void CreateSelfDamageMsg(CMoveShape* pShape);
	//减血消息
	void CreateDestDamageMsg(CMoveShape* pShape);
	//创建闪避消息
	void CreateDestDodgeMsg(CMoveShape* pShape);
	//创建格挡消息
	void CreateDestBlockMsg(CMoveShape* pShape);
	//招架消息
	void CreateDestParryMsg(CMoveShape* pShape);
	//抵抗消息
	void CreateDestResistMsg(CMoveShape* pShape);
	//吸收消息
	void CreateDestSuckMsg(CMoveShape* pShape);
	//反弹消息
	void CreateDestReboundMsg(CMoveShape* pShape);

	typedef void COpSendDamageMsg::CREATEMSG(CMoveShape* pShape);

	CREATEMSG* m_pCreateMsg;

	//类型
	eSendMsgType m_eMsgType;
	//是否暴击
	long m_lBlastAttack;;
	//特殊值脚本
	string m_strBlastVariaName;
public:
	//执行
    void Execute(CMoveShape* pShape);
};
#endif
