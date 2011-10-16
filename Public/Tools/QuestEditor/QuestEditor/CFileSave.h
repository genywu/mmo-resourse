#pragma once
#include "tinyxml.h"
#include "CAttribute.h"
#include "CStep.h"
#include "CEncouragement.h"
class CFileSave
{
public:
	//保存任务
	vector<tagRole*>m_vcRole;
	//保存任务的属性
	vector<tagRoleAttribute*>m_vcAttribute;
	//保存任务的步骤
	vector<tagRoleStep*>m_vcStep;
	//保存任务的奖励
	vector<tagRoleEncouragement*>m_vcEncouragement;
	CFileSave(void);
	~CFileSave(void);

public:
    /// 重读资源
    void OnReWrite();
    /// 
    void OnSaveRes();

    /// 重写保存List文件列表
    void OnSaveFileList();

};
