#include "Merger2.h"

//////////////////////////////////////////////////////////////////////////////////////////
// 全局函数
//////////////////////////////////////////////////////////////////////////////////////////

bool LoadConf(const char *filename);
void MakeGameConnStr();
void MakeLoginConnStr();

char *FixName(char *n_name, const char *o_name, bool existent = false);		//第1次使用false
//char *GetGUIDString(char *s);
char *GetDateString(_variant_t v, char *s);
bool CopyChunk(_RecordsetPtr &des, const _RecordsetPtr &src, const char *fileds);
//ULONG ReassignGoodsID(ULONG o_id);
void Log(const char *str);
void LogChanged(FILE *fChanged, const char *str);

BYTE GetNum(char str[3]);

//////////////////////////////////////////////////////////////////////////////////////////
// 全局变量
//////////////////////////////////////////////////////////////////////////////////////////
char sql[2048],msg[1024];
// 记录文件
FILE *file;

// 帐号记录 格式由系统运营提供，该记录他们需要
FILE *fAC;		// Account changed
FILE *fPNC;		// Player name changed

string profix;
string spechar;

int nWorldID;

// 数据库配置
SQL_SETUP sqlDes; // 目的数据库配置
SQL_SETUP sqlSrc; // 源数据库配置

SQL_SETUP sqlLoginDes; // 目的数据库配置
SQL_SETUP sqlLoginSrc; // 源数据库配置


string cnstrDes;
string cnstrSrc;
string logincnstrDes;
string logincnstrSrc;

ID_SETUP idDes;	// 目的数据库ID配置
//ID_SETUP idSrc; // 源数据库ID配置

map<string,string> changed_player_name;
map<ULONG,ULONG> changed_player_id;
map<ULONG,ULONG> changed_faction_id;

clock_t begin_time,total_time;

BYTE num;
VARIANT_BOOL bEof;

HRESULT hr;
_ConnectionPtr cnDes;
_ConnectionPtr cnSrc;

_ConnectionPtr logincnDes;
_ConnectionPtr logincnSrc;


