///
/// @file MoverInternal.h
/// @author Kevin Lynx
/// @brief internal definitions for Mover
///
#ifndef ___MOVER_INTERNAL_H_
#define ___MOVER_INTERNAL_H_

#include "../../../Public/Setup/GlobalSetup.h"

#ifndef MOVER_TEST
#define PutoutLog(a, b, c, ...)
#define LOGFILE ""
#define CONLOG(exp) 
#else
#define LOGFILE m_logFile
//#define CONLOG(exp) if(exp) PutoutLog
#endif

#define FLOATEQ(f1, f2) (abs((f1)-(f2)) < 0.001f)
// TODO: put these consts in config file
#define REQ_MAX         (GlobalSetup::MoveConf().reqMax)
#define DIS_PERSTEP     (GlobalSetup::MoveConf().disPerStep)
#define DIS_MAX         (GlobalSetup::MoveConf().syncMaxDis)
#define MAX_ERRRELDIS   (GlobalSetup::MoveConf().errMaxDis)
#define MIN_ERRRELDIS   (GlobalSetup::MoveConf().errMinDis)
#define REQ_MAXDIS      (GlobalSetup::MoveConf().reqMaxDis)


#endif
