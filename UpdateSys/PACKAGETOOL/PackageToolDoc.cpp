// PackageToolDoc.cpp :  CPackageToolDoc 类的实现
//

#include "stdafx.h"
#include "PackageTool.h"
#include "MainFrm.h"
#include "PackageToolDoc.h"
#include ".\packagetooldoc.h"
#include "VersionDialog.h"
#include "makeautoupdate.h"
#include "MakePatch.h"
#include "../../PUBLIC/Crc32Static.h"
#include "../../PUBLIC/readwrite.h"
#include "../../public/MD5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//void PutDebugString(const char *msg)
//{
//	AfxMessageBox(msg);
//}
// CPackageToolDoc
IMPLEMENT_DYNCREATE(CPackageToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CPackageToolDoc, CDocument)
	ON_COMMAND(ID_32784, OnOpenLastProject)
	ON_COMMAND(ID_32785, OnSaveCurrProject)
	ON_COMMAND(ID_FILE_SAVERESINFOTOUPDATE, OnFileSaveresinfotoupdate)
//	ON_COMMAND(ID_MAKEAUTOUPDATEBYDEFAULT, OnMakeautoupdatebydefault)
//	ON_COMMAND(ID_MAKEAUTOUPDATEBYSELECT, OnMakeautoupdatebyselect)
	//ON_COMMAND(ID_FILE_OPENCLIENTINFO, OnFileOpenclientinfo)
	ON_COMMAND(ID_FILE_OPENORIGININFO, OnFileOpenorigininfo)
	ON_COMMAND(ID_MAKEDEFAULTPATCH, OnMakedefaultpatch)
	ON_COMMAND(ID_MAKESELECTPATCH, OnMakeselectpatch)
	ON_COMMAND(ID_MAKEAUTOUPDATEBYDEFAULTANDPATCH, OnMakeautoupdatebydefaultandpatch)
	ON_COMMAND(ID_MAKEAUTOUPDATEBYSELECTANDPATCH, OnMakeautoupdatebyselectandpatch)
	ON_COMMAND(ID_FILE_OPENRESINFOTABLE, OnFileOpenOtherFilesinfo)
	ON_COMMAND(ID_GENERATEMD5CODE, &CPackageToolDoc::OnGeneratemd5code)
	ON_COMMAND(ID_SETUPCLIENTMODE, &CPackageToolDoc::OnSetupclientmode)
	ON_COMMAND(ID_SETUPSERVERMODE, &CPackageToolDoc::OnSetupservermode)
	ON_COMMAND(ID_MAKESERVERPATCH, &CPackageToolDoc::OnMakeserverpatch)
	ON_COMMAND(ID_MAKESELECTSERVERPATCH, &CPackageToolDoc::OnMakeselectserverpatch)
END_MESSAGE_MAP()


// CPackageToolDoc 构造/析构

CPackageToolDoc::CPackageToolDoc()
:m_strCurFolder("")
,m_strExWorkPath("")
,m_pOriginResource(NULL)
,m_pClientResource(NULL)
,m_pUpdateInfo(NULL)
,m_pPatchPackage(NULL)
{
	// TODO: 在此添加一次性构造代码

	m_strPackageName = "Game.dp";
	m_strFileInfoName = "PackRes.cfg";
	m_strUpdateFileName = "LastUpdate.cfg";
	m_strDefaultProName = "Default.ptc";

	m_strGameVersion="00.00.0000";
	m_pUpdateInfo=NULL;

	m_nDisplayOriInfo=0;
	m_pOpenOtherFilesInfo=NULL;
	m_strOtherFileInfoName="";
	m_lCurPatchMode = 1;
}

CPackageToolDoc::~CPackageToolDoc()
{
	if(m_pOriginResource)		delete m_pOriginResource;
	if(m_pUpdateInfo)			delete m_pUpdateInfo;
	if(m_pClientResource)		delete m_pClientResource;
	if(m_pOpenOtherFilesInfo)	delete m_pOpenOtherFilesInfo;
}

BOOL CPackageToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	CHAR* strPath = new CHAR[MAX_PATH];
	if(::GetCurrentDirectory(MAX_PATH,strPath))
	{
		m_strExWorkPath=string(strPath);
	}

	m_strDefaultProName = m_strExWorkPath+"\\"+m_strDefaultProName;
	m_strUpdateFileName = m_strExWorkPath+"\\"+m_strUpdateFileName;

	FILE* pf = fopen(m_strDefaultProName.c_str(),"rb");
	if(pf)
	{
		fscanf(pf,"%s ",strPath);
		m_strPackageName=strPath;
		fscanf(pf,"%s ",strPath);
		m_strFileInfoName=strPath;
		fscanf(pf,"%s\n",strPath);
		m_strCurFolder=strPath;
		fclose(pf);
	}

	delete []strPath;
	//装载版本号
	LoadVersion();

	//装载包的配置信息
	LoadPackageInfos();

	//装载压缩信息
	LoadCompressInfos();
	//打开上次项目
	//OpenDefaultProject();
	return TRUE;
}


void	CPackageToolDoc::LoadVersion()
{
	//读出版本号
	string strTempt = m_strExWorkPath+"\\Game.vs";
	FILE* pf = fopen(strTempt.c_str(),"rb");
	if(pf)
	{
		fscanf(pf,"%s",m_strGameVersion.c_str());
		fclose(pf);
	}
}

//保存版本号
void	CPackageToolDoc::SaveVersion()
{

	//保存当前版本号
	string strTempt = m_strExWorkPath+"\\Game.vs";
	FILE* pf = fopen(strTempt.c_str(),"wb");
	if(pf)
	{
		fprintf(pf,"%s\r\n",m_strGameVersion.c_str());
		fclose(pf);
	}
}


