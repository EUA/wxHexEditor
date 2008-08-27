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


#include "wxHexCtrl.h"

#include "wx/caret.h"
#include "wx/numdlg.h"
//#define X 0

BEGIN_EVENT_TABLE(wxHexCtrl,wxScrolledWindow )
	EVT_CHAR( wxHexCtrl::OnChar )
	EVT_SIZE( wxHexCtrl::OnResize )
    EVT_PAINT(wxHexCtrl::OnPaint )
    //EVT_MOUSE( wxHexCtrl::OnResize)
    EVT_RIGHT_DOWN( wxHexCtrl::OnMouseRight )
    EVT_SET_FOCUS( wxHexCtrl::OnFocus )
    EVT_KILL_FOCUS( wxHexCtrl::OnKillFocus )
END_EVENT_TABLE()

//IMPLEMENT_DYNAMIC_CLASS(wxHexCtrl, wxScrolledWindow)

wxHexCtrl::wxHexCtrl(wxWindow *parent,
			wxWindowID id,
			const wxString &value,
			const wxPoint &pos,
			const wxSize &size,
			long style,
			const wxValidator& validator)
			: wxScrolledWindow( parent, id,
								pos, size,
								wxSUNKEN_BORDER )
{
    m_text = (wxChar *)NULL;

	HexDefaultAttr = wxTextAttr(( *wxBLACK ),
								wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ),
								wxFont(10, wxMODERN, wxNORMAL, wxNORMAL, 0, wxT("") ));

	HexSelectAttr  = wxTextAttr(wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ),
								wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ),
								wxFont(10, wxMODERN, wxNORMAL, wxNORMAL, 0, wxT("") ));

	//wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION)
	//HexAttr.SetFont(wxFont(10, wxMODERN, wxNORMAL, wxNORMAL, 0, wxT("")));
//	SetBackgroundStyle( wxBG_STYLE_CUSTOM );
//  SetBackgroundColour( *wxGREEN );
    ClearSelection();
    SetDefaultStyle( HexDefaultAttr );
    m_Caret.x = m_Caret.y =
    m_Window.x = m_Window.y = 1;
    m_Margin.x = m_Margin.x = 0;
    select.start = select.end = 0;
    select.selected = false;
    CreateCaret();
    ChangeSize();

    wxCaret *caret = GetCaret();
    if ( caret )
		caret->Show(false);
}
wxHexCtrl::~wxHexCtrl()
{
    m_text.Clear();
    wxCaretSuspend cs(this);
    wxBufferedPaintDC dc( this );
    PrepareDC( dc );
    dc.SetFont( HexDefaultAttr.GetFont() );
	dc.SetTextForeground( HexDefaultAttr.GetTextColour() );
    dc.SetTextBackground( HexDefaultAttr.GetBackgroundColour() );
	wxBrush bbrush( HexDefaultAttr.GetBackgroundColour() );
    dc.SetBackground(bbrush );
    dc.Clear();

}

void wxHexCtrl::Clear( bool RePaint, bool cursor_reset ){
	m_text.Clear();
	if( cursor_reset )
		SetInsertionPoint(0);
	ClearSelection( RePaint );
	}

void wxHexCtrl::CreateCaret(){
    wxCaret *caret = new wxCaret(this, m_Char.x, m_Char.y);
    SetCaret(caret);
    caret->Move(m_Margin.x, m_Margin.x);
    caret->Show();
	}

void wxHexCtrl::FirstLine( bool MoveCaret ){
	m_Caret.y = 0;
	if(MoveCaret)
		DoMoveCaret();
	}

void wxHexCtrl::LastLine( bool MoveCaret ){
	m_Caret.y = m_Window.y - 1;
	if ( IsDenied() )
		NextChar( false );
	if ( GetInsertionPoint() > GetLastPosition() ){
		wxBell();
		SetInsertionPoint( GetLastPosition() );
		}
	if(MoveCaret)
		DoMoveCaret();
	}

void wxHexCtrl::Home( bool MoveCaret ){
	m_Caret.x = 0;
	if( MoveCaret )
		DoMoveCaret();
	}

void wxHexCtrl::End( bool MoveCaret ){
	m_Caret.x = m_Window.x - 1;
	if ( IsDenied() )
		PrevChar( false );
	if ( GetInsertionPoint() > GetLastPosition() ){
		wxBell();
		SetInsertionPoint( GetLastPosition() );
		}
	if(MoveCaret)
		DoMoveCaret();
	}

void wxHexCtrl::PrevChar( bool MoveCaret ){
	if ( !m_Caret.x-- ){
		End( false );
		PrevLine( false );
		}
	if ( IsDenied() )
		PrevChar( false );
	if( MoveCaret )
		DoMoveCaret();
	}

