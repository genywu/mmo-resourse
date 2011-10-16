///
/// @file GraphUtils.cpp
/// @brief some util functions for Graphics( line/rectangle/cycle etc )
///
#include "stdafx.h"
#include <assert.h>
#include "GraphUtils.h"

#define LINEB( x, y, a ) ( y - a * x )

namespace Graph
{
    Line GetLine( float x1, float y1, float x2, float y2 )
    {
        Line l;
        if( FloatEq( x1, x2 ) )
        {
            l.flag = Line::NOA;
            l.a = x1;
            l.b = 0.0f;
        }
        else
        {
            l.flag = Line::HASA;
            l.a = (y2 - y1) / (x2 - x1);
            l.b = LINEB( x1, y1, l.a );
        }
        return l;
    }

    Line GetParaLine( const Line &l, float x, float y )
    {
        Line rl;
        rl.flag = l.flag;
        if( l.flag == Line::NOA )
        {
            rl.a = x;
            rl.b = 0.0f;
        }
        else
        {
            rl.a = l.a;
            rl.b = LINEB( x, y, rl.a );
        }
        return rl;
    }

    Line GetVerLine( const Line &l, float x, float y )
    {
        Line rl;
        if( l.flag == Line::NOA )
        {
            rl.flag = Line::HASA;
            rl.a = 0.0f;
            rl.b = y;
        }
        else if( FloatEq( l.a, 0.0f ) )
        {
            rl.flag = Line::NOA;
            rl.a = x;
            rl.b = 0.0f;
        }
        else
        {
            rl.flag = Line::HASA;
            rl.a = -1.0f / l.a;
            rl.b = LINEB( x, y, rl.a );
        }
        return rl;
    }

#define CHECK_RETURN( x, a, b ) \
    if( FloatEq( x, a ) ) return Line::ON; \
    if( x < b ) return Line::LEFT; \
    return Line::RIGHT;

    int GetRelPos( const Line &l, float x, float y )
    {
        if( l.flag == Line::NOA )
        {
            CHECK_RETURN( x, l.a, l.a );
        }
        else if( FloatEq( l.a, 0.0f ) )
        {
            CHECK_RETURN( -y, -l.b, -l.b );
        }
        else if( l.a < 0.0f )
        {
            float r = l.a * x + l.b - y;
            CHECK_RETURN( r, 0.0f, 0.0f );
        }
        float r = l.a * x + l.b - y;
        CHECK_RETURN( r, 0.0f, 0.0f );
    }

    int CompLineVal( float x, float y, const Line &l )
    {
        float ret = l.a * x + l.b - y;
        if( FloatEq( ret, 0.0f ) )
        {
            return 0;
        }
        if( ret > 0.0f )
        {
            return 1;
        }
        return -1;
    }

    bool IsInLineRange( float x, float y, const Line &l1, const Line &l2 )
    {
        if( l1.flag == Line::HASA )
        {
            int l1Val = CompLineVal( x, y, l1 );
            int l2Val = CompLineVal( x, y, l2 );
            return l1Val == 0 || l2Val == 0 || l1Val != l2Val;
        }
        float minx = min( l1.a, l2.a );
        float maxx = max( l1.a, l2.a );
        return x >= minx && x <= maxx;
    }

    PosPair GetPosOnLine( const Line &l, float x, float y, float dis )
    {
        Point p1, p2;
        if( l.flag == Line::NOA )
        {
            p1.y = y + dis;
            p1.x = x;
            p2.y = y - dis;
            p2.x = x;
        }
        else
        {
            float angle = atan( l.a );
            float yoffset = sin( angle ) * dis;
            float xoffset = cos( angle ) * dis;
            p1.x = x + xoffset;
            p1.y = y + yoffset;
            p2.x = x - xoffset;
            p2.y = y - yoffset;
        }
        return std::make_pair( p1, p2 );
    }

    Point GetPosOnLine( float sx, float sy, float dx, float dy, float dis )
    {
        Point p;
        float xOffset = dx - sx;
        float yOffset = dy - sy;
        float ddis = sqrt( xOffset * xOffset + yOffset * yOffset );
        float ratio = dis / ddis;
        p.y = sy + ratio * yOffset;
        p.x = sx + ratio * xOffset;
        return p;
    }

    Point GetPosOnLine( float sx, float sy, float angle, float dis )
    {
        float xOffset = dis * cos( angle );
        float yOffset = dis * sin( angle );
        return Point( sx + xOffset, sy + yOffset );
    }

#define BOOLSAME( b1, b2 ) ((b1&&b2) || (!b1&&!b2))