string CPackageToolDoc::GetCurFolder()
{
	if(m_nDisplayOriInfo==1)
		return m_strCurFolder;
	else if(m_nDisplayOriInfo==2)
		return m_strExWorkPath+"\\"+"Client";
	else if(m_nDisplayOriInfo==3)
		return m_strOtherFileInfoName;
	else
		return "";
}


void	CPackageToolDoc::LoadCurFileInfoUpdateInf(string strUpdateFileName)
{
	if(strUpdateFileName=="")
		strUpdateFileName= m_strUpdateFileName;
	FILE* pf = fopen(strUpdateFileName.c_str(),"rb");
	if(pf==NULL)
		return;
	else
		 fclose(pf);
	if(m_pUpdateInfo==NULL)
	{
		m_pUpdateInfo=new CFilesInfo;
	}
	//装载文件信息
	m_pUpdateInfo->Clear();
	m_pUpdateInfo->Load(strUpdateFileName);
}

//装载包信息
void	CPackageToolDoc::LoadPackageInfos()
{
	string filename = m_strExWorkPath+"\\PackageConfig.ini";
	ifstream stream;
	stream.open(filename.c_str());
	if (!stream.is_open())
	{
		return;
	}

	set<DWORD>	CheckIDData;
	typedef set<DWORD>::iterator itData;
	for(int i=0;i< 32;i++)
	{
		DWORD dwNum = 1<<i;
		CheckIDData.insert(1<<i);
	}

	set<string> CheckNameData;CheckNameData.clear();
	typedef set<string>::iterator itCheckName;
	
	bool bFlag = true;
	m_PackageInfos.clear();
	m_PackageConfigInfos.clear();
	while (ReadTo(stream, "#"))
	{
		tagPackageInfo	PackConfInfo;
		tagPackFileInfo Info;
		stream >> PackConfInfo.dwID
			>> PackConfInfo.strFileName
			>> PackConfInfo.dwIndexNum
			>> PackConfInfo.dwEmptyIndexNum;

		itData it = CheckIDData.find(PackConfInfo.dwID);
		if(it == CheckIDData.end())
		{
			AfxMessageBox("配置的包ID不符合规则(ID=2^n,n<32)！");
			bFlag = false;
			break;
		}
		itPackage itPack = m_PackageConfigInfos.find(PackConfInfo.dwID);
		if(itPack != m_PackageConfigInfos.end())
		{
			AfxMessageBox("检查出重复的包ID，请检查配置文件(PackageConfig.ini)！");
			bFlag = false;
			break;
		}

		itCheckName itName = CheckNameData.find(PackConfInfo.strFileName);
		if(itName != CheckNameData.end())
		{
			AfxMessageBox("检查出重复的包名字，请检查配置文件(PackageConfig.ini)！");
			bFlag = false;
			break;
		}
		else
		{
			CheckNameData.insert(PackConfInfo.strFileName);
		}

		PackConfInfo.pPackage = NULL;
		Info.dwID= PackConfInfo.dwID;
		Info.strFileName = PackConfInfo.strFileName;
		Info.dwIndexNum = PackConfInfo.dwIndexNum;
		Info.dwEmptyIndexNum = PackConfInfo.dwEmptyIndexNum;
		m_PackageConfigInfos[PackConfInfo.dwID] = PackConfInfo;
		m_PackageInfos.push_back(Info);
	}

	if(bFlag == false)
	{
		m_PackageConfigInfos.clear();
		m_PackageInfos.clear();
	}
}

//装载压缩信息
void	CPackageToolDoc::LoadCompressInfos()
{
	string filename = m_strExWorkPath+"\\CompressConfig.ini";
	ifstream stream;
	stream.open(filename.c_str());
	if (!stream.is_open())
	{
		return;
	}
	m_CompressInfos.clear();
	while (ReadTo(stream, "#"))
	{
		tagPackFileInfo Info;
		stream >> Info.dwID
			>> Info.strFileName;
		m_CompressInfos.push_back(Info);
	}
}
//保存当前信息作为更新信息基础
void	CPackageToolDoc::SaveCurFileInfoUpdateInfo(CString& strFileName)	
{
	if(m_pOriginResource==NULL || strFileName =="")	return;

	string strTemptFile(strFileName.GetBuffer());	
	
	if(m_pOriginResource)
		m_pOriginResource->GetFilesInfo()->Save(strTemptFile);
	AfxMessageBox("存储完毕！");
}

// CPackageToolDoc 序列化

void CPackageToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CPackageToolDoc 诊断

#ifdef _DEBUG
void CPackageToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPackageToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPackageToolDoc 命令

//当打开一个文件时调用
void CPackageToolDoc::OnOpenfolder(CString strFolder)
{
	if(strFolder == "")	return;
	if(m_nDisplayOriInfo == 1)
	{
		::AfxMessageBox("已打开原始资源信息！");
		return;
	}
	m_nDisplayOriInfo=1;

	m_strCurFolder = strFolder;

	if(m_pOriginResource)
	{
		delete m_pOriginResource;
	}

	POSITION pos = GetFirstViewPosition();
	CView* pView = GetNextView(pos);
	if(pView)
	{
		Packages TemptPackages;
		m_pOriginResource = new CClientResource(ORIGIN_RES,m_strCurFolder,TemptPackages,m_strFileInfoName,
			pView->m_hWnd);
		m_pOriginResource->LoadClientResource();
	}
}

