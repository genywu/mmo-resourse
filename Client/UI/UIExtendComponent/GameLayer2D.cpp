#include "stdafx.h"
#include "GameLayer2D.h"
//#include "../../ui/cegui/cegui/include/RendererModules/Direct3D9/CEGUIDirect3D9GeometryBuffer.h"
#include "../../../Client/Engine/Render.h"
#include "../UIDef.h"
Layer2D::Layer2D():m_pRootBuff(&CEGUI::System::getSingleton().getGUISheet()->getGeometryBuffer()),m_pCustomBuff(0),
                    m_IsMatrixValid(false)
{
    CEGUI::Renderer *pCEGUIRender = CEGUI::System::getSingleton().getRenderer();
    const CEGUI::Rect scrn(CEGUI::Vector2(0, 0), pCEGUIRender->getDisplaySize());
    //render::Interface *pInterface = render::Interface::GetInstance();
    //LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
    //m_p2DBuff = new CEGUI::Direct3D9GeometryBuffer(pDevice);
    m_pRootBuff->setClippingRegion(scrn);

}

Layer2D::~Layer2D()
{

}

void Layer2D::RenderImage(int iX,int iY,int iWidth, int iHeight, const char *imageSetName,const char *image)
{
    const CEGUI::Image &Ima = CEGUI::ImagesetManager::getSingleton().
        get(imageSetName).getImage(image);
    Ima.draw(*m_pRootBuff, CEGUI::Vector2((float)iX,(float)iY) ,CEGUI::Size((float)iWidth, (float)iHeight), 0);
}

void Layer2D::RenderImage(CEGUI::GeometryBuffer& DestBuff, int iX,int iY,int iWidth,int iHeight, const char *imageSetName, const char *image)
{
    const CEGUI::Image &Ima = CEGUI::ImagesetManager::getSingleton().
        get(imageSetName).getImage(image);
    m_pCustomBuff = &DestBuff;
    Ima.draw(*m_pCustomBuff, CEGUI::Vector2((float)iX,(float)iY) ,CEGUI::Size((float)iWidth, (float)iHeight), 0);
}

void Layer2D::RenderText(int iX,int iY,const char *pszString, D3DCOLOR color, int iStyleIndex,const float space_extra,
                         const float x_scale, const float y_scale)
{
    using namespace CEGUI;
    Font* fnt = System::getSingleton().getDefaultFont();
    if (fnt)
    {
        const CEGUI::ColourRect colourRect(color);
        fnt->drawText(*m_pRootBuff, ToCEGUIString(pszString), Vector2((float)iX, (float)iY), 0,
            colourRect, space_extra, x_scale ,y_scale);
    }
}

void Layer2D::RenderRollText(float fStartX,float fStartY, 
							 const char* strRenderText,
							 D3DCOLOR color, 
							 CEGUI::Rect* clip_rect /* = 0 */, 
							 int iPixSpeed /* = 1 */, 
							 int iRollStyle /* = 1 */, 
							 const float x_scale /* = 1.0f */, 
							 const float y_scale /* = 1.0f */, 
							 const float space_extra /* = 0.0f */)
{
	using namespace CEGUI;
	Font* fnt = System::getSingleton().getDefaultFont();
	if(fnt)
	{
		const CEGUI::ColourRect colourRect(color);
		static int elapseTime = timeGetTime();
		int curtime = timeGetTime();
		Vector2 pos(fStartX,fStartY);
		int s = iPixSpeed * (curtime - elapseTime) / 1000;
		switch(iRollStyle){
			case 1://¡û
				pos.d_x = fStartX - s;
				break;
			case 2://¡ú
				pos.d_x = fStartX + s;
				break;
			default:
				break;
		}
		fnt->drawText(*m_pRootBuff,strRenderText,pos,clip_rect,colourRect);
		if( s >= fnt->getTextExtent(strRenderText))
			elapseTime = timeGetTime();
	}
}

void Layer2D::ClearBuffer()
{
    m_pRootBuff->reset();
    if(m_pCustomBuff)
        m_pCustomBuff->reset();
}

void Layer2D::SetupView()
{
    CEGUI::Renderer *pRender = CEGUI::System::getSingleton().getRenderer();
    float width  = pRender->getDisplaySize().d_width;
    float height = pRender->getDisplaySize().d_height;
    if (!m_IsMatrixValid)
    {
        const float fov = 0.523598776f;
        const float w = width;
        const float h = height;
        const float aspect = w / h;
        const float midx = w * 0.5f;
        const float midy = h * 0.5f;
        float fviewDistance = midx / (aspect * 0.267949192431123f);

        D3DXVECTOR3 eye(midx, midy, -fviewDistance);
        D3DXVECTOR3 at(midx, midy, 1);
        D3DXVECTOR3 up(0, -1, 0);

        D3DXMATRIX tmp;
        D3DXMatrixMultiply(&m_matrix,
            D3DXMatrixLookAtRH(&m_matrix, &eye, &at, &up),
            D3DXMatrixPerspectiveFovRH(&tmp, fov, aspect,
            fviewDistance * 0.5f,
            fviewDistance * 2.0f));
        m_IsMatrixValid = true;
    }

    render::Interface *pInterface = render::Interface::GetInstance();
    LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
    D3DVIEWPORT9 vp;
    vp.X = static_cast<DWORD>(0);
    vp.Y = static_cast<DWORD>(0);
    vp.Width = static_cast<DWORD>(width);
    vp.Height = static_cast<DWORD>(height);
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;
    pDevice->SetViewport(&vp);
    pDevice->SetTransform(D3DTS_PROJECTION, &m_matrix);
}