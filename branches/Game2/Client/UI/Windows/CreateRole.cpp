/**
* @filename:CreateRole.cpp
* @date: 2010/7/12
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 角色创建部分
*/

/////////////////////////////////////////////////
// zhaohang  2010/7/12 
// 恢复角色的性别，脸型，发型，发色选择功能，其他功能在UI/CreateRole目录下面
////////////////////////////////////////////////
#include "stdafx.h"
#include "CreateRoleDef.h"
#include "../UIDef.h"
#include "../GamePage/CreateRole/CreateRoleEvent.h"
#include "../../GameClient/ClientApp/Other/CountryList.h"
///<设值界面初始化状态的属性和事件注册
void SetCreateRoleInitProperty(CEGUI::Window* pgWnd);

///< 国家变更
bool OnCountryChanged(const CEGUI::EventArgs& e);
///< 角色性别变更
bool OnSexChanged(const CEGUI::EventArgs& e);
///< 脸型变更
bool OnFacChanged(const CEGUI::EventArgs& e);
///< 发型变更
bool OnHairChanged(const CEGUI::EventArgs& e);
///< 发色变更
bool OnHairColorChanged(const CEGUI::EventArgs& e);
///< 根据选则的角色，重新填充脸型，发型，可选UI数据
void ResetDataBySexSelChanged();

///< 根据选择的发型，重新填充发色可选UI数据
void ResetHairColorDateOnHairChanged();

void SetCreateRoleInitProperty(CEGUI::Window* pgWnd)
{
	if(!pgWnd)
		return;
	CEGUI::Combobox* SelCountry = WComboBox(pgWnd->getChildRecursive(CREATEROLE_SEL_COUNTRY_CCB));
	if(SelCountry)
	{
		SelCountry->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,CEGUI::SubscriberSlot(OnCountryChanged));
		SelCountry->setReadOnly(true);
		CCountryList::MapCountryList* mapCL = CCountryList::GetCountryList();
		for(uint cnt = 0; cnt < mapCL->size(); ++cnt)
		{
			const char* ctName = CCountryList::GetCountryName((BYTE)cnt+1);
			if(ctName)
			{
				CEGUI::ListboxTextItem* lti = new CEGUI::ListboxTextItem(ToCEGUIString(ctName));
				lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
				lti->setID(cnt+1);//ID和国家ID关联
				if(cnt == 0)
					lti->setSelected(true);//设置国家默认值
				SelCountry->addItem(lti);
			}
		}
		//根据国家默认值，设置显示文本
		SelCountry->getEditbox()->setText(ToCEGUIString(CCountryList::GetCountryName(0+1)));//加一是因为data/CountryList.xml配置造成
		/***********************************************************************/
		/* zhaohang fix 2010-9-3
		/***********************************************************************/
		CREvent::SetSelectCountry(1);//逻辑上的国家默认值
	}
	CEGUI::Combobox* selHair = WComboBox(pgWnd->getChildRecursive(CREATEROLE_SEL_HAIR_CCB));
	if(selHair)
	{
		selHair->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,CEGUI::SubscriberSlot(OnHairChanged));
		selHair->setReadOnly(true);
	}
	CEGUI::Combobox* selHairColor = WComboBox(pgWnd->getChildRecursive(CREATEROLE_SEL_HAIRCOLOR_CCB));
	if(selHairColor)
	{
		selHairColor->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,CEGUI::SubscriberSlot(OnHairColorChanged));
		selHairColor->setReadOnly(true);
	}
	CEGUI::Combobox* selFac = WComboBox(pgWnd->getChildRecursive(CREATEROLE_SEL_FAC_CCB));
	if(selFac)
	{
		selFac->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,CEGUI::SubscriberSlot(OnFacChanged));
		selFac->setReadOnly(true);
	}
	CEGUI::Combobox* selSex = WComboBox(pgWnd->getChildRecursive(CREATEROLE_SEL_SEX_CCB));
	if(selSex)
	{
		CEGUI::ListboxTextItem* itm1  = new CEGUI::ListboxTextItem(ToCEGUIString(CREATEROLE_SEX_MALE));
		itm1->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		itm1->setID(0);//用ID和性别关联
		selSex->addItem(itm1);
		CEGUI::ListboxTextItem* itm2 = new CEGUI::ListboxTextItem(ToCEGUIString(CREATEROLE_SEX_FAMALE));
		itm2->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		itm2->setID(1);//用ID和性别关联
		selSex->addItem(itm2);
		//注册事件
		selSex->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,CEGUI::SubscriberSlot(OnSexChanged));
		selSex->setReadOnly(true);
	}
}

