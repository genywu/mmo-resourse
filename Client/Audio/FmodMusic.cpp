
#include "stdafx.h"
#include "FmodAudioDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//构造一个音乐对象
//参数说明
//	filename:声音文件名字
//返回说明
//	如果构造成功,音乐进入存活状态,否则不可用,并且很快被删除回收
CFmodMusic::CFmodMusic(const char *filename,FSOUND_STREAM *pStream,DWORD dwMode)
:Audio(filename)
{
	assert(pStream);
	m_dwMode = dwMode;
	m_pStream=pStream;
	m_bAlive=m_pStream!=NULL;
	SetPlayingType(Audio::PLAY_NONE);
	m_nChannel=FSOUND_FREE;
	SetVolume(Audio::s_fMusicVolume);
}

//删除音乐对象
//参数说明
//	无
//返回说明
//	无
CFmodMusic::~CFmodMusic()
{
	Release(false);
}

//播放并返回当前播放所使用的channel
//参数说明
//	loops:播放声音的循环次数,如果小于1,表示一直循环播放;反之则播放loops次后停止
//返回说明
//	如果成功播放,返回所使用的channel;失败则返回-1
int CFmodMusic::Play(int loops)
{	
	assert(m_pStream);
	if(m_bAlive == false)	return -1;
	m_nLoops=loops;

	m_nChannel=FSOUND_Stream_PlayEx(FSOUND_FREE,m_pStream,0,TRUE);
	if(m_nChannel == -1)
	{
		m_bAlive = false;
		Log4c::Warn(ROOT_MODULE,"FSOUND_Stream_PlayEx()(filename:%s) failed(ErrorInfo:%s).",
						GetFilename(),FMOD_ErrorString(FSOUND_GetError()));
		return -1;
	}

	//重新设置循环模式
	DWORD dwMode = FSOUND_Stream_GetMode(m_pStream);
	dwMode = dwMode&0xFFFFFFF0;
	if(loops==0)
		dwMode |= FSOUND_LOOP_NORMAL;
	else
		dwMode |= FSOUND_LOOP_OFF;
	if( FSOUND_Stream_SetMode(m_pStream,dwMode) == FALSE)
	{
		m_nChannel = FSOUND_FREE;
		m_bAlive = false;
		Log4c::Warn(ROOT_MODULE,"FSOUND_Stream_GetMode() failed(%s).",
					FMOD_ErrorString(FSOUND_GetError()));
		return -1;
	}

	if(FSOUND_SetVolume(m_nChannel,GetVolume()) == FALSE)
	{
		m_nChannel = FSOUND_FREE;
		m_bAlive = false;
		Log4c::Warn(ROOT_MODULE,"CFmodMusic::Play:FSOUND_SetVolume() failed(%s).",
				FMOD_ErrorString(FSOUND_GetError()));
		return -1;
	}

	SetPlayingType(loops>0?Audio::PLAY_ONCE:Audio::PLAY_LOOP);
	FSOUND_SetPaused(m_nChannel,FALSE);
	return m_nChannel;
}


//渐入音乐
//参数说明
//	time:声音渐入的时间的,在这个时间内声音的音量从0到当前最大音量
//	loops:播放声音的循环次数,如果小于1,表示一直循环播放;反之则播放loops次后停止
//返回说明
//	如果成功播放,返回所使用的channel;失败则返回-1
int CFmodMusic::FadeIn(unsigned int time,int loops)
{
	SetVolume(0);
	int channel=Play(loops);
	if(channel!=-1)
	{
		SetPlayingType(Audio::PLAY_FADEIN);
		m_nFadeTime=time;
		m_nFadeStartTime=GetCurTickCount();
	}
	return channel;
}

//渐出音乐
//参数说明
//	time:声音渐出的时间的,在这个时间内声音的音量从当前最大到0,之后停止
//	loops:播放声音的循环次数,如果小于1,表示一直循环播放;反之则播放loops次后停止
//返回说明
//	如果成功播放,返回所使用的channel;失败则返回-1
int CFmodMusic::FadeOut(unsigned int time,int loops)
{
	if(GetCurChannel() == -1 || IsAlive() == false )	return -1;
	//int channel=Play(loops);
	SetPlayingType(Audio::PLAY_FADEOUT);
	m_nFadeTime=time;
	m_nFadeStartTime=GetCurTickCount();
	return GetCurChannel();
}

