#pragma once

class Cmd
{
	Cmd *m_pPrev;
	Cmd *m_pNext;

	BOOL m_bExecuted;

public:
	Cmd(void);
	virtual ~Cmd(void);

	Cmd * GetPrev() { return m_pPrev;}
	Cmd * GetNext() { return m_pNext;}

	void  SetPrev(Cmd *pPrev) { m_pPrev = pPrev;}
	void  SetNext(Cmd *pNext) { m_pNext = pNext;}

	virtual bool Execute(void);
	virtual void UnExecute(void);

	BOOL  GetExecuted() const { return m_bExecuted;}

	virtual void Dump(void);
};
