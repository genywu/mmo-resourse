/*****************************************************************************************
//	外界访问更新程序接口方法定义
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-13
------------------------------------------------------------------------------------------
	这是剑侠升级程序的接口定义，以及内部运行机理的一点简单描述。

	此模块对外只包含三个接口函数SwordUpdate，SwordUpdateGetResult与RegisterAppLaunchedAfterUpdate。
前者用于启动升级程序，中间一个用于查询升级程序的执行状况，后者用于告知升级程序在完成升级
后要如何重新启动程序。建议客户程序执行到需要时才载入此模块，在此模块执行结束后即释放它。

	（升级借口的）客户调用SwordUpdate方法来启动生级程序。此方法可能的返回值为
SWORDUPDATE_R_OTHER_INSTNACE_EXIST或SWORDUPDATE_R_LAUCHING；前者表示此升级程序已经有另外
一个实例正在被执行，生级程序不再被启动；后者表示正在启动升级程序。此方法的两个参数暂时保
留未被用到，应用时请传入0值。
	调用SwordUpdateGetResult可以查询升级程序的执行状况。其中参数pInformation为指向一个字
符缓冲区的指针，用于获取文本描述的执行信息，此其空间必须大于或者等于256字节，此模块可能
在此缓冲区内填入至长256字节的文本内容（包括本本结束符号）。函数返回时如果文本缓冲区里保
存的为长度为0的字符串，表示没有新的文本执行讯息描述。参数nParam保留未用，应用时请传入0值。
此方法的返回值可能为下面所列‘此程序升级模块返回值定义’SWORDUPDATE_R_*值中的任意一个，
但SWORDUPDATE_R_OTHER_INSTNACE_EXIST除外。关于这些值的含义请参看值定义处的注释说明。


	当此模块出现返回值SWORDUPDATE_R_FINISHED_WAIT，此模块会另开进程执行另外一个执行替换
文件功能的执行模块，此模块会等待占用要被替换文件的程序结束，然后执行替换文件任务然后，
然后执行通过RegisterAppLaunchedAfterUpdate注册的升级程序在完成升级后要启动程序。
	此模块在开始执行升级后会在系统中创建一个互斥信号名称为"SwordOnLineUpdateMutex"。在完
成升级过程后删除这个信号。此模块会另建线程来执行升级程序。

	可能之后会添加一些诸如文件版本判断的接口。

*****************************************************************************************/

//=========================================
//		此程序升级模块返回值定义
//=========================================
#define	SWORDUPDATE_R_IDLE						0	//高唱对面的女孩看过来，游手好闲中。
#define	SWORDUPDATE_R_ERROR						1	//升级程序执行遇到错误。
#define	SWORDUPDATE_R_OTHER_INSTNACE_EXIST		2	//升级程序已经被执行
#define	SWORDUPDATE_R_LAUCHING					3	//升级程序正在被启动
#define	SWORDUPDATE_R_NEEDLESS					4	//不需要升级，以及是最新版本
#define	SWORDUPDATE_R_IN_PROGRESS				6	//正在升级中
#define	SWORDUPDATE_R_FINISHED					5	//升级结束
#define	SWORDUPDATE_R_FINISHED_UPDATE_SELF		7	//完成此模块自身的升级，请关闭此模块，并重新启动生级后的版本。
#define	SWORDUPDATE_R_FINISHED_WAIT				8	//完成更新文件的下载，但是有有部分需要替换的旧文件未被替换，
													//等待替换程序已经启动，请关闭占用要被替换文件的程序。
/*
//开始生级程序执行，参数暂时未被用到
int SwordUpdate(unsigned int uParam, int nParam);
//获取升级程序执行结果
int	SwordUpdateGetResult(char* pInformation, int* pnInfomationLen, int* pnProgressPercent, int nParam);
//告知升级程序在完成升级后要如何重新启动程序
int RegisterAppLaunchedAfterUpdate(const char* pApplicationName, char* pCmdLine);
*/

//--------------------------------------------------------------------------
//	SwordUpdate函数原型，开始生级程序执行，参数暂时未被用到
//--------------------------------------------------------------------------
typedef int (*fnSwordUpdate)(unsigned int uParam, int nParam);
//--------------------------------------------------------------------------
//	SwordUpdateGetResult函数原型，获取升级程序执行结果
//--------------------------------------------------------------------------
typedef int (*fnSwordUpdateGetResult)(char* pInformation, int* pnInfomationLen, int* pnProgressPercent, int nParam);
 
//--------------------------------------------------------------------------
//	RegisterAppLaunchedAfterUpdate函数原型，告知升级程序在完成升级后要如何重新启动程序
//--------------------------------------------------------------------------
typedef int (*fnRegisterAppLaunchedAfterUpdate)(const char* pApplicationName, char* pCmdLine);
