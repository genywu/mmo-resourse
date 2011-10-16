#include "stdafx.h"
#include "Other/AudioList.h"
#include "../Audio/FmodAudioDef.h"
#include "Player.h"
#include "Goods/Goods.h"
#include "Monster.h"
#include "Citygate.h"
#include "../GameClient/Game.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


HANDLE hAudioEngineThreadHandle=0;		//更新声音系统状态的线程的线程句柄
bool bAndioEngineRunning=true;			//指示更新声音系统状态线程是否运行
// 更新声音状态的线程
unsigned __stdcall AudioEngineThreadFunc( void* pArguments ) 
{
	CAudioList *pAudioList=(CAudioList *)pArguments;
	while( bAndioEngineRunning )
	{
		pAudioList->UpdateAudio();
		Sleep(50);
	}
	_endthreadex( 0 );
    return 0; 
} 

//构造声音工具
//参数说明
//	无
//返回说明
//	无
CAudioList::CAudioList(void) : m_strMusicFile("")
{
	m_pAudioEngine=CFmodEngine::GetInstance(g_hWnd);
	if(m_pAudioEngine==NULL)
	{
		MessageBox(g_hWnd,"创建声音引擎失败！","错误",MB_ICONERROR);
		return;
	}
	m_lPlayerInterTime = 0;
	m_bPlayMonsterOppear = true;
	LoadSetup("data\\other.ini");
	m_bFadeState = false;
	
	unsigned threadID;
	hAudioEngineThreadHandle = (HANDLE)_beginthreadex(NULL,0,AudioEngineThreadFunc,this,0,&threadID);
}

//释放删除声音工具
//参数说明
//	无
//返回说明
//	无
CAudioList::~CAudioList(void)
{
	if(m_pAudioEngine==NULL)return;
	bAndioEngineRunning=false;
	WaitForSingleObject(hAudioEngineThreadHandle,100);
	CloseHandle(hAudioEngineThreadHandle);
	delete m_pAudioEngine;
}

