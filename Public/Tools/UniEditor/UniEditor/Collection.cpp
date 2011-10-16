#include "StdAfx.h"
#include "Collection.h"
#include "Region.h"

//采集物具体信息类
CCollectionInfo::CCollectionInfo(void)
{
	m_strCollName=_T("0");
	m_nProbability=50;
}

CCollectionInfo::~CCollectionInfo(void)
{
}

ifstream &operator>>(ifstream &rstream, CCollectionInfo &rCollectionInfo)
{
	rstream>>rCollectionInfo.m_strCollName>>rCollectionInfo.m_nProbability;
	return rstream;
}
//inline string GetNameString(string str)
//{
//	char strVal[100]; 
//	int i=0,j=0;
//	for(;40!=str[i];i++)
//	{	
//	}
//	i+=1;
//	for(;41!=str[i];i++,j++)	
//	{
//		strVal[j]=str[i];
//		if(41==str[i+1])
//		{
//			strVal[j+1]='\0';
//			break;
//		}
//	}
//	return strVal;
//}
ofstream &operator<<(ofstream &rstream, CCollectionInfo &rCollectionInfo)
{
	//string str=GetNameString(rCollectionInfo.m_strCollName);
	rstream<<rCollectionInfo.m_strCollName<<strSep<<rCollectionInfo.m_nProbability
		<<endl;
	return rstream;
}
void CCollectionInfo::CollInfo2String(CString &str)
{
	str.Format(_T("\t%-14s %2u%%\n"),
		m_strCollName, m_nProbability);
}

//CCollection类
CCollection::CCollection(void)
{

	m_nTime=10;
	m_nStartTime=1;
}
void CCollection::Collection2String(CString &str)
{
	str.Format(_T("\t(%-3u, %-3u; %-3u, %-3u) 数量:%-3u 出生时间:%-3u 起始时间:%-3u 方向:%-3d\n"),
		m_rcCoor.left, m_rcCoor.top, m_rcCoor.right, m_rcCoor.bottom, m_uQuantity, m_nTime,
		m_nStartTime, m_iDirection);
}
CCollection::CCollection(const long x, const long y)
{
	m_rcCoor		=   CRect(1, 1, x, y);			// 坐标
	m_nTime		= 0;								// 产生时间
	m_nStartTime	= 0;								// 开始时间
}
CCollection::CCollection(CCollection &rCollection)
{
	m_uNo			= rCollection.GetNo();					// 编号
	m_rcCoor		= rCollection.GetCoordinate();			// 坐标
	m_uQuantity		= rCollection.GetQuantity();			// 数量
	m_iDirection	= rCollection.GetDirection();			// 方向

	m_nTime		= rCollection.GetTimeBorn();			// 产生时间
	m_nStartTime	= rCollection.GetTimeStart();			// 开始时间
	VECCOLLINFO &vecInfo = rCollection.GetCollectionInfo();
	for(VECCOLLINFOITR itr=vecInfo.begin(); itr!=vecInfo.end(); ++itr)
	{
		CCollectionInfo *pCollInfo = new CCollectionInfo(**itr);
		m_vecCollectionInfo.push_back(pCollInfo);
	}
}
CCollection::~CCollection(void)
{
	for(VECCOLLINFOITR itr=m_vecCollectionInfo.begin(); itr!=m_vecCollectionInfo.end(); ++itr)
	{
		delete *itr;
	}
	m_rcCoor;
}
ifstream & operator>>(ifstream &rstream, CCollection &rcollection)
{
	rstream>>rcollection.m_rcCoor.left>>rcollection.m_rcCoor.top
		>>rcollection.m_rcCoor.right>>rcollection.m_rcCoor.bottom>>rcollection.m_uQuantity
		>>rcollection.m_nTime>>rcollection.m_nStartTime>>rcollection.m_iDirection;

	return rstream;
}

ofstream & operator<<(ofstream &rstream, CCollection &rcollection)
{
	if (rcollection.m_bSwitch)
	{
		rstream << strSharp;
	} 
	else
	{
		rstream << strSep;
	}
	rstream<<rcollection.m_uNo
		<< strSep
		<<rcollection.m_rcCoor.left
		<< strSep
		<<rcollection.m_rcCoor.top
		<< strSep
		<<rcollection.m_rcCoor.right
		<< strSep
		<<rcollection.m_rcCoor.bottom
		<< strSep
		<<rcollection.m_uQuantity
		<< strSep
		<<rcollection.m_nTime
		<< strSep
		<<rcollection.m_nStartTime
		<< strSep
		<<rcollection.m_iDirection
		<< endl << endl;

	return rstream;
}

