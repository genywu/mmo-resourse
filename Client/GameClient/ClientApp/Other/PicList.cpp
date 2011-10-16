#include "StdAfx.h"
#include "../BaseObject.h"
#include "../shape.h"
#include "../player.h"
#include "PicList.h"
#include "../../EngineExtend/CBitmapX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPicList::CPicList(void)
{	
}

CPicList::~CPicList(void)
{
	ReleaseAll();	
}

CPicList::tagBitmap* CPicList::Load(long lType, long lGraphicsID, bool bFind)
{
	// 要求查找以有的图像资源
	if( bFind == true )
	{
		tagBitmap* pBitmap = GetBitmap( lType, lGraphicsID );
		if( pBitmap )	// 查找到图像
		{
			pBitmap->lUseCount++;	// 使用数加1
			return pBitmap;
		}
	}

	// 从文件读取图像数据
	tagBitmap stBitmap;
	stBitmap.lType = lType;
	stBitmap.lGraphicsID = lGraphicsID;
	stBitmap.lUseCount = 1;

	char strbuf[MAX_PATH];
	switch(lType)
	{
	case PT_STATE_ICON:
		sprintf(strbuf,	"pictures\\states\\%d.jpg", lGraphicsID);
		break;

	case PT_SKILL_ICON:
		{
			// 基本攻击类技能，使用统一图标
			//if(lGraphicsID >= SKILL_BASE_ATTACK && lGraphicsID <= SKILL_BASE_BLADEFIST)
			//	lGraphicsID = SKILL_BASE_ATTACK;
			sprintf(strbuf,	"pictures\\skills\\%d.jpg", lGraphicsID);
		}
		break;

	case PT_SKILL_ICON_GRAY:
		{
			// 基本攻击类技能，使用统一图标
			//if(lGraphicsID >= SKILL_BASE_ATTACK && lGraphicsID <= SKILL_BASE_BLADEFIST)
			//	lGraphicsID = SKILL_BASE_ATTACK;
			sprintf(strbuf,	"pictures\\skills\\%d.jpg", lGraphicsID);
		}
		break;

	case PT_COUNTRY_ICON:
		sprintf(strbuf,	"pictures\\country\\%d.tga", lGraphicsID);
		break;

	case PT_PLAYER_COUNTRY_ICON:
		sprintf(strbuf,	"pictures\\player-country\\%d.tga", lGraphicsID);
		break;
	case PT_GOODS_ICON:
		sprintf(strbuf,	"pictures\\goods\\%d.jpg", lGraphicsID);
		break;
	case PT_QUEST_ICON:
		sprintf(strbuf,	"pictures\\quest\\%d.jpg", lGraphicsID);
		break;
	case PT_MONSTER_ICON:
		sprintf(strbuf,	"pictures\\monstericon\\%d.tga", lGraphicsID);
		break;
	case PT_MINIMAP_ICON:
		sprintf(strbuf,	"pictures\\tsicon\\%d.tga", lGraphicsID);
		break;
	case PT_TALKBOX_ICON:
		sprintf(strbuf,	"pictures\\talkbox\\%d.jpg", lGraphicsID);
		break;
	case PT_TEXT_ICON:
		sprintf(strbuf,	"pictures\\text\\%d.tga", lGraphicsID);
		break;
	case PT_MONSTER_HEAD_ICON:
		sprintf(strbuf,	"pictures\\monsterhead\\%d.tga", lGraphicsID);
		break;
	case PT_TSGOODS_ICON:
		sprintf(strbuf,	"pictures\\TsGoods\\%d.jpg", lGraphicsID);
		break;
	default:		
		return NULL;
	}

	stBitmap.pBitmap = CBitmapX::GetBitmapX(strbuf);

	m_listPic.push_back(stBitmap); 	
	list<tagBitmap>::iterator iter=m_listPic.end();
	iter--;
	tagBitmap *pstBitmap=&(*iter);
	return pstBitmap;
}

