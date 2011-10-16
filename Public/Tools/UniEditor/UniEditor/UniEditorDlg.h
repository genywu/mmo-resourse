// UniEditorDlg.h : 头文件
//
#include "RegionTree.h"
#include "DlgMonster.h"
#include "DlgMoninfo.h"
//#include "DlgNPC.h"
#include "DlgFileList.h"
#include "TextLog.h"
#include "ListLog.h"
#include "DlgCollection.h"
#include "DlgCollInfo.h"
#include "DlgSignPos.h"
#include "DlgRoadsInfo.h"
#include "NumList.h"
#include <set>
#include <string>

#pragma once
using namespace std;

#define		IDT_REGION					10023
#define		MAX_NUM						10000

#define		LBS_NOTDOWN					0
#define		LBS_DOWN_SELECT_RECT		1
#define		LBS_DOWN_LEFT_TOP			5
#define		LBS_DOWN_RIGHT_BOTTOM		8
#define		LBS_DOWN_NPC				9

typedef		set<string>					SETUPDATE;
typedef		SETUPDATE::iterator			SETUPDATEITR;

const RECT rcMainDlg		= {0, 0, 1400, 900};	// 主框架窗口
const RECT rcTreeRegion		= {0, 0, 200, 800};		// 左边树形列表
const SIZE ptTreeRegion		= {10, 10};				// 树形列表起点
const RECT rcMap			= {0, 0, 700, 700};		// 主框架窗口
const SIZE ptMap			= {215, 10};			// 地图场景起点
const RECT rcDlg			= {0, 0, 860, 285};		// 右边编辑对话框
const SIZE ptDlg			= {215, 740};			// 编辑对话框起点
const RECT rcLog			= {0, 0, 200, 512};		// 主框架窗口
const SIZE ptLog			= {700, 10};			// 编辑对话框起点

// CUniEditorDlg 对话框
class CUniEditorDlg : public CDialog
{
// 构造
public:
	CUniEditorDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CUniEditorDlg();						// 析构函数

// 对话框数据
	enum { IDD = IDD_UNIEDITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 操作
public:
	HACCEL m_hAccelTable;
	BOOL DrawMap(void);									// 绘制地图区域
	BOOL DrawRegion(void);								// 绘制新场景
	BOOL DrawRegion(float fx);								// 缩放
	BOOL DrawMonster(CDC &dc, const unsigned uNo=-1);	// 绘制Monster信息
	BOOL DrawCollection(CDC &dc, const unsigned uNo=-1);		// 绘制NPC信息
	BOOL DrawSignPos(CDC &dc,const unsigned uNo=-1);			//绘制路标点信息
	BOOL DrawRoads(CDC &dc,const unsigned uNo2=-1,const unsigned uNo1=-1, unsigned uNo=-1);			//绘制路径


	void DrawMonsterVector(CDC &dc);
	void DrawCollectionVector(CDC &dc);
	inline const long GetRegionWidht()		{return m_lRegionWidth;}
	inline const long GetRegionHeight()		{return m_lRegionHeight;}
	inline SETUPDATE &GetUpdateList()		{return m_setUpdate;}
	inline CRegionTree *GetTreeCtrl()const	{return m_ptreeRegion;}
	inline VECMONSTER &GetVecMonster()		{return m_vecMonster;}
	inline VECMONINFO &GetVecMonInfo()		{return m_vecMoninfo;}
//	inline VECNPC &GetVecNPC()				{return m_vecNPC;}
	inline MAPTREE &GetMapTree()			{return m_mapTree;}
	inline Cell* GetCell()const				{return m_pCell;}
	static CTextLog* GetLog()				{return m_pTextLog;}

