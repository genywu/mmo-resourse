#include "StdAfx.h"
#include "Gapp.h"
#include "../../../SCMPublic/def/UserDef.h"
//#include "../../../SCMPublic/class/CGameInfo.h"
#include <TlHelp32.h>
#include <psapi.h>
#include "HardWareInfo.h"
#pragma comment(lib,"Psapi.lib")

CGapp::CGapp(void)
: m_pPatch(NULL)
, m_dwProcessID(0)
, m_MainAITick(0)
, m_dwLoadResTimerID(-1)
, m_dwMainLoopTimerID(-1)
, m_dwShutDownTimerId(-1)
, m_dwPingTimerID(-1)
, m_bLatest(0)
, m_hHandle(NULL)
{
	_dwGappState = eSS_Stop;
	memset(m_szPath,0,BASE_TEXT_SIZE);
	memset(m_szTitle,0,BASE_TEXT_SIZE);
	memset(m_szMapFileName,0,BASE_NAME_SIZE);
	memset(m_szMapObjName,0,BASE_NAME_SIZE);
	memset(m_szOutPutDir,0,BASE_NAME_SIZE);
	memset(m_szNextVersion,0,BASE_NAME_SIZE);
	memset(m_szPhysName,0,BASE_NAME_SIZE);
}

CGapp::~CGapp(void)
{
	if (m_pPatch!=NULL)
	{
		delete m_pPatch;
		m_pPatch = NULL;
	}
}

BOOL CGapp::Init(TiXmlElement* p)
{
	int nID = 0;
	int nVersion=0;
	int nStatus=0;
	p->Attribute("ID",&nID);

	const char* szType=p->Attribute("TYPE");
	if (szType!=NULL)
	{
		if (0 == strcmp(szType,"AS"))
		{
			_dwGappType = eGT_AS;
		}
		else if (0 == strcmp(szType,"LS"))
		{
			_dwGappType = eGT_LS;
		}
		else if (0 == strcmp(szType,"DBS"))
		{
			_dwGappType = eGT_DB;
		}
		else if (0 == strcmp(szType,"WS"))
		{
			_dwGappType = eGT_WS;
		}
		else if (0 == strcmp(szType,"GS"))
		{
			_dwGappType = eGT_GS;
		}
		else if (0 == strcmp(szType,"LOGS"))
		{
			_dwGappType = eGT_LOGS;
		}
		else if (0 == strcmp(szType,"BS"))
		{
			_dwGappType = eGT_BS;
		}
		else
		{
			LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP47"),szType);
		}
	}
	
	const char* str=p->Attribute("Name");
	if (str!=NULL)
	{	
		strcpy(m_szPhysName,str);
	}
	//显示名,在SUC上显示
	const char* cTitle=p->Attribute("Title");
	if (cTitle!=NULL)
	{	
		strcpy(_szGappName,cTitle);
	}
	const char* path=p->Attribute("Path");
	if (str!=NULL)
	{	
		strcpy(m_szPath,path);
	}
	str = p->Attribute("MapFileName");
	if (str!=NULL)
	{
		strcpy(m_szMapFileName,str);
	}
	str = p->Attribute("MapFileObj");
	if (str!=NULL)
	{
		strcpy(m_szMapObjName,str);
	}
	str = p->Attribute("OutDir");
	if (str!=NULL)
	{
		strcpy(m_szOutPutDir,str);
	}
	_dwGappID=nID;
	SetVersion();
	return TRUE;
}
void CGapp::SetInfo(const char* strTitle)
{
	strcpy(m_szTitle,strTitle);
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP48"),_dwGappID,GetDisplayName());
}

