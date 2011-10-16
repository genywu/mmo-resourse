/********************************************************************
	created:	2010/10/18
	created:	18:10:2010   9:55
	filename: 	e:\SVN_P6\trunk\Client\GameClient\ClientApp\message\geniusMessage.cpp
	file path:	e:\SVN_P6\trunk\Client\GameClient\ClientApp\message
	file base:	geniusMessage
	file ext:	cpp
	author:		zhaohang(kaba) (kuailekaba@gmail.com)
	
	purpose:	天赋消息模块
*********************************************************************/
#include "stdafx.h"
#include "../../../Net/Message.h"
#include "../Skills/SkillListXml.h"
//响应天赋消息
void OnGeniusMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
		//更新技能属性
	case MSG_S2C_GENIUS_SKILLUPDATE:
		//更新模块结构
		// 1.SkillID ------技能ID 
		// 2.SkillLevel ---技能等级
		// 3.size ------更新技能属性项数 
		//     |__ strKey ----属性项的关键字
		//     |__ value  ----属性项的值
		long SkillID = pMsg->GetDWord();
		long SkillLevel = pMsg->GetLong();
		long size = pMsg->GetLong();
		std::vector<string>vecStrKey;
		std::vector<long>vecValue;
		for(long i = 0 ;i < size ; i++)
		{
			char temp[64];
			vecStrKey.push_back(pMsg->GetStr(temp,64));
			vecValue.push_back(pMsg->GetLong());
		}
		//调用更新接口,更新全局技能模块属性
		UpdateSkillXmlList(SkillID,SkillLevel,vecStrKey,vecValue);
		break;
	}
}