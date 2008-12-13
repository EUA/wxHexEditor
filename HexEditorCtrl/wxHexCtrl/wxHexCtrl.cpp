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
    EVT_LEFT_DOWN( wxHexCtrl::OnMouseLeft )
    EVT_LEFT_DOWN( wxHexOffsetCtrl::OnMouseLeft )
    //EVT_MOUSE( wxHexCtrl::OnResize)
    EVT_RIGHT_DOWN( wxHexCtrl::OnMouseRight )
    EVT_MENU( __idTagSelect__, wxHexCtrl::OnTagSelection )
	EVT_MENU( __idTagEdit__, wxHexCtrl::OnTagEdit )
	EVT_MOTION( wxHexCtrl::OnMouseMove )
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
	HexDefaultAttr = wxTextAttr(wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ),
								wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ),
								wxFont(
									10,					// point size
									wxMODERN,			// family
									wxFONTSTYLE_NORMAL,	// style
									wxFONTWEIGHT_BOLD,// weight
									true,				// underline
									wxT(""),			// facename
									wxFONTENCODING_CP437) );// msdos encoding


	SetSelectionStyle( HexDefaultAttr );

	HexDefaultAttr = wxTextAttr( *wxBLACK,
								*wxWHITE,
								//wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ),
								wxFont(
									10,					// point size
									wxMODERN,			// family
									wxFONTSTYLE_NORMAL,	// style
									wxFONTWEIGHT_NORMAL,// weight
									false,				// underline
									wxT(""),			// facename
									wxFONTENCODING_CP437) );// msdos encoding

    ClearSelection( false );
    SetDefaultStyle( HexDefaultAttr );
    m_Caret.x = m_Caret.y =
    m_Window.x = m_Window.y = 1;
    m_Margin.x = m_Margin.x = 0;
	LastRightClickAt = wxPoint(0,0);
    select.selected = false;
    CreateCaret();

  //  ChangeSize();

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
	TagArray.Clear();
}