BOOL CGapp::Start()
{
	if (!Reset() || !CheckProcessNum())
	{
		return FALSE;
	}
	STARTUPINFO si;
	memset(&si,0,sizeof(STARTUPINFO));
	PROCESS_INFORMATION pi;
 	char tempPath[BASE_TEXT_SIZE]={0};
	strcpy(tempPath,m_szPath);
	strcat(tempPath,"\\");
	strcat(tempPath,m_szPhysName);
	//创建进程启动GAPP
	BOOL bRet = ::CreateProcess(tempPath,NULL,NULL,NULL,FALSE,0,NULL,m_szPath,&si,&pi);
	if (!bRet)
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP49"),_szGappName);
		GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP49"),_dwGappID,_szGappName);
		return FALSE;
	}
	m_dwProcessID = pi.dwProcessId;
	//创建定时器,这个时间是GAPP读资源文件的所需的时间
	tagSMTimerVar* pVar = GetSM()->GetTimerVar();
	pVar->TimerType=GAPP_LOADRES;
	m_dwLoadResTimerID = GetSM()->GetGappTimer()->Schedule(this,pVar,timeGetTime()+GetSM()->GetSccPrama()->dwStartTimeOut);
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP50"),m_dwLoadResTimerID);
	return TRUE;
}

bool CGapp::CheckProcessNum()
{
	DWORD dwTotalNum=GetSM()->GetGappNumByType((eGappType)_dwGappType);
	HANDLE hprocess = NULL;
	PROCESSENTRY32 pe32 = {0};
	hprocess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hprocess == (HANDLE)-1)
	{
		return false;
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);
	DWORD dwRunningNum=0;
	if (Process32First(hprocess,&pe32))
	{
		do 
		{
			if (strcmp(m_szPhysName,pe32.szExeFile)==0)
			{
				dwRunningNum+=1;
			}
		} while(Process32Next(hprocess,&pe32));
	}
	CloseHandle(hprocess);
	if (dwRunningNum>=dwTotalNum)
	{
		GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP80"),
			GetDisplayName());
		return false;
	}
	return true;
}

bool CGapp::Reset()
{
	if (_dwGappState==eSS_Running)
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP68"),m_szTitle);
		return false;
	}
	_dwGappState=eSS_Stop;
	return true;
}

BOOL CGapp::ShutDown()
{
	if (!FindProcByProcID())
	{
		GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP89"),_szGappName);
		return FALSE;
	}
	CMessage msg(MSG_SM2GAPP_SHUTDOWN);
	msg.SendToGapp(_dwGappID);
	return TRUE;
}
void CGapp::AddToByteArray(DBWriteSet& dbWriteSet)
{	
	//信息产生时间
	_dwGappTime = timeGetTime();
	dbWriteSet.AddToByteArray(this, sizeof(tagGappBaseInfo));
}

void CGapp::SetState(DWORD sdwstate)
{
	_dwGappState = sdwstate;
	if(sdwstate == eSS_Stop)
	{
		//创建定时器，超时后检查是否关闭成功
		tagSMTimerVar* pVar = GetSM()->GetTimerVar();
		pVar->TimerType=GAPP_SHUTDOWN;
		m_dwShutDownTimerId = GetSM()->GetGappTimer()->Schedule(this,pVar,timeGetTime()+GetSM()->GetSccPrama()->dwShutdownTimeOut);
	}
	else if(sdwstate == eSS_Running)
	{

	}
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd,LPARAM lparam)
{
	DWORD dwThreadID;
	GetWindowThreadProcessId(hWnd,&dwThreadID);
	PWNDINFO pInfo=(PWNDINFO)lparam;
	if (dwThreadID==pInfo->_dwProcID)
	{
		pInfo->_hWnd=hWnd;
		return FALSE;
	}
	return TRUE;
}
HWND GetHwnd(DWORD procID)
{
	WNDINFO wi;
	wi._dwProcID=procID;
	wi._hWnd=NULL;
	EnumWindows(EnumWindowsProc,(LPARAM)&wi);
	return wi._hWnd;
}

bool CGapp::EnumGappWindow(DWORD dwProcID)
{
	WNDINFO wndinfo;
	wndinfo._dwProcID=m_dwProcessID;
	wndinfo._hWnd=NULL;
	::EnumWindows(EnumWindowsProc,(LPARAM)&wndinfo);
	if (wndinfo._hWnd!=NULL)
	{
		m_hHandle=wndinfo._hWnd;
		//SM重启了，通过枚举窗口获取GAPP的窗口标题
		GetWindowText(wndinfo._hWnd,m_szTitle,BASE_TEXT_SIZE);
	}
	return true;
}

