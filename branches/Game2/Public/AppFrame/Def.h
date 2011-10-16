// -*- C++ -*-

/**
 * @file   Def.h
 * @date   Wed Nov 18 14:01:21 CST 2009
 * @author Fox(yulefox.at.gmail.com)
 */

#pragma once

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

namespace AppFrame {

#define INLINE inline

#if USE_WCHAR_T
typedef std::wstring _String;
#else
typedef std::string _String;
#endif

typedef _String String;

typedef std::ifstream IFile;
typedef std::ofstream OFile;
typedef std::ostream OStream;
typedef std::ostringstream OStringStream;

#define AssertFalse(a) assert(a)

#define NON_COPYABLE(TYPE)                      \
    private:                                    \
    TYPE(const TYPE&);                          \
    TYPE& operator=(const TYPE&)

#define NON_PUBLIC(TYPE)                        \
    private:                                    \
    TYPE();                                     \

const unsigned int maxStringLength = 5 * 1024;

}; // namespace AppFrame
