#pragma once

class Sun
{
	BOOL			m_bDrag;
	D3DXQUATERNION  m_qOld,m_qNow;
	D3DXMATRIX		m_mRotSnapshot,m_mRot;
	D3DXVECTOR3		m_vOldVector,m_vCurVector;
	float			m_fRadius;
	D3DXVECTOR3     m_vDefDirection;
	D3DXVECTOR3		m_vCurDirection;

	GameModel *		m_pGameModel;
	GameModel::AnimInfo *m_pAnimInfo;
public:
	Sun(void);
	~Sun(void);

	void ScreenToVector( int iScreenPtX, int iScreenPtY ,int iScreenWidth,int iScreenHeight,float fRadius,D3DXVECTOR3 *pVector);
	D3DXQUATERNION QuatFromBallPoints(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo);
	void HandleMessage(const D3DXMATRIX *pViewMatrix);
	const D3DXVECTOR3 * GetLightDirection() const { return &m_vCurDirection;}

	void Render();
};