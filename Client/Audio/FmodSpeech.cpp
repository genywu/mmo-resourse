
#include "stdafx.h"
#include "FmodAudioDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//构造一个声音对象
//参数说明
//	filename:声音文件名字
//	pSample:初始的流,一个流可能被多个声音对象使用
//	b2d:是否是2D声音
//返回说明
//	如果成功构造,声音进入存活状态,否则声音非存活状态,不可用
CFmodSpeech::CFmodSpeech(const char *filename,FSOUND_SAMPLE *pSample,DWORD dwMode)
:Audio(filename)
{
	m_dwMode = dwMode;
	//m_b3D=b3D;
	m_nLoops = 1;
	m_pSample=pSample;
	assert(m_pSample);
	if(m_pSample)
	{
		m_bAlive=m_pSample!=NULL;
		m_nChannel=FSOUND_FREE;
		SetPlayingType(Audio::PLAY_NONE);
		SetVolume(Audio::s_fSpeechVolume);
	}
}

//删除声音对象
//参数说明
//	无
//返回说明
//	无
CFmodSpeech::~CFmodSpeech()
{
	Release(false);
}

//释放声音所关联的资源
//参数说明
//	bDelay 是否延迟释放，如果延迟释放，只做一个无效的标记，稍后由FmodEngine释放。
//返回说明
//	无
void CFmodSpeech::Release(bool bDelay)
{
	Stop();
	m_bAlive=false;
}

//播放声音并返回当前播放所使用的channel
//参数说明
//	loops:播放声音的循环次数,如果小于1,表示一直循环播放;反之则播放loops次后停止
//返回说明
//	如果成功播放,返回所使用的channel;失败则返回-1
int CFmodSpeech::Play(int loops)
{
	assert(m_pSample);
	m_nLoops=loops;
	m_nChannel=FSOUND_PlaySoundEx(FSOUND_FREE,m_pSample,0,TRUE);	
	//出错
	if(m_nChannel==-1)
	{
		m_bAlive = false;
		Log4c::Warn(ROOT_MODULE,"FSOUND_PlaySoundEx() failed(filename:%s,ModeType:%d,info:%s).",
						GetFilename(),GetMode(),FMOD_ErrorString(FSOUND_GetError()));
		return -1;
	}

	//设置失败
	if( FSOUND_SetLoopMode(m_nChannel,loops==0?
								FSOUND_LOOP_NORMAL:FSOUND_LOOP_OFF)== FALSE )
	{
		m_nChannel = FSOUND_FREE;
		m_bAlive = false;
		Log4c::Warn(ROOT_MODULE,"FSOUND_SetLoopMode() failed(%s).",FMOD_ErrorString(FSOUND_GetError()));
		return -1;
	}
	//如果是3D,设置位置属性
	if( GetMode() == FSOUND_HW3D )
	{
		if( FSOUND_3D_SetAttributes(m_nChannel,m_pPos,0) == FALSE)
		{
			m_nChannel = FSOUND_FREE;
			m_bAlive = false;
			Log4c::Warn(ROOT_MODULE,"FSOUND_3D_SetAttributes() failed(%s).",FMOD_ErrorString(FSOUND_GetError()));
			return -1;
		}
	}
	if( FSOUND_SetVolume(m_nChannel,GetVolume()) == FALSE)
	{
		m_nChannel = FSOUND_FREE;
		m_bAlive = false;
		Log4c::Warn(ROOT_MODULE,"FSOUND_SetVolume() failed(%s).",FMOD_ErrorString(FSOUND_GetError()));
		return -1;
	}
	SetPlayingType(m_nLoops<1?Audio::PLAY_LOOP:Audio::PLAY_ONCE);
	FSOUND_SetPaused(m_nChannel,FALSE);
	return m_nChannel;
}

//渐入声音
//参数说明
//	time:声音渐入的时间的,在这个时间内声音的音量从0到当前最大音量
//	loops:播放声音的循环次数,如果小于1,表示一直循环播放;反之则播放loops次后停止
//返回说明
//	如果成功播放,返回所使用的channel;失败则返回-1
int CFmodSpeech::FadeIn(unsigned int time,int loops)
{
	SetVolume(0);
	int channel=Play(loops);
	if( channel != -1)
	{
		SetPlayingType(Audio::PLAY_FADEIN);
		m_nFadeTime=time;
		m_nFadeStartTime=GetCurTickCount();
	}
	return channel;
}

