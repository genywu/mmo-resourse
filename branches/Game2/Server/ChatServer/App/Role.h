#ifndef ROLE_H
#define ROLE_H

enum ChannelType
{
    CHANNEL_PRIVATE = 1,
    CHANNEL_TEAM    = 1 << 1,
    CHANNEL_FACTION = 1 << 2,
    CHANNEL_REGION  = 1 << 3,
    CHANNEL_COUNTRY = 1 << 4,
    CHANNEL_WORLD   = 1 << 5,
    CHANNEL_SCREEN  = 1 << 6,
};

// warning: use "Role" only if initialize "HRole"
// e.g.
// HRole role(new Role);
struct Role
{
    string m_Name;
    CGUID m_Id;
    CGUID m_TeamId;
    CGUID m_FactionId;
    uint m_RegionId;
    uint m_CountryId;
    uint m_WsId;        // world server id
    uint m_Channels;    // channels bitset, see enum ChannelType
    uint m_SignCode;    // sign code used for validate, see WS for details
    uint m_GsSocketId;  // game server socket id
    uint m_SocketId;

    Role()
        :m_Name(""), m_Id(NULL_GUID), m_TeamId(NULL_GUID), m_FactionId(NULL_GUID), 
        m_RegionId(0), m_CountryId(0), m_WsId(0), m_Channels(~0), m_SignCode(0), m_GsSocketId(0), m_SocketId(0){}
};

#include "../../../Public/Module/Handle.h"
typedef Handle<Role> HRole;

bool operator<(const HRole& lhs, const HRole& rhs);

struct SocketIdCmp
{
    bool operator()(const HRole& lhs, const HRole& rhs) const
    {
        return lhs->m_SocketId < rhs->m_SocketId;
    }
};

struct RoleNameCmp
{
    bool operator()(const HRole& lhs, const HRole& rhs) const
    {
        return lhs->m_Name < rhs->m_Name;
    }
};

#endif//#ifndef ROLE_H