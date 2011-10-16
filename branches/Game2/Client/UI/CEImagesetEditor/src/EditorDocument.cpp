///////////////////////////////////////////////////////////////////////////////
//  For project details and authors, refer to README and AUTHORS files
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
// Standard wxWidget includes
#include "wx/wx.h"
#include "wx/filename.h"
#include <CEGUIXMLParser.h>
#include <CEGUIImageset.h>
#include <CEGUISystem.h>
#include <CEGUIDefaultResourceProvider.h>
#include <RendererModules/OpenGL/CEGUIOpenGLRenderer.h>

#include "CEImagesetEditor.h" 
// More specific wxWidgets includes
#include "wx/docview.h"
#include "EditorDocument.h"
#include "EditorView.h"
#include "CEGUIHelper.h"
#include "ImagesetHandler.h"

using namespace CEGUI;


IMPLEMENT_DYNAMIC_CLASS( EditorDocument, wxDocument )

// Declared in Imageset::xmlHandler
const wxPoint EditorDocument::c_defaultNativeResolution( wxPoint( 640, 480 ) );

//-----------------------------------------------------------------------
EditorDocument::EditorDocument()
{
    reset();
}

//-----------------------------------------------------------------------
bool EditorDocument::OnOpenDocument( const wxString& filename )
{
    if ( filename.empty() || !wxDocument::OnOpenDocument( filename ) )
        return false;

    wxString relFilename( getDefaultResourceGroupRelativePath( filename ) );

    // create handler object
    ImagesetHandler handler( this );

    // This could take some time, enter 'busy' state
    wxWindowDisabler wd; wxBusyCursor bc;

    // do parse (which uses handler to create actual data)
    try
    {
        DefaultResourceProvider* rp = static_cast<DefaultResourceProvider*>
            (CEGUI::System::getSingleton().getResourceProvider());
        wxFileName wxFN(filename);
        wxString strCurResourceGroup = wxFN.GetPath();
        CEGUI::String filePath = CEGUIHelper::ToCEGUIString(strCurResourceGroup);
        rp->setResourceGroupDirectory("editor_data", filePath);
        rp->setDefaultResourceGroup( "editor_data" );
        wxString fnameExt = wxFN.GetName();
        fnameExt.Append('.');
        fnameExt.Append(wxFN.GetExt());
        System::getSingleton().getXMLParser()->parseXMLFile(
            handler,
            CEGUIHelper::ToCEGUIString( fnameExt ),
            "Imageset.xsd",
            "" );
    }
    catch ( ... )
    {
        Logger::getSingleton().logEvent( "EditorDocument::onOpenDocument - loading of Imageset from file '" +
                                          CEGUIHelper::ToCEGUIString( filename ) + "' failed.", Errors );
        throw;
    }

    // Since we have overwritten this method, we must notify the views ourselves!
    UpdateAllViews();

    SetFilename( filename, true );
    SetTitle( filename );

    Modify( false );

    return true;
}

