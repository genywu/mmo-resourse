#pragma once

class GameModel;
class CShape;

// 模型管理类
class CPicList
{
public:
	CPicList(void);
	virtual ~CPicList(void);

	// 模型结构
	struct tagModel
	{
		long		lType;			// 类型
		long		lGraphicsID;	// 图片编号
		DWORD		lUseCount;		// 使用的次数
		GameModel	*pModel;		// 模型		
	};

protected:
	list<tagModel*>	m_listModel;					// 模型列表

public:
	tagModel* Load(long lType, long lGraphicsID, bool bFind = true);		// 载入一套资源( 在载入之前是否要查找要求载入的图像是否存在 )
	BOOL Release(long lType, long lGraphicsID);		// 释放一套资源
	BOOL ReleaseAll(void);							// 釋放所有資源

	// 显示
	BOOL Display(CShape* pShape, float x, float y, int Alpha = 16);

	// 获取
	tagModel* GetModel(long lType, long lGraphicsID);	// 获取

	//test 影子
	DWORD m_dwShadowShaderGroupHandle;
	void  Create();
	void  Destroy();
};
