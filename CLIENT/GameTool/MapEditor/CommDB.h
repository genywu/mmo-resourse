#pragma once

#include "..\..\frostengine\singleton.h"

class EditMap;
class TerrainMesh;
class TimeSystem;

class CommDB
{
	DECLARE_SINGLETON(CommDB);

	EditMap * m_pMap;
	TerrainMesh *m_pTerrainMesh;

	static const DWORD VERSION = 0x00010005;//1.5

public:
	CommDB(void);
	~CommDB(void);

	EditMap * GetMap() { return m_pMap;}
    void SetMap(EditMap *pMap) { m_pMap = pMap;}

	TerrainMesh* GetTerrainMesh() {return m_pTerrainMesh;}
	void SetTerrainMesh(TerrainMesh *pTerrainMesh) {m_pTerrainMesh = pTerrainMesh;}


};
