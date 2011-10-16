#pragma once


class CDisplayModel;
class GameSelectRay;

// 模型管理类
class GameBuildList
{
private:
	static GameBuildList* intance;
public:
	static void InitInstance();
	static GameBuildList* GetInstance();
	static void ReleaseInstance();
public:
	GameBuildList(void);
	virtual ~GameBuildList(void);

	// 物件结构
	class Build:public RCObject,public LoadState
	{
	public:
		struct tagLightSource
		{
			//光源属性                      
			DWORD		dwColor;
			float		fRadius;
			//动态属性
			BOOL		bDynamic;
			float		fMinIntensity;
			float		fMaxIntensity;
			DWORD		dwCycleTime;	

			float		fHeight;
		};

		struct tagSoundSource
		{
			//声源属性
			WORD  wID;	//文件索引
			float fHeight;
			float fRolloff;
			float fReferenceDistance;
			float fMaxGain;
			float fMinGain;  
		};

		struct tagBlockData
		{										
			BYTE byType;//TYPE NORMAL HIGH
		  	int iXMin;
			int iXMax;
			int iZMin;
			int iZMax;
		};

		struct tagModel
		{
			tagModel();
			~tagModel();
			BOOL  bAutoHide;
			BOOL  bLighted;
			DWORD  dwGlobalTimer;
			CDisplayModel *pGameModel;
			DWORD dwTrianglCount;
			BOOL bprojected;
			DWORD dwprojectedsize;
			
		};
	private:
		//属性
		WORD            m_wID;			// 建筑编号 0x0000    0 type   000 id
		char m_pszFilePath[MAX_PATH];

		tagModel	    *m_pModel;		
		tagLightSource  *m_pLightSource;
		tagSoundSource  *m_pSoundSource;	
		tagBlockData    *m_pBlockData;
	

		//阻挡站位数据（未计算）[按照一个立方格计算]
	
	public:
		inline WORD   GetID() const { return m_wID;}
		void   SetID(WORD wID) { m_wID = wID;}
		void SetFilePath(const char* pszPath)
		{strncpy(m_pszFilePath,pszPath,MAX_PATH);}

		tagLightSource * GetLightSource() const { return m_pLightSource;}
		tagSoundSource * GetSoundSource() const { return m_pSoundSource;}
		tagBlockData   * GetBlockData()   const   { return m_pBlockData;}
		tagModel	   * GetModel()		  const { return m_pModel;}

		BOOL Create(const char *pszFileName,DWORD dwTextureProcision=0);
		BOOL Create(CRFile* pFile,bool bAsyn=true);
		void Destroy();

		Build();
		~Build();
	};

protected:
	typedef map<WORD,Build*> mapBuild;
	typedef map<WORD,Build*>::iterator itBuild;
	mapBuild m_mapBuild;					// 建物模型列表
	
public:
	
	void  Initial();
	void  Release();
//资源读取、管理和释放接口
private:
	//同步创建默认的Model资源
	Build * LoadBuild(WORD wID,bool bAsyn=true);
public:
	//请求一个Build
	Build *AcquireBuild(WORD wID,bool bAsyn=true);
	//释放一个Build
	void ReleaseBuild(WORD wID,Build* pbuild);
	void OnReadBuildCompleted(CRFile* prfFile,void* pData);
	//异步读取的回调函数
	static void ReadBuilCallBack(CRFile* prfFile,void* pData);
};
