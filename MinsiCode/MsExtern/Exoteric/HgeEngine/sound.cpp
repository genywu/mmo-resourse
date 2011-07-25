#include "Precompiled.h"
/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: audio routines
*/


#include "hge_impl.h"


#define BASSDEF(f) (WINAPI *f)    // define the functions as pointers
#include "bass.h"

#define LOADBASSFUNCTION(f) *((VOID**)&f)=(VOID*)GetProcAddress(hBass,#f)


HEFFECT CALL HGE_Impl::Effect_Load(CONST TCHAR *filename, DWORD size)
{
    DWORD _size, length, samples;
    HSAMPLE hs;
    HSTREAM hstrm;
    BASS_CHANNELINFO info;
    VOID *buffer, *data;

    if(hBass)
    {
        if(bSilent) return 1;

        if(size) { data=(VOID *)filename; _size=size; }
        else
        {
            data=Resource_Load(filename, &_size);
            if(!data) return NULL;
        }
    
        hs=BASS_SampleLoad(TRUE, data, 0, _size, 4, BASS_SAMPLE_OVER_VOL);
        if(!hs) {
            hstrm=BASS_StreamCreateFile(TRUE, data, 0, _size, BASS_STREAM_DECODE);
            if(hstrm) {
                length=(DWORD)BASS_ChannelGetLength(hstrm);
                BASS_ChannelGetInfo(hstrm, &info);
                samples=length;
                if(info.chans < 2) samples>>=1;
                if((info.flags & BASS_SAMPLE_8BITS) == 0) samples>>=1;
                buffer=BASS_SampleCreate(samples, info.freq, 2, 4, info.flags | BASS_SAMPLE_OVER_VOL);
                if(!buffer)
                {
                    BASS_StreamFree(hstrm);
                    _PostError(_T("Can't create sound effect: Not enough memory"));
                }
                else
                {
                    BASS_ChannelGetData(hstrm, buffer, length);
                    hs=BASS_SampleCreateDone();
                    BASS_StreamFree(hstrm);
                    if(!hs)    _PostError(_T("Can't create sound effect"));
                }
            }
        }

        if(!size) Resource_Free(data);
        return hs;
    }
    else return 0;
}

HCHANNEL CALL HGE_Impl::Effect_Play(HEFFECT eff)
{
    if(hBass)
    {
        HCHANNEL chn;
        chn=BASS_SampleGetChannel(eff, FALSE);
        BASS_ChannelPlay(chn, TRUE);
        return chn;
    }
    else return 0;
}

HCHANNEL CALL HGE_Impl::Effect_PlayEx(HEFFECT eff, INT volume, INT pan, FLOAT pitch, BOOL loop)
{
    if(hBass)
    {
        BASS_SAMPLE info;
        HCHANNEL chn;
        BASS_SampleGetInfo(eff, &info);

        chn=BASS_SampleGetChannel(eff, FALSE);
        BASS_ChannelSetAttributes(chn, (INT)(pitch*info.freq), volume, pan);

        info.flags &= ~BASS_SAMPLE_LOOP;
        if(loop) info.flags |= BASS_SAMPLE_LOOP;
        BASS_ChannelSetFlags(chn, info.flags);
        BASS_ChannelPlay(chn, TRUE);
        return chn;
    }
    else return 0;
}


VOID CALL HGE_Impl::Effect_Free(HEFFECT eff)
{
    if(hBass) BASS_SampleFree(eff);
}


HMUSIC CALL HGE_Impl::Music_Load(CONST TCHAR *filename, DWORD size)
{
    VOID *data;
    DWORD _size;
    HMUSIC hm;

    if(hBass)
    {
        if(size)
        {
            data=(VOID *)filename;
            _size=size;
        }
        else
        {
            data=Resource_Load(filename, &_size);
            if(!data) return 0;
        }

        hm=BASS_MusicLoad(TRUE, data, 0, 0, BASS_MUSIC_PRESCAN | BASS_MUSIC_POSRESETEX | BASS_MUSIC_RAMP, 0);
        if(!hm)    _PostError(_T("Can't load music"));
        if(!size) Resource_Free(data);
        return hm;
    }
    else return 0;
}

