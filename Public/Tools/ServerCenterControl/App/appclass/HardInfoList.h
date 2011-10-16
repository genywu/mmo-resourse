



#pragma once

/*
	缓存
*/
class HardInfoList
{
public:
	HardInfoList(DWORD dwSMID);
	~HardInfoList(VOID);

public:
	//!
	VOID							Release(VOID);

	//!								添加一个硬件信息节点
	BOOL							AddNode(tagHardwareInfo &HardwareInfo, BOOL bSendToUser = FALSE);
	//!								添加一组硬件信息节点
	BOOL							AddNodeList(tagHardwareInfo *pHardwareInfo, LONG lNum);

	//!								发送自身信息到客户端
	VOID							SendInfoToSocket(LONG lSocket);
	//!								发送自身信息到所有客户端
	VOID							SendInfoToAll(VOID);

public:
	//!								响应写入完成
	static		VOID				OnDBWriteFinish(tagHardwareInfo *pHardwareInfo, DWORD dwNum, DWORD dwSMID = 0);

private:
	//! 一组硬件信息节点
	struct tagHardInfoGroup
	{
		tagHardwareInfo	*pHardwareInfo;
		DWORD			dwUseNum;

		tagHardInfoGroup(VOID){memset(this, 0, sizeof(tagHardInfoGroup));}
	};

private:
	//!
	VOID							AddGroup(tagHardInfoGroup &HardInfoGroup);
	//!								请求向数据库写入数据
	VOID							RequestWriteDB(tagHardInfoGroup &HardInfoGroup);
	//!								得到当前缓存的所有数据
	VOID							GetCacheAllInfo(tagHardInfoGroup &HardInfoGroup);

private:
	DWORD							m_dwSMID;
	//!
	tagHardInfoGroup				m_CurrGroup;
	//!
	list<tagHardInfoGroup>			m_queueCacheInfo;

private:
	//!										正在写入数据库的数据
	static		set<tagHardwareInfo*>		m_setWriting;
};