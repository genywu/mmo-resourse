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

#ifndef _DIALOG_ABOUT_H_
#define _DIALOG_ABOUT_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////
#include <wx/sizer.h>

#include "wx/statline.h"
#include "wx/generic/aboutdlgg.h"
#include "wx/aboutdlg.h"

#if wxUSE_HYPERLINKCTRL
    #include "wx/hyperlink.h"
#endif // wxUSE_HYPERLINKCTRL

#if wxUSE_COLLPANE
    #include "wx/collpane.h"
#endif // wxUSE_COLLPANE

//////////////////////////////////////////////////////////////////////////
// DIALOGABOUTINFO
//////////////////////////////////////////////////////////////////////////

/**  DialogAboutInfo - Information shown by our custom "About" Dialog.
 */
class DialogAboutInfo : public wxAboutDialogInfo
{
    public:

        /************************************************************************
         * GETTERS/SETTERS
         ************************************************************************/
        
        /** Sets the footnote text (which is added at the bottom of the about dialog 
         * between static lines).
         * @access public 
         * @qualifier none
         * @param footnote Footnote text
         * @return void
         * @remarks Free format
         */
        void SetFootnote(const wxString& footnote) { m_footnote = footnote; }

        /** Verifies if the user has added a footnote text.
         * @access public 
         * @qualifier const
         * @return bool True if footnote exists, false otherwise
         * @remarks Free format
         */
        bool HasFootnote() const { return !m_footnote.empty(); }

        /** Returns the footnote text.
         * @access public 
         * @qualifier const
         * @return wxString Footnote text (can be empty)
         * @remarks Free format
         */
        wxString GetFootnote() const { return m_footnote; }

        //----------------------------------------------------------------------------

        /** Applies the 'built with' text, which indicates the used third party applications.
         * @access public 
         * @qualifier none
         * @param builtWith Built with text
         * @return void
         * @remarks Free format
         */
        void SetBuiltWith(const wxString& builtWith) { m_builtWith = builtWith; }

        /** Verifies if the user added 'built with' text.
         * @access public 
         * @qualifier const
         * @return bool True if 'built with' exists, false otherwise
         * @remarks Free format
         */
        bool HasBuiltWith() const { return !m_builtWith.empty(); }

        /** Returns the 'built with' text.
         * @access public 
         * @qualifier const
         * @return wxString 'Built with' text
         * @remarks Free format
         */
        wxString GetBuiltWith() const { return m_builtWith; }

        //----------------------------------------------------------------------------

        /** Assigns the supplied array strings as the current about box hyper links and descriptions.
         * @access public 
         * @qualifier none
         * @param hyperLinks Array String containing the various links
         * @param hyperLinksDescs Array String containing the various links descriptions
         * @return void
         * @remarks No format verification is made on the hyper links. It is up to the user
         * to make sure that the input is correct (e.g. http://).
         * @remarks These hyper links (encapsulated inside a collapsible pane) are in addition to 
         * the URL field which is the main hyper link that is displayed in the about box.
         * @remarks If the hyper links descriptions are empty, the hyper links itself will be used
         * as description.
         * @remarks This function doesn't verify a mismatch between hyper links and their descriptions
         */
        void SetHyperLinks(const wxArrayString& hyperLinks, const wxArrayString& hyperLinksDescs) 
        { 
            m_hyperLinks = hyperLinks; 
            m_hyperLinksDescs = hyperLinksDescs.empty() ? hyperLinks : hyperLinksDescs;
        }
                
        /** Adds the supplied hyper link (and description) to the current list of hyper links.
         * @access public 
         * @qualifier none
         * @param hyperLink Hyper link to add
         * @param hyperLinkDesc Hyper link description
         * @return void
         * @remarks No format verification is made on the hyper link. It is up to the user
         * to make sure that the input is correct (e.g. http://).
         * @remarks These hyper links (encapsulated inside a collapsible pane) are in addition to 
         * the URL field which is the main hyper link that is displayed in the about box.
         * @remarks If the hyper link description is empty, the hyper link itself will be used
         * as description.
         */
        void AddHyperLink(const wxString& hyperLink, const wxString& hyperLinkDesc) 
        { 
            m_hyperLinks.push_back(hyperLink);
            m_hyperLinksDescs.push_back(hyperLinkDesc.empty() ? hyperLink : hyperLinkDesc);
        }

