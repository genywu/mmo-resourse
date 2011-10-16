class CFaction;

#pragma once

class CUnion
{
public:
	//! 用于从DB数据构造以存在的对象
	CUnion(void);
	//! 用于游戏中玩家创建新的对象（失败会抛出异常）
	CUnion(const CGUID &Guid, const char *pName, CFaction *pMaster);
	//! 
	~CUnion(void);

	//!	基本功能
	///////////////////////////////////////////////////////////////////////////////////////
public:
	//!										第一次保存
	void									Insert(VOID);
	//!										保存
	void									Save(VOID);
	//!										编码
	BOOL									AddToByteArray(vector<BYTE>& ByteArray);
	//!										解码
	BOOL									Decode(BYTE* pData, LONG &lPos);

public:
	//!										得到组织ID
	const	CGUID&							GetExID(void)const;
	//!										得到组织的名字
	const	char*							GetName(void)const;
	//!										得到盟主ID
	const	CGUID&							GetMasterID(void)const;
	//!										得到盟主的名字
	const	char*							GetMasterName(void)const;

	//!										设置组织ID
	const	CGUID&							SetExID(const CGUID& guid);
	//!										设置帮会的名字
	void									SetName(const char* pName);
	//!										设置盟主ID
	const	CGUID&							SetMasterID(const CGUID& guid);
	//!										设置盟主的名字
	void									SetMasterName(const char* pName);


	//!										添加新成员
	BOOL									AddNewMember(CFaction *pMember, BOOL bIsMaster = FALSE);
	//!										删除成员
	BOOL									DelMember(const CGUID& MemberGuid);
	//!										修改盟主
	BOOL									ChangeMaster(const CGUID& NewMasterGuid);
	//!										设置成员权限
	BOOL									SetPurview(const CGUID& MemberGuid, DWORD dwNewPurview);
	//!										解散
	BOOL									Disband(VOID);
	

	
	
	


	//!	For DBManager
	///////////////////////////////////////////////////////////////////////////////////////
public:
	//!										得到基本信息
	tagUnionBaseData&						GetUnionBaseData(VOID)		{return m_UnionBaseData;}
	//!										得到成员列表
	map<CGUID, tagUnionMemInfo>&			GetUnionMemInfoMap(VOID)	{return m_mapUnionMemInfo;}



	//!	
	///////////////////////////////////////////////////////////////////////////////////////
private:
	//!										基本资料
	tagUnionBaseData						m_UnionBaseData;
	//!										成员列表
	map<CGUID, tagUnionMemInfo>				m_mapUnionMemInfo;

public:
	//!										数据变更标记
	DWORD									m_dwDataChanged;
};
