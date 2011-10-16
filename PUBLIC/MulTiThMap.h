#pragma once
//提供一可以多线程操作的map
//提示:操作前执行Lock(),操作后执行UnLock();
//Author:Joe
//Create Time:2004.6.17
template<class TKey,class TData>
class MulTiThMap:
	public map<TKey,TData>
{
private:
	CRITICAL_SECTION	csMap;
public:
	MulTiThMap()	{InitializeCriticalSection(&csMap);}
	~MulTiThMap()	{DeleteCriticalSection(&csMap);	}
	void Lock()		{EnterCriticalSection(&csMap);}
	void UnLock()	{LeaveCriticalSection(&csMap);}

};