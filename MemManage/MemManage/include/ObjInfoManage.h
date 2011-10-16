





/*

*/


#include "mmdef.h"

#ifndef _OBJ_INFO_MANAGE_H_
#define _OBJ_INFO_MANAGE_H_



namespace XM_Tools
{
	//////////////////////////////////////////////////////////////////////////////////////////
	//! 对象信息管理
	//! 这是一个同步类，在异步接口中应用需要加锁
	//////////////////////////////////////////////////////////////////////////////////////////
	class CObjInfoManage
	{
	public:
		CObjInfoManage(void);
		~CObjInfoManage(void);

	public:
		//!										得到一个新的运行时ID
		unsigned long							GetNewRTID(void);

		//!										注册一个对象信息
		bool									RegisterObj(const char *pRegName, unsigned long uRTID);	

		//!										设置信息输出函数和输出路径
		void									SetOutInfoSetup(OutInfoFun pOutInfoFun, const char *pFileName);
		//!										运行时对象信息输出
		void									OutRunTimeObjInfo(void);

	public:
		//!										添加了一个对象
		bool									AddObj(unsigned long uRTID);
		//!										删除了一个对象
		bool									DelObj(unsigned long uRTID);
									

	private:

		//! 对象运行时信息
		struct tagObjRunInfo
		{
			//!				运行时ID
			unsigned long	uRTID;
			//!				实例数量
			unsigned long	uInstNum;
			//!				最大示例数量
			unsigned long	uMaxInstNum;
			//!				对象名
			char			szName[CLASS_NAME_SIZE];

			tagObjRunInfo(void){memset(this, 0, sizeof(tagObjRunInfo));}
		};

	private:
		//!										对象运行时ID种子
		unsigned long							m_uRTIDSeed;
		//!
		stdext::hash_map<unsigned long, tagObjRunInfo>	m_mapObjInfo;
		//!										输出函数
		OutInfoFun								m_OutInfoFun;
		//!										输出文件
		const char								*m_pOutFile;
	};


	//////////////////////////////////////////////////////////////////////////////////////////
	CObjInfoManage& GetObjInfoManage(void);
};
#endif //! _OBJ_INFO_MANAGE_H_