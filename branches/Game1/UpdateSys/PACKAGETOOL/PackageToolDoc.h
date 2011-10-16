// PackageToolDoc.h :  CPackageToolDoc 类的接口
//

#include "../../PUBLIC/FilesInfo.h"
#include "../../PUBLIC/Package.h"
#include "../../PUBLIC/ClientResource.h"
#pragma once

struct tagStaDisplay
{
	string strOp;
	string strFileName;
	tagStaDisplay()
	:strOp("")
	,strFileName("")
	{
	}
};

#define OPEN_FOLDER WM_USER+1					//打开文件夹时调用
#define FILE_RERESH WM_USER+2					//文件属性刷新了
#define INSERT_ITEM WM_USER+3					//插入文件项


#define STATUS_UPDATE WM_USER+4					//更新状态栏显示

class CPackageToolDoc : public CDocument
{
protected: // 仅从序列化创建
	CPackageToolDoc();
	DECLARE_DYNCREATE(CPackageToolDoc)

private:
	string		m_strCurFolder;						//当前操作资源文件夹
	string		m_strPackageName;					//包名
	string		m_strFileInfoName;					//客户端资源信息名
	string		m_strDefaultProName;				//缺省项目名称
	

	Packages		m_PackageConfigInfos;		//包配置信息
	PackTypeInfos	m_PackageInfos;					//配置的包信息
	CompTypeInfos	m_CompressInfos;				//配置的压缩信息

	string		m_strGameVersion;					//游戏当前版本号

	string		m_strUpdateFileName;				//更新信息文件名
	CFilesInfo*	m_pUpdateInfo;						//最后一次操作的资源信息文件
	
	CClientResource*	m_pOriginResource;			//客户端原始资源
	CClientResource*	m_pClientResource;			//打包后的客户端资源目录

	string m_strExWorkPath;							//当前执行文件的工作路径

	tagStaDisplay	m_StaDisplay;
	int				m_nDisplayOriInfo;				//0:空 1:原始资源信息 2:游戏资源信息 3:其他文件信息
	CFilesInfo		*m_pOpenOtherFilesInfo;			//打开的其他文件信息
	string			m_strOtherFileInfoName;

	long			m_lCurPatchMode;				// 0:服务器模式 1;客户端模式(默认)
	CPackage*		m_pPatchPackage;
	// 属性
public:
	long			GetCurPatchMode(void) { return m_lCurPatchMode; }
	void			SetCurPatchMode(long lMode) { m_lCurPatchMode = lMode; }
	Packages&		GetPackConfigInfos()			{return m_PackageConfigInfos;}

	CPackage*		GetPatchPackage(void) { return m_pPatchPackage; }
	PackTypeInfos&	GetPackageInfos()				{return m_PackageInfos;}
	CompTypeInfos&	GetCompressInfos()				{return m_CompressInfos;}

	const string&	GetPackFormatSring(DWORD dwPackID);
	const string&	GetCompFormatSring(DWORD dwCompID);

	int	GetDisplayOriInfo()				{return m_nDisplayOriInfo;}

	void	SetStatStatuOp(string& strOp);
	void	SetStatStatuFileName(string& strFileName);

	bool	SetGameVersion(string& strGameVersion);

	void	LoadVersion();
	void	SaveVersion();																//保存版本号
	void	LoadCurFileInfoUpdateInf(string strUpdateFileNam="");
	void	LoadPackageInfos();															//装载包信息
	void	LoadCompressInfos();														//装载压缩信息
	void	SaveCurFileInfoUpdateInfo(CString& strFileName);							//保存当前信息作为更新信息基础

// 操作
public:
	string GetCurFolder();

	CClientResource* GetClientResource()	{return m_pOriginResource;}


	VOID	UpdateFileInfoToOpFilZesInfo(string& strFileName,DWORD dwCrc32,DWORD dwSize,bool bFolder,
							DWORD dwPackageType=0,DWORD dwCompressType =0);				//更新一个文件信息
	BOOL	GetFileInfoFromOpFilesInfo(string& strFileName,DWORD& dwSize,DWORD& dwCrc32,
						DWORD& dwPackageType,DWORD& dwCompressType,bool& bFolder);		//得到一个文件的信息

	//当打包发生变化时候，更新文件信息
	VOID	UpdateFileInfoToOpFilesWithChild(string& strFileName,DWORD dwPackageType,DWORD dwCompressType,
											bool bForcePack = false,bool bForceCompress = false);

	void	 OnOpenfolder(CString strFolder);
	void	 ReLoadFolder(string& strFolder,bool bWithChild = true);
	void	 OnOpenGameResouce(CString strFolder);
	void	 OnOpenPatchResouce(CString strFolder); // 打开patch资源文件

	void	MakePakcage();																//根据当前操作的文件信息进行打包
	void	ReMakePakcage();															//重新整理包 
	void	MakePakcageEnd();

	void	OnAddFolder(string& strPos,string& strFolder);
	void	OnAddFile(string& strPos,string& strFileName);
	void	OnDelFileFolder(string& strPos,string& strFolderFileName);
	void	OnExtrFileFolder(string& strPos,string& strFolderFileName);

	void	OpenDefaultProject();
	void	SaveProject(string& strName);
	void	UpdateClientResourToView();
// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CPackageToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOpenLastProject();
	afx_msg void OnSaveCurrProject();
	virtual void OnCloseDocument();
	afx_msg void OnFileSaveresinfotoupdate();
	afx_msg void OnMakeautoupdatebydefault();
	afx_msg void OnMakeautoupdatebyselect();
	//afx_msg void OnFileOpenclientinfo();
	afx_msg void OnFileOpenorigininfo();
	afx_msg void OnMakedefaultpatch();
	afx_msg void OnMakeselectpatch();
	afx_msg void OnMakeautoupdatebydefaultandpatch();
	afx_msg void OnMakeautoupdatebyselectandpatch();
	afx_msg void OnFileOpenOtherFilesinfo();
	afx_msg void OnGeneratemd5code();
	afx_msg void OnSetupclientmode();
	afx_msg void OnSetupservermode();
	afx_msg void OnMakeserverpatch();
	afx_msg void OnMakeselectserverpatch();
};

//void PutDebugString(const char *msg);
