
#include "stdafx.h"
#include "FmodAudioDef.h"
#include "samplemanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

float Audio::s_fMusicVolume=1.0f;
float Audio::s_fSoundVolume=1.0f;
float Audio::s_fSpeechVolume=1.0f;

bool Audio::m_bSoundVolumeChange = false;
bool Audio::m_bMusicVolumeChange = false;
bool Audio::m_bSpeechVolumeChange = false;

CFmodEngine *CFmodEngine::s_pInstance=NULL;

//CRITICAL_SECTION CFmodEngine::m_CriticalSectionAudioMSG;


//获取/创建声音引擎的实例
//参数说明
//	hwnd窗口句柄
//返回说明
//	如果初始化成功返回全局唯一的引擎实例,否则返回空
CFmodEngine *CFmodEngine::GetInstance(HWND hWnd)
{
	if(s_pInstance==NULL)
	{
		if( FSOUND_SetBufferSize(200) == FALSE)
		{
			Log4c::Warn(ROOT_MODULE,"FSOUND_SetBufferSize(int) failed(%s).",FMOD_ErrorString(FSOUND_GetError()));
			return NULL;
		}
		if( FSOUND_SetHWND(hWnd) == FALSE )
		{
			Log4c::Warn(ROOT_MODULE,"FSOUND_SetHWND(void*) failed(%s).",FMOD_ErrorString(FSOUND_GetError()));
			return NULL;
		}
		if( FSOUND_Init(44100,16,0) == FALSE )
		{
			Log4c::Warn(ROOT_MODULE,"FSOUND_Init() failed(%s).",FMOD_ErrorString(FSOUND_GetError()));
			return NULL;
		}
		s_pInstance=new CFmodEngine();
	}
	return s_pInstance;
}

//构造声音引擎
//参数说明
//	无
//返回说明
//	无
CFmodEngine::CFmodEngine()
{
	memset(m_pListenerPosition,0,sizeof(m_pListenerPosition));
	memset(m_pListenerVelocity,0,sizeof(m_pListenerVelocity));
	memset(m_pListenerFace,0,sizeof(m_pListenerFace));
	memset(m_pListenerUp,0,sizeof(m_pListenerUp));
	m_setAudioList.clear();
	m_bMusicEnabled=false;
	m_bSoundEnabled=false;
	m_bSpeechEnabled=false;

	m_nMaxSW2DChannels = 16;
	m_nMaxHW2DChannels = 0;
	m_nMaxHW3DChannels = 0;

	m_nSoundsSW2D = 0;
	m_nSoundsHW2D = 0;
	m_nSoundsHW3D = 0;

	m_nSpeechsSW2D = 0;
	m_nSpeechsHW2D = 0;
	m_nSpeechsHW3D = 0;
	m_dwLastGCTime= GetCurTickCount();

	m_Fades.clear();
	InitializeCriticalSection(&m_CSFade);

	int total = 0;
	//得到最大2D和3D硬件信道数量
	if( FSOUND_GetNumHWChannels(&m_nMaxHW2DChannels,
								&m_nMaxHW3DChannels,&total) == FALSE)
	{
		Log4c::Warn(ROOT_MODULE,"FSOUND_GetNumHWChannels(int*,int*,int*) failed(%s).",FMOD_ErrorString(FSOUND_GetError()));
	}
	m_nMaxHW2DChannels = min(16,m_nMaxHW2DChannels);
	m_nMaxHW3DChannels = min(16,m_nMaxHW3DChannels);
	//输出信息
	int nTotalChannels = FSOUND_GetMaxChannels();
	char strmsg[250]="";
	Log4c::Warn(ROOT_MODULE,"MaxSW2DChannels:%d,MaxHW2DChannels:%d,MaxHW3DChannels:%d,TotalHWChannel:%d,TotalChannel:%d.",
				m_nMaxSW2DChannels,m_nMaxHW2DChannels,m_nMaxHW3DChannels,total,nTotalChannels);
	//m_nMaxHW3DChannels = 0;
	//分配Sample管理器
	m_pSmapleManager = new CSampleManager();
}

//释放删除声音引擎
//参数说明
//	无
//返回说明
//	无
CFmodEngine::~CFmodEngine()
{
	RelaseAll();
	delete m_pSmapleManager;

	DeleteCriticalSection(&m_CSFade);

	FSOUND_Close();
}

