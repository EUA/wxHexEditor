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

#ifndef wxHEX_CTRL_H
#define wxHEX_CTRL_H

#include <wx/textctrl.h>
#include <wx/caret.h>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "TagDialogGui.h"
#define idTagMenu 1500
#include <wx/colordlg.h>
#include <wx/popupwin.h>
struct TagElement{
			unsigned start;
			unsigned end;
			wxString tag;
			wxColourData FontClrData;
			wxColourData NoteClrData;
			//WXFont tagFont;
			};
WX_DEFINE_ARRAY(TagElement *, wxArrayTAG);

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

class wxHexCtrl : public wxScrolledWindow{
	public:
		wxHexCtrl() { }
		wxHexCtrl( wxWindow *parent );
		wxHexCtrl(wxWindow *parent,
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
		//void SetStyle( unsigned from, unsigned to, wxTextAttr& new_attr );
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
		char* HexToChar(const wxString& HexValue);
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
virtual bool inline IsDenied() { return IsDenied( m_Caret.x );}
virtual bool IsDenied( int x );
virtual	bool IsAllowedChar(char chr);
		int xCountDenied( int x );

		// Movement Support
virtual	int CharacterPerLine( void );
		int BytePerLine( void )	{ return CharacterPerLine() / 2; }
		int ByteCapacity( void ){ return m_Window.y*BytePerLine(); }
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
		wxArrayTAG	TagArray;
		void SetSelection( unsigned start, unsigned end );
		void ClearSelection( bool RePaint = true );
		struct selector: public TagElement{		//selection
			bool selected;		//selection available variable
			} select;
		void TagPainter( wxMemoryDC& DC, TagElement& TG );
		void TagPopup(TagElement& TG, const wxPoint& pos, wxWindow *parent );
		void RePaint(){
			wxPaintEvent painter;
			OnPaint( painter );
			}
	protected:
		// event handlers
		void OnPaint( wxPaintEvent &event );
		void OnSize( wxSizeEvent &event );
		void OnChar( wxKeyEvent &event );
		void OnMouseLeft( wxMouseEvent& event );
		void OnMouseRight( wxMouseEvent& event );
		void OnMouseMove( wxMouseEvent& event );
		void OnFocus( wxFocusEvent& event );
		void OnKillFocus( wxFocusEvent& event );
		void OnResize( wxSizeEvent &event );
		void OnTagSelection( wxCommandEvent &event );
		void ShowContextMenu(const wxPoint& pos);
		DECLARE_EVENT_TABLE();

		void OnTestCall(void); // 4 Test
		void ChangeSize();	// update the geometry
		wxPoint   m_Margin;	// the margin around the text (looks nicer)
		wxPoint   m_Caret;	// position (in text coords) of the caret
		wxPoint	  m_Window;	// the size (in text coords) of the window
		wxString  m_text;
		wxTextAttr HexDefaultAttr;
		//wxTextAttr HexSelectAttr;
//		wxMutex frost;
	public:
		wxPoint   m_Char;	// size (in pixels) of one character
	   // DECLARE_DYNAMIC_CLASS(wxHexCtrl)
	};

class wxHexTextCtrl : public wxHexCtrl{
	public:
		wxHexTextCtrl():wxHexCtrl(){}
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
		bool inline IsDenied(){ return false; }
		bool inline IsDenied( int x ){ return false; }
		void Replace(unsigned text_location, const wxChar& value, bool paint);
		void ChangeValue( const wxString& value, bool paint );
		void SetDefaultStyle( wxTextAttr& new_attr );		//For caret diet (to 1 pixel)
		int PixelCoordToInternalPosition( wxPoint mouse );
		int ToVisiblePosition( int InternalPosition ){ return InternalPosition; }
		int ToInternalPosition( int VisiblePosition ){ return VisiblePosition; }
		bool IsAllowedChar(char chr);
	};

class wxHexOffsetCtrl : public wxHexCtrl{
	public:
		wxHexOffsetCtrl():wxHexCtrl(){}
		wxHexOffsetCtrl( wxWindow *parent ): wxHexCtrl( parent ){}
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
				};
		bool inline IsDenied(){ return false; }
		bool inline IsDenied( int x ){ return false; }
		void ChangeValue( const wxString& value, bool paint );
		int ToVisiblePosition( int InternalPosition ){ return InternalPosition; }
		int ToInternalPosition( int VisiblePosition ){ return VisiblePosition; }
	};
#endif
