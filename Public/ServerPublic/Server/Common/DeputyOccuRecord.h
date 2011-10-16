//		文件名		DeputyOccuRecord.h
//		作者		heliangchen
//		时间		2008/2/1
//		简介		副职业的记录。包括职业的经验，等级，和制作列表

#pragma once
#include "../../../Common/DeputyOccu.h"

class CDeputyOccuRecord
{
public:
	CDeputyOccuRecord();
	~CDeputyOccuRecord();
	//压副职业数据

	bool AddDeputyOccuDataByteArray(vector<uchar>* pByteArray);
	//为客户端压数据
	bool AddDeputyOccuDataForClient(vector<uchar>* pByteArray);
	//解副职业数据
	bool DecordDeputyOccuDataFromByteArray(uchar* pByte, long& pos);
	//为客户端解数据
	bool DecordDeputyOccuDataForClient(uchar* pByte, long& pos);

	bool CodeToDataBlock(DBWriteSet& setWriteDB);
	bool CodeToDataBlockForClient(DBWriteSet& setWriteDB);
	bool DecodeFromDataBlock(DBReadSet& setReadDB);

public:
	
    ////取得采集经验
	//long GetCollectionExp(){return m_lCollectionExp;}
	////设置采集经验
	//void SetCollectionExp(long l){m_lCollectionExp=l;}
	////取得采集等级
	//long GetCollectionGrade(){return m_lCollectionGrade;}
	////设置采集等级
	//void SetCollectionGrade(long l){m_lCollectionGrade=l;}
	////取得制作经验
	//long GetFactureExp(){return m_lFactureExp;}
	////设置制作经验
	//void SetFactureExp(long l){m_lFactureExp=l;}
	////取得制作等级
	//long GetFactureGrade(){return m_lFactureGrade;}
	////设置制作等级
	//void SetFactureGrade(long l){m_lFactureGrade=l;}

	//取得累计成功率
	long GetFactureSuccRatio(){return m_lFectureSuccRatio;}
	//加上累计成功率
	void AddFactureSuccRatio(long l){m_lFectureSuccRatio+=l;}
	//清空累计成功率
	void ClearFactureSuccRatio(){m_lFectureSuccRatio= 0;}
	//添加制作条目
	bool AddFacture(ulong l);
	//寻找制作条目
	bool FindFacture(ulong l);
	//删除制作条目
	bool DeleteFacture(ulong l);
	//清空制作条目
	bool CleanFacture();
	//当调整配置经验后,按配置文件修正
	void ModifyFactureExp(long& level,long& lExp);
	void ModifyCollectionExp(long& level,long& lExp);

public:
	long m_lCollectionExp;			//采集经验
	long m_lCollectionGrade;		//采集等级
	long m_lFactureExp;				//制作经验
	long m_lFactureGrade;			//制作等级
	long m_lFectureSuccRatio;		//累计成功率
	set<ulong> m_setFactureList;	//制作列表
private:
};
