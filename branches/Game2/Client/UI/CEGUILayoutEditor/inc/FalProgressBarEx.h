/***********************************************************************
filename:   FalProgressBar.h
created:    Sat Jul 2 2005
author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
#ifndef _FalProgressBarEx_h_
#define _FalProgressBarEx_h_

#include "../../CEGUI/cegui/include/WindowRendererSets/Falagard/FalModule.h"
#include "../../CEGUI/cegui/include/WindowRendererSets/Falagard/FalProgressBarProperties.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
    ProgressBar class for the FalagardBase module.
    */
    class FalagardProgressBarEx : public WindowRenderer
    {
    public:
        static const utf8   TypeName[];     //! type name for this widget.
        /*!
        \brief
        Constructor
        */
        FalagardProgressBarEx(const String& type);

        bool isVertical() const;
        bool isReversed() const;

        void setVertical(bool setting);
        void setReversed(bool setting);

        void render();

    protected:
        // settings to make this class universal.
        bool d_vertical;    //!< True if progress bar operates on the vertical plane.
        bool d_reversed;    //!< True if progress grows in the opposite direction to usual (i.e. to the left / downwards).

        // property objects
        static FalagardProgressBarProperties::VerticalProgress d_verticalProperty;
        static FalagardProgressBarProperties::ReversedProgress d_reversedProperty;
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // end of guard _FalProgressBar_h_
