#pragma once

#include "..\..\engine\render.h"
#include "..\..\engine\ui.h"

enum CELL_MATERIAL
{
		CM_NULL = 0, //¿Õ
		CM_GRASS,    //²ÝµØ
		CM_MUD,		 //Äà°Í
		CM_SAND,     //É³µØ
		CM_WOOD,     //Ä¾°å
		CM_STONE,    //Ê¯°å
		CM_WATER,    //Ë®
		CM_SNOW,	 //Ñ©µØ
		CM_MARISH,	 //ÕÓÔó
		CM_END
};

class MtlListBox : public ui::ListBox
{
public:

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent);

	MtlListBox(void);
	~MtlListBox(void);
};
