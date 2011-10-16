//********************************************
//	寻路 相关函数 soft
//  创建于2000年3月22日
//	最后修改于2006年7月
//********************************************

#include "stdafx.h"
#include "../region.h"
#include "RegionList.h"
#include "findpath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//构造
CFindPath::CFindPath()
{
	m_listPath.clear();
	m_mapOpen.clear();
	m_mapClose.clear();
}


//构造
CFindPath::CFindPath(CRegion *cm)
{
	Width=cm->GetWidth();
	Height=cm->GetHeight();
	m_pMap = cm;

	m_listPath.clear();
	m_mapOpen.clear();
	m_mapClose.clear();

	Init(Width, Height);
}

//析构
CFindPath::~CFindPath()
{
	m_listPath.clear();
	Release();
}

//
void CFindPath::SetRegion(CRegion *pRegion)
{
	Width=pRegion->GetWidth();
	Height=pRegion->GetHeight();
	m_pMap = pRegion;

	Init(Width, Height);
}

//初始化节点数组
void CFindPath::Init(int w, int h)
{
	Width=w;
	Height=h;

	m_mapOpen.clear();
	m_mapClose.clear();
}

//释放
void CFindPath::Release()
{
}

//************************************************
//主函数
bool CFindPath::Find(int x1,int y1, int x2, int y2)
{
	m_listPath.clear();
	DWORD dwStartTime = timeGetTime();

	//起始点
	sx=x1;
	sy=y1;
	//目的地
	ex=x2;
	ey=y2;

	if( sx < 0 || sx >= Width || sy < 0 || sy >= Height )
	{
		return false;
	}

	const int RangeW = m_pMap->GetWidth();
	const int RangeH = m_pMap->GetHeight();

	//求出寻路范围
	Range.left=sx-RangeW;
	if( Range.left<0 ) Range.left=0;

	Range.right=Range.left+RangeW*2+1;
	if( Range.right>Width ) Range.right=Width;

	Range.top=sy-RangeH;
	if( Range.top<0 ) Range.top=0;

	Range.bottom=Range.top+RangeH*2+1;
	if( Range.bottom>Height ) Range.bottom=Height;

	// 初始化
	m_mapOpen.clear();
	m_mapClose.clear();

	// 把起始点加入到待处理列表
	tagNote stNote;
	stNote.step = 0;
	stNote.x = sx;
	stNote.y = sy;
	stNote.ox = stNote.oy = -1;
	stNote.fStep = 0.f;
	stNote.fJudge = Judge(sx, sy);
	m_mapOpen[m_pMap->GetWidth() * sy + sx] = stNote;

	//主循环
	while(1)
	{
		//遍历待处理节点MAP
		//找出fJudge+fStep最小的节点
		//返回值p=格子的下标
		int p=FindShortest();

		// 如果到不了目的点则到离目标点最近的一点
		if( p==-1 )
		{
			p=FindShortestInClose();
			ex=m_mapClose[p].x;
			ey=m_mapClose[p].y;

			// 重新初始化
			m_mapOpen.clear();
			m_mapClose.clear();

			// 把起始点加入到待处理列表
			tagNote stNote;
			stNote.step = 0;
			stNote.x = sx;
			stNote.y = sy;
			stNote.ox = stNote.oy = -1;
			stNote.fStep = 0.f;
			stNote.fJudge = Judge(sx, sy);
			m_mapOpen[m_pMap->GetWidth() * sy + sx] = stNote;

			continue;	//重来
		}

		//找到路径 或 超时
		if( timeGetTime() > dwStartTime + 6000 || abs(m_mapOpen[p].x-ex) < 1 && abs(m_mapOpen[p].y-ey)  < 1 )
		{
			long lTheSteps = m_mapOpen[p].step+2;
			stPos pPoint;
			int ix = m_mapOpen[p].ox, iy = m_mapOpen[p].oy;
			int i = InClose(ix, iy);

			for(int n = lTheSteps; n >= 0; n--)
			{
				if (ix <= 0 && iy <= 0)
					break;
				pPoint.x = ix;
				pPoint.y = iy;
				m_listPath.push_front(pPoint);

				//得到上一点的位置
				i = InClose(ix, iy);
				ix = m_mapClose[i].ox;
				iy = m_mapClose[i].oy;
			}
			pPoint.x = m_mapOpen[p].x;
			pPoint.y = m_mapOpen[p].y;
			m_listPath.push_back(pPoint);
			return true;
		}

		int fx = m_mapOpen[p].x;
		int fy = m_mapOpen[p].y;
		int oldx = m_mapOpen[p].ox;
		int oldy = m_mapOpen[p].oy;
		int nStep = m_mapOpen[p].step + 1;
		float fStep1 = m_mapOpen[p].fStep + 1.414f;
		float fStep2 = m_mapOpen[p].fStep + 1.f;
		long dir = GetLineDir((long)oldx, (long)oldy,(long) fx,(long)fy);

		Try( fx-1, fy-1 , nStep, fStep1, fx, fy, dir, 1.414f);	//左上
		Try( fx+1, fy-1 , nStep, fStep1, fx, fy, dir, 1.414f);	//右上
		Try( fx+1, fy+1 , nStep, fStep1, fx, fy, dir, 1.414f);	//右下
		Try( fx-1, fy+1 , nStep, fStep1, fx, fy, dir, 1.414f);	//左下

		Try( fx, fy-1 , nStep, fStep2, fx, fy, dir, 1.f);	//上
		Try( fx, fy+1 , nStep, fStep2, fx, fy, dir, 1.f);	//下
		Try( fx-1, fy , nStep, fStep2, fx, fy, dir, 1.f);	//左
		Try( fx+1, fy , nStep, fStep2, fx, fy, dir, 1.f);	//右

		// 加入到CLOSE表
		if( InClose(m_mapOpen[p].x, m_mapOpen[p].y) == -1 )
			AddToClose( m_mapOpen[p] );

		// 从待处理列表移出
		m_mapOpen.erase(p);
	}
}
bool CFindPath::Find(int x1,int y1, int x2, int y2,int lWidth, int lHieht)
{
	m_listPath.clear();
	DWORD dwStartTime = timeGetTime();

	//起始点
	sx=x1;
	sy=y1;
	//目的地
	ex=x2;
	ey=y2;

	if( sx < 0 || sx >= Width || sy < 0 || sy >= Height )
	{
		return false;
	}

	const int RangeW = lWidth;
	const int RangeH = lHieht;

	//求出寻路范围
	Range.left=sx-RangeW;
	if( Range.left<0 ) Range.left=0;

	Range.right=Range.left+RangeW*2+1;
	if( Range.right>Width ) Range.right=Width;

	Range.top=sy-RangeH;
	if( Range.top<0 ) Range.top=0;

	Range.bottom=Range.top+RangeH*2+1;
	if( Range.bottom>Height ) Range.bottom=Height;

	// 初始化
	m_mapOpen.clear();
	m_mapClose.clear();

	// 把起始点加入到待处理列表
	tagNote stNote;
	stNote.step = 0;
	stNote.x = sx;
	stNote.y = sy;
	stNote.ox = stNote.oy = -1;
	stNote.fStep = 0.f;
	stNote.fJudge = Judge(sx, sy);
	m_mapOpen[m_pMap->GetWidth() * sy + sx] = stNote;

	//主循环
	while(1)
	{
		//遍历待处理节点MAP
		//找出fJudge+fStep最小的节点
		//返回值p=格子的下标
		int p=FindShortest();

		// 如果到不了目的点则到离目标点最近的一点
		if( p==-1 )
		{
			p=FindShortestInClose();
			ex=m_mapClose[p].x;
			ey=m_mapClose[p].y;

			// 重新初始化
			m_mapOpen.clear();
			m_mapClose.clear();

			// 把起始点加入到待处理列表
			tagNote stNote;
			stNote.step = 0;
			stNote.x = sx;
			stNote.y = sy;
			stNote.ox = stNote.oy = -1;
			stNote.fStep = 0.f;
			stNote.fJudge = Judge(sx, sy);
			m_mapOpen[m_pMap->GetWidth() * sy + sx] = stNote;

			continue;	//重来
		}

		//找到路径 或 超时
		if( timeGetTime() > dwStartTime + 6000 || abs(m_mapOpen[p].x-ex) < 1 && abs(m_mapOpen[p].y-ey)  < 1 )
		{
			long lTheSteps = m_mapOpen[p].step+2;
			stPos pPoint;
			int ix = m_mapOpen[p].ox, iy = m_mapOpen[p].oy;
			int i = InClose(ix, iy);

			for(int n = lTheSteps; n >= 0; n--)
			{
				if (ix <= 0 && iy <= 0)
					break;
				pPoint.x = ix;
				pPoint.y = iy;
				m_listPath.push_front(pPoint);

				//得到上一点的位置
				i = InClose(ix, iy);
				ix = m_mapClose[i].ox;
				iy = m_mapClose[i].oy;
			}
			pPoint.x = m_mapOpen[p].x;
			pPoint.y = m_mapOpen[p].y;
			m_listPath.push_back(pPoint);
			return true;
		}

		int fx = m_mapOpen[p].x;
		int fy = m_mapOpen[p].y;
		int oldx = m_mapOpen[p].ox;
		int oldy = m_mapOpen[p].oy;
		int nStep = m_mapOpen[p].step + 1;
		float fStep1 = m_mapOpen[p].fStep + 1.414f;
		float fStep2 = m_mapOpen[p].fStep + 1.f;
		long dir = GetLineDir((long)oldx, (long)oldy,(long) fx,(long)fy);

		Try( fx-1, fy-1 , nStep, fStep1, fx, fy, dir, 1.414f);	//左上
		Try( fx+1, fy-1 , nStep, fStep1, fx, fy, dir, 1.414f);	//右上
		Try( fx+1, fy+1 , nStep, fStep1, fx, fy, dir, 1.414f);	//右下
		Try( fx-1, fy+1 , nStep, fStep1, fx, fy, dir, 1.414f);	//左下

		Try( fx, fy-1 , nStep, fStep2, fx, fy, dir, 1.f);	//上
		Try( fx, fy+1 , nStep, fStep2, fx, fy, dir, 1.f);	//下
		Try( fx-1, fy , nStep, fStep2, fx, fy, dir, 1.f);	//左
		Try( fx+1, fy , nStep, fStep2, fx, fy, dir, 1.f);	//右

		// 加入到CLOSE表
		if( InClose(m_mapOpen[p].x, m_mapOpen[p].y) == -1 )
			AddToClose( m_mapOpen[p] );

		// 从待处理列表移出
		m_mapOpen.erase(p);
	}
}
//********************************
// 处理子节点
// lx,ly 尝试处理的坐标
// Step 
int CFindPath::Try(int lx, int ly, int step, float fStep, int Oldx, int Oldy, int dir, float fAddStep)
{
	//边界
	if( lx < Range.left || lx >= Range.right || ly<Range.top || ly>=Range.bottom ) 
	{
		return -1;
	}

	//不是目标的障碍
	if( m_pMap->GetBlock(lx, ly) != CRegion::BLOCK_NO &&
		 m_pMap->GetBlock(lx, ly) != CRegion::BLOCK_AIM  &&
		(lx != ex || ly != ey) )
	{
		return -2;
	}

	// 是否属于禁行区域中的点
	for(size_t i=0; i < m_vecPathRect.size(); ++i)
	{
		if(IsInRect(lx,ly,m_vecPathRect[i]))
			return -2;
	}

	// 如果两步的方向一样，那么实际的距离减半
	int nNewDir = GetLineDir((long)Oldx, (long)Oldy, (long)lx,(long)ly);
	if (nNewDir == dir)
	{
		fStep -= fAddStep / 5.f;
	}

	int find = InOpen(lx, ly);
	if( find != -1 )	//在OPEN表中
	{
		float ju = Judge(lx, ly);	//估值

		//更新OPEN表
		if( fStep + ju < m_mapOpen[find].fStep + m_mapOpen[find].fJudge )
		{
			m_mapOpen[find].step = step;
			m_mapOpen[find].ox = Oldx;
			m_mapOpen[find].oy = Oldy;
			m_mapOpen[find].fStep = fStep;
			m_mapOpen[find].fJudge = ju;
		}
	}
	else	//不在OPEN表中
	{
		find = InClose(lx, ly);
		if( find !=-1 )		//在CLOSE表中
		{
			float ju = Judge(lx, ly);	//估值
		
			//更新Close表
			if( fStep + ju < m_mapClose[find].fStep + m_mapClose[find].fJudge )
			{
				m_mapClose[find].step = step;
				m_mapClose[find].ox = Oldx;
				m_mapClose[find].oy = Oldy;
				m_mapClose[find].fStep = fStep;
				m_mapClose[find].fJudge = ju;

				AddToOpen( m_mapClose[find] );	//加到OPEN表
				m_mapClose.erase(find);
			}
		}
		else	//都不在
		{
			tagNote note;
			note.step = step;
			note.x = lx;
			note.y = ly;
			note.ox = Oldx;
			note.oy = Oldy;
			note.fStep = fStep;
			note.fJudge = Judge(lx, ly);
						
			AddToOpen(note);	//加入OPEN表
		}
	}

	return 1;
}

