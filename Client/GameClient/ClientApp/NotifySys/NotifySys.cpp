#include "stdafx.h"
#include "NotyfySys.h"
#include "../../Client/ui/Windows/NotifyCenterDef.h"
#include <list>
float NotifyRenderStringMgr::d_fSpeed = 0.0f;
int NotifyRenderStringMgr::d_iDir = 0;
std::list<NotifyRenderStringMgr::tagRQString> NotifyRenderStringMgr::strList;
int NotifyRenderStringMgr::d_elapseTime = 0;
int NotifyRenderStringMgr::d_maxRollTime;

NotifyRenderStringMgr::NotifyRenderStringMgr()
{
	LoadSetup();
	strList.clear();
}

NotifyRenderStringMgr::~NotifyRenderStringMgr()
{

}

void NotifyRenderStringMgr::LoadSetup()
{
	CRFile* pfile = rfOpen("data/RollWindowSetup.ini");
	if(!pfile)
	{
		MessageBox(g_hWnd,"Can't find file data/RollWindowSetup.ini","Error",MB_OK);
		return;
	}
	stringstream stream;
	pfile->ReadToStream(stream);
	rfClose(pfile);
	std::string temp;;
	stream >> temp >> d_maxRollTime
		   >> temp >> d_fSpeed 
		   >> temp >> d_iDir;
}

void NotifyRenderStringMgr::AddText(std::string str,RenderQueue rq /*= RQ_FIFTH*/)
{
	tagRQString tempRQ;
	tempRQ.rq = rq;
	char format[128];
	sprintf_s(format,128,"[ROLL 'Speed:%f Dir:%d']",d_fSpeed,d_iDir);
	tempRQ.str = format + str;
	if ( rq == RQ_FIFTH)
	{
		strList.push_back(tempRQ);
	}
	else if( rq == RQ_FST)
	{
		strList.push_front(tempRQ);
		d_elapseTime = timeGetTime();
		//最高优先级，重新设置窗口滚动文字
		FireUIEvent(NOTIFYCENTER_PAGE_NAME,NOTIFYCENTER_EVENT_CHANGETEXT);
	}
	else//重排列表
	{
		strList.sort();
	}
}

void NotifyRenderStringMgr::UpdateTime()
{
	int curTime = timeGetTime();
//#ifdef _DEBUG
//	char temp[256];
//	sprintf_s(temp,256,"%d\n",curTime - d_elapseTime);
//	OutputDebugStr(temp);
//#endif
	//超过显示时间
	if( (curTime - d_elapseTime) / 1000 >= d_maxRollTime)
	{
		d_elapseTime = timeGetTime();
		if(IsEmpty() == false)
		{
			strList.pop_front();
			//重新设置窗口显示文字
			FireUIEvent(NOTIFYCENTER_PAGE_NAME,NOTIFYCENTER_EVENT_CHANGETEXT);
		}
	}
}

string& NotifyRenderStringMgr::FrontText()
{
	return strList.front().str;
}

bool NotifyRenderStringMgr::IsEmpty()
{
	return strList.empty();
}