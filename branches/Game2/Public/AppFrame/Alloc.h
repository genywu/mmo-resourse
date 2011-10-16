// -*- C++ -*-

/**
 * @file    Alloc.h
 * @date    Wed Nov 18 15:37:18 CST 2009
 * @author  Fox(yulefox.at.gmail.com)
 */

#pragma once

namespace AppFrame {

// These two macros simply encapsulate new and delete oprators. They would
// be instead in future.

#define GAME_OBJ_NEW(POINTER, CONSTRUCTOR)      \
    POINTER = new CONSTRUCTOR

#define GAME_OBJ_DELETE(POINTER)                \
    do {                                        \
        delete POINTER;                         \
        POINTER = NULL;                         \
    } while (0)

template<class Type>
class Alloc
{
public:
    static Type* New() {
        return new Type();
    }

    static void Delete(Type* instance) {
        delete instance;
        instance = 0;
    }

    static const bool mDeleteAtExit = false;
}; // class Alloc

}; // namespace AppFrame

using AppFrame::Alloc;
