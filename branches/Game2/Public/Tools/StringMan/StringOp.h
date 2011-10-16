/**
 * @file StringOp.h
 * @author Fox (xiangyunfeng@svn.com)
 * @date Mar. 22, 2010
 */

#pragma once

bool LoadResDir(bool sync = true);
bool LoadCallbackFunc(bool sync = true);
void UpdateStringItem();

/// You need to register your menu procession before mainloop and call
/// AppFrame::ExitThread() before return.
unsigned __stdcall MainThreadFunc(void* arg);
