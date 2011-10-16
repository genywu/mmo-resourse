///
/// @file OpPosEx.h
/// @author Kevin Lynx
/// @brief Position operator
///
#ifndef ___BUFFSKILL_OPPOSEX_H_
#define ___BUFFSKILL_OPPOSEX_H_

#include "BuffSkillDef.h"

class OpPositionEx : public COperator
{
public:
    enum DirType { DIR_FRONT, DIR_BACK, DIR_LEFT, DIR_RIGHT };
    enum PosType { POS_SIMPLE, POS_SLIP };

public:
    OpPositionEx( stModuParam *param );

    OpPositionEx( void *p );

    ~OpPositionEx();

    void InitBaseData( eObj obj, eObj refObj, const std::string &dir,
        const std::string &pos, const std::string &speed, float dis );

    void Execute( CMoveShape *shape );

private:
    int GetDirType( const std::string &dir );

    int GetPosType( const std::string &pos );

    float GetMoveDir( float faceDir );
private:
    int m_dirType;
    int m_posType;
    int m_refObj;
    float m_speed;
    float m_dis;
};

#endif
