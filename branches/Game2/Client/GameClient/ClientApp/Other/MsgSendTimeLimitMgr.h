/**
* @filename:MsgSendTimeLimitMgr.h
* @date: 2010/4/1
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 客户端消息发送时间限制管理类,参照Chat.h实现
*/
enum eMsgSendType{
	MST_TYPE_CHAT_SCRIPT,//聊天里发送脚本
	MST_TYPE_RANK_TIME,//排行榜查询
	MSG_TYPE_SKILL_SET_HOTKEY,//mainbar上设置技能热键
	MSG_TYPE_QUEST,    //任务消息
	MST_NUM,			//查询类型数目
};
class MsgSendTimeLimitMgr
{
public:
	MsgSendTimeLimitMgr();
	~MsgSendTimeLimitMgr();

	/// @brief
	/// 判断是否超出时间间隔了 
	/// @param e
	/// 消息发送类型
	/// @return 
	/// 如果 超出就返回true，没有，就弹出提示框，并返回false
	static bool IsTimeOut(eMsgSendType e);

	/// @brief
	/// 更新上次发送时间
	/// @param e
	/// 消息发送类型
	/// @return
	/// 
	static void UpdateLastSendTime(eMsgSendType e);
private:
	static ulong m_Interleave[MST_NUM]; //发送间隔
	static ulong m_LastSendTime[MST_NUM];//上次发送时间
};

