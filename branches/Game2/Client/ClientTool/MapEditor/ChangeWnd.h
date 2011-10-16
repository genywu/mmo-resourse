#pragma once
#include "..\..\engine\ui.h"

//切换点编辑
//列表
enum eCHANGE_POINT_TYPE
{
	CPT_NORMAL,		// 直接切换
	CPT_RANDOM,		// 随机切换
	CPT_SCRIPT,		// 脚本触发
};

// 切换点
struct tagDoor
{
	char szName[64];

	struct tagSwitch
	{
		long			lState;							// 切换状态
		long			lRegionID;						// 出口点ID号
		long			lCoordX;						// 出口点坐标
		long			lCoordY;
		long			lDir;							// 出口点方向
	}tSwitch;
};

class WndDoorEdit :
	public ui::Wnd
{
	

	std::vector<tagDoor *> m_vDoorList;

	ui::ListBox * m_pListBox;
	ui::ListBox * m_pListDetail;

	ui::Button * m_pBtnAdd;
	ui::Button * m_pBtnDel;
	ui::Button * m_pBtnModify;


public:
	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent);
	void Destroy();

    virtual void Update();

	WndDoorEdit(void);
	~WndDoorEdit(void);

	void InsertDoor(tagDoor *pDoor);
	int GetCurDoorIndex();
	int GetDoorCount() { return (int)m_vDoorList.size();}
	tagDoor * GetDoor(DWORD dwIndex) ;

	void ClearAllDoor();

};
