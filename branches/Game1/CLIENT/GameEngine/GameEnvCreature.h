#pragma once
#include "..\frostengine\utility.h"
#include "gamemodel.h"
#include "DisplayModel.h"

class GameMap;

//包含在地图中
class GameBaseEnvCreature
{
protected:
	enum eEnvCreatureAlphaFade
	{
		EC_Fade_Out	 = -1,			//渐出状态
		EC_Fade_None = 0,			//没有渐变效果
		EC_Fade_In	 = 1,			//渐进状态
	};

	CDisplayModel * m_pModel;
	render::BoundingBox m_RegionBox;

	float m_fScale;
	float m_fRadius;
	DWORD m_dwAlphaFadeTimeLength;

	BOOL m_bIsDisplay;
	BOOL m_bFollowPlayer;		//是否跟随玩家出生
	D3DXVECTOR3 m_vBirthPos;	//固定出生点坐标

public:
	GameBaseEnvCreature();
	virtual ~GameBaseEnvCreature() {}

	virtual void Update(const D3DXVECTOR3 * pPosition, render::Frustum * pFrustum) {};
	virtual void Render(const GameMap * pMap, render::Camera * pCamera) {};
	virtual BOOL Create(utility::Text::Node * pNode, char * szResourcePath) { return TRUE; };
	virtual void Destroy() {};

	static GameBaseEnvCreature * CreateEnvCreature(const char * szFileName);
	static FLOAT FieldRandom(FLOAT fStart, FLOAT fEnd);			//取得浮点型随机数
	static DWORD FieldRandom(DWORD dwStart, DWORD dwEnd);		//取得整型随机数
};



class GameEnvCreature_Butterfly : public GameBaseEnvCreature
{
	DWORD m_dwMinLifeTime;
	DWORD m_dwMaxLifeTime;
	DWORD m_dwLastTime;

	float m_fMinSpeed;
	float m_fMaxSpeed;

	float m_fMinAngle;
	float m_fMaxAngle;

	struct tagButterfly
	{
		int	  iAlphaMode;				//正数为渐进，负数为渐出，0为正常模式
		float fSpeed;
		float fRotationY;
		float fCurrentAlphaValue;
		DWORD dwLifeTime;
		DWORD dwAlphaFadeStartTime;

		D3DXVECTOR3	  vPosition;
		AnimInfo	* pAnimInfo;
	};

	tagButterfly * m_pButterflyArray;
	DWORD		   m_dwButterflyCount;

public:
	GameEnvCreature_Butterfly();
	~GameEnvCreature_Butterfly();

	BOOL Create(utility::Text::Node * pNode, char * szResourcePath);		//创建蝴蝶环境生物
	void Destroy();															//销毁蝴蝶资源
	void Update(const D3DXVECTOR3 * pPosition, render::Frustum * pFrustum);
	void AI(tagButterfly & tButterfly, DWORD dwTimeElapse);					//蝴蝶AI
	void Render(const GameMap * pMap, render::Camera * pCamera);
};



class GameEnvCreature_Crow : public GameBaseEnvCreature
{
	DWORD m_dwMinLifeTime;
	DWORD m_dwMaxLifeTime;

	DWORD m_dwLastTime;

	float m_fMinRadomSpeed;
	float m_fMaxRadomSpeed;

	DWORD m_dwMinTurnForceTime;			//随机转向力度变化周期
	DWORD m_dwMaxTurnForceTime;			//随机转向力度变化周期
	float m_fMinTurnForce;				//最小转向力度
	float m_fMaxTurnForce;				//最大转向力度

	DWORD m_dwMinTurnDirectionTime;		//方向变化时间
	DWORD m_dwMaxTurnDirectionTime;

	BOOL m_bLighted;					//光

	struct tagCrow
	{
		int	  iAlphaMode;				//正数为渐进，负数为渐出，0为正常模式
		float fSpeed;
		float fRotationY;
		float fTurnForce;
		float fTurnDirection;
		float fCurrentAlphaValue;
		DWORD dwLifeTime;
		DWORD dwTurnForceLastTime;
		DWORD dwLastTurnDirectionTime;
		DWORD dwAlphaFadeStartTime;
		//DWORD dwModelLevel;

		D3DXVECTOR3   vPosition;
		AnimInfo	* pAnimInfo;
	};	

	tagCrow * m_pCrowArray;
	DWORD	  m_dwCrowCount;

public:
	GameEnvCreature_Crow();
	~GameEnvCreature_Crow();

	BOOL Create(utility::Text::Node * pNode, char * szResourcePath);
	void Destroy();
	void Update(const D3DXVECTOR3 * pPosition, render::Frustum * pFrustum);
	void AI(const D3DXVECTOR3 * pPosition, tagCrow * pCrow, DWORD dwCurrentTime, DWORD dwTimeElapse);
	void Render(const GameMap * pMap, render::Camera * pCamera);

	float CalculateDirection(const D3DXVECTOR3 * pDst, const D3DXVECTOR3 * pSrc);
};



class GameEnvCreature_Beetle : public GameBaseEnvCreature
{
	D3DXVECTOR3 m_vCenter;

	DWORD m_dwMinLifeTime;
	DWORD m_dwMaxLifeTime;

	DWORD m_dwLastTime;

	float m_fMinRadomSpeed;
	float m_fMaxRadomSpeed;

	DWORD m_dwMinTurnForceTime;			//随机转向力度变化周期
	DWORD m_dwMaxTurnForceTime;			//随机转向力度变化周期
	float m_fMinTurnForce;				//转向力度
	float m_fMaxTurnForce;

