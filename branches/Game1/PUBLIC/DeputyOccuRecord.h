//		文件名		DeputyOccuRecord.h
//		作者		heliangchen
//		时间		2008/2/1
//		简介		副职业的记录。包括职业的经验，等级，和制作列表
#pragma once
#include <set>
using namespace std;

class CDeputyOccuRecord
{
public:
	CDeputyOccuRecord();
	~CDeputyOccuRecord();
	//压副职业数据
	BOOL AddDeputyOccuDataByteArray(vector<BYTE>* pByteArray);
	//为客户端压数据
	BOOL AddDeputyOccuDataForClient(vector<BYTE>* pByteArray);
	//解副职业数据
	BOOL DecordDeputyOccuDataFromByteArray(BYTE* pByte, long& pos);
	//为客户端解数据
	BOOL DecordDeputyOccuDataForClient(BYTE* pByte, long& pos);

	BOOL CodeToDataBlock(DBWriteSet& setWriteDB);
	BOOL CodeToDataBlockForClient(DBWriteSet& setWriteDB);
	BOOL DecodeFromDataBlock(DBReadSet& setReadDB);
public:
	////取得采集经验
	//LONG GetCollectionExp(){return m_lCollectionExp;}
	////设置采集经验
	//VOID SetCollectionExp(LONG l){m_lCollectionExp=l;}
	////取得采集等级
	//LONG GetCollectionGrade(){return m_lCollectionGrade;}
	////设置采集等级
	//VOID SetCollectionGrade(LONG l){m_lCollectionGrade=l;}
	////取得制作经验
	//LONG GetFactureExp(){return m_lFactureExp;}
	////设置制作经验
	//VOID SetFactureExp(LONG l){m_lFactureExp=l;}
	////取得制作等级
	//LONG GetFactureGrade(){return m_lFactureGrade;}
	////设置制作等级
	//VOID SetFactureGrade(LONG l){m_lFactureGrade=l;}
	//取得累计成功率
	LONG GetFactureSuccRatio(){return m_lFectureSuccRatio;}
	//加上累计成功率
	VOID AddFactureSuccRatio(LONG l){m_lFectureSuccRatio+=l;}
	//清空累计成功率
	VOID ClearFactureSuccRatio(){m_lFectureSuccRatio= 0;}
	//添加制作条目
	BOOL AddFacture(DWORD l);
	//寻找制作条目
	BOOL FindFacture(DWORD l);
	//删除制作条目
	BOOL DeleteFacture(DWORD l);
	//清空制作条目
	BOOL CleanFacture();
	//当调整配置经验后,按配置文件修正
	void ModifyFactureExp(long& level,long& lExp);
	void ModifyCollectionExp(long& level,long& lExp);
public:
	LONG m_lCollectionExp;			//采集经验
	LONG m_lCollectionGrade;		//采集等级
	LONG m_lFactureExp;				//制作经验
	LONG m_lFactureGrade;			//制作等级
	LONG m_lFectureSuccRatio;		//累计成功率
	set<DWORD> m_setFactureList;	//制作列表
private:
};
