/**
* @filename:TeamSetup.h
* @date: 2010/6/7
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 

* @purpose: 队伍界面配置
*/

///队伍界面配置管理类
class TeamSetup
{
public:
	///
	TeamSetup();
	///
	~TeamSetup();
	///从setup/UISetup/Team.ini读取配置
	bool ReadSetup();
	///
	int GetInterval() { return interval; }
	///
	int GetMaxItem() const { return maxItem;}
	///
	int GetCurItemNum() const { return CurItemNum; }
	///
	void ResetCurItemNum() { CurItemNum = 0; }
	///
	void CurItemNumAdded() { CurItemNum++ ;}
	///
	CEGUI::Vector2& GetPagePos()  { return pagePos; }
	///
	CEGUI::Vector2& GetHeadPos()  { return headWndOffset; }
	///
	CEGUI::Vector2& GetHpPos()  { return HpWndOffset; }
	///
	CEGUI::Vector2& GetMpPOs()  { return MpWndOffset; }
	///
	CEGUI::Vector2& GetPlayerPos() { return PlayerWndOffset;}
	///
	CEGUI::Vector2& GetLevelPos() { return LevelWndOffset; }
	
	///
	float GetItemHeight() const { return itemSize.d_height; }
	///
	float GetItemWidth() const { return itemSize.d_width; }
	///
	CEGUI::UVector2 GetItemSize() const { return CEGUI::UVector2(cegui_absdim(itemSize.d_width),
		cegui_absdim(itemSize.d_height)); }
	///
	CEGUI::UVector2 GetHeadSize() const { return CEGUI::UVector2(cegui_absdim(headWndSize.d_width),
		cegui_absdim(headWndSize.d_height)); }
	///
	CEGUI::UVector2 GetHpSize() const { return CEGUI::UVector2(cegui_absdim(HpWndSize.d_width),
		cegui_absdim(HpWndSize.d_height)); }
	///
	CEGUI::UVector2 GetMpSize() const { return CEGUI::UVector2(cegui_absdim(MpWndSize.d_width),
		cegui_absdim(MpWndSize.d_height)); }
	///
	CEGUI::UVector2 GetPlayerSize() const { return CEGUI::UVector2(cegui_absdim(PlayerWndSize.d_width),
		cegui_absdim(PlayerWndSize.d_height)); }
	///
	CEGUI::UVector2 GetLevelSize() const { return CEGUI::UVector2(cegui_absdim(LevelWndSize.d_width),
		cegui_absdim(LevelWndSize.d_height)); }
	///
	const char* GetItemWndType() const { return ItemWndType; }
	///
	const char* GetItemWndName() const { return ItemWndName; }
	///
	const char* GetHeadWndType() const { return headWndType; }
	///
	const char* GetHeadWndName() const { return headWndName; }
	///
	const char* GetMpWndType() const { return MpWndType; }
	///
	const char* GetMpWndName() const { return MpWndName; }
	///
	const char* GetHPWndType() const { return HpWndType; }
	///
	const char* GetHPWndName() const { return HpWndName; }

	///
	const char* GetPlayerWndName() const { return PlayerWndName; }
	///
	const char* GetPlayerWndType() const { return PlayerWndType; }
	///
	const char* GetLevelWndName() const { return LevelWndName; }
	///
	const char* GetLevelWndType() const { return LevelWndType; }
private:
	uint maxItem;				///最大显示队员数
	uint CurItemNum;			///当前item数目
	CEGUI::Vector2   pagePos;	///窗口初始位置
	CEGUI::Size		 itemSize;	///item的大小
	int interval;				///Item间隔距离

	char ItemWndName[256];	///条目窗口的名称前缀
	char ItemWndType[256];	///条目窗口类型

	char headWndType[256];	///头像窗口类型名称
	char headWndName[256];  ///头像窗口名称
	CEGUI::Size	headWndSize; ///头像窗口的大小
	CEGUI::Vector2  headWndOffset;///头像窗口的位置偏移

	char HpWndType[256];
	char HpWndName[256];
	CEGUI::Size HpWndSize;
	CEGUI::Vector2 HpWndOffset;

	char MpWndType[256];
	char MpWndName[256];
	CEGUI::Size MpWndSize;
	CEGUI::Vector2 MpWndOffset;

	char PlayerWndName[256];
	char PlayerWndType[256];
	CEGUI::Size PlayerWndSize;
	CEGUI::Vector2 PlayerWndOffset;

	char LevelWndName[256];
	char LevelWndType[256];
	CEGUI::Size LevelWndSize;
	CEGUI::Vector2 LevelWndOffset;
};