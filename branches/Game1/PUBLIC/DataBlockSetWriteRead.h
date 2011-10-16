
//=============================================================================
/**
 *  file: DataBlockWriteRead.h
 *
 *  Brief:定义了一个数据块读写集合
 *
 *  Authtor:wangqiao
 *	
 *	Date:2008-11-21
 */
//=============================================================================

#ifndef __DATABLOCKSET_WRITEREAD__H__
#define __DATABLOCKSET_WRITEREAD__H__

#pragma once

class CDataBlock;
struct tagDBOpParam;
class CDataBlockAllocator;
class CGUID;

//定义了一个写数据块的集合
typedef struct tagDataBlockWriteSet
{
	typedef vector<CDataBlock*>::iterator itDataSet;
	vector<CDataBlock*>* pDataSet;

	//数据块分配器
	CDataBlockAllocator* pDBAllocator;
	//当前数据块集合操作参数
	tagDBOpParam* pDBWriteParam;
	unsigned long *pToltalSize;

	tagDataBlockWriteSet()
		:pDBAllocator(NULL)
		,pDBWriteParam(NULL)
		,pDataSet(NULL)
		,pToltalSize(NULL)
	{
	}

	void Initial(CDataBlockAllocator* pAllocator,
							tagDBOpParam* pWriteParam,
							vector<CDataBlock*>* pSet,
							unsigned long* pSize)
	{
		pDBAllocator = pAllocator;
		pDBWriteParam = pWriteParam;
		pDataSet = pSet;
		pToltalSize =pSize;
	}

	void AddToByteArray(void* pSource, long n);	// 添加数据
	void AddToByteArray(const char* pStr);
	void AddToByteArray(const CGUID& guid);
	inline void AddToByteArray(long l)
	{
		AddBuff((BYTE*)&l,sizeof(long));
	}
	inline void AddToByteArray(short l)
	{
		AddBuff((BYTE*)&l,sizeof(short));
	}
	inline void AddToByteArray(char l)
	{
		AddBuff((BYTE*)&l,sizeof(char));
	}
	inline void AddToByteArray(float l)
	{
		AddBuff((BYTE*)&l,sizeof(float));
	}
	inline void AddToByteArray(double l)
	{
		AddBuff((BYTE*)&l,sizeof(double));
	}
	inline void AddToByteArray(BYTE l)
	{
		AddBuff((BYTE*)&l,sizeof(BYTE));
	}
	inline void AddToByteArray(WORD l)
	{
		AddBuff((BYTE*)&l,sizeof(WORD));
	}
	inline void AddToByteArray(DWORD l)
	{
		AddBuff((BYTE*)&l,sizeof(DWORD));
	}
private:
	void AddBuff(BYTE* pBuf, long size);
	void AddWrDataBlock();
}DBWriteSet;


typedef struct tagDataBlockReadSet
{
	typedef vector<CDataBlock*>::iterator itDataSet;
	vector<CDataBlock*>* pDataSet;

	//当前数据块集合操作参数
	tagDBOpParam* pDBReadParam;

	tagDataBlockReadSet()
		:pDBReadParam(NULL)
		,pDataSet(NULL)
	{
	}
	void Initial(tagDBOpParam* pReadParam,
						vector<CDataBlock*>* pSet)
	{
		pDBReadParam = pReadParam;
		pDataSet = pSet;
	}
	
	void* GetBufferFromByteArray(void* pBuf, long lLen);
	char* GetStringFromByteArray(char* pStr, long lMaxLen);
	bool  GetBufferFromByteArray(CGUID& guid);

	// 从CByteArray获取long
	inline long GetLongFromByteArray()
	{
		long l = 0;
		GetBuff((BYTE*)&l,sizeof(long));
		return l;
	}

	// 从CByteArray获取short
	inline short GetShortFromByteArray()
	{
		short l = 0;
		GetBuff((BYTE*)&l,sizeof(short));
		return l;
	}

	// 从CByteArray获取char
	inline char GetCharFromByteArray()
	{
		char l  = 0;
		GetBuff((BYTE*)&l,sizeof(char));
		return l;
	}

	inline float GetFloatFromByteArray()
	{
		float l = 0;
		GetBuff((BYTE*)&l,sizeof(float));
		return l;
	}

	inline double GetDoubleFromByteArray()
	{
		double l = 0;
		GetBuff((BYTE*)&l,sizeof(double));
		return l;
	}
	
	inline BYTE GetByteFromByteArray()
	{
		BYTE l = 0;
		GetBuff((BYTE*)&l,sizeof(BYTE));
		return l;
	}

	inline WORD GetWordFromByteArray()
	{
		WORD l= 0;
		GetBuff((BYTE*)&l,sizeof(WORD));
		return l;
	}

	inline DWORD GetDwordFromByteArray()
	{
		DWORD l = 0;
		GetBuff((BYTE*)&l,sizeof(DWORD));
		return l;
	}

private:
	void* GetBuff(BYTE* pByte, long size);
	void AddRdDataBlock();

}DBReadSet;

#endif