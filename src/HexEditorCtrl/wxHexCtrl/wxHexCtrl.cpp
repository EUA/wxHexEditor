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


#include "wxHexCtrl.h"
#include <wx/encconv.h>

BEGIN_EVENT_TABLE(wxHexCtrl,wxScrolledWindow )
	EVT_CHAR( wxHexCtrl::OnChar )
	EVT_SIZE( wxHexCtrl::OnSize )
	EVT_PAINT( wxHexCtrl::OnPaint )
	EVT_LEFT_DOWN( wxHexCtrl::OnMouseLeft )
	//EVT_LEFT_DOWN( wxHexOffsetCtrl::OnMouseLeft )
	//EVT_MOUSE( wxHexCtrl::OnResize)
	EVT_RIGHT_DOWN( wxHexCtrl::OnMouseRight )
	EVT_MENU( __idTagAddSelect__, wxHexCtrl::OnTagAddSelection )
	EVT_MENU( __idTagEdit__, wxHexCtrl::OnTagEdit )
	EVT_MOTION( wxHexCtrl::OnMouseMove )
	EVT_SET_FOCUS( wxHexCtrl::OnFocus )
	//EVT_KILL_FOCUS( wxHexCtrl::OnKillFocus )
END_EVENT_TABLE()


//#define _Use_Alternate_DrawText_ //For debugged drawtext for wx 2.9.x on Mac

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
	HexDefaultAttr = wxTextAttr(
								wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ),
								wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ),
								wxFont(
									10,
									wxFONTFAMILY_MODERN,	// family
									wxFONTSTYLE_NORMAL,	// style
									wxFONTWEIGHT_BOLD,// weight
									true,				// underline
									wxT(""),			// facename
									wxFONTENCODING_CP437) );// msdos encoding

	//Need to create object before Draw operation.
	ZebraStriping=new int;
	*ZebraStriping=-1;

	internalBufferDC=NULL;
	internalBufferBMP=NULL;

	HexFormat = wxT("xx ");

	SetSelectionStyle( HexDefaultAttr );

	HexDefaultAttr = wxTextAttr(
								//*wxBLACK, //Deprecated :p
								wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ),
								wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ),
								//*wxWHITE, //Deprecated :p
								//wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ),
								wxFont(
									10,
									wxFONTFAMILY_MODERN,	// family
									wxFONTSTYLE_NORMAL,	// style
									wxFONTWEIGHT_NORMAL,// weight
									false,				// underline
									wxT(""),			// facename
									wxFONTENCODING_CP437) );// msdos encoding

   ClearSelection( false );
   SetDefaultStyle( HexDefaultAttr );

   m_Caret.x = m_Caret.y =
   m_Window.x = m_Window.y = 1;
   m_Margin.x = m_Margin.y = 0;
	LastRightClickPosition = wxPoint(0,0);
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
    wxCaret *caret = new wxCaret(this, m_CharSize.x, m_CharSize.y);
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

inline bool wxHexCtrl::IsDenied( int x ){	// State Of The Art :) Hex plotter function by idents avoiding some X axes :)
	return IsDeniedCache[x];
	}

