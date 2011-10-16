#ifndef _DUPLIREGIONSSETUP_H_
#define _DUPLIREGIONSSETUP_H_


class CDupliRegionSetup
{
	struct tagDupliRegion
	{
		long	lRegionID;		//原始ID
		long	lDupRegionID;	//副本的地图ID
	};
public:
	CDupliRegionSetup(void);
	~CDupliRegionSetup(void);

	bool Load();
	bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码

	bool IsDupliRegion(long lRegionID,long lDupliRegionID);

	//WorldServer使用,从原始场景和副本场景中随机得到一场景
	long GetRandomRegion(long lRegionID);
private:

	typedef list<tagDupliRegion>::iterator itDR;
	list<tagDupliRegion>	m_DupliRegions;
};

#endif
