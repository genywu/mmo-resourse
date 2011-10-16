
/// @file ScriptPlayerFunc.h
/// @brief Script内的玩家操作函数
///

// class Script {
///
/// 设置玩家属性
/// @param attrName 属性名
/// @param attrVal 属性值
/// @param playerID [可选]玩家GUID，默认为源对象
///
bool SetPlayerAttr( const char *attrName, double attrVal, const CGUID &playerID = NULL_GUID );

///
/// 获取玩家属性值
/// @param attrName 属性名
/// @param playerID [可选]玩家GUID，默认为源对象
/// @return 属性值
///
double GetPlayerAttr( const char *attrName, const CGUID &playerID = NULL_GUID );

///
/// 改变玩家属性，加法操作
/// @param attrName 属性名
/// @param attrVal 属性值
/// @param playerID [可选]玩家GUID，默认为源对象
///
bool ChangePlayerAttr( const char *attrName, double attrVal, const CGUID &playerID = NULL_GUID );

///
/// 获取玩家角色名
/// @param playerID [可选]玩家GUID，默认为源对象
/// @return 玩家名
///
const char *GetPlayerName( const CGUID &playerID = NULL_GUID );

///
/// 获取玩家GUID
/// @param name 玩家角色名
/// @return 玩家GUID 
///
CGUID GetPlayerID( const char *name );

///
/// 改变玩家金钱
/// @param money 金钱值
/// @param playerID [可选]玩家GUID，默认为源对象
///
void ChangePlayerMoney( long money, const CGUID &playerID = NULL_GUID );

///
/// 获取玩家金钱
/// @param playerID [可选]玩家GUID，默认为源对象
/// @return 玩家金钱数
///
long GetPlayerMoney( const CGUID &playerID = NULL_GUID );

///
/// 设置玩家金钱
/// @param money 金钱值
/// @param playerID [可选]玩家GUID，默认为源对象
///
void SetPlayerMoney( long money, const CGUID &playerID = NULL_GUID );

///
/// 改变玩家点券
/// @param ticket 点券值
/// @param playerID [可选]玩家GUID，默认为源对象
///
void ChangePlayerTicket( long ticket, const CGUID &playerID = NULL_GUID );

///
/// 获取玩家点券
/// @param playerID [可选]玩家GUID，默认为源对象
/// @return 玩家点券数
///
long GetPlayerTicket( const CGUID &playerID = NULL_GUID );

///
/// 设置玩家点券
/// @param money 点券值
/// @param playerID [可选]玩家GUID，默认为源对象
///
void SetPlayerTicket( long ticket, const CGUID &playerID = NULL_GUID );

///
/// 根据配置改变移动速度
/// @param playerID [可选]玩家GUID，默认为源对象
///
void ChangeBaseSpeed( const CGUID &playerID = NULL_GUID );

///
/// 设置玩家坐标
/// @param x X坐标
/// @param y Y坐标
/// @param playerID [可选]玩家GUID，默认为源对象
///
void SetPlayerPos( long x, long y, const CGUID &playerID = NULL_GUID );

///
/// 设置玩家方向
/// @param dir 方向值
/// @param playerID [可选]玩家GUID，默认为源对象
///
void SetPlayerDir( long dir, const CGUID &playerID = NULL_GUID );

/// 
/// 取得玩家所属国家
/// @param playerID [可选]玩家GUID，默认为源对象
/// @return 国家ID
///
long GetCountry( const CGUID &playerID = NULL_GUID );

///
/// 改变玩家所属国家
/// @param country 国家ID
/// @param playerID [可选]玩家GUID，默认为源对象
///
void ChangeCountry( long country, const CGUID &playerID = NULL_GUID );

///
/// 获取玩家国家贡献值
/// @param playerID [可选]玩家GUID，默认为源对象
///
long GetContribute( const CGUID &playerID = NULL_GUID );

///
/// 设置玩家国家贡献值
/// @param contribute 贡献值
/// @param playerID [可选]玩家GUID，默认为源对象
///
void SetContribute( long contribute, const CGUID &playerID = NULL_GUID );

///
/// 检查玩家是否充值过
/// @param playerID [可选]玩家GUID，默认为源对象
/// @return true表示充值过
///
bool IsCharged( const CGUID &playerID = NULL_GUID );

///
/// 设置玩家充值标志
/// @param c 为1时表示充值过
/// @param playerID [可选]玩家GUID，默认为源对象
///
void SetCharged( int c, const CGUID &playerID = NULL_GUID );

///
/// 重置指定职业的职业等级
/// @param occu 职业，-1表示当前职业
///
void CheckOccuLvl( long occu );

///
/// 死亡复活
/// @param type 0安全回城，1原地复活
/// @param ratio
///
void Relive( long type, long ratio = 50 );

///
/// 改变玩家职业
/// 
void ChangeOccu( long occu );

///
/// 自动移动到指定坐标
/// @param x X坐标
/// @param y Y坐标
///
void AutoMove( float x, float y );

///
/// 传送指定场景上指定范围内的玩家到另一个场景
/// @param src 要传送玩家所在场景GUID
/// @param x1 y1 x2 y2 原场景范围
/// @param type 目标场景类型
/// @param dest 目标场景GUID
///
void MovePlayer( const CGUID &src, long x1, long y1, long x2, long y2,
				long type, const CGUID &dest, long dx1, long dy1, long dx2, long dy2 );

///
/// 切换无敌模式
/// @param mode 为0时关闭无敌，非0开启无敌
///
void GodMode( int mode );

///
/// 判断是否是GM
/// @param name 玩家角色名
/// @return true表示是GM
///
bool IsGM( const char *name );

///
/// 禁锢指定玩家
/// @param name 玩家角色名
/// @param flag 0禁锢，非0解禁
///
void DurancePlayer( const char *name, int flag );

///
/// 获取当前玩家所在GameServer ID
///
long GetServer();

///
/// 判断是否为结婚队伍，两个人是否同图
/// @param playerID [可选]玩家GUID，默认为源对象
/// @return 对方GUID
///
CGUID GetOtherOneInMarriageTeam( const CGUID &playerID = NULL_GUID );

///
/// 得到结婚步骤
///
long GetPlayerMarriageStep( const CGUID &playerID = NULL_GUID );

///
/// 设置结婚步骤 
///
void SetPlayerMarriageStep( long step, const CGUID &playerID = NULL_GUID );

///
/// 设置src为dest的结婚对象
///
void SetPlayerSpouse( const CGUID &src, const CGUID &dest = NULL_GUID );

///
/// 删除玩家结婚对象
///
void DelPlayerSpouse( const CGUID &playerID = NULL_GUID );

///
/// 取得玩家结婚对象GUID
///
CGUID GetPlayerSpouse( const CGUID &playerID = NULL_GUID );

///
/// 取得结婚对象的名字
///
const char *GetPlayerSpouseName( const CGUID &playerID = NULL_GUID );

///
/// 得到魔化元素属性
///
long GetElementProperty( long etype, long ptype, const CGUID &playerID = NULL_GUID );

///
/// 设置魔化元素属性
///
long SetElementProperty( long etype, long ptype, long val, const CGUID &playerID = NULL_GUID );

/// 
/// 设置技能天赋id
/// 
void StudyGenius(long id,long lvl,const CGUID& playerID = NULL_GUID);

/// 
///  删除天赋
/// 
void RemoveGenius(long id,const CGUID& playerID = NULL_GUID);

// };