//播放场景声音
//参数说明
//	pShape:CShape对象	lType:CShape类型	wAction:相应的动作	vRole:作用位置	loop:是否循环	nLoops:重复播放次数
//返回说明
//	返回播放该声音的channel,-1表示播放失败
int CAudioList::Play(CShape * pShape, long lType, WORD wAction, Vector3 vRole, bool loop, int nLoops)
{
	if(m_pAudioEngine==NULL)return 0;
	static char *strAction[] = {"stand","idle","walk", "run", "hurt", "inton","block","parry","miss","mergrim","dead","open","serhurt"};
	char strbuf[MAX_PATH]="";
	
	bool bEnv = false;
	//攻击伤害
	enum 
	{
		ACT_HURT01 = 4,			// 普通
		ACT_HURT02 = 12,		// 被重击	
	};
	switch(lType)
	{
	// 玩家音效
	case TYPE_PLAYER:
		{
			CPlayer * pPlayer = dynamic_cast<CPlayer*>(pShape);
			if( wAction < 12 && wAction != CShape::ACT_INTONATE &&
				wAction != CShape::ACT_MEGRIM && wAction != CShape::ACT_OPEN)
			{
				sprintf(strbuf,	"sounds/players/p%03d%s.wav", pPlayer->GetGraphicsID(), strAction[wAction]);
			}
		}
		break;

	// 物品音效
	case TYPE_GOODS:
		{
			CGoods * pGoods = dynamic_cast<CGoods*>(pShape);
			if (pGoods==NULL)
				sprintf(strbuf,	"sounds/equipments/%05d.wav", wAction);

			switch ( wAction )
			{
				// 丢弃
			case CGoods::GOOD_DROP:
				sprintf(strbuf,	"sounds/equipments/%05d.wav", pGoods->GetProperty()->dwSound);
				break;

				// 铠甲被弱伤害以及普通武器挥动
			case CGoods::GOOD_NORMAL:
				{
					CGoodsList::tagBaseProperty * t = pGoods->GetProperty();
					sprintf(strbuf,	"sounds/equipments/%05d.wav", pGoods->GetProperty()->dwSoundID1);
				}
				break;

				// 铠甲被伤害以及特殊武器挥动
			case CGoods::GOOD_EXCEPT:
				{
					CGoodsList::tagBaseProperty * p = pGoods->GetProperty();
					sprintf(strbuf,	"sounds/equipments/%05d.wav", pGoods->GetProperty()->dwSoundID2);
				}
				break;
			}
		}
		break;

	// 怪物音效
	case TYPE_MONSTER:
		{
			CMonster * pMonster = dynamic_cast<CMonster*>(pShape);
			if (pMonster==NULL)
				return 0;

			if (wAction == 0xFFFF&&m_bPlayMonsterOppear)	// 出现音效
			{
				sprintf(strbuf,	"sounds/monsters/m%03dappear.wav", pMonster->GetSoundID());
			}
			//怪物只播放受伤和死亡音效
			else if(wAction==CShape::ACT_BEATTACK || wAction==CShape::ACT_DIED || wAction == ACT_HURT02)	//攻击音效 
			{
				sprintf(strbuf,	"sounds/monsters/m%03d%s.wav", pMonster->GetSoundID(), strAction[wAction]);
			}
			else if(wAction == 0xFFF0)
			{
				sprintf(strbuf,	"sounds/monsters/m%03dattack.wav", pMonster->GetSoundID());
			}
		}
		break;

	// 地表音效
	case TYPE_REGION:
		{
			if (pShape == NULL)
				return 0;
			CPlayer * pPlayer = dynamic_cast<CPlayer*>(pShape);
			if(pPlayer == NULL)
				return 0;
			CClientRegion::tagCell *pCell = GetGame()->GetRegion()->GetCell(pShape->GetTileX(),pShape->GetTileY());
			if (pCell)
			{
				//sprintf(strbuf,"sounds/regions/r%03d%s.wav", pCell->lMaterial,strAction[wAction]);
				//根据人物是否在乘骑状态播放相应的音效，如果人物在乘骑状态，则要先获得坐骑的类型。
				if(!pPlayer->IsRide())
				{
					long Id = pPlayer->GetGraphicsID();
	                 sprintf(strbuf,"sounds/regions/r%03d%s.wav", pCell->lMaterial,strAction[wAction]);
				}else{ 
					 CGoods *goodhuose = pPlayer->GetEquip(CPlayer::EQUIP_RIDER);
					 if(NULL == goodhuose)
						 return 0;
					 long houseIndex = goodhuose->GetIndex();
					 //获得坐骑的武器对应的动作类型，以此来区分不同坐骑的音效。
					 //DWORD ActType = CGoodsList::GetPropertyByEquipID(houseIndex)->BaseProperty.dwWeaponActType;
					 CGoods *pGoods = NULL;
					 pGoods=new CGoods();
				     CGoodsList::FillGoodsInfo(houseIndex,pGoods);
					 DWORD ActType = pGoods->GetProperty()->dwWeaponActType;
					 //SAFE__DELETE(pGoods);
					 delete pGoods;
					 sprintf(strbuf,"sounds/regions/rh%02d%03d%s.wav", ActType,pCell->lMaterial,strAction[wAction]);

				}
			}
		}
		break;

	// 技能音效
	case TYPE_SKILL:
		{
			sprintf(strbuf,	"sounds/skills/%03d.wav", wAction);
		}
		break;

	case TYPE_CITYGATE:
		{
			CCityGate * pCityGate = dynamic_cast<CCityGate*>(pShape);
			if(pCityGate == NULL)
				return 0;

			if(wAction == CShape::ACT_OPEN) wAction = 12;
			sprintf(strbuf,	"sounds/citygate/g%03d%s.wav", pCityGate->GetGraphicsID(), strAction[wAction]);
		}
		break;
	default:
		return 0;
	}

	return Play3DSound(strbuf,nLoops,vRole.x,vRole.y,vRole.z);
}

//播放场景声音
//参数说明
//	pSound:场景声音表示的结构体	lRegionID:场景的ID	loop:是否循环	nLoops:重复播放的次数
//返回说明
//	播放该声音的channel,-1表示播放失败
int CAudioList::Play(CClientRegion::REGION_SOUND_PTR pSound,long lRegionID,bool loop, int nLoops)
{
	if(m_pAudioEngine==NULL)return 0;
	if(loop)nLoops=-1;
	char filename[MAX_PATH]=""; 
	sprintf(filename,"sounds/regions/%d/%d.wav",lRegionID,pSound->Sound.wID);

	Log4c::Trace(ROOT_MODULE,"Start Play Region Sound.(filename:%s).",filename);

	return Play3DSound(filename,nLoops,pSound->zpos,pSound->Sound.fHeight,pSound->xpos);
}

//播放声音文件
//参数说明
//	filename:文件名字	loop:是否循环	bEnv:是否环境声音(即是2D声音)	nLoops:重复次数
//返回说明
//	返回播放该声音的channel,-1表示播放失败
int CAudioList::Play(const char* filename, bool loop, bool bEnv, int nLoops)
{
	if(m_pAudioEngine==NULL)return 0;
	if(filename==0)return -1;
	if(loop)nLoops=-1;
	if( bEnv )
	{
		if(IsPlaying(filename))
		{
			return -1;
		}else{
		    return Play2DSound(filename);
		}

	}
	else
	{
		return Play3DSound(filename,nLoops,0,0,0);
	}
}

