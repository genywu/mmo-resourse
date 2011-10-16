/**
* @filename:SelectLSMgr.h
* @date: 2010/6/4
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 选服的数据结构定义
*/
struct tagLS{
	std::string ip;
	ushort port;
	std::string serverName;
	std::string strArea;
	long lHip;
	int iAreaId;
	int igroupId;
	int iServerState;
	int iServerXK;
	tagLS()
	{
		ip = "";
		port = 0;
		serverName = "";
		strArea = "";
		lHip = 0;
		iAreaId = 0;
		igroupId = 0;
		iServerState = 0;
		iServerXK = 0;
	};
};
typedef std::vector<tagLS>LSVECTOR;

//采用单件模式
class LSHelp
{
public:
	LSHelp();
	~LSHelp();
	void ReadLSInfo();
	tagLS* GetLS(size_t index){
		if(index >= m_vecLS.size())
			return NULL;
		else
			return &m_vecLS[index];
	}
	size_t GetLSSize() const { return m_vecLS.size(); }
	tagLS* GetSelLS() const { return m_selLS ;}
	void SetSelLS(tagLS* selLS){ m_selLS = selLS ;}
private:
	LSVECTOR	m_vecLS;
	tagLS*		m_selLS;
};