	DWORD m_dwMinTurnDirectionTime;
	DWORD m_dwMaxTurnDirectionTime;

	// 中心改变时间
	DWORD m_dwMinCenterChangeTime;
	DWORD m_dwMaxCenterChangeTime;
	DWORD m_dwLastCenterChangeTime;
	float m_fRandomCenterRadius;

	BOOL m_bLighted;			//光

	struct tagBeetle
	{
		int	  iAlphaMode;				//正数为渐进，负数为渐出，0为正常模式
		float fSpeed;
		float fRotationY;
		float fTurnForce;
		float fTurnDirection;
		float fCurrentAlphaValue;
		DWORD dwLifeTime;
		DWORD dwTurnForceLastTime;
		DWORD dwTurnDirectionLastTime;
		DWORD dwModelLevel;
		DWORD dwAlphaFadeStartTime;

		D3DXVECTOR3   vPosition;
		AnimInfo	* pAnimInfo;
	};	

	tagBeetle * m_pBeetleArray;
	DWORD		m_dwBeetleCount;

public:
	GameEnvCreature_Beetle();
	~GameEnvCreature_Beetle();

	BOOL Create(utility::Text::Node * pNode, char * szResourcePath);
	void Destroy();
	void Update(const D3DXVECTOR3 * pPosition, render::Frustum * pFrustum);
	void AI(tagBeetle * pBeetle, DWORD dwCurrentTime, DWORD dwTimeElapse);
	void Render(const GameMap * pMap, render::Camera * pCamera);

	float CalculationDirection(const D3DXVECTOR3 * pDest,const D3DXVECTOR3 * pSrc);
};



class GameEnvCreature_Rabbit : public GameBaseEnvCreature
{
	enum RABBIT_STATUS
	{
		RS_STAND = 0,
		RS_IDLE,
		RS_WALK,
	};

	DWORD m_dwMinLifeTime;
	DWORD m_dwMaxLifeTime;

	DWORD m_dwLastTime;

	float m_fMinRadomSpeed;
	float m_fMaxRadomSpeed;

	DWORD m_dwMinTurnForceTime;			//随机转向力度变化周期
	DWORD m_dwMaxTurnForceTime;			//随机转向力度变化周期
	float m_fMinTurnForce;				//转向力度
	float m_fMaxTurnForce;

	DWORD m_dwMinTurnDirectionTime;
	DWORD m_dwMaxTurnDirectionTime;

	DWORD m_dwMinStandInterval;
	DWORD m_dwMaxStandInterval;

	DWORD m_dwMinIdleInterval;
	DWORD m_dwMaxIdleInterval;

	DWORD m_dwMinWalkInterval; 
	DWORD m_dwMaxWalkInterval;

	BOOL  m_bLighted;				//光

	struct tagRabbit
	{
		int	  iAlphaMode;				//正数为渐进，负数为渐出，0为正常模式
		float fSpeed;
		float fRotationY;
		float fTurnForce;
		float fTurnDirection;
		float fCurrentAlphaValue;
		DWORD dwLifeTime;
		DWORD dwTurnForceLastTime;
		DWORD dwTurnDirectionLastTime;
		DWORD dwLastAI_Time;
		DWORD dwAlphaFadeStartTime;

		D3DXVECTOR3		vPosition;
		RABBIT_STATUS	eStatus;
		AnimInfo	  * pAnimInfo;
	};	

	tagRabbit * m_pRabbitArray;
	DWORD		m_dwRabbitCount;

public:
	GameEnvCreature_Rabbit();
	~GameEnvCreature_Rabbit();

	BOOL Create(utility::Text::Node * pNode, char * szResourcePath);
	void Destroy();
	void Update(const D3DXVECTOR3 * pPosition, render::Frustum * pFrustum);
	void AI(tagRabbit * pRabbit, DWORD dwCurrentTime, DWORD dwTimeElapse);
	void Render(const GameMap * pMap, render::Camera * pCamera);
	void ChangeStatus(tagRabbit * pRabbit, RABBIT_STATUS eStatus, DWORD dwCurTime);
};



class GameEnvCreature_Meteor : public GameBaseEnvCreature
{
	float m_fHeight;

	BOOL  m_bLighted;				//是否受光照影响
	DWORD m_dwLiveTimeMax;			//流星的最大存活时间

	D3DXVECTOR3 m_vSpeedMin;		//流星速度矢量下限
	D3DXVECTOR3 m_vSpeedMax;		//流星速度矢量上限

	struct tagMeteor
	{
		int	  iAlphaMode;			//正数为渐进，负数为渐出，0为正常模式
		float fCurrentAlphaValue;
		DWORD dwStartTime;			//流星的出生时间
		DWORD dwLiveTimeMax;		//流星的最大存活时间
		DWORD dwAlphaFadeStartTime;

		D3DXVECTOR3 vPosition;
		D3DXVECTOR3 vSpeed;			//流星的速度矢量

		AnimInfo * pAnimInfo;
	};	

	tagMeteor * m_pMeteorArray;
	DWORD		m_dwMeteorCount;

public:
	GameEnvCreature_Meteor();
	~GameEnvCreature_Meteor();

	BOOL Create(utility::Text::Node * pNode, char * szResourcePath);		//创建流星
	void Destroy();															//销毁流星
	void Update(const D3DXVECTOR3 * pPosition, render::Frustum * pFrustum);	//更新流星
	void AI(tagMeteor * pMeteor);											//流星AI
	void Render(const GameMap * pMap, render::Camera * pCamera);			//渲染流星
};