/********************************************************************
created:	2010/09/06
created:	6:9:2010   10:32
filename: 	e:\SVN_P6\trunk\Client\GameClient\ClientApp\Skills\SkillIni.h
file path:	e:\SVN_P6\trunk\Client\GameClient\ClientApp\Skills
file base:	SkillIni
file ext:	h
author:		zhaohang(kaba) (kuailekaba@gmail.com)

purpose:	新版技能的文案内容配表、显示配置文件、姿态需求配置文件管理类
*********************************************************************/
#include "../../../../Public/AppFrame/TextRes.h"
/** \addtogroup Skill
* @{
*/
#ifndef SAFEDELETE
#define SAFEDELETE(e) if(e) { delete e ; e = NULL ;}
#endif

#ifndef MSGBOX
#define MSGBOX(e) MessageBox(NULL,e,"Error",MB_ICONERROR); Log4c::Error(ROOT_MODULE,e);
#endif

/**检查技能相关配置的工具基类*/
class CheckSkillIni{
public:
	///技能配置模式消息框类型
	enum eMsgBoxType{
		///文件不存在
		MBT_FILENOTFOUND,
		///格式不正确
		MBT_FORMATERROR,
	};
	///构造函数
	CheckSkillIni();
	///析构函数
	virtual ~CheckSkillIni();

	/**弹出技能配置检查模式消息框
	@param pathName 检查的完整文件路径名
	@param msgtype 弹出的模式消息框类型
	*/
	void MsgBox(const char* pathName,eMsgBoxType msgtype);
	/**检查文件是否打开成功
	@param file 用CRFile打开的文件指针
	@param pathName 文件完整路径名
	*/
	bool CheckFile(CRFile* file,const char* pathName);

private:
	/**检查文件格式是否正确
	@param vecStr 检查的字符串数组
	@param pathName 检查的文件完整路径名
	@return 匹配返回true，不匹配返回false
	*/
	virtual bool CheckFileHeader(std::vector<std::string>& vecStr,const char* pathName) = 0;

};
/**保存一项技能里的一个等级和描述的数据结构*/
struct tagSkillDocLevel{
private:
	///等级
	uint level; 
	///对应的描述
	std::string strDesc;
public:
	/**构造函数*/
	tagSkillDocLevel();
	/**析构函数*/
	~tagSkillDocLevel();

	/**设置等级
	@param Level 设置等级值
	*/
	void SetLevel(uint Level);

	/**获得等级*/
	uint GetLevel();

	/**设置等级描述
	@remarks
	如果文本为NULL，则自动将描述设为""
	@param desc 描述文本
	*/
	void SetDesc(const char* desc);
	/**设置等级描述
	@remarks
	如果文本为NULL，则自动将描述设为""
	@param desc 描述文本
	*/
	void SetDesc(std::string desc);
	/**返回等级描述*/
	const char* GetDesc();
};

/**重定义的保存一个技能等级和描述的数据结构*/
typedef tagSkillDocLevel tagSkillLD;

/**保存一项技能文案的数据结构*/
struct tagSkillDoc{
private:
	///技能ID
	uint skillID;
	///技能名
	std::string strSkillName;
	///技能等级-描述 的map
	std::map<uint,tagSkillLD*> mapSkillLD; 
	///
	typedef std::map<uint,tagSkillLD*> MapSKILLLD;
public:
	///构造函数
	tagSkillDoc();
	//析构函数
	~tagSkillDoc();

	/**返回技能ID*/
	uint GetSkillID();
	/**设置技能ID
	@param id 
	*/
	void SetSkillID(uint id);
	/**返回技能名称*/
	const char* GetSkillName();
	/**设置技能名称
	@param name 技能的名称
	*/
	void SetSkillName(const char* name);

	/**设置技能名称
	@param name 技能的名称
	*/
	void SetSkillName(std::string name);

	///**增加等级*/
	//void AddLevel(tagSkillLD* Level);

	///**增加等级
	//@return 如果已有指定的等级，则返回false，否则返回true
	//*/
	bool AddLevel(tagSkillLD* Level);
	/**移除指定的等级*/
	void RemovedLevel(uint Level);

	/**移除所有等级*/
	void RemovedAllLevel();

	/**获取指定等级的Level*/
	tagSkillLD* GetLevel(uint Level);

};
/**检查技能文案配置是否正确的类*/
class CheckSkillDocIni: public CheckSkillIni
{
private:
	friend class SkillDocIni;
	/**
	@copydoc CheckSkillIni::CheckFileHeader
	*/	bool CheckFileHeader(std::vector<std::string>&vecStr,const char* pathName);
	
	///检查技能ID是否符合规则
	bool CheckID(uint id);
	///检查技能名称是否符合规则
	bool CheckSkillName(std::string str);
	///检查等级是否符合规则
	bool CheckLevel(uint level);
	///检查等级描述是否符合规则
	bool CheckLevelDesc(std::string str);

};

/**从指定的文件读取技能文案配置的类*/
class SkillDocIni{
private:
	///按技能ID为索引的文案配置表
	typedef std::map<uint,tagSkillDoc*> MapSKILLDOC;
	///文案配置表
	MapSKILLDOC mSkillDoc;
	///文件配置的检查类
	CheckSkillDocIni check;
public:
	/**构造函数*/
	SkillDocIni();
	/**析构函数*/
	~SkillDocIni();

	/**读取配置文件
	@param pathName 完整路径名称
	*/
	bool LoadFile(const char* pathName = "data/skills/SkillDocCon.ini");

	/**存储配置文件*/
	bool SaveFile(const char* pathName = "data/skills/SkillDocCon.ini");

