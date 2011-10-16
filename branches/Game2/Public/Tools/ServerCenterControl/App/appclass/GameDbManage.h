



#pragma once


class GameDbManage
{
public:
	//!	成为数据库操作者
	//! 对数据库执行一个脚本
	//! 
private:
	//! 数据库
	struct tagGameDbSetup : public tagGameDbInfo
	{
		string strConnectString;
	};
};