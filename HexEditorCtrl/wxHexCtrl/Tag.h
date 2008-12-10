/***********************************(GPL)********************************
*	wxHexEditor is a hex edit tool for editing massive files in Linux   *
*	Copyright (C) 2006  Erdem U. Altinyurt                              *
*                                                                       *
*	This program is free software; you can redistribute it and/or       *
*	modify it under the terms of the GNU General Public License         *
*	as published by the Free Software Foundation; either version 2      *
*	of the License, or any later version.                               *
*                                                                       *
*	This program is distributed in the hope that it will be useful,     *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of      *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
*	GNU General Public License for more details.                        *
*                                                                       *
*	You should have received a copy of the GNU General Public License   *
*	along with this program;                                            *
*   if not, write to the Free Software	Foundation, Inc.,               *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA        *
*                                                                       *
*               home  : wxhexeditor.sourceforge.net                     *
*               email : death_knight at gamebox.net                     *
*************************************************************************/

#ifndef __Tag_h__
#define __Tag_h__

#include <wx/colordlg.h>
#include <wx/popupwin.h>

#include "TagDialogGui.h"

class TagElement{
	public:
		TagElement();
		~TagElement();
		TagElement( int64_t _start, int64_t _end, wxString _tag, wxColourData fntclr, wxColourData noteclr);
		int64_t start;
		int64_t end;
		wxString tag;
		wxColourData FontClrData;
		wxColourData NoteClrData;
		bool visible;
		void Show( const wxPoint& pos, wxWindow *parent );
		void Hide( void );
		wxPopupWindow *wxP;
	};

class TagDialog : public TagDialogGui{
	public:
		TagDialog( TagElement& TE, wxWindow* parent );
		void OnFontColor( wxCommandEvent& event );
		void OnNoteColor( wxCommandEvent& event );
		void OnSave( wxCommandEvent& event );
		void OnDelete( wxCommandEvent& event );
		void ChooseColor( wxColourData& tmpClrData );
		TagElement& Tag;
		TagElement TmpTag;
	};

#endif // __Tag_h__