	/**按技能ID作为可以增加一项技能文案配置
	@param skilldoc 指向被增加项的指针
	@return 
		ID已存在，返回false，否则返回true
	*/
	bool AddSkillDoc(tagSkillDoc* skilldoc);
	/**移除一项技能文案配置
	@param skillid 指定要移除项的技能的id
	*/
	void RemovedSkillDoc(uint skillid); 

	/**返回指定技能ID的技能文案配置项
	@param skillid
	*/
	tagSkillDoc* GetSkillDoc(uint skillid);

protected:
	/**移除所有项*/
	void RemovedAllSkillDoc();
};

/**客户端技能显示数据结构*/
struct tagSkillShowState 
{
	///对应的技能等级
	int mlevel;
	///吟唱时间
	int mIntonateTime;
	///内力消耗值
	int mEnergy;
	///法力消耗值
	int mMP;
	///气血消耗值
	int mHP;
	///消耗物品原始名
	char mGoodsName[256];
	///消耗物品数量
	int mGoodsNum;
	///冷却时间
	int mCoolDown;
	///最小攻击距离
	int mMinAttackDistance;
	///最大攻击距离
	int mMaxAttackDistance;
	///构造函数，置所有值为-1
	tagSkillShowState();
	///析构函数
	~tagSkillShowState();
	///重载赋值运算符
	tagSkillShowState& operator = (const tagSkillShowState& ref);
};

///按技能等级作为索引的显示数据map表
typedef std::map<uint,tagSkillShowState*> MapSkillLevelData;

/**客户端技能显示具体数据存储类*/
class SkillShow{
private:
	///技能ID
	int mSkillID;
	///技能等级数据map表
	MapSkillLevelData mLevelDataMap;
public:
	///构造函数
	SkillShow();
	///析构函数
	~SkillShow();
	/**设置技能ID
	@param id
	*/
	void SetID(int id);
	/**获取技能ID*/
	int GetID();
	/**获取指定等级的技能数据
	@param level 技能的等级
	*/
	tagSkillShowState* GetSkillShowState(int level);
	/**添加等级数据
	@param add  等级数据
	*/
	bool AddSkillShowState(tagSkillShowState* add);

	/**移除指定等级的显示数据
	@param level 指定等级
	*/
	bool RemovedSkillShowState(int level);

	/**移除所有等级的显示数据*/
	void RemovedAllSkillShowState();
};

///按技能ID的技能显示配置map表
typedef std::map<int,SkillShow*>MapSkillShow;

/**SkillShowIni配置的文件的格式检查类*/
class CheckSkillShowIni: public CheckSkillIni
{
private:
	friend class SkillShowIni;
	/**
	@copydoc CheckSkillIni::CheckFileHeader
	*/
	bool CheckFileHeader(std::vector<std::string>&vecStr,const char* pathName);
protected:
private:
};

/**客户端技能显示配置文件管理类*/
class SkillShowIni{
private:
	///按技能ID的技能显示配置map表
	MapSkillShow  mSkillShowMap;
	///配置文件的格式检查
	CheckSkillShowIni mCheck;
public:
	///构造函数
	SkillShowIni();
	///析构函数
	~SkillShowIni();
	/**添加SkillShow项
	@param item 增加的SkillShow项
	@return 
		已有指定的技能ID就返回false，否则返回true
	*/
	bool AddSkillShow(SkillShow* item);

	/**移除指定ID的SkillShow项
	@param id 技能id
	@return
		成功返回true，失败返回false
	*/
	bool RemovedSkillShow(int id);

	/**移除索引SkillShow项*/
	void RemovedAllSkillShow();

	/**返回指定id的skillShow项
	@param id 技能ID
	@return
		返回对应技能id的显示项，没有就返回false
	*/
	SkillShow* GetSkillSow(int id);

	/**读取指定的配置文件
	@param pathName 文件的完整路径名
	@return
		成功返回true，失败返回false
	*/
	bool LoadFile(const char* pathName);

	/**存储配置文件
	@param pathName 文件的完整路径名
	*/
	void SaveFile(const char* pathName);

};


/**技能姿态Pose文件检查类*/
class CheckSkillPoseIni: public CheckSkillIni
{
private:
	friend class SkillPoseIni;
	/**
	@copydoc CheckSkillIni::CheckFileHeader
	*/	bool CheckFileHeader(std::vector<std::string>&vecStr,const char* pathName);
protected:
private:
};

///存储技能ID-姿态数据结构
struct tagSkillPose 
{
	///技能ID
	int id;
	///技能对应的姿态描述
	std::string strPose;
};
///按技能ID存储的姿态数据表
typedef std::map<int,tagSkillPose*>MapSkillPose;
///技能姿态配置文件管理类
class SkillPoseIni{
private:
	///文件检查
	CheckSkillPoseIni mCheck;
	///读取的姿态数据表
	MapSkillPose   mSkillPoseList;
public:
	///构造函数
	SkillPoseIni();
	///析构函数
	~SkillPoseIni();

	/**读取文件
	@param pathName 文件完整路径名
	*/
	bool LoadFile(const char* pathName);

	/**保存文件
	@param pathName 文件完整路径名
	*/
	bool SaveFile(const char* pathName);

	/**添加姿态
	@param add 被添加的一项姿态数据
	*/
	bool AddPose(tagSkillPose* add);

	/**按key移除姿态
	@param key  关键字为技能ID
	*/
	bool RemovedPose(int key);

	/**移除所有姿态数据*/
	void RemovedAllPose();

	/**按key获取姿态
	@param id 技能ID为关键字
	*/
	tagSkillPose* GetPose(int id);

};
/** @} */