///
/// @file MonsterServerHandler.h
/// @author Kevin Lynx
/// @date 9.3.2008
/// @brief To handle the detail of the monster server.
///
#ifndef ___MONSTER_SERVER_HANDLER_H_
#define ___MONSTER_SERVER_HANDLER_H_

#include "MSConfig.h"
#include "../public/kl_singleton.h"

class CServer;
class CDataBlockAllocator;

///
/// MSHandler, is to handle the detail of the monster server, 
/// including host the server, process the messages etc.
///
class CMSHandler : public kl_common::singleton<CMSHandler>
{
public:
	/// ctor
	CMSHandler();

	/// dtor
	~CMSHandler();

	///
	/// Init the handler, including host the server.
	///
	/// @param ip local ip to be binded.
	/// @param port local port to be binded.
	///
	bool Init( const std::string &cfg_file, CDataBlockAllocator *allocator, long flag );

	///
	/// Release the handler, must be called manually.
	///
	void Release();

	///
	/// Process the monster server messages.
	///
	void ProcessMessage();

public:
	/// config reader.You can retrieve the reader directly.
	CMSConfig m_Config;
private:
	/// server
	CServer *m_pServer;
	/// flag to indicate itself.
	long m_lFlag;
};

#endif // ___MONSTER_SERVER_HANDLER_H_