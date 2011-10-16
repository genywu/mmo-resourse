///
/// @file OverviewDoc.h
/// @brief 文档主页
///

/*! @mainpage 脚本函数说明
 * @section pre_sec 概述
 * 本文档由doxygen工具自动生成，用于描述脚本接口的使用。
 * 所有的脚本接口按照其所在位置被分为两大类：
 * - 全局脚本函数：可以直接使用，文档中显示位于 ScriptFunc 名字空间内;
 * - Script表内脚本函数：位于Script表内，可先通过 GetScript 获取。例如：
 * @code local script = GetScript()
 * script:AddInfo( "测试字符串" ) 
 * @endcode
 * <B>凡是位于前缀带"Script"的文件里的函数，均位于Script表下，否则就是全局的。</B>
 *
 * @section module_sec 模块结构
 * - <B>基础函数</B>: BaseFunc.h BaseFunc.cpp ScriptBaseFunc.h
 *
 * - <B>客户端交互函数</B>: ClientFunc.h ClientFunc.cpp ScriptClientFunc.h
 *
 * - <B>公会函数</B>: OrganFunc.cpp ScriptOrganFunc.h
 *
 * - <B>GM函数</B>: ScriptGlobalFunc.h
 *
 * - <B>物品操作函数</B>: ScriptGoodsFunc.h
 *
 * - <B>任务操作函数</B>: ScriptQuestFunc.h
 *
 * - <B>玩家操作函数</B>: ScriptPlayerFunc.h PlayerFunc.h PlayerFunc.cpp
 *
 * - <B>Shape操作函数</B>: ScriptShapeFunc.h
 *
 * - <B>场景函数</B>: RgnFunc.h RgnFunc.cpp ScriptRgnFunc.h
 *
 * - <B>相位函数</B>: ScriptPhaseFunc.h
 *
 * - <B>技能函数</B>: ScriptSkillFunc.h
 *
 * - <B>脚本变量函数</B>: VarListFuncs.cpp
 *
 * - <B>怪物仇恨函数</B>: ScriptTauntFunc.h
 * 
 * @section global_const_sec 全局常量说明
 * 脚本环境中事先注册了一些全局常量，可用于脚本函数的调用参数，增强可读性
 * - <B>脚本变量相关</B><BR>
 * 用于 AddVar()/ SetVar()/ GetVar() 等接口的参数，包括：
 *	- VT_SCRIPT: 仅用于技能伤害脚本里，表示对技能实例上的脚本变量操作
 *	- VT_SRCSHAPE: 对源对象身上的脚本变量操作
 *	- VT_DESTSHAPE: 对目标对象身上的脚本变量操作
 *	- VT_GLOBAL: 对全局变量（全服务器变量）的操作，仅能用于 SetVar()/ GetVar()接口
 *	- INVALID_NUM: 用于 GetVar() 获取数字变量失败时的返回值
 * - <B>Shape类型</B><BR>
 * 用于 Script::SetAttr()/ Script::GetAttr() 之类的函数，包括：
 *	- TYPE_PLAYER: 玩家类型
 *	- TYPE_MONSTER: 怪物类型
 * - <B>部分函数的执行结果</B><BR>
 *	- EXEC_ERROR: 函数执行错误
 *	- EXEC_FAIL: 失败
 *	- EXEC_SUCCESS: 执行成功
 * - <B>客户端对话框</B><BR>
 *	- MB_YESNO: 用于 GameMessage() 函数，表示弹出包含"YES/NO"选择的对话框
 *	- MB_OK: 用于 GameMessage() 函数，表示弹出确定对话框
 * - <B>场景类型</B><BR>
 *  - RGN_NORMAL: 普通场景类型
 *  - RGN_PERSONAL: 个人副本类型
 *  - RGN_PERSONAL_HOUSE: 个人房屋副本类型
 *  - RGN_TEAM: 队伍副本类型
 * - <B>技能脚本变量中的常量</B><BR>
 * 在技能伤害脚本里，附加了一个VT_SCRIPT脚本变量列表，可以通过 VarListFuncs.cpp 中的接口存取。
 * 该脚本变量列表里事先定义了几个常量，这些常量可用于 ScriptSkillFunc.h 中的接口参数：
 *  - $ParamF: 施放技能的SHAPE标识
 *  - $ParamS: 技能实例标识
 *  - $ParamA: 技能拥有者SHAPE标识，对于(DE)BUFF而言，拥有者和施放者不同，普通技能相同
 *  - $ParamB: 目标SHPAE标识
 *
 * @section code_sec 脚本编写注意事项
 * - 尽量保持所有的lua变量为局部的，如：local a = xxx
 * - 尽量保持所有自己定义的lua函数也是局部的，如：local function f() ... end
 * - 对有些返回字符串的函数，其返回值可能为nil，也可能为""，需要根据具体接口而定
 * - “脚本变量”指的是绑定在场景、玩家、怪物、技能实例等对象上的数据，与lua本身的变量不同；
 *    脚本变量有三种类型：数字、字符串、GUID，函数 GetVar()如果没有找到指定的变量，将返回一
 *    个无效值，分别是：INVALID_NUM(数字类型)、""(字符串类型)、CGUID.GUID_INVALID(GUID类型)；
 *    判断一个GUID是否有效（不为空），还可以调用函数 CGUID::IsInvalid。
 * @section declare_global LUA脚本中全局变量的使用
 * LUA全局变量可用于不同脚本之间的数据传递，但是默认情况下是不能定义全局变量的，例如：
 * @code a = 1 @endcode 
 * 无法正常定义全局变量，取而代之的是使用特定的接口来定义：
 * @code
 * declare( varName, initVal ) -- 定义全局变量，参数分别为：变量名、变量初始值
 * undeclare( varName ) -- 删除全局变量，参数为：变量名
 * -- 例子：
 * declare( "a", { [1] = 2 } )
 * undeclare( "a" )
 * @endcode
 * 可以判断一个变量是否为nil来确定其是否被定义：
 * @code
 * if a == nil then ....
 * @endcode
 *
 */

