/* AlphaFadeObjectList.h
 * 功能: 透明渐变对象显示列表类
 * 摘要: 该类对象包含了一个场景下所有需要透明渐变对象的显示信息
 *		 目前主要包含了透明渐出的对象,而该表里的对象会在渐出完成时,自动删除掉
 * 作者: lpf
 * 创建日期: 2008-10-13
 */

#pragma once

class CShape;
class AnimInfo;
class CDisplayModel;

// 透明渐变对象显示列表类
class CAlphaFadeObjectList
{
	vector<CShape *> m_vecObjectList;	//渐变操作对象表

public:
	// 对象透明渐变模式
	enum eALPHA_FADE_MODE
	{
		ALPHA_FADE_NONE = 0,			//没有渐变操作
		ALPHA_FADE_IN,					//渐进
		ALPHA_FADE_OUT,					//渐出
	};

	CAlphaFadeObjectList(void);			//构造函数
	~CAlphaFadeObjectList(void);		//析构函数

	// 操作接口
	void ReleaseAllObject();						//释放所有对象
	void ReleaseObjectById(DWORD dwId);				//释放指定索引对象
	void AddFadeInObject(CShape * pShape);			//增加一个渐进操作对象
	void AddFadeOutObject(CShape * pShape);			//增加一个渐出操作对象

	void Update();				//更新列表中的对象
	void Render();				//渲染列表中的对象

private:
	void RenderShadow(CShape * pShape);				//渲染阴影
};