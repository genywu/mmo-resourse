//=============================================================================
/**
 *  file: RegionInfo.h
 *
 *  Brief:显示场景信息
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================


#pragma once
#include "afxwin.h"
#include "resource.h"
//#include "../App/MoveShape.h"
//#include "../App/GameClient.h"

class CRegion ;
class CClientRegion;
class CPlayer;
class CMoveShape;
class CGameClient;
// CRegionInfo 对话框

class CRegionInfo : public CDialog
{
	DECLARE_DYNAMIC(CRegionInfo)

public:
	CRegionInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRegionInfo();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_REGION_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	//当前显示的场景
	CClientRegion*	m_pDisplayRegion;
	long m_lDrawingRegionID;					//正在绘制的场景id
	long m_lClientID;							//当前显示客户端的ID
	CRect			m_rcClient;
	CRect			m_rcRegion;
	//画在窗口中心的场景位置
	CPoint			m_ptCenter;
	//在构造函数中建立 几个DC对象和bitmap对象
	CDC	*m_DisplayMemDC;		//显示缓存
	// 场景位图
	CBitmap	*m_Bitmap;

public:
	BOOL ReadRgnData();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	void DrawCell(CDC &dcMem);
	void DrawShape(CDC &dcMemory);
	void SetDisplayRegion(CGameClient* pClient);
	void SetRegioBitmapData();
	// 当前坐标
	float m_PlayerX;			//mainPlayer 坐标
	float m_PlayerY;

	float m_RegionID;			//场景ID

	void SetDisplayCoordinate(long x,long y);
	void SetPlayerRegion(CPlayer* pPlayer);
	CPlayer* mMainPlayer;
};
