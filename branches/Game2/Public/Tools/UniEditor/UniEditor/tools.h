//include header files
#ifdef _WIN32
	#include <windows.h>
	#define SLASHC		'\\'
	#define SLASHSTR	"\\"
#else
	#include <sys/utsname.h>
	#include <sys/time.h>
	#include <time.h>
	#include <unistd.h>
	#include <errno.h>

	#define SLASHC		'/'
	#define SLASHSTR	"/"

	#define __int64 long long
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//Get OS Version by string
const char *osver();

//Declare counter function
void tstart();
void tend();
double tval();

size_t atoik(char *s);
void *Malloc(size_t sz);
