#pragma once

#include "..\..\frostengine\ui.h"
#include "..\..\frostengine\render.h"
#include "..\..\gameengine\gamemodel.h"
#include "..\..\gameengine\textureprojective.h"
#include "..\..\gameengine\GameSelectRay.h"
#include "light.h"
#include "AppClient\Wing.h"
#include "AppClient\Weapon.h"
#include "AppClient\BindModel.h"

enum CONTROL_CAMEAR_OPERATION{ CCO_NONE,CCO_MOVE,CCO_ROTATE,CCO_SCALE};

enum eModelType
{
	MT_ROLE,	//角色模型
	MT_BIND		//绑定模型
};

class ui::QuickMenu;
class WndEdit;
class WndEditBGColor;
class WndEditLightColor;
class WndAnimModify;
class WndGridMovingSpeedModifyDlg;
class WndSelectLevel;
class WndHotkey;
class WndAbout;
class WndAction;
class WndPart;
class WndLocator;
class WndBindModel;
class WndAnimFrameEx;
class WndGuage;
class GameModel;
class CRoleModel;
class CtrlExplorer;
class CtrlAttribEditor;
class WndModelTexture;
class WndShaderModify;
class WndModelTree;
class WndPartTree;
class WndModelModify;
class WndPartModify;
class WndTrailTree;
class WndTrailModify;
class WndParticleTree;
class WndParticleModify;
//class CGameModelManager;

class WndViewer : public ui::Wnd
{
	friend class WndAction;
	friend class WndLocator;
	friend class WndModel;
	friend class WndAnimModify;
	friend class WndSelectLevel;
	friend class WndModelTexture;
	friend class WndShaderModify;

	render::Camera  * m_pCamera;
	render::Frustum * m_pFrustum;
	SunLight		* m_pSunLight;
	GameSelectRay	  m_ViewfinderRay;

	float					 m_fRotateAngle;
	CONTROL_CAMEAR_OPERATION m_eCamOpType;

	BOOL m_bLightEnable;
	BOOL m_bDisplayWireFrame;
	BOOL m_bShowGrid;
	BOOL m_bShowGridMoving;
	BOOL m_bShowSizeBox;
	BOOL m_bShowPickBox;
	BOOL m_bShowLocator;
	BOOL m_bShowBone;
	BOOL m_bShowNormal;
	BOOL m_bShowCameraInfo;
	BOOL m_bAnimPlaying;
	BOOL m_bAnimLooped; 
	BOOL m_bShowLightArrow;
	BOOL m_bShowLocatorWnd;
	BOOL m_bSwitchBK;					//切换背景图片开关(真则显示背景图片,否则显示背景颜色)
	BOOL m_bExAnimFrame;				//处理导出动画帧
	BOOL m_bShowViewfinder;				//显示取景器
	BOOL m_bTrail;						//是否显示模型尾迹
	BOOL m_bShowUI;						//是否显示UI
	DWORD m_dwAnimOnceLastTime;			//播放一次的最后时间
	DWORD m_dwGridMovingStartTime;
	DWORD m_dwGridTexMovingStartTime;
	float m_fGridMovingSpeed;

	int		   m_iLevelModelIndex;		//当前显示等级信息的模型索引
	eModelType m_eLevelModelType;		//当前显示等级信息的模型类型

	int	  m_hCursorHandle;
	DWORD m_dwBK;											//背景图片的句柄(为0,则说明没有加载背景图片)
	DWORD m_dwGridTex;										//地表纹理句柄
	render::Interface::tagTextureInfo * m_ptBKTexInfo;		//当前背景纹理信息

    DWORD	 m_dwGroupMenuItemCount;			//主模型菜单项总数
	DWORD	 m_dwWeaponLeftMenuItemIndex;		//左手武器菜单索引
	DWORD	 m_dwWeaponRightMenuItemIndex;		//右手武器菜单索引
	DWORD	 m_dwAmbientColor;
	D3DCOLOR m_dwLightDirectionColor;

