/// 
/// @file ScriptGoodsFunc.h
/// @brief Script内的物品操作函数
///

// class Script {

///
/// \brief 添加道具
/// \param goodsOriginalName 道具原始名
/// \param goodsAmount 道具的数量
/// \return 成功与否
///
bool AddGoods(const char* goodsOriginalName, int goodsAmount);

/// \brief 添加装备
/// \param goodsOriginalName 道具原始名
/// \param goodsAmount 道具的数量
/// \param armamentLevel 冲等等级
/// \param evilLevel 魔化等级
/// \param playerName 玩家名字
/// \return 成功与否
bool AddArmament(const char* goodsOriginalName, int goodsAmount, int armamentLevel, int evilLevel, const char* playerName=NULL);

/// \brief 删除道具
/// \param goodsOriginalName 道具原始名
/// \param goodsAmount 道具的数量
/// \return 成功删除的道具数量
int DelGoods(const char *goodsOriginalName, int goodsAmount);

/// \brief 丢道具到地上
/// \param x 场景的X坐标
/// \param y 场景的Y坐标
/// \param goodsOriginalName 道具原始名
/// \param goodsAmount 道具数量
/// \return 成功与否
bool AddGoodsToRgn(double x, double y, const char* goodsOriginalName, int goodsAmount);

/// \brief 绑定道具
/// \param containerID 背包类型
/// \param containerPos 道具在背包中的位置
/// \param bindType 0：绑定，1：解绑
/// \return 成功与否
bool BindGoods(int containerID, int containerPos, int bindType);

/// \brief 冻结道具
/// \param containerID 背包类型
/// \param containerPos 道具在背包中的位置
/// \param bindType 0：绑定，1：解绑
/// \return 成功与否
bool FrostGoods(int containerID, int containerPos, int bindType);

/// \brief 检查原始背包，装备栏和子背包的某个道具的数量
/// \param goodsOriginalName 道具原始名
/// \return 道具数量
int CheckGoods(const char* goodsOriginalName);

/// \brief 检查银行的某个道具的数量
/// \param goodsOriginalName 道具原始名
/// \return 道具数量
int CheckGoodsInBank(const char* goodsOriginalName);

/// \brief 检查能否容纳一定数量的某个道具
/// \param goodsOriginalName 道具原始名
/// \param goodsAmount 道具数量
/// \return 成功与否
bool CheckSpace(const char* goodsOriginalName, int goodsAmount);

/// \brief 获取原始背包的道具数量
/// \return 道具数量
int GetGoodsNum();

/// \brief 获取剩余格子数量
/// \return 剩余格子数量
int GetGoodsFree();

/// \brief 获取某装备位置的装备
/// \param pos 装备位置
/// \param playerGuid 玩家GUID
/// \return 道具基本属性ID
int GetEquipIDByPos(int pos, const CGUID& playerGuid=NULL_GUID);

/// \brief 获取某装备位置的道具原始名
/// \param pos 装备位置
/// \param playerGuid 玩家GUID
/// \return equipment 道具原始名
const char* GetEquipNameByPos(int pos, const CGUID& playerGuid);

/// \brief 获取道具附加属性
/// \param containerId 背包类型
/// \param containerPos 道具在背包中的位置
/// \param addonPropertyIdx 道具附加属性ID
/// \param valueId 值1还是值2
/// \param playerName 玩家名字
/// \return 道具附加属性
int GetPlayerGoodsAddonProperty(int containerId, int pos, int addonPropertyIdx, int valueId, const char* playerName=NULL);

/// \brief sets 道具附加属性
/// \param containerId 背包类型
/// \param containerPos 道具在背包中的位置
/// \param addonPropertyIdx 道具附加属性ID
/// \param valueId 值1还是值2
/// \param的值 道具附加属性的值
/// \param playerName 玩家名字
/// \return 成功与否
bool SetPlayerGoodsAddonProperty(int containerId, int containerPos, int addonPropertyIdx, int valueId, int value, const char* playerName=NULL);

/// \brief 删除玩家道具
/// \param playerName 玩家名字
/// \param goodsOriginalName 道具原始名
/// \param goodsAmount 道具数量
/// \return 真正删除的道具数量
int DelPlayerGoods(const char* playerName, const char* goodsOriginalName, int goodsAmount);

/// \brief 删除触发脚本使用的道具
/// \param goodsAmount 道具数量
/// \return 真正删除的道具数量
int DelUsedGoods(int goodsAmount);

/// \brief 获取触发脚本使用的道具的数量
/// \return 道具数量
int CheckUsedGoods();

/// \brief 获取触发脚本使用的道具的附加属性的值
/// \param addonPropertyId 附加属性ID
/// \param valueId 值1还是值2
/// \return 道具附加属性的值1
int GetUsedGoodsPropertiy(int addonPropertyId, int valueId);

/// \brief 设置使用的道具附加属性的值
/// \param addonPropertyId 道具附加属性ID
/// \param valueId 值1还是值2
/// \param的值 道具附加属性的值
/// \return 成功与否
bool SetUsedGoodsPropertiy(int addonPropertyId, int valueId, int value);

