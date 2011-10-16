//
// sndabase.h

#ifndef SNDABASE_H
#define SNDABASE_H

//#ifndef USE_SDOARSA
//#define USE_SDOARSA
//#endif //USE_SDOARSA

#if defined(_WIN32) || defined(_WIN64)
#define SNDACALL			__stdcall
#else
#define SNDACALL
#endif

#if !defined(interface)
#define interface	struct
#endif

#if !defined(PURE)
#define PURE		= 0
#endif

#define SNDACALLBACK		SNDACALL
#define SNDAMETHOD(Type)	virtual Type SNDACALL
#define SNDAAPI(Type)		extern "C" Type SNDACALL

typedef long HRESULT;
typedef unsigned long ULONG;
typedef const void* SNDAIID;

interface ISDUnknown
{
	SNDAMETHOD(HRESULT) QueryInterface(SNDAIID iid, void** ppv) PURE;
	SNDAMETHOD(ULONG) _Reserved() PURE;
	SNDAMETHOD(ULONG) Release() PURE;
};

#endif // SNDABASE_H