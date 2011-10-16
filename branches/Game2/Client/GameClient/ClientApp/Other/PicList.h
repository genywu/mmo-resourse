#pragma once

class CBitmapX;
class CShape;

// 模型管理类
class CPicList
{
public:
	enum ePicType
	{
		PT_STATE_ICON,			// 状态图标
		PT_SKILL_ICON,			// 技能图标
		PT_SKILL_ICON_GRAY,		// 技能图标灰色
		PT_COUNTRY_ICON,		// 人物名字边上的国家标记
		PT_PLAYER_COUNTRY_ICON,	// 属性面板的国家标记
		PT_GOODS_ICON,			// 背包的物品图标
		//--------------07.9.10liuke添加任务图标------------
		PT_QUEST_ICON,			// 任务的图片图标
		PT_MONSTER_ICON,		// 怪物的称号图标
		PT_MINIMAP_ICON,		// 小地图上的特殊图标
		PT_TALKBOX_ICON,		// talkbox中的附加背景图标
		PT_TEXT_ICON,			// talkbox中的图标
		//--------------------------------------------------
		PT_MONSTER_HEAD_ICON,	// 怪物头像图片
		PT_TSGOODS_ICON,		// 增值商店物品图片
		PT_PET_ICON,			// 宠物头像图片
	};

	CPicList(void);
	virtual ~CPicList(void);

	// 模型结构
	struct tagBitmap
	{
		long		lType;			// 类型
		long		lGraphicsID;	// 图片编号
		DWORD		lUseCount;		// 使用的次数
		CBitmapX	*pBitmap;		// 模型
		tagBitmap()
		{
			lType		= -1;
			lGraphicsID = 0;
			lUseCount	= 0;
			pBitmap		= NULL;
		}
	};

protected:
	list<tagBitmap>	m_listPic;					// 模型列表

public:
	tagBitmap* Load(long lType, long lGraphicsID, bool bFind = true);		// 载入一套资源( 在载入之前是否要查找要求载入的图像是否存在 )
	BOOL Release(long lType, long lGraphicsID);		// 释放一套资源
	BOOL ReleaseAll(void);							// 釋放所有資源

	// 显示
	BOOL Display(long lType,long lGraphicsID,long x, long y, DWORD color = 0xffffffff, RECT *rectClip=NULL);
	// 拉伸显示
	BOOL Display(long lType,long lGraphicsID,long x, long y, long w, long h, DWORD color = 0xffffffff);

	// 获取
	tagBitmap* GetBitmap(long lType, long lGraphicsID);	// 获取

	tagBitmap* GetBitmap(CBitmapX* pBitmapX);			// 获取

    //获取图片路径文件名
    const char * GetPicFilePathName(long lType, long lGraphicsID);
};
