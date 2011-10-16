#include "stdafx.h"
#include "MyRenderedStringParser.h"
#include "AutoMoveParser.h"
#include "GoodsParser.h"
#include "LinkButtonParser.h"
#include "RenderedStringRollTextComponent.h"
#include "../UIDef.h"
const String MyRenderedStringParser::NewLineName("BR");
const String MyRenderedStringParser::AutoMoveName("MOVE");
const String MyRenderedStringParser::ColorName("COLOR");
const String MyRenderedStringParser::ImageName("IMG");
const String MyRenderedStringParser::GoodsName("GOODS");
const String MyRenderedStringParser::LinkBtnName("LINK");
const String MyRenderedStringParser::RollTextName("ROLL");

MyRenderedStringParser::MyRenderedStringParser():
d_fSpeed(0.0f),
d_iDir(0)
{
    
}
MyRenderedStringParser::~MyRenderedStringParser()
{

}
RenderedString MyRenderedStringParser::parse(const String& input_string,
                                    Font* initial_font,
                                    const ColourRect* initial_colours)
{ 
    const char * ParseText = CEGUIStringToAnsiChar(input_string);
    // first-time initialisation (due to issues with static creation order)
    if (!d_initialised)
    {
        d_tagHandlers[NewLineName] = static_cast<CEGUI::BasicRenderedStringParser::TagHandler>(&MyRenderedStringParser::handleNewLine);
        d_tagHandlers[AutoMoveName] = static_cast<CEGUI::BasicRenderedStringParser::TagHandler>(&MyRenderedStringParser::handleAutoMove);
        d_tagHandlers[ColorName] = static_cast<CEGUI::BasicRenderedStringParser::TagHandler>(&MyRenderedStringParser::handleColour);
        d_tagHandlers[ImageName] = static_cast<CEGUI::BasicRenderedStringParser::TagHandler>(&MyRenderedStringParser::handleImage);
        d_tagHandlers[GoodsName] = static_cast<CEGUI::BasicRenderedStringParser::TagHandler>(&MyRenderedStringParser::handleGoods);
        d_tagHandlers[LinkBtnName] = static_cast<CEGUI::BasicRenderedStringParser::TagHandler>(&MyRenderedStringParser::handleLinkBtn);
		d_tagHandlers[RollTextName] = static_cast<CEGUI::BasicRenderedStringParser::TagHandler>(&MyRenderedStringParser::handleRollText);
        initialiseTagHandlers();
		d_fSpeed = 0;
		d_iDir = 0;
    }

    initialiseDefaultState();

    // maybe override initial font.
    if (initial_font)
        d_fontName = initial_font->getName();

    // maybe override initial colours.
    if (initial_colours)
        d_colours = *initial_colours;

    RenderedString rs;
    String curr_section;
    size_t curr_pos = 0;
    while (curr_pos < input_string.length())
    {
        size_t cstart_pos = input_string.find_first_of('[', curr_pos);

        // if no control sequence start char, add remaining text verbatim.
        if (String::npos == cstart_pos)
        {
            curr_section += input_string.substr(curr_pos);
            curr_pos = input_string.length();
        }
        else if (cstart_pos == curr_pos || input_string[cstart_pos - 1] != '\\')
        {
            // append everything up to the control start to curr_section.
            curr_section += input_string.substr(curr_pos, cstart_pos - curr_pos);

            // scan forward for end of control sequence
            size_t cend_pos = input_string.find_first_of(']', cstart_pos);
            // if not found, treat as plain text
            if (String::npos == cend_pos)
            {
                curr_pos++;
                curr_section += input_string.substr(curr_pos);
                curr_pos = input_string.length();
            }
            // extract control string
            else
            {
                appendRenderedText(rs, curr_section);
                curr_section.clear();

                String ctrl_string(
                    input_string.substr(cstart_pos + 1,
                    cend_pos - cstart_pos - 1));
                
                curr_pos = cend_pos + 1;
                processControlString(rs, ctrl_string);
                continue;
            }
        }
        else
        {
            curr_section += input_string.substr(curr_pos,
                cstart_pos - curr_pos - 1);
            curr_section += '[';
            curr_pos = cstart_pos + 1;
            continue;
        }

        appendRenderedText(rs, curr_section);
        curr_section.clear();
    }
    return rs;
}

