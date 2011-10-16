/* WeatherFlutter.h
 * 功能: 为了丰富场景效果，使游戏场景显得更加华丽同时更能映托出整体环境的感觉，我们将修改
 *		 天气效果的表现或增加环境效果。模拟魔法能量升腾，雪花飞舞，蒲公英被吹动等效果。
 * 摘要: 可以设定“粒子”的贴图或者是动画序列
 *		 可以设定该“粒子”的大小 ，即与原始模型大小的比例值
 *		 可以设定起始高度（与水平面为“0”，可以输入负值——即在水平面以下
 *		 可以设定在该矢量方向上的随机移动方向的变化随机值（范围值）
 *		 可以设定存在时间（单位：MS毫秒）
 *		 可以设定“粒子”初始移动矢量方向及速度值
 *		 可以配置数量，且该数量值是恒定不变的（不管是否消亡或者出生）
 * 作者: lpf
 * 创建日期: 2008-06-26
 */

#pragma once
#include "Weather.h"

class WeatherFlutter : public Weather
{
public:
	struct tagGranule
	{
		D3DXVECTOR3 vPos;			//颗粒的当前坐标
		D3DXVECTOR3 vSpeed;			//颗粒的速度矢量

		bool  bLiving;				//当前颗粒是否存活
		DWORD dwLiveTimeMax;		//颗粒的最大存活时间
		DWORD dwStartTime;			//颗粒动画的开始播放时间
	};

private:
	tagGranule * m_tGranules;

	float m_fSize;      // 基准尺寸

	D3DXVECTOR3 m_vSpeedMin;			//颗粒速度矢量下限
	D3DXVECTOR3 m_vSpeedMax;			//颗粒速度矢量上限
	DWORD m_dwLiveTimeMax;				//颗粒的最大存活时间
	DWORD m_dwFrameLineNum;				//动画帧的单行数量
	float m_fFrameWH;					//动画帧的纹理宽度
	DWORD m_dwFrameMax;					//颗粒动画的最大帧数
	DWORD m_dwFrameInterval;			//颗粒动画的帧间隔

public:
	WeatherFlutter();					//构造函数
	virtual ~WeatherFlutter();			//析构函数

	void		 EnableLiving(bool bEnable);					//设定所有的颗粒是否被激活
	virtual void ModifyParameters(const char * pszConfig);		//修改参数

	virtual bool Create();				//创建飞舞天气
	virtual void Destroy();				//销毁飞舞天气

	virtual void Render();																//渲染天气
	virtual bool Update(BOOL bEnable, D3DXVECTOR3 & vWindForce = D3DXVECTOR3(0, 0, 0));	//更新天气

private:
	virtual void ReLiveGranules(tagGranule * pFlutterGranules);									//重新为飞舞颗粒赋予生命
	virtual void ProcessAnimTexture(VERTEX * pVertex, DWORD dwStartTime, DWORD dwCurrentTime);	//处理纹理动画
};