inline bool wxHexCtrl::IsDenied_NoCache( int x ){	// State Of The Art :) Hex plotter function by idents avoiding some X axes :)
//		x%=m_Window.x;						// Discarding y axis noise
	if(1){ //EXPERIMENTAL
		if( ( ( m_Window.x - 1 ) % HexFormat.Len() == 0 )	// For avoid hex divorcings
			&& ( x == m_Window.x - 1 ))
			return true;
		return HexFormat[x%(HexFormat.Len())]==' ';
		}

	if( ( ( m_Window.x - 1 ) % 3 == 0 )		// For avoid hex divorcings
		&& ( x == m_Window.x - 1 ))
		return true;
//	if( x == 3*8 )
//		return true;
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

int wxHexCtrl::CharacterPerLine( bool NoCache ){	//Without spaces
	if( not NoCache )
		return CPL;
	int avoid=0;
	for ( int x = 0 ; x < m_Window.x ; x++)
		avoid += IsDeniedCache[x];
	CPL=m_Window.x - avoid;
	//std::cout << "CPL: " << CPL << std::endl;
	return ( m_Window.x - avoid );
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
	mouse.x = ( mouse.x > m_CharSize.x*m_Window.x ? m_CharSize.x*m_Window.x-1 : mouse.x);
	mouse.y = ( mouse.y < 0 ? 0 : mouse.y);
	mouse.y = ( mouse.y > m_CharSize.y*m_Window.y ? m_CharSize.y*m_Window.y-1 : mouse.y);
	int x = (mouse.x - m_Margin.x) / m_CharSize.x;
	int y = (mouse.y - m_Margin.y) / m_CharSize.y;
	return wxPoint(x,y);
	}

void wxHexCtrl::SetFormat( wxString fmt ){
	HexFormat = fmt;
//	for(int i=0 ; i < m_Window.x+1 ; i++)
//		IsDeniedCache[i]=IsDenied_NoCache(i);
	}

wxString wxHexCtrl::GetFormat( void ){
	return HexFormat;
	}

void wxHexCtrl::SetDefaultStyle( wxTextAttr& new_attr ){
	HexDefaultAttr = new_attr;

    wxClientDC dc(this);
    dc.SetFont( HexDefaultAttr.GetFont() );
    SetFont( HexDefaultAttr.GetFont() );
    m_CharSize.y = dc.GetCharHeight();
    m_CharSize.x = dc.GetCharWidth();

    wxCaret *caret = GetCaret();
    if ( caret )
		caret->SetSize(m_CharSize.x, m_CharSize.y);
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
#ifdef _DEBUG_CARET_
	std::cout << "MoveCaret(wxPoint) Coordinate X:Y = " << p.x	<< " " << p.y << std::endl;
#endif
   m_Caret = p;
   DoMoveCaret();
}

void wxHexCtrl::MoveCaret(int x){
#ifdef _DEBUG_CARET_
	std::cout << "MoveCaret(ınt) = " << x << std::endl;
#endif
	m_Caret.y = x/CharacterPerLine();
   m_Caret.x = x - m_Caret.y*CharacterPerLine();
   DoMoveCaret();
}

void wxHexCtrl::DoMoveCaret(){
   wxCaret *caret = GetCaret();
   if ( caret )
		caret->Move(m_Margin.x + m_Caret.x * m_CharSize.x,
                    m_Margin.x + m_Caret.y * m_CharSize.y);
}

inline wxMemoryDC* wxHexCtrl::CreateDC(){
//	wxBitmap *bmp=new wxBitmap(this->GetSize().GetWidth(), this->GetSize().GetHeight());
//	wxMemoryDC *dcTemp = new wxMemoryDC();
//	dcTemp->SelectObject(*bmp);
//	return dcTemp;
	if(internalBufferDC not_eq NULL)
		delete internalBufferDC;
	if(internalBufferBMP not_eq NULL)
		delete internalBufferBMP;

	internalBufferBMP=new wxBitmap(this->GetSize().GetWidth(), this->GetSize().GetHeight());
	internalBufferDC = new wxMemoryDC();
	internalBufferDC->SelectObject(*internalBufferBMP);
	return internalBufferDC;
	}

inline wxDC* wxHexCtrl::UpdateDC(){
	if(internalBufferDC==NULL)
		internalBufferDC = CreateDC();
	wxMemoryDC *dcTemp = internalBufferDC;
	//wxPaintDC *dcTemp = new wxPaintDC(this);
	//wxBufferedPaintDC *dcTemp= new wxBufferedPaintDC(this); //has problems with MacOSX

#ifdef _DEBUG_SIZE_
		std::cout << "wxHexCtrl::Update Sizes: " << this->GetSize().GetWidth() << ":" << this->GetSize().GetHeight() << std::endl;
#endif
	dcTemp->SetFont( HexDefaultAttr.GetFont() );
	dcTemp->SetTextForeground( HexDefaultAttr.GetTextColour() );
	dcTemp->SetTextBackground( HexDefaultAttr.GetBackgroundColour() ); //This will be overriden by Zebra stripping
	wxBrush dbrush( HexDefaultAttr.GetBackgroundColour() );

	dcTemp->SetBackground(dbrush );
	dcTemp->SetBackgroundMode( wxSOLID ); // overwrite old value
	dcTemp->Clear();

	wxString line;
	line.Alloc( m_Window.x+1 );
	wxColour col_standart(HexDefaultAttr.GetBackgroundColour());

	wxColour col_zebra(0x00FFEEEE);
// TODO (death#1#): Remove colour lookup for speed up
	wxString Colour;
	if( wxConfig::Get()->Read( _T("ColourHexBackgroundZebra"), &Colour) )
		col_zebra.Set( Colour );

	size_t z = 0;
	size_t textLength=m_text.Length();
//	char bux[1000];  //++//
	for ( int y = 0 ; y < m_Window.y; y++ ){	//Draw base hex value without color
		line.Empty();

		if (*ZebraStriping != -1 )
			dcTemp->SetTextBackground( (y+*ZebraStriping)%2 ? col_standart : col_zebra);

		for ( int x = 0 ; x < m_Window.x; x++ ){
			if( IsDenied(x)){
				line += wxT(' ');
				//bux[x]=' ';//++//
				continue;
				}
			if(z >= textLength)
				break;
	//		bux[x]=CharAt(z);//++//
			line += CharAt(z++);
			//dcTemp->DrawText( wxString::From8BitData(&t), m_Margin.x + x*m_CharSize.x, m_Margin.y + y * m_CharSize.y );
			}
/*
		#ifndef __WXGTK__
		//Error on WXGTK. Need test on other platforms.
		line = wxString(line.To8BitData(), wxCSConv(wxFONTENCODING_CP437), line.Len());
		#else
		//line = CP473toUnicode(line);
		#endif
*/
		dcTemp->DrawText( line, m_Margin.x, m_Margin.y + y * m_CharSize.y );
		}

	int TAC = TagArray.Count();
	if( TAC != 0 ){
		for(int i = 0 ; i < TAC ; i++)
			TagPainter( dcTemp, *TagArray.Item(i) );
		}
	if(select.selected)
		TagPainter( dcTemp, select );

	DrawCursorShadow(dcTemp);

	if(ThinSeperationLines.Count() > 0)
		for( unsigned i=0 ; i < ThinSeperationLines.Count() ; i++)
			DrawSeperationLineAfterChar( dcTemp, ThinSeperationLines.Item(i) );

	return dcTemp;
}

inline void wxHexCtrl::DrawCursorShadow(wxDC* dcTemp){
	if( m_Window.x <= 0 or
		FindFocus()==this)
		return;

	int y=m_CharSize.y*( m_Caret.y ) + m_Margin.y;
	int x=m_CharSize.x*( m_Caret.x ) + m_Margin.x;

	dcTemp->SetPen( *wxBLACK_PEN );
	dcTemp->SetBrush( *wxTRANSPARENT_BRUSH );
	dcTemp->DrawRectangle(x,y,m_CharSize.x*2+1,m_CharSize.y);
	}


inline void wxHexCtrl::DrawSeperationLineAfterChar( wxDC* dcTemp, int seperationoffset ){
	if(m_Window.x > 0){
		wxPoint z = InternalPositionToVisibleCoord( seperationoffset );
		int y1=m_CharSize.y*( 1+z.y )+ m_Margin.y;
		int y2=y1-m_CharSize.y;
		int x1=m_CharSize.x*(z.x)+m_Margin.x;
		int x2=m_CharSize.x*2*m_Window.x+m_Margin.x;

		dcTemp->SetPen( *wxRED_PEN );
		dcTemp->DrawLine( 0,y1,x1,y1);
		if( z.x not_eq 0)
			dcTemp->DrawLine( x1,y1,x1,y2);
		dcTemp->DrawLine( x1,y2,x2,y2);
		}
	}

void wxHexCtrl::RePaint( void ){
	wxCaretSuspend cs(this);
	wxDC* dcTemp = UpdateDC();
	if( dcTemp != NULL ){
		wxClientDC dc( this );
#ifdef _Use_Graphics_Contex_
		wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
		gc->DrawBitmap( *internalBufferBMP, 0.0, 0.0,dc.GetSize().GetWidth(), dc.GetSize().GetHeight());
		delete gc;
#else
		dc.Blit(0, 0, this->GetSize().GetWidth(), this->GetSize().GetHeight(), dcTemp, 0, 0, wxCOPY);
#endif
		///delete dcTemp;
		}
	}

void wxHexCtrl::OnPaint( wxPaintEvent &WXUNUSED(event) ){
	wxDC* dcTemp = UpdateDC();
	if( dcTemp != NULL ){
		wxPaintDC dc( this ); //wxPaintDC because here is under native wxPaintEvent.
#ifdef _Use_Graphics_Contex_
		wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
		gc->DrawBitmap( *internalBufferBMP, 0.0, 0.0, dc.GetSize().GetWidth(), dc.GetSize().GetHeight());
		delete gc;
#else
		dc.Blit(0, 0, this->GetSize().GetWidth(), this->GetSize().GetHeight(), dcTemp, 0, 0, wxCOPY);
#endif
		///delete dcTemp;
		}
	}

void wxHexCtrl::TagPainter( wxDC* DC, TagElement& TG ){
	//Selection Painter
	DC->SetFont( HexDefaultAttr.GetFont() );
	DC->SetTextForeground( TG.FontClrData.GetColour() );
//		DC->SetTextBackground( TG.NoteClrData.GetColour() );
	DC->SetTextBackground( TG.SoftColour( TG.NoteClrData.GetColour() ));

	wxBrush sbrush( TG.NoteClrData.GetColour() );
	//preparation for wxGCDC for semi transparent marking.
	//wxBrush sbrush(wxBrush(wxColour(255,0,0,128),wxBRUSHSTYLE_TRANSPARENT ));
	DC->SetBackground( sbrush );
	DC->SetBackgroundMode( wxSOLID ); // overwrite old value
	m_CharSize.y = DC->GetCharHeight();
	m_CharSize.x = DC->GetCharWidth();

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
	wxPoint _end_   = InternalPositionToVisibleCoord( end );
	wxPoint _temp_  = _start_;

	//char bux[1024];//++//
	for ( ; _temp_.y <= _end_.y ; _temp_.y++ ){
		wxString line;
		_temp_.x = ( _temp_.y == _start_.y ) ? _start_.x : 0;	//calculating local line start
		int z = ( _temp_.y == _end_.y ) ? _end_.x : m_Window.x;	// and end point
		for ( int x = _temp_.x; x < z; x++ ){					//Prepare line to write process
			if( IsDenied(x) ){
				if(x+1 < z){
					line += wxT(' ');
					//bux[x]=' ';//++//
//#if wxCHECK_VERSION(2,9,0) & defined( __WXOSX__ ) //OSX DrawText bug
//					DC->DrawText( wxString::FromAscii(' '), m_Margin.x + x*m_CharSize.x, m_Margin.y + _temp_.y * m_CharSize.y );
//#endif
					}
				continue;
				}
			//bux[x]=CharAt(start); //++//
			line += CharAt(start++);

//#if wxCHECK_VERSION(2,9,0) & defined( __WXOSX__ ) //OSX DrawText bug
//			DC->DrawText( wxString::FromAscii(ch), m_Margin.x + x*m_CharSize.x, m_Margin.y + _temp_.y * m_CharSize.y );
//#endif
			}
//#if !(wxCHECK_VERSION(2,9,0) & defined( __WXOSX__ )) //OSX DrawText bug

		///Cannot convert from the charset 'Windows/DOS OEM (CP 437)'!
//		line=wxString(bux, wxCSConv(wxFONTENCODING_CP437),  _temp_.y);
		//line=wxString(line.To8BitData(), wxCSConv(wxFONTENCODING_ALTERNATIVE),  line.Len());

		//line=CP473toUnicode(line);
		DC->DrawText( line, m_Margin.x + _temp_.x * m_CharSize.x,	//Write prepared line
								m_Margin.x + _temp_.y * m_CharSize.y );
//#endif
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
		// TODO (death#1#): if text selected, enter from begining!
		// TODO (death#2#): If text Selected, than  remove select first?
				select.selected=false;
				(chr>='a'&&chr<='z')?(chr-=('a'-'A')):(chr=chr);	//Upper() for Char
				WriteHex(chr);

				//CharAt(m_Caret.x, m_Caret.y) = ch;
/*				wxCaretSuspend cs(this);
                wxClientDC dc(this);
                dc.SetFont(m_font);
                dc.SetBackgroundMode(wxSOLID); // overwrite old value
                dc.DrawText(chr, m_Margin.x + m_Caret.x * m_CharSize.x,
                                m_Margin.x + m_Caret.y * m_CharSize.y );
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

	m_Window.x = (size.x - 2*m_Margin.x) / m_CharSize.x;
	m_Window.y = (size.y - 2*m_Margin.x) / m_CharSize.y;
	if ( m_Window.x < 1 )
		m_Window.x = 1;
	if ( m_Window.y < 1 )
		m_Window.y = 1;

	for(int i=0 ; i < m_Window.x+1 ; i++)
		IsDeniedCache[i]=IsDenied_NoCache(i);
	CharacterPerLine( true );//Updates CPL static int

	//This Resizes internal buffer!
	CreateDC();

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
		m_text += wxString::Format(wxT("%02X"), static_cast<unsigned char>(buffer.at(i)));
	if(repaint)
		RePaint();
	}

void wxHexCtrl::SetBinValue( char* buffer, int byte_count, bool repaint ){
	m_text.Clear();
	for( int i=0 ; i < byte_count ; i++ )
		m_text += wxString::Format(wxT("%02X"), static_cast<unsigned char>(buffer[i]));
	if(repaint)
		RePaint();
	}

wxString wxHexCtrl::GetValue( void ){
	return m_text;
	}

void wxHexCtrl::Replace(unsigned hex_location, const wxChar& value, bool repaint){
	if( hex_location < m_text.Length() )
		m_text[hex_location] = value;
	else{
		m_text << value;
		m_text << wxT("0");
		}
	if(repaint)
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
                dc.DrawText(chr, m_Margin.x + m_Caret.x * m_CharSize.x,
                                m_Margin.x + m_Caret.y * m_CharSize.y );
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
	return static_cast<char*>(HexToBin(hx).GetData())[0];
	}

int atoh(const char hex){
	return ( hex >= '0' and hex <= '9' ) ? hex -'0' :
			( hex >= 'a' and hex <= 'f' ) ? hex -'a' + 10:
			( hex >= 'A' and hex <= 'F' ) ? hex -'A' + 10:
			-1;
			}

wxMemoryBuffer wxHexCtrl::HexToBin(const wxString& HexValue){
	wxMemoryBuffer memodata;
	memodata.SetBufSize(HexValue.Length()/3+1);
	char bfrL, bfrH;
	for(unsigned int i=0 ; i < HexValue.Length() ; i+=2){
		if( HexValue[i] == ' ' or HexValue[i] == ',' ){	//Removes space and period chars.
			i--; //Means +1 after loop increament of +2. Don't put i++ due HexValue.Length() check
			continue;
			}
		else if ((HexValue[i] == '0' and ( HexValue[i+1] == 'x' or HexValue[i+1] == 'X'))){ //Removes "0x" and "0X" strings.
			continue; //Means +2 by loop increament.
			}
		bfrH = atoh( HexValue[i] );
		bfrL = atoh( HexValue[i+1] );
		//Check for if it's Hexadecimal
		if( not (bfrH < 16 and bfrL < 16 and bfrH >= 0 and bfrL >= 0 )){
				wxBell();
				return memodata;
			}
		bfrL = bfrH << 4 | bfrL;
		memodata.AppendByte( bfrL );
		}
	return memodata;
	}
//------------EVENT HANDLERS---------------//
void wxHexCtrl::OnFocus(wxFocusEvent& event ){
	wxCaret *caret = GetCaret();
   if ( caret )
		caret->Show(true);
	}

void wxHexCtrl::OnKillFocus(wxFocusEvent& event ){
#ifdef _DEBUG_MOUSE_
	std::cout << "wxHexCtrl::OnKillFocus()" << std::endl;
#endif
	wxCaretSuspend cs(this);
	wxCaret *caret = GetCaret();
   if ( caret )
		caret->Show(false);

	if( *TagMutex ){
		for( unsigned i = 0 ; i < TagArray.Count() ; i++ )
			TagArray.Item(i)->Hide();
		*TagMutex = false;
		}
	event.Skip();
	}

void wxHexCtrl::OnSize( wxSizeEvent &event ){
#ifdef _DEBUG_SIZE_
		std::cout << "wxHexCtrl::OnSize X,Y" << event.GetSize().GetX() <<',' << event.GetSize().GetY() << std::endl;
#endif
	ChangeSize();
   event.Skip();
	}

void wxHexCtrl::OnMouseMove( wxMouseEvent& event ){
#ifdef _DEBUG_MOUSE_
	std::cout << "wxHexCtrl::OnMouseMove Coordinate X:Y = " << event.m_x	<< " " << event.m_y
			<< "\tLMR mouse button:" << event.m_leftDown << event.m_middleDown << event.m_rightDown << std::endl;
#endif
	if(event.m_leftDown){
		select.end = PixelCoordToInternalPosition( event.GetPosition() );
		SetInsertionPoint( select.end );
		if(select.start != select.end)
			select.selected = true;
		else
			select.selected = false;
#ifdef _DEBUG_SELECT_
		std::cout << "wxHexCtrl::Selection is " << (select.selected?"true":"false") << " from " << select.start << " to " << select.end << std::endl;
#endif
		RePaint();
		}
	else{
		unsigned TagDetect = PixelCoordToInternalPosition( event.GetPosition() );
		TagElement *TAX;
		for( unsigned i = 0 ; i < TagArray.Count() ; i++ ){
			TAX = TagArray.Item(i);
			if( (TagDetect >= TAX->start ) && (TagDetect < TAX->end ) ){	//end not included!
				if( not (*TagMutex) and  wxTheApp->IsActive() ) {
					*TagMutex=true;
					TAX->Show( event.GetPosition()+(static_cast<wxWindow*>( event.GetEventObject() ))->ClientToScreen(wxPoint(0,0)) , this );
					}
				break;
				}
			}
		}
	}
TagElement* wxHexCtrl::GetTagByPix( wxPoint PixPos ){
	unsigned TagDetect = PixelCoordToInternalPosition( PixPos );
	TagElement *TAX;
	for( unsigned i = 0 ; i < TagArray.Count() ; i++ ){
		TAX = TagArray.Item(i);
		if( (TagDetect >= TAX->start ) && (TagDetect < TAX->end ) )
			return TagArray.Item(i);
		}
	return NULL;
	}

void wxHexCtrl::OnMouseLeft( wxMouseEvent& event ){
	SetInsertionPoint( PixelCoordToInternalPosition( event.GetPosition() ) );
	select.start=GetInsertionPoint();
	}

void wxHexCtrl::OnMouseRight( wxMouseEvent& event ){
	event.Skip();
	LastRightClickPosition = event.GetPosition();
	ShowContextMenu( LastRightClickPosition );
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
		menu.Append(__idTagAddSelect__, _T("Tag Selection"));
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
	unsigned pos = PixelCoordToInternalPosition( LastRightClickPosition );
	for( unsigned i = 0 ; i < TagArray.Count() ; i++ ){
		TAG = TagArray.Item(i);
		if( TAG->isCover( pos ) ){
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

void wxHexCtrl::OnTagAddSelection( wxCommandEvent& event ){
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
			msg << _T("ReadByte/WriteByte: ");
			if( x == ReadByte(0) )
				msg << _T("OK");
			else
				msg << _T("FAILED");
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

///------HEXTEXTCTRL-----///
/*
inline wxChar CP437toUnicodeCHR( const unsigned char& chr){
	return CP437Table[chr];
	}

inline wxString CP437toUnicode( wxString& line ){
	wxString ret;
	for(unsigned i=0; i < line.Len() ; i++)
		ret+=CP437Table[line[i]];
	return ret;
	}
*/
inline wxChar wxHexTextCtrl::Filter(const unsigned char& ch){
	return CodepageTable[ch];
	}

inline wxString wxHexTextCtrl::FilterMBBuffer(const char *str, int Len, int fontenc ){
	wxString ret;
	//wxCSConv mcv(wxFONTENCODING_UTF8);
	// size_t WC2MB(char* buf, const wchar_t* psz, size_t n) const
	// size_t MB2WC(wchar_t* buf, const char* psz, size_t n) const
	wxString z;
	if(fontenc==wxFONTENCODING_UTF8)
		for( int i=0 ; i< Len ; i++){
			unsigned char ch = str[i];
			if(ch < 0x20) ret+='.';									// Control characters
			else if( ch >= 0x20 and ch <= 0x7E ) ret+=ch;	// ASCII compatible part
			else if(ch == 0x7F) ret+='.';							// Control character
			else if( ch >= 0x80 and ch < 0xC0 ) ret+='.';	// 6 Bit Extension Region
			else if( ch >= 0xC0 and ch < 0xC2 ) ret+='.';	// Invalid UTF8 2 byte codes
			else if( ch >= 0xC2 and ch < 0xE0 ) {				// 2 Byte UTF Start Codes
				z=wxString::FromUTF8( str+i, 2);
				//z=wxString( str+i, wxCSConv(wxFONTENCODING_UTF8), 2);
				if( z.Len() > 0){
					ret+=z;
					ret+=' ';
					i+=1;
					}
				else
					ret+='.';
				}
			else if( ch >= 0xE0 and ch < 0xF0 ){				// 3 Byte UTF Start Codes
				z=wxString::FromUTF8( str+i, 3);
				if( z.Len() > 0){
					ret+=z;
					ret+=wxT("  ");
					i+=2;
					}
				else
					ret+='.';
				}
			else if( ch >= 0xF0 and ch < 0xF5 ){				// 4 Byte UTF Start Codes
	//			ret+=wxString(str+i, wxConvUTF8, 4);
				z=wxString::FromUTF8( str+i, 4);
				if( z.Len() > 0){
					ret+=z;
					ret+=wxT("   ");
					i+=3;
					}
				else
					ret+='.';
				}
			else if( ch >= 0xF5 and ch <=0xFF ) ret+='.'; // Invalid UTF8 4 byte codes
			}

	if(fontenc==wxFONTENCODING_GB2312)
		for( int i=0 ; i< Len ; i++){
			unsigned char ch = str[i];
			if(ch < 0x20) ret+='.';									// Control characters
			else if( ch >= 0x20 and ch <= 0x7E ) ret+=ch;	// ASCII compatible part
			else if(ch < 0x80) ret+='.';							// Void Characters
			else { // 0x21 + 0x80
				z=wxString( str+i, wxCSConv(wxFONTENCODING_GB2312), 2);
				if( z.Len() > 0){
					ret+=z;
					if( z.Len() == 1 )
						ret+=' ';
					i+=1;
					}
				else
					ret+='.';
				}
		}

	if(Codepage.StartsWith(wxT("TSCII"))){
///		0x82 4Byte
///		0x87 3Byte
///		0x88->0x8B 2Byte
///		0x8C 4Byte
///		0x99->0x9C 2Byte
///		0xCA->0xFD 2Byte

		int p;
		for(int i=0;i<Len;i++){
			p=0;
			if(i<0x80)
				ret+=Filter(str[i]);
			else{
				if(i>0x82)	p+=3;
				if(i>0x87)	p+=2;
				if(i>0x8B)	p+=(0x8B-0x88)*1;
				if(i>0x8C)	p+=3;
				if(i>0x9C)	p+=(0x9C-0x99)*1;
				if(i>0xFD)	p+=(0xFD-0xCA)*1;

				if(i==0x82)	ret+=CodepageTable.Mid(i,4);
				else if(i==0x87)					ret+=CodepageTable.Mid(i+p,3);
				else if(i>=0x88 and i<=0x8B)	ret+=CodepageTable.Mid((i-0x88)*2+p,2);
				else if(i==0x8C)					ret+=CodepageTable.Mid(i+p,4);
				else if(i>=0x99 and i<=0x9C)	ret+=CodepageTable.Mid((i-0x99)*2+p,2);
				else if(i>=0xCA and i<=0xFD)	ret+=CodepageTable.Mid((i-0xCA)*2+p,2);
				else ret+=CodepageTable[p];
				}
			}
		}

	return ret;
	}

wxString wxHexTextCtrl::PrepareCodepageTable(wxString codepage){
	wxString newCP;
	char bf[256];
	if(codepage.StartsWith(wxT("ASCII"))){
		for (unsigned i=0; i<=0xFF ; i++){
			if(i<0x20 or i>=0x7F)		newCP+=wxT('.');  //Control chars replaced with dot
			if(i>=0x20 and i<0x7F)		newCP+=wxChar(i);	//ASCII region
			}
		}
	//Indian Script Code for Information Interchange
	else if(codepage.StartsWith(wxT("ISCII"))){
		for (unsigned i=0; i<=0xA1 ; i++)
			newCP+=wxChar((i<0x20 or i>=0x7F) ? '.' : i);
		//Unicode eq of 0xD9 is \x25CC or \x00AD
		newCP+=wxT("\x0901\x0902\x0903\x0905\x0906\x0907\x0908\x0909\x090A\x090B"\
					  "\x090E\x090F\x0910\x090D\x0912\x0913\x0914\x0911\x0915\x0916"\
					  "\x0917\x0918\x0919\x091A\x091B\x091C\x091D\x091E\x091F\x0920"\
					  "\x0921\x0922\x0923\x0924\x0925\x0926\x0927\x0928\x0929\x092A"\
					  "\x092B\x092C\x092D\x092E\x092F\x095F\x0930\x0931\x0932\x0933"\
					  "\x0934\x0935\x0936\x0937\x0938\x0939\x25CC\x093E\x093F\x0940"\
					  "\x0941\x0942\x0943\x0946\x0947\x0948\x0945\x094A\x094B\x094C"\
					  "\x0949\x094D\x093C\x0964......\x0966\x0967\x0968"
					  "\x0969\x096A\x096B\x096C\x096D\x096E\x096F");
		}

	//Tamil Script Code for Information Interchange
	else if(codepage.StartsWith(wxT("TSCII"))){
		newCP=PrepareCodepageTable(wxT("ASCII")).Mid(0,0x80);
		newCP+=wxT(	"\x0BE6\x0BE7\x0BB8\x0BCD\x0BB0\x0BC0\x0B9C\x0BB7\x0BB8\x0BB9\x0B95"\
						"\x0BCD\x0BB7\x0B9C\x0BCD\x0BB7\x0BCD\x0BB8\x0BCD\x0BB9\x0BCD\x0B95"\
						"\x0BCD\x0BB7\x0BCD\x0BE8\x0BE9\x0BEA\x0BEB\x2018\x2019\x201C\x201D"\
						"\x0BEC\x0BED\x0BEE\x0BEF\x0B99\x0BC1\x0B9E\x0BC1\x0B99\x0BC2\x0B9E"\
						"\x0BC2\x0BF0\x0BF1\x0BF2\x00A0\x0BBE\x0BBF\x0BC0\x0BC1\x0BC2\x0BC6"\
						"\x0BC7\x0BC8\x00A9\x0BD7\x0B85\x0B86.\x0B88\x0B89\x0B8A\x0B8E\x0B8F"\
						"\x0B90\x0B92\x0B93\x0B94\x0B83\x0B95\x0B99\x0B9A\x0B9E\x0B9F\x0BA3"\
						"\x0BA4\x0BA8\x0BAA\x0BAE\x0BAF\x0BB0\x0BB2\x0BB5\x0BB4\x0BB3\x0BB1"\
						"\x0BA9\x0B9F\x0BBF\x0B9F\x0BC0\x0B95\x0BC1\x0B9A\x0BC1\x0B9F\x0BC1"\
						"\x0BA3\x0BC1\x0BA4\x0BC1\x0BA8\x0BC1\x0BAA\x0BC1\x0BAE\x0BC1\x0BAF"\
						"\x0BC1\x0BB0\x0BC1\x0BB2\x0BC1\x0BB5\x0BC1\x0BB4\x0BC1\x0BB3\x0BC1"\
						"\x0BB1\x0BC1\x0BA9\x0BC1\x0B95\x0BC2\x0B9A\x0BC2\x0B9F\x0BC2\x0BA3"\
						"\x0BC2\x0BA4\x0BC2\x0BA8\x0BC2\x0BAA\x0BC2\x0BAE\x0BC2\x0BAF\x0BC2"\
						"\x0BB0\x0BC2\x0BB2\x0BC2\x0BB5\x0BC2\x0BB4\x0BC2\x0BB3\x0BC2\x0BB1"\
						"\x0BC2\x0BA9\x0BC2\x0B95\x0BCD\x0B99\x0BCD\x0B9A\x0BCD\x0B9E\x0BCD"\
						"\x0B9F\x0BCD\x0BA3\x0BCD\x0BA4\x0BCD\x0BA8\x0BCD\x0BAA\x0BCD\x0BAE"\
						"\x0BCD\x0BAF\x0BCD\x0BB0\x0BCD\x0BB2\x0BCD\x0BB5\x0BCD\x0BB4\x0BCD"\
						"\x0BB3\x0BCD\x0BB1\x0BCD\x0BA9\x0BCD\x0B87" );

///		0x82 4Byte
///		0x87 3Byte
///		0x88->0x8B 2Byte
///		0x8C 4Byte
///		0xCA->0xFD 2Byte
///		0x99->0x9C 2Byte
		}

	else if(codepage.StartsWith(wxT("VSCII"))){
		for (unsigned i=0; i<=0x7F ; i++)
			if( i==0x02) newCP+=wxT("\x1EB2");
			else if( i==0x05) newCP+=wxT("\x1EB4");
			else if( i==0x06) newCP+=wxT("\x1EAA");
			else if( i==0x14) newCP+=wxT("\x1EF6");
			else if( i==0x19) newCP+=wxT("\x1EF8");
			else if( i==0x1E) newCP+=wxT("\x1EF4");
			else newCP+=wxChar((i<0x20 or i>=0x7F) ? '.' : i);

		newCP+=wxT(	"\x1EA0\x1EAE\x1EB0\x1EB6\x1EA4\x1EA6\x1EA8\x1EAC\x1EBC\x1EB8"\
						"\x1EBE\x1EC0\x1EC2\x1EC4\x1EC6\x1ED0\x1ED2\x1ED4\x1ED6\x1ED8"\
						"\x1EE2\x1EDA\x1EDC\x1EDE\x1ECA\x1ECE\x1ECC\x1EC8\x1EE6\x0168"\
						"\x1EE4\x1EF2\x00D5\x1EAF\x1EB1\x1EB7\x1EA5\x1EA7\x1EA9\x1EAD"\
						"\x1EBD\x1EB9\x1EBF\x1EC1\x1EC3\x1EC5\x1EC7\x1ED1\x1ED3\x1ED5"\
						"\x1ED7\x1EE0\x01A0\x1ED9\x1EDD\x1EDF\x1ECB\x1EF0\x1EE8\x1EEA"\
						"\x1EEC\x01A1\x1EDB\x01AF\x00C0\x00C1\x00C2\x00C3\x1EA2\x0102"\
						"\x1EB3\x1EB5\x00C8\x00C9\x00CA\x1EBA\x00CC\x00CD\x0128\x1EF3"\
						"\x0110\x1EE9\x00D2\x00D3\x00D4\x1EA1\x1EF7\x1EEB\x1EED\x00D9"\
						"\x00DA\x1EF9\x1EF5\x00DD\x1EE1\x01B0\x00E0\x00E1\x00E2\x00E3"\
						"\x1EA3\x0103\x1EEF\x1EAB\x00E8\x00E9\x00EA\x1EBB\x00EC\x00ED"\
						"\x0129\x1EC9\x0111\x1EF1\x00F2\x00F3\x00F4\x00F5\x1ECF\x1ECD"\
						"\x1EE5\x00F9\x00FA\x0169\x1EE7\x00FD\x1EE3\x1EEE") ;
		}

	//OEM PC-DOS
	else if(codepage.Find(wxT("CP437")) not_eq wxNOT_FOUND ){
		//CP437 Control Symbols
		newCP=wxT(  "\x20\x263A\x263B\x2665\x2666\x2663\x2660\x2022\x25D8\x25CB\x25D9"\
						"\x2642\x2640\x266A\x266B\x263C\x25BA\x25C4\x2195\x203C\x00B6\x00A7"\
						"\x25AC\x21A8\x2191\x2193\x2192\x2190\x221F\x2194\x25B2\x25BC" );

		//ASCII compatible part
		for( unsigned i=0x20 ; i < 0x7F ; i++ )
			newCP += wxChar(i);

		//Extended ASCII region
		newCP+=wxT(	"\x2302\x00C7\x00FC\x00E9\x00E2\x00E4\x00E0\x00E5\x00E7\x00EA\x00EB"\
						"\x00E8\x00EF\x00EE\x00EC\x00C4\x00C5\x00C9\x00E6\x00C6\x00F4\x00F6"\
						"\x00F2\x00FB\x00F9\x00FF\x00D6\x00DC\x00A2\x00A3\x00A5\x20A7\x0192"\
						"\x00E1\x00ED\x00F3\x00FA\x00F1\x00D1\x00AA\x00BA\x00BF\x2310\x00AC"\
						"\x00BD\x00BC\x00A1\x00AB\x00BB\x2591\x2592\x2593\x2502\x2524\x2561"\
						"\x2562\x2556\x2555\x2563\x2551\x2557\x255D\x255C\x255B\x2510\x2514"\
						"\x2534\x252C\x251C\x2500\x253C\x255E\x255F\x255A\x2554\x2569\x2566"\
						"\x2560\x2550\x256C\x2567\x2568\x2564\x2565\x2559\x2558\x2552\x2553"\
						"\x256B\x256A\x2518\x250C\x2588\x2584\x258C\x2590\x2580\x03B1\x00DF"\
						"\x0393\x03C0\x03A3\x03C3\x00B5\x03C4\x03A6\x0398\x03A9\x03B4\x221E"\
						"\x03C6\x03B5\x2229\x2261\x00B1\x2265\x2264\x2320\x2321\x00F7\x2248"\
						"\x00B0\x2219\x00B7\x221A\x207F\x00B2\x25A0\x00A0");
		}

	else if(codepage.Find(wxT("CP850")) not_eq wxNOT_FOUND ){
//		for (unsigned i=0; i<=0xFF ; i++)
//			bf[i] =i;
//		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_CP850), 256); //Why this doesnt works?
		newCP=PrepareCodepageTable(wxT("CP437")).Mid(0,0x80);
		newCP+= wxT("\xC7\xFC\xE9\xE2\xE4\xE0\xE5\xE7\xEA\xEB\xE8\xEF\xEE\xEC\xC4\xC5\xC9"\
						"\xE6\xC6\xF4\xF6\xF2\xFB\xF9\xFF\xD6\xDC\xF8\xA3\xD8\xD7\x0192\xE1\xED"\
						"\xF3\xFA\xF1\xD1\xAA\xBA\xBF\xAE\xAC\xBD\xBC\xA1\xAB\xBB\x2591\x2592"\
						"\x2593\x2502\x2524\xC1\xC2\xC0\xA9\x2563\x2551\x2557\x255D\xA2\xA5"\
						"\x2510\x2514\x2534\x252C\x251C\x2500\x253C\xE3\xC3\x255A\x2554\x2569"\
						"\x2566\x2560\x2550\x256C\xA4\xF0\xD0\xCA\xCB\xC8\x0131\xCD\xCE\xCF"\
						"\x2518\x250C\x2588\x2584\xA6\xCC\x2580\xD3\xDF\xD4\xD2\xF5\xD5\xB5"\
						"\xFE\xDE\xDA\xDB\xD9\xFD\xDD\xAF\xB4\xAD\xB1\x2017\xBE\xB6\xA7\xF7"\
						"\xB8\xB0\xA8\xB7\xB9\xB3\xB2\x25A0\xA0");
		}

	else if(codepage.StartsWith(wxT("MIK"))){
		newCP=PrepareCodepageTable(wxT("CP437")).Mid(0,0x80);
		for (unsigned i=0x80; i<0xC0 ; i++)
			newCP += wxChar(i-0x80+0x0410);

		newCP += wxT(	"\x2514\x2534\x252C\x251C\x2500\x253C\x2563\x2551\x255A\x2554\x2569\x2566\x2560\x2550\x256C\x2510"\
							"\x2591\x2592\x2593\x2502\x2524\x2116\x00A7\x2557\x255D\x2518\x250C\x2588\x2584\x258C\x2590\x2580"\
							"\x03B1\x00DF\x0393\x03C0\x03A3\x03C3\x00B5\x03C4\x03A6\x0398\x03A9\x03B4\x221E\x03C6\x03B5\x2229"\
							"\x2261\x00B1\x2265\x2264\x2320\x2321\x00F7\x2248\x00B0\x2219\x00B7\x221A\x207F\x00B2\x25A0\x00A0" );
		}

	// ISO/IEC 8859 Part1-15
	else	if(codepage.Find(wxT("8859-")) not_eq wxNOT_FOUND ){
		//Masking default area
		for (unsigned i=0; i<=0xFF ; i++)
			bf[i] =  (i< 0x20 or i==0x7F or i==0xAD or (i>=0x80 and i<=0x9F ))	? '.' : i;

		//Detecting exact encoding
		int q=codepage.Find(wxT("8859-"))+5;

		//Filtering gaps
		if(codepage.Mid(q,2).StartsWith(wxT("3 ")))			bf[0xA5]=bf[0xAE]=bf[0xBE]=bf[0xC3]=bf[0xD0]=bf[0xE3]=bf[0xF0]='.';
		else if(codepage.Mid(q,2).StartsWith(wxT("6 "))){	//Arabic
			for(int i=0xA1 ; i<=0xC0 ; i++) bf[i]='.';
			bf[0xA4]=0xA4;
			bf[0xAC]=0xAC;
			bf[0xBB]=0xBB;
			bf[0xBF]=0xBF;
			for(int i=0xDB ; i<=0xDF ; i++) bf[i]='.';
			for(int i=0xF3 ; i<=0xFF ; i++) bf[i]='.';
			}
		else if(codepage.Mid(q,2).StartsWith(wxT("7 ")))	bf[0xAE]=bf[0xD2]=bf[0xFF]='.';
		else if(codepage.Mid(q,2).StartsWith(wxT("8 "))){	//Hebrew
			for(int i=0xBF ; i<=0xDE ; i++) bf[i]='.';
			for(int i=0xFB ; i<=0xFF ; i++) bf[i]='.';
			bf[0xA1]='.';
			}
		else if(codepage.Mid(q,2).StartsWith(wxT("11")))	bf[0xDB]=bf[0xDC]=bf[0xDD]=bf[0xDE]=bf[0xFC]=bf[0xFD]=bf[0xFE]=bf[0xFF]='.';

		//Encoding
		if		 (codepage.Mid(q,2).StartsWith(wxT("1 ")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_1), 256);
		else if(codepage.Mid(q,2).StartsWith(wxT("2 ")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_2), 256);
		else if(codepage.Mid(q,2).StartsWith(wxT("3 ")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_3), 256);
		else if(codepage.Mid(q,2).StartsWith(wxT("4 ")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_4), 256);
		else if(codepage.Mid(q,2).StartsWith(wxT("5 ")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_5), 256);
		// Arabic Output not looks good.
		else if(codepage.Mid(q,2).StartsWith(wxT("6 ")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_6), 256);
		else if(codepage.Mid(q,2).StartsWith(wxT("7 ")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_7), 256);
		// Hebrew Output not looks good.
		else if(codepage.Mid(q,2).StartsWith(wxT("8 ")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_8), 256);
		else if(codepage.Mid(q,2).StartsWith(wxT("9 ")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_9), 256);
		else if(codepage.Mid(q,2).StartsWith(wxT("10")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_10), 256);
		else if(codepage.Mid(q,2).StartsWith(wxT("11")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_11), 256);
		else if(codepage.Mid(q,2).StartsWith(wxT("12")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_12), 256);
		else if(codepage.Mid(q,2).StartsWith(wxT("13")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_13), 256);
		else if(codepage.Mid(q,2).StartsWith(wxT("14")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_14), 256);
		else if(codepage.Mid(q,2).StartsWith(wxT("15")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_15), 256);
		//else if(codepage.Mid(q,2).StartsWith(wxT("16")))	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_ISO8859_16), 256);
		}

	// Windows Code Pages
	else if(codepage.Find(wxT("CP125")) not_eq wxNOT_FOUND ){
		for (unsigned i=0; i<=0xFF ; i++)
			bf[i] = (i< 0x20 or i==0x7F or i==0xAD) ? '.' : i;

		//Detecting Encoding
		char q=codepage[codepage.Find(wxT("CP125"))+5];

		//Filtering gaps
		if		 (q=='0') bf[0x81]=bf[0x83]=bf[0x88]=bf[0x90]=bf[0x98]='.';
		else if(q=='1') bf[0x98]='.';
		else if(q=='2') bf[0x81]=bf[0x8D]=bf[0x8F]=bf[0x90]=bf[0x9D]='.';
		else if(q=='3') bf[0x81]=bf[0x88]=bf[0x8A]=bf[0x8C]=bf[0x8D]=
							 bf[0x8E]=bf[0x8F]=bf[0x90]=bf[0x98]=bf[0x9A]=
							 bf[0x9C]=bf[0x9D]=bf[0x9E]=bf[0x9F]=bf[0xAA]=bf[0xD2]=bf[0xFF]='.';
		else if(q=='4') bf[0x81]=bf[0x8D]=bf[0x8E]=bf[0x8F]=bf[0x90]=bf[0x9D]=bf[0x9E]='.';
		else if(q=='5') bf[0x81]=bf[0x88]=bf[0x8A]=bf[0x8C]=bf[0x8D]=
							 bf[0x8E]=bf[0x8F]=bf[0x90]=bf[0x9A]=
							 bf[0x9C]=bf[0x9D]=bf[0x9E]=bf[0x9F]=bf[0xCA]=
							 bf[0xD9]=bf[0xDA]=bf[0xDB]=bf[0xDC]=bf[0xDD]=
							 bf[0xDE]=bf[0xDF]=bf[0xFB]=bf[0xFC]=bf[0xFF]='.';
		else if(q=='7') bf[0x81]=bf[0x83]=bf[0x88]=bf[0x8A]=bf[0x8C]=
							 bf[0x90]=bf[0x98]=bf[0x9A]=bf[0x9C]=bf[0x9F]=bf[0xA1]=bf[0xA5]='.' ;

		//Encoding
		if		(q=='0')		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_CP1250), 256);
		else if(q=='1')	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_CP1251), 256);
		else if(q=='2')	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_CP1252), 256);
		else if(q=='3')	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_CP1253), 256);
		else if(q=='4')	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_CP1254), 256);
		// Hebrew Output not looks good.
		else if(q=='5')	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_CP1255), 256);
		// Arabic Output from right issue!
		else if(q=='6')	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_CP1256), 256);
		else if(q=='7')	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_CP1257), 256);
		else if(q=='8'){ //Windows Vietnamese
			newCP=PrepareCodepageTable(wxT("CP1252")); //ANSI
			newCP[0x8A]=newCP[0x8E]=newCP[0x9A]=newCP[0x9E]='.';
			newCP[0xC3]=wxChar(0x0102);
			newCP[0xCC]='.';//wxChar(0x0300);
			newCP[0xD0]=wxChar(0x0110);
			newCP[0xD2]='.';//wxChar(0x0309);
			newCP[0xD5]=wxChar(0x01A0);
			newCP[0xDD]=wxChar(0x01AF);
			newCP[0xDE]='.';//wxChar(0x0303);
			newCP[0xE3]=wxChar(0x0103);
			newCP[0xEC]='.';//wxChar(0x0301);
			newCP[0xF0]=wxChar(0x0111);
			newCP[0xF2]='.';//wxChar(0x0323);
			newCP[0xF5]=wxChar(0x01A1);
			newCP[0xFD]=wxChar(0x01B0);
			newCP[0xFE]=wxChar(0x20AB);
			}
		}

	else if(codepage.StartsWith(wxT("KOI7"))){
		newCP=PrepareCodepageTable(wxT("ASCII")).Mid(0,0x60);
		newCP += wxT(	"\x042E\x0410\x0411\x0426\x0414\x0415\x0424\x0413\x0425\x0418\x0419\x041A\x041B\x041C\x041D\x041E"\
							"\x041F\x042F\x0420\x0421\x0422\x0423\x0416\x0412\x042C\x042B\x0417\x0428\x042D\x0429\x0427.");
		for (unsigned i=0x80; i<=0xFF ; i++)
			newCP += '.';
		}
	else if(codepage.StartsWith(wxT("KOI8"))){
		for (unsigned i=0; i<=0xFF ; i++)
			bf[i] = (i<0x20 or i==0x7F)	? '.' : i;
		if(codepage.StartsWith(wxT("KOI8-R"))) newCP+=wxString( bf, wxCSConv(wxFONTENCODING_KOI8), 256);
		if(codepage.StartsWith(wxT("KOI8-U"))) newCP+=wxString( bf, wxCSConv(wxFONTENCODING_KOI8_U), 256);
		}



	else if(codepage.StartsWith(wxT("JIS X 0201"))){
		for (unsigned i=0; i<0xFF ; i++)
			if(i<0x80)
				newCP += ((i<0x20 or i==0x7F)	? '.' : wxChar(i));
			else if( i>=0xA1 and i<0xE0)
				newCP += wxChar(i-0xA0+0xFF60);
			else
				newCP +='.';
		}

	else if(codepage.StartsWith(wxT("TIS-620"))){
		newCP=PrepareCodepageTable(wxT("ISO/EIC 8859-11")); //Identical
		}

	else if(codepage.StartsWith(wxT("EBCDIC"))){
		//Control chars replaced with dot
		for (unsigned i=0; i<0x40 ; i++)
			newCP+=wxChar('.');

		/// \x00AD (Soft Hypen) replaced with dot .
		//EBCDIC Table
		newCP+=wxT( "\x0020\x00A0\x00E2\x00E4\x00E0\x00E1\x00E3\x00E5\x00E7\x00F1"\
						"\x005B\x002E\x003C\x0028\x002B\x0021\x0026\x00E9\x00EA\x00EB"\
						"\x00E8\x00ED\x00EE\x00EF\x00EC\x00DF\x005D\x0024\x002A\x0029"\
						"\x003B\x005E\x002D\x002F\x00C2\x00C4\x00C0\x00C1\x00C3\x00C5"\
						"\x00C7\x00D1\x00A6\x002C\x0025\x005F\x003E\x003F\x00F8\x00C9"\
						"\x00CA\x00CB\x00C8\x00CD\x00CE\x00CF\x00CC\x0060\x003A\x0023"\
						"\x0040\x0027\x003D\x0022\x00D8\x0061\x0062\x0063\x0064\x0065"\
						"\x0066\x0067\x0068\x0069\x00AB\x00BB\x00F0\x00FD\x00FE\x00B1"\
						"\x00B0\x006A\x006B\x006C\x006D\x006E\x006F\x0070\x0071\x0072"\
						"\x00AA\x00BA\x00E6\x00B8\x00C6\x00A4\x00B5\x007E\x0073\x0074"\
						"\x0075\x0076\x0077\x0078\x0079\x007A\x00A1\x00BF\x00D0\x00DD"\
						"\x00DE\x00AE\x00A2\x00A3\x00A5\x00B7\x00A9\x00A7\x00B6\x00BC"\
						"\x00BD\x00BE\x00AC\x007C\x00AF\x00A8\x00B4\x00D7\x007B\x0041"\
						"\x0042\x0043\x0044\x0045\x0046\x0047\x0048\x0049.\x00F4"\
						"\x00F6\x00F2\x00F3\x00F5\x007D\x004A\x004B\x004C\x004D\x004E"\
						"\x004F\x0050\x0051\x0052\x00B9\x00FB\x00FC\x00F9\x00FA\x00FF"\
						"\x005C\x00F7\x0053\x0054\x0055\x0056\x0057\x0058\x0059\x005A"\
						"\x00B2\x00D4\x00D6\x00D2\x00D3\x00D5\x0030\x0031\x0032\x0033"\
						"\x0034\x0035\x0036\x0037\x0038\x0039\x00B3\x00DB\x00DC\x00D9\x00DA.");
		}

	else if(codepage.StartsWith(wxT("Mac")) or codepage==wxT("CP10000")){
//		char bf[256];
//		for (unsigned i=0; i<=0xFF ; i++)
//			bf[i] = (i<0x20 or i==0x7F)	? '.' : i;
//		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACROMAN), 256);

		//Control chars replaced with dot
		for (unsigned i=0; i<0x20 ; i++)				newCP+=wxChar('.');
		//ASCII compatible part
		for( unsigned i=0x20 ; i < 0x7F ; i++ )	newCP += wxChar(i);
		//Macintosh Roman CP10000 extension table
		newCP+=wxT(	".\x00C4\x00C5\x00C7\x00C9\x00D1\x00D6\x00DC\x00E1\x00E0\x00E2"\
						"\x00E4\x00E3\x00E5\x00E7\x00E9\x00E8\x00EA\x00EB\x00ED\x00EC"\
						"\x00EE\x00EF\x00F1\x00F3\x00F2\x00F4\x00F6\x00F5\x00FA\x00F9"\
						"\x00FB\x00FC\x2020\x00B0\x00A2\x00A3\x00A7\x2022\x00B6\x00DF"\
						"\x00AE\x00A9\x2122\x00B4\x00A8\x2260\x00C6\x00D8\x221E\x00B1"\
						"\x2264\x2265\x00A5\x00B5\x2202\x2211\x220F\x03C0\x222B\x00AA"\
						"\x00BA\x03A9\x00E6\x00F8\x00BF\x00A1\x00AC\x221A\x0192\x2248"\
						"\x2206\x00AB\x00BB\x2026\x00A0\x00C0\x00C3\x00D5\x0152\x0153"\
						"\x2013\x2014\x201C\x201D\x2018\x2019\x00F7\x25CA\x00FF\x0178"\
						"\x2044\x20AC\x2039\x203A\xFB01\xFB02\x2021\x00B7\x201A\x201E"\
						"\x2030\x00C2\x00CA\x00C1\x00CB\x00C8\x00CD\x00CE\x00CF\x00CC"\
						"\x00D3\x00D4\xF8FF\x00D2\x00DA\x00DB\x00D9\x0131\x02C6\x02DC"\
						"\x00AF\x02D8\x02D9\x02DA\x00B8\x02DD\x02DB\x02C7" );
		}

	else if(codepage.StartsWith(wxT("UTF8"))){
		newCP=wxEmptyString;
		}

	else if(codepage.StartsWith(wxT("GB2312"))){
		newCP=wxEmptyString;
		}
	Codepage=codepage;
	return CodepageTable=newCP;
	}