void MyRenderedStringParser::processControlString(RenderedString& rs,
                                                     const String& ctrl_str)
{
    String var_str = "";
    String val_str = "";

    size_t spacePos = ctrl_str.find(" ");
    if (spacePos == String::npos)
    {
        var_str = ctrl_str;
    }
    else
    {
        var_str = ctrl_str.substr(0,spacePos);
        val_str = ctrl_str.substr(spacePos+1);
    }
   
    // look up handler function
    TagHandlerMap::iterator i = d_tagHandlers.find(var_str);
    // despatch handler, or log error
    if (i != d_tagHandlers.end())
    {
        (this->*(*i).second)(rs, val_str);
        return;
    }
    else
    {
        String appText = "[";
        appText += ctrl_str;
        appText += "]";
        appendRenderedText(rs,appText);
        Logger::getSingleton().logEvent(
            "BasicRenderedStringParser::processControlString: unknown "
            "control variable '" + var_str + "'.  Ignoring!");
        return;
    }
}
void MyRenderedStringParser::appendRenderedText(RenderedString& rs, const String& text) const
{
	size_t cpos = 0;
	// split the given string into lines based upon the newline character
	while (text.length() > cpos)
	{
		// find next newline
		const size_t nlpos = text.find('\n', cpos);
		// calculate length of this substring
		const size_t len =
			((nlpos != String::npos) ? nlpos : text.length()) - cpos;

		// construct new text component and append it.
		if (len > 0)
		{
			RenderedStringRollTextComponent rtc(d_fSpeed,d_iDir,text.substr(cpos, len), d_fontName);
			//RenderedStringTextComponent rtc(text.substr(cpos,len),d_fontName);
			rtc.setPadding(d_padding);
			rtc.setColours(d_colours);
			rtc.setVerticalFormatting(d_vertAlignment);
			rtc.setAspectLock(d_aspectLock);
			rs.appendComponent(rtc);
		}

		// break line if needed
		if (nlpos != String::npos)
			rs.appendLineBreak();

		// advance current position.  +1 to skip the \n char
		cpos += len + 1;
	}
}
void MyRenderedStringParser::handleNewLine(RenderedString& rs, const String& value)
{
    rs.appendLineBreak();
}

void MyRenderedStringParser::handleAutoMove(RenderedString& rs, const String& value)
{
    //解析[MOVE] 标记
    AutoMoveParser amparser;
    CEGUI::String WndName = amparser.Parse(value);  
    RenderedStringWidgetComponent rwc(WndName);
    rwc.setPadding(d_padding);
    rwc.setVerticalFormatting(d_vertAlignment);
    rwc.setAspectLock(d_aspectLock);
    rs.appendComponent(rwc);
}

void MyRenderedStringParser::handleColour(RenderedString& rs, const String& value)
{
    BasicRenderedStringParser::handleColour(rs,value);
}

void MyRenderedStringParser::handleImage(RenderedString& rs, const String& value)
{
    BasicRenderedStringParser::handleImage(rs,value);
}

void MyRenderedStringParser::handleGoods(RenderedString& rs, const String& value)
{
    //解析[GOODS]标记
    GoodsParser goodsParser;
    CEGUI::String wndName = goodsParser.Parse(value);
    RenderedStringWidgetComponent rwc(wndName);
    rwc.setPadding(d_padding);
    rwc.setVerticalFormatting(d_vertAlignment);
    rwc.setAspectLock(d_aspectLock);
    rs.appendComponent(rwc);
}

void MyRenderedStringParser::handleLinkBtn(RenderedString& rs, const String& value)
{
    LinkButtonParser linkParser;
    CEGUI::String wndName = linkParser.Parse(value);
    RenderedStringWidgetComponent rwc(wndName);
    rwc.setPadding(d_padding);
    rwc.setVerticalFormatting(d_vertAlignment);
    rwc.setAspectLock(d_aspectLock);
    rs.appendComponent(rwc);
}

//
void MyRenderedStringParser::handleRollText(RenderedString& rs,const String& value)
{
	using namespace std;
	if( value.empty())
		return ;

	sscanf(value.c_str()," 'Speed:%f Dir:%d'",&d_fSpeed,&d_iDir);

}

void MyRenderedStringParser::initialiseDefaultState()
{
	d_fSpeed = 0.0f;
	d_iDir = 0;
	d_padding = Rect(0, 0, 0, 0);
	d_colours = d_initialColours;
	d_fontName = d_initialFontName;
	d_imageSize.d_width = d_imageSize.d_height = 0.0f;
	d_vertAlignment = VF_BOTTOM_ALIGNED;
	d_aspectLock = false;
}