HCHANNEL CALL HGE_Impl::Music_Play(HMUSIC mus, BOOL loop, INT volume, INT order, INT row)
{
    if(hBass)
    {
        DWORD pos = BASS_MusicGetOrderPosition(mus);
        if(order == -1) order = LOWORD(pos);
        if(row == -1) row = HIWORD(pos);
        BASS_ChannelSetPosition(mus, MAKEMUSICPOS(order, row));

        BASS_CHANNELINFO info;
        BASS_ChannelGetInfo(mus, &info);
        BASS_ChannelSetAttributes(mus, info.freq, volume, 0);

        info.flags &= ~BASS_SAMPLE_LOOP;
        if(loop) info.flags |= BASS_SAMPLE_LOOP;
        BASS_ChannelSetFlags(mus, info.flags);

        BASS_ChannelPlay(mus, FALSE);

        return mus;
    }
    else return 0;
}

VOID CALL HGE_Impl::Music_Free(HMUSIC mus)
{
    if(hBass) BASS_MusicFree(mus);
}

VOID CALL HGE_Impl::Music_SetAmplification(HMUSIC music, INT ampl)
{
    if(hBass) BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_AMPLIFY, ampl);
}

INT CALL HGE_Impl::Music_GetAmplification(HMUSIC music)
{
    if(hBass) return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_AMPLIFY);
    else return -1;
}

INT CALL HGE_Impl::Music_GetLength(HMUSIC music)
{
    if(hBass)
    {
        return BASS_MusicGetOrders(music);
    }
    else return -1;
}

VOID CALL HGE_Impl::Music_SetPos(HMUSIC music, INT order, INT row)
{
    if(hBass)
    {
        BASS_ChannelSetPosition(music, MAKEMUSICPOS(order, row));
    }
}

BOOL CALL HGE_Impl::Music_GetPos(HMUSIC music, INT *order, INT *row)
{
    if(hBass)
    {
        DWORD pos;
        pos = BASS_MusicGetOrderPosition(music);
        if(pos == -1) return FALSE;
        *order = LOWORD(pos);
        *row = HIWORD(pos);
        return TRUE;
    }
    else return FALSE;
}

VOID CALL HGE_Impl::Music_SetInstrVolume(HMUSIC music, INT instr, INT volume)
{
    if(hBass)
    {
        BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_VOL_INST + instr, volume);        
    }
}

INT CALL HGE_Impl::Music_GetInstrVolume(HMUSIC music, INT instr)
{
    if(hBass)
    {
        return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_VOL_INST + instr);        
    }
    else return -1;
}

VOID CALL HGE_Impl::Music_SetChannelVolume(HMUSIC music, INT channel, INT volume)
{
    if(hBass)
    {
        BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_VOL_CHAN + channel, volume);        
    }
}

INT CALL HGE_Impl::Music_GetChannelVolume(HMUSIC music, INT channel)
{
    if(hBass)
    {
        return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_VOL_CHAN + channel);        
    }
    else return -1;
}

HSTREAM CALL HGE_Impl::Stream_Load(CONST TCHAR *filename, DWORD size)
{
    VOID *data;
    DWORD _size;
    HSTREAM hs;
    CStreamList *stmItem;

    if(hBass)
    {
        if(bSilent) return 1;

        if(size) { data=(VOID *)filename; _size=size; }
        else
        {
            data=Resource_Load(filename, &_size);
            if(!data) return 0;
        }
        hs=BASS_StreamCreateFile(TRUE, data, 0, _size, 0);
        if(!hs)
        {
            _PostError(_T("Can't load stream"));
            if(!size) Resource_Free(data);
            return 0;
        }
        if(!size)
        {
            stmItem=new CStreamList;
            stmItem->hstream=hs;
            stmItem->data=data;
            stmItem->next=streams;
            streams=stmItem;
        }
        return hs;
    }
    else return 0;
}

VOID CALL HGE_Impl::Stream_Free(HSTREAM stream)
{
    CStreamList *stmItem=streams, *stmPrev=0;

    if(hBass)
    {
        while(stmItem)
        {
            if(stmItem->hstream==stream)
            {
                if(stmPrev) stmPrev->next=stmItem->next;
                else streams=stmItem->next;
                Resource_Free(stmItem->data);
                delete stmItem;
                break;
            }
            stmPrev=stmItem;
            stmItem=stmItem->next;
        }
        BASS_StreamFree(stream);
    }
}