void wxHexCtrl::NextChar( bool MoveCaret ) {
	if ( ++m_Caret.x == m_Window.x ){
		Home( false );
		NextLine( false );
		}
	else if ( IsDenied() )
		NextChar( false );
	else if ( GetInsertionPoint() > GetLastPosition() ){
		wxBell();
		SetInsertionPoint( GetLastPosition() );
		}
	if( MoveCaret )
		DoMoveCaret();
	}

void wxHexCtrl::PrevLine( bool MoveCaret ){
	if ( !m_Caret.y-- ){
		m_Caret.y++;
		Home( false );
		wxBell();
		}
	if( MoveCaret )
		DoMoveCaret();
	}

void wxHexCtrl::NextLine( bool MoveCaret ){
	if ( ++m_Caret.y == m_Window.y ) {
		m_Caret.y--;
		End( false );
		wxBell();
		}
	else if ( GetInsertionPoint() > GetLastPosition() ){
		wxBell();
		SetInsertionPoint( GetLastPosition() );
		}
	if( MoveCaret )
		DoMoveCaret();
	}

bool inline wxHexCtrl::IsDenied( int x ){	// State Of The Art :) Hex plotter function by idents avoiding some X axes :)
//		x%=m_Window.x;						// Discarding y axis noise
	if( ( ( m_Window.x - 1 ) % 3 == 0 )		// For avoid hex divorcings
		&& ( x == m_Window.x - 1 ))
		return true;
	//if( x == 3*8 )
	//	return true;
	return !( ( x + 1 ) % 3 );				// Byte coupling
	}

int wxHexCtrl::xCountDenied( int x ){
	for( int i = 0, denied = 0 ; i <  m_Window.x ; i++ ){
		if( IsDenied(i) )
			denied++;
		if( i == x )
			return denied;
		}
	return -1;
	}

int wxHexCtrl::CharacterPerLine( void ){
	int avoid=0;
	for ( int x = 0 ; x < m_Window.x ; x++)
		if ( IsDenied(x) )
			avoid++;
	return  ( m_Window.x - avoid );
	}

int wxHexCtrl::GetInsertionPoint( void ){
	return ( m_Caret.x - xCountDenied(m_Caret.x) ) + CharacterPerLine() * m_Caret.y;
	}

void wxHexCtrl::SetInsertionPoint( unsigned int pos ){
	if(pos > m_text.Length())
		pos = m_text.Length();
	pos = ToExactPosition(pos);
	MoveCaret( pos%m_Window.x , pos/m_Window.x );
	}

int wxHexCtrl::ToExactPosition( int InternalPosition ){
	if( CharacterPerLine() == 0 ) return 0;
	int y = InternalPosition / CharacterPerLine();
	int x = InternalPosition - y * CharacterPerLine();
	for( int i = 0, denied = 0 ; i < m_Window.x ; i++ ){
		if( IsDenied(i) ) denied++;
		if( i - denied == x )
			return ( i + y * m_Window.x );
		}
	wxLogError(wxString::Format(_T("Fatal error at fx ToExactPosition(%d)"),InternalPosition));
	return 0;
	}

int wxHexCtrl::ToInternalPosition( int ExactPosition ){
	int y = ExactPosition / m_Window.x;
	int x = ExactPosition - y * m_Window.x;
	return ( x - xCountDenied(x) + y * CharacterPerLine() );
	}

wxPoint wxHexCtrl::InternalPositionToExactCoord( int position ){
	if( position < 0 )
		wxLogError(wxString::Format(_T("Fatal error at fx InternalPositionToExactCoord(%d)"),position));
	int x = m_Window.x? m_Window.x : 1;	//prevents divide zero error;
	int pos = ToExactPosition( position );
	return wxPoint( pos - (pos / x) * x, pos / x );
	}

int wxHexCtrl::PixelCoordToInternalPosition( wxPoint mouse ){
	mouse = PixelCoordToInternalCoord( mouse );
	return ( mouse.x - xCountDenied(mouse.x) + mouse.y * CharacterPerLine() );
	}

wxPoint wxHexCtrl::PixelCoordToInternalCoord( wxPoint mouse ){
	mouse.x = ( mouse.x < 0 ? 0 : mouse.x);
	mouse.x = ( mouse.x > m_Char.x*m_Window.x ? m_Char.x*m_Window.x-1 : mouse.x);
	mouse.y = ( mouse.y < 0 ? 0 : mouse.y);
	mouse.y = ( mouse.y > m_Char.y*m_Window.y ? m_Char.y*m_Window.y-1 : mouse.y);
	int x = (mouse.x - m_Margin.x) / m_Char.x;
	int y = (mouse.y - m_Margin.y) / m_Char.y;
	return wxPoint(x,y);
	}

