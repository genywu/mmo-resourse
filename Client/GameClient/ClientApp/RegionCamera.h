/* CRegionCamera.h
 * 功能: 将引擎的相机进行更高层次的封装，引擎相机需要的数据都在这个对象上获取。
 * 摘要: 该相机支持类似脚本函数操作，即调用一次操作函数，相机自动完成移动，缩放旋转等操作
 *		 不同类型的操作不会相互影响,可以同时执行
 *		 当一种类型的操作没有完成时,又在外部被调用了同样类型的操作时,相机会执行后一次操作
 * 作者: lpf
 * 创建日期: 2008-02-02
 */

#pragma once
#include "../../Engine/Render.h"
// 场景相机类
class CRegionCamera
{
	// 场景相机状态枚举
	enum eREGION_CAMERA_STATE
	{
		// 基本操作状态
		SCS_ROTATE_H = 0x00000001,		//相机水平旋转
		SCS_ROTATE_V = 0x00000002,		//相机垂直旋转
		SCS_ZOOM	 = 0x00000004,		//相机视距缩放
		SCS_MOVE	 = 0x00000008,		//相机平移

		// 相机坐标恢复状态
		SCS_REPOS	 = 0x00000010,		//相机当前状态是否是初始状态
		SCS_REPOS_H	 = 0x00000020,		//相机重设水平坐标(回复到初试状态)
		SCS_REPOS_V	 = 0x00000040,		//相机重设垂直坐标(回复到初试状态)
		SCS_REPOS_D	 = 0x00000080,		//相机重设距离(回复到初试状态)

		// 特效状态
		SCS_SHOCK	 = 0x00000100,		//相机震动
		SCS_SMOOTH	 = 0x00000200, 		//相机平滑

		// 其他状态
		SCS_ROTATE_H_ABLE	 = 0x00001000,	//是否可以水平旋转
		SCS_KEYBOARD_ABLE	 = 0x00002000,	//键盘是否有效
		SCS_MOUSE_WHEEL_ABLE = 0x00004000,	//鼠标滚轮是否有效
		SCS_LOCK_ROTATE		 = 0x00008000,	//是否锁定旋转(仅针对用户鼠标和键盘操作)
	};

	// 以下是用于存储相机自动调整的参数
	DWORD m_dwState;				//当前相机状态

	// 相机移动相关
	D3DXVECTOR3 m_vMoveSpeed;		//相机移动速度向量
	D3DXVECTOR3 m_vMoveDis;			//相机当前移动距离向量
	float		m_fMoveDis;			//相机需要移动的距离

	// 相机旋转相关
	int   m_iHoriAngle;				//水平视角
	float m_fHCurAngle;				//相机当前旋转角度
	int	  m_iHDesAngle;				//相机目的旋转角度
	float m_fHRotateSpeed;			//相机旋转角速度

	// 相机垂直视角相关
	int   m_iMinVertAngle;			//最小垂直角度
	int   m_iMaxVertAngle;			//最大垂直角度
	int   m_iVertAngle;				//当前垂直角度
	float m_fVRotateSpeed;			//相机垂直旋转角速度
	float m_fVCurAngle;				//相机当前角度
	int	  m_iVDesAngle;				//相机目的角度

	int m_nScorll;					//-1:向左转 0:不动 1:向右转

	// 相机缩放相关
	float m_fZoomSpeed;				//缩放速度
	float m_fZoomDesDis;			//缩放目的距离

	// 相机震动相关
	float m_fShockMaxRatio;			//震动幅度
	float m_fShcokrOffset;			//震动偏移
	DWORD m_dwShockStartTime;		//震动开始时间
	DWORD m_dwShockDelayTime;		//震动间隔
	DWORD m_dwShockTimeLength;		//震动时长

	// 以下是用于存储玩家手动调整相机的参数
	int   m_iOldHoriAngle;			//记录原度数
	int   m_iSourceHoriAngle;		//最初的度数
	int   m_iOldVertAngle;			//记录原度数
	int   m_iSourceVertAngle;		//最初的度数
	POINT m_ptOldMouse;				//鼠标右键按下,记录此坐标

	// 观察距离
	float m_fMinVAngle;				//当前视距的最小仰角
	float m_fDistance;				//玩家当前操作视距

