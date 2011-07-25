#include "Precompiled.h"
#include "MsSound.h"
#pragma comment(lib, "fmod.lib")

CMsSound::CMsSound(DWORD dwSoundCount)
: m_pSystem         (NULL)
, m_IsInit          (FALSE)
, m_nFmodInitMode   (0)
, m_IsStartupSound  (FALSE)
, m_dwSoundCount    (dwSoundCount)
{
    _ENTER_FUN_C;

    // FMOD类型返回值
    FMOD_RESULT frRet = FMOD_OK;

    // 初始化声音数组为空
    m_ppSound = NEW lpSound[m_dwSoundCount];
    AssertEx(AL_CATAST, m_ppSound, _T("内存不足"));
    
    // 初始化音轨数组为空
    m_ppChannel = NEW lpChannel[m_dwSoundCount];
    AssertEx(AL_CATAST, m_ppChannel, _T("内存不足"));

    m_lpfVolume = NEW FLOAT[m_dwSoundCount];
    AssertEx(AL_CATAST, m_lpfVolume, _T("内存不足"));

    if (m_lpfVolume)
    {
        // 初始化音轨音量数组为默认值
        FOR_x_TO(i, m_dwSoundCount)
        {
            m_lpfVolume[i] = GAME_NORMAL_VOLUME;
        }
    }
    else
    {
        AssertEx(AL_NORMAL, m_lpfVolume, _T("m_lpfVolume为空"));
    }

    // 创建系统
    frRet = System_Create(&m_pSystem);
    AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("创建FMOD系统失败,函数失败"));

    _LEAVE_FUN_DISPOSE_END_C;
}

CMsSound::~CMsSound(VOID)
{
    _ENTER_FUN_C;

    // FMOD类型返回值
    FMOD_RESULT frRet = FMOD_OK;

    // 释放音轨资源
    for (DWORD i = 0; i< m_dwSoundCount; i++)
    {
        if (m_ppChannel[i])
        {
            BOOL bPlaying = this->IsPlaying(i);
            if (bPlaying)
            {
                // 停止播放
                frRet = m_ppChannel[i]->stop();
                AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("释放音轨资源失败,函数失败"));
                m_ppChannel[i] = NULL;
            }
        }
    }
    SAFE_DELETE_ARRAY(m_ppChannel);

    // 释放声音资源
    for (DWORD i = 0; i< m_dwSoundCount; i++)
    {
        if (m_ppSound[i])
        {
            frRet = m_ppSound[i]->release();
            AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("释放声音资源失败,函数失败"));
            m_ppSound[i] = NULL;
        }
    }
    SAFE_DELETE_ARRAY(m_ppSound);

    // 释放系统
    if (m_pSystem)
    {
        // 关闭音乐引擎系统
        frRet = m_pSystem->close();
        AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("关闭FMOD系统失败,函数失败"));

        // 释放音乐引擎系统资源
        frRet = m_pSystem->release();
        AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("释放音乐引擎系统资源失败,函数失败"));

        // 将系统指针置空
        m_pSystem = NULL;
    }

    SAFE_DELETE_ARRAY(m_lpfVolume);

    _LEAVE_FUN_DISPOSE_END_C;
}

// 初始化声音类
BOOL CMsSound::Init(BOOL b3DSound)
{
    _ENTER_FUN_C;

    // FMOD类型返回值
    FMOD_RESULT frRet = FMOD_OK;

    // 如果指定3D模式
    if (b3DSound)
    {
        // 获取声音类的模式
        m_nFmodInitMode = FMOD_INIT_DSOUND_HRTFNONE;
    }

    // 初始化
    frRet = m_pSystem->init(m_dwSoundCount, m_nFmodInitMode, NULL);
    AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("初始化FMOD系统失败,函数失败"));

    // 将初始化标记置真
    m_IsInit = TRUE;
    m_IsStartupSound = TRUE;

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 打开声音
BOOL CMsSound::Open(DWORD dwIndex, CONST ACHAR* strSoundName, BOOL IsLoop)
{
    _ENTER_FUN_C;

    // 不提供音频服务
    if (!m_IsStartupSound)
    {
        return TRUE;
    }

    // FMOD类型返回值
    FMOD_RESULT frRet = FMOD_OK;

    // 打开音频文件的模式
    INT FmodFlag = FMOD_HARDWARE;

    // 如果是3D播放模式
    if (m_nFmodInitMode == FMOD_INIT_DSOUND_HRTFFULL)
    {
        // 打开文件以3D模式
        FmodFlag |= FMOD_3D;
    }

    // 是否循环
    if (IsLoop)
    {
        // 默认循环模式
        FmodFlag |= FMOD_LOOP_NORMAL;
    }

    // 不是循环方式
    else
    {
        // 关闭循环
        FmodFlag |= FMOD_LOOP_OFF;
    }

    // 初始化
    frRet = m_pSystem->createSound(
        strSoundName,
        FmodFlag,
        NULL,
        &m_ppSound[dwIndex]
    );

    AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("createSound失败,函数失败"));

    // 将初始化标记置真
    m_IsInit = TRUE;

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}


