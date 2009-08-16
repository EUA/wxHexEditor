/***********************************(GPL)********************************
*   wxHexEditor is a hex edit tool for editing massive files in Linux   *
*   Copyright (C) 2006  Erdem U. Altinyurt                              *
*                                                                       *
*   This program is free software; you can redistribute it and/or       *
*   modify it under the terms of the GNU General Public License         *
*   as published by the Free Software Foundation; either version 2      *
*   of the License, or any later version.                               *
*                                                                       *
*   This program is distributed in the hope that it will be useful,     *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
*   GNU General Public License for more details.                        *
*                                                                       *
*   You should have received a copy of the GNU General Public License   *
*   along with this program;                                            *
*   if not, write to the Free Software	Foundation, Inc.,                *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA        *
*                                                                       *
*               home  : wxhexeditor.sourceforge.net                     *
*               email : death_knight at gamebox.net                     *
*************************************************************************/

#ifndef __wxHexCtrl__
#define __wxHexCtrl__
#include <stdint.h>
#include <wx/defs.h>
#include <wx/buffer.h>
#include <wx/textctrl.h>
#include <wx/caret.h>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "Tag.h"

#define __idTagSelect__ 1500
#define __idTagEdit__ 1501

WX_DEFINE_ARRAY(TagElement *, ArrayOfTAG);

class wxHexCtrl : public wxScrolledWindow{
	public:
//		wxHexCtrl() { }
		wxHexCtrl( wxWindow *parent );
		wxHexCtrl( wxWindow *parent,
				wxWindowID id,
				const wxString &value = wxEmptyString,
				const wxPoint &pos = wxDefaultPosition,
				const wxSize &size = wxDefaultSize,
				long style = 0,
				const wxValidator& validator = wxDefaultValidator);
		~wxHexCtrl();
		//wxChar& CharAt(int x, int y) { return m_text.GetWritableChar(x + m_Chars.x * y); }
		//wxChar& CharAt(int x, int y) { return m_text.GetWritableChar(x + byte_per_line * y); }
		wxChar& CharAt(int offset){ return m_text.GetWritableChar(offset); }

		// Operations
virtual void SetDefaultStyle( wxTextAttr& new_attr );
		void SetSelectionStyle( wxTextAttr& new_attr );
virtual void CreateCaret( void );
		void MoveCaret( int x );
		void MoveCaret( wxPoint p );
		void Clear( bool ClearDC=true, bool cursor_reset=true );
		void SetBinValue( wxString buffer, bool repaint = true );
		void SetBinValue( char* buffer, int byte_count, bool repaint = true );
		void SetValue( wxString buffer, bool repaint = true );
		wxString GetValue( void );
		long ReadBytes( char* buffer, int start_location, int byte_count, bool no_repaint = false );
		char ReadByte(int byte_location);
static const char* HexToChar(const wxString& HexValue);
static wxMemoryBuffer HexToBin(const wxString& HexValue);
virtual void Replace(unsigned from, unsigned to, const wxString& value);
virtual void Replace(unsigned hex_location, const wxChar& value, bool paint=true);
		void WriteByte( const unsigned char& byte );
		void WriteHex( const wxString& text_value );

		// Caret Movement
		void Home( bool MoveCaret = true );
		void End( bool MoveCaret = true );
		void FirstLine( bool MoveCaret = true );
		void PrevChar( bool MoveCaret = true );
		void NextChar( bool MoveCaret = true );
		void PrevLine( bool MoveCaret = true );
		void NextLine( bool MoveCaret = true );
		void LastLine( bool MoveCaret = true );
		void DoMoveCaret();		// move the caret to m_Caret.x, m_Caret.y

		// Shaper Classes, All other classes has to be depended to this function for proper action!
virtual bool IsDenied() { return IsDenied( m_Caret.x );}
virtual bool IsDenied( int x );
virtual bool IsAllowedChar(const char& chr);
//virtual	const char Filter(const char& ch);
		int xCountDenied( int x );
		wxSize GetCharSize(){return m_CharSize;}

		// Movement Support
virtual int CharacterPerLine( void );
virtual int BytePerLine( void )	{ return CharacterPerLine() / 2; }
virtual int ByteCapacity( void ){ return m_Window.y*BytePerLine(); }
		int LineCount( void )	{ return m_Window.y; }
		int ActiveLine( void )	{ return m_Caret.y+1; } //ReAllocated, start from 1, NOT 0
		int GetByteCount( void ){ return m_text.Length()/2;	}
		int GetInsertionPoint( void );
		void SetInsertionPoint( unsigned int pos );
		int GetLastPosition( void ){ return m_text.Length() - 1; }
virtual int ToVisiblePosition( int InternalPosition );
virtual int ToInternalPosition( int VisiblePosition );
virtual int PixelCoordToInternalPosition( wxPoint mouse );
		wxPoint InternalPositionToVisibleCoord( int position );
		wxPoint PixelCoordToInternalCoord( wxPoint mouse );