//播放3D声音
//参数说明
// filename:文件名 fX,fY,fHeight:位置信息	loop:是否循环	nLoops:重复次数
//返回说明
//	返回播放声音的channel,-1表示播放失败
int CAudioList::Play(const char* filename, float fX, float fY, float fHeight, bool loop, int nLoops)
{
	if(m_pAudioEngine==NULL)return 0;
	if(filename==0)return -1;
	if(loop)nLoops=-1;
	return this->Play3DSound(filename,nLoops,fY, fHeight, fX);
}

//更新声音引擎以及它所维护的声音和音乐的状态
//参数说明
//	无
//返回说明
//	无
//注意事项
//	声音系统需要定期更新状态,因此客户程序必须定期调用,建议有专门的线程来处理,时间间隔建议在50MS
//声音的渐入渐出以及自动回收都依赖于状态更新
void CAudioList::UpdateAudio()
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->Update();
	//char msg[32];
	//sprintf(msg,"sounds:%d",m_pAudioEngine->GetSoundsCount());
	//SetWindowText(g_hWnd,msg);
}

void CAudioList::Run()
{
	if(m_pAudioEngine)
		m_pAudioEngine->Run();
	CountMonsterOppear();
}

//播放音乐
//参数说明
//	filename:音乐文件名 loops:播放的次数,小于1表示始终循环播放
//返回说明
//	无	如果想知道声音是否成功播放,可用IsPlaying
void CAudioList::PlayMusic(const char *filename,int loops)
{
	if(m_pAudioEngine==NULL)return;
	if(filename==0)return;
	CFmodMusic *pMusic=m_pAudioEngine->GetMusicEx(filename,FSOUND_HW2D);
	if(pMusic==0)return;
	int channel = pMusic->Play(loops);
	if(channel == -1 && FSOUND_GetError() == FMOD_ERR_PLAY)
	{
		if( pMusic->GetMode() == FSOUND_HW2D )
		{
			m_pAudioEngine->SetHW2DMaxChnnels(0);
		}
	}
}


//渐入播放音乐
//参数说明
//	filename:音乐文件名	time:声音渐入的时间	loops:播放的次数,小于1表示始终循环播放
//返回说明
//	无,在time的时间内,音乐的音量从0线形增大到当前音乐的音量,变化频率依赖声音引擎
//状态更新频率.如果想知道声音是否成功播放,可用IsPlaying
void CAudioList::FadeInMusic(const char *filename,unsigned int time,int loops)
{
	if(m_pAudioEngine==NULL)return;
	if(filename==0)return;
	CFmodMusic *pMusic=m_pAudioEngine->GetMusicEx(filename,FSOUND_HW2D);
	if(pMusic==0)return;
	int channel=pMusic->FadeIn(time,loops);
	pMusic->SetVolume(0);
	//int channel = pMusic->Play(loops);
	if( channel != -1)
	{
		int nMaxVolume=max(0,min(255,(int)(Audio::s_fMusicVolume*255)));
		m_pAudioEngine->Fade(pMusic->GetCurChannel(),true,nMaxVolume,time);
	}
	else
	{
		if(FSOUND_GetError() == FMOD_ERR_PLAY && pMusic->GetMode() == FSOUND_HW2D )
		{
			m_pAudioEngine->SetHW2DMaxChnnels(0);
		}
	}
}

