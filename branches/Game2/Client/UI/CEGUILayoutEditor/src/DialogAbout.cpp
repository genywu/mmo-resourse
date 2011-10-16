///////////////////////////////////////////////////////////////////////////////
//  For project details and authors, refer to "CELayoutEditor.cpp".
//
//  This file is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This file is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//  To view the licence online, go to: http://www.gnu.org/copyleft/gpl.html
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "CELayoutEditor.h"
#include "Config.h"

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

#define BORDER 5

COMPILE_MINOR("Uniformize flag use : Sometimes we use wxSizerFlags() and others not")

//////////////////////////////////////////////////////////////////////////
// DIALOGABOUT FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
bool DialogAbout::Create(const DialogAboutInfo& info)
{
    // Create the "About" box
    // NOTE: We need to use the main frame as the parent here, otherwise 
    // the CELayoutEditor::OnExit() will not be called and proper cleanup
    // will not be performed when the main frame is deleted
    if(!wxDialog::Create(wxGetApp().GetMainFrame(), wxID_ANY, _("About ") + info.GetName(),
        wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER|wxDEFAULT_DIALOG_STYLE))
        return false;

    // Create the text sizer (responsible for vertically aligning the elements in the "About" box)
    m_sizerText = new wxBoxSizer(wxVERTICAL);

    // Obtain application name and version
    wxString nameAndVersion = info.GetName();
    if ( info.HasVersion() )
        nameAndVersion << wxT(' ') << info.GetVersion();

    // Create title label (application name + version)
    wxStaticText* const label = new wxStaticText(this, wxID_ANY, nameAndVersion);

    // Make sure title lable font is bigger and bold
    wxFont fontBig(*wxNORMAL_FONT);
    fontBig.SetPointSize(fontBig.GetPointSize() + 2);
    fontBig.SetWeight(wxFONTWEIGHT_BOLD);
    (void)label->SetFont(fontBig);
    
    // Add title label to sizer
    // NOTE: No need to free the custodial pointer label 
    // since it will be handled internally by wxWidgets wxSizer::Add()
    (void)m_sizerText->Add(label, wxSizerFlags().Centre().Border());
    (void)m_sizerText->AddSpacer(BORDER);

    // Add copyright and description
    // NOTE: If they are empty, they are ignored
    AddText(info.GetCopyright());
    AddText(info.GetDescription());

    // Check if there are any contacts
    if(info.HasContacts())
    {
        // Get contacts and contacts' email
        const wxArrayString contacts = info.GetContacts();
        const wxArrayString contactsEmail = info.GetContactsEmails();

        // Obtain contacts and contacts' email count
        const size_t contactsCount = contacts.size();
        const size_t contactsEmailCount = contactsEmail.size();

        // Make sure both counts are the same
        // NOTE: We're silently ignoring count mismatches
        if(contactsCount == contactsEmailCount)
        {
            // Create a framed static box for displaying the contacts and respective vertical sizer
            // NOTE: No need to free the custodial pointer contactsGroup since it will be handled 
            // internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
            wxStaticBox* const contactsGroup = new wxStaticBox(this, wxID_ANY, wxT("Contacts:"));
            wxStaticBoxSizer* const contactsSizer = new wxStaticBoxSizer(contactsGroup, wxVERTICAL);
          
            // Scan all existing contacts
            for(size_t i=0; i<contactsCount; i++)
            {
                // Create sizer to align the contact elements horizontally
                wxBoxSizer* const rowSizer = new wxBoxSizer(wxHORIZONTAL);

                // Create the contacts elements
                // NOTE: The mailto: tag is automatically added
                wxStaticText* const contact = new wxStaticText(this, wxID_ANY, contacts[i] + wxT(" - "));
                wxStaticText* const beginTag = new wxStaticText(this, wxID_ANY, wxT("<"));
                wxHyperlinkCtrl* const contactEmail = new wxHyperlinkCtrl(this, wxID_ANY, contactsEmail[i], wxT("mailto:")  + contactsEmail[i]);
                wxStaticText* const endTag = new wxStaticText(this, wxID_ANY, wxT(">"));

                // Add contact elements to horizontal sizer, making sure they don't stretch
                // NOTE: No need to free the custodial pointers contact, beginTag, contactEmail 
                // and endTag since they will be handled internally by wxWidgets wxSizer::Add()
                (void)rowSizer->Add(contact, 0, wxSTRETCH_NOT, BORDER);
                (void)rowSizer->Add(beginTag, 0, wxSTRETCH_NOT, BORDER);
                (void)rowSizer->Add(contactEmail, 0, wxSTRETCH_NOT, BORDER);
                (void)rowSizer->Add(endTag, 0, wxSTRETCH_NOT, BORDER);

                // Add horizontal sizer to vertical one (no stretching)
                // NOTE: No need to free the custodial pointer rowSizer 
                // since it will be handled internally by wxWidgets wxSizer::Add()
                (void)contactsSizer->Add(rowSizer, 0, wxSTRETCH_NOT | wxALL, BORDER);        
            }

            // Add contacts vertical sizer to the "About" box text sizer, making sure it expands to contents
            // NOTE: No need to free the custodial pointer contactsSizer 
            // since it will be handled internally by wxWidgets wxSizer::Add()
            (void)m_sizerText->Add(contactsSizer, 0, wxEXPAND | wxALL, BORDER);
        } // if
    } // if

    // Do we have a website?
    if (info.HasWebSite())
    {
        // Add website to "About" box, centered on screen
        #if wxUSE_HYPERLINKCTRL
            (void)m_sizerText->AddSpacer(BORDER);
            AddControl(new wxHyperlinkCtrl(this, wxID_ANY, info.GetWebSiteDescription(),info.GetWebSiteURL()));
        #else
            AddText(info.GetWebSiteURL());
        #endif // wxUSE_HYPERLINKCTRL/!wxUSE_HYPERLINKCTRL
    }

    // Give me some space
    (void)m_sizerText->AddSpacer(BORDER);
    
    // Only add the collapsible panes if supported
    #if wxUSE_COLLPANE

        // Add application license (if exists), with text left-aligned
        if ( info.HasLicence() )
            AddCollapsiblePane(wxT("License"), info.GetLicence(), wxALIGN_LEFT);

        // Add application 'Built With' (if exists), with text left-aligned
        if ( info.HasBuiltWith() )
            AddCollapsiblePane(wxT("Built With"), info.GetBuiltWith(), wxALIGN_LEFT);

        // Add developers (if exist), with each developer left-aligned one per line
        if ( info.HasDevelopers() )
            AddCollapsiblePane(wxT("Developers"), info.GetDevelopers(), info.GetDevelopersEmails(), info.GetDevelopersNicks(), info.GetDevelopersDescs(), wxALIGN_LEFT);
            
        // Add doc writers (if exist), with each doc writer left-aligned one per line
        if ( info.HasDocWriters() )
            AddCollapsiblePane(wxT("Documentation writers"), info.GetDocWriters(), info.GetDocWritersEmails(), info.GetDocWritersNicks(), info.GetDocWritersDescs(), wxALIGN_LEFT);

        // Add artists (if exist), with each doc writer left-aligned one per line
        if ( info.HasArtists() )
            AddCollapsiblePane(wxT("Artists"), info.GetArtists(), info.GetArtistsEmails(), info.GetArtistsNicks(), info.GetArtistsDescs(), wxALIGN_LEFT);

        // Add translators (if exist), with each doc writer left-aligned one per line
        if ( info.HasTranslators() )
            AddCollapsiblePane(wxT("Translators"), info.GetTranslators(), info.GetTranslatorsEmails(), info.GetTranslatorsNicks(), info.GetTranslatorsDescs(), wxALIGN_LEFT);

        // Check if hyper links are supported and exist
        #if wxUSE_HYPERLINKCTRL
        if(info.HasHyperLinks())
        {
            // Get hyper links and descriptions
            const wxArrayString hyperLinks = info.GetHyperLinks();
            const wxArrayString hyperLinksDescs = info.GetHyperLinksDescs();

            // Obtain hyper links and description sizes
            const size_t hyperLinksCount = hyperLinks.size();
            const size_t hyperLinksDescsCount = hyperLinksDescs.size();

            // Make sure both counts are the same
            // NOTE: We're silently ignoring count mismatches
            if(hyperLinksCount == hyperLinksDescsCount)
            {
                // Create collapsible pane and vertical sizer
                wxBoxSizer* const sizerHyper = new wxBoxSizer(wxVERTICAL);
                wxCollapsiblePane* const pane = new wxCollapsiblePane(this, wxID_ANY, wxT("Links"));

                // Scan all existing hyper links
                for(size_t i=0; i<hyperLinks.size(); i++)
                {
                    // Create hyper link element and add it to the vertical sizer
                    // NOTE: No need to free the custodial pointer hyperLink 
                    // since it will be handled internally by wxWidgets wxSizer::Add()
                    wxHyperlinkCtrl* const hyperLink = new wxHyperlinkCtrl(pane->GetPane(), wxID_ANY, hyperLinksDescs[i], hyperLinks[i]);
                    (void)sizerHyper->Add(hyperLink, wxSizerFlags().Left());
                }

                // Add vertical sizer to collapsible pane
                // NOTE: No need to free the custodial pointer sizerHyper 
                // since it will be handled internally by wxWidgets wxWindowBase::SetSizer()
                pane->GetPane()->SetSizer(sizerHyper);

                // Add pane to the "About" box text sizer, making sure it expands to contents
                // NOTE: all the wxCollapsiblePanes must be added with a null proportion value
                // NOTE2: No need to free the custodial pointer pane 
                // since it will be handled internally by wxWidgets wxSizer::Add()
                (void)m_sizerText->Add(pane,wxSizerFlags(0).Expand().Border(wxBOTTOM));
            }
        } // if
        #endif // wxUSE_HYPERLINKCTRL

        // Add acknowledgments (if exist), with each acknowledgment left-aligned one per line
        if ( info.HasAcknowledgments() )
            AddCollapsiblePane(wxT("Acknowledgments"), info.GetAcknowledgments(), info.GetAcknowledgmentsEmails(), info.GetAcknowledgmentsNicks(), info.GetAcknowledgmentsDescs(), wxALIGN_LEFT);
    #endif // wxUSE_COLLPANE

    // Do we have a footnote?
    if (info.HasFootnote())
    {
        // Give me a little space
        (void)m_sizerText->AddSpacer(BORDER);

        // Create the footnote, centered, encapsulated between two horizontal static lines
        // NOTE: Footnote appears smaller than normal text
        AddControl(new wxStaticLine(this), wxSizerFlags().Expand());
        wxStaticText* const footnote = new wxStaticText(this, wxID_ANY, info.GetFootnote());
        wxFont fontSmall(*wxNORMAL_FONT);
        fontSmall.SetPointSize(fontSmall.GetPointSize() - 1);
        fontSmall.SetStyle(wxFONTSTYLE_ITALIC);
        (void)footnote->SetFont(fontSmall);

        // Add footnote to "About" box text sizer
        // NOTE: No need to free the custodial pointer footnote 
        // since it will be handled internally by wxWidgets wxSizer::Add()
        (void)m_sizerText->Add(footnote, wxSizerFlags().Centre());
        AddControl(new wxStaticLine(this), wxSizerFlags().Expand());
    }
    
    // Create sizer for the icon
    wxSizer* const sizerIconAndText = new wxBoxSizer(wxHORIZONTAL);
    
    // Add icon to the previous sizer (if supported) 
    #if wxUSE_STATBMP
        const wxIcon icon = info.GetIcon();
        if ( icon.Ok() )
            (void)sizerIconAndText->Add(new wxStaticBitmap(this, wxID_ANY, icon), wxSizerFlags().Border(wxRIGHT));        
    #endif // wxUSE_STATBMP

    // Add "About" box text sizer to icon sizer
    (void)sizerIconAndText->Add(m_sizerText, wxSizerFlags(1).Expand());

    // Create the sizer top, which will handle all elements in the "About" box (text + icon + buttons)
    // NOTE: No need to free the custodial pointer sizerIconAndText 
    // since it will be handled internally by wxWidgets wxSizer::Add()
    wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);
    (void)sizerTop->Add(sizerIconAndText, wxSizerFlags(1).Expand().Border());

    // Add button sizer to sizer top
    wxSizer* const sizerBtns = CreateButtonSizer(wxOK);
    if ( sizerBtns )
    {
        // NOTE: No need to free the custodial pointer sizerBtns 
        // since it will be handled internally by wxWidgets wxSizer::Add()
        (void)sizerTop->Add(sizerBtns, wxSizerFlags().Expand().Border());
    }

    // Set sizer top as the sizer to use
    // NOTE: No need to free the custodial pointer sizerTop 
    // since it will be handled internally by wxWidgets wxWindowBase::SetSizerAndFit()
    SetSizerAndFit(sizerTop);

    // Centre "About" box on screen
    CentreOnScreen();

    // We're done and everything worked like a charm
    return true;
}

