#include "stdafx.h"
#include "samplemanager.h"
#include "..\..\Public\Package\RFile.h"
#include "..\..\Public\Package\ClientResource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

float CSampleManager::s_fMinDistance=13.0f;
float CSampleManager::s_fMaxDistance=50.0f;

CSampleManager::CSampleManager(void)
{
	m_dwSampleTotalNum	= 1;
	m_dwSampleStartTime	= GetCurTickCount();

	m_dwStreamTotalNum	= 1;
	m_dwStreamStartTime	= GetCurTickCount();

	m_Samples.clear();
	m_Streams.clear();
}

CSampleManager::~CSampleManager(void)
{
	Clear();
}

FSOUND_SAMPLE *CSampleManager::GetSample(const char* strFileName,DWORD dwMode)
{
	if(strFileName == NULL)	return NULL;

	tagKey key = {strFileName,dwMode};
	itSample it = m_Samples.find(key);
	if(it != m_Samples.end())
	{
		(*it).second.nRefCount++;
		(*it).second.dwUseNum++;
		m_dwSampleTotalNum++;
		return (*it).second.pSample;
	}

	//没有则尝试装载一个
	void *pData = NULL;
	int len = 0;
	FSOUND_SAMPLE *pSample = LoadSample(strFileName,dwMode,pData,len);
	if(pSample == NULL)	return NULL;

	DWORD dwTime = GetCurTickCount();
	m_dwSampleTotalNum++;
	tagSampleRef sampleref = {pSample,1,dwTime,1,pData,len};
	m_Samples[key] = sampleref;

	return pSample;
}

FSOUND_STREAM *CSampleManager::GetStream(const char* strFileName,DWORD dwMode)
{
	if(strFileName == NULL)	return NULL;

	tagKey key = {strFileName,dwMode};
	itStream it = m_Streams.find(key);
	if( it != m_Streams.end() )
	{
		tagStreamRef &stream = (*it).second;
		if(stream.nRefCount == 0)
		{
			stream.nRefCount++;
			stream.dwUseNum++;
			m_dwStreamTotalNum++;
			return stream.pStream;
		}
		return NULL;
	}

	void *pData = NULL;
	int len = 0;
	FSOUND_STREAM *pStream = LoadStream(strFileName,dwMode,pData,len);
	if(pStream == NULL)	return NULL;

	DWORD dwTime = GetCurTickCount();
	m_dwStreamTotalNum++;

	tagStreamRef streamref = {pStream,1,dwTime,1,pData,len};
	m_Streams[key] = streamref;

	return pStream;
}

void CSampleManager::Release(FSOUND_SAMPLE *pSample)
{
	if(pSample == NULL)	return;

	itSample it = m_Samples.begin();
	for(;it != m_Samples.end();it++)
	{
		tagSampleRef & sample = (*it).second;
		if(sample.pSample == pSample)
		{
			--sample.nRefCount;
			break;
		}
	}
	return;
}

void CSampleManager::Release(FSOUND_STREAM *pStream)
{
	itStream it = m_Streams.begin();
	for(; it != m_Streams.end();it++ )
	{
		tagStreamRef &stream = (*it).second;
		if(stream.pStream == pStream)
		{
			stream.nRefCount --;
			break;
		}
	}
}

//从文件装载一个声音资源
FSOUND_SAMPLE *CSampleManager::LoadSample(const char* strFileName,
										  DWORD dwMode,void*& pBuf,int& len)
{
	CRFile* prfile = rfOpen(strFileName);
	if( prfile == NULL)
	{
		Log4c::Warn(ROOT_MODULE,"CSampleManager::LoadSample:rfopen(%s) failed.",strFileName);
		return NULL;
	}
	int size = prfile->GetDatalen();
	if(size == 0)	return NULL;
	char *pData = new char[size];
	prfile->ReadData(pData,size);
	rfClose(prfile);

	FSOUND_SAMPLE *pSample=FSOUND_Sample_Load(FSOUND_FREE,pData,FSOUND_LOADMEMORY|dwMode,0,size);
	//装载失败
	if(pSample == NULL)
	{
		delete[] pData;
		Log4c::Warn(ROOT_MODULE,"Load %s sound file failed(%s).",
				strFileName,FMOD_ErrorString(FSOUND_GetError()));
		return NULL;
	}

	if( FSOUND_Sample_SetMinMaxDistance(pSample,
								CSampleManager::s_fMinDistance,
								CSampleManager::s_fMaxDistance) == FALSE)
	{
		delete[] pData;
		FSOUND_Sample_Free(pSample);
		Log4c::Warn(ROOT_MODULE,"FSOUND_Sample_SetMinMaxDistance() failed(%s).",
				FMOD_ErrorString(FSOUND_GetError()));
		return NULL;
	}
	pBuf = pData;
	len  = size;
	return pSample;
}