const char * CPicList::GetPicFilePathName(long lType, long lGraphicsID)
{
    static char strbuf[MAX_PATH];
    strcpy(strbuf,"");
    switch(lType)
    {
    case PT_STATE_ICON:
        sprintf(strbuf,	"pictures\\states\\%d.jpg", lGraphicsID);
        break;

    case PT_SKILL_ICON:
        {
            // 基本攻击类技能，使用统一图标
            //if(lGraphicsID >= SKILL_BASE_ATTACK && lGraphicsID <= SKILL_BASE_BLADEFIST)
            //	lGraphicsID = SKILL_BASE_ATTACK;
            sprintf(strbuf,	"pictures\\skills\\%d.jpg", lGraphicsID);
        }
        break;

    case PT_SKILL_ICON_GRAY:
        {
            // 基本攻击类技能，使用统一图标
            //if(lGraphicsID >= SKILL_BASE_ATTACK && lGraphicsID <= SKILL_BASE_BLADEFIST)
            //	lGraphicsID = SKILL_BASE_ATTACK;
            sprintf(strbuf,	"pictures\\skills\\%d.jpg", lGraphicsID);
        }
        break;

    case PT_COUNTRY_ICON:
        sprintf(strbuf,	"pictures\\country\\%d.tga", lGraphicsID);
        break;

    case PT_PLAYER_COUNTRY_ICON:
        sprintf(strbuf,	"pictures\\player-country\\%d.tga", lGraphicsID);
        break;
    case PT_GOODS_ICON:
        sprintf(strbuf,	"pictures\\goods\\%d.jpg", lGraphicsID);
        break;
    case PT_QUEST_ICON:
        sprintf(strbuf,	"pictures\\quest\\%d.jpg", lGraphicsID);
        break;
    case PT_MONSTER_ICON:
        sprintf(strbuf,	"pictures\\monstericon\\%d.tga", lGraphicsID);
        break;
    case PT_MINIMAP_ICON:
        sprintf(strbuf,	"pictures\\tsicon\\%d.tga", lGraphicsID);
        break;
    case PT_TALKBOX_ICON:
        sprintf(strbuf,	"pictures\\talkbox\\%d.jpg", lGraphicsID);
        break;
    case PT_TEXT_ICON:
        sprintf(strbuf,	"pictures\\text\\%d.tga", lGraphicsID);
        break;
    case PT_MONSTER_HEAD_ICON:
        sprintf(strbuf,	"pictures\\monsterhead\\%d.tga", lGraphicsID);
        break;
    case PT_TSGOODS_ICON:
        sprintf(strbuf,	"pictures\\TsGoods\\%d.jpg", lGraphicsID);
        break;
    default:		
        return NULL;
    }
    return strbuf;
}

BOOL CPicList::Release(long lType, long lGraphicsID)
{
	tagBitmap stBitmap;

	// 查找是否有该资源
	list<tagBitmap>::iterator it;
	for(it = m_listPic.begin(); it != m_listPic.end(); it++)
	{
		stBitmap = *it;

		// 资源列表中有这个资源
		if( stBitmap.lType == lType && stBitmap.lGraphicsID == lGraphicsID )
		{
			stBitmap.lUseCount--;
			if( stBitmap.lUseCount == 0 )
			{			
				SAFE_DECREF(stBitmap.pBitmap);
				m_listPic.erase( it );
			}
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CPicList::ReleaseAll(void)
{
	tagBitmap stBitmap;

	// 查找是否有该资源
	list<tagBitmap>::iterator it;
	for( it = m_listPic.begin(); it != m_listPic.end(); it++ )
	{
		stBitmap =*it;		
		SAFE_DECREF(stBitmap.pBitmap);		
	}
	m_listPic.clear();

	return TRUE;
}

BOOL CPicList::Display(long lType,long lGraphicsID,long x, long y, DWORD color, RECT *rectClip)
{
	BOOL ret = FALSE;

	tagBitmap* pBitmap = GetBitmap( lType, lGraphicsID );
	if( pBitmap == NULL )
	{
		// 读图, 不用查找是否有这个图像资源, 因为这里查过了
		pBitmap = Load(lType, lGraphicsID, false);
	}

	if( pBitmap && pBitmap->pBitmap )
	{
		if (rectClip)
			pBitmap->pBitmap->SetClip(rectClip->left, rectClip->top, rectClip->right, rectClip->bottom);
		else
			pBitmap->pBitmap->SetClip(0,0,0,0);
		pBitmap->pBitmap->Draw(x,y,color);
	}

	return ret;
}

// 拉伸显示
BOOL CPicList::Display(long lType,long lGraphicsID,long x, long y, long w, long h, DWORD color)
{
	BOOL ret = FALSE;

	tagBitmap* pBitmap = GetBitmap( lType, lGraphicsID );
	if( pBitmap == NULL )
	{
		// 读图, 不用查找是否有这个图像资源, 因为这里查过了
		pBitmap = Load(lType, lGraphicsID, false);
	}
	if( pBitmap && pBitmap->pBitmap )
	{
		pBitmap->pBitmap->SetColor(color);
		pBitmap->pBitmap->Draw(x,y,(float)w,(float)h);
	}

	return ret;
}

// 获取
CPicList::tagBitmap* CPicList::GetBitmap(long lType, long lGraphicsID)
{
	tagBitmap* pBitmap = NULL;

	// 查找是否有该资源
	list<tagBitmap>::iterator it;
	for(it = m_listPic.begin(); it != m_listPic.end(); it++)
	{
		pBitmap = &(*it);

		// 资源列表中有这个资源
		if( pBitmap->lType == lType && pBitmap->lGraphicsID == lGraphicsID )
		{
			return pBitmap;
		}
	}

	return NULL;
}

// 获取
CPicList::tagBitmap* CPicList::GetBitmap(CBitmapX* pBitmapX)
{
	tagBitmap* pBitmap = NULL;
	// 查找是否有该资源
	list<tagBitmap>::iterator it;
	for(it = m_listPic.begin(); it != m_listPic.end(); it++)
	{
		pBitmap = &(*it);

		// 资源列表中有这个资源
		if( pBitmap->pBitmap == pBitmapX )
		{
			return pBitmap;
		}
	}
	return NULL;
}