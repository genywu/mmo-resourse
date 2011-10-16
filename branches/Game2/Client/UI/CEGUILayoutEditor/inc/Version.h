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

#ifndef _VERSION_H_
#define _VERSION_H_

//////////////////////////////////////////////////////////////////////////
// VERSION INFO
//////////////////////////////////////////////////////////////////////////

// NOTE: the constants below must be changed with each new version
#define CE_LAYOUT_EDITOR_MAJOR_VERSION 0
#define CE_LAYOUT_EDITOR_MINOR_VERSION 6
#define CE_LAYOUT_EDITOR_RELEASE_NUMBER 3

//////////////////////////////////////////////////////////////////////////
// VERSION CHECKERS
//////////////////////////////////////////////////////////////////////////

/** Check if the current version is at least major.minor.release 
 * @remarks This is not really useful inside the editor, but might come in handy
 * if the editor is used by another application in the future (e.g. tools framework)
 */
#define CE_LAYOUT_EDITOR_CHECK_VERSION(major,minor,release) \
    (CE_LAYOUT_EDITOR_MAJOR_VERSION > (major) || \
    (CE_LAYOUT_EDITOR_MAJOR_VERSION == (major) && CE_LAYOUT_EDITOR_MINOR_VERSION > (minor)) || \
    (CE_LAYOUT_EDITOR_MAJOR_VERSION == (major) && CE_LAYOUT_EDITOR_MINOR_VERSION == (minor) && CE_LAYOUT_EDITOR_RELEASE_NUMBER >= (release)))

#endif // _VERSION_H_
