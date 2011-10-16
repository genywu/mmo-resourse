#pragma once
#include "bounsoper.h"
#include "../../../dbaccess/BounsDB/RsBouns.h"

class CBounsRollBackOper :
	public CBounsOper
{
private:
	CGUID	m_guidBouns;
public:
	CBounsRollBackOper(eOperType eOt);
	~CBounsRollBackOper(void);

	void SetBounsGUID(const CGUID &guid)	{ m_guidBouns = guid; }

	virtual BOOL DoOperator();
};
