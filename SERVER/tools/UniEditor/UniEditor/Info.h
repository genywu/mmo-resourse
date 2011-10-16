#pragma once
#include "UniEditor.h"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class CInfo
{
public:
	CInfo(void);
	CInfo(CInfo &rInfo);
public:
	~CInfo(void);

// 数据读写
public:
	const inline bool GetSwitch()const						{return m_bSwitch;}
	const inline unsigned GetNo()const						{return m_uNo;}
	const inline CRect GetCoordinate()const					{return &m_rcCoor;}
	const inline unsigned GetQuantity()const				{return m_uQuantity;}
	const inline int GetDirection()const					{return m_iDirection;}

	inline void SetSwitch(const bool bSwitch = true)		{m_bSwitch = bSwitch;}
	inline void SetNo(const unsigned uNo)					{m_uNo = uNo;}
	inline void SetCoorLeft(const int val)					{m_rcCoor.left = val;}
	inline void SetCoorTop(const int val)					{m_rcCoor.top = val;}
	inline void SetCoorRight(const int val)					{m_rcCoor.right = val;}
	inline void SetCoorBottom(const int val)				{m_rcCoor.bottom = val;}
	inline void SetQuantity(const unsigned uQuantity)		{m_uQuantity = uQuantity;}
	inline void SetDirection(const int iDirection)			{m_iDirection = iDirection;}
	bool OffsetCoor(const CPoint point, long lWidth, long lHeight);
	bool SetCoor(const CRect &rect, long lWidth, long lHeight);
	bool RectIsValid(const CRect &rect, long lWidth, long lHeight);					// 矩形合法


	bool		m_bSwitch;			// 开关 '#'
	unsigned	m_uNo;				// 编号
	CRect		m_rcCoor;			// 坐标
	unsigned	m_uQuantity;		// 数量
	int			m_iDirection;		// 方向
};
