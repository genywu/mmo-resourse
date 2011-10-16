#pragma once

// 特效设置
class CEffectSetup
{
public:
	CEffectSetup(void);
	virtual ~CEffectSetup(void);
public:
	
	struct tagEffectSetup
	{
		DWORD dwEffectKey;		    // 编号
		BOOL  bEnable;				// 是否有效
		float fRadius;				// 范围半径
		DWORD dwColor;				// 颜色
		DWORD dwType;				// 类型  0 + 1 x
		DWORD dwFadeStartTime;
		DWORD dwFadeLength;
	};
private:
	static map<DWORD, tagEffectSetup>		m_mapEffectSetup;			
	
public:
	static BOOL LoadEffectSetup(const char* filename);
	static map<DWORD,tagEffectSetup>* GetEffectSetup(){return &m_mapEffectSetup;}
};
