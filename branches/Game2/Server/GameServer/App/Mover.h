///
/// @file Mover.h
/// @author Kevin Lynx
/// @brief check out float-moving doc for more details.
///
#ifndef ___MOVER_H_
#define ___MOVER_H_

class CMoveShape;

#define MOVER_TEST

/// Manage the movement for shape, specific for player and pet.
class Mover
{
private:
    enum { INVALID_TIMER = -1 };
    enum { WAIT_STOP, WAIT_MOVE };
public:
    Mover();

    ~Mover();

    void Init( CMoveShape *owner );

    void Release();

    bool IsMoving() const;

    void OnTimeOut();

    void OnMove( float clientX, float clientY, float destX, float destY );

    void OnStopMove( float clientX, float clientY );

    void OnSetPos( float x, float y );

    float GetDestX() const { return m_destX; }
    float GetDestY() const { return m_destY; }
private:

    void OnMoveNoCheck( float clientX, float clientY, float destX, float destY );

    void OnStopMoveNoCheck( float clientX, float clientY );

    bool CheckBlock( float srcX, float srcY, float destX, float destY );

    bool CheckBlockAStar( float srcX, float srcY, float destX, float destY );

    bool CheckBlockDirect( float srcX, float srcY, float destX, float destY );

    void OnFailed();

    void Start();

    void Stop();

    bool Check();

    void ResetChkData();

    bool IsFar( float clientX, float clientY );

    bool IsBlock( float x, float y );

    bool IsDestPos( float x, float y );

    void AddErrRecord( float clientX, float clientY );

    void SetChkStart( float clientX, float clientY );

    bool IsBlock( int x, int y ) { return IsBlock( (float)x, (float)y ); }

    void KickOut();

    void OnStart();

    void OnStop();

    void OnArrive();
private:
    CMoveShape *m_owner;
    long m_timerID;
    float m_destX;
    float m_destY;
    float m_offsetX;
    float m_offsetY;
    ulong m_chkStartTime;
    int m_requestCnt;
    float m_chkStartX;
    float m_chkStartY;
    float m_relErrDis;
    int m_msgState;
    int m_updateCnt;
#ifdef MOVER_TEST
    char m_logFile[256];
#endif
};

#endif