//设置3D声音最小最大距离
//参数说明
//	min:最小距离 max:最大距离
//返回说明
//	无
void CFmodEngine::SetMinMaxDistance(float min,float max)
{
	CSampleManager::s_fMinDistance=min;
	CSampleManager::s_fMaxDistance=max;
}

//设置rollofffactor
//参数说明
//	factor:rollofffactor,详情请参考Fmod帮助文档
//返回说明
//	无
void CFmodEngine::SetRolloffFactor(float factor)
{
	FSOUND_3D_SetRolloffFactor(factor);
}

//设置distancefactor
//参数说明
//	factor:distancefactor,详情请参考Fmod帮助文档
//返回说明
//	无
void CFmodEngine::SetDistanceFactor(float factor)
{
	FSOUND_3D_SetDistanceFactor(factor);
}

//立即释放并清除所有的声音
//参数说明
//	无
//返回说明
//	无
void CFmodEngine::RelaseAll()
{
	set<Audio *>::iterator it;
	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
	{
		delete (*it);
	}
	m_setAudioList.clear();
}

//立即释放并清除所有的声音
//参数说明
//	无
//返回说明
//	无
//void CFmodEngine::ReleaseAllSounds()
//{
//	set<Audio *>::iterator it;
//	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
//	{
//		if((*it)->IsMusic())continue;
//		(*it)->Release();
//	}
//}

void CFmodEngine::StopAllSounds()
{
	set<Audio *>::iterator it;
	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
	{
		if((*it)->IsSound())
		{
			(*it)->Stop();
		}
	}
}
void CFmodEngine::StopAllSpeechs()
{
	set<Audio *>::iterator it;
	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
	{
		if((*it)->IsSpeech())
		{
			(*it)->Stop();
		}
	}
}

//立即释放并清除所有的音乐
//参数说明
//	无
//返回说明
//	无
//void CFmodEngine::ReleaseAllMusic()
//{
//	
//	set<Audio *>::iterator it;
//	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
//	{
//		if(!(*it)->IsMusic())continue;
//		(*it)->Release();
//	}
//}

void CFmodEngine::StopAllMusic()
{
	set<Audio *>::iterator it;
	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
	{
		if((*it)->IsMusic())
			(*it)->Stop();
	}
}

//释放一个声音对象,该声音会在下次状态更新的时候被删除
//参数说明
//	pAudio:要释放并删除的声音对象
//返回说明
//	无
void CFmodEngine::ReleaseAudio(Audio *pAudio)
{
	if( pAudio )
	{
		pAudio->Release();
	}
}

//查询当前总共存在的声音（包括音乐）
//参数说明
//	无
//返回说明
//	总数量
size_t CFmodEngine::GetSoundsCount()
{
	return (int)m_setAudioList.size();
}

//根据文件名字获取(加载)音乐
//参数说明
//	filename:文件名
//返回说明
//	如果已经存在这个文件名的音乐对象,返回该对象;否则创建并返回新的音乐对象
//注意事项
//	客户程序不适宜保留该音乐的引用在以后使用!可能会被自动释放删除
//CFmodMusic *CFmodEngine::GetMusic(const char *filename)
//{
//	if(!m_bMusicEnabled)return NULL;
//	if(filename==NULL)return NULL;
//
//	EnterCriticalSection(&CFmodEngine::m_CriticalSectionAudioMSG);
//	set<Audio *>::iterator it;
//	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
//	{
//		Audio *pAudio=(*it);
//		if( pAudio->IsAlive() && pAudio->IsMusic() && stricmp(filename,pAudio->GetFilename())==0 )
//		{
//			LeaveCriticalSection(&CFmodEngine::m_CriticalSectionAudioMSG);
//			return dynamic_cast<CFmodMusic *>(pAudio);
//		}
//	}
//	CFmodMusic *pMusic=new CFmodMusic(filename);
//	if(pMusic->IsAlive())
//	{
//		m_setAudioList.insert(pMusic);
//		LeaveCriticalSection(&CFmodEngine::m_CriticalSectionAudioMSG);
//		return pMusic;
//	}
//	else
//	{
//		pMusic->Release(false);
//		delete pMusic;
//	}
//	LeaveCriticalSection(&CFmodEngine::m_CriticalSectionAudioMSG);
//	return NULL;
//}

