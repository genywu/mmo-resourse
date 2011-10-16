#include "StdAfx.h"
#include "NPC.h"
#include "ListLog.h"

CNPC::CNPC(void)
{
	m_rcCoor		= new CRect(1, 1, 1, 1);		// 坐标
}

CNPC::~CNPC(void)
{
}

void CNPC::NPC2String(CString &str)
{
	str.Format(_T("%-22s (%-3u, %-3u; %-3u, %-3u)\t图片:%-4u 数量:%-3u 方向:%-3d %s\n"),
		m_strName.c_str(), m_rcCoor.left, m_rcCoor.top, m_rcCoor.right, m_rcCoor.bottom,
		m_uPicID, m_uQuantity, m_iDirection, m_strScript.c_str());
}

ifstream & operator>>(ifstream &rstream, CNPC &rNPC)
{
	rstream >> rNPC.m_rcCoor.left >> rNPC.m_rcCoor.top
		>> rNPC.m_rcCoor.right >> rNPC.m_rcCoor.bottom >> rNPC.m_strName
		>> rNPC.m_uPicID >> rNPC.m_uQuantity >> rNPC.m_iDirection
		>> rNPC.m_strScript;

	return rstream;
}

ofstream & operator<<(ofstream &rstream, const CNPC &rNPC)
{
	if (rNPC.m_bSwitch)
	{
		rstream << strSharp;
	} 
	else
	{
		rstream << strSep;
	}

	if (0 == rNPC.m_uNo || 1 == rNPC.m_uNo)
	{
		rstream << rNPC.m_uNo;
	}

	rstream << strSep << rNPC.m_rcCoor.left << strSep 
		<< rNPC.m_rcCoor.top << strSep << rNPC.m_rcCoor.right << strSep 
		<< rNPC.m_rcCoor.bottom << strSep << rNPC.m_strName << strSep 
		<< rNPC.m_uPicID << strSep << rNPC.m_uQuantity << strSep 
		<< rNPC.m_iDirection << strSep << rNPC.m_strScript << endl;

	return rstream;
}
