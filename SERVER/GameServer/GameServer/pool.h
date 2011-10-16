

#include <list>
#include <vector>
#include <set>
#include <map>

using namespace std;

//!默认的一块内存的对象数量
static	long	g_ObjCount = 100;
//!默认的初始内存块数量
static	long	g_blockCount = 1;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ObjectPool
{
public:
	ObjectPool(void);

	~ObjectPool(void);

public:
	//!			设置某种尺寸对象的分配策略
	bool		SetPolicy(long size, long objCount, long blockCount);

	//!			获得一块空间
	void*		AllotObj(long size);

	//!			归还一块空间
	bool		FreeObj(void* p, long size = 0);

	//!			释放所有对象空间，若还有对象未通过FreeObj接口归还，则抛出异常
	void		Release(void);

public:
	//!			占用的内存空间大小(byte)
	long		GetBufSize(void);

	//!			正在使用的空间大小(byte)
	long		GetUesedSize(void);

	//!			未使用的空间大小(byte)
	long		GetFreeSize(void);

private:

	class Blocks
	{
	public:
		Blocks(long objSize, long blockObjNum = g_ObjCount, long g_blockCount = 1);

		~Blocks(void);
		

	public:
		void*				alloc_obj(void);

		bool				free_obj(void *p);

		bool				free_buf(void);

	public:
		long				get_buf_size(void);

		long				get_uesed_size(void);

		long				get_free_size(void);

	private:
		set<void*>			m_useSet;
		list<void*>			m_unUseList;
		vector<char*>		m_bufs;
		long				m_objSize;
		long				m_num;
	};

	//!内存块列表<尺寸, 块对象>
	map<long, Blocks*>				m_SizeTable;

};