//渐出声音
//参数说明
//	time:声音渐出的时间的,在这个时间内声音的音量从当前最大到0,之后停止
//	loops:播放声音的循环次数,如果小于1,表示一直循环播放;反之则播放loops次后停止
//返回说明
//	如果成功播放,返回所使用的channel;失败则返回-1
int CFmodSpeech::FadeOut(unsigned int time,int loops)
{
	if(GetCurChannel() == -1 || IsAlive() == false )	return -1;
	//int channel=Play(loops);
	SetPlayingType(Audio::PLAY_FADEOUT);
	m_nFadeTime=time;
	m_nFadeStartTime=GetCurTickCount();
	return GetCurChannel();
}

//获取声音当前的channel
//参数说明
//	无
//返回说明
//	返回当前channel,-1是无效的channel,声音可能无效或未播放
int CFmodSpeech::GetCurChannel()
{
	return m_nChannel;
}

//返回声音的播放长度,单位是毫秒
//参数说明
//	无
//返回说明
//	长度的毫秒数,-1表示无效
int CFmodSpeech::GetAudioLength()
{
	//if(!IsAlive())return -1; 
	//return -1;
	return FSOUND_Sample_GetLength(m_pSample);
}

//停止声音
//参数说明
//	无
//返回说明
//	无
void CFmodSpeech::Stop()
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

//判断声音是否处于存活状态,非存活状态则很快会被删除释放
//参数说明
//	无
//返回说明
//	true表示声音存活可以被播放,false则相反
bool CFmodSpeech::IsAlive()
{
	return m_bAlive;
}

//判断声音是否正在播放
//参数说明
//	无
//返回说明
//	true表示声音在播放,false则相反
bool CFmodSpeech::IsPlaying()
{
	if(m_nChannel==FSOUND_FREE)	return false;
	return FSOUND_IsPlaying(m_nChannel)!=0;
}

//判断声音是否是2D声音
//参数说明
//	无
//返回说明
//	true表示声音是2D声音,false则相反
bool CFmodSpeech::Is3D()
{
	if( GetMode() == FSOUND_HW3D)	return true;
	return false;
}

//设置声音的位置
//参数说明
//	pos:x,y,z的坐标,长度至少为3的数组
//返回说明
//	无
//注意事项
//	pos的长度至少为3,否则会产生数组越界的错误;pos必须是有效指针
void CFmodSpeech::SetPosition(float *pos)
{
	if(pos==0)return;
	SetPosition(pos[0],pos[1],pos[2]);
}

//设置声音的位置
//参数说明
//	x:x坐标	y:y坐标	z:坐标
//返回说明
//	无
void CFmodSpeech::SetPosition(float x,float y,float z)
{
	m_pPos[0]=x;m_pPos[1]=y;m_pPos[2]=z;
}

//获取声音的坐标,存储在x,y,z的引用变量里
//参数说明
//	x:存储x坐标的引用变量 y:存储y坐标的引用变量	z:存储z坐标的引用变量
//返回说明
//	无
void CFmodSpeech::GetPosition(float &x,float &y,float &z)
{
	x=m_pPos[0];y=m_pPos[1];z=m_pPos[2];
}

//获取声音的坐标,存储在pos数组里
//参数说明
//	pos:长度至少为3的数组
//返回说明
//	无
//注意事项
//	pos必须是有效指针且长度至少为3
void CFmodSpeech::GetPosition(float *pos)
{
	memcpy(pos,m_pPos,sizeof(m_pPos));
}

