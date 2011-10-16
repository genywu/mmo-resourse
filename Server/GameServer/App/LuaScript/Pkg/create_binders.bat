tolua++.exe -o ..\Binders\GUIDbind.cpp -H ..\Binders\GUIDbind.h -I stdafx.h GUID.pkg
tolua++.exe -o ..\Binders\ScriptBind.cpp -H ..\Binders\ScriptBind.h -I stdafx.h Script.pkg
tolua++.exe -o ..\Binders\BaseBind.cpp -H ..\Binders\BaseBind.h -I stdafx.h Base.pkg
tolua++.exe -o ..\Binders\PlayerBind.cpp -H ..\Binders\PlayerBind.h -I stdafx.h Player.pkg
pause