//渐变音乐
//参数说明
//	time:声音渐出的时间的,在这个时间内声音的音量从当前最大到0,之后停止
//	loops:播放声音的循环次数,如果小于1,表示一直循环播放;反之则播放loops次后停止
//返回说明
//	如果成功播放,返回所使用的channel;失败则返回-1
int CFmodMusic::Fade(unsigned int time, float volume, int loops)
{
    if(GetCurChannel() == -1 || IsAlive() == false )	return -1;
	int VolumeTemp = GetVolume();
	m_fFadeCurVolume = (float)GetVolume()/255.0f;
	if(GetPlayingType()==Audio::PLAY_FADEOUT||GetPlayingType()==Audio::PLAY_FADEIN)
		return GetCurChannel(); 
	if(m_fFadeCurVolume>volume)
	{
		SetPlayingType(Audio::PLAY_FADEDOWN);
	}else if(m_fFadeCurVolume<volume){
		SetPlayingType(Audio::PLAY_FADEUP);
	}else{
		return GetCurChannel();
	}
	m_fFadeVolume = volume;
	m_nFadeTime=time;
	m_nFadeStartTime=GetCurTickCount();
	return GetCurChannel();
}


//停止音乐,同时释放音乐的资源,该音乐不久会被删除
//参数说明
//	无
//返回说明
//	无
void CFmodMusic::Stop()
{
	if(m_nChannel!=FSOUND_FREE)
	{
		if( FSOUND_StopSound(m_nChannel) == FALSE)
		{
			Log4c::Warn(ROOT_MODULE,"FSOUND_StopSound() failed(%s).",FMOD_ErrorString(FSOUND_GetError()));
		}
		m_nChannel=FSOUND_FREE;
	}
	SetPlayingType(Audio::PLAY_NONE);
	m_bAlive = false;
}

//释放音乐所关联的资源
//参数说明
//	bDelay 是否延迟释放，如果延迟释放，只做一个无效的标记，稍后由FmodEngine释放。
//返回说明
//	无
void CFmodMusic::Release(bool bDelay)
{
	Stop();
	m_bAlive=false;
}

//获取声音当前的channel
//参数说明
//	无
//返回说明
//	返回当前channel,-1是无效的channel,声音可能无效或未播放
int CFmodMusic::GetCurChannel()
{
	return m_nChannel;
}

//判断音乐是否处于存活状态,非存活状态则很快会被删除释放
//参数说明
//	无
//返回说明
//	true表示声音存活可以被播放,false则相反
bool CFmodMusic::IsAlive()
{
	return m_bAlive;
}


//判断音乐是否正在播放
//参数说明
//	无
//返回说明
//	true表示声音在播放,false则相反
bool CFmodMusic::IsPlaying()
{
	if(m_nChannel==FSOUND_FREE)return false;
	return FSOUND_IsPlaying(m_nChannel)!=0;
}

//返回音乐的播放长度,单位是毫秒
//参数说明
//	无
//返回说明
//	长度的毫秒数,-1表示无效
int CFmodMusic::GetAudioLength()
{
	if(!IsAlive())return -1;
	return FSOUND_Stream_GetLengthMs(m_pStream);
}

