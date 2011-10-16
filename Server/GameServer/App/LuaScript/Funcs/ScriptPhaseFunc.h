/// 
/// @file  ScriptPhaseFunc.h
/// @brief 相位功能的一些脚本函数
/// 

/// 
/// 针对相位功能的创建.. 此脚本只能针对野战类型
/// @param   size1      范围1的大小
/// @param   size2      范围2的大小
/// @param   trigger    触发器主被动类型 (0为被动，1为主动)
/// @param   scriptfile 运行脚本文件
/// @return  成功与否
/// 
bool   CreatePhase(float size1,float size2,long trigger,const char* scriptfile);

/// 
/// 激活相位功能,进入状态
/// @param etype      计数器类型(add,sub)
/// @param lTimer     定时器触发时间
/// @param lExipires  定时器周期时间(0,1)
/// 
void   ActivePhase(long etype,long lTimer,long lExipires);

/// 
/// 得到相位的属性值
/// @param  pName  传入的属性名参数
/// @return 返回参数值
/// 
long  GetPhasePoint(const char* pName);

/// 
/// 得到相位的属性值
/// @param  GID    相位ID
/// @param  pName  传入的属性名参数
/// @return 返回参数值
///  
long  GetPhasePoint(const CGUID& id , const char* pName);

///
/// 在相位区域中创建一个指定的怪物
/// @param  OrgName   怪物的原始名
/// @param  x_point   相对相位的绝对坐标
/// 
void CreatePhaseMonster(const char* OrgName,int Amount,int x_point,int y_point,
                        int Width,int Height,int rgnId,const char* scriptFileName,
                        const char* talkFileName,int dir);

/// 
/// 相位操作
/// @param  Oper   eRemit=1, //传送  
///                eLeave=2, //离开(离队,队长驱逐,退出游戏) 
///                eEscape=3,//逃跑 
///                eReJoin=4,//归队操作
/// 
long  OnPhaseOper(long Oper);

/// 
/// 相位事件
/// @param  Event  eExit=1,   //正常结束事件触发  
///                eDie=2,    死亡 
///                eSecKill=3,秒杀 
///                eAgonal=4, 玩家濒死状态
/// 
long  OnPhaseEvent(long eExit);
   
/// 
/// 设置相位结束
/// 
bool   SetPhaseEnd();

/// 
/// 关于相位结束的触发
/// @param  Filename   相位条件结束触发的脚本
/// 
void   SetPhaseEndScriptFile(const char* Filename);

///
/// 关闭指定相位相位功能
/// @param  PhaseId   关闭指定的相位
/// 
void   ClosePhase(const CGUID&  PhaseId);

///
/// 关闭玩家所在相位相位功能
/// 
void   ClosePhase();

/// 
/// 返回相位的ID
/// 
const CGUID& GetPhaseID();

///
/// 返回玩家相位ID
/// 
const CGUID& GetShapePhaseID();

/// 
/// 得到相位操作时间
/// 
long  GetCostTime(const CGUID&  PhaseId);

/// 
/// 关闭定时器
/// 
void  CancelPhaseTimer(const CGUID&  PhaseId);

///
/// 给相位一个发送一个消息
/// 
void  PostPhaseMsg(const CGUID& PhaseId,const char* pMsg,ulong dwColor =0, ulong dwBkColor=0,long ePos=1);
