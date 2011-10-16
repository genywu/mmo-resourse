#include "stdafx.h"
#include "playertracker.h"
#include "ClientRegion.h"
#include "Player.h"
//extern Vector3 d;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

float CPlayerTracker::s_fDistance = 0.f;

CPlayerTracker::CPlayerTracker(void)
{
	m_iHoriAngle = 135;
	m_iSourceHoriAngle = 135;

	//垂直视角
	m_iMinVertAngle = 5;
	m_iMaxVertAngle = 85;
	m_iVertAngle    = 43;
	m_bsmooth = TRUE;
	m_brepos = FALSE;
#ifndef _DEBUG
	//观察距离
	m_fMinDistance = 13.f;
	m_fMaxDistance = 22.f;
	m_fDistance = 17.f;
#else
	m_fMinDistance = 1.0f;
	m_fMaxDistance = 45.0f;
	m_fDistance = 17.0f;
#endif

	m_nScorll = 0;

	m_bKeyboardAble = true;
	m_bMouseWheelAble = true;

	m_bShocked = FALSE;
}

CPlayerTracker::~CPlayerTracker(void)
{
}

void CPlayerTracker::Run(void)
{
	//控制
	CMouse *pMouse = GetMouse();
	CKeyBoard *pKeyboard = GetKeyboard();

	//鼠标到左右边缘，左右旋转
	POINT ptMouse;
	ptMouse.x = pMouse->GetMouseX();
	ptMouse.y = pMouse->GetMouseY();

	//int iWidth = pRender->GetScreenWidth();

	// 键盘控制转方向
	if (m_bKeyboardAble)
	{
		if (m_bHRotataion)
		{
			if (pKeyboard->IsKeyDown(DIK_LEFT) && pKeyboard->IsKeyDown(DIK_LCONTROL))
				//|| GetGame()->GetSetup()->lControl == 1 && pMouse->GetMouseX()<5 
				//&& pMouse->GetMouseY()>0 && pMouse->GetMouseY()<SCREEN_HEIGHT)
			{
				m_nScorll=-1;
			}

			if (m_nScorll<0)
			{
				//left
				m_iHoriAngle -= 3;
				m_iHoriAngle = m_iHoriAngle % 360;

				if (abs(m_iHoriAngle%10)<=1)
				{
					m_nScorll=0;
				}
			}

			if (pKeyboard->IsKeyDown(DIK_RIGHT) && pKeyboard->IsKeyDown(DIK_LCONTROL))
				//||	GetGame()->GetSetup()->lControl == 1 && pMouse->GetMouseX()>SCREEN_WIDTH-5
				//&&	pMouse->GetMouseY()>0 && pMouse->GetMouseY()<SCREEN_HEIGHT)
			{
				m_nScorll=1;
			}

			if (m_nScorll>0)
			{
				//right
				m_iHoriAngle +=3;
				m_iHoriAngle = m_iHoriAngle % 360;

				if (abs(m_iHoriAngle%45)<=1)
				{
					m_nScorll=0;
				}
			}

			if (pKeyboard->IsKeyDown(DIK_NEXT) && pKeyboard->IsKeyDown(DIK_LCONTROL))
			{
				m_iVertAngle ++;
				if (m_iVertAngle > m_iMaxVertAngle)
					m_iVertAngle = m_iMaxVertAngle;
			}

			if (pKeyboard->IsKeyDown(DIK_PRIOR) && pKeyboard->IsKeyDown(DIK_LCONTROL))
			{
				m_iVertAngle --;
				if (m_iVertAngle < m_iMinVertAngle)
					m_iVertAngle = m_iMinVertAngle;
			}
		}

		////键盘放缩
		if (pKeyboard->IsKeyDown(DIK_SUBTRACT) || pKeyboard->IsKeyDown(DIK_UP))
		{
			s_fDistance -= 0.5f;
			if (s_fDistance > m_fMaxDistance) s_fDistance = m_fMaxDistance;
			else
				if (s_fDistance < m_fMinDistance) s_fDistance = m_fMinDistance;
		}

		if (pKeyboard->IsKeyDown(DIK_ADD) || pKeyboard->IsKeyDown(DIK_DOWN))
		{
			s_fDistance += 0.5f;
			if (s_fDistance > m_fMaxDistance) s_fDistance = m_fMaxDistance;
			else
				if (s_fDistance < m_fMinDistance) s_fDistance = m_fMinDistance;
		}
	}

	//滚轮
	if (m_bMouseWheelAble && pMouse->GetCurrentButton() == MB_WHEEL)
	{
		s_fDistance -= (float)((short)HIWORD(pMouse->GetWheelDelta())) * 0.01f;
		if (s_fDistance > m_fMaxDistance) s_fDistance = m_fMaxDistance;
		else
			if (s_fDistance < m_fMinDistance) s_fDistance = m_fMinDistance;

		float fzDelta = (float)((short)HIWORD(pMouse->GetWheelDelta()));
		
		//	m_pCamera->MovePosition(&D3DXVECTOR3(0,0,fzDelta * 0.01f));
		m_bsmooth = false;
	}

	// 鼠标右键操作

	bool bRightUp = false;
	// 鼠标右键按下一次
	if (pMouse->GetCurrentButton() == RB_DOWN)
	{
		m_ptOldMouse.x = pMouse->GetMouseX();
		m_iOldHoriAngle = m_iHoriAngle;
	}

	// 鼠标右键弹起
	if (pMouse->GetCurrentButton() == RB_UP)
	{
		bRightUp = true;
	}

	// 鼠标右键按下不放
	if (pMouse->IsRightButtonDown() && !bRightUp)
	{
		if ( (pMouse->GetMouseX()-m_ptOldMouse.x)>1 ||  (pMouse->GetMouseX()-m_ptOldMouse.x)<-1 )
		{
			m_iHoriAngle = (int)( m_iOldHoriAngle + (pMouse->GetMouseX() - m_ptOldMouse.x)*0.5 ) % 360 ;
			
			m_brepos = FALSE;
		}
		
	}

	// 鼠标右键双击
	if (pMouse->GetCurrentButton() == RB_DCLICK)
	{
		m_brepos = TRUE;
	}
	//回到默认摄像机镜头角度
	if (m_brepos)
	{
		if (abs(m_iHoriAngle - m_iSourceHoriAngle) <= 5)
		{
			m_brepos = FALSE;
			m_iHoriAngle = m_iSourceHoriAngle;
		}
		else
		{
			if (m_iHoriAngle > 180)
			{
				m_iHoriAngle -= 360;
			}
			else if(m_iHoriAngle < -180)
			{
				m_iHoriAngle += 360;
			}
			if (m_iHoriAngle > -45 && m_iHoriAngle < 135)
			{
				m_iHoriAngle += 4;
			}
			else if (m_iHoriAngle <= -45 || m_iHoriAngle >= 135)
			{
				m_iHoriAngle -= 4;
			}
			
		}
		
	}

	

	


	if (s_fDistance>0.f)
		m_fDistance = s_fDistance;
}

