//功能：对选择多个组件设置行，列间距的对话框
//#include "../../../wxWidgets/include/wx/dialog.h"
//2010.9.13 nikai
#pragma once
#include "EditorDocument.h"
class DialogAlignCtrl:public wxDialog
{
public:
    DialogAlignCtrl(wxWindow *pParent);
    ~DialogAlignCtrl();

private:
    enum 
    {
        ID_RADIOBUTTON,
        ID_TXT_INPUT,
        ID_CONFIRM_BUTTON,
        ID_CANCEL_BUTTON
    };
    wxRadioBox*		m_RadioAlign;
    wxTextCtrl*     m_TxtInput;
    wxButton*		m_BtnConfirm;
    wxButton*		m_BtnCancel;
    DECLARE_EVENT_TABLE();
    void OnConfirm(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnTextChanged(wxCommandEvent& event);
    void OnSelectStyle(wxCommandEvent& event);
    //对齐方式
    long            m_paddingStyle;
    //间距值
    long            m_paddingValue;
    EditorDocument *m_document;
public:
    void SetDocument (EditorDocument* document) {m_document = document;}
};