//从当前文件信息表中更新一个文件信息
VOID	CPackageToolDoc::UpdateFileInfoToOpFilZesInfo(string& strFileName,DWORD dwCrc32,DWORD dwSize,bool bFolder, DWORD dwPackageType,DWORD dwCompressType)
{
	if(m_pOriginResource)
		m_pOriginResource->GetFilesInfo()->UpdateFileInfo(strFileName,dwCrc32,dwSize,dwSize,dwSize,bFolder,dwPackageType,dwCompressType);
}

//更新文件信息，包括子文件
VOID	CPackageToolDoc::UpdateFileInfoToOpFilesWithChild(string& strFileName,DWORD dwPackageType,DWORD dwCompressType,
														  bool bForcePack,bool bForceCompress)
{
	if(m_pOriginResource)
		m_pOriginResource->GetFilesInfo()->UpdateFileInfoWithChild(strFileName,dwPackageType,dwCompressType,
																	bForcePack,bForceCompress);
}

//从当前文件信息表中得到一个文件的信息
BOOL	CPackageToolDoc::GetFileInfoFromOpFilesInfo(string& strFileName,DWORD& dwSize,DWORD& dwCrc32,DWORD& dwPackageType,DWORD& dwCompressType,bool& bFolder)
{
	if(m_pOriginResource)
	{
		DWORD dwOrginSize, dwValidSize;
		return m_pOriginResource->GetFilesInfo()->GetFileInfo(strFileName,dwSize,dwOrginSize,dwValidSize,dwCrc32,dwPackageType,dwCompressType,bFolder);
	}
	return false;
}

//根据当前操作的文件信息进行打包
void	CPackageToolDoc::MakePakcage()
{
	if(m_pOriginResource==NULL)	return;


	PackTypeInfos TemptPacksInfo;
	CVersionDialog VerDialog(TemptPacksInfo,false);
	VerDialog.m_strOldVersion=m_pOriginResource->GetFilesInfo()->GetVersion().c_str();
	VerDialog.m_strNewVersion=VerDialog.m_strOldVersion;
	//更新版本信息
	if(VerDialog.DoModal()==IDOK)
	{
		m_strGameVersion=VerDialog.m_strNewVersion;
		//保存变化的版本号
		SaveVersion();

		//设置信息的版本
		m_pOriginResource->GetFilesInfo()->SetVersion(m_strGameVersion);

		//创建一个客户端目录
		string strTempt = m_strExWorkPath+"\\"+"Client";
		::CreateDirectory(strTempt.c_str(),NULL);
		//创建放包的目录
		string strTemptPack = strTempt+"\\"+"Data";
		::CreateDirectory(strTemptPack.c_str(),NULL);
		if(m_pClientResource==NULL)
		{
			m_pClientResource = new CClientResource(GAME_RES,strTempt,m_PackageConfigInfos,
				m_strFileInfoName);
			//装载客户端资源文件
			m_pClientResource->Create();
		}

		m_pClientResource->GetFilesInfo()->SetVersion(m_strGameVersion);
		m_pClientResource->GetFilesInfo()->SetPackageTypeInfo(m_PackageInfos);
		m_pOriginResource->MakePakcage(m_pClientResource);
	}
}

void	CPackageToolDoc::MakePakcageEnd()
{
	m_pOriginResource->GetFilesInfo()->Save(m_strUpdateFileName);

	//保存自动跟新文件信息
	string strTempt = m_strExWorkPath+"\\"+"AutoUpdate-";
	strTempt += m_pOriginResource->GetVersion();

	//创建一个目录
	::CreateDirectory(strTempt.c_str(),NULL);

	string strFileInfo = strTempt +"\\PackRes.cfg";
	m_pOriginResource->GetFilesInfo()->Save(strFileInfo);
	DWORD dwCrc32;
	CCrc32Static::FileCrc32Filemap(strFileInfo.c_str(),dwCrc32);
	//保存版本号
	string strVersion = strTempt +"\\AutoUpdateGame.vs";
	FILE* pfAuto = fopen(strVersion.c_str(),"wb");
	if(pfAuto)
	{
		fprintf(pfAuto,"%s\n",m_strGameVersion.c_str());
		fprintf(pfAuto,"%u",dwCrc32);
		fclose(pfAuto);
	}
	//保存版本号文件到Patch文件夹
	string strAutoVersion = m_strExWorkPath +"\\AutoUpdateGame.vs";
	FILE* pf = fopen(strAutoVersion.c_str(),"wb");
	if(pf)
	{
		fprintf(pf,"%s\n",strVersion.c_str());
		fprintf(pf,"%u",dwCrc32);
		fclose(pf);
	}
}


//重新整理包 
void	CPackageToolDoc::ReMakePakcage()
{
	if(m_pClientResource)
	{
		m_pClientResource->ReMakePackage();
	}
}



