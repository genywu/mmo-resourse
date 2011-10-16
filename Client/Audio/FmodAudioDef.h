/*------------------------------------------------------------------------------------------------

声音引擎模块

作者	:	汪亚军
日期	:	2004-11-30

封装FMOD音效系统的API,使之可以用较为简单方便的方式调用

本模块结构如下:
				Audio<------------------------CFmodEngine
				/   \
			   /	 \
		CFmodSound  CFmodMusic
Audio抽象表示所有声音,CFmodSound和CFmodMusic分别表示声效和音乐
CFmodEngine集合了引擎功能,用于创建和维护声音对象，以及管理整个声音引擎，包括设备管理、属性操作等

关于使用
	客户程序通过获取CFmodEngine的实例对象创建和使用声音于音乐资源,客户程序不适宜保留对声音和音乐
的引用,而是通过FindSound和FindMusic来查存在的找声音和音乐对象,这样来使用CFmodSound和CFmodMusic.
	音乐对象在使用完毕后会自动被回收资源并删除;2D声音和由声音文件第一次创建的3D声音会被缓存,如果
不显式调用CFmodSound的Relase方法,则它一直存在于内存;重复创建的3D声音会在播放完后自动回收和删除,
因此,建议客户程序不要保留对声音对象的引用.
	关于各种全局属性的操作(比如音量,监听者位置,方向等信息),由CFmodEngine负责.

-------------------------------------------------------------------------------------------------*/
#ifndef __FMODAUDIODEF__
#define __FMODAUDIODEF__

#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include "fmod.h"
#include "fmod_errors.h"
using namespace std;

class CSampleManager;

//抽象的声音表示类
class Audio{	
public:
	static float s_fMusicVolume;//全局音乐音量
	static float s_fSoundVolume;//全局声音音量
	static float s_fSpeechVolume;//全局语音音量

	static bool m_bMusicVolumeChange;
	static bool m_bSoundVolumeChange;
	static bool m_bSpeechVolumeChange;

public:

	enum eAudioPlayingType{
		PLAY_NONE,//无播放类型，表示声音不播放
		PLAY_ONCE,//声音播放一次
		PLAY_LOOP,//声音循环播放
		PLAY_FADEIN,//声音渐入
		PLAY_FADEOUT,//声音渐出 
		PLAY_FADEDOWN,//声音渐变（减小）
		PLAY_FADEUP,//声音渐变（变大）
	};
private:
	int			m_nPlayingType;				//声音的播放类型
	string		m_strFilename;				//声音的文件名字
	int			m_nVolume;

protected:
	DWORD		m_dwMode;

public:
	//获取声音的播放类型,为eAudioPlayingType之中的一种
	int GetPlayingType(){return m_nPlayingType;}

	//设置声音的播放类型
	void SetPlayingType(eAudioPlayingType type){m_nPlayingType=type;}

	//获取声音的文件名字
	const char *GetFilename(){return m_strFilename.c_str();}

	//判断声音是否是音乐
	virtual bool IsMusic(){return false;}
	virtual bool IsSound(){return false;}
	virtual bool IsSpeech(){return false;}

	//判断声音是否是2D
	virtual bool Is3D()	{return false;}
	DWORD	GetMode()	{return m_dwMode;}

	//判断声音是否正在播放
	virtual bool IsPlaying()=0;

	//获取声音当前所使用的channel
	virtual int	GetCurChannel()=0;
	
	//播放声音
	virtual int Play(int loops=1)=0;

	//渐入声音
	virtual int FadeIn(unsigned int time,int loops)=0;

	//渐出声音
	virtual int FadeOut(unsigned int time,int loops)=0;

	//停止声音
	virtual void Stop()=0;

	//释放声音,bDelay指明是否延迟释放
	virtual void Release(bool bDelay=true)=0;
	
	//获取声音的时间长度
	virtual int GetAudioLength()=0;

	//设置声音的音量大小
	virtual void SetVolume(float volume){m_nVolume=max(0,min(255,(int)(volume*255)));};

	//获取声音的音量大小
	virtual int GetVolume(){return m_nVolume;};

	//判断声音是否存活
	virtual bool IsAlive()=0;

