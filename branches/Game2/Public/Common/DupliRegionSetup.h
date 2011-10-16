#ifndef _DUPLIREGIONSSETUP_H_
#define _DUPLIREGIONSSETUP_H_


class CDupliRegionSetup
{
	struct tagDupliRegion
	{
		long	lRegionID;		//原始ID
		long	lDupRegionID;	//副本的地图ID
	}; 
    
    typedef list<tagDupliRegion>::iterator itDR;

public:

	CDupliRegionSetup(void);
	~CDupliRegionSetup(void);

	bool Load();
    
    /// 添加到CByteArray
	bool AddToByteArray(vector<BYTE>* pByteArray);		
    
    /// 解码
	bool DecordFromByteArray(BYTE* pByte, long& pos);	

	bool IsDupliRegion(long lRegionID,long lDupliRegionID);

	//WorldServer使用,从原始场景和副本场景中随机得到一场景
	long GetRandomRegion(long lRegionID);

private:

    list<tagDupliRegion>	m_DupliRegions;
};

#endif