void wxHexCtrl::SetDefaultStyle( wxTextAttr& new_attr ){
	HexDefaultAttr = new_attr;

    wxClientDC dc(this);
    dc.SetFont( HexDefaultAttr.GetFont() );
    SetFont( HexDefaultAttr.GetFont() );
    m_Char.y = dc.GetCharHeight();
    m_Char.x = dc.GetCharWidth();

    wxCaret *caret = GetCaret();
    if ( caret )
		caret->SetSize(m_Char.x, m_Char.y);
	RePaint();
}

void wxHexCtrl::SetSelectionStyle( wxTextAttr& new_attr ){
	HexSelectAttr = new_attr;
	}

void wxHexCtrl::SetSelection( unsigned start, unsigned end ){
	select.start = start;
	select.end = end;
	select.selected = true;
	RePaint();
	}

void wxHexCtrl::ClearSelection( bool repaint ){
	select.start = 0;
	select.end = 0;
	select.selected = false;
	if( repaint )
		RePaint();
	}



/*
void wxHexCtrl::SetStyle(unsigned start, unsigned end, wxTextAttr& new_attr ){
	wxCaretSuspend cs(this);
	if(1){
		wxBitmap bmp(this->GetSize().GetWidth(), this->GetSize().GetHeight());
		wxMemoryDC dcTemp;
		dcTemp.SelectObject(bmp);
		dcTemp.SetFont( HexDefaultAttr.GetFont() );
		dcTemp.SetTextForeground( HexDefaultAttr.GetTextColour() );
		dcTemp.SetTextBackground( HexDefaultAttr.GetBackgroundColour() );
		wxBrush dbrush( HexDefaultAttr.GetBackgroundColour() );
		dcTemp.SetBackground(dbrush );
		dcTemp.Clear();

		wxString line;
		unsigned int z = 0;
		for ( int y = 0 ; y < m_Window.y; y++ ){
			line.Empty();
			for ( int x = 0 ; x < m_Window.x; x++ ){
				if( IsDenied(x)){
					line += wxT(' ');
					continue;
					}
				if(z > m_text.Length())
					break;
				wxChar ch = CharAt(z++);
				line += ch;
				}
			dcTemp.DrawText( line, m_Margin.x, m_Margin.x + y * m_Char.y );
			}

		dcTemp.SetFont( new_attr.GetFont() );
		dcTemp.SetTextForeground( new_attr.GetTextColour() );
		dcTemp.SetTextBackground( new_attr.GetBackgroundColour() );
		wxBrush sbrush( new_attr.GetBackgroundColour() );
		dcTemp.SetBackground( sbrush );
		dcTemp.SetBackgroundMode( wxSOLID ); // overwrite old value
		m_Char.y = dcTemp.GetCharHeight();
		m_Char.x = dcTemp.GetCharWidth();

		if( start > end ){
			int temp = end;
			end = start;
			start = temp;
			}

		if( end > m_text.Length() )
			end = m_text.Length();

		wxPoint _start_ = InternalPositionToExactCoord( start );
		wxPoint _end_	= InternalPositionToExactCoord( end );
		wxPoint _temp_	= _start_;
		for ( ; _temp_.y <= _end_.y ; _temp_.y++ ){
			wxString line;
			_temp_.x = ( _temp_.y == _start_.y ) ? _start_.x : 0;	//calculating local line start
			int z = ( _temp_.y == _end_.y ) ? _end_.x : m_Window.x;	// and end point
			for ( int x = _temp_.x; x < z; x++ ){					//Prepare line to write process
				if( IsDenied(x) ){
					if(x+1 < z)
						line += wxT(' ');
					continue;
					}
				wxChar ch = CharAt(start++);
				line += ch;
				}
			dcTemp.DrawText( line, m_Margin.x + _temp_.x * m_Char.x,	//Write prepared line
								   m_Margin.x + _temp_.y * m_Char.y );
			}

		wxPaintDC dc( this );
		PrepareDC( dc );
		dc.Blit(0, 0, this->GetSize().GetWidth(), this->GetSize().GetHeight(), &dcTemp, 0, 0, wxCOPY);
		}

	if(0){	//Alternate way
		wxBitmap bmp(this->GetSize().GetWidth(), this->GetSize().GetHeight());
		wxMemoryDC dcTemp;
		dcTemp.SelectObject(bmp);
	//	DoPrepareDC(dcTemp);
		dcTemp.SetFont( new_attr.GetFont() );
		dcTemp.SetTextForeground( new_attr.GetTextColour() );
		dcTemp.SetTextBackground( new_attr.GetBackgroundColour() );
		wxBrush bbrush( new_attr.GetBackgroundColour() );
		dcTemp.SetBackground( bbrush );
		dcTemp.SetBackgroundMode( wxSOLID ); // overwrite old value
		m_Char.y = dcTemp.GetCharHeight();
		m_Char.x = dcTemp.GetCharWidth();
		dcTemp.Clear();

		if( start > end ){
			int temp = end;
			end = start;
			start = temp;
			}

		if( end > m_text.Length() )
			end = m_text.Length();

		wxPoint _start_ = InternalPositionToExactCoord( start );
		wxPoint _end_	= InternalPositionToExactCoord( end );
		wxPoint _temp_	= _start_;
		for ( ; _temp_.y <= _end_.y ; _temp_.y++ ){
			wxString line;
			_temp_.x = ( _temp_.y == _start_.y ) ? _start_.x : 0;	//calculating local line start
			int z = ( _temp_.y == _end_.y ) ? _end_.x : m_Window.x;	// and end point
			for ( int x = _temp_.x; x < z; x++ ){					//Prepare line to write process
				if( IsDenied(x) ){
					if(x+1 < z)
						line += wxT(' ');
					continue;
					}
				wxChar ch = CharAt(start++);
				line += ch;
				}
			dcTemp.DrawText( line, _temp_.x * m_Char.x,	//Write prepared line
								   _temp_.y * m_Char.y );
			}

		wxPaintDC dc( this );
		//First Line
		int x = ((( _start_.y == _end_.y ) ? _end_.x - ( end % 2 ? 0:1 ): m_Window.x) - _start_.x );
		dc.Blit(m_Margin.x + _start_.x*m_Char.x, m_Margin.x + _start_.y*m_Char.y,
				m_Char.x*x, m_Char.y,
				&dcTemp,
				_start_.x*m_Char.x,_start_.y*m_Char.y,
				wxCOPY);

		//Detects and draw if there is a middle block available
		if( _start_.y < _end_.y - 1 ){
			int y = _end_.y - 1 - _start_.y;
			dc.Blit(m_Margin.x, m_Margin.x + (_start_.y+1)*m_Char.y,
					m_Window.x*m_Char.x, y*m_Char.y,
					&dcTemp,
					0, (_start_.y+1)*m_Char.y,
					wxCOPY);
					}
		//Last Line
		if( _start_.y != _end_.y  ){
			int x = _end_.x  - ( end % 2 ? 0:1 );
			dc.Blit(m_Margin.x, m_Margin.x + (_end_.y)*m_Char.y,
					x*m_Char.x, m_Char.y,
					&dcTemp,
					0, (_end_.y)*m_Char.y,
					wxCOPY);
					}
		}
	if(0){
		wxBufferedPaintDC dc( this );
		//DoPrepareDC( dc );
		dc.SetFont( new_attr.GetFont() );
		dc.SetTextForeground( new_attr.GetTextColour() );
		dc.SetTextBackground( new_attr.GetBackgroundColour() );
		wxBrush bbrush( new_attr.GetBackgroundColour() );
		dc.SetBackground( bbrush );
		dc.SetBackgroundMode( wxSOLID ); // overwrite old value
		m_Char.y = dc.GetCharHeight();
		m_Char.x = dc.GetCharWidth();

		if( start > end ){
			int temp = end;
			end = start;
			start = temp;
			}

		if( end > m_text.Length() )
			end = m_text.Length();

		wxPoint _start_ = InternalPositionToExactCoord( start );
		wxPoint _end_	= InternalPositionToExactCoord( end );
		wxPoint _temp_	= _start_;
		for ( ; _temp_.y <= _end_.y ; _temp_.y++ ){
			wxString line;
			_temp_.x = ( _temp_.y == _start_.y ) ? _start_.x : 0;	//calculating local line start
			int z = ( _temp_.y == _end_.y ) ? _end_.x : m_Window.x;	// and end point
			for ( int x = _temp_.x; x < z; x++ ){					//Prepare line to write process
				if( IsDenied(x) ){
					if(x+1 < z)
						line += wxT(' ');
					continue;
					}
				wxChar ch = CharAt(start++);
				line += ch;
				}
			dc.DrawText( line,	m_Margin.x + _temp_.x * m_Char.x,	//Write prepared line
								m_Margin.x + _temp_.y * m_Char.y );
			}
		}

	frost.Unlock();
	}
*/

