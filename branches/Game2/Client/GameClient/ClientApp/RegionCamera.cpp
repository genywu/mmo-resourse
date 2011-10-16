#include "stdafx.h"
#include "RegionCamera.h"
#include "ClientRegion.h"
#include "Player.h"
#include "Other/AudioList.h"
#include "../../EngineExtend/GameMap.h"
#include "../GameClient/Game.h"
#include "../../Input/Mouse.h"
#include "../../Input/KeyBoard.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
    // 观察距离
    float CRegionCamera::s_fMinDistance = 2.0f;		//1.0f
    float CRegionCamera::s_fMaxDistance = 24.0f;	//45.0f
#else
    // 观察距离
    float CRegionCamera::s_fMinDistance = 5.0f;		//(13.0f)
    float CRegionCamera::s_fMaxDistance = 26.0f;	//(22.0f)
#endif

float CRegionCamera::s_fDistance = s_fMaxDistance;

/*
 * 功能: 构造函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.02
 * 修改日志:
 *	2008.07.17 - lpf
 * 		修改了Release版本编译时的镜头最近和最远距离;
 *	2008.10.17 - lpf
 * 		修改了m_iMinVertAngle为20，原来为5;
 *	2008.10.21 - lpf
 * 		修改了Release版本:m_fMaxDistance由30改为24;
 *		m_iMinVertAngle由20改为35;
 *		m_iMaxVertAngle由85改为80;
 *	2008.10.23 - lpf
 * 		修改了Release版本:
 *		m_fMaxDistance由24改为26;
 *		m_iMinVertAngle由35改为30;
 */
CRegionCamera::CRegionCamera(void)
{
	// 相机初始化状态
	m_dwState = 0;
	m_dwState |= SCS_REPOS;
	m_dwState |= SCS_KEYBOARD_ABLE;
	m_dwState |= SCS_MOUSE_WHEEL_ABLE;

	// 相机移动相关
	m_vMoveSpeed.x = 0.0f;
	m_vMoveSpeed.y = 0.0f;
	m_vMoveSpeed.z = 0.0f;
	m_vMoveDis.x   = 0.0f;
	m_vMoveDis.y   = 0.0f;
	m_vMoveDis.z   = 0.0f;
	m_fMoveDis	   = 0.0f;

	// 相机旋转相关
	m_iHoriAngle	= 135;
	m_fHCurAngle	= 0.0f;
	m_iHDesAngle	= 0;
	m_fHRotateSpeed = 0.0f;

	// 相机垂直视角相关
    m_iMinVertAngle = 30;
    m_iMaxVertAngle = 80;
    m_iVertAngle    = 43;
	m_fVRotateSpeed = 0.0f;
	m_fVCurAngle	= 0.0f;
	m_iVDesAngle	= 0;

	m_nScorll = 0;

	// 相机缩放相关
	m_fZoomSpeed  = 0.0f;
	m_fZoomDesDis = 0.0f;

	// 相机震动相关
	m_fShockMaxRatio	= 0.0f;
	m_fShcokrOffset		= 0.0f;
	m_dwShockStartTime	= 0;
	m_dwShockDelayTime	= 0;
	m_dwShockTimeLength = 0;

	// 以下是用于存储玩家手动调整相机的参数
	m_iOldHoriAngle	   = 0;
	m_iSourceHoriAngle = 135;
	m_iOldVertAngle	   = 5;
	m_iSourceVertAngle = 43;
	m_ptOldMouse.x	   = 0;
	m_ptOldMouse.y	   = 0;
	m_fMinVAngle       = 40.0f;

	m_fDistance = s_fMaxDistance;
}



/*
 * 功能: 析构函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.02
 */
CRegionCamera::~CRegionCamera(void)
{
}



/*
 * 功能: 场景相机玩家操作处理
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.02
 * 修改日志:
 *	2008.07.09- lpf
 *		增加了对摄象机旋转角度的锁定,主要针对玩家通过鼠标键盘控制摄象机时;
 *	2008.07.23- lpf
 *		恢复相机操作可以被键盘和鼠标中键的视距缩放操作中断;
 *	2008.07.30- lpf
 *		鼠标右键双击恢复视角的点击事件时,不恢复视距;
 *	2008.10.17 - lpf
 * 		增加了游戏垂直视角控制由鼠标右键按下上下拖动;
 *	2008.12.04 - lpf
 * 		增加了如果应用程序失去焦点,则清空鼠标信息的处理;
 *	2008.12.16 - lpf
 * 		取消了使用键盘水平旋转视角的过度处理.在收到键盘旋转消息时,直接改变旋转角度,然后就立即改变m_nScorll变量值为0,以停止旋转;
 */