//-----------------------------------------------------------------------
bool EditorDocument::OnSaveDocument( const wxString& filename )
{
    try
    {
        Imageset* newImageset = generateRealImageset();

        std::ofstream fileSave;

        fileSave.open ( static_cast<const char*>( wxConvLibc.cWX2MB( filename ) ), std::ios::out ) ;
        if ( !fileSave.is_open() )
        {
            wxLogError ( wxT( "Error: could not open file for saving. Is it read-only?" ) ) ;
            return false;
        }

        ImagesetManager::getSingleton().writeImagesetToStream(
            CEGUIHelper::ToCEGUIString( m_imagesetName ), fileSave );

        fileSave.close() ;
        
        // cleanup imageset
        ImagesetManager::getSingleton().destroy( *newImageset );

        // Tell the document it's "clean" again
        Modify(false);
    }
    catch (CEGUI::Exception& e)
    {
        wxLogError ( wxT( "EditorDocument::OnSaveDocument - Error: %s" ),
                        CEGUIHelper::ToWXString( e.getMessage() ).c_str() );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------
bool EditorDocument::OnCloseDocument()
{
    reset();
    return true;
}

//-----------------------------------------------------------------------
bool EditorDocument::OnNewDocument()
{
    wxDocument::OnNewDocument();

    reset();

    // Since we have overwritten this method, we must notify the views ourselves!
    UpdateAllViews();

    return true;
}

//-----------------------------------------------------------------------
void EditorDocument::reset()
{
    deleteAllRegions();
    setImagesetName( wxT( "" ) );
    setImageFilename( wxT( "" ) );
    setNativeResolution( c_defaultNativeResolution );
    setAutoScaled( false );
    Modify( false );
}

//-----------------------------------------------------------------------
void EditorDocument::setImageFilename( const wxString& newfilename )
{
    if ( m_imageFilename != newfilename )
    {
        m_imageFilename = newfilename;

        if ( isViewValid() )
            getView()->onChangeImageFile( newfilename );

        Modify( true );
    }
}

//-----------------------------------------------------------------------
void EditorDocument::setImagesetName( const wxString& newname )
{
    if ( m_imagesetName != newname )
    {
        m_imagesetName = newname;

        if ( isViewValid() )
            getView()->onChangeImagesetName( newname );

        Modify( true );
    }
}

//-----------------------------------------------------------------------
void EditorDocument::setNativeResolution( const wxPoint& newnativeres )
{
    if ( m_nativeResolution != newnativeres )
    {
        m_nativeResolution = newnativeres;

        if ( isViewValid() )
            getView()->onChangeNativeResolution( newnativeres );

        Modify( true );
    }
}

//-----------------------------------------------------------------------
void EditorDocument::setAutoScaled( const bool newAutoScaled )
{
    if ( m_autoScaled != newAutoScaled )
    {
        m_autoScaled = newAutoScaled;

        if ( isViewValid() )
            getView()->onChangeAutoScaled( newAutoScaled );

        Modify( true );
    }
}

//-----------------------------------------------------------------------
void EditorDocument::addRegion( const wxString& name, const wxRect& dim,const wxPoint &OffsetPoint)
{
    m_mapSetRectangles[ name ].rect = dim;
    m_mapSetRectangles[ name ].offset = OffsetPoint;

    if ( isViewValid() )
        getView()->onAddRegion( name, dim, OffsetPoint);

    Modify( true );
}

//-----------------------------------------------------------------------
void EditorDocument::deleteRegion( const wxString& name )
{
    m_mapSetRectangles.erase( name );

    if ( isViewValid() )
        getView()->onDelRegion( name );

    Modify( true );
}

//-----------------------------------------------------------------------
void EditorDocument::deleteAllRegions()
{
    m_mapSetRectangles.clear();

    if ( isViewValid() )
        getView()->onDelAllRegion();

    Modify( true );
}

void EditorDocument::setRegionArea( const wxString& name, const wxRect& dim, const bool evt_src)
{
    if (getRegionArea(name) == dim)
        return;

    m_mapSetRectangles[ name ].rect = dim;

    if ( isViewValid() )
        getView()->onSetRegionArea(name, dim, evt_src);

    Modify( true );
}
//-----------------------------------------------------------------------
void EditorDocument::setRegionArea( const wxString& name, const wxRect& dim, const wxPoint& point, const bool evt_src )
{
    // if area is the same, do nothing (saves infinite recursion possibility)
    if (getRegionArea(name) == dim && GetRegionOffset(name) == point)
        return;

    m_mapSetRectangles[ name ].rect = dim;
    m_mapSetRectangles[ name ].offset = point;

    if ( isViewValid() )
        getView()->onSetRegionArea(name, dim, evt_src);

    Modify( true );
}

wxPoint EditorDocument::GetRegionOffset( const wxString& name ) const
{
    wxPoint point;
    mapNamedRegion::const_iterator iter = 
        m_mapSetRectangles.find( name );

    if ( iter != m_mapSetRectangles.end() )
        point = iter->second.offset;

    return point;
}
//-----------------------------------------------------------------------
wxRect EditorDocument::getRegionArea( const wxString& name ) const
{
    wxRect area;

    mapNamedRegion::const_iterator iter = 
        m_mapSetRectangles.find( name );

    if ( iter != m_mapSetRectangles.end() )
        area = iter->second.rect;

    return area;
}

//-----------------------------------------------------------------------
void EditorDocument::renameRegion( const wxString& currentname, const wxString& newname )
{
    // check the new name does not exist in CEGUI
    if (WindowManager::getSingleton().isWindowPresent(
        CEGUIHelper::ToCEGUIString( newname ) ) )
    {
        wxLogError ( wxT( "EditorDocument::renameRegion - Error: A region with that name already exists." ) ) ;
    }
    // new name is fine, go ahead with the rename
    else
    {
        wxRect dim    = m_mapSetRectangles[ currentname ].rect;
        wxPoint point = m_mapSetRectangles[ currentname ].offset;
        m_mapSetRectangles.erase( currentname );
        m_mapSetRectangles[ newname ].rect = dim;
        m_mapSetRectangles[ newname ].offset = point;
        if ( isViewValid() )
            getView()->onRenameRegion( currentname, newname );

        Modify( true );
    }
}

//-----------------------------------------------------------------------
void EditorDocument::Modify( bool modify )
{
    wxDocument::Modify( modify );
    UpdateAllViews();
}

//-----------------------------------------------------------------------
CEGUI::Imageset* EditorDocument::generateRealImageset()
{
    const String imagesetName ( CEGUIHelper::ToCEGUIString( m_imagesetName ) );
    const String imageFilename ( CEGUIHelper::ToCEGUIString( m_imageFilename ) );

    ImagesetManager& isMgr = ImagesetManager::getSingleton();

    if ( isMgr.isDefined( imagesetName ) )
        // TODO: Maybe ask user whether to continue here?
        isMgr.destroy( imagesetName );

    // create the imageset
    Imageset& imageset(isMgr.createFromImageFile( imagesetName, imageFilename ));

    // auto-scale options
    imageset.setAutoScalingEnabled( m_autoScaled );
    imageset.setNativeResolution( CEGUI::Size( m_nativeResolution.x, m_nativeResolution.y ) );

    // iterate over all the elements in the class
    mapNamedRegion::iterator it;
    for( it = m_mapSetRectangles.begin(); it != m_mapSetRectangles.end(); ++it )
    {
        const String imageName = CEGUIHelper::ToCEGUIString( wxString( it->first ) );

        if ( !imageset.isImageDefined( imageName ) )
        {
            const wxRect r( it->second.rect);
            const CEGUI::Rect newDim( ( ( r.GetLeft() > 0 ) ? r.GetLeft() : 0 ),
                               ( ( r.GetTop() > 0 ) ? r.GetTop() : 0 ),
                               ( ( r.GetRight() + 1 > 0 ) ? r.GetRight() + 1 : 0 ),
                               ( ( r.GetBottom() + 1 > 0 ) ? r.GetBottom() + 1 : 0 ) );
            const CEGUI::Point p ( it->second.offset.x, it->second.offset.y );
            imageset.defineImage( imageName, newDim, p );
        }
    }

    if ( imageset.isImageDefined ( "full_image" ) )
        imageset.undefineImage( "full_image" );

    return &imageset;
}

//-----------------------------------------------------------------------
bool EditorDocument::isViewValid() const
{
    return GetFirstView() != 0;
}

//-----------------------------------------------------------------------
EditorView* EditorDocument::getView() const
{
    return static_cast<EditorView*>( GetFirstView() );
}

//-----------------------------------------------------------------------
wxString EditorDocument::getDefaultResourceGroupRelativePath( const wxString& filename ) const
{
    wxString resourcePath( wxT( "./" ) );
    wxFileName filespec( filename );

    CEGUI::DefaultResourceProvider* rp =
        static_cast<CEGUI::DefaultResourceProvider*>(
            CEGUI::System::getSingleton().getResourceProvider() );

    CEGUI::String defResGrp = rp->getDefaultResourceGroup();

    if ( !defResGrp.empty() )
        resourcePath = CEGUIHelper::ToWXString(
            rp->getResourceGroupDirectory( defResGrp ) );

    filespec.MakeRelativeTo( resourcePath );
    return filespec.GetFullPath();
}