void	CPackageToolDoc::OnAddFolder(string& strPos,string& strFolder)
{
	/*if(m_nDisplayOriInfo)
	{
		if(m_pOriginResource)
		{
			if(m_pOriginResource->OnAddFolder(strPos,strFolder))
			{
				POSITION pos = GetFirstViewPosition();
				while (pos != NULL)
				{
					CView* pView = GetNextView(pos);
					if(pView) pView->SendMessage(OPEN_FOLDER);
				}
			}
		}
	}
	else
	{
		AfxMessageBox("客户端资源禁止操作。");
	}*/
}
void	CPackageToolDoc::OnAddFile(string& strPos,string& strFileName)
{
	/*if(m_nDisplayOriInfo)
	{
		if(m_pOriginResource)
		{
			if(m_pOriginResource->OnAddFile(strPos,strFileName))
			{
				POSITION pos = GetFirstViewPosition();
				while (pos != NULL)
				{
					CView* pView = GetNextView(pos);
					if(pView) pView->SendMessage(OPEN_FOLDER);
				}
			}
		}
	}
	else
	{
		AfxMessageBox("客户端资源禁止操作。");
	}*/
}
void	CPackageToolDoc::OnDelFileFolder(string& strPos,string& strFolderFileName)
{
	/*if(m_nDisplayOriInfo)
	{
		if(m_pOriginResource)
		{
			if(m_pOriginResource->OnDelFileFolder(strPos,strFolderFileName))
			{
				POSITION pos = GetFirstViewPosition();
				while (pos != NULL)
				{
					CView* pView = GetNextView(pos);
					if(pView) pView->SendMessage(OPEN_FOLDER);
				}
			}
		}
	}
	else
	{
		AfxMessageBox("客户端资源禁止操作。");
	}*/
}
void	CPackageToolDoc::OnExtrFileFolder(string& strPos, string& strFolderFileName)
{
	if(m_nDisplayOriInfo == 1)
	{
		if(m_pOriginResource)
			m_pOriginResource->OnExtrFileFolder(strPos,strFolderFileName);
	}
	else if(m_nDisplayOriInfo == 2)
	{
		if(m_pClientResource)
			m_pClientResource->OnExtrFileFolder(strPos,strFolderFileName);
	}
	else if(m_nDisplayOriInfo == 4)
	{
		if(m_pClientResource)
			m_pClientResource->OnExtrFileFolderFromPatch(strPos,strFolderFileName, m_pPatchPackage);
	}
}
void CPackageToolDoc::OnOpenLastProject()
{
	// TODO: 在此添加命令处理程序代码
	OpenDefaultProject();
}

void CPackageToolDoc::OnSaveCurrProject()
{
	// TODO: 在此添加命令处理程序代码
	SaveProject(string(""));
}

void	CPackageToolDoc::OpenDefaultProject()
{

	if(m_nDisplayOriInfo == 1)
	{
		AfxMessageBox("已打开原始资源信息！");
		return;
	}
	m_nDisplayOriInfo=1;

	if(m_pOriginResource)
	{
		delete m_pOriginResource;
	}

	POSITION pos = GetFirstViewPosition();
	CView* pView = GetNextView(pos);
	if(pView)
	{
		Packages TemptPackages;
		m_pOriginResource = new CClientResource(ORIGIN_RES,m_strCurFolder,TemptPackages,m_strFileInfoName,pView->m_hWnd);
		CString strTempt2("");
		if(m_pOriginResource->Load()==false)
		{
			CString str;
			str.Format("没有信息文件,请重新打开该文件夹进行信息统计.");
			AfxMessageBox((LPCTSTR)str);
		}

	}
}

void	CPackageToolDoc::SaveProject(string& strName)
{
	string strTemptName = strName;
	if(strName=="")
	{
		strTemptName=m_strDefaultProName;
	}

	FILE* pf = fopen(strTemptName.c_str(),"wb");
	if(pf)
	{
		fprintf(pf,"%s ",m_strPackageName.c_str());
		fprintf(pf,"%s ",m_strFileInfoName.c_str());
		fprintf(pf,"%s\n",m_strCurFolder.c_str());

		fclose(pf);
	}
	else
	{
		::AfxMessageBox("保存包项目配置出错！");
		return;
	}

	//保存信息列表
	if(m_pOriginResource)
		m_pOriginResource->Save();
}

void CPackageToolDoc::OnCloseDocument()
{
	// TODO: 在此添加专用代码和/或调用基类

	//保存缺省项目
	SaveProject(string(""));

	CDocument::OnCloseDocument();
}

void	 CPackageToolDoc::ReLoadFolder(string& strFolder,bool bWithChild)
{
	if(m_pOriginResource)
		m_pOriginResource->ReLoadClientResourece(strFolder,bWithChild);

}

void	CPackageToolDoc::UpdateClientResourToView()
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		if(pView) pView->SendMessage(OPEN_FOLDER);
	}
}


const string&	CPackageToolDoc::GetPackFormatSring(DWORD dwPackID)
{
	static string strFormat = "";
	strFormat.clear();
	if( ( dwPackID & 0x1 ) == 1)
		strFormat += "无,";

	itTypeInfo it = m_PackageInfos.begin();
	for(;it != m_PackageInfos.end();it++)
	{
		if( ( dwPackID & (*it).dwID ) != 0)
		{
			strFormat += (*it).strFileName;
			strFormat += ",";
		}
	}
	strFormat = strFormat.substr(0,strFormat.length() -1);
	return strFormat;
}


const string&	CPackageToolDoc::GetCompFormatSring(DWORD dwCompID)
{
	static string strFormat = "";
	strFormat.clear();
	if( ( dwCompID & 0x1 ) == 1)
		strFormat += "无,";

	itTypeInfo it = m_CompressInfos.begin();
	for(;it != m_CompressInfos.end();it++)
	{
		if( ( dwCompID & (*it).dwID ) != 0)
		{
			strFormat += (*it).strFileName;
			strFormat += ",";
		}
	}
	strFormat = strFormat.substr(0,strFormat.length() -1);
	return strFormat;
}


void	CPackageToolDoc::SetStatStatuOp(string& strOp)
{
	m_StaDisplay.strOp= strOp;

	theApp.GetMainWnd()->SendMessage(STATUS_UPDATE,(WPARAM)&m_StaDisplay);
}

