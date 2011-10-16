//’ŸªΩ¿‡≥ËŒÔ	addby:nikai	 2008.15.
#pragma once
class CPet;
class CSummonPet:public CPet
{
public:
	CSummonPet();
	virtual ~CSummonPet();
	virtual void AI();
	virtual bool Display();
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData = true);
};