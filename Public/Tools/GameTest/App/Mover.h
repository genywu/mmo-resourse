///
///
///
#ifndef ___MOVER_H
#define ___MOVER_H

class CMoveShape;
class CFindPath;

/*
功能：浮点移动模块
作者：倪凯
日期：2010.7.7
*/
class CFloatMove
{
public:
    CFloatMove();
    ~CFloatMove();
    /*
    功能: 对A*寻路后的路径点 转换成 需要转向的线段点。
    作者：倪凯
    日期：2010.7.7
    */
    struct tagMovPos 
    {
        float x;
        float y;
        tagMovPos():x(0.f),y(0.f){}
        const tagMovPos& operator = (const tagMovPos &other)
        {
            if (&other == this)
            {
                return *this;
            }
            x = other.x;
            y = other.y;
        }
    };
    /*玩家操作的最小距离
    说明:(玩家当前位置和目标的距离超过此值 才移动)
    */
    const float MINIMUS_OP_DISTANCE;
    
    /*直线移动上的最大距离 (超过此值的两点将被拆分)*/
    const float MAX_LINE_DISTANCE;

    //A*寻路后 拆分转向点和超长路径点后的线段路径点 
    list<tagMovPos>  A_MoveList;       
    //直线移动拆分过长路径后的线段点
    list<tagMovPos>  LineMoveList;
    //转换A*路径为转向线段路径
    void UpdateA_MoveList(CFindPath *pFindPath,const float fPosX,
        const float fPosY,const float fDestX,
        const float fDestY);
    
    //直线移动时超过最大步长的拆分处理
    void UpdatelinearMoveList(const float fPosX,const float fPosY,
        const float fDestX, const float fDestY);
    //是否是直线移动
    bool  bIsBeelineMove;

    //上一个路径点是否是A*中的
    bool  bIsPreviousMoveByA_Star;

};

///
/// author: Kevin Lynx
///
class Mover
{
public:
    Mover( CMoveShape *owner );

    ~Mover();

    void Move( float destX, float destY );

    bool Update();

    void Reset();

    void MoveTo( float destX, float destY );

    void OnTimeOut( ulong timerid );

    void Stop();
private:
    void OnStop();

    void OnArrived();
private:
    float m_offsetX;
    float m_offsetY;
    CMoveShape *m_owner;
    ulong m_lastUpdate;
    ulong m_endTime;
    CFloatMove m_floatMove;
    ulong m_arriveTimer;
    bool m_arrived;
    bool m_finished;
};


#endif