void wxHexCtrl::MoveCaret(int x, int y){
    m_Caret.x = x;
	m_Caret.y = y;
    DoMoveCaret();
}

void wxHexCtrl::MoveCaret(int x){
	m_Caret.y = x/CharacterPerLine();
    m_Caret.x = x - m_Caret.y*CharacterPerLine();
    DoMoveCaret();
}

void wxHexCtrl::DoMoveCaret(){
    wxCaret *caret = GetCaret();
    if ( caret )
		caret->Move(m_Margin.x + m_Caret.x * m_Char.x,
                    m_Margin.x + m_Caret.y * m_Char.y);
}

// NB: this method is horrible inefficient! But required!

void wxHexCtrl::OnPaint( wxPaintEvent &WXUNUSED(event) ){
    wxCaretSuspend cs(this);

	if(1){	//Hand made buffer code - check performance with alternative
		wxBitmap bmp(this->GetSize().GetWidth(), this->GetSize().GetHeight());
		wxMemoryDC dcTemp;
		dcTemp.SelectObject(bmp);
		dcTemp.SetFont( HexDefaultAttr.GetFont() );
		dcTemp.SetTextForeground( HexDefaultAttr.GetTextColour() );
		dcTemp.SetTextBackground( HexDefaultAttr.GetBackgroundColour() );
		wxBrush dbrush( HexDefaultAttr.GetBackgroundColour() );
		dcTemp.SetBackground(dbrush );
		dcTemp.Clear();

		wxString line;
		unsigned int z = 0;
		for ( int y = 0 ; y < m_Window.y; y++ ){
			line.Empty();
			for ( int x = 0 ; x < m_Window.x; x++ ){
				if( IsDenied(x)){
					line += wxT(' ');
					continue;
					}
				if(z > m_text.Length())
					break;
				wxChar ch = CharAt(z++);
				line += ch;
				}
			dcTemp.DrawText( line, m_Margin.x, m_Margin.x + y * m_Char.y );
			}

		if(select.selected){	//Selection Painter
			dcTemp.SetFont( HexSelectAttr.GetFont() );
			dcTemp.SetTextForeground( HexSelectAttr.GetTextColour() );
			dcTemp.SetTextBackground( HexSelectAttr.GetBackgroundColour() );
			wxBrush sbrush( HexSelectAttr.GetBackgroundColour() );
			dcTemp.SetBackground( sbrush );
			dcTemp.SetBackgroundMode( wxSOLID ); // overwrite old value
			m_Char.y = dcTemp.GetCharHeight();
			m_Char.x = dcTemp.GetCharWidth();

			int start = select.start;
			int end = select.end;

			if( start > end ){
				int temp = end;
				end = start;
				start = temp;
				}

			if( start < 0 )
				start = 0;
			if ( end > ByteCapacity()*2)
				 end = ByteCapacity()*2;

			wxPoint _start_ = InternalPositionToExactCoord( start );
			wxPoint _end_	= InternalPositionToExactCoord( end );
			wxPoint _temp_	= _start_;
			for ( ; _temp_.y <= _end_.y ; _temp_.y++ ){
				wxString line;
				_temp_.x = ( _temp_.y == _start_.y ) ? _start_.x : 0;	//calculating local line start
				int z = ( _temp_.y == _end_.y ) ? _end_.x : m_Window.x;	// and end point
				for ( int x = _temp_.x; x < z; x++ ){					//Prepare line to write process
					if( IsDenied(x) ){
						if(x+1 < z)
							line += wxT(' ');
						continue;
						}
					wxChar ch = CharAt(start++);
					line += ch;
					}
				dcTemp.DrawText( line, m_Margin.x + _temp_.x * m_Char.x,	//Write prepared line
									   m_Margin.x + _temp_.y * m_Char.y );
				}
			}
		wxPaintDC dc( this );
		PrepareDC( dc );
		dc.Blit(0, 0, this->GetSize().GetWidth(), this->GetSize().GetHeight(), &dcTemp, 0, 0, wxCOPY);
		}

	if(0){	//Alternate code
		wxBufferedPaintDC bdc(this);
		bdc.SetFont( HexDefaultAttr.GetFont() );
		bdc.SetTextForeground( HexSelectAttr.GetTextColour() );
		bdc.SetTextBackground( HexSelectAttr.GetBackgroundColour() );
		wxBrush bbrush( HexSelectAttr.GetBackgroundColour() );
		bdc.SetBackground(bbrush );
		bdc.Clear();

		wxString line;
		unsigned int z = 0;
		for ( int y = 0 ; y < m_Window.y; y++ ){
			line.Empty();
			for ( int x = 0 ; x < m_Window.x; x++ ){
				if( IsDenied(x)){
					line += wxT(' ');
					continue;
					}
				if(z > m_text.Length())
					break;
				wxChar ch = CharAt(z++);
				line += ch;
			}
			bdc.DrawText( line, m_Margin.x, m_Margin.x + y * m_Char.y );
			}
		}
	}
