/********************************************************************
created:	2010/10/28
created:	28:10:2010   14:03
filename: 	e:\SVN_P6\trunk\Client\UI\UIExtendComponent\RenderedStringRollTextComponent.h
file path:	e:\SVN_P6\trunk\Client\UI\UIExtendComponent
file base:	RenderedStringRollTextComponent
file ext:	h
author:		zhaohang(kaba) (kuailekaba@gmail.com)

purpose:	滚动文字的绘制组件
*********************************************************************/
#include "CEGUIRenderedStringComponent.h"
namespace CEGUI{
	class RenderedStringRollTextComponent:public CEGUI::RenderedStringComponent
		//,public CEGUI::RenderEffect
	{
	public:
		//! Constructor
		RenderedStringRollTextComponent();
		RenderedStringRollTextComponent(float speed,int dir,const CEGUI::String& text);
		RenderedStringRollTextComponent(float speed,int dir,const CEGUI::String& text, const CEGUI::String& font_name);
		RenderedStringRollTextComponent(float speed,int dir,const CEGUI::String& text, CEGUI::Font* font);
		//! Set the text to be rendered by this component.
		void setText(const String& text);
		//! return the text that will be rendered by this component
		const String& getText() const;
		//! set the font to use when rendering the text.
		void setFont(Font* font);
		//! set the font to use when rendering the text.
		void setFont(const String& font_name);
		//! return the font set to be used.  If 0 the default font will be used.
		Font* getFont() const;
		//! Set the colour values used when rendering this component.
		void setColours(const ColourRect& cr);
		//! Set the colour values used when rendering this component.
		void setColours(const colour& c);
		//! return the ColourRect object used when drawing this component.
		const ColourRect& getColours() const;

		// implementation of abstract base interface
		void draw(GeometryBuffer& buffer, const Vector2& position,
			const ColourRect* mod_colours, const Rect* clip_rect,
			const float vertical_space, const float space_extra) const;

		Size getPixelSize() const;
		bool canSplit() const;

		RenderedStringRollTextComponent* split(float split_point, bool first_component);
		RenderedStringRollTextComponent* clone() const;
		size_t getSpaceCount() const;

		void SetSpeed(float speed);
		float GetSpeed();
		void SetDir(int iDir);
		int GetDir();

	protected:
		static size_t getNextTokenLength(const String& text, size_t start_idx);

		//! pointer to the image drawn by the component.
		String d_text;
		//! Font to use for text rendering, 0 for system default.
		Font* d_font;
		//! ColourRect object describing the colours to use when rendering.
		ColourRect d_colours;

		//! roll Speed
		float d_fSpeed;
		//! roll Direct
		int d_iDir;
	};
}