void wxHexTextCtrl::Replace(unsigned text_location, const wxChar& value, bool paint){
	if( text_location < m_text.Length() )
		m_text[text_location] = Filter(value);
//		m_text[text_location] = value;
	else{
		m_text << Filter(value);
//		m_text << value;
		//m_text << wxT("0");
		}
	RePaint();
	}

void wxHexTextCtrl::ChangeValue( const wxString& value, bool paint ){
	m_text = value;
	if( paint )
		RePaint();
	}

void wxHexTextCtrl::SetBinValue( char* buffer, int len, bool paint ){
	m_text.Clear();
	if(Codepage.StartsWith(wxT("UTF8")))
		m_text << FilterMBBuffer(buffer,len,wxFONTENCODING_UTF8);
	else if(Codepage.StartsWith(wxT("GB2312")))
		m_text << FilterMBBuffer(buffer,len,wxFONTENCODING_GB2312);
	else if(Codepage.StartsWith(wxT("TSCII")))
		m_text << FilterMBBuffer(buffer,len,0);
	else
		for( unsigned i=0 ; i<len ; i++ )
			m_text << Filter(buffer[i]);

//	m_text << FilterUTF8(buffer,len);
//	m_text=wxString(buffer, wxCSConv(wxFONTENCODING_CP1252),  len);
//	m_text=wxString(buffer, wxCSConv(wxFONTENCODING_UTF8),  len);

	if( paint )
		RePaint();
	}

