#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\frostengine\ui.h"

#include "WndAbout.h"

WndAbout::WndAbout(void)
{
	m_pListUpdateInfo = NULL;
	m_strEditionTile = "Model Viewer 5.0.6.4";
}

WndAbout::~WndAbout(void)
{
}

BOOL WndAbout::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rc = {0,0,80,20};
	m_pBtnOK = new ui::Button;
	VERIFY(m_pBtnOK->Create("确定",&rc,FWS_VISIBLE,this));
	m_pBtnOK->CenterWndTo((m_tWindowInfo.rcRect.right - m_tWindowInfo.rcRect.left)/2,pRect->bottom - 24);	

	RECT rcList = {0,50,300,470};
	m_pListUpdateInfo = new ui::ListBox;
	m_pListUpdateInfo->Create("UpdateInfo",&rcList,FWS_VISIBLE,this);

	// 第一段：大功能改进（框架级别），第二段增加新的小功能，第三段修改目前程序以修正BUG，第四段小的非BUG类型的程序修改
	static vector<string> vecInfo;
	vecInfo.push_back("<版本修正>");
	vecInfo.push_back("1.0.0.0 (2003 ~ 2004.1 贾涛)");
	vecInfo.push_back("2.0.0.0 (2007.12.6 lpf)");
	vecInfo.push_back("  采用新的异步读取模式");
	vecInfo.push_back("2.0.5.2 (2007.12.14)");
	vecInfo.push_back("  王桥 - 修正了在选择显示模型部件相应等级时,程");
	vecInfo.push_back("         序不确定性的崩溃");
	vecInfo.push_back("  lpf  - 修正了具有等级属性的部件无法显示颜色动");
	vecInfo.push_back("         画和渐隐动画");
	vecInfo.push_back("3.0.0.0 (2008.01.04 lpf)");
	vecInfo.push_back("  修改了模型载入流程");
	vecInfo.push_back("  主要增加了绑定模型列表和角色模型列表");
	vecInfo.push_back("  修正了渲染模型将UI控件遮挡问题");
	vecInfo.push_back("  全面调整了快捷键操作");
	vecInfo.push_back("3.1.0.3 (2008.01.07 lpf)");
	vecInfo.push_back("  增加了显示背景图片的功能");
	vecInfo.push_back("  对所有模型加载时的文件名进行了错误提示判断");
	vecInfo.push_back("3.1.1.3 (2008.01.17 lpf)");
	vecInfo.push_back("  修正了载入名字过长的模型文件时,程序会崩溃.");
	vecInfo.push_back("3.2.2.3 (2008.01.18 彭谦)");						//line: 20"
	vecInfo.push_back("  增加了投影纹理显示的功能");
	vecInfo.push_back("  当投影纹理的配置文件不存在时,载入出错会报告");
	vecInfo.push_back("3.2.3.3 (2008.01.24 lpf)");
	vecInfo.push_back("  在绑定点对话框中增加了一个[坐标原点]绑定点");
	vecInfo.push_back("3.2.5.3 (2008.01.30 彭谦)");
	vecInfo.push_back("  修正了关于投影纹理的动画效果错误");
	vecInfo.push_back("  F5刷新动画时,投影纹理动画也会被重置");
	vecInfo.push_back("3.3.4.3 (2008.02.02 lpf)");
	vecInfo.push_back("  缩短了连续按F5刷新动画时间间隔");
	vecInfo.push_back("  按F5刷新动画时,将当前所有模型动画从头播放");
	vecInfo.push_back("3.4.5.4 (2008.02.28 lpf)");
	vecInfo.push_back("  修正了拖尾模型的纹理不能正常显示");
	vecInfo.push_back("  增加载入模型后按住Ctrl,移动鼠标可移动模型");
	vecInfo.push_back("3.4.6.4 (2008.03.26 彭谦)");
	vecInfo.push_back("  增加了公告牌的缩放功能");
	vecInfo.push_back("3.5.6.4 (2008.04.02 彭谦)");
	vecInfo.push_back("  修改了关于粒子效果的功能");
	vecInfo.push_back("3.6.7.5 (2008.04.02 lpf)");
	vecInfo.push_back("  增加了动画帧导出功能");
	vecInfo.push_back("  增加了模型尾迹显示");
	vecInfo.push_back("3.8.7.5 (2008.06.17 lpf)");
	vecInfo.push_back("  同步了新的引擎底层");
	vecInfo.push_back("   增加了取景器功能");				
	vecInfo.push_back("4.0.0.0 (2008.07.15)");							//line:44
	vecInfo.push_back("  倪凯 - 增加了对模型Shader的编辑功能");
	vecInfo.push_back("  彭谦 - 粒子发射器增加了新的功能");
	vecInfo.push_back("4.1.0.0 (2008.07.16 倪凯)");
	vecInfo.push_back("  编辑Shader时,增加了替换纹理功能");
	vecInfo.push_back("4.2.0.0 (2008.07.28 倪凯)");
	vecInfo.push_back("  增加了Model，part，trial，particle文件的");
	vecInfo.push_back("  编辑功能");
	vecInfo.push_back("4.2.1.0 (2008.09.24 lpf)");
	vecInfo.push_back("  增加了导出动画帧时,包含参考模型动画");
	vecInfo.push_back("4.2.1.1 (2008.11.19 彭谦)");
	vecInfo.push_back("  修改了粒子渐变的BUG");
	vecInfo.push_back("4.3.2.1 (2008.12.31 lpf)");
	vecInfo.push_back("  增加了运动地表功能,方便美工对步伐动作调整");
	vecInfo.push_back("  修正了3D渲染层在进行_DEBUG_DrawTriangle时,纹");
	vecInfo.push_back("  理坐标数据源的Stride设定错误");
	vecInfo.push_back("4.4.2.1 (2009.2.3 彭谦)");
	vecInfo.push_back("  增加了粒子自身空间变换功能");
	vecInfo.push_back("4.6.2.2 (2010.1.5 Lpf)");
	vecInfo.push_back("  增加了模型组件Part项超出程序高度时,用新窗体处");
	vecInfo.push_back("  理的功能;");
	vecInfo.push_back("  新增自定义导出动画帧图像尺寸的功能;");
	vecInfo.push_back("  修正了2D坐标轴显示错位;");
	vecInfo.push_back("4.7.5.4 (2010.1.25 Lpf)");
	vecInfo.push_back("  修正F5键刷新后模型部件选中状态被归位");
	vecInfo.push_back("  修正绑定模型类似盾之类的装备无法隐藏;");
	vecInfo.push_back("  导出动画帧包括绑定模型;");
	vecInfo.push_back("  修正了只有索引为0的第一个绑定模型装备设定有效;");
	vecInfo.push_back("  修正点击第二个绑定模型More的部件列表会崩溃;");
	vecInfo.push_back("  导出动画帧文件名扩展为8位数;");
	vecInfo.push_back("4.7.6.4 (2010.2.4 Lpf)");
	vecInfo.push_back("  扩展了菜单项Item数量为128,原来是64,否则会崩溃;");
	vecInfo.push_back("4.8.6.4 (2010.2.22 Lpf)");
	vecInfo.push_back("  增加了是否显示UI的功能,可通过快捷键Ctrl+H实现;");
	vecInfo.push_back("4.9.6.4 (2010.4.08 Lpf)");
	vecInfo.push_back("  增加了水面效果模型的读取与显示;");
	vecInfo.push_back("5.0.6.4 (2010.4.13 Lpf)");
	vecInfo.push_back("  增加了水面效果模型外形控制,增加了圆形水面;");

	for (size_t st = 0; st < vecInfo.size(); ++st)
		m_pListUpdateInfo->InsertItem(vecInfo[st].c_str());

	return TRUE;
}