//渐出播放音乐
//参数说明
//	filename:音乐文件名	time:声音渐出的时间	loops:播放的次数,小于1表示始终循环播放
//返回说明
//	无,在time的时间内,音乐的音量从当前最大音乐音量线形减小到当0并停止,变化频率
//依赖声音引擎状态更新的频率.如果想知道声音是否成功播放,可用IsPlaying
void CAudioList::FadeOutMusic(const char *filename,unsigned int time,int loops)
{
	if(m_pAudioEngine==NULL)return;
	if(filename==0)return;
	CFmodMusic *pMusic=m_pAudioEngine->FindMusic(filename);
	if(pMusic==0 || pMusic->IsAlive() == false)return;

	pMusic->FadeOut(time,loops);
	int nMaxVolume=max(0,min(255,(int)(Audio::s_fMusicVolume*255)));
	//m_pAudioEngine->Fade(pMusic->GetCurChannel(),false,nMaxVolume,time);
}
//渐变播放音乐
//参数说明
//	filename:音乐文件名	time:声音渐出的时间	loops:播放的次数,小于1表示始终循环播放
//返回说明
//	无,在time的时间内,音乐的音量从当前最大音乐音量线形减小到当0并停止,变化频率
//依赖声音引擎状态更新的频率.如果想知道声音是否成功播放,可用IsPlaying
void CAudioList::FadeMusic(const char *filename,unsigned int time,float Volume,int loops)
{
	if(m_pAudioEngine==NULL)return;
	if(filename==0)return;
	CFmodMusic *pMusic=m_pAudioEngine->FindMusic(filename);
	if(pMusic==0 || pMusic->IsAlive() == false)return;

	//if(pMusic->GetPlayingType()==Audio::PLAY_FADEDOWN||pMusic->GetPlayingType()==Audio::PLAY_FADEUP)
		//return;

	pMusic->Fade(time,Volume,loops);
	int nMaxVolume=max(0,min(255,(int)(Audio::s_fMusicVolume*255)));
	//m_pAudioEngine->Fade(pMusic->GetCurChannel(),false,nMaxVolume,time);
}

void CAudioList::SetFadeState(bool b)
{
	//CFmodMusic *pMusic=m_pAudioEngine->FindMusic(GetBackgroundMusic());
	//if(pMusic==0 || pMusic->IsAlive() == false)return;
	//if(pMusic->GetPlayingType()==Audio::PLAY_FADEIN||pMusic->GetPlayingType()==Audio::PLAY_FADEOUT) return;
	m_bFadeState = b;
}


//停止一个已经在播放的音乐
//参数说明
//	filename:音乐文件名
//返回说明
//	无
void CAudioList::StopMusic(const char *filename)
{
	if(m_pAudioEngine==NULL)return;
	if(filename==0)return;
	CFmodMusic *pMusic=m_pAudioEngine->FindMusic(filename);
	if(pMusic==0)return;
	pMusic->Stop();
}

//释放一个已经在播放的音乐
//参数说明
//	filename:音乐文件名
//返回说明
//	无
void CAudioList::ReleaseMusic(const char *filename)
{	
	if(m_pAudioEngine==NULL)return;
	if(filename==0)return;
	CFmodMusic *pMusic=m_pAudioEngine->FindMusic(filename);
	if(pMusic==0)return;
	pMusic->Stop();
}

//播放3D声音
//参数说明
//	filename:声音文件名	loops:播放次数,小于1表示始终循环播放 x,y,z:声音的空间位置
//返回说明
//	如果成功播放,返回播放声音的channel,否则返回-1
int CAudioList::Play3DSound(const char *filename,int loops,float x,float y,float z)
{
	if(m_pAudioEngine==NULL)return -1;
	if(filename==0)return -1;
	
	//对于只播放一次的声音，判断其是否在可播放范围内
	if(loops==1)
	{
		float dx=GetGame()->GetMainPlayer()->GetPosX()-z,dy=GetGame()->GetMainPlayer()->GetPosY()-x;
		if(abs(dx) > GetGame()->GetSetup()->l3DSoundsRange|| abs(dy) > GetGame()->GetSetup()->l3DSoundsRange)
		{
			return -1;
		}
	}
	CFmodSound *pSound=m_pAudioEngine->GetSoundEx(filename,FSOUND_HW3D);

	if(pSound==0)return -1;
	pSound->SetPosition(x,y,z);
	int channel=pSound->Play(loops);
	//如果播放错误,
	if(channel == -1 && FSOUND_GetError() == FMOD_ERR_PLAY)
	{
		if( pSound->GetMode() == FSOUND_HW3D )
		{
			m_pAudioEngine->SetHW3DMaxChnnels(0);
		}
		if( pSound->GetMode() == FSOUND_HW2D )
		{
			m_pAudioEngine->SetHW2DMaxChnnels(0);
		}
	}
	int length = pSound->GetAudioLength();
	if(length<1000)
	{
		pSound->Release(false);
		channel=-1;
	}
	return channel;	
}

