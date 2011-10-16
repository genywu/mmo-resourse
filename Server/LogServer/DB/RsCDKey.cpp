#include "StdAfx.h"
#include ".\rscdkey.h"
#include "..\..\nets\basemessage.h"
#include "..\..\nets\MySocket.h"
#include "..\..\server\loginserver\loginserver\game.h"
#include "../../public/Date.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRsCDKey::CRsCDKey(void)
{
}

CRsCDKey::~CRsCDKey(void)
{
}

//函数:	检测CDKEY是否有效
//参数:	帐号，密码
//返回:
//		LOGIN_OK				成功
//		LOGINDB_ERROR			数据库打开错误
//		LOGIN_CDKEYDISABLE		账号停用
//		LOGIN_BEBANED			帐号禁用时间
//		LOGIN_PASSWORDERROR		密码错误
//		LOGIN_NOTCDKEY			帐号不存在
//备注:
//		如果连续3次输入错误密码，禁用1分钟该账户
int CRsCDKey::CDKeyIsValidate(DWORD dwIP ,const char *cdkey,vector<BYTE>& password, char *szOutBanTime)
{
	//SQL语句的安全性检查,不能含有',=,''等等
	string strCheck(cdkey);
	long lPos = (long)strCheck.find_first_of("'");
	if(lPos >= 0)
		return LOGIN_NOTCDKEY;
	lPos = (long)strCheck.find_first_of("=");
	if(lPos >= 0)
		return LOGIN_NOTCDKEY;
	lPos = (long)strCheck.find_first_of(" ");
	if(lPos >= 0)
		return LOGIN_NOTCDKEY;

	// 检查帐号是否存在
	char sql[256];
	_ConnectionPtr cn;
	_RecordsetPtr rs;

	try
	{
		CreateCn(cn);
		CreateRs(rs);

		OpenCn(cn);
		sprintf(sql,"SELECT * FROM CSL_CDKEY WHERE cdkey='%s'", cdkey); //cdkey is PK
		OpenRs(sql, rs, cn/*m_Connection*/);

		// 帐户不存在
		if(rs->GetadoEOF()) 
		{
			ReleaseRs(rs);
			ReleaseCn(cn);
			return LOGIN_NOTCDKEY;
		}
		// 帐户存在
		else 
		{
			// 检查帐号是否被禁止
//			char currtime[20];
//			char bantime[20];
			SYSTEMTIME ct,bt;

			GetLocalTime(&ct);
			tagTime cTime,bTime;
			memcpy(&cTime,&ct,sizeof(tagTime));

			// 2004-12-31 14:16:1
			//wsprintf(currtime, "%d-%d-%d %d:%d:%d", ct.wYear,ct.wMonth,ct.wDay,ct.wHour,ct.wMinute,ct.wSecond);
			
			// 1999-1-1 1:1:1
			VariantTimeToSystemTime(double( rs->GetCollect("ban_time") ) ,&bt);


			memcpy(&bTime,&bt,sizeof(tagTime));

			
			//wsprintf(bantime,"%s",);

			if(cTime < bTime)
			{
				ReleaseRs(rs);
				ReleaseCn(cn);
				sprintf(szOutBanTime, "%d年%d月%d日 %d点%d分",
					bTime.Year(), bTime.Month(), bTime.Day(), bTime.Hour(), bTime.Minute());
				return LOGIN_BEBANED; 
			}

			string strMD5;
			if( GetPassWord(rs,strMD5) == false )
			{
				ReleaseRs(rs);
				ReleaseCn(cn);
				return LOGINDB_ERROR; // 数据库操作错误  
			}
			char strDBMD5[50];
			strcpy(strDBMD5,strMD5.c_str());
			strupr(strDBMD5);

			//把二进制密码转换成字符串
			char strClientMD5[50];
			static char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7',
				'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
			BYTE* pByte = &password[0];
			for(int i=0;i < 16;i++)
			{
				int bValue = password[i];
				int bValue1 = bValue>>4;
				int bValue2 = bValue&0x0F;
				if(bValue1 >= 0 && bValue1 <= 15 && bValue2 >= 0 && bValue2 <= 15)
				{
					strClientMD5[i*2] = hexDigits[bValue1];
					strClientMD5[i*2+1] = hexDigits[bValue2];
				}
			}
			strClientMD5[32]='\0';
			strupr(strClientMD5);
			
			if( strcmp(strDBMD5,strClientMD5) != 0 ) // 密码不等
			{
				// 错误计数+1
				map<string,int>::iterator it=GetGame()->GetPWErrorMap()->find(string(cdkey));
				if(it!=GetGame()->GetPWErrorMap()->end())
				{
					//3次错误输入密码禁用该帐号
					if(it->second>=2)
					{
						CDKeyBan(cdkey,1);
						GetGame()->GetPWErrorMap()->erase(it);
					}
					else 
					{
						it->second++;
					}
				}
				else
				{
					GetGame()->GetPWErrorMap()->insert(map<string,int>::value_type(string(cdkey),1));
				}

				ReleaseRs(rs);
				ReleaseCn(cn);
				return  LOGIN_PASSWORDERROR;
			}
			else	//密码正确
			{
				/*
				 * 	这里做IP检测，是否允许玩家登录
				 */

				// 检测ip是否被允许
				if( !IPIsAllowed(dwIP) )
				{
					return LOGIN_IP_FORBID;
				}

				// 如果ip是否被禁止
				if( IPIsForbidded(dwIP) )
				{
					return LOGIN_IP_FORBID;
				}

				
				/*
				 * 	这了IP列表检测，是否允许GM登录
				 */
				if( !IsBetweenIP(cdkey, dwIP) )	{
					ReleaseRs(rs);
					ReleaseCn(cn);
					return LOGIN_CDKEYDISABLE;
				}


				//清除其密码错误的列表记录
				map<string,int>::iterator it=GetGame()->GetPWErrorMap()->find(string(cdkey));
				if(it!=GetGame()->GetPWErrorMap()->end())
				{
					GetGame()->GetPWErrorMap()->erase(it);
				}

				ReleaseRs(rs);
				ReleaseCn(cn);
				return  LOGIN_OK;			// 匹配
			}
			
		}
	}
	catch(_com_error &e)
	{
		ReleaseRs(rs);
		ReleaseCn(cn);
		PrintErr("RsCDKey cdkey is validate eorror", e);
		return LOGINDB_ERROR; // 数据库操作错误 
	}
}

