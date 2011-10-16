/**
* @filename:3DIcon.h
* @date: 2010/3/4
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 结合CEGUI+光芒引擎实现3DIcon控件
*/


#pragma  once

//class CBitmapX;
//class GameGraphics2D;
class CShape;
class AnimInfo;
namespace CEGUI
{
    class String;
    class WindowEventArgs;
    class RenderingSurface;
    struct Vertex;
}
class C3DIconEffect:public CEGUI::RenderEffect
{
public:
    C3DIconEffect(CEGUI::Window * owner);
    
	~C3DIconEffect();

    int getPassCount() const;
    void performPreRenderFunctions(const int pass);
    void performPostRenderFunctions();
	
	///  把纹理效果添加到窗口容器上
	bool realiseGeometry(CEGUI::RenderingWindow& window, CEGUI::GeometryBuffer& geometry);
    
    //每一帧更新
    bool update(const float elapsed, CEGUI::RenderingWindow& window);

	/// @brief
	/// 重新设置旋转角度
	/// @param angle
	/// 旋转的角度(单位°) 
	/// @return void
	/// 
	void ReSetRotAngle(float angle){ m_RotAngle = angle ; }
    float GetRotAngle()             {return m_RotAngle;}

	//设置需要显示的Shape
    void SetIconShape(CShape *pShape);

    //设置模型旋转
    void SetLeftRot(bool b) { m_bLeftRotation = b; }
    void SetRightRot(bool b) { m_bRightRotation = b; }

    //设置渲染位置
    void SetIconPos(float x,float y,float z) {m_DrawX = x; m_DrawY = y; m_DrawZ = z;}

    //设置缩放值
    void SetIconScale(float xScale,float yScale,float zScale)
        {m_Scal[0] = xScale;
         m_Scal[1] = yScale;
         m_Scal[2] = zScale;}
    void SetRttType(long type)   {m_lRttType = type;}
protected:
	
	CShape   *m_Shape;//模型数据
	AnimInfo *m_AniInfo;//动画数据
	unsigned int m_ActionNum;//动作名称
	unsigned int m_RotDir;//旋转方向（0.不装 1.右转 -1.左转)
	
	float m_DrawX;//模型的X坐标
	float m_DrawY;//模型的Y坐标
    float m_DrawZ;//模型的Z坐标
	float m_RotAngle;//模型的旋转角度

	float m_Scal[3];//缩放
    int   m_3DIconTextureID;  //渲染到模型纹理ID
    long  m_lRttType;
private:
    CEGUI::Vertex  vb[6];
    CEGUI::Window  *ownerWnd;
    bool    m_bLeftRotation;
    bool    m_bRightRotation;
 
};