//***************************
//寻找最优点 in Open Table
int CFindPath::FindShortest()
{
	float fTheShort = 9999999.f;	//临时变量
	int Num=-1;

	if( m_mapOpen.size() ==0 ) return -1;	//死路

	//遍历待处理节点MAP
	//找出fJudge+fStep最小的节点
	//返回格子的下标
	for(map<long, tagNote>::iterator it=m_mapOpen.begin(); it!=m_mapOpen.end(); it++)
	{
		if( it->second.fJudge + it->second.fStep < fTheShort )
		{
			fTheShort = it->second.fJudge + it->second.fStep;
			Num = it->first;
		}
	}

	return Num;
}

//***************************
//寻找最优点 in Close Table
int CFindPath::FindShortestInClose()
{
	float fTheShort1=9999999.f, fTheShort2=9999999.f;	//临时变量
	int Num=-1;
	float s1, s2;

	for(map<long, tagNote>::iterator it=m_mapClose.begin(); it!=m_mapClose.end(); it++)
	{
		s1 = sqrtf( float(it->second.x-ex)*(it->second.x-ex) + float(it->second.y-ey)*(it->second.y-ey));
		if( s1 < fTheShort1 )
		{
			fTheShort1 = s1;
			fTheShort2 = sqrtf( float(it->second.x-sx)*(it->second.x-sx) + float(it->second.y-sy)*(it->second.y-sy));
			Num = it->first;
		}
		else if( s1 == fTheShort1 )
		{
			s2 = sqrtf( float(it->second.x-sx)*(it->second.x-sx) + float(it->second.y-sy)*(it->second.y-sy));
			if( s2 < fTheShort2 )
			{
				fTheShort2 = s2;
				Num=it->first;
			}
		}
	}
	return Num;
}

