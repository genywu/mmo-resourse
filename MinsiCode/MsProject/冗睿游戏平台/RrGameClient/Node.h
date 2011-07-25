#pragma once                //┃//┃
#ifndef __NODE_H__          //┃//┃
#define __NODE_H__          //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：Node.h              //┃
//功能描述：游戏Tree节点        //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛
#define CHILDMAX 10
#define NODEWIDTH 40
#define NODEHIGHT 20
#define NODEWIDTHINDENT  5

using namespace std;

class CNode
{
public:
    CNode(VOID);
    ~CNode(VOID);

    BOOL Init(RECT PaterRect,DWORD dwNodeId, BOOL bIsRoot = FALSE);//初始化当前节点
    VOID Realese();//从当前节点开始释放
    VOID AddNode(DWORD dwNewNodeId);//添加节点
    VOID OpenNode(BOOL bOpen);//展开关闭当前节点
    BOOL IsOpen(){return m_bIsOpen;}
    typedef vector<CNode*> ChildNodeList;

    RECT GetRect(){return m_rect;}
    VOID UpdataRect();
    VOID Render(HDC hDC);
    ChildNodeList GetChildList(){return m_pChildList;}
private:
    ChildNodeList    m_pChildList;//子节点表
    DWORD            m_dwChildCurNum;//子节点个数
    BOOL             m_bIsOpen;//是否展开
    RECT             m_rect;//节点区域
    DWORD            m_dwNodeId;//节点序列
    DWORD            m_dwChildId;//子节点序列
};


//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
