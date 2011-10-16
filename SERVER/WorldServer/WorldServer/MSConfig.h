///
/// @file MSConfig.h
/// @author Kevin Lynx
/// @brief To read the config file for the monster server.
///
#ifndef ___MS_CONFIG_H_
#define ___MS_CONFIG_H_

#include <map>
#include <string>

class TiXmlElement ;

///
/// CMSConfig is used to parse the monster server config.
///
class CMSConfig
{
public:
  /// Address info.
  struct tagAddr
  {
    std::string m_strIP;
    unsigned short m_nPort;

    tagAddr( const std::string &ip = "", unsigned short port = 0 ) : m_strIP( ip ), m_nPort( port )
    {
    }

    bool operator== ( const tagAddr &other ) const
    {
      return m_strIP == other.m_strIP && m_nPort == other.m_nPort;
    }
  };

  struct tagServiceParam
  {
    tagAddr     m_Addr;
    long        m_nMaxFreeSockOperNum;
    long        m_nMaxFreeIOOperNum;
    long        m_lIOOperDataBufNum;
    bool        m_bCheck;
    long        m_lForbidTime;
    long        m_lMaxMsgLen;
    long        m_lMaxConNums;
    long        m_lTotalClientsNum;
    long        m_lPendingWrBufNum;
    long        m_lPendingRdBufNum;
    long        m_lMaxSendSizePerSecond;
    long        m_lMaxRecvSizePerSecond;
    long        m_lMaxBlockSendMsgNum;
    long        m_lConPendingWrBufNum;
    long        m_lConPendingRdBufNum;
    long        m_lConMaxSendSizePerSecond;
    long        m_lConMaxRecvSizePerSecond;
    long        m_lConMaxBlockSendMsgNum;


    tagServiceParam ()
      : m_Addr ()
      ,m_nMaxFreeSockOperNum (100)
      , m_nMaxFreeIOOperNum (100)
      , m_lIOOperDataBufNum (2)
      , m_bCheck (0)
      , m_lForbidTime (1000)
      , m_lMaxMsgLen (102400)
      , m_lMaxConNums (1000)
      , m_lTotalClientsNum (10)
      , m_lPendingWrBufNum (4096)
      , m_lPendingRdBufNum (2048)
      , m_lMaxSendSizePerSecond (8192)
      , m_lMaxRecvSizePerSecond (4096)
      , m_lMaxBlockSendMsgNum (1024)
      , m_lConPendingWrBufNum (10240)
      , m_lConPendingRdBufNum (10240)
      , m_lConMaxSendSizePerSecond (81920)
      , m_lConMaxRecvSizePerSecond (81920)
      , m_lConMaxBlockSendMsgNum (1024)
    {
    }

  };


  /// monster server list.[id-addr]
  typedef std::map<int, tagAddr> MSListType;

  /// region info
  struct tagRgn
  {
    long m_lValue;
    int m_iMSID;

    tagRgn( long value = 0, int id = 0 ) : m_lValue( value ), m_iMSID( id )
    {
    }
  };
  /// region list.[id-rgn]
  typedef std::map<int, tagRgn> RGNListType;

public:
  /// ctor
  CMSConfig() 
  {
  }
  /// dtor
  ~CMSConfig()
  {
  }

  ///
  /// Load the config file
  ///
  bool Load( const std::string &file );

  ///
  /// Get local address, the local address means the listen address.
  ///
  const tagServiceParam &GetLocalAddr() const
  {
    return m_ServiceParam;
  }

  ///
  /// Get MS addr by its id, if find failed, return a default tagAddr.
  ///
  tagAddr GetMSAddr( int id ) const;

  ///
  /// Get MS id by its addr.
  ///
  int GetMSID( const tagAddr &addr ) const;

  ///
  /// Get region info by its id.(region id), if find failed, return a default tagRgn.
  ///
  tagRgn GetRGNInfo( int id ) const ;

private:
  /// For internal use, load the local address.
  bool LoadLocal( TiXmlElement *pElem );
  /// For internal use, load the monster server list.
  bool LoadMSList( TiXmlElement *pElem );
  /// For internal use, load the region list.
  bool LoadRNGList( TiXmlElement *pElem );
private:
  /// local address
  tagServiceParam m_ServiceParam;
  /// monster server list.
  MSListType m_MSList;
  /// region list.
  RGNListType m_RGNList;
};

#endif // ___MS_CONFIG_H_