//------------------------------------------------------------------------
void DialogAbout::AddControl(wxWindow *win, const wxSizerFlags& flags)
{
    // Validations
    wxCHECK_RET( m_sizerText, wxT("Can only be called after Create()") );
    wxASSERT_MSG( win, wxT("Can't add NULL window to about dialog") );

    // Add control to the text sizer (responsible for vertically aligning the 
    // elements in the "About" box) using the specified flags
    (void)m_sizerText->Add(win, flags);
}

//------------------------------------------------------------------------
void DialogAbout::AddText(const wxString& text, const wxSizerFlags& flags)
{
    // Add text to "About" box (if not empty)
    if ( !text.empty() )
        AddControl(new wxStaticText(this, wxID_ANY, text), flags);
}

//------------------------------------------------------------------------
void DialogAbout::AddCollapsiblePane(const wxString& title, const wxString& text, long style)
{
    // Validations
    wxCHECK_RET( m_sizerText, wxT("Can only be called after Create()") );

    // Create collapsible pane and the text control (using the specified style)
    wxCollapsiblePane* const pane = new wxCollapsiblePane(this, wxID_ANY, title);
    wxStaticText* const txt = new wxStaticText(pane->GetPane(), wxID_ANY, text, wxDefaultPosition, wxDefaultSize, style);

    // Don't make the text unreasonably wide
    static const int maxWidth = wxGetDisplaySize().x/3;
    txt->Wrap(maxWidth);

    // Add the pane to the text sizer (responsible for vertically aligning the elements in the "About" box)
    // NOTE: all the wxCollapsiblePanes must be added with a null proportion value
    // NOTE2: No need to free the custodial pointer pane 
    // since it will be handled internally by wxWidgets wxSizer::Add()
    (void)m_sizerText->Add(pane, wxSizerFlags(0).Expand().Border(wxBOTTOM));
}

