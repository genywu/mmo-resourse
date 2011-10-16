
#include <vector>
#include "Map.h"
#include "cmd.h"

class CmdMapFillBuild : public Cmd
{
	//格子列表
	DWORD m_dwGridIndexCount;
	DWORD * m_pOldGridIndexArray;//旧格子索引
	Grid::tagBuild * m_pOldGridBuildArray;//保存旧格子中建筑的信息
	
	Grid::tagBuild m_tBuild;

public:
	CmdMapFillBuild(void);
	virtual ~CmdMapFillBuild(void);

	void SetGridIndex(DWORD dwIndex,DWORD dwGridIndex);
	void SetGridIndexCount(DWORD dwCount);
	void SetBuild(const Grid::tagBuild &tBuild);  
	
	virtual bool Execute(void);
	virtual void UnExecute(void);

	virtual void Dump(void);
};