//渐入播放3D声音
//参数说明
//	filename:声音文件名 x,y,z:声音的空间位置 time:渐入的时间 loops:播放次数,小于1表示始终循环播放
//返回说明
//	如果成功播放,返回播放声音的channel,否则返回-1.在time的时间内,声音音量从0到当前最大声音音量线形
//增大,变化频率依赖声音引擎状态更新的频率
//int CAudioList::FadeIn3DSound(const char *filename,float x,float y,float z,unsigned int time,int loops)
//{
//	/*if(m_pAudioEngine==NULL)return 0;
//	if(filename==0)return -1;
//	CFmodSound *pSound=m_pAudioEngine->GetSoundEx(filename,FSOUND_HW3D);
//	if(pSound==0) return -1;
//	pSound->SetPosition(x,y,z);
//	int channel=pSound->FadeIn(time,loops);
//	return channel;*/
//	return -1;
//}
//
//
////渐出播放3D声音
////参数说明
////	filename:声音文件名 x,y,z:声音的空间位置 time:渐出的时间 loops:播放次数,小于1表示始终循环播放
////返回说明
////	如果成功播放,返回播放声音的channel,否则返回-1.在time的时间内,声音音量从当前最大声音音量到0线形
////减小并停止,变化频率依赖声音引擎状态更新的频率
//int CAudioList::FadeOut3DSound(const char *filename,float x,float y,float z,unsigned int time,int loops)
//{
//	/*if(m_pAudioEngine==NULL)return 0;
//	if(filename==0)return -1;
//	CFmodSound *pSound=m_pAudioEngine->FindSound(filename,-1);
//	if(pSound==0)return 0;
//	pSound->SetPosition(x,y,z);
//	int channel=pSound->FadeOut(time,loops);
//	return channel;*/
//	return -1;
//}

//停止3D声音
//参数说明
//	filename:声音文件名 channel:播放该声音的channel
//返回说明
//	无 如果存在该文件和channel的3D声音就停止
void CAudioList::Stop3DSound(const char *filename,int channel)
{
	if(m_pAudioEngine==NULL)return;
	if(filename==0)return;
	CFmodSound *pSound=m_pAudioEngine->FindSound(filename,channel);
	if(pSound==0)return;
	pSound->Stop();
}

//停止3D声音
//参数说明
//	filename:声音文件名 channel:播放该声音的channel
//返回说明
//	无 如果存在该文件和channel的3D声音就停止
void CAudioList::Stop3DSound(int channel)
{
	if(m_pAudioEngine==NULL)return;
	CFmodSound *pSound=m_pAudioEngine->FindSound(channel);
	if(pSound==0)return;
	pSound->Stop();
}

//停止播放语音
void CAudioList::StopSpeech( const char *filename,int channel )
{
	if(m_pAudioEngine==NULL)return;
	if(filename==0)return;
	CFmodSpeech *pSpeech=m_pAudioEngine->FindSpeech(filename,-1);
	if(pSpeech==0)return;
	pSpeech->Stop();
}

//释放3D声音
//参数说明
//	filename:声音文件名 channel:播放该声音的channel
//返回说明
//	无 如果存在该文件和channel的3D声音就释放
void CAudioList::Release3DSound(const char *filename,int channel)
{
	if(m_pAudioEngine==NULL)return;
	if(filename==0)return;
	CFmodSound *pSound=m_pAudioEngine->FindSound(filename,channel);
    if(pSound)
		pSound->Stop();
}

//设置3D声音的位置
//参数说明
//	filename:声音文件名 channel:播放该声音的channel x,y,z:位置坐标
//返回说明
//	无 如果存在该文件和channel的3D声音就设置为x,y,z的坐标
//void CAudioList::Set3DSoundPosition(const char *filename,int &channel,float x,float y,float z)
//{
//	if(m_pAudioEngine==NULL)return;
//	CFmodSound *pSound=m_pAudioEngine->FindSound(filename,channel);
//	if(pSound==0)return;
//	pSound->SetPosition(x,y,z);
//}

//播放2D声音
//参数说明
//	filename:声音文件名 loops:播放次数,小于1表示始终循环播放
//返回说明
//	如果播放成功返回播放该声音的channel
int CAudioList::Play2DSound(const char *filename,int loops)
{
	if(m_pAudioEngine==NULL)
		return 0;

	if(filename==NULL)
		return -1;
	if(IsPlaying(filename,-1)) 
		return -1;

	CFmodSound *pSound=m_pAudioEngine->GetSoundEx(filename,FSOUND_HW2D);
	if (pSound == NULL)	return -1;
	int channel = pSound->Play(loops);
	//如果播放错误,
	if(channel == -1 && FSOUND_GetError() == FMOD_ERR_PLAY)
	{
		if( pSound->GetMode() == FSOUND_HW3D )
		{
			m_pAudioEngine->SetHW3DMaxChnnels(0);
		}
		if( pSound->GetMode() == FSOUND_HW2D )
		{
			m_pAudioEngine->SetHW2DMaxChnnels(0);
		}
	}
	return channel;
}