CFmodMusic *CFmodEngine::GetMusicEx(const char *strFilename,DWORD dwMode)
{
	if(!m_bMusicEnabled)return NULL;
	if(strFilename==NULL)return NULL;
	if(strcmp(strFilename,"") == 0)	return NULL;

	if(dwMode == FSOUND_HW3D) dwMode = FSOUND_HW2D;
	if(dwMode == FSOUND_HW2D && m_nMaxHW2DChannels == 0) dwMode = FSOUND_2D;

	//判断当前声音数量是否超过最大值
	if(dwMode == FSOUND_HW2D && m_nSoundsHW2D >= m_nMaxHW2DChannels)	return NULL;
	if(dwMode == FSOUND_2D   && m_nSoundsSW2D >= m_nMaxSW2DChannels)	return NULL;

	set<Audio *>::iterator it;
	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
	{
		Audio *pAudio=(*it);
		//如果存在,且是激活状态的，返回NULL
		if(pAudio && stricmp(strFilename,pAudio->GetFilename())==0 )
		{
			if(pAudio->IsMusic() && pAudio->IsAlive())
				return NULL;
		}
	}

	if(m_pSmapleManager==NULL) return NULL;
	//从管理器中得到sample
	FSOUND_STREAM *pStream = m_pSmapleManager->GetStream(strFilename,dwMode);
	if(pStream == NULL)	return NULL;

	CFmodMusic *pMusic=new CFmodMusic(strFilename,pStream,dwMode);
	m_setAudioList.insert(pMusic);
	if(dwMode == FSOUND_HW3D)
		m_nSoundsHW3D++;
	else if(dwMode == FSOUND_HW2D)
		m_nSoundsHW2D++;
	else if(dwMode == m_nSoundsSW2D)
		m_nSoundsSW2D++;
	return pMusic;
}

//根据文件名字查找已经存在的音乐
//参数说明
//	filename:文件名
//返回说明
//	如果已经存在这个文件名的音乐对象,返回该对象;否则创建并返回空
//注意事项
//	客户程序不适宜保留该音乐的引用在以后使用!可能会被自动释放删除
CFmodMusic *CFmodEngine::FindMusic(const char *filename)
{
	if(filename==NULL)return NULL;
	set<Audio *>::iterator it;
	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
	{
		Audio *pAudio=(*it);
		if( pAudio->IsMusic() && stricmp(filename,pAudio->GetFilename())==0 )
		{
			return dynamic_cast<CFmodMusic *>(pAudio);
		}
	}	
	return NULL;
}

//根据文件名字获取(加载)声音对象
//参数说明
//	filename:文件名
//	b2d:指示是否是2D声音
//返回说明
//	如果已经存在这个文件名的声音对象并且该声音不处于播放状态,返回该对象;
//	否则创建并返回新的声音对象,如果该声音不是使用此次文件名第一次创建的,
//	则该声音会在播放完毕后自动被回收
//注意事项
//	客户程序不适宜保留该声音的引用在以后使用!可能会被自动释放删除
//	如果是2D声音,如果存在该文件的实例对象,直接返回该声音对象
//CFmodSound *CFmodEngine::GetSound(const char *filename,bool b2d)
//{
//	static int num2d=0,num3d=0,total=0;
//	if(total==0)FSOUND_GetNumHWChannels(&num2d,&num3d,&total);
//	if(!m_bSoundEnabled)return NULL;
//	if(filename==NULL)return NULL;
//	if(b2d&&num2d<=m_nSounds2D)return NULL;
//	if(!b2d&&num3d<=m_nSounds3D)return NULL;
//	EnterCriticalSection(&CFmodEngine::m_CriticalSectionAudioMSG);
//	CFmodSound *pSound=NULL;
//	FSOUND_SAMPLE *pSample=NULL;
//	set<Audio *>::iterator it;
//	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
//	{
//		CFmodSound *pSound=dynamic_cast<CFmodSound *>((*it));
//		if(pSound==NULL)continue;
//		if( !pSound->IsAlive() )continue;
//		if( stricmp(pSound->GetFilename(),filename)==0 )
//		{
//			if( !pSound->IsPlaying() && b2d==pSound->Is2D() )
//			{
//				LeaveCriticalSection(&CFmodEngine::m_CriticalSectionAudioMSG);
//				return pSound;
//			}
//			pSample=pSound->m_pSample;
//		}
//	}
//	pSound=new CFmodSound(filename,pSample,b2d);
//	if(pSound->IsAlive())
//	{
//		m_setAudioList.insert(pSound);
//		LeaveCriticalSection(&CFmodEngine::m_CriticalSectionAudioMSG);
//		return pSound;
//	}
//	else
//	{
//		pSound->Release(false);
//		delete pSound;
//	}
//	LeaveCriticalSection(&CFmodEngine::m_CriticalSectionAudioMSG);
//	return NULL;
//}

