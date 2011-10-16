
#include "stdafx.h"

#include "..\..\..\FrostEngine\utility.h"
#include "..\..\..\FrostEngine\console.h"
#include "..\..\..\FrostEngine\math.h"

#include "..\..\..\GameEngine\gamemap.h"
#include "..\..\..\GameEngine\gameselectray.h"
#include "..\..\..\GameEngine\DisplayModel.h"


#include "RoleModel.h"
#include "..\..\..\..\public\basedef.h"
//#include "clientregion.h"

CRoleModel::CRoleModel()
{
	m_iCurrentSelActionIndex = -1;
	m_pDisplayModel			 = NULL;
}

CRoleModel::~CRoleModel()
{	
}