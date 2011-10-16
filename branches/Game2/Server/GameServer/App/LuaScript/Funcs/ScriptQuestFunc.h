/// 
/// @file ScriptQuestFunc.h
/// @brief Script内的任务相关函数
///

// class Script {

/// @brief 给玩家添加某个任务
bool AddQuest(int questId);

/// @brief 获取玩家的主线任务ID
int GetMainQuestId();

/// @brief 玩家完成某个任务
bool CompleteQuest(int questId);

/// @brief 玩家放弃某个任务
void AbandonQuest(int questId);

/// @brief 获取玩家某个任务的状态
/// @return 任务状态值，0：无任务，未完成；1：无任务，已完成；2：有任务，未完成
int GetQuestState(int questId);

/// @brief 获取玩家有效的任务的数量
int GetValidQuestAmount();

/// @brief 更新某个任务的步骤
void UpdateQuestStep(int questId, int stepId);

/// @brief 更新某个任务某一目标的所有值？
void UpdateQuestParam(int questId, int paramIdx, int param, int regionId, int x, int y);

/// @brief 更新某个任务的某个索引的值？
void UpdateQuestData(int questId, int paramIdx, int value);

/// @brief 获取某个任务的步骤的数量
int GetQuestStepAmount(int questId);

/// @brief 获取某个任务的某个步骤的某个索引的值
int GetParam(int questId, int stepId, int idx);

/// @brief 获取某个任务的某个步骤的某个索引的附加属性的值
int GetQuestAccParam(int questId, int stepId, int idx);

/// @brief 设置某个任务的某个步骤的某个索引的附加属性的值
bool SetQuestAccParam(int questId, int stepId, int idx, int value);

/// @brief 添加某个任务的某个步骤的某个索引的附加属性的值
bool AddQuestAccParam(int questId, int stepId, int idx, int value);

/// @brief 获取某个任务的某个步骤的某个索引的场景ID
int GetQuestRegionID(int questId, int stepId, int idx);

/// @brief 设置某个任务的某个步骤的某个索引的场景ID
bool SetQuestRegionID(int questId, int stepId, int idx, int regionId);

/// @brief 获取某个任务的某个步骤的某个索引的场景X坐标
int GetQuestRegionX(int questId, int stepId, int idx);

/// @brief 设置某个任务的某个步骤的某个索引的场景X坐标
bool SetQuestRegionX(int questId, int stepId, int idx, int x);

/// @brief 获取某个任务的某个步骤的某个索引的场景Y坐标
int GetQuestRegionY(int questId, int stepId, int idx);

/// @brief 设置某个任务的某个步骤的某个索引的场景Y坐标
bool SetQuestRegionY(int questId, int stepId, int idx, int y);

/// @brief 获取玩家的任务列表的任务数量
int GetQuestAmount();

/// @brief 获取玩家的主线任务的章节编号
int GetQuestChapter();

/// @brief 设置玩家的主线任务的章节编号
bool SetQuestChapter(int num);

/// @brief 添加玩家的任务记录
bool AddFinishedQuestRecord(int questId);

/// @brief 删除玩家的任务记录
bool DelFinishedQuestRecord(int questId);

/// @brief 获取玩家的某个任务是否完成
bool GetQuestFinishedRecord(int questId);

/// @brief 获取玩家的某个任务的文本类型
int GetQuestTextType(int questId);

/// @brief 获取玩家的某个任务的名字
const char* GetQuestName(int questId);

/// @brief 玩家的某个任务是否被记录
int IsQuestRecorded(int questId);

/// @brief 获取玩家的某个任务的类型
int GetQuestType(int questId);

/// @brief 获取某个任务是否可以共享
int GetQuestIsShare(int questId);

/// @brief 判断技能条件是否满足
bool GetQuestReqSkill(int questId);

/// @brief 判断任务的性别限制条件
bool GetQuestSexLimit(int questId);

