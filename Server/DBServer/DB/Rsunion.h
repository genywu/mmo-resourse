#pragma once
#include "..\myadobase.h"

#include "..\..\server\dbserver\appworld\DBUnion.h"
//#include "..\..\server\dbserver\appworld\Organizingsystem\organizingctrl.h"

class CRsUnion :
	public CMyAdoBase
{
public:
	CRsUnion(void);
	~CRsUnion(void);

public:
	//long AddConfederation(const CGUID& lMasterID,const char* strName);				//添加一条盟的记录
	//bool DelConfederation(const CGUID& lConfederID,_ConnectionPtr& cn);		//删除一个盟得记录
	//bool LoadConfederation(CDBUnion* pConf);							//装载盟
	//bool SaveConfederation(CDBUnion* pConf,_ConnectionPtr& cn);	//保存盟
	//int LoadAllConfederation();									//装载所有盟
	//bool SaveAllConfederation();
	//bool LoadConfeMembers(CDBUnion *pConf,_ConnectionPtr& cn);			//装载盟成员
	//bool GetUnionMemInfo(tagMemInfo& MemInfo,_ConnectionPtr& cn);//得到同盟成员信息
	//bool SaveConfeMembers(CDBUnion *pConf,_ConnectionPtr& cn);			//保存盟成员
};