void CPackageToolDoc::SetStatStatuFileName(string& strFileName)
{
	m_StaDisplay.strFileName= strFileName;
	
	theApp.GetMainWnd()->SendMessage(STATUS_UPDATE,(WPARAM)&m_StaDisplay);
}

bool	CPackageToolDoc::SetGameVersion(string& strGameVersion)
{
	string strVersion(strGameVersion);

	strupr((char*)strVersion.c_str());
	bool bFlag = true;
	if(strVersion.length()==10)
	{
		for(int i=0;i < 10; i++)
		{
			if(i==2 || i == 5)
			{
				if(strVersion[i] != '.')
				{
					bFlag=false;
					break;
				}
			}
			else
			{
				if( !(strVersion[i] >= '0' && strVersion[i] <= '9' ) && 
					!(strVersion[i] >= 'A' && strVersion[i] <= 'Z' ) )
				{
					bFlag=false;
					break;
				}
			}
		}
	}
	else
	{
		bFlag=false;
	}

	if(bFlag==false)
	{
		string strTempt("此版本号错误:\n");
		strTempt += strVersion;
		::AfxMessageBox(strTempt.c_str());
		return false;
	}
	else
	{
		m_strGameVersion=strGameVersion;
		return true;
	}
	return false;
}

void CPackageToolDoc::OnFileSaveresinfotoupdate()
{
	// TODO: 在此添加命令处理程序代码
	//保存更新原始值

	CFileDialog	OpenDlg(FALSE,"cfg","*.cfg",OFN_EXTENSIONDIFFERENT,"cfg");
	if (OpenDlg.DoModal() == IDOK && OpenDlg.GetFileExt() == "cfg")
	{
		CString strFile = OpenDlg.GetPathName();
		SaveCurFileInfoUpdateInfo(strFile);
	}
	
}

//make autoupdate package
void CPackageToolDoc::OnMakeautoupdatebydefault()
{
	// TODO: 在此添加命令处理程序代码1
	if(m_pOriginResource==NULL)	return;

	LoadCurFileInfoUpdateInf();

	if(m_pUpdateInfo==NULL)
	{
		AfxMessageBox("No Original Update Info of Files!");
		return;
	}

	PackTypeInfos& TemptPacksInfo = GetPackageInfos();
	CVersionDialog VerDialog(TemptPacksInfo,true);
	VerDialog.m_strOldVersion=m_strGameVersion.c_str();
	VerDialog.m_strNewVersion=m_strGameVersion.c_str();
	//更新版本信息
	if(VerDialog.DoModal()==IDOK)
	{
		m_strGameVersion=VerDialog.m_strNewVersion;
		//保存变化的版本号
		SaveVersion();

		m_pOriginResource->GetFilesInfo()->SetPackageTypeInfo(GetPackageInfos());
		CMakeAutoUpdate MakeAutoUpdate(m_strGameVersion,m_strExWorkPath,VerDialog.m_nPackType,
										m_pUpdateInfo,m_pOriginResource);
		MakeAutoUpdate.MakeAutoUpdatePackage(GetCurPatchMode(), true, false);
	}
}

void CPackageToolDoc::OnMakeautoupdatebyselect()
{
	// TODO: 在此添加命令处理程序代码

	if(m_pOriginResource==NULL)	return;

	CFileDialog	OpenDlg(true,"cfg","*.cfg",OFN_EXTENSIONDIFFERENT,"cfg");
	if (OpenDlg.DoModal() == IDOK && OpenDlg.GetFileExt() == "cfg")
	{
		CString strFile = OpenDlg.GetPathName();

		LoadCurFileInfoUpdateInf(strFile.GetBuffer());

		if(m_pUpdateInfo==NULL)
		{
			AfxMessageBox("No Original Update Info of Files!");
			return;
		}

		PackTypeInfos& TemptPacksInfo = GetPackageInfos();
		CVersionDialog VerDialog(TemptPacksInfo,true);

		VerDialog.m_strOldVersion=m_strGameVersion.c_str();
		VerDialog.m_strNewVersion=m_strGameVersion.c_str();
		//更新版本信息
		if(VerDialog.DoModal()==IDOK)
		{
			m_strGameVersion=VerDialog.m_strNewVersion;
			//保存变化的版本号
			SaveVersion();

			
			m_pOriginResource->GetFilesInfo()->SetPackageTypeInfo(GetPackageInfos());
			CMakeAutoUpdate MakeAutoUpdate(m_strGameVersion,m_strExWorkPath,VerDialog.m_nPackType,
											m_pUpdateInfo,m_pOriginResource);

			MakeAutoUpdate.MakeAutoUpdatePackage(GetCurPatchMode(), false, true);
		}
	}
}