//根据文件名字获取(加载)声音对象
//参数说明
//	filename:文件名
//	bHW3D:指示是否是3D的声音
CFmodSound *CFmodEngine::GetSoundEx(const char *strFilename,DWORD dwMode)
{	
	if(m_bSoundEnabled == false) return NULL;
	if(strFilename == NULL)	return NULL;
	if(strcmp(strFilename,"") == 0)	return NULL;
	//判断是否支持HW
	if(dwMode == FSOUND_HW3D && m_nMaxHW3DChannels == 0) dwMode = FSOUND_HW2D;
	if(dwMode == FSOUND_HW2D && m_nMaxHW2DChannels == 0) dwMode = FSOUND_2D;

	//判断当前声音数量是否超过最大值
	if(dwMode == FSOUND_HW3D && m_nSoundsHW3D >= m_nMaxHW3DChannels)	return NULL;
	if(dwMode == FSOUND_HW2D && m_nSoundsHW2D >= m_nMaxHW2DChannels)	return NULL;
	if(dwMode == FSOUND_2D   && m_nSoundsSW2D >= m_nMaxSW2DChannels)	return NULL;

	if(m_pSmapleManager==NULL) return NULL;
	//从管理器中得到sample
	FSOUND_SAMPLE *pSample = m_pSmapleManager->GetSample(strFilename,dwMode);
	if(pSample == NULL)	return NULL;

	CFmodSound *pSound=new CFmodSound(strFilename,pSample,dwMode);
	m_setAudioList.insert(pSound);
	if(dwMode == FSOUND_HW3D)
		m_nSoundsHW3D++;
	else if(dwMode == FSOUND_HW2D)
		m_nSoundsHW2D++;
	else if(dwMode == m_nSoundsSW2D)
		m_nSoundsSW2D++;
	return pSound;
}

//根据文件名字和channel查找已经存在的声音
//参数说明
//	filename:文件名
//	channel:声音播放所使用的channel,-1表示查找2D声音
//返回说明
//	如果找到用该文件创建并且正使用这个channel的声音,则返回该声音,否则返回空
//注意事项
//	客户程序不适宜保留该声音的引用在以后使用!可能会被自动释放删除
CFmodSound *CFmodEngine::FindSound(const char *filename,int channel)
{
	if(filename==NULL)	return NULL;
	set<Audio *>::iterator it;
	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
	{
		if( (*it) && (*it)->IsSound() )
		{
			CFmodSound *pSound=(CFmodSound *)(*it);
			if(pSound==NULL)	continue;
			if(	stricmp(filename,pSound->GetFilename())	!=0	)	continue;

			return pSound;
			/*DWORD dwMode = pSound->GetMode();
			if( (dwMode ==FSOUND_HW2D || dwMode == FSOUND_2D) && channel == -1)
				return pSound;
			if(dwMode == FSOUND_HW3D &&  pSound->GetCurChannel() == channel )
				return pSound;*/
		}
	}
	return NULL;
}

//根据channel查找已经存在的声音(只对3D音效有效)
//参数说明
//	channel:声音播放所使用的channel
//返回说明
//	如果找到用该文件创建并且正使用这个channel的声音,则返回该声音,否则返回空
//注意事项
//	客户程序不适宜保留该声音的引用在以后使用!可能会被自动释放删除
CFmodSound *CFmodEngine::FindSound(int channel)
{
	if(channel==-1)	return NULL;
	set<Audio *>::iterator it;
	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
	{
		if( (*it) && (*it)->IsSound() )
		{
			CFmodSound *pSound=(CFmodSound *)(*it);
			if(pSound==NULL)	continue;
			DWORD dwMode = pSound->GetMode();
			if(dwMode == FSOUND_HW3D &&  pSound->GetCurChannel() == channel )
				return pSound;
		}
	}
	return NULL;
}