bool CGapp::SetProcessID()
{	
	if (m_dwProcessID>0)
	{
		EnumGappWindow(m_dwProcessID);
		return true;
	}
	HANDLE hprocess = NULL;
	PROCESSENTRY32 pe32 = {0};
	hprocess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hprocess == (HANDLE)-1)
	{
		return false;
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hprocess,&pe32))
	{
		do 
		{
			if (strcmp(m_szPhysName,pe32.szExeFile)==0)
			{
				//是第一次调用此函数
				if(m_hHandle==NULL)
				{
					if (GetSM()->IsSameProcID(pe32.th32ProcessID))
					{
						continue;
					}
					else
						m_dwProcessID = (DWORD)pe32.th32ProcessID;
					//获取使用的内存
					//GetHardWareLoad();
					EnumGappWindow(m_dwProcessID);
				}
				return true;
			}
		} while(Process32Next(hprocess,&pe32));
	}
	CloseHandle(hprocess);
	return false;

}

bool CGapp::GetHardWareLoad()
{
	HANDLE hprocess;
	PROCESS_MEMORY_COUNTERS pmc;
	hprocess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE,m_dwProcessID);
	if (NULL == hprocess)
	{
		return false;
	}
	if (GetProcessMemoryInfo(hprocess,&pmc,sizeof(pmc)))
	{
		DWORD dwMemoryLoad = (DWORD)pmc.PagefileUsage /1024;
	}
	CloseHandle(hprocess);
	return true;
}
void CGapp::DelayTest()
{
	/*CMessage msg(MSG_SM2GAPP_TESTCONN);
	long lTime = timeGetTime();
	msg.Add(lTime);
	msg.SendToGapp(_dwGappID);*/
//	int usage=CHardWareInfo::GetInstance().GetProcUsage(m_dwProcessID);
//	if (usage>=0)
//	{
//		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP51"),usage);
//	}
	RequestAITick();
}

long CGapp::SetDelayTime(DWORD dwtime)
{
	if ((DWORD)m_listDelayTime.size() > GetSM()->GetSccPrama()->dwSaveHardWareInfoSize)
	{
		m_listDelayTime.pop_front();
		m_listDelayTime.push_back(dwtime);
	}
	else
		m_listDelayTime.push_back(dwtime);	
	return dwtime;
}

long CGapp::GetDelayTime()
{
	long n = (long)m_listDelayTime.size();
	list<long>::iterator it=m_listDelayTime.begin();
	long sum=0;
	for (;it!=m_listDelayTime.end();it++)
	{
		sum += *it;
	}
	_dwGappDelayTime = sum/n;
	return _dwGappDelayTime;
}

BOOL CGapp::InitMemServer()
{
	if (strcmp(m_szMapFileName,"")!=0)
	{
		DWORD dwID = GetInst(CMemServer).AddServer(_dwGappID, m_szMapObjName,m_szMapFileName,m_szOutPutDir);
		if (dwID == _dwGappID)
		{
			LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP52"),m_szOutPutDir,m_szMapObjName);
		}
		else
		{	
			LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP53"),m_szMapObjName);
		}
	}
	//通过进程名检查是否有已启动的GAPP
	if (SetProcessID())
	{
		RequestAITick();
	}
	
	return TRUE;
}

BOOL CGapp::CheckVersion(const char* szVersion)
{
	if (szVersion == NULL)
	{
		RequestFile("Game.vs");
	}
	//更新到指定版本
	else
	{
		stringstream str;
		str<<szVersion;
		CmpVersion(str);
	}
	return TRUE;
}