//播放语音
//参数说明
//	filename:声音文件名 loops:播放次数,小于1表示始终循环播放
//返回说明
//	如果播放成功返回播放该声音的channel
int CAudioList::PlaySpeech(const char *filename,int loops)
{
	if(m_pAudioEngine==NULL)
		return 0;

	if(filename==NULL)
		return -1;

	CFmodSpeech *pSpeech=m_pAudioEngine->GetSpeechEx(filename,FSOUND_HW2D);
	if (pSpeech == NULL)	return -1;
	int channel = pSpeech->Play(loops);
	////如果播放错误,
	//if(channel == -1 && FSOUND_GetError() == FMOD_ERR_PLAY)
	//{
	//	if( pSound->GetMode() == FSOUND_HW3D )
	//	{
	//		m_pAudioEngine->SetHW3DMaxChnnels(0);
	//	}
	//	if( pSound->GetMode() == FSOUND_HW2D )
	//	{
	//		m_pAudioEngine->SetHW2DMaxChnnels(0);
	//	}
	//}
	return channel;
}

//播放3D语音
//参数说明
//	filename:声音文件名	loops:播放次数,小于1表示始终循环播放 x,y,z:声音的空间位置
//返回说明
//	如果成功播放,返回播放声音的channel,否则返回-1
int CAudioList::Play3DSpeech(const char *filename,int loops,float x,float y,float z)
{
	if(m_pAudioEngine==NULL)return -1;
	if(filename==0)return -1;
	
	//对于只播放一次的声音，判断其是否在可播放范围内
	if(loops==1)
	{
		//int dx=GetGame()->GetMainPlayer()->GetPosX()-z,dy=GetGame()->GetMainPlayer()->GetPosY()-x;
		float dx=GetGame()->GetMainPlayer()->GetPosX()-x,dy=GetGame()->GetMainPlayer()->GetPosY()-y;
		if(abs(dx)>GetGame()->GetSetup()->l3DSoundsRange||abs(dy)>GetGame()->GetSetup()->l3DSoundsRange)
		{
			return -1;
		}
	}
	CFmodSpeech *pSpeech=m_pAudioEngine->GetSpeechEx(filename,FSOUND_HW3D);
	if(pSpeech==0)return -1;
	pSpeech->SetPosition(y,z,x);
	int channel=pSpeech->Play(loops);
	//如果播放错误,
	if(channel == -1 && FSOUND_GetError() == FMOD_ERR_PLAY)
	{
		if( pSpeech->GetMode() == FSOUND_HW3D )
		{
			m_pAudioEngine->SetHW3DMaxChnnels(0);
		}
		if( pSpeech->GetMode() == FSOUND_HW2D )
		{
			m_pAudioEngine->SetHW2DMaxChnnels(0);
		}
	}
	int length = pSpeech->GetAudioLength();
	if(length<1000)
	{
		pSpeech->Release(false);
		channel=-1;
	}
	return channel;	
}

//渐入播放2D声音
//参数说明
//	filename:声音文件名 time:渐入的时间 loops:播放次数,小于1表示始终循环播放
//返回说明
//	如果播放成功返回播放该声音的channel 在time的时间内,声音音量从0到当前最大声音音量线形
//增大,变化频率依赖声音引擎状态更新的频率
int CAudioList::FadeIn2DSound(const char *filename,unsigned int time,int loops)
{
	if(m_pAudioEngine==NULL)return 0;
	if(filename==0)return -1;
	CFmodSound *pSound=m_pAudioEngine->GetSoundEx(filename,FSOUND_HW2D);
	if(pSound==0)return -1;
	pSound->SetVolume(0);
	int channel=pSound->Play(loops);
	if(channel != -1)
	{
		int nMaxVolume=max(0,min(255,(int)(Audio::s_fSoundVolume*255)));
		m_pAudioEngine->Fade(pSound->GetCurChannel(),true,nMaxVolume,time);
	}
	else
	{
		//如果播放错误,
		if( FSOUND_GetError() == FMOD_ERR_PLAY )
		{
			if( pSound->GetMode() == FSOUND_HW3D )
			{
				m_pAudioEngine->SetHW3DMaxChnnels(0);
			}
			if( pSound->GetMode() == FSOUND_HW2D )
			{
				m_pAudioEngine->SetHW2DMaxChnnels(0);
			}
		}
	}
	return channel;
}

