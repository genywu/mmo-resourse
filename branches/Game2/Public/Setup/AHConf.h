///
/// @file AHConf.h
/// @brief Config reader for AH module
///
#ifndef ___AH_CONF_H_
#define ___AH_CONF_H_

class TiXmlElement;

namespace AH
{
    struct Conf
    {
        char goodsOrig[64];
        int timeOut;
        int checkInter;
        int saveInter;
        int goodsCnt;
        float chargePer;
        int orderCnt;
        int agentMax;
    };
    
    class ConfReader
    {
    public:
        ConfReader();

        void Reset();

        bool Load();

        void Encode( DBWriteSet &db );

        void Decode( DBReadSet &db );
    private:
        void LoadGlobal( TiXmlElement *elem );
    public:
        Conf m_Cfg;
    };
}

#endif
