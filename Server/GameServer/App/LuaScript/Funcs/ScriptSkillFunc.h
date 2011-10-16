/// 
/// @file ScriptSkillFunc.h
/// @brief Script内的技能函数
/// @details 大部分技能函数都只能用于技能伤害脚本里
///

#define DEF_SHAPE (ulong)(-1)

// class Script {
///
/// 添加状态
/// @param ID 状态ID
/// @param level 状态等级
/// @param shape [可选]要添加到的shape，默认为源对象
///
bool AddState( long ID, long level, ulong shape = DEF_SHAPE );

///
/// 移除状态
/// @param ID 状态ID
/// @param level 状态等级
/// @param count [可选]状态数量
/// @param shape [可选]指定移除对象，默认为源对象
///
void RemoveState( long ID, long level, long count = 0, ulong shape = DEF_SHAPE );

///
/// 给源玩家添加一个技能
/// @param ID 技能ID
/// @param level 技能等级
/// @return 成功返回true
///
bool AddSkill( long ID, long level );

/// 
/// 移除源玩家身上的某个技能
/// @param ID 技能ID
/// @param level 技能等级
///
void DelSkill( long ID, long level );

///
/// 获取指定技能的等级
/// @param 技能
/// @return 技能等级
///
long GetSkillLevel( ulong skillPtr );

///
/// 判断学习技能条件
/// @param ID 技能ID
/// @return true表示可学习
///
bool CheckStudySkill( long ID );

///
/// 获取状态个数
/// @param shapeID 对象GUID
/// @param shapeType 对象类型
/// @return 指定对象上的状态个数
///
long GetAllStateNum( const CGUID &shapeID = NULL_GUID, long shapeType = 0 );

///
/// 删除所有DEBUFF
/// @param shapePtr 对象标识
///
void DelDeBuff( ulong shapePtr );

/// 
/// 删除所有DEBUFF和BUFF
/// @param shapePtr 对象标识
///
void DelAllBuff( ulong shapePtr );

///
/// 根据对象标识设置其属性
/// @param shapePtr 对象标识
/// @param attrName 属性名
/// @param attrVal 属性值
///
void SetMoveShapeAttrEx( ulong shapePtr, const char *attrName, double attrVal );

/// 
/// 根据对象标识获取其属性
/// @param shapePtr 对象标识
/// @param attrName 属性名
/// @return 属性值
///
double GetMoveShapeAttrEx( ulong shapePtr, const char *attrName );

///
/// 根据对象标识改变其属性
/// @param shapePtr 对象标识
/// @param attrName 属性名
/// @param attrVal 属性值
///
void ChangeMoveShapeAttrEx( ulong shapePtr, const char *attrName, double attrVal );

///
/// 给指定对象添加DEBUFF后的处理
/// @param srcPtr 源对象标识
/// @param destPtr 目标对象标识
///
void AddDeBuffToObj( ulong srcPtr, ulong destPtr );

///
/// 技能复活,已取消
/// @param skillPtr 技能标识
///
void ReliveSkill( ulong skillPtr );

///
/// 获取PK判断结果
///
long GetPkResult( ulong srcPtr, ulong destPtr, long addSub );

///
/// 根据对象标识获取其类型
/// @param shapePtr 对象标识
/// @return 对象类型
///
long GetObjectType( ulong shapePtr );

/// 
/// 根据对象标识获取彼此间的距离
/// @param srcPtr 源对象标识
/// @param destPtr 目标对象标识
/// @return 两对象之间的距离
///
double GetDistance( ulong srcPtr, ulong destPtr );

///
/// 发送技能列表
///
void SendSkillList( const char *str );

///
/// 获取能力加成系数
/// @return 能量加成系数
///
float GetEnergyRatio( long param, uchar magic );

///
/// 扣除能力值
///
void DeductEnergy( uchar magic );

/// 
/// 设置源对象为目标对象的攻击目标
/// @param srcPtr 源对象
/// @param destPtr 目标对象
///
void SetTarget( ulong srcPtr, ulong destPtr );

///
/// 判断目标装备盾牌
///
long GetShieldNum( ulong destPtr );

///
/// 获取技能的脚本参数
///
long GetSkillParam( ulong skillPtr, long pos );

///
/// 获取技能的元素类型
/// @param skillPtr 技能标识
/// @return 技能的元素类型
///
long GetSkillElement( ulong skillPtr );

///
/// 获取节能的类别
/// @param skillPtr 技能标识
/// @return 技能类别
///
long GetSkillUseType( ulong skillPtr );

long GetSkillModType( ulong skillPtr );

///
/// 通过ID获得状态相关参数
///
long GetStateParamByID( long ID, const char *param, 
					   const CGUID &shapeID = NULL_GUID, long shapeType = 0 );

///
/// 通过索引得到状态相关参数
///
long GetStateParamByIndex( long ID, const char *param, 
					   const CGUID &shapeID = NULL_GUID, long shapeType = 0 );

///
/// 检查施放者
///
bool CheckStateFirer( ulong skillPtr );

///
/// 通过技能ID获取技能等级
///
long GetSkillLv( long ID );

///
/// 在脚本中使用的复活接口
///
void ReliveInScript( ulong shapePtr, long type );

///
/// 根据参数对象得到相应的guid
/// @param shapePtr 对象标识
///
CGUID GetGUIDByParam( ulong shapePtr );

///
/// 让指定对象运行脚本
/// @param shapePtr 对象标识
/// @param scrip 对象文件名
///
void RunScriptByDest( ulong shapePtr, const char *script );

///
/// 启动技能
///
bool BeginUseSkill( long ID, long level );

///
/// 根据事件参数设置作用对象
///
void SetEventDestObj( ulong skillPtr );

///
/// 根据技能或状态指针得到相应的等级
///
int GetBaseModuleLv( ulong skillPtr );

///
/// 获取物品附加属性
///
long GetGoodsAddonProperty( ulong skillPtr, int addonID, int pos );

///
/// 获取技能相关的物品信息
/// @param skillPtr 技能标识
/// @return 物品所在容器
/// @return 物品所在容器内的位置
///
void GetGoodsPlacePos( ulong skillPtr, long *place = 0, long *pos = 0 );

///
/// 使用指定位置的物品
///
void UseGoodsByPlacePos( long place, long pos );

///
/// 物品使用成功
///
void SuccedUseItem( ulong skillPtr );

///
/// 取得动作修正时间
///
long GetActModify( ulong skillPtr );

///
/// 向客户端发送解除物品锁定消息
///
void SendUnlockGoodsMsg( long place, long pos );

///
/// 获取对象的受益者信息
/// @param shapePtr 对象标识
/// @return 受益人GUID
/// @return 受益人类型
///
CGUID GetBeneficiaryInfo( ulong shapePtr, long *type = 0 );

///
/// 获取击杀者信息
/// @return 击杀者GUID
/// @return 击杀者类型
///
CGUID GetKillerInfo( long *type = 0 );

/// 
/// 获取攻击者信息
/// @return 攻击者GUID
/// @return 攻击者类型
///
CGUID GetAttackerInfo( long *type = 0 );

/// 
/// 获取技能基本属性值
/// 
long  GetSkillValue(long skillid,long lvl,std::string name);

/// 
/// 获取技能基本属性(脚本调用)
/// @param skillPtr 动态参数stModuParam
/// @param name     获得当前的属性值
/// 
long  GetSkillAttriValue(ulong skillPtr,std::string name);

/// 
/// 删除指定类型的状态或者Buf
/// @param SkillType  技能类型
/// @param Count      技能数量
/// 
void  ClearSkillAttackType(long  SkillType,long Count);

// };