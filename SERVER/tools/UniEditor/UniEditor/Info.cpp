#include "StdAfx.h"
#include "Info.h"

CInfo::CInfo(void)
{
	m_bSwitch		= true;				// 开关
	m_uNo			= 1;				// 编号
	m_uQuantity		= 1;				// 数量
	m_iDirection	= -1;				// 方向
}

CInfo::CInfo(CInfo &rInfo)
{
	m_bSwitch		= rInfo.GetSwitch();			// 开关
	m_uNo			= rInfo.GetNo();				// 编号
	m_rcCoor		= rInfo.GetCoordinate();		// 坐标
	m_uQuantity		= rInfo.GetQuantity();			// 数量
	m_iDirection	= rInfo.GetDirection();			// 方向
}

CInfo::~CInfo(void)
{
}

bool CInfo::OffsetCoor(const CPoint point, long lWidth, long lHeight)
{
	m_rcCoor.left	+= point.x;
	m_rcCoor.right	+= point.x;
	m_rcCoor.top	+= point.y;
	m_rcCoor.bottom	+= point.y;

	if(m_rcCoor.right>lWidth)
	{
		m_rcCoor.left = lWidth - m_rcCoor.Width();
		m_rcCoor.right = lWidth;
	}

	if(m_rcCoor.left<1)
	{
		m_rcCoor.right = 1 + m_rcCoor.Width();
		m_rcCoor.left = 1;
	}

	if(m_rcCoor.bottom>lHeight)
	{
		m_rcCoor.top = lHeight - m_rcCoor.Height();
		m_rcCoor.bottom = lHeight;
	}

	if(m_rcCoor.top<1)
	{
		m_rcCoor.bottom = 1 + m_rcCoor.Height();
		m_rcCoor.top = 1;
	}

	return true;
}

bool CInfo::SetCoor(const CRect &rect, long lWidth, long lHeight)
{
	if(m_rcCoor != rect && RectIsValid(rect, lWidth, lHeight))
	{
		m_rcCoor = rect;
		return true;
	}
	return false;
}

// 矩形合法
bool CInfo::RectIsValid(const CRect &rect, long lWidth, long lHeight)
{
	if(rect.Width()<0 || rect.Height()<0 || rect.left<1 || rect.right>lWidth || rect.top<1 || rect.bottom>lHeight)
	{
		return false;
	}
	
	return true;
}
