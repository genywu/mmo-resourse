#pragma once
#include "CPet.h"

class CFairy:public	CPet
{
public:
	CFairy();
	virtual ~CFairy();
	virtual bool DecodePetFromDataBlock(DBReadSet& setReadDB, bool bExData = true);

	virtual void AI();
};