	virtual void Run() = 0;
	//更新声音状态
	virtual void Update()=0;

	//释放删除声音
	virtual ~Audio(){};

protected:
	//构造声音
	Audio(const char *filename):m_strFilename(filename){}
};

//Fmod声音对象
class CFmodSound : public Audio
{
	friend class CFmodEngine;
private:
	//声音所关联的流对象指针
	FSOUND_SAMPLE	*m_pSample;
	
	//播放声音的channel	
	int				m_nChannel;

	//声音渐入或者渐出的时间
	unsigned int	m_nFadeTime;

	//声音开始渐入或者渐出的时间
	unsigned int	m_nFadeStartTime;

	//声音最后一次活动（播放）的的时间
	//unsigned int	m_nLastActiveTime;

	//声音的循环次数
	int				m_nLoops;

	//声音是否存活
	bool			m_bAlive;

	//声音是否是3D
	bool			m_b3D;

private:
	/*构造声音*/
	CFmodSound(const char *filename,FSOUND_SAMPLE *pStream,DWORD dwMode);	
	
	//声音的位置
	float m_pPos[3];

public:

	/*播放声音,loops表示循环次数*/
	int Play(int loops=1);

	/*声音音量从小到最大的播放,time指定渐进的时间,loops表示声音循环次数*/
	int FadeIn(unsigned int time,int loops);

	/*声音音量从最大到停止的播放,time指定渐进的时间,loops表示声音循环次数,但音量到最小后就停止*/
	int FadeOut(unsigned int time,int loops);

	/*停止声音*/
	void Stop();

	/*释放声音关联的资源*/
	void Release(bool bDelay=true);

	/*声音是否存活,可以使用*/
	bool IsAlive();

	/*声音是否正在播放*/
	bool IsPlaying();

	/*是否是2D声音*/
	bool Is3D();

	bool IsSound(){return true;}

	/*设置当前播放声音的channel*/
	void SetCurChannel(int nCannel){m_nChannel = nCannel;}

	/*得到当前播放声音的channel*/
	int GetCurChannel();

	/*得到声音的播放长度,毫秒*/
	int GetAudioLength();

	/*设置声音的空间位置,对2D声音没有效果*/
	void SetPosition(float x,float y,float z);

	/*设置声音的空间位置,对2D声音没有效果*/
	void SetPosition(float *pos);

	/*返回声音的空间位置,对2D声音没有效果*/
	void GetPosition(float &x,float &y,float &z);

	/*返回声音的空间位置,对2D声音没有效果*/
	void GetPosition(float *pos);

	FSOUND_SAMPLE *GetSample()	{return m_pSample;}

	void Run();
	/*刷新声音状态*/
	void Update(){}

	/*删除该类*/
	~CFmodSound();
};

//Fmod语音对象
class CFmodSpeech : public Audio
{
	friend class CFmodEngine;
private:
	//声音所关联的流对象指针
	FSOUND_SAMPLE	*m_pSample;
	
	//播放声音的channel	
	int				m_nChannel;

	//声音渐入或者渐出的时间
	unsigned int	m_nFadeTime;

	//声音开始渐入或者渐出的时间
	unsigned int	m_nFadeStartTime;

	//声音最后一次活动（播放）的的时间
	//unsigned int	m_nLastActiveTime;

	//声音的循环次数
	int				m_nLoops;

	//声音是否存活
	bool			m_bAlive;

	//声音是否是3D
	bool			m_b3D;

private:
	/*构造声音*/
	CFmodSpeech(const char *filename,FSOUND_SAMPLE *pStream,DWORD dwMode);	
	
	//声音的位置
	float m_pPos[3];

public:

	/*播放声音,loops表示循环次数*/
	int Play(int loops=1);

	/*声音音量从小到最大的播放,time指定渐进的时间,loops表示声音循环次数*/
	int FadeIn(unsigned int time,int loops);

	/*声音音量从最大到停止的播放,time指定渐进的时间,loops表示声音循环次数,但音量到最小后就停止*/
	int FadeOut(unsigned int time,int loops);

	/*停止声音*/
	void Stop();

	/*释放声音关联的资源*/
	void Release(bool bDelay=true);