        /** Verifies if the user has supplied any hyper link.
         * @access public 
         * @qualifier const
         * @return bool True if hyper links exist, false otherwise
         * @remarks These hyper links (encapsulated inside a collapsible pane) are in addition to 
         * the URL field which is the main hyper link that is displayed in the about box.
         */
        bool HasHyperLinks() const { return !m_hyperLinks.empty(); }

        /** Returns the hyper links the user added.
         * @access public 
         * @qualifier const
         * @return const wxArrayString& Current hyper links
         * @remarks These hyper links (encapsulated inside a collapsible pane) are in addition to 
         * the URL field which is the main hyper link that is displayed in the about box.
         */
        const wxArrayString& GetHyperLinks() const { return m_hyperLinks; }

        /** Returns the hyper links descriptions the user added.
         * @access public 
         * @qualifier const
         * @return const wxArrayString& Current hyper links descriptions
         * @remarks These hyper links (encapsulated inside a collapsible pane) are in addition to 
         * the URL field which is the main hyper link that is displayed in the about box.
         */
        const wxArrayString& GetHyperLinksDescs() const { return m_hyperLinksDescs; }

        //----------------------------------------------------------------------------

        /** Assigns the supplied array strings as the current about box acknowledgments, 
         * acknowledgments' emails, acknowledgments' nicks and acknowledgments' descriptions.
         * @access public 
         * @qualifier none
         * @param acknowledgments Array String containing the various acknowledgments
         * @param acknowledgmentsEmails Array String containing the various acknowledgments' emails
         * @param acknowledgmentsNicks Array String containing the various acknowledgments' nicks
         * @param acknowledgmentsDescs Array String containing the various acknowledgments' descriptions
         * @return void
         * @remarks No format verification is made on the acknowledgments' email. It is up to the user
         * to make sure that the input is correct (e.g. aa@bb.com).
         * @remarks The mailto: tag should not be specified in the acknowledgments' email.
         * @remarks This function doesn't verify a mismatch between acknowledgments and their emails 
         * and nicks.
         */
        void SetAcknowledgments(const wxArrayString& acknowledgments, const wxArrayString& acknowledgmentsEmails, const wxArrayString& acknowledgmentsNicks, const wxArrayString& acknowledgmentsDescs) 
        { 
            m_acknowledgments = acknowledgments; 
            m_acknowledgmentsEmails = acknowledgmentsEmails;
            m_acknowledgmentsNicks = acknowledgmentsNicks;
            m_acknowledgmentsDescs = acknowledgmentsDescs;
        }

        /** Adds the supplied acknowledgments (and email, nick and description) to the current list of 
         * acknowledgments.
         * @access public 
         * @qualifier none
         * @param acknowledgment Acknowledgment to add
         * @param acknowledgmentEmail Acknowledgment's email
         * @param acknowledgmentNick Acknowledgment's nick
         * @param acknowledgmentDesc Acknowledgment's description
         * @return void
         * @remarks No format verification is made on the acknowledgments's email. It is up to the user
         * to make sure that the input is correct (e.g. aa@bb.com).
         * @remarks The mailto: tag should not be specified in the acknowledgments's email.
         */
        void AddAcknowledgment(const wxString& acknowledgment, const wxString& acknowledgmentEmail, const wxString& acknowledgmentNick, const wxString& acknowledgmentDesc) 
        { 
            m_acknowledgments.push_back(acknowledgment);
            m_acknowledgmentsEmails.push_back(acknowledgmentEmail);
            m_acknowledgmentsNicks.push_back(acknowledgmentNick);
            m_acknowledgmentsDescs.push_back(acknowledgmentDesc);
        }

