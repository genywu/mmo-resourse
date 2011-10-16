#include "stdafx.h"
#include "GoodsViewerPage.h"
#include "GoodsViewerEvent.h"

GoodsViewerPage::GoodsViewerPage()
{
	LoadPage();
}

GoodsViewerPage::~GoodsViewerPage()
{
	if(m_GVEvent)
	{
		delete m_GVEvent ; m_GVEvent = NULL ;
	}
}

void GoodsViewerPage::LoadPage()
{
	SetLayOutFileName("GoodsTree1.layout");
	//相关的事件处理放到GVEvent中
	m_GVEvent = new GVEvent(ON_LOAD);
	ProcessPageEvent(m_GVEvent);
}

bool GoodsViewerPage::Open()
{
	CEGUI::Window *pPage = GetPageWindow();
	if (pPage)
	{
		pPage->setVisible(true);
		pPage->activate();
		pPage->setAlwaysOnTop(true);
	}
	else
	{
		CEGUI::InvalidRequestException("GoodsViewerPage Open Failed!");
		return false;
	}
	return true;
}

bool GoodsViewerPage::Close()
{
	CEGUI::Window *pPage = GetPageWindow();
	if(pPage)
	{
		pPage->setVisible(false);
		pPage->setAlwaysOnTop(false);
	}
	else
		return false;
	return true;
}

void GoodsViewerPage::ReleasePage()
{

}