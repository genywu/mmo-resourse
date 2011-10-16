/**
* @filename:GameWorld.h
* @date: 2009/12/15
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose:  封装游戏世界类
*/
#pragma  once

class GameWorld
{
	HWND m_hWnd;
	static GameWorld *m_Instance;
public:
	
	/// @brief
	/// 获取游戏世界类实例指针
	static GameWorld *GetInstance(void);


	/// @brief
	/// 初始化游戏世界类

	static void InitInstance(void);

	/// @brief
	/// 销毁游戏时间类实例

	static void ExitInstance(void);

	/// @brief
	/// 创建游戏世界类需要的内容
	/// @param: in hWnd
	/// 窗口句柄

	void Create(HWND hWnd);

	/// @brief
	/// 释放游戏世界创建并使用的资源
	void Destroy(void);

	/// @brief
	/// 刷新游戏世界
	bool Update(void);

	/// @brief
	/// 渲染游戏世界
	void Render(void);
};