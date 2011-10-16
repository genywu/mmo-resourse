///
/// @file ScriptBaseFunc.cpp
/// @brief ²Î¼û ScriptBaseFunc.h
///
#include "stdafx.h"
#include "../Script.h"

CGUID Script::GetSrcShapeID()
{
	return m_RunEnv.srcShape == NULL ? NULL_GUID : 
		m_RunEnv.srcShape->GetExID();
}

CGUID Script::GetDestShapeID()
{
	return m_RunEnv.destShape == NULL ? NULL_GUID : 
		m_RunEnv.destShape->GetExID();
}

CGUID Script::GetCurRgn()
{
    return m_RunEnv.region == NULL ? NULL_GUID :
        m_RunEnv.region->GetExID();
}
