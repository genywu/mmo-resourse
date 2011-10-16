#ifndef BASE_ITEM_DEF_H
#define BASE_ITEM_DEF_H


	//所有种族类别
	enum eClass
	{
		C_Human=0,		//人形
		C_Element,		//元素
		C_Alien,		//异族
		C_Animal,		//动物
		C_Undead,		//亡灵
		C_Evil,			//恶魔
		C_Hexapods,		//昆虫

		C_Max,
	};

	//元素类型
	typedef enum eElementalType
	{
		ET_Fire=0,		//火系
		ET_Water,		//水系
		ET_Earth,		//土系
		ET_Wind,		//风系
		ET_Dark,		//暗系
		ET_Holy,		//圣系

		ET_Max,
	}eEleType;

	enum eNpcType
	{
		NT_Normal=0,		//普通NPC
		NT_Monster,		//怪物
		NT_Guard,		//守卫
	};

	enum eOccupation
	{
		OCC_Novice=0,			//初心者
		OCC_Fighter,			//战士
		OCC_Mystic,				//法师(人类法师)
		OCC_Rogue,				//行者
		OCC_Knight,				//骑士
		OCC_Warrior,			//斗士
		OCC_Scout,				//斥候
		OCC_Craftsman,			//艺人
		OCC_Warlock,			//术士
		OCC_Cleric,				//牧师
		OCC_Paladin,			//圣骑士
		OCC_DarkKnight,			//暗骑士
		OCC_Gladiator,			//角斗士
		OCC_Berserker,			//狂战士
		OCC_Ranger,				//巡守
		OCC_Assassin,			//刺客
		OCC_BladeDancer,		//剑舞者
		OCC_Bard,				//吟游诗人
		OCC_Sorcerer,			//巫师
		OCC_Necromancer,		//亡灵巫师
		OCC_Bishop,				//主教
		OCC_Prophet,			//贤者(先知)

		OCC_Max,				//职业最大数
	};

	//Debuf的类型
	enum eDefbuffType
	{
		DFT_Bled,		//流血Debuf
		DET_Dizzy,		//击晕
		DET_Lull,		//麻痹
		DET_Hypnosis,	//催眠
		DET_Poison,		//中毒
		DET_Fix,		//定身
		DET_Max,
	};

	//所有种族类别
	const int MONSTER_SKILL_SUM=6;
	const int MONSTER_AI_REFERRENCE_NUM=5;


#endif//BASE_ITEM_DEF_H