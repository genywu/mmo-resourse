///
/// @file NumIDGen.h
/// @author Kevin Lynx
///
#ifndef ___NUM_ID_GEN_H_
#define ___NUM_ID_GEN_H_

class NumIDGen
{
public:
    NumIDGen( int base = 0 ) : m_Base( base )
    {
    }

    int Gen()
    {
        return m_Base++;
    }
private:
    int m_Base;
};

#endif
