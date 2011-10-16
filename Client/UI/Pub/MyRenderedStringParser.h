
/*客户端扩展CEGUI文本解析 方便以后追加新的文本解析标记 2010*5.19*/
#pragma once
#include "../../ui/cegui/cegui/include/CEGUIBasicRenderedStringParser.h"
using namespace CEGUI;

class MyRenderedStringParser : public BasicRenderedStringParser
{
public:
   
    MyRenderedStringParser();
    virtual ~MyRenderedStringParser();
    static const String NewLineName;
    static const String AutoMoveName;
    static const String ColorName;
    static const String ImageName;
    static const String GoodsName;
    static const String LinkBtnName;
	static const String RollTextName;
    virtual RenderedString parse(const String& input_string,
        Font* initial_font,
        const ColourRect* initial_colours);
    virtual void processControlString(RenderedString& rs, const String& ctrl_str);
    void handleAutoMove(RenderedString& rs, const String& value);
    void handleNewLine(RenderedString& rs, const String& value);
    void handleColour(RenderedString& rs, const String& value);
    void handleImage(RenderedString& rs, const String& value);
    void handleGoods(RenderedString& rs, const String& value);
    void handleLinkBtn(RenderedString& rs, const String& value);
	/***********************************************************************/
	/*  zhaohang 2010-10-28 add*/
	/***********************************************************************/
	//解析滚动文本
	void handleRollText(RenderedString& rs,const String& value);

	virtual void appendRenderedText(RenderedString& rs, const String& text) const;

	void initialiseDefaultState();
protected:
	mutable float d_fSpeed;
	mutable CEGUI::uint d_iDir;
};

