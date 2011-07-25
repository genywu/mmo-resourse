#pragma once
#include "fmod.hpp"
using namespace FMOD;

// 每次改变音量的值
#define VOLUME_CHANGE_ONCE_VALUE    0.05f

// 音量最大值
#define VOLUME_MAX_VALUE            2.0f

// 音量最小值
#define VOLUME_MIN_VALUE            0.0f

// 默认音量
#define GAME_NORMAL_VOLUME          0.8f
typedef Sound* lpSound;
typedef Channel* lpChannel;

class CMsSound
{
public: // 构造、析构
    CMsSound(DWORD dwSoundCount);
    ~CMsSound(VOID);

public: // 接口

    BOOL Open(      // 打开声音
        DWORD dwIndex,
        CONST ACHAR* strSoundName,
        BOOL IsLoop
        );

    BOOL Play(      // 播放声音
        DWORD dwIndex,
        BOOL bPaused,
        BOOL IsMid = FALSE
        );

    BOOL SetVolume( // 设置音量
        DWORD dwIndex,
        FLOAT fVolume
        );

    BOOL Init(BOOL b3DSound);       // 初始化声音类
    BOOL Pause(DWORD dwIndex);      // 暂停播放
    BOOL Continue(DWORD dwIndex);   // 继续播放
    BOOL Stop(DWORD dwIndex);       // 停止播放
    BOOL AddVolume(DWORD dwIndex);  // 增加音量
    BOOL SubVolume(DWORD dwIndex);  // 减少音量
    BOOL IsPlaying(DWORD dwIndex);  // 是否正在播放

private:// 私有
    
    INT         m_nFmodInitMode;    // 声音类的创建模式
    BOOL        m_IsInit;           // 声音类是否初始化标记
    System*     m_pSystem;          // 引擎系统指针
    DWORD       m_dwSoundCount;     // 加载声音个数
    lpSound*    m_ppSound;          // 声音指针数组
    lpChannel*  m_ppChannel;        // 通道指针数组
    LPFLOAT     m_lpfVolume;        // 音量
    BOOL        m_IsStartupSound;   // 是否启动音频
};
