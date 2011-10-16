#ifndef ROLEMGR_H
#define ROLEMGR_H

#include "Role.h"

// RoleMgr is short for "role manager".
// using RoleMgr class as a singleton, e.g. GetInst(RoleMgr).
class RoleMgr
{
public:
    const HRole FindRole(uint roleSocketId)const;
    const HRole FindRole(const CGUID& roleId)const;
    const HRole FindRole(uint wsId, const string& roleName)const;

    void AddRole(const HRole&);
    void DelRole(const HRole&);

    void PrivateChat(const HRole& sender, const string& chatStr, const string& receiverName)const;
    void ScreenChat(const HRole& sender, const string& chatStr, const vector<CGUID>& receiverIds)const;
    void RegionChat(const HRole& sender, const string& chatStr)const;
    void WorldChat(const HRole& sender, const string& chatStr)const;
    void TeamChat(const HRole& sender, const string& chatStr)const;
    void FactionChat(const HRole& sender, const string& chatStr)const;

    void ChangeRegion(const CGUID& roleId, uint regionId, uint gsSocketId);
    void JoinTeam(const CGUID& roleId, const CGUID& teamId);
    void LeaveTeam(const CGUID& roleId);
    void JoinFaction(const CGUID& roleId, const CGUID& factionId);
    void LeaveFaction(const CGUID& roleId);
    void DisbandTeam(const CGUID& teamId);
    void DisbandFaction(const CGUID& factionId);

    void OnGsDisconnect(uint gsSocketId);



private:
    typedef set<HRole>                  RoleSet;        // roles sorted by id
    typedef set<HRole, SocketIdCmp>     SocketSet;      // roles sorted by socket id
    typedef set<HRole, RoleNameCmp>     NameSet;        // roles sorted by name
private:
    RoleSet                             m_Roles;        // all roles sorted by id
    SocketSet                           m_Sockets;      // all roles sorted by socket id
    map<uint, NameSet>                  m_Names;        // world server id -- roles
    map<CGUID, RoleSet>                 m_Teams;        // team id -- roles
    map<CGUID, RoleSet>                 m_Factions;     // faction id -- roles
    map<uint, map<uint, RoleSet> >      m_Regions;      // world server id -- region id -- roles
    map<uint, RoleSet>                  m_GameServers;  // game server socket -- roles



    // validates role procedure:
    // GS -> CS role informations and sign code, CS holds it;
    // GS -> GC role informations and sign code;
    // GC -> CS role id and sign code;
    // refer to WS if you want the details of "sign code".
public:
    void AddRole_Unchecked(const HRole&);
    const HRole PopRole_Unchecked(const CGUID& roleId);
private:
    RoleSet m_UncheckedRoles; // unchecked roles
};

#endif//#ifndef ROLEMGR_H