void wxHexTextCtrl::SetDefaultStyle( wxTextAttr& new_attr ){
	HexDefaultAttr = new_attr;

	wxClientDC dc(this);
	dc.SetFont( HexDefaultAttr.GetFont() );
	SetFont( HexDefaultAttr.GetFont() );
	m_CharSize.y = dc.GetCharHeight();
	m_CharSize.x = dc.GetCharWidth();

	wxCaret *caret = GetCaret();
	if ( caret )
		caret->SetSize(1, m_CharSize.y);

	RePaint();
}

int wxHexTextCtrl::PixelCoordToInternalPosition( wxPoint mouse ){
	mouse.x = ( mouse.x < 0 ? 0 : mouse.x);
	mouse.x = ( mouse.x > m_CharSize.x*m_Window.x ? m_CharSize.x*m_Window.x-1 : mouse.x);
	mouse.y = ( mouse.y < 0 ? 0 : mouse.y);
	mouse.y = ( mouse.y > m_CharSize.y*m_Window.y ? m_CharSize.y*m_Window.y-1 : mouse.y);

	int x = (mouse.x - m_Margin.x) / m_CharSize.x;
	int y = (mouse.y - m_Margin.y) / m_CharSize.y;
	return ( x + y * CharacterPerLine() );
	}

