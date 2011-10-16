#include "stdafx.h"
#include "RankPage.h"
#include "RankPageEvent.h"
RankPage::RankPage()
{
	m_event = NULL;
	LoadPage();
}

RankPage::~RankPage()
{
	SAFE_DELETE(m_event);
}

void RankPage::LoadPage()
{
	SetLayOutFileName("RankPage.layout");
	m_event = new RankPageEvent(ON_LOAD);
	ProcessPageEvent(m_event);
}

bool RankPage::Close()
{
	CEGUI::Window* pPage = GetPageWindow();
	if(!pPage)
	{
		throw CEGUI::InvalidRequestException("RankPage Close failed!");
		return false;
	}
	else
	{
		pPage->setVisible(false);
		pPage->setAlwaysOnTop(false);
	}
	return true;
}

bool RankPage::Open()
{
	CEGUI::Window* wnd = GetPageWindow();
	if(wnd)
	{
		wnd->setVisible(true);
		wnd->setAlwaysOnTop(true);
	}
	else
	{
		throw CEGUI::InvalidRequestException("RankPage Open failed!");
		return false;
	}
	return true;
}

void RankPage::ReleasePage()
{
	SAFE_DELETE(m_event);
}

void RankPage::UpdatePage()
{
	if(m_event)
	{
		m_event->SetEventType(ON_UPDATE);
		ProcessPageEvent(m_event);
	}
}