	ui::MenuBar	  * m_pMenuBar;
	ui::QuickMenu * m_pQuickMenu;	//快捷菜单

	D3DXVECTOR3		m_vRolePos;
	CDisplayModel * m_pModel;		//当前显示的角色主模型
	CDisplayModel * m_pRefModel;	//显示角色参考模型

	std::vector<CRoleModel *> m_vRoleModels;			//角色模型动态数组
	std::vector<CBindModel *> m_vBindModels;			//绑定模型动态数组

	WndAnimModify				* m_pWndAnimModify;
	WndGridMovingSpeedModifyDlg * m_pWndGridMovingSpeedModify;
	WndHotkey					* m_pWndHotkeyList;
	WndAbout					* m_pWndAbout;
	WndEditBGColor				* m_pWndEditBKColor;
	WndEditLightColor			* m_pWndEditLightColor;
	WndSelectLevel				* m_pWndSelectLevel;
	WndAction					* m_pWndAction;
	WndPart						* m_pWndPart;
	WndModel		  			* m_pWndModels;
	WndLocator		  			* m_pWndLocator;
	WndAnimFrameEx	  			* m_pWndAnimFrameEx;
	WndGuage		  			* m_pWndGuage;
	WndModelTexture	  			* m_pWndModelTexture;
	WndShaderModify   			* m_pWndShaderModify;
	WndModelTree	  			* m_pWndModelTree;
	WndPartTree		  			* m_pWndPartTree;
	WndModelModify    			* m_pWndModelModify;
	WndPartModify	  			* m_pWndPartModify;
	WndTrailTree	  			* m_pWndTrailTree;
	WndTrailModify	  			* m_pWndTrailModify;
	WndParticleTree	  			* m_pWndParticleTree;
	WndParticleModify 			* m_pWndParticleModify;
	DWORD m_dwStartTime;
	DWORD m_dwCurFrame;

	AnimInfo m_cAnimInfo;			//角色模型动画信息
	AnimInfo m_cAnimInfoRef;		//参考模型动画信息

	// 其他
	char  m_szPathNameTmp[_MAX_PATH];				//当前打开的模型文件名
	char  m_szViewframeDis[20];
	float m_fYRot;
	bool  m_bKeyState[256];							//用于键盘单击检测的键盘状态

	// 动画帧导出相关
	DWORD			   m_dwAnimTimeLength;
	DWORD			   m_dwAnimFrameIndex;
	DWORD			   m_dwAnimFPS;
	char			 * m_szpFileFormat;
	D3DXIMAGE_FILEFORMAT m_IFF;
	LPDIRECT3DSURFACE9 m_pOldRenderTarget;
	LPDIRECT3DSURFACE9 m_pOldZStencil;
	LPDIRECT3DTEXTURE9 m_pRenderTexture;
	LPDIRECT3DSURFACE9 m_pDepthStencilSurface;

	D3DXVECTOR3		TargetPos;
public:
	WndViewer(void);
	~WndViewer(void);

