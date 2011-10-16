#pragma once
#include "../../GamePage.h"
class PackageEvent;
class PackagePage:public GamePage
{
public:    
    PackagePage();

    virtual ~PackagePage();

    virtual void LoadPage();

    virtual void ReleasePage();

    virtual bool Open();

    virtual bool Close();

    void UpdatePage(long lGoodPos);

    PackageEvent * GetPackageEvent()    {return m_PackageEvent;}
private:
    PackageEvent *m_PackageEvent;
};