void CFmodMusic::Run()
{
	if( m_bAlive == false)	return;
	bool bPlaying = IsPlaying();
	bool bVolumeChange = Audio::m_bMusicVolumeChange;
	if(bVolumeChange)
	{
		SetVolume(Audio::s_fMusicVolume);
		Audio::m_bMusicVolumeChange = false;
	}

	switch(GetPlayingType())
	{
	case Audio::PLAY_ONCE:
		break;
	case Audio::PLAY_LOOP:
		break;
	case Audio::PLAY_FADEOUT:
		{
			if(bPlaying == false)	break;
			DWORD time=GetCurTickCount()-m_nFadeStartTime;
			float fVolume=Audio::s_fMusicVolume-time*Audio::s_fMusicVolume/m_nFadeTime;
			SetVolume(fVolume);
			bVolumeChange = true;
			if(time>m_nFadeTime)
			{
				m_bAlive = false;
				Stop();
			}
		}
		break;
	case Audio::PLAY_FADEIN:
		{
			DWORD time=GetCurTickCount()-m_nFadeStartTime;
			float fVolume=time*Audio::s_fMusicVolume/m_nFadeTime;
			SetVolume(fVolume);
			bVolumeChange = true;
			if(time>m_nFadeTime)
			{
				SetVolume(Audio::s_fMusicVolume);
				SetPlayingType(Audio::PLAY_LOOP);
				m_nLoops = 0;
			}
		}
		break;
	case Audio::PLAY_FADEDOWN:
		{
			DWORD time=GetCurTickCount()-m_nFadeStartTime;
			float fFadeVolume =  m_fFadeCurVolume - m_fFadeVolume;
			float fVolume=m_fFadeCurVolume-time*fFadeVolume/m_nFadeTime;
			SetVolume(fVolume);
			bVolumeChange = true;
			if(time>m_nFadeTime)
			{
				SetVolume(m_fFadeVolume);
				SetPlayingType(Audio::PLAY_LOOP);
				m_nLoops = 0;
			}
		}
		break;
	case Audio::PLAY_FADEUP:
		{
			DWORD time=GetCurTickCount()-m_nFadeStartTime;
			float fFadeVolume = m_fFadeVolume - m_fFadeCurVolume;
			float fVolume = m_fFadeCurVolume + time*fFadeVolume/m_nFadeTime;
			SetVolume(fVolume);
			bVolumeChange = true;
			if(time>m_nFadeTime)
			{
				SetVolume(m_fFadeVolume);
				SetPlayingType(Audio::PLAY_LOOP);
				m_nLoops = 0;
			}
		}
		break;
	default:
		break;
	}
	if(bPlaying == false)
		m_bAlive = false;
	if(bVolumeChange)
	{
		if(FSOUND_SetVolume(m_nChannel,GetVolume()) == FALSE)
		{
			m_nChannel = FSOUND_FREE;
			m_bAlive = false;
			Log4c::Warn(ROOT_MODULE,"CFmodMusic::Run:FSOUND_SetVolume() failed(%s).",FMOD_ErrorString(FSOUND_GetError()));
		}
	}
}

//更新音乐的状态
//参数说明
//	无
//返回说明
//	无
//void CFmodMusic::Update()
//{
//	switch(GetPlayingType())
//	{
//	case Audio::PLAY_ONCE:
//		SetVolume(Audio::s_fMusicVolume);
//		break;
//	case Audio::PLAY_LOOP:
//		SetVolume(Audio::s_fMusicVolume);
//		break;
//	case Audio::PLAY_FADEOUT:
//		{
//			DWORD time=GetCurTickCount()-m_nFadeStartTime;
//			if(time>m_nFadeTime)
//			{
//				SetPlayingType(Audio::PLAY_NONE);
//				SetVolume(0);
//				Stop();
//			}
//			else
//			{
//				float fVolume=Audio::s_fMusicVolume-time*Audio::s_fMusicVolume/m_nFadeTime;
//				SetVolume(fVolume);
//			}
//		}
//		break;
//	case Audio::PLAY_FADEIN:
//		{
//			DWORD time=GetCurTickCount()-m_nFadeStartTime;
//			if(time>m_nFadeTime)
//			{
//				SetPlayingType(m_nLoops<=0?Audio::PLAY_LOOP:Audio::PLAY_ONCE);
//				SetVolume(Audio::s_fMusicVolume);
//			}
//			else
//			{
//				float fVolume=time*Audio::s_fMusicVolume/m_nFadeTime;
//				SetVolume(fVolume);
//			}
//		}
//		break;
//	default:
//		break;
//	}
//	if(m_nChannel!=-1)
//	{
//		FSOUND_SetVolume(m_nChannel,GetVolume());
//	}
//}