void CRegionCamera::Run(void)
{
	// 输入设备对象
	CMouse	  * pMouse	  = GetMouse();
	CKeyBoard * pKeyboard = GetKeyboard();

	// 鼠标到左右边缘，左右旋转
	POINT ptMouse;
	ptMouse.x = pMouse->GetMouseX();
	ptMouse.y = pMouse->GetMouseY();

	// 键盘控制转方向
	if (m_dwState & SCS_KEYBOARD_ABLE)
	{
		if (m_dwState & SCS_ROTATE_H_ABLE && !(m_dwState & SCS_LOCK_ROTATE))
		{
			if (pKeyboard->IsKeyDown(DIK_LEFT) && pKeyboard->IsKeyDown(DIK_LCONTROL))
			{
				//|| GetGame()->GetSetup()->lControl == 1 && pMouse->GetMouseX()<5 
				//&& pMouse->GetMouseY()>0 && pMouse->GetMouseY()<SCREEN_HEIGHT)
				m_nScorll =- 1;
				ResumeCamera();
			}

			if (m_nScorll < 0)
			{
				// left
				m_iHoriAngle -= 3;
				m_iHoriAngle = m_iHoriAngle % 360;

//				if (abs(m_iHoriAngle%10) <= 1)
					m_nScorll=0;
			}

			if (pKeyboard->IsKeyDown(DIK_RIGHT) && pKeyboard->IsKeyDown(DIK_LCONTROL))
			{
				//||	GetGame()->GetSetup()->lControl == 1 && pMouse->GetMouseX()>SCREEN_WIDTH-5
				//&&	pMouse->GetMouseY()>0 && pMouse->GetMouseY()<SCREEN_HEIGHT)
				m_nScorll = 1;
				ResumeCamera();
			}

			if (m_nScorll > 0)
			{
				// right
				m_iHoriAngle += 3;
				m_iHoriAngle = m_iHoriAngle % 360;

				//if (abs(m_iHoriAngle%45) <= 1)
					m_nScorll = 0;
			}

			if (pKeyboard->IsKeyDown(DIK_NEXT) && pKeyboard->IsKeyDown(DIK_LCONTROL))
			{
				++m_iVertAngle;
				if (m_iVertAngle > m_iMaxVertAngle)
					m_iVertAngle = m_iMaxVertAngle;

				ResumeCamera();
			}

			if (pKeyboard->IsKeyDown(DIK_PRIOR) && pKeyboard->IsKeyDown(DIK_LCONTROL))
			{
				--m_iVertAngle;
				if (m_iVertAngle < m_iMinVertAngle)
					m_iVertAngle = m_iMinVertAngle;

				ResumeCamera();
			}
		}

		// 有输入框时无效
		//IGUIRoot *pRoot = IGUIRoot::GetGUIInstance();
		//if(!pRoot->GetFocusComp())
		//{
		//	// 键盘放缩
		//	if (pKeyboard->IsKeyDown(DIK_SUBTRACT) || pKeyboard->IsKeyDown(DIK_UP))
		//	{
		//		s_fDistance -= 0.5f;

		//		if (s_fDistance > s_fMaxDistance)
		//			s_fDistance = s_fMaxDistance;
		//		else if (s_fDistance < s_fMinDistance)
		//			s_fDistance = s_fMinDistance;

		//		ResumeCamera();
		//		RemoveCameraState();
		//		m_dwState &= ~SCS_SMOOTH;
		//	}

		//	if (pKeyboard->IsKeyDown(DIK_ADD) || pKeyboard->IsKeyDown(DIK_DOWN))
		//	{
		//		s_fDistance += 0.5f;
		//		if (s_fDistance > s_fMaxDistance)
		//			s_fDistance = s_fMaxDistance;
		//		else if (s_fDistance < s_fMinDistance)
		//			s_fDistance = s_fMinDistance;

		//		ResumeCamera();
		//		RemoveCameraState();
		//		m_dwState &= ~SCS_SMOOTH;
		//	}
		//}
	}

	// 滚轮
	if ((m_dwState & SCS_MOUSE_WHEEL_ABLE) && pMouse->GetCurrentButton() == MB_WHEEL)
	{
		s_fDistance -= (float)((short)HIWORD(pMouse->GetWheelDelta())) * 0.01f;
		if (s_fDistance > s_fMaxDistance)
		{
			s_fDistance = s_fMaxDistance;	
		}
		else if (s_fDistance < s_fMinDistance)
		{
			s_fDistance = s_fMinDistance;
		}
		float fzDelta = (float)((short)HIWORD(pMouse->GetWheelDelta()));
		if (GetGame()->GetMainPlayer()->GetAction() != CShape::ACT_WALK&&GetGame()->GetMainPlayer()->GetAction() != CShape::ACT_RUN)
		{
			m_dwState &= ~SCS_SMOOTH;
		}
		
		ResumeCamera();
		RemoveCameraState();
	}

	extern bool g_bIsFocus;
	if (!g_bIsFocus)
		pMouse->ClearMessage();

	// 鼠标右键操作
	bool bRightUp = false;

	// 鼠标右键按下一次
	if (pMouse->GetCurrentButton() == RB_DOWN)
	{
		m_ptOldMouse.x  = pMouse->GetMouseX();
		m_ptOldMouse.y  = pMouse->GetMouseY();
		m_iOldHoriAngle = m_iHoriAngle;
		m_iOldVertAngle = m_iVertAngle;
		ResumeCamera();
	}

	// 鼠标右键弹起
	if (pMouse->GetCurrentButton() == RB_UP)
		bRightUp = true;

	// 鼠标右键按下不放
	if (pMouse->IsRightButtonDown() && !bRightUp)
	{
		if ((pMouse->GetMouseX() - m_ptOldMouse.x) > 1 || (pMouse->GetMouseX() - m_ptOldMouse.x) < -1)
		{
			if (!(m_dwState & SCS_LOCK_ROTATE))
			{
				m_iHoriAngle = (int)( m_iOldHoriAngle + (pMouse->GetMouseX() - m_ptOldMouse.x)*0.5 ) % 360;
				RemoveCameraState();
			}
			/*m_iVertAngle = (int)( m_iOldVertAngle + (pMouse->GetMouseX() - m_ptOldMouse.x)*0.5 ) % 360;
			m_dwState	 &= ~SCS_REPOS_V;*/
		}

		m_iVertAngle += (int)(((float)pMouse->GetMouseMoveY() * 0.5f));
		if (m_iVertAngle < m_iMinVertAngle)
			m_iVertAngle = m_iMinVertAngle;
		else if (m_iVertAngle > m_iMaxVertAngle)
			m_iVertAngle = m_iMaxVertAngle;

		ResumeCamera();
	}

	//IGUIRoot *pRoot = IGUIRoot::GetGUIInstance();
	//if(pRoot && pRoot->GetGUIComp())
	//{
	//	CComponent * pComp = pRoot->GetGUIComp()->GetComponentAt(pMouse->GetMouseX(),pMouse->GetMouseY());
	//	if(pComp && pComp->IsEnabled() && pComp->IsVisible() && 
	//		strcmp(pComp->GetName(),"ChatInfo")!=0 && strcmp(pComp->GetName(),"TeamPage")!=0 
	//		&& strcmp(pComp->GetName(),"MedalTipPage")!=0)
	//	{
	//		return;
	//	}
	//	// 鼠标右键双击
	//	if (pMouse->GetCurrentButton() == RB_DCLICK && !(m_dwState & SCS_LOCK_ROTATE))
	//	{
	//		m_dwState |= SCS_REPOS_H;
	//		m_dwState |= SCS_REPOS_V;
	//		//m_dwState |= SCS_REPOS_D;
	//	}
	//}
}



