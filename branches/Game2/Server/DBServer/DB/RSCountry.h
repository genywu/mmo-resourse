// 国家数据库存取内
// 提供访问数据库接口

#ifndef _DBCountry_h_
#define _DBCountry_h_
#include "../MyAdobase.h"

//#include "..\..\server\dbserver\appworld\DBcountry.h"
//#include "..\..\server\dbserver\appworld\country\countryhandler.h"
class CDBCountry;
class CMessage;
struct LoadThreadData;

class CRSCountry :
	public CMyAdoBase
{
public:
	CRSCountry(void);
	virtual ~CRSCountry(void);

public:
	bool							Load(std::list<LoadThreadData*>& loadDataList, _ConnectionPtr& cn);
	bool							Save(CDBCountry* pCountry ,_ConnectionPtr& cn);

};


#endif