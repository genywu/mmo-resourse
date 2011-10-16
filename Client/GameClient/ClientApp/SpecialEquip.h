#pragma once

class GameMap;
class AnimInfo;
class CDisplayModel;
class CShape;
class CSpecialEquip
{
public:
	CSpecialEquip();
	virtual ~CSpecialEquip();

	void SetModelID(DWORD wID) { m_dwModelID = wID;}
	void SetType(DWORD dwType) { m_dwType = dwType;}
	void SetLevel(DWORD level);
	DWORD GetLevel(){return m_dwLevel;}
	AnimInfo* GetAnimInfo(){return m_pAnimInfo;}
	virtual void SetSimpleMesh(bool bsimplemesh){m_bsimplemesh = bsimplemesh;}
	//设置绑定的位置
	void SetLocator(DWORD dwLocator){m_dwLocator=dwLocator;}
	DWORD GetLocator(){return m_dwLocator;}

	void SetModelColorCompel(DWORD dwColor);		//强制设定装备显示模型颜色

	BOOL IsVisible() const { return m_bVisiable;}
	DWORD&  GetState()  {return m_eStatus;}
	CDisplayModel* GetDisplayModel(){return m_pDisplayModel;}
	VOID SetVisible(BOOL bVisible)
	{
		m_bVisiable=bVisible;
	}
	virtual void Copy(CSpecialEquip* psequip);

	virtual void Destroy();
	//设置显示模型
	virtual void SetDisplayModel();

	//隐藏
	void Hide();

	//显示
	virtual void Display(CShape * pParentObj);																				//显示武器
	virtual void DisplayForFlashColor(CDisplayModel * pParentModel, AnimInfo * pParentAnimInfo, DWORD dwFlashColor);		//显示装备带闪光颜色的特殊效果(强制接受灯光处理)
	virtual void DisplayForExStateColor(CDisplayModel * pParentModel, AnimInfo * pParentAnimInfo, DWORD dwExColor);			//显示装备带异常状态颜色的特殊效果(强制接受灯光处理)

protected:
	DWORD m_dwModelID;	
	DWORD m_dwLevel;
	DWORD m_dwType;
	BOOL m_bVisiable;
	DWORD m_eStatus;
	bool m_bsimplemesh;

	//绑定点位置
	DWORD m_dwLocator;
	AnimInfo *m_pAnimInfo;
	//显示模型
	CDisplayModel *m_pDisplayModel;
};