void wxHexCtrl::Clear( bool RePaint, bool cursor_reset ){
	m_text.Clear();
	if( cursor_reset )
		SetInsertionPoint(0);
	OnTagHideAll();
	TagArray.Clear();
	/*BUG!!!
	TagElement *TAG;
	while( TagArray.Count() > 0 ){
		int x =  TagArray.Count();
		TAG = TagArray.Item(0);
		TagArray.Remove(0);
		}
	*/
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

int wxHexCtrl::xCountDenied( int x ){		//Counts denied character locations (spaces) on given x coordination
	for( int i = 0, denied = 0 ; i <  m_Window.x ; i++ ){
		if( IsDenied(i) )
			denied++;
		if( i == x )
			return denied;
		}
	return -1;
	}

int wxHexCtrl::CharacterPerLine( void ){	//Without spaces
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
	pos = ToVisiblePosition(pos);
	MoveCaret( wxPoint(pos%m_Window.x , pos/m_Window.x) );
	}

int wxHexCtrl::ToVisiblePosition( int InternalPosition ){	// I mean for this string on hex editor  "00 FC 05 C[C]" , while [] is cursor
	if( CharacterPerLine() == 0 ) return 0;					// Visible position is 8 but internal position is 11
	int y = InternalPosition / CharacterPerLine();
	int x = InternalPosition - y * CharacterPerLine();
	for( int i = 0, denied = 0 ; i < m_Window.x ; i++ ){
		if( IsDenied(i) ) denied++;
		if( i - denied == x )
			return ( i + y * m_Window.x );
		}
	wxLogError(wxString::Format(_T("Fatal error at fx ToVisiblePosition(%d)"),InternalPosition));
	return 0;
	}

int wxHexCtrl::ToInternalPosition( int VisiblePosition ){
	int y = VisiblePosition / m_Window.x;
	int x = VisiblePosition - y * m_Window.x;
	return ( x - xCountDenied(x) + y * CharacterPerLine() );
	}
																	// 00 15 21 CC FC
																	// 55 10 49 54 [7]7
wxPoint wxHexCtrl::InternalPositionToVisibleCoord( int position ){	// Visible position is 19, Visible Coord is (9,2)
	if( position < 0 )
		wxLogError(wxString::Format(_T("Fatal error at fx InternalPositionToVisibleCoord(%d)"),position));
	int x = m_Window.x? m_Window.x : 1;	//prevents divide zero error;
	int pos = ToVisiblePosition( position );
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
	wxColourData clrData;
	clrData.SetColour( new_attr.GetTextColour() );
	select.FontClrData = clrData;
	clrData.SetColour( new_attr.GetBackgroundColour() );
	select.NoteClrData = clrData;
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

void wxHexCtrl::MoveCaret(wxPoint p){
#ifdef _DEBUG_
	std::cout << "MoveCaret(wxPoint) Coordinate X:Y = " << p.x	<< " " << p.y << std::endl;
#endif
    m_Caret = p;
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
		for ( int y = 0 ; y < m_Window.y; y++ ){	//Draw base hex value without color
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

		int TAC = TagArray.Count();
		if( TAC != 0 ){
			TagElement *TAX;
			for(int i = 0 ; i < TAC ; i++){
				TAX = TagArray.Item(i);
				TagPainter( dcTemp, *TAX );
				}
			}
		if(select.selected)
			TagPainter( dcTemp, select );
		wxPaintDC dc( this );
		PrepareDC( dc );
		dc.Blit(0, 0, this->GetSize().GetWidth(), this->GetSize().GetHeight(), &dcTemp, 0, 0, wxCOPY);
		}
/*
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

*/
	}
void wxHexCtrl::TagPainter( wxMemoryDC& DC, TagElement& TG ){
	{	//Selection Painter
		DC.SetFont( HexDefaultAttr.GetFont() );
		DC.SetTextForeground( TG.FontClrData.GetColour() );
		DC.SetTextBackground( TG.NoteClrData.GetColour() );
		wxBrush sbrush( TG.NoteClrData.GetColour() );
		DC.SetBackground( sbrush );
		DC.SetBackgroundMode( wxSOLID ); // overwrite old value
		m_Char.y = DC.GetCharHeight();
		m_Char.x = DC.GetCharWidth();

		int start = TG.start;
		int end = TG.end;

		if( start > end ){
			int temp = end;
			end = start;
			start = temp;
			}

		if( start < 0 )
			start = 0;
		if ( end > ByteCapacity()*2)
			 end = ByteCapacity()*2;
// TODO (death#1#): Here problem with Text Ctrl.Use smart pointer...?
		wxPoint _start_ = InternalPositionToVisibleCoord( start );
		wxPoint _end_	= InternalPositionToVisibleCoord( end );
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
			DC.DrawText( line, m_Margin.x + _temp_.x * m_Char.x,	//Write prepared line
								   m_Margin.x + _temp_.y * m_Char.y );
			}
		}
	}

bool wxHexCtrl::IsAllowedChar(const char& chr){
	return isxdigit( chr );
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
//--------WRITE FUNCTIONS-------------//

void wxHexCtrl::WriteHex( const wxString& value ){		//write string as hex value to current position
	Replace(GetInsertionPoint(), GetInsertionPoint()+value.Length(), value);
	}

void wxHexCtrl::WriteByte( const unsigned char& byte ){	//write string as bin value to current position
	unsigned byte_location = GetInsertionPoint()/2;
	wxString buffer;
	buffer << (byte >> 4);
	buffer << (byte & 0x0F);
	Replace( byte_location*2,byte_location*2+2,buffer );
	}

void wxHexCtrl::SetBinValue( wxString buffer, bool repaint ){
	m_text.Clear();
	for( unsigned i=0 ; i < buffer.Length() ; i++ )
		m_text += wxString::Format(_("%02X"), static_cast<unsigned char>(buffer.at(i)));
	if(repaint)
		RePaint();
	}
void wxHexCtrl::SetBinValue( char* buffer, int byte_count, bool repaint ){
	m_text.Clear();
	for( int i=0 ; i < byte_count ; i++ )
		m_text += wxString::Format(_("%02X"), static_cast<unsigned char>(buffer[i]));
	if(repaint)
		RePaint();
	}

wxString wxHexCtrl::GetValue( void ){
	return m_text;
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
//------------EVENT HANDLERS---------------//
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

void wxHexCtrl::OnMouseMove( wxMouseEvent& event ){
#ifdef _DEBUG2_
	std::cout << "MouseMove Coordinate X:Y = " << event.m_x	<< " " << event.m_y
			<< "\tLMR mouse button:" << event.m_leftDown << event.m_middleDown << event.m_rightDown << std::endl;
#endif
	if(event.m_leftDown){
		select.end = PixelCoordToInternalPosition( event.GetPosition() );
		SetInsertionPoint( select.end );
		if(select.start != select.end)
			select.selected = true;
		else
			select.selected = false;
#ifdef _DEBUG_
		std::cout << "Selection is " << (select.selected?"true":"false") << " from " << select.start << " to " << select.end << std::endl;
#endif
		RePaint();
		}
	else{
		unsigned TagDetect = PixelCoordToInternalPosition( event.GetPosition() );
		TagElement *TAX;
		for( unsigned i = 0 ; i < TagArray.Count() ; i++ ){
			TAX = TagArray.Item(i);
			if( (TagDetect >= TAX->start ) && (TagDetect < TAX->end ) )	//end not included!
				{
				TAX->Show( event.GetPosition()+((wxWindow*)event.GetEventObject())->ClientToScreen(wxPoint(0,0)) , this );
				break;
				}
			else if( TAX->visible == true )
				TAX->Hide();
			}
		}
	}

void wxHexCtrl::OnMouseLeft( wxMouseEvent& event ){
	SetInsertionPoint( PixelCoordToInternalPosition( event.GetPosition() ) );
	select.start=GetInsertionPoint();
	}

void wxHexCtrl::OnMouseRight( wxMouseEvent& event ){
	event.Skip();
	LastRightClickAt = event.GetPosition();
	ShowContextMenu( LastRightClickAt );
	}

void wxHexCtrl::ShowContextMenu( wxPoint pos ){
	wxMenu menu;

	unsigned TagPosition = PixelCoordToInternalPosition( pos );
	TagElement *TAG;
	for( unsigned i = 0 ; i < TagArray.Count() ; i++ ){
		TAG = TagArray.Item(i);
		if( (TagPosition >= TAG->start ) && (TagPosition < TAG->end ) ){	//end not included!
			menu.Append(__idTagEdit__, _T("Tag Edit"));
			break;
			}
		}

	if( select.selected ){
		menu.Append(__idTagSelect__, _T("Tag Selection"));
		}
//  menu.AppendSeparator();
    PopupMenu(&menu, pos);
    // test for destroying items in popup menus
#if 0 // doesn't work in wxGTK!
    menu.Destroy(Menu_Popup_Submenu);
    PopupMenu( &menu, event.GetX(), event.GetY() );
#endif // 0
	}

void wxHexCtrl::OnTagEdit( wxCommandEvent& event ){
	TagElement *TAG;
	unsigned pos = PixelCoordToInternalPosition( LastRightClickAt );
	for( unsigned i = 0 ; i < TagArray.Count() ; i++ ){
		TAG = TagArray.Item(i);
		if( pos >= TAG->start && pos <= TAG->end ){
			TAG->Hide();	//Hide first, or BUG by double hide...
			TagElement TAGtemp = *TAG;
			TagDialog *x=new TagDialog( TAGtemp, this );
			switch( x->ShowModal() ){
				case wxID_SAVE:
					*TAG = TAGtemp;
					break;
				case wxID_DELETE:
					{
					delete TAG;
					TagArray.Remove(TAG);
					}
					break;
				default:
					break;
				}
			}
		break;
		}
	}

void wxHexCtrl::OnTagSelection( wxCommandEvent& event ){
	if(select.selected){
		TagElement *TAG = new TagElement;
		TAG->start=select.start;
		TAG->end=select.end;
		TagDialog *x=new TagDialog( *TAG, this );
		if( x->ShowModal() == wxID_SAVE)
			TagArray.Add( TAG );
		else
			delete TAG;
		x->Destroy();
		}
	}

void wxHexCtrl::OnTagHideAll( void ){
	for( unsigned i = 0 ; i < TagArray.Count() ; i++ )
		TagArray.Item(i)->Hide();
	}

void wxHexCtrl::OnTestCall( void ){
	wxBell();
    wxFrame *frame = wxDynamicCast(GetParent(), wxFrame);
    wxString msg;
    if ( frame && frame->GetStatusBar() )
    switch(1){
		case 0:{	// ToVisiblePosition & ToInternalPosition fx test case
			for(unsigned int i = 0 ; i < m_text.Length() ; i++)
				if( ToInternalPosition(i) != ToInternalPosition(ToVisiblePosition(ToInternalPosition(i)))){
					msg.Printf(_T("To[Visible/Internal]Position fx test false at: %d"), i);
					break;
					}
				std::cout << "To[Visible/Internal]Position fx test success" << std::endl;
			break;
			}
		case 1:{	// SetInsertionPoint & GetInsertionPoint fx test case
			for (int i = 0 ; i < GetLastPosition() ; i++ ){
				SetInsertionPoint(i);
				if( i != GetInsertionPoint() )
					std::cout << "[Set/Get]InsertionPoint false at: " <<  GetInsertionPoint() << i  << std::endl;
				}
			std::cout << "[Set/Get]InsertionPoint fx test success" << std::endl;
			break;
			}
		case 2:{
			char x = 0;
			WriteByte(x);
			msg.Empty();
			msg << _("ReadByte/WriteByte: ");
			if( x == ReadByte(0) )
				msg << _("OK");
			else
				msg << _("FAILED");
			frame->SetStatusText(msg, 0);
			}
			break;
		case 5:{
			//SetStyle(4,5,HexSelectAttr);
			break;
			}
		case 6:{
			char x[] = "0123456789000000";
			SetBinValue(x,16);
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
	}



//------HEXTEXTCTRL-----/
inline bool wxHexTextCtrl::IsAllowedChar(const char& chr){
	return ((chr !=173) && ( (chr>31 && chr<127) || chr>159));	//Visible Char filter
	}

wxChar wxHexTextCtrl::Filter(const char& ch){
	if( IsAllowedChar(ch) )
		return ch;
	else
		return '.'; //Special Character '?'
	}

void wxHexTextCtrl::Replace(unsigned text_location, const wxChar& value, bool paint){
	if( text_location < m_text.Length() )
		m_text[text_location] = Filter(value);
	else{
		m_text << Filter(value);
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

void wxHexOffsetCtrl::SetValue( int64_t position ){
	SetValue( position, BytePerLine );
	}

void wxHexOffsetCtrl::SetValue( int64_t position, int byteperline ){
	int64_t temp_offset_position = offset_position = position;
	BytePerLine = byteperline;
	m_text.Clear();
	for( int i=0 ; i<LineCount() ; i++ ){
		if (hex_offset)
			m_text << ( wxString::Format( wxT("%012lX"), temp_offset_position ) );
		else
			m_text << ( wxString::Format( wxT("%012ld"), temp_offset_position ) );
		temp_offset_position += BytePerLine;
		}
	RePaint();
	}

void wxHexOffsetCtrl::OnMouseLeft( wxMouseEvent& event ){
	hex_offset = hex_offset ? false : true;
	SetValue( offset_position );
	}
