#pragma once
#include "../Engine/render.h"

class GameMap;
class GameBaseEnvCreature;
class GameBaseEnvEffect;
class GameSkybox;

class GameScene
{
    render::Frustum     * m_pFrustum;
    render::Camera      * m_pCamera;
    render::Camera      * m_pOriginalCamera;
    GameMap             * m_pGameMap;
    GameBaseEnvCreature * m_pGameEnvCreature;
    GameBaseEnvEffect	* m_pGameEnvEffect;
    GameSkybox			* m_pGameSkyBox;
    BOOL				  m_bFadeIn;
    DWORD				  m_dwFadeInTimeStart;
    DWORD				  m_dwFadeInTimeLength;
public:
    GameScene();
    ~GameScene();
    void CreateSence(const char * map, const char * camera,
        const char * envCreature, const char * enveffect,bool isFadeIn = true);
    void Destroy();

    void RenderLayerFront();
    void RenderLayerBack();
    
    void ReadCamera(render::Camera *pCamera,const char *szName);

    BOOL IsFadeFinish() const { return !m_bFadeIn;}

    render::Camera * GetCamera() {return m_pCamera;}

    GameMap * GetGameMap()  {return m_pGameMap;}
};