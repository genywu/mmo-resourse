/* RollLantern.h
 * 功能: 跑马灯实现类
 * 摘要: 主要用于在游戏中进行系统信息滚动显示,有Alpha渐变功能
 * 作者: lpf
 * 创建日期: 2009-01-05
 */
class CBitmapX;
#include "../../Engine/Render.h"
#include "../../Engine/Utility.h"
#pragma once

// 透明渐变对象显示列表类
class CRollLantern
{
	// 走马灯透明渐变模式
	enum eALPHA_FADE_MODE
	{
		ALPHA_FADE_NONE = 0,			//没有渐变操作
		ALPHA_FADE_IN,					//渐进
		ALPHA_FADE_OUT,					//渐出
	};

	float	   m_fSpeed;				//内容滚动速度(像素/毫秒)
	DWORD	   m_dwNumLoop;				//循环次数
	POINT	   m_ptPos;					//走马灯绘制坐标(即背景图片绘制坐标)
	long	   m_lClipLeft;				//裁剪的左方坐标(该坐标在相对坐标系,相对于走马灯绘制坐标)
	long	   m_lClipRight;			//裁剪的右方坐标(该坐标在相对坐标系,相对于走马灯绘制坐标)
	long	   m_lContentOffsetHeight;	//显示内容高度偏移(该坐标在相对坐标系,相对于走马灯绘制坐标)
	string	   m_sContent;				//显示内容
	CBitmapX * m_pBackGroundPic;		//背景图片
	FONTINFO   m_stFontInfo;			//内容字体信息

	bool			 m_bIsFinished;			//走马灯已经显示完成
	long			 m_lContentOffset;		//内容绘制偏移
	long			 m_lContentOffsetLeft;	//内容滚动偏移最左值
	long			 m_lContentOffsetRight;	//内容滚动偏移最右值
	DWORD			 m_dwStartTime;			//走马灯开始时间(毫秒)
	DWORD			 m_dwLoopCount;			//内容循环计数
	float			 m_fAlphaPer;			//当前渐变百分数
	DWORD			 m_dwAlphaTimeLen;		//渐变时间长度(毫秒)
	DWORD			 m_dwAlphaStartTime;	//渐变开始时间(毫秒)
	eALPHA_FADE_MODE m_eAlphaMode;			//走马灯出现消失的渐变模式

	RECT			   m_rcSourceLeft;		//源矩形
	RECT			   m_rcSourceRight;		//源矩形
	LPDIRECT3DSURFACE9 m_pOffscreenLeft;	//左方离屏表面
	LPDIRECT3DSURFACE9 m_pOffscreenRight;	//右方离屏表面

public:
	CRollLantern();			//构造函数
	~CRollLantern();		//析构函数

	// 访问数据成员
	void  GetPos(POINT & pt)	{ pt = m_ptPos; }					//得到走马灯绘制坐标(即背景图片绘制坐标)
	bool  GetIsFinished()		{ return m_bIsFinished; }			//得到走马灯是否已经显示完成
	float GetContentOffsetLeft();									//得到内容滚动偏移最左值
	float GetContentOffsetRight();									//得到内容滚动偏移最右值

	// 设定数据成员
	void SetSpeed(float f)					{ m_fSpeed = f; }					//设定内容滚动速度(毫秒/像素)
	void SetNumLoop(DWORD dw)				{ m_dwNumLoop = dw; }				//设定循环次数
	void SetPos(POINT & pt);													//设定走马灯绘制坐标(即背景图片绘制坐标)
	void SetClip(long lLeft, long lRight);										//设定裁剪的坐标(该坐标在相对坐标系,相对于走马灯绘制坐标)
	void SetContentOffsetHeight(long l)		{ m_lContentOffsetHeight = l; }		//设定显示内容高度偏移(该坐标在相对坐标系,相对于走马灯绘制坐标)
	void SetContent(const char * psz);											//设定显示内容
	bool SetBGPic(const char * pszFilePathName);								//设定背景图片
	void SetFontInfo(FONTINFO & stFontInfo) { m_stFontInfo = stFontInfo; }		//设定内容字体信息
	void SetAlphaTimeLen(DWORD dw)			{ m_dwAlphaTimeLen = (dw != 0) ? dw : 5000; }			//设定渐变时间长度(毫秒)

	// 操作接口
	void Start();				//开始显示走马灯
	void Update();				//更新走马灯
	void Draw();				//绘制走马灯
	void ReleaseOffscreen()					{ SAFERELEASE(m_pOffscreenLeft); SAFERELEASE(m_pOffscreenRight); }		//销毁离屏表面
	bool CreateOffscreenLeft();	//创建左方离屏表面
	bool CreateOffscreenRight();//创建右方离屏表面

private:
	void UpdateClipRect();		//更新裁剪矩形
};