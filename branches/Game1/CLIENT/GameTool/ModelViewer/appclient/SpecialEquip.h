#pragma once

class GameMap;
class AnimInfo;
class CDisplayModel;

class CSpecialEquip
{
public:
	CSpecialEquip();
	virtual ~CSpecialEquip();

	void SetModelID(WORD wID) { m_wModelID = wID;}
	void SetType(DWORD dwType) { m_dwType = dwType;}
	void SetLevel(DWORD level) {m_dwLevel = level;}
	//设置绑定的位置
	void SetLocator(DWORD dwLocator){m_dwLocator=dwLocator;}

	BOOL IsVisible() const { return m_bVisiable;}
	int&  GetState()  {return m_eStatus;}

	AnimInfo * GetAnimInfo()			{ return m_pAnimInfo; }

	CDisplayModel* GetDisplayModel(){return m_pDisplayModel;}
	VOID SetVisible(BOOL bVisible)	{m_bVisiable=bVisible;}
	virtual void Copy(CSpecialEquip* psequip);

	void Create();
	void Destroy();
	
	void Update();
	void Render(render::Camera *pCamera,GameMap *pMap,BOOL alpha = false);
	void RenderToScreen(int iX,int iY,float fScale,float fRotation,render::Camera* pcamera,D3DXMATRIX* matmodelworld,D3DXMATRIX* matworld);
	void RenderShadow();

	//设置显示模型
	void SetDisplayModel();
	void SetDisplayModel(const char * pszFileName);				//从文件名载入模型资源
	//隐藏
	void Hide();
	//显示
	void Display(CDisplayModel *pParentModel,AnimInfo *pParentAnimInfo);
protected:
	WORD m_wModelID;	
	DWORD m_dwLevel;
	DWORD m_dwType;
	BOOL m_bVisiable;
	int m_eStatus;

	//绑定点位置
	DWORD m_dwLocator;
	AnimInfo *m_pAnimInfo;

	//显示模型
	CDisplayModel *m_pDisplayModel;
};