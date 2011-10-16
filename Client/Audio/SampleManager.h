#ifndef _SAMPLE_MANAGER_H_
#define _SAMPLE_MANAGER_H_
//Sample管理器

#include "fmod.h"
#include "fmod_errors.h"

class CSampleManager
{
public:
	static float s_fMinDistance;//3D声音的最小距离
	static float s_fMaxDistance;//3D声音的最大距离

	struct tagSampleRef
	{
		FSOUND_SAMPLE *pSample;	//资源句柄
		int nRefCount;			//引用次数
		DWORD dwStartRefTime;	//最后一次引用时间
		DWORD dwUseNum;			//使用过的总次数
		void *pData;
		int datalen;
	};

	struct tagKey
	{
		string strName;			//资源的文件名
		DWORD  dwMode;			//装入模式
	};

	class key_compare
	{
	public:
		bool operator( )( const tagKey& Key1, const tagKey& Key2 ) const
		{
			int nRet = Key1.strName.compare(Key2.strName);
			if(nRet < 0)
				return true;
			else if(nRet == 0)
				if(Key1.dwMode < Key2.dwMode)
					return true;
			return false;
		}

	};

	struct tagStreamRef
	{
		FSOUND_STREAM *pStream;	//资源句柄
		int nRefCount;			//引用次数
		DWORD dwStartRefTime;	//最后一次引用时间
		DWORD dwUseNum;			//使用过的总次数
		void *pData;
		int datalen;
	};

public:
	CSampleManager(void);
	virtual ~CSampleManager(void);

	//得到一个sample资源
	//如果列表中有,计数器加1
	//如果没有，则装在资源，初始值置0
	FSOUND_SAMPLE *GetSample(const char* strFileName,DWORD dwMode);
	//当CFmodSound销毁的时候，调用此结构，使计数器减1
	void Release(FSOUND_SAMPLE *pSample);

	FSOUND_STREAM *GetStream(const char* strFileName,DWORD dwMode);

	void Release(FSOUND_STREAM *pStream);
	/*垃圾回收*/
	void GrabageCollection();

private:
	void Clear(void);
	FSOUND_SAMPLE *LoadSample(const char* strFileName,DWORD dwMode,void*& pBuf,int& len);
	void UnLoadSample(FSOUND_SAMPLE *pSample);

	FSOUND_STREAM *LoadStream(const char* strFileName,DWORD dwMode,void*& pBuf,int& len);
	void UnLoadStream(FSOUND_STREAM *pStream);

private:
	DWORD	m_dwSampleStartTime;
	DWORD	m_dwSampleTotalNum;

	typedef map<tagKey,tagSampleRef,key_compare>	mapSample;
	typedef map<tagKey,tagSampleRef,key_compare>::iterator itSample;
	mapSample	m_Samples;

	DWORD	m_dwStreamStartTime;
	DWORD	m_dwStreamTotalNum;

	typedef map<tagKey,tagStreamRef,key_compare>	mapStream;
	typedef map<tagKey,tagStreamRef,key_compare>::iterator itStream;
	mapStream	m_Streams;
};

#endif
