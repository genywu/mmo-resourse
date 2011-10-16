/*\
 	created:	2005-5-2 11:35:32
 	filename: 	d:\Project\Server\WorldServer\AppWorld\CdkeyGoods.h
 	file path:	d:\Project\Server\WorldServer\AppWorld
 	file base:	CdkeyGoods
 	file ext:	h
 	author:		Bugs
 	
 	purpose:	Cdkey Goods Structure
\*/

#pragma once
#ifndef _INC_CCDKEYGOODS_H
#define _INC_CCDKEYGOODS_H

class CCdkeyGoods
{
public:
	CCdkeyGoods(void);
	virtual ~CCdkeyGoods(void);

public:
	string m_strDepotPassword;
	CBank m_cBank;
	CDepot m_cDepot;
};


#endif // _INC_CCDKEYGOODS_H