//函数：	禁用帐号
//参数：	帐号、时间（单位：分钟）
bool CRsCDKey::CDKeyBan(const char* cdkey,long lminute)
{
	char sql[256];
	_ConnectionPtr cn;

	if(!CreateCn(cn)) return false;
	if(!OpenCn(cn)) return false;

	sprintf(sql,"UPDATE CSL_CDKEY SET ban_time=DATEADD(minute,%d,GETDATE()) WHERE cdkey='%s'",
		lminute,cdkey);
	if(!ExecuteCn(sql , cn/*m_Connection*/))
	{
		ReleaseCn(cn);
		AddLogText("RcCDKey cdkey ban error");
		return false;
	}
	ReleaseCn(cn);
	return true;
}

bool CRsCDKey::GetPassWord(_RecordsetPtr& rs,BYTE* pByte)
{
	try
	{
		long lSize = rs->GetFields()->GetItem("password")->ActualSize;
		if(lSize == 16)
		{
			_variant_t varian_t1 = rs->GetCollect("password");
			BYTE* pBuf = NULL;
			SafeArrayAccessData(varian_t1.parray,(void**)&pBuf);
			SafeArrayUnaccessData(varian_t1.parray);
			if(pBuf)
			{
				memcpy(pByte,pBuf,16);
				return true;
			}
		}
		
	}
	catch (_com_error &e)
	{
		PrintErr("Get PassWordeorror", e);
		return false;
	}
	return false;
}


