#include "stdafx.h"
#include "SelectLSMgr.h"
#include "../../../../Public/DataStream/ReadWrite.h"
LSHelp::LSHelp()
{
	m_vecLS.clear(); m_selLS = NULL ; 
}

LSHelp::~LSHelp()
{
	m_vecLS.clear();
	m_selLS = NULL;
}

void LSHelp::ReadLSInfo()
{
	m_vecLS.clear();
	std::ifstream file;
	file.open("setup/serverlist.ini");
	if(!file.is_open())
	{
		MessageBox(g_hWnd,"setup/serverlist.ini is not exit","Error",MB_OK);
		return;
	}
	while(ReadTo(file,"#"))
	{
			tagLS ls;
			string ip("");
			ushort port(0);
			file >> ls.ip
				>> ls.port 
				>> ls.serverName
				>> ls.strArea
				>> ls.lHip
				>> ls.iAreaId
				>> ls.igroupId
				>> ls.iServerState
				>> ls.iServerXK;
			m_vecLS.push_back(ls);
			if(file.eof())
			{
				file.close();
				return;
			}
	}
	file.close();
};
