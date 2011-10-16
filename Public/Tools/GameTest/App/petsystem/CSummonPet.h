//’ŸªΩ¿‡≥ËŒÔ	addby:nikai	 2008.15.
#pragma once
#include "CPet.h"
class CSummonPet:public CPet
{
public:
	CSummonPet();
	virtual ~CSummonPet();
	virtual bool DecodePetFromDataBlock(DBReadSet& setReadDB, bool bExData = true);
	virtual void AI( );
};