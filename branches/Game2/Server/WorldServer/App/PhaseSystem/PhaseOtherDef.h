#pragma  once 

#ifdef _GAME_SERVER
#define PHASE_LOG_FILE "PhaseSysGS"
#elif defined _WORLD_SERVER_
#define PHASE_LOG_FILE "PhaseSysWS"
#endif
