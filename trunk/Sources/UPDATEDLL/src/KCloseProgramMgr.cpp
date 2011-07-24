//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KCloseProgramMgr.cpp
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-2-5 11:28:43
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KCloseProgramMgr.h"
#include "DataDefine.h"
#include "PublicFun.h"


//----------------------------------------------------------------------------
//   Close program by parameter
//----------------------------------------------------------------------------
KClosedProgram::KClosedProgram(const KProcessInfo ProcessInfo)
{
    m_nCloseFlag = false; 
    m_kProcessInfo = ProcessInfo;
    m_bInitFlag = true;
}

KClosedProgram::~KClosedProgram()
{
    m_bInitFlag = false;
    m_nCloseFlag = false;
}

// Return  1 : Exist process and exit successfully
//         0 : Exist process but exit failed
int KClosedProgram::CloseProgram()
{
    HINSTANCE Hinstance = 0;

    if (!m_bInitFlag)
        return 0;
   
    Hinstance = ShellExecute(
        NULL, 
        "open", 
        m_kProcessInfo.szFileName, 
        m_kProcessInfo.szCloseParam, 
        "", 
        SW_MINIMIZE 
    );
    if ((int)Hinstance <= 32)
        return 0;
    
    return 1;
}

int KClosedProgram::StartProgram()
{
    HINSTANCE Hinstance = 0;

    if ((!m_bInitFlag) || (!m_nCloseFlag))
        return false;

    Hinstance = ShellExecute(
        NULL, 
        "open", 
        m_kProcessInfo.szFileName, 
        m_kProcessInfo.szStartParam, 
        "", 
        SW_MINIMIZE 
    );
    if ((int)Hinstance <= 32)
        return false;
    
    return true;
}


//----------------------------------------------------------------------------
//  ClosedProgram manager
//----------------------------------------------------------------------------
KCloseProgramMgr::KCloseProgramMgr()
{
    m_pClosedProgramList = NULL;
}

KCloseProgramMgr::~KCloseProgramMgr()
{
    PKCLOSEDPROGRAMLIST pCloseProgramList = NULL;
    
    while (m_pClosedProgramList)
    {
        pCloseProgramList = m_pClosedProgramList->pNext;
        KAV_DELETE(m_pClosedProgramList->pClosedProgram);
        KAV_DELETE(m_pClosedProgramList);
        m_pClosedProgramList = pCloseProgramList;
    }
}

int KCloseProgramMgr::AddClosedProgram(KBaseClosedProgram *pClosedProgram)
{  
    int nResult                     = false;
    PKCLOSEDPROGRAMLIST pCloseProgramList  = NULL;

    KAV_PROCESS_ERROR(pClosedProgram);
    
    pCloseProgramList = new KCLOSEDPROTRMLIST;
    KAV_PROCESS_ERROR(pCloseProgramList);
    pCloseProgramList->pClosedProgram = pClosedProgram;
    if (m_pClosedProgramList)
        pCloseProgramList->pNext = m_pClosedProgramList;
    else 
        pCloseProgramList->pNext = NULL;

    m_pClosedProgramList = pCloseProgramList;
    
    nResult = true;
Exit0:
    return nResult;

}

int KCloseProgramMgr::RunAllPrograms()
{
    PKCLOSEDPROGRAMLIST pCloseProgramList = NULL;

    pCloseProgramList = m_pClosedProgramList;
    while (pCloseProgramList)
    {
        if (pCloseProgramList->pClosedProgram)
            pCloseProgramList->pClosedProgram->StartProgram();

        pCloseProgramList = pCloseProgramList->pNext;
    }

    return true;
}


