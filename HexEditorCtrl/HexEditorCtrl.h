/***********************************(GPL)********************************
*	wxHexEditor is a hex edit tool for editing massive files in Linux   *
*	Copyright (C) 2007  Erdem U. Altinyurt                              *
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
#ifndef __wxHexEditorCtrl__
#define __wxHexEditorCtrl__

#include "HexEditorCtrlGui.h"

class HexEditorCtrl: public HexEditorCtrlGui {
	public:
		HexEditorCtrl(wxWindow* parent, int id,
						const wxPoint& pos=wxDefaultPosition,
						const wxSize& size=wxDefaultSize,
						long style=0);
		void ReadFromBuffer( int64_t position, int lenght, char *buffer, bool cursor_reset = true, bool paint = true );

	protected:
		int HexPerLine( void )  { return hex_ctrl->HexPerLine(); }
virtual int BytePerLine( void )	{ return HexPerLine() / 2; }
		int ByteCapacity( void ){ return hex_ctrl->ByteCapacity(); }
		int LineCount( void )	{ return hex_ctrl->LineCount(); }
		int ActiveLine( void )	{ return hex_ctrl->ActiveLine(); }
		int GetByteCount( void ){ return hex_ctrl->GetByteCount(); }
//		int GetInsertionPoint( void );
//		void SetInsertionPoint( unsigned int pos );
		int GetLastPosition( void ){ return hex_ctrl->GetLastPosition(); }
/*
virtual int ToExactPosition( int InternalPosition );
virtual int ToInternalPosition( int ExactPosition );
virtual int PixelCoordToInternalPosition( wxPoint mouse );
		wxPoint InternalPositionToExactCoord( int position );
		wxPoint PixelCoordToInternalCoord( wxPoint mouse );
*/

		wxChar TextFilter(const unsigned char ch);
		void Clear( bool RePaint, bool cursor_reset=true );
//		DECLARE_EVENT_TABLE()
		virtual void OnResize( wxSizeEvent& event);
		void ClearPaint( void );
		void PaintSelection( void );
		struct selector{		//selection structure
			enum states{ SELECTION_FALSE = 0, SELECTION_TRUE, SELECTION_END };
			enum states state;
			int64_t start_offset;	//real start position
			int64_t end_offset;	//real end position, included to selection
			} selection;
		void HexCharReplace( long char_location, const wxChar chr);
		void TextCharReplace( long char_location, const wxChar chr);
		void MyFreeze();
		void MyThaw();
		int GetLocalHexInsertionPoint( void );
virtual	void SetLocalHexInsertionPoint( int hex_location );
		void OnOffsetMouseFocus( wxMouseEvent& event );
		bool hex_offset;
		int64_t start_offset;	//holds current start offset of file

//		virtual void OnKeyboardChar( wxKeyEvent& event );
//		virtual void OnKeyboardInput( wxKeyEvent& event );
	};

#endif