//渐入播放2D声音
//参数说明
//	filename:声音文件名 time:渐入的时间 loops:播放次数,小于1表示始终循环播放
//返回说明
//	如果播放成功返回播放该声音的channel 在time的时间内,声音音量从当前最大声音音量到0线形
//减小并停止,变化频率依赖声音引擎状态更新的频率
int CAudioList::FadeOut2DSound(const char *filename,unsigned int time,int loops)
{
	if(m_pAudioEngine==NULL)return 0;
	if(filename==0)return -1;
	CFmodSound *pSound=m_pAudioEngine->FindSound(filename,-1);
	if(pSound==0 || pSound->IsAlive() == false)	return -1;

	int nMaxVolume=max(0,min(255,(int)(Audio::s_fSoundVolume*255)));
	m_pAudioEngine->Fade(pSound->GetCurChannel(),false,nMaxVolume,time);

	return pSound->GetCurChannel();
}

//停止2D声音
//参数说明
//	filename:声音文件名
//返回说明
//	如果播存在该文件的2D声音就停止
//void CAudioList::Stop2DSound(const char *filename)
//{
//	if(m_pAudioEngine==NULL)return;
//	if(filename==0)return;
//	CFmodSound *pSound=m_pAudioEngine->FindSound(filename,-1);
//	if(pSound==0)return;
//	pSound->Stop();
//}

//释放2D声音
//参数说明
//	filename:声音文件名
//返回说明
//	如果播存在该文件的2D声音就释放
void CAudioList::Release2DSound(const char *filename)
{
	if(m_pAudioEngine==NULL)return;
	if(filename==0)return;
	CFmodSound *pSound=m_pAudioEngine->FindSound(filename,-1);
	if(pSound==0)return;
	pSound->Release();
}

//判断一个声音是否在播放状态
//参数说明
//	filename:声音文件名 channel:播放声音的channel,-1表示音乐或者2D声音,默认值为-1
//返回说明
//	如果存在这个声音并且在播放返回真,否则返回假
bool CAudioList::IsPlaying(const char *filename,int channel)
{
	if(m_pAudioEngine==NULL)return false;
	Audio *pAudio=NULL;
	pAudio=m_pAudioEngine->FindSound(filename,channel);
	if(pAudio)
	{
		return pAudio->IsPlaying();
	}
	pAudio=m_pAudioEngine->FindMusic(filename);
	if(pAudio)
	{
		return pAudio->IsPlaying();
	}
	return false;
}

//设置声音系统的音乐的音量
//参数说明
//	fVolume:音量大小,有效范围是0到1,超过这个范围取0(小于0)或1(大于1)
//返回说明
//	无
void CAudioList::SetMusicVolume(float fVolume)
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->SetMusicVolume(fVolume);
}

//设置声音系统的音乐的音量
//参数说明
//	fVolume:音量大小,有效范围是0到1,超过这个范围取0(小于0)或1(大于1)
//返回说明
//	无
void CAudioList::SetSpeechVolume(float fVolume)
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->SetSpeechVolume(fVolume);
}

//设置声音系统的音乐是否打开
//参数说明
//	b:开关
//返回说明
//	无
void CAudioList::SetMusicEnabled(bool b)
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->SetMusicEnabled(b);
	if(b&&!IsPlaying(GetBackgroundMusic()))
	{
		FadeInMusic(GetBackgroundMusic(),5000);
	}
}

//设置声音系统的声音是否打开
//参数说明
//	b:开关
//返回说明
//	无
void CAudioList::SetSoundEnabled(bool b)
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->SetSoundEnabled(b);
}

//设置声音系统的声音是否打开
//参数说明
//	b:开关
//返回说明
//	无
void CAudioList::SetSpeechEnabled(bool b)
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->SetSpeechEnabled(b);
}

//设置声音系统的声音的音量
//参数说明
//	fVolume:音量大小,有效范围是0到1,超过这个范围取0(小于0)或1(大于1)
//返回说明
//	无
void CAudioList::SetSoundVolume(float fVolume)
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->SetSoundVolume(fVolume);
}

//设置声音系统监听者位置
//参数说明
//	pos:位置坐标数组
//返回说明
//	无
//注意事项
//	pos可以为空,但是不能是无效数组或者长度小于3!!!
void CAudioList::SetListenerPosition(float *pos)
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->SetListenerPosition(pos);
}

//设置声音系统监听者位置
//参数说明
//	x,y,z:坐标
//返回说明
//	无
void CAudioList::SetListenerPosition(float x,float y,float z)
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->SetListenerPosition(x,y,z);
}

//设置声音系统监听者速度向量
//参数说明
//	vel:表示速度向量的数组
//返回说明
//	无
//注意事项
//	vel可以为空,但是不能是无效数组或者长度小于3!!!
void CAudioList::SetListenerVelocity(float *vel)
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->SetListenerVelocity(vel);
}

