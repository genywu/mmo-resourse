/**
* @filename:GoodsModelManager.h
* @date: 2009/12/28
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 物品模型显示管理器，单件模式,将模型管理器的使用封装到此处了
*/

#pragma once

#include "../../../EngineExtend/DisplayModel.h"

class GoodsModelManager
{
	DECLARE_SINGLETON(GoodsModelManager);
public:
	GoodsModelManager();
	~GoodsModelManager();


	/// @brief
	/// 打开指定的模型文件
	/// @param szModefFileName
	/// 模型文件名默认为"model"
	/// @param IsFullPathName
	/// 标识是否是完整路径，默认为false
	/// @return
	/// 打开模型成功，返回true，失败返回false
	bool OpenModel(const char *szModelFileName="model",bool IsFullPathName=false);

	/// @brief
	/// 获取当前模型文件名
	const char* GetOpenModelFileName() const
	{
		return m_ModelFileName;
	}

	/// @brief
	/// 设置当前打开的模型文件
	/// @param szModeFileName
	/// 文件名
	void SetOpenModelFileName(const char* szModelFileName)
	{
		if( szModelFileName !=NULL )
		{
			strcpy(m_ModelFileName,szModelFileName);
		}
	}

	/// @brief
	/// 设置模型文件路径
	/// @param szPathName
	/// 默认为空
	void SetPathName(const char* szPathName="")
	{
		if(szPathName != NULL)
		{
			strcpy(m_PathName,szPathName);
		}
	}


	/// @brief
	/// 获取模型文件路径
	/// @return
	/// const char*
	const char* GetPathName() const
	{
		return m_PathName;
	}

	/// @brief
	/// 初始化摄像机
	/// @return
	///  成功返回true，失败返回false
	bool InitiliseCamera();


	/// @brief
	/// 重置摄像机位置和观察位置
	/// @param XPos
	/// X 的绝对坐标
	/// @param YPos
	/// Y 的绝对坐标,默认为0.5f
	/// @param ZPos
	/// Z 的绝对坐标，默认为-3.0f
	void ResetCamera(float XPos = 0.0f ,float YPos = 0.5f ,float ZPos = -2.5f);

	/// @brief
	/// 移动摄像机到相对位置
	/// @param relX
	/// X 的相对坐标
	/// @param relY
	/// Y 的相对坐标
	/// @param relZ
	/// Z 的相对坐标
	void MoveCameraRel(float relX = 0.0f,float relY = 0.0f,float relZ = 0.0f);

	/// @brief
	/// 获取摄像机位置
	/// @param
	/// 
	/// @return const D3DVECTOR3
	/// 
	const D3DXVECTOR3 GetCameraPosition() const { return *(m_Camera->GetPosition());} 
	/// @brief
	/// 更新物品模型管理的相关状态
	void Update();

	/// @brief
	/// 渲染模型

	bool RenderModel();


	/// @brief
	/// 释放模型资源
	void ReleaseModel();


	/// @brief
	/// 销毁模型管理器 
	void Destory();


	D3DMATRIX GetWorlMatrix() const { return m_matWorld; }


	D3DMATRIX GetViewMatrix() const { return m_matView; }


	D3DMATRIX GetProjectMatrix() const { return m_matProject; }


	/// @brief
	/// 重置模型位置和角度
	void ResetPosAndRot() { m_PosX = 0.0f; m_PosY = 0.0f ; m_PosH = 0.0f; m_Rot = 0.0f; }

	/// @brief
	/// 移动模型到绝对位置
	/// @param X
	/// @parma Y
	/// @parma H
	void MoveTo(float X,float Y,float H) { m_PosX = X ; m_PosY = Y ; m_PosH = H ;}


	/// @brief
	/// 移动模型到相对位置
	/// @param relX
	/// @param relY
	/// @param relH

	void MoveRel(float relX,float relY,float relH) { m_PosX += relX; m_PosY += relY; m_PosH += relH; }


	/// @brief
	/// 模型绕Y轴旋转
	/// @param angle
	/// 旋转角度
	void RotationY(float angle) { m_Rot += angle; }

	/// @brief
	/// 模型放缩
	/// @param matScal
	/// 放缩矩阵

	void Scale(D3DXMATRIX &matScal){ m_cAnimInfo.SetScalMatrix(&matScal);}

	/// @brief
	/// 以累积方式放缩模型
	/// @param relXScal
	/// X轴上的放缩值
	/// @param relYScal
	/// Y轴上的放缩值
	/// @param relZScal
	/// Z轴上的放缩值
	void ScaleCumulate(float relXScal,float relYScal,float resZScal)
	{
		m_ScaleX += relXScal;
		m_ScaleY += relYScal;
		m_ScaleZ += resZScal;
		D3DXMATRIX matScal;
		D3DXMatrixScaling(&matScal,m_ScaleX,m_ScaleY,m_ScaleZ);
		Scale(matScal);
	};


	/// @brief
	/// 重置累积的放缩值
	/// @param XScal
	/// 默认为1.0
	/// @param YScal
	/// 默认为1.0
	/// @param ZScal
	/// 默认为1.0
	/// @return
	/// 
	void ResetScaleCumulate(float XScal = 1.0f,float YScal = 1.0f,float ZScal = 1.0f)
	{
		m_ScaleX = XScal;
		m_ScaleY = YScal;
		m_ScaleZ = ZScal;
	}
protected:
private:

	/// @brief
	/// 设置坐标变换
	void SetupTransform(void);
	

	/// @brief
	/// 还原坐标变换
	void ResetTransform();

	/// @brief
	/// 设置视区
	/// @param in rect
	/// 视区的矩形
	void SetViewPort(const RECT& rect);

	/// @brief
	/// 设置视区大小
	/// @param xStart
	/// 视区X起始坐标
	/// @param yStart
	/// 视区Y起始坐标
	/// @param Width
	/// 视区的宽
	/// @param Height
	/// @视区的高
	void SetViewPort(int xStart,int yStart,int Width,int Height);

	//模型文件名称
	char m_ModelFileName[MAX_PATH];
	//模型文件路径
	char m_PathName[MAX_PATH];

	//用于模型显示
	CDisplayModel* m_DisplayModel;
	CGameModelManager* m_GameModelManager;	
	render::Camera* m_Camera;
	render::Frustum* m_Frustum;
	AnimInfo		m_cAnimInfo;///<模型动画信息

	D3DMATRIX		m_matWorld;///<用于保存修改之前的矩阵
	D3DMATRIX		m_matView;
	D3DMATRIX		m_matProject;

	float	m_PosX;	///<模型的X坐标
	float   m_PosY;	///<模型的Y坐标
	float   m_PosH;    ///<模型的H坐标（高度),就是屏幕的Y坐标
	float   m_Rot;		///<模型旋转角度

	
	float   m_ScaleX;  ///<累积的模型放缩值X
	float   m_ScaleY;  ///<累积的模型放缩值Y
	float   m_ScaleZ;  ///<累积的模型放缩值Z
	
	D3DVIEWPORT9 m_OldViewPort;	///<存储绘制模型前的视区;
};




