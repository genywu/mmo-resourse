/********************************************************************
	created:	2010/11/04
	created:	4:11:2010   16:52
	filename: 	e:\SVN_P6\trunk\Client\GameClient\ClientApp\NotifySys\NotyfySys.h
	file path:	e:\SVN_P6\trunk\Client\GameClient\ClientApp\NotifySys
	file base:	NotyfySys
	file ext:	h
	author:		zhaohang(kaba) (kuailekaba@gmail.com)
	
	purpose:	滚动公告的逻辑数据管理
*********************************************************************/
/**公告字符串数据管理类*/
class NotifyRenderStringMgr{
public:
	/**渲染优先级定义*/
	enum RenderQueue{
		RQ_FST,
		RQ_SEC,
		RQ_TRD,
		RQ_FORTH,
		RQ_FIFTH
	};
public:
	/**按优先级排序的字符串列表*/
	struct tagRQString{
		RenderQueue rq;
		std::string str;
		tagRQString(){
			rq = RQ_FST;
			str = "";
		}
		bool operator < (const tagRQString& ref){
			return rq < ref.rq;
		}
	};
	///构造函数,读取了显示配置
	NotifyRenderStringMgr();
	///析构函数
	~NotifyRenderStringMgr();

	/**增加渲染字符串项,每增加一个，会自动排序
	@param rq
		优先级
	@param str
	    字符串内容
	*/
	static void AddText(std::string str,RenderQueue rq = RQ_FIFTH);
	/**获取渲染字符列表的第一个值*/
	static string& FrontText();
	/**是否为空*/
	static bool IsEmpty();

	/**	对滚动时间计时,每帧调用,到达最长滚动显示时间，
	*	弹出当前显示字符串，通知界面显示下一条，并重新计时
	*/
	static void UpdateTime();
private:
	///字符串列表
	static std::list<tagRQString> strList;
	///最长滚动显示时间
	static int d_maxRollTime;
	///滚动方向
	static int d_iDir;
	///滚动速度,像素/每秒
	static float d_fSpeed;
	///
	static int d_elapseTime;
	///读配置
	static void LoadSetup();
};