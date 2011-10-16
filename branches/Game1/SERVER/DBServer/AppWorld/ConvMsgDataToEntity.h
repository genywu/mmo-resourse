#ifndef CONV_MSG_DATA_TO_ENTITY
#define CONV_MSG_DATA_TO_ENTITY
#include <string>
#include <map>
#include <list>

using namespace std;

class CPlayer;
class CGoods;
class CCountry;
class CEntityProperty;

class 	CCard;
struct 	tagAttackCityTime;
struct 	tagVilWarSetup;
struct 	tagEnemyFaction;
class 	CWorldRegion;
class 	CUnion;
class 	CFaction;


// 将DBS的MSG数据和游戏实体相互转换
class CMsgDataWithEntityConvertor
{
public:
	typedef std::list<string> OperPropertyNameList;
	typedef OperPropertyNameList::iterator OperPropertyNameListItr;


	CMsgDataWithEntityConvertor();
	~CMsgDataWithEntityConvertor();

	// Interfaces
	void MsgDataToEntity(CEntityProperty* ep, CPlayer* player);
	void MsgDataToEntity(CEntityProperty* ep, CGoods* goods);
	void MsgDataToEntity(CEntityProperty* ep, CCountry* country);
	void MsgDataToEntity(CEntityProperty* ep, CCard* card);
	void MsgDataToEntity(CEntityProperty* ep, tagAttackCityTime* CityWarParam);
	void MsgDataToEntity(CEntityProperty* ep, tagVilWarSetup* VillageWarSetup);
	void MsgDataToEntity(CEntityProperty* ep, tagEnemyFaction* EnemyFactions);
	void MsgDataToEntity(CEntityProperty* ep, CWorldRegion* pRegion);
	void MsgDataToEntity(CEntityProperty* ep, CUnion *pConf);
	void MsgDataToEntity(CEntityProperty* ep, CFaction* pFaction);
	// Setup
	void MsgDataToEntity(CEntityProperty* ep);

	// 需要先指定操作的属性名字,使用 标志操作 接口
	void EntityToMsgData(CPlayer* player,	vector<BYTE>& pBA);
	void EntityToMsgData(CGoods* goods,		vector<BYTE>& pBA);
	void EntityToMsgData(CCountry* country, vector<BYTE>& pBA);
	void EntityToMsgData(CCard* card, const CGUID& entityID, vector<BYTE>& pBA);
	void EntityToMsgData(tagAttackCityTime* CityWarParam, const CGUID& entityID, vector<BYTE>& pBA);
	void EntityToMsgData(tagVilWarSetup* VillageWarSetup, const CGUID& entityID, vector<BYTE>& pBA);
	void EntityToMsgData(tagEnemyFaction* EnemyFactions, const CGUID& entityID, vector<BYTE>& pBA);
	void EntityToMsgData(CWorldRegion* pRegion, vector<BYTE>& pBA);
	void EntityToMsgData(CUnion *pConf, vector<BYTE>& pBA);
	void EntityToMsgData(CFaction* pFaction, vector<BYTE>& pBA);
	// Setup
	void EntityToMsgData(vector<BYTE>& pBA);

	// 标志操作
	// 从队列后加入
	void PushBackOperPropertyName(string& name);
	// 从队列前弹出
	string PopFrontOperPropertyName(void);
	// num
	long GetOperPropertyNameNum(void);
	// 清空标志队列
	void ClearOperPropertyNameList(void) { m_OperPropertyName.clear(); }

private:
	// 记录某位置属性被操作的状态
	std::list<string> m_OperPropertyName;

	// 游戏实体变量名和变量指针对应表
	std::map<std::string, void*> m_mapPlayerProperty;
	std::map<std::string, void*> m_mapCityWarProperty;
};

extern CMsgDataWithEntityConvertor g_MsgDataWithEntityConvertor;
#endif//CONV_MSG_DATA_TO_ENTITY