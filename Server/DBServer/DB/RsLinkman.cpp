
#include "StdAfx.h"
#include "RsLinkman.h"

RsLinkmanManage g_RsLinkmanManage;

//!								得到唯一实例
RsLinkmanManage& RsLinkmanManage::GetRsLinkmanManage(void)
{
	return g_RsLinkmanManage;
}


//!		在数据库中读取初始化数据
void	RsLinkmanManage::LoadPlayerInitData_FromDB(const CGUID &PlayerGuid, _ConnectionPtr &cn, vector<tagLinkman_DB>& vData)
{
	// 存盘线程传过来的连接不存在
	if(cn==NULL) 
	{
		PrintErr("存盘线程传过来的连接不存在");
		return;
	}
	if (NULL_GUID == PlayerGuid)
	{
		return;
	}

	_RecordsetPtr rs;
	try
	{
		TESTHR(CreateRs(rs));

		char szGuid[64] = {0};
		PlayerGuid.tostring(szGuid);

		char szSql[512] = {0};
		sprintf(szSql,"SELECT * FROM FriendsMember WHERE OwnerGUID='%s' OR AimGUID='%s' ORDER BY OwnerGUID, AimGUID;", szGuid, szGuid);
		TESTHR(OpenRs(szSql, rs, cn));

		vData.clear();

		while(!rs->GetadoEOF())
		{
			tagLinkman_DB Linkman_DB;

			_variant_t vOwnerGUID	= rs->GetCollect("OwnerGUID");
			_variant_t vAimGUID		= rs->GetCollect("AimGUID");

			Linkman_DB.OwnerGUID	= CGUID(((char*)(_bstr_t)vOwnerGUID));
			Linkman_DB.AimGUID		= CGUID(((char*)(_bstr_t)vAimGUID));
			Linkman_DB.uGroupID		= rs->GetCollect("GroupID");
			Linkman_DB.uDelState	= rs->GetCollect("DelState");
			strcpy(Linkman_DB.szAimName, (_bstr_t) rs->GetCollect("AimName"));
			
			vData.push_back(Linkman_DB);

			// 下条记录
			rs->MoveNext();
		}
		ReleaseRs(rs);

		return;
	}	
	catch(_com_error &e)
	{
		PrintErr("Load player linkman initData error", e);
		ReleaseRs(rs);

		return;
	}
}

//! 储存一条联系人记录
BOOL RsLinkmanManage::SaveLinkman(tagLinkman_DB &Linkman_DB, _ConnectionPtr &cn)
{
	char szSql[512]			= {0};
	char szOwnerGuid[64]	= {0};
	char szAimGuid[64]		= {0};

	Linkman_DB.OwnerGUID.tostring(szOwnerGuid);
	Linkman_DB.AimGUID.tostring(szAimGuid);

	sprintf(szSql, "INSERT INTO \
				   FriendsMember(OwnerGUID, AimGUID, AimName, GroupID) \
				   VALUES('%s', '%s', N'%s', %d);",
				   szOwnerGuid, szAimGuid, Linkman_DB.szAimName, Linkman_DB.uGroupID
				   );
	
	if(!ExecuteCn(szSql, cn))
	{
		PrintErr("存储联系人出错！");
		return FALSE;
	}

#ifdef _DEBUG
	PrintErr("写入联系人信息成功！！调试时在数据表“FriendsMember”中设置了联合主键（OwnerGUID + AimGUID + GroupID），确认写入无误后应取消该主键！");
#endif
	return TRUE;
}

//! 删除一条联系人记录
BOOL RsLinkmanManage::DeleteLinkman(tagLinkman_DB &Linkman_DB, _ConnectionPtr &cn)
{
	char szSql[512] = {0};

	char szOwnerGuid[64]	= {0};
	char szAimGuid[64]		= {0};

	Linkman_DB.OwnerGUID.tostring(szOwnerGuid);
	Linkman_DB.AimGUID.tostring(szAimGuid);

	sprintf(szSql, "DELETE FriendsMember WHERE OwnerGUID = '%s' AND AimGUID = '%s' AND GroupID = %d;",
				   szOwnerGuid, szAimGuid, Linkman_DB.uGroupID);

	if(!ExecuteCn(szSql, cn))
	{
		PrintErr("删除联系人出错！");
		return FALSE;
	}

	return TRUE;
}