//设置声音系统监听者速度向量
//参数说明
//	x,y,z:坐标
//返回说明
//	无
void CAudioList::SetListenerVelocity(float x,float y,float z)
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->SetListenerVelocity(x,y,z);
}

//设置声音系统监听者方向
//参数说明
//	face:朝前的方向向量 up:朝上的方向向量
//返回说明
//	无
//注意事项
//	face和up可以为空,但是不能是无效数组或者长度小于3!!!
void CAudioList::SetListenerOritation(float *face,float *up)
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->SetListenerOritation(face,up);
}

//设置声音系统监听者方向
//参数说明
//	facex,facey,facez:朝前的方向向量 upx,upy,upz:朝上的方向向量
//返回说明
//	无
void CAudioList::SetListenerOritation(float facex,float facey,float facez,float upx,float upy,float upz)
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->SetListenerOritation(facex,facey,facez,upx,upy,upz);
}

//播放一个装备物品操作时候它所对应声音
//参数说明
//	dwIndex:该物品的索引编号 b3D:是否是3D声音 x,y,z:声音坐标
//返回说明
//	无
void CAudioList::PlayEquipmentSound(DWORD dwIndex,bool b3D,float x,float y,float z)
{
	if(m_pAudioEngine==NULL)return;
	if(dwIndex==0)return;
	char strName[MAX_PATH]="";

	CGoodsList::tagGoods2 *pGoods = CGoodsList::GetProperty(dwIndex);
	if (pGoods)
	{
		DWORD dwSound=pGoods->BaseProperty.dwSound;
		sprintf(strName,"SOUNDS\\equipments\\%05d.wav",dwSound);
		if( b3D )
		{
			Play3DSound(strName,1,x,y,z);
		}
		else
		{
			Play2DSound(strName);
		}
	}
}

//设置游戏背景音乐的文件名
//参数说明
//	filename:文件名字 如果为空表示设置背景音乐为无
//返回说明
//	无
void CAudioList::SetBackgroundMusic(const char *filename)
{
	if(m_pAudioEngine==NULL)return;
	if(filename)
	{
        m_strMusicFile=string(filename);
	}
	else
	{
		m_strMusicFile=string("");
	}
}

//获取游戏背景音乐的文件名
//参数说明
//	无
//返回说明
//	返回背景声音的文件名,如果是空字符串(""),表示没有背景音乐
const char *CAudioList::GetBackgroundMusic()
{
	return m_strMusicFile.c_str();
}

//加载设置,配置参数
void CAudioList::LoadSetup(const char *filename)
{
	if(m_pAudioEngine==NULL)return;
	float rolloff;
	float mindistance,maxdistance;
	float distancefactor;
	long PlayOppearSoundTime;

	CRFile* prfile = rfOpen(filename);
	if( prfile )
	{	
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);
		string s;
		stream >> s >> rolloff
			>> s >> mindistance
			>> s >> maxdistance
			>> s >> distancefactor
			>> s >> CMoveShape::highhurt_monster
			>> s >> CMoveShape::highhurt_boss
			>> s >> CMoveShape::highhurt_knight
			>> s >> CMoveShape::highhurt_bow
			>> s >> CMoveShape::highhurt_magic
			>> s >> CMoveShape::mildhurt_monster
			>> s >> CMoveShape::mildhurt_boss
			>> s >> CMoveShape::mildhurt_knight
			>> s >> CMoveShape::mildhurt_bow
			>> s >> CMoveShape::mildhurt_magic
			>> s >> PlayOppearSoundTime;

		m_pAudioEngine->SetMinMaxDistance(mindistance,maxdistance);
		m_pAudioEngine->SetRolloffFactor(rolloff);
		m_pAudioEngine->SetDistanceFactor(distancefactor);
		m_lPlayOppearSoundTime = PlayOppearSoundTime;
	}
}

/*垃圾回收*/
void CAudioList::GarbageColloction()
{
	if(m_pAudioEngine==NULL)return;
	m_pAudioEngine->GrabageCollection();
}

/*计算怪物出现的时间，用于控制玩家进入游戏时不会立即播放怪物出现的音效*/
void CAudioList::CountMonsterOppear()
{
	if(m_bPlayMonsterOppear==false)
	{
		long dwCurTime = (long)timeGetTime();
		if( (dwCurTime-m_lPlayerInterTime)>m_lPlayOppearSoundTime)
		{
          m_bPlayMonsterOppear=true;
		  m_lPlayerInterTime = 0;
		}
	}
}

/*设置是否播放出现音效*/
void CAudioList::SetOppearSoundEnble(bool l)
{
    m_bPlayMonsterOppear=l;
	m_lPlayerInterTime = timeGetTime();
}
