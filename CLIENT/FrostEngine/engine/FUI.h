/*

            ○○○○○○
			 ○                                      ○
			○○○    ○○ ○     ○○    ○○○  ○○○
		   ○  ○      ○○    ○    ○  ○        ○
		  ○          ○      ○    ○     ○     ○
	  ○○○      ○○○       ○○    ○○○    ○○

			"FROST" 3D GRAPHICS RENDERING ENGINE.

					 WRITTEN BY JIATAO.
--------------------------------------------------------------------
					COPYRIGHT 2002-2003. 

描述: 界面管理模块
作者: 贾涛
建立: 2001 - 2002
修改: 2003/10/31
*/

#pragma once
#include "fsingleton.h"
#include <windows.h>
#include <list>
#include <vector>
#include <string>
#include <d3d9.h>
#include <d3dx9.h>
#include "futility.h"

//消息类型
enum UI_INPUT_MSG_TYPE{
	UIMT_NONE,
	UIMT_KB_KEYDOWN,//DATA IS DIK CODE
	UIMT_KB_KEYUP,  //DATA IS DIK CODE
	UIMT_KB_CHAR,   //DATA IS VK CHAR
	UIMT_MS_BTNDOWN,//DATA IS DOWN BUTTON MASK
	UIMT_MS_BTNUP,  //DATA IS DOWN BUTTON MASK
	UIMT_MS_MOVE ,  //DATA IS DOWN BUTTON MASK
	UIMT_MS_WHEEL,  //DATA IS SCROLL VALUE
};

//___CLASS__(ui::Wnd)___DEFINE___
//风格
#define FWS_DEBUG       0x00000001 //调试
#define FWS_VISIBLE     0x00000002 //可见
#define FWS_DISABLE     0x00000004 //屏蔽消息处理
#define FWS_FILLBKG	    0x00000008 //填充背景
#define FWS_POPUP	    0X00000010 //最前面
#define FWS_DRAG	    0x00000020 //允许拖动
#define FWS_SIZE	    0x00000040 //允许改变尺寸
#define FWS_TOPMOST	    0x00000080 //最前面（保留）
#define FWS_OUTLINE     0x00000100 //画边框
#define FWS_SHADOW      0x00000200 //画阴影
//颜色
#define FWC_BACK		0x802F437A
#define FWC_TEXT		0xffffffff
#define FWC_LOW			0x40000000
#define FWC_MIDDLE		0xc0808080
#define FWC_HIGH		0xc0bbc5ff
#define FWC_RED			0xc0ff0000
#define FWC_GREEN		0xc000ff00
#define FWC_BLUE		0xc00000ff
#define FWC_BLACK		0xc0222222
#define FWC_WHITE		0xc0ffffff
//属性
#define FWA_FRAMESIZE   0x00000003 //边框宽度
//点击
#define FW_POINT_OUTSIDE   0x00000000 //点在窗口外面
#define FW_POINT_INSIDE    0x00000001 //点在窗口里面
#define FW_POINT_INTERSECT 0x00000002 //点于窗口边框相交（在窗口里面)

//__CLASS__(ui::Manager)___ENUM____
enum UI_POPUPWINDOW_STATE { UPS_NONE,UPS_BEGIN,UPS_RUNNING,UPS_END};

class ui
{
public:
	class Manager;

	class Wnd  
	{
		friend Manager;
	public:
		struct _tagWindowInfo
		{
			char szName[256];									//窗口名称
			BOOL bActive;										//激活标志
			BOOL bDrag;											//拖动标志
			WORD wSize;											//缩放标志
			DWORD dwStyle;										//窗口风格
			RECT rcRect;										//窗口矩形

			D3DCOLOR colorBack;									//背景色
			D3DCOLOR colorText;									//文本色

			D3DCOLOR colorLow;									//暗色
			D3DCOLOR colorMiddle;								//间色
			D3DCOLOR colorHigh;									//高亮

		}m_tWindowInfo;

		Wnd *m_pParent;											//父窗口指针
		std::list<Wnd *>m_listChild;							//子窗口指针

	protected:
		void AddChild(Wnd *pWnd);
		void RemoveChild(Wnd *pWnd);
	public:
		void ShowWnd(BOOL bShow);								//显示窗口

		virtual	void MoveWnd(int iOX,int iOY);
		virtual void SizeWnd(int iCX,int iCY);

