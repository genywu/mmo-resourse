#include "stdafx.h"
#include "CEGUIRenderedStringTextComponent.h"
#include "CEGUIFontManager.h"
#include "CEGUIFont.h"
#include "CEGUISystem.h"
#include "CEGUIExceptions.h"
#include "CEGUITextUtils.h"
#include "RenderedStringRollTextComponent.h"
//#include "CEGUIBase.h"
// Start of CEGUI namespace section
namespace CEGUI
{
	//----------------------------------------------------------------------------//
	RenderedStringRollTextComponent::RenderedStringRollTextComponent():
d_fSpeed(0),
d_iDir(0),
d_font(0),
d_colours(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF)

{
}

//----------------------------------------------------------------------------//
RenderedStringRollTextComponent::RenderedStringRollTextComponent(float speed,
																 int iDir,
																 const String& text):
d_fSpeed(speed),
d_iDir(iDir),
d_text(text),
d_font(0),
d_colours(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF)

{
}

//----------------------------------------------------------------------------//
RenderedStringRollTextComponent::RenderedStringRollTextComponent(float speed,
																 int iDir,
																 const String& text,
																 const String& font_name) :
d_fSpeed(speed),
d_iDir(iDir),
d_text(text),
d_font(font_name.empty() ? 0 : &FontManager::getSingleton().get(font_name)),
d_colours(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF)
{

}

//----------------------------------------------------------------------------//
RenderedStringRollTextComponent::RenderedStringRollTextComponent(float speed,
																 int iDir,
																 const String& text,
																 Font* font):
d_fSpeed(speed),
d_iDir(iDir),
d_text(text),
d_font(font),
d_colours(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF)
{

}
//------------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
void RenderedStringRollTextComponent::setText(const String& text)
{
	d_text = text;
}

//----------------------------------------------------------------------------//
const String& RenderedStringRollTextComponent::getText() const
{
	return d_text;
}

//----------------------------------------------------------------------------//
void RenderedStringRollTextComponent::setFont(Font* font)
{
	d_font = font;
}

//----------------------------------------------------------------------------//
void RenderedStringRollTextComponent::setFont(const String& font_name)
{
	d_font =
		font_name.empty() ? 0 : &FontManager::getSingleton().get(font_name);
}

//----------------------------------------------------------------------------//
Font* RenderedStringRollTextComponent::getFont() const
{
	return d_font;
}

//----------------------------------------------------------------------------//
void RenderedStringRollTextComponent::setColours(const ColourRect& cr)
{
	d_colours = cr;
}

//----------------------------------------------------------------------------//
void RenderedStringRollTextComponent::setColours(const colour& c)
{
	d_colours.setColours(c);
}

//----------------------------------------------------------------------------//
const ColourRect& RenderedStringRollTextComponent::getColours() const
{
	return d_colours;
}
//----------------------------------------------------------------------------//
void RenderedStringRollTextComponent::draw(GeometryBuffer& buffer,
										   const Vector2& position,
										   const ColourRect* mod_colours,
										   const Rect* clip_rect,
										   const float vertical_space,
										   const float space_extra) const
{
	Font* fnt = d_font ? d_font : System::getSingleton().getDefaultFont();

	if (!fnt)
		return;

	Vector2 final_pos(position);
	float y_scale = 1.0f;

	// handle formatting options
	switch (d_verticalFormatting)
	{
	case VF_BOTTOM_ALIGNED:
		final_pos.d_y += vertical_space - getPixelSize().d_height;
		break;

	case VF_CENTRE_ALIGNED:
		final_pos.d_y += (vertical_space - getPixelSize().d_height) / 2 ;
		break;

	case VF_STRETCHED:
		y_scale = vertical_space / getPixelSize().d_height;
		break;

	case VF_TOP_ALIGNED:
		// nothing additional to do for this formatting option.
		break;

	default:
		CEGUI_THROW(InvalidRequestException("RenderedStringTextComponent::draw: "
			"unknown VerticalFormatting option specified."));
	}

	// apply padding to position:
	final_pos += d_padding.getPosition();

	// apply modulative colours if needed.
	ColourRect final_cols(d_colours);
	if (mod_colours)
		final_cols *= *mod_colours;

	// draw the text string.

	//计算时间
	static uint elapseTime = timeGetTime();
	uint curtime = timeGetTime();
	float textWidth = fnt->getTextExtent(d_text);
	float rectWidth = clip_rect->getWidth();
	float rectHeight = clip_rect->getHeight();
	float maxWidth = max(textWidth,rectWidth);

	//计算位移
	float fS = d_fSpeed * (curtime - elapseTime) / 1000;
	if(d_iDir == 1)
	{
		final_pos.d_x -= fS;
		if(fS >= maxWidth)
		{
			elapseTime = timeGetTime();
#ifdef _DEBUG
			char temp[256];
			sprintf_s(temp,256,"RectHeight:%f\n",rectHeight);
			OutputDebugStr(temp);
#endif
		}
	}
	else if (d_iDir == 2)
	{
		final_pos.d_x += fS;
		if(fS >= maxWidth)
			elapseTime = timeGetTime();
	}
	else if (d_iDir == 3)
	{
		final_pos.d_y -= fS;
		if(fS >= maxWidth)
			elapseTime = timeGetTime();
	}
	else if( d_iDir == 4)
	{
		final_pos.d_y += fS;
		if( fS >= rectHeight)
			elapseTime = timeGetTime();
	}
	
	fnt->drawText(buffer, d_text, final_pos, clip_rect, final_cols,
		space_extra, 1.0f, y_scale);
}

//----------------------------------------------------------------------------//
Size RenderedStringRollTextComponent::getPixelSize() const
{
	Font* fnt = d_font ? d_font : System::getSingleton().getDefaultFont();

	Size psz(d_padding.d_left + d_padding.d_right,
		d_padding.d_top + d_padding.d_bottom);

	if (fnt)
	{
		psz.d_width += fnt->getTextExtent(d_text);
		psz.d_height += fnt->getFontHeight();
	}

	return psz;
}

//----------------------------------------------------------------------------//
bool RenderedStringRollTextComponent::canSplit() const
{
	return d_text.length() > 1;
}

//----------------------------------------------------------------------------//
RenderedStringRollTextComponent* RenderedStringRollTextComponent::split(
	float split_point, bool first_component)
{
	Font* fnt = d_font ? d_font : System::getSingleton().getDefaultFont();

	// This is checked, but should never fail, since if we had no font our
	// extent would be 0 and we would never cause a split to be needed here.
	if (!fnt)
		CEGUI_THROW(InvalidRequestException(
		"RenderedStringRollTextComponent::split: "
		"unable to split with no font set."));

	// create 'left' side of split and clone our basic configuration
	RenderedStringRollTextComponent* lhs = new RenderedStringRollTextComponent;
	lhs->d_padding = d_padding;
	lhs->d_verticalFormatting = d_verticalFormatting;
	lhs->d_font = d_font;
	lhs->d_colours = d_colours;
	//速度和方向
	lhs->d_fSpeed = d_fSpeed;
	lhs->d_iDir = d_iDir;

	// calculate the 'best' place to split the text
	size_t left_len = 0;
	float left_extent = 0.0f;

	while (left_len < d_text.length())
	{
		size_t token_len = getNextTokenLength(d_text, left_len);
		// exit loop if no more valid tokens.
		if (token_len == 0)
			break;

		const float token_extent = 
			fnt->getTextExtent(d_text.substr(left_len, token_len));

		// does the next token extend past the split point?
		if (left_extent + token_extent > split_point)
		{
			// if it was the first token, split the token itself
			if (first_component && left_len == 0)
			{
				size_t sz = fnt->getCharAtPixel(d_text.substr(0, token_len), split_point);
				left_len = max(sz,static_cast<size_t>(1));
				/*fnt->getCharAtPixel(
				d_text.substr(0, token_len), split_point));*/
			}
			// left_len is now the character index at which to split the line
			break;
		}

		// add this token to the left side
		left_len += token_len;
		left_extent += token_extent;
	}

	// perform the split.
	lhs->d_text = d_text.substr(0, left_len);

	// here we're trimming leading delimiters from the substring range 
	size_t rhs_start =
		d_text.find_first_not_of(TextUtils::DefaultWrapDelimiters, left_len);
	if (rhs_start == String::npos)
		rhs_start = left_len;

	d_text = d_text.substr(rhs_start);

	return lhs;
}

//----------------------------------------------------------------------------//
size_t RenderedStringRollTextComponent::getNextTokenLength(const String& text,
														   size_t start_idx)
{
	String::size_type word_start =
		text.find_first_not_of(TextUtils::DefaultWrapDelimiters, start_idx);

	if (word_start == String::npos)
		word_start = start_idx;

	String::size_type word_end =
		text.find_first_of(TextUtils::DefaultWrapDelimiters, word_start);

	if (word_end == String::npos)
		word_end = text.length();

	return word_end - start_idx;
}

//----------------------------------------------------------------------------//
RenderedStringRollTextComponent* RenderedStringRollTextComponent::clone() const
{
	RenderedStringRollTextComponent* c = new RenderedStringRollTextComponent(*this);
	return c;
}

//----------------------------------------------------------------------------//
size_t RenderedStringRollTextComponent::getSpaceCount() const
{
	// TODO: The value calculated here is a good candidate for caching.

	size_t space_count = 0;

	// Count the number of spaces in this component.
	// NB: here I'm not countng tabs since those are really intended to be
	// something other than just a bigger space.
	const size_t char_count = d_text.length();
	for (size_t c = 0; c < char_count; ++c)
		if (d_text[c] == ' ') // TODO: There are other space characters!
			++space_count;

	return space_count;
}

//----------------------------------------------------------------------------//
//int RenderedStringRollTextComponent::getPassCount() const
//{
//	return 1;
//}
//void RenderedStringRollTextComponent::performPostRenderFunctions()
//{
//
//}
//void RenderedStringRollTextComponent::performPreRenderFunctions(const int pass)
//{
//	
//}
//bool RenderedStringRollTextComponent::realiseGeometry(CEGUI::RenderingWindow& window, CEGUI::GeometryBuffer& geometry)
//{
//	return false;
//}
//bool RenderedStringRollTextComponent::update(const float elapsed, CEGUI::RenderingWindow& window)
//{
//	CEGUI::System::getSingleton().signalRedraw();
//	return false;
//}
}