void CPlayerTracker::Update(void)
{
	CClientRegion * c = GetGame()->GetRegion();
	if ( c != NULL /*&& GetGame()->GetSetup()->bSound*/ )
	{	 
		Vector3 * pos;
		Vector3 face;
		Vector3 right;
		Vector3 up;
		
		pos = c->GetCamera()->GetPosition();
		//pos->z = -pos->z;
		GetGame()->GetAudioList()->SetListenerPosition(*pos);
		
		//pos->z = -pos->z;
		face = (*c->GetCamera()->GetView()) - (*pos);
		
		Vector3 v = face;

		D3DXVec3Normalize(&face,&face);
		
		D3DXVec3Cross(&right,&Vector3(0,1,0),&face);
		D3DXVec3Normalize(&right,&right);
		D3DXVec3Cross(&up,&face,&right);

		//face.z = -face.z;
		//up.z = -up.z;
		GetGame()->GetAudioList()->SetListenerOritation(face,up);
	
		if (g_bDebug)
		{
			if (GetKeyboard()->IsKeyDown(DIK_TAB))
			{
				char t[255];
				sprintf(t,"距离平方:%f",(v.x)*(v.x)+v.y*v.y+(v.z)*(v.z));
				GetGame()->TextOut(140, 2*GetGame()->GetFontWidth()+20, t, 255, D3DCOLOR_ARGB(255,255,255,255));
			}
		}
	}
	
	if (m_bShocked)
	{
		static const float fOffsetArray[6] = { 1.0f,-0.6f,0.8f,-0.2f,0.4f,0.0f};
        //m_fShockRatio = fRatio;
		DWORD dwCurrentTime = timeGetTime();
		static int iOffPos = 0;
		DWORD dwTimeElapse = dwCurrentTime - m_dwShockStartTime;
		if (dwTimeElapse > m_dwShockDelayTime)
		{
			/*dwTimeElapse -= m_dwShockDelayTime;

			int iPos = dwTimeElapse * 6 / m_dwShockTimeLength;
			float fValue = fOffsetArray[iPos];
			if (iPos + 1 < 6)
			{	
				float fS = (float)dwTimeElapse / (float)m_dwShockTimeLength;
				fS = fS - (float)(int)fS;
				fValue = fOffsetArray[iPos] + fS * (fOffsetArray[iPos+ 1] - fOffsetArray[iPos]);
			}
			m_fShcokrOffset = fValue * m_fShockMaxRatio;*/
			m_fShcokrOffset = fOffsetArray[iOffPos] * m_fShockMaxRatio;
			if (dwTimeElapse > m_dwShockTimeLength)
			{
				iOffPos = 0;
				m_bShocked = FALSE;
			}

			if (iOffPos > 4)
				iOffPos = 0;
			else
				++iOffPos;
		}
	}
}

void CPlayerTracker::GetPosition(D3DXVECTOR3 &vPosition)
{

	//计算水平旋转方位
	float z = cosf(D3DXToRadian(m_iHoriAngle));
	float x = sinf(D3DXToRadian(m_iHoriAngle));

	float y = sinf(D3DXToRadian(m_iVertAngle)) * m_fDistance;
	float r = cosf(D3DXToRadian(m_iVertAngle)) * m_fDistance;

	vPosition = D3DXVECTOR3(x * r,y,z * r);
}

void CPlayerTracker::SetShock(float fMaxRatio,DWORD dwDelayTime,DWORD dwTimeLength)
{
	m_fShockMaxRatio = fMaxRatio;
	m_dwShockDelayTime = dwDelayTime;
	m_dwShockStartTime = timeGetTime();
	m_dwShockTimeLength = dwTimeLength;
	m_fShcokrOffset = 0.0f;
	m_bShocked = TRUE;
}

