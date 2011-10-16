/*Clog.h
*摘要:定义和实现日志文件的生成以及保存的函数
*功能:实现根据不同的功能利用函数的重载,生成不同的日志,并将日志保存到文件中去。
*作者:王钦
*创建日期:2010-7-6
*/
#pragma once
#include "QuestEditorDlg.h"

class CLog
{
public:
    CLog(void);
    ~CLog(void);
    //保存日志内容的容器
    vector<string>m_Log;
    //创建日志___创建修改任务属性的日志
    void CreatLogList(vector<tagRoleAttribute*>pPre ,
        vector<tagRoleAttribute*>pUped);
    //创建日志___创建修改任务奖励的日志
    void CreatLogList(vector<tagRoleEncouragement*>pPre ,
        vector<tagRoleEncouragement*>pUped);
    //创建日志___创建修改任务步骤的日志
    void CreatLogList(vector<tagRoleStep*> pPre, tagRoleStep* pUpdatedeStep);
    //创建日志___创其他的操作日志
    void CreatLogList(string strLog);
    //保存日志文件
    void SaveLog();
};