bool wxHexCtrl::IsAllowedChar(char chr){
	return isxdigit( chr );
	}

bool wxHexTextCtrl::IsAllowedChar(char chr){
	return ((chr !=173) && (chr>31 && chr<127 || chr>159));
	}

void wxHexCtrl::OnChar( wxKeyEvent &event ){
	switch (event.GetKeyCode()){
		case WXK_LEFT:case WXK_NUMPAD_LEFT:			PrevChar();			break;
		case WXK_RIGHT:case WXK_NUMPAD_RIGHT:		NextChar();			break;
		case WXK_UP:case WXK_NUMPAD_UP:				PrevLine();			break;
		case WXK_DOWN:case WXK_NUMPAD_DOWN:			NextLine();			break;
		case WXK_HOME:case WXK_NUMPAD_HOME:			Home();				break;
		case WXK_END:case WXK_NUMPAD_END:			End();				break;
		case WXK_RETURN:			Home( false );	NextLine();			break;
		case WXK_PAGEUP:case WXK_NUMPAD_PAGEUP:
		case WXK_PAGEDOWN:case WXK_NUMPAD_PAGEDOWN:	break;
		case WXK_DELETE:case WXK_NUMPAD_DELETE:
			if( !IsDenied() )
				Replace(GetInsertionPoint(),'0');
			else
				wxBell();
			break;
		case WXK_BACK:
			if( GetInsertionPoint()!=0 ){
				PrevChar();
				if( !IsDenied() )
					Replace(GetInsertionPoint(),'0');
				}
			else
				wxBell();
			break;
// TODO (death#3#): CTRL+X
		default:
			wxChar chr = event.GetKeyCode();
			if( IsAllowedChar(chr) && !event.AltDown() && !event.ShiftDown() && !IsDenied() ){
		// TODO (death#1#): if text selected, enter from beggining!
		// TODO (death#2#): If text Selected, than  remove selection first?
				select.selected=false;
				(chr>='a'&&chr<='z')?(chr-=('a'-'A')):(chr=chr);	//Upper() for Char
				WriteHex(chr);
				//CharAt(m_Caret.x, m_Caret.y) = ch;

/*				wxCaretSuspend cs(this);
                wxClientDC dc(this);
                dc.SetFont(m_font);
                dc.SetBackgroundMode(wxSOLID); // overwrite old value
                dc.DrawText(chr, m_Margin.x + m_Caret.x * m_Char.x,
                                m_Margin.x + m_Caret.y * m_Char.y );
                NextChar();
*/
				}
			else
				wxBell();
				event.Skip();
			break;

	}//switch end
//	wxYield();
//	hex_selector(event);
//	paint_selection();
	DoMoveCaret();
    }