	static float s_fMinDistance;	//玩家最小视距
	static float s_fMaxDistance;	//玩家最大视距
	static float s_fDistance;		//记录玩家的视角距离

public:
	CRegionCamera(void);
	~CRegionCamera(void);

	void Run(void);					//场景相机玩家操作处理
	void Update(void);				//场景相机状态自动更新

	// 数据访问函数
	bool  IsShock(void)								{ return (m_dwState & SCS_SHOCK) ? true : false; }								//是否震动
	bool  IsSmooth(void)							{ return (m_dwState & SCS_SMOOTH) ? true : false; }								//是否平滑
	float GetSockOffset(void)						{ return m_fShcokrOffset; }														//得到震动偏移
	float GetHoriAngle(void)						{ return (float)m_iHoriAngle * (3.1415926f / 180.0f); }							//得到水平旋转角度
	void  GetPosition(D3DXVECTOR3 & vPosition);																						//得到相机局部坐标
	float GetCameraPosModify(void)					{ return (m_fDistance - s_fMinDistance) / (s_fMaxDistance - s_fMinDistance); }	//得到相机修改后的坐标
	float GetDistance()								{ return s_fDistance; }															//得到玩家的视角距离
	void  SetMinVertAngle(int ag)					{ m_iMinVertAngle = ag; }
	void  SetMaxVertAngle(int ag)					{ m_iMaxVertAngle = ag; }

	// 数据设定函数
	void SetSmooth(bool b)							{ b ? m_dwState |= SCS_SMOOTH : m_dwState &= ~SCS_SMOOTH; }						//设定平滑
	void SetKeyboardAble(bool b)					{ b ? m_dwState |= SCS_KEYBOARD_ABLE : m_dwState &= ~SCS_KEYBOARD_ABLE; }		//设定是否接受键盘
	void SetMouseWheelAble(bool b)					{ b ? m_dwState |= SCS_MOUSE_WHEEL_ABLE : m_dwState &= ~SCS_MOUSE_WHEEL_ABLE; }	//设定是否接受鼠标转动
	void SetHRotation(bool b)  						{ b ? m_dwState |= SCS_ROTATE_H_ABLE : m_dwState &= ~SCS_ROTATE_H_ABLE; }		//设定是否水平旋转
	void SetLockRotation(bool b)					{ b ? m_dwState |= SCS_LOCK_ROTATE : m_dwState &= ~SCS_LOCK_ROTATE; }			//设定摄象机是否锁定旋转
	void SetCameraReposH(bool b)					{ b ? m_dwState |= SCS_REPOS_H : m_dwState &= ~SCS_REPOS_H; }					//设定相机是否恢复水平视角
	void SetCameraReposV(bool b)					{ b ? m_dwState |= SCS_REPOS_V : m_dwState &= ~SCS_REPOS_V; }					//设定相机是否恢复垂直视角
	void SetCameraReposD(bool b)					{ b ? m_dwState |= SCS_REPOS_D : m_dwState &= ~SCS_REPOS_D; }					//设定相机是否恢复距离视角
	void SetCameraHAngle(int  angle)				{ m_iVertAngle    = angle;}		//设置相机俯视角度
	// 相机自动操作函数
	void ShockCamera(float fMaxRatio, DWORD dwDelayTime, DWORD dwTimeLength);		//震动相机
	void MoveCamera(float fSpeed, float fDesX, float fDesY);						//平滑移动相机到目标点
	void RotateCameraH(float fSpeed, int iDesAngle);								//水平旋转相机
	void RotateCameraV(float fSpeed, int iDesAngle);								//垂直旋转相机
	void ZoomCamera(float fSpeed, float fDesDis);									//缩放相机到注视点的距离
	void ResumeCamera(void);														//恢复相机设置到玩家进入场景时状态

	// 辅助函数
	void SmoothCamera(D3DXVECTOR3 & vView, D3DXVECTOR3 & vMainPos);			//平滑场景相机
	void SetCamera(render::Camera * pCamera, float fX, float fY);			//设定引擎相机
	void RemoveCameraState();												//祛除相机当前状态信息

	//摄像机操作方式
	void TransformCamer(long lOperaMode,long lParam1,long lParam2,long lParam3);
};