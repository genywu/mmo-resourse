/********************************************************************
	created:	2010/09/26
	created:	26:9:2010   14:23
	filename: 	e:\SVN_P6\trunk\Client\GameClient\ClientApp\Skills\SkillDecorator.h
	file path:	e:\SVN_P6\trunk\Client\GameClient\ClientApp\Skills
	file base:	SkillDecorator
	file ext:	h
	author:		zhaohang(kaba) (kuailekaba@gmail.com)
	
	purpose:	尝试使用装饰模式来扩展CSkillXml的功能
*********************************************************************/
class CMoveShape;

///技能的公共基类
class SkillComponent{
public:
	///虚析构函数,从它继承的类有自己的析构函数实现
	virtual ~SkillComponent();

	///执行技能开始步骤
	bool virtual StepBegin(long lID, long lLevel, 
		long sType,
		const CGUID& sID, long lDestX,
		long lDestY, float fDir, 
		vector<long> vType, vector<CGUID> vID) = 0;
	/// 执行发送技能消息步骤
	bool virtual StepSend(DWORD dwID,DWORD dwLevel,
		CMoveShape *sShape, CMoveShape *dShape,
		long dX, long dY) = 0;
	///执行技能实施步骤
	bool virtual StepRun(long lID,long lLevel,
		long sType,const CGUID& sID,
		long dX,long dY,float fDir,
		vector<long>vType,vector<CGUID>vID) = 0;
	///执行技能结束步骤
	bool virtual StepEnd(long lID,long lLevel,
		long sType,const CGUID& sID,float fDir)=0;
	///执行技能回收步骤
	bool virtual StepSummon()=0;
};

///技能的被装饰类
class ConcreteSkillComp:public SkillComponent
{
public:
	///构造函数
	ConcreteSkillComp();
	///析构函数
	~ConcreteSkillComp();

	///执行技能开始步骤
	bool virtual StepBegin(long lID, long lLevel, 
		long sType,
		const CGUID& sID, long lDestX,
		long lDestY, float fDir, 
		vector<long> vType, vector<CGUID> vID);
	/// 执行发送技能消息步骤
	bool virtual StepSend(DWORD dwID,DWORD dwLevel,
		CMoveShape *sShape, CMoveShape *dShape,
		long dX, long dY);
	///执行技能实施步骤
	bool virtual StepRun(long lID,long lLevel,
		long sType,const CGUID& sID,
		long dX,long dY,float fDir,
		vector<long>vType,vector<CGUID>vID);
	///执行技能结束步骤
	bool virtual StepEnd(long lID,long lLevel,
		long sType,const CGUID& sID,float fDir);
	///执行技能回收步骤
	bool virtual StepSummon();
protected:
	SkillComponent* m_Comp;
};
