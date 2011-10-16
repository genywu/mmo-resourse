// ScriptClaer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "UpdateFolder.h"

int _tmain(int argc, _TCHAR* argv[])
{
	if(argv[1] == NULL || argv[2] == NULL)
	{
		printf("请按如下方式键入转换目标位置和结果存放位置：\r\nScriptClaer C:/script C:/script_new \r\n程序终止！\r\n");
		return 0;
	}

	printf("确认输入的参数，清理开始【Y/N】");
	char UserEnter = getchar();
	if ('N' == UserEnter || 'n' == UserEnter)
	{
		printf("无法打开%s，用户终止！\r\n", argv[1]);
		return 0;
	}

	UpdateFolder folder;
	if(!folder.LoadDataFromDir(argv[1]))
	{
		printf("无法打开【%s】，程序终止！\r\n", argv[1]);
		return 0;
	}
	
	if (!folder.WriteDataToDir(argv[2]))
	{
		printf("写入【%s】位置文件数量为零，程序完成！\r\n", argv[1]);
	}

	printf("清理完毕！\r\n");
	return 0;
}

