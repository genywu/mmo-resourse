#pragma once

#include "..\..\engine\ui.h"
#include "..\..\engine\render.h"

#include "buildtree.h"


class EditMap;

enum CONTROL_CAMEAR_OPERATION{ CCO_NONE,CCO_MOVE,CCO_ROTATE,CCO_SCALE,CCO_VMOVE};

//当前编辑状态
enum EDIT_STATUS_TYPE 
{ 
//	EST_WATER= 0,//"水面"
	EST_TERRAINTEXTURE = 0,//"地表"
	EST_TERRAINHEIGHT,//"地形"
	EST_BUILDING,//建筑"
	EST_MATERIAL,//""材质"
	EST_BLOCKFLAG,//阻挡
	EST_SWITCH,//切换点
	EST_SECURITYFLAG,//安全区
	EST_WARFLAG,//城战区
	EST_ENVEFFECTFLAG,//环境效果
	EST_ENVCREATUREFLAG,//环境生物
	EST_GRIDVISIBLE,//挖洞
	EST_PAINTCOLOR,//颜色
	EST_WEATHEREFFECTEDIT,//天气
	EST_CAMERAPATH,//路径
	EST_END};


class AboutWnd;

class WndWater;
class MtlListBox;
class BlockListBox;
class ListBoxSecurity;
class ListBoxWar;
class WndDoorEdit;
class WndPopupDoorEdit;
class WndDoorEdit;
class ToolDoorEdit;
class ToolBuildEdit;
class ToolLightEdit;
class WndTerrainTileSet;
class WndSelectBuild;
class WndTerrainHeightEdit;
class WndSetTerrainTile;
class WndSetSkyBox;
//class WndHeightEdit
class WndMiniMap;
class WndLightAdjust;
class WndRandomReplace;
class WndExport;
class WndFogAdjust;
class WndCameraPath;
class WndNewMap;


class ToolBlockEdit;
class ToolMtlEdit;
class ToolSecurityEdit;
class ToolWarEdit;
class ToolSetTile;
class ToolTerrainHeight;
//class ToolHeightEdit;
class ToolGridVisibleEdit;
class ToolWeatherEffectFlagEdit;
class ToolPaintColor;
class ToolEnvEffectFlagEdit;
class ToolEnvCreatureFlagEdit;
class GameSkybox;


namespace light
{
	class DirectionalLight;
};

class SelectFrustum;
class NewMapDlg;

// 基本格子结构
struct tagCell
{
	unsigned	short bBlock	:3;	// 阻挡（0：可行走  1：可飞行  2：不可飞行）
	unsigned	short bSecurity	:3;	// 安全区
	unsigned	short lWar		:2;	// 城战区
	unsigned	short lMaterial	:8;	// 材质
	WORD		lSwitch;			// 切换点(编号从1开始)
};


#define PICK_SELECT_RECT_SIZE 8

#define TERRAINMASK				0x00000001   //地表
#define BUILDINGMASK			0x00000002   //建筑
#define BLOCKMASK				0x00000004   //阻挡
#define MATERIALMASK			0x00000008   //材质
#define SECURITYMASK			0x00000010   //安全区
#define WARMASK					0x00000020   //战场
#define GRIDMASK				0x00000040	 //网格
#define SWITCHMASK				0x00000080
#define ENVCREATUREMASK		    0x00000100    
#define LIGHTMASK				0x00000200   //灯光
#define ENVEFFECTMASK		    0x00000400   //环境效果标志
//#define GROUNDHELPERMASK        0x00000800   //护垫
//#define ENABLELIGHTCALCULATE    0x00001000	 //光照计算
#define WEATHEREFFECTMASK		0x00002000	 //天气区域标志

#define MOUSEHOTX  7
#define MOUSEHOTY  6


class EditWnd :	public ui::Wnd
{
	DWORD m_dwAutoSaveTimer;
	BOOL  m_bHasEditFileName;
	char  m_szCurrentEditFile[_MAX_PATH];

	DWORD m_dwRenderFlag;

