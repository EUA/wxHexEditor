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

#ifndef __wxHexCtrl__
#define __wxHexCtrl__
#include <stdint.h>
#include <wx/defs.h>
#include <wx/buffer.h>
#include <wx/textctrl.h>
#include <wx/caret.h>
#include <wx/wx.h>
#include <wx/config.h>
#include <wx/dcbuffer.h>
#include <wx/clipbrd.h>
#include "Tag.h"

#define __idTagAddSelect__ 1500
#define __idTagEdit__ 1501
#define __idOffsetHex__ 1502

//#define _Use_Graphics_Contex_

inline wxChar CP473toUnicode( unsigned char ch );
inline wxString CP473toUnicode( wxString& line );

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
		wxChar CharAt(int offset){ return m_text.GetChar(offset); }

		// Operations
		void SetFormat( wxString fmt );
		wxString GetFormat( void );
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
		bool IsDeniedCache[1024];//cache, Enought for this days...
		int CPL;
virtual bool IsDenied() { return IsDenied( m_Caret.x );}
virtual bool IsDenied( int x );
virtual bool IsDenied_NoCache( int x );
virtual bool IsAllowedChar(const char& chr);
//virtual	const char Filter(const char& ch);
		int xCountDenied( int x );
		wxSize GetCharSize(){return m_CharSize;}

		// Movement Support
virtual void DrawCursorShadow(wxDC*);
virtual int CharacterPerLine( bool NoCache=false );
virtual int BytePerLine( void ){ return CharacterPerLine() / 2; }
virtual int ByteCapacity( void ){ return m_Window.y*BytePerLine(); }
virtual int GetByteCount( void ){ return m_text.Length()/2;	}
		int LineCount( void )	{ return m_Window.y; }
		int ActiveLine( void )	{ return m_Caret.y+1; } //ReAllocated, start from 1, NOT 0
		int GetInsertionPoint( void );
virtual void SetInsertionPoint( unsigned int pos );
		int GetLastPosition( void ){ return m_text.Length() - 1; }
virtual int ToVisiblePosition( int InternalPosition );
virtual int ToInternalPosition( int VisiblePosition );
virtual int PixelCoordToInternalPosition( wxPoint mouse );
		wxPoint InternalPositionToVisibleCoord( int position );
		wxPoint PixelCoordToInternalCoord( wxPoint mouse );

		// TAG Support and Selection
		ArrayOfTAG TagArray;
		wxArrayInt ThinSeperationLines;
		TagElement* GetTagByPix( wxPoint PixelCoord );
		void SetSelection( unsigned start, unsigned end );
		void ClearSelection( bool RePaint = true );
		struct selector: public TagElement{		//select
			bool selected;		//select available variable
			} select;
virtual void TagPainter( wxDC* DC, TagElement& TG );
		void RePaint( void );

inline void DrawSeperationLineAfterChar( wxDC* DC, int offset );
		void OnTagHideAll( void );
		bool *TagMutex;
		int *ZebraStriping;

	protected:
		wxDC* UpdateDC( void );
		wxMemoryDC* CreateDC( void );
		wxMemoryDC* internalBufferDC;
		wxBitmap*   internalBufferBMP;
		wxString		HexFormat;
		// event handlers
		wxPoint LastRightClickPosition;	//Holds last right click for TagEdit function
		void OnPaint( wxPaintEvent &event );
		void OnSize( wxSizeEvent &event );
		void OnChar( wxKeyEvent &event );
virtual void OnMouseLeft( wxMouseEvent& event );
virtual void OnMouseRight( wxMouseEvent& event );
		virtual void OnMouseMove( wxMouseEvent& event );
		void OnKillFocus( wxFocusEvent& event ); //tempotarily publicized
		void OnFocus( wxFocusEvent& event );

		void OnTagAddSelection( wxCommandEvent &event );
		void OnTagEdit( wxCommandEvent &event );
		void ShowContextMenu( wxPoint pos );
		DECLARE_EVENT_TABLE();

		void OnTestCall(void); // 4 Test
	public:
