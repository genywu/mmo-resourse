/// 
/// @file  ScriptPetFunc.h
/// @brief 宠物系统脚本函数
/// 

// class Script {

/// 根据宠物索引ID创建宠物
/// @param  type		宠物类型
/// @param  id			宠物索引ID
/// @param  isWild		是否野生标志
/// @return	创建宠物成功与否
bool CreatePet(long type, long id, long isWild);

/// 根据宠物GUID删除宠物
/// @param  guid		宠物GUID
/// @return	成功与否
bool DelPet(const CGUID& guid);

/// 设置宠物属性
/// @param attrName 属性名
/// @param attrVal 属性值
/// @param guid 宠物GUID
/// @return	成功与否
bool SetPetAttr(const char *attrName, double attrVal, const CGUID& guid);

/// 获取宠物属性值
/// @param attrName 属性名
/// @param guid 宠物GUID
/// @return 属性值
long GetPetAttr(const char* attrName, const CGUID& guid);

/// 激活宠物
/// @param  guid		宠物GUID
/// @return	成功与否
bool ActivePet(const CGUID& guid);

/// 为玩家宠物添加技能
/// @param  guid		宠物GUID
/// @param  id			技能ID
/// @param  level		技能等级
/// @param	pos			位置
/// @return	添加宠物技能成功与否
bool AddPetSkill(const CGUID& guid, long id, long level, long pos);

/// 获取当前焦点宠物GUID
/// @return	焦点宠物GUID
const CGUID& GetFocusPetGUID();

/// 获取玩家宠物品级
/// @param guid			宠物GUID
/// @return	宠物品级
long GetPetGrade(const CGUID& guid);

/// 获取玩家宠物后天资质已强化次数
/// @param guid			宠物GUID
/// @return	宠物后天资质已强化次数
long GetPetDAfterCnt(const CGUID& guid);

/// 重置宠物天资
/// @param  guid		宠物GUID
/// @return	重置天资成功与否
bool ResetPetTalent(const CGUID& guid);

/// 重置宠物后天资质强化次数
/// @param  guid		宠物GUID
/// @return	重置后天资质强化次数成功与否
bool ResetPetDAfterCnt(const CGUID& guid);

/// 后天资质强化
/// @param  guid		宠物GUID
/// @param	val			强化系数(道具强化效果值)
/// @return	后天资质强化成功与否
bool UpgradePetDAfterVal(const CGUID& guid, long val);

/// 重置宠物悟性点
/// @param  guid		宠物GUID
/// @return	重置宠物悟性点成功与否
bool ResetPetSavvy(const CGUID& guid);

/// 取得玩家最大可拥有侍从数目
/// @return 玩家最大可拥有侍从数目
long GetMaxServantNum(void);

/// 侍从经验获取
/// @param  guid		宠物GUID
/// @param  val			经验值
void IncPetExp(const CGUID& guid, long val);

/// 侍从修为经验获取
/// @param  guid		宠物GUID
/// @param  val			修为经验值
void IncPetDExp(const CGUID& guid, long val);

/// 宠物升级
/// @param  guid		宠物GUID
/// @param  newLev		宠物新等级
/// @return 宠物升级是否成功
bool PetLevUp(const CGUID& guid, long newLev);

// };