int wxHexTextCtrl::GetInsertionPoint( void ){
	return m_Caret.x  + CharacterPerLine() * m_Caret.y;
	}

void wxHexTextCtrl::SetInsertionPoint( unsigned int pos ){
	if(pos > m_text.Length())
		pos = m_text.Length();
	MoveCaret( wxPoint(pos%m_Window.x , pos/m_Window.x) );
	}

void wxHexTextCtrl::ChangeSize(){
	unsigned gip = GetInsertionPoint();
	wxSize size = GetClientSize();

	m_Window.x = (size.x - 2*m_Margin.x) / m_CharSize.x;
	m_Window.y = (size.y - 2*m_Margin.x) / m_CharSize.y;
	if ( m_Window.x < 1 )
		m_Window.x = 1;
	if ( m_Window.y < 1 )
		m_Window.y = 1;

	//This Resizes internal buffer!
	CreateDC();

	RePaint();
	SetInsertionPoint( gip );
	}

void wxHexTextCtrl::DrawCursorShadow(wxDC* dcTemp){
	if( m_Window.x <= 0 or
		FindFocus()==this )
		return;

	#ifdef _DEBUG_CARET_
	std::cout << "DrawCursorShadow(x,y) :" << m_Caret.x << ',' <<  m_Caret.y << std::endl;
	#endif // _DEBUG_CARET_

	int y=m_CharSize.y*( m_Caret.y ) + m_Margin.y;
	int x=m_CharSize.x*( m_Caret.x ) + m_Margin.x;

	dcTemp->SetPen( *wxBLACK_PEN );
	dcTemp->SetBrush( *wxTRANSPARENT_BRUSH );
	dcTemp->DrawRectangle(x,y,m_CharSize.x+1,m_CharSize.y);
	}
