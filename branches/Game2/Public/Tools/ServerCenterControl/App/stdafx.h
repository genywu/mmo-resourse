// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define DONT_USE_DB


#include "../../../../Public/DefPublic.h"
#include "../../../../Public/ServerPublic/ServerPublic.h"
#include "../../../../Public/jsonlib/jsonlib/json/json.h"

#include "../Net/AinMessage.h"
#include "../Net/Message.h"

#include "../../../SCMPublic/def/MsgGMT_SCC2SM.h"
#include "../../../SCMPublic/def/MsgGMT_SCC2SUC.h"
#include "../../../SCMPublic/def/Unvarying.h"
#include "../../../SCMPublic/class/LogTools.h"
#include "../../../SCMPublic/def/InfoDef.h"


#include "../ServerCenterControl/ServiceSCC.h"


#include "Setup/AinMsgType.h"
#include "AppClass/ActionLogManage.h"