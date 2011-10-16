#pragma once
// 树形控件的头文件
#include "afxcmn.h"
#include "Region.h"
#include "RegionList.h"
#include "ListLog.h"

#include <fstream>
#include <string>
#include <list>
#include <map>
#include <vector>

#define		ITEM_ROOT				0
#define		ITEM_REGION_UNEXISTED	1

using namespace std;

typedef map<unsigned, CRegion *>				MAPTREE;
typedef map<unsigned, CRegion *>::iterator		ITRTREE;
typedef list<CRegionList *>						LISTREGION;
typedef list<CRegionList *>::iterator			ITRREGION;
struct tagMonsterName
{
	string strOrigName;
	string strName;
};
struct tagCollectionName
{
	string strOrigName;
	string strName;
};
typedef vector<tagMonsterName *>	VECMONSTERNAME;
typedef VECMONSTERNAME::iterator	VECMONSTERNAMEITR;
typedef vector<tagCollectionName *>	VECCOLLECTIONNAME;
typedef VECCOLLECTIONNAME::iterator	VECCLOOECTIONNAMEITR;

const string strFileRegion	= _T("Setup\\regionlist.ini");	// 场景列表文件

enum { ITEM_LAYER_ROOT = 0, ITEM_LAYER_REGION, ITEM_LAYER_TYPE, ITEM_LAYER_EDIT,
ITEM_LAYER_MONSTER,ITEM_LAYER_COLLECTION,ITEM_LAYER_SIGNPOS};
enum { ITEM_OFFSET_MONSTER = 0, ITEM_OFFSET_NPC,ITEM_OFFSET_SIGNPOS,ITEM_OFFSET_ROADS};
enum { ERROR_NONE = 0, ERROR_RECT};

class CRegionTree :
	public CTreeCtrl
{
public:
	CRegionTree(void);
public:
	~CRegionTree(void);

// 初始化
public:
	BOOL InitialRegionTree();

// 消息响应函数
public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);

// 操作函数
public:
	inline MAPTREE &GetMapTree()					{return m_mapTree;}
	inline const DWORD_PTR GetRegionNo()const		{return m_uNo;}
	inline const HTREEITEM GetItem()const			{return m_hItem;}
	inline LISTREGION &GetRegionList()				{return m_listRegionList;}
	inline void SetErrorType(unsigned uErrorType)	{m_uErrorType = uErrorType;}

	const UINT GetLayer(HTREEITEM hItem);
	const UINT GetOffset(HTREEITEM hItem);
	BOOL ReadData(HTREEITEM hItem);															// 读取数据
	BOOL AddItemMonster(HTREEITEM hItem, CRegion *pRegion, CMonster *pMonster = NULL);		// 增加MONSTER结点
	BOOL AddItemMonInfo(HTREEITEM hItem, CRegion *pRegion, CMonsterInfo *pMonInfo = NULL);	// 增加MONSTERINFO结点
	

	BOOL AddItemCollection(HTREEITEM hItem, CRegion *pRegion, CCollection *pCollection= NULL);
	BOOL AddItemCollInfo(HTREEITEM hItem, CRegion *pRegion, CCollectionInfo *pCollInfo = NULL);	// 增加CollInfo结点


	BOOL AddItemNPC(HTREEITEM hItem, CRegion *pRegion, CNPC *pNPC = NULL);					// 增加NPC结点
	BOOL DelItemMonster(HTREEITEM hItem, unsigned uOffset, CRegion *pRegion);				// 删除MONSTER结点
	BOOL DelItemMonInfo(HTREEITEM hItem, unsigned uOffset, CRegion *pRegion);				// 删除MONSTERINFO结点
	
	BOOL DelItemRoadsPoint(HTREEITEM hItem,int m,int n, CRegion *pRegion);
	BOOL DelItemRoads(HTREEITEM hItem, unsigned uOffset, CRegion *pRegion);

	BOOL DelItemCollInfo(HTREEITEM hItem, unsigned uOffset, CRegion *pRegion);				//删除 Collectioninfo节点
	BOOL DelItemCollection(HTREEITEM hItem, unsigned uOffset, CRegion *pRegion);					// 删除Collection结点
	BOOL AddNode(HTREEITEM hItem, unsigned uLayer, unsigned uOffset, CRegion *pRegion);		// 增加结点
	BOOL DelNode(HTREEITEM hItem, unsigned uLayer, unsigned uOffset, CRegion *pRegion);		// 删除结点
	BOOL AddItemMonster(HTREEITEM hItem, CRegion *pRegion, CRect *rect,CMonster *pMonster=NULL);		//快捷增加节点
	BOOL AddItemCollection(HTREEITEM hItem, CRegion *pRegion, CRect *rect,CCollection *pCollection=NULL);		//快捷增加collention节点
	BOOL AddItemSignPos(HTREEITEM hItem, CRegion *pRegion, CRect *rect,CSignPos *pSignPos=NULL);		//快捷增加路标点节点
	BOOL AddItemSignPos(HTREEITEM hItem, CRegion *pRegion,CSignPos *pSignPos=NULL);		//快捷增加路标点节点
	BOOL DelItemSignPos(HTREEITEM hItem, unsigned uOffset, CRegion *pRegion);			//删除路标点
	BOOL AddItemRoadsPoint(HTREEITEM hItem, CRegion *pRegion, int n,CRect *rect,CRoads *pRoads=NULL);		//快捷增加路径点
	BOOL AddItemRoads(HTREEITEM hItem, CRegion *pRegion,CRoads *pRoads = NULL);	//快捷增加路径
	void DlgADD(CRect *rect);

//////////////////////////////////////////////////////////////////////
	void			SetItemFont(HTREEITEM hItem, LOGFONT&);
	void			SetItemBold(HTREEITEM hItem, BOOL);
	void			SetItemColor(HTREEITEM hItem, COLORREF);
	BOOL			GetItemFont(HTREEITEM hItem, LOGFONT *);
	BOOL			GetItemBold(HTREEITEM hItem);
	COLORREF		GetItemColor(HTREEITEM hItem);
	int				m_ncurrRoads;		//当前选中的路径

// 成员变量
protected:
	struct Color_Font {
		COLORREF color;
		LOGFONT  logfont;
	};

	CMap <void*, void*, Color_Font, Color_Font&> m_mapColorFont;

	BOOL		m_bExist;			// Exist or not
	UINT		m_uLayer;			// 当前选择的结点所在层次
	UINT		m_uOffset;			// 当前选择的结点偏移
	UINT		m_uRCLayer;			// 右键选择结点所在层次
	UINT		m_uRCOffset;		// 右键选择结点偏移
	HTREEITEM	m_hItem;			// 当前选择的结点
	HTREEITEM	m_hRCItem;			// 右键选择的结点
	DWORD_PTR	m_uNo;				// 当前选择的场景编号
	DWORD_PTR	m_uRCNo;			// 右键选择的场景编号
	MAPTREE		m_mapTree;			// 树形结点列表映射
	CRegion		*m_pRCRegion;		// 右键选择的场景
	unsigned	m_uErrorType;		// 结点信息类型
	LISTREGION	m_listRegionList;	// 可编辑场景列表
	VECMONSTERNAME	m_vecMonsterName;	
	VECCOLLECTIONNAME	m_vecCollectionName;
public:
	afx_msg void OnPopMenuAdd();
public:
	afx_msg void OnPopMenuDel();
public:
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnPaint();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