//void CPackageToolDoc::OnFileOpenclientinfo()
//{
//	// TODO: 在此添加命令处理程序代码
//	if(m_nDisplayOriInfo)
//	{
//		m_nDisplayOriInfo=false;
//		if(m_pOriginResource)
//		{
//			m_pOriginResource->GetFilesInfo()->SetDisplayWnd(NULL);
//			m_pOriginResource->Save();
//		}
//
//		if(m_pClientResource)
//		{
//			delete m_pClientResource;m_pClientResource=NULL;
//		}
//		
//		string strTempt = m_strExWorkPath+"\\"+"Client";
//
//		Packages TemptPackages;
//		m_pClientResource = new CClientResource(strTempt,TemptPackages,m_strFileInfoName);
//		//装载客户端资源文件
//		m_pClientResource->LoadEx();
//
//		POSITION pos = GetFirstViewPosition();
//		CView* pView = GetNextView(pos);
//		if(pView)
//		{
//			m_pClientResource->GetFilesInfo()->SetDisplayWnd(pView->m_hWnd);
//			m_pClientResource->GetFilesInfo()->UpdateAllInfoToView(NULL);
//		}
//	}
//}
void	 CPackageToolDoc::OnOpenPatchResouce(CString strFolder) // 打开patch资源文件
{
	m_nDisplayOriInfo = 4;
	if(m_pOriginResource)
	{
		m_pOriginResource->GetFilesInfo()->SetDisplayWnd(NULL);
		m_pOriginResource->Save();
	}

	if(m_pClientResource)
	{
		delete m_pClientResource;m_pClientResource=NULL;
	}

	string strTempt = strFolder;

	Packages TemptPackages;
	m_pClientResource = new CClientResource(GAME_RES,strTempt,TemptPackages,m_strFileInfoName);
	//装载客户端资源文件
	m_pClientResource->LoadEx();

	POSITION pos = GetFirstViewPosition();
	CView* pView = GetNextView(pos);
	if(pView)
	{
		m_pClientResource->GetFilesInfo()->UpdateAllInfoToViewEx(pView->m_hWnd);
	}

	m_pPatchPackage = new CPackage(NULL, strTempt);
	m_pPatchPackage->Open( "\\Patch", false);
}
void	 CPackageToolDoc::OnOpenGameResouce(CString strFolder)
{	
	m_nDisplayOriInfo = 2;
	if(m_pOriginResource)
	{
		m_pOriginResource->GetFilesInfo()->SetDisplayWnd(NULL);
		m_pOriginResource->Save();
	}

	if(m_pClientResource)
	{
		delete m_pClientResource;m_pClientResource=NULL;
	}
	
	string strTempt = strFolder;

	Packages TemptPackages;
	m_pClientResource = new CClientResource(GAME_RES,strTempt,TemptPackages,m_strFileInfoName);
	//装载客户端资源文件
	m_pClientResource->LoadEx();

	POSITION pos = GetFirstViewPosition();
	CView* pView = GetNextView(pos);
	if(pView)
	{
		//m_pClientResource->GetFilesInfo()->SetDisplayWnd(pView->m_hWnd);
		//m_pClientResource->GetFilesInfo()->UpdateAllInfoToView(NULL);
		m_pClientResource->GetFilesInfo()->UpdateAllInfoToViewEx(pView->m_hWnd);
	}
}

void CPackageToolDoc::OnFileOpenorigininfo()
{
	// TODO: 在此添加命令处理程序代码
	/*if(m_nDisplayOriInfo==false)
	{
		m_nDisplayOriInfo=true;
		if(m_pClientResource)
			m_pClientResource->GetFilesInfo()->SetDisplayWnd(NULL);

		if(m_pOriginResource==NULL)
		{
			OpenDefaultProject();
		}
		else
		{
			POSITION pos = GetFirstViewPosition();
			CView* pView = GetNextView(pos);
			if(pView)
			{
				m_pOriginResource->GetFilesInfo()->SetDisplayWnd(pView->m_hWnd);
				m_pOriginResource->GetFilesInfo()->UpdateAllInfoToView(NULL);
			}
		}
	}*/
}

void CPackageToolDoc::OnMakedefaultpatch()
{
	// TODO: 在此添加命令处理程序代码
	// TODO: 在此添加命令处理程序代码1
	if(m_pOriginResource==NULL)	return;

	LoadCurFileInfoUpdateInf();

	if(m_pUpdateInfo==NULL)
	{
		AfxMessageBox("No Original Update Info of Files!");
		return;
	}

	PackTypeInfos& TemptPacksInfo = GetPackageInfos();
	CVersionDialog VerDialog(TemptPacksInfo,true);
	VerDialog.m_strOldVersion=m_strGameVersion.c_str();
	VerDialog.m_strNewVersion=m_strGameVersion.c_str();
	//更新版本信息
	if(VerDialog.DoModal()==IDOK)
	{
		m_strGameVersion=VerDialog.m_strNewVersion;
		//保存变化的版本号
		SaveVersion();

		m_pOriginResource->GetFilesInfo()->SetPackageTypeInfo(GetPackageInfos());
		CMakePatch PatchUpdate(m_strGameVersion,m_strExWorkPath,VerDialog.m_nPackType,
			m_pUpdateInfo,m_pOriginResource);
		PatchUpdate.MakePatch(false);
	}
}

void CPackageToolDoc::OnMakeselectpatch()
{
	// TODO: 在此添加命令处理程序代码
	if(m_pOriginResource==NULL)	return;

	CFileDialog	OpenDlg(true,"cfg","*.cfg",OFN_EXTENSIONDIFFERENT,"cfg");
	if (OpenDlg.DoModal() == IDOK && OpenDlg.GetFileExt() == "cfg")
	{
		CString strFile = OpenDlg.GetPathName();

		LoadCurFileInfoUpdateInf(strFile.GetBuffer());

		if(m_pUpdateInfo==NULL)
		{
			AfxMessageBox("No Original Update Info of Files!");
			return;
		}

		PackTypeInfos& TemptPacksInfo = GetPackageInfos();
		CVersionDialog VerDialog(TemptPacksInfo,true);
		VerDialog.m_strOldVersion=m_strGameVersion.c_str();
		VerDialog.m_strNewVersion=m_strGameVersion.c_str();
		//更新版本信息
		if(VerDialog.DoModal()==IDOK)
		{
			m_strGameVersion=VerDialog.m_strNewVersion;
			//保存变化的版本号
			SaveVersion();

			m_pOriginResource->GetFilesInfo()->SetPackageTypeInfo(GetPackageInfos());
			CMakePatch PatchUpdate(m_strGameVersion,m_strExWorkPath,VerDialog.m_nPackType,
				m_pUpdateInfo,m_pOriginResource);
			PatchUpdate.MakePatch(true);
		}
	}
}

