/**
* @filename:ControlGoodsView.h
* @date: 2010/1/27
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 

* @purpose: 控制模型查看的UI类
*/
#pragma  once
#include "../../CEGUIEngine/BaseUIClass.h"

namespace UIClass
{
	class ControlGoodsView:public BaseUIClass
	{
	public:
		ControlGoodsView();
		virtual ~ControlGoodsView();

		/// @brief
		/// 重写初始化的虚函数，定制自己的CEGUI处理
		/// @param
		/// 
		/// @return
		/// 
		bool Init();
	protected:
		/// @brief
		/// 处理向右旋转
		/// @param e
		bool HandleRightRot(const CEGUI::EventArgs& e);
		/// @brief
		/// 处理向左旋转
		bool HandleLeftRot(const CEGUI::EventArgs& e);

		/// @brief
		/// 处理上旋转
		/// @param
		/// 
		/// @return
		/// 
		bool HandleTopRot(const CEGUI::EventArgs& e);

		/// @brief
		/// 处理下旋转
		/// @param
		/// 
		/// @return
		/// 
		bool HandleBottomRot(const CEGUI::EventArgs& e);

		/// @brief
		/// 处理放大
		/// @param
		/// 
		/// @return
		/// 
		bool HandleZoomIn(const CEGUI::EventArgs& e);

		/// @brief
		/// 处理缩小
		/// @param
		/// 
		/// @return
		/// 
		bool HandleZoomOut(const CEGUI::EventArgs& e);

		/// @brief
		/// 处理Slider
		/// @param
		/// 
		/// @return
		/// 
		bool HandlSlider(const CEGUI::EventArgs& e);
	private:
		CEGUI::Slider* m_Slider;///<指向界面的滑块窗口指针
		static float s_SliderInitialValue;///layout文件中Slider配置的初始值
		static float s_SliderClickStepSize;///<layout文件中Slider的ClickStepSize值
		static const float m_CameraMaxZPos;///<定义摄像机距离模型的最大Z值，因为使用的是左手坐标系，所以是负值
		static const float m_CameraMinZPos;///<定义摄像机距离模型的最小Z值
		static const float m_CameraStepZ;///<定义摄像机移动的单位Z值
	};
}