        /** Verifies if the user has supplied any acknowledgment.
         * @access public 
         * @qualifier const
         * @return bool True if contacts exist, false otherwise
         */
        bool HasAcknowledgments() const { return !m_acknowledgments.empty(); }

        /** Returns the acknowledgments the user added.
         * @access public 
         * @qualifier const
         * @return const wxArrayString& Current acknowledgments
         */
        const wxArrayString& GetAcknowledgments() const { return m_acknowledgments; }

        /** Returns the acknowledgments' emails the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current acknowledgments' emails
         */
        const wxArrayString& GetAcknowledgmentsEmails() const { return m_acknowledgmentsEmails; }

        /** Returns the acknowledgments' nicks the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current acknowledgments' nicks
         */
        const wxArrayString& GetAcknowledgmentsNicks() const { return m_acknowledgmentsNicks; }

        /** Returns the acknowledgments' descriptions the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current acknowledgments' descriptions
         */
        const wxArrayString& GetAcknowledgmentsDescs() const { return m_acknowledgmentsDescs; }

        //----------------------------------------------------------------------------
        
        /** Assigns the supplied array strings as the current about box contacts, 
         * contacts' emails and contacts' nicks.
         * @access public 
         * @qualifier none
         * @param contacts Array String containing the various contacts
         * @param contactsEmails Array String containing the various contacts' emails
         * @param contactsNicks Array String containing the various contacts' nicks
         * @param contactsDescs Array String containing the various contacts' descriptions
         * @return void
         * @remarks No format verification is made on the contacts' email. It is up to the user
         * to make sure that the input is correct (e.g. aa@bb.com).
         * @remarks The mailto: tag should not be specified in the contacts' email.
         * @remarks This function doesn't verify a mismatch between contacts and their emails 
         * and nicks.
         */
        void SetContacts(const wxArrayString& contacts, const wxArrayString& contactsEmails, const wxArrayString& contactsNicks, const wxArrayString& contactsDescs) 
        { 
            m_contacts = contacts; 
            m_contactsEmails = contactsEmails;
            m_contactsNicks = contactsNicks;
            m_contactsDescs = contactsDescs;
        }

        /** Adds the supplied contact (and email, nick and description) to the current 
         * list of contacts.
         * @access public 
         * @qualifier none
         * @param contact Contact to add
         * @param contactEmail Contact's email
         * @param contactNick Contact's nick
         * @param contactDesc Contact's description
         * @return void
         * @remarks No format verification is made on the contact's email. It is up to the user
         * to make sure that the input is correct (e.g. aa@bb.com).
         * @remarks The mailto: tag should not be specified in the contact's email.
         */
        void AddContact(const wxString& contact, const wxString& contactEmail, const wxString& contactNick, const wxString& contactDesc) 
        { 
            m_contacts.push_back(contact);
            m_contactsEmails.push_back(contactEmail);
            m_contactsNicks.push_back(contactNick);
            m_contactsDescs.push_back(contactDesc);
        }

        /** Verifies if the user has supplied any contact.
         * @access public 
         * @qualifier const
         * @return bool True if contacts exist, false otherwise
         */
        bool HasContacts() const { return !m_contacts.empty(); }

        /** Returns the contacts the user added.
         * @access public 
         * @qualifier const
         * @return const wxArrayString& Current contacts
         */
        const wxArrayString& GetContacts() const { return m_contacts; }

        /** Returns the contacts' emails the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current contacts' emails
         */
        const wxArrayString& GetContactsEmails() const { return m_contactsEmails; }

        /** Returns the contacts' nicks the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current contacts' nicks
         */
        const wxArrayString& GetContactsNicks() const { return m_contactsNicks; }

        /** Returns the contacts' descriptions the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current contacts' descriptions
         */
        const wxArrayString& GetContactsDescs() const { return m_contactsDescs; }

        //----------------------------------------------------------------------------