void CPackageToolDoc::OnMakeautoupdatebydefaultandpatch()
{
	// TODO: 在此添加命令处理程序代码
	m_lCurPatchMode = 1;
	if(m_pOriginResource==NULL)	return;

	LoadCurFileInfoUpdateInf();

	if(m_pUpdateInfo==NULL)
	{
		AfxMessageBox("No Original Update Info of Files!");
		return;
	}

	PackTypeInfos& TemptPacksInfo = GetPackageInfos();
	CVersionDialog VerDialog(TemptPacksInfo,true);
	VerDialog.m_strOldVersion=m_strGameVersion.c_str();
	VerDialog.m_strNewVersion=m_strGameVersion.c_str();
	//更新版本信息
	if(VerDialog.DoModal()==IDOK)
	{
		m_strGameVersion=VerDialog.m_strNewVersion;
		//保存变化的版本号
		SaveVersion();

		m_pOriginResource->GetFilesInfo()->SetPackageTypeInfo(GetPackageInfos());
		CMakeAutoUpdate MakeAutoUpdate(m_strGameVersion,m_strExWorkPath,VerDialog.m_nPackType,
										m_pUpdateInfo,m_pOriginResource);
		MakeAutoUpdate.MakeAutoUpdatePackage(GetCurPatchMode(), false, true);
	}
}

void CPackageToolDoc::OnMakeautoupdatebyselectandpatch()
{
	// TODO: 在此添加命令处理程序代码
	m_lCurPatchMode = 1;
	if(m_pOriginResource==NULL)	return;

	CFileDialog	OpenDlg(true,"cfg","*.cfg",OFN_EXTENSIONDIFFERENT,"cfg");
	if (OpenDlg.DoModal() == IDOK && OpenDlg.GetFileExt() == "cfg")
	{
		CString strFile = OpenDlg.GetPathName();

		LoadCurFileInfoUpdateInf(strFile.GetBuffer());

		if(m_pUpdateInfo==NULL)
		{
			AfxMessageBox("No Original Update Info of Files!");
			return;
		}

		PackTypeInfos& TemptPacksInfo = GetPackageInfos();
		CVersionDialog VerDialog(TemptPacksInfo,true);
		VerDialog.m_strOldVersion=m_pUpdateInfo->GetVersion().c_str();
		VerDialog.m_strNewVersion=m_strGameVersion.c_str();
		//更新版本信息
		if(VerDialog.DoModal()==IDOK)
		{
			m_strGameVersion=VerDialog.m_strNewVersion;
			//保存变化的版本号
			SaveVersion();

			m_pOriginResource->GetFilesInfo()->SetPackageTypeInfo(GetPackageInfos());
			CMakeAutoUpdate MakeAutoUpdate(m_strGameVersion,m_strExWorkPath,VerDialog.m_nPackType,
											m_pUpdateInfo,m_pOriginResource);
			MakeAutoUpdate.MakeAutoUpdatePackage(GetCurPatchMode(), true, true);
		}
	}
}

void CPackageToolDoc::OnFileOpenOtherFilesinfo()
{
	// TODO: 在此添加命令处理程序代码

	CFileDialog	OpenDlg(true,"cfg","*.cfg",OFN_EXTENSIONDIFFERENT,"cfg");
	if (OpenDlg.DoModal() == IDOK && OpenDlg.GetFileExt() == "cfg")
	{
		CString strFile = OpenDlg.GetPathName();

		if(m_nDisplayOriInfo == 1)
		{
			m_pOriginResource->GetFilesInfo()->SetDisplayWnd(NULL);
			m_pOriginResource->Save();
		}
		else if(m_nDisplayOriInfo == 2)
		{
			m_pClientResource->GetFilesInfo()->SetDisplayWnd(NULL);
		}
		else if(m_nDisplayOriInfo == 3)
		{
			m_pOpenOtherFilesInfo->SetDisplayWnd(NULL);
		}
		if(m_pOpenOtherFilesInfo )
		{
			delete m_pOpenOtherFilesInfo;
			m_pOpenOtherFilesInfo = NULL;
		}
		m_nDisplayOriInfo = 3;

		POSITION pos = GetFirstViewPosition();
		CView* pView = GetNextView(pos);
		if(pView)
		{
			m_pOpenOtherFilesInfo=new CFilesInfo;
			m_strOtherFileInfoName = string(strFile.GetBuffer());
			m_pOpenOtherFilesInfo->Load(m_strOtherFileInfoName);
			m_pOpenOtherFilesInfo->UpdateAllInfoToViewEx(pView->m_hWnd);
		}
	}
}

