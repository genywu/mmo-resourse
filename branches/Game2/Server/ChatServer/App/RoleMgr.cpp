#include "stdafx.h"
#include "RoleMgr.h"
#include "../Net/Msg.h"

// warning: the "tmp" is used only in search.
static HRole tmp(new Role);

const HRole RoleMgr::FindRole(uint roleSocketId)const
{
    tmp->m_SocketId = roleSocketId;
    SocketSet::const_iterator it = m_Sockets.find(tmp);
    if (it == m_Sockets.end())
        return HRole();

    return *it;
}

const HRole RoleMgr::FindRole(const CGUID& roleId)const
{
    tmp->m_Id = roleId;
    RoleSet::const_iterator it = m_Roles.find(tmp);
    if (it == m_Roles.end())
        return HRole();

    return *it;
}

const HRole RoleMgr::FindRole(uint wsId, const string& roleName)const
{
    map<uint, NameSet>::const_iterator itWs = m_Names.find(wsId);
    if (itWs == m_Names.end())
        return HRole();

    tmp->m_Name = roleName;
    NameSet::const_iterator it = itWs->second.find(tmp);
    if (it == itWs->second.end())
        return HRole();

    return *it;
}

void RoleMgr::AddRole(const HRole& role)
{
    m_Roles.insert(role);
    m_Sockets.insert(role);
    m_Names[role->m_WsId].insert(role);
    if (role->m_TeamId != NULL_GUID)
        m_Teams[role->m_TeamId].insert(role);
    if (role->m_FactionId != NULL_GUID)
        m_Factions[role->m_FactionId].insert(role);
    m_Regions[role->m_WsId][role->m_RegionId].insert(role);
    m_GameServers[role->m_GsSocketId].insert(role);
}

void RoleMgr::DelRole(const HRole& role)
{
    m_Roles.erase(role);
    m_Sockets.erase(role);
    m_Names[role->m_WsId].erase(role);
    if (role->m_TeamId != NULL_GUID)
    {
        map<CGUID, RoleSet>::iterator itTeam = m_Teams.find(role->m_TeamId);
        if (itTeam != m_Teams.end())
        {
            itTeam->second.erase(role);
            if (itTeam->second.empty())
            {
                m_Teams.erase(itTeam);
            }
        }
    }
    if (role->m_FactionId != NULL_GUID)
    {
        map<CGUID, RoleSet>::iterator itFaction = m_Factions.find(role->m_FactionId);
        if (itFaction != m_Factions.end())
        {
            itFaction->second.erase(role);
            if (itFaction->second.empty())
            {
                m_Factions.erase(itFaction);
            }
        }
    }
    m_Regions[role->m_WsId][role->m_RegionId].erase(role);
    m_GameServers[role->m_GsSocketId].erase(role);
}

void RoleMgr::PrivateChat(const HRole& sender, const string& chatStr, const string& receiverName)const
{
    tmp->m_Name = receiverName;
    const HRole receiver = FindRole(sender->m_WsId, receiverName);
    if (receiver.Bound())
    {
        if (receiver->m_Channels & CHANNEL_PRIVATE)
        {
            Msg msg(MSG_CS2C_CHAT);
            msg.Add((long)CHAT_PRIVATE);
            msg.Add(sender->m_Name.c_str());
            msg.Add(chatStr.c_str());
            msg.SendToClient(receiver->m_SocketId);
        }
    }
    else
    {
    }
}

void RoleMgr::ScreenChat(const HRole& sender, const string& chatStr, const vector<CGUID>& receiverIds)const
{
    for (vector<CGUID>::const_iterator it = receiverIds.begin();
        it != receiverIds.end(); ++it)
    {
        // finds receiver
        tmp->m_Id = *it;
        RoleSet::const_iterator itRole = m_Roles.find(tmp);
        if (itRole != m_Roles.end())
        {
            const HRole& receiver = *itRole;
            if ((receiver->m_Channels & CHANNEL_SCREEN)
                && receiver->m_WsId == sender->m_WsId
                && receiver->m_RegionId == sender->m_RegionId)//avoid cheating
            {
                Msg msg(MSG_CS2C_CHAT);
                msg.Add((long)CHAT_NORMAL);
                msg.Add(sender->m_Name.c_str());
                msg.Add(chatStr.c_str());
                msg.SendToClient(receiver->m_SocketId);
            }
        }
    }
}

void RoleMgr::RegionChat(const HRole& sender, const string& chatStr)const
{
    // finds the world
    map<uint, map<uint, RoleSet> >::const_iterator itWsMap = m_Regions.find(sender->m_WsId);
    if (itWsMap == m_Regions.end())
        return;

    // finds the region from the world
    map<uint, RoleSet>::const_iterator itRgnMap = itWsMap->second.find(sender->m_RegionId);
    if (itRgnMap == itWsMap->second.end())
        return;

    // sends messages to the roles in that region
    const RoleSet& roleSet = itRgnMap->second;
    for (RoleSet::const_iterator itRole = roleSet.begin();
        itRole != roleSet.end(); ++itRole)
    {
        const HRole& receiver = *itRole;
        if (receiver->m_Channels & CHANNEL_REGION)
        {
            Msg msg(MSG_CS2C_CHAT);
            msg.Add((long)CHAT_REGION);
            msg.Add(sender->m_Name.c_str());
            msg.Add(chatStr.c_str());
            msg.SendToClient(receiver->m_SocketId);
        }
    }
}

