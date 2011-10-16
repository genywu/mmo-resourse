// WebUIController.h: interface for the CWebUIController class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_WEBUICONTROLLER_H__4F772AB0_A3CE_4378_BA35_C1E144CE1BDD__INCLUDED_)
//#define AFX_WEBUICONTROLLER_H__4F772AB0_A3CE_4378_BA35_C1E144CE1BDD__INCLUDED_
//
//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000
//
//class CWebUIController  
//{
//public:
// CWebUIController();
// virtual ~CWebUIController();
//
//};
//
//#endif // !defined(AFX_WEBUICONTROLLER_H__4F772AB0_A3CE_4378_BA35_C1E144CE1BDD__INCLUDED_)


#ifndef __WEBUICONTROLLER_H__
#define __WEBUICONTROLLER_H__
 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
 
#include <MSHTMHST.H>   
#include <EXDISP.H>
#include <EXDISPID.H> 
 
inline 
HRESULT _CoAdvise(IUnknown* pUnkCP, IUnknown* pUnk, const IID& iid, LPDWORD pdw)
{
    IConnectionPointContainer* pCPC = NULL;
    IConnectionPoint* pCP = NULL;
    HRESULT hRes = pUnkCP->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
    if(SUCCEEDED(hRes) && pCPC != NULL)
    {
        hRes = pCPC->FindConnectionPoint(iid, &pCP);
        if(SUCCEEDED(hRes) && pCP != NULL)
        {
            hRes = pCP->Advise(pUnk, pdw);
            pCP->Release();
        }
 
        pCPC->Release();
    }
 
    return hRes;
}
 
inline 
HRESULT _CoUnadvise(IUnknown* pUnkCP, const IID& iid, DWORD dw)
{
    IConnectionPointContainer* pCPC = NULL;
    IConnectionPoint* pCP = NULL;
    HRESULT hRes = pUnkCP->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
    if(SUCCEEDED(hRes) && pCPC != NULL)
    {
        hRes = pCPC->FindConnectionPoint(iid, &pCP);
        if(SUCCEEDED(hRes) && pCP != NULL)
        {
            hRes = pCP->Unadvise(dw);
            pCP->Release();
        }
 
        pCPC->Release();
    }
 
    return hRes;
}
 
class CWebUIController : public DWebBrowserEvents2, public IDocHostUIHandler
{
    ULONG m_uRefCount;
 
    IWebBrowser2* m_pWebBrowser2;
    DWORD m_dwCookie;
 
    BOOL m_bEnable3DBorder;
    BOOL m_bEnableScrollBar;
 
public:
    CWebUIController() : m_uRefCount(0), m_pWebBrowser2(NULL), m_dwCookie(0)
    {
        m_bEnable3DBorder = TRUE;
        m_bEnableScrollBar = TRUE;
    }
 
    virtual ~CWebUIController()
    {
    }
 
protected:
    // IUnknown Methods
    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject)
    {
        *ppvObject = NULL;
 
        if(IsEqualGUID(riid, DIID_DWebBrowserEvents2) ||
            IsEqualGUID(riid, IID_IDispatch))
        {
            *ppvObject = (DWebBrowserEvents2*)this;
            AddRef();
            return S_OK;
        }
        else if(IsEqualGUID(riid, IID_IDocHostUIHandler) ||
            IsEqualGUID(riid, IID_IUnknown))
        {
            *ppvObject = (IDocHostUIHandler*)this;
            AddRef();
            return S_OK;
        }
 
        return E_NOINTERFACE;
    }
 
public:
    STDMETHOD_(ULONG, AddRef)(void)
    {
        m_uRefCount++;
        return m_uRefCount;
    }
 
    STDMETHOD_(ULONG, Release)(void)
    {
        m_uRefCount--;
        ULONG uRefCount = m_uRefCount;
        if(uRefCount == 0)
            delete this;
 
        return uRefCount;
    }
 
protected:
    // IDispatch Methods
    STDMETHOD(GetTypeInfoCount)(unsigned int FAR* pctinfo)
    {
        return E_NOTIMPL;
    }
     
    STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo** pptinfo)        
    {
        return E_NOTIMPL;
    }
     
     
    STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR FAR* FAR* rgszNames, 
        unsigned int cNames, LCID lcid, DISPID FAR* rgDispId)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(Invoke)(DISPID dispidMember,REFIID riid, LCID lcid, WORD wFlags,
        DISPPARAMS* pDispParams, VARIANT* pvarResult,
        EXCEPINFO* pexcepinfo, UINT* puArgErr)
    {
        if(!pDispParams)
            return E_INVALIDARG;
         
        switch(dispidMember)
        {
            //
            // The parameters for this DISPID are as follows:
            // [0]: URL to navigate to - VT_BYREF|VT_VARIANT
            // [1]: An object that evaluates to the top-level or frame
            //      WebBrowser object corresponding to the event. 
        case DISPID_NAVIGATECOMPLETE2:
             
            //
            // The IDocHostUIHandler association must be set
            // up every time we navigate to a new page.
            //
            if(pDispParams->cArgs >= 2 && pDispParams->rgvarg[1].vt == VT_DISPATCH)
                SetCustomDoc(pDispParams->rgvarg[1].pdispVal);
            else
                return E_INVALIDARG;
             
            break;
             
        default:
            break;
        }
         
        return S_OK;
    }
     
    // IDocHostUIHandler Methods