        /** Assigns the supplied array strings as the current about box developers, 
         * developers' emails, developers' nicks and developer's descriptions.
         * @access public 
         * @qualifier none
         * @param developers Array String containing the various developers
         * @param developersEmails Array String containing the various developers' emails
         * @param developersNicks Array String containing the various developers' nicks
         * @param developersDescs Array String containing the various developers' descriptions
         * @return void
         * @remarks No format verification is made on the developers' email. It is up to the user
         * to make sure that the input is correct (e.g. aa@bb.com).
         * @remarks The mailto: tag should not be specified in the developers' email.
         * @remarks This function doesn't verify a mismatch between developers and their emails 
         * and nicks.
         */
        void SetDevelopers(const wxArrayString& developers, const wxArrayString& developersEmails, const wxArrayString& developersNicks, const wxArrayString& developersDescs) 
        { 
            wxAboutDialogInfo::SetDevelopers(developers);
            m_developersEmails = developersEmails;
            m_developersNicks = developersNicks;
            m_developersDescs = developersDescs;
        }

        /** Adds the supplied developer (and email, nick and description) to the current list of 
         * developers.
         * @access public 
         * @qualifier none
         * @param developer Developer to add
         * @param developerEmail Developer's email
         * @param developerNick Developer's nick
         * @param developerDesc Developer's description
         * @return void
         * @remarks No format verification is made on the developer's email. It is up to the user
         * to make sure that the input is correct (e.g. aa@bb.com).
         * @remarks The mailto: tag should not be specified in the developer's email.
         */
        void AddDeveloper(const wxString& developer, const wxString& developerEmail, const wxString& developerNick, const wxString& developerDesc) 
        { 
            wxAboutDialogInfo::AddDeveloper(developer);
            m_developersEmails.push_back(developerEmail);
            m_developersNicks.push_back(developerNick);
            m_developersDescs.push_back(developerDesc);
        }

        /** Returns the developers' emails the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current developers' emails
         */
        const wxArrayString& GetDevelopersEmails() const { return m_developersEmails; }

        /** Returns the developers' nicks the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current developers' nicks
         */
        const wxArrayString& GetDevelopersNicks() const { return m_developersNicks; }

        /** Returns the developers' descriptions the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current developers' descriptions
         */
        const wxArrayString& GetDevelopersDescs() const { return m_developersDescs; }

        //----------------------------------------------------------------------------

        /** Assigns the supplied array strings as the current about box artists, 
         * artists' emails, artists' nicks and artists' descriptions.
         * @access public 
         * @qualifier none
         * @param artists Array String containing the various artists
         * @param artistsEmails Array String containing the various artists' emails
         * @param artistsNicks Array String containing the various artists' nicks
         * @param artistsDescs Array String containing the various artists' descriptions
         * @return void
         * @remarks No format verification is made on the artists' email. It is up to the user
         * to make sure that the input is correct (e.g. aa@bb.com).
         * @remarks The mailto: tag should not be specified in the artists' email.
         * @remarks This function doesn't verify a mismatch between artists and their emails 
         * and nicks.
         */
        void SetArtists(const wxArrayString& artists, const wxArrayString& artistsEmails, const wxArrayString& artistsNicks, const wxArrayString& artistsDescs) 
        { 
            wxAboutDialogInfo::SetArtists(artists);
            m_artistsEmails = artistsEmails;
            m_artistsNicks = artistsNicks;
            m_artistsDescs = artistsDescs;
        }

        /** Adds the supplied artist (and email, nick and description) to the current list of 
         * artists.
         * @access public 
         * @qualifier none
         * @param artist Artist to add
         * @param artistEmail Artist's email
         * @param artistNick Artist's nick
         * @param artistDesc Artist's description
         * @return void
         * @remarks No format verification is made on the artist's email. It is up to the user
         * to make sure that the input is correct (e.g. aa@bb.com).
         * @remarks The mailto: tag should not be specified in the artist's email.
         */
        void AddArtist(const wxString& artist, const wxString& artistEmail, const wxString& artistNick, const wxString& artistDesc) 
        { 
            wxAboutDialogInfo::AddArtist(artist);
            m_artistsEmails.push_back(artistEmail);
            m_artistsNicks.push_back(artistNick);
            m_artistsDescs.push_back(artistDesc);
        }

