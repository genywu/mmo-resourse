///
/// @file EffSector.h
/// @author Kevin Lynx
/// 
#ifndef ___SKILL_EFFSECTOR_H_
#define ___SKILL_EFFSECTOR_H_

#include "EffectObjects.h"
#include "../../../../Public/Module/GraphUtils.h"

class EffSector : public CEffectObjects
{
public:
	struct tagTmpParam
	{
		stModuParam *pParam;
		CCondiGroup *pCondiGroup;
	};
	EffSector( stModuParam *pParam, CCondiGroup *pCondiGroup );

	EffSector( void *pParam );

	~EffSector(void);
public:
	void InitBaseData( const string &strPosition, float dis, float angle,int num,bool Within,string strNum);

	unsigned int GetEffectObjs( list<CMoveShape*> &listObjs );

private:
    Graph::PosPair GetOperPos();

    size_t GetAroundShapes( std::vector<CShape*> &shapes );

private:
    //是否是范围内对象
    bool      m_IsWithinObj;
    ePosition m_ePos;
    float     m_maxDis;
    float     m_angle;
    //作用数量
    int       m_nNum;
    //作用对象变量
    string    m_strNum;   

private:
    CMoveShape* m_pExcept;
};

#endif
