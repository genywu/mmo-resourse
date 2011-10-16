#ifndef DB_UNION_H
#define DB_UNION_H
#include "BaseDBEntity.h"
#include "../../../public/OrganizingDef.h"
#include "../public/DBDefine.h"
#include <list>

using namespace std;




class CDBUnion : public CBaseDBEntity
{
public:
	CDBUnion();
	CDBUnion(const CGUID& guid);
	~CDBUnion();

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

//	void Create(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Delete(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Load(long msgid, BYTE* msgBuf, long& msgBufPos, vector<BYTE>& pBA);
//	void Save(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Update(long msgid, BYTE* msgBuf, long& msgBufPos);
//
//	tagFacBaseProperty&			GetProperty(void)	{ return m_Property;	}		
//	string&						GetName(void)		{ return m_strName;		}
//	const CGUID&				GetMasterID(void)	{ return m_lMasterID;	}
//	map<CGUID,tagMemInfo>&		GetMembers(void)	{ return m_Members;		}
//
//
//	void SetProperty(tagFacBaseProperty& value)		{ m_Property = value;	}
//	void SetName(string& value)						{ m_strName = value;	}
//	void SetMasterID(const CGUID& value)			{ m_lMasterID = value;	}
//	void SetMembers(map<CGUID,tagMemInfo>& value)	{ m_Members = value;	}
//
//	AttrNameToEnumMap& GetAttrNameToEnumMap(void) { return m_AttrNameToEnumMap; }
//
//	//////////////////////////////////////////////////////////////////////////
//	bool GetPropertyData(vector<BYTE>& ByteArray);				//得到属性数据
//	bool GetMembersData(vector<BYTE>& ByteArray);				//得到成员信息数据
//
//private:
//	AttrNameToEnumMap m_AttrNameToEnumMap;
//	void InitAttrNameToEnumMap(void);
//
//private:
//	//////////////////////////////////////////////////////////////////////////
//	tagFacBaseProperty			m_Property;			//该联盟的属性48
//	string						m_strName;			//联盟名字MAXSIZE_FactionName
//	CGUID						m_lMasterID;		//联盟ID4
//	map<CGUID,tagMemInfo>		m_Members;			//该帮所有成员以及他的权限和称号76*300=22800
};

#endif//DB_UNION_H