protected:
    STDMETHOD(ShowContextMenu)(DWORD dwID, POINT FAR* ppt, IUnknown FAR* pcmdtReserved,
                              IDispatch FAR* pdispReserved)
    {
        return E_NOTIMPL;
    }
     
    STDMETHOD(GetHostInfo)(DOCHOSTUIINFO FAR* pInfo)
    {
        if(pInfo != NULL)
        {
            pInfo->dwFlags |= (m_bEnable3DBorder ? 0 : DOCHOSTUIFLAG_NO3DBORDER);
            pInfo->dwFlags |= (m_bEnableScrollBar ? 0 : DOCHOSTUIFLAG_SCROLL_NO);
        }
 
        return S_OK;
    }
     
    STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject FAR* pActiveObject,
                    IOleCommandTarget FAR* pCommandTarget,
                    IOleInPlaceFrame  FAR* pFrame,
                    IOleInPlaceUIWindow FAR* pDoc)
    {
        return E_NOTIMPL;
    }
     
    STDMETHOD(HideUI)(void)
    {
        return E_NOTIMPL;
    }
     
    STDMETHOD(UpdateUI)(void)
    {
        return E_NOTIMPL;
    }
     
    STDMETHOD(EnableModeless)(BOOL fEnable)
    {
        return E_NOTIMPL;
    }
 
    STDMETHOD(OnDocWindowActivate)(BOOL fActivate)
    {
        return E_NOTIMPL;
    }
 
    STDMETHOD(OnFrameWindowActivate)(BOOL fActivate)
    {
        return E_NOTIMPL;
    }
 
    STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow FAR* pUIWindow,
                           BOOL fRameWindow)
    {
        return E_NOTIMPL;
    }
 
    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID FAR* pguidCmdGroup,
                                   DWORD nCmdID)
    {
        return E_NOTIMPL;
    }
 
    STDMETHOD(GetOptionKeyPath)(LPOLESTR FAR* pchKey, DWORD dw)
    {
        return E_NOTIMPL;
    }
 
    STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget,
                            IDropTarget** ppDropTarget)
    {
        return E_NOTIMPL;
    }
 
    STDMETHOD(GetExternal)(IDispatch** ppDispatch)
    {
        return E_NOTIMPL;
    }
 
    STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn,
                           OLECHAR** ppchURLOut)
    {
        return E_NOTIMPL;
    }
 
    STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet)
    {
        return E_NOTIMPL;
    }
 
public:
    HRESULT SetWebBrowser(IWebBrowser2* pWebBrowser2)
    {
        // Unadvise the event sink, if there was a
        // previous reference to the WebBrowser control.
        if(m_pWebBrowser2)
        {
            _CoUnadvise(m_pWebBrowser2, DIID_DWebBrowserEvents2, m_dwCookie);
            m_dwCookie = 0;
 
            m_pWebBrowser2->Release();
        }
 
        m_pWebBrowser2 = pWebBrowser2;
        if(pWebBrowser2 == NULL)
            return S_OK;
 
        m_pWebBrowser2->AddRef();
 
        return _CoAdvise(m_pWebBrowser2, (IDispatch*)this, DIID_DWebBrowserEvents2, &m_dwCookie);
    }
 
    void Enable3DBorder(BOOL bEnable = TRUE)
    {
        m_bEnable3DBorder = bEnable;
    }
 
    void EnableScrollBar(BOOL bEnable = TRUE)
    {
        m_bEnableScrollBar = bEnable;
    }
 
private:
    void SetCustomDoc(LPDISPATCH lpDisp)
    {
        if(lpDisp == NULL)
            return;
 
        IWebBrowser2* pWebBrowser2 = NULL;
        HRESULT hr = lpDisp->QueryInterface(IID_IWebBrowser2, (void**)&pWebBrowser2);
 
        if(SUCCEEDED(hr) && pWebBrowser2)
        {
            IDispatch* pDoc = NULL;
            hr = pWebBrowser2->get_Document(&pDoc);
         
            if(SUCCEEDED(hr) && pDoc)
            {
                ICustomDoc* pCustDoc = NULL;
                hr = pDoc->QueryInterface(IID_ICustomDoc, (void**)&pCustDoc);
                if(SUCCEEDED(hr) && pCustDoc != NULL)
                {
                       pCustDoc->SetUIHandler(this);
                       pCustDoc->Release();
                }
 
                pDoc->Release();
            }
 
            pWebBrowser2->Release();
        }
    }
};
 
#endif // __WEBUICONTROLLER_H__



