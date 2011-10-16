#ifndef LINKMANMGR_H
#define LINKMANMGR_H

#include "Role.h"

// LinkmanMgr is short for "linkman manager".
// using LinkmanMgr class as a singleton, e.g. GetInst(LinkmanMgr).
class LinkmanMgr
{

private:
    typedef set<HRole>                      LinkmanSetOnline;   // linkman set
    typedef map<string, LinkmanSetOnline>   GroupMapOnline;     // group name - linkman set
    typedef map<HRole, GroupMapOnline>      LinkmanMapOnline;   // a role - group name - linkman set

    typedef set<string>                     LinkmanSetOffline;  // linkman set
    typedef map<string, LinkmanSetOffline>  GroupMapOffline;    // group name - linkman set
    typedef map<HRole, GroupMapOffline>     LinkmanMapOffline;  // a role - group name - linkman set
private:
    // "MeLink" means others in my linkman list
    // "LinkMe" means me in others' linkman list
    LinkmanMapOnline    m_MeLinkOnline;
    LinkmanMapOffline   m_MeLinkOffline;
    
    LinkmanMapOnline    m_LinkMeOnline;
    LinkmanMapOffline   m_LinkMeOffline;
};

#endif//#ifndef LINKMANMGR_H