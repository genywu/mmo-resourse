///
/// @file ScriptCaller.h
/// @author Kevin Lynx
///
///
#ifndef ___SCRIPT_CALLER_H_
#define ___SCRIPT_CALLER_H_

class CMoveShape;
class VariableList;
namespace StateAI
{
    class ScriptCaller
    {
    public:
        ScriptCaller( CMoveShape *shape );

        void AddParam( const char *varName, double varVal );

        void AddParam( const char *varName, const char *varVal );

        void AddParam( const char *varName, const CGUID &varVal );

        void Call( const char *script );
    private:
        VariableList *m_VarList;
        CMoveShape *m_Shape;
    };
}

#endif
