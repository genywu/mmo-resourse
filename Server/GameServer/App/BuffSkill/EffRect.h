///
/// @file EffRect.h
/// @author Kevin Lynx
/// 
#ifndef ___SKILL_EFFRECT_H_
#define ___SKILL_EFFRECT_H_

#include "EffectObjects.h"
#include "../../../../Public/Module/GraphUtils.h"

class EffRect : public CEffectObjects
{
public:
	struct tagTmpParam
	{
		stModuParam *pParam;
		CCondiGroup *pCondiGroup;
	};
	EffRect( stModuParam *pParam, CCondiGroup *pCondiGroup );

	EffRect( void *pParam );

	~EffRect(void);
public:
	void InitBaseData( const string &strPosition, float dis, float width,int num,bool Within,string strNum);

	unsigned int GetEffectObjs( list<CMoveShape*> &listObjs );

private:

    /// 取可能被影响到的SHAPES。
    size_t GetAroundShapes( const Graph::PosPair &poss, std::vector<CShape*> &shapes );

    /// 取得原点和目标点
    Graph::PosPair GetOperPos();

    /// 取得矩形两组平行线
    void GetRectLines( const Graph::PosPair &poss, Graph::Line &l1, Graph::Line &l2,
        Graph::Line &l3, Graph::Line &l4 );

private:
    //是否是范围内对象
    bool      m_IsWithinObj;
	ePosition m_ePos;
    float     m_maxDis;
    float     m_width;
    int       m_nNum;
    string    m_strNum;

private:
    CMoveShape *m_pExcept;
};

#endif