///------HEXOFFSETCTRL-----///
void wxHexOffsetCtrl::SetValue( uint64_t position ){
	SetValue( position, BytePerLine );
	}

void wxHexOffsetCtrl::SetValue( uint64_t position, int byteperline ){
	offset_position = position;
	BytePerLine = byteperline;
	m_text.Clear();

   wxString format=GetFormatString();

	wxULongLong_t ull = ( offset_position );
	if( offset_mode == 's' ){//Sector Indicator!
		for( int i=0 ; i<LineCount() ; i++ ){
			m_text << wxString::Format( format, (1+ull/sector_size), ull%sector_size );
			//m_text << GetFormatedOffsetString( ull );
			ull += BytePerLine;
			}
		}
	else
		for( int i=0 ; i<LineCount() ; i++ ){
			m_text << wxString::Format( format, ull );
			ull += BytePerLine;
			}
	RePaint();
	}

wxString wxHexOffsetCtrl::GetFormatedOffsetString( uint64_t c_offset, bool minimal ){
   if(offset_mode=='s')
		return wxString::Format( GetFormatString(minimal), (1+c_offset/sector_size), c_offset%sector_size );
	return wxString::Format( GetFormatString(minimal), c_offset );
	}

wxString wxHexOffsetCtrl::GetFormatString( bool minimal ){
   wxString format;
   if(offset_mode=='s'){
   	int sector_digit=0;
   	int offset_digit=0;
   	if(not minimal){
			while((1+offset_limit/sector_size) > pow(10,++sector_digit));
			while(sector_size > pow(10,++offset_digit));
			}
		format << wxT("%0") << sector_digit << wxLongLongFmtSpec << wxT("u:%0") << offset_digit << wxT("u");
		return format;
		}
	format << wxT("%0") <<
			(minimal? 0 : GetDigitCount())
			<< wxLongLongFmtSpec << wxChar( offset_mode );
   if( offset_mode=='X' )
        format << wxChar('h');
	else if ( offset_mode=='o')
        format << wxChar('o');
	return format;
    }