void wxHexCtrl::ChangeSize(){
	unsigned gip = GetInsertionPoint();
    wxSize size = GetClientSize();
    m_Window.x = (size.x - 2*m_Margin.x) / m_Char.x;
    m_Window.y = (size.y - 2*m_Margin.x) / m_Char.y;
    if ( m_Window.x < 1 )
		m_Window.x = 1;
    if ( m_Window.y < 1 )
		m_Window.y = 1;

    //m_text.Empty();
	RePaint();
    SetInsertionPoint( gip );

#if wxUSE_STATUSBAR
    wxFrame *frame = wxDynamicCast(GetParent(), wxFrame);

    if ( frame && frame->GetStatusBar() ){
    	wxString msg;
        msg.Printf(_T("Panel size is (%d, %d)"), m_Window.x, m_Window.y);
        frame->SetStatusText(msg, 1);
    }
#endif // wxUSE_STATUSBAR
}

void wxHexCtrl::WriteHex( const wxString& value ){
// TODO (death#4#): Need Insertion?
	Replace(GetInsertionPoint(), GetInsertionPoint()+value.Length(), value);
	}

void wxHexCtrl::WriteByte( const unsigned char& byte ){
	unsigned byte_location = GetInsertionPoint()/2;
	wxString buffer;
	buffer << (byte >> 4);
	buffer << (byte & 0x0F);
	Replace( byte_location*2,byte_location*2+2,buffer );
	}