BOOL CGapp::UpdateGapp(const char* szPatchName)
{
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP54"),_dwGappID,_szGappName);
	
	//等待包文件写入磁盘
	char szServerPatch[BASE_TEXT_SIZE]="";
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP55"),_szGappName,szPatchName);
	strcpy(szServerPatch,m_szPath);
	strcat(szServerPatch,"\\updatefiles\\");
	strcat(szServerPatch,szPatchName);
	//更新 
	string strVersion = m_pPatch->Update(m_szPath,szPatchName);
	m_pPatch->Release();
	CMessage msg(MSG_SM2SCC_CTRL_Notify_UpdateGapp);
	if (strVersion != "")
	{
		memset(_szGappVersion,0,MAX_GAPP_NAME_SZIE);
		strcpy(_szGappVersion,strVersion.c_str());
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP56"),_dwGappID,_szGappName);
		msg.Add((BYTE)1);
	}
	else
	{
		msg.Add((BYTE)0);
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP57"),_dwGappID,_szGappName);
	}
	msg.Add(_dwGappID);
	msg.Add(_szGappVersion);
	msg.SendToAllSCC();
	delete m_pPatch;
	m_pPatch = NULL;	
	//如果不是最新版本，则继续更新
	if (m_bLatest !=0)
	{
		CheckVersion();
	}
	return TRUE;
}
BOOL CGapp::SetVersion()
{
	char filename[BASE_TEXT_SIZE]="";
	strcpy(filename,m_szPath);
	strcat(filename,"\\PackRes.cfg");
	FILE *fp = fopen(filename,"r");
	if (fp == NULL)
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP58"),_szGappName);
		return FALSE;
	}
	fread(_szGappVersion,10,1,fp);
	fclose(fp);
	return TRUE;
}

void CGapp::RequestFile(char* filename)
{
	CMessage msg(MSG_SM2SCC_CTRL_Request_File);
	msg.Add(_dwGappID);
	msg.Add(_dwGappType);
	msg.Add(filename);
	msg.SendToAllSCC();
}
//更新获取文件由此函数开始
BOOL CGapp::CmpVersion(stringstream& stream)
{
	//比较版本的时候先读取本地最新版本
	SetVersion();
	char szVersion[BASE_NAME_SIZE]="";
	stream>>szVersion;
	//与SCC上的版本不一样
	if(strcmp(_szGappVersion,szVersion) != 0)
	{
		//生成如"Update-00.00.0001.vs"文件名
		char szFile[BASE_NAME_SIZE]="";
		strcpy(szFile,"Update-");
		strcat(szFile,_szGappVersion);
		strcat(szFile,".vs");
		memset(m_szNextVersion,0,BASE_NAME_SIZE);
		strcpy(m_szNextVersion,szFile);
		RequestFile(szFile);
	}
	else
	{
		m_bLatest = 0;
		GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP90"),_dwGappID,_szGappName,_szGappVersion);
	}
	return TRUE;
}

BOOL CGapp::GetNextVersionPatch(stringstream& stream)
{
	char szVersion[BASE_NAME_SIZE]="";
	char szCrc32[BASE_NAME_SIZE]="";
	stream>>szCrc32>>szVersion;
	DWORD dwCrc32 = (DWORD)atof(szCrc32);
	if (m_pPatch != NULL)
	{
		delete m_pPatch;
		m_pPatch = NULL;
	}
	m_pPatch = new CPatch;
	m_pPatch->SetCrc32(dwCrc32);
	strcat(szVersion,".zip");
	//获取下一个版本升级包
	RequestFile(szVersion);
	return TRUE;
}


BOOL CGapp::GetPackRes()
{
	//请求原始包
	CMessage msg(MSG_SM2SCC_CTRL_Request_ServerPack);
	msg.Add(_dwGappID);
	msg.Add(_dwGappType);
	msg.SendToAllSCC();
	return TRUE;
}

BOOL CGapp::WritePackRes(DBReadSet& dbreadset)
{
	UpdatePack pack;
	if (!pack.DeCode(dbreadset))
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP59"),_dwGappID,_szGappName);
		return FALSE;
	}
	//重命名原有文件
	char szTo[BASE_TEXT_SIZE]={0};
	strcpy(szTo,m_szPath);
	char time[BASE_NAME_SIZE]={0};
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	_snprintf(time,MAX_PATH,"%02d_%02d_%02d_%02d_%02d",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	strcat(szTo,time);
	int nerrorno = rename(m_szPath,szTo);
	if (nerrorno != 0)
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP60"),errno,GetLastError());
	}
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP61"));
	MakeSureDirectoryPathExists(m_szPath);
	
	CMessage msg(MSG_SM2SCC_CTRL_Notify_ServerPack);	
	if (pack.WriteDataToDir(m_szPath))
	{
		msg.Add((BYTE)1);
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP62"),_dwGappID,_szGappName);
	}
	else
	{
		msg.Add((BYTE)0);
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP63"),_dwGappID,_szGappName);
	}
	//更新版本
	SetVersion();
	DBWriteSet dbWriteSet;
	msg.GetDBWriteSet(dbWriteSet);
	AddToByteArray(dbWriteSet);
	msg.SendToAllSCC();
	return TRUE;
}

