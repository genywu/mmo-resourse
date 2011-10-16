#pragma  once

class CSkillXml;
#include "Shape.h"
class CSkillAI : public CShape
{
public:
	CSkillAI(void);
	virtual ~CSkillAI(void);

	virtual void AI();
	virtual bool Display();
	virtual bool StepBeginAI(vector<long> vecType, vector<CGUID> vecID);
	virtual bool StepRunAI(vector<long> vecType, vector<CGUID> vecID);
	virtual bool StepEndAI();

	static CSkillAI* CreateSkillAI(CSkillXml* pSkill, long lAI);
protected:

private:
	
};