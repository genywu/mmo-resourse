/******************************纹理投影系统模块******************************/
/****BUILD BY 彭谦**/
/****2007-10-26*****/
/******************************纹理投影系统模块******************************/
#pragma once

#include <list>
#include <d3d9.h>
#include <d3dx9.h>
#include "DisplayModel.h"
class CTextureProjective:public LoadState
{
public:
	CTextureProjective();
	~CTextureProjective();
	//DWORD     GetType(){return m_dwtype;}
	void	  SetPosDirAlphaNowtime(float x,float y,float z,float diry,float alpha,DWORD time);
	void      UpdateMatView();
	void      SetLookatPos(D3DXVECTOR3 *pos);
	void      SetNowDirY(float fdiry){m_fdir = fdiry;}
	void      SetNowAlpha(float falpha){m_falpha = falpha;}
	void      SetNowTime(DWORD* time);
	float     GetPosX(){return m_pvlookatpoint->x;};
	float     GetPosZ(){return m_pvlookatpoint->z;};
	//void      SetName(char* name){strcpy(m_sname,name);}
	//char*      GetName(){return m_sname;}
	//D3DVECTOR GetPos(){return m_pvlookatpoint;}

	int       GetSize(){return m_dwsize;};
	void      SetSize(DWORD dwsize);
	void	  SetAlive(BOOL alive){m_balive = alive;}
	//float	  GetSize(){return m_size;}
	void      SetLifeTime(long time);
	//DWORD     GetLifeTime(){return m_dwlifetime;}
	//DWORD     GetNowTime(){return m_dwnowtime;}
	void      AddRendMesh(D3DXVECTOR3* pos,DWORD size);
	void      AddNewTextureProj(CTextureProjective* t);
	//void      UpdateMapArray();
	DWORD     GetMyTpSize(){return (DWORD)m_listmytp.size();}
	void      SetVisiable(BOOL visable);
	BOOL      IsTypeOf(DWORD type){return (type & m_dwtype);}
	BOOL      IsBillboardY()      {return m_billboardy;}
	BOOL      IsAlive()           {return m_balive;}
	BOOL      IsVisable()		  {return m_bvisable;}
	void      CreateFromFile(CRFile* pfile);
	void      SetStartTime(DWORD time){m_dwstarttime = time;}
	DWORD     GetLiveTime(){return m_ilifetime;}
	DWORD GetShaderGroupHandle()	{return m_dwtexturegroup;}
	BOOL  CreateShaderGroup(CRFile* pFile,DWORD dwTextureProcision=0);
	const char*GetShaderFileName()	{return m_pszShaderFileName;}
	struct POSDIRALPHATIME
	{
		D3DXVECTOR3 pos;
		float fdiry;
		float falpha;
		DWORD time;
	};
	std::list<POSDIRALPHATIME> m_listpostime;
	struct BUILDMESH
	{
		D3DXVECTOR3* pos;
		DWORD num;
	};
	std::list<BUILDMESH>m_listmesh;

	//所有对象链表渲染
	//static void RendTextureProjctive(render::Camera* pcamera);
	static std::list<CTextureProjective*>& GetAllCTextureProjectiveList(){return m_listalltp;}
	static CTextureProjective* CreateTextureProjctive();
	static void SetBufferSize(DWORD size);
	static D3DXVECTOR3* GetBuffer(){return m_pposbuffer;}
	static void DestroyTextureProjctive(CTextureProjective* ptp);
	static void SetState();
	static void SetTextureState();
	static void EndState();
	static void CheckReSetRelease();
	static void CheckReSetReBuild();
	static void Destroy();
	//自身渲染
	void SetMixedModel();
	void Rend(D3DXVECTOR3* pcameraposition,D3DXMATRIX matworld,D3DXMATRIX matview,D3DXMATRIX matproj);
	//自身所有对象渲染
	//void Display();
protected:
	//**************************公共属性************************************//
	char m_pszShaderFileName[MAX_PATH];
	//动画类型
	DWORD m_dwtype;
	//起始时间
	DWORD m_dwstarttime;
	//生命时间(-1为无限)
	DWORD m_ilifetime;
	//目前时间
	DWORD* m_dwnowtime;
	//用来定义视图矩阵
	D3DXVECTOR3  m_vseepoint;
	D3DXVECTOR3* m_pvlookatpoint;
	D3DXVECTOR3  m_vheaddir;
	//视图矩阵
	D3DXMATRIX m_matview;
	//纹理变换矩阵
	D3DXMATRIX m_mattexture;
	//正交投影矩阵
	D3DXMATRIX m_matproj;
	//纹理缩放平移矩阵
	D3DXMATRIX m_mattranscal;
	//纹理句柄数组
	DWORD  m_dwtexturegroup;
	DWORD  m_dwtexturegroupindex;
	//是否BILLBOARDY
	BOOL	  m_billboardy;
	//是否ALPHA
	BOOL      m_balpha;
	//是否有效
	BOOL      m_balive;
	//半径(1格为单位)
	DWORD	  m_dwsize;
	//char      m_sname[255];

