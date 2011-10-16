#include "stdafx.h"
#include "TeamSetup.h"
#include "../../../Public/DataStream/ReadWrite.h"
TeamSetup::TeamSetup():interval(0)
{
	headWndType[0] = '\0';
	headWndName[0] = '\0';

	HpWndName[0] = '\0';
	HpWndType[0] = '\0';

	MpWndName[0] = '\0';
	MpWndType[0] = '\0';

	CurItemNum = 0;
}

TeamSetup::~TeamSetup()
{
	headWndType[0] = '\0';
	headWndName[0] = '\0';

	HpWndName[0] = '\0';
	HpWndType[0] = '\0';

	MpWndName[0] = '\0';
	MpWndType[0] = '\0';

	CurItemNum = 0;
}

bool  TeamSetup::ReadSetup()
{
	std::ifstream file;
	file.open("setup/UISetup/Team.ini");
	if(file.is_open()== false)
	{
		MessageBox(g_hWnd,"Open setup/UISetup/Team.ini failed !","Error",MB_OK);
		return false;
	}
	char temp;
	while(ReadTo(file,"#"))
	{
		file >> maxItem 
			>> pagePos.d_x
			>> pagePos.d_y
			>> itemSize.d_width
			>> itemSize.d_height
			>> interval
			>> ItemWndType
			>> ItemWndName
			>> headWndType
			>> headWndName
			>> headWndSize.d_width
			>> temp
			>> headWndSize.d_height
			>> headWndOffset.d_x
			>> temp
			>> headWndOffset.d_y
			
			>> HpWndType
			>> HpWndName
			>> HpWndSize.d_width
			>> temp
			>> HpWndSize.d_height
			>> HpWndOffset.d_x
			>> temp
			>> HpWndOffset.d_y
			
			>> MpWndType
			>> MpWndName
			>> MpWndSize.d_width
			>> temp
			>> MpWndSize.d_height
			>> MpWndOffset.d_x
			>> temp
			>> MpWndOffset.d_y

			>> PlayerWndType
			>> PlayerWndName
			>> PlayerWndSize.d_width
			>> temp
			>> PlayerWndSize.d_height
			>> PlayerWndOffset.d_x
			>> temp
			>> PlayerWndOffset.d_y

			>> LevelWndType
			>> LevelWndName
			>> LevelWndSize.d_width
			>> temp
			>> LevelWndSize.d_height
			>> LevelWndOffset.d_x
			>> temp
			>> LevelWndOffset.d_y;
		if(file.eof())
		{
			file.close();
			return true;
		}
	}
	file.close();
	return true;
}