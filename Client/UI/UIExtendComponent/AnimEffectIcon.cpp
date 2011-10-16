#include "stdafx.h"
#include "AnimEffectIcon.h"

AnimEffectIcon::AnimEffectIcon(CEGUI::Window * owner,CEGUI::Imageset & ImageSet, bool isLooped):AnimImageSet(ImageSet)
{
    OwnerWnd = owner;
    bIsLoop = isLooped;
    lFrameCount = AnimImageSet.getImageCount();
    lStartFrame = 0;
    EndFrame = lFrameCount-1;
    lCurFrame = 0;
    lFrameTime = 0;
    lLastDrawTime = 0;
    dwAnimTotalTime = 0;
    dwAnimStartTime = 0;
    bIsAniming = false;
}

AnimEffectIcon::~AnimEffectIcon()
{

}

//每一帧更新
bool AnimEffectIcon::update()
{
    static char strsetImage[128] = "";
    if (OwnerWnd && OwnerWnd->isVisible() && 
        bIsAniming && lLastDrawTime - timeGetTime() > lFrameTime)
    {
        lCurFrame++;
        if (lCurFrame >= lFrameCount  )
        {
            if(bIsLoop)
                lCurFrame = 1;
            else
            {    
                lCurFrame = 1;
                bIsAniming = false;
                OwnerWnd->setProperty("Image","");
                OwnerWnd->notifyScreenAreaChanged(true);
                return false;
            }
            lLastDrawTime = timeGetTime();
        }
        sprintf_s<128>(strsetImage,"set:%s image:%d",AnimImageSet.getName().c_str(),lCurFrame);
        OwnerWnd->setProperty("Image",strsetImage);
        OwnerWnd->notifyScreenAreaChanged(true);
    }
    return false;
}

void AnimEffectIcon::BeginAnimateEffect(DWORD totalTime)
{
    dwAnimTotalTime = totalTime;
    bIsAniming = true;
    lFrameTime = totalTime / lFrameCount;
    lLastDrawTime = dwAnimStartTime = timeGetTime();
    lCurFrame = 0;
}