HCHANNEL CALL HGE_Impl::Stream_Play(HSTREAM stream, BOOL loop, INT volume)
{
    if(hBass)
    {
        BASS_CHANNELINFO info;
        BASS_ChannelGetInfo(stream, &info);
        BASS_ChannelSetAttributes(stream, info.freq, volume, 0);

        info.flags &= ~BASS_SAMPLE_LOOP;
        if(loop) info.flags |= BASS_SAMPLE_LOOP;
        BASS_ChannelSetFlags(stream, info.flags);
        BASS_ChannelPlay(stream, TRUE);
        return stream;
    }
    else return 0;
}

VOID CALL HGE_Impl::Channel_SetPanning(HCHANNEL chn, INT pan)
{
    if(hBass) BASS_ChannelSetAttributes(chn, -1, -1, pan);
}

VOID CALL HGE_Impl::Channel_SetVolume(HCHANNEL chn, INT volume)
{
    if(hBass) BASS_ChannelSetAttributes(chn, -1, volume, -101);
}

VOID CALL HGE_Impl::Channel_SetPitch(HCHANNEL chn, FLOAT pitch)
{
    if(hBass)
    {
        BASS_CHANNELINFO info;
        BASS_ChannelGetInfo(chn, &info);
        BASS_ChannelSetAttributes(chn, (INT)(pitch*info.freq), -1, -101);
    }
}

VOID CALL HGE_Impl::Channel_Pause(HCHANNEL chn)
{
    if(hBass) BASS_ChannelPause(chn);
}

VOID CALL HGE_Impl::Channel_Resume(HCHANNEL chn)
{
    if(hBass) BASS_ChannelPlay(chn, FALSE);
}

VOID CALL HGE_Impl::Channel_Stop(HCHANNEL chn)
{
    if(hBass) BASS_ChannelStop(chn);
}

VOID CALL HGE_Impl::Channel_PauseAll()
{
    if(hBass) BASS_Pause();
}

VOID CALL HGE_Impl::Channel_ResumeAll()
{
    if(hBass) BASS_Start();
}

VOID CALL HGE_Impl::Channel_StopAll()
{
    if(hBass)
    {
        BASS_Stop();
        BASS_Start();
    }
}

BOOL CALL HGE_Impl::Channel_IsPlaying(HCHANNEL chn)
{
    if(hBass)
    {
        if(BASS_ChannelIsActive(chn)==BASS_ACTIVE_PLAYING) return TRUE;
        else return FALSE;
    }
    else return FALSE;
}

FLOAT CALL HGE_Impl::Channel_GetLength(HCHANNEL chn) {
    if(hBass)
    {
        return BASS_ChannelBytes2Seconds(chn, BASS_ChannelGetLength(chn));
    }
    else return -1;
}

FLOAT CALL HGE_Impl::Channel_GetPos(HCHANNEL chn) {
    if(hBass)
    {
        return BASS_ChannelBytes2Seconds(chn, BASS_ChannelGetPosition(chn));
    }
    else return -1;
}

VOID CALL HGE_Impl::Channel_SetPos(HCHANNEL chn, FLOAT fSeconds) {
    if(hBass)
    {
        BASS_ChannelSetPosition(chn, BASS_ChannelSeconds2Bytes(chn, fSeconds));
    }
}

VOID CALL HGE_Impl::Channel_SlideTo(HCHANNEL channel, FLOAT time, INT volume, INT pan, FLOAT pitch)
{
    if(hBass)
    {
        BASS_CHANNELINFO info;
        BASS_ChannelGetInfo(channel, &info);

        INT freq;
        if(pitch == -1) freq = -1;
        else freq = (INT)(pitch*info.freq);

        BASS_ChannelSlideAttributes(channel, freq, volume, pan, DWORD(time*1000));
    }
}

BOOL CALL HGE_Impl::Channel_IsSliding(HCHANNEL channel)
{
    if(hBass)
    {
        if(BASS_ChannelIsSliding(channel)) return TRUE;
        else return FALSE;
    }
    else return FALSE;
}


//////// Implementation ////////


