#pragma once

#include "fmodel.h"

class effector
{
	enum EFFECTORTYPE { EFF_UNKNOWN,EFF_COLORLERP,EFF_UVOFFSET,EFF_FLATSHADOW};
public:
	class Effector
	{
	protected:
		enum EFFECTORTYPE m_eType;

		DWORD m_dwCycleTime;//效果循环时间长度

		DWORD m_dwShaderGroupHandle;

	public:
		Effector(void);
		~Effector(void);

		DWORD   GetShaderGroupHandle() const { return m_dwShaderGroupHandle;}
		void    SetCylceTime(DWORD dwTime) { m_dwCycleTime = dwTime;}

		virtual BOOL Create(const char * pszFileName);
		virtual BOOL ProcessVertices(model::Buffer* pBuffer,DWORD dwStartTime,DWORD dwCurrentTime,float fTimeModifier);
		virtual void Destroy();
	};

	class ColorLerp: public Effector
	{
		D3DXCOLOR m_tColorStart;
		D3DXCOLOR m_tColorEnd;

		D3DXCOLOR * CalculateColorLerp(D3DXCOLOR *pOut,float fRatio, BOOL bLerpDirect);

	public:
		ColorLerp(void);
		~ColorLerp(void);

		void SetParameter(const D3DXCOLOR *pColorStart,const D3DXCOLOR *pColorEnd);

		BOOL ProcessVertices(model::Buffer* pBuffer,DWORD dwStartTime,DWORD dwCurrentTime,float fTimeModifier);

	};

	class UVOffset : public Effector
	{
		D3DXVECTOR2 m_vOffset;	

	public:
		UVOffset(void);
		~UVOffset(void);

		void SetParameter(const D3DXVECTOR2 *pOffset);
		virtual BOOL ProcessVertices(model::Buffer* pBuffer,DWORD dwStartTime,DWORD dwCurrentTime,float fTimeModifier);

	};

	class FlatShadow : public Effector
	{
		DWORD m_dwColor;//shadow color
		DWORD m_dwLightType;	//0 directional 1 point light
		D3DXVECTOR3 m_vLightVector;//0 in directional type vector is origin  1 in point light vector is position

	public:
		FlatShadow(void);
		~FlatShadow(void);

		void SetParameter(const D3DXVECTOR3 *pLightVector,DWORD dwLightType,D3DCOLOR dwColor);

		virtual BOOL ProcessVertices(model::Buffer* pBuffer,DWORD dwStartTime,DWORD dwCurrentTime,float fTimeModifier);
	};

};