int main(int argc, char *argv[]) {
	// 初始ADO
	::CoInitialize(NULL);
	srand(static_cast<unsigned int>(time(NULL)));

	// 读取配置文件
	if(argc >= 2) { // 从console读取
		sprintf(msg, argv[1]);
	}
	else { // 默认配置文件
		strcpy(msg,"merger2.ini");
	}

	if( !LoadConf(msg) )
	{
		printf("Config file not found!\n");
		exit(1);
	}



	MakeGameConnStr();
	MakeLoginConnStr();

	// connecte string initialized
	// 创建数据库连接对象
	hr = cnDes.CreateInstance(__uuidof(Connection));
	if(FAILED(hr)) exit(1);
	hr = cnSrc.CreateInstance(__uuidof(Connection));
	if(FAILED(hr)) exit(1);
	// 创建数据库连接对象
	hr = logincnDes.CreateInstance(__uuidof(Connection));
	if(FAILED(hr)) exit(1);
	hr = logincnSrc.CreateInstance(__uuidof(Connection));
	if(FAILED(hr)) exit(1);

	// 设置查询超时
	cnDes->CommandTimeout = 1800;
	cnSrc->CommandTimeout = 1800;
	logincnDes->CommandTimeout = 1800;
	logincnSrc->CommandTimeout = 1800;


	time_t t = time(NULL);
	tm *n = localtime(&t);

	// main programer strating
	sprintf(msg, "mearge-%s-%s-%02d%02d.log", sqlDes.Database.data(), sqlSrc.Database.data(), n->tm_mday, n->tm_min);
	file = fopen(msg, "a+"); // 打开记录日志
	
	sprintf(msg, "Account-%s-%s-%02d%02d.log", sqlDes.Database.data(), sqlSrc.Database.data(), n->tm_mday, n->tm_min);
	fAC = fopen(msg, "a+"); 

	sprintf(msg, "Playername-%s-%s-%02d%02d.log", sqlDes.Database.data(), sqlSrc.Database.data(), n->tm_mday, n->tm_min);
	fPNC = fopen(msg, "a+"); 


	if(!file) printf("操作记录日志未启动!\r\n");
	if(!fAC) printf("操作记录日志未启动!\r\n");
	if(!fPNC) printf("操作记录日志未启动!\r\n");

	LogChanged(fAC, "<Begin>\n");
	LogChanged(fPNC, "<Begin>\n");

	Log("<Begin>\n");

	sprintf(msg, "<info>\t目标数据库:%s %s\n", sqlDes.Server.data(), sqlDes.Database.data());
	Log(msg);
	sprintf(msg, "<info>\t源数据库:%s %s\n", sqlSrc.Server.data(), sqlSrc.Database.data());
	Log(msg);


	//profix
	sprintf(msg, "<info>\t重名修正前缀和特殊字符:%s%s\tWORLDID%d\n",profix.data(),spechar.data(),nWorldID);
	Log(msg);


	_RecordsetPtr rs;

	try {
		// 打开数据库连接
		hr = cnDes->Open(cnstrDes.data(), "", "", adConnectUnspecified);
		if(FAILED(hr)) _com_issue_error(E_FAIL);
		hr = cnSrc->Open(cnstrSrc.data(), "", "", adConnectUnspecified);
		if(FAILED(hr)) _com_issue_error(E_FAIL);

		hr = rs.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)) _com_issue_error(E_FAIL);


		total_time = begin_time = clock();

		Log("<info>\t载入目的数据库CSL_SETUP表值\n");
		// 初始Des的ID_SETUP
		hr = rs->Open("SELECT TOP 1 * FROM CSL_SETUP", cnDes.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);
		if(rs->GetRSEOF()) {
			printf("fetal:[ Des ] no record in setup table\r\n");
			Log("<fetal>\t请检查目标数据库SETUP表是否被初始化！\n");
			_com_issue_error(E_FAIL);
		}

		idDes.PlayerID = (long)rs->GetCollect("PlayerID");
		idDes.OrganizingID = (long)rs->GetCollect("OrganizingID");
		idDes.LeavewordID =  (long)rs->GetCollect("LeaveWordID");
		rs->Close();


		sprintf(msg, "<setup>\tPlayerID\t= %d\n", idDes.PlayerID);
		Log(msg);
		sprintf(msg, "<setup>\tOrganizingID\t= %d\n",idDes.OrganizingID);
		Log(msg);
		sprintf(msg, "<setup>\tLeavewordID\t= %d\n",idDes.LeavewordID);
		Log(msg);


		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);


		////////////////////////////////////////////////////////////////////////////////////////
		//
		// 处理把角色从Src到Des  CSL_Country
		//
		////////////////////////////////////////////////////////////////////////////////////////
		Log("<info>\t处理国家数据开始...\n");

		hr = rs->Open("SELECT * FROM CSL_Countrys" ,
			cnSrc.GetInterfacePtr(),  adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);

		while( !rs->GetRSEOF() ) {
			_RecordsetPtr rs11;
			try {
				hr = rs11.CreateInstance(__uuidof(Recordset));
				if(FAILED(hr)) _com_issue_error(E_FAIL);

				sprintf(sql, "SELECT TOP 1 * FROM CSL_Countrys WHERE id=%d ORDER BY id",(long)rs->GetCollect("id"));
				hr = rs11->Open(sql, cnDes.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
				if(FAILED(hr)) _com_issue_error(E_FAIL);
				
				if( !rs11->GetRSEOF() )
				{
					sprintf(sql, "UPDATE CSL_Countrys SET power=%d, tech_level=%d WHERE id=%d",
						(long)rs->GetCollect("power") + (long) rs11->GetCollect("power") ,
						(long)rs->GetCollect("tech_level") + (long) rs11->GetCollect("tech_level"),
						(long)rs11->GetCollect("ID"));
				}
				else
				{
					sprintf(sql, "INSERT INTO CSL_Countrys VALUES(%d,%d,%d,0)",
						(long)rs->GetCollect("ID") ,
						(long)rs->GetCollect("power"),
						(long)rs->GetCollect("tech_level"));
				}
				cnDes->Execute(sql, NULL, adCmdText);

				rs11->Close();
			}
			catch(_com_error &e) {
				sprintf( msg,"<ado err>\t数据库异常(while cournty): %s\n", static_cast<const char*>(e.Description()) );
				Log(msg);
			}
			rs11.Release();
			rs->MoveNext();
		}
		rs->Close();

		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);

		////////////////////////////////////////////////////////////////////////////////////////
		//
		// 处理把角色从Src到Des  CSL_PALYER_BASE
		//
		////////////////////////////////////////////////////////////////////////////////////////

		Log("<info>\t处理玩家基本数据开始...\n");
				
		hr = rs->Open("SELECT * FROM CSL_PLAYER_BASE ORDER BY ID", cnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);

		while( !rs->GetRSEOF() ) {

			_RecordsetPtr rs11;

			ULONG old_playerid = 0;
			ULONG new_playerid = 0;
			char old_playername[32]="";
			char new_playername[32]="";

			try {

				hr = rs11.CreateInstance(__uuidof(Recordset));
				if(FAILED(hr)) _com_issue_error(E_FAIL);

				// 这里重新排列玩家ID
				old_playerid = (long)rs->GetCollect("ID");
				new_playerid = ++idDes.PlayerID;
				changed_player_id[old_playerid] = new_playerid;

				// 获取Name
				strcpy(old_playername, (const char*)(_bstr_t)(rs->GetCollect("Name")) );

				sprintf(sql, "SELECT TOP 1 Name FROM CSL_PLAYER_BASE WHERE Name='%s' ORDER BY name", old_playername);
				rs11->Open(sql, cnDes.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
				bEof = rs11->GetRSEOF();
				rs11->Close();

				if(!bEof) { //有重复的NAME
					FixName(new_playername, old_playername);
					do {
						sprintf(sql, "SELECT TOP 1 Name FROM CSL_PLAYER_BASE WHERE Name='%s' ORDER BY id", new_playername);
						hr = rs11->Open(sql, cnDes.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
						if(FAILED(hr)) _com_issue_error(E_FAIL);

						bEof = rs11->GetRSEOF();
						rs11->Close();
						
						if(!bEof) {
							new_playername[0] = 0;
							FixName(new_playername, old_playername, true);
						}
					}
					while(!bEof);

					sprintf(msg,"<record>\t 原始玩家名:[%s] 修正玩家名:[%s]\n", old_playername, new_playername);
					Log(msg);
					sprintf(msg,"%s,%s\n", old_playername, new_playername);
					LogChanged(fPNC,msg);
				}
				else { // 没有重复的NAME
					strcpy(new_playername, old_playername);
				}
				
				changed_player_name[old_playername] = new_playername;

				// 删除时间
				_variant_t var = rs->GetCollect("DelDate");
				char delDate[20] = "";
				if( var.vt != VT_NULL ) {
					GetDateString(var, delDate);
				}

				if( strcmp(delDate,"1900-1-1") ) {
				sprintf( sql,"INSERT INTO CSL_PLAYER_BASE \
							VALUES(%d,'%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s')",
							new_playerid,
							new_playername,
							(const char*)(_bstr_t)rs->GetCollect("Account"),
							(long)rs->GetCollect("Levels"),
							(long)rs->GetCollect("Occupation"),
							(long)rs->GetCollect("Sex"),
							(long)rs->GetCollect("Country"),

							(long)rs->GetCollect("HEAD"),

							(long)rs->GetCollect("HELM"),
							(long)rs->GetCollect("BODY"),
							(long)rs->GetCollect("GLOV"),
							(long)rs->GetCollect("BOOT"),
							(long)rs->GetCollect("WEAPON"),
							(long)rs->GetCollect("BACK"),
							(long)rs->GetCollect("HelmLevel"),
							(long)rs->GetCollect("BodyLevel"),
							(long)rs->GetCollect("GlovLevel"),
							(long)rs->GetCollect("BootLevel"),
							(long)rs->GetCollect("WeaponLevel"),
							(long)rs->GetCollect("BackLevel"),

							(long)rs->GetCollect("Region"),
							delDate );
				}
				else {
				sprintf( sql,"INSERT INTO CSL_PLAYER_BASE \
							VALUES(%d,'%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
							new_playerid,
							new_playername,
							(const char*)(_bstr_t)rs->GetCollect("Account"),
							(long)rs->GetCollect("Levels"),
							(long)rs->GetCollect("Occupation"),
							(long)rs->GetCollect("Sex"),
							(long)rs->GetCollect("Country"),
							(long)rs->GetCollect("HEAD"),
							(long)rs->GetCollect("HELM"),
							(long)rs->GetCollect("BODY"),
							(long)rs->GetCollect("GLOV"),
							(long)rs->GetCollect("BOOT"),
							(long)rs->GetCollect("WEAPON"),
							(long)rs->GetCollect("BACK"),
							(long)rs->GetCollect("HelmLevel"),
							(long)rs->GetCollect("BodyLevel"),
							(long)rs->GetCollect("GlovLevel"),
							(long)rs->GetCollect("BootLevel"),
							(long)rs->GetCollect("WeaponLevel"),
							(long)rs->GetCollect("BackLevel"),
							(long)rs->GetCollect("Region") );
				}


				cnDes->Execute(sql, NULL, adCmdText);

			} // end try
			catch(_com_error &e) {
				sprintf( msg,"<ado err>\t数据库异常(for player base): %s - 玩家:旧[%d] 新[%d] 旧[%s] 新[%s]\n", 
							 static_cast<const char*>(e.Description()), 
							 old_playerid, new_playerid,old_playername, new_playername);
				Log(msg);
			}

			rs11.Release();
			rs->MoveNext();
		} // end while
		rs->Close();

		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);



		////////////////////////////////////////////////////////////////////////////////////////
		//
		// 处理把角色从Src到Des  CSL_PALYER_ABILITY
		//
		////////////////////////////////////////////////////////////////////////////////////////
		Log("<info>\t处理玩家详细数据开始...\n");
		hr = rs->Open( "SELECT * FROM CSL_PLAYER_ABILITY ORDER BY ID", cnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);
		while(!rs->GetRSEOF()) {

			_RecordsetPtr rs11;

			ULONG old_playerid = 0;
			ULONG new_playerid = 0;
			char old_playername[32]="";
			char new_playername[32]="";

			try {

				hr = rs11.CreateInstance(__uuidof(Recordset));
				if(FAILED(hr)) _com_issue_error(E_FAIL);

				// 获取ID
				old_playerid = (long)rs->GetCollect("ID");
				new_playerid = changed_player_id[old_playerid];

				// 仅存在ability里。
				if(!new_playerid) {
					sprintf(msg,"<warrning>\t角色ID[%d]仅存在Player_ability中\n",old_playerid);
					Log(msg);
					rs->MoveNext();
					continue;
				}

				string temp;
				// 获取NAME
				strcpy( old_playername, (const char *)(_bstr_t)rs->GetCollect("Name") );
				temp = changed_player_name[old_playername];
				strcpy( new_playername, temp.data() );

				_variant_t var = rs->GetCollect("SaveTime");
				char saveDate[20] = "";
				if( var.vt != VT_NULL ) {
					GetDateString(var, saveDate);
				}

				var = rs->GetCollect("depotpassword");
				char depostpassword[20] = "";
				if( var.vt != VT_NULL ) {
					strcpy(depostpassword,(const char*)(_bstr_t)rs->GetCollect("depotpassword"));
				}

				sprintf(sql, "INSERT INTO CSL_PLAYER_ABILITY(ID,Name,SaveTime,RegionID,PosX,PosY,Dir,Account,Title,Levels,Exp,\
							 HeadPic,FacePic,Occupation,Sex,SpouseID,MurdererTime,PkCount,KillCount,HitTopLog,\
							 LoanMax,Loan,LoanTime,Pk_Normal,Pk_Team,Pk_Union,Pk_Badman,\
							 Yp,Hp,Mp,Rp,BaseMaxHp,BaseMaxMp,BaseMaxYp,BaseMaxRp,BaseStr,BaseDex,\
							 BaseCon,BaseInt,BaseMinAtk,BaseMaxAtk,BaseHit,BaseBurden,BaseCCH,BaseDef,BaseDodge,\
							 BaseAtcSpeed,BaseElementResistant,BaseHpRecoverSpeed,BaseMpRecoverSpeed,silence, \
							 UnionId,Money,RemainPoint,Tribe,BaseVigour,BaseMaxVigour,BaseCredit,DisplayHeadPiece, \
							 country, contribute, IsCharged, QuestTimeBegin, QuestTimeLimit, Quest, depotpassword) \
							 VALUES(%d,'%s','%s',%d,%f,%f,%d,'%s','temptitle',%d,%u,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\
							 %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,0,0,0,0,%d,%d,%d,%d,\
							 %d,%d,%d,%d,%d,%d,'%s')",
							 new_playerid,new_playername, saveDate, (long)rs->GetCollect("RegionID"),
							 (float)rs->GetCollect("PosX"),(float)rs->GetCollect("PosY"),(long)rs->GetCollect("Dir"),(const char*)(bstr_t)rs->GetCollect("Account"),
							 (long)rs->GetCollect("Levels"),(unsigned long)rs->GetCollect("Exp"),(long)rs->GetCollect("HeadPic"),(long)rs->GetCollect("FacePic"),
							 (long)rs->GetCollect("Occupation"),(long)rs->GetCollect("Sex"),(long)rs->GetCollect("SpouseID"),(long)rs->GetCollect("MurdererTime"),(long)rs->GetCollect("PkCount"),
							 (long)rs->GetCollect("KillCount"),(long)rs->GetCollect("HitTopLog"),
							 (long)rs->GetCollect("LoanMax"),(long)rs->GetCollect("Loan"),
							 (unsigned long)rs->GetCollect("LoanTime"),
							 (long)rs->GetCollect("Pk_Normal"),(long)rs->GetCollect("Pk_Team"),
							 (long)rs->GetCollect("Pk_Union"),(long)rs->GetCollect("Pk_Badman"),(long)rs->GetCollect("Yp"),(long)rs->GetCollect("Hp"),
							 (long)rs->GetCollect("Mp"),(long)rs->GetCollect("Rp"),(long)rs->GetCollect("BaseMaxHp"),(long)rs->GetCollect("BaseMaxMp"),
							 (long)rs->GetCollect("BaseMaxYp"),(long)rs->GetCollect("BaseMaxRp"),(long)rs->GetCollect("BaseStr"),(long)rs->GetCollect("BaseDex"),
							 (long)rs->GetCollect("BaseCon"),(long)rs->GetCollect("BaseInt"),(long)rs->GetCollect("BaseMinAtk"),(long)rs->GetCollect("BaseMaxAtk"),
							 (long)rs->GetCollect("BaseHit"),(long)rs->GetCollect("BaseBurden"),(long)rs->GetCollect("BaseCCH"),(long)rs->GetCollect("BaseDef"),
							 (long)rs->GetCollect("BaseDodge"),(long)rs->GetCollect("BaseAtcSpeed"),(long)rs->GetCollect("BaseElementResistant"),
							 (long)rs->GetCollect("BaseHpRecoverSpeed"),(long)rs->GetCollect("BaseMpRecoverSpeed"),
							 (long)rs->GetCollect("silence"),
							 (long)rs->GetCollect("BaseVigour"),(long)rs->GetCollect("BaseMaxVigour"),(long)rs->GetCollect("BaseCredit"),(long)rs->GetCollect("DisplayHeadPiece"),
							 (long)rs->GetCollect("country"),(long)rs->GetCollect("contribute"),(long)rs->GetCollect("IsCharged"),
							 (unsigned long)rs->GetCollect("QuestTimeBegin"),(long)rs->GetCollect("QuestTimeLimit"),(long)rs->GetCollect("Quest"),
							 depostpassword);


				cnDes->Execute(sql, NULL, adCmdText);

				sprintf(sql, "SELECT HotKey,ListSkill,ListState,ListFriendName,VariableList FROM CSL_PLAYER_ABILITY WHERE ID=%d ORDER BY ID", new_playerid);
				rs11->Open(sql, cnDes.GetInterfacePtr(), adOpenStatic, adLockOptimistic, adCmdText);

				if(!CopyChunk(rs11, rs, "HotKey")) {
					rs11->Update();
					rs11->Close();
					_com_issue_error(E_FAIL);
				}
				if(!CopyChunk(rs11, rs, "ListSkill")) {
					rs11->Update();
					rs11->Close();
					_com_issue_error(E_FAIL);
				}
				if(!CopyChunk(rs11, rs, "ListState")) {
					rs11->Update();
					rs11->Close();
					_com_issue_error(E_FAIL);
				}
				if(!CopyChunk(rs11, rs, "ListFriendName")) {
					rs11->Update();
					rs11->Close();
					_com_issue_error(E_FAIL);
				}

				if(!CopyChunk(rs11, rs, "VariableList")) {
					rs11->Update();
					rs11->Close();
					_com_issue_error(E_FAIL);
				}
			
				rs11->Update();
				rs11->Close();
			
			} // end try
			catch(_com_error &e) {
				sprintf( msg,"<ado err>\t数据库异常(for player ability): %s - 玩家:旧[%d] 新[%d] 旧[%s] 新[%s]\n", 
							 static_cast<const char*>(e.Description()), 
							 old_playerid, new_playerid,old_playername, new_playername);
				Log(msg);
			}
			rs11.Release();
			rs->MoveNext();
		} // end while
		rs->Close();

		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);


		
		////////////////////////////////////////////////////////////////////////////////////////
		//
		// 处理该玩家的任务 csl_player_quest_ex
		//
		////////////////////////////////////////////////////////////////////////////////////////
		Log("<info>\t处理玩家任务开始...\n");
		hr = rs->Open("SELECT * FROM csl_player_quest_ex ORDER BY playerID", cnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);
		while(!rs->GetRSEOF()) {
			
			_RecordsetPtr rs11;

			ULONG old_playerid = 0;
			ULONG new_playerid = 0;
			try {

				hr = rs11.CreateInstance(__uuidof(Recordset));
				if(FAILED(hr)) _com_issue_error(E_FAIL);

				// 获取ID
				old_playerid = (long)rs->GetCollect("playerID");
				new_playerid = changed_player_id[old_playerid];

				// 仅存在Player_quest_ex里。
				if(!new_playerid) {
					sprintf(msg,"<warrning>\t角色ID[%d]仅存在Player_quest_ex中\n",old_playerid);
					Log(msg);
					rs->MoveNext();
					continue;
				}

				sprintf(sql, "INSERT INTO csl_player_quest_ex(playerID) VALUES(%d)", new_playerid);
				cnDes->Execute(sql, NULL, adCmdText);

				sprintf(sql, "SELECT QuestData FROM csl_player_quest_ex WHERE playerID=%d ORDER BY playerID", new_playerid);
				rs11->Open(sql, cnDes.GetInterfacePtr(), adOpenStatic, adLockOptimistic, adCmdText);

				if(!CopyChunk(rs11, rs, "QuestData")) {
					rs11->Update();
					rs11->Close();
					_com_issue_error(E_FAIL);
				}

				rs11->Update();
				rs11->Close();
			
			} // end try
			catch(_com_error &e) {
				sprintf( msg,"<ado err>\t数据库异常(for player quest_ex): %s - 玩家:旧[%d] 新[%d]\n", 
							 static_cast<const char*>(e.Description()), old_playerid, new_playerid);
				Log(msg);
			}
			rs11.Release();
			rs->MoveNext();
		} // end while
		rs->Close();

		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);

		////////////////////////////////////////////////////////////////////////////////////////
		//
		// 处理该玩家的物品 player_goods
		//
		////////////////////////////////////////////////////////////////////////////////////////
		Log("<info>\t处理玩家物品数据开始...\n");
		hr = rs->Open("SELECT * FROM player_goods ORDER BY playerID", cnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);
		while(!rs->GetRSEOF()) { //无记录就是没物品
	
			ULONG old_playerid=0, new_playerid=0;
			
			try {
				// 获取ID
				old_playerid = (long)rs->GetCollect("playerID");
				new_playerid = changed_player_id[old_playerid];

				if(old_playerid != 0 && new_playerid == 0) { // 不为0才是玩家身上的物品，需要看他是否在野，
					sprintf(msg,"<warrning>\t角色ID[%d]数据不足(playerid)，无法复制.在player_goods中\n",old_playerid);
					Log(msg);
					rs->MoveNext();
					continue;
				}

				sprintf(sql, "INSERT INTO player_goods \
							 VALUES('%s','%s',%d,%d,'%s',%d,%d,%d,%d)",
							 (const char*)(_bstr_t)rs->GetCollect("ID"),
							 (const char*)(_bstr_t)rs->GetCollect("goodsID"),(long)rs->GetCollect("goodsIndex"),
							 new_playerid,(const char*)(_bstr_t)rs->GetCollect("name"),
							 (long)rs->GetCollect("price"),(long)rs->GetCollect("amount"),
							 (long)rs->GetCollect("place"),(long)rs->GetCollect("position"));

				cnDes->Execute(sql, NULL, adCmdText);

			} // end try
			catch(_com_error &e) {
				sprintf( msg,"<ado err>\t数据库异常(for player goods): %s\n", static_cast<const char*>(e.Description()) );
				Log(msg);
			}

			rs->MoveNext();
		} // end while循环物品
		rs->Close();
		// 该角色下的物品合并完

		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);

		////////////////////////////////////////////////////////////////////////////////////////
		//
		// 物品属性
		//
		////////////////////////////////////////////////////////////////////////////////////////
		Log("<info>\t处理玩家物品属性数据开始...\n");
		hr = rs->Open("SELECT * FROM extend_properties ORDER BY id", cnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);

		while(!rs->GetRSEOF()) { //无记录就是没属性
			try
			{
				sprintf(sql, "INSERT INTO extend_properties(type,modifierValue1,modifierValue2,ID) \
							 VALUES(%d,%d,%d,'%s')",
							 (long)rs->GetCollect("type"),
							 (long)rs->GetCollect("modifierValue1"),
							 (long)rs->GetCollect("modifierValue2"),
							 (const char*)(_bstr_t)rs->GetCollect("ID") );

				cnDes->Execute(sql, NULL, adCmdText);
				
			} // end try
			catch(_com_error &e) {
				sprintf( msg,"<ado err>\t数据库异常(for player goods propertis):%s\n", static_cast<const char*>(e.Description()) );
				Log(msg);
			}
			rs->MoveNext();
		}
		rs->Close();


		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);


		////////////////////////////////////////////////////////////////////////////////////////
		//
		// 处理帮会
		// 
		////////////////////////////////////////////////////////////////////////////////////////
		Log("<info>\t处理帮会数据开始...\n");

		// 首先处理 CSL_FACTION_BaseProperty
		hr = rs->Open("SELECT * FROM CSL_FACTION_BaseProperty", cnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);

		Log("<info>\tfaction_baseproperty开始...\n");
		while( !rs->GetRSEOF() ) {
	
			_RecordsetPtr rs11;
			_RecordsetPtr rs12;
			ULONG old_factionid = 0;
			ULONG new_factionid = 0;
			char old_factionname[32] = "";
			char new_factionname[32] = "";
			ULONG old_playerid = 0;
			ULONG new_playerid = 0;

			try {
				hr = rs11.CreateInstance(__uuidof(Recordset));
				if(FAILED(hr)) _com_issue_error(E_FAIL);

				hr = rs12.CreateInstance(__uuidof(Recordset));
				if(FAILED(hr)) _com_issue_error(E_FAIL);
				// 获取刚才更新后的玩家ID
				old_playerid = (long)rs->GetCollect("MasterID");
				new_playerid = changed_player_id[old_playerid];

				if(!new_playerid) {
					sprintf(msg,"<warrning>\t角色ID[%d]数据不足，无法复制.在CSL_FACTION_BaseProperty中\n",old_playerid);
					Log(msg);
					rs->MoveNext();
					continue;
				}


				// 获取ID和NMAE
				old_factionid = (long)rs->GetCollect("ID");
				strcpy( old_factionname, (const char *)(_bstr_t)rs->GetCollect("Name") );
				// 获取新的ID和NAME
				new_factionid = ++idDes.OrganizingID;

				sprintf(sql, "SELECT TOP 1 Name FROM CSL_FACTION_BaseProperty WHERE Name='%s'",old_factionname);	
				hr = rs11->Open(sql, cnDes.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
				if(FAILED(hr)) _com_issue_error(E_FAIL);

				bEof = rs11->GetRSEOF();
				rs11->Close();

				if( !bEof ) { //名字存在
					FixName(new_factionname, old_factionname);

					do {
						sprintf(sql, "SELECT TOP 1 Name FROM CSL_FACTION_BaseProperty WHERE Name='%s'", new_factionname);
						hr = rs11->Open(sql, cnDes.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
						if(FAILED(hr)) _com_issue_error(E_FAIL);

						bEof = rs11->GetRSEOF();
						rs11->Close();

						if(!bEof) {
							new_factionname[0] = 0;
							FixName(new_factionname, old_factionname, true);
						}
					}
					while(!bEof);

					sprintf(msg, "<record>\t原始帮会名:[%s] 修正帮会名:[%s]\n", old_factionname, new_factionname);
					Log(msg);
				}
				else { // 无重复名，直接用 
					strcpy(new_factionname, old_factionname);
				}
				

				char date[20] = "";
				_variant_t var = rs->GetCollect("EstablishedTime");
				if( var.vt != VT_NULL ) {
					GetDateString(var, date);
				}


				sprintf(sql, "INSERT INTO \
							 CSL_FACTION_BaseProperty(ID,Name,MasterID,Levels,Experience,MemberNums,\
							 VillageWarVictorCounts,OffenseVictorCounts,DefenceVictorCounts,bPermit,\
							 lPro1,lPro2,DelRemainTime,EstablishedTime,Country) \
							 VALUES(%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d)",
							 new_factionid,new_factionname,new_playerid,(long)rs->GetCollect("Levels"),
							 (long)rs->GetCollect("Experience"),							 
							 (long)rs->GetCollect("MemberNums"),(long)rs->GetCollect("VillageWarVictorCounts"),
							 (long)rs->GetCollect("OffenseVictorCounts"),(long)rs->GetCollect("DefenceVictorCounts"),
							 (long)rs->GetCollect("bPermit"),(long)rs->GetCollect("lPro1"),
							 (long)rs->GetCollect("lPro2"),(long)rs->GetCollect("DelRemainTime"), date ,
							 (long)rs->GetCollect("Country"));

				cnDes->Execute(sql, NULL, -1);


				////////////////////////////////////////////////////////////////////////////////////////
				//
				// 处理该角色的帮会 CSL_FACTION_Ability
				//
				////////////////////////////////////////////////////////////////////////////////////////

				sprintf(sql, "SELECT * FROM CSL_FACTION_Ability WHERE FactionID=%d", old_factionid);
				hr = rs11->Open(sql, cnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
				if(FAILED(hr)) _com_issue_error(E_FAIL);

				if( !rs11->GetRSEOF() ) { // 该玩家是某帮的帮主，合并他的帮会属性

					char date[20] = "";
					_variant_t var = rs11->GetCollect("LastUploadIconDataTime");
					if( var.vt != VT_NULL ) {
						GetDateString(var, date);
					}

					sprintf(sql, "INSERT INTO \
								 CSL_FACTION_Ability(FactionID,LastUploadIconDataTime,EnemyOrganizingID) \
								 VALUES(%d,'%s',%d)",
								 new_factionid,date,0 );

					cnDes->Execute(sql, NULL, -1);


					sprintf(sql, "SELECT * FROM CSL_FACTION_Ability WHERE FactionID=%d", new_factionid);
					rs12->Open(sql, cnDes.GetInterfacePtr(), adOpenStatic, adLockOptimistic, adCmdText);

					if(!CopyChunk(rs12, rs11, "Pronounce")) {
						rs11->Close();
						rs12->Update();
						rs12->Close();
						_com_issue_error(E_FAIL);
					}

					if(!CopyChunk(rs12, rs11, "IconData")) {
						rs11->Close();
						rs12->Update();
						rs12->Close();
						_com_issue_error(E_FAIL);
					}

					rs11->Close();
					rs12->Update();
					rs12->Close();
				}
				else {
					sprintf(msg, "<error>\t 帮会[%d][%s]在ability表中无对应记录\n", old_factionid, old_factionname);
					Log(msg);
				}


				// 添加到更改表。
				changed_faction_id[old_factionid] = new_factionid;
			}
			catch(_com_error &e) {

				sprintf( msg,"<ado err>\t数据库异常(while faction): %s\n", static_cast<const char*>(e.Description()) );
				Log(msg);
			}
			rs->MoveNext();
			rs11.Release();
			rs12.Release();

		} // end while faction
		rs->Close();

		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);


		////////////////////////////////////////////////////////////////////////////////////////
		// 根据changed_player_id 和 changed_faction_id
		// 合并 CSL_FACTION_Members
		//
		////////////////////////////////////////////////////////////////////////////////////////

		Log("<info>\t处理帮会成员开始...\n");
		hr = rs->Open("SELECT * FROM CSL_FACTION_Members ORDER BY FactionID", cnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);
		
		Log("<info>\tfaction_member开始...\n");

		while( !rs->GetRSEOF() ) { // 该玩家是某帮的成员，合并他的帮到DES
			ULONG old_factionid = 0;
			ULONG new_factionid = 0;
			ULONG old_playerid = 0;
			ULONG new_playerid = 0;
			char date[20] = "";
			try {
				
				//

				old_factionid = (long)rs->GetCollect("FactionID");
				old_playerid = (long)rs->GetCollect("PlayerID");

				
				new_factionid = changed_faction_id[old_factionid];
				new_playerid = changed_player_id[old_playerid];


				
				if(!new_playerid) {
					sprintf(msg,"<warrning>\t角色ID[%d]数据不足，无法复制.在CSL_FACTION_Members中\n",old_playerid);
					Log(msg);
					rs->MoveNext();
					continue;
				}


				_variant_t var = rs->GetCollect("LastOnlineTime");
				if( var.vt != VT_NULL ) {
					GetDateString(var, date);
				}

				sprintf(sql, 
					"INSERT INTO CSL_FACTION_Members \
					VALUES(%d,%d,%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s')",
					new_factionid,new_playerid,(long)rs->GetCollect("MemberLvl"),(const char*)(_bstr_t)rs->GetCollect("Title"),
					(long)rs->GetCollect("bControbute"),(long)rs->GetCollect("PV_Disband"),(long)rs->GetCollect("PV_Exit"),
					(long)rs->GetCollect("PV_DubJobLvl"),(long)rs->GetCollect("PV_ConMem"),(long)rs->GetCollect("PV_FireOut"),
					(long)rs->GetCollect("PV_Pronounce"),(long)rs->GetCollect("PV_LeaveWord"),(long)rs->GetCollect("PV_EditLeaveWord"),
					(long)rs->GetCollect("PV_ObtainTax"),(long)rs->GetCollect("PV_OperCityGate"),(long)rs->GetCollect("PV_EndueROR"),
					date);

				cnDes->Execute(sql, NULL, -1);

			}
			catch(_com_error &e) {

				sprintf( msg,"<ado err>\t数据库异常(while membsers faction): %s\n", static_cast<const char*>(e.Description()) );
				Log(msg);
			}
			rs->MoveNext();
		} // end while CSL_FACTION_Members
		rs->Close();
		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);


		////////////////////////////////////////////////////////////////////////////////////////
		// 
		// 处理联盟 CSL_UNION_BaseProperty
		// 
		////////////////////////////////////////////////////////////////////////////////////////

		Log("<info>\t处理联盟数据开始...\n");
		hr = rs->Open("SELECT * FROM CSL_UNION_BaseProperty ORDER BY ID", cnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);
		while( !rs->GetRSEOF() ) {
		
			_RecordsetPtr rs11;

			ULONG old_factionid = 0;
			ULONG new_factionid = 0;
			ULONG old_unionid = 0;
			ULONG new_unionid = 0;
			char old_unionname[32] = "";
			char new_unionname[32] = "";

			try {
				hr = rs11.CreateInstance(__uuidof(Recordset));
				if(FAILED(hr)) _com_issue_error(E_FAIL);


				// 获取ID和NMAE
				old_unionid = (long)rs->GetCollect("ID");
				strcpy( old_unionname, (const char *)(_bstr_t)rs->GetCollect("Name") );
				// 获取新的ID和NAME
				new_unionid = ++idDes.OrganizingID;

				sprintf(sql, "SELECT TOP 1 Name FROM CSL_UNION_BaseProperty WHERE Name='%s'",old_unionname);	
				hr = rs11->Open(sql, cnDes.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
				if(FAILED(hr)) _com_issue_error(E_FAIL);
				bEof = rs11->GetRSEOF();
				rs11->Close();

				if( !bEof ) { //名字存在
					FixName(new_unionname, old_unionname);

					do {
						sprintf(sql, "SELECT TOP 1 Name FROM CSL_UNION_BaseProperty WHERE Name='%s'", new_unionname);
						hr = rs11->Open(sql, cnDes.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
						if(FAILED(hr)) _com_issue_error(E_FAIL);

						bEof = rs11->GetRSEOF();
						rs11->Close();

						if(!bEof) {
							new_unionname[0] = 0;
							FixName(new_unionname, old_unionname ,true);
						}
					}
					while(!bEof);

					sprintf(msg, "<record>\t原始联盟名:[%s] 修正联盟名:[%s]\n", old_unionname, new_unionname);
					Log(msg);
				}
				else { // 无重复名，直接用 
					strcpy(new_unionname, old_unionname);
				}
				

				// 获取刚才更新后的玩家ID
				old_factionid = (long)rs->GetCollect("MasterID");
				new_factionid = changed_faction_id[old_factionid];


				sprintf(sql, "INSERT INTO \
							 CSL_UNION_BaseProperty(ID,Name,MasterID) \
							 VALUES(%d,'%s',%d)",
							 new_unionid,new_unionname,new_factionid);
				cnDes->Execute(sql, NULL, -1);

				// 处理完联盟

				// 添加到更改表。
				changed_faction_id[old_unionid] = new_unionid; // 帮会和联盟ID不会相同，都是从orgID里生成的
			}
			catch(_com_error &e) {
				sprintf( msg,"<ado err>\t数据库异常(while union): %s\n", static_cast<const char*>(e.Description()) );
				Log(msg);
			}
			rs->MoveNext();
			rs11.Release();

		} // end while union
		rs->Close();

		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);


		////////////////////////////////////////////////////////////////////////////////////////
		// 
		// 处理联盟 CSL_UNION_Members
		// 
		////////////////////////////////////////////////////////////////////////////////////////
		Log("<info>\t处理联盟成员开始...\n");
		hr = rs->Open("SELECT * FROM CSL_UNION_Members ORDER BY UnionID", cnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);

		while( !rs->GetRSEOF() ) { // 该玩家是某帮的成员，合并他的帮到DES
			ULONG old_unionid = 0;
			ULONG new_unionid = 0;
			ULONG old_factionid = 0;
			ULONG new_factionid = 0;

			char date[20] = "";
			try {
				old_unionid = (long)rs->GetCollect("UnionID");
				old_factionid = (long)rs->GetCollect("FactionID");
				
				_variant_t var = rs->GetCollect("LastOnlineTime");
				if( var.vt != VT_NULL ) {
					GetDateString(var, date);
				}

				new_unionid = changed_faction_id[old_unionid];
				new_factionid = changed_faction_id[old_factionid];

				sprintf(sql, 
					"INSERT INTO CSL_UNION_Members \
					VALUES(%d,%d,%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s')",
					new_unionid,new_factionid,(long)rs->GetCollect("MemberLvl"),(const char*)(_bstr_t)rs->GetCollect("Title"),
					(long)rs->GetCollect("bControbute"),(long)rs->GetCollect("PV_Disband"),(long)rs->GetCollect("PV_Exit"),
					(long)rs->GetCollect("PV_DubJobLvl"),(long)rs->GetCollect("PV_ConMem"),(long)rs->GetCollect("PV_FireOut"),
					(long)rs->GetCollect("PV_Pronounce"),(long)rs->GetCollect("PV_LeaveWord"),(long)rs->GetCollect("PV_EditLeaveWord"),
					(long)rs->GetCollect("PV_ObtainTax"),(long)rs->GetCollect("PV_OperCityGate"),(long)rs->GetCollect("PV_EndueROR"),
					date);

				cnDes->Execute(sql, NULL, -1);
				// 处理完帮会
			}
			catch(_com_error &e) {
				sprintf( msg,"<ado err>\t数据库异常(while members union): %s\n", static_cast<const char*>(e.Description()) );
				Log(msg);
			}
			rs->MoveNext();
		} // end while CSL_FACTION_Members
		rs->Close();
		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);




		////////////////////////////////////////////////////////////////////////////////////////
		// 
		// 处理 CSL_setup
		// 
		////////////////////////////////////////////////////////////////////////////////////////
		Log("<info>\t更新Csl_setup\n");
		// 最后写入SETUP表
		sprintf(sql,
			"UPDATE CSL_SETUP SET PlayerID=%d,OrganizingID=%d,LeaveWordID=%d",
			idDes.PlayerID,
			idDes.OrganizingID,
			idDes.LeavewordID);
		cnDes->Execute(sql, NULL, -1);

		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );

		Log(msg);

		sprintf(msg, "<setup>\tPlayerID\t= %d\n", idDes.PlayerID);
		Log(msg);
		sprintf(msg, "<setup>\tOrganizingID\t= %d\n",idDes.OrganizingID);
		Log(msg);
		sprintf(msg, "<setup>\tLeavewordID\t= %d\n",idDes.LeavewordID);
		Log(msg);

		////////////////////////////////////////////////////////////////////////////////////////
		// 
		// 处理 点卡金部分
		// 
		////////////////////////////////////////////////////////////////////////////////////////
		//Log("<info>\t处理点卡金部分据开始...\n");
		//// 处理尚位赠送的物品
		//hr = rs->Open( "SELECT * FROM CSL_Card_Largess WHERE SendNum > ObtainedNum", cnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		//if(FAILED(hr)) _com_issue_error(E_FAIL);
		//while(!rs->GetRSEOF()) {
		//	char old_cdkey[32]="";
		//	char new_cdkey[32]="";
		//	
		//	strcpy( old_cdkey, (const char *)(_bstr_t)rs->GetCollect("Cdkey") );

		//	_strlwr(old_cdkey);

		//	string temp;
		//	// 是否修正帐号
		//	temp = changed_cdkey[old_cdkey];
		//	strcpy( new_cdkey, temp.data() );

		//	if( !temp.length() ) { // 如果没有修正帐号，就用旧帐号
		//		strcpy( new_cdkey, old_cdkey );
		//	}
		//	
		//	try {
		//		sprintf(sql, "INSERT INTO CSL_Card_Largess(cdkey,sendnum,ObtainedNum) VALUES('%s',%d,%d)",
		//			new_cdkey, (long)rs->GetCollect("sendnum"), (long)rs->GetCollect("ObtainedNum") );
		//		cnDes->Execute(sql, NULL, adCmdText);
		//	} // end try
		//	catch(_com_error &e) {
		//		sprintf( msg,"<ado err>\t数据库异常(for CSL_Card_Largess): %s\n", static_cast<const char*>(e.Description()) );
		//		Log(msg);
		//	}
		//	rs->MoveNext();
		//} // end while
		//rs->Close();

		//sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		//Log(msg);
	}
	catch(_com_error &e) {
		sprintf( msg,"<ado err>\t数据库异常(GameDB): %s\n", static_cast<const char*>(e.Description()) );
		Log(msg);
	}



	try
	{
		// 提前打开loginDB数据库，但只用到他的des部分
		// 打开数据库连接
		hr = logincnDes->Open(logincnstrDes.data(), "", "", adConnectUnspecified);
		if(FAILED(hr)) _com_issue_error(E_FAIL);
		hr = logincnSrc->Open(logincnstrSrc.data(), "", "", adConnectUnspecified);
		if(FAILED(hr)) _com_issue_error(E_FAIL);

		////////////////////////////////////////////////////////////////////////////////////////
		// 
		// 处理 赠送部份
		// 
		////////////////////////////////////////////////////////////////////////////////////////
		Log("<info>\t处理赠送部份数据开始...\n");

		// 处理尚位赠送的物品
		hr = rs->Open( "SELECT * FROM Largess WHERE SendNum > ObtainedNum", cnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);
		while(!rs->GetRSEOF()) {
			try {
				_variant_t var = rs->GetCollect("SendTime");
				char sendtime[20] = "";
				if( var.vt != VT_NULL ) {
					GetDateString(var, sendtime);
				}

				sprintf(sql, "INSERT INTO Largess(cdkey,GoodsIndex,GoodsName,GoodsLevel,SendNum,WorldID,SendTime,IsProcessed) \
							 VALUES('%s',%d,'%s',%d,%d,%d,'%s',0)", (const char*)(_bstr_t)rs->GetCollect("cdkey"),
							 (long)rs->GetCollect("GoodsIndex"),(const char*)(_bstr_t)rs->GetCollect("GoodsName"),(long)rs->GetCollect("GoodsLevel"),
							 (long)rs->GetCollect("SendNum")-(long)rs->GetCollect("ObtainedNum"),nWorldID,sendtime );

				logincnDes->Execute(sql, NULL, adCmdText);

			} // end try
			catch(_com_error &e) {
				sprintf( msg,"<ado err>\t数据库异常(for player Largess): %s", static_cast<const char*>(e.Description()));
				Log(msg);
			}
			rs->MoveNext();
		}

		rs->Close();
		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);
	}
	catch(_com_error &e) {
		sprintf( msg,"<ado err>\t数据库异常(GameDBLoginDB): %s\n", static_cast<const char*>(e.Description()) );
		Log(msg);
	}


	////////////////////////////////////////////////////////////////////////////////////////
	// 
	// 处理 loginDB
	// 
	////////////////////////////////////////////////////////////////////////////////////////
	
	//sprintf(cnstrDes,"Provider='sqloledb'; Data Source='%s'; Initial Catalog='%s'; User ID='%s'; Password='%s'",
	//	sqlLoginDes.Server, sqlLoginDes.Database, sqlLoginDes.Uid, sqlLoginDes.Pwd);
	//sprintf(cnstrSrc,"Provider='sqloledb'; Data Source='%s'; Initial Catalog='%s'; User ID='%s'; Password='%s'",
	//	sqlLoginSrc.Server, sqlLoginSrc.Database, sqlLoginSrc.Uid, sqlLoginSrc.Pwd);

	sprintf(msg, "<info>\tLoginDB目标数据库:%s %s\n", sqlLoginDes.Server.data(), sqlLoginDes.Database.data());
	Log(msg);
	sprintf(msg, "<info>\tLoginDB源数据库:%s %s\n", sqlLoginSrc.Server.data(), sqlLoginSrc.Database.data());
	Log(msg);

	try {

		Log("<info>\t处理LoginDB，复制未更名帐号开始...\n");

		hr = rs->Open("SELECT * FROM CSL_CDKEY ORDER BY cdkey",
			logincnSrc.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		if(FAILED(hr)) _com_issue_error(E_FAIL);


		char date[20] = "";
		_RecordsetPtr rs11;
		hr = rs11.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)) _com_issue_error(E_FAIL);

		while( !rs->GetRSEOF() ) {

			sprintf(sql, "SELECT TOP 1 * FROM CSL_CDKEY WHERE cdkey='%s' ORDER BY cdkey", (const char *)(_bstr_t)(rs->GetCollect("cdkey")));
			hr = rs11->Open(sql, logincnDes.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
			if(FAILED(hr)) _com_issue_error(E_FAIL);

			bEof = rs11->GetRSEOF();
			rs11->Close();

			if( !bEof ) { // 目标数据库存在改帐号了。忽略复制
				rs->MoveNext();
				continue;
			}

			// 复制该帐号
			_variant_t var = rs->GetCollect("reg_date");
			if( var.vt != VT_NULL ) {
				GetDateString(var, date);
			}
			sprintf( sql, "INSERT INTO CSL_CDKEY(cdkey,password,reg_date) VALUES('%s','%s','%s')",
				(const char *)(_bstr_t)(rs->GetCollect("cdkey")),
				(const char *)(_bstr_t)(rs->GetCollect("password")),
				date);
			logincnDes->Execute(sql, NULL, -1);

			rs->MoveNext();
		}
		rs->Close();

		sprintf( msg, "<info>\t完成！用时：%d\n", begin_time = clock()-begin_time );
		Log(msg);

	}
	catch(_com_error &e) {
		sprintf( msg,"<ado err>\t数据库异常(LoginDB): %s\n", static_cast<const char*>(e.Description()) );
		Log(msg);
	}



	sprintf( msg, "<info>\t所有完成！总用时：%d\n", clock()-total_time );
	Log(msg);

	LogChanged(fAC, "<End>\n");
	LogChanged(fPNC, "<End>\n");

	Log("<End>\n");


	changed_player_id.clear();
	changed_faction_id.clear();
	changed_player_name.clear();

	if(file) fclose(file);
	if(fAC) fclose(fAC);
	if(fPNC) fclose(fPNC);

	if(cnDes->GetState() == adStateOpen) cnDes->Close();
	if(cnSrc->GetState() == adStateOpen) cnSrc->Close();
	if(logincnDes->GetState() == adStateOpen) logincnDes->Close();
	if(logincnSrc->GetState() == adStateOpen) logincnSrc->Close();

	cnDes.Release();
	cnSrc.Release();
	logincnDes.Release();
	logincnSrc.Release();

	// 释放ADO
	::CoUninitialize();
	return 0;

}