//根据文件名字获取(加载)语音对象
//参数说明
//	filename:文件名
//	bHW3D:指示是否是3D的语音
CFmodSpeech *CFmodEngine::GetSpeechEx(const char *strFilename,DWORD dwMode)
{	
	if(m_bSpeechEnabled == false) return NULL;
	if(strFilename == NULL)	return NULL;
	if(strcmp(strFilename,"") == 0)	return NULL;
	//判断是否支持HW
	if(dwMode == FSOUND_HW3D && m_nMaxHW3DChannels == 0) dwMode = FSOUND_HW2D;
	if(dwMode == FSOUND_HW2D && m_nMaxHW2DChannels == 0) dwMode = FSOUND_2D;

	//判断当前声音数量是否超过最大值
	if(dwMode == FSOUND_HW3D && m_nSoundsHW3D >= m_nMaxHW3DChannels)	return NULL;
	if(dwMode == FSOUND_HW2D && m_nSoundsHW2D >= m_nMaxHW2DChannels)	return NULL;
	if(dwMode == FSOUND_2D   && m_nSoundsSW2D >= m_nMaxSW2DChannels)	return NULL;

	if(m_pSmapleManager==NULL) return NULL;
	//从管理器中得到sample
	FSOUND_SAMPLE *pSample = m_pSmapleManager->GetSample(strFilename,dwMode);
	if(pSample == NULL)	return NULL;

	CFmodSpeech *pSpeech=new CFmodSpeech(strFilename,pSample,dwMode);
	m_setAudioList.insert(pSpeech);
	if(dwMode == FSOUND_HW3D)
		m_nSpeechsHW3D++;
	else if(dwMode == FSOUND_HW2D)
		m_nSpeechsHW2D++;
	else if(dwMode == m_nSoundsSW2D)
		m_nSpeechsSW2D++;
	return pSpeech;
}

//根据文件名字和channel查找已经存在的语音
//参数说明
//	filename:文件名
//	channel:声音播放所使用的channel,-1表示查找2D语音
//返回说明
//	如果找到用该文件创建并且正使用这个channel的声音,则返回该声音,否则返回空
//注意事项
//	客户程序不适宜保留该声音的引用在以后使用!可能会被自动释放删除
CFmodSpeech *CFmodEngine::FindSpeech(const char *filename,int channel)
{
	if(filename==NULL)	return NULL;
	set<Audio *>::iterator it;
	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
	{
		if( (*it) && (*it)->IsSpeech() )
		{
			CFmodSpeech *pSpeech=(CFmodSpeech *)(*it);
			if(pSpeech==NULL)	continue;
			if(	stricmp(filename,pSpeech->GetFilename())	!=0	)	continue;

			return pSpeech;
			/*DWORD dwMode = pSound->GetMode();
			if( (dwMode ==FSOUND_HW2D || dwMode == FSOUND_2D) && channel == -1)
				return pSound;
			if(dwMode == FSOUND_HW3D &&  pSound->GetCurChannel() == channel )
				return pSound;*/
		}
	}
	return NULL;
}

//更新监听者属性
//参数说明
//	无
//返回说明
//	无
void CFmodEngine::SetListenerAttributes()
{
	FSOUND_3D_Listener_SetAttributes(m_pListenerPosition, m_pListenerVelocity,
		m_pListenerFace[0], m_pListenerFace[1], m_pListenerFace[2],
		m_pListenerUp[0], m_pListenerUp[1], m_pListenerUp[2]);
}

//设置监听者的方向
//参数说明
//	face:前面的方向向的量表示 up:上面的方向的向量表示
//返回说明
//	无
//注意事项
// face和up必须是有效的并且长度至少为3的数组
void CFmodEngine::SetListenerOritation(float *face,float *up)
{
	if(face==0 || up==0 )return;
	memcpy(m_pListenerFace,face,sizeof(m_pListenerFace));
	memcpy(m_pListenerUp,up,sizeof(m_pListenerUp));

	//设置
	SetListenerAttributes();
}

