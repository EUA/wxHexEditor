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
#include <wx/fontmap.h>

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

	size_t textLenghtLimit = 0;
	size_t textLength=m_text.Length();
//	char bux[1000];  //++//
/*** Hex to Color Engine Prototype
   char chr;
   unsigned char chrP,chrC;
   chrP=0;
	if(1)
	for ( int y = 0 ; y < m_Window.y; y++ ){
		for ( int x = 0 ; x < m_Window.x; x++ ){
			if( IsDenied(x)){
				chr=' ';
				dcTemp->DrawText( wxString::From8BitData(&chr), m_Margin.x + x*m_CharSize.x, m_Margin.y + y * m_CharSize.y );
				//bux[x]=' ';//++//
				continue;
				}
			if(textLenghtLimit >= textLength)
				break;
	//		bux[x]=CharAt(z);//++//
			chr = CharAt(textLenghtLimit++);

			if( not (chrP++ % 2) )
			   chrC = (atoh( chr ) << 4) | atoh( CharAt(textLenghtLimit) );

			unsigned char R=(chrC>>5)*0xFF/7;
			unsigned char G=(0x07 & (chrC>>2))*0xFF/7;
			unsigned char B=(0x03 & chrC)*0xFF/3;
		   dcTemp->SetTextBackground( wxColour(R,G,B) );
			dcTemp->DrawText( wxString::From8BitData(&chr), m_Margin.x + x*m_CharSize.x, m_Margin.y + y * m_CharSize.y );
			}
		}
	else
//*/
	for ( int y = 0 ; y < m_Window.y; y++ ){	//Draw base hex value without color tags
		line.Empty();

		if (*ZebraStriping != -1 )
			dcTemp->SetTextBackground( (y+*ZebraStriping)%2 ? col_standart : col_zebra);

		for ( int x = 0 ; x < m_Window.x; x++ ){
			if( IsDenied(x)){
				line += wxT(' ');
				//bux[x]=' ';//++//
				continue;
				}
			if(textLenghtLimit >= textLength)
				break;
	//		bux[x]=CharAt(z);//++//
			line += CharAt(textLenghtLimit++);
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

	if(ThinSeparationLines.Count() > 0)
		for( unsigned i=0 ; i < ThinSeparationLines.Count() ; i++)
			DrawSeperationLineAfterChar( dcTemp, ThinSeparationLines.Item(i) );

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


void wxHexCtrl::DrawSeperationLineAfterChar( wxDC* dcTemp, int seperationoffset ){
#ifdef _DEBUG_
		std::cout << "DrawSeperatıonLineAfterChar(" <<  seperationoffset << ")" << std::endl;
#endif

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

inline wxString wxHexTextCtrl::FilterMBBuffer( const char *str, int Len, int fontenc ){
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
			else if( ch == 0x7F) ret+='.';						// Control character
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

	else if(fontenc==wxFONTENCODING_UTF16){
		for( int i=0 ; i< Len-1 ; i+=2){
			z+=wxString( str+i, wxCSConv(wxFONTENCODING_UTF16), 2)+wxT(" ");
			if( z.Len() > 0){
				ret+=z;
				if( z.Len() == 1 )
					ret+=' ';
					i+=1;
					}
			else
				ret += wxT("  ");
			}
		}

	else if(fontenc==wxFONTENCODING_UTF16LE)
		for( int i=0 ; i< Len-1 ; i+=2){
			ret+=wxString( str+i, wxCSConv(wxFONTENCODING_UTF16LE), 2)+wxT(" ");
		}

	else if(fontenc==wxFONTENCODING_UTF16BE)
		for( int i=0 ; i< Len-1 ; i+=2)
			ret+=wxString( str+i, wxCSConv(wxFONTENCODING_UTF16BE), 2)+wxT(" ");


	else if(fontenc==wxFONTENCODING_UTF32)
		for( int i=0 ; i< Len-3 ; i+=4)
			ret+=wxString( str+i, wxCSConv(wxFONTENCODING_UTF32), 4)+wxT("   ");

	else if(fontenc==wxFONTENCODING_UTF32LE){
		}

	else if(fontenc==wxFONTENCODING_UTF32BE){
		}

	else if(fontenc==wxFONTENCODING_UNICODE){
		}

	else if(fontenc==wxFONTENCODING_SHIFT_JIS){
		//wxCSConv m_SJISConv = wxCSConv(wxFONTENCODING_SHIFT_JIS); //error on linux
		wxCSConv SJISConv = wxCSConv(wxT("SHIFT-JIS"));
		for( int i=0 ; i< Len ; i++){
			unsigned char ch = str[i];
			if(ch < 0x20) ret+='.';									//Control characters
			else if( (ch >= 0x20 and ch <= 0x7E) or			//ASCII
						(ch > 0xA0 and ch < 0xE0 )){				//One-byte JIS X 0208 character
				ret+=wxString( str+i, SJISConv, 1);
				//ret+='O';//for debug
				}
			else if(ch < 0x81 or ch >= 0xF0 or ch==0xA0) ret+='.';	// Void Characters
			else if(Len>i){ 											//First byte of a double-byte JIS X 0208 character
				ch = str[i+1];											//Fetching second character.
				if( ch>=0x40 and ch!=0x7F and ch<=0xFC ){		//Second byte of a double-byte JIS X 0208 character
					z=wxString( str+i, SJISConv, 2);
					if(z.Len()>0){
						ret+=z + wxT(" ");
						i++;
						}
					else
						ret+='.';
					//ret+=wxString( str+i, wxCSConv(wxFONTENCODING_SHIFT_JIS), 2);
					//ret+='X';//for debug
					}
				else
					ret+='.'; // First character is fit but second doesn't. So this is void character
				}
			}
		}

	else if(fontenc==wxFONTENCODING_CP949){//"EUC-KR"
		for( int i=0 ; i< Len ; i++){
			unsigned char ch = str[i];
			if(ch>=0x21 and ch<=0x7E)	ret+=wxChar(ch);		//ASCII part
			else if(ch>=0xA1 and ch<=0xFE and Len>i ){ // KS X 1001 (G1, code set 1) is encoded as two bytes in GR (0xA1-0xFE)
				ch=str[i+1];
				if(ch>=0xA1 and ch<=0xFE){
					z=wxString( str+i, wxCSConv(wxFONTENCODING_CP949), 2);
					if(z.Len()>0){
						ret+=z + wxT(" ");
						i++;
						}
					else
						ret+='.';
					}
				else
					ret+='.';
				}
			else
				ret+='.';
			}
		}

	else if(fontenc==wxFONTENCODING_BIG5){
//		wxCSConv Big5Conv = wxCSConv(wxFONTENCODING_BIG5); //Error on linux
		wxCSConv Big5Conv = wxCSConv(wxT("BIG5"));
//		First byte ("lead byte") 	0x81 to 0xfe (or 0xa1 to 0xf9 for non-user-defined characters)
//		Second byte 	0x40 to 0x7e, 0xa1 to 0xfe
		for( int i=0 ; i< Len ; i++){
			unsigned char ch = str[i];
			if(ch<0x20 or ch==0x7F or ch==0x80 or ch==0xFF )
				ret+='.';
			else if( (ch >= 0x81 and ch <= 0xFE) or	//1.st byte
				 (ch >= 0xA1 and ch < 0xF9 ) ){
				ch = str[i+1];
				if((ch >= 0x40 and ch <= 0x7E) or		//2.nd byte
					(ch >= 0xA1 and ch <= 0xFE )){
					z=wxString(str+i, Big5Conv, 2);
					if( z.Len() > 0){
						ret+=z;
						ret+=wxT(" ");
						i+=1;
					}
					else
						ret+='.';
					}
				else
					ret+='.';
				}
			else
				ret+=wxString(str+i, Big5Conv, 1);
			}
		}

	else if(fontenc==wxFONTENCODING_CP936){//"GBK"
///		range 		byte 1 	byte 2 				cp 		GB 18030 GBK 1.0 	CP936 	GB 2312
///	Level GBK/1 	A1–A9 	A1–FE 				846 		728 		717 		702 		682
///	Level GBK/2 	B0–F7 	A1–FE 				6,768 	6,763 	6,763 	6,763		6,763
///	Level GBK/3 	81–A0 	40–FE except 7F 	6,080 	6,080		6,080 	6,080
///	Level GBK/4 	AA–FE 	40–A0 except 7F 	8,160 	8,160 	8,160 	8,080
///	Level GBK/5 	A8–A9 	40–A0 except 7F 	192 		166 		166 		166
///	user-defined 	AA–AF 	A1–FE 				564
///	user-defined 	F8–FE 	A1–FE 				658
///	user-defined 	A1–A7 	40–A0 except 7F 	672
///	total: 												23,940 	21,897 	21,886 	21,791 	7,445
		for( int i=0 ; i< Len ; i++){
			unsigned char ch1 = str[i];
			if(ch1>=0x21 and ch1<=0x7E)	ret+=wxChar(ch1); 	//ASCII part
			else if(ch1>=0xA1 and ch1<=0xFE and Len>i ){ 	// KS X 1001 (G1, code set 1) is encoded as two bytes in GR (0xA1-0xFE)
				unsigned char ch2=str[i+1];
		//		if(ch>=0x40 and ch<=0xFE){
				if(
					(ch1>=0xA1 and ch1<=0xA9 and ch2>=0xA1 and ch2<=0xFE ) or //GBK/1
					(ch1>=0xB0 and ch1<=0xF7 and ch2>=0xA1 and ch2<=0xFE ) or //GBK/2
					(ch1>=0x81 and ch1<=0xA0 and ch2>=0x40 and ch2<=0xFE and ch2!=0x7F) or //GBK/3
					(ch1>=0xAA and ch1<=0xFE and ch2>=0x40 and ch2<=0xA0 and ch2!=0x7F ) or //GBK/4
					(ch1>=0xA8 and ch1<=0xA9 and ch2>=0x40 and ch2<=0xA0 and ch2!=0x7F ) or //GBK/5
					(ch1>=0xAA and ch1<=0xAF and ch2>=0xA1 and ch2<=0xFE) or //user-defined
					(ch1>=0xF8 and ch1<=0xFE and ch2>=0xA1 and ch2<=0xFE) or //user-defined
					(ch1>=0xA1 and ch1<=0xA7 and ch2>=0x40 and ch2<=0xA0 and ch2!=0x7F) //user-defined
					){
					z=wxString( str+i, wxCSConv(wxFONTENCODING_CP949), 2);
					if(z.Len()>0){
						ret+=z + wxT(" ");
						i++;
						}
					else
						ret+='.';
					}
				else
					ret+='.';
				}
			else
				ret+='.';
			}
		}

//	else if(fontenc==wxFONTENCODING_GB2312)
//		for( int i=0 ; i< Len ; i++){
//			unsigned char ch = str[i];
//			if(ch < 0x20) ret+='.';									// Control characters
//			else if( ch >= 0x20 and ch <= 0x7E ) ret+=ch;	// ASCII compatible part
//			else if(ch < 0x80) ret+='.';							// Void Characters
//			else { // 0x21 + 0x80
//				z=wxString( str+i, wxCSConv(wxFONTENCODING_GB2312), 2);
//				if( z.Len() > 0){
//					ret+=z;
//					if( z.Len() == 1 )
//						ret+=' ';
//					i+=1;
//					}
//				else
//					ret+='.';
//				}
//		}

	else if(fontenc==wxFONTENCODING_EUC_JP){
		wxCSConv EUCJPConv = wxCSConv(wxFONTENCODING_EUC_JP);
		//wxCSConv EUCJPConv = wxCSConv(wxT("EUC-JP"));

    ///A character from the lower half of JIS-X-0201 (ASCII, code set 0) is represented by one byte, in the range 0x21 – 0x7E.
    ///A character from the upper half of JIS-X-0201 (half-width kana, code set 2) is represented by two bytes, the first being 0x8E, the second in the range 0xA1 – 0xDF.
    ///A character from JIS-X-0208 (code set 1) is represented by two bytes, both in the range 0xA1 – 0xFE.
    ///A character from JIS-X-0212 (code set 3) is represented by three bytes, the first being 0x8F, the following two in the range 0xA1 – 0xFE.
		for( int i=0 ; i< Len ; i++){
			unsigned char ch = str[i];
			if(ch<0x20 or ch==0x7F )			ret+='.';				//Control chars
			else if(ch>=0x21 and ch<=0x7E)	ret+=wxChar(ch);		//ASCII part
			else if(ch==0x8E and Len > i){								//half-width kana first byte
				ch = str[i+1];
				if(ch>=0xA1 and ch<=0xDF)									//half-width kana second byte
					ret+=wxString(str+i++, EUCJPConv, 2) + wxT(" ");
				else
					ret+='.';
				}
			else if(ch>=0xA1 and ch <=0xFE){		//JIS-X-0208 first byte
				ch = str[i+1];
				if(ch>=0xA1 and ch <=0xFE){		//JIS-X-0208 second byte
					z=wxString(str+i, EUCJPConv, 2);
					if(z.Len()>0){
						ret+=z + wxT(" ");
						i++;
						}
					else
						ret+='.';
					}
				else
					ret+='.';
				}
			else if(ch==0x8F and (Len>i+1)){	//JIS-X-0212 first byte
				unsigned char ch1=str[i+1];
				unsigned char ch2=str[i+2];
				if((ch1>=0xA1 and ch1<=0xFE) and
					(ch2>=0xA1 and ch2<=0xFE)){	//JIS-X-0212 second byte
					ret+=wxString(str+i, EUCJPConv, 3) + wxT("  ");
					i+=2;
					}
				else
					ret+='.';
				}
			else
				ret+='.';
			}
		}

	else if(Codepage.StartsWith(wxT("TSCII"))){
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

//wxArrayString wxHexTextCtrl::GetSupportedEncodings(void){
//	return GetSupportedEncodings(void);
//	}

wxArrayString GetSupportedEncodings(void){
	wxArrayString AvailableEncodings;
	wxString CharacterEncodings[] = {
					wxT("ASCII - American Standard Code for Information Interchange"),
					wxT("*ISCII - Indian Script Code for Information Interchange"),
					wxT("KOI7 Код Обмена Информацией, 7 бит"),
					wxT("KOI8-R Код Обмена Информацией, 8 бит"),
					wxT("KOI8-U Код Обмена Информацией, 8 бит"),
					wxT("ISO/IEC 6937"),
					wxT("ISO/IEC 8859-1 Latin-1 Western European"),
					wxT("ISO/IEC 8859-2 Latin-2 Central European"),
					wxT("ISO/IEC 8859-3 Latin-3 South European"),
					wxT("ISO/IEC 8859-4 Latin-4 North European"),
					wxT("ISO/IEC 8859-5 Latin/Cyrillic"),
					wxT("*ISO/IEC 8859-6 Latin/Arabic"),
					wxT("ISO/IEC 8859-7 Latin/Greek"),
					wxT("*ISO/IEC 8859-8 Latin/Hebrew"),
					wxT("ISO/IEC 8859-9 Latin/Turkish"),
					wxT("ISO/IEC 8859-10 Latin/Nordic"),
					wxT("*ISO/IEC 8859-11 Latin/Thai"),
					wxT("ISO/IEC 8859-13 Latin-7 Baltic Rim"),
					wxT("ISO/IEC 8859-14 Latin-8 Celtic"),
					wxT("ISO/IEC 8859-15 Latin-9"),
					wxT("ISO/IEC 8859-16 Latin-10 South-Eastern European"),
					wxT("*Windows CP874 - Thai"),
					wxT("Windows CP1250 - Central and Eastern European"),
					wxT("Windows CP1251 - Cyrillic Script"),
					wxT("Windows CP1252 - ANSI"),
					wxT("Windows CP1253 - Greek Modern"),
					wxT("Windows CP1254 - Turkish"),
					wxT("*Windows CP1255 - Hebrew"),
					wxT("*Windows CP1256 - Arabic"),
					wxT("Windows CP1257 - Baltic"),
					wxT("Windows CP1258 - Vietnamese"),
					wxT("VSCII - Vietnamese Standard Code for Information Interchange"),
					wxT("*TSCII - Tamil Script Code for Information Interchange"),
					wxT("*JIS X 0201 - Japanese Industrial Standard "),
					wxT("*TIS-620 - Thai Industrial Standard 620-2533"),
					wxT("EBCDIC  037 - IBM U.S. Canada"),
					wxT("EBCDIC  285 - IBM Ireland U.K."),
					wxT("EBCDIC  424 - IBM Hebrew"),
					wxT("EBCDIC  500 - IBM International"),
					wxT("EBCDIC  875 - IBM Greek"),
					wxT("EBCDIC 1026 - IBM Latin 5 Turkish"),
					wxT("EBCDIC 1047 - IBM Latin 1"),
					wxT("EBCDIC 1140 - IBM U.S. Canada with €"),
					wxT("EBCDIC 1146 - IBM Ireland U.K. with €"),
					wxT("EBCDIC 1148 - IBM International with €"),
					wxT("*ANSEL - American National Standard for Extended Latin"),
					wxT("DEC Multinational Character Set - VT220"),
					wxT("OEM - IBM PC/DOS CP437 - MS-DOS Latin US"),
					wxT("*PC/DOS CP720 - MS-DOS Arabic"),
					wxT("PC/DOS CP737 - MS-DOS Greek"),
					wxT("PC/DOS CP775 - MS-DOS Baltic Rim"),
					wxT("PC/DOS CP850 - MS-DOS Latin 1"),
					wxT("PC/DOS CP852 - MS-DOS Latin 2"),
					wxT("PC/DOS CP855 - MS-DOS Cyrillic"),
					wxT("*PC/DOS CP856 - Hebrew"),
					wxT("PC/DOS CP857 - MS-DOS Turkish"),
					wxT("PC/DOS CP858 - MS-DOS Latin 1 Update"),
					wxT("PC/DOS CP860 - MS-DOS Portuguese"),
					wxT("PC/DOS CP861 - MS-DOS Icelandic"),
					wxT("*PC/DOS CP862 - MS-DOS Hebrew"),
					wxT("PC/DOS CP863 - MS-DOS French Canada"),
					wxT("*PC/DOS CP864 - MS-DOS Arabic 2"),
					wxT("PC/DOS CP866 - MS-DOS Cyrillic Russian"),
					wxT("PC/DOS CP869 - MS-DOS Greek 2"),
					wxT("PC/DOS CP1006 - Arabic"),
					wxT("PC/DOS KZ-1048 - Kazakhstan"),
					wxT("PC/DOS MIK Code page"),
					wxT("PC/DOS Kamenický Encoding"),
					wxT("PC/DOS Mazovia Encoding"),
					wxT("*PC/DOS Iran System Encoding Standard"),
					wxT("*Big5"),
					wxT("*GBK - GB2312 - Guojia Biaozhun (国家标准)"),
					wxT("UTF8 - Universal Character Set"),
					wxT("UTF16 - Universal Character Set"),
					wxT("UTF16LE - Universal Character Set"),
					wxT("UTF16BE - Universal Character Set"),
					wxT("UTF32 - Universal Character Set"),
					wxT("UTF32LE - Universal Character Set"),
					wxT("UTF32BE - Universal Character Set"),
					wxT("Unicode"),
					wxT("*EUC-JP Extended Unix Code for Japanese"),
					wxT("*EUC-KR Extended Unix Code for Korean"),
					wxT("*Shift JIS"),

					wxT("Macintosh CP10000 - MacRoman"),
					wxT("Macintosh CP10007 - MacCyrillic"),
					wxT("Macintosh CP10006 - MacGreek"),
					wxT("Macintosh CP10079 - MacIcelandic"),
					wxT("Macintosh CP10029 - MacLatin2"),
					wxT("Macintosh CP10081 - MacTurkish"),

		#ifdef __WXMAC__
					wxT("Macintosh Arabic"),
					wxT("Macintosh Celtic"),
					wxT("Macintosh Central European"),
					wxT("Macintosh Croatian"),
					wxT("Macintosh Cyrillic"),
					wxT("Macintosh Devanagari"),
					wxT("Macintosh Dingbats"),
					wxT("Macintosh Gaelic"),
					wxT("Macintosh Greek"),
					wxT("Macintosh Gujarati"),
					wxT("Macintosh Gurmukhi"),
					wxT("Macintosh Hebrew"),
					wxT("Macintosh Icelandic"),
					wxT("Macintosh Inuit"),
					wxT("Macintosh Keyboard"),
					wxT("Macintosh Roman"),
					wxT("Macintosh Romanian"),
					wxT("Macintosh Symbol"),
					wxT("Macintosh Thai"),
					wxT("Macintosh Tibetan"),
					wxT("Macintosh Turkish"),
					wxT("Macintosh Ukraine"),
			#endif

					wxT("*AtariST"),
					wxT("*Windows CP932 - Japanese (Shift JIS)"),
					wxT("*Windows CP936 - Chinese Simplified (GBK)"),
					wxT("*Windows CP949 - Korean (EUC-KR)"),
					wxT("*Windows CP950 - Chinese Traditional (Big5)")
					};

		int EncCnt = sizeof( CharacterEncodings ) / sizeof( wxString );
		for( int i=0 ; i< EncCnt ; i++ )
			AvailableEncodings.Add(CharacterEncodings[i]);
		return AvailableEncodings;
		}

wxString wxHexTextCtrl::PrepareCodepageTable(wxString codepage){
/****Python script for fetch Code page tables*********
import urllib,sys
def cpformat( a ):
   q = urllib.urlopen( a ).read().split('\n')
   w=[i.split('\t') for i in q]
   e=[i.split('\t') for i in q if not i.startswith('#')]
   r=[i[1] for i in e if len(i)>1]
   z=0
   sys.stdout.write('wxT("')
   for i in r[0x00:]:
      if not z%0x10 and z!=0:
         sys.stdout.write('"\\\n"')
      if i=='':
         sys.stdout.write('.')
      elif i[2:4]=='00':
         sys.stdout.write('\\x'+i[4:].upper())
      else:
         sys.stdout.write('\\x'+i[2:].upper())
      z+=1
   sys.stdout.write('" );\n')

a='http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/PC/CP437.TXT'
cpformat(a)
*******************************************************/
	Codepage=codepage;
	wxString newCP;
	FontEnc=wxFONTENCODING_ALTERNATIVE;
	char bf[256];
	if(codepage.Find(wxT("ASCII")) not_eq wxNOT_FOUND){
		for (unsigned i=0; i<=0xFF ; i++){
			if(i<0x20 or i>=0x7F)		newCP+='.';		  //Control chars replaced with dot
			if(i>=0x20 and i<0x7F)		newCP+=wxChar(i);//ASCII region
			}
		}

	else if(codepage.Find(wxT("ANSEL")) not_eq wxNOT_FOUND){
		for (unsigned i=0; i<0xA1 ; i++){
			if(i<0x20 or i>=0x7F)		newCP+='.';		  //Control chars and voids replaced with dot
			if(i>=0x20 and i<0x7F)		newCP+=wxChar(i);//ASCII region
			}
		newCP+=wxT(	"\x0141\x00D8\x0110\x00DE\x00C6\x0152\x02B9\x00B7\x266D\x00AE\x00B1\x01A0\x01AF\x02BC."\
						"\x02BB\x0142\x00F8\x0111\x00FE\x00E6\x0153\x02BA\x0131\x00A3\x00F0.\x01A1\x01B0.."\
						"\x00B0\x2113\x2117\x00A9\x266F\x00BF\x00A1" );

		for (unsigned i=0xC7; i<0xE0 ; i++) newCP+='.';//Void Region
		newCP+=wxT(	"\x0303\x0300\x0301\x0302\x0303\x0304\x0306\x0307\x0308\x030C\x030A\xFE20\xFE21\x0315\x030B"\
						"\x0310\x0327\x0328\x0323\x0324\x0325\x0333\x0332\x0326\x031C\x032E\xFE22\xFE23..\x0313");
		}

	//Indian Script Code for Information Interchange
	else if(codepage.Find(wxT("ISCII")) not_eq wxNOT_FOUND){
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
	else if(codepage.Find(wxT("TSCII")) not_eq wxNOT_FOUND){
		newCP=PrepareCodepageTable(wxT("ASCII")).Mid(0,0x80);
		///		0x82 4Byte
		///		0x87 3Byte
		///		0x88->0x8B 2Byte
		///		0x8C 4Byte
		///		0xCA->0xFD 2Byte
		///		0x99->0x9C 2Byte
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
		}

	else if(codepage.Find(wxT("VSCII")) not_eq wxNOT_FOUND){
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

	// OEM PC/DOS
	else if(codepage.Find(wxT("DOS")) not_eq wxNOT_FOUND ){
		//CP437 Control Symbols
		newCP=wxT(  "\x20\x263A\x263B\x2665\x2666\x2663\x2660\x2022\x25D8\x25CB\x25D9"\
						"\x2642\x2640\x266A\x266B\x263C\x25BA\x25C4\x2195\x203C\x00B6\x00A7"\
						"\x25AC\x21A8\x2191\x2193\x2192\x2190\x221F\x2194\x25B2\x25BC" );

		//ASCII compatible part
		for( unsigned i=0x20 ; i < 0x7F ; i++ )
			newCP += wxChar(i);

		newCP+=wxChar(0x2302); //0x7F symbol

		for (unsigned i=0x80; i<=0xFF ; i++)
			bf[i-0x80] =  wxChar(i);

		if((codepage.Find(wxT("CP437")) not_eq wxNOT_FOUND ) or //Extended ASCII region of CP437
		  (codepage.Find(wxT("OEM")) not_eq wxNOT_FOUND ))
//			newCP+=wxString( bf, wxCSConv(wxT("CP437")), 0x80);
			newCP+=wxT(	"\xC7\xFC\xE9\xE2\xE4\xE0\xE5\xE7\xEA\xEB\xE8\xEF\xEE\xEC\xC4\xC5"\
							"\xC9\xE6\xC6\xF4\xF6\xF2\xFB\xF9\xFF\xD6\xDC\xA2\xA3\xA5\x20A7\x0192"\
							"\xE1\xED\xF3\xFA\xF1\xD1\xAA\xBA\xBF\x2310\xAC\xBD\xBC\xA1\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524\x2561\x2562\x2556\x2555\x2563\x2551\x2557\x255D\x255C\x255B\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\x255E\x255F\x255A\x2554\x2569\x2566\x2560\x2550\x256C\x2567"\
							"\x2568\x2564\x2565\x2559\x2558\x2552\x2553\x256B\x256A\x2518\x250C\x2588\x2584\x258C\x2590\x2580"\
							"\x03B1\xDF\x0393\x03C0\x03A3\x03C3\xB5\x03C4\x03A6\x0398\x03A9\x03B4\x221E\x03C6\x03B5\x2229"\
							"\x2261\xB1\x2265\x2264\x2320\x2321\xF7\x2248\xB0\x2219\xB7\x221A\x207F\xB2\x25A0\xA0" );

		else if(codepage.Find(wxT("CP720")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP720")), 0x80);
			newCP+=wxT(	"..\xE9\xE2.\xE0.\xE7\xEA\xEB\xE8\xEF\xEE..."\
							".\x0651\x0652\xF4\xA4\x0640\xFB\xF9\x0621\x0622\x0623\x0624\xA3\x0625\x0626\x0627"\
							"\x0628\x0629\x062A\x062B\x062C\x062D\x062E\x062F\x0630\x0631\x0632\x0633\x0634\x0635\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524\x2561\x2562\x2556\x2555\x2563\x2551\x2557\x255D\x255C\x255B\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\x255E\x255F\x255A\x2554\x2569\x2566\x2560\x2550\x256C\x2567"\
							"\x2568\x2564\x2565\x2559\x2558\x2552\x2553\x256B\x256A\x2518\x250C\x2588\x2584\x258C\x2590\x2580"\
							"\x0636\x0637\x0638\x0639\x063A\x0641\xB5\x0642\x0643\x0644\x0645\x0646\x0647\x0648\x0649\x064A"\
							"\x2261\x064B\x064C\x064D\x064E\x064F\x0650\x2248\xB0\x2219\xB7\x221A\x207F\xB2\x25A0\xA0" );

		else if(codepage.Find(wxT("CP737")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP737")), 0x80);
			newCP+=wxT( "\x0391\x0392\x0393\x0394\x0395\x0396\x0397\x0398\x0399\x039A\x039B\x039C\x039D\x039E\x039F\x03A0"\
							"\x03A1\x03A3\x03A4\x03A5\x03A6\x03A7\x03A8\x03A9\x03B1\x03B2\x03B3\x03B4\x03B5\x03B6\x03B7\x03B8"\
							"\x03B9\x03BA\x03BB\x03BC\x03BD\x03BE\x03BF\x03C0\x03C1\x03C3\x03C2\x03C4\x03C5\x03C6\x03C7\x03C8"\
							"\x2591\x2592\x2593\x2502\x2524\x2561\x2562\x2556\x2555\x2563\x2551\x2557\x255D\x255C\x255B\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\x255E\x255F\x255A\x2554\x2569\x2566\x2560\x2550\x256C\x2567"\
							"\x2568\x2564\x2565\x2559\x2558\x2552\x2553\x256B\x256A\x2518\x250C\x2588\x2584\x258C\x2590\x2580"\
							"\x03C9\x03AC\x03AD\x03AE\x03CA\x03AF\x03CC\x03CD\x03CB\x03CE\x0386\x0388\x0389\x038A\x038C\x038E"\
							"\x038F\xB1\x2265\x2264\x03AA\x03AB\xF7\x2248\xB0\x2219\xB7\x221A\x207F\xB2\x25A0\xA0" );

		else if(codepage.Find(wxT("CP775")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP775")), 0x80);
			newCP+=wxT( "\x0106\xFC\xE9\x0101\xE4\x0123\xE5\x0107\x0142\x0113\x0156\x0157\x012B\x0179\xC4\xC5"\
							"\xC9\xE6\xC6\x014D\xF6\x0122\xA2\x015A\x015B\xD6\xDC\xF8\xA3\xD8\xD7\xA4"\
							"\x0100\x012A\xF3\x017B\x017C\x017A\x201D\xA6\xA9\xAE\xAC\xBD\xBC\x0141\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524\x0104\x010C\x0118\x0116\x2563\x2551\x2557\x255D\x012E\x0160\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\x0172\x016A\x255A\x2554\x2569\x2566\x2560\x2550\x256C\x017D"\
							"\x0105\x010D\x0119\x0117\x012F\x0161\x0173\x016B\x017E\x2518\x250C\x2588\x2584\x258C\x2590\x2580"\
							"\xD3\xDF\x014C\x0143\xF5\xD5\xB5\x0144\x0136\x0137\x013B\x013C\x0146\x0112\x0145\x2019"\
							".\xB1\x201C\xBE\xB6\xA7\xF7\x201E\xB0\x2219\xB7\xB9\xB3\xB2\x25A0\xA0" );

		else if(codepage.Find(wxT("CP850")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP850")), 0x80);
			newCP+=wxT( "\xC7\xFC\xE9\xE2\xE4\xE0\xE5\xE7\xEA\xEB\xE8\xEF\xEE\xEC\xC4\xC5"\
							"\xC9\xE6\xC6\xF4\xF6\xF2\xFB\xF9\xFF\xD6\xDC\xF8\xA3\xD8\xD7\x0192"\
							"\xE1\xED\xF3\xFA\xF1\xD1\xAA\xBA\xBF\xAE\xAC\xBD\xBC\xA1\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524\xC1\xC2\xC0\xA9\x2563\x2551\x2557\x255D\xA2\xA5\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\xE3\xC3\x255A\x2554\x2569\x2566\x2560\x2550\x256C\xA4"\
							"\xF0\xD0\xCA\xCB\xC8\x0131\xCD\xCE\xCF\x2518\x250C\x2588\x2584\xA6\xCC\x2580"\
							"\xD3\xDF\xD4\xD2\xF5\xD5\xB5\xFE\xDE\xDA\xDB\xD9\xFD\xDD\xAF\xB4"\
							".\xB1\x2017\xBE\xB6\xA7\xF7\xB8\xB0\xA8\xB7\xB9\xB3\xB2\x25A0\xA0");

		else if(codepage.Find(wxT("CP852")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP852")), 0x80);
			newCP+=wxT( "\xC7\xFC\xE9\xE2\xE4\x016F\x0107\xE7\x0142\xEB\x0150\x0151\xEE\x0179\xC4\x0106"\
							"\xC9\x0139\x013A\xF4\xF6\x013D\x013E\x015A\x015B\xD6\xDC\x0164\x0165\x0141\xD7\x010D"\
							"\xE1\xED\xF3\xFA\x0104\x0105\x017D\x017E\x0118\x0119\xAC\x017A\x010C\x015F\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524\xC1\xC2\x011A\x015E\x2563\x2551\x2557\x255D\x017B\x017C\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\x0102\x0103\x255A\x2554\x2569\x2566\x2560\x2550\x256C\xA4"\
							"\x0111\x0110\x010E\xCB\x010F\x0147\xCD\xCE\x011B\x2518\x250C\x2588\x2584\x0162\x016E\x2580"\
							"\xD3\xDF\xD4\x0143\x0144\x0148\x0160\x0161\x0154\xDA\x0155\x0170\xFD\xDD\x0163\xB4"\
							".\x02DD\x02DB\x02C7\x02D8\xA7\xF7\xB8\xB0\xA8\x02D9\x0171\x0158\x0159\x25A0\xA0" );

		else if(codepage.Find(wxT("CP855")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP855")), 0x80);
			newCP+=wxT(	"\x0452\x0402\x0453\x0403\x0451\x0401\x0454\x0404\x0455\x0405\x0456\x0406\x0457\x0407\x0458\x0408"\
							"\x0459\x0409\x045A\x040A\x045B\x040B\x045C\x040C\x045E\x040E\x045F\x040F\x044E\x042E\x044A\x042A"\
							"\x0430\x0410\x0431\x0411\x0446\x0426\x0434\x0414\x0435\x0415\x0444\x0424\x0433\x0413\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524\x0445\x0425\x0438\x0418\x2563\x2551\x2557\x255D\x0439\x0419\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\x043A\x041A\x255A\x2554\x2569\x2566\x2560\x2550\x256C\xA4"\
							"\x043B\x041B\x043C\x041C\x043D\x041D\x043E\x041E\x043F\x2518\x250C\x2588\x2584\x041F\x044F\x2580"\
							"\x042F\x0440\x0420\x0441\x0421\x0442\x0422\x0443\x0423\x0436\x0416\x0432\x0412\x044C\x042C\x2116"\
							".\x044B\x042B\x0437\x0417\x0448\x0428\x044D\x042D\x0449\x0429\x0447\x0427\xA7\x25A0\xA0" );

		else if(codepage.Find(wxT("CP856")) not_eq wxNOT_FOUND )
			//newCP+=wxString( bf, wxCSConv(wxT("CP856")), 0x80);
			newCP+=wxT( "\x05D0\x05D1\x05D2\x05D3\x05D4\x05D5\x05D6\x05D7\x05D8\x05D9\x05DA\x05DB\x05DC\x05DD\x05DE\x05DF"\
							"\x05E0\x05E1\x05E2\x05E3\x05E4\x05E5\x05E6\x05E7\x05E8\x05E9\x05EA.\xA3.\xD7."\
							".........\xAE\xAC\xBD\xBC.\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524...\xA9\x2563\x2551\x2557\x255D\xA2\xA5\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C..\x255A\x2554\x2569\x2566\x2560\x2550\x256C\xA4"\
							".........\x2518\x250C\x2588\x2584\xA6.\x2580"\
							"......\xB5.......\xAF\xB4"\
							".\xB1\x2017\xBE\xB6\xA7\xF7\xB8\xB0\xA8\xB7\xB9\xB3\xB2\x25A0\xA0" );

		else if(codepage.Find(wxT("CP857")) not_eq wxNOT_FOUND ){
//			bf[0xD5-0x80]=bf[0xE7-0x80]=bf[0xF2-0x80]='.';
//			newCP+=wxString( bf, wxCSConv(wxT("CP857")), 0x80);
//			newCP[0xD5]=wxChar(0x20AC); //Euro Sign
			//Updated 0xD5 with euro sign
			newCP+=wxT(	"\xC7\xFC\xE9\xE2\xE4\xE0\xE5\xE7\xEA\xEB\xE8\xEF\xEE\x0131\xC4\xC5"\
							"\xC9\xE6\xC6\xF4\xF6\xF2\xFB\xF9\x0130\xD6\xDC\xF8\xA3\xD8\x015E\x015F"\
							"\xE1\xED\xF3\xFA\xF1\xD1\x011E\x011F\xBF\xAE\xAC\xBD\xBC\xA1\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524\xC1\xC2\xC0\xA9\x2563\x2551\x2557\x255D\xA2\xA5\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\xE3\xC3\x255A\x2554\x2569\x2566\x2560\x2550\x256C\xA4"\
							"\xBA\xAA\xCA\xCB\xC8\x20AC\xCD\xCE\xCF\x2518\x250C\x2588\x2584\xA6\xCC\x2580"\
							"\xD3\xDF\xD4\xD2\xF5\xD5\xB5.\xD7\xDA\xDB\xD9\xEC\xFF\xAF\xB4"\
							".\xB1.\xBE\xB6\xA7\xF7\xB8\xB0\xA8\xB7\xB9\xB3\xB2\x25A0\xA0" );
		}

		else if(codepage.Find(wxT("CP858")) not_eq wxNOT_FOUND ){
			newCP=PrepareCodepageTable(wxT("PC/DOS CP850"));
			newCP[0xD5]=wxChar(0x20AC);
			}

		else if(codepage.Find(wxT("CP860")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP860")), 0x80);
			newCP+=wxT(	"\xC7\xFC\xE9\xE2\xE3\xE0\xC1\xE7\xEA\xCA\xE8\xCD\xD4\xEC\xC3\xC2"\
							"\xC9\xC0\xC8\xF4\xF5\xF2\xDA\xF9\xCC\xD5\xDC\xA2\xA3\xD9\x20A7\xD3"\
							"\xE1\xED\xF3\xFA\xF1\xD1\xAA\xBA\xBF\xD2\xAC\xBD\xBC\xA1\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524\x2561\x2562\x2556\x2555\x2563\x2551\x2557\x255D\x255C\x255B\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\x255E\x255F\x255A\x2554\x2569\x2566\x2560\x2550\x256C\x2567"\
							"\x2568\x2564\x2565\x2559\x2558\x2552\x2553\x256B\x256A\x2518\x250C\x2588\x2584\x258C\x2590\x2580"\
							"\x03B1\xDF\x0393\x03C0\x03A3\x03C3\xB5\x03C4\x03A6\x0398\x03A9\x03B4\x221E\x03C6\x03B5\x2229"\
							"\x2261\xB1\x2265\x2264\x2320\x2321\xF7\x2248\xB0\x2219\xB7\x221A\x207F\xB2\x25A0\xA0" );

		else if(codepage.Find(wxT("CP861")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP861")), 0x80);
			newCP+=wxT(	"\xC7\xFC\xE9\xE2\xE4\xE0\xE5\xE7\xEA\xEB\xE8\xD0\xF0\xDE\xC4\xC5"\
							"\xC9\xE6\xC6\xF4\xF6\xFE\xFB\xDD\xFD\xD6\xDC\xF8\xA3\xD8\x20A7\x0192"\
							"\xE1\xED\xF3\xFA\xC1\xCD\xD3\xDA\xBF\x2310\xAC\xBD\xBC\xA1\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524\x2561\x2562\x2556\x2555\x2563\x2551\x2557\x255D\x255C\x255B\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\x255E\x255F\x255A\x2554\x2569\x2566\x2560\x2550\x256C\x2567"\
							"\x2568\x2564\x2565\x2559\x2558\x2552\x2553\x256B\x256A\x2518\x250C\x2588\x2584\x258C\x2590\x2580"\
							"\x03B1\xDF\x0393\x03C0\x03A3\x03C3\xB5\x03C4\x03A6\x0398\x03A9\x03B4\x221E\x03C6\x03B5\x2229"\
							"\x2261\xB1\x2265\x2264\x2320\x2321\xF7\x2248\xB0\x2219\xB7\x221A\x207F\xB2\x25A0\xA0" );

		else if(codepage.Find(wxT("CP862")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP862")), 0x80);
			newCP+=wxT(	"\x05D0\x05D1\x05D2\x05D3\x05D4\x05D5\x05D6\x05D7\x05D8\x05D9\x05DA\x05DB\x05DC\x05DD\x05DE\x05DF"\
							"\x05E0\x05E1\x05E2\x05E3\x05E4\x05E5\x05E6\x05E7\x05E8\x05E9\x05EA\xA2\xA3\xA5\x20A7\x0192"\
							"\xE1\xED\xF3\xFA\xF1\xD1\xAA\xBA\xBF\x2310\xAC\xBD\xBC\xA1\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524\x2561\x2562\x2556\x2555\x2563\x2551\x2557\x255D\x255C\x255B\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\x255E\x255F\x255A\x2554\x2569\x2566\x2560\x2550\x256C\x2567"\
							"\x2568\x2564\x2565\x2559\x2558\x2552\x2553\x256B\x256A\x2518\x250C\x2588\x2584\x258C\x2590\x2580"\
							"\x03B1\xDF\x0393\x03C0\x03A3\x03C3\xB5\x03C4\x03A6\x0398\x03A9\x03B4\x221E\x03C6\x03B5\x2229"\
							"\x2261\xB1\x2265\x2264\x2320\x2321\xF7\x2248\xB0\x2219\xB7\x221A\x207F\xB2\x25A0\xA0");

		else if(codepage.Find(wxT("CP863")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP863")), 0x80);
			newCP+=wxT(	"\xC7\xFC\xE9\xE2\xC2\xE0\xB6\xE7\xEA\xEB\xE8\xEF\xEE\x2017\xC0\xA7"\
							"\xC9\xC8\xCA\xF4\xCB\xCF\xFB\xF9\xA4\xD4\xDC\xA2\xA3\xD9\xDB\x0192"\
							"\xA6\xB4\xF3\xFA\xA8\xB8\xB3\xAF\xCE\x2310\xAC\xBD\xBC\xBE\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524\x2561\x2562\x2556\x2555\x2563\x2551\x2557\x255D\x255C\x255B\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\x255E\x255F\x255A\x2554\x2569\x2566\x2560\x2550\x256C\x2567"\
							"\x2568\x2564\x2565\x2559\x2558\x2552\x2553\x256B\x256A\x2518\x250C\x2588\x2584\x258C\x2590\x2580"\
							"\x03B1\xDF\x0393\x03C0\x03A3\x03C3\xB5\x03C4\x03A6\x0398\x03A9\x03B4\x221E\x03C6\x03B5\x2229"\
							"\x2261\xB1\x2265\x2264\x2320\x2321\xF7\x2248\xB0\x2219\xB7\x221A\x207F\xB2\x25A0\xA0");

		else if(codepage.Find(wxT("CP864")) not_eq wxNOT_FOUND ){
			//0xA7 replaced with EUR
//			newCP+=wxString( bf, wxCSConv(wxT("CP864")), 0x80);
			newCP+=wxT(	"\xB0\xB7\x2219\x221A\x2592\x2500\x2502\x253C\x2524\x252C\x251C\x2534\x2510\x250C\x2514\x2518"\
							"\x03B2\x221E\x03C6\xB1\xBD\xBC\x2248\xAB\xBB\xFEF7\xFEF8..\xFEFB\xFEFC."\
							"\xA0.\xFE82\xA3\xA4\xFE84.\x20AC\xFE8E\xFE8F\xFE95\xFE99\x060C\xFE9D\xFEA1\xFEA5"\
							"\x0660\x0661\x0662\x0663\x0664\x0665\x0666\x0667\x0668\x0669\xFED1\x061B\xFEB1\xFEB5\xFEB9\x061F"\
							"\xA2\xFE80\xFE81\xFE83\xFE85\xFECA\xFE8B\xFE8D\xFE91\xFE93\xFE97\xFE9B\xFE9F\xFEA3\xFEA7\xFEA9"\
							"\xFEAB\xFEAD\xFEAF\xFEB3\xFEB7\xFEBB\xFEBF\xFEC1\xFEC5\xFECB\xFECF\xA6\xAC\xF7\xD7\xFEC9"\
							"\x0640\xFED3\xFED7\xFEDB\xFEDF\xFEE3\xFEE7\xFEEB\xFEED\xFEEF\xFEF3\xFEBD\xFECC\xFECE\xFECD\xFEE1"\
							"\xFE7D\x0651\xFEE5\xFEE9\xFEEC\xFEF0\xFEF2\xFED0\xFED5\xFEF5\xFEF6\xFEDD\xFED9\xFEF1\x25A0." );
			newCP[0x25]=wxChar(0x066A); // ARABIC PERCENT SIGN ⟨٪⟩
			}

		else if(codepage.Find(wxT("CP865")) not_eq wxNOT_FOUND ){
			newCP=PrepareCodepageTable(wxT("PC/DOS CP437" ));
			newCP[0x9B]=wxChar(0xF8);
			newCP[0x9D]=wxChar(0xD8);
			newCP[0xAF]=wxChar(0xA4);
			}

		else if(codepage.Find(wxT("CP866")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP866")), 0x80);
			newCP+=wxT(	"\x0410\x0411\x0412\x0413\x0414\x0415\x0416\x0417\x0418\x0419\x041A\x041B\x041C\x041D\x041E\x041F"\
							"\x0420\x0421\x0422\x0423\x0424\x0425\x0426\x0427\x0428\x0429\x042A\x042B\x042C\x042D\x042E\x042F"\
							"\x0430\x0431\x0432\x0433\x0434\x0435\x0436\x0437\x0438\x0439\x043A\x043B\x043C\x043D\x043E\x043F"\
							"\x2591\x2592\x2593\x2502\x2524\x2561\x2562\x2556\x2555\x2563\x2551\x2557\x255D\x255C\x255B\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\x255E\x255F\x255A\x2554\x2569\x2566\x2560\x2550\x256C\x2567"\
							"\x2568\x2564\x2565\x2559\x2558\x2552\x2553\x256B\x256A\x2518\x250C\x2588\x2584\x258C\x2590\x2580"\
							"\x0440\x0441\x0442\x0443\x0444\x0445\x0446\x0447\x0448\x0449\x044A\x044B\x044C\x044D\x044E\x044F"\
							"\x0401\x0451\x0404\x0454\x0407\x0457\x040E\x045E\xB0\x2219\xB7\x221A\x2116\xA4\x25A0\xA0" );

		else if(codepage.Find(wxT("CP869")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP869")), 0x80);
			newCP+=wxT(	"......\x0386.\xB7\xAC\xA6\x2018\x2019\x0388\x2015\x0389"\
							"\x038A\x03AA\x038C..\x038E\x03AB\xA9\x038F\xB2\xB3\x03AC\xA3\x03AD\x03AE\x03AF"\
							"\x03CA\x0390\x03CC\x03CD\x0391\x0392\x0393\x0394\x0395\x0396\x0397\xBD\x0398\x0399\xAB\xBB"\
							"\x2591\x2592\x2593\x2502\x2524\x039A\x039B\x039C\x039D\x2563\x2551\x2557\x255D\x039E\x039F\x2510"\
							"\x2514\x2534\x252C\x251C\x2500\x253C\x03A0\x03A1\x255A\x2554\x2569\x2566\x2560\x2550\x256C\x03A3"\
							"\x03A4\x03A5\x03A6\x03A7\x03A8\x03A9\x03B1\x03B2\x03B3\x2518\x250C\x2588\x2584\x03B4\x03B5\x2580"\
							"\x03B6\x03B7\x03B8\x03B9\x03BA\x03BB\x03BC\x03BD\x03BE\x03BF\x03C0\x03C1\x03C3\x03C2\x03C4\x0384"\
							".\xB1\x03C5\x03C6\x03C7\xA7\x03C8\x0385\xB0\xA8\x03C9\x03CB\x03B0\x03CE\x25A0\xA0" );

		else if(codepage.Find(wxT("CP1006")) not_eq wxNOT_FOUND )
//			newCP+=wxString( bf, wxCSConv(wxT("CP1006")), 0x80);
			newCP+=wxT(	"\xA0\x06F0\x06F1\x06F2\x06F3\x06F4\x06F5\x06F6\x06F7\x06F8\x06F9\x060C\x061B.\x061F\xFE81"\
							"\xFE8D\xFE8E\xFE8E\xFE8F\xFE91\xFB56\xFB58\xFE93\xFE95\xFE97\xFB66\xFB68\xFE99\xFE9B\xFE9D\xFE9F"\
							"\xFB7A\xFB7C\xFEA1\xFEA3\xFEA5\xFEA7\xFEA9\xFB84\xFEAB\xFEAD\xFB8C\xFEAF\xFB8A\xFEB1\xFEB3\xFEB5"\
							"\xFEB7\xFEB9\xFEBB\xFEBD\xFEBF\xFEC1\xFEC5\xFEC9\xFECA\xFECB\xFECC\xFECD\xFECE\xFECF\xFED0\xFED1"\
							"\xFED3\xFED5\xFED7\xFED9\xFEDB\xFB92\xFB94\xFEDD\xFEDF\xFEE0\xFEE1\xFEE3\xFB9E\xFEE5\xFEE7\xFE85"\
							"\xFEED\xFBA6\xFBA8\xFBA9\xFBAA\xFE80\xFE89\xFE8A\xFE8B\xFEF1\xFEF2\xFEF3\xFBB0\xFBAE\xFE7C\xFE7D" );

		else if(codepage.Find(wxT("KZ-1048")) not_eq wxNOT_FOUND ){
			newCP+=wxT( "\x0402\x0403\x201A\x0453\x201E\x2026\x2020\x2021\x20AC\x2030\x0409\x2039\x040A\x049A\x04BA\x040F"\
							"\x0452\x2018\x2019\x201C\x201D\x2022\x2013\x2014.\x2122\x0459\x203A\x045A\x049B\x04BB\x045F"\
							"\xA0\x04B0\x04B1\x04D8\xA4\x04E8\xA6\xA7\x0401\xA9\x0492\xAB\xAC.\xAE\x04AE"\
							"\xB0\xB1\x0406\x0456\x04E9\xB5\xB6\xB7\x0451\x2116\x0493\xBB\x04D9\x04A2\x04A3\x04AF" );
			for( int i=0; i<0x40; i++)
				newCP+=wxChar(0x0410+i);
//							"\x0410\x0411\x0412\x0413\x0414\x0415\x0416\x0417\x0418\x0419\x041A\x041B\x041C\x041D\x041E\x041F"\
//							"\x0420\x0421\x0422\x0423\x0424\x0425\x0426\x0427\x0428\x0429\x042A\x042B\x042C\x042D\x042E\x042F"\
//							"\x0430\x0431\x0432\x0433\x0434\x0435\x0436\x0437\x0438\x0439\x043A\x043B\x043C\x043D\x043E\x043F"\
//							"\x0440\x0441\x0442\x0443\x0444\x0445\x0446\x0447\x0448\x0449\x044A\x044B\x044C\x044D\x044E\x044F" );
			}
		else if(codepage.Find(wxT("MIK")) not_eq wxNOT_FOUND ){
			for (unsigned i=0x80; i<0xC0 ; i++)
				newCP += wxChar(i-0x80+0x0410);

			newCP += wxT(	"\x2514\x2534\x252C\x251C\x2500\x253C\x2563\x2551\x255A\x2554\x2569\x2566\x2560\x2550\x256C\x2510"\
								"\x2591\x2592\x2593\x2502\x2524\x2116\x00A7\x2557\x255D\x2518\x250C\x2588\x2584\x258C\x2590\x2580"\
								"\x03B1\x00DF\x0393\x03C0\x03A3\x03C3\x00B5\x03C4\x03A6\x0398\x03A9\x03B4\x221E\x03C6\x03B5\x2229"\
								"\x2261\x00B1\x2265\x2264\x2320\x2321\x00F7\x2248\x00B0\x2219\x00B7\x221A\x207F\x00B2\x25A0\x00A0" );
			}

		else if(codepage.Find(wxT("Kamenick")) not_eq wxNOT_FOUND ){
			newCP=PrepareCodepageTable(wxT("PC/DOS CP437" ));
			newCP=newCP.Mid(0,0x80)+
					+wxT(	"\x010C\x00FC\x00E9\x010F\x00E4\x010E\x0164\x010D\x011B\x011A\x0139\x00CD\x013E\x013A\x00C4\x00C1"\
							"\x00C9\x017E\x017D\x00F4\x00F6\x00D3\x016F\x00DA\x00FD\x00D6\x00DC\x0160\x013D\x00DD\x0158\x0165"\
							"\x00E1\x00ED\x00F3\x00FA\x0148\x0147\x016E\x00D4\x0161\x0159\x0155\x0154\x00BC\x00A7\x00AB\x00BB")
					+newCP.Mid(0xB0,0xFF-0xB0);
			}

		else if(codepage.Find(wxT("Mazovia")) not_eq wxNOT_FOUND ){
			newCP=PrepareCodepageTable(wxT("PC/DOS CP437" ));
			newCP=newCP.Mid(0,0x80)+
					+wxT(	"\x00C7\x00FC\x00E9\x00E2\x00E4\x00E0\x0105\x00E7\x00EA\x00EB\x00E8\x00EF\x00EE\x0107\x00C4\x0104"\
							"\x0118\x0119\x0142\x00F4\x00F6\x0106\x00FB\x00F9\x015A\x00D6\x00DC\x00A2\x0141\x00A5\x015B\x0192"\
							"\x0179\x017B\x00F3\x00D3\x0144\x0143\x017A\x017C\x00BF\x2310\x00AC\x00BD\x00BC\x00A1\x00AB\x00BB" )
					+newCP.Mid(0xB0,0xFF-0xB0);
			}
		else if(codepage.Find(wxT("Iran")) not_eq wxNOT_FOUND ){
			newCP=PrepareCodepageTable(wxT("PC/DOS CP437" ));
			newCP=newCP.Mid(0,0x80)+
					+wxT(	"\x06F0\x06F1\x06F2\x06F3\x06F4\x06F5\x06F6\x06F7\x06F8\x06F9\x060C\x0640\x061F\xFE81\xFE8B\x0621"\
							"\xFE8D\xFE8E\xFE8F\xFE91\xFB56\xFB58\xFE95\xFE97\xFE99\xFE9B\xFE9D\xFE9F\xFB7C\xFB7C\xFEA1\xFEA3"\
							"\xFEA5\xFEA7\x062F\x0630\x0631\x0632\x0698\xFEB1\xFEB3\xFEB5\xFEB7\xFEB9\xFEBB\xFEBD\xFEBF\x0637" )
					+newCP.Mid(0xB0,0xFF-0xB0);
			}

		}

	else	if(codepage.Find(wxT("ISO/IEC")) not_eq wxNOT_FOUND ){
		if(codepage.Find(wxT("6937")) not_eq wxNOT_FOUND){
			newCP=PrepareCodepageTable(wxT("ASCII")).Mid(0,0xA0);
			//SHY replaced with dot (at 0xFF)
			newCP+=wxT(	"\xA0\xA1\xA2\xA3.\xA5.\xA7\xA4\x2018\x201C\xAB\x2190\x2191\x2192\x2193"\
							"\xB0\xB1\xB2\xB3\xD7\xB5\xB6\xB7\xF7\x2019\x201D\xBB\xBC\xBD\xBE\xBF.\x0300\x0301"\
							"\x0302\x0303\x0304\x0306\x0307\x0308.\x030A\x0327.\x030B\x0328\x030C\x2015\xB9"\
							"\xAE\xA9\x2122\x266A\xAC\xA6....\x215B\x215C\x215D\x215E\x2126\xC6\x0110\xAA"\
							"\x0126.\x0132\x013F\x0141\xD8\x0152\xBA\xDE\x0166\x014A\x0149\x0138\xE6\x0111"\
							"\xF0\x0127\x0131\x0133\x0140\x0142\xF8\x0153\xDF\xFE\x0167\x014B.");
			return CodepageTable=newCP;
			}


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
		else if(codepage.Mid(q,2).StartsWith(wxT("16"))){
			//newCP+=wxString( bf, wxCSConv(wxT("ISO8859-16")), 256);
			newCP=PrepareCodepageTable(wxT("ASCII")).Mid(0,0xA0);
			newCP+=wxT(	"\xA0\x0104\x0105\x0141\x20AC\x201E\x0160\xA7\x0161\xA9\x0218\xAB\x0179\xAD\x017A\x017B"\
							"\xB0\xB1\x010C\x0142\x017D\x201D\xB6\xB7\x017E\x010D\x0219\xBB\x0152\x0153\x0178\x017C"\
							"\xC0\xC1\xC2\x0102\xC4\x0106\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF"\
							"\x0110\x0143\xD2\xD3\xD4\x0150\xD6\x015A\x0170\xD9\xDA\xDB\xDC\x0118\x021A\xDF"\
							"\xE0\xE1\xE2\x0103\xE4\x0107\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF"\
							"\x0111\x0144\xF2\xF3\xF4\x0151\xF6\x015B\x0171\xF9\xFA\xFB\xFC\x0119\x021B\xFF" );
			}
		}

	// Windows Code Pages
	else if(codepage.Find(wxT("Windows")) not_eq wxNOT_FOUND ){
		if(codepage.Find(wxT("CP874")) not_eq wxNOT_FOUND ){
			newCP=PrepareCodepageTable(wxT("ISO/IEC 8859-11"));
			unsigned char a[]="\x80\xB5\x91\x92\x93\x94\x95\x96\x97";  //Patch Index
			wxString b=wxT("\x20AC\x2026\x2018\x2019\x201C\x201D\x2022\x2013\x2014");	//Patch Value
			for(int i=0;i<9;i++)
				newCP[a[i]]=b[i];
			}
		else if(codepage.Find(wxT("CP932")) not_eq wxNOT_FOUND ) FontEnc=wxFONTENCODING_CP932;//ShiftJS
		else if(codepage.Find(wxT("CP936")) not_eq wxNOT_FOUND ) FontEnc=wxFONTENCODING_CP936;//GBK
		else if(codepage.Find(wxT("CP949")) not_eq wxNOT_FOUND ) FontEnc=wxFONTENCODING_CP949; //EUC-KR
		else if(codepage.Find(wxT("CP950")) not_eq wxNOT_FOUND ) FontEnc=wxFONTENCODING_CP950;//BIG5
		else{
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
		}

	else if(codepage.Find(wxT("AtariST")) not_eq wxNOT_FOUND ){
		newCP+=PrepareCodepageTable(wxT("ASCII")).Mid(0x0,0x80);
		newCP+=PrepareCodepageTable(wxT("DOS CP437")).Mid(0x80,0x30);
		newCP+=wxT(	"\x00E3\x00F5\x00D8\x00F8\x0153\x0152\x00C0\x00C3\x00D5\x00A8\x00B4\x2020\x00B6\x00A9\x00AE\x2122"
						"\x0133\x0132\x05D0\x05D1\x05D2\x05D3\x05D4\x05D5\x05D6\x05D7\x05D8\x05D9\x05DB\x05DC\x05DE\x05E0"
						"\x05E1\x05E2\x05E4\x05E6\x05E7\x05E8\x05E9\x05EA\x05DF\x05DA\x05DD\x05E3\x05E5\x00A7\x2227\x221E");
		newCP+=PrepareCodepageTable(wxT("DOS CP437")).Mid(0xE0,0x20);
		newCP[0x9E]=wxChar(0x00DF);
		newCP[0xE1]=wxChar(0x03B2);
		newCP[0xEC]=wxChar(0x222E);
		newCP[0xEE]=wxChar(0x2208);
		newCP[0xFE]=wxChar(0x00B3);
		newCP[0xFF]=wxChar(0x00AF);
		}

	else if(codepage.Find(wxT("KOI7")) not_eq wxNOT_FOUND ){
		newCP=PrepareCodepageTable(wxT("ASCII")).Mid(0,0x60);
		newCP += wxT(	"\x042E\x0410\x0411\x0426\x0414\x0415\x0424\x0413\x0425\x0418\x0419\x041A\x041B\x041C\x041D\x041E"\
							"\x041F\x042F\x0420\x0421\x0422\x0423\x0416\x0412\x042C\x042B\x0417\x0428\x042D\x0429\x0427.");
		for (unsigned i=0x80; i<=0xFF ; i++)
			newCP += '.';
		}

	else if(codepage.Find(wxT("KOI8")) not_eq wxNOT_FOUND ){
		for (unsigned i=0; i<=0xFF ; i++)
			bf[i] = (i<0x20 or i==0x7F)	? '.' : i;
		if(codepage.StartsWith(wxT("KOI8-R"))) newCP+=wxString( bf, wxCSConv(wxFONTENCODING_KOI8), 256);
		if(codepage.StartsWith(wxT("KOI8-U"))) newCP+=wxString( bf, wxCSConv(wxFONTENCODING_KOI8_U), 256);
		}

	else if(codepage.Find(wxT("JIS X 0201")) not_eq wxNOT_FOUND ){
		for (unsigned i=0; i<0xFF ; i++)
			if(i==0x5C)
				newCP += wxChar(0xA5); //JPY
			else if(i==0x7E)
				newCP += wxChar(0x203E);//Overline
			else if(i<0x80)
				newCP += ((i<0x20 or i==0x7F)	? '.' : wxChar(i));
			else if( i>=0xA1 and i<0xE0)
				newCP += wxChar(i-0xA0+0xFF60);
			else
				newCP +='.';
		}

	else if(codepage.Find(wxT("TIS-620")) not_eq wxNOT_FOUND ){
		newCP=PrepareCodepageTable(wxT("ISO/IEC 8859-11")); //Identical
		}

	else if(codepage.Find(wxT("EBCDIC")) not_eq wxNOT_FOUND ){
		//Control chars replaced with dot
		for (unsigned i=0; i<0x40 ; i++)
			newCP+=wxChar('.');

		/// \x00AD (Soft Hypen) replaced with dot .
		/// \x009F (End Of File ) replaced with dot .
		//EBCDIC Table
		newCP+=wxT( "\x20\xA0\xE2\xE4\xE0\xE1\xE3\xE5\xE7\xF1\xA2\x2E\x3C\x28\x2B\x7C"\
						"\x26\xE9\xEA\xEB\xE8\xED\xEE\xEF\xEC\xDF\x21\x24\x2A\x29\x3B\xAC"\
						"\x2D\x2F\xC2\xC4\xC0\xC1\xC3\xC5\xC7\xD1\xA6\x2C\x25\x5F\x3E\x3F"\
						"\xF8\xC9\xCA\xCB\xC8\xCD\xCE\xCF\xCC\x60\x3A\x23\x40\x27\x3D\x22"\
						"\xD8\x61\x62\x63\x64\x65\x66\x67\x68\x69\xAB\xBB\xF0\xFD\xFE\xB1"\
						"\xB0\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72\xAA\xBA\xE6\xB8\xC6\xA4"\
						"\xB5\x7E\x73\x74\x75\x76\x77\x78\x79\x7A\xA1\xBF\xD0\xDD\xDE\xAE"\
						"\x5E\xA3\xA5\xB7\xA9\xA7\xB6\xBC\xBD\xBE\x5B\x5D\xAF\xA8\xB4\xD7"\
						"\x7B\x41\x42\x43\x44\x45\x46\x47\x48\x49.\xF4\xF6\xF2\xF3\xF5"\
						"\x7D\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\xB9\xFB\xFC\xF9\xFA\xFF"\
						"\x5C\xF7\x53\x54\x55\x56\x57\x58\x59\x5A\xB2\xD4\xD6\xD2\xD3\xD5"\
						"\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\xB3\xDB\xDC\xD9\xDA.");

		if(codepage.Find(wxT("037")) not_eq wxNOT_FOUND ){
			//This is EBCDIC 037	Already
			}

		else if(codepage.Find(wxT("285")) not_eq wxNOT_FOUND ){
			unsigned char a[]="\x4A\x5B\xA1\xB0\xB1\xBA\xBC";  //Patch Index
			unsigned char b[]="\x24\xA3\xAF\xA2\x5B\x5E\x7E";	//Patch Value
			for(int i=0;i<7;i++)
				newCP[a[i]]=wxChar(b[i]);
			}

		else if(codepage.Find(wxT("424")) not_eq wxNOT_FOUND ){
			newCP.Clear();
			for (unsigned i=0; i<0x40 ; i++)
				newCP+=wxChar('.');
			// At 0xFF, (0x9F) replaced with .
			newCP+=wxT( "\x20\x05D0\x05D1\x05D2\x05D3\x05D4\x05D5\x05D6\x05D7\x05D8\xA2\x2E\x3C\x28\x2B\x7C"\
							"\x26\x05D9\x05DA\x05DB\x05DC\x05DD\x05DE\x05DF\x05E0\x05E1\x21\x24\x2A\x29\x3B\xAC"\
							"\x2D\x2F\x05E2\x05E3\x05E4\x05E5\x05E6\x05E7\x05E8\x05E9\xA6\x2C\x25\x5F\x3E\x3F"\
							".\x05EA..\xA0...\x2017\x60\x3A\x23\x40\x27\x3D\x22"\
							".\x61\x62\x63\x64\x65\x66\x67\x68\x69\xAB\xBB...\xB1"\
							"\xB0\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72...\xB8.\xA4"\
							"\xB5\x7E\x73\x74\x75\x76\x77\x78\x79\x7A.....\xAE"\
							"\x5E\xA3\xA5\xB7\xA9\xA7\xB6\xBC\xBD\xBE\x5B\x5D\xAF\xA8\xB4\xD7"\
							"\x7B\x41\x42\x43\x44\x45\x46\x47\x48\x49......"\
							"\x7D\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\xB9....."\
							"\x5C\xF7\x53\x54\x55\x56\x57\x58\x59\x5A\xB2....."\
							"\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\xB3....." );
			}


		else if(codepage.Find(wxT("500")) not_eq wxNOT_FOUND ){
			unsigned char a[]="\x4A\x4F\x5A\x5F\xB0\xBA\xBB\xCA"; //Patch Index
			unsigned char b[]="\x5B\x21\x5D\x5E\xA2\xAC\x7C\x2E";	//Patch Value
			for(int i=0;i<8;i++)
				newCP[a[i]]=wxChar(b[i]);
			}

		else if(codepage.Find(wxT("875")) not_eq wxNOT_FOUND ){
			newCP=wxEmptyString;
			for (unsigned i=0; i<0x40 ; i++)
				newCP+=wxChar('.');
			// At 0xCA, (0xAD) replaced with .
			// At 0xFF, (0x9F) replaced with .
			// Others are 0x1A originaly, replaced with .
			newCP+=wxT(	"\x20\x0391\x0392\x0393\x0394\x0395\x0396\x0397\x0398\x0399\x5B\x2E\x3C\x28\x2B\x21"\
							"\x26\x039A\x039B\x039C\x039D\x039E\x039F\x03A0\x03A1\x03A3\x5D\x24\x2A\x29\x3B\x5E"\
							"\x2D\x2F\x03A4\x03A5\x03A6\x03A7\x03A8\x03A9\x03AA\x03AB\x7C\x2C\x25\x5F\x3E\x3F"\
							"\xA8\x0386\x0388\x0389\xA0\x038A\x038C\x038E\x038F\x60\x3A\x23\x40\x27\x3D\x22"\
							"\x0385\x61\x62\x63\x64\x65\x66\x67\x68\x69\x03B1\x03B2\x03B3\x03B4\x03B5\x03B6"\
							"\xB0\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72\x03B7\x03B8\x03B9\x03BA\x03BB\x03BC"\
							"\xB4\x7E\x73\x74\x75\x76\x77\x78\x79\x7A\x03BD\x03BE\x03BF\x03C0\x03C1\x03C3"\
							"\xA3\x03AC\x03AD\x03AE\x03CA\x03AF\x03CC\x03CD\x03CB\x03CE\x03C2\x03C4\x03C5\x03C6\x03C7\x03C8"\
							"\x7B\x41\x42\x43\x44\x45\x46\x47\x48\x49.\x03C9\x0390\x03B0\x2018\x2015"\
							"\x7D\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\xB1\xBD.\x0387\x2019\xA6"\
							"\x5C.\x53\x54\x55\x56\x57\x58\x59\x5A\xB2\xA7..\xAB\xAC"\
							"\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\xB3\xA9..\xBB.");
			}

		else if(codepage.Find(wxT("1026")) not_eq wxNOT_FOUND ){
			newCP=wxEmptyString;
			for (unsigned i=0; i<0x40 ; i++)
				newCP+=wxChar('.');
			// At 0xCA, (0xAD) replaced with .
			// At 0xFF, (0x9F) replaced with .
			newCP+=wxT(	"\x20\xA0\xE2\xE4\xE0\xE1\xE3\xE5\x7B\xF1\xC7\x2E\x3C\x28\x2B\x21"\
							"\x26\xE9\xEA\xEB\xE8\xED\xEE\xEF\xEC\xDF\x011E\x0130\x2A\x29\x3B\x5E"\
							"\x2D\x2F\xC2\xC4\xC0\xC1\xC3\xC5\x5B\xD1\x015F\x2C\x25\x5F\x3E\x3F"\
							"\xF8\xC9\xCA\xCB\xC8\xCD\xCE\xCF\xCC\x0131\x3A\xD6\x015E\x27\x3D\xDC"\
							"\xD8\x61\x62\x63\x64\x65\x66\x67\x68\x69\xAB\xBB\x7D\x60\xA6\xB1"\
							"\xB0\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72\xAA\xBA\xE6\xB8\xC6\xA4"\
							"\xB5\xF6\x73\x74\x75\x76\x77\x78\x79\x7A\xA1\xBF\x5D\x24\x40\xAE"\
							"\xA2\xA3\xA5\xB7\xA9\xA7\xB6\xBC\xBD\xBE\xAC\x7C\xAF\xA8\xB4\xD7"\
							"\xE7\x41\x42\x43\x44\x45\x46\x47\x48\x49.\xF4\x7E\xF2\xF3\xF5"\
							"\x011F\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\xB9\xFB\x5C\xF9\xFA\xFF"\
							"\xFC\xF7\x53\x54\x55\x56\x57\x58\x59\x5A\xB2\xD4\x23\xD2\xD3\xD5"\
							"\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\xB3\xDB\x22\xD9\xDA.");

			}

		else if(codepage.Find(wxT("1047")) not_eq wxNOT_FOUND ){
			unsigned char a[]="\x8F\xAD\xB0\xBA\xBB\xBD";  //Patch Index
			unsigned char b[]="\x5E\x5B\xAC\xDD\xA8\x5D";  //Patch Value
			for(int i=0;i<6;i++)
				newCP[a[i]]=wxChar(b[i]);
			}
		else if(codepage.Find(wxT("1040")) not_eq wxNOT_FOUND ){
			newCP[0x9F]=wxChar(0x20AC);
			}

		else if(codepage.Find(wxT("1146")) not_eq wxNOT_FOUND ){
			newCP=PrepareCodepageTable( wxT("EBCDIC 285"));
			newCP[0x9F]=wxChar(0x20AC);
			}

		else if(codepage.Find(wxT("1148")) not_eq wxNOT_FOUND ){
			newCP=PrepareCodepageTable( wxT("EBCDIC 500"));
			newCP[0x9F]=wxChar(0x20AC);
			}
		}

	else if(codepage.Find(wxT("Macintosh")) not_eq wxNOT_FOUND ){
		//Control chars replaced with dot
		for (unsigned i=0; i<0x20 ; i++)
			newCP+=wxChar('.');
		//ASCII compatible part
		for( unsigned i=0x20 ; i < 0x7F ; i++ )
			newCP += wxChar(i);
		newCP+='.';//0xFF delete char

		if( codepage.Find(wxT("CP10000")) not_eq wxNOT_FOUND )		//Macintosh Roman extension table
			newCP+=wxT(	"\xC4\xC5\xC7\xC9\xD1\xD6\xDC\xE1\xE0\xE2\xE4\xE3\xE5\xE7\xE9\xE8"\
							"\xEA\xEB\xED\xEC\xEE\xEF\xF1\xF3\xF2\xF4\xF6\xF5\xFA\xF9\xFB\xFC"\
							"\x2020\xB0\xA2\xA3\xA7\x2022\xB6\xDF\xAE\xA9\x2122\xB4\xA8\x2260\xC6\xD8"\
							"\x221E\xB1\x2264\x2265\xA5\xB5\x2202\x2211\x220F\x03C0\x222B\xAA\xBA\x2126\xE6\xF8"\
							"\xBF\xA1\xAC\x221A\x0192\x2248\x2206\xAB\xBB\x2026\xA0\xC0\xC3\xD5\x0152\x0153"\
							"\x2013\x2014\x201C\x201D\x2018\x2019\xF7\x25CA\xFF\x0178\x2044\xA4\x2039\x203A\xFB01\xFB02"\
							"\x2021\xB7\x201A\x201E\x2030\xC2\xCA\xC1\xCB\xC8\xCD\xCE\xCF\xCC\xD3\xD4"\
							".\xD2\xDA\xDB\xD9\x0131\x02C6\x02DC\xAF\x02D8\x02D9\x02DA\xB8\x02DD\x02DB\x02C7" );

		else if( codepage.Find(wxT("CP10029")) not_eq wxNOT_FOUND )		//Macintosh Latin2 extension table
			newCP+=wxT( "\xC4\x0100\x0101\xC9\x0104\xD6\xDC\xE1\x0105\x010C\xE4\x010D\x0106\x0107\xE9\x0179"\
							"\x017A\x010E\xED\x010F\x0112\x0113\x0116\xF3\x0117\xF4\xF6\xF5\xFA\x011A\x011B\xFC"\
							"\x2020\xB0\x0118\xA3\xA7\x2022\xB6\xDF\xAE\xA9\x2122\x0119\xA8\x2260\x0123\x012E"\
							"\x012F\x012A\x2264\x2265\x012B\x0136\x2202\x2211\x0142\x013B\x013C\x013D\x013E\x0139\x013A\x0145"\
							"\x0146\x0143\xAC\x221A\x0144\x0147\x2206\xAB\xBB\x2026\xA0\x0148\x0150\xD5\x0151\x014C"\
							"\x2013\x2014\x201C\x201D\x2018\x2019\xF7\x25CA\x014D\x0154\x0155\x0158\x2039\x203A\x0159\x0156"\
							"\x0157\x0160\x201A\x201E\x0161\x015A\x015B\xC1\x0164\x0165\xCD\x017D\x017E\x016A\xD3\xD4"\
							"\x016B\x016E\xDA\x016F\x0170\x0171\x0172\x0173\xDD\xFD\x0137\x017B\x0141\x017C\x0122\x02C7" );


		else if( codepage.Find(wxT("CP10079")) not_eq wxNOT_FOUND )		//Macintosh Icelandic extension table
			newCP+=wxT(	"\xC4\xC5\xC7\xC9\xD1\xD6\xDC\xE1\xE0\xE2\xE4\xE3\xE5\xE7\xE9\xE8"\
							"\xEA\xEB\xED\xEC\xEE\xEF\xF1\xF3\xF2\xF4\xF6\xF5\xFA\xF9\xFB\xFC"\
							"\xDD\xB0\xA2\xA3\xA7\x2022\xB6\xDF\xAE\xA9\x2122\xB4\xA8\x2260\xC6\xD8"\
							"\x221E\xB1\x2264\x2265\xA5\xB5\x2202\x2211\x220F\x03C0\x222B\xAA\xBA\x2126\xE6\xF8"\
							"\xBF\xA1\xAC\x221A\x0192\x2248\x2206\xAB\xBB\x2026\xA0\xC0\xC3\xD5\x0152\x0153"\
							"\x2013\x2014\x201C\x201D\x2018\x2019\xF7\x25CA\xFF\x0178\x2044\xA4\xD0\xF0\xDE\xFE"\
							"\xFD\xB7\x201A\x201E\x2030\xC2\xCA\xC1\xCB\xC8\xCD\xCE\xCF\xCC\xD3\xD4"\
							".\xD2\xDA\xDB\xD9\x0131\x02C6\x02DC\xAF\x02D8\x02D9\x02DA\xB8\x02DD\x02DB\x02C7" );


		else if( codepage.Find(wxT("CP10006")) not_eq wxNOT_FOUND )		//Macintosh Greek CP10006 extension table
			newCP+=wxT(	"\xC4\xB9\xB2\xC9\xB3\xD6\xDC\x0385\xE0\xE2\xE4\x0384\xA8\xE7\xE9\xE8"\
							"\xEA\xEB\xA3\x2122\xEE\xEF\x2022\xBD\x2030\xF4\xF6\xA6\xAD\xF9\xFB\xFC"\
							"\x2020\x0393\x0394\x0398\x039B\x039E\x03A0\xDF\xAE\xA9\x03A3\x03AA\xA7\x2260\xB0\x0387"\
							"\x0391\xB1\x2264\x2265\xA5\x0392\x0395\x0396\x0397\x0399\x039A\x039C\x03A6\x03AB\x03A8\x03A9"\
							"\x03AC\x039D\xAC\x039F\x03A1\x2248\x03A4\xAB\xBB\x2026\xA0\x03A5\x03A7\x0386\x0388\x0153"\
							"\x2013\x2015\x201C\x201D\x2018\x2019\xF7\x0389\x038A\x038C\x038E\x03AD\x03AE\x03AF\x03CC\x038F"\
							"\x03CD\x03B1\x03B2\x03C8\x03B4\x03B5\x03C6\x03B3\x03B7\x03B9\x03BE\x03BA\x03BB\x03BC\x03BD\x03BF"\
							"\x03C0\x03CE\x03C1\x03C3\x03C4\x03B8\x03C9\x03C2\x03C7\x03C5\x03B6\x03CA\x03CB\x0390\x03B0." );

		else if( codepage.Find(wxT("CP10007")) not_eq wxNOT_FOUND )//Macintosh Cyrillic extension table
			newCP+=wxT( "\x0410\x0411\x0412\x0413\x0414\x0415\x0416\x0417\x0418\x0419\x041A\x041B\x041C\x041D\x041E\x041F"\
							"\x0420\x0421\x0422\x0423\x0424\x0425\x0426\x0427\x0428\x0429\x042A\x042B\x042C\x042D\x042E\x042F"\
							"\x2020\xB0\xA2\xA3\xA7\x2022\xB6\x0406\xAE\xA9\x2122\x0402\x0452\x2260\x0403\x0453"\
							"\x221E\xB1\x2264\x2265\x0456\xB5\x2202\x0408\x0404\x0454\x0407\x0457\x0409\x0459\x040A\x045A"\
							"\x0458\x0405\xAC\x221A\x0192\x2248\x2206\xAB\xBB\x2026\xA0\x040B\x045B\x040C\x045C\x0455"\
							"\x2013\x2014\x201C\x201D\x2018\x2019\xF7\x201E\x040E\x045E\x040F\x045F\x2116\x0401\x0451\x044F"\
							"\x0430\x0431\x0432\x0433\x0434\x0435\x0436\x0437\x0438\x0439\x043A\x043B\x043C\x043D\x043E\x043F"\
							"\x0440\x0441\x0442\x0443\x0444\x0445\x0446\x0447\x0448\x0449\x044A\x044B\x044C\x044D\x044E\xA4" );

		else if( codepage.Find(wxT("CP10081")) not_eq wxNOT_FOUND )		//Macintosh Turkish extension table
			newCP+=wxT(	"\xC4\xC5\xC7\xC9\xD1\xD6\xDC\xE1\xE0\xE2\xE4\xE3\xE5\xE7\xE9\xE8"\
							"\xEA\xEB\xED\xEC\xEE\xEF\xF1\xF3\xF2\xF4\xF6\xF5\xFA\xF9\xFB\xFC"\
							"\x2020\xB0\xA2\xA3\xA7\x2022\xB6\xDF\xAE\xA9\x2122\xB4\xA8\x2260\xC6\xD8"\
							"\x221E\xB1\x2264\x2265\xA5\xB5\x2202\x2211\x220F\x03C0\x222B\xAA\xBA\x2126\xE6\xF8"\
							"\xBF\xA1\xAC\x221A\x0192\x2248\x2206\xAB\xBB\x2026\xA0\xC0\xC3\xD5\x0152\x0153"\
							"\x2013\x2014\x201C\x201D\x2018\x2019\xF7\x25CA\xFF\x0178\x011E\x011F\x0130\x0131\x015E\x015F"\
							"\x2021\xB7\x201A\x201E\x2030\xC2\xCA\xC1\xCB\xC8\xCD\xCE\xCF\xCC\xD3\xD4"\
							".\xD2\xDA\xDB\xD9.\x02C6\x02DC\xAF\x02D8\x02D9\x02DA\xB8\x02DD\x02DB\x02C7" );

		#ifdef __WXMAC__
		else{
			for (unsigned i=0; i<=0xFF ; i++)
				bf[i] = (i<0x20 or i==0x7F)	? '.' : i;
			if( codepage.Find(wxT("Arabic")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACARABIC), 256);
			if( codepage.Find(wxT("Arabic Ext")) not_eq wxNOT_FOUND )	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACARABICEXT), 256);
			if( codepage.Find(wxT("Armanian")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACARMENIAN), 256);
			if( codepage.Find(wxT("Bengali")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACBENGALI), 256);
			if( codepage.Find(wxT("Burmese")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACBURMESE), 256);
			if( codepage.Find(wxT("Celtic")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACCELTIC), 256);
			if( codepage.Find(wxT("Central European")) not_eq wxNOT_FOUND )	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACCENTRALEUR), 256);
			if( codepage.Find(wxT("Chinese Imperial")) not_eq wxNOT_FOUND )	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACCHINESESIMP), 256);
			if( codepage.Find(wxT("Chinese Traditional")) not_eq wxNOT_FOUND )	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACCHINESETRAD), 256);
			if( codepage.Find(wxT("Croatian")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACCROATIAN), 256);
			if( codepage.Find(wxT("Cyrillic")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACCYRILLIC), 256);
			if( codepage.Find(wxT("Devanagari")) not_eq wxNOT_FOUND )	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACDEVANAGARI), 256);
			if( codepage.Find(wxT("Dingbats")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACDINGBATS), 256);
			if( codepage.Find(wxT("Ethiopic")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACETHIOPIC), 256);
			if( codepage.Find(wxT("Gaelic")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACGAELIC), 256);
			if( codepage.Find(wxT("Georgian")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACGEORGIAN), 256);
			if( codepage.Find(wxT("Greek")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACGREEK), 256);
			if( codepage.Find(wxT("Gujarati")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACGUJARATI), 256);
			if( codepage.Find(wxT("Gurmukhi")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACGURMUKHI), 256);
			if( codepage.Find(wxT("Hebrew")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACHEBREW), 256);
			if( codepage.Find(wxT("Icelandic")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACICELANDIC), 256);
			if( codepage.Find(wxT("Japanese")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACJAPANESE), 256);
			if( codepage.Find(wxT("Kannada")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACKANNADA), 256);
			if( codepage.Find(wxT("Keyboard")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACKEYBOARD), 256);
			if( codepage.Find(wxT("Khmer")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACKHMER), 256);
			if( codepage.Find(wxT("Korean")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACKOREAN), 256);
			if( codepage.Find(wxT("Laotian")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACLAOTIAN), 256);
			if( codepage.Find(wxT("Malajalam")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACMALAJALAM), 256);
//			if( codepage.Find(wxT("Min")) not_eq wxNOT_FOUND )	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACMAX), 256);
//			if( codepage.Find(wxT("Max")) not_eq wxNOT_FOUND )	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACMIN), 256);
			if( codepage.Find(wxT("Mongolian")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACMONGOLIAN), 256);
			if( codepage.Find(wxT("Oriya")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACORIYA), 256);
			if( codepage.Find(wxT("Roman ")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACROMAN), 256);
			if( codepage.Find(wxT("Romanian")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACROMANIAN), 256);
			if( codepage.Find(wxT("Sinhalese")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACSINHALESE), 256);
			if( codepage.Find(wxT("Symbol")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACSYMBOL), 256);
			if( codepage.Find(wxT("Tamil")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACTAMIL), 256);
			if( codepage.Find(wxT("Telugu")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACTELUGU), 256);
			if( codepage.Find(wxT("Thai")) not_eq wxNOT_FOUND )			newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACTHAI), 256);
			if( codepage.Find(wxT("Tibetan")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACTIBETAN), 256);
			if( codepage.Find(wxT("Turkish")) not_eq wxNOT_FOUND )		newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACTURKISH), 256);
			if( codepage.Find(wxT("Viatnamese")) not_eq wxNOT_FOUND )	newCP+=wxString( bf, wxCSConv(wxFONTENCODING_MACVIATNAMESE), 256);
			}
		#endif// __WXMAC__
		}

	else if( codepage.Find(wxT("DEC Multinational")) not_eq wxNOT_FOUND ){
		newCP=PrepareCodepageTable(wxT("ISO/IEC 8859-1 "));//Warning! Watch the space after last 1
		unsigned char a[]="\xA0\xA4\xA6\xA8\xAC\xAD\xAE\xAF\xB4\xB8\xBE\xD0\xDE\xF0\xFD\xFE\xFF";  //Patch Index
		unsigned char b[]="...\xA4..........\xFF..";	//Patch Value
		for(int i=0;i<17;i++)
			newCP[a[i]]=wxChar(b[i]);

		newCP[0xD7]=wxChar(0x0152);
		newCP[0xDD]=wxChar(0x0178);
		newCP[0xF7]=wxChar(0x0153);
		}

	else if(codepage.Find(wxT("UTF8 ")) not_eq wxNOT_FOUND )		FontEnc=wxFONTENCODING_UTF8;
	else if(codepage.Find(wxT("UTF16 ")) not_eq wxNOT_FOUND )		FontEnc=wxFONTENCODING_UTF16;
	else if(codepage.Find(wxT("UTF16LE")) not_eq wxNOT_FOUND )	FontEnc=wxFONTENCODING_UTF16LE;
	else if(codepage.Find(wxT("UTF16BE")) not_eq wxNOT_FOUND )	FontEnc=wxFONTENCODING_UTF16BE;
	else if(codepage.Find(wxT("UTF32 ")) not_eq wxNOT_FOUND )		FontEnc=wxFONTENCODING_UTF32;
	else if(codepage.Find(wxT("UTF32LE")) not_eq wxNOT_FOUND )	FontEnc=wxFONTENCODING_UTF32LE;
	else if(codepage.Find(wxT("UTF32BE")) not_eq wxNOT_FOUND )	FontEnc=wxFONTENCODING_UTF32BE;
	else if(codepage.Find(wxT("GB2312")) not_eq wxNOT_FOUND )	FontEnc=wxFONTENCODING_GB2312;
	else if(codepage.Find(wxT("GBK")) not_eq wxNOT_FOUND )		FontEnc=wxFONTENCODING_CP936;
	else if(codepage.Find(wxT("Shift JIS")) not_eq wxNOT_FOUND )FontEnc=wxFONTENCODING_SHIFT_JIS;//CP932
	else if(codepage.Find(wxT("Big5")) not_eq wxNOT_FOUND )		FontEnc=wxFONTENCODING_BIG5;//CP950
	else if(codepage.Find(wxT("EUC-JP")) not_eq wxNOT_FOUND )	FontEnc=wxFONTENCODING_EUC_JP;
	else if(codepage.Find(wxT("EUC-KR")) not_eq wxNOT_FOUND )	FontEnc=wxFONTENCODING_CP949; //EUC-KR
//	else if(codepage.StartsWith(wxT("EUC-CN")))		FontEnc=wxFONTENCODING_GB2312;
//else if(codepage.Find(wxT("Linux Bulgarian")) not_eq wxNOT_FOUND )		FontEnc=wxFONTENCODING_BULGARIAN;
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
	if(FontEnc!=wxFONTENCODING_ALTERNATIVE){
		if(not wxFontMapper::Get()->IsEncodingAvailable( FontEnc ) )
			wxMessageBox(wxT("Encoding is not available!"), wxT("Error!"), wxOK|wxCENTRE );
		m_text << FilterMBBuffer(buffer,len,FontEnc);
		}
	else
		for( int i=0 ; i<len ; i++ )
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
			m_text << wxString::Format( format, (ull/sector_size), ull%sector_size );
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
		return wxString::Format( GetFormatString(minimal), (c_offset/sector_size), c_offset%sector_size );
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
	uint64_t address = offset_position + p.y*BytePerLine;
	wxString adr;
	if(offset_mode=='s')
		adr = wxString::Format( GetFormatString(), (1+address/sector_size), address%sector_size);
	else
		adr = wxString::Format( GetFormatString(), address);

	if(wxTheClipboard->Open()) {
		wxTheClipboard->Clear();
		if( not wxTheClipboard->SetData( new wxTextDataObject( adr )) )
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