	//是否显示
	BOOL      m_bvisable;
	//自身对象
	std::list<CTextureProjective * > m_listmytp;
	std::list<CTextureProjective * >::iterator m_itmytp;
	//所有对象链表
	static std::list<CTextureProjective * > m_listalltp;
	//要渲染的顶点数据BUFFER
	static D3DXVECTOR3* m_pposbuffer;
	static DWORD m_dwbuffersize;
	D3DCOLOR  m_mixcolor;
	//D3DCOLOR* m_pcolor;
	//D3DXVECTOR2* m_puv;
	DWORD m_vectornum;
	DWORD m_usenum;
	WORD m_mixedmodel;
	BOOL m_bfollowedowner;
	float m_fdir;
	float m_falpha;
	//**************************公共属性*****************************//


	//**************************缩放动画属性************************************//
	//分段缩放列表
	std::vector<float> m_vsize;
	//缩放总时间
	DWORD m_dwsizetime;
	//**************************缩放动画属性*************************//


	//**************************旋转动画属性************************************//
	//旋转总时间
	DWORD m_dwrotationtime;
	//旋转方向(1:顺时针,-1:逆时针)
	DWORD m_dwrotadir;
	//**************************旋转动画属性*************************//


	//**************************变色动画属性************************************//
	//分段变色列表
	std::vector<D3DCOLOR> m_vcolor;
	//变色总时间
	DWORD m_dwcolortime;
	//**************************变色动画属性*************************//


	//**************************可见度动画属性************************************//
	//分段渐变列表
	std::vector<float> m_vvisiblity;
	//渐变总时间
	DWORD m_dwvisibilitytime;
	//**************************可见度动画属性*************************//

	//**************************序列帧动画属性************************************//
	/*DWORD m_dwuvtimeperframe;
	DWORD m_dwuvtime;
	DWORD m_dwrow;*/
	//**************************序列帧动画属性*************************//


private:
	DWORD InterpolationColor(DWORD dwTimeElapse);
	float InterpolationSize(DWORD dwTimeElapse);
	float InterpolationVisibility(DWORD dwTimeElapse);
	//D3DXMATRIX  UpdateUVAnim(DWORD dwTimeElapse);
	void SetRotation(DWORD dwTimeElapse);
	void SetDir(float dir);
	void SetDiffuseColor(DWORD color);
	void SetAlpha(float alpha);


	//阴影渲染部分
public:
	struct SHADOWTEXTURE  
	{
		SHADOWTEXTURE()
		{
			pShadowTexture = NULL;
		}
		~SHADOWTEXTURE(){};
		LPDIRECT3DTEXTURE9       pShadowTexture;
		D3DXVECTOR3 pos;
		DWORD size;
		void Release()
		{
			SAFERELEASE(pShadowTexture);
		}
	};
	// 添加即时阴影
	// 不能够将DISPLAYMODEL的指针和ANIMINFO添加到链表统一渲染,
	// 因为这2个指针的内容随时在变换,只能马上渲染到纹理,
	// 每次都要设置状态,有可能对效率产生影响,但是影响程度不确定
	static void BeginRendShadow(D3DXVECTOR3 pos,DWORD size);
	static void AddRendShadow(CDisplayModel* pdmodel,AnimInfo* pAnimInfo);
	static void EndRendShadow();
	static void RendShadow(SHADOWTEXTURE* p,D3DXVECTOR3 dir,D3DXMATRIX matworld,D3DXMATRIX matview,D3DXMATRIX matproj);
	static std::list<SHADOWTEXTURE>& GetShadowTextureList(){return m_listshadow;}
	static UINT GetUsedTextureNum(){return m_ushadowusednum;}
	static HRESULT CreateShadowTexture();
	static void SetLightPos(D3DXVECTOR3 lightpos);
	static void SetShadowState();
	static void ClearNum();
	static void ReSetShadowRelease();
	static void ReSetShadowReBuild();
	static void SetSimpleShadow(bool b = false);
	static D3DXMATRIX GetShadowVPSMatrix(D3DXMATRIX matworld){return matworld * m_matshadowview * m_matshadowproj * m_matshadowscanf;}
	D3DXMATRIX GetTPVPSMatrix(D3DXMATRIX matworld){return matworld * m_matview * m_matproj * m_mattranscal;}
	static LPDIRECT3DSURFACE9		m_pTextureSurf;	
	//static void 
protected:

