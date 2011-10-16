
////////////////////////////////////////////////////////////////////////////////
//MailDef.cpp/////////////////////////////////////////////////////////////////////
//日期类:
//Author:陈先进
//Create Time:2008.9.22
#include "StdAfx.h"
#include "maildef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

tagMailParam::tagMailParam()
{
	strWriter.clear();		
	strSubject.clear();		
	strText.clear();		
	strExText.clear();		
	tRemainTime = 0;
	tEndTime = 0;
	tWriteTime = 0;

	lCost = 0;			
	lGold = 0;			
	bRead = 0;			
	lType = 0;	
	bSave = 0;
	WriterGuid=NULL_GUID;		
	strReceiverList.clear();
	lCGoods.clear();		
	lSGoods.clear();
	vMailGoods.clear();		
}
