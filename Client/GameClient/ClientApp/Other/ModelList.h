#pragma once

//#include <feffector.h>
#include  <..\..\..\client\gameclient\gameextend\gamemodel.h>
//#include <gamemodel.h>
class GameModel;
class CShape;

// 模型管理类
class CModelList
{
public:
	CModelList(void);
	virtual ~CModelList(void);

	// 模型结构
	struct tagModel
	{
		DWORD		lUseCount;		// 使用的次数
		GameModel	*pModel;		// 模型o		
	};

protected:
	struct tagModelKey
	{
		long		lType;			// 类型
		long		lGraphicsID;	// 图片编号
	};

	class hash_model_compare:public hash_compare<tagModelKey>
	{
	public:
		//计算散列值
		size_t operator( )( const tagModelKey& Key ) const
		{
			return Key.lType;
		}
		//less operator
		bool operator( )( const tagModelKey& _Key1,const tagModelKey& _Key2 ) const
		{
			if(_Key1.lType < _Key2.lType)	return true;
			else if(_Key1.lType == _Key2.lType && _Key1.lGraphicsID < _Key2.lGraphicsID)	return true;
			return false;
		}
	};

	typedef hash_map<tagModelKey,tagModel*,hash_model_compare>	HMModels;
	typedef hash_map<tagModelKey,tagModel*,hash_model_compare>::iterator itModel;

	HMModels	m_Models;	 				// 模型列表

public:
	tagModel* Load(long lType, long lGraphicsID, bool bFind = true);		// 载入一套资源( 在载入之前是否要查找要求载入的图像是否存在 )
	BOOL Release(long lType, long lGraphicsID);		// 释放一套资源
	BOOL ReleaseAll(void);							// 釋放所有資源

	// 显示
	void Display(CShape* pShape, float x, float y,float fHeight,render::Camera * pCamera);
	
	//Add by 彭谦
	void DisplaySpecialEquipment(CShape* pShape,render::Camera * pCamera);
	void UpdateSpecialEquipment(CShape* pShape);
	//Add by 彭谦
	
	void DisplayAsShadow(CShape *pShape, float x, float y,float fHeight,const D3DXMATRIX *pBillboardMatrix,const D3DXMATRIX * pBillboardMatrixY,DWORD dwColor);

	// 获取
	tagModel* GetModel(long lType, long lGraphicsID);	// 获取

	// 处理模型
	BOOL SetupAnimInfo(GameModel* pGameModel, CShape* pShape, float x, float y,float fHeight,const D3DXMATRIX *pBillboardMatrix,const D3DXMATRIX * pBillboardMatrixY);

	// 获取定位点坐标
	BOOL GetLocatorPos(CShape* pShape, DWORD dwLocatorName, const D3DXMATRIX *pBillboardMatrix,const D3DXMATRIX * pBillboardMatrixY, float &x, float &y, float &fHeight);

	// 显示GOODS、SKILL等图标
	void DisplayIcon(long lType, long lGraphicsID, int iLevel,int iDrawX, int iDrawY,float fRotation, bool bAction,GameModel::AnimInfo *pAnimInfo,float fScale = 100.0f);
	void DisplayIcon(CShape *pShape,int iDrawX, int iDrawY, float fRotation);

	void SetupEquipment(GameModel* pGameModel, CShape * pShape);

	BOOL IsFinished(CShape* pShape);		// 是否显示完成一个循环
private:
	void SetLevel(GameModel* pGameModel,DWORD dwGroup, DWORD dwPart, int iLevel);
};