// 设置IMAGE数据
bool CopyChunk(_RecordsetPtr &des, const _RecordsetPtr &src, const char *fileds ) {
	_variant_t varBLOB;
	long size;

	try {
		// read
		size = src->GetFields()->GetItem(fileds)->GetActualSize();
		if(size) {
			varBLOB = src->GetFields()->GetItem(fileds)->GetChunk(size);

			/*varBLOB.vt != VT_NULL && varBLOB.vt != VT_EMPTY */
			des->GetFields()->GetItem(fileds)->AppendChunk(varBLOB);		//write
		}
		return true;
	}
	catch(_com_error &e) {
		printf((char*)e.Description());
		sprintf(msg,"<ado err>\t数据库异常(Chunk): %s\n", static_cast<const char*>(e.Description()));
		fputs(msg, file);
		return false;
	}
}

// 记录日志
void Log(const char *str) {
	if(!str || !file) return;
	printf(str);
	fputs(str,file);
	fflush(file);
}
void LogChanged(FILE *fChanged, const char *str) {
	if(!str || !fChanged) return;
	fputs(str,fChanged);
	fflush(fChanged);
}

char *FixName(char *n_name, const char *o_name, bool existent) {

	if( !n_name || !o_name ) return NULL;

	char temp[32];
	char buf[8];
	if( existent ) {
		// 存在了的，需要加序列数
		sprintf(temp,"%s%02d%s%s", profix.data() , GetNum(buf), spechar.data(), o_name);
	}
	else {
		// 第1次，肯定不知道是否存在，直接休整
		sprintf(temp,"%s%s%s", profix.data() , spechar.data(), o_name);
	}

	//修复双字节字符 并截取到16字节内
	unsigned char c;
	while( strlen(temp) > 16 ) {
		c = temp[strlen(temp)-1];
		int s = static_cast<int>(strlen(temp));
		if( c <= 128 ) {
			// ASCII字符
			temp[s-1] = 0;
		}
		else {
			// 双字节字符
			temp[s-1] = 0;
			temp[s-2] = 0;
		}
	}

	strcpy(n_name, temp);

	return n_name;
}
//char *GetGUIDString(char *s) {
//	if(!s) return NULL;
//	GUID guid;
//	CoCreateGuid(&guid);
//	sprintf(s, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
//		guid.Data1, guid.Data2, guid.Data3, guid.Data4[0],guid.Data4[1],
//		guid.Data4[2],guid.Data4[3],guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]);
//	return s;
//}
char *GetDateString(_variant_t v, char *s) {
	if(!s) return NULL;
	if( v.vt != VT_NULL )
	{
		SYSTEMTIME st;
		VariantTimeToSystemTime(v.date, &st);
		sprintf(s,"%d-%d-%d %d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	}
	return s;
}

BYTE GetNum(char str[3]) {
	BYTE t;
	if( num>=100 ) {
		num=0;
	}

	t = num;

	sprintf(str, "%02d", t);
	str[2] = 0;

	num++;
	return t;
}



bool LoadConf(const char *filename)
{
	if( !filename ) return false;

	ifstream stream;
	stream.open(filename);

	if( !stream.is_open() )
	{
		printf("Configuration file: '%s' can not open!\r\n", msg);
		return false;
	}

	string strTemp;

	stream	
		>> strTemp >> sqlDes.Server
		>> strTemp >> sqlDes.Database
		>> strTemp >> sqlDes.Uid
		>> strTemp >> sqlDes.Pwd

		>> strTemp >> sqlSrc.Server
		>> strTemp >> sqlSrc.Database
		>> strTemp >> sqlSrc.Uid
		>> strTemp >> sqlSrc.Pwd

		>> strTemp >> profix
		>> strTemp >> spechar
		>> strTemp >> nWorldID

		>> strTemp >> sqlLoginDes.Server
		>> strTemp >> sqlLoginDes.Database
		>> strTemp >> sqlLoginDes.Uid
		>> strTemp >> sqlLoginDes.Pwd

		>> strTemp >> sqlLoginSrc.Server
		>> strTemp >> sqlLoginSrc.Database
		>> strTemp >> sqlLoginSrc.Uid
		>> strTemp >> sqlLoginSrc.Pwd
		; // end

	stream.close();
	return true;
}

void MakeGameConnStr()
{
	cnstrDes = "Provider='sqloledb'; Data Source='" + sqlDes.Server +
		"'; Initial Catalog='" + sqlDes.Database.data() +
		"'; User ID='" + sqlDes.Uid.data() +
		"'; Password='" + sqlDes.Pwd.data() + "'";

	cnstrSrc = "Provider='sqloledb'; Data Source='" + sqlSrc.Server +
		"'; Initial Catalog='" + sqlSrc.Database.data() +
		"'; User ID='" + sqlSrc.Uid.data() +
		"'; Password='" + sqlSrc.Pwd.data() + "'";
}

void MakeLoginConnStr()
{
	logincnstrDes = "Provider='sqloledb'; Data Source='" + sqlLoginDes.Server +
		"'; Initial Catalog='" + sqlLoginDes.Database.data() +
		"'; User ID='" + sqlLoginDes.Uid.data() +
		"'; Password='" + sqlLoginDes.Pwd.data() + "'";

	logincnstrSrc = "Provider='sqloledb'; Data Source='" + sqlLoginSrc.Server +
		"'; Initial Catalog='" + sqlLoginSrc.Database.data() +
		"'; User ID='" + sqlLoginSrc.Uid.data() +
		"'; Password='" + sqlLoginSrc.Pwd.data() + "'";
}