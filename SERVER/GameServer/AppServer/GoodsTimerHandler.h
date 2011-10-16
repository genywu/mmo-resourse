///
/// @file GoodsTimerHandler.h
/// @author Kevin Lynx
/// @brief Implement a simple timer wrapper for player's goods.
///
#ifndef ___GOODS_TIMER_HANDLER_H_
#define ___GOODS_TIMER_HANDLER_H_

#include <map>
#include <list>

class CPlayer;
class GoodsTimerHandler
{
public:
	struct TimerKey
	{
		long lTriger;

		TimerKey( long triger ) : 
			lTriger( triger )
		{
		}

		bool operator< ( const TimerKey &other ) const
		{
			return lTriger < other.lTriger;
		}
	};
private:
	typedef std::list<CGUID> GoodsListT;

	struct TimerKeyHandle
	{
		TimerKey Key;
		long lTimerID;
		TimerKeyHandle( const TimerKey &key, long id = 0 ) :
			Key( key ), lTimerID( id )
		{
		}
	};

	struct TimerKeyHandleLess
	{
		bool operator() ( const TimerKeyHandle &left, const TimerKeyHandle &right ) const
		{
			return left.Key < right.Key;
		}
	};
	typedef std::map<TimerKeyHandle, GoodsListT*, TimerKeyHandleLess> GoodsTimerTableT;

	typedef void (CPlayer::*HandleFuncT)( const CGUID &goodsID );
public:
	GoodsTimerHandler()
	{
		SetUnique( true );
	}

	~GoodsTimerHandler()
	{
	}

	/// Register a new timer.
	/// This funtion use 'lTrigerTime' as the handle to a list of goods. If the timer already exists,
	/// only add the new goodsID.
	long Register( CPlayer *pPlayer, const CGUID &goodsID, long lTrigerTime, long lInterval, void *arg );

	/// UnRegister a goods timer.
	void UnRegister( CPlayer *pPlayer, const CGUID &goodsID );

	/// UnRegister all the timers.
	void UnRegisterAll( CPlayer *pPlayer );

	/// Handle the time out event.
	void OnTimeOut( CPlayer *pPlayer, const TimerKey &key,HandleFuncT callback );

	/// Make sure one goods only has one timer.
	void SetUnique( bool bUnique )
	{
		m_bUnique = bUnique;
	}

	bool IsUnique() const
	{
		return m_bUnique;
	}
private:
	GoodsTimerTableT m_GoodsTimers;
	bool m_bUnique;
};

#endif
