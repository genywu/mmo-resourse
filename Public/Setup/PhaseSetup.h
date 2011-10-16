/** 
 *  @file   PhaseSetup.h
 *  @author Expter
 *
 *  @brief  关于相位功能的服务器的配置文件管理
 *  @date   2010/03/18
 */

#pragma once

class PhaseSetup
{

public:
    /// 配置信息结构体属性
    struct  Config 
    {
        long  lAgonal;
        long  lHpNum;
        long  lDropGoodNum;
        long  lIsOpen;
    };

    typedef std::map<long,std::string>     PhaseTable;
    typedef PhaseTable::iterator           PIter;
    typedef std::map<long,PhaseTable>      PhaseHash;
    typedef PhaseHash::iterator            PHITER;
    typedef PhaseHash::const_iterator      PHCITER;
    typedef PhaseTable::const_iterator     PCIter;

public:

    /// 构造函数
    PhaseSetup(void);

    ~PhaseSetup(void);

    ///  配置释放
    void Release();

    ///  配置文件读取
    long Load();

    ///  解码配置数据
    void PhaseConfigDecode(DBReadSet& dbRead) ;

    ///  编码配置数据
    void PhaseConfigEnCode(DBWriteSet&  dbWrite);

    ///  功能开关
    int  PhaseEnable()  const  { return m_config.lIsOpen ; }

    ///  返回配置属性
    Config   GetConfig() const { return m_config;  }

    ///  获得当前应该触发脚本文件名
    const char*  GetScript(long Scriptid , long type = 0 )const ;

private:
    ///  基本统一配置信息
    Config              m_config;
    ///  脚本触发列表
    PhaseHash           m_PhaseTable;
};
