/*装饰宠物模块  只跟随主人
//	AddBy: Nikai	2009.5.24
*/
#pragma once
class CPet;

class COrnaPet:public	CPet
{
public:
	COrnaPet();
	virtual ~COrnaPet();
	virtual void AI();
	virtual bool Display();
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData = true);
};