void wxHexCtrl::Replace(unsigned hex_location, const wxChar& value, bool paint){
	if( hex_location < m_text.Length() )
		m_text[hex_location] = value;
	else{
		m_text << value;
		m_text << wxT("0");
		}
	RePaint();
	}

void wxHexCtrl::Replace(unsigned from, unsigned to, const wxString& value){
	if( from >= to ) return;
// TODO (death#4#): IsHEX?
	if (!value.IsEmpty()){
// TODO (death#4#): Optimization available with use direct buffer copy
		for( int i = 0; static_cast<unsigned>(i) < value.Length() && from < to ; i++,from++ ){
			Replace( from, value[i], false );
/*			if( GetByteCount() <= from+i )	//add new hex char
				m_text << value[i];
			else						//replace old hex
				m_text[from+i] = value[i];
*/
			}
		SetInsertionPoint( to );
/*				wxCaretSuspend cs(this);
                wxClientDC dc(this);
                dc.SetFont(m_font);
                dc.SetBackgroundMode(wxSOLID); // overwrite old value
                dc.DrawText(chr, m_Margin.x + m_Caret.x * m_Char.x,
                                m_Margin.x + m_Caret.y * m_Char.y );
                NextChar();
*/
		RePaint();
		}
	else
		wxBell();
	}

void wxHexCtrl::WriteBytes( char* buffer, int byte_count, bool repaint ){
	m_text.Clear();
	for( int i=0 ; i < byte_count ; i++ )
		m_text += wxString::Format(_("%02X"), static_cast<unsigned char>(buffer[i]));
	if(repaint)
		RePaint();
	}

char wxHexCtrl::ReadByte( int byte_location ){
	wxString hx;
	hx << m_text[ byte_location*2 ] << m_text[ byte_location*2+1 ];
	return *HexToChar(hx);
	}

//long wxHexCtrl::ReadBytes(unsigned char* buffer,long start_location, long byte_count){
//	wxString hex_string = GetRange(start_location*3,(start_location+byte_count)*3);
//ALPHA oglu APLHA CODE
//	return i;
//    }

char* wxHexCtrl::HexToChar(const wxString& HexValue){
	char *buffer = new char[HexValue.Length()/3+2];//+1 for "\0"

	for(unsigned int i=0,j=0 ; j < HexValue.Length()/3+1 ; i+=2,j++){
		if(HexValue[i]>='0' && HexValue[i]<='9')
			buffer[j] = HexValue[i] - '0';
		else if(HexValue[i]>='A' && HexValue[i]<='F')
			buffer[j] = HexValue[i] - 'A' + 10;
		else if(HexValue[i]>='a' && HexValue[i]<='f')
			buffer[j] = HexValue[i] - 'a' + 10;
		else if(HexValue[i] == ' '){	//Removes space char
			i--;
			continue;
			}
		else{
			wxBell();
			return NULL;
			}

		buffer[j] <<= 4;

		if(HexValue[i+1]>='0' && HexValue[i+1]<='9')
			buffer[j] |= HexValue[i+1] - '0';
		else if(HexValue[i+1]>='A' && HexValue[i+1]<='F')
			buffer[j] |= HexValue[i+1] - 'A' + 10;
		else if(HexValue[i+1]>='a' && HexValue[i+1]<='f')
			buffer[j] |= HexValue[i+1] - 'a'+ 10;
		else		// There is no space char remove code here, Hex codes must be sticked together.
			{
			wxBell();
			return NULL;
			}
		buffer[j+1]='\0';
		}
	return buffer;
	}

void wxHexCtrl::OnFocus(wxFocusEvent& event ){
	wxCaret *caret = GetCaret();
    if ( caret )
		caret->Show(true);
	}
void wxHexCtrl::OnKillFocus(wxFocusEvent& event ){
	wxCaret *caret = GetCaret();
    if ( caret )
		caret->Show(false);
	}

void wxHexCtrl::OnResize( wxSizeEvent &event ){
	ChangeSize();
    event.Skip();
	}