/// @brief 判断是否携带任务限定宠物
bool GetQuestTotePet(int questId);

/// @brief 判定玩家的某个任务是否被显示
int IsQuestDisplayed(int questId);

/// @brief 判定玩家的某个任务是否能反复接
int IsQuestRepeated(int questId);

/// @brief 判定玩家的某个任务是否在死亡后失败
int IsQuestFailOnDead(int questId);

/// @brief 判定玩家的某个任务是否在失败后能重新接
int IsQuestRepeatedOnFail(int questId);

/// @brief 获取玩家的某个任务的最小等级限制
int GetQuestMinLvLimit(int questId);

/// @brief 获取玩家的某个任务的最大等级限制
int GetQuestMaxLvLimit(int questId);

/// @brief 获取玩家的某个任务的职业限制
int GetQuestOccupationLimit(int questId, int occupationIdx);

/// @brief 获取玩家的某个任务的章节数
int GetQuestChapterAmount(int questId);

/// @brief 获取玩家的某个任务的下某个任务ID
int GetNextQuestId(int questId);

/// @brief 获取玩家的某个任务的某个索引对应的前置任务ID
int GetPreQuest(int questId, int idx);

/// @brief 获取玩家的某个任务的前置任务数量
int GetPreQuestAmount(int questId);

/// @brief 获取目标模板类型
int GetAimAMTType(int questId, int stepId, int aimId);

/// @brief 获取目标类型
int GetAimType(int questId, int stepId, int aimId);

/// @brief 获取目标ID
int GetAimId(int questId, int stepId, int aimId);

/// @brief 获取目标数量
int GetAimAmount(int questId, int stepId, int aimId);

/// @brief 获取内存中变量索引
int GetAimParameIndex(int questId, int stepId, int aimId);

/// @brief 获取掉落这种物品的生物类型
int GetDropFrom(int questId, int stepId, int aimId);

/// @brief 获取掉落几率
int GetDropOdds(int questId, int stepId, int aimId);

/// @brief 获取任务目标的名字
const char* GetAimName(int questId, int stepId, int aimId);

/// @brief 获取备用属性
int GetPrepareParam(int questId, int stepId, int aimId, int index);

/// @brief 获取任务奖励数量
int GetQuestEncouragementSum(int questId);

/// @brief 获取index对应的步骤奖励名字
const char* GetStepEncouragementName(int questId, int stepId, int index);

/// @brief 获取index对应的任务奖励名字
const char* GetQuestEncouragementName(int questId, int index);

/// @brief 获取index对应的步骤奖励类型
int GetStepEncouragementType(int questId, int stepId, int index);

/// @brief 获取index对应的任务奖励类型
int GetQuestEncouragementType(int questId, int index);

/// @brief 获取index对应的步骤奖励ID
int GetStepEncouragementID(int questId, int stepId, int index);

/// @brief 获取index对应的任务奖励ID
int GetQuestEncouragementID(int questId, int index);

/// @brief 获取index对应的步骤奖励数量
int GetStepEncouragementNum(int questId, int stepId, int index);

/// @brief 获取index对应的任务奖励数量
int GetQuestEncouragementNum(int questId, int index);

/// @brief 获取index对应的步骤奖励所占格子数
int GetStepEncouragementCell(int questId, int index);

/// @brief 获取index对应的任务奖励所占格子数
int GetQuestEncouragementCell(int questId);

/// @brief 获取任务总步骤数
int GetStepSum(int questId);

/// @brief 获取任务步骤总目标数
int GetStepAimNum(int questId, int stepId);

/// @brief 此任务是否在任务系统中
int FindRoleIsInSystem(int questId);

/// @brief 某任务的某一步骤是否在任务系统中
int FindRoleStepIsInSystem(int questId, int stepId);

/// @brief 获得玩家点Npc的状态
int GetQuestStateId(int questId);

/// @brief 获得任务NPC的文本显示id
const char* GetQuestDisplayText(int questId,int state);
// };