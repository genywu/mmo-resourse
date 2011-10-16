#include "StdAfx.h"
#include "Patch.h"

CPatch::CPatch(void)
:m_pPatchFilesInfo(NULL)
, m_pPatchPackage(NULL)
, m_pClientResource(NULL)
, m_strWorkPath("")
, m_strExePath("")
{
	
}

CPatch::~CPatch(void)
{
}

BOOL CPatch::Init(const char* filename)
{
	// 查找客户端路径,注册表
	//注册表信息
	if (filename == NULL)
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP70"),filename);
		return FALSE;
	}

	string strTemp(filename);
	
	m_strExePath = m_strWorkPath = strTemp;

	

	string strGameExeName = m_strWorkPath + "\\PackRes.cfg";

	// 查找GameClient.exe
	WIN32_FIND_DATA stFindData;
	HANDLE hFind = FindFirstFile(strGameExeName.c_str(), &stFindData);
	if (hFind==INVALID_HANDLE_VALUE)
	{
		CHAR* strPath = new CHAR[MAX_PATH];
		::GetSystemDirectory(strPath,MAX_PATH);
		string strTemptr(strPath);
		SAFE_DELETE_ARRAY(strPath);

		string strTemptFoler;strTemptFoler.clear();
		strTemptFoler.insert(0,strTemptr,0,2);

		////#ifdef	_DEBUG
		CHAR* strWorkPath = new CHAR[MAX_PATH];
		if(::GetCurrentDirectory(MAX_PATH,strWorkPath))
		{
			m_strExePath = string(strWorkPath);
			m_strWorkPath = m_strExePath;
		}
		strGameExeName = m_strWorkPath + "\\PackRes.cfg";
		SAFE_DELETE_ARRAY(strWorkPath);

		HANDLE hFind = FindFirstFile(strGameExeName.c_str(), &stFindData);
		if (hFind==INVALID_HANDLE_VALUE)
		{
			LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP71"),strGameExeName);
			return false;
		}
	}

	//string tmpUpdatePath = m_strWorkPath;

	//::CreateDirectory(tmpUpdatePath.c_str(), NULL);

	m_strFilesInfoName="PackRes.cfg";
	m_strTemptFilesInfoName=m_strExePath+"\\Patch.cfg";	//临时包
	m_strTemptPackageName=m_strExePath+"\\Patch.dp";//临时文件姓名
	if(m_pClientResource)
	{
		delete m_pClientResource;
		m_pClientResource=NULL;
	}

	m_pClientResource = new CClientResource(GAME_RES,m_strWorkPath,m_strFilesInfoName);
	return TRUE;
}

string CPatch::Update(const char* filename,const char* patchname)
{
	m_strPatchName = patchname;
	string str="";
	if(!Init(filename))
		return str;
	if(!GetPatchData())
		return str;
	if(!LoadClientResource(this))
		return str;

	//得到新版本号,检查是否更新成功
	
	string tmpFolder = m_strWorkPath + "\\PackRes.cfg";
	FILE* file = fopen(tmpFolder.c_str(), "rb");
	char strVersion[16] = {0};
	if(file)
	{
		fread(strVersion, 10, 1, file);
		fclose(file);
	}
	
	if (strcmp(strVersion,m_listStrUpdatedVersion.c_str()) == 0)
	{
		return m_listStrUpdatedVersion;
	}
	return str;
}
bool CPatch::Release()
{
	m_pClientResource->Release();
	//CClientResource::ReleaseInstance();
	m_pPatchPackage->Release();
	return true;
}