	EDIT_STATUS_TYPE m_eEditStatus;

	render::Camera  * m_pCamera;
	render::Frustum * m_pFrustum;

	GameSkybox *m_pGameSkybox;

	light::DirectionalLight * m_pDirectionalLight;
	
	CONTROL_CAMEAR_OPERATION m_eCamOpType;

	ui::QuickMenu * m_pModeMenu;

//	WndWater			 * m_pWndWater;				//水面
	MtlListBox			 * m_pMtlListBox;			//材质
	ListBoxSecurity  	 * m_pListBoxSecurity;		//安全区
	ListBoxWar			 * m_pListBoxWar;			//城战
	WndDoorEdit			 * m_pWndDoorEdit;			//切换点
	WndRandomReplace	 * m_pWndRandomReplace;
	WndMiniMap      	 * m_pWndMiniMap;
	WndTerrainTileSet    * m_pWndTerrainTileSet;	//图素选择窗口
	WndSelectBuild       * m_pSelectBuildWnd; 
	WndTerrainHeightEdit * m_pWndTerrainHeightEdit; //地表高度编辑窗口

	BuildTree			  m_theBuildTree;

	ToolBlockEdit       * m_pToolBlockEdit;
	ToolMtlEdit         * m_pToolMtlEdit;
	ToolSecurityEdit    * m_pToolSecurityEdit;
	ToolWarEdit		    * m_pToolWarEdit;	
	ToolEnvEffectFlagEdit * m_pToolEnvEffectFlagEdit;	
	ToolEnvCreatureFlagEdit *m_pToolEnvCreatureFlagEdit;
	ToolDoorEdit	    * m_pToolDoorEdit;
	ToolSetTile         * m_pToolSetTile;
	ToolBuildEdit	    * m_pToolBuildEdit;
	ToolGridVisibleEdit * m_pToolGridVisibleEdit;
	ToolLightEdit	    * m_pToolLightEdit;
	ToolTerrainHeight	* m_pToolTerrainHeight;
	ToolPaintColor	    * m_pToolPaintColor;
	ToolWeatherEffectFlagEdit * m_pToolWeatherEffectFlagEdit;
	
	BOOL m_bShowGrid;	
	BOOL m_bShowMap;
	BOOL m_bShowTexture;

	BOOL m_bEditSunLightDirection;

	BOOL m_bWireframeMode;
	BOOL m_bLockCamera;
	BOOL m_bShowNormal;
	BOOL m_bShowFog;
	BOOL m_bShowSkybox;

	enum  CURSOR_STATUS
	{
		CURSOR_NORMAL,
		CURSOR_ROTATION,
		CURSOR_MOVE,
		CURSOR_SCALE,
		CURSOR_SUCKCOLOR,
		CURSOR_END,
	};

	/*struct tagCursor
	{
		DWORD dwHotX,dwHotY;
        DWORD hTextureHandle;
	} m_tCursor[CURSOR_END];*/
	
	//enum CURSOR_STATUS m_eCursorStatus;

	ui::MenuBar *m_pMenuBar;

	AboutWnd * m_pAboutWnd;
	
	DWORD m_dwStartTime;
	DWORD m_dwCurFrame;

	WndNewMap        * m_pNewMapDlg;
	WndPopupDoorEdit * m_pWndPopupDoorEdit;
	WndExport		 * m_pWndExport;
	WndCameraPath    * m_pWndCameraPath;
	WndLightAdjust   * m_pWndLightAdjust;
	WndFogAdjust		* m_pWndFogAdjust;
	WndSetTerrainTile* m_pWndSetTerrainTile;
	WndSetSkyBox* m_pWndSetSkyBox;



	//环境光变化(24)小时


private:
	
	int  _GetFps(void);

	void MenuBarProc(void);
	void CreateMenuBar(void);
	void DestroyMenuBar(void);
	//void _ShowCursor(void);
	void _ShowInformation(void);
	void _SetTransform(void);
	BOOL _UpdateCamera(void);

