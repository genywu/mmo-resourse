///
/// @file GraphUtils.h
/// @brief some util functions for Graphics( line/rectangle/cycle etc )
///
#ifndef ___GRAPH_UTILS_H_
#define ___GRAPH_UTILS_H_

#include <math.h>
#include <utility>

namespace Graph
{
    const float PI = 3.141592654f;

    /// Check whether the two float numbers equal.
    inline bool FloatEq( float a, float b )
    {
        return abs( a - b ) < 0.001f;
    }

    /// ax + b = y
    struct Line
    {
        float a;
        float b;
        enum { NOA, HASA } flag;
        enum RelPos { LEFT, RIGHT, ON };
    };

    struct Point
    {
        float x, y;
        Point( float xp = 0.0f, float yp = 0.0f ) : x( xp ), y( yp ) {}
    };

    enum RetType { ANGLE, TANA };

    typedef std::pair<Point, Point> PosPair;

    /// Get a line by two points.
    Line GetLine( float x1, float y1, float x2, float y2 );

    /// Get a parallel line.
    Line GetParaLine( const Line &l, float x, float y );

    /// Get the relative position of a point.
    int GetRelPos( const Line &l, float x, float y );

    /// Get a vertical line from the line.
    Line GetVerLine( const Line &l, float x, float y );

    /// The two lines must be parallel.
    bool IsInLineRange( float x, float y, const Line &l1, const Line &l2 );

    /// Get a point on the line at the specified distance.
    PosPair GetPosOnLine( const Line &l, float x, float y, float dis );

    /// Get a point between (sx,sy) and (dx,dy) at dis distance.
    Point GetPosOnLine( float sx, float sy, float dx, float dy, float dis );

    /// Angle in radian.
    Point GetPosOnLine( float sx, float sy, float angle, float dis );

    /// Get the point on the direction on the line.
    Point GetPosOnDir( float sx, float sy, float refX, float refY, const PosPair &poss );

    float GetLineX( const Line &l, float y, float defX );

    float GetLineY( const Line &l, float x, float defY );

    /// Get 3 points on the two lines vertical each other.
    /// Suppose dis1 on line1, dis2 on line2, destPos on line1.
    std::pair<Point, PosPair> GetPosInVerLins( float dis1, float dis2, const Point &crossPos, const Point &destPos );

    float Angle2Radian( float angle );

    /// Left is PI/2, right is -PI/2, back is PI.
    float OffsetAngle( float radian, float offset );

    /// Get tan value between l and the line determined by crossPos and destPos points.
    /// The angle >= 0 and <= 180. tana = (k1-k2)/(1+k1*k2)
    /// The Line::a in l must >= 0.
    std::pair<RetType, float> GetTanOnDir( const Line &l, const Point &crossPos, 
        const Point &destPos, const Point &chkPos );

    /// The open sector is determined by crossPos, destPos and the angle.
    bool IsInOpenSector( const Line &l, const Point &crossPos, 
        const Point &destPos, const Point &chkPos, float angle, float tana );
}

#endif