		// TAG Support and Selection
		ArrayOfTAG TagArray;
		void SetSelection( unsigned start, unsigned end );
		void ClearSelection( bool RePaint = true );
		struct selector: public TagElement{		//select
			bool selected;		//select available variable
			} select;
virtual void TagPainter( wxMemoryDC& DC, TagElement& TG );
		void RePaint(){
			wxPaintEvent painter;
			OnPaint( painter );
			}
		void OnTagHideAll( void );
		bool *TagMutex;



	protected:
		// event handlers
		wxPoint LastRightClickPosition;	//Holds last right click for TagEdit function
		void OnPaint( wxPaintEvent &event );
		void OnSize( wxSizeEvent &event );
		void OnChar( wxKeyEvent &event );
virtual void OnMouseLeft( wxMouseEvent& event );
virtual void OnMouseRight( wxMouseEvent& event );
virtual void OnMouseMove( wxMouseEvent& event );
		void OnFocus( wxFocusEvent& event );
		void OnKillFocus( wxFocusEvent& event );
		void OnResize( wxSizeEvent &event );
		void OnTagSelection( wxCommandEvent &event );
		void OnTagEdit( wxCommandEvent &event );
		void ShowContextMenu( wxPoint pos );
		DECLARE_EVENT_TABLE();

		void OnTestCall(void); // 4 Test
		void ChangeSize();	// update the geometry
		wxPoint   m_Margin;	// the margin around the text (looks nicer)
		wxPoint   m_Caret;	// position (in text coords) of the caret
		wxPoint	  m_Window;	// the size (in text coords) of the window
		wxString  m_text;
		wxTextAttr HexDefaultAttr;
		//wxTextAttr HexSelectAttr;

		wxSize  m_CharSize;	// size (in pixels) of one character
	   // DECLARE_DYNAMIC_CLASS(wxHexCtrl)
	};

class wxHexTextCtrl : public wxHexCtrl{
	public:
//		wxHexTextCtrl():wxHexCtrl(){}
		wxHexTextCtrl( wxWindow *parent ): wxHexCtrl( parent ){}
		wxHexTextCtrl( wxWindow *parent,
				wxWindowID id,
				const wxString &value = wxEmptyString,
				const wxPoint &pos = wxDefaultPosition,
				const wxSize &size = wxDefaultSize,
				long style = 0,
				const wxValidator& validator = wxDefaultValidator) :
				wxHexCtrl(parent, id, value, pos, size, style, validator){
				wxWindow::SetCursor( wxCURSOR_CHAR );
				};
		bool IsDenied(){ return false; }
		bool IsDenied( int x ){ return false; }
		void Replace(unsigned text_location, const wxChar& value, bool paint);
		void ChangeValue( const wxString& value, bool paint );
		void SetDefaultStyle( wxTextAttr& new_attr );		//For caret diet (to 1 pixel)
		int PixelCoordToInternalPosition( wxPoint mouse );
		int ToVisiblePosition( int InternalPosition ){ return InternalPosition; }
		int ToInternalPosition( int VisiblePosition ){ return VisiblePosition; }
		bool IsAllowedChar(const char& chr);
		wxChar Filter(const char& chr);
		int BytePerLine( void ){return CharacterPerLine();};
	};

class wxHexOffsetCtrl : public wxHexCtrl{
	public:
//		wxHexOffsetCtrl():wxHexCtrl(){ hex_offset=false;offset_position=0; }
		wxHexOffsetCtrl( wxWindow *parent ): wxHexCtrl( parent ){ hex_offset=false; offset_position=0;}
		wxHexOffsetCtrl(wxWindow *parent,
				wxWindowID id,
				const wxString &value = wxEmptyString,
				const wxPoint &pos = wxDefaultPosition,
				const wxSize &size = wxDefaultSize,
				long style = 0,
				const wxValidator& validator = wxDefaultValidator) :
				wxHexCtrl(parent, id, value, pos, size, style, validator){
				GetCaret()->Hide();
				SetCaret( NULL );
				hex_offset=false;
				offset_position=0;
				};
		bool IsDenied(){ return false; }
		bool IsDenied( int x ){ return false; }
		int ToVisiblePosition( int InternalPosition ){ return InternalPosition; }
		int ToInternalPosition( int VisiblePosition ){ return VisiblePosition; }
		void SetValue( int64_t position );
		void SetValue( int64_t position, int byteperline );
		void OnMouseRight( wxMouseEvent& event ){event.Skip(false);}
		void OnMouseLeft( wxMouseEvent& event );
		void OnMouseMove( wxMouseEvent& event ){event.Skip(false);}
		bool hex_offset;
		int64_t offset_position;
		int BytePerLine;
	};
#endif