BOOL CPatch::GetPatchData()
{
	string tmpZipFileNmae = m_strWorkPath +"\\updatefiles\\" + m_strPatchName/*"00.00.0002.zip"*/;
	DWORD dwCalCrc32 = 0;
	//CRC校验
	HANDLE hFile = CreateFile(tmpZipFileNmae.c_str(),
		GENERIC_READ,0,(LPSECURITY_ATTRIBUTES) NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,(HANDLE) NULL);

	if (hFile == INVALID_HANDLE_VALUE) { 
		return FALSE; 
	}
	DWORD dwSize = ::GetFileSize(hFile,NULL);
	BYTE* pBuf = new BYTE[dwSize];
	DWORD dwReadSize;
	BOOL result = ReadFile(hFile,pBuf,dwSize,&dwReadSize,NULL);


	if( !result )
	{
		delete []pBuf;
		return FALSE;
	}
	CloseHandle(hFile);
	if (result == FALSE) 
	{ 
		delete []pBuf;
		return FALSE;
	} 
	if(dwSize != dwReadSize)
	{ 
		delete []pBuf;
		return FALSE;
	} 
	CCrc32Static::DataCrc32(pBuf, dwSize, dwCalCrc32);
	delete []pBuf;

	if (dwCalCrc32 != m_dwCrc32)
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP72"));
		GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP72"));
		return FALSE;
	}
	//解压缩数据
	unzFile dataZipFile = unzOpen(tmpZipFileNmae.c_str());
	if(dataZipFile)
	{
		unz_file_info zipFileInfo;
		if( UNZ_OK == unzLocateFile(dataZipFile, "Patch.cfg", 1) )
		{
			unzGetCurrentFileInfo(dataZipFile, &zipFileInfo, NULL, 0,
				NULL, 0, NULL, 0);

			char* buf = new char[zipFileInfo.uncompressed_size];
			unzOpenCurrentFile(dataZipFile);
			unzReadCurrentFile(dataZipFile, buf, zipFileInfo.uncompressed_size);
			string tmpUpdatePath = m_strWorkPath + "\\updatefiles\\Patch.cfg";
			FILE* file = fopen(tmpUpdatePath.c_str(), "wb");
			if(file)
			{
				fwrite(buf, zipFileInfo.uncompressed_size, 1, file);
				fclose(file);
			}
			unzCloseCurrentFile(dataZipFile);
			SAFE_DELETE_ARRAY(buf);
		}

		// 找到Patch.pak
		if( UNZ_OK == unzLocateFile(dataZipFile, "Patch.dp", 1) )
		{
			unzGetCurrentFileInfo(dataZipFile, &zipFileInfo, NULL, 0,
				NULL, 0, NULL, 0);

			char* buf = new char[zipFileInfo.uncompressed_size];
			unzOpenCurrentFile(dataZipFile);
			unzReadCurrentFile(dataZipFile, buf, zipFileInfo.uncompressed_size);
			string tmpUpdatePath = m_strWorkPath + "\\updatefiles\\Patch.dp";
			FILE* file = fopen(tmpUpdatePath.c_str(), "wb");
			if(file)
			{
				fwrite(buf, zipFileInfo.uncompressed_size, 1, file);
				fclose(file);
			}
			unzCloseCurrentFile(dataZipFile);
			SAFE_DELETE_ARRAY(buf);
		}

		if(!m_pPatchFilesInfo)
			m_pPatchFilesInfo = new CFilesInfo;
		m_pPatchFilesInfo->Clear();
		string tmpPatchRilPath = m_strWorkPath + "\\updatefiles\\Patch.cfg";
		m_pPatchFilesInfo->Load(tmpPatchRilPath);
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP73"),m_strPatchName.c_str());

		if(!m_pPatchPackage)
			m_pPatchPackage = new CPackage(NULL,m_strExePath);
		m_pPatchPackage->Open("\\updatefiles\\Patch");
	}
	else
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP74"),tmpZipFileNmae);
	}

	unzClose(dataZipFile);

	//得到新版本号
	string tmpFolder = m_strWorkPath + "\\updatefiles\\Patch.cfg";
	FILE* file = fopen(tmpFolder.c_str(), "rb");
	char strVersion[16] = {0};
	if(file)
	{
		fread(strVersion, 10, 1, file);
		m_listStrUpdatedVersion = strVersion;
		fclose(file);
	}
	return TRUE;
}