void ResetDataBySexSelChanged()
{
	CEGUI::WindowManager& mgr = GetWndMgr();
	CEGUI::Combobox* SelFac = WComboBox(mgr.getWindow(CREATEROLE_SEL_FAC_CCB));
	uint SelSex = (uint)CREvent::GetSelectSex();
	if(SelFac)
	{
		SelFac->resetList();
		//更改性别后，修改默认Face和HairStyle时,对应修改文本显示
		SelFac->getEditbox()->setText(CEGUI::PropertyHelper::intToString(0));
		for(short i = 0 ; i < CREvent::GetFaceNum(SelSex) ; ++i)
		{
			CEGUI::ListboxTextItem* lti = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::intToString(i));
			lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
			lti->setID(i);//ID和FacIndex关联
			if(0==i)		//更改性别后，修改默认Face和HairStyle时,对应修改ItemList选中状态
				lti->setSelected(true);
			SelFac->addItem(lti);
		}
	}
	CEGUI::Combobox* SelHair = WComboBox(mgr.getWindow(CREATEROLE_SEL_HAIR_CCB));
	if(SelHair)
	{
		SelHair->resetList();
		//更改性别后，修改默认Face和HairStyle时,对应修改文本显示
		SelHair->getEditbox()->setText(CEGUI::PropertyHelper::intToString(0));
		for(short i = 0 ; i < CREvent::GetHairNum(SelSex) ; ++i)
		{
			CEGUI::ListboxTextItem* lti = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::intToString(i));
			lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
			lti->setID(i);// ID和HairIndex关联
			if(0 == i)//更改性别后，修改默认Face和HairStyle时,对应修改ItemList选中状态
				lti->setSelected(true);
			SelHair->addItem(lti);
		}
	}
}

void ResetHairColorDateOnHairChanged()
{
	CEGUI::WindowManager& mgr = GetWndMgr();
	CEGUI::Combobox* hairColor = WComboBox(mgr.getWindow(CREATEROLE_SEL_HAIRCOLOR_CCB));
	if(hairColor)
	{
		hairColor->resetList();
		//清空发色类型文本
		hairColor->getEditbox()->setText("");
		for(short i = 0 ; i < CREvent::GetHairColorNum(CREvent::GetSelectSex(),(WORD)CREvent::GetHair()) ; ++i)
		{
			CEGUI::ListboxTextItem* lti = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::intToString(i));
			lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
			lti->setID(i);//ID和HairColorIndex关联
			hairColor->addItem(lti);
		}
	}
}

bool OnSexChanged(const CEGUI::EventArgs& e)
{
	CEGUI::Combobox* cbb = WComboBox(WEArgs(e).window);
	CEGUI::ListboxItem* lti = cbb->getSelectedItem();
	if(lti)
	{
		CREvent::SetSelectSex(lti->getID());
		//更改性别后，修改默认Face和HairStyle,使得模型能够正常显示
		CREvent::SetFace(0);
		CREvent::SetHairStyle(0);
	}
	else
	{
		CREvent::SetSelectSex(0);
		//更改性别后，修改默认Face和HairStyle,使得模型能够正常显示
		CREvent::SetFace(0);
		CREvent::SetHairStyle(0);
	}
	ResetDataBySexSelChanged();
	return true;
}

bool OnFacChanged(const CEGUI::EventArgs& e)
{
	CEGUI::Combobox* cbb = WComboBox(WEArgs(e).window);
	CEGUI::ListboxItem* lti = cbb->getSelectedItem();
	if(lti)
		CREvent::SetFace(lti->getID());
	else
		CREvent::SetFace(0);
	return true;
}

bool OnHairChanged(const CEGUI::EventArgs& e)
{
	CEGUI::Combobox* cbb = WComboBox(WEArgs(e).window);
	CEGUI::ListboxItem* lti = cbb->getSelectedItem();
	if(lti)
		CREvent::SetHairStyle(lti->getID());
	else
		CREvent::SetHairStyle(0);
	ResetHairColorDateOnHairChanged();
	return true;
}
bool OnHairColorChanged(const CEGUI::EventArgs& e)
{
	CEGUI::Combobox* cbb = WComboBox(WEArgs(e).window);
	CEGUI::ListboxItem* lti = cbb->getSelectedItem();
	if(lti)
		CREvent::SetHairColor(lti->getID());
	else
		CREvent::SetHairColor(0);
	return true;
}

bool OnCountryChanged(const CEGUI::EventArgs& e)
{
	CEGUI::Combobox* cbb = WComboBox(WEArgs(e).window);
	CEGUI::ListboxItem* lti = cbb->getSelectedItem();
	if(lti)
		CREvent::SetSelectCountry(lti->getID());
	else
		CREvent::SetSelectCountry(1);//range由Data/CountryList.xml 配置决定,这里根据配置设置默认国家ID为1
	return true;
}