//设置监听者的方向
//参数说明
//	facex,facey,facez:前面的方向向的量表示 upx,upy,upz:上面的方向的向量表示
//返回说明
//	无
void CFmodEngine::SetListenerOritation(float facex,float facey,float facez,float upx,float upy,float upz)
{
	float face[3]={facex,facey,facez},up[3]={upx,upy,upz};
	SetListenerOritation(face,up);
}

//设置监听者的位置
//参数说明
//	pos:位置的向量表示
//返回说明
//	无
//注意事项
// pos是有效的并且长度至少为3的数组
void CFmodEngine::SetListenerPosition(float *pos)
{
	if(pos==0)return;
	memcpy(m_pListenerPosition,pos,sizeof(m_pListenerPosition));

	//设置
	SetListenerAttributes();
}

//设置监听者的位置
//参数说明
//	x,y,z:位置的向量表示
//返回说明
//	无
void CFmodEngine::SetListenerPosition(float x,float y,float z)
{
	float pos[3]={x,y,z};
	SetListenerPosition(pos);

	//设置
	SetListenerAttributes();
}

//设置监听者的速度
//参数说明
//	vel:速度的向量表示
//返回说明
//	无
//注意事项
// vel是有效的并且长度至少为3的数组
void CFmodEngine::SetListenerVelocity(float *vel)
{
	if(vel==0)return;
	memcpy(m_pListenerVelocity,vel,sizeof(m_pListenerVelocity));

	//设置
	SetListenerAttributes();
}

//设置监听者的速度
//参数说明
//	x,y,z:速度的向量表示
//返回说明
//	无
void CFmodEngine::SetListenerVelocity(float x,float y,float z)
{
	float vel[3]={x,y,z};
	SetListenerVelocity(vel);
}

//设置所有音乐的统一的最大音量
//参数说明
//	fVolume:音量,从0到1
//返回说明
//	无
void CFmodEngine::SetMusicVolume(float fVolume)
{
	Audio::s_fMusicVolume=fVolume;
	Audio::m_bMusicVolumeChange = true;
}

//设置所有声音的统一的最大音量
//参数说明
//	fVolume:音量,从0到1
//返回说明
//	无
void CFmodEngine::SetSoundVolume(float fVolume)
{
	Audio::s_fSoundVolume=fVolume;
	Audio::m_bSoundVolumeChange = true;
}

//设置所有语音的统一的最大音量
//参数说明
//	fVolume:音量,从0到1
//返回说明
//	无
void CFmodEngine::SetSpeechVolume(float fVolume)
{
	Audio::s_fSpeechVolume=fVolume;
	Audio::m_bSpeechVolumeChange = true;
}

//设置音乐是否有效果
//参数说明
//	b: 是否打开音乐
//返回说明
//	无
void CFmodEngine::SetMusicEnabled(bool b)
{
	if(!b&&m_bMusicEnabled)
	{
		//ReleaseAllMusic();
		StopAllMusic();
	}
	m_bMusicEnabled=b;
}

//设置声音是否有效果
//参数说明
//	b: 是否打开音乐
//返回说明
//	无
void CFmodEngine::SetSoundEnabled(bool b)
{
	if(!b&&m_bSoundEnabled)
	{
		//ReleaseAllSounds();
		StopAllSounds();
	}
	m_bSoundEnabled=b;
}

//设置语音是否有效果
//参数说明
//	b: 是否打开音乐
//返回说明
//	无
void CFmodEngine::SetSpeechEnabled(bool b)
{
	if(!b&&m_bSpeechEnabled)
	{
		//ReleaseAllSounds();
		StopAllSpeechs();
	}
	m_bSpeechEnabled=b;
}



