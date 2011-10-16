///////////////////////////////////////////////////////////////////////////////
///  MailManager.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\GameServer\AppServer\Mail
///
///  CREATED: 09/12/2008  by 陈先进
///
///  PURPOSE:   邮件管理类
///
//////////////////////////////////////////////////////////////////////
#include <map>
#include "Mail.h"
#include "../BaseObject.h"
#pragma once

class CMailManager : public BaseMemObj
{
public:
	CMailManager();
	~CMailManager();

	bool		AddMail(CMail *pMail,CPlayer *pPlayer=NULL);							//发信
	bool		SendMailToWS(CMail *pMail);												//向ws发信
	void	    SendMailToClient(CGUID guid,CMail *pMail);								//向客户端发信 const CGUID &guid,
	void		SendAllMailToClient(vector<BYTE> &vectorByte,CPlayer *pPlayer);			//向客户端返回所有邮件信息
	void		DeleteMail(list<CGUID> &guid,CPlayer *pPlayer);							//发送删除邮件到ws
	void		DeleteMailToClient(list<CGUID> &guid,CPlayer *pPlayer);					//发送删除邮件到Client
	void		DeleteMailToWS(list<CGUID> &guid,CPlayer *pPlayer);						//发送删除邮件到WS
	void		ReadMail(CGUID &guid,CPlayer *pPlayer);									//读取邮件
	void		GetGoodsFromMail(CGUID &guid,CPlayer *pPlayer,long lIndex);				//从邮箱取物品放到背包
	void		GetGoldFromMail(CGUID &guid,CPlayer *pPlayer);							//从邮箱中取金钱
	void		SendGetGoldFromMailToWS(CGUID &guid,CPlayer *pPlayer);					//向WS发送取金钱消息
	void		PayMoneyMail(CMail *pMail,CPlayer *pPlayer);							//付款收信
	
	bool		CheckMoney(CMail *pMail, CPlayer *pPlayer);								//检查金钱
	bool		CheckInvalidCharacter(string strText);									//检查是否有非法字符
	bool		CheckPostage(CMail *pMail, CPlayer *pPlayer);							//检查和扣除邮资
	bool		CheckManyReceiver(CMail *pMail);										//检查多收信中是否包含物品金钱	
	bool		CheckReceiver(CMail *pMail,CPlayer *pPlayer);							//检查收信人是否是自己	
	void		SendPayMailMsg(const CGUID &guid,BYTE bValue,CPlayer *pPlayer);			//发送付款收信消息到客户端
	long		ReMoveGoldAndGoods(CMail *pMail,CPlayer *pPlayer);						//发送邮件给收信人
	void		SendMailToReceiver(CGUID &guid,long lRet);								//发送扣除物品金钱的结果
	void		SendGetGoodsGSToClient(CGUID &MailGuid,CGUID &PlayerGuid,long lIndex);	//发送取物品给客户端

	void		SendMailFailedToClient(CGUID guid,long lValue);							//邮件发送失败	
	void		DecordFromByteArray(BYTE* pByte, long& pos,tagMailParam *pMailParam);	//解码
	void		DecordMailMsgFromByteArray(BYTE* pByte, long& pos,tagMailParam *pMailParam);//解码邮件信息	
	long		GetYuanBaoNum(long lIndex,long lNum,CPlayer *pPlayer);					//获取元宝数量	
	void		SendPMail(CGUID &MailGuid,CPlayer *pPlayer,long lRet);					//赠送类邮件
	void		SendPresentMailToWS(CMail *pMail,long lRet);							//发送赠送邮件到WS
	void		SendReceiveMail(CPlayer *pPlayer);										//收信
	void		AgreeToPresent(CMail *pMail,CPlayer *pPlayer,BYTE bType);				//同意赠送
	bool	    CheckPresentGoods(CMail *pMail,CPlayer *pPlayer,long &lNum);			//检查增值物品
	void		BuyItemByMail(CMail *pMail,CPlayer *pPlayer);							//通过邮件购买增值道具	
	bool		CheckDeleteMail(list<CGUID> &lguid,CPlayer *pPlayer);									//检查删信	
	bool		SetMapMail(CMail *pMail);												//加信

	void		SendReadMailRet(CGUID &MailGuid,const CGUID &PlayerGuid, BYTE bRet);			//发送读取邮件结果	
	eMAILRESULT	CheckMailCondition(CMail *pMail, CPlayer *pPlayer);						//检查邮件发送条件	
	CMail		*GetMail(CGUID &guid);													//获取邮件

	//系统邮件
	void		InitSysMail();															//初始化系统邮件
	void		InitSysMailParam(tagMailParam *pParam,tagMailParam *pPamramList);		//初始化系统邮件参数
	bool		AddSysMail(long lMailid, CMail *pMail);									//添加系统邮件
	void		ClearSysMail();															//清除系统邮件
	void		SendSysMailMsgToReceiver(long lSysID);									//发送系统邮件到WS
	void		SendSysMail(CMail *pMail,CPlayer *pPlayer,long lSysID,long lEnd);		//发送系统邮件
	bool		CheckMailToEverybody(CMail *pMail,CPlayer *pPlayer);					//检查群发类邮件条件
	bool		CheckMailCountryToEverybody(CMail *pMail,CPlayer *pPlayer);				//检查群发类邮件国家条件
	void		SendSysMailMsg(CMail *pMail,CPlayer *pPlayer);							//发送系统邮件消息
	void		SendSysMailToReciver(CPlayer *pPlayer);									//在玩家第一次收信时将没有收到的系统邮件接收
	void		SendSysMailMsgToReciver(CPlayer *pPlayer);								//在玩家进入游戏时如果有系统邮件发送的消息提示	
	void		SendSysMailByScript(CMail *pMail);										//通过脚本发送系统邮件
	CMail		*GetSysMail(long  lMailID);												//通过ID得到系统邮件
	void		ReleaseMail(list<CGUID> &lMailGuid);									//释放邮件
private:

	map<CGUID,CMail*>	m_mapMail;  
	map<long,CMail*>	m_mapSysMail;													//系统邮件
	//map<CGUID,CMail*>   m_mapExSysMail;													//系统附件邮件
};