FSOUND_STREAM *CSampleManager::LoadStream(const char* strFileName,
										  DWORD dwMode,void*& pBuf,int& len)
{
	assert(strFileName);
	CRFile* prfile = rfOpen(strFileName);
	if( prfile == NULL)
	{
		Log4c::Warn(ROOT_MODULE,"CSampleManager::LoadSample:rfopen(%s) failed.",
				strFileName);
		return NULL;
	}
	int size = prfile->GetDatalen();
	if(size == 0)	return NULL;
	char *pData = new char[size];
	prfile->ReadData(pData,size);
	rfClose(prfile);


//	FSOUND_STREAM *ppStream=FSOUND_Stream_Open(filename, FSOUND_NORMAL | FSOUND_2D | FSOUND_MPEGACCURATE /*| FSOUND_NONBLOCKING*/ , 0, 0);
	FSOUND_STREAM *pStream=FSOUND_Stream_Open(	pData,
												FSOUND_LOADMEMORY|FSOUND_NORMAL|dwMode/*|FSOUND_MPEGACCURATE*/,
												0,size);
	//delete[] pData;
	if(pStream == NULL)
	{
		delete[] pData;
		Log4c::Warn(ROOT_MODULE,"FSOUND_Stream_Open(%s) failed(%s).",
				strFileName,FMOD_ErrorString(FSOUND_GetError()));
		return NULL;
	}

	pBuf = pData;
	len = size;
	return pStream;
}

//卸载一个声音资源
void CSampleManager::UnLoadSample(FSOUND_SAMPLE *pSample)
{
	if(pSample == NULL)	return;
	FSOUND_Sample_Free(pSample);
}

void CSampleManager::UnLoadStream(FSOUND_STREAM *pStream)
{
	if(pStream == NULL)	return;
	if(FSOUND_Stream_Close(pStream)== FALSE)
	{
		Log4c::Warn(ROOT_MODULE,"FSOUND_Stream_Close() failed(%s).",
				FMOD_ErrorString(FSOUND_GetError()));
	}
}

void CSampleManager::Clear(void)
{
	itSample it = m_Samples.begin();
	for(;it != m_Samples.end();it++)
	{
		tagSampleRef &sample = (*it).second;
		UnLoadSample(sample.pSample);
		delete[] sample.pData;
	}
	m_Samples.clear();

	itStream  it1 = m_Streams.begin();
	for(;it1 != m_Streams.end();it1++)
	{
		tagStreamRef &stream = (*it1).second;
		UnLoadStream(stream.pStream);
		delete[] stream.pData;
	}
	m_Streams.clear();
}

void CSampleManager::GrabageCollection()
{
	size_t size = m_Samples.size();
	//总数小于20，所有保留

	DWORD dwcurtime = GetCurTickCount();
	if(size > 16)
	{
		DWORD dwAverUseInter = (dwcurtime-m_dwSampleStartTime)/m_dwSampleTotalNum;
		itSample it = m_Samples.begin();
		for(;it != m_Samples.end();)
		{
			tagSampleRef sample = (*it).second;
			//没有sound使用该sample
			if( sample.nRefCount == 0 )
			{
				//综合使用次数和未使用时间计算出一个参考值，来确定是否释放该sample
				DWORD dwInter= dwcurtime-sample.dwStartRefTime;
				DWORD dwUseInter = dwInter/sample.dwUseNum;
				if(dwUseInter > dwAverUseInter && dwInter >= dwUseInter)
				{
					UnLoadSample(sample.pSample);
					delete[] sample.pData;
					it = m_Samples.erase(it);
					continue;
				}
			}
			it++;
		}
	}
	if(size > 2)
	{
		DWORD dwAverUseInter = (dwcurtime-m_dwStreamStartTime)/m_dwStreamTotalNum;
		itStream it = m_Streams.begin();
		for(;it != m_Streams.end();)
		{
			tagStreamRef &stream = (*it).second;
			if(stream.nRefCount == 0)
			{
				DWORD dwInter= dwcurtime-stream.dwStartRefTime;
				DWORD dwUseInter = dwInter/stream.dwUseNum;
				if(dwUseInter > dwAverUseInter && dwInter >= dwUseInter)
				{
					UnLoadStream(stream.pStream);
					delete[] stream.pData;
					it = m_Streams.erase(it);
					continue;
				}
			}
			it++;
		}
	}
}