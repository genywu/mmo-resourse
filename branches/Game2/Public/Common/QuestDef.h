/**
* @filename:QuestDef.h
* @date: 2010/7/7
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 任务模块的宏定义
*/

namespace QS
{
	///< 消息操作类型
	enum {
		QS_ADD,
		QS_REMOVE,
		QS_COMPLETE,
		QS_UPDATE_STEP,
		QS_UPDATE_PARAM,
		QS_UPDATE_SHARELIST,
		QS_UPDATE_DATA,
		QS_UPDATE_RECORD,
		QS_UPDATE_MER,
		QS_OPEN_UI,
		QS_SHARE_OPEN,
		QS_SHARE_CUR,
		QS_TALK,
		QS_ACCPET,	//接收任务
		QS_DISBAND,	//放弃任务
		QS_SHARE,	//共享任务
        QS_SHARE_Ret,//共享回馈
	};
}