    bool IsPosBetween( float sx, float sy, float refX,float refY, const Point &pos )
    {
        bool xLess = refX - sx < 0.0f;
        bool yLess = refY - sy < 0.0f;
        bool xLessP1 = pos.x - sx < 0.0f;
        bool yLessP1 = pos.y - sy < 0.0f;
        if( FloatEq( pos.x, sx ) || FloatEq( pos.x, refX ) )
        {
            xLessP1 = BOOLSAME( yLess, yLessP1 ) ? xLess : !xLess;
        }
        if( FloatEq( pos.y, sy ) || FloatEq( pos.y, refY ) )
        {
            yLessP1 = BOOLSAME( xLess, xLessP1 ) ? yLess : !yLess;
        }

        return BOOLSAME( xLess, xLessP1 ) || BOOLSAME( yLess, yLessP1 );
    }

    Point GetPosOnDir( float sx, float sy, float refX, float refY, const PosPair &poss )
    {
        const Point &p1 = poss.first;
        const Point &p2 = poss.second;
        if( IsPosBetween( sx, sy, refX, refY, p1 ) )
        {
            return p1;
        }
        return p2;
    }

    float GetLineX( const Line &l, float y, float defx )
    {
        if( l.flag == Line::NOA )
        {
            return l.a;
        }
        else if( FloatEq( l.a, 0.0f ) )
        {
            return defx;
        }
        return ( y - l.b ) / l.a;
    }

    float GetLineY( const Line &l, float x, float defy )
    {
        if( l.flag == Line::NOA )
        {
            return defy;
        }
        else if( FloatEq( l.a, 0.0f ) )
        {
            return 0.0f;
        }
        return l.a * x + l.b;
    }

    std::pair<Point, PosPair> GetPosInVerLins( float dis1, float dis2, const Point &crossPos, const Point &destPos )
    {
        Point p1, p2, p3;
        float xOffset = destPos.x - crossPos.x;
        float yOffset = destPos.y - crossPos.y;
        float dis3 = sqrt( xOffset * xOffset + yOffset * yOffset );
        float disRatio = dis1 / dis3;
        // p1 is at dis1 from crossPos on line1
        p1.x = disRatio * xOffset + crossPos.x;
        p1.y = disRatio * yOffset + crossPos.y;
        // p2 is at dis2 from crossPos on line2
        disRatio = dis2 / dis3;
        p2.x = disRatio * yOffset + crossPos.x;
        p2.y = -disRatio * xOffset + crossPos.y;
        // p3 is on line2, and at the same dis(dis2) as p2
        p3.x = crossPos.x - ( p2.x - crossPos.x );
        p3.y = crossPos.y - ( p2.y - crossPos.y );

        return std::make_pair( p1, std::make_pair( p2, p3 ) );
    }

    std::pair<RetType, float> GetTanOnDir( const Line &l, const Point &crossPos, 
        const Point &destPos, const Point &chkPos )
    {
        int relPos = GetRelPos( l, chkPos.x, chkPos.y );
        bool between = IsPosBetween( crossPos.x, crossPos.y, destPos.x, destPos.y, chkPos );
        if( relPos == Line::ON )
        {
            return between ? std::make_pair( ANGLE, 0.0f ) :
                std::make_pair( ANGLE, 180.0f );
        }
        Line l2 = GetLine( crossPos.x, crossPos.y, chkPos.x, chkPos.y );
        if( l.flag == Line::NOA && FloatEq( l2.a, 0.0f ) ||
            l2.flag == Line::NOA && FloatEq( l.a, 0.0f ) )
        {
            return std::make_pair( ANGLE, 90.0f );
        }
        if( l.flag == Line::NOA )
        {
            return std::make_pair( TANA, between ? 1 / abs( l2.a ) : -1 / abs( l2.a ) );
        }
        if( l2.flag == Line::NOA )
        {
            return std::make_pair( TANA, between ? 1 / abs( l.a ) : -1 / abs( l.a ) );
        }
        if( FloatEq( l.a * l2.a, -1.0f ) )
        {
            return std::make_pair( ANGLE, 90.0f );
        }
        // god damn...the normal case comes finaly
        float k1, k2;
        if( relPos == Line::LEFT )
        {
            k1 = l.a;
            k2 = l2.a;
        }
        else
        {
            k1 = l2.a;
            k2 = l.a;
        }
        return std::make_pair( TANA, ( k2 - k1 ) / ( 1 + k1 * k2 ) );
    }

    float Angle2Radian( float angle )
    {
        return 2 * PI / 360 * angle;
    }

    float OffsetAngle( float radian, float offset )
    {
        return radian + offset;
    }

    bool IsInOpenSector( const Line &l, const Point &crossPos, 
        const Point &destPos, const Point &chkPos, float angle, float tana )
    {
        std::pair<Graph::RetType, float> fret = Graph::GetTanOnDir( l, crossPos, destPos, 
            chkPos );

        bool in = false;
        if( fret.first == Graph::ANGLE )
        {
            in = fret.second <= angle;
        }
        else if( tana < 0 && fret.second > 0 ||
            tana < 0 && fret.second < 0 && tana > fret.second ||
            tana > 0 && fret.second > 0 && tana > fret.second )
        {
            in = true; 
        }
        return in;
    }
}
