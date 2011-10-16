// 场景对象管理器 [11/23/2009 Square]
#ifndef _ClientRgnObjMgr_H_
#define _ClientRgnObjMgr_H_
#pragma once

class CBaseObject;
typedef hash_map<CGUID,CBaseObject*,hash_guid_compare>	CMapRgnObj;

class CClientRgnObjMgr
{
public:
	CClientRgnObjMgr(void);
	~CClientRgnObjMgr(void);

	bool AddObject(CBaseObject* pObject);
	void RemoveObject(CBaseObject* pObject);
	CBaseObject* FindObject(long lType,const CGUID& guid);
	CBaseObject*	FindObject(const CGUID& guid);

	CMapRgnObj*	GetObjectList() { return &m_mapObject; }
private:
	CMapRgnObj		m_mapObject;

};
#endif//_RgnObjMgr_H_