void wxHexCtrl::OnMouseRight( wxMouseEvent& event ){
	wxBell();

#if wxUSE_STATUSBAR
	event.Skip(true);

    wxFrame *frame = wxDynamicCast(GetParent(), wxFrame);
    static int i=39;
    wxString msg;
    if ( frame && frame->GetStatusBar() )
    switch(1){
    	case 0:{	// ToExactPosition & ToInternalPosition fx manual debug case
			if( i++ > 50 )
				i = 0;
			msg.Empty();
			msg.Printf(_T("ToExactPosition(%d)=%d"), i, ToExactPosition(i));
			frame->SetStatusText(msg, 0);
			msg.Empty();
			msg.Printf(_T("ToInternalPosition(%d)=%d"), i, ToInternalPosition(i) );
			frame->SetStatusText(msg, 1);
			break;
			}
		case 1:{	// ToExactPosition & ToInternalPosition fx manual test case
			for(i = 0 ; i < 100 ; i++){
				int denied = 0;
				while( IsDenied(i-denied) )
					denied++;
				if( i-denied != ToExactPosition(ToInternalPosition(i))){
					msg.Empty();
					msg.Printf(_T("To(Exact/Internal)Position fx are false at: %d"), i);
					frame->SetStatusText(msg, 0);
					break;
					}
				}
			msg.Empty();
			msg.Printf(_T("To(Exact/Internal)Position fx are false at: %d"), i);
			frame->SetStatusText(msg, 0);
			break;
			}
		case 2:{	// SetInsertionPoint & GetInsertionPoint fx manual debug case
			if( ++i > GetLastPosition() )
				i = 0;
			SetInsertionPoint(i);
			DoMoveCaret();
			msg.Empty();
			msg.Printf(_T("SetInsertionPoint: %d"), i);
			frame->SetStatusText(msg, 0);
			msg.Empty();
			msg.Printf(_T("GetInsertionPoint: %d"), GetInsertionPoint() );
			frame->SetStatusText(msg, 1);
			break;
			}
		case 3:{	// SetInsertionPoint & GetInsertionPoint fx manual test case
			for( i = 0 ; i < 100 ; i++){
				SetInsertionPoint(i);
				if( GetLastPosition() != i ){
					msg.Empty();
					msg.Printf(_T("(G/S)etInsertionPoint False At: %d"), i);
					frame->SetStatusText(msg, 0);
					break;
					}
				}
				msg.Empty();
				msg.Printf(_T("(G/S)etInsertionPoint True"), i);
				frame->SetStatusText(msg, 0);
			break;
			}
		case 4:{
			char x = 0;
			WriteByte(x);
			msg.Empty();
			msg << _("ReadByte/WriteByte: ");
			if( x == ReadByte(0) )
				msg << _("OK");
			else
				msg << _("FAILED");
			frame->SetStatusText(msg, 0);
			break;
			}
		case 5:{
			//SetStyle(4,5,HexSelectAttr);
			break;
			}
		case 6:{
			char x[] = "0123456789000000";
			WriteBytes(x,16);
			/*
			msg.Empty();
			msg << _("ReadByte/WriteByte: ");
			if( x == ReadByte(0) )
				msg << _("OK");
			else
				msg << _("FAILED");
			frame->SetStatusText(msg, 0);
			*/
			break;
			}
		}
#endif // wxUSE_STATUSBAR
	}

void wxHexTextCtrl::Replace(unsigned text_location, const wxChar& value, bool paint){
	if( text_location < m_text.Length() )
		m_text[text_location] = value;
	else{
		m_text << value;
		//m_text << wxT("0");
		}
	RePaint();
	}

void wxHexTextCtrl::ChangeValue( const wxString& value, bool paint ){
	m_text = value;
	if( paint )
		RePaint();
	}

void wxHexTextCtrl::SetDefaultStyle( wxTextAttr& new_attr ){
	HexDefaultAttr = new_attr;

    wxClientDC dc(this);
    dc.SetFont( HexDefaultAttr.GetFont() );
    SetFont( HexDefaultAttr.GetFont() );
    m_Char.y = dc.GetCharHeight();
    m_Char.x = dc.GetCharWidth();

    wxCaret *caret = GetCaret();
    if ( caret )
		caret->SetSize(1, m_Char.y);
	RePaint();
}

int wxHexTextCtrl::PixelCoordToInternalPosition( wxPoint mouse ){
	mouse.x = ( mouse.x < 0 ? 0 : mouse.x);
	mouse.x = ( mouse.x > m_Char.x*m_Window.x ? m_Char.x*m_Window.x-1 : mouse.x);
	mouse.y = ( mouse.y < 0 ? 0 : mouse.y);
	mouse.y = ( mouse.y > m_Char.y*m_Window.y ? m_Char.y*m_Window.y-1 : mouse.y);

	int x = (mouse.x - m_Margin.x + m_Char.x/2) / m_Char.x;
	int y = (mouse.y - m_Margin.y             ) / m_Char.y;
	return ( x + y * CharacterPerLine() );
	}

void wxHexOffsetCtrl::ChangeValue( const wxString& value, bool paint ){
	m_text = value;
	if( paint )
		RePaint();
	}