/// \brief 买卖道具
/// \param npcName NPC名字
void BuyGoods(const char* npcName);

/// \brief 获取道具附加属性的值
/// \param goodsId 道具ID
/// \param propertyId 道具基本属性ID
/// \param valueId 值1还是值2
/// \return 道具附加属性的值
int GetGoodsAddonProperty(int goodsId, int propertyId, int valueId);

/// \brief 设置玩家的装备炼度全满
void FullRefine();

/// \brief 获取武器强度
/// \param srcPoint 玩家指针
/// \return 武器强度等级的值1
int GetWeaponStr(ulong srcPoint);

/// \brief 获取道具显示名
/// \param goodsOriginalName 道具原始名
/// \return 道具显示名
const char* GetGoodsDisplayName(const char* goodsOriginalName);

/// \brief 玩家回城
/// \return 成功与否
bool BackToCity();

/// \brief 开始一个通用道具容器操作
/// \param onFinishScript 完成操作的脚本
/// \param onPutScript 响应物品放入的脚本
/// \param titleId 容器名称ID
/// \param buttonId 容器按钮ID
/// \param txt 显示文本
/// \param radius 关闭距离
/// \param maxPushNum 容器最大放置物品数
/// \return 成功与否
bool BeginContainerGoodsOpt(const char* onFinishScript, 
							const char* onPutScript, 
							ulong titleId, 
							ulong buttonId, 
							const char* txt, 
							long radius,
							ulong maxPushNum);

/// \brief 获取容器中待操作的道具
/// \param goodsIdx 第几个操作栏位（1~4）
/// \return 道具GUID
CGUID GetOptGoodsInContainer(ulong goodsIdx);

/// \brief 放入待操作的道具到容器中
/// \param goodsGuid 道具GUID
/// \param goodsIdx 第几个操作栏位（1~4）
/// \return 成功与否
bool PutOptGoodsInContainer(const CGUID& goodsGuid, ulong goodsIdx);

/// \brief 清除容器中的待操作道具
/// \param goodsIdx 第几个操作栏位（1~4）
/// \return 成功与否
bool RemoveOptGoodsInContainer(ulong goodsIdx);

/// \brief 刷新容器文本
/// \param txt 容器文本
/// \return 成功与否
bool RefreshContainerTxt(const char* txt);

/// \brief 刷新容器显示的金钱数量
/// \param 容器显示的金钱数量
/// \return 成功与否
bool RefreshContainerGold(ulong goldAmount);

/// \brief 关闭一个通用道具容器操作
void EndContainerGoodsOpt();

/// \brief 获取道具原始名
/// \param goodsGuid 道具GUID
/// \return 道具原始名
const char* GetGoodsOriginalName(const CGUID& goodsGuid);

/// \brief 获取道具附加属性的值
/// \param goodsGuid 道具GUID
/// \param addonPropertyId 道具的附加属性ID
/// \param valueId 值1还是值2
/// \return 道具附加属性的值
long GetGoodsAddonPropertyValues(const CGUID& goodsGuid, long addonPropertyId, long valueId);

/// \brief set 道具附加属性的值
/// \param goodsGuid 道具GUID
/// \param addonPropertyId 道具的附加属性ID
/// \param valueId 值1还是值2
/// \param newValue 道具附加属性的值
/// \return 成功与否
bool SetGoodsAddonPropertyValues(const CGUID& goodsGuid, long addonPropertyId, long valueId, long newValue);

/// \brief 重置道具的随机属性值
/// \param goodsGuid 道具GUID
/// \return 成功与否
bool ResetGoodsStochastic(const CGUID& goodsGuid);

/// \brief 删除道具
/// \param goodsGuid 道具GUID
/// \return 成功与否
bool DelGoodsByGuid(const CGUID& goodsGuid);

/// \brief 创建道具，返回第一个道具的GUID
/// \param goodsOriginalName 道具原始名
/// \param goodsAmount 道具的数量
/// \return 第一个道具的GUID
CGUID AddGoodsReturnGuid(const char* goodsOriginalName, uint goodsAmount);

/// \brief 是否是冻结的道具
/// \param goodsGuid 道具GUID
/// \return 0：失败，1：冻结，2：未冻结
int IsFrostGoods(const CGUID& goodsGuid);

/// \brief 获取正在使用的道具的原始名
/// \return 正在使用的道具的原始名
const char* GetUsedGoodsOriginalName();

/// \brief 获取正在使用的道具的GUID
/// \return 正在使用的道具的GUID
CGUID GetUsedGoodsGuid();

/// \brief 设置道具附加描述文本
/// \param goodsGuid 道具GUID
/// \param txt 道具附加描述文本
/// \return 成功与否
bool SetAddonDesc(const CGUID& goodsGuid, const char* txt);

/// \brief 获取道具附加描述文本
/// \param goodsGuid 道具GUID
/// \return 道具附加描述文本
const char* GetAddonDesc(const CGUID& goodsGuid);

/// \brief 获取当前操作的装备附加属性
/// \detail 可用于装备/卸装物品脚本里
/// \param pidx 附加属性栏位
/// \param vids 值栏位
/// \return 未找到返回0
long GetUsedEquipProperty( int pidx, int vidx );
// };