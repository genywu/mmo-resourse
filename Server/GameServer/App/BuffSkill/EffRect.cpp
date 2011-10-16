///
/// @file EffRect.cpp
/// @author Kevin Lynx
/// 
#include "stdafx.h"
#include "EffRect.h"

EffRect::EffRect( stModuParam *pParam, CCondiGroup *pCondiGroup ) : 
    CEffectObjects( pParam, pCondiGroup )
{
    m_pExcept = NULL;
}

EffRect::EffRect( void *pParam ) :
    CEffectObjects(((tagTmpParam*)pParam)->pParam, ((tagTmpParam*)pParam)->pCondiGroup)
{
    m_pExcept = NULL;
}

EffRect::~EffRect(void)
{
    m_pExcept = NULL;
}

void EffRect::InitBaseData( const string &strPosition, float dis, float width ,int num,bool Within,string strNum)
{
    m_nNum = num;
    m_IsWithinObj = Within;
    m_strNum = strNum;
    m_ePos = GetPosition( strPosition );
    m_maxDis = dis;
    m_width = width;
}

unsigned int EffRect::GetEffectObjs( list<CMoveShape*> &listObjs )
{

    if ( m_strNum!="" )
    {
        /// by:expter 2010/08/31 技能增加一个参数
        m_nNum = m_pParam->pFirer->GetSkillValue(m_pParam->nID,m_pParam->nLvl,m_strNum);
    }
    if( m_nNum < 0 )
        m_nNum = 0;

    uint nNum = 0;

    // 取得作用点
    Graph::PosPair poss = GetOperPos();

    // 取得可能影响的SHAPE
    std::vector<CShape*> shapes;
    if( GetAroundShapes( poss, shapes ) == 0 )
    {
        return 0;
    }

    // 取得矩形4条边
    Graph::Line l1, l2, l3, l4;
    GetRectLines( poss, l1, l2, l3, l4 );

    if ( m_pExcept )
    {
#ifdef _SKILL_INFO_D_    
        Log4c::Trace(SKILL_MODULE, "EffRect:作用于对象外的玩家\n");    
#endif
    }
    // 判定每个SHAPE是否确实在矩形内
    for( std::vector<CShape*>::iterator it = shapes.begin();
        it != shapes.end(); ++ it )
    {
        CShape *shape = *it;
        if( shape == m_pParam->pUser ) continue;
        if( Graph::IsInLineRange( shape->GetPosX(), shape->GetPosY(), l1, l2 ) &&
            Graph::IsInLineRange( shape->GetPosX(), shape->GetPosY(), l3, l4 ) )
        {
            ++nNum;		
            if (m_nNum>0)
            {      
                if (m_nNum<(int)nNum)
                {
                    break;
                }
            }
            
            CMoveShape *moveShape = (CMoveShape*) shape;
            /// 除去范围内对象
            if( m_pExcept && m_pExcept == moveShape )
            {
                continue;                       
            }

            listObjs.push_back( moveShape );
        }
    }
    return nNum;
}

size_t EffRect::GetAroundShapes( const Graph::PosPair &poss, std::vector<CShape*> &shapes )
{
    CServerRegion *rgn = (CServerRegion*)(m_pParam->pUser->GetFather());
    long offset = (long) m_width;
    long sx = (long) poss.first.x;
    long sy = (long) poss.first.y;
    long dx = (long) poss.second.x;
    long dy = (long) poss.second.y;

    if( dx == sx && dy == sy ) // the same tile
    {
        rgn->GetShape( dx, dy, shapes ); 
    }
    else
    {
        long minx = min( sx, dx );
        long maxx = max( sx, dx );
        long miny = min( sy, dy );
        long maxy = max( sy, dy );
        minx -= offset;
        maxx += offset;
        miny -= offset;
        maxy += offset;
        long xOffset = maxx - minx + 1;
        for( ; miny <= maxy; ++ miny )
        {
            for( ; minx <= maxx; ++ minx )
            {
                rgn->GetShape( minx, miny, shapes );
            }
            minx -= xOffset;
        }
    }

    return shapes.size();    
}

Graph::PosPair EffRect::GetOperPos()
{
    Graph::Point p1, p2;
    p1.x = m_pParam->pUser->GetPosX();
    p1.y = m_pParam->pUser->GetPosY();
    p2.x = p2.y = 0.0f;

    CServerRegion *rgn = (CServerRegion*)(m_pParam->pUser->GetFather());
    m_pParam->pMouseShape = dynamic_cast<CMoveShape*>( 
        rgn->FindChildObject( m_pParam->nMouseShapeType, m_pParam->MouseShapeID ) );

    if( m_ePos == ePos_Dest && m_pParam->pMouseShape )
    {
        p2.x = m_pParam->pMouseShape->GetPosX();
        p2.y = m_pParam->pMouseShape->GetPosY();
        if( m_IsWithinObj)
            m_pExcept = m_pParam->pMouseShape;
    }
    else if( m_ePos == ePos_Coord )
    {
        p2.x = (float) m_pParam->nMouseX;
        p2.y = (float) m_pParam->nMouseY;
    }

    return std::make_pair( p1, p2 );
}

// l1//l2  l3//l4
void EffRect::GetRectLines( const Graph::PosPair &poss, Graph::Line &l1, Graph::Line &l2,
        Graph::Line &l3, Graph::Line &l4 )
{
    const Graph::Point &crossPos = poss.first;
    const Graph::Point &destPos = poss.second;

    // 由交叉点、终点、目标距离、宽度确定矩形的2个顶点和目标点
    std::pair<Graph::Point, Graph::PosPair> triPoss = Graph::GetPosInVerLins( m_maxDis,
        m_width, crossPos, destPos );
    const Graph::Point &rp1 = triPoss.second.first;
    const Graph::Point &rp2 = triPoss.second.second;
    const Graph::Point &disPos = triPoss.first;

    // 根据坐标偏移确定矩形的另2个顶点
    float xOffset = rp1.x - crossPos.x;
    float yOffset = rp1.y - crossPos.y;
    Graph::Point rp3, rp4;
    rp3.x = disPos.x + xOffset;
    rp3.y = disPos.y + yOffset;
    rp4.x = disPos.x - xOffset;
    rp4.y = disPos.y - yOffset;

    // 由4个顶点确定2组平行边
    l1 = Graph::GetLine( rp1.x, rp1.y, rp2.x, rp2.y );
    l2 = Graph::GetParaLine( l1, rp3.x, rp3.y );
    l3 = Graph::GetLine( rp1.x, rp1.y, rp3.x, rp3.y );
    l4 = Graph::GetParaLine( l3, rp4.x, rp4.y );
}