//------------------------------------------------------------------------
void DialogAbout::AddCollapsiblePane(const wxString& title, const wxArrayString& names, const wxArrayString& emails, const wxArrayString& nicks, const wxArrayString& descs, long style)
{
    // Validations
    wxCHECK_RET( m_sizerText, wxT("Can only be called after Create()") );

    // Obtain number of names, emails and nicks
    const size_t namesCount = names.size();
    const size_t emailsCount = emails.size();
    const size_t nicksCount = nicks.size();

    // Make sure all counts are the same
    // NOTE: We're silently ignoring count mismatches
    if( (namesCount == emailsCount) && (namesCount == nicksCount) )
    {
        // Create collapsible pane 
        wxCollapsiblePane* const pane = new wxCollapsiblePane(this, wxID_ANY, title);

        // Create sizer to align the various elements vertically
        wxBoxSizer* const verticalSizer = new wxBoxSizer(wxVERTICAL);

        // Scan all existing names
        for(size_t i=0; i<namesCount; i++)
        {
            // Create sizer to align the various elements horizontally
            wxBoxSizer* const horizontalSizer = new wxBoxSizer(wxHORIZONTAL);

            // Create name element - non stretching and border.
            // NOTE: No need to free the custodial pointer name 
            // since it will be handled internally by wxWidgets wxSizer::Add()
            wxStaticText* const name = new wxStaticText(pane->GetPane(), wxID_ANY, names[i], wxDefaultPosition, wxDefaultSize, style);
            (void)horizontalSizer->Add(name, 0, wxSTRETCH_NOT, BORDER);

            // Add nick element (if it exists) - non stretching and border.
            if(nicks[i] && nicks[i].Length() > 0)
            {
                wxStaticText* const nick = new wxStaticText(pane->GetPane(), wxID_ANY, wxT(" (") + nicks[i] + wxT(")"), wxDefaultPosition, wxDefaultSize, style);

                // NOTE: No need to free the custodial pointer nick 
                // since it will be handled internally by wxWidgets wxSizer::Add()
                (void)horizontalSizer->Add(nick, 0, wxSTRETCH_NOT, BORDER);
            }

            // Add email element (if it exists) - non stretching and border.
            // NOTE: The mailto: tag is automatically added
            if(emails[i] && emails[i].Length() > 0)
            {
                wxStaticText* const beginTag = new wxStaticText(pane->GetPane(), wxID_ANY, wxT(" - <"), wxDefaultPosition, wxDefaultSize, style);
                wxHyperlinkCtrl* const email = new wxHyperlinkCtrl(pane->GetPane(), wxID_ANY, emails[i], wxT("mailto:") + emails[i]);
                wxStaticText* const endTag = new wxStaticText(pane->GetPane(), wxID_ANY, wxT(">"), wxDefaultPosition, wxDefaultSize, style);

                // NOTE: No need to free the custodial pointers beginTag, email and
                // endTag since they will be handled internally by wxWidgets wxSizer::Add()
                (void)horizontalSizer->Add(beginTag, 0, wxSTRETCH_NOT, BORDER);
                (void)horizontalSizer->Add(email, 0, wxSTRETCH_NOT, BORDER);
                (void)horizontalSizer->Add(endTag, 0, wxSTRETCH_NOT, BORDER);
            }

            // Add horizontal sizer to vertical one  - no stretching and no border
            // NOTE: No need to free the custodial pointer horizontalSizer 
            // since it will be handled internally by wxWidgets wxSizer::Add()
            (void)verticalSizer->Add(horizontalSizer, 0, wxSTRETCH_NOT | wxALL); 

            // Add description element in a new line (if it exists) - No stretching but border.
            // NOTE: Text is dark grey
            if(descs[i] && descs[i].Length() > 0)
            {
                wxStaticText* const desc = new wxStaticText(pane->GetPane(), wxID_ANY, wxT(" - ") + descs[i], wxDefaultPosition, wxDefaultSize, style);
                (void)desc->SetForegroundColour(wxColour(75,75,75));
                // NOTE: No need to free the custodial pointer desc 
                // since it will be handled internally by wxWidgets wxSizer::Add()
                (void)verticalSizer->Add(desc, 1, wxSTRETCH_NOT | wxALL, BORDER); 
            }
        }
        
        // Add vertical sizer to collapsible pane
        // NOTE2: No need to free the custodial pointer verticalSizer 
        // since it will be handled internally by wxWidgets wxWindowBase::SetSizer()
        pane->GetPane()->SetSizer(verticalSizer);

        // Add the pane to the text sizer (responsible for vertically aligning the elements in the "About" box)
        // NOTE: all the wxCollapsiblePanes must be added with a null proportion value
        // NOTE2: No need to free the custodial pointer pane 
        // since it will be handled internally by wxWidgets wxSizer::Add()
        (void)m_sizerText->Add(pane, wxSizerFlags(0).Expand().Border(wxBOTTOM)); 
    }
}
