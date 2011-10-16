//Session.h/////////////////////////////////////////////////////////////////////
//对象类:会话对象抽象基类
//Author:安海川
//Create Time:2008.11.03
#ifndef _INC_CSESSION_41DCC469033C_INCLUDED
#define _INC_CSESSION_41DCC469033C_INCLUDED

#include "../../../../Public/ServerPublic/db/DBDefine.h"

//##ModelId=41DCC469033C
class CSession 
{
public:
	
	struct tagSessionStep 
	{
		tagSessionStep()
		{
			lStep = -1;
		}
		~tagSessionStep()
		{

		}

		long				lStep;
	};

	typedef vector<tagSessionStep> SessionStepVec;
	typedef SessionStepVec::iterator SessionStepVecItr;

public:
	virtual const CGUID&	GetExID()					{return m_GUID;};
	virtual void			SetExID(const CGUID& guid)	{m_GUID = guid;};


	SessionStepVecItr&		GetCurSessionStepItr(void)		{ return m_CurSessionStepItr; }
	void					NextCurSessionStepItr(void)		{ m_CurSessionStepItr++; }
	void					BackCurSessionStepItr(void)		{ m_CurSessionStepItr--; }
	bool					IsEndSessionStepItr(void)		{ return (m_CurSessionStepItr == m_vecSessionSteps.end()); }
	tagSessionStep&			AddOneSessionStep(long step);
	tagSessionStep&			GetCurSessionStep(void);


	//##ModelId=41DE28EC0242
	virtual BOOL IsSessionEnded() = 0;

	//##ModelId=41DE197F0290
	virtual VOID Release() = 0;

	//##ModelId=41DDF8FA00EA
	virtual BOOL IsSessionAvailable() = 0;

	virtual BOOL IsSessionShouldBeenRemoved() = 0;

	//##ModelId=41DCD1EF008C
	CSession(DWORD dwLifeTime);

	//##ModelId=41DCD1F60157
	CSession(const CGUID& guid, DWORD dwLifeTime);

	//##ModelId=41DCD2070109
	virtual ~CSession();

	//##ModelId=41DCD273036B
	virtual VOID AI() = 0;

	//正常开始
	//##ModelId=41DCD60D0119
	virtual BOOL Start( BOOL bFlag = FALSE ) = 0;

	//正常结束
	//##ModelId=41DCD61A00DA
	virtual BOOL End( BOOL bFlag = FALSE ) = 0;

	//异常结束
	//##ModelId=41DCD62A0186
	virtual BOOL Abort( BOOL bFlag = FALSE ) = 0;

	virtual BOOL OnSessionStarted( BOOL bFlag ) = 0;

	virtual BOOL OnSessionEnded( BOOL bFlag ) = 0;

	virtual BOOL OnSessionAborted( BOOL bFlag ) = 0;

	
	//##ModelId=421158C70330
	VOID SendNotification(CHAR* strMsg, DWORD dwColor = 0xffffffff);

	void SetOwnerName(const char* szName) 
	{ 
		if(szName)
		{
			memset(m_szOwnerName, 0, sizeof(m_szOwnerName));
			strcpy(m_szOwnerName, szName); 
		}
	}
	const char* GetOwnerName(void){ return m_szOwnerName; }

	SESSION_TYPE GetSessionType(void) { return m_dwSessionType; }
	void SetSessionType(SESSION_TYPE type) { m_dwSessionType = type; }
protected:
	SessionStepVecItr m_CurSessionStepItr;
	char	m_szOwnerName[64];
	static tagSessionStep g_NullSessionStep;
	SessionStepVec m_vecSessionSteps;

	SESSION_TYPE m_dwSessionType;

	CGUID m_GUID;
	//##ModelId=41DE31E700BB
	BOOL m_bSessionEnded;

	//##ModelId=41DE2D520232
	BOOL m_bSessionStarted;

	//##ModelId=41DE2D7401D4
	BOOL m_bSessionAborted;

	//##ModelId=41DCD08E000F
	DWORD m_dwStartingTimeStamp;

	//##ModelId=41DCD0A7000F
	DWORD m_dwLifeTime;

	BOOL m_bRemoveFlag;
};

#endif /* _INC_CSESSION_41DCC469033C_INCLUDED */
