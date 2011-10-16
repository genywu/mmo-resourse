/// 
/// @file ScriptRgnFunc.h
/// @brief Script内场景相关函数
///

 //class Script {

///
/// \brief 获取当前场景的属性值
///
int GetRgnAttr(const char* attrName);

///
/// \brief 设置当前场景的属性值
///
void SetRgnAttr(const char* attrName, int attrValue);

///
/// \brief 获取当前场景指定坐标的区域类型
/// \return 0=普通区域，1=竞技区，2=安全区，3=城战区
///
int GetCellType(int x, int y);

///
/// \brief 获取当前场景指定坐标的区域类型
/// \return 0=普通区域，1=竞技区，2=国战区，3=安全区，-1=出错
///
int GetMapInfo(int x, int y);

///
/// \brief 设置地图阻挡
/// \param block 0=无阻挡，1=可飞行，2=不可飞行，3=人
///
void SetBlock(const CGUID& regionGuid, int x, int y, int block);

///
/// \brief 获取某个场景的无阻挡的随机点
///
bool GetRegionRandomPos(const CGUID& regionGuid=NULL_GUID, long* x=NULL, long* y=NULL);

///
/// \brief 获取某个场景某块区域内的无阻挡的随机点
///
bool GetRandomPosInRange(long regionId, int x, int y, int width, int height, long* randX=NULL, long* randY=NULL);

///
/// \brief 获取某个点到另一个点的方向
///
long GetLineDir(int srcX, int srcY, int destX, int destY);

///
/// \brief 根据某个点的坐标，方向，步长，获取下一个点的坐标
///
void GetNextPosByDir(int dir, int x, int y, int step, int* destX=NULL, int* destY=NULL);

///
/// \brief 场景上所有玩家触发某一脚本
///
void RunScriptForAllPlayerInRgn(const char* scriptFilePath);

///
/// \brief 获取某个普通场景的GUID
/// \param regionId 场景的模板的ID
///
CGUID GetNormalDupRgnID(long regionId);

///
/// \brief 切换地图
///
bool ChangeRgn(long regionId, long x=-1, long y=-1, long dir=0, long range=0, long carriageDistance=0);

///
/// \brief 切换副本地图
///
bool ChangeDupRgn(const CGUID &regionId, long regionType, long x, long y);

///
/// \brief 获取当前场景的GUID
///
CGUID GetCurRgnGUID();

///
/// \brief 获取某个副本的人数上限
///
int GetDupRgnPlayerMaxNum(const CGUID& regionGuid);

///
/// \brief 以副本为执行者执行一个脚本
///
bool RunDupRgnScript(const char* scriptFilename);

///
/// \brief 获取当前的副本的数量的上限
///
int GetDupRgnMaxNum(int regionType);

///
/// \brief 设置指定副本的管理权开放
///
void SetDupRgnOperFlag(const CGUID& regionGuid, long operationFlag);

///
/// \brief 获取指定副本的管理权开放
///
bool GetDupRgnOperFlag(const CGUID& regionGuid);

///
/// \brief 获取执行者当前所在队伍中的指定队伍顺序号的角色的GUID
///
CGUID GetTeamMaterIDByPos(int pos);

///
/// \brief 修改指定副本的永久显示名
///
void EditDupRgnName(const CGUID& regionGuid, const char* regionName);

///
/// \brief 创建道具
///
void CreatePHGoods(const CGUID& ownerGuid, const char* name, int counts, 
						   int x, int y, int width, int height, int dir, 
						   const char* script, const char* talk);

///
/// \brief 删除个人房屋副本道具
///
void DeletePHGoods(const CGUID& ownerGuid, const CGUID& goodGuid);

///
/// \brief 获取当前脚本玩家个人副本的GUID
///
CGUID GetCurPlayerPDupRgnID();

///
/// \brief 获取当前脚本玩家个人房屋副本的GUID
///
CGUID GetCurPlayerPHDupRgnID();

///
/// \brief 设置当前脚本玩家个人副本的GUID
///
void SetPlayerPDupRgnID(const CGUID& ownerGuid, const CGUID& regionGuid);

///
/// \brief 设置当前脚本玩家个人房屋副本的GUID
///
void SetPlayerPHDupRgnID (const CGUID& ownerGuid, const CGUID& regionGuid);

///
/// \brief 获取本地GS的ID
///
int GetLocalGSID();

///
/// \brief 将玩家切换到指定副本地图
///
bool ChangeDupRgnByRgnID(const CGUID& playerGuid, int regionType, const CGUID& regionGuid, int x, int y);

///
/// \brief 获取当前脚本调用者的GUID
///
CGUID GetCallerGUID();

///
/// \brief 调用指定玩家或怪物的指定脚本
///
void CallScriptByGUID(const CGUID& shapeGuid, const char* scriptFilename);

///
/// \brief 清除当前GS的所有指定类型的副本
///
void ClearCurGSDupRgn(int regionType);

///
/// \brief 设置当前脚本的队伍副本的GUID
///
void AddTeamRgnID(const CGUID& playerGuid, int tempRegionId, const CGUID& regionGuid);

///
/// \brief 为当前脚本队伍清除副本GUID
///
void DelTeamRgnID(const CGUID& playerGuid, int tempRegionId);

///
/// \brief 为当前脚本队伍清除所有副本GUID
///
void ClearTeamRgnID(const CGUID& playerGuid);

///
/// \brief 设置据点状态
///
void SetRgnStrongPointState(const char* footholdName, int footholdState);

///
/// \brief 获取据点状态
///
int GetRgnStrongPointState(const char* footholdName);


 //};