/*
 * 功能: 场景相机状态自动更新
 * 摘要: 该函数功能主要实现,根据自身的当前状态自动更新相机内部数据,无须外部干预
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.02
 * 修改日志:
 *	2008.03.08 - lpf
 *		修正了相机在执行水平旋转和垂直旋转,并到达目的位置时,会不停的震动问题
 *	2008.10.17 - lpf
 * 		暂时屏蔽了"处理垂直角度与距离的限制";
 */
void CRegionCamera::Update(void)
{
	CClientRegion * c = GetGame()->GetRegion();
	if (c == NULL) /*&& GetGame()->GetSetup()->bSound*/ 
		return;

	// 处理场景3D音效
	D3DXVECTOR3 * pos;
	D3DXVECTOR3	  face;
	D3DXVECTOR3   right;
	D3DXVECTOR3   up;

	pos = c->GetCamera()->GetPosition();

	//pos->z = -pos->z;
	GetGame()->GetAudioList()->SetListenerPosition(*pos);

	//pos->z = -pos->z;
	face = (*c->GetCamera()->GetView()) - (*pos);

	Vector3 v = face;
	D3DXVec3Normalize(&face, &face);
	D3DXVec3Cross(&right, &Vector3(0, 1, 0), &face);
	D3DXVec3Normalize(&right, &right);
	D3DXVec3Cross(&up, &face, &right);

	//face.z = -face.z;
	//up.z   = -up.z;
	GetGame()->GetAudioList()->SetListenerOritation(face, up);

	if (g_bDebug)
	{
		if (GetKeyboard()->IsKeyDown(DIK_TAB))
		{
			char t[255];
			sprintf(t, "距离平方:%f", (v.x) * (v.x) + v.y * v.y + (v.z) * (v.z));
			GetGame()->OutPutText(140, 40, t, 255, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}

	// 回到默认摄像机镜头角度H
	if (m_dwState & SCS_REPOS_H)
	{
		if (abs(m_iHoriAngle - m_iSourceHoriAngle) <= 5)
		{
			m_dwState	 &= ~SCS_REPOS_H;
			m_iHoriAngle = m_iSourceHoriAngle;
		}else
		{
			if (m_iHoriAngle > 180)
				m_iHoriAngle -= 360;
			else if(m_iHoriAngle < -180)
				m_iHoriAngle += 360;
			if (m_iHoriAngle > -45 && m_iHoriAngle < 135)
				m_iHoriAngle += 4;
			else if (m_iHoriAngle <= -45 || m_iHoriAngle >= 135)
				m_iHoriAngle -= 4;
		}
	}
	// 回到默认摄像机镜头角度V
	if (m_dwState & SCS_REPOS_V)
	{
		if (abs(m_iVertAngle - m_iSourceVertAngle) <= 5)
		{
			m_dwState	 &= ~SCS_REPOS_V;
			m_iVertAngle = m_iSourceVertAngle;
		}else
		{
			m_iVertAngle += (m_iSourceVertAngle - m_iVertAngle)/abs(m_iSourceVertAngle - m_iVertAngle) * 4;
		}
	}
	// 回到默认摄像机镜头角度V
	if (m_dwState & SCS_REPOS_D)
	{
		if (abs(s_fDistance - 17.0f) <= 1.0f)
		{
			m_dwState	 &= ~SCS_REPOS_D;
			s_fDistance = 17.0f;
		}else
		{
			s_fDistance += (17.0f - s_fDistance)/abs(17.0f - s_fDistance) * 0.5f;
		}
	}

	// 处理相机移动
	if (m_dwState & SCS_MOVE)
	{
		if (D3DXVec3Length(&m_vMoveDis) < m_fMoveDis)
			m_vMoveDis += m_vMoveSpeed;
		//else
		//	m_dwState &= ~SCS_MOVE;
	}

	// 处理相机水平旋转
	if (m_dwState & SCS_ROTATE_H)
	{
		if (m_iHoriAngle > m_iHDesAngle)
		{
			m_fHCurAngle -= m_fHRotateSpeed;
			m_iHoriAngle = (int)m_fHCurAngle;

			if (m_iHoriAngle < m_iHDesAngle)
				m_iHoriAngle = m_iHDesAngle;
		}else if (m_iHoriAngle < m_iHDesAngle)
		{
			m_fHCurAngle += m_fHRotateSpeed;
			m_iHoriAngle = (int)m_fHCurAngle;

			if (m_iHoriAngle > m_iHDesAngle)
				m_iHoriAngle = m_iHDesAngle;
		}else
			m_dwState &= ~SCS_ROTATE_H;
	}

	// 处理相机垂直旋转
	if (m_dwState & SCS_ROTATE_V)
	{
		if (m_iVertAngle > m_iVDesAngle)
		{
			m_fVCurAngle -= m_fVRotateSpeed;
			m_iVertAngle = (int)m_fVCurAngle;

			if (m_iVertAngle < m_iVDesAngle)
				m_iVertAngle = m_iVDesAngle;
		}else if (m_iVertAngle < m_iVDesAngle)
		{
			m_fVCurAngle += m_fVRotateSpeed;
			m_iVertAngle = (int)m_fVCurAngle;

			if (m_iVertAngle > m_iVDesAngle)
				m_iVertAngle = m_iVDesAngle;
		}else
			m_dwState &= ~SCS_ROTATE_V;
	}

	// 处理相机缩放
	if (m_dwState & SCS_ZOOM)
	{
		if (m_fZoomSpeed > 0.0f)
		{
			if (s_fDistance < m_fZoomDesDis)
				s_fDistance += m_fZoomSpeed;
			else
				m_dwState &= ~SCS_ZOOM;
		}else if (m_fZoomSpeed < 0.0f)
		{
			if (s_fDistance > m_fZoomDesDis)
				s_fDistance += m_fZoomSpeed;
			else
				m_dwState &= ~SCS_ZOOM;
		}

		if (s_fDistance > s_fMaxDistance)
		{
			s_fDistance = s_fMaxDistance;
			m_dwState	&= ~SCS_ZOOM;
		}else if (s_fDistance < s_fMinDistance)
		{
			s_fDistance = s_fMinDistance;
			m_dwState	&= ~SCS_ZOOM;
		}
	}

	// 处理相机震动
	if (m_dwState & SCS_SHOCK)
	{
		static const float fOffsetArray[6] = { 1.0f, -0.6f, 0.8f, -0.2f, 0.4f, 0.0f };
		static int		   iOffPos		   = 0;
		
		DWORD dwCurrentTime = timeGetTime();
		DWORD dwTimeElapse  = dwCurrentTime - m_dwShockStartTime;
		if (dwTimeElapse > m_dwShockDelayTime)
		{
			m_fShcokrOffset = fOffsetArray[iOffPos] * m_fShockMaxRatio;
			if (dwTimeElapse > m_dwShockTimeLength)
			{
				iOffPos	  = 0;
				m_dwState &= ~SCS_SHOCK;
			}

			if (iOffPos > 4)
				iOffPos = 0;
			else
				++iOffPos;
		}
	}

	// 处理垂直角度与距离的限制
	//float angle = s_fDistance/17.0f * 43.0f;
	//m_fMinVAngle = angle > 43.0f?43.0f:angle;
	//if (m_iVertAngle < m_fMinVAngle)
	//{
	//	m_iVertAngle = m_fMinVAngle;
	//	m_fVCurAngle = m_iVertAngle;
	//}

	if (s_fDistance > 0.0f)
		m_fDistance = s_fDistance;
}



/*
 * 功能: 得到相机局部坐标
 * 摘要: 该函数返回的坐标仅仅是依据相机旋转角度和视距计算而来的局部坐标,不包含位移信息
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.02
 */
void CRegionCamera::GetPosition(D3DXVECTOR3 &vPosition)
{
	float x, y, z, r;

	// 计算水平旋转方位
	if (m_dwState & SCS_ROTATE_H)
	{
		z = cosf(D3DXToRadian(m_fHCurAngle));
		x = sinf(D3DXToRadian(m_fHCurAngle));
	}else
	{
		z = cosf(D3DXToRadian(m_iHoriAngle));
		x = sinf(D3DXToRadian(m_iHoriAngle));
	}

	// 计算垂直旋转方位
	if (m_dwState & SCS_ROTATE_V)
	{
		y = sinf(D3DXToRadian(m_fVCurAngle)) * m_fDistance;
		r = cosf(D3DXToRadian(m_fVCurAngle)) * m_fDistance;
	}else
	{
		y = sinf(D3DXToRadian(m_iVertAngle)) * m_fDistance;
		r = cosf(D3DXToRadian(m_iVertAngle)) * m_fDistance;
	}

	vPosition = D3DXVECTOR3(x * r, y, z * r);
}



/*
 * 功能: 震动相机
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.02
 */
void CRegionCamera::ShockCamera(float fMaxRatio, DWORD dwDelayTime, DWORD dwTimeLength)
{
	m_fShockMaxRatio	= fMaxRatio;
	m_dwShockDelayTime	= dwDelayTime;
	m_dwShockStartTime	= timeGetTime();
	m_dwShockTimeLength = dwTimeLength;
	m_fShcokrOffset		= 0.0f;
	m_dwState			|= SCS_SHOCK;
}



/*
 * 功能: 平滑移动相机到目标点
 * 摘要: 该函数仅调用一次,传入足够的信息,即自动改变摄象机状态
 * 参数: fSpeed – 移动速度(每帧移动的绝对距离)
 *		 fDesX  – 目标点X坐标
 *		 fDesY   - 目标点Y坐标
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.01
 */
void CRegionCamera::MoveCamera(float fSpeed, float fDesX, float fDesY)
{
	CClientRegion * pRegion = GetGame()->GetRegion();
	D3DXVECTOR3		vView	= *(pRegion->GetCamera()->GetView());
	D3DXVECTOR3		vDesView(fDesY, pRegion->GetGameMap()->CalculateHeight(fDesY, fDesX) - GetCameraPosModify(), fDesX);

	m_vMoveSpeed = vDesView - vView;
	m_fMoveDis	 = D3DXVec3Length(&m_vMoveSpeed);
	D3DXVec3Normalize(&m_vMoveSpeed, &m_vMoveSpeed);

	m_vMoveSpeed *= fSpeed;
	m_vMoveDis.x = 0.0f;
	m_vMoveDis.y = 0.0f;
	m_vMoveDis.z = 0.0f;
	m_dwState	 |= SCS_MOVE;
	m_dwState	 &= ~SCS_REPOS;
}



/*
 * 功能: 水平旋转相机
 * 摘要: 该函数仅调用一次,传入足够的信息,即自动改变摄象机状态
 * 参数: fSpeed    – 旋转角速度
 *		 iDesAngle – 目的角度
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.01
 */
void CRegionCamera::RotateCameraH(float fSpeed, int iDesAngle)
{
	m_fHRotateSpeed	= fSpeed;
	m_fHCurAngle	= (float)m_iHoriAngle;
	m_iHDesAngle	= iDesAngle;
	m_dwState		|= SCS_ROTATE_H;
	m_dwState		&= ~SCS_REPOS;
}



/*
 * 功能: 垂直旋转相机
 * 摘要: 该函数仅调用一次,传入足够的信息,即自动改变摄象机状态
 * 参数: fSpeed    – 旋转角速度
 *		 iDesAngle – 目的角度
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.01
 */
void CRegionCamera::RotateCameraV(float fSpeed, int iDesAngle)
{
	if (iDesAngle > m_iMaxVertAngle)
		iDesAngle = m_iMaxVertAngle;
	if (iDesAngle < m_iMinVertAngle)
		iDesAngle = m_iMinVertAngle;

	m_fVRotateSpeed	= fSpeed;
	m_fVCurAngle	= (float)m_iVertAngle;
	m_iVDesAngle	= iDesAngle;
	m_dwState		|= SCS_ROTATE_V;
	m_dwState		&= ~SCS_REPOS;
}



/*
 * 功能: 缩放相机到注视点的距离
 * 摘要: 该函数仅调用一次,传入足够的信息,即自动改变摄象机状态
 * 参数: fSpeed  – 旋转角速度
 *		 fDesDis – 目的距离
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.01
 */
void CRegionCamera::ZoomCamera(float fSpeed, float fDesDis)
{
	if (m_fDistance < m_fZoomDesDis)
		m_fZoomSpeed  = abs(fSpeed);
	else
		m_fZoomSpeed  = abs(fSpeed) * -1.0f;

	m_fZoomDesDis = fDesDis;
	m_dwState	  |= SCS_ZOOM;
	m_dwState	  &= ~SCS_REPOS;
}



/*
 * 功能: 恢复相机设置到玩家进入场景时状态
 * 摘要: 该函数仅调用一次,传入足够的信息,即自动改变摄象机状态
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.01
 */
void CRegionCamera::ResumeCamera(void)
{
	if (m_dwState & SCS_REPOS)
		return;

	// 祛除相机的某些状态
	m_dwState &= ~SCS_ROTATE_H;		//相机水平旋转
	m_dwState &= ~SCS_ROTATE_V;		//相机垂直旋转
	m_dwState &= ~SCS_ZOOM;			//相机视距缩放
	m_dwState &= ~SCS_MOVE;			//相机平移

	m_dwState |= SCS_REPOS;
	m_dwState &= ~SCS_SMOOTH;

	m_iHoriAngle = 135;
	m_iVertAngle = 43;
	s_fDistance  = 17.0f;
}



/*
 * 功能: 平滑场景相机
 * 摘要: -
 * 参数: vView	  - 相机的视点坐标
 *		 vMainPos - 主角的世界坐标
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.02
 */
void CRegionCamera::SmoothCamera(D3DXVECTOR3 & vView, D3DXVECTOR3 & vMainPos)
{
	
	static float	   cameraspeed = 0.0f;
	static float	   lasttime	   = (float)timeGetTime();
	static D3DXVECTOR3 lastviewpos(vView);
	static D3DXVECTOR3 lastmainpos(vMainPos);

	float nowtime	  = (float)timeGetTime();
	float deltatime   = (nowtime - lasttime) / 1000;
	
	float playerspeed = FM_Distance(vMainPos, lastmainpos);
	float mod		  = FM_Distance(vMainPos, vView);
	float viewdistance = FM_Distance(lastviewpos,vView);

	D3DXVECTOR3 dir = vMainPos - vView;
	D3DXVec3Normalize(&dir, &dir);
	if (mod > 6)
	{
		vView = vMainPos;
		return;
	}

	mod = mod * 0.08f;
	vView = vMainPos;

	
	lastviewpos			= vView;
	lasttime			= nowtime;
	lastmainpos			= vMainPos;
}



/*
 * 功能: 设定引擎相机
 * 摘要: -
 * 参数: pCamera - 需要设定的引擎相机指针
 *		 fX		 - 玩家处于世界的X坐标
 *		 fY		 - 玩家处于世界的Y坐标
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.02.02
 */
void CRegionCamera::SetCamera(render::Camera * pCamera, float fX, float fY)
{
	float x1(fY);
	float z1(fX);
	CClientRegion * pRegion = GetGame()->GetRegion();
	D3DXVECTOR3 vMainPlayerPos(x1, pRegion->GetGameMap()->CalculateHeight(x1, z1) + 0.5f - GetCameraPosModify(), z1);
	D3DXVECTOR3 vTPos;
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vViewTmp;

	GetPosition(vTPos);

	// 相机震动
	if (m_dwState & SCS_SHOCK)
		vMainPlayerPos.y += m_fShcokrOffset;

	static D3DXVECTOR3 vView(vMainPlayerPos);

	// 相机平滑
	if (m_dwState & SCS_SMOOTH)
		SmoothCamera(vView, vMainPlayerPos);
	else
	{
		vView	  = vMainPlayerPos;
		m_dwState |= SCS_SMOOTH;
	}

	vViewTmp = vView;

	// 相机移动
	if (m_dwState & SCS_MOVE)
	{
		vTPos    += m_vMoveDis;
		vViewTmp += m_vMoveDis;
	}

	// 设定相机
	vPos = vView + vTPos;
	pCamera->SetView(&vViewTmp);
	if (GetGame())
	{
		if (GetGame()->GetRegion())
		{
			if (GetGame()->GetRegion()->GetGameMap())
			{
				if (vPos.y - 0.5 < GetGame()->GetRegion()->GetGameMap()->CalculateHeight(vPos.x,vPos.z))
				{
					vPos.y = GetGame()->GetRegion()->GetGameMap()->CalculateHeight(vPos.x,vPos.z) + 2;
				}
			}
		}
	}
	pCamera->SetPosition(&vPos);
}



/*
 * 功能: 祛除相机当前状态信息
 * 摘要: 但不做数据恢复操作,保持当前状态
 *		 目前只针对摄象机恢复视角操作
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.07.23
 */
void CRegionCamera::RemoveCameraState()
{
	// 祛除相机的某些状态
	m_dwState &= ~SCS_REPOS_H;
	m_dwState &= ~SCS_REPOS_V;
	m_dwState &= ~SCS_REPOS_D;
}



//摄像机操作方式
void CRegionCamera::TransformCamer(long lOperaMode,long lParam1,long lParam2,long lParam3)
{
	switch(lOperaMode)
	{
		//震动
	case 1:
		ShockCamera((float)lParam1,lParam2,lParam3);
		break;
		//平滑移动
	case 2:
		MoveCamera((float)lParam1,(float)lParam2,(float)lParam3);
		break;
		//水平旋转
	case 3:
		RotateCameraH((float)lParam1,lParam2);
		break;
		//垂直旋转
	case 4:
		RotateCameraV((float)lParam1,lParam2);
		break;
		//距离变化
	case 5:
		ZoomCamera((float)lParam1,(float)lParam2);
		break;
		//恢复
	case 6:
		ResumeCamera();
		break;
	}
}