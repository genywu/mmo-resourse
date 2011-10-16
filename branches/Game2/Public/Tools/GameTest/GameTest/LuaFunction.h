#pragma once

class CLuaFunction
{
public:
	CLuaFunction(void);
	~CLuaFunction(void);

	static CLuaFunction* AddGoods(CString OriginName,int Num,int lLevel);
private:
	CString OriginName;			//原始名
	int Num;				//数量
	int lLevel;				//等级
};
