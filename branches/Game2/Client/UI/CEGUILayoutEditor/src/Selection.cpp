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
#include "Selection.h"

using namespace CEGUI;

//////////////////////////////////////////////////////////////////////
// SELECTION PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
size_t Selection::Size() const
{
	// Use the largest collection
	return m_allBoxes.size() ;
}

//------------------------------------------------------------------------
bool Selection::Select (CEGUI::Window* aWindow) 
{
	// Toggle when already selected
	if (IsSelected (aWindow))
	{
		Unselect(aWindow);
	}
	else if (!IsParentOrChild(aWindow))
	{
		// Only top windows are moveable
		m_moveableBoxes.push_back (WindowBox(aWindow));
		// Select the window and its children
		m_allBoxes.push_back (WindowBox(aWindow));
		//SelectChildren(aWindow);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
bool Selection::SelectAll (CEGUI::Window* aWindow) 
{
	wxASSERT_MSG(aWindow != NULL, wxT("Given Windows may not be NULL"));

	// Reset
	ClearSelection();

	// Select
	m_moveableBoxes.push_back (WindowBox(aWindow));
	m_allBoxes.push_back (WindowBox(aWindow));

	// Children too, but not resizable
	SelectChildren(aWindow);

	return true;
}

//------------------------------------------------------------------------
void Selection::Unselect(const CEGUI::Window* const aWindow)
{
	DeleteWindowBox(m_allBoxes, aWindow);
	DeleteWindowBox(m_moveableBoxes, aWindow);
	// Remove selected children too
	UnselectChildren(aWindow);
}

//------------------------------------------------------------------------
bool Selection::IsSelected(const CEGUI::Window* const aWindow) 
{
	// Use the largest collection
	for (Boxes::iterator boxIt = m_allBoxes.begin(); boxIt != m_allBoxes.end(); ++boxIt)
	{
		if (aWindow == boxIt->GetWindow())
		{			
			return true;
		}
	}

	// Isn't selected
	return false;
}

//------------------------------------------------------------------------
void Selection::ClearSelection() 
{
	// Remove all boxes
	m_allBoxes.clear();
	m_moveableBoxes.clear();
}

//------------------------------------------------------------------------
WindowBox* Selection::FindBoxFor(const CEGUI::Window* const aWindow) 
{
	// Look in the largest collection
	for (Boxes::iterator boxIt = m_allBoxes.begin(); boxIt != m_allBoxes.end(); ++boxIt)
	{
		if (boxIt->GetWindow() == aWindow)
		{
			return &*boxIt;
		}
	}
	return NULL;
}

//------------------------------------------------------------------------
void Selection::Update()
{	// Tick all boxes
	ResetBoxes(m_allBoxes);
	ResetBoxes(m_moveableBoxes);
}

//------------------------------------------------------------------------
void Selection::ResetBoxes(Boxes& aBoxes)
{
	for (Boxes::iterator boxIt = aBoxes.begin(); boxIt != aBoxes.end(); ++boxIt)
	{
		boxIt->Reset();
	}
}

//////////////////////////////////////////////////////////////////////////
// SELECTION PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////

// ------------------------------------------------------------------------
bool Selection::IsParentOrChild(const CEGUI::Window* const aWindow) 
{
    for (Boxes::iterator boxIt = m_allBoxes.begin(); boxIt != m_allBoxes.end(); ++boxIt)
    {
		// Is the given element the parent of one of the currently selected elements,
		// or is the given element the child of one of the currently selected elements? 
		// (We only have to look one level deep)
        if (boxIt->GetWindow()->getParent() == aWindow || boxIt->GetWindow()->isChild(aWindow))
        {
            // Bummer... cannot select then
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------
void Selection::SelectChildren(const CEGUI::Window* aWindow)
{
	for (size_t i=0; i<aWindow->getChildCount(); i++)
	{
        CEGUI::Window* child = aWindow->getChildAtIdx(i);
		// Skip __auto windows
		if (child->getName().find("__auto_") == String::npos && child->getName().find("__TabPane__"))
		{	// Add as an non-resizable box
			m_allBoxes.push_back (WindowBox(child, false));
			// Recurse!
			SelectChildren(child);
		}
	}
}

//------------------------------------------------------------------------
void Selection::UnselectChildren(const CEGUI::Window* aWindow)
{
	for (size_t i=0; i<aWindow->getChildCount(); i++)
	{
        CEGUI::Window* child = aWindow->getChildAtIdx(i);
		// Delete from the large collection, cause that one contains the children
		// Also, the moveable collection does *not* contain children
		if (DeleteWindowBox(m_allBoxes, child))
		{	// Recurse!
			UnselectChildren(child);
		}
	}
}

//------------------------------------------------------------------------
bool Selection::DeleteWindowBox(Boxes& aBoxes, const CEGUI::Window* aWindow)
{
	for (Boxes::iterator boxIt = aBoxes.begin(); boxIt != aBoxes.end(); ++boxIt)
	{
		if (boxIt->GetWindow() == aWindow)
		{
			aBoxes.erase(boxIt) ;
			return true;
		}
	}
	return false;
}
