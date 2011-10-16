#pragma once
#include "..\..\engine\singleton.h"

#define MAP_VERSION_1_5		0x00010005		//1.5
#define MAP_VERSION_1_6		0x00010006		//1.6
#define MAP_VERSION_1_7		0x00010007		//1.7
#define MAP_VERSION_CURRENT	MAP_VERSION_1_7

class EditMap;
class TerrainMesh;
class TimeSystem;

class CommDB
{
	DECLARE_SINGLETON(CommDB);

	EditMap * m_pMap;
	TerrainMesh *m_pTerrainMesh;

public:
	CommDB(void);
	~CommDB(void);

	EditMap * GetMap() { return m_pMap;}
    void SetMap(EditMap *pMap) { m_pMap = pMap;}

	TerrainMesh* GetTerrainMesh() {return m_pTerrainMesh;}
	void SetTerrainMesh(TerrainMesh *pTerrainMesh) {m_pTerrainMesh = pTerrainMesh;}


};
