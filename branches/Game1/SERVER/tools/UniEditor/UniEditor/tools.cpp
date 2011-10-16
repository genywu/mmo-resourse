
//Realize counter function
#include "stdafx.h"
#include "tools.h"

#pragma warning	(disable : 4996)

#ifdef _WIN32
static LARGE_INTEGER _tstart, _tend;
static LARGE_INTEGER freq;

void tstart(void)
{
    static int first = 1;

    if(first) {
        QueryPerformanceFrequency(&freq);
        first = 0;
    }
    QueryPerformanceCounter(&_tstart);
}
void tend(void)
{
    QueryPerformanceCounter(&_tend);
}

double tval()
{
    return ((double)_tend.QuadPart -
                (double)_tstart.QuadPart)/((double)freq.QuadPart);
}

#else
static struct timeval _tstart, _tend;
static struct timezone tz;

void tstart(void)
{
    gettimeofday(&_tstart, &tz);
}
void tend(void)
{
    gettimeofday(&_tend,&tz);
}

double tval()
{
    double t1, t2;

    t1 =  (double)_tstart.tv_sec + (double)_tstart.tv_usec/(1000*1000);
    t2 =  (double)_tend.tv_sec + (double)_tend.tv_usec/(1000*1000);
    return t2-t1;
}
#endif

//Get OS version by string
const char *osver()
{
#ifdef _WIN32
	static char verbuf[256];
	OSVERSIONINFO VersionInfo;

	VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
	if(GetVersionEx(&VersionInfo)) {
		if(strlen(VersionInfo.szCSDVersion) > 200)
			VersionInfo.szCSDVersion[100] = 0;
		sprintf(verbuf, "Windows %d.%d build%d PlatformId %d SP=\"%s\"",
			VersionInfo.dwMajorVersion,
			VersionInfo.dwMinorVersion,
			VersionInfo.dwBuildNumber,
			VersionInfo.dwPlatformId,
			VersionInfo.szCSDVersion);
		return verbuf;
	}
	else {
		return "WINDOWS UNKNOWN";
	}
#else
	struct utsname ubuf;
	static char verbuf[4*SYS_NMLN + 4];

	if(uname(&ubuf))
		return "LINUX UNKNOWN";
	sprintf(verbuf,"%s %s %s %s",
		ubuf.sysname,
		ubuf.release,
		ubuf.version,
		ubuf.machine);
	return verbuf;
#endif
}

size_t atoik(char *s)
{
	size_t ret = 0;
	size_t base;

	if(*s == '0') {
		base = 8;
		if(*++s == 'x' || *s == 'X') {
			base = 16;
			s++;
		}
	}
	else
		base = 10;

	for(; isxdigit(*s); s++) {
		if(base == 16)
			if(isalpha(*s))
				ret = base*ret + (toupper(*s) - 'A');
			else
				ret = base*ret + (*s - '0');
		else if(isdigit(*s))
				ret = base*ret + (*s - '0');
		else
			break;
	}
	for(; isalpha(*s); s++) {
		switch(toupper(*s)) {
		case 'K': ret *= 1024; break;
		case 'M': ret *= 1024*1024; break;
		default:
			return ret;
		}
	}
	return ret;
}

//alloc Memory
void *Malloc(size_t sz)
{	
	char *p;
	p = (char *)malloc(sz);

	if(p == NULL)
	{
		(void)printf("malloc(%d) failed\n",sz);
		exit(1);

	}
	return (void *)p;
}

