#include "stdafx.h"
#include "Msg.h"
#include "../App/RoleMgr.h"//for RoleMgr::DelRole

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void OnChatMessage(Msg*); // 响应聊天消息

// 发送消息到指定客户端
long Msg::SendToClient(long lSocketID)
{
    assert(s_NetServer4Client);
    *((long*)Base()) = GetSize()+HEAD_SIZE;
    //分配一个消息
    CBaseMessage *pMsg = CBaseMessage::AllocMsg();
    vector<CDataBlock*>& MsgData = GetMsgData();
    pMsg->SetMsgData(MsgData);
    MsgData.clear();
    s_NetServer4Client->ASend(lSocketID,pMsg);
    return true;
}

string GetIpStr(DWORD dwIP)
{
    char strIP[256];
    sprintf(strIP, "%d.%d.%d.%d", LOBYTE(LOWORD(dwIP)), HIBYTE(LOWORD(dwIP)), LOBYTE(HIWORD(dwIP)), HIBYTE(HIWORD(dwIP)));
    return strIP;
}

bool Msg::IsValidGsIp()
{
    const set<string>& ips = GetInst(Game).GetSetup()->validGsIp;
    if (ips.find(GetIpStr(GetIP())) == ips.end())
        return false;
    return true;
}

long Msg::DefaultRun()
{
    switch (GetType())
    {
    case BASEMSG_Socket_Accept:
        {
            if (GetNetFlag() & NF_Server_GameServer)
            {
                //is game server valid?
                if (!IsValidGsIp())
                {
                    s_NetServer4GS->DisConn(GetSocketID());
                    break;
                }

                char str[256];
                sprintf(str, "gs: %s[%d]", GetIpStr(GetIP()).c_str(), GetSocketID());
                AppFrame::AddToList("clients", str);
                Log4c::Trace(ROOT_MODULE,"Connects game server: %s", str);
            }
            else if (GetNetFlag() & NF_Server_GameClient)
            {
                char str[256];
                sprintf(str, "gc: %s[%d]", GetIpStr(GetIP()).c_str(), GetSocketID());
                AppFrame::AddToList("clients", str);
                Log4c::Trace(ROOT_MODULE,"Connects game client: %s", str);
            }
        }
        break;
    case BASEMSG_Socket_Close:
        {
            if (GetNetFlag() & NF_Server_GameServer)
            {
                if (!IsValidGsIp())
                    break;

                char str[256];
                sprintf(str, "gs: %s[%d]", GetIpStr(GetIP()).c_str(), GetSocketID());
                AppFrame::DelFromList("clients", str);
               Log4c::Warn(ROOT_MODULE,"Disconnects game server: %s", str);

                //从玩家管理中删除该GS的玩家并断开玩家连接
                GetInst(RoleMgr).OnGsDisconnect(GetSocketID());
            }
            else if (GetNetFlag() & NF_Server_GameClient)
            {
                char str[256];
                sprintf(str, "gc: %s[%d]", GetIpStr(GetIP()).c_str(), GetSocketID());
                AppFrame::DelFromList("clients", str);
                Log4c::Trace(ROOT_MODULE,"Disconnects game client: %s", str);

                //从玩家管理中删除该玩家
                const HRole role = GetInst(RoleMgr).FindRole(GetSocketID());
                if (role.Bound())
                    GetInst(RoleMgr).DelRole(role);
            }
        }
        break;
    }
    return true;
}
// 消息处理
long Msg::Run()
{
    switch (GetType() & 0xffffff00)
    {
    case 0:
        DefaultRun();
        break;

    default:
        OnChatMessage(this);
        break;
    }
    return 0;
}

bool Msg::Initial(CDataBlockAllocator* pDBAllocator, long nMaxFreeMsgNum)
{
    CBaseMessage::NewMsg = NewMessage;
    return CBaseMessage::Initial(pDBAllocator,nMaxFreeMsgNum);
}

bool Msg::Release()
{
    CBaseMessage::NewMsg = NULL;
    return CBaseMessage::Release();
}

CBaseMessage* Msg::NewMessage()
{
    return new Msg();
}

CServer* Msg::s_NetServer4GS = NULL;
CServer* Msg::s_NetServer4Client = NULL;

void Msg::SetNetServer4GS(CServer* p)
{
    s_NetServer4GS = p;
}

void Msg::SetNetServer4Client(CServer* p)
{
    s_NetServer4Client = p;
}