void RoleMgr::WorldChat(const HRole& sender, const string& chatStr)const
{
    // finds the world
    map<uint, NameSet>::const_iterator itWs = m_Names.find(sender->m_WsId);
    if (itWs == m_Names.end())
        return;
    const NameSet& world = itWs->second;

    // sends messages to the roles in that world
    for (NameSet::const_iterator itRole = world.begin();
        itRole != world.end(); ++itRole)
    {
        const HRole& receiver = *itRole;
        if (receiver->m_Channels & CHANNEL_WORLD)
        {
            Msg msg(MSG_CS2C_CHAT);
            msg.Add((long)CHAT_WORLD);
            msg.Add(sender->m_Name.c_str());
            msg.Add(chatStr.c_str());
            msg.SendToClient(receiver->m_SocketId);
        }
    }
}

void RoleMgr::TeamChat(const HRole& sender, const string& chatStr)const
{
    // finds the team
    map<CGUID, RoleSet>::const_iterator itTeam = m_Teams.find(sender->m_TeamId);
    if (itTeam == m_Teams.end())
        return;
    const RoleSet& roleSet = itTeam->second;

    // sends messages to the roles in that team
    for (RoleSet::const_iterator itRole = roleSet.begin();
        itRole != roleSet.end(); ++itRole)
    {
        const HRole& receiver = *itRole;
        if (receiver->m_Channels & CHANNEL_TEAM)
        {
            Msg msg(MSG_CS2C_CHAT);
            msg.Add((long)CHAT_TEAM);
            msg.Add(sender->m_Name.c_str());
            msg.Add(chatStr.c_str());
            msg.SendToClient(receiver->m_SocketId);
        }
    }
}

void RoleMgr::FactionChat(const HRole& sender, const string& chatStr)const
{
    // finds the faction
    map<CGUID, RoleSet>::const_iterator itFaction = m_Factions.find(sender->m_FactionId);
    if (itFaction == m_Factions.end())
        return;
    const RoleSet& roleSet = itFaction->second;

    // sends messages to the roles in that faction
    for (RoleSet::const_iterator itRole = roleSet.begin();
        itRole != roleSet.end(); ++itRole)
    {
        const HRole& receiver = *itRole;
        if (receiver->m_Channels & CHANNEL_FACTION)
        {
            Msg msg(MSG_CS2C_CHAT);
            msg.Add((long)CHAT_FACTION);
            msg.Add(sender->m_Name.c_str());
            msg.Add(chatStr.c_str());
            msg.SendToClient(receiver->m_SocketId);
        }
    }
}

void RoleMgr::ChangeRegion(const CGUID& roleId, uint regionId, uint gsSocketId)
{
    // finds the role
    tmp->m_Id = roleId;
    RoleSet::iterator it = m_Roles.find(tmp);
    if (it == m_Roles.end())
        return;
    HRole role = *it;

    // changes region
    if (role->m_RegionId != regionId)
    {
        m_Regions[role->m_WsId][role->m_RegionId].erase(role);
        role->m_RegionId = regionId;
        m_Regions[role->m_WsId][role->m_RegionId].insert(role);
    }

    // maybe changes game server
    if (role->m_GsSocketId != gsSocketId)
    {
        m_GameServers[role->m_GsSocketId].erase(role);
        role->m_GsSocketId = gsSocketId;
        m_GameServers[role->m_GsSocketId].insert(role);
    }
}

void JoinTeam(const CGUID& roleId, const CGUID& teamId){}
void LeaveTeam(const CGUID& roleId, const CGUID& teamId){}
void JoinFaction(const CGUID& roleId, const CGUID& factionId){}
void LeaveFaction(const CGUID& roleId, const CGUID& factionId){}
void DisbandTeam(const CGUID& teamId){}
void DisbandFaction(const CGUID& factionId){}

void RoleMgr::OnGsDisconnect(uint gsSocketId)
{
    // finds game server
    map<uint, RoleSet>::iterator itGsMap = m_GameServers.find(gsSocketId);
    if (itGsMap == m_GameServers.end())
        return;
    RoleSet& gs = itGsMap->second;

    // deletes all roles in that server
    while (!gs.empty())
    {
        HRole role = *gs.begin();
        DelRole(role);
        //warning: 这里绝对不要简写成DelRole(*gs.begin())。
        //句柄的引用实在是个危险的东西。
    }
    //以下是原先的写法，也是不对的。
    //for (RoleSet::iterator it = gs.begin();
    //    it != gs.end(); ++it)
    //{
    //    DelRole(*it);
    //}
}

void RoleMgr::AddRole_Unchecked(const HRole& role)
{
    m_UncheckedRoles.insert(role);
}

const HRole RoleMgr::PopRole_Unchecked(const CGUID& roleId)
{
    tmp->m_Id = roleId;
    RoleSet::iterator it = m_UncheckedRoles.find(tmp);
    if (it != m_UncheckedRoles.end())
    {
        HRole role = *it;
        m_UncheckedRoles.erase(it);
        return role;
    }
    else
        return HRole();
}