//********************************************
//	寻路 相关函数
//  创建于2000年3月22日
//********************************************

#pragma once

class CRegion;
class CRegionList;

//*************
//寻路类
class CFindPath
{
public:
	//节点
	struct	tagNote
	{
		int step;		//从开始点实际到达的步数
		int x,y;		//坐标点
		int ox, oy;		//该点上一个点的坐标

		float fStep;	//从起始点到该点的实际值，直方向1，斜方向1.414
		float fJudge;	//估计值，该点到目标点的直线距离
	};

	// 寻找到的路径结构
	struct stPos
	{
		long x;
		long y;
		stPos()
		{
			x = 0;
			y = 0;
		}
	};

private:
	int sx,sy;			//起始点
	int ex,ey;			//目标点
	int Width, Height;	//搜索大小
	RECT Range;			//寻路的地图范围

	map<long, tagNote> m_mapOpen;		//待处理节点
	map<long, tagNote> m_mapClose;		//处理过的节点

	list<stPos>	m_listPath;	//寻找到的路径

	CRegion *m_pMap;		//地图

public:
	CFindPath();	//构造
	CFindPath(CRegion *);	//构造
	~CFindPath();			//析构

	void	SetRegion(CRegion *pRegion);
	bool Find(int, int, int, int);		//主函数
	bool Find(int, int, int, int,int lWidth, int lHieht);		//主函数
	list<stPos>* GetPathList()	{return &m_listPath;}
	CRegion* GetRegion()		{return m_pMap;}

private:
	void Init(int,int);		//初始化两个节点数祖
	void Release();			//释放

	int Try(int, int, int, float, int, int, int, float);	//处理子节点
	float Judge(int, int );		//估值函数（从当前点到目标点）
	int FindShortest();			//从OPEN表寻找最优点
	int FindShortestInClose();	//寻找最优点 in Close Table

	int AddToOpen(tagNote);		//加入到OPEN表
	int AddToClose(tagNote);	//加入到CLOSE表

	int InOpen(int, int);	//是否在OPEN表中 -1=false
	int InClose(int, int);	//是否在CLOSE表中 -1=false

	// ==============================跨场景移动（刘科08.5.6）=================================
public:
	// 为指定点到目标点添加响应的路径点
	bool AddPathPos(long lRegionID, long lSourx, long lSoury, long lTargx, long lTargy);
	bool AddPathPos(long lRegionID, long lSourx, long lSoury, long lTargx, long lTargy, long lWidth, long lHeight);
	// 获得当前场景的禁行区列表
	vector<RECT>& GetPathRectList()	{return	m_vecPathRect;}
private:
	// 在当前场景下的禁行区
	vector<RECT>	m_vecPathRect;
	// =======================================================================================
};
