//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KCloseProgramMgr.h
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-2-5 11:28:38
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef __KCLOSE_PROGRAM_MGR_H__
#define __KCLOSE_PROGRAM_MGR_H__


struct KBaseClosedProgram
{
    int m_nCloseFlag;
    virtual int CloseProgram() = 0;
    virtual int StartProgram() = 0;
};

typedef struct tagKProcessInfo
{
    char szFileName[MAX_PATH];          // Full file name
    unsigned uID;                       // Process ID of running file in KAVIPC
    char szCloseParam[MAX_PATH];        // Parameter for close program
    char szStartParam[MAX_PATH];        // Parameter for start program
    
} KProcessInfo;


//---------------------------------------------------------------------------------
//   Close program by parameter : /quit or /exist
//---------------------------------------------------------------------------------

class KClosedProgram : public KBaseClosedProgram
{
public:
    KClosedProgram(const KProcessInfo ProcessInfo);
    ~KClosedProgram();
    
    // Return  1 : Exist process and exit successfully
    //         0 : Exist process but exit failed
    //        -1 : Not exist process
    int CloseProgram();
    int StartProgram();    

protected:
    KProcessInfo m_kProcessInfo;
    bool m_bInitFlag;    
};


//---------------------------------------------------------------------------------
//   Close program manager
//---------------------------------------------------------------------------------
typedef struct tagKCLOSEDPROGRAMLIST
{
    tagKCLOSEDPROGRAMLIST *pNext;
    KBaseClosedProgram *pClosedProgram;
} KCLOSEDPROTRMLIST, *PKCLOSEDPROGRAMLIST;


class KCloseProgramMgr
{
public:
    KCloseProgramMgr();
    ~KCloseProgramMgr();     
    int RunAllPrograms();
    int AddClosedProgram(KBaseClosedProgram *pClosedProgram);

private:
    PKCLOSEDPROGRAMLIST m_pClosedProgramList;        
};

#endif      //__KCLOSE_PROGRAM_MGR_H__
