//=============================================================================
/**
 *  file: DataBlock.h
 *
 *  Brief:有固定大小的内存快，动态分配数据块的基本机构。
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-6-13
 */
//=============================================================================
#ifndef __DATABLOCK__H__
#define __DATABLOCK__H__
#pragma once

class CDataBlock
{
public:
	CDataBlock(long maxsize);
	CDataBlock(void *maxsize);
	
	long m_lTestFlag;
public:
	virtual ~CDataBlock(void);

private:
	//是否是初始化数据
	bool m_bInitData;
	//数据起始地址
	BYTE *m_pBase;

	//当前数据块大小
	long m_nCurSize;
	//总数据块大小
	const long m_nMaxSize;
public:
	BYTE *Base()	{return m_pBase;}

	long GetCurSize()	{return m_nCurSize;}
	void SetCurSize(long lSize);

	long GetMaxSize()	{return m_nMaxSize;}

	void SetIsInitData(bool b)	{m_bInitData=b;}
	bool GetIsInitData()	{return m_bInitData;}
private:
  //屏蔽这些操作
  CDataBlock &operator= (const CDataBlock &);
  CDataBlock (const CDataBlock &);
};

#endif