	//unsigned *GetMonster(const CPoint point);							// 获取point处所有Monster编号信息
	//unsigned *GetNPC(const CPoint point);						 		// 获取point处所有NPC编号信息
	unsigned GetMonster(const CPoint point);							// 获取point处最上层Monster编号信息
	unsigned GetCollection(const CPoint point);							// 获取point处最上层Collection编号信息
	unsigned GetSignPos(const CPoint point);							// 获取point处最上层路标点信息
	unsigned GetRoadsPoint(const CPoint point);
	long GetBlockCellNumber(const CRect &rect);						// 计算区域内阻挡格子数
	long GetUnBlockCellNumber(const CRect &rect);						// 计算区域内无阻挡格子数
	CString MonsterDensity();											// 计算框选区域内怪物密度
	CString GetEditInfo(const unsigned uNo, const CPoint point, const unsigned uType = ITEM_OFFSET_MONSTER);

	// 计算矩形区域内编辑信息
	bool UpdateFileList(int  nflag);									// 更新文件列表
	bool Backup(const CString &szFileName);								// 备份文件
	bool SaveFile(const CString &szFileName);							// 保存文件
	CString UpdateLog();												// 更新日志
	void SearchTreeNode(CPoint point,UINT ntpye);									//定位树节点
	void InvertLine(CDC *pDC,CPoint ptFrom,CPoint ptTo);								//橡皮筋线
	void ZoomInterpolation(BYTE* pixel, BYTE*& tempPixel, int width, int height, 
					   UINT& outWidth, UINT& outHeight, double fx, double fy);			//缩放图像
	void DrawJpg(unsigned int unNo);
	string SearchRoadsPoint(CPoint point);
	void CheckScript(int nType,vector<long>*vecData);
	void DrawMonsterFirst(vector<long>vecNo);
	void DrawCollectionFirst(vector<long>vecNo);
// 公共成员
public:
	CDlgMonster		m_dlgMonster;				// MONSTER批次信息编辑对话框
	CDlgMoninfo		m_dlgMoninfo;				// MONSTER详细信息编辑对话框
//	CDlgNPC			m_dlgNPC;					// NPC详细信息编辑对话框
	CDlgCollection	m_dlgCollection;			// 采集物批次信息对话框
	CDlgCollInfo	m_dlgCollInfo;				// 采集物详细信息对话框
	CDlgSignPos		m_dlgSignPos;				//路标点
	CDlgRoadsInfo	m_dlgRoadsPoint;			//路径
	static BOOL		m_bUpdateMonster;			// 编辑更新标记
	static BOOL		m_bUpdateMonInfo;			// 编辑更新标记
	
	//static BOOL		m_bUpdateNPC;				// 编辑更新标记
	static BOOL		m_bUpdateRoads;
	static BOOL		m_bUpdateCollection;		
	static BOOL		m_bUpdateCollInfo;
	static BOOL		m_bUpdateSignPos;
	static BOOL		m_bSelChange;				// 选择结点变化
	static CString	m_szModify;					// 修改内容
	static int CALLBACK Compositor(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort);
protected:
	static CTextLog	*m_pTextLog;				// 操作日志

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

// 成员
	BLENDFUNCTION	m_bf;						//画透明图	
	int		m_nTimes;					//当前放大倍数
	CScrollBar		m_HScroll;
	CScrollBar		m_VScroll;
	BOOL			m_bNewRec;					//绘制标识
	CPoint			m_ptFrom;					//绘制矩形起点
	CPoint			m_ptTo;						//绘制矩形终点
	CRegionTree		*m_ptreeRegion;				// 树形控件
	CInfo			*m_pInfo;					// 当前编辑内容
	//HTREEITEM		m_hItem;					// 当前选择结点
	//HTREEITEM		m_hParentItem;				// 当前选择结点之父结点
	MAPTREE			m_mapTree;					// 结点存储映射
	VECMONSTER		m_vecMonster;				// MONSTER向量
	VECMONINFO		m_vecMoninfo;				// MONSTER具体信息向量

