

#ifndef _OBJ_INFO_MANAGE_CPP_
#define _OBJ_INFO_MANAGE_CPP_



#include "../include/ObjInfoManage.h"

namespace XM_Tools
{
	//////////////////////////////////////////////////////////////////////////////////////////
	//! 对象信息管理
	//////////////////////////////////////////////////////////////////////////////////////////



	CObjInfoManage::CObjInfoManage(void)
		:m_uRTIDSeed(0)
		,m_OutInfoFun(NULL)
		,m_pOutFile(NULL)
	{
	}

	CObjInfoManage::~CObjInfoManage(void)
	{
#ifdef LOG_ALLOC_FREE_INFO
		if(NULL == m_OutInfoFun || NULL == m_pOutFile)
			return;

		std::string strTempInfo("---------------------------对象池析构信息---------------------------\r\n");
		if(0 != m_mapObjInfo.size())
		{
			for (std::map<unsigned long, tagObjRunInfo>::iterator ite = m_mapObjInfo.begin(); m_mapObjInfo.end() != ite; ++ite)
			{
				if (0 != ite->second.uInstNum)
				{
					char szObjInfo[512] = {0};
					sprintf(szObjInfo, "对象【%s】还有【%u】个没有释放！！！\r\n", ite->second.szName, ite->second.uInstNum);
					strTempInfo += szObjInfo;
				}
			}
		}

		strTempInfo += "-------------------------------------------------------------------\r\n";
		m_OutInfoFun(m_pOutFile, strTempInfo.c_str());
#endif
	}

	//! 设置信息输出函数和输出路径
	void CObjInfoManage::SetOutInfoSetup(OutInfoFun pOutInfoFun, const char *pFileName)
	{
		m_OutInfoFun = pOutInfoFun;
		m_pOutFile = pFileName;
	}


	//! 运行时对象信息输出
	void CObjInfoManage::OutRunTimeObjInfo(void)
	{
#ifdef LOG_ALLOC_FREE_INFO
		if(NULL == m_OutInfoFun || NULL == m_pOutFile)
			return;

		std::string strTempInfo("---------------------------对象池运行时信息---------------------------\r\n");
		if(0 != m_mapObjInfo.size())
		{
			for (std::map<unsigned long, tagObjRunInfo>::iterator ite = m_mapObjInfo.begin(); m_mapObjInfo.end() != ite; ++ite)
			{
				if (0 < ite->second.uInstNum)
				{
					char szObjInfo[512] = {0};
					sprintf(szObjInfo, "对象【%s】有【%u】个。\r\n", ite->second.szName, ite->second.uInstNum);
					strTempInfo += szObjInfo;
				}
			}
		}

		strTempInfo += "-------------------------------------------------------------------\r\n";
		m_OutInfoFun(m_pOutFile, strTempInfo.c_str());
#endif
	}

	//! 得到一个新的运行时ID
	unsigned long CObjInfoManage::GetNewRTID(void)
	{
		return ++m_uRTIDSeed;
	}

#ifdef LOG_ALLOC_FREE_INFO
	//! 注册一个对象信息
	bool CObjInfoManage::RegisterObj(std::string &strRegName, unsigned long uRTID)
	{
		std::map<std::string, unsigned long>::iterator ite = m_mapNameToID.find(strRegName);
		if (m_mapNameToID.end() == ite)
		{
			m_mapNameToID.insert(std::make_pair(strRegName, uRTID));

			MM_ASSERY(m_mapObjInfo.end() == m_mapObjInfo.find(uRTID));
			tagObjRunInfo ObjRunInfo;
			ObjRunInfo.uInstNum = 0;
			ObjRunInfo.uRTID	= uRTID;
			strcpy_s(ObjRunInfo.szName, CLASS_NAME_SIZE, strRegName.c_str());
			m_mapObjInfo.insert(std::make_pair(uRTID, ObjRunInfo));

			return true;
		}
		return false;
	}

	
#else
	//! 注册一个对象信息
	bool CObjInfoManage::RegisterObj(unsigned long uRTID)
	{
		if(m_mapObjInfo.end() != m_mapObjInfo.find(uRTID))
		{
			tagObjRunInfo ObjRunInfo;
			ObjRunInfo.uInstNum = 0;
			ObjRunInfo.uRTID	= uRTID;
			m_mapObjInfo.insert(std::make_pair(uRTID, ObjRunInfo));

			return true;
		}
		return false;
	}
#endif

	//! 添加了一个对象
	bool CObjInfoManage::AddObj(unsigned long uRTID)
	{
		std::map<unsigned long, tagObjRunInfo>::iterator ite = m_mapObjInfo.find(uRTID);
		if(m_mapObjInfo.end() != ite)
		{
			ite->second.uInstNum += 1;
			return true;
		}
		return false;
	}

	//! 删除了一个对象
	bool CObjInfoManage::DelObj(unsigned long uRTID)
	{
		std::map<unsigned long, tagObjRunInfo>::iterator ite = m_mapObjInfo.find(uRTID);
		if(m_mapObjInfo.end() != ite)
		{
			if(0 < ite->second.uInstNum)
			{
				ite->second.uInstNum -= 1;
				return true;
			}
		}
		return false;
	}

	CObjInfoManage& GetObjInfoManage(void)
	{
		static CObjInfoManage c_ObjInfoManage;
		return c_ObjInfoManage;
	}
};

#endif //! _OBJ_INFO_MANAGE_CPP_