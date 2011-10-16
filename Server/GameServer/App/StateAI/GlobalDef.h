///
/// @file GlobalDef.h
/// @author Kevin Lynx
/// @brief Some global definitions used in StateAI. 
///
#ifndef ___GLOBAL_DEF_H_
#define ___GLOBAL_DEF_H_

#ifdef _ONLY_TEST

/// some temp definitions
typedef long CGUID;

#endif

struct Point
{
	long x, y;
	Point( long x = 0, long y = 0 )
	{
		this->x = x; this->y = y;
	}

    void Reset()
    {
        x = y = 0;
    }
};

#endif
