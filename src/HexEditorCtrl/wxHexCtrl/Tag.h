/***********************************(GPL)********************************
*   wxHexEditor is a hex edit tool for editing massive files in Linux   *
*   Copyright (C) 2010  Erdem U. Altinyurt                              *
*                                                                       *
*   This program is free software; you can redistribute it and/or       *
*   modify it under the terms of the GNU General Public License         *
*   as published by the Free Software Foundation; either version 2      *
*   of the License.                                                     *
*                                                                       *
*   This program is distributed in the hope that it will be useful,     *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
*   GNU General Public License for more details.                        *
*                                                                       *
*   You should have received a copy of the GNU General Public License   *
*   along with this program;                                            *
*   if not, write to the Free Software Foundation, Inc.,                *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA        *
*                                                                       *
*               home  : www.wxhexeditor.org                             *
*               email : spamjunkeater@gmail.com                         *
*************************************************************************/


#ifndef __Tag_h__
#define __Tag_h__
#include <iostream>
#include <stdint.h>
#include <wx/clrpicker.h>
#if wxCHECK_VERSION(2, 9, 0)
	#include <wx/cmndata.h>
#endif
#if wxCHECK_VERSION(2, 9, 1)
	#include <wx/colourdata.h>
#endif
#include <wx/popupwin.h>
#include <wx/dcmemory.h>
#include "TagDialogGui.h"

//class TagElement:public wxPopupWindow{
class TagElement{
	public:
		TagElement();
		~TagElement();
		TagElement( uint64_t _start, uint64_t _end, wxString _tag, wxColourData fntclr, wxColourData noteclr);
		TagElement( uint64_t _start, uint64_t _end, wxString _tag, wxColour fntclr, wxColour noteclr);
		uint64_t start;
		uint64_t end;
		wxString tag;
		wxColourData FontClrData;
		wxColourData NoteClrData;
		bool visible;
		void Show( const wxPoint& pos, wxWindow *parent );
		void Hide( void );
		bool isCover( uint64_t );
		void print(void);
		wxColour SoftColour(wxColour);
		static int TagCompare(TagElement **first, TagElement **second);
//		bool operator<( TagElement a, TagElement b){ return a.start < b.start }
//		bool operator>( TagElement a, TagElement b){ return a.start > b.start }

//		void OnKillFocus( wxFocusEvent& event );
#ifndef __WXMAC__
		wxPopupWindow *wxP;
#endif	//wxPopupWindows is not yet implemented in MacOSX
	};

class TagDialog : public TagDialogGui{
	public:
		TagDialog( TagElement& TE, wxWindow* parent );
		void OnSave( wxCommandEvent& event );
		void OnDelete( wxCommandEvent& event );
		void OnColorChange( wxCommandEvent& event );
		TagElement& Tag;
		TagElement TmpTag;
	};


#endif // __Tag_h__