//**********************************
//估值
float CFindPath::Judge(int lx, int ly)
{
//	int ret;
//	ret=abs(ex-lx)+abs(ey-ly);
//	return ret;

	return sqrtf( float(ex-lx)*(ex-lx) + float(ey-ly)*(ey-ly));
}

////**********************************
////估值
//int CFindPath::Judge(int Num)
//{
//	int ret;
//	ret=abs(ex-m_mapOpen[Num].x)+abs(ey-m_mapOpen[Num].y);
//	return ret;
//}

//****************************************
//加入到OPEN表
int CFindPath::AddToOpen(tagNote note)
{
	long key = m_pMap->GetWidth() * note.y + note.x;
	m_mapOpen[key] = note;
	return key;
}

//****************************************
//加入到CLOSE表
int CFindPath::AddToClose(tagNote note)
{
	long key = m_pMap->GetWidth() * note.y + note.x;
	m_mapClose[key] = note;
	return key;
}

//************************************
//是否在OPEN表中
int CFindPath::InOpen(int lx, int ly)
{
	long key = m_pMap->GetWidth() * ly + lx;
	if (m_mapOpen.find(key) != m_mapOpen.end())
	{
		return key;
	}
	return -1;
}

//************************************
//是否在CLOSE表中
int CFindPath::InClose(int lx, int ly)
{
	long key = m_pMap->GetWidth() * ly + lx;
	if (m_mapClose.find(key) != m_mapClose.end())
	{
		return key;
	}
	return -1;
}
bool CFindPath::AddPathPos(long lRegionID, long lSourx, long lSoury, long lTargx, long lTargy, long lWidth, long lHeight)
{
	m_vecPathRect.clear();
	CRegionList::tagRegion * pRegion = CRegionList::GetRegion(lRegionID);
	if(pRegion)
	{
		// 找出开始点和结束点，按照大小顺序
		long lStartX = min(lSourx,lTargx), lEndX = max(lSourx,lTargx);
		long lStartY = min(lSoury,lTargy), lEndY = max(lSoury,lTargy);
		RECT rt;
		rt.left		= lStartX;
		rt.top		= lStartY;
		rt.right	= lEndX;
		rt.bottom	= lEndY;
		// 查找此次寻路需要通过的禁行区域（起始点和终点围成的区域与禁行区相交，将禁行区域保存）
		vector<RECT> vecRect = pRegion->vec_stRect;
		for (size_t i=0; i<vecRect.size(); ++i)
		{
			// 找出当前寻路范围下的禁行区域
			if(IsInstersect(vecRect[i],rt))
			{
				// 去除目标点和终点所在的禁行区域
				if(!IsInRect(lStartX,lStartY,vecRect[i]) && !IsInRect(lEndX,lEndY,vecRect[i]))
					m_vecPathRect.push_back(vecRect[i]);
			}
		}
		// 寻路
		Find(lSourx,lSoury,lTargx,lTargy,lWidth,lHeight);
		return true;
	}
	return false;
}
// ==============================自动移动（刘科08.5.6）=================================

