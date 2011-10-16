/*-------------------------------------------------------------------------------------------------------------------------------------------

游戏客户端声音播放和管理的工具类,隔离客户端程序逻辑和声音引擎的具体实现,便于使用和维护

作者	:	汪亚军
日期	:	2004-12-02

这个类的目的是为了简化客户端程序对声音的使用和控制,是其他程序不用关心声音引擎的具体细节
在播放和控制声音的时候,以文件名来简化对对象的操作,对音乐和2D声音一个文件名对应一个声音对
象;如果是3D声音,还有一个channel信息区分同一个文件产生的不同的声音.

---------------------------------------------------------------------------------------------------------------------------------------------*/
#pragma once
#include "../clientregion.h"
class CFmodEngine;
class CShape;
class CBaseObject;
class CFmodAudio;
class CFmodSound;
class CFmodMusic;
class CFmoEngine;

typedef D3DXVECTOR3 Vector3;

//客户端的声音管理控制类
class CAudioList
{
private:
	//引擎实例
	CFmodEngine *m_pAudioEngine;
	//背景音乐文件名
	string m_strMusicFile;
	//背景音乐的渐变状态
	bool  m_bFadeState;

private:
public:	// soft 2004.12.23
	//加载配置信息
	void LoadSetup(const char *filename);

public:
	//构造
	CAudioList(void);
	//释放
	virtual ~CAudioList(void);

public:
    
	/*根据CShape的类型动作和动作播放声音*/
	int Play(CShape * pShape, long lType, WORD wAction, Vector3 vRole, bool loop=false, int nLoops = 1);

	/*播放场景声音*/
	int Play(CClientRegion::REGION_SOUND_PTR pSound,long lRegionID,bool loop=false, int nLoops = 1);

	/*播放声音文件*/
	int Play(const char* filename, bool loop=false, bool bEnv = false, int nLoops = 1);

	/*播放声音文件,3D*/
	int Play(const char* filename, float fX, float fY, float fZ, bool loop=false, int nLoops = 1);

	/*播放音乐*/
	void PlayMusic( const char *filename,int loops=0 );

	/*渐入音乐*/
	void FadeInMusic( const char *filename,unsigned int time,int loops=0 );

	/*渐出音乐*/
	void FadeOutMusic( const char *filename,unsigned int time,int loops=0 );

	/*渐变音乐*/
	void FadeMusic( const char *filename,unsigned int time,float Volume,int loops=0 );

	bool GetFadeState(){return m_bFadeState;};

	void SetFadeState(bool b);

	/*停止音乐*/
	void StopMusic(const char *filename);

	/*释放音乐*/
	void ReleaseMusic(const char *filename);

	/*播放2D声音*/
	int Play2DSound( const char *filename,int loops=1 );

	/*播放语音*/
	int PlaySpeech( const char *filename,int loops=1 );

	/*停止语音*/
	void StopSpeech( const char *filename,int channel );

	/*播放3D语音*/
	int Play3DSpeech(const char *filename,int loops,float x,float y,float z);

	/*渐入2D声音*/
	int FadeIn2DSound( const char *filename,unsigned int time,int loops=0 );

	/*渐出2D声音*/
	int FadeOut2DSound( const char *filename,unsigned int time,int loops=0 );

	/*停止2D声音*/
	//void Stop2DSound( const char *filename );

	/*释放2D声音*/
	void Release2DSound( const char *filename );

	/*播放3D声音*/
	int Play3DSound( const char *filename,int loops,float x,float y,float z );

	///*渐入3D声音*/
	//int FadeIn3DSound( const char *filename,float x,float y,float z,unsigned int time,int loops=0 );

	///*渐出3D声音*/
	//int FadeOut3DSound( const char *filename,float x,float y,float z,unsigned int time,int loops=0 );

	/*停止3D声音*/
	void Stop3DSound( const char *filename,int channel );

	/*停止3D声音*/
	void Stop3DSound( int channel );

	/*释放3D声音*/
	void Release3DSound( const char *filename,int channel );

	/*设置3D声音的位置*/
	//void Set3DSoundPosition(const char *filename,int &channel,float x,float y,float z );

	/*更新声音系统状态*/
	void UpdateAudio();

	void Run();

	/*设置音乐音量*/
	void SetMusicVolume(float fVolume);

	/*设置声音音量*/
	void SetSoundVolume(float fVolume);

	/*设置语音音量*/
	void SetSpeechVolume(float fVolume);

	/*设置声音是否打开*/
	void SetSoundEnabled(bool b);
	/*设置语音是否打开*/
	void SetSpeechEnabled(bool b);

	/*设置音乐是否打开*/
	void SetMusicEnabled(bool b);

	/*判断该文件的声音是否在播放,channel为-1表示2D声音或者音乐*/
	bool IsPlaying(const char *filename,int channel=-1);

	/*设置耳朵的位置*/
	void SetListenerPosition(float *pos);

	/*设置耳朵位置*/
	void SetListenerPosition(float x,float y,float z);

	/*设置耳朵速度*/
	void SetListenerVelocity(float *vel);

	/*设置耳朵速度*/
	void SetListenerVelocity(float x,float y,float z);

	/*设置耳朵方向*/
	void SetListenerOritation(float *face,float *up);

	/*设置耳朵方向*/
	void SetListenerOritation(float facex,float facey,float facez,float upx,float upy,float upz);

	/*播放装备物品物品的声音*/
	void PlayEquipmentSound(DWORD dwIndex,bool b3D=false,float x=0,float y=0,float z=0);

	/*设置背景音乐文件名*/
	void SetBackgroundMusic(const char *filename);

	/*获取背景音乐文件名*/
	const char *GetBackgroundMusic();

	/*垃圾回收*/
	void GarbageColloction();
private:
	long m_lPlayerInterTime;     // 玩家点击进入游戏的时间
	bool m_bPlayMonsterOppear;   // 是否播放怪物出现的音效
	long m_lPlayOppearSoundTime; // 播放怪物出现音效的时间
	void CountMonsterOppear();     //计算怪物出现的时间，用于控制玩家进入游戏时不会立即播放怪物出现的音效
public:
	void SetOppearSoundEnble(bool l);//设置是否播放出现音效
};