BOOL CPatch::CheckGameRunFolder()
{
	string strFilesInfoName = m_strWorkPath+"\\updatefiles\\PackRes.cfg";
	WIN32_FIND_DATA fdt;

	HANDLE hFind = FindFirstFile(strFilesInfoName.c_str(), &fdt);
	if( hFind == INVALID_HANDLE_VALUE )
		return FALSE;
	FindClose(hFind);

	/*strFilesInfoName = m_strWorkPath+"\\Game.dp";
	hFind = FindFirstFile(strFilesInfoName.c_str(), &fdt);
	if( hFind == INVALID_HANDLE_VALUE )
	return FALSE;
	FindClose(hFind);*/

	return TRUE;
}


//装载客户端资源信息
BOOL CPatch::LoadClientResource(CPatch* pPatch)
{

	CClientResource* pClientResour = pPatch->m_pClientResource;

	int nMax = 1;
	int nPos = 0;

	if(pClientResour->LoadEx( 
		pPatch->m_pPatchFilesInfo->GetPackageTypeInfo() )==false)
	{

		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP75"));
		return FALSE;
	}
//	char strTitle[200];
	string tmpFolder = pPatch->m_strExePath + "\\PackRes.cfg";
	FILE* file = fopen(tmpFolder.c_str(), "rb");
	char strVersion[16] = {0};
	if(file)
	{
		fread(strVersion, 10, 1, file);
		fclose(file);
	}

	/*sprintf(strTitle,"升级游戏从版本%s 到版本%s。 ", strVersion,
		pPatch->m_pPatchFilesInfo->GetVersion().c_str());

	LogTools::OutLogAndTxt(strTitle);*/
	string strTemptOld;		
	string strTemptNew;
	strTemptOld.insert(0,pClientResour->GetVersion(),0,5);
	strTemptNew.insert(0,strVersion,0,5);

#ifndef	_DEBUG
	if(strTemptOld != strTemptNew)
	{
		//被升级的游戏版本不符
	
		PutDebugString("被升级的游戏版本不符。");
		return FALSE;
	}
#endif

	//得到不同部分
	CFilesInfo DirfFilesInfo;
	pPatch->m_pPatchFilesInfo->GetDifferenceByCrc32( *(pClientResour->GetFilesInfo()),DirfFilesInfo);

	int nTotalSize = 0;

	DWORD	dwBufSize = 1024*1024*2;
	BYTE *pBuf = new BYTE[dwBufSize];
	BOOL bFlag = UpdateClientResource(DirfFilesInfo.GetRootFile(),&DirfFilesInfo,pBuf,dwBufSize,
		pPatch->m_pPatchPackage,pClientResour,NULL,nTotalSize);
	delete[] pBuf;

	if( bFlag )
	{
		pClientResour->GetFilesInfo()->SetVersion(pPatch->m_pPatchFilesInfo->GetVersion());
		pClientResour->UpdateSave();
	}
	else
	{
		pClientResour->UpdateSave();
	}
	return TRUE;
}