	static D3DXMATRIX m_matshadowtexture,m_matshadowview,m_matshadowproj,m_matshadowscanf;
	static D3DXVECTOR3 m_LightPos;

	// 用于生成阴影贴图的表面

	// 用于生成阴影贴图深度对比的深度缓冲区
	static LPDIRECT3DSURFACE9		m_pShadowZ;	
	// 用于RTT纹理对比的深度缓冲区
	static LPDIRECT3DSURFACE9		m_pRTTZ;	
	// 用于保存原来的后备缓冲区和深度缓冲区
	static LPDIRECT3DSURFACE9		m_pOldBackBuffer, m_pOldZBuffer;	
	// 用于保存原来的视口
	static D3DVIEWPORT9				m_oldViewPort;		
	// 用于保存原来的世界转换、视图、透视矩阵
	static D3DXMATRIX				m_oldMatWorld,m_oldMatView, m_oldMatProj;
	// 用于阴影的视口
	static D3DVIEWPORT9				m_shadowViewPort;							
	// 渲染即时阴影所需要的纹理
	static std::list<SHADOWTEXTURE> m_listshadow;
	static SHADOWTEXTURE m_simpleshadow;
	static bool m_bsimpleshadow;
	// 当前帧阴影纹理的使用数量
	static UINT m_ushadowusednum;

	static D3DXMATRIX m_mattempview;
	static D3DXMATRIX m_mattempprojective;
	static SHADOWTEXTURE* GetCanUseShadowTexture();
	//阴影渲染部分
public:
	// 状态
	enum eTYPE
	{
		TYPE_BODY,			
		TYPE_UPBODYLEFT,
		TYPE_UPBODYRIGHT,
		TYPE_HEAD,			
		TYPE_HUGEBODY,
		TYPE_HEADHEADWEARL,
		TYPE_UPBODYHEADWEARL,
		TYPE_UPBODYHEADWEARR,
	};
	enum eALPHATYPE
	{
		ALPHATYPE_MAINPLAYER,
		ALPHATYPE_CIRCLE,
		ALPHATYPE_NONE,
	};

	struct RTTRESOURCE
	{
		RTTRESOURCE()
		{
			used = false;
			bupdated = false;
		}
		void Release()
		{
			SAFERELEASE(pRttTexture);
		}
		LPDIRECT3DTEXTURE9 pRttTexture;
		bool used;
		bool bupdated;

	};
	//用于挖洞的Alpha图(主角)
	static LPDIRECT3DTEXTURE9 m_pRttAlphaMainPlayer;
	//用于挖洞的Alpha图(队友)
	static LPDIRECT3DTEXTURE9 m_pRttAlphaCircle;
	//渲染到纹理
	//纹理列表
	static std::vector<RTTRESOURCE> m_vecrtt;
	//产生纹理并返回句柄
	static int CreateRTTTexture();
	//开始渲染到纹理(type:0全身照,1大头照)
	static bool BeginRendToTexture(int& texhandle);
	static bool RendToTexture(CDisplayModel* pmodel,AnimInfo* pani,int type = 0);
	static void EndRendToTexture();
	static void DrawTextureToScreen(int x,int y,int s,int handle,int alpha = 0);
	static void SetRttUsedFalse(int i);
	static void ReSetRTTRelease();
	static void ReSetRTTReBuild();
    static LPDIRECT3DTEXTURE9 GetRTTtexture(int texHandle);
protected:





};


