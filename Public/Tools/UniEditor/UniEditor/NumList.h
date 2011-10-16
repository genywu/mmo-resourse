#pragma once
#include "Region.h"

// CNumList

class CNumList : public CListCtrl
{
	DECLARE_DYNAMIC(CNumList)

public:
	struct tagList
	{
		tagList()
		{
			strName="";
			nNum=0;
			vecNo.clear();
		}
		string strName;
		int nNum;
		vector<long>vecNo;
	};
	BOOL InitialList();
	CNumList();
	virtual ~CNumList();
	void SetData(CRegion* pRegion,int type);
	BOOL GetVecData(int nType,string str,vector<long>&vecData);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClickNumlist(NMHDR *pNMHDR, LRESULT *pResult);
private:
	map<string,tagList>m_mapStrMonster;
	map<string,tagList>m_mapStrColl;

};


