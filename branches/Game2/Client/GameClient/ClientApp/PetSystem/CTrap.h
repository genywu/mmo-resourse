/*ÏÝÚå nikai 2009/3/13*/
#pragma once
class CPet;

class CTrap:public	CPet
{
private:
	int counter ;
public:
	CTrap();
	virtual ~CTrap();
	virtual void AI();
	virtual bool Display();
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData = true);
};