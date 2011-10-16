#include "stdafx.h"
#include "ModelTools.h"
#include "../../../Public/Common/BaseDef.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//根据type和id得到模型资源路径
void GetModelFormatString(long lType,long lID,char* pszPath)
{
	switch(lType)
	{
	case TYPE_PLAYER:
		sprintf(pszPath,"model/player/%03d/model", lID);
		break;
	case TYPE_COLLECTION:
		sprintf(pszPath,"model/collection/%03d/model", lID);
		break;
	case TYPE_PET:
		sprintf(pszPath,"model/Pet/%03d/model", lID);
		break;
	case TYPE_MONSTER:
		sprintf(pszPath,"model/monster/%03d/model", lID);
		break;
	case TYPE_GOODS:
		sprintf(pszPath,"model/goods/%03d/model", lID);
		break;
	case TYPE_EFFECT:
		sprintf(pszPath,"model/effect/%03d/model", lID);
		break;
	case TYPE_SKILL:
		sprintf(pszPath,"model/skill/%03d/model", lID);
		break;
	case TYPE_BUILD:
	case TYPE_CITYGATE:
		sprintf(pszPath,"model/Construction/%03d/model", lID);
		break;
	case TYPE_TALKBOXMODEL:
		sprintf(pszPath,"model/TalkBoxModel/%03d/model", lID);
		break;
	default:
		sprintf(pszPath,"");
		break;
	}
	return;
}