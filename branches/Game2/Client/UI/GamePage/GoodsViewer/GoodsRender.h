/**
* @filename:GoodsRender.h
* @date: 2010/3/17
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 物品的渲染控制类
*/

class CDisplayModel;
class AnimInfo;
class CGameModelManager;

//宏定义默认物品的放缩值
#ifndef DEFAULT_SCALE
#define DEFAULT_SCALE 1500.0f
#endif

#ifndef DEFAULT_ZDISTANCE
#define DEFAULT_ZDISTANCE 1000
#endif

#ifndef DEFAULT_XTRANS
#define DEFAULT_XTRANS 370
#endif

#ifndef DEFAULT_YTRANS
#define DEFAULT_YTRANS 0
#endif

class GoodsRender 
{
public:
	GoodsRender();
	~GoodsRender();

	/// @brief
	/// 根据模型路径和ID加载模型
	/// @param PathName
	/// 模型文件的存储路径
	/// @param ID
	/// 模型的Graphite ID
	/// @return
	/// 
	static bool LoadModel(uint ID,const char* PathName ="model/goods");

	/// @brief
	/// 模型的渲染	
	/// @param
	/// 
	/// @return
	/// 
	static void Render();

	/// @brief
	/// 重新设置旋转角度增量
	/// @param xRot
	/// 绕X轴旋转角度增量
	/// @param yRot
	/// 绕Y轴旋转角度增量
	/// @param zRot
	/// 绕Z轴旋转角度增量
	/// @return
	/// 
	static void ReSetRotAngle(float xRot = 5.0f ,float yRot = 5.0f ,float zRot = 5.0f) ;

	/// @brief
	/// 重新设置模型放缩值
	/// @param scale
	/// 放缩值大小
	/// @return
	/// 
	static void ResetScal(float scale = 2.0f);

	/// @brief
	/// 重新设置模型位置
	/// @param xTran
	/// 模型在X轴上的平移
	/// @param yTran
	/// 模型在Y轴上的平移
	/// @param zTran
	/// 模型在Z轴上的平移
	/// @return
	/// 
	static void ResetTranslation(float xTran = 1 ,float yTran = 1 ,float zTran = 1 );
	/// @brief
	/// 设置各轴上的平移增量值
	/// @param xTranAdd
	/// 模型在X轴上的平移增量
	/// @param yTran
	/// 模型在Y轴上的平移增量
	/// @param zTran
	/// 模型在Z轴上的平移增量
	/// @return
	static void SetTranAdd(float xTranAdd = 1,float yTranAdd = 1, float zTranAdd = 1); 
protected:

private:
	static float m_XRot;
	static float m_YRot;
	static float m_ZRot;
	static float m_Scale;
	static float m_XTrans;
	static float m_YTrans;
	static float m_ZTrans;

	//用于模型显示
	static CDisplayModel* m_DispModel;
	static CGameModelManager* m_GameModelManager;
	static AnimInfo* m_AniInfo;//模型的动画
};