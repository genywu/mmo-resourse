#pragma once
#include "CPet.h"

class CTrap:public	CPet
{
private:

public:
	CTrap();
	virtual ~CTrap();
	virtual bool DecodePetFromDataBlock(DBReadSet& setReadDB, bool bExData = true);
	virtual void AI( );
};