void wxHexOffsetCtrl::OnMouseRight( wxMouseEvent& event ){
	switch( offset_mode ){
        case 'u': offset_mode = 'X'; break;
        case 'X': offset_mode = 'o'; break;
        case 'o': offset_mode = (sector_size ? 's' :'u'); break;
        case 's': offset_mode = 'u'; break;
        default : offset_mode = 'u';
        }

	wxString s= wxChar( offset_mode );
   wxConfigBase::Get()->Write( _T("LastOffsetMode"), s);

	SetValue( offset_position );
	}

void wxHexOffsetCtrl::OnMouseLeft( wxMouseEvent& event ){
	wxPoint p = PixelCoordToInternalCoord( event.GetPosition() );
	uint64_t adress = offset_position + p.y*BytePerLine;
	wxString adr;
	if(offset_mode=='s')
		adr = wxString::Format( GetFormatString(), (1+adress/sector_size), adress%sector_size);
	else
		adr = wxString::Format( GetFormatString(), adress);

	if(wxTheClipboard->Open()) {
		wxTheClipboard->Clear();
		if( not wxTheClipboard->SetData( new wxTextDataObject( adr )) );
			wxBell();
		wxTheClipboard->Flush();
		wxTheClipboard->Close();
		}
	}

unsigned wxHexOffsetCtrl::GetDigitCount( void ){
	digit_count=0;
	int base=0;
   switch( offset_mode){
        case 'u': base=10; break;
        case 'X': base=16; break;
        case 'o': base= 8; break;
        case 's': base=10; break;
        }
	if( offset_mode=='s'){
		int digit_count2=0;
		while(1+(offset_limit/sector_size) > pow(base,++digit_count));
		while(sector_size > pow(base,++digit_count2));
		digit_count+=digit_count2;
		}

	while(offset_limit > pow(base,++digit_count));
	if( digit_count < 6)
		digit_count=6;

	return digit_count;
	}

unsigned wxHexOffsetCtrl::GetLineSize( void ){
        unsigned line_size = GetDigitCount();
	    if( offset_mode=='X' or offset_mode=='o' )
            line_size++;
        return line_size;
        }