// 为指定点到目标点添加响应的路径点
bool CFindPath::AddPathPos(long lRegionID, long lSourx, long lSoury, long lTargx, long lTargy)
{
	m_vecPathRect.clear();
	CRegionList::tagRegion * pRegion = CRegionList::GetRegion(lRegionID);
	if(pRegion)
	{
		// 找出开始点和结束点，按照大小顺序
		long lStartX = min(lSourx,lTargx), lEndX = max(lSourx,lTargx);
		long lStartY = min(lSoury,lTargy), lEndY = max(lSoury,lTargy);
		RECT rt;
		rt.left		= lStartX;
		rt.top		= lStartY;
		rt.right	= lEndX;
		rt.bottom	= lEndY;
		// 查找此次寻路需要通过的禁行区域（起始点和终点围成的区域与禁行区相交，将禁行区域保存）
		vector<RECT> vecRect = pRegion->vec_stRect;
		for (size_t i=0; i<vecRect.size(); ++i)
		{
			// 找出当前寻路范围下的禁行区域
			if(IsInstersect(vecRect[i],rt))
			{
			// 去除目标点和终点所在的禁行区域
			if(!IsInRect(lStartX,lStartY,vecRect[i]) && !IsInRect(lEndX,lEndY,vecRect[i]))
				m_vecPathRect.push_back(vecRect[i]);
			}
		}
		// 寻路
		Find(lSourx,lSoury,lTargx,lTargy);
		return true;
	}
	return false;
}
// =======================================================================================