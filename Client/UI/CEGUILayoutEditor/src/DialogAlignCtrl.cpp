#include "pch.h"

#include "Config.h"
#include "CELayoutEditor.h"
#include "DialogAlignCtrl.h"
#include "SelectionMover.h"
BEGIN_EVENT_TABLE(DialogAlignCtrl, wxDialog)

EVT_BUTTON            (ID_CONFIRM_BUTTON, DialogAlignCtrl::OnConfirm)
EVT_BUTTON            (ID_CANCEL_BUTTON,  DialogAlignCtrl::OnCancel)
EVT_TEXT              (ID_TXT_INPUT, DialogAlignCtrl::OnTextChanged)   
EVT_RADIOBOX		  (ID_RADIOBUTTON, DialogAlignCtrl::OnSelectStyle) 

END_EVENT_TABLE()

#define BORDER            5
DialogAlignCtrl::DialogAlignCtrl(wxWindow *pParent):
m_RadioAlign(NULL),
m_TxtInput(NULL),
m_BtnConfirm(NULL),
m_BtnCancel(NULL),
m_paddingStyle(0),
m_paddingValue(0),
m_document(0),
wxDialog(pParent, wxID_ANY, wxT("AlginCtrlDialog"), wxDefaultPosition, wxSize(350, 200) )
{
    wxBoxSizer* const rootSizer = new wxBoxSizer(wxVERTICAL);
    const wxString options[] = {wxT("Hovz Padding"), wxT("Vert Padding")};
    m_RadioAlign = new wxRadioBox(this, ID_RADIOBUTTON, wxT("Set Align Style:"), wxDefaultPosition, wxDefaultSize, 2, options);
    rootSizer->Add(m_RadioAlign, 0, wxEXPAND | wxALL, 0);
    
    wxStaticBox* const typeGroup = new wxStaticBox(this, ID_RADIOBUTTON, wxT("Input the padding value(Number required!):"));
    m_TxtInput = new wxTextCtrl(this, ID_TXT_INPUT, wxT(""), wxDefaultPosition, wxDefaultSize);
    m_BtnConfirm = new wxButton(this, ID_CONFIRM_BUTTON, wxT("Confirm"));
    m_BtnCancel = new wxButton(this, ID_CANCEL_BUTTON, wxT("Cancel"));

    wxStaticBoxSizer* const typeSizer = new wxStaticBoxSizer(typeGroup, wxHORIZONTAL);
    // Use entire sizer
    (void)typeSizer->Add(m_TxtInput, 1, wxEXPAND | wxALL, BORDER);
    (void)rootSizer->Add(typeSizer, 0, wxEXPAND | wxALL, BORDER);


    wxBoxSizer* const buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    // Add the button  (may not stretch)
    (void)buttonSizer->Add(m_BtnConfirm, 0, wxALL, BORDER);
    //buttonSizer->Add(m_LoadLayout, 0, wxALL, BORDER);
    (void)buttonSizer->Add(m_BtnCancel, 0, wxALL, BORDER);
    (void)rootSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, BORDER);

    SetSizer(rootSizer);
}

DialogAlignCtrl::~DialogAlignCtrl()
{
    
}

void DialogAlignCtrl::OnConfirm(wxCommandEvent& event)
{
    if(m_document)
    {    SelectionMover(m_document->GetSelection()).AlignSpaceBetween(m_paddingValue,
        (SelectionMover::PaddingAlign)m_paddingStyle);
       m_document->Modify(true); 
    }
    EndDialog(wxID_CANCEL);
}

void DialogAlignCtrl::OnCancel(wxCommandEvent& event)
{
#if wxCHECK_VERSION(2,8,0)
    // this is no longer supported
    EndDialog(wxID_CANCEL);
#else
    wxDialog::OnCancel(event);
    Show(false);
#endif
}

void DialogAlignCtrl::OnTextChanged(wxCommandEvent& event)
{
    wxString curTxt = m_TxtInput->GetValue();
    if (!curTxt.IsNumber())
    {
        m_TxtInput->SetValue(L"");
    }
    curTxt.ToLong(&m_paddingValue);
}
void DialogAlignCtrl::OnSelectStyle(wxCommandEvent& event)
{
    int id = event.GetInt();
    if(id == 0)
        m_paddingStyle = SelectionMover::HorzPadding;
    else
        m_paddingStyle = SelectionMover::VertPadding;
}