        /** Returns the artists' emails the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current artists' emails
         */
        const wxArrayString& GetArtistsEmails() const { return m_artistsEmails; }

        /** Returns the artists' nicks the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current artists' nicks
         */
        const wxArrayString& GetArtistsNicks() const { return m_artistsNicks; }

        /** Returns the artists' descriptions the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current artists' descriptions
         */
        const wxArrayString& GetArtistsDescs() const { return m_artistsDescs; }

        //----------------------------------------------------------------------------

        /** Assigns the supplied array strings as the current about box translators, 
         * translators' emails, translators' nicks and translators' descriptions.
         * @access public 
         * @qualifier none
         * @param translators Array String containing the various translators
         * @param translatorsEmails Array String containing the various translators' emails
         * @param translatorsNicks Array String containing the various translators' nicks
         * @param translatorsDescs Array String containing the various translators' descriptions
         * @return void
         * @remarks No format verification is made on the translators' email. It is up to the user
         * to make sure that the input is correct (e.g. aa@bb.com).
         * @remarks The mailto: tag should not be specified in the translators' email.
         * @remarks This function doesn't verify a mismatch between translators and their emails 
         * and nicks.
         */
        void SetTranslators(const wxArrayString& translators, const wxArrayString& translatorsEmails, const wxArrayString& translatorsNicks, const wxArrayString& translatorsDescs) 
        { 
            wxAboutDialogInfo::SetTranslators(translators);
            m_translatorsEmails = translatorsEmails;
            m_translatorsNicks = translatorsNicks;
            m_translatorsDescs = translatorsDescs;
        }

        /** Adds the supplied translator (and email, nick and description) to the current list of 
         * translators.
         * @access public 
         * @qualifier none
         * @param translator Translator to add
         * @param translatorEmail Translator's email
         * @param translatorNick Translator's nick
         * @param translatorDesc Translator's description
         * @return void
         * @remarks No format verification is made on the translator's email. It is up to the user
         * to make sure that the input is correct (e.g. aa@bb.com).
         * @remarks The mailto: tag should not be specified in the translator's email.
         */
        void AddTranslator(const wxString& translator, const wxString& translatorEmail, const wxString& translatorNick, const wxString& translatorDesc) 
        { 
            wxAboutDialogInfo::AddTranslator(translator);
            m_translatorsEmails.push_back(translatorEmail);
            m_translatorsNicks.push_back(translatorNick);
            m_translatorsDescs.push_back(translatorDesc);
        }

        /** Returns the translators' emails the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current translators' emails
         */
        const wxArrayString& GetTranslatorsEmails() const { return m_translatorsEmails; }

        /** Returns the translators' nicks the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current translators' nicks
         */
        const wxArrayString& GetTranslatorsNicks() const { return m_translatorsNicks; }

        /** Returns the translators' descriptions the user added.
        * @access public
        * @qualifier const
        * @return const wxArrayString& Current translators' descriptions
        */
        const wxArrayString& GetTranslatorsDescs() const { return m_translatorsDescs; }

        //----------------------------------------------------------------------------

        /** Assigns the supplied array strings as the current about box doc writers, 
         * doc writers' emails, doc writers' nicks and doc writers' descriptions.
         * @access public 
         * @qualifier none
         * @param docWriters Array String containing the various doc writers
         * @param docWritersEmails Array String containing the various doc writers' emails
         * @param docWritersNicks Array String containing the various doc writers' nicks
         * @param docWritersDescs Array String containing the various doc writers' descriptions
         * @return void
         * @remarks No format verification is made on the doc writers' email. It is up to the user
         * to make sure that the input is correct (e.g. aa@bb.com).
         * @remarks The mailto: tag should not be specified in the doc writers' email.
         * @remarks This function doesn't verify a mismatch between doc writers and their emails 
         * and nicks.
         */
        void SetDocWriters(const wxArrayString& docWriters, const wxArrayString& docWritersEmails, const wxArrayString& docWritersNicks, const wxArrayString& docWritersDescs) 
        { 
            wxAboutDialogInfo::SetDocWriters(docWriters);
            m_docWritersEmails = docWritersEmails;
            m_docWritersNicks = docWritersNicks;
            m_docWritersDescs = docWritersDescs;
        }