void WndAbout::Destroy()
{
	SAFEDESTROY(m_pBtnOK);
	SAFEDESTROY(m_pListUpdateInfo);
	ui::Wnd::Destroy();
}

void WndAbout::Update()
{
	ui::Wnd::UpdateDefault();
	
	if (m_pBtnOK->IsClicked())
	{
		//关闭自己
		ModifyStyle(0,FWS_VISIBLE);
		ui::Manager::GetInstance()->EndPopupWnd();
	}

	ui::Wnd::UpdateChild();
}

void WndAbout::Render()
{
	render::Interface		   * pInterface	= render::Interface::GetInstance();
	render::Interface::Layer2D * pLayer2D   = pInterface->GetLayer2D();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	if (dwStyle & FWS_VISIBLE)
	{
		RECT &rc = m_tWindowInfo.rcRect;
		int iX	 = rc.left, iY = rc.top, iW = rc.right - rc.left + 1, iH = rc.bottom - rc.top + 1;

		

		// 绘制背景
		pLayer2D->DrawSolidQuad(iX + 4, iY + 4, iW, iH, m_tWindowInfo.colorLow);
		pLayer2D->DrawSolidQuad(iX, iY, iW, iH, m_tWindowInfo.colorBack);
		pLayer2D->DrawWireQuad(iX, iY, iW, iH, m_tWindowInfo.colorHigh);

		// 绘制内容
		DWORD dwTextWidth = pLayer2D->CalcTextWidth(m_strEditionTile.c_str(), (int)m_strEditionTile.length());
		int iStartX = iX + iW / 2 - dwTextWidth / 2;
		pLayer2D->DrawSolidQuad(iStartX - 3, iY + 12, dwTextWidth + 5, pLayer2D->GetFontSize() + 8, 0xf0001020);
		ui::Wnd::SetUIRendState();
		pLayer2D->TextOut(iStartX, iY + 14, m_strEditionTile.c_str(), (int)m_strEditionTile.length(), m_tWindowInfo.colorText);

		// 绘制版本历史
		/*int iPosX = iX + 12;
		int iPosY = iY + 30;
		for (int i = 1; i < 52; ++i)
			pLayer2D->TextOut(iPosX, iPosY += (pLayer2D->GetFontSize() + 3), szInfo[i], lstrlen(szInfo[i]), m_tWindowInfo.colorText);*/
	}

	ui::Wnd::RenderChild();
}
void WndAbout::BeginPopUp(void)
{
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}
void WndAbout::EndPopUp(void)
{
	ModifyStyle(0,FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
}