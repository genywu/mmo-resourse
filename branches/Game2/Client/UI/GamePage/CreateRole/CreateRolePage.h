#pragma once
#include "../../GamePage.h"

class GameScene;
class CREvent;
class CreateRolePage:public GamePage
{
public:
    CreateRolePage();
    ~CreateRolePage();
    virtual bool Open();

    virtual bool Close();

    virtual void ReleasePage();

    virtual void LoadPage();

    static void SetRandCountryRange(BYTE lRange);

    void Display();
private:
    GameScene * m_CreateSence;
    CREvent   * m_event;
};