        /** Adds the supplied doc writer (and email, nick and description) to the current list of 
         * doc writers.
         * @access public 
         * @qualifier none
         * @param docWriters Doc writer to add
         * @param docWritersEmail Doc writer's email
         * @param docWritersNick Doc writer's nick
         * @param docWritersDesc Doc writer's description
         * @return void
         * @remarks No format verification is made on the doc writer's email. It is up to the user
         * to make sure that the input is correct (e.g. aa@bb.com).
         * @remarks The mailto: tag should not be specified in the doc writer's email.
         */
        void AddDocWriter(const wxString& docWriter, const wxString& docWriterEmail, const wxString& docWriterNick, const wxString& docWriterDesc) 
        { 
            wxAboutDialogInfo::AddDocWriter(docWriter);
            m_docWritersEmails.push_back(docWriterEmail);
            m_docWritersNicks.push_back(docWriterNick);
            m_docWritersDescs.push_back(docWriterDesc);
        }

        /** Returns the doc writers' emails the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current doc writers' emails
         */
        const wxArrayString& GetDocWritersEmails() const { return m_docWritersEmails; }

        /** Returns the doc writers' nicks the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current doc writers' nicks
         */
        const wxArrayString& GetDocWritersNicks() const { return m_docWritersNicks; }

        /** Returns the doc writers' descriptions the user added.
         * @access public
         * @qualifier const
         * @return const wxArrayString& Current doc writers' descriptions
         */
        const wxArrayString& GetDocWritersDescs() const { return m_docWritersDescs; }

    private:

        /************************************************************************
         * PRIVATE MEMBERS
         ************************************************************************/
        
        /** Footnote text - Added at the bottom of the about dialog between static lines
         */
        wxString m_footnote;

        /** Built with (third party applications used to build the editor) text - Encapsulated
         * inside a collapsible pane.
         */
        wxString m_builtWith;

        /** List of hyper links the user supplied - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_hyperLinks;

        /** List of hyper links descriptions the user supplied - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_hyperLinksDescs;
        
        /** List of acknowledgments - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_acknowledgments;

        /** List of acknowledgments' emails - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_acknowledgmentsEmails;

        /** List of acknowledgments' nicks - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_acknowledgmentsNicks;

        /** List of acknowledgments' descriptions - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_acknowledgmentsDescs;

        /** List of contacts - Appear inside a static box near the top of the about box
         */
        wxArrayString m_contacts;

        /** List of contacts' emails - Appear inside a static box near the top of the about box
         */
        wxArrayString m_contactsEmails;

        /** List of contacts' nicks - Appear inside a static box near the top of the about box
         */
        wxArrayString m_contactsNicks;

        /** List of contacts' descriptions - Appear inside a static box near the top of the about box
         */
        wxArrayString m_contactsDescs;
        
        /** List of developers' emails - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_developersEmails;

        /** List of developers' nicks - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_developersNicks;

        /** List of developers' descriptions - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_developersDescs;

        /** List of artists' emails - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_artistsEmails;

        /** List of artists' nicks - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_artistsNicks;

        /** List of artists' descriptions - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_artistsDescs;

        /** List of translators' emails - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_translatorsEmails;

        /** List of translators' nicks - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_translatorsNicks;

        /** List of translators' descriptions - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_translatorsDescs;

        /** List of doc writers' emails - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_docWritersEmails;

        /** List of doc writers' nicks - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_docWritersNicks;

        /** List of doc writers' descriptions - Encapsulated inside a collapsible pane.
         */
        wxArrayString m_docWritersDescs;
};