	BOOL Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent);
	void Destroy();

	void Update();
	void Render(void);
	int  GetFps(void);

	void MenuBarProc(void);
	void CreateMenuBar(void);
	void DestroyMenuBar(void);
	void QuickMenuProc(void);				//快捷菜单事件处理

	void ShowCursor(void);
	void ShowFps(void);
	void ShowAxis(void);
	void ShowGrid(void);
	void ShowInfo(void);
	void DisplayTextureProjective();
	void GetBuffer(int x,int z,DWORD size,D3DXVECTOR3* posbuff);

	void SetupTransform(void);
	void SetMenuBarItemCheck(void);
	void SetQuickMenuVisible(bool bIsVis)		{ bIsVis ? m_pQuickMenu->BeginPopUp() : m_pQuickMenu->EndPopUp(); }		//设定快捷菜单是否可见
	void SwitchQuickMenuVisible(void);			//切换快捷菜单是否可见
	void MoveQuickMenu(POINT & ptDes)			{ m_pQuickMenu->MoveWndTo(ptDes.x, ptDes.y); }							//移动快捷菜单

	CDisplayModel * GetModel()					{ return m_pModel; }
	CDisplayModel * GetCurrentSelBindModel();	//得到当前选择的绑定模型
	AnimInfo	  * GetAnimInfo()				{ return &m_cAnimInfo; }
	void			GetModelName(string & sModelName, const string & sModelPathName);
	void			SetCurOperFileName(const char * pszFileName)		{ strcpy(m_szPathNameTmp, pszFileName); }			//设定当前操作的文件路径名

	float GetGridMovingSpeed()					{ return m_fGridMovingSpeed; }
	void SetGridMovingSpeed(float f)			{ m_fGridMovingSpeed = f; }

	void SetLevelModelIndex(int iIndex)			{ m_iLevelModelIndex = iIndex; }
	void SetLevelModelType(eModelType eType)	{ m_eLevelModelType = eType; }

	render::Camera  *GetCamera(){return m_pCamera;}

	void HotkeyProc(void);
	void CalculateNormal(void);
	void SaveNormal(void);
	void ExportCamera(void);
	void ImportCamera(const char *szFileName);
	void ExportAnimationFrame();									//导出动画序列帧
	void SaveVertexColor(void);
	void ResetCamera(void);
	void CreateTemplateModelFile();
	void CheckFile();

	// 打开模型文件函数组
	void OpenMainModelFile(const char * pszFileName);				//打开主模型文件
	void OpenBindModelFile(const char * pszFileName);				//打开绑定模型文件
	void OpenRefModelFile(const char * pszFileName);				//打开参考模型文件
	void OpenBKPicFile(const char * pszFileName);					//打开背景图片文件
	void OpenGridTexFile(const char * pszFileName);					//打开地表纹理

	// 创建菜单条内不固定数量的菜单项
	void CreateMainModelMenu(const char * pszFileName);				//创建主模型菜单
	void CreateActionMenu(const char * pszFileName);				//创建动作菜单
	void CreatePartList(const char * pszFileName);					//创建部件列表菜单
	void CreateBindModelMenu(CBindModel * pBindModel);				//创建主菜单绑定模型条目

	bool KeyClick(BYTE * pbyKeyBuffer, int iScanCodes);				//检测键盘按键单击事件
	WndModel *GetWndModel()		{return m_pWndModels;}				//获取模型窗口
	WndShaderModify *GetWndShaderModify()	{return m_pWndShaderModify;}	//获取SHADER修改窗口
	WndModelTexture *GetWndModelShaderTree(){return m_pWndModelTexture;}	//获取Shader树形窗口
	WndPartTree		*GetWndPartTree()		{return m_pWndPartTree;}		//获取part树形窗口
	WndModelTree	*GetWndModelTree()		{return m_pWndModelTree;}		//获取model树形窗口
	WndModelModify  *GetWndModelModify()	{return m_pWndModelModify;}		//获取model修改窗口
	WndPartModify   *GetWndPartModify()		{return m_pWndPartModify;}		//获取part修改窗口
	WndTrailTree	*GetWndTrailTree()		{return m_pWndTrailTree;}		//获取Trail树形窗口
	WndTrailModify  *GetWndTrailModify()	{return m_pWndTrailModify;}		//获取Trail修改窗口
	WndParticleTree *GetWndParticleTree()	{return m_pWndParticleTree;}	//获取Pariticle树形窗口
	WndParticleModify *GetWndParticleModify() {return m_pWndParticleModify;}	//获取Particle修改窗口
	WndSelectLevel * GetWndSelectLevel()	{ return m_pWndSelectLevel; }	//获取级别选择界面
};