	/*声音是否存活,可以使用*/
	bool IsAlive();

	/*声音是否正在播放*/
	bool IsPlaying();

	/*是否是2D声音*/
	bool Is3D();

	bool IsSpeech(){return true;}

	/*得到当前播放声音的channel*/
	int GetCurChannel();

	/*得到声音的播放长度,毫秒*/
	int GetAudioLength();

	/*设置声音的空间位置,对2D声音没有效果*/
	void SetPosition(float x,float y,float z);

	/*设置声音的空间位置,对2D声音没有效果*/
	void SetPosition(float *pos);

	/*返回声音的空间位置,对2D声音没有效果*/
	void GetPosition(float &x,float &y,float &z);

	/*返回声音的空间位置,对2D声音没有效果*/
	void GetPosition(float *pos);

	FSOUND_SAMPLE *GetSample()	{return m_pSample;}

	void Run();
	/*刷新声音状态*/
	void Update(){}

	/*删除该类*/
	~CFmodSpeech();
};




//Fmod音乐对象
class CFmodMusic : public Audio
{
	friend class CFmodEngine;
private:
	//是否存活
	bool				m_bAlive;

	//音乐关联的流的对象指针
	FSOUND_STREAM		*m_pStream;

	//音乐播放所使用的channel
	int					m_nChannel;

	//音乐渐入或渐出的时间范围
	unsigned int		m_nFadeTime;

	//音乐开始渐入（出）时的时间
	unsigned int		m_nFadeStartTime;

	//音乐播放的次数
	int					m_nLoops;

	//音乐渐变时的目标音量
	float                 m_fFadeVolume;

	//音乐在执行渐变时刻的当前音量
	float                 m_fFadeCurVolume;

private:
	//构造音乐对象
	CFmodMusic(const char *filename,FSOUND_STREAM *pStream,DWORD dwMode);

public:
	//判断是否是音乐,返回真
	bool IsMusic(){return true;}

	/*播放音乐*/
	int Play(int loops=-1);

	/*音乐渐入*/
	int FadeIn(unsigned int time,int loops);

	/*音乐渐出*/
	int FadeOut(unsigned int time,int loops);

	/*音乐渐变*/
	int Fade(unsigned int time,float volume,int loops);

	/*停止音乐*/
	void Stop();

	/*释放音乐*/
	void Release(bool bDelay=true);

	/*是否存活*/
	bool IsAlive();

	/*是否在播放*/
	bool IsPlaying();

	/*获取音乐的时间长度*/
	int GetAudioLength();

	/*获取当前使用的channel*/
	int GetCurChannel();

	FSOUND_STREAM *GetStream()	{return m_pStream;}
	void Run();
	/*更新状态*/
	void Update(){}

	/*删除释放*/
	~CFmodMusic();
};

//Fmod声音引擎
class CFmodEngine
{
public:
	/*获取一个引擎实例*/
	static CFmodEngine* GetInstance(HWND hwnd);
	/*提供临界区控制*/
	//static CRITICAL_SECTION m_CriticalSectionAudioMSG;
private:
	//全局唯一的引擎实例
	static CFmodEngine* s_pInstance;
private:

	struct tagFade
	{
		int  nChannel;
		bool bFadeIn;
		int nMaxVolume;
		DWORD dwStartTime;
		DWORD dwFadeTime;
	};
	//sample管理器
	CSampleManager *m_pSmapleManager;
	//声音对象的集合
	set<Audio *> m_setAudioList;
	//监听者的位置
	float m_pListenerPosition[3];
	//监听者的速度
	float m_pListenerVelocity[3];
	//监听者的面方向
	float m_pListenerFace[3];
	//监听者的上方向
	float m_pListenerUp[3];
	//是否打开声音
	bool m_bSoundEnabled;
	//是否打开音乐
	bool m_bMusicEnabled;
	//是否打开语音
	bool m_bSpeechEnabled;

	//SW2D声音总数量
	int m_nSoundsSW2D;
	//HW2D声音总数量
	int m_nSoundsHW2D;
	//HW3D声音总数量
	int m_nSoundsHW3D;