//////////////////////////////////////////////////////////////////////////
// DIALOGABOUT
//////////////////////////////////////////////////////////////////////////

/** DialogAbout - Custom "About" box.
  * @remarks This class is based on wxWidgets wxGenericAboutDialog (version 2.8.6) 
  * class but it has been heavily modified to allow for greater flexibility and
  * information display
  */
class DialogAbout : public wxDialog
{
    public:

        /************************************************************************
         * CONSTRUCTORS/DESTRUCTORS
         ************************************************************************/
        
        /** Default constructor. You must call @see Create to really initialize the dialog.
         * @access public 
         * @qualifier none
         * @return 
         */
        DialogAbout() : m_sizerText(wx_static_cast(wxSizer *, NULL)) { Init(); }
        
        /** Constructor which fully initializes the object (via a call to @see Create).
         * @access public 
         * @qualifier none
         * @param info Class with information for the "About" box fields
         * @return 
         */
        DialogAbout(const DialogAboutInfo& info) :
        m_sizerText(wx_static_cast(wxSizer *, NULL))
        {
            Init();
            (void)Create(info);
        }

        /************************************************************************
         * PUBLIC FUNCTIONS
         ************************************************************************/
        
        /** Constructs the "About" box fields.
         * @access public 
         * @qualifier none
         * @param info
         * @return bool
         * @remarks This should only be called manually if the default constructor was used
         */
        bool Create(const DialogAboutInfo& info);

    protected:

        /************************************************************************
         * PROTECTED FUNCTIONS
         ************************************************************************/
        
        /** Adds an arbitrary control to the text sizer contents with the specified flags.
         * @access protected 
         * @qualifier none
         * @param win Control to add
         * @param flags Flags to employ
         * @return void
         */
        void AddControl(wxWindow *win, const wxSizerFlags& flags = wxSizerFlags().Border(wxDOWN).Centre());

        /** Adds the text, if it's not empty, to the text sizer contents using the specified flags.
         * @access protected 
         * @qualifier none
         * @param text Text to add
         * @param flags Flags to employ
         * @return void
         */
        void AddText(const wxString& text, const wxSizerFlags& flags = wxSizerFlags().Border(wxDOWN).Centre());

        #if wxUSE_COLLPANE
            /** Adds a collapsible pane (if supported) containing the given text, using the supplied style.
             * @access protected 
             * @qualifier none
             * @param title Title of the collapsible pane
             * @param text Text inserted inside the collapsible pane
             * @param style Style to employ
             * @return void
             */
            void AddCollapsiblePane(const wxString& title, const wxString& text, long style = wxALIGN_CENTRE);

            /** Adds a collapsible pane (if supported) containing the given names (and associated 
             * emails, nicks and descriptions) using the supplied style. The format of each entry 
             * is as follows:
             *
             * Name (Nick) - <Email> 
             *   - Description
             *
             * Nick, Email and description are optional and will not be included if empty.             *
             * @access protected 
             * @qualifier none
             * @param title Title of the collapsible pane
             * @param names Array String with all the entry names
             * @param emails Array String with all the entry names' emails
             * @param nicks Array String with all the entry names' nicks
             * @param descs Array String with all the entry names' descs
             * @param style Style to employ
             * @return void
             */
            void AddCollapsiblePane(const wxString& title, const wxArrayString& names, const wxArrayString& emails, const wxArrayString& nicks, const wxArrayString& descs, long style = wxALIGN_CENTRE);
        #endif // wxUSE_COLLPANE

    private:

        /************************************************************************
         * PRIVATE FUNCTIONS
         ************************************************************************/
        
        /** Common part of all constructors, responsible for initializing the "About" box.
         * @access private 
         * @qualifier none
         * @return void
         */
        void Init() { m_sizerText = NULL; }

        /************************************************************************
         * PRIVATE MEMBERS
         ************************************************************************/
        
        /** Sizer used to align the "About" box text vertically
         */
        wxSizer *m_sizerText;
};

#endif // _DIALOG_ABOUT_H_
