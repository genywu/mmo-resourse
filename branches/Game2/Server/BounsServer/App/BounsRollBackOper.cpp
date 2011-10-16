#include "StdAfx.h"
#include "BounsRollBackOper.h"

CBounsRollBackOper::CBounsRollBackOper(eOperType eOt)
{
	SetOperType(eOt);
}

CBounsRollBackOper::~CBounsRollBackOper(void)
{
}

BOOL CBounsRollBackOper::DoOperator()
{
    HRESULT hRt = GetGame()->GetRsBouns()->BounsRollBack(m_cnConnPtr, m_guidBouns);
    if(hRt != S_OK)
        return FALSE;
	return TRUE;
}