BOOL CGapp::GetFile(DBReadSet& dbreadset)
{
	char szFileName[BASE_NAME_SIZE]="";
	//得到文件信息
	CUpdateFile file;
	if(!file.DeCode(dbreadset))
	{
		GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP64"));
		return FALSE;
	}
	strcpy(szFileName,file.GetName());
	
	stringstream stream;
	stream<<file.GetData();
	string strType(szFileName);
	if (strcmp(szFileName,"Game.vs") == 0)
	{
		CmpVersion(stream);
	}
	else
	{
		size_t pos = (int)strType.find("zip");
		if (pos>=0 && pos < 0xffff )
		{
			//将包文件写入磁盘
			char szPath[BASE_TEXT_SIZE]="";
			strcpy(szPath,m_szPath);
			strcat(szPath,"\\updatefiles\\");
			MakeSureDirectoryPathExists(szPath);
			if(file.WriteDataToPath(szPath))
			{
				//开始更新
				UpdateGapp(szFileName);
			}
			else
			{
				GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP85"),_szGappName);
			}
		}
		else
		{
			size_t pos = strType.find("Update");
			if (pos >= 0 && pos < 0xffff)
			{
				GetNextVersionPatch(stream);
			}
			else
			{
				GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP86"),szFileName);
			}
		}
	}
	return TRUE;
}

void CGapp::OnTimeOut(DWORD dwTimerID,DWORD dwcurTime,const void* pargs)
{
	tagSMTimerVar* timervar = (tagSMTimerVar*)(pargs);
	switch(timervar->TimerType)
	{
	case GAPP_LOADRES:
		{
			GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP87"),_szGappName,m_dwLoadResTimerID);
		}
		break;
	case GAPP_MAINLOOP: 
		{
			if (CheckStart())
			{

			}
		}
		break;
	case GAPP_SHUTDOWN:
		{
			if(IsShutDownOk())
			{
				_dwGappState=eSS_Stop;
				LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP65"),_dwGappID,m_szTitle);
				CMessage msg(MSG_SM2SCC_CTRL_Notify_SHUTDOWN);
				msg.Add(_dwGappID);
				msg.SendToAllSCC();
			}
		}
		break;
		//AI计数超时表示GAPP繁忙
	case GAPP_PING:
		{
			if (_dwGappState==eSS_Running)
			{
				_dwGappState = eSS_Busy;
			}
		}
		break;
	default:
		break;
	}
	
}

void CGapp::RequestAITick()
{
	if (_dwGappState == eSS_Busy)
	{
		GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP88"),_szGappName);
		return;
	}
	CMessage msg(MSG_SM2GAPP_GET_MAINAITICK);
	msg.SendToGapp(_dwGappID);
	//发送获取心跳计数消息后，注册定时器
	tagSMTimerVar* pVar = GetSM()->GetTimerVar();
	pVar->TimerType=GAPP_PING;
	//创建获取AITick定时器
	m_dwPingTimerID = GetSM()->GetGappTimer()->Schedule(
		this,pVar,timeGetTime()+GetSM()->GetSccPrama()->dwPingTimeOut);	
}

void CGapp::CancelLoadResTimer()
{
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP66"),m_dwLoadResTimerID);
	GetSM()->GetGappTimer()->Cancel(m_dwLoadResTimerID);
	OnTimerCancel(m_dwLoadResTimerID,0);
	m_dwLoadResTimerID = -1;
}

