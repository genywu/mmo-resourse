// pcallbackfunc.cpp: implementation of the pcallbackfunc class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "pcallbackfunc.h"
#include <iostream>
using namespace std;

/************************************************************************
*declare callback function 
*when response message arrived these function will be called
*all these callback functions can not be blocked and must return quickly
************************************************************************/
void PCallbackAccountAuthenRes(GSBsipAccountAuthenResDef *p)
{
    printf("AccountAuthen Result=%d\n", p->result);
}		
void PCallbackAccountLockExRes(GSBsipAccountLockExResDef *p)
{
    printf("AccountLockEx Result=%d\n", p->result);
}
void PCallbackAccountUnlockExRes(GSBsipAccountUnlockExResDef *p)
{
    printf("AccountUnlockEx Result=%d\n", p->result);
}	
void PCallbackAwardAuthenRes(GSBsipAwardAuthenResDef *p)
{
    printf("AwardAuthen Result=%d\n", p->result);
}
void PCallbackAwardAckRes(GSBsipAwardAckResDef *p)
{
    printf("AwardAck Result=%d\n", p->result);
}

void PCallbackTokenDepositRes(GSBsipTokenDepositResDef *p)
{
    printf("TokenDeposit Result=%d\n", p->result);
}