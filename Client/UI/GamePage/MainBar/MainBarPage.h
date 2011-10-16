#pragma once
#include "../../GamePage.h"

class MainBarPage:public GamePage
{
public:    
    MainBarPage();

    virtual ~MainBarPage(){};

    virtual void LoadPage();

    virtual void ReleasePage();

    virtual bool Open();

    virtual bool Close();

    void    Run();

    void    Display();
};