	void _CreateModeMenu(void);
	void _DestroyModeMenu(void);
	void ModeMenuProc(void);
	void _ShowStatus(void);
	void _CreateEditUI(void);
	void _SetupEditUI(void);
	void _DestroyEditUI(void);
	void _ShowAxis(void);
	void _NewMap(int iWidth,int iHeight);
	void _CenterCamera(int iWidth,int iDepth);
	void _CheckHotKey(void);
	void _ExportRgnFile();
	void _CreateEditTool();
	void _DestroyEditTool();

	void _SetMenuBarItemCheck(void);

	void OnSaveFile();
	void OnSaveFileAs();
	void OnOpenFile();	
	void _SetEditFileName(const char * pszFileName);
	void _DoAutoSave(void);
	void SaveMap(const char * pszFileName);
	
public:
	EditWnd(void);
	~EditWnd(void);

	BOOL Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent);
	void Destroy();

	void Update();
	void Render(void);

	//interface
	render::Camera*		GetCamera() const { return m_pCamera;}
	render::Frustum*	GetFrustum() const { return m_pFrustum;}

//	WndWater *			GetWndWater()	{ return m_pWndWater; }
	MtlListBox*			GetMtlListBox() { return  m_pMtlListBox;}
	ListBoxSecurity*	GetListBoxSecurity() { return m_pListBoxSecurity;}
	ListBoxWar*			GetListBoxWar() { return m_pListBoxWar;}
	WndPopupDoorEdit*	GetWndPopupDoorEdit() { return m_pWndPopupDoorEdit;}
	WndExport		*   GetWndExport() { return m_pWndExport;}
	WndCameraPath	*	GetWndCameraPath() { return m_pWndCameraPath;}
	
	WndLightAdjust  *	GetWndLightAdjust() { return m_pWndLightAdjust;}
	WndFogAdjust    *	GetWndFogAdjust() { return m_pWndFogAdjust;}
	WndDoorEdit*		GetWndDoorEdit()   { return m_pWndDoorEdit;}
	WndRandomReplace*	GetWndRandomReplace()   { return m_pWndRandomReplace;}
	WndNewMap  *		GetNewMapDlg()   { return m_pNewMapDlg;}
	WndSelectBuild*		GetWndSelectBuild() { return m_pSelectBuildWnd;}
	WndTerrainTileSet*	GetWndTerrainTileSet() { return m_pWndTerrainTileSet; }
	WndTerrainHeightEdit* GetWndTerrainHeightEdit() { return m_pWndTerrainHeightEdit; }
	WndMiniMap*			GetWndMiniMap() { return m_pWndMiniMap; }

	BuildTree *		    GetBuildTree() { return &m_theBuildTree;}
	GameSkybox*&        GetSkyBox() { return m_pGameSkybox; }

	ToolSetTile*		GetTileTool() { return m_pToolSetTile; }	
	ToolBuildEdit*		GetToolBuildEdit() { return m_pToolBuildEdit;}
	ToolLightEdit*		GetToolLightEdit() { return m_pToolLightEdit;}
	ToolDoorEdit*		GetToolDoorEdit()  { return m_pToolDoorEdit;}
	ToolTerrainHeight*	GetToolTerrainHeight() { return m_pToolTerrainHeight; }
	ToolPaintColor*		GetToolPaintColor() { return m_pToolPaintColor; }

	BOOL GetWireframeMode() { return m_bWireframeMode;}
	void SaveMiniMap(void);
	void WriteTGA(UINT uBits, UINT uWidth, UINT uHeight, BYTE *pixels, FILE *pf);
	void CreatePopupWnd(void);
	void _DestroyPopupWnd(void);
	void _ExportSoundSourceListFile(void);
	void OnExportCamera(void);
	void ImportCamera(const char *szFileName);
	void _RenderReference(void);
	void ExportMap(BOOL bSurface, BOOL bHeightmap, BOOL bColormap, BOOL bGrid, BOOL bNode,BOOL bSound);
	bool FileIsExist(LPCSTR sourceFile,LPCSTR lpFileName);
	void QuickExprot();
};
