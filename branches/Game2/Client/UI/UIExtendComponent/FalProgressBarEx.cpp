/***********************************************************************
filename:   FalProgressBarEx.cpp
created:    2010.10.21
author:     nikia 
*************************************************************************/
/*
    在原有FalProgressBar渲染器上 文本渲染
*/
#include "stdafx.h"
#include "FalProgressBarEx.h"
namespace CEGUI
{
    const utf8 FalagardProgressBarEx::TypeName[] = "Falagard/ProgressBarEx";
    FalagardProgressBarProperties::VerticalProgress FalagardProgressBarEx::d_verticalProperty;
    FalagardProgressBarProperties::ReversedProgress FalagardProgressBarEx::d_reversedProperty;
    FalagardProgressBarEx::FalagardProgressBarEx(const String& type):
    WindowRenderer(type,"ProgressBar"),
        d_vertical(false),
        d_reversed(false)
    {
        registerProperty(&d_verticalProperty);
        registerProperty(&d_reversedProperty);
    }
    void FalagardProgressBarEx::render()
    {
        const StateImagery* imagery;

        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();
        // try and get imagery for our current state
        imagery = &wlf.getStateImagery(d_window->isDisabled() ? "Disabled" : "Enabled");
        // peform the rendering operation.
        imagery->render(*d_window);

        // get imagery for actual progress rendering
        imagery = &wlf.getStateImagery(d_window->isDisabled() ? "DisabledProgress" : "EnabledProgress");

        // get target rect for this imagery
        Rect progressRect(wlf.getNamedArea("ProgressArea").getArea().getPixelRect(*d_window));

        // calculate a clipper according to the current progress.
        Rect progressClipper(progressRect);

        ProgressBar* w = (ProgressBar*)d_window;
        if (d_vertical)
        {
            float height = PixelAligned(progressClipper.getHeight() * w->getProgress());

            if (d_reversed)
            {
                progressClipper.setHeight(height);
            }
            else
            {
                progressClipper.d_top = progressClipper.d_bottom - height;
            }
        }
        else
        {
            float width = PixelAligned(progressClipper.getWidth() * w->getProgress());

            if (d_reversed)
            {
                progressClipper.d_left = progressClipper.d_right - width;
            }
            else
            {
                progressClipper.setWidth(width);
            }
        }

        // peform the rendering operation.
        imagery->render(*d_window, progressRect, 0, &progressClipper);

        // perform the text rendering operation
        imagery = &wlf.getStateImagery("Label");
        imagery->render(*d_window);
    }

    bool FalagardProgressBarEx::isVertical() const
    {
        return d_vertical;
    }

    bool FalagardProgressBarEx::isReversed() const
    {
        return d_reversed;
    }

    void FalagardProgressBarEx::setVertical(bool setting)
    {
        d_vertical = setting;
    }

    void FalagardProgressBarEx::setReversed(bool setting)
    {
        d_reversed = setting;
    }
}
