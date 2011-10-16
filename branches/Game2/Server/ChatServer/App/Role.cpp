#include "stdafx.h"
#include "Role.h"

bool operator<(const HRole& lhs, const HRole& rhs)
{
    return lhs->m_Id < rhs->m_Id;
}