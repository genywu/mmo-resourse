#pragma once

class CPlayerCommand
{
public:
	CPlayerCommand(void){};
	~CPlayerCommand(void){};
	static bool Parse(char* txtCmd);
};