#pragma once
#include "../../GamePage.h"

class GameScene;
class CPlayer;
class SREvent;
class SelectRolePage:public GamePage
{
public:
    SelectRolePage();

    virtual ~SelectRolePage();

    virtual bool Open();

    virtual bool Close();

    virtual void ReleasePage();

    virtual void LoadPage();

    void Display();

    void AddPlayer(CPlayer* pPlayer);

    void ClearPlayer();

    ulong GetPlayerCount();

	CEGUI::Window* GetWindow(const char* WindwoName);

    CPlayer * GetSelPlayer();

	CPlayer * GetPlayer(ulong index);

    bool          m_bRoleLeftRotate;    //向左旋转
    bool          m_bRoleRightRotate;   //向右旋转
private:

    GameScene   * m_SelectSence;

    static float  s_RolePos[4];		// 站的位置
    static int    m_CurrentAction;		// 当前动作
    static int    m_NextAction;           // 下一个动作

	SREvent* m_event;
};