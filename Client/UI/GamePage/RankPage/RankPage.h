/**
* @filename:RankPage.h
* @date: 2010/3/23
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 排行榜界面UI
*/
#pragma  once
#include "../../GamePage.h"
//前置类声明
class RankPageEvent;
///排行榜UI界面类
class RankPage:public GamePage
{
public:    
	///
	RankPage();
	///
	virtual ~RankPage();
	///
	virtual void LoadPage();
	///
	virtual void ReleasePage();
	///
	virtual bool Open();
	///
	virtual bool Close();

	///更新界面上的数据显示
	void UpdatePage();

private:
	/// UI界面事件
	RankPageEvent* m_event;
};