///
/// @file ClientResLoader.cpp
/// @author Kevin Lynx
/// @brief Concrete resource loader for client, only for DEBUGing purporse now.
///
#include "stdafx.h"
//#define ENABLE_ALOADER //uncomment to enable A_TIME_STAT
#include "ClientResLoader.h"


static int LoadSkillImpl( void *u )
{
    typedef BOOL (*func)();
    func f = (func) u;
    ALOADER_TIME_STAT( "A_Skill",
    f();
    )
    return 1;
}

static int LoadStateImpl( void *u )
{
    typedef BOOL (*func) ( const char * );
    func f = (func) u;
    ALOADER_TIME_STAT( "A_State", 
    f("data/StateXml.xml");    
    )
    return 1;
}

static int LoadQuestImpl( void *u )
{
    typedef BOOL (*func) ( const char * );
    func f = (func) u;
    ALOADER_TIME_STAT( "A_Quest", 
    f( "quest/QuestList.xml" );
    )
    return 1;
}

static int LoadTextImpl( void *u )
{
    ALOADER_TIME_STAT( "A_Text", 
        AppFrame::LoadTextRes();
    )
    return 1;
}

static int LoadOccuSkillImpl( void *u )
{
    ALOADER_TIME_STAT( "A_OccuSkill",
    COccupSkillList *occuList = (COccupSkillList*) u;
    occuList->Init( "data/OccupSkillList.xml" );
    )
    return 1;
}

static int LoadGoodsImpl( void *u )
{
    // here we can remove the dependence on GoodsList class
    typedef BOOL (*func)( const char * );
    func f = (func) u;
    ALOADER_TIME_STAT( "A_Goods", 
    f("data/goods.dat") ;
    )
    return 1;
}

static int LoadDeputyOccuImpl( void *u )
{
    typedef bool (*func)( const char * );
    func f = (func) u;
    ALOADER_TIME_STAT( "A_DeputyOccu", 
    f("data/DeputyOccu/DeputyOccu.xml") ;
    )
    return 1;
}

static int LoadRegionMapImpl( void *u )
{
    typedef BOOL (*func)();
    func f = (func) u;
    ALOADER_TIME_STAT( "A_Map",
    f();
    )
    return 1;
}

void ALoadSkill( const char *name, void *u )
{
    GetInst( LoaderTable ).Request( name, LoadSkillImpl, u );
}

void ALoadState( const char *name, void *u )
{
    GetInst( LoaderTable ).Request( name, LoadStateImpl, u );
}

void ALoadQuest( const char *name, void *u )
{
    GetInst( LoaderTable ).Request( name, LoadQuestImpl, u );
}

void ALoadText( const char *name, void *u )
{
    GetInst( LoaderTable ).Request( name, LoadTextImpl, u );
}

void ALoadOccuSkill( const char *name, void *u )
{
    GetInst( LoaderTable ).Request( name, LoadOccuSkillImpl, u );
}

void ALoadGoods( const char *name, void *u )
{
    GetInst( LoaderTable ).Request( name, LoadGoodsImpl, u );
}

void ALoadDeputyOccu( const char *name, void *u )
{
    GetInst( LoaderTable ).Request( name, LoadDeputyOccuImpl, u );
}

void ALoadMap( const char *name, void *u )
{
    GetInst( LoaderTable ).Request( name, LoadRegionMapImpl, u );
}

CRFile *rfOpenLocal( const char *file )
{
    FILE *fp = fopen( file, "rb" );
    if( fp == NULL )
    {
        return NULL;
    }
    CRFile *rfile = new CRFile( file, fp );
    fclose( fp );
    return rfile;
}
