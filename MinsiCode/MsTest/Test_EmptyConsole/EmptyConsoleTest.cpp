#include "Precompiled.h"

class CA
{
public:
    CA(VOID)
    {
        this->Init();
    }
    virtual ~CA(VOID)
    {
        this->Release();
    }
    virtual VOID Init(VOID)
    {
        std::cout << "初始化CA" << std::endl;
    }
    virtual VOID Release(VOID)
    {
        this->RealRelease();
    }
    virtual VOID RealRelease(VOID)
    {
        std::cout << "释放CA" << std::endl;
    }
private:
};

class CB
    :public CA
{
public:
    CB(VOID)
    {
        this->Init();
    }
    virtual ~CB(VOID)
    {
        this->Release();
    }
    virtual VOID Init(VOID)
    {
        std::cout << "初始化CB" << std::endl;
    }
    virtual VOID Release(VOID)
    {
        this->RealRelease();
    }
    virtual VOID RealRelease(VOID)
    {
        std::cout << "释放CB" << std::endl;
    }
private:
};

class CC
    :public CB
{
public:
    CC(VOID)
    {
        this->Init();
    }
    virtual ~CC(VOID)
    {
        this->Release();
    }
    virtual VOID Init(VOID)
    {
        std::cout << "初始化CC" << std::endl;
    }
    virtual VOID Release(VOID)
    {
        CB::Release();
        CA::Release();
    }
    virtual VOID RealRelease(VOID)
    {
        std::cout << "释放CC" << std::endl;
    }
private:
};

INT __fastcall MainCall(INT,LPTCHAR*)
{
    _ENTER_FUN_C;

    MSLOG.SetSaveLog(TRUE);
    MSLOG.SetEnableLog(TRUE);

    {
        CC cc;
        cc.Release();
        //CA* pa = &cc;
        //CB* pb = &cc;
        //pa->Init();
        //pa->Release();
        //pb->Init();
        //pb->Release();
    }

    _tsystem(_T("PAUSE"));
    return 0;
    _LEAVE_FUN_DISPOSE_END_C;
    return INVALID_NID;
}
