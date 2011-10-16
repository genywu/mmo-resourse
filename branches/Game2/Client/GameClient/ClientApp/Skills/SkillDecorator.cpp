#include "stdafx.h"
#include "SkillDecorator.h"

SkillComponent::~SkillComponent()
{

}
ConcreteSkillComp::ConcreteSkillComp()
{

}
ConcreteSkillComp::~ConcreteSkillComp()
{

}
bool ConcreteSkillComp::StepBegin(long lID, long lLevel,
								  long sType, const CGUID& sID,
								  long lDestX, long lDestY, float fDir, 
								  vector<long> vType, vector<CGUID> vID)
{
	//这里什么都不用做
	return true;
}
bool ConcreteSkillComp::StepSend(DWORD dwID,DWORD dwLevel,
								 CMoveShape *sShape, CMoveShape *dShape,
								 long dX, long dY)
{
	//这里什么都不用做
	return true;
}
bool ConcreteSkillComp::StepRun(long lID,long lLevel, long sType,
								const CGUID& sID, 
								long dX,long dY,float fDir, 
								vector<long>vType,vector<CGUID>vID)
{
	//这里什么都不做
	return true;
}
bool ConcreteSkillComp::StepEnd(long lID,long lLevel, long sType,
								const CGUID& sID,float fDir)
{
	//这里什么都不做
	return true;
}
bool ConcreteSkillComp::StepSummon()
{
	//这里什么都不做
	return true;
}