bool CRsCDKey::GetPassWord(_RecordsetPtr& rs,string& strMD5)
{
	try
	{
		strMD5 = (_bstr_t)rs->GetCollect("password");
		if(strMD5.length() != 32)
		{
			return false;
		}
		return true;
	}
	catch (_com_error &e)
	{
		PrintErr("Get PassWordeorror", e);
		return false;
	}
	return false;
}

bool CRsCDKey::IPIsAllowed(DWORD dwIP)
{
	if( !GetGame()->GetSetup()->bCheckAllowIP )
		return true;

	// 交换IP四个字节的顺序
	BYTE bt1 = LOBYTE(LOWORD(dwIP));
	BYTE bt2 = HIBYTE(LOWORD(dwIP));
	BYTE bt3 = LOBYTE(HIWORD(dwIP));
	BYTE bt4 = HIBYTE(HIWORD(dwIP));
	dwIP = MAKELONG(MAKEWORD(bt4, bt3), MAKEWORD(bt2, bt1));

	// 检查IP是否允许（特殊业务）
	char sql[256];

	_ConnectionPtr cn;
	_RecordsetPtr rs;

	try
	{
		CreateCn(cn);
		CreateRs(rs);

		OpenCn(cn);
		sprintf(sql,"SELECT count(*) as exp1 FROM ip_allow where 0x%x >= int_begin and 0x%x <= int_end", dwIP, dwIP);
		OpenRs(sql, rs, cn/*m_Connection*/);

		if( !rs->GetadoEOF() )
		{
			long lCount = rs->GetCollect("exp1");
			if (lCount > 0)
			{
				ReleaseRs(rs);
				ReleaseCn(cn);
				return true;
			}
		}
	}
	catch(_com_error &e)
	{
		PrintErr("check ip allow error", e);
	}

	
	ReleaseRs(rs);
	ReleaseCn(cn);
	return false;
}

bool CRsCDKey::IPIsForbidded(DWORD dwIP)
{
	if( !GetGame()->GetSetup()->bCheckForbidIP )
		return false;

	// 交换IP四个字节的顺序
	BYTE bt1 = LOBYTE(LOWORD(dwIP));
	BYTE bt2 = HIBYTE(LOWORD(dwIP));
	BYTE bt3 = LOBYTE(HIWORD(dwIP));
	BYTE bt4 = HIBYTE(HIWORD(dwIP));
	dwIP = MAKELONG(MAKEWORD(bt4, bt3), MAKEWORD(bt2, bt1));

	// 检查IP是否禁止（特殊业务）
	char sql[256];

	_ConnectionPtr cn;
	_RecordsetPtr rs;

	try
	{
		CreateCn(cn);
		CreateRs(rs);

		OpenCn(cn);
		sprintf(sql,"SELECT count(*) as exp1 FROM ip_forbid where 0x%x >= int_begin and 0x%x <= int_end", dwIP, dwIP);
		OpenRs(sql, rs, cn/*m_Connection*/);

		if( !rs->GetadoEOF() )
		{
			long lCount = rs->GetCollect("exp1");
			if (lCount > 0)
			{
				ReleaseRs(rs);
				ReleaseCn(cn);
				return true;
			}
		}
	}
	catch(_com_error &e)
	{
		PrintErr("check ip allow error", e);
	}


	ReleaseRs(rs);
	ReleaseCn(cn);
	return false;
}

