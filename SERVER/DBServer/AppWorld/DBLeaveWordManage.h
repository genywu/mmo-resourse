

/*
*	file:		DBLeaveWordManage.h
*	Brief:		留言系统数据库操作管理
*	detail:		
*				
*				
*	Authtor:	张科智
*	Datae:		2008-03-07
*/

#pragma once

#include "..\..\..\dbaccess\worlddb\RsLeaveWord.h"
#include "..\public\TimerQueue.h"

class DBLeaveWordManage
{
public:
	DBLeaveWordManage(void);
	~DBLeaveWordManage(void);

	static	DBLeaveWordManage&	GetInstance(void);

public:
	//!				响应配置消息
	void			OnSetupMessage			(CMessage *pMsg);
	//!				响应留言消息
	void			OnSaveMessage			(CMessage *pMsg);
	//!				响应取出留言消息
	void			OnPopMessage			(CMessage *pMsg);
	//!				响应清理过期留言
	void			OnClearOuttimeMessage	(CMessage *pMsg);

private:



private:
	//!										留言最长保留时间
	LONG									m_lMaxSaveTime;
	

};