	//VECNPC			m_vecNPC;					// NPC向量
	VECCOLLECTION	m_vecCollection;			//采集物向量
	VECCOLLINFO		m_vecCollInfo;				//采集物具体信息
	VECSIGNPOS		m_vecSignPos;				//路标点
	MAPROADS		m_mapRoads;					//路径
	VECROADPOINT	m_vecRoadsPoint;			//路径点
	long			m_lRegionHeight;			// 场景高度
	long			m_lRegionWidth;				// 场景宽度
	Cell			*m_pCell;					// 获取场景格子信息
	DWORD_PTR		m_uRegionPrev;				// 上次选择场景编号
	DWORD_PTR		m_uRegionCurr;				// 当前选择场景编号
	UINT			m_uLayer;					// 选择结点层次
	UINT			m_uOffset;					// 选择结点偏移
	CToolTipCtrl	m_toolTip;					// TOOLTIP
	CBitmap			m_Bitmap;					// 场景位图
	CBitmap			m_BitJpg;					//图片位图			
	CDC				m_dcMemory;					// 内存DC
	CDC				m_dcJpg;					//图片内存DC
	CDC				m_dcContent;					//怪物和采集物内存DC
	DWORD_PTR		m_uParentOffset;			// 父结点偏移
	float			m_fMonsterRate;				// 区域内Monster比率
	unsigned		m_uLBStyle;					// 鼠标左键按下
	BOOL			m_bRBDown;					// 鼠标右键按下
	BOOL			m_bMouseDown;				// 鼠标按下(左右均可)
	bool			m_bModify;					// 鼠标修改
	bool			m_bCreateNew;				// 鼠标绘制新的MONSTER/NPC
	CPoint			m_ptOrigin;					// 鼠标按下时的坐标
	SETUPDATE		m_setUpdate;				// 更新列表
	CRect			m_rcMap;					// 右边地图场景
	CRect			m_rcMonsterDensity;			// 测定怪物密度的矩形
	CButton			m_btnSave;					// 保存按钮
	CListLog		m_listLog;					// 编辑日志列表
	//CListCtrl		m_listNum;					//统计数目列表
	bool			m_bShift;					//Shift是否按下
	
	bool			m_bJpg;						//是否显示JPG图片
	bool			m_bLButton;					//鼠标左键按下
	bool			m_bRButton;					//鼠标右键按下
	CPoint			m_pt;
	//CRect			m_rcMoveRect;
	CPoint			m_ptStart;
	bool			m_bSpace;
	bool			m_bSignPos;					//是否选中路标节点
	bool			m_bMonster;
	bool			m_bCollection;
	bool			m_bRegion;
	bool			m_bRoads;	
	vector<long>	m_vecMonsterNo;				//需要额外显示的MONSTER
	vector<long>	m_vecCollectionNo;			//需要额外显示的Collection
	// 颜色分配表
	enum { CL_NOBLOCK, CL_FLY, CL_NOFLY, CL_SWITCH, CL_ACTIVE_MONSTER, CL_INACTIVE_MONSTER, CL_ACTIVE_NPC, CL_INACTIVE_NPC, CL_MONSTER_DENSITY ,REGION_BLOCK };
	COLORREF		m_clDraw[16];				// 绘制地图所用颜色

public:
	static bool			m_bCtrl;					//Control是否按下
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ItemChanged(DWORD_PTR uNo);
	void HitCurrItem(int n,string str1,string str2,string str3);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnAddItem();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	bool ShowJpgGif(CDC* pDC,CString strPath, int x, int y);
public:
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnVkadd();
	afx_msg void OnZoomout();
	afx_msg void OnZoomin();
	afx_msg void OnHScroll(UINT SBCode,UINT nPos,CScrollBar *pScrollBar);
	afx_msg void OnVScroll(UINT SBCode,UINT nPos,CScrollBar *pScrollBar);
	afx_msg void OnBnClickedRefresh();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

private:
	void RelativeCoor(CPoint* ptOri);
public:
	void CUniEditorDlg::ZoomCoor(CPoint* ptOri, bool bToPhysical = true);
	CNumList m_listNum;
	CNumList m_listColl;
	CListCtrl m_ScriptList;
	afx_msg void OnHdnItemclickNumlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickNumlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickNumlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListcoll(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListcoll(NMHDR *pNMHDR, LRESULT *pResult);

	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
};
