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


wxString CP473T1=wxT(" \x263A\x263B\x2665\x2666\x2663\x2660\x2022"\
							"\x25D8\x25CB\x25D9\x2642\x2640\x266A\x266B\x263C"\
							"\x25BA\x25C4\x2195\x203C\x00B6\x00A7\x25AC\x21A8"\
							"\x2191\x2193\x2192\x2190\x221F\x2194\x25B2\x25BC");

wxString CP473T2=wxT("\x2302\x00C7\x00FC\x00E9\x00E2\x00E4\x00E0\x00E5"\
							"\x00E7\x00EA\x00EB\x00E8\x00EF\x00EE\x00EC\x00C4"\
							"\x00C5\x00C9\x00E6\x00C6\x00F4\x00F6\x00F2\x00FB"\
							"\x00F9\x00FF\x00D6\x00DC\x00A2\x00A3\x00A5\x20A7"\
							"\x0192\x00E1\x00ED\x00F3\x00FA\x00F1\x00D1\x00AA"\
							"\x00BA\x00BF\x2310\x00AC\x00BD\x00BC\x00A1\x00AB"\
							"\x00BB\x2591\x2592\x2593\x2502\x2524\x2561\x2562"\
							"\x2556\x2555\x2563\x2551\x2557\x255D\x255C\x255B"\
							"\x2510\x2514\x2534\x252C\x251C\x2500\x253C\x255E"\
							"\x255F\x255A\x2554\x2569\x2566\x2560\x2550\x256C"\
							"\x2567\x2568\x2564\x2565\x2559\x2558\x2552\x2553"\
							"\x256B\x256A\x2518\x250C\x2588\x2584\x258C\x2590"\
							"\x2580\x03B1\x00DF\x0393\x03C0\x03A3\x03C3\x00B5"\
							"\x03C4\x03A6\x0398\x03A9\x03B4\x221E\x03C6\x03B5"\
							"\x2229\x2261\x00B1\x2265\x2264\x2320\x2321\x00F7"\
							"\x2248\x00B0\x2219\x00B7\x221A\x207F\x00B2\x25A0\x00A0");

inline wxChar CP473toUnicodeCHR( const unsigned char& chr){
	if(chr<0x20) return CP473T1[chr];
	if(chr>0x7E) return CP473T2[chr-0x7F];
	return chr;
	}

inline wxString CP473toUnicode( wxString& line ){
	wxString ret;
	for(unsigned i=0; i < line.Len() ; i++){
		if(line[i]<0x20)
			ret+=CP473T1[line[i]];
		else if(line[i]>0x7E)
			ret+=CP473T2[line[i]-0x7F];
		else
			ret+=line[i];
		}
	return ret;
	}

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

		#ifndef __WXGTK__
		//Error on WXGTK. Need test on other platforms.
		line = wxString(line.To8BitData(), wxCSConv(wxFONTENCODING_CP437), line.Len());
		#else
		//line = CP473toUnicode(line);
		#endif

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
inline bool wxHexTextCtrl::IsAllowedChar(const unsigned char& chr){
	return ((chr !=173) && ( (chr>31 && chr<127) || chr>159));	//Visible Char filter
	//return true;
	}

wxChar wxHexTextCtrl::Filter(const unsigned char& ch){
	return ch;
	if( IsAllowedChar(ch) )
		return ch;
	else
		return '.'; //Special Character '?'
	}
*/

void wxHexTextCtrl::Replace(unsigned text_location, const wxChar& value, bool paint){
	if( text_location < m_text.Length() )
//		m_text[text_location] = Filter(value);
		m_text[text_location] = value;
	else{
//		m_text << Filter(value);
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

void wxHexTextCtrl::SetBinValue( char* buffer, int len, bool paint ){
	m_text.Clear();
	for( unsigned i=0 ; i<len ; i++ )
//		text_string << text_ctrl->Filter(buffer[i]);
//		text_string << static_cast<wxChar>((unsigned char)(buffer[i]));
		m_text << CP473toUnicodeCHR( buffer[i] );
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