	//SW2D语音总数量
	int m_nSpeechsSW2D;
	//HW2D语音总数量
	int m_nSpeechsHW2D;
	//HW3D语音总数量
	int m_nSpeechsHW3D;

	//可利用的software mixed 2d channels.
	int m_nMaxSW2DChannels;
	//可利用的hardware mixed 2d channels.
	int m_nMaxHW2DChannels;
	//可利用的hardware mixed 3d channels.
	int m_nMaxHW3DChannels;

	DWORD m_dwLastGCTime;

	typedef map<int,tagFade>	mapFade;
	typedef map<int,tagFade>::iterator itFade;
	mapFade	m_Fades;
	CRITICAL_SECTION m_CSFade;

private:
	//更新监听者的信息
	void SetListenerAttributes();
	//构造引擎
	CFmodEngine();
public:
	//释放引擎
	~CFmodEngine();
	
	/*设置rolloff factor*/
	void SetRolloffFactor(float factor);

	/*设置mindistance和maxdistance*/
	void SetMinMaxDistance(float min,float max);

	/*设置distancefactor*/
	void SetDistanceFactor(float factor);

	/*设置音乐的音量*/
	void SetMusicVolume(float fVolume);

	/*设置声音的音量*/
	void SetSoundVolume(float fVolume);

	/*设置语音的音量*/
	void SetSpeechVolume(float fVolume);

	/*设置音乐是否有效*/
	void SetMusicEnabled(bool b);

	/*设置声音是否有效*/
	void SetSoundEnabled(bool b);

	/*设置语音是否有效*/
	void SetSpeechEnabled(bool b);

	/*设置监听者的位置*/
	void SetListenerPosition(float *pos);
	
	/*设置监听者的位置*/
	void SetListenerPosition(float x,float y,float z);
	
	/*设置监听者的速度*/
	void SetListenerVelocity(float *vel);

	/*设置监听者的速度*/
	void SetListenerVelocity(float x,float y,float z);
	
	/*设置监听者的方向*/
	void SetListenerOritation(float *face,float *up);
	
	/*设置监听者的方向*/
	void SetListenerOritation(float facex,float facey,float facez,float upx,float upy,float upz);

	void Run();
	/*更新引擎状态*/
	void Update();

	/*垃圾回收*/
	void GrabageCollection();

	/*查询总的声音数量*/
	size_t GetSoundsCount();

	/*获取声音*/
	//CFmodSound *GetSound(const char *strFilename,bool b2d);

	//获取声音
	CFmodSound *GetSoundEx(const char *strFilename,DWORD dwMode);

	/*查找声音*/
	CFmodSound *FindSound(const char *strFilename,int channel=-1);

	/*通过channel查找声音*/
	CFmodSound *FindSound(int channel=-1);

	//获取语音
	CFmodSpeech *GetSpeechEx(const char *strFilename,DWORD dwMode);

	/*查找语音*/
	CFmodSpeech *FindSpeech(const char *strFilename,int channel=-1);
	/*获取音乐*/
	//CFmodMusic *GetMusic(const char *strFilename);

	CFmodMusic *GetMusicEx(const char *strFilename,DWORD dwMode);

	/*获取音乐*/
	CFmodMusic *FindMusic(const char *strFilename);

	/*释放Audio*/
	void ReleaseAudio(Audio *audio);

	/*释放声音*/
	//void ReleaseAllSounds();
	void StopAllSounds();

	/*释放语音*/
	//void ReleaseAllSounds();
	void StopAllSpeechs();

	/*释放声音*/
	//void ReleaseAllMusic();
	void StopAllMusic();

	/*释放全部资源*/
	void RelaseAll();

	void SetHW3DMaxChnnels(int nMax)	{m_nMaxHW3DChannels = nMax;}
	void SetHW2DMaxChnnels(int nMax)	{m_nMaxHW2DChannels = nMax;}
	void Fade(int nChannel,bool bFadeIn,int nMaxVolume,DWORD dwFadeTime);

	int GetHW3DMaxChnnels()             {return m_nMaxHW3DChannels;}
	int GetHW2DMaxChnnels()             {return m_nMaxHW2DChannels;}
};

#endif