void CFmodEngine::Run()
{
	set<Audio *>	NotAliveAudio;NotAliveAudio.clear();

	set<Audio *>::iterator it;
	for(it=m_setAudioList.begin();it!=m_setAudioList.end();)
	{
		Audio *pAudio=(*it);
		if(pAudio)
		{		
			pAudio->Run();
			if(pAudio->IsAlive() == false)
			{
				if(pAudio->IsSound())
				{
					FSOUND_SAMPLE *pSample = ((CFmodSound*)pAudio)->GetSample();
					m_pSmapleManager->Release(pSample);
				}
				if(pAudio->IsMusic())
				{
					FSOUND_STREAM *pStream = ((CFmodMusic*)pAudio)->GetStream();
					m_pSmapleManager->Release(pStream);
				}
				if(pAudio->IsSpeech())
				{
					FSOUND_SAMPLE *pSample = ((CFmodSpeech*)pAudio)->GetSample();
					m_pSmapleManager->Release(pSample);
				}
				it = m_setAudioList.erase(it);
				DWORD dwMode = pAudio->GetMode();
				if(dwMode == FSOUND_HW3D)
					m_nSoundsHW3D--;
				else if(dwMode == FSOUND_HW2D)
					m_nSoundsHW2D--;
				else if(dwMode == m_nSoundsSW2D)
					m_nSoundsSW2D--;
				delete pAudio;
				continue;
			}
		}
		it++;
	}

	//每过2分钟收集一次没有使用的资源
	DWORD dwCurTime = GetCurTickCount();
	if( dwCurTime-m_dwLastGCTime > 120000)
	{
		m_pSmapleManager->GrabageCollection();
	}
}

void CFmodEngine::Fade(int nChannel,bool bFadeIn,int nMaxVolume,DWORD dwFadeTime)
{
	DWORD dwCurTime = GetCurTickCount();
	tagFade fade = {nChannel,bFadeIn,nMaxVolume,dwCurTime,dwFadeTime};
	EnterCriticalSection(&m_CSFade);
	m_Fades[nChannel] = fade;
	LeaveCriticalSection(&m_CSFade);
}

//更新引擎以及它所维护的声音和音乐的状态
//参数说明
//	无
//返回说明
//	无
//注意事项
//	声音系统需要定期更新状态,因此客户程序必须定期调用,建议有专门的线程来处理,时间间隔建议在50MS
//声音的渐入渐出
void CFmodEngine::Update()
{
	//SetListenerAttributes();
	FSOUND_Update();
	DWORD dwCurTime=GetCurTickCount();
	EnterCriticalSection(&m_CSFade);
	itFade it = m_Fades.begin();
	for(;it != m_Fades.end();)
	{
		int nVolume = 0;
		bool bErase = false;
		tagFade &fade = (*it).second;
		DWORD time=dwCurTime-fade.dwStartTime;
		if(fade.bFadeIn)
		{
			nVolume=time*fade.nMaxVolume/fade.dwFadeTime;
			if(time>fade.dwFadeTime)
			{
				bErase = true;
			}
		}
		else
		{
			nVolume=fade.nMaxVolume-time*fade.nMaxVolume/fade.dwFadeTime;
			if(time>fade.dwFadeTime)
			{
				bErase = true;
				if( FSOUND_StopSound(fade.nChannel) == FALSE)
				{
					Log4c::Warn(ROOT_MODULE,"CFmodEngine::Update:FSOUND_StopSound() failed(%s).",
							FMOD_ErrorString(FSOUND_GetError()));
				}
			}
		}
		if(FSOUND_SetVolume(fade.nChannel,nVolume) == FALSE)
		{
			bErase = true;
			Log4c::Warn(ROOT_MODULE,"CFmodEngine::Update:FSOUND_SetVolume() failed(%s).",
					FMOD_ErrorString(FSOUND_GetError()));
		}
		if(bErase)
			it = m_Fades.erase(it);
		else
			it++;
	}
	LeaveCriticalSection(&m_CSFade);

	/*set<Audio *>::iterator it;
	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
	{
		Audio *pAudio=(*it);
		if(pAudio==NULL)
		{
			continue;
		}
		if(pAudio->IsAlive())
		{
			pAudio->Update();
		}
	}*/
}

/*垃圾回收*/
void CFmodEngine::GrabageCollection()
{
	static set<Audio *> setDeleted;
	setDeleted.clear();

	set<Audio *>::iterator it;
	for(it=m_setAudioList.begin();it!=m_setAudioList.end();it++)
	{
		Audio *pAudio=(*it);
		if(pAudio==NULL)
		{
			continue;
		}
		if(pAudio->IsAlive() == false)
		{
			setDeleted.insert(pAudio);
		}
	}

	for(it=setDeleted.begin();it!=setDeleted.end();it++)
	{
		(*it)->Release(false);
		delete (*it);
		m_setAudioList.erase((*it));
	}
}

