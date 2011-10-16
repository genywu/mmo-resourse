////////////////////////////////////////////////////////////////////////////////////////////
//
// 合并两个数据库
// 1. 创建一个全新干净的数据库，暂定名Des
// 2. 将要合并的两个数据库 “整理”，两个数据库暂定名为Src1,Src2
// 3. 第一次执行将S1，合并到D，重新排列相关ID，
// 4. 再次执行，在将S2，合并到D，重新排列相关ID
// 5. 最后D数据库就是S1，S2的合并数据库，所有ID都进行重新排列的，重复名字也做了替换
//
////////////////////////////////////////////////////////////////////////////////////////////
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","RSEOF")

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <map>
//#include <set>
#include <fstream>
#include <string>

using namespace std;


// 数据库配置
struct SQL_SETUP {
	string Server;
	string Database;
	string Uid;
	string Pwd;
};


// 玩家ID，组织ID，留言ID
struct ID_SETUP {
	DWORD PlayerID;
	DWORD OrganizingID;
	DWORD LeavewordID;

	ID_SETUP() {
		PlayerID=0;
		OrganizingID=0;
		LeavewordID=0;
	}
};