virtual void ChangeSize();	// update the geometry
	protected:
		wxPoint   m_Margin;	// the margin around the text (looks nicer)
		wxPoint   m_Caret;	// position (in text coords) of the caret
		wxPoint   m_Window;	// the size (in text coords) of the window
		wxString  m_text;
		wxTextAttr HexDefaultAttr;
		wxMutex MutexResize;
		//wxTextAttr HexSelectAttr;

		wxSize  m_CharSize;	// size (in pixels) of one character
	   // DECLARE_DYNAMIC_CLASS(wxHexCtrl)
	};

class wxHexTextCtrl : public wxHexCtrl{
	public:
		wxString CodepageTable;
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

				wxString cp;
				wxConfigBase::Get()->Read( _T("CharacterEncoding"), &cp, wxT("OEM") );
				PrepareCodepageTable(cp);
				};
		wxString PrepareCodepageTable(wxString);
inline bool IsDenied(){ return false; }
inline bool IsDenied( int x ){ return false; }
inline int CharacterPerLine( void ){ return m_Window.x; }
inline int BytePerLine( void ){ return CharacterPerLine(); }
inline int GetByteCount( void ){ return m_text.Length(); }
		void Replace(unsigned text_location, const wxChar& value, bool paint);
		void ChangeValue( const wxString& value, bool paint );
		void SetBinValue( char* buffer, int len, bool paint );
		void SetDefaultStyle( wxTextAttr& new_attr );		//For caret diet (to 1 pixel)
		int PixelCoordToInternalPosition( wxPoint mouse );
		int ToVisiblePosition( int InternalPosition ){ return InternalPosition; }
		int ToInternalPosition( int VisiblePosition ){ return VisiblePosition; }
//		bool IsAllowedChar(const unsigned char& chr);
		int GetInsertionPoint( void );
		void SetInsertionPoint( unsigned int pos );
		void ChangeSize();
		wxChar Filter(const unsigned char& chr);
		wxString FilterUTF8(const char *str, int len);
inline void DrawCursorShadow(wxDC* dcTemp);
	};

class wxHexOffsetCtrl : public wxHexCtrl{
	public:
//		wxHexOffsetCtrl():wxHexCtrl(){ hex_offset=false;offset_position=0; }
		wxHexOffsetCtrl(wxWindow *parent): wxHexCtrl( parent ){
			offset_mode='u';
			offset_position=0;
			digit_count=6;
			}

		wxHexOffsetCtrl(wxWindow *parent,
				wxWindowID id,
				const wxString &value = wxEmptyString,
				const wxPoint &pos = wxDefaultPosition,
				const wxSize &size = wxDefaultSize,
				long style = 0,
				const wxValidator& validator = wxDefaultValidator) :
				wxHexCtrl(parent, id, value, pos, size, style, validator){
				wxCaret *caret = GetCaret();
				if(caret)
					GetCaret()->Hide();
				SetCaret( NULL );

            //offset_mode='u';
            offset_mode=wxConfigBase::Get()->Read( _T("LastOffsetMode"), wxT("u") )[0];
            if( offset_mode=='s' )	// No force to sector mode at startup.
					offset_mode='u';

				offset_position=0;
				digit_count=6;
				};
		wxString GetFormatString( bool minimal=false );
      wxString GetFormatedOffsetString( uint64_t c_offset, bool minimal=false );
		void SetOffsetLimit( uint64_t max_offset ){offset_limit = max_offset;}
		unsigned GetDigitCount( void );
		unsigned GetLineSize( void );  //Digit count plus Formating chars like h,o if required
inline bool IsDenied(){ return false; }
inline bool IsDenied( int x ){ return false; }
		int ToVisiblePosition( int InternalPosition ){ return InternalPosition; }
		int ToInternalPosition( int VisiblePosition ){ return VisiblePosition; }
		void SetValue( uint64_t position );
		void SetValue( uint64_t position, int byteperline );
		void OnMouseRight( wxMouseEvent& event );
		void OnMouseLeft( wxMouseEvent& event );
		void OnMouseMove( wxMouseEvent& event ){event.Skip(false);}
		char offset_mode;
		uint64_t offset_position;
		int BytePerLine;
		int sector_size;

	private:
		uint64_t offset_limit;
		unsigned digit_count;
inline void DrawCursorShadow(wxDC* dcTemp){}
	};
#endif