void CPackageToolDoc::OnGeneratemd5code()
{
	if(!m_pOriginResource)
	{
		MessageBox(NULL, "请先打开一个资源项目！", "错误", MB_OK);
		return;
	}

	// TODO: 在此添加命令处理程序代码
	m_strCurFolder;
	m_strFileInfoName;

	// 计算打包文件的crc32值
	string exeName = m_strCurFolder + "\\setup\\serverlist.ini";
	DWORD dwCrc32 = 0;
	CCrc32Static::FileCrc32Filemap(exeName.c_str(), dwCrc32);

	if(dwCrc32 == 0)
	{
		MessageBox(NULL, "未找到该项目setup目录下的ServerList.ini文件！", "错误", MB_OK);
		return;
	}

	//保存自动跟新文件信息
	string strTempt = m_strExWorkPath+"\\"+"AutoUpdate-";
	strTempt += m_pOriginResource->GetVersion();
	string vsName = strTempt + "\\AutoUpdateGame.vs";
	
	string strCrc32;
	string strVer;

	ifstream myfile;
	myfile.open(vsName.c_str());
	if (!myfile.is_open())
	{
		char szOutSStr[1024];
		sprintf(szOutSStr, "未找到该补丁包%s目录下的AutoUpdateGame.vs文件！版本号%s。",
			strTempt.c_str(),m_pOriginResource->GetVersion().c_str());
		MessageBox(NULL, szOutSStr, "错误", MB_OK);
		return;
	}
	myfile >> strVer;
	myfile >> strCrc32;
	myfile.close();

	ofstream myfile1;
	myfile1.open(vsName.c_str());
	if (!myfile1.is_open())
	{
		char szOutSStr[1024];
		sprintf(szOutSStr, "未找到该补丁包%s目录下的AutoUpdateGame.vs文件！版本号%s。",
			strTempt.c_str(),m_pOriginResource->GetVersion().c_str());
		MessageBox(NULL, szOutSStr, "错误", MB_OK);
		return;
	}
	myfile1 << strVer << "\r\n";
	myfile1 << dwCrc32 << "\r\n";
	myfile1.close();

	char szOutSStr[1024];
	sprintf(szOutSStr, "成功修改该补丁包%s目录下的AutoUpdateGame.vs文件！版本号%s。",
		strTempt.c_str(),m_pOriginResource->GetVersion().c_str());
	MessageBox(NULL, szOutSStr, "提示", MB_OK);
}


void CPackageToolDoc::OnSetupclientmode()
{
	// TODO: 在此添加命令处理程序代码
	m_lCurPatchMode = 1;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if(pMainFrame)
	{
		CMenu* pMenu = pMainFrame->GetMenu();
		if(pMenu)
		{
			pMenu->CheckMenuItem(ID_SETUPSERVERMODE, MF_UNCHECKED);
			pMenu->CheckMenuItem(ID_SETUPCLIENTMODE, MF_CHECKED);
		}
	}
}

void CPackageToolDoc::OnSetupservermode()
{
	// TODO: 在此添加命令处理程序代码
	m_lCurPatchMode = 0;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if(pMainFrame)
	{
		CMenu* pMenu = pMainFrame->GetMenu();
		if(pMenu)
		{
			pMenu->CheckMenuItem(ID_SETUPSERVERMODE, MF_CHECKED);
			pMenu->CheckMenuItem(ID_SETUPCLIENTMODE, MF_UNCHECKED);
		}
	}
}

void CPackageToolDoc::OnMakeserverpatch()
{
	// TODO: 在此添加命令处理程序代码
	m_lCurPatchMode = 0;

	if(m_pOriginResource==NULL)	return;

	LoadCurFileInfoUpdateInf();

	if(m_pUpdateInfo==NULL)
	{
		AfxMessageBox("No Original Update Info of Files!");
		return;
	}

	PackTypeInfos& TemptPacksInfo = GetPackageInfos();
	CVersionDialog VerDialog(TemptPacksInfo,true);
	VerDialog.m_strOldVersion=m_strGameVersion.c_str();
	VerDialog.m_strNewVersion=m_strGameVersion.c_str();
	//更新版本信息
	if(VerDialog.DoModal()==IDOK)
	{
		m_strGameVersion=VerDialog.m_strNewVersion;
		//保存变化的版本号
		SaveVersion();

		m_pOriginResource->GetFilesInfo()->SetPackageTypeInfo(GetPackageInfos());
		CMakeAutoUpdate MakeAutoUpdate(m_strGameVersion,m_strExWorkPath,VerDialog.m_nPackType,
			m_pUpdateInfo,m_pOriginResource);
		MakeAutoUpdate.MakeAutoUpdatePackage(GetCurPatchMode(), false, true);
	}
}

void CPackageToolDoc::OnMakeselectserverpatch()
{
	// TODO: 在此添加命令处理程序代码
	m_lCurPatchMode = 0;

	CFileDialog	OpenDlg(true,"cfg","*.cfg",OFN_EXTENSIONDIFFERENT,"cfg");
	if (OpenDlg.DoModal() == IDOK && OpenDlg.GetFileExt() == "cfg")
	{
		CString strFile = OpenDlg.GetPathName();

		LoadCurFileInfoUpdateInf(strFile.GetBuffer());

		if(m_pUpdateInfo==NULL)
		{
			AfxMessageBox("No Original Update Info of Files!");
			return;
		}

		PackTypeInfos& TemptPacksInfo = GetPackageInfos();
		CVersionDialog VerDialog(TemptPacksInfo,true);
		VerDialog.m_strOldVersion=m_pUpdateInfo->GetVersion().c_str();
		VerDialog.m_strNewVersion=m_strGameVersion.c_str();
		//更新版本信息
		if(VerDialog.DoModal()==IDOK)
		{
			m_strGameVersion=VerDialog.m_strNewVersion;
			//保存变化的版本号
			SaveVersion();

			m_pOriginResource->GetFilesInfo()->SetPackageTypeInfo(GetPackageInfos());
			CMakeAutoUpdate MakeAutoUpdate(m_strGameVersion,m_strExWorkPath,VerDialog.m_nPackType,
				m_pUpdateInfo,m_pOriginResource);
			MakeAutoUpdate.MakeAutoUpdatePackage(GetCurPatchMode(), true, true);
		}
	}
}
