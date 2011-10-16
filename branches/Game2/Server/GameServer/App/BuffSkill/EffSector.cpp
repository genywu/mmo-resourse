///
/// @file EffSector.cpp
/// @author Kevin Lynx
/// 
#include "stdafx.h"
#include "EffSector.h"

EffSector::EffSector( stModuParam *pParam, CCondiGroup *pCondiGroup ) : 
    CEffectObjects( pParam, pCondiGroup )
{
    m_pExcept = NULL;
}

EffSector::EffSector( void *pParam ) :
    CEffectObjects(((tagTmpParam*)pParam)->pParam, ((tagTmpParam*)pParam)->pCondiGroup)
{
    m_pExcept = NULL;
}

EffSector::~EffSector(void)
{
    m_pExcept = NULL;
}

void EffSector::InitBaseData( const string &strPosition, float dis, float angle , int num ,bool Within,string strNum)
{
    m_nNum = num;
    m_IsWithinObj = Within;
    m_strNum = strNum;
    m_ePos = GetPosition( strPosition );
    m_maxDis = dis;
    m_angle = angle;
}

unsigned int EffSector::GetEffectObjs( list<CMoveShape*> &listObjs )
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
    const Graph::Point &crossPos = poss.first;
    const Graph::Point &destPos = poss.second;

    // 取得可能影响的SHAPE
    std::vector<CShape*> shapes;
    if( GetAroundShapes( shapes ) == 0 )
    {
        return 0;
    }

    // 扇形中轴线
    Graph::Line l = Graph::GetLine( crossPos.x, crossPos.y, destPos.x, destPos.y );
    // 扇形角度一半的tan值
    float tana = tan( Graph::Angle2Radian( m_angle ) );

    if ( m_pExcept)
    {
#ifdef _SKILL_INFO_D_
        Log4c::Trace(SKILL_MODULE, "EffSector:作用于对象外的玩家");    
#endif
    }
    // 判定每个SHAPE是否确实在扇形内
    for( std::vector<CShape*>::iterator it = shapes.begin();
        it != shapes.end(); ++ it )
    {
        CShape *shape = *it;
        if( shape == m_pParam->pUser ) continue;
        Graph::Point chkPos( shape->GetPosX(), shape->GetPosY() );
        if( Distance( chkPos.x, chkPos.y, crossPos.x, crossPos.y ) <= m_maxDis &&
            Graph::IsInOpenSector( l, crossPos, destPos, chkPos, m_angle, tana ) )
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

size_t EffSector::GetAroundShapes( std::vector<CShape*> &shapes )
{
    CServerRegion *rgn = (CServerRegion*)(m_pParam->pUser->GetFather());
    rgn->FindAroundObject( m_pParam->pUser, TYPE_MONSTER, shapes );
    rgn->FindAroundObject( m_pParam->pUser, TYPE_PLAYER, shapes );
    return shapes.size();
}

Graph::PosPair EffSector::GetOperPos()
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
