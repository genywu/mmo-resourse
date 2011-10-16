#include "StdAfx.h"
#include ".\cdkeygoods.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCdkeyGoods::CCdkeyGoods(void)
{
	m_cDepot.SetContainerVolume( 8, 12 );
	m_strDepotPassword="";
}

CCdkeyGoods::~CCdkeyGoods(void)
{
	m_cBank.Release();
	m_cDepot.Release();
}