void CGapp::OnTimerCancel(DWORD dwTimeID,const void* parg)	
{
	if(parg)
	{
		tagSMTimerVar* pTimvar = (tagSMTimerVar*)parg;
		if (pTimvar->pvar!= NULL)
		{
			SAFE_DELETE(pTimvar->pvar);
		}
		GetSM()->FreeTimerVar(pTimvar);
	}
}
//GAPP的主循环第一次发来的AI计数
void CGapp::SetMainTick(DWORD tick)
{
	LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP67"),m_szTitle);
	if (0 == tick)
	{
	}
	m_MainAITick = tick;
	tagSMTimerVar* pVar = GetSM()->GetTimerVar();
	pVar->TimerType=GAPP_MAINLOOP;
	//创建获取AITick定时器
	m_dwMainLoopTimerID = GetSM()->GetGappTimer()->Schedule(this,pVar,timeGetTime()+5000);	
}
//与第一次AI计数比较
void CGapp::CmpAITick(DWORD tick)
{
	//收到此信息表示GAPP运行正常
	CancelPingTimer();
	//AI计数增大且进程号不为0，m_dwProcessID不为0则已找到该进程
	if (m_dwProcessID==0)
	{
		SetProcessID();
	}
	if(_dwGappState == eSS_Running)
		return;
	if (tick - m_MainAITick>=0 && FindProcByProcID() && m_dwProcessID!=0)
	{
		m_MainAITick = tick;
		_dwGappState = eSS_Running;
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP68"),m_szTitle);
		CMessage msg(MSG_SM2SCC_CTRL_Notify_START);
		msg.Add(_dwGappID);
		msg.SendToAllSCC();
	}
	else
	{
		LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP69"),m_szTitle);
	}
}

void CGapp::CancelPingTimer()
{
	GetSM()->GetGappTimer()->Cancel(m_dwPingTimerID);
	m_dwPingTimerID = -1;
	OnTimerCancel(m_dwPingTimerID,0);
}

bool CGapp::FindGappWindow()
{
	HWND hwnd = ::FindWindow(NULL,m_szTitle);
	if (hwnd != NULL)
	{
		return true;
	}
	return false;
}

bool CGapp::CheckStart()
{
	//检查进程和窗口
	if (!FindProcByProcID())
	{
		GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP81"),m_szTitle);
		return false;
	}
	if (!FindGappWindow())
	{
		GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP82"),m_szTitle);
		return false;
	}
	RequestAITick();
	return true;
}

bool CGapp::IsShutDownOk()
{
	//关闭失败
	if (FindProcByProcID())
	{
		GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP83"),m_szTitle);
		return false;
	}
	if (FindGappWindow())
	{	
		GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP84"),m_szTitle);
		return false;
	}
	//重置HANDLE
	m_hHandle=NULL;
	return true;
}


bool CGapp::SetLSPort(DWORD dwPort)
{
	if (_dwGappType!=eGT_LS)
		return false;
	char szPath[BASE_TEXT_SIZE]={0};
	strcpy(szPath,m_szPath);
	strcat(szPath,"\\setup.ini");
	fstream file(szPath);
	if (!file.is_open())
	{
		return false;
	}
	vector<string>vecStr;
	string strTitle,strData;
	while (1)
	{
		file>>strTitle>>strData;
		if (file.eof())
		{
			break;
		}
		vecStr.push_back(strTitle);
		vecStr.push_back(strData);
	}
	file.close();

	char szPort[BASE_NAME_SIZE]={0};
	sprintf(szPort,"%d",dwPort);
	vecStr[1]=szPort;
	
	ofstream ofile(szPath);
	if (!ofile.is_open())
	{
		return false;
	}

	for (int i=0;i<(int)vecStr.size();i++)
	{
		if (i%2==0)
		{
			ofile<<vecStr[i].c_str()<<"\t";
		}
		else
			ofile<<vecStr[i].c_str()<<"\n";
	}
	ofile.close();
 	return true;
}

bool CGapp::FindProcByProcID()
{
	bool bFind=false;
	HANDLE hprocess = NULL;
	PROCESSENTRY32 pe32 = {0};
	hprocess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hprocess,&pe32))
	{
		do 
		{
			if(m_dwProcessID==pe32.th32ProcessID)
				bFind = true;
			
		} while(Process32Next(hprocess,&pe32));
	}
	return bFind;
}