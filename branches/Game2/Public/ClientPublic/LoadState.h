//=============================================================================
/**
 *  file: LoadState.h
 *
 *  Brief:用来表示资源的装载状态
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-11-12
 */
//=============================================================================

#ifndef __LOAD_STATE__
#define __LOAD_STATE__

//枚举资源装载的状态
enum eLoadState
{
	Load_Not,  //未装载
	Load_Doing,//正在装载
	Load_Did,  //装载完毕
};

class LoadState
{
protected:
	LoadState(void)
	:m_eLoad(Load_Not)
	{
	}
	virtual ~LoadState(void)
	{
	}
	//状态值
	eLoadState m_eLoad;	
public:
	void SetLoadState(eLoadState eState){m_eLoad = eState;}
	eLoadState GetLoadState()	{return m_eLoad;}
};

#endif