		BOOL IsActive() { return m_tWindowInfo.bActive;}

		DWORD GetStyle() { return m_tWindowInfo.dwStyle;}	
		void  SetStyle(DWORD dwStyle) { m_tWindowInfo.dwStyle = dwStyle;}

		int  PtInRect(const POINT *pPoint);

		virtual void HitTest(const POINT *pPoint,Wnd **ppWnd);

		virtual void SetWndRect(RECT *pRect);
		virtual void GetWndRect(RECT *pRect);

		Wnd *GetParent() { return m_pParent;}

		
		virtual BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent);
		virtual void Destroy();

		virtual void Update();
		virtual void Render();	

		void UpdateDefault(void);
		void UpdateChild(void);

		void RenderDefault(void);
		void RenderChild(void);

		Wnd();
		virtual ~Wnd();

		void MoveWndTo(int iX, int iY);
		void CenterWndTo(int iX, int iY);

		void SetBkColor(D3DCOLOR dwColor) { m_tWindowInfo.colorBack = dwColor;}
		void SetTextColor(D3DCOLOR dwColor) { m_tWindowInfo.colorBack = dwColor;}	

		void ModifyStyle(DWORD dwAdd , DWORD dwRemove);
		
		const char *GetName() const { return m_tWindowInfo.szName;}
	};

	//---------------------
	//  按钮控件
	//	历史: 2001 - 2002
	////////////////////////////////////////////////////////////////////

	enum BUTTONSTATETYPE {BTN_STATE_NORMAL,BTN_STATE_DOWN,BTN_STATE_OVER};

	class Button :	public Wnd
	{
	protected:
		BUTTONSTATETYPE m_eBtnState;//0 normal 1 down 2 over
		BOOL m_bClicked;

	public:

		BOOL IsClicked();

		virtual void Render();
		virtual void Update();

		Button(void);
		~Button(void);
	};

	class Static :	public Wnd
	{
	public:

		virtual void Render();
		virtual void Update();

		Static(void){};
		~Static(void){};
	};

	/////////////////////////////////////////////////////////////////////
	//	说明:  菜单条控件
	//	历史:  2002 - 2003
	////////////////////////////////////////////////////////////////////
	#define MENUBAR_MAX_ITEM_TEXT 64
	#define MENUBAR_MAX_ITEM	  64
	#define MENUBAR_MAX_MENU_TEXT 64
	#define MENUBAR_MAX_MENU	  256
	#define POPUPMENU_MAX_ITEM    64 //容纳菜单项目个数
	#define POPUPMENU_MAX_TEXT    64

	enum MENUBARSTATETYPE {MENUBAR_STATE_NORMAL,MENUBAR_STATE_POPUP};

	#define MBS_DISABLE       0x0001
	#define MBS_CHECKED		  0x0002
	#define MBS_ITEM_SPACEBAR 0x8001

	class MenuBar : public ui::Wnd
	{
		MENUBARSTATETYPE m_eMenuBarState;
		DWORD m_dwHoverMenu;
	public:
		class Menu 
		{
			struct _tagItem
			{
				DWORD m_dwStyle;
				char szText[MENUBAR_MAX_ITEM_TEXT];
				RECT rc;
			public:
				_tagItem():m_dwStyle(0){ rc.left = rc.top = rc.right = rc.bottom = 0;}
				void Enable() { m_dwStyle &= ~MBS_DISABLE;}
			};

			_tagItem m_stItemArray[MENUBAR_MAX_ITEM];
			DWORD m_dwItemCnt;

			BOOL  m_bClicked;

			DWORD m_dwHoverItem;
			DWORD m_dwSelItem;

		public:

			DWORD m_dwStyle;
			char  m_szText[MENUBAR_MAX_MENU_TEXT];

			RECT m_rcBtn;

			int  m_iDropWidth,m_iDropHeight;//下拉宽度

			Menu():m_dwStyle(0),m_dwItemCnt(0),m_bClicked(0),m_dwHoverItem(0xffffffff),m_dwSelItem(0xffffffff) {m_rcBtn.left = 
				m_rcBtn.top = m_rcBtn.right = m_rcBtn.bottom = m_iDropWidth = m_iDropHeight = 0;}

			const char *GetItemText(DWORD dwItem);

			DWORD GetItemCnt() const { return m_dwItemCnt;}

			void  Enable()  { m_dwStyle &= ~MBS_DISABLE;}
			void  Disable() { m_dwStyle |= MBS_DISABLE;}

			void  EnableItem(DWORD dwItem);
			void  DisableItem(DWORD dwItem);

			void  SetCheckItem(DWORD dwItem,BOOL bCheck);
			BOOL  IsChecked(DWORD dwItem);

			DWORD AddItem(const char * pszText);

			void Render(void);
			void Update(void);

			DWORD HitTestItem(POINT *pPoint);

			void MoveMenu(int iOX,int iOY);
			BOOL PtInRect(const POINT * pPoint);

			BOOL IsClicked();
			DWORD GetSelItem() const { return m_dwSelItem;}
			void  SetSelItem(DWORD dwItem) { m_dwSelItem = dwItem;}

			const char * GetText() const { return m_szText;}

			void ClearItem();
		};

	private:
		Menu m_stMenuArray[MENUBAR_MAX_MENU];
		DWORD m_dwMenuCnt;

		DWORD m_dwSelMenu;
		DWORD m_dwSelMenuItem;
		BOOL  m_bSelected;//选择标志

		DWORD HitTestMenu(POINT *pPoint);

	public:
		MenuBar(void);
		~MenuBar(void);

		virtual void MoveWnd(int iOX,int iOY);
		virtual void SizeWnd(int iCX,int iCY);
		virtual void HitTest(const POINT *pPoint, ui::Wnd **ppWnd);

		virtual void Render(void);
		virtual void Update(void);

		DWORD AddMenu(const char *szText);

		BOOL  TestSelMenu(DWORD &dwMenu,DWORD &dwItem);

		void OpenPopupMenu();
		void ClosePopupMenu();

		DWORD AddMenuItem(DWORD dwMenu, const char * pszText);

		void DisableMenu(DWORD dwMenu);
		void EnableMenu(DWORD dwMenu);

		void DisableMenuItem(DWORD dwMenu,DWORD dwItem);
		void EnableMenuItem(DWORD dwMenu,DWORD dwItem);

		void SetCheckMenuItem(DWORD dwMenu,DWORD dwItem,BOOL bCheck);

		DWORD GetMenuCnt() { return  m_dwMenuCnt;}

		void ClearMenuItem(DWORD dwMenu);
		

		Menu * GetMenu(DWORD dwMenu);
	};

	/////////////////////////////////////////////////////////////////////
	//	说明:  快捷菜单
	//	历史:  2002 - 2003
	////////////////////////////////////////////////////////////////////
	class QuickMenu : public ui::Wnd
	{
		enum QUICKMENUSTATE { QMS_NORMAL,QMS_HOVER};
		
		class Item : public ui::Wnd
		{
			QUICKMENUSTATE m_eState;	
		public:
			Item() 
			{ 
				m_eState = QMS_NORMAL;
			}
			virtual void Render();
			virtual void Update();

			BOOL LineIntersect(POINT ptLineA[2],POINT ptLineB[2]);
			BOOL LineRectIntersect(POINT ptLine[2]);
			BOOL CheckHover(void);
		};

		std::vector<Item *> m_vItemArray;

		DWORD m_dwRadius;

	public:
		QuickMenu(void) {m_dwRadius = 64;};
		virtual ~QuickMenu(void){};

		BOOL Create(const char *pszName, const RECT *pRECT, DWORD dwStyle, ui::Wnd *pParent);
		virtual void Destroy(void);

		virtual void Render(void);
		virtual void Update(void);	

		DWORD AddItem(const char *pszName,const RECT *pRect);
		
		BOOL TestSelMenu(DWORD &dwItem);						//取回选择的索引
		void BeginPopUp();										//开始弹出
		void EndPopUp();										//结束弹出	
		void SetRadius(DWORD dwRadius);							//设置导航距离
	};
	

	/////////////////////////////////////////
	//说明： 滚动条
	//历史：2002 - 2003
	/////////////////////////////////////////
	
	#define FWS_SBS_VERTICAL 0x00010000  //垂直
	#define FWS_SBS_HORIZON  0x00020000	 //水平

	class ScrollBar : public Wnd
	{
		int m_iRange; //实际的滚动的范围

		int m_iSliderSize; //划块长度	
		int m_iSliderPos;  //滑块位置    	
		int m_iSliderRange;//滚动范围

		BOOL  m_bDrag;

		int   m_iOffset;
		int   m_iPageSize; //滚动页长度

		BOOL  m_bSliding;  //拉动状态

	public:

		ScrollBar(void);
		~ScrollBar(void);

		virtual void Update();
		virtual void Render();
		virtual void SetWndRect(RECT *pRect);

		void SetRange(int iRange);
		void SetPageSize(int iPageSize);
		int  GetPageSize() const { return m_iPageSize;}

		void SetPos(int iPos);
		int  GetPos(void);

		BOOL IsSliding();

		void CalcScrollBar(void);		
	};

	/////////////////////////////////////////////////////////////////////
	//	说明:  列表框
	//	历史:  2002 - 2003
	////////////////////////////////////////////////////////////////////

	#define FWS_LBS_CHECK			0x00010000
	#define FW_LB_SCROLLBAR_WIDTH   16

	class ListBox :	public ui::Wnd
	{
		class Item 
		{
			std::string m_szText;
			BOOL  m_bChecked;
		public:
			BOOL GetCheck() const { return m_bChecked;}
			void SetCheck(BOOL bValue) { m_bChecked = bValue;}
			
			const char * GetText() const { return m_szText.c_str();}
			void  SetText(const char *pszText) { m_szText = pszText;}
		};

		std::vector<Item *>m_vItemArray;


		int m_iSelectIndex;
		int m_iHoverIndex;

		int m_iScrollPos;

		int m_iDisplayMax;

		RECT m_rcWorkarea;//显示区域 = 窗口 - 滚动区域

		ScrollBar *m_pScrollBar;

		BOOL m_bSelChanged;//选择变化了

	public:

		void InsertItem(const char *pszText);

		virtual BOOL Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent);
		virtual void Destroy();

		virtual	void MoveWnd(int iOX,int iOY);

		void Calculate();

		int  GetSelItem() const { return m_iSelectIndex;}
		void SetSelItem(int iIndex) { m_iSelectIndex = iIndex;}

		void Update();
		void Render();

		int  GetItemCount() const { return (int)m_vItemArray.size();}

		ListBox();
		~ListBox();

		void DeleteAllItem();

		BOOL GetItemChecked(int iItem);
		void SetItemChecked(int iItem,BOOL bValue);
		const char * GetItemText(int iItem) { return m_vItemArray[iItem]->GetText();}


		BOOL IsSelChanged() { BOOL bResult = m_bSelChanged; m_bSelChanged = FALSE;return bResult;}

	};
	/////////////////////////////////////////////////////////////////////
	//	说明:  单行编辑框
	//	历史: 2001 - 2002
	////////////////////////////////////////////////////////////////////

	#define INPUT_MAX_CHAR 256

	class LineEditBox : public Wnd  
	{

		enum CONTROLKEYSTATUS { CONTROL_KEY_NONE,CONTROL_KEY_LEFT,CONTROL_KEY_RIGHT,CONTROL_KEY_BACKSPACE,CONTROL_KEY_DELETE,
			CONTROL_KEY_HOME,CONTROL_KEY_END};

		CONTROLKEYSTATUS m_eControlKeyStatus;

		BOOL m_bShiftDown,m_bCtrlDown;

		DWORD m_dwCursorDelay;
		DWORD m_dwLastCursorTime;
		BOOL  m_bCursorBlink;

		DWORD  m_dwRepeatRate;
		DWORD  m_dwLastRepeatTime;

		int m_iCursor;
		int m_iSelStart,m_iSelEnd;	
		int m_iDisplayPos;

		//temp data for display
		int  m_iTempDisplayLen;
		int  m_iTempTextX,m_iTempTextY,m_iTempTextW,m_iTempTextH;
		int  m_iTempFontHeight;
		int  m_iTempSelX,m_iTempSelWidth;

		WORD m_pwInputData[INPUT_MAX_CHAR+1];
		WORD m_pwWidthTable[INPUT_MAX_CHAR+2];

		BOOL m_bEnter;

	protected:	
		void PreCalcDisplayData();
		void RightScroll();
		void LeftScroll(int iChars);
		void WStrcat(WORD *pDst,WORD *pSrc);
		void WStrncpy(WORD *pDst,WORD *pSrc,int iLen);
		void End();
		void Home();
		void Delete();
		void CalcStringWidthTable();
		void DeleteText(int iPos,int iCnt);
		void Backspace();
		void MakeDisplayString(char *pszString,int iLen);
		int  WStrlen(WORD *pSrc);	
		void WStrcpy(WORD *pDst,WORD *pSrc);
		void Right();
		void Left();

	public:

		int  InsertText(int iPos, const char *pText,int iLen);
		BOOL CheckEnter();
		void GetEditText(char *pszString,int iLen);

		int  GetLength();
		void Clear();

		virtual void Render();
		virtual void Update();
		LineEditBox();
		virtual ~LineEditBox();

	};

	/////////////////////////////////////////////////////////////////////
	//	说明:  TAB列表控件
	//	历史:  2002 - 2003
	////////////////////////////////////////////////////////////////////
	#define FTABCTL_ITEM_OFFSET_RIGHT -70
	#define FTABCTL_ITEM_OFFSET_TOP 2

	class TabCtl :	public Wnd
	{
		class Label
		{
		protected:
			BOOL m_bHighLight;

			char m_szText[256];
			int  m_iWidth,m_iHeight;
			int  m_iNameWidth,m_iNameHeight;

			int  m_iX,m_iY;

		public:
			BOOL IsHighLight();

			void Create(const char *pszName);

			void SetPosition(int iX,int iY) { m_iX = iX; m_iY = iY;}
			void DrawLabel();

			int  GetWidth()	 { return m_iWidth;}
			int  GetHeight() { return m_iHeight;}

			Label() :
			m_bHighLight(FALSE)
			{}
			virtual ~Label(){};

			void SetHighLight(BOOL bHighLight) { m_bHighLight = bHighLight;}

			const char * GetText() const { return m_szText;}
		};

		std::vector<Label *>m_vectorLabel;

		int m_iLabelDisplayPos,m_iLabelDisplayCount;

		int m_dwHighLightLabel;

		Button *m_pBtnLeft,*m_pBtnRight;

		void CalcLabelDisplayCount();
		void SetupButton(void);
		void CalcDisplayLabel(void);

	public:
		virtual BOOL Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent);
		virtual void Destroy();

		virtual void Update();
		virtual void Render();

		void AddLabel(const char * pszName);
		
		DWORD GetHighLightLabel() { return m_dwHighLightLabel;}
		DWORD GetLabelCount() const { return (DWORD)m_vectorLabel.size();}
		void  SetHighLightLabel(DWORD dwLabel);
		const char * GetLabelText(DWORD dwLabel);
		
		void  DeleteAllLabel();

		TabCtl(void);
		~TabCtl(void);

	
	};

	/////////////////////////////////////////////////////////////////////
	//	说明:  界面管理器
	//	历史:  2002 - 2003
	////////////////////////////////////////////////////////////////////
	class Manager  
	{
		//input data buffer
		DECLARE_SINGLETON(Manager);

		Manager();
		virtual ~Manager() {};

		Wnd *m_pMainWnd;
		Wnd *m_pActiveWnd;

		struct _tagPopupWindowState
		{
			UI_POPUPWINDOW_STATE m_ePopupWindowState;
			Wnd *pTargetWnd;
			Wnd *pOldActive;
		}m_stPopupWindowState;


		BOOL m_bEdit;

	public:

		struct _tagINPUTINFO
		{
			//消息相关数据
			UI_INPUT_MSG_TYPE eType;
			DWORD  dwData;
			WPARAM wParam;
			LPARAM lParam;

			//查询状态
			POINT ptMouse,ptMouseOld;
			DWORD dwMouseButtonState;

			BYTE  byKeyBuffer[256];

		}m_stInputInfoBuffer;

	public:

		Wnd * GetActiveWindow() { return m_pActiveWnd;}
		Wnd * GetPopupWindow()  { return m_stPopupWindowState.pTargetWnd;}

		void InactiveWindow(Wnd *pWnd);

		Wnd * GetMainWnd() const { return m_pMainWnd;}

		_tagINPUTINFO * GetInputInfoBuffer() { return  &m_stInputInfoBuffer;}

		void Render();																//渲染
        void Update();																//刷新

		void BeginPopupWnd(Wnd * pWnd);												//打开popup窗口
		void EndPopupWnd();															//关闭popup窗口

		void ActiveWindow(Wnd * pWnd);												//激活窗口

		BOOL Create();																		
		void Destroy(void);

		void SetMainWnd(Wnd *pWnd);													//设置渲染窗口
		void ModifyStyle(ui::Wnd * pWnd,DWORD dwAdd,DWORD dwRemove);
	};

};
