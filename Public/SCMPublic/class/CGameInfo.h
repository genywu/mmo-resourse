#include <time.h>


#pragma once

/*
	游戏服务器运行中产生的信息，由下面这些类进行管理
	
	tagGameInfoData：	游戏信息的内容。信息的具体数据，意义由SM和SUC定义，在SCC上作为无意义数据进行统一保存
	CGameInfo：			带有信息类型和记录时间的信息，作为GMT3个工具间信息数据传输的最小单位
	CGameInfoGroup：	包含多条CGameInfo的类型，用于消息交换
*/

//!--------------------------------------------------------------------------------------------
//! 游戏信息的内容
struct tagGameInfoData
{
	//!							记录时间
	time_t						_tTime;

	DWORD 						_dwData1;
	DWORD 						_dwData2;

	tagGameInfoData(VOID){memset(this, 0, sizeof(tagGameInfoData));}
	tagGameInfoData(DWORD dwData){time(&_tTime);_dwData1=dwData;_dwData2=0;}
};

//! 由SCC申明使用
struct tagDbGameInfo : public tagGameInfoData
{
	DWORD dwInfoType;
};

//!--------------------------------------------------------------------------------------------
//! 一组游戏信息
class CGameInfo
{
public:
	CGameInfo(DWORD dwType):m_dwType(dwType) {}

public:
	//!							获得信息类型
	DWORD						GetType(VOID)	{return m_dwType;}

	//!							编码到数据集
	BOOL						Code(DBWriteSet &setWriteDB) const;

    BOOL                        Code(Json::Value& var) const;
	//!							从数据集解码
	BOOL						DeCode(DBReadSet &setReadDB);

	//!							添加一条信息
	VOID						AddInfo(tagGameInfoData &GameInfoData) {m_vGameInfoData.push_back(GameInfoData);}


protected:
	//!							信息类型
	DWORD						m_dwType;
	//!							信息内容
	vector<tagGameInfoData>		m_vGameInfoData;
};

