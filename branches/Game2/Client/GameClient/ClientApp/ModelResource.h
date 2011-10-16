#pragma once

class GameModel;
// 模型资源管理类
class CModelResource
{
public:
	// 类型定义
	enum MODEL_TYPE
	{
		TYPE_WING 			    = 1,	// 翅膀
		TYPE_RIDE				= 2,	// 坐骑
		TYPE_WEAPON				= 3,	// 武器
		TYPE_SHADOW             = 4,
		TYPE_TALISMAN           = 5,    //不知道什么东西
	};
public:
	CModelResource(void);
	virtual ~CModelResource(void);

protected:
	struct ModelKey
	{
		long Type;
		DWORD GraphicsID;
	};
	class hash_model_compare:public hash_compare<ModelKey>
	{
	public:
		//计算散列值
		size_t operator( )( const ModelKey& Key ) const
		{
			return Key.Type;
		}
		//less operator
		bool operator( )( const ModelKey& _Key1,const ModelKey& _Key2 ) const
		{
			if(_Key1.Type < _Key2.Type)	return true;
			else if(_Key1.Type == _Key2.Type && _Key1.GraphicsID < _Key2.GraphicsID)	return true;
			return false;
		}
	};

	typedef hash_map<ModelKey,GameModel*,hash_model_compare>	HMModels;
	typedef hash_map<ModelKey,GameModel*,hash_model_compare>::iterator itModel;

	HMModels m_mapGameModel;

public:
	GameModel * Get(long Type,WORD wModelID);

	GameModel * FindModel(long Type,WORD wModelID);
	GameModel * ReadModel(long Type,WORD wModelID);
	void Release(long Type,WORD wModelID);
	void ReleaseAll();

	void Create(void);
	void Destroy();
};