BOOL HGE_Impl::_SoundInit()
{
    if(!bUseSound || hBass) return TRUE;

    hBass=LoadLibraryA("bass.dll");
    if (!hBass)
    {
        _PostError(_T("Can't load BASS.DLL"));
        return FALSE;
    }

    LOADBASSFUNCTION(BASS_GetVersion);

    if (HIWORD(BASS_GetVersion()) != BASSVERSION)
    {
        _PostError(_T("Incorrect BASS.DLL version"));
        return FALSE;
    }

    LOADBASSFUNCTION(BASS_GetDeviceDescription);
    LOADBASSFUNCTION(BASS_Init);
    LOADBASSFUNCTION(BASS_Free);
    LOADBASSFUNCTION(BASS_Start);
    LOADBASSFUNCTION(BASS_Pause);
    LOADBASSFUNCTION(BASS_Stop);
    LOADBASSFUNCTION(BASS_SetConfig);
    //LOADBASSFUNCTION(BASS_ErrorGetCode);

    LOADBASSFUNCTION(BASS_SampleLoad);
    LOADBASSFUNCTION(BASS_SampleCreate);
    LOADBASSFUNCTION(BASS_SampleCreateDone);
    LOADBASSFUNCTION(BASS_SampleGetInfo);
    LOADBASSFUNCTION(BASS_SampleGetChannel);
    LOADBASSFUNCTION(BASS_SampleFree);
    
    LOADBASSFUNCTION(BASS_MusicLoad);
    LOADBASSFUNCTION(BASS_MusicFree);
    LOADBASSFUNCTION(BASS_MusicGetOrders);
    LOADBASSFUNCTION(BASS_MusicGetOrderPosition);
    LOADBASSFUNCTION(BASS_MusicSetAttribute);
    LOADBASSFUNCTION(BASS_MusicGetAttribute);

    LOADBASSFUNCTION(BASS_StreamCreateFile);
    LOADBASSFUNCTION(BASS_StreamFree);
    
    LOADBASSFUNCTION(BASS_ChannelGetInfo);
    LOADBASSFUNCTION(BASS_ChannelGetAttributes);
    LOADBASSFUNCTION(BASS_ChannelSetAttributes);
    LOADBASSFUNCTION(BASS_ChannelSlideAttributes);
    LOADBASSFUNCTION(BASS_ChannelIsSliding);
    LOADBASSFUNCTION(BASS_ChannelSetFlags);
    LOADBASSFUNCTION(BASS_ChannelGetData);
    LOADBASSFUNCTION(BASS_ChannelPlay);
    LOADBASSFUNCTION(BASS_ChannelPause);
    LOADBASSFUNCTION(BASS_ChannelStop);
    LOADBASSFUNCTION(BASS_ChannelIsActive);
    LOADBASSFUNCTION(BASS_ChannelGetLength);
    LOADBASSFUNCTION(BASS_ChannelGetPosition);
    LOADBASSFUNCTION(BASS_ChannelSetPosition);
    LOADBASSFUNCTION(BASS_ChannelSeconds2Bytes);
    LOADBASSFUNCTION(BASS_ChannelBytes2Seconds);

    bSilent=FALSE;
    if (!BASS_Init(-1,nSampleRate,0,hwnd,NULL))
    {
        System_Log(_T("BASS Init failed, using no sound"));
        BASS_Init(0,nSampleRate,0,hwnd,NULL);
        bSilent=TRUE;
    }
    else
    {
        System_Log(_T("Sound Device: %s"),BASS_GetDeviceDescription(1));
        System_Log(_T("Sample rate: %ld\n"), nSampleRate);
    }

    //BASS_SetConfig(BASS_CONFIG_BUFFER, 5000);
    //BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 50);

    _SetFXVolume(nFXVolume);
    _SetMusVolume(nMusVolume);
    _SetStreamVolume(nStreamVolume);

    return TRUE;
}

VOID HGE_Impl::_SoundDone()
{
    CStreamList *stmItem=streams, *stmNext;
    
    if(hBass)
    {
        BASS_Stop();
        BASS_Free();

        //INT err = BASS_ErrorGetCode(); 

        FreeLibrary(hBass);
        hBass=0;

        while(stmItem)
        {
            stmNext=stmItem->next;
            Resource_Free(stmItem->data);
            delete stmItem;
            stmItem=stmNext;
        }
        streams=0;
    }
}

VOID HGE_Impl::_SetMusVolume(INT vol)
{
    if(hBass) BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, vol);
}

VOID HGE_Impl::_SetStreamVolume(INT vol)
{
    if(hBass) BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, vol);
}

VOID HGE_Impl::_SetFXVolume(INT vol)
{
    if(hBass) BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, vol);
}






