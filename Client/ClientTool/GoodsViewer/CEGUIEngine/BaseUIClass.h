/**
* @filename:BaseUIClass.h
* @date: 2010/1/27
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 

* @purpose: 所有UI类(界面窗口管理，事件响应)的纯虚基类，定义UI类时必须从BaseUIClass类派生,\
*           定义这个是为了方便CEGUIManager执行UI类的构造函数
*/

#pragma  once
///前向声明
class CEGUIManager;
#include <CEGUI.h>
namespace UIClass
{
	using namespace CEGUI;
	class BaseUIClass
	{
	public:
		BaseUIClass(){};
		virtual ~BaseUIClass(){};
		//virtual  BaseUIClass* CreateObjClass() = 0;
		//virtual bool DestroyObjClass(BaseUIClass *pBaseUIClass) = 0;
		virtual bool Init()=0;///<纯虚函数，派生类重写这个函数，做CEGUI窗口创建、挂载到根窗口，事件处理函数的注册等工作
	protected:
		Window* m_RootWindow;///< 指向layout的RootWindow
	private:
		
		friend CEGUIManager;///<CEGUIManager作为友元类，使得可以调用Create*，Destory*函数
	};
}