// 播放声音
BOOL CMsSound::Play(DWORD dwIndex, BOOL bPaused, BOOL IsMid)
{
    _ENTER_FUN_C;

    // 不提供音频服务
    if (!m_IsStartupSound)
    {
        return TRUE;
    }

    // FMOD类型返回值
    FMOD_RESULT frRet = FMOD_OK;

    // 播放声音
    frRet = m_pSystem->playSound(
        FMOD_CHANNEL_REUSE,
        m_ppSound[dwIndex],
        BOOL_To_bool(bPaused),
        &m_ppChannel[dwIndex]);
    AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("播放声音失败,函数失败"));

    if (IsMid)
    {
        ::SleepEx(20, TRUE);
    }

    // 设置音量
    frRet = m_ppChannel[dwIndex]->setVolume(m_lpfVolume[dwIndex]);
    AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("设置音量失败,函数失败"));

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 暂停播放
BOOL CMsSound::Pause(DWORD dwIndex)
{
    _ENTER_FUN_C;

    // 不提供音频服务
    if (!m_IsStartupSound)
    {
        return TRUE;
    }

    // FMOD类型返回值
    FMOD_RESULT frRet = FMOD_OK;

    // 暂停播放
    BOOL bPlaying = this->IsPlaying(dwIndex);
    if (bPlaying)
    {
        frRet = m_ppChannel[dwIndex]->setPaused(TRUE);
        AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("暂停播放失败,函数失败"));
    }

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 继续播放
BOOL CMsSound::Continue(DWORD dwIndex)
{
    _ENTER_FUN_C;

    // 不提供音频服务
    if (!m_IsStartupSound)
    {
        return TRUE;
    }

    // FMOD类型返回值
    FMOD_RESULT frRet = FMOD_OK;

    // 暂停播放
    bool bPaused;
    m_ppChannel[dwIndex]->getPaused(&bPaused);
    if (bPaused)
    {
        // 继续播放
        frRet = m_ppChannel[dwIndex]->setPaused(FALSE);
        AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("继续播放失败,函数失败"));
    }

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 停止播放
BOOL CMsSound::Stop(DWORD dwIndex)
{
    _ENTER_FUN_C;

    // 不提供音频服务
    if (!m_IsStartupSound)
    {
        return TRUE;
    }

    // FMOD类型返回值
    FMOD_RESULT frRet = FMOD_OK;

    // 如果音轨中有声音
    if (m_ppChannel[dwIndex])
    {
        BOOL bPlaying = this->IsPlaying(dwIndex);
        if (bPlaying)
        {
            // 停止播放
            frRet = m_ppChannel[dwIndex]->stop();
            AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("停止播放失败,函数失败"));
        }
        // 将音轨置空
        m_ppChannel[dwIndex] = NULL;
    }

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 设置音量
BOOL CMsSound::SetVolume(DWORD dwIndex, FLOAT fVolume)
{
    _ENTER_FUN_C;

    // 不提供音频服务
    if (!m_IsStartupSound)
    {
        return TRUE;
    }

    // FMOD类型返回值
    FMOD_RESULT frRet = FMOD_OK;

    // 如果音量过大
    if (fVolume > VOLUME_MAX_VALUE)
    {
        // 等于最大音量
        fVolume = VOLUME_MAX_VALUE;
    }
    // 如果音量过小
    else if (fVolume < VOLUME_MIN_VALUE)
    {
        // 等于最小音量
        fVolume = VOLUME_MIN_VALUE;
    }

    // 更新音量
    m_lpfVolume[dwIndex] = fVolume;
    frRet = m_ppChannel[dwIndex]->setVolume(m_lpfVolume[dwIndex]);
    AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("音量更新失败,函数失败"));

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 增加音量
BOOL CMsSound::AddVolume(DWORD dwIndex)
{
    _ENTER_FUN_C;

    // 不提供音频服务
    if (!m_IsStartupSound)
    {
        return TRUE;
    }

    // FMOD类型返回值
    FMOD_RESULT frRet = FMOD_OK;

    // 如果音量已经最大
    if (m_lpfVolume[dwIndex] >= VOLUME_MAX_VALUE)
    {
        // 返回成功
        return TRUE;
    }

    // 增加音量
    m_lpfVolume[dwIndex] += VOLUME_CHANGE_ONCE_VALUE;

    // 更新音量
    frRet = m_ppChannel[dwIndex]->setVolume(m_lpfVolume[dwIndex]);
    AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("音量更新失败,函数失败"));

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 减少音量
BOOL CMsSound::SubVolume(DWORD dwIndex)
{
    _ENTER_FUN_C;

    // 不提供音频服务
    if (!m_IsStartupSound)
    {
        return TRUE;
    }

    // FMOD类型返回值
    FMOD_RESULT frRet = FMOD_OK;

    // 如果音量已经最大
    if (m_lpfVolume[dwIndex] <= VOLUME_MIN_VALUE)
    {
        // 返回成功
        return TRUE;
    }

    // 减少音量
    m_lpfVolume[dwIndex] -= VOLUME_CHANGE_ONCE_VALUE;

    // 更新音量
    frRet = m_ppChannel[dwIndex]->setVolume(m_lpfVolume[dwIndex]);
    AssertEx(AL_NORMAL, FMOD_OK == frRet, _T("音量更新失败,函数失败"));

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 是否正在播放
BOOL CMsSound::IsPlaying(DWORD dwIndex)
{
    _ENTER_FUN_C;

    // 不提供音频服务
    if (!m_IsStartupSound)
    {
        return FALSE;
    }

    // 布尔类型返回值
    bool bRet = TRUE;
    if (m_ppChannel[dwIndex])
    {
        m_ppChannel[dwIndex]->isPlaying(&bRet);
        return bRet;
    }
    else
    {
         return FALSE;
    }

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}
