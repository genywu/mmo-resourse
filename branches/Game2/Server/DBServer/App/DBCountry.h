#ifndef DB_COUNTRY_H
#define DB_COUNTRY_H
#include "BaseDBEntity.h"
#include <list>
#include "../public/DBDefine.h"

using namespace std;



class CDBCountry : public CBaseDBEntity
{
public:
	CDBCountry();
	CDBCountry(const CGUID& id);
	~CDBCountry();

	BYTE GetID(void) { return m_ID; }
	void SetID(BYTE id) { m_ID = id; }

	// interfaces
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos);
	

	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	long	GetPower(void)		{ return m_lPower; }
	long	GetTechLevel(void)	{ return m_lTechLevel;}
	const CGUID&	GetKing(void)		{ return m_lKing;}

	void	SetPower	(long value)	{ m_lPower = value; }
	void	SetTechLevel(long value)	{ m_lTechLevel = value;}
	void	SetKing		(const CGUID& value)	{ m_lKing = value;}

	void	AddEntityDataToMsg(vector<BYTE>& pBA);

	virtual CDBCountry& operator=(CDBCountry& right)
	{
		if(this != &right)
		{
			m_lPower = right.m_lPower;
			m_lTechLevel = right.m_lTechLevel;
			m_lKing = right.m_lKing;
			
			m_ID = right.m_ID;
		}

		return *this;
	}
private:
	//////////////////////////////////////////////////////////////////////////
	long	m_lPower;
	long	m_lTechLevel;
	CGUID	m_lKing;

	BYTE    m_ID;
};

#endif//DB_COUNTRY_H