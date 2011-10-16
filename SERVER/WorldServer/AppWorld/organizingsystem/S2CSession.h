/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 会话记录模板
//! SessionInfo：一个可以进行（=、==）操作的类型
template<class SessionInfo>
class S2CSession
{
public:
	//! lMaxHoldTime：会话的保持时间（单位：秒）
	//! bAllowOverlay：创建新会话时，是否允许覆盖内容相同的会话（覆盖的同时会重新计时）
	S2CSession(LONG lMaxHoldTime, BOOL bAllowOverlay = TRUE)
		:
	m_lMaxHoldTime(lMaxHoldTime), 
		m_bAllowOverlay(bAllowOverlay)
	{}

	~S2CSession(void){}

public:
	//! 创建一个新会话
	//! 如果不想使用构造时定义的有效保持时间，
	//!		则将参数lExceptiveHoldTime的值设置为这个会话需要保持的时间,
	//!		这个设置值只对当前创建的会话有效，不会影响之前和之后的操作。
	BOOL CreateSession(const SessionInfo &Info, LONG lExceptiveHoldTime = 0)
	{
		//! 清理超时会话
		LONG now_time = time(NULL);
		ClearOuttime(now_time);

		//! 接着进行查找
		list<SessionNode>::iterator ite = m_listSession.begin();
		for (; ite != m_listSession.end(); ++ite)
		{
			if(ite->Info == Info)
			{
				if (m_bAllowOverlay)
				{
					m_listSession.erase(ite);
					break;
				}
				else
				{
					return FALSE;
				}
			}
		}

		SessionNode NewNode;
		NewNode.Info		= Info;
		if(0 != lExceptiveHoldTime)
			NewNode.lEndTime	= now_time + lExceptiveHoldTime;
		else
			NewNode.lEndTime	= now_time + m_lMaxHoldTime;

		//! 如果是例外形式，则为了保证队列按时间早晚顺序不变，需要找出一个合适的位置插入
		if (0 != lExceptiveHoldTime)
		{
			list<SessionNode>::iterator ite = m_listSession.begin();
			for (; ite != m_listSession.end(); ++ite)
			{
				if (ite->lEndTime > NewNode.lEndTime)
				{
					if (m_listSession.begin() == ite)
						m_listSession.push_front(NewNode);
					else
						m_listSession.insert(--ite, NewNode);

					return TRUE;
					//! 如果没有找到这个位置，说明可以直接放到队列末尾
				}
			}
		}

		//! 否则直接放在队列末尾
		m_listSession.push_back(NewNode);
		return TRUE;

	}
	//! 验证一个会话
	//! bRemove：验证完后是否删除会话
	BOOL TestSession(const SessionInfo &Info, BOOL bRemove = TRUE)
	{
		//! 清理超时会话
		LONG now_time = time(NULL);
		ClearOuttime(now_time);

		list<SessionNode>::iterator ite = m_listSession.begin();
		for (; ite != m_listSession.end(); ++ite)
		{
			if(ite->Info == Info)
			{
				if (bRemove)
					m_listSession.erase(ite);
				return TRUE;
			}
		}
		return FALSE;
	}

	LONG Size(VOID)
	{
		return m_listSession.size();
	}

	//! 将保存的有效会话作为传入的函数参数，并调用
	//！[第一个函数指针参数] 调用的第一个参数从保存的session中取出
	//! [第二个参数pParam] 用作 [第一个函数指针参数] 调用的第2个自定义参数
	void RunFunction_ForEach(void (*pFun)(SessionInfo&, void*), void* pParam)
	{
		ClearOuttime(time(NULL));

		list<SessionNode>::iterator ite = m_listSession.begin();
		for (; ite != m_listSession.end(); ++ite)
		{
			(*pFun)(ite->Info, pParam);
		}
	}

private:
	void ClearOuttime(LONG now_time)
	{
		list<SessionNode>::iterator ite = m_listSession.begin();
		for (; ite != m_listSession.end(); ++ite)
		{
			if(ite->lEndTime < now_time)
			{
				m_listSession.erase(m_listSession.begin(), ++ite);
				return;
			}
		}
	}


private:
	//! 会话节点
	struct  SessionNode
	{
		LONG		lEndTime;
		SessionInfo	Info;
	};

	//!					保存的会话
	list<SessionNode>	m_listSession;
	//!					会话最长时间（秒）
	LONG				m_lMaxHoldTime;
	//!					创建的会话已存在时，是否允许覆盖（覆盖时重新计时）
	BOOL				m_bAllowOverlay;
};