//更新相应的文件
BOOL CPatch::UpdateClientResource(tagFileInfo* ptagFileInfo,CFilesInfo* pFilesInfo,BYTE*& pBuf,DWORD& dwBuflen,
									  CPackage* pPackage, CClientResource* pClientResource,HWND hwnd,int& nTotalSize)
{
	if(ptagFileInfo==NULL || pFilesInfo==NULL || pPackage==NULL 
		|| pClientResource==NULL)
		return false;

	string strFileName = pFilesInfo->GetFileTextWithParent(ptagFileInfo);
	if( pFilesInfo->HaveChild(ptagFileInfo) )
	{
		//如果没有此文件夹，则添加
		tagFileInfo* pCurFolderInfo = pClientResource->GetFilesInfo()->GetFileInfoByText(strFileName);
		if(pCurFolderInfo==NULL)
		{
			string strParentName = pFilesInfo->GetFileTextWithParent(ptagFileInfo->ParentFileInfo);
			pClientResource->GetFilesInfo()->AddFolderInfo(strParentName,ptagFileInfo->strFileName,
				ptagFileInfo->dwSize,ptagFileInfo->dwOrginSize,ptagFileInfo->dwValidSize,ptagFileInfo->dwPackageType,
				ptagFileInfo->dwCompressType);
		}

		if( ENABLEBIT(ptagFileInfo->dwPackageType,NOPACK) )
		{
			//尝试创建该文件夹
			string strTemptFileName = pClientResource->GetCurFolder()+strFileName;
			::CreateDirectory(strTemptFileName.c_str(),NULL);
		}

		for(itFileInfo it = ptagFileInfo->ChildFilesInfo.begin();
			it != ptagFileInfo->ChildFilesInfo.end();it++)
		{
			if(!UpdateClientResource((*it).second,pFilesInfo,pBuf,dwBuflen,
				pPackage,pClientResource,hwnd,nTotalSize))
			{
				return FALSE;
			}
		}
	}
	else if(ptagFileInfo->bFolder == false)
	{
		//nTotalSize += ptagFileInfo->dwSize;
		//SetDisplayInfo(hwnd,nTotalSize);

		if( !ENABLEBIT(ptagFileInfo->dwPackageType,NOPACK) )
		{
			DWORD dwSize = pPackage->GetFileSize(strFileName.c_str());
			// 0字节文件不写
			if(dwSize == 0) return true;

			tagFileIndex FileIndex;
			if(dwSize > dwBuflen)
			{
				delete[] pBuf;
				dwBuflen = dwSize;
				pBuf = new BYTE[dwSize];
			}

			//从包中提取数据
			if(pPackage->ExtractToBuf(strFileName.c_str(),pBuf,dwSize,FileIndex))
			{
				try
				{
					CPackage *pGamePackage = pClientResource->GetPackageForUpdate(ptagFileInfo->dwPackageType);
					if(pGamePackage)
					{
						//	FileIndex.dwRealSize=FileIndex.dwSize;
						int nRet = pGamePackage->WriteData(FileIndex,pBuf);
						if(nRet == false)
						{
							char outPtr[1024];
							sprintf(outPtr, AppFrame::GetText("IDS_SM_TIP76"), strFileName.c_str());
							LogTools::OutLogAndTxt(outPtr);
							return false;
						}
					}
				}
				catch (...)
				{
					char strTempt[256]="";
					sprintf(strTempt,AppFrame::GetText("IDS_SM_TIP77"),
						strFileName.c_str(),dwSize);
					LogTools::OutLogAndTxt(strTempt);
					return false;
				}
			}
			else
			{
				char outPtr[1024];
				sprintf(outPtr, AppFrame::GetText("IDS_SM_TIP78"), strFileName.c_str());
				LogTools::OutLogAndTxt(outPtr);
				return false;
			}
		}
		else
		{
			try
			{
				string strTemptFileName = pClientResource->GetCurFolder()+strFileName;
				//提取到文件
				if( pPackage->ExtractToFile(strFileName.c_str(),strTemptFileName.c_str()) == false)
				{
					char outPtr[1024];
					sprintf(outPtr, AppFrame::GetText("IDS_SM_TIP79"), strFileName.c_str());
					LogTools::OutLog(outPtr);
					return false;
				}
			}
			catch (...)
			{
				return false;
			}
		}

		pClientResource->GetFilesInfo()->UpdateFileInfo(strFileName,ptagFileInfo->dwCrc32,
			ptagFileInfo->dwSize,ptagFileInfo->dwOrginSize,ptagFileInfo->dwValidSize,false,
			ptagFileInfo->dwPackageType,ptagFileInfo->dwCompressType);

		nTotalSize += ptagFileInfo->dwSize;
	}
	return TRUE;
}