void CFmodSpeech::Run()
{
	//看是否出于活动
	if( m_bAlive == false)	return;
	bool bPlaying = IsPlaying();
	bool bVolumeChange = Audio::m_bSpeechVolumeChange; 
	if(bVolumeChange)
	{
		SetVolume(Audio::s_fSpeechVolume);
		Audio::m_bSpeechVolumeChange = true;
	}

	switch(GetPlayingType())
	{
	case Audio::PLAY_ONCE:
		{
			if(!bPlaying)
			{
				if(--m_nLoops>0)
				{
					int nChannel = FSOUND_PlaySound(m_nChannel,m_pSample);
					if(nChannel == -1 || m_nChannel != nChannel)
					{
						Log4c::Warn(ROOT_MODULE,"FSOUND_PlaySoundEx() failed(%s).",FMOD_ErrorString(FSOUND_GetError()));
					}
					else
					{
						bPlaying = true;
					}
				}
			}
		}
		break;
	case Audio::PLAY_LOOP:
		{
		}
		break;
		//渐进
	/*case Audio::PLAY_FADEIN:
		{
			if(bPlaying == false)	break;
			DWORD time=GetCurTickCount()-m_nFadeStartTime;
			float fVolume=time*Audio::s_fSoundVolume/m_nFadeTime;
			SetVolume(fVolume);
			bVolumeChange = true;
			if(time>=m_nFadeTime)
			{
				SetPlayingType(Audio::PLAY_ONCE);
				m_nLoops = 1;
			}
		}
		break;*/
		//渐出
	/*case Audio::PLAY_FADEOUT:
		{
			if(bPlaying == false)	break;
			DWORD time=GetCurTickCount()-m_nFadeStartTime;
			float fVolume=Audio::s_fSoundVolume-time*Audio::s_fSoundVolume/m_nFadeTime;
			SetVolume(fVolume);
			bVolumeChange = true;
			if(time>=m_nFadeTime)
			{
				m_bAlive = false;
			}
		}
		break;*/
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
			Log4c::Warn(ROOT_MODULE,"CFmodSound::Run:FSOUND_SetVolume() failed(%s).",FMOD_ErrorString(FSOUND_GetError()));
		}
	}
}

//更新声音的状态
//参数说明
//	无
//返回说明
//	无
//void CFmodSound::Update()
//{
//	switch(GetPlayingType())
//	{
//	case Audio::PLAY_ONCE:
//		{
//			if(!IsPlaying())
//			{
//				if(--m_nLoops>0)
//				{
//					m_nChannel = FSOUND_PlaySound(m_nChannel,m_pSample);
//				}
//				else
//				{
//					Stop();
//				}
//			}
//		}
//		break;
//	case Audio::PLAY_LOOP:
//		{
//			SetVolume(Audio::s_fSoundVolume);
//		}
//		break;
//	case Audio::PLAY_FADEIN:
//		{
//			DWORD time=GetCurTickCount()-m_nFadeStartTime;
//			if(time>=m_nFadeTime)
//			{
//				SetPlayingType(Audio::PLAY_LOOP);
//				SetVolume(Audio::s_fSoundVolume);
//			}
//			else
//			{
//				float fVolume=time*Audio::s_fSoundVolume/m_nFadeTime;
//				SetVolume(fVolume);
//			}
//		}
//		break;
//	case Audio::PLAY_FADEOUT:
//		{
//			DWORD time=GetCurTickCount()-m_nFadeStartTime;
//			if(time>m_nFadeTime)
//			{
//				Stop();
//			}
//			else
//			{
//				float fVolume=Audio::s_fSoundVolume-time*Audio::s_fSoundVolume/m_nFadeTime;
//				SetVolume(fVolume);
//			}
//		}
//		break;
//	default:
//		break;
//	}	
//	if( IsPlaying() )
//	{
//		if(m_b3D)FSOUND_3D_SetAttributes(m_nChannel,m_pPos,0);
//		FSOUND_SetVolume(m_nChannel,GetVolume());
//	}
//	else
//	{
//		/*if(m_bCloned)
//		{
//			static DWORD dwTimerStart=0;
//			if(dwTimerStart=0)dwTimerStart=GetCurTickCount();
//			else if(GetCurTickCount()-dwTimerStart>3000)
//			{
//				m_bAlive=false;
//				dwTimerStart=0;
//			}
//		}
//		else if(GetCurTickCount()-m_nLastActiveTime>30000)
//		{
//			m_bAlive=false;
//
//		}*/
//	}
//}
