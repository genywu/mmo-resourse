/**
* @filename:AnimEffectIcon.h
* @date: 2010/3/4
* @author:  nikai
* @purpose: 序列帧动画
*/

#pragma  once

namespace CEGUI
{
    class String;
    class Imageset;
}

class AnimEffectIcon
{
public:
    AnimEffectIcon(CEGUI::Window * owner,CEGUI::Imageset & ImageSet, bool isLooped = false);
	
    ~AnimEffectIcon();
    
    //每一帧更新
    bool update();

    //设置开始播放
    /*param:totalTime 动画播放总时间*/
    void BeginAnimateEffect(DWORD totalTime);
private:
    // 容器窗口指针
    CEGUI::Window * OwnerWnd;

    // 动画的帧数
    long lFrameCount;

    // 动画开始帧
    long lStartFrame;

    // 动画结束帧
    long EndFrame;

    // 动画的当前帧
    long lCurFrame;
   
    // 动画每帧的时间
    unsigned long lFrameTime;
    
    // 最后一次绘制的时间
    unsigned long lLastDrawTime;
   
    // 动画循环标志
    bool bIsLoop;

    //动画播放总时间
    DWORD dwAnimTotalTime;

    //动画播放开始时间
    DWORD dwAnimStartTime;

    //是否正常播放中
    bool  bIsAniming;
    
    //动画图像集
    CEGUI::Imageset & AnimImageSet;
};