bool CRsCDKey::IsBetweenIP(const char *szCdkey, DWORD dwIP) {
	
	if( !GetGame()->GetSetup()->bCheckBetweenIP ) return true;

	char sql[256];
	unsigned char ipv4[4] = {0,0,0,0};

	ipv4[0] = LOBYTE(LOWORD(dwIP)); 
	ipv4[1] = HIBYTE(LOWORD(dwIP));
	ipv4[2] = LOBYTE(HIWORD(dwIP));
	ipv4[3] = HIBYTE(HIWORD(dwIP));
	_ConnectionPtr cn;
	_RecordsetPtr rs;

	try {
		CreateCn(cn);
		CreateRs(rs);

		OpenCn(cn);
		sprintf(sql,"SELECT * FROM ip_list WHERE cdkey='%s'", szCdkey);
		OpenRs(sql, rs, cn/*m_Connection*/);

		// 如果不存在与ip列表中，就直接进去。
		if( rs->GetadoEOF() ) {
			ReleaseRs(rs);
			ReleaseCn(cn);
			return true;
		}
		// 如果ip列表存在，则需要匹配列表 中没一个ip的范围。
		// 其中有任意1个匹配才允许进入，所有不匹配则不允许进入。
		else {
			bool bIsLessEqual; // is <=
			bool bIsMoreEqual; // is >=
			unsigned long ipb;
			unsigned char ipv4b[4] = {0,0,0,0};
			
			unsigned long ipe;
			unsigned char ipv4e[4] = {0,0,0,0};

			while( !rs->GetadoEOF() ) {

				ipb = STR2UL( (_bstr_t)(rs->GetCollect("ip_begin")) );

				ipe = STR2UL( (_bstr_t)(rs->GetCollect("ip_end")) );
				
				ipv4b[0] = LOBYTE(LOWORD(ipb)); 
				ipv4b[1] = HIBYTE(LOWORD(ipb));
				ipv4b[2] = LOBYTE(HIWORD(ipb));
				ipv4b[3] = HIBYTE(HIWORD(ipb));

				ipv4e[0] = LOBYTE(LOWORD(ipe)); 
				ipv4e[1] = HIBYTE(LOWORD(ipe));
				ipv4e[2] = LOBYTE(HIWORD(ipe));
				ipv4e[3] = HIBYTE(HIWORD(ipe));

				// 判断处理
				bIsMoreEqual = false;
				bIsLessEqual = false;

				// 先检测  当前IP是否大于等于起始IP
				if( ipv4[0] == ipv4b[0] ) {
					if( ipv4[1] == ipv4b[1] ) {
						if( ipv4[2] == ipv4b[2] ) {
							if( ipv4[3] >= ipv4b[3] ) {
								bIsMoreEqual = true;
							}
						}
						else if( ipv4[2] > ipv4b[2] ) {
							bIsMoreEqual = true;
						}
					}
					else if( ipv4[1] > ipv4b[1] ) {
						bIsMoreEqual = true;
					}
				}
				else if( ipv4[0] > ipv4b[0] ) {
					bIsMoreEqual = true;
				}
				// 在继续检测 当前IP是否小于等于结束IP
				if(bIsMoreEqual) {
					if( ipv4[0] == ipv4e[0] ) {
						if( ipv4[1] == ipv4e[1] ) {
							if( ipv4[2] == ipv4e[2] ) {
								if( ipv4[3] <= ipv4e[3] ) {
									bIsLessEqual = true;
								}
							}
							else if( ipv4[2] < ipv4e[2] ) {
								bIsLessEqual = true;
							}
						}
						else if( ipv4[1] < ipv4e[1] ) {
							bIsLessEqual = true;
						}
					}
					else if( ipv4[0] < ipv4e[0] ) {
						bIsLessEqual = true;
					}
				}

				// 如果当前ip在列表内，就允许进去。
				if( bIsLessEqual && bIsMoreEqual ) {
					ReleaseRs(rs);
					ReleaseCn(cn);
					return true;
				}

				rs->MoveNext();
			}
		}
	}
	catch(_com_error &e) {
		PrintErr("check ip list error", e);
	}

	ReleaseRs(rs);
	ReleaseCn(cn);
	return false;
}
