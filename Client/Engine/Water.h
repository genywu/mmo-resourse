/*
* Water.h
* 功能: 水面渲染模块
* 摘要: 主要用于处理采用法线扰动模式下的新的水面效果
* 作者: 李鹏飞lpf
* 创建日期: 2009-09-02
*/
#pragma once
#include "singleton.h"

#define WATERVERTEX_MAX 500

namespace Water
{
    class CWater;

    enum eType
    {
        WaterType_Rect,			//矩形
        WaterType_Round,		//圆形
    };

    // 渲染元素结构体
    struct _tagRenderElement
    {
        D3DXVECTOR3 vPos;					//坐标
        DWORD		dwElapseTime;			//消逝时间

        CWater * pWater;		//指向原始数据,只保存指针
    };

    // 管理器类
    class CManager
    {
        DECLARE_SINGLETON(CManager);

        std::list<CWater *>			  m_listAllWaters;					//所有水面列表
        std::list<CWater *>::iterator m_itWater;						//迭代器
        std::list<_tagRenderElement>			m_listRenderWaters;		//渲染列表
        std::list<_tagRenderElement>::iterator  m_itRenderWaters;		//迭代器

        void BeginRender(BOOL bFogEnable);		//开始水面渲染
        void EndRender();						//结束水面渲染

    public:
        static bool s_bIsRenderWater;			//水面渲染开关

        void Destroy();							//销毁

        long GetNumAllWater()			{ return (long)m_listAllWaters.size(); }			//得到所有水面数量
        long GetNumRenderWater()		{ return (long)m_listRenderWaters.size(); }			//得到渲染水面数量

        void AddWater(CWater * pWater);							//添加水面
        void AddRenderWater(_tagRenderElement & Element);		//添加渲染水面
        void RemoveWater(CWater * pWater);						//移除水面

        void Render(BOOL bFogEnable);							//渲染
    };

    // 水面类(相当于GameModel,同样的对象只创建一份)
    class CWater : public LoadState
    {
        friend class CManager;

        long m_lID;								//ID
        char m_szWaterFileName[MAX_PATH];		//水面配置文件名字
        char m_szShaderFileName[MAX_PATH];		//着色器文件名字

    public:
        eType	 m_eType;		//外形
        long	 m_lNumVertex;	//顶点数量
        float	 m_fRadius;		//半径
        float	 m_fWidth;		//宽度
        float	 m_fLength;		//长度
        D3DCOLOR * m_dwpColor;	//颜色

        // 法线纹理层参数
        float m_fNorSpeedX;			//法线纹理流动速度X轴方向
        float m_fNorSpeedY;			//法线纹理流动速度Y轴方向
        float m_fNormat00;			//法线纹理矩阵参数
        float m_fNormat01;			//法线纹理矩阵参数
        float m_fNormat10;			//法线纹理矩阵参数
        float m_fNormat11;			//法线纹理矩阵参数

        // 环境纹理层参数
        float m_fEnvSpeedX;			//环境纹理流动速度X轴方向
        float m_fEnvSpeedY;			//环境纹理流动速度Y轴方向

    private:
        DWORD m_dwShaderGroupHandle;			//着色变量组句柄

        void Render(_tagRenderElement & Element);		//渲染水面

    public:
        CWater();			//构造函数
        ~CWater();			//析构函数

        const char * GetWaterFileName()						{ return m_szWaterFileName; }				//得到水面基本配置文件名
        const char * GetShaderFileName()					{ return m_szShaderFileName; }				//得到着色变量配置文件名
        DWORD		 GetShaderGroupHandle()					{ return m_dwShaderGroupHandle; }			//得到着色变量组句柄

        void SetWaterFileName(const char * pszFileName)		{ strcpy_s(m_szWaterFileName, MAX_PATH, pszFileName); }			//设定水面基本配置文件名
        void SetShaderFileName(const char * pszFileName)	{ strcpy_s(m_szShaderFileName, MAX_PATH, pszFileName); }		//设定着色变量配置文件名

        bool Create(CRFile * pFile);												//创建水面
        bool CreateShaderGroup(CRFile * pFile, DWORD dwTextureProcision = 0);		//创建着色变量组
    };
}