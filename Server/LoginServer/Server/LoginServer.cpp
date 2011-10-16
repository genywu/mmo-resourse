// LoginServer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Game.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    